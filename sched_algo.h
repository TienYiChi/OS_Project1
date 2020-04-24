#include "proc_info.h"

#ifndef OS_PROJECT1_SCHED_ALGORITHM
#define OS_PROJECT1_SCHED_ALGORITHM
/* function prototypes here */
int FIFO(PROC_INFO *procs, int len, ...);
int RR(PROC_INFO *procs, int len, ...);
int SJF(PROC_INFO *procs, int len, ...);
int PSJF(PROC_INFO *procs, int len, ...);
void print_pri(PROC_INFO *procs, int len);
#endif