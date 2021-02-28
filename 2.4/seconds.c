#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include <asm/param.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t, *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};

static unsigned long initial_jiffies;

/*  This function gets called when the module is loaded. */
int proc_init(void) {
    //  Creates the /proc/seconds/ entry
    proc_create(PROC_NAME, 0, NULL, &proc_ops);
    initial_jiffies = jiffies;
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

/*  This function gets calledn when the module is removed. */
void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/*  This function gets called when /proc/seconds is read. */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    unsigned long rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    
    unsigned long current_jiffies = jiffies;
    unsigned long seconds_elapsed = (current_jiffies - initial_jiffies) / HZ;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    rv = sprintf(buffer, "%lu\n", seconds_elapsed);

    //  Copies kernel space buffer to user space buffer usr_buf
    copy_to_user(usr_buf, buffer, rv);

    return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Computes the number of seconds elapsed since the module was first inserted in the kernel");
MODULE_AUTHOR("Worg");
