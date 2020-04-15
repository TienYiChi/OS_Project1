#include "proc_info.h"

#ifndef OS_PROJECT1_SCHED_ALGORITHM
#define OS_PROJECT1_SCHED_ALGORITHM
/* function prototypes here */
void FIFO(PROC_INFO *procs, int len);
void RR(PROC_INFO *procs, int len);
void SJF(PROC_INFO *procs, int len);
void PSJF(PROC_INFO *procs, int len);
#endif