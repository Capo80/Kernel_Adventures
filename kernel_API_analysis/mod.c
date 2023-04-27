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
//#include "syms.h"

unsigned int SYMS_TOTAL = 139262;
struct file* info_file = NULL;
loff_t pos = 0;

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};
/*
unsigned long (*_kallsyms_lookup_name)(const char *name) = NULL;
int (*_kallsyms_on_each_symbol)(int (*fn)(void *, const char *, struct module *, unsigned long), void *data) = NULL;

static int save_symbol(void *data, const char *namebuf, struct module *module, unsigned long address)
{
	unsigned long lookup_addr;
	int to_write;
	char buffer[1000];

	if (address >= 0xffffffff81000000) {
		to_write = snprintf(buffer, 1000, "%s:\t\t%lx\n", namebuf, *((unsigned long*)address));
		to_write = kernel_write(info_file, buffer, to_write, &pos);
	}
    return 0;
}

static __init int test_init(void) {
	
	int to_write, i;
	char buffer[1000];
	int ret = 0;
	
	info_file = filp_open("/home/user/Desktop/info.txt", O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG| S_IRWXO);

	register_kprobe(&kp);

    _kallsyms_lookup_name = (unsigned long (*)(const char* name)) (((unsigned long)kp.addr) + 5) ;

	unregister_kprobe(&kp);

	_kallsyms_on_each_symbol = (int (*)(int (*fn)(void *, const char *, struct module *, unsigned long), void *data)) _kallsyms_lookup_name("kallsyms_on_each_symbol");
	
	pos = 0;
	for (i = 0; i < SYMS_TOTAL; i++) {
		
		kp.symbol_name = syms[i];
		ret = register_kprobe(&kp);
		if (ret >= 0) {
			to_write = snprintf(buffer, 1000, "%s\n", syms[i]);
			to_write = kernel_write(info_file, buffer, to_write, &pos);			
		}
		unregister_kprobe(&kp);
		if (i % 1000 == 0)
			printk("%d/%d\n", i, SYMS_TOTAL);
		//printk("%lx\n", _kallsyms_lookup_name("__SCK__tp_func_icc_set_bw_end"));
	}
	//_kallsyms_on_each_symbol(save_symbol, NULL);

	return 0;
}
*/

static __init int test_init(void) {

	register_kprobe(&kp);

	return 0;
}
void __exit test_exit(void) {


	pr_info("module exit\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
