#include "probe_helpers.h"

struct probe_data {
	unsigned long uargs_address;
};

char* func_name	= "strndup_user";
char* valid_uargs;

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct probe_data *data;

	data = (struct probe_data *)ri->data;
	data->uargs_address = regs_get_kernel_argument(regs, 0);;
	return 0;
}
NOKPROBE_SYMBOL(entry_handler);


static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct probe_data *data;

	data = (struct probe_data *)ri->data;

	if (data->uargs_address == 0x4242)
        regs_set_return_value(regs, (unsigned long) valid_uargs);

	return 0;
}
NOKPROBE_SYMBOL(ret_handler);

static struct kretprobe my_kretprobe = {
	.handler			= ret_handler,
	.entry_handler		= entry_handler,
	.data_size		= sizeof(struct probe_data),
	/* Probe up to 20 instances concurrently. */
	.maxactive		= 20,
};

int init_probe() {

    int ret;

	valid_uargs = kmalloc(1, GFP_KERNEL);
	my_kretprobe.kp.symbol_name = func_name;

    ret = register_kretprobe(&my_kretprobe);
	if (ret < 0) {
		pr_err("register_kretprobe failed, returned %d\n", ret);
		return ret;
	}

    printk(KERN_INFO "probe in\n");
    return 0;
}

void remove_probe() {

	unregister_kretprobe(&my_kretprobe);

}
