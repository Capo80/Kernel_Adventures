#include "probe_helpers.h"

char* func_name	= "_printk";
char* valid_uargs;

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{

	pr_info("[KPROBE] A printk call is starting...\n");
	return 0;
}
NOKPROBE_SYMBOL(entry_handler);


static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{

	pr_info("[KPROBE] A printk call is ending...\n");
	return 0;
}
NOKPROBE_SYMBOL(ret_handler);

static struct kretprobe my_kretprobe = {
	.handler			= ret_handler,
	.entry_handler		= entry_handler,
	/* Probe up to 20 instances concurrently. */
	.maxactive		= 20,
};

int init_probe() {

    int ret;

	my_kretprobe.kp.symbol_name = func_name;

    ret = register_kretprobe(&my_kretprobe);
	if (ret < 0) {
		pr_err("register_kretprobe failed, returned %d\n", ret);
		return ret;
	}

    return 0;
}

void remove_probe() {

	unregister_kretprobe(&my_kretprobe);

}
