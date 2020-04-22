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
                raise = 0;
            } else {
                if(raise == 0) {
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
        param.sched_priority = 2;
        if (sched_setscheduler(procs[head].pid, SCHED_FIFO, &param) != 0) {
            perror("sched_setscheduler4");
        }
        raise = head;
    }

    print_pri(procs, len);
    return raise;
};

int PSJF(PROC_INFO *procs, int len, int q_num)
{
    // printf("Running %s...\n", __func__);
    int pri_level = 1;
    int max_level = 1;
    int min_exec = 0;
    struct sched_param param = { .sched_priority = 1 };
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            if(min_exec == 0) {
                min_exec = procs[i].t_remain;
            }
            if(min_exec > procs[i].t_remain) {
                min_exec = procs[i].t_remain;
                pri_level += 1;
                max_level = pri_level+1;
                procs[i].order = max_level;
            } else {
                procs[i].order = pri_level;
            }
        }
    }

    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            if(procs[i].order == max_level) {
                param.sched_priority = 2;
                if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                    perror("sched_setscheduler2");
                }
            } else {
                param.sched_priority = 1;
                if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                    perror("sched_setscheduler3");
                }
            }
        }
    }
    print_pri(procs, len);
    return min_exec;
};

int SJF(PROC_INFO *procs, int len, int q_num)
{
    printf("Running %s...\n", __func__);
};

void print_pri(PROC_INFO *procs, int len)
{
    struct sched_param param;
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            sched_getparam(procs[i].pid, &param);
            printf("%s: %d | ", procs[i].name, param.sched_priority);
        }
    }
    printf("\n");
};