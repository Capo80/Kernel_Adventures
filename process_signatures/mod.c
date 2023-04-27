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
#include <linux/crypto.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>

#include "libs/probe_helpers.h"
#include "libs/hash_helper.h"


void hash_analyzer(struct work_struct *work);
DECLARE_DELAYED_WORK(hash_work, hash_analyzer);

void hash_analyzer(struct work_struct *work) {

	printk("Starting hash analysis\n");
	save_hash_snapshot("/info.txt");

	schedule_delayed_work(&hash_work, msecs_to_jiffies(30000));
}

static __init int test_init(void) {
	
	
	schedule_delayed_work(&hash_work, msecs_to_jiffies(10));

	//init_probe();

	return 0;
}

void __exit test_exit(void) {

	/*
	int i = 0;
	struct list_head *position = NULL ; 
	struct list_head *vm_position = NULL ; 
	struct process_info  *info  = NULL ;
	struct vm_area_info  *vm_info  = NULL ;
	list_for_each ( position , &process_info_list ) 
    { 
		info = list_entry(position, struct process_info , list ); 
		printk ("path  =  %s, vm_count = %d\n" , info->pathname , info->vm_area_number);
		list_for_each (vm_position , &info->vm_area_list ) {
			vm_info = list_entry(vm_position, struct vm_area_info , list ); 
			printk("\t- vm_size = %lu, vm_path = %s, vm_md5 = %s\n", vm_info->size, vm_info->path, vm_info->md5);
		}
    }
	remove_probe();
	*/

	flush_scheduled_work();

	pr_info("module exit\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
