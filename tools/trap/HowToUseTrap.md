# How to Use TRAP (TizenRT dump and Abort Parser) Tool
Here we explain how to enable build support for DUMP upload,
and how to actually upload and parse it in the event of a crash during runtime.

## Contents
> [Build Steps](#how-to-enable-memory-dumps)  
> [Upload Steps](#how-to-upload-RAMDUMP-or-UserfsDUMP)  
> [Parsing Steps](#how-to-parse-RAMDUMP)  
> [Porting Guide](#how-to-port-memory-dump-functionality)  

## How to enable memory dumps
Below configuration must be enabled to support dump upload
1. Enable crashdump support (CONFIG_BOARD_CRASHDUMP=y)
```
Hardware Configuration > Board Selection -> Enable Board level logging of crash dumps to y
```
2. Enable board dumps (CONFIG_BOARD_DUMP_UART=y)
```
Hardware Configuration > Board Selection -> Enable dumping of contents via UART to y
```
3. Enable Stack Dump (CONFIG_ARCH_STACKDUMP=y)
```
Hardware Configuration > Chip Selection -> Dump stack on assertions to y
```
4. Enable Debug (CONFIG_DEBUG=y)
```
Debug Options -> Enable Debug Output Features to y
```
5. Enable frame pointer for generating stack frame (CONFIG_FRAME_POINTER=y)
```
Debug Options -> Enable backtracking using Frame pointer register  to y
```
```
NOTE: - For devices that use ARM Cortex M, backtracking of frame pointer is not supported.
Hence, it is not possible to obtain exact call stack for the crash.
Ramdump may not produce results as expected in such cases.
```
6. Enable CONFIG_BOARD_ASSERT_AUTORESET to enable resetting of TARGET device after extracting dumps
```
Hardware Configuration > Board Selection -> Reset a board on assert status automatically
```
7. Enable CONFIG_BCH to enable Block-to-Character driver support for extracting External Userfs Partition Dump
```
Device Drivers -> Block-to-character(BCH) Support
```

## How to upload RAMDUMP or UserfsDUMP
### In Linux
With the DUMP Tool configured above, whenever the target board crashes because of an assert condition, it enters into PANIC mode, and displays the following message:
```
	****************************************************
	Disconnect this serial terminal and run TRAP Tool
	****************************************************
```
After you see this message, you can upload the dumps by following the step below:  
1. Disconnect/close your serial terminal (may be minicom)  

2. Run TRAP tool
```
cd $TIZENRT_BASEDIR/tools/trap/
./trap.sh
```
3. TRAP Tool will prompt for user's input for device adapter connected to the linux machine.
```
Please enter serial port adapter:
For example: /dev/ttyUSB0 or /dev/ttyACM0
Enter:
/dev/ttyUSB0
/dev/ttyUSB0 open failed!!
Please enter correct device port
Enter:
/dev/ttyUSB1
Target device locked and ready to DUMP!!!
```
4. After entering device adapter information, the tool will provide a list of options for the user to choose from
```
Choose from the following options:-
1. RAM Dump
2. Userfs Dump
3. Both RAM and Userfs dumps
4. External Userfs Partition Dump
5. Exit TRAP Tool
6. Reboot TARGET Device
1 (-> RAM Dump option chosen)
```
5. If RAM Dump option is chosen, tool will prompt the user for the regions to be dumped on successful handshake
```
DUMPING RAM CONTENTS
do_handshake: Target Handshake successful

=========================================================================
Ramdump Region Options:
1. ALL  ( Address: 02023800, Size: 968704 )

=========================================================================
Please enter desired ramdump option as below:
        1 for ALL
Please enter your input: 1

ramdump_recv: No. of Regions to be dumped received

```
6. TRAP Tool receives the RAM contents from target.
```
Receiving ramdump......

=========================================================================
```
```
Dumping data, Address: 0x02023800, Size: 968704bytes
=========================================================================
[===========================================================>]
Ramdump received successfully..!
```
7. The TRAP tool will again provide a list of options for the user to choose from
```
Choose from the following options:-
1. RAM Dump
2. Userfs Dump
3. Both RAM and Userfs dumps
4. External Userfs Partition Dump
5. Exit TRAP Tool
6. Reboot TARGET Device
2 (-> Userfs Dump option chosen)
```
8. If Userfs Dump option is chosen, tool will dump the region on successful handshake
```
DUMPING USERFS CONTENTS
do_handshake: Target Handshake successful

=========================================================================
Filesystem start address = 4620000, filesystem size = 1024000
=========================================================================

Receiving file system dump.....
[==============================================================>]

Filesystem Dump received successfully
```
9. The TRAP tool will again provide a list of options for the user to choose from
```
Choose from the following options:-
1. RAM Dump
2. Userfs Dump
3. Both RAM and Userfs dumps
4. External Userfs Partition Dump
5. Exit TRAP Tool
6. Reboot TARGET Device
4 (-> External Userfs Partition dump option chosen)
```
10. If External Userfs Partition dump option is chosen, tool will dump the Userfs partition on the External Flash on successful handshake..
```
DUMPING EXTERNAL USERFS DUMP PARTITION
do_handshake: Target Handshake successful

=========================================================================
External filesystem size = 03145728
=========================================================================

Receiving external file system dump.....
[===================-========================================================================================
=============================================================================================================
=================>]

External Userfs partition dump received successfully
```
11. The TRAP tool will again provide a list of options for the user to choose from
```
Choose from the following options:-
1. RAM Dump
2. Userfs Dump
3. Both RAM and Userfs dumps
4. External Userfs Partition Dump
5. Exit TRAP Tool
6. Reboot TARGET Device
6 (-> Reboot TARGET Device option chosen)
```
12. If Reboot TARGET Device option is chosen, tool will send a Reboot signal string to the TARGET device and exit
```
CONFIG_BOARD_ASSERT_AUTORESET needs to be enabled to reboot TARGET Device after a crash
do_handshake: Target Handshake successful
Dump tool exits after successful operation
```
13. The TRAP tool exits if user chooses Option 5.
```
Dump tool exits after successful operation
```
14. NOTE: The device stays in a loop, waiting for the next handshake/reboot signal till the user sends the Reboot signal.

### In Windows
With the RAMDUMP configured above, whenever the target board crashes because an assert condition, it enters PANIC mode, and displays the following message:  
```
	****************************************************
	Disconnect this serial terminal and Run Ramdump Tool
	****************************************************
```
After you see this message, you can upload the ramdump by following the step below:  
1. Disconnect/close your serial terminal (may be TeraTerm)

2. Open windows powershell & Run ramdump tool
```
<tool_path>:\Ramdump_windows.ps1
```
3. Enter the COM port number for your device
```
Please enter COM port number & press enter : <ComPort>
```
Once connection with the COM port is established, you will get following message:  
--> Connection established.

Now you can enter desired ramdump region from the options: (Multi-heap scenario)
```
Target Handshake SUCCESSFUL !!!
Target entered to ramdump mode

=========================================================================
Ramdump Region Options:
1. ALL
2. Region : 0 ( Address: 0x02023800, Size: 61440)       [Heap index = 0]
3. Region : 1 ( Address: 0x02032800, Size: 81920)       [Heap index = 1]
4. Region : 2 ( Address: 0x02046800, Size: 825344)      [Heap index = 0]
=========================================================================
Please enter desired ramdump option as below:
1 for ALL
2 for Region 0
25 for Region 0 & 3 ...

Please enter your input : 1

```
4. Ramdump Tool receives the ram contents from target.
```
Target No. of Regions to be dumped received!

Receiving ramdump......

Target Region info received!
=========================================================================
Dumping Region: 0, Address: 0x02023800, Size:61440bytes
=========================================================================
[===>]
Copying...
to C:\Users\thapa.v\ramdump_0x02023800_0x61440.bin

Target Region info received!
=========================================================================
Dumping Region: 1, Address: 0x02032800, Size:81920bytes
=========================================================================
[=====>]
Copying...
to C:\Users\thapa.v\ramdump_0x02032800_0x81920.bin

Target Region info received!
=========================================================================
Dumping Region: 2, Address: 0x02046800, Size:825344bytes
=========================================================================
[=======================================>
Copying...
to C:\Users\thapa.v\ramdump_0x02046800_0x825344.bin

Ramdump received successfully..!
```

## How to parse RAMDUMP
TRAP Script provides two interfaces: CUI and GUI

### TRAP using CUI

#### To display Debug Symbols/Crash point using assert logs
1. Change the directory to trap
```
cd $TIZENRT_BASEDIR/tools/trap/
```
2. Copy crash logs  
    First copy the crash logs to a file in tools/trap/`<log_file>`
3. Run Ramdump Parser Script and see the Output  
    $ python3 ramdumpParser.py -t `<Log file path>`

    ex)
    $ python3 ramdumpParser.py -t ./log_file

Example Call Stack Output for App crash is as follows:
```
*************************************************************
dump_file         : None
log_file          : .log_file
elf_file          : ../../build/output/bin/tinyara.axf
*************************************************************

Number of applicaions : 2
App[1] is : app1
App[2] is : app2

----------------------------------------------------------
-------------------------- DEBUG SYMBOLS IN "app1" TEXT RANGE -------------------------
Dump_address	 Symbol_address	  Symbol_name	File_name

PC_value	 Symbol_address	  Symbol_name	File_name
-----------------------------------------------------------------------------------------
App Crash point is as follows:
[ Caller - return address (LR) - of the function which has caused the crash ]

App name          : app2
symbol addr       : 0x000014e5
function name     : main
file              : /root/tizenrt/loadable_apps/loadable_sample/wifiapp/wifiapp.c:113

App Crash point is as follows:
[ Current location (PC) of assert ]
 - Exact crash point might be -4 or -8 bytes from the PC.

App name          : app2
symbol addr       : 0x000014d6
function name     : main
file              : /root/tizenrt/loadable_apps/loadable_sample/wifiapp/wifiapp.c:129

-------------------------- DEBUG SYMBOLS IN "app2" TEXT RANGE -------------------------
Dump_address	 Symbol_address	  Symbol_name	File_name
0x1dc5	 0x1dc5 	  printf	/root/tizenrt/os/include/stdio.h:406
0x1dc5	 0x1dc5 	  printf	/root/tizenrt/os/include/stdio.h:406
0x14e5	 0x1479 	  main	/root/tizenrt/loadable_apps/loadable_sample/wifiapp/wifiapp.c:57
0x1479	 0x1439 	  __fixunssfsi
0x1ef5	 0x1ef0 	  g_cb_handler	/root/tizenrt/lib/libc/sched/task_startup.c:119

PC_value	 Symbol_address	  Symbol_name	File_name
-----------------------------------------------------------------------------------------
----------------------------------------------------------
```
ex)
$ python3 ramdumpParser.py -t ./logs

Example Call Stack Output for Kernel crash is as follows:
```
*************************************************************
dump_file         : None
log_file          : ./logs
elf_file          : ../../build/output/bin/tinyara.axf
*************************************************************


----------------------------------------------------------
Kernel Crash point is as follows:
[ Caller - return address (LR) - of the function which has caused the crash ]

symbol addr       : 0x0e001c1b
function name     : setbasepri
file              : /root/tizenrt/os/include/arch/armv8-m/irq.h:247

Kernel Crash point is as follows:
[ Current location (PC) of assert ]
 - Exact crash point might be -4 or -8 bytes from the PC.

symbol addr       : 0x10010f1e
function name     : os_start
file              : /root/tizenrt/os/kernel/init/os_start.c:610

--------------------------- DEBUG SYMBOLS IN KERNEL TEXT RANGE --------------------------
Dump_address	 Symbol_address	  Symbol_name	File_name
0xe0060b3	 0xe00608c 	  lowvsyslog	/root/tizenrt/os/include/syslog.h:263
0xe0067f5	 0xe0067f4 	  lowoutstream_putc	/root/tizenrt/lib/libc/stdio/lib_lowoutstream.c:76
0xe005e89	 0xe005e88 	  lib_noflush	/root/tizenrt/os/include/tinyara/streams.h:605
0xe0086ef	 0xe008678 	  up_usagefault	/root/tizenrt/os/arch/arm/src/armv8-m/up_usagefault.c:84
0x10012375	 0x1001235c 	  up_doirq	/root/tizenrt/os/arch/arm/src/armv8-m/up_doirq.c:91
0x1001207d	 0x1001203c 	  exception_common	/root/tizenrt/os/arch/arm/src/armv8-m/up_exception.S:156
0x10012619	 0x10012618 	  up_unblock_task	/root/tizenrt/os/include/tinyara/arch.h:472
0xe001c1b	 0xe001bcc 	  task_activate	/root/tizenrt/os/include/sched.h:141
0x10010f1e	 0x10010d78 	  os_start	/root/tizenrt/os/include/tinyara/init.h:89

PC_value	 Symbol_address	  Symbol_name	File_name
0x10010f1e	 0x10010d78 	  os_start	/root/tizenrt/os/include/tinyara/init.h:89
-----------------------------------------------------------------------------------------
----------------------------------------------------------
```
ex)
$python3 ramdumpParser.py -t ./log.txt

Example Call Stack Output for Common Binary crash is as follows:
```
*************************************************************
dump_file         : None
log_file          : ./log.txt
elf_file          : ../../build/output/bin/tinyara.axf
*************************************************************

Number of applicaions : 2
App[1] is : common
App[2] is : app

----------------------------------------------------------
App Crash point is as follows:
[ Current location (PC) of assert ]
 - Exact crash point might be -4 or -8 bytes from the PC.

App name         : common
symbol addr      : 0x0003cdea
function name    : sched_get_priority_max
/root/product/.tizenrt/lib/libc/sched/sched_getprioritymax.c:110

App Crash point is as follows:
[ Caller - return address (LR) - of the function which has caused the crash ]

App name         : app
symbol addr      : 0x00000551
function name    : set_app_main_task
file : /root/product/main/set_app_main/src/set_app_main.c:380 (discriminator 1)

-------------------------- DEBUG SYMBOLS IN APPLICATION TEXT RANGE -------------------------
Dump_address	 Symbol_address	  Symbol_name	File_name
0x551	 0x3e2 	  gstSupportCourseInfo	/root/product/main/set_app_main/src/set_app_main.c:312
0x3011	 0x3011 	  wm_easysetup_event_handler	/root/product/main/fill_ocf/src/fill_ocf.c:80
0x57a1	 0x5729 	  process_operation_uri_from_bixby
0x2eb9	 0x2e11 	  device_publish_resource_list_getter	/root/product/main/fill_ocf/src/fill_ocf.c:204
0x2e11	 0x2df7 	  init_product_resources	/root/product/main/fill_ocf/src/fill_ocf.c:228
0x1e49	 0x1e41 	  otn_wifi_apporve_update	/root/product/apps/otn/wifi_updater/src/otn_wifi_update.c:159
0x2df7	 0x2df7 	  init_product_resources	/root/product/main/fill_ocf/src/fill_ocf.c:228
0x105	 0x105 	  set_app_main_task	/root/product/main/set_app_main/src/set_app_main.c:128
0x105	 0x105 	  set_app_main_task	/root/product/main/set_app_main/src/set_app_main.c:128

PC_value	 Symbol_address	  Symbol_name	File_name
-----------------------------------------------------------------------------------------
----------------------------------------------------------

```
#### To get call stack using RAM dump
1. Enable memory dumps. Refer to [How to enable memory dumps](how-to-enable-memory-dumps)
2. Get RAM dump using [How to upload RAMDUMP or UserfsDUMP](how-to-upload-ramdump-or-userfsdump) [Options 1- 7]
3. Change the directory to trap.
```
cd $TIZENRT_BASEDIR/tools/trap/
```
4. [Optional] Copy crash logs if any  
    First copy the crash logs to a file in tools/trap/`<log_file>`
5. Run Ramdump Parser Script and see the Output  
    $ python3 ramdumpParser.py -t `<Log file path>` -r `<Ramdump file path>`

    ex)
    $ python3 ramdumpParser.py -t ./log_file -r ../../ramdump_0x02023800_0x02110000.bin OR
    $ python3 ramdumpParser.py -r ../../ramdump_0x02023800_0x02110000.bin

