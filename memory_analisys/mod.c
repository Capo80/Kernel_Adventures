#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/mm_types.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/stddef.h>

#include "page_table_libs/page_table_utils.h"

#define CHECK_PID 4524

static __init int test_init(void) {

	struct task_struct* p;
	struct vm_area_struct* vm;
        unsigned long page;
        unsigned int not_present = 0;

	printk("module init");

        for_each_process(p) {

		if (p->pid == CHECK_PID) {
			printk("Task %s (pid = %d)\n",p->comm, task_pid_nr(p));

			if (p->mm != NULL && p->mm->mmap != NULL) {
				vm = p->mm->mmap;
				while (vm != NULL) {
                                        not_present = 0;

					printk("addr: %lx - %lx\n", vm->vm_start, vm->vm_end);
                                        for (page = ((unsigned long) vm->vm_start) & PAGE_MASK; page < (unsigned long) vm->vm_end; page += PAGE_SIZE) {
                                                
                                                if (!is_bit_set(page, ACCESS_BIT)) {
                                                        not_present += 1;
                                                }

                                        }

                                        printk("pages not present: %u / %lx\n", not_present, ((unsigned long) vm->vm_end - vm->vm_start)/PAGE_SIZE);

					vm = vm->vm_next;
				}
			}
		}
	}
        return 0;
}

void __exit test_exit(void) {

        printk("module exit");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
