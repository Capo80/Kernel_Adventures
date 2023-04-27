#include "kprobes_helper.h"

#define KPROBES_COUNT 		0
#define KRETPROBES_COUNT 	1

struct test_ret
{
	unsigned long test;
};

DEFINE_PER_CPU(unsigned long, test);

int test_handler(struct kprobe *ri, struct pt_regs *regs)
{	
	//printk("kprobe: %lx\n", (unsigned long) ri);
	unsigned long* temp = (unsigned long) &test;
	while (temp > 0) {
		temp -= 1; 
		//printk("kprobe: %lx\n", (unsigned long) temp);
		//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
		if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) ri) {
			//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
			break;
		}
	}

	if (temp == 0) {
		pr_err("failed to find per_cpu kprobe variable\n");
		return 0;
	}

	printk("nmissed: %lu\n", ri->nmissed);

	__this_cpu_write(*temp, NULL);
	preempt_enable();
	msleep(100);
	preempt_disable();
	__this_cpu_write(*temp, ri);

	return 0;
	
}
NOKPROBE_SYMBOL(test_handler);

int test_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	
	//struct addr_info *addr_info = (struct addr_info *)ri->data;

	//printk("kprobe: %lx\n", (unsigned long) ri);
	// unsigned long* temp = (unsigned long) &test;
	
	// while (temp > 0) {
	// 	temp -= 1; 
	// 	//printk("kprobe: %lx\n", (unsigned long) temp);
	// 	//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
	// 	if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) &ri->rph->rp->kp) {
	// 		//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
	// 		break;
	// 	}
	// }

	// //printk("value: %lx - %lx - %lx\n", (unsigned long) &ri->rph->rp->kp, (unsigned long) temp, __this_cpu_read(*temp));
	// if (temp == 0) {
	// 	pr_err("failed to find per_cpu kprobe variable\n");
	// 	return 0;
	// }

	// printk("nmissed: %lu\n", ri->rph->rp->kp.nmissed);
	
	// __this_cpu_write(*temp, NULL);
	// preempt_enable();
	// msleep(100);
	// preempt_disable();
	// __this_cpu_write(*temp, ri);
	return 0;

}
NOKPROBE_SYMBOL(test_ret_handler);

int test_ret_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	//struct addr_info *addr_info = (struct addr_info *)ri->data;
	unsigned long* temp = (unsigned long) &test;
	
	while (temp > 0) {
		temp -= 1; 
		//printk("kprobe: %lx\n", (unsigned long) temp);
		//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
		if ((unsigned long) __this_cpu_read(*temp) == (unsigned long) &ri->rph->rp->kp) {
			//printk("value: %lx - %lx\n", (unsigned long) temp, __this_cpu_read(*temp));
			break;
		}
	}

	//printk("value: %lx - %lx - %lx\n", (unsigned long) &ri->rph->rp->kp, (unsigned long) temp, __this_cpu_read(*temp));
	if (temp == 0) {
		pr_err("failed to find per_cpu kprobe variable\n");
		return 0;
	}

	printk("nmissed: %lu\n", ri->rph->rp->kp.nmissed);
	
	__this_cpu_write(*temp, NULL);
	preempt_enable();
	msleep(100);
	preempt_disable();
	__this_cpu_write(*temp, ri);
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
			.symbol_name = "close_fd"
		}
	},
};

struct kprobe vaultfs_kprobes[] = {
	{
		.pre_handler	= test_handler,
		.symbol_name 	= "close_fd" 
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