Example Call Stack Output for Kernel crash is as follows:
```
*************************************************************
dump_file         : ../../ramdump_0x02023800_0x02110000.bin
log_file          : logs
elf_file          : ../../build/output/bin/tinyara
*************************************************************

self.ram_base_addr 2023800
self.ram_end_addr 2110000
----------------------------------------------------------
Kernel Crash point is as follows:
[ Caller - return address (LR) - of the function which has caused the crash ]

symbol addr       : 0x040cd264
function name     : irqrestore
file              : /root/tizenrt/os/include/arch/armv7-r/irq.h:414

Kernel Crash point is as follows:
[ Current location (PC) of assert ]
 - Exact crash point might be -4 or -8 bytes from the PC.

symbol addr       : 0x040d53cc
function name     : test_func
file              : /root/tizenrt/apps/examples/hello/hello_main.c:67

--------------------------- DEBUG SYMBOLS IN KERNEL TEXT RANGE --------------------------
Dump_address	 Symbol_address	  Symbol_name	File_name
0x40c9718	 0x40c94fc 	  up_assert	/root/tizenrt/os/include/assert.h:211
0x40ebc72	 0x40ebbd8 	  __FUNCTION__.6146
0x40ebbcb	 0x40ebbcb 	  __FUNCTION__.6135
0x40cf4f8	 0x40cf4e0 	  lowsyslog	/root/tizenrt/os/include/syslog.h:251
0x40c98e4	 0x40c98b0 	  arm_dataabort	/root/tizenrt/os/arch/arm/src/armv7-r/arm_dataabort.c:101
0x40c98e4	 0x40c98b0 	  arm_dataabort	/root/tizenrt/os/arch/arm/src/armv7-r/arm_dataabort.c:101
0x40c827c	 0x40c8220 	  arm_vectordata	/root/tizenrt/os/arch/arm/src/armv7-r/arm_vectors.S:498
0x40cc1d4	 0x40cc18c 	  task_start	/root/tizenrt/os/kernel/task/task_start.c:133
0x40d3c30	 0x40d3c2c 	  hello_main	/root/tizenrt/apps/examples/hello/hello_main.c:73

PC_value	 Symbol_address	  Symbol_name	File_name
0x40d3c30	 0x40d3c2c 	  hello_main	/root/tizenrt/apps/examples/hello/hello_main.c:73
-----------------------------------------------------------------------------------------
----------------------------------------------------------

CALL STACK of Aborted task:
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

[<40d53cc>] hello_main+0x18 [Line 67 of hello_main.c]
[<40cceb8>] task_start+0x50 [Line 180 of task_start.c]

&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

```
### TRAP using GUI
The UI configuration of TRAP is as follows

