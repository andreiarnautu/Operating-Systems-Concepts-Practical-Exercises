/**
 *  Module that creates a linked list with 4 elements, traverses it and
 *  prints its contents to the kernel log buffer
 */
#include <linux/list.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>

#define MODULE_NAME     "color_list"

struct color {
    int red;
    int blue;
    int green;
    struct list_head list;
};

static void add_new_color(const int red, const int blue, const int green, struct list_head *color_list) {
    struct color *new_color;

    new_color = kmalloc(sizeof(*new_color), GFP_KERNEL);

    new_color->red = red;
    new_color->blue = blue;
    new_color->green = green;

    INIT_LIST_HEAD(&new_color->list);  //  initialize the "list" member in "new_color"
    list_add_tail(&new_color->list, color_list);  //  add the color to the end of the linked list
}

static int module_init_function(void) {
    struct color *ptr, *next;
    static LIST_HEAD(color_list);
    
    printk(KERN_INFO "%s module loaded!\n", MODULE_NAME);

    //  Create a list_head object, which we use as a reference to the head of the list
    add_new_color(255, 0, 0, &color_list);  //  red
    add_new_color(0, 255, 0, &color_list);  //  blue
    add_new_color(0, 0, 255, &color_list);  //  green
    add_new_color(255, 0, 165, &color_list);  //  orange

    //  Traverse the linked list and print the colors
    list_for_each_entry(ptr, &color_list, list) {
        printk(KERN_INFO "red = %d -- blue = %d -- green = %d\n", ptr->red, ptr->blue, ptr->green);
    }

    //  Free memory
    list_for_each_entry_safe(ptr, next, &color_list, list) {
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
