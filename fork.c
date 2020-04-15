#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <signal.h>
#include "sched_algo.h"

#define MAX_COUNT 50
#define BUF_SIZE 100

typedef struct proc_info
{
    char name[10];
    pid_t pid;
    int t_start;
    int t_exec;
    int status;
} PROC_INFO;

PROC_INFO procs[20];

int check_finished(PROC_INFO* procs, int len)
{
    int finished = 1;
    for(int i = 0; i < len; i++) {
        finished *= procs[i].status;
    }

    return finished;
}

void main(void)
{
    int    num_procs, status;
    char   buf[BUF_SIZE];
    char   policy[10];
    for(int i = 0; i < 20; i++) {
        procs[i].pid = -1;
        procs[i].status = 1;
    }
    scanf("%s", policy);
    scanf("%d", &num_procs);
    printf("Policy: %s\n", policy);
    printf("Number of processes: %d\n", num_procs);

    for(int i = 0; i < num_procs; i++) {
        scanf("%s %d %d", procs[i].name, &procs[i].t_start, &procs[i].t_exec);
        procs[i].status = 0;
        printf("Name: %s, start at: %d, exec for %d\n", procs[i].name, procs[i].t_start, procs[i].t_exec);
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
    unsigned int t_sleep = (unsigned int)ll_t_quantum;
    fflush(stdout);
    
    struct timeval ts_main;
    gettimeofday(&ts_main, NULL);
    long long ll_t_current = ts_main.tv_sec*1000LL + ts_main.tv_usec/1000;
    long long ll_t_start = ll_t_current;
    long long num_quantum = 0;
    long long old_num_quantum = 0;

    do {
        gettimeofday(&ts_main, NULL);
        ll_t_current = ts_main.tv_sec*1000LL + ts_main.tv_usec/1000;
        num_quantum = (ll_t_current - ll_t_start) / ll_t_quantum;
        for(int i = 0; i < num_procs; i++) {
            if(procs[i].pid < 0 && old_num_quantum <= procs[i].t_start && num_quantum >= procs[i].t_start) {
                pid_t pid = fork();
                switch (pid) {
                    case 0:
                        printf("%s %d\n", procs[i].name, getpid());
                        {
                            volatile unsigned long j, k;
                            for(k=0;k<procs[i].t_exec;k++) {
                                for(j=0;j<1000000UL;j++);
                            }
                        }
                        // TODO: sys call and use printk
                        // syscall(333, getpid())
                        exit(0);
                        break;
                    
                    default:
                        procs[i].pid = pid;
                        break;
                }
            } else if(procs[i].pid > 0) {
                printf("Child: %d\n", procs[i].pid);
                // Check pid status; if finished, set procs[i].status = 1
                if(kill(procs[i].pid, 0) == 0) {
                    continue;
                } else {
                    printf("Child: %d finished.\n", procs[i].pid);
                    procs[i].status = 1;
                }
            }
        }
        old_num_quantum = num_quantum;
        waitpid(-1, &status, WNOHANG);
        usleep(t_sleep*1000);
    } while(!check_finished(procs, num_procs));

    return;
}