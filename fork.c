#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MAX_COUNT 50
#define BUF_SIZE 100

void main(void)
{
    pid_t  pid;
    int    i;
    double diff_t;
    char   buf[BUF_SIZE];

    struct timeval te, ts;
    printf("PID: %d", getpid());
    gettimeofday(&ts, NULL);
    long long mts = ts.tv_sec*1000LL + ts.tv_usec/1000;
    {
        volatile unsigned long j; 
        for(int k=0;k<500;k++)
        {
            for(j=0;j<1000000UL;j++);
        }
    }
    gettimeofday(&te, NULL); // get current time
    long long mte = te.tv_sec*1000LL + te.tv_usec/1000;
    printf("Unit time: %lld", mte - mts);
    fflush(stdout);
    for(int k=0;k<5;k++)
    {
        pid = fork();
        switch (pid)
        {
            case 0:
                for (i = 1; i <= MAX_COUNT; i++) {
                    sprintf(buf, "This line is from child pid %d, value = %d\n", getpid(), i);
                    write(1, buf, strlen(buf));
                }
                exit(0);
                break;
            
            default:
                for (i = 1; i <= MAX_COUNT; i++) {
                    sprintf(buf, "This line is from pid %d, value = %d\n", getpid(), i);
                    write(1, buf, strlen(buf));
                }
                break;
        }
    }
    // wait(NULL);

    if(pid > 0)
    {
        printf("Parent ends.\n");
    }
    return;
}