#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/string.h>
#include <linux/jiffies.h>

#include "libs/kprobes_helper.h"

static __init int test_init(void) {
	pr_info("module init\n");
	return init_kprobes();	

}

void __exit test_exit(void) {


	remove_probes();
	pr_info("module exit\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
