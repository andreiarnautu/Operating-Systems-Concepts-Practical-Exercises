/**
 *  Kernel module that traverses the list of active processes in the system in order of their pid value.
 */
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

#define PROC_NAME   "linear_iteration"

//  This function is called when the module is loaded
static int proc_init(void) {
    struct task_struct *task;

    printk(KERN_INFO "linear_iteration module added!\n");

    for_each_process(task) {
        printk(KERN_INFO "command = [%s] --- pid = [%d] --- state = [%ld]\n", task->comm, task->pid, task->state);
    }

    return 0;
}

//  This function is called when the module is removed
static void proc_exit(void) {
    printk(KERN_INFO "linear_iteration module removed!\n");
}

module_init( proc_init );
module_init( proc_exit );
