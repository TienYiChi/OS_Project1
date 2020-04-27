#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/time.h>

asmlinkage void sys_done_running(char *name, int pid, long int start_s, long int start_ns, long int end_s, long int end_ns)
{
    printk("%s %d %ld.%ld %ld.%ld\n", name, pid, start_s, start_ns, end_s, end_ns);
    return;
}
