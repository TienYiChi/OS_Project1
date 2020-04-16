#include <sys/types.h>

#ifndef OS_PROJECT1_PROC_INFO
#define OS_PROJECT1_PROC_INFO

typedef struct proc_info
{
    char name[10];
    pid_t pid;
    int t_start;
    int t_exec;
    int t_remain;
    int order;
    int rr_pri;
    int finish;
} PROC_INFO;

#endif