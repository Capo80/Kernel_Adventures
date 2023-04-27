#include "kprobes_helper.h"

#define KPROBES_COUNT 		0
#define KRETPROBES_COUNT 	1

struct test_ret
{
	unsigned long correct_return;
};

void sk_thunk_tail(void);

int test_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	// ret_trampoline 		0xffffffff8106d4c0
	// real return address 	0xffffffff8136801d
	struct test_ret *info = (struct test_ret *)ri->data;

	//printk("Pre handler called\n");
	//save real return
	info->correct_return = *((unsigned long*) regs->sp);

	//push our return on the stack
	//regs->sp -= sizeof(unsigned long);
	*((unsigned long*) regs->sp) = (unsigned long*) sk_thunk_tail;

	//*((unsigned long*) regs->sp) = saved_rsp;

	return 0;

}
NOKPROBE_SYMBOL(test_ret_handler);

//tail call - can sleep here
void kproble_tail_call(struct kretprobe_instance *ri, struct pt_regs *regs)
{	

	printk("Tail called: %lx\n", regs->ip);

	//test if we explode
	unsigned long* test = kmalloc(8, GFP_KERNEL);
	kfree(test);

	//can sleep here
	//msleep(10);


	//free extra structures
	kfree(ri);
	kfree(regs);

	return;
}

/**
 * @brief 
 * 
 * This is a ret probe, which means all the arguments of the funtion have already been consumed
 * which means we cand use rdi, rsi directly to pass arguments to our tail call, as the value do not need to be preserverd
 * 
 */
int test_ret_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	struct kretprobe_instance *saved_ri;
	struct pt_regs *saved_regs;
	struct test_ret *info = (struct test_ret *)ri->data;

	//printk("Handler called, missed: %lu\n", ri->rph->rp->kp.nmissed);

	//printk("Preparing, preparing tail call...");

	//pass the correct return address so that our thunk can restore it
	regs->dx = info->correct_return;

	//printk("Preparing structs...");

	//save the kprobe structures to a memory area - then pass that area in the argument registers
	saved_ri = kmalloc(sizeof(struct kretprobe_instance), GFP_ATOMIC);
	saved_regs = kmalloc(sizeof(struct pt_regs), GFP_ATOMIC);
	memcpy(saved_ri, ri, sizeof(struct kretprobe_instance));
	memcpy(saved_regs, regs, sizeof(struct pt_regs));

	regs->di = (unsigned long) saved_ri;
	regs->si = (unsigned long) saved_regs;
	
	return 1;

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
			.symbol_name = "do_mmap"
		},
	},
};

struct kprobe vaultfs_kprobes[] = {
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