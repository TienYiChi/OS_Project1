#include "proc_info.h"

#ifndef OS_PROJECT1_SCHED_ALGORITHM
#define OS_PROJECT1_SCHED_ALGORITHM
/* function prototypes here */
int FIFO(PROC_INFO *procs, int len, int q_num);
int RR(PROC_INFO *procs, int len, int q_num);
int SJF(PROC_INFO *procs, int len, int q_num);
int PSJF(PROC_INFO *procs, int len, int q_num);
void print_rr_pri(PROC_INFO *procs, int len);
#endif