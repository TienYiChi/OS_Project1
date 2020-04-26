#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/time.h>

asmlinkage void sys_done_running(char *name, int pid, const struct timespec start, const struct timespec end)
{
    printk("%s %d %ld.%ld %ld.%ld", name, pid, start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
    return;
}
