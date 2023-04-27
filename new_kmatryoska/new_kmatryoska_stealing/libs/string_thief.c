#include "string_thief.h"

static struct kprobe kp = {
	.symbol_name	= "do_sys_openat2",
};
unsigned char done = 0;
char* stolen_user_address = NULL;
DEFINE_MUTEX(wait_steal_mutex);
DEFINE_SPINLOCK(wait_load_lock);
DECLARE_WAIT_QUEUE_HEAD(wait_steal_queue);

static int __kprobes handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    char* filename;
    long len;

    if (__sync_bool_compare_and_swap(&done, 0, 1)) {

        printk("got in - passing pointer\n");
        
        char* filename = (char*) regs_get_kernel_argument(regs, 1);
        len = strnlen_user(filename, 4096);

        stolen_user_address = filename + len - 1;

        printk(KERN_INFO "Stolen user address %p - strlen: %ld\n", stolen_user_address, strnlen_user(stolen_user_address, 4096));
        wake_up(&wait_steal_queue);

    }

	return 0;
}


int init_probe() {

    int ret;

    done = 0;
	kp.pre_handler = handler_pre;

    ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
    printk(KERN_INFO "probe in\n");
    return 0;
}

void remove_probe() {

    unregister_kprobe(&kp);

}
