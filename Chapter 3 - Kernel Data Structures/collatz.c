/**
 *  Kernel module that receives a number as a module parameter and
 *  computes the Collatz sequence of that number. It is stored in a
 *  linked list and thenm printed to the kernel log buffer.
 */
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#define MODULE_NAME     "collatz"

//  establish the "start" variable as a parameter to the kernel module
static int start = 25;
module_param(start, int, 0);

struct collatz_element {
    int value;
    struct list_head list;
}

void add_number(int number, struct list_head *list) {
    struct collatz_number *elem;
    elem = kmalloc(sizeof(*elem), GFP_KERNEL);
    elem->value = number;

    INIT_LIST_HEAD(&elem->list);  //  initialize the list member in "elem"
    list_add_tail(&elem->list, list);   //  add the element to the linked list
}

static int module_init_function(void) {
    static LIST_HEAD(collatz_sequence);
    struct collatz_element *ptr, *next;
    int number = start;

    printk(KERN_INFO "%s module loaded!\n", MODULE_NAME);

    if (number <= 0) {
        printk(KERN_INFO "Error: the starting number must be an integer greater than 0!\n");
        return 0;
    }

    //  build the collatz sequence and store it in a linked list
    while (number > 1) {
        add_number(number, &collatz_sequence);

        if (number % 2 == 0) {
            number /= 2;
        } else {
            number = number * 3 + 1;
        }
    }

    add_number(1, &collatz_sequence);

    //  traverse the list and print the sequence
    list_for_each_entry(ptr, &collatz_sequence, list) {
        printk(KERN_INFO "%d\n", ptr->value);
    }

    //  free memory
    list_for_each_entry_safe(ptr, next, &collatz_sequence, list) {
        list_del(&ptr->list);
        kfree(ptr);
    }

    return 0;
}

static void module_exit_function(void) {
    printk(KERN_INFO "%s module removed!\n", MODULE_NAME);
}

module_init( module_init_function );
module_exit( module_exit_function );
