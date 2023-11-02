#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kprobes.h>

#include "control.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luca Capotombolo");

static __init int init_discovery_spurious(void) {

	int err = 0;

	printk("%s: module init\n",MOD_NAME);

	err = discovery_spurious_entry();

	if (err) goto out;

out:
	return err;
}

void __exit exit_discovery_spurious(void) {

	pr_info("%s: module exit\n",MOD_NAME);

}

module_init(init_discovery_spurious);
module_exit(exit_discovery_spurious);
