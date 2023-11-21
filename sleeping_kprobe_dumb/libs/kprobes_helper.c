#include "kprobes_helper.h"

#define KPROBES_COUNT 		0
#define KRETPROBES_COUNT 	1

struct test_ret
{
	unsigned long test;
};

#define UDELAY 300

int test_handler(struct kprobe *ri, struct pt_regs *regs)
{	

	int ret;

	ret = kprobe_sleep_header(ri);
	if (ret < 0) {
		return 0; //we failed - cant sleep
	}

	if (!in_atomic()) {	//we could still be in a non-preemptibable situation
		usleep_range(UDELAY-10, UDELAY+10);
	}

	//printk("nmissed: %lu\n", ri->nmissed);

	ret = kprobe_sleep_footer(ri); // dont know how to recover if we fail here
	return 0;
	
}
NOKPROBE_SYMBOL(test_handler);

int test_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	int ret;

	ret = kretprobe_sleep_header(ri);
	if (ret < 0) {
		return 0; //we failed - cant sleep
	}

	if (!in_atomic()) {	//we could still be in a non-preemptibable situation
		usleep_range(UDELAY-10, UDELAY+10);
	}

	ret = kretprobe_sleep_footer(ri); // dont know how to recover if we fail here
	return 0;

}
NOKPROBE_SYMBOL(test_ret_handler);

int test_ret_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	

	int ret;

	ret = kretprobe_sleep_header(ri);
	if (ret < 0) {
		return 0; //we failed - cant sleep
	}

	if (!in_atomic()) {	//we could still be in a non-preemptibable situation
		usleep_range(UDELAY-10, UDELAY+10);
	}

	ret = kretprobe_sleep_footer(ri); // dont know how to recover if we fail here
	return 0;

}
NOKPROBE_SYMBOL(test_ret_ret_handler);

struct kretprobe vaultfs_kretprobes[] = {
	{
		.handler		= test_ret_ret_handler,
		.entry_handler		= test_ret_handler,
		.data_size		= sizeof(struct test_ret),
		/* Probe up to 20 instances concurrently. */
		.maxactive		= 20,
		.kp = {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0)
			.symbol_name = "close_fd"
#else
			.symbol_name = "__close_fd"
#endif
		}
	},
};

struct kprobe vaultfs_kprobes[] = {
	{
		.pre_handler	= test_handler,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,11,0)
		.symbol_name = "close_fd"
#else
		.symbol_name = "__close_fd"
#endif
	}
};

int init_kprobes(void) {
	int i, ret;
	
	for (i = 0; i < KRETPROBES_COUNT; i++) {
        ret = register_kretprobe(&vaultfs_kretprobes[i]);
        if (ret < 0) {
            printk(KERN_ERR "register_kretprobe %d failed, returned %d\n", i, ret);
            goto exit;
        }
    }

    for (i = 0; i < KPROBES_COUNT; i++) {
        ret = register_kprobe(&vaultfs_kprobes[i]);
        if (ret < 0) {
            printk(KERN_ERR "register_kprobe failed, returned %d\n", ret);
            goto unreg_kretprobes;
        }
    }

	return ret;

unreg_kretprobes:
    for (i = 0; i < KRETPROBES_COUNT; i++)
        unregister_kretprobe(&vaultfs_kretprobes[i]);
exit:
	return ret;

}

void remove_probes(void) {
	int i;

    for (i = 0; i < KPROBES_COUNT; i++)
        unregister_kprobe(&vaultfs_kprobes[i]);

    for (i = 0; i < KRETPROBES_COUNT; i++)
        unregister_kretprobe(&vaultfs_kretprobes[i]);
}