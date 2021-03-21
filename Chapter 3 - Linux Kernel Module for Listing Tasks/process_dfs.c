/**
 *  Kernel module that does a DFS on the system's process tree.
 */
#include <linux/init.h>
#include <linux/ched.h>
#include <linux/kernel.h>
#include <linux/module.h>

void dfs(struct task_struct *task) {
    struct task_struct *child;
    struct list_head *list;

    printk(KERN_INFO "command = [%s] --- pid = [%d] --- ppid = [%ld]\n", task->comm, task->pid, task->real_parent->pid);

    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        dfs(child);
    }
}

static int dfs_module_init(void) {
    printk(KERN_INFO "process_dfs module loaded!\n");

    dfs(&init_task);  //  init_task is a macro for the root of the process tree

    return 0;
}

static void dfs_module_exit(void) {
    printk(KERN_INFO "process_dfs module removed!\n");
}

module_init( dfs_module_init );
module_exit( dfs_module_exit );
