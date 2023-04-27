#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include "libs/kprobes_helper.h"

//    0xffffffffc066e00c:  mov    QWORD PTR gs:[rip+0x3f9c6048],0x123        # 0x34060
//    0xffffffffc066e018:  mov    rsi,QWORD PTR gs:[rip+0x3f9c6040]        # 0x34060


//DEFINE_PER_CPU(unsigned long *, test);

static __init int test_init(void) {

	pr_info("module init\n");
	pr_info("module func: %lx\n", (unsigned long) test_handler);

	return init_kprobes();
}

void __exit test_exit(void) {

	remove_probes();

    ssleep(1);

	pr_info("module exit\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
