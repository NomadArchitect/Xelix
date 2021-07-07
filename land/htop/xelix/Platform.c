/*
htop - xelix/Platform.c
(C) 2014 Hisham H. Muhammad
(C) 2015 David C. Hunt
(C) 2019-2020 Lukas Martini
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include <string.h>
#include "Platform.h"
#include "CPUMeter.h"
#include "MemoryMeter.h"
#include "SwapMeter.h"
#include "TasksMeter.h"
#include "LoadAverageMeter.h"
#include "ClockMeter.h"
#include "HostnameMeter.h"
#include "UptimeMeter.h"

/*{
#include "Action.h"
#include "BatteryMeter.h"
#include "SignalsPanel.h"
#include "XelixProcess.h"

#define LAST_PROCESSFIELD 100
}*/

const SignalItem Platform_signals[] = {
   { .name = " 0 Cancel",    .number =  0 },
};

const unsigned int Platform_numberOfSignals = sizeof(Platform_signals)/sizeof(SignalItem);

ScreenDefaults Platform_defaultScreens[] = {
   {
      .name = "Main",
      .columns = "PID USER M_RESIDENT STATE PERCENT_MEM Command",
      .sortKey = "PID",
   },
};
const unsigned int Platform_numberOfDefaultScreens = sizeof(Platform_defaultScreens)/sizeof(ScreenDefaults);

ProcessFieldData Process_fields[] = {
   [0] = {
      .name = "",
      .title = NULL,
      .description = NULL,
      .flags = 0, },
   [PID] = {
      .name = "PID",
      .title = "    PID ",
      .description = "Process/thread ID",
      .flags = 0, },
   [COMM] = {
      .name = "Command",
      .title = "Command ",
      .description = "Command line",
      .flags = 0, },
   [STATE] = {
      .name = "STATE",
      .title = "S ",
      .description = "Process state (S sleeping, R running, D disk, Z zombie, T traced, W paging)",
      .flags = 0, },
   [PPID] = {
      .name = "PPID",
      .title = "   PPID ",
      .description = "Parent process ID",
      .flags = 0, },
   [PGRP] = {
      .name = "PGRP",
      .title = "   PGRP ",
      .description = "Process group ID",
      .flags = 0, },
   [SESSION] = {
      .name = "SESSION",
      .title = "   SESN ",
      .description = "Process's session ID",
      .flags = 0, },
/*   [TTY_NR] = {
      .name = "TTY_NR",
      .title = "    TTY ",
      .description = "Controlling terminal",
      .flags = 0, },
*/
   [TPGID] = {
      .name = "TPGID",
      .title = "  TPGID ",
      .description = "Process ID of the fg process group of the controlling terminal",
      .flags = 0, },
/*   [MINFLT] = {
      .name = "MINFLT",
      .title = "     MINFLT ",
      .description = "Number of minor faults which have not required loading a memory page from disk",
      .flags = 0, },
   [MAJFLT] = {
      .name = "MAJFLT",
      .title = "     MAJFLT ",
      .description = "Number of major faults which have required loading a memory page from disk",
      .flags = 0, },
   [PRIORITY] = {
      .name = "PRIORITY",
      .title = "PRI ",
      .description = "Kernel's internal priority for the process",
      .flags = 0, },
   [NICE] = {
      .name = "NICE",
      .title = " NI ",
      .description = "Nice value (the higher the value, the more it lets other processes take priority)",
      .flags = 0, },
   [STARTTIME] = {
      .name = "STARTTIME",
      .title = "START ",
      .description = "Time the process was started",
      .flags = 0, },
   [PROCESSOR] = {
      .name = "PROCESSOR",
      .title = "CPU ",
      .description = "Id of the CPU the process last executed on",
      .flags = 0, },
*/
   [M_SIZE] = {
      .name = "M_SIZE",
      .title = " VIRT ",
      .description = "Total program size in virtual memory",
      .flags = 0, },
   [M_RESIDENT] = {
      .name = "M_RESIDENT",
      .title = "  RES ",
      .description = "Resident set size, size of the text and data sections, plus stack usage",
      .flags = 0, },
   [ST_UID] = {
      .name = "ST_UID",
      .title = " UID ",
      .description = "User ID of the process owner",
      .flags = 0, },
   [PERCENT_CPU] = {
      .name = "PERCENT_CPU",
      .title = "CPU% ",
      .description = "Percentage of the CPU time the process used in the last sampling",
      .flags = 0, },
   [PERCENT_MEM] = {
      .name = "PERCENT_MEM",
      .title = "MEM% ",
      .description = "Percentage of the memory the process is using, based on resident memory size",
      .flags = 0, },
   [USER] = {
      .name = "USER",
      .title = "USER      ",
      .description = "Username of the process owner (or user ID if name cannot be determined)",
      .flags = 0, },
/*
   [TIME] = {
      .name = "TIME",
      .title = "  TIME+  ",
      .description = "Total time the process has spent in user and system time",
      .flags = 0, },
   [NLWP] = {
      .name = "NLWP",
      .title = "NLWP ",
      .description = "Number of threads in the process",
      .flags = 0, },
*/
   [TGID] = {
      .name = "TGID",
      .title = "   TGID ",
      .description = "Thread group ID (i.e. process ID)",
      .flags = 0, },
   [LAST_PROCESSFIELD] = {
      .name = "*** report bug! ***",
      .title = NULL,
      .description = NULL,
      .flags = 0, },
};
int Platform_numberOfFields = LAST_PROCESSFIELD;

