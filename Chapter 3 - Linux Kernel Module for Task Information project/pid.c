/**
  *  Kernel module that communicates with /proc file system
  *
  *  After the module is loaded into the system, you can access it like this:
  *     echo "1" > /proc/pid --- where 1 or whatever other number represents the process's pid for which we want to print stats
  *     cat /proc/pid
  *     e.g. output: command = [systemd] -- pid = [1] -- state = [1]
  */
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uacces.h>

#define BUFFER_SIZE     128
#define PROC_NAME       "pid"

//  current pid
static long l_pid;

static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write
};

//  This function is called when the module is loaded.

static int proc_init(void)
{
    //  creates the /proc/procfs entry
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);

    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    return 0;
}

static void proc_exit(void)
{
    //  removes the /proc/procfs entry
    remove_proc_entry(PROC_NAME, NULL);

    printk(KERN_INFO "proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called every time /proc/pid is read.
 *
 * The function is called repeatedly until it returns 0,
 * so there must be a logic that ensures it ultimately
 * returns 0
 */
static ssize_t proc_read(struct file *file , char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];

    static int completed = 0;
    struct task_struct *tsk = NULL;

    if (completed) {
        completed = 0;
        return 0;
    }

    tsk = pid_task(find_vpid(l_pid), PIDTYPE_PID);

    if (tsk == NULL) {
        rv = sprintf(buffer, "The given pid is not valid\n");
    } else {
        rv = sprintf(buffer, "command = [%s] -- pid = [%d] -- state = [%ld]\n", tsk->comm, tsk->pid, tsk->state);
    }

    completed = 1;

    //  copies the contents of kernel buffer to userspace usr_buf
    if (copy_to_user(usr_buf, buffer, rv)) {
        rv = -1;
    }
    return rv;
}

/**
 * This function is called each time we write to the /proc file system.
 */
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char *k_mem;

    //  allocate kernel memory
    k_mem = kmalloc(count, GFP_KERNEL);

    //  copies user space usr_buf to kernel buffer
    if (copy_from_user(k_mem, usr_buf, count)) {
        printk(KERN_INFO "Error copying from user\n");
        return -1;
    }

    //  Store the value of the pid for which we want to print stats
    sscanf(k_mem, "%ld", &l_pid);
    kfree(k_mem);

    return count;
}

module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
