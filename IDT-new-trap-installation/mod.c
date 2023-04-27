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

#include "control.h"

extern void my_handler(void);

static __init int my_irq_init(void) {

	int err = 0;

	printk("%s: module init\n",MOD_NAME);

	err = setup_my_irq();
	if (err) goto out;

	//trigger interrupt
	asm volatile("int $0xfa":::);

out:
	return err;
}

void __exit my_irq_exit(void) {

	cleanup_my_irq();

	pr_info("%s: module exit\n",MOD_NAME);

}

module_init(my_irq_init);
module_exit(my_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francesco Quaglia");