ProcessPidColumn Process_pidColumns[] = {
   { .id = PID, .label = "PID" },
   { .id = PPID, .label = "PPID" },
   { .id = TPGID, .label = "TPGID" },
   { .id = TGID, .label = "TGID" },
   { .id = PGRP, .label = "PGRP" },
   { .id = SESSION, .label = "SESN" },
   { .id = 0, .label = NULL },
};

MeterClass* Platform_meterTypes[] = {
   &CPUMeter_class,
   &ClockMeter_class,
   &LoadAverageMeter_class,
   &LoadMeter_class,
   &MemoryMeter_class,
   &TasksMeter_class,
   &HostnameMeter_class,
   &UptimeMeter_class,
   &AllCPUsMeter_class,
   &AllCPUs2Meter_class,
   &LeftCPUsMeter_class,
   &RightCPUsMeter_class,
   &LeftCPUs2Meter_class,
   &RightCPUs2Meter_class,
   &BlankMeter_class,
   NULL
};

void Platform_setBindings(Htop_Action* keys) {
   (void) keys;
}

extern char Process_pidFormat[20];

int Platform_getUptime() {
   FILE* fp = fopen("/sys/tick", "r");
   if(!fp) {
      return 0;
   }

   int uptime;
   int ticks;
   int tick_rate;

   if(fscanf(fp, "%d %d %d\n", &uptime, &ticks, &tick_rate) != 3) {
      fclose(fp);
      return 0;
   }
   return uptime;
}

void Platform_getLoadAverage(double* one, double* five, double* fifteen) {
   *one = 0;
   *five = 0;
   *fifteen = 0;
}

int Platform_getMaxPid() {
   return 65536;
}

double Platform_setCPUValues(Meter* this, int cpu) {
   (void) this;
   (void) cpu;
   return 0.0;
}

void Platform_setMemoryValues(Meter* this) {
   FILE* fp = fopen("/sys/mem_info", "r");
   if(!fp) {
      return;
   }

   unsigned int mem_total = 0, mem_used = 0;

   while(!feof(fp)) {
      char name[100];
      unsigned int value;

      if(fscanf(fp, "%100[^:]: %u\n", name, &value) != 2) {
         continue;
      }

      if(!strcmp(name, "mem_total")) {
         mem_total = value;
      } else if(!strcmp(name, "mem_used")) {
         mem_used = value;
      }
   }

   ProcessList* pl = (ProcessList*) this->pl;
   this->total = mem_total / 1024;
   this->values[0] = mem_used / 1024;
   this->values[1] = 0;
   this->values[2] = 0;

   fclose(fp);
}

void Platform_setSwapValues(Meter* this) {
   (void) this;
}

bool Process_isThread(Process* this) {
   (void) this;
   return false;
}

char* Platform_getProcessEnv(pid_t pid) {
   (void) pid;
   return "foo=bar";
}