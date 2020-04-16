#include <stdio.h>
#include <sched.h>
#include "sched_algo.h"

int FIFO(PROC_INFO *procs, int len, int q_num)
{
    // printf("Running %s...\n", __func__);
    struct sched_param param;
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            param.sched_priority = procs[i].order;
            sched_setscheduler(procs[i].pid, SCHED_FIFO, &param);
        }
    }
    return 0;
};

int RR(PROC_INFO *procs, int len, int q_num)
{
    // printf("Running %s...\n", __func__);
    int raise = -1;
    int head = -1;
    struct sched_param param = { .sched_priority = 1 };
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            sched_getparam(procs[i].pid, &param);
            if(head < 0) {
                head = i;
            }
            if(param.sched_priority == 2) {
                param.sched_priority = 1;
                if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                    perror("sched_setscheduler1");
                }
                procs[i].rr_pri = 1;
                raise = 1;
            } else {
                if(raise == 1) {
                    param.sched_priority = 2;
                    if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                        perror("sched_setscheduler2");
                    }
                    procs[i].rr_pri = 2;
                    raise = i;
                } else {
                    // Step back to lowest priority.
                    param.sched_priority = 1;
                    if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                        perror("sched_setscheduler3");
                    }
                    procs[i].rr_pri = 1;
                }
            }
        }
    }

    if(raise < 0) {
        printf("Nothing is raised.");
        param.sched_priority = 2;
        if (sched_setscheduler(procs[head].pid, SCHED_FIFO, &param) != 0) {
            perror("sched_setscheduler4");
        }
        raise = head;
    }

    return raise;
};

int SJF(PROC_INFO *procs, int len, int q_num)
{
    printf("Running %s...\n", __func__);
};

int PSJF(PROC_INFO *procs, int len, int q_num)
{
    printf("Running %s...\n", __func__);
};

void print_rr_pri(PROC_INFO *procs, int len)
{
    struct sched_param param;
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            sched_getparam(procs[i].pid, &param);
            printf("%s: %d. ", procs[i].name, param.sched_priority);
        }
    }
    printf("\n");
};