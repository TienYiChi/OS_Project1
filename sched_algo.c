#include <stdio.h>
#include <sched.h>
#include <stdarg.h>
#include "sched_algo.h"

int FIFO(PROC_INFO *procs, int len, ...)
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

int RR(PROC_INFO *procs, int len, ...)
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

int PSJF(PROC_INFO *procs, int len, ...)
{
    // printf("Running %s...\n", __func__);
    va_list argp;
    va_start(argp, len);
    int q_begin = va_arg(argp, int);
    int q_end = va_arg(argp, int);

    int set = 0;
    int min_exec = 0;
    struct sched_param param = { .sched_priority = 1 };

    // Check who is running right now
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            sched_getparam(procs[i].pid, &param);
            if(param.sched_priority == 2) {
                procs[i].t_remain -= (q_end - q_begin);
                break;
            }
        }
    }

    // Priority re-ordering
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            if(min_exec == 0) {
                min_exec = procs[i].t_remain;
            }
            if(min_exec > procs[i].t_remain) {
                min_exec = procs[i].t_remain;
            }
        }
    }

    // One with max priority scheduled
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            if(procs[i].t_remain == min_exec && set == 0) {
                param.sched_priority = 2;
                if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                    perror("sched_setscheduler2");
                }
                set = 1;
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

int SJF(PROC_INFO *procs, int len, ...)
{
    // printf("Running %s...\n", __func__);
    int running = -1;
    int set = 0;
    int min_exec = 0;
    struct sched_param param = { .sched_priority = 1 };

    // Check who is running right now
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            sched_getparam(procs[i].pid, &param);
            if(param.sched_priority == 2) {
                running = i;
                break;
            }
        }
    }

    // Priority re-ordering
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            if(min_exec == 0) {
                min_exec = procs[i].t_exec;
            }
            // Skip the one that is already running
            if(i != running && min_exec > procs[i].t_exec) {
                min_exec = procs[i].t_exec;
            }
        }
    }

    // One with max priority scheduled
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            if(running >= 0) {
                if(i == running) {
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
            } else {
                if(min_exec == procs[i].t_exec && set == 0) {
                    param.sched_priority = 2;
                    if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                        perror("sched_setscheduler2");
                    }
                    set = 1;
                } else {
                    param.sched_priority = 1;
                    if (sched_setscheduler(procs[i].pid, SCHED_FIFO, &param) != 0) {
                        perror("sched_setscheduler3");
                    }
                }
            }
        }
    }
    print_pri(procs, len);
    return min_exec;
};

void print_pri(PROC_INFO *procs, int len)
{
    struct sched_param param;
    for(int i = 0; i < len; i++) {
        if(procs[i].pid > 0 && procs[i].finish == 0) {
            sched_getparam(procs[i].pid, &param);
            printf("%s: %d, t_remain: %d | ", procs[i].name, param.sched_priority, procs[i].t_remain);
        }
    }
    printf("\n");
    fflush(stdout);
};