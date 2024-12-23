#include "kprobes_helper.h"
#include "linux/mm_types.h"

#define KPROBES_COUNT 		0
#define KRETPROBES_COUNT 	1

#define HOOK			"a.out"

struct addr_info {
    
    unsigned long address;
    unsigned long flags;
};

int fault_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	


	// hook only one process
	if (strstr(current->comm, HOOK) == NULL)
	    return 0;

	unsigned long address = (unsigned long) regs_get_kernel_argument(regs, 1);
	unsigned long flags = (unsigned int) regs_get_kernel_argument(regs, 2);
	
	struct addr_info *addr_info = (struct addr_info *)ri->data;

	addr_info->address = address;
	addr_info->flags = flags;
	return 0;

}
NOKPROBE_SYMBOL(fault_handler);

int fault_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	

	if (strstr(current->comm, HOOK) == NULL)
	    return 0;
	
	struct addr_info *addr_info = (struct addr_info *)ri->data;
	if (addr_info->address != 0) {

	    pr_info("Got fault at:	    %lx\n", addr_info->address);
	    pr_info("- W access:	    %x\n", addr_info->flags & FAULT_FLAG_WRITE);
	    pr_info("- X access:	    %x\n", addr_info->flags & FAULT_FLAG_INSTRUCTION);
	    pr_info("- Remote access:	    %x\n", addr_info->flags & FAULT_FLAG_REMOTE);
	    pr_info("- PTE access:	    %x\n", addr_info->flags & FAULT_FLAG_ORIG_PTE_VALID);

	}


	return 0;

}
NOKPROBE_SYMBOL(fault_ret_handler);

struct kretprobe vaultfs_kretprobes[] = {
	{
		.handler		= fault_ret_handler,
		.entry_handler		= fault_handler,
		.data_size		= sizeof(struct addr_info),
		/* Probe up to 20 instances concurrently. */
		.maxactive		= 20,
		.kp = {
			.symbol_name = "handle_mm_fault"
		}
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
