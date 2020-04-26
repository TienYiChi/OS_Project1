#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "proc_info.h"
#include "sched_algo.h"

int check_finished(PROC_INFO* procs, int len)
{
    int finished = 1;
    for(int i = 0; i < len; i++) {
        finished *= procs[i].finish;
    }

    return finished;
}

void main(void)
{
    struct sched_param param;
    int max_priority = sched_get_priority_max(SCHED_FIFO);
    if(max_priority == -1) 
    {
        perror("sched_get_priority_max failed.");
        exit(1);
    }
    param.sched_priority = max_priority;
    sched_setscheduler(0, SCHED_FIFO, &param);

    int    num_procs, status, raise;
    char   policy[10];
    int   (*run_sched)(PROC_INFO*, int, ...);

    scanf("%s", policy);
    scanf("%d", &num_procs);
    // printf("Policy: %s\n", policy);
    // printf("Number of processes: %d\n", num_procs);
    fflush(stdout);

    PROC_INFO* procs = malloc(num_procs*sizeof(PROC_INFO));

    for(int i = 0; i < num_procs; i++) {
        scanf("%s %d %d", procs[i].name, &procs[i].t_start, &procs[i].t_exec);
        procs[i].t_remain = procs[i].t_exec;
        procs[i].finish = 0;
        procs[i].pid = -1;
        procs[i].order = -1;
        procs[i].rr_pri = -1;
        // printf("Name: %s, start at: %d, exec for %d\n", procs[i].name, procs[i].t_start, procs[i].t_exec);
    }

    struct timeval te, ts;

    gettimeofday(&ts, NULL);
    {
        volatile unsigned long j; 
        for(j=0;j<1000000UL;j++);
    }
    gettimeofday(&te, NULL);

    long long mts = ts.tv_sec*1000LL + ts.tv_usec/1000;
    long long mte = te.tv_sec*1000LL + te.tv_usec/1000;
    long long ll_t_quantum = mte - mts;
    int t_sleep = (int)ll_t_quantum;
    
    struct timeval ts_main;
    gettimeofday(&ts_main, NULL);
    long long ll_t_current = ts_main.tv_sec*1000LL + ts_main.tv_usec/1000;
    long long ll_t_start = ll_t_current;

    switch (policy[0])
    {
        case 'F':
            run_sched = &FIFO;
            break;
        case 'R':
            run_sched = &RR;
            break;
        case 'S':
            run_sched = &SJF;
            break;
        case 'P':
            run_sched = &PSJF;
            break;
        default:
            break;
    }

    int num_arrived = num_procs;
    int num_done = 0;
    long long old_num_quantum = 0;
    long long num_quantum = 0;
    // For RR
    int old_units = 0;
    int units = 0;
    // For PSJF
    int last_exec = 0;
    int new_exec = 0;

    struct timespec clock_start, clock_end;

    do {
        gettimeofday(&ts_main, NULL);
        ll_t_current = ts_main.tv_sec*1000LL + ts_main.tv_usec/1000;
        num_quantum = (ll_t_current - ll_t_start) / ll_t_quantum;
        new_exec = num_quantum;
        units = num_quantum / 500;
        
        for(int i = 0; i < num_procs; i++) {
            if(procs[i].pid < 0 && old_num_quantum <= procs[i].t_start && num_quantum >= procs[i].t_start) {
                pid_t pid = fork();
                switch (pid) {
                    case 0: {
                        clock_gettime(CLOCK_REALTIME, &clock_start);
                        cpu_set_t mask;
                        CPU_ZERO(&mask);
                        CPU_SET(0, &mask);
                        if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
                            perror("sched_setaffinity");
                        }

                        {
                            volatile unsigned long j, k;
                            for(k=0;k<procs[i].t_exec;k++) {
                                for(j=0;j<1000000UL;j++);
                            }
                        }
                        clock_gettime(CLOCK_REALTIME, &clock_end);
                        // TODO: sys call and use printk
                        syscall(333, "Project1", getpid(), clock_start.tv_nsec, clock_end.tv_nsec);
                        exit(0);
                        break;
                    }
                    default:
                        procs[i].pid = pid;
                        procs[i].order = num_arrived;
                        procs[i].rr_pri = 1;
                        num_arrived -= 1;
                        raise = (*run_sched)(procs, num_procs, last_exec, new_exec);
                        printf("%s %d\n", procs[i].name, procs[i].pid);
                        fflush(stdout);
                        last_exec = new_exec;
                        break;
                }
            } else if(procs[i].pid > 0) {
                waitpid(-1, &status, WNOHANG);
                if(kill(procs[i].pid, 0) == 0) {
                    continue;
                } else {
                    if(procs[i].finish == 0 && num_done < num_procs) {
                        procs[i].finish = 1;
                        // Re-schedule whenever someone is finished.
                        raise = (*run_sched)(procs, num_procs, last_exec, new_exec);
                        last_exec = new_exec;
                        num_done += 1;
                    }
                }
            }
        }

        // RR re-schedule every 500 units.
        if((policy[0] == 'R') && old_units < units && num_done < num_procs) {
            raise = (*run_sched)(procs, num_procs);
            old_units = units;
        }

        old_num_quantum = num_quantum;
        usleep(t_sleep*1000);

    } while(!check_finished(procs, num_procs));

    free(procs);

    return;
}