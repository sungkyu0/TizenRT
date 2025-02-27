/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License\n");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
#include <tinyara/config.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <stddef.h>
#include <stdlib.h>
#include <queue.h>
#include <assert.h>
#include <stress_tool/st_perf.h>
#include "st_perf_print.h"

#define PERF_ERR(msg) \
	printf("[perf error]" msg "\t%s%d\n", __FUNCTION__, __LINE__);

#define CONTAINER_OF(ptr, type, member) \
	((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

#define PR_GET_SMOKE(ptr) \
	CONTAINER_OF(ptr, st_smoke, entry)

static int g_keep_running = 0;

/**
 * Inner Function
 */
int _calc_performance(char *title, st_performance *p, st_elapsed_time *duration)
{
	st_performance_time *start = &duration->start;
	st_performance_time *end = &duration->end;

	unsigned int start_time = start->second * 1000000 + start->micro;
	unsigned int end_time = end->second * 1000000 + end->micro;
	unsigned int elapsed = end_time - start_time;

	st_performance_stat *stat = &p->stat;
	if (stat->count == 0) {
		stat->start.second = start->second;
		stat->start.micro = start->micro;
	}
	stat->count++;
	stat->end.second = end->second;
	stat->end.micro = end->micro;

	stat->sum += elapsed;
	if (stat->count == 1) {
		stat->max = elapsed;
		stat->min = elapsed;
	} else {
		if (elapsed > stat->max) {
			stat->max = elapsed;
		}
		if (elapsed < stat->min) {
			stat->min = elapsed;
		}
	}
	p->stat.total_elapsed += elapsed;
	if (p->expect != 0 && p->expect < elapsed) {
		stat->fail++;
		stat->result = STRESS_TC_FAIL;
		return -1;
	}
	return 0;
}

void _calc_stability(st_stability *stab, st_tc_result r)
{
	st_stability_stat *s = &stab->stat;
	s->count++;
	switch (r) {
	case STRESS_TC_PASS:
		s->pass++;
		break;
	case STRESS_TC_FAIL:
		s->fail++;
		s->result = STRESS_TC_FAIL;
		break;
	case STRESS_TC_SKIP:
		s->skip++;
		s->result = STRESS_TC_SKIP;
		break;
	default:
		printf("fail %s:%d\n", __FILE__, __LINE__);
		break;
	}
}

void _run_smoke(st_smoke *smoke)
{
	if (!smoke) {
		return;
	}
	int cnt = 0;
	st_tc_result ret;
	st_func *unit = smoke->func;
	st_stability *stab = smoke->stability;

	print_smoke_title(smoke);

	for (; cnt < smoke->repeat_size; cnt++) {
		int perf_result = 0;
		if (unit->setup) {
			// update rt performance, stability
			if (unit->setup(NULL) != STRESS_TC_PASS) {
				// reset
				// if teardown fails then remained testcase could be affected.
				// so remained procedures could be useless
				// so stopping running smoke would be better
				stab->stat.result = STRESS_TC_SETUP_FAIL;
				break;
			}
		}
		st_elapsed_time duration;
		ret = unit->tc(&duration);
		perf_result = _calc_performance(unit->tc_name, smoke->performance, &duration);
		_calc_stability(smoke->stability, ret);

		if (unit->teardown) {
			if (unit->teardown(NULL) != STRESS_TC_PASS) {
				// same to the setup procedure
				stab->stat.result = STRESS_TC_TEARDOWN_FAIL;
				break;
			}
		}
		if (g_keep_running == 0 && ret != STRESS_TC_PASS) {
			break;
		}
	}
	print_smoke_result(smoke);
}

void _perf_free_pack(st_pack *pack)
{
	if (!pack) {
		return;
	}

	st_smoke *smoke = PR_GET_SMOKE(pack->queue.head);
	while (smoke) {
		if (smoke->func) {
			free(smoke->func);
		}
		sq_entry_t *entry = smoke->entry.flink;
		sq_rem(&smoke->entry, &pack->queue);
		free(smoke);

		if (!entry) {
			break;
		}
		smoke = PR_GET_SMOKE(entry);
	}
	if (pack->title) {
		free(pack->title);
	}
}

void _perf_print_result(st_pack *pack)
{
	if (!pack) {
		return;
	}

	print_smoke_summary_title();

	st_smoke *smoke = PR_GET_SMOKE(pack->queue.head);
	while (smoke) {
		print_smoke_summary(smoke);
		if (!smoke->entry.flink) {
			break;
		}
		smoke = PR_GET_SMOKE(smoke->entry.flink);
	}

	print_smoke_summary_end();
}
/*
 * Public Function
 */
void perf_run(st_pack *pack)
{
	if (!pack) {
		return;
	}

	print_testsuite_title(pack);
	if (pack->setup) {
		print_testsuite_setup(pack);
		pack->setup(NULL);
	}
	sq_entry_t *entry = pack->queue.head;
	st_smoke *smoke = PR_GET_SMOKE(entry);

	while (smoke) {
		_run_smoke(smoke);
		entry = entry->flink;
		if (!entry) {
			break;
		}
		smoke = PR_GET_SMOKE(entry);
	}
	if (pack->teardown) {
		print_testsuite_teardown(pack);
		pack->teardown(NULL);
	}
	print_testsuite_result(pack);
	_perf_print_result(pack);
	_perf_free_pack(pack);
}

void perf_add_item(st_pack *pack, int repeat, char *tc_desc,
				   st_unit_tc func_init, st_unit_tc func_deinit,
				   st_unit_tc func_setup, st_unit_tc func_teardown, st_unit_tc func,
				   unsigned int expect, st_performance *perf, st_stability *stab)
{
	st_func *sfunc = (st_func *)malloc(sizeof(st_func));
	if (!sfunc) {
		PERF_ERR("func alloc fail\n");
		return;
	}
	sfunc->tc_name = tc_desc;
	sfunc->setup = func_setup;
	sfunc->teardown = func_teardown;
	sfunc->init = func_init;
	sfunc->deinit = func_deinit;
	sfunc->tc = func;

	perf->expect = expect;

	st_smoke *smoke = (st_smoke *)malloc(sizeof(st_smoke));
	if (!smoke) {
		PERF_ERR("malloc fail\n");
		free(sfunc);
		return;
	}
	smoke->repeat_size = repeat;
	smoke->performance = perf;
	smoke->stability = stab;
	smoke->func = sfunc;
	smoke->entry.flink = NULL;

	sq_addlast(&smoke->entry, &pack->queue);
}

void perf_add_global(st_pack *pack, st_unit_tc global_setup, st_unit_tc global_teardown, const char *title)
{
	pack->setup = global_setup;
	pack->teardown = global_teardown;
	int len = strlen(title);
	pack->title = (char *)malloc(len + 1);
	if (!pack->title) {
		assert(0);
	}
	strncpy(pack->title, title, len + 1);
}

void perf_set_keeprunning(int enable)
{
	g_keep_running = enable;
}