| (X) (-)           | Dump Parser           |        |
| ----------------- |:---------------------:| ------:|
| ELF path          | `<Your ELF path>`     | Browse |
| (O) AssertLog     |                       |        |
| (O) AssertLogFile |                       |        |
| (O) Ramdump       |                       |        |
| Ramdump path      | `<Your Ramdump path>` | Browse |
| Run TRAP    |                       |        |

1. Run GUI Ramdump Parser Script
```
cd $TIZENRT_BASEDIR/tools/trap/
python gui_dumpParser.py
```

2. Browse ELF path
3. Select Ramdump mode
4. Browse Ramdump path
5. Click `Run TRAP` button
6. See the Output

### Example Call Stack Output
```
********************************************************************
Board Crashed at :
PC: [0x40cb800] simulate_data_abort+0x20 [Line 63 of  "hello_main.c]"
LR: [0x40cb340] up_putc+0x28 [Line 1102 of  "chip/s5j_serial.c]"
FP: 0x2024fc4 and SP: 0x2024fb8
*******************************************************************
Call Trace of Crashed Task :[appmain] with pid :2 and state :TSTATE_TASK_RUNNING
*******************************************************************
[<40cb800>] simulate_data_abort+0x20         [Line 63 of \"hello_main.c\"]
[<40cb828>] hello_main+0x18         [Line 68 of \"hello_main.c\"]
[<40c9fec>] task_start+0x64         [Line 173 of \"task/task_start.c\"]
********************************************************************
```
## How to port memory dump functionality
To port TRAP tool for a new board, do the following steps:

1. Add low level chip specific API's to receive and transfer characters through UART:

a. **up_putc()** : Output one byte on the serial console
```
 * Prototype: int up_putc(int ch)
 * Input Parameters:
 *   ch - chatacter to output
 * Returned Value:
 *   sent character
```
b. **up_getc()** : Read one byte from the serial console
```
 * Prototype: int up_getc(void)
 * Input Parameters:
 *   none
 * Returned Value:
 *   int value, -1 if error, 0~255 if byte successfully read
```
c. **up_puts()** : Output string on the serial console
```
 * Prototype: void up_puts(const char *str)
 * Input Parameters:
 *   str - string to output
 * Returned Value:
 *   none
```
2. Source the low level API file in the Make.defs of os/arch/<cpu_name>/src/<chip_name>
```
CMN_CSRCS += <chip_name>_serial.c
```
3. Source the crashdump.c file in chip and board Makefile (os/board/<board_name>/src)
```
DEPPATH += --dep-path $(TOPDIR)/board/common
VPATH += :$(TOPDIR)/board/common

ifeq ($(CONFIG_BOARD_CRASHDUMP),y)
CSRCS += crashdump.c
endif
```
4. Add board_crashdump() API hook to architecture specific up_assert() if it does not exist already.
```
#if defined(CONFIG_BOARD_CRASHDUMP)
       board_crashdump(up_getsp(), this_task(), (uint8_t *)filename, lineno);
#endif
```
5. In trap.c, configure correct port parameters for the the board's tty serial device port n configure_tty function.
Like BaudRate, StopBits, Parity, Databits, HardwareFlowControl.
6. In trap.c, add if the serial device port does not exist already.
```
        /* Get the tty type  */
        if (!strcmp(dev_file, "/dev/ttyUSB1")) {
                strncpy(tty_type, "ttyUSB1", TTYTYPE_LEN);
        } else if (!strcmp(dev_file, "/dev/ttyACM0")) {
                strncpy(tty_type, "ttyACM0", TTYTYPE_LEN);
        } else {
                printf("Undefined tty %s\n", dev_file);
                return -1;
        }
```
