#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void done_running(char* name, int pid, float nsec_start, float nsec_end)
{
    printk("%s %d %f %f", name, pid, nsec_start, nsec_end);
    return;
}