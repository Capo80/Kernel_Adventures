/*
* 
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.
* 
* This module is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* @file mod.c 
* @brief This is a source for the Linux Kernel Module which sets up
*        a not used IDT entry 
*        the actual isytalle handler works with PTI-off setting of the kernel 
*
* @author Francesco Quaglia
*
* @date November 22, 2020
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kprobes.h>
#include <linux/delay.h>
#include <linux/signal.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/ptrace.h>

#include "libs/trap_controller.h"

extern void my_handler(void);

static int tracee_kthread(void *unused) {
	
	pr_info("Tracee started: %d\n", current->pid);

	//disallow_signal(SIGTRAP);

    pr_info("Context level %lx\n",  preempt_count());
	
	set_current_state(TASK_INTERRUPTIBLE);
	
    pr_info("Context level %lx\n",  preempt_count());

	asm volatile("int $0x3\n":
	: "a" (10000)
	:);
	//schedule();

	pr_info("Int called\n");

	do_exit(0);

}

static __init int my_irq_init(void) {

	int err = 0;
	struct task_struct* tracee_ts;

	printk("module init\n");

	err = setup_tracer();
	if (err) goto out;

	pr_info("Tracer setup complete");
    
    //Create the kernel thread with name 'mythread'
    tracee_ts = kthread_create(tracee_kthread, NULL, "tracee");
    if (tracee_ts) {
            pr_info("Thread Created successfully\n");
			//wake_up_process(tracee_ts);
			trace_task(tracee_ts);
    } else {
            pr_info("Thread creation failed\n");
            return PTR_ERR(tracee_ts);
    }
	
out:
	return err;
}

void __exit my_irq_exit(void) {

	teardown_tracer();

	pr_info("module exit\n");

}

module_init(my_irq_init);
module_exit(my_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia");
