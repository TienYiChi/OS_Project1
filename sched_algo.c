#include <sched.h>
#include "sched_algo.h"

void FIFO(PROC_INFO *procs, int len)
{
    struct sched_param param;
    for(int i = 0; i < len; i++) {
        if(procs[i].order > 0 && procs[i].status == 0) {
            param.sched_priority = procs[i].order;
            sched_setscheduler(procs[i].pid, SCHED_FIFO, &param);
        }
    }
};

void RR(PROC_INFO *procs, int len)
{

};

void SJF(PROC_INFO *procs, int len)
{

};

void PSJF(PROC_INFO *procs, int len)
{
    
};