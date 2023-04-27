#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");

MODULE_AUTHOR("Pasquale Caporaso");
MODULE_DESCRIPTION("Workqueue test");				  

void wq_handler(struct work_struct* args);
void wq_handler_2(struct work_struct* args);
DECLARE_DELAYED_WORK(rep_workqueue, wq_handler);
DECLARE_DELAYED_WORK(rep_workqueue_2, wq_handler_2);

struct gdt_desc_ptr {
	unsigned short size;
	unsigned long address;
} __attribute__((packed)) ;

#define store_gdt(ptr) asm volatile("sgdt %0":"=m"(*ptr))

void wq_handler(struct work_struct* args) {

    struct gdt_desc_ptr gdtptr;
    unsigned long* seg_info;

    store_gdt(&gdtptr);

    seg_info = (unsigned long*) gdtptr.address;
	printk("[WQ] GDTR is at %lx - size is %d\n",gdtptr.address, gdtptr.size); 

    printk("[WQ] Null segment: %lx\n", seg_info[0]);
    printk("[WQ] KC segment: %lx\n", seg_info[1]);
    printk("[WQ] KD segment: %lx\n", seg_info[2]);
    printk("[WQ] UC segment: %lx\n", seg_info[3]);
    printk("[WQ] UD segment: %lx\n", seg_info[4]);

    //schedule_delayed_work(&rep_workqueue, msecs_to_jiffies(2000));
}


void wq_handler_2(struct work_struct* args) {

    printk("Hello! i am the work queue 2\n");
    printk("working on mm_struct: %lx\n", (unsigned long) current->active_mm);
    printk("Working on page table: %lx\n", (unsigned long) current->active_mm->pgd);


    schedule_delayed_work(&rep_workqueue_2, msecs_to_jiffies(2000));
}

int init_module(void) {
	
    printk("Scheduling work queue...\n");

    struct gdt_desc_ptr gdtptr;
    unsigned long* seg_info;

    store_gdt(&gdtptr);

    seg_info = (unsigned long*) gdtptr.address;
	printk("[INIT] GDTR is at %lx - size is %d\n",gdtptr.address, gdtptr.size); 

    printk("[INIT] Null segment: %lx\n", seg_info[0]);
    printk("[INIT] KC segment: %lx\n", seg_info[1]);
    printk("[INIT] KD segment: %lx\n", seg_info[2]);
    printk("[INIT] UC segment: %lx\n", seg_info[3]);
    printk("[INIT] UD segment: %lx\n", seg_info[4]);
    schedule_delayed_work(&rep_workqueue, 0);
    //schedule_delayed_work(&rep_workqueue_2, 0);
	return 0;
}

void cleanup_module(void) {

    cancel_delayed_work(&rep_workqueue);
    cancel_delayed_work(&rep_workqueue_2);

	printk("Shutting down\n");
}