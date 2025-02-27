###########################################################################
#
# Copyright 2017 Samsung Electronics All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied. See the License for the specific
# language governing permissions and limitations under the License.
#
###########################################################################

-include $(TOPDIR)/.config
-include $(TOPDIR)/Make.defs
include $(APPDIR)/Make.defs

# built-in application info

APPNAME = wm_test
FUNCNAME = $(APPNAME)_main
THREADEXEC = TASH_EXECMD_ASYNC

# Wi-Fi manager example

ASRCS =

MAINSRC = wm_test.c wm_test_on_off.c wm_test_utils.c wm_test_interop.c
CSRCS = wm_test_stress.c\
wm_test_stress_test1.c\
wm_test_stress_test2.c\
wm_test_stress_test3.c\
wm_test_stress_test4.c\
wm_test_daemon.c\
wm_test_network.c\
wm_test_event_tc.c\
wm_test_connect_stable.c

ifeq ($(CONFIG_VIRTUAL_WLAN),y)
CSRCS += wm_test_mock.c
endif

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))
MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

ifneq ($(CONFIG_BUILD_KERNEL),y)
  OBJS += $(MAINOBJ)
endif

ifeq ($(CONFIG_WINDOWS_NATIVE),y)
  BIN = $(APPDIR)\libapps$(LIBEXT)
else
ifeq ($(WINTOOL),y)
  BIN = $(APPDIR)\\libapps$(LIBEXT)
else
  BIN = $(APPDIR)/libapps$(LIBEXT)
endif
endif

ifeq ($(WINTOOL),y)
  INSTALL_DIR = "${shell cygpath -w $(BIN_DIR)}"
else
  INSTALL_DIR = $(BIN_DIR)
endif

CONFIG_EXAMPLES_WIFIMANAGER_TEST_PROGNAME ?= wm_test$(EXEEXT)
PROGNAME = $(CONFIG_EXAMPLES_WIFIMANAGER_TEST_PROGNAME)

ROOTDEPPATH = --dep-path .

# Common build

all: .built
.PHONY: clean depend distclean

$(AOBJS): %$(OBJEXT): %.S
	$(call ASSEMBLE, $<, $@)

$(COBJS) $(MAINOBJ): %$(OBJEXT): %.c
	$(call COMPILE, $<, $@)

.built: $(OBJS)
	$(call ARCHIVE, $(BIN), $(OBJS))
	@touch .built

ifeq ($(CONFIG_BUILD_KERNEL),y)
$(BIN_DIR)$(DELIM)$(PROGNAME): $(OBJS) $(MAINOBJ)
	@echo "LD: $(PROGNAME)"
	$(Q) $(LD) $(LDELFFLAGS) $(LDLIBPATH) -o $(INSTALL_DIR)$(DELIM)$(PROGNAME) $(ARCHCRT0OBJ) $(MAINOBJ) $(LDLIBS)
	$(Q) $(NM) -u  $(INSTALL_DIR)$(DELIM)$(PROGNAME)

install: $(BIN_DIR)$(DELIM)$(PROGNAME)

else
install:

endif

ifeq ($(CONFIG_BUILTIN_APPS)$(CONFIG_EXAMPLES_WIFIMANAGER_TEST),yy)
$(BUILTIN_REGISTRY)$(DELIM)$(FUNCNAME).bdat: $(DEPCONFIG) Makefile
	$(call REGISTER,$(APPNAME),$(FUNCNAME),$(THREADEXEC))

context: $(BUILTIN_REGISTRY)$(DELIM)$(APPNAME)_main.bdat

else
context:

endif

.depend: Makefile $(SRCS)
	@$(MKDEP) $(ROOTDEPPATH) "$(CC)" -- $(CFLAGS) -- $(SRCS) >Make.dep
	@touch $@

depend: .depend

clean:
	$(call DELFILE, .built)
	$(call CLEAN)

distclean: clean
	$(call DELFILE, Make.dep)
	$(call DELFILE, .depend)

-include Make.dep
.PHONY: preconfig
preconfig:
