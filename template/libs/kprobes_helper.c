#include "kprobes_helper.h"

#define KPROBES_COUNT 		0
#define KRETPROBES_COUNT 	1

int do_ioctl_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	
	struct file* file = (struct file*) regs_get_kernel_argument(regs, 0);
	unsigned int fd = (unsigned long) regs_get_kernel_argument(regs, 1);
	unsigned int cmd = (unsigned int) regs_get_kernel_argument(regs, 2);
	unsigned long arg = (unsigned long) regs_get_kernel_argument(regs, 3);
	struct addr_info *addr_info = (struct addr_info *)ri->data;
	
	return 0;

}
NOKPROBE_SYMBOL(do_ioctl_handler);

int do_ioctl_ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{	
	int sys_ret = regs_return_value(regs);
	struct addr_info *addr_info = (struct addr_info *)ri->data;

	return 0;

}
NOKPROBE_SYMBOL(do_ioctl_ret_handler);

struct kretprobe vaultfs_kretprobes[] = {
	{
		.handler		= do_ioctl_ret_handler,
		.entry_handler		= do_ioctl_handler,
		.data_size		= sizeof(struct addr_info),
		/* Probe up to 20 instances concurrently. */
		.maxactive		= 20,
		.kp = {
			//.symbol_name = "vfs_ioctl"
			.symbol_name = "do_vfs_ioctl"
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