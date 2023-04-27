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
#include <linux/string.h>

#include "libs/probe_helpers.h"
#include "libs/smart_api_finder.h"

static __init int test_init(void) {

	init_probe();

	printk("This is a normal printk call");

	smart_api_call("_printk", 1, "This is smart printk call\n");	

	return 0;
}

void __exit test_exit(void) {

	remove_probe();

	pr_info("module exit\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
