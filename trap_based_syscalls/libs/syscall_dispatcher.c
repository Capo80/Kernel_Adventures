#include "syscall_dispatcher.h"

#define __X32_SYSCALL_BIT	    0x40000000
#define __SYSCALL_MASK          (~(__X32_SYSCALL_BIT))
#define __NR_syscall_max        547
#define NR_syscalls             __NR_syscall_max + 1

typedef asmlinkage long (*sys_call_ptr_t)(unsigned long, unsigned long,
					  unsigned long, unsigned long,
					  unsigned long, unsigned long);

sys_call_ptr_t stub_syscall_table[] = {
    (sys_call_ptr_t) do_exit,
};

DECLARE_PER_CPU(int, __preempt_count);

static __always_inline unsigned char interrupt_context_level(void)
{
	unsigned long pc = preempt_count();
	unsigned char level = 0;

	level += !!(pc & (NMI_MASK));
	level += !!(pc & (NMI_MASK | HARDIRQ_MASK));
	level += !!(pc & (NMI_MASK | HARDIRQ_MASK | SOFTIRQ_OFFSET));

	return level;
}

void kernel_do_syscall_64(struct pt_regs *regs) {
    
    unsigned long nr = regs->ax;
    int preemp_save = __preempt_count;

    //YOLO
    raw_cpu_add_4(__preempt_count, -preemp_save);
    
    //  eflags 46
    //  Context: 110000
    pr_info("nmi %dx\n",  in_nmi());
    pr_info("hirq %d\n",  in_hardirq());
    pr_info("sirq %d\n",  in_serving_softirq());
    pr_info("task %d\n",  in_task());

    pr_info("Context level %d\n",  nmi_count());

    
    local_irq_enable();


    pr_info("Syscall %ld starting...\n", regs->orig_ax);

    pr_info("Context level %lx\n",  preempt_count());

    //call correct syscall
    if (likely((nr & __SYSCALL_MASK) < NR_syscalls)) {
		//regs->ax = sys_call_table[nr & __SYSCALL_MASK](
		regs->ax = stub_syscall_table[0](
        	regs->di, regs->si, regs->dx,
			regs->r10, regs->r8, regs->r9);
	}
    
    if (irqs_disabled()) {
        printk("ERROR: IRQs are disabled after syscall %ld\n", regs->orig_ax);
        local_irq_enable();
    }
	
    
    local_irq_disable();

    raw_cpu_add_4(__preempt_count, preemp_save);

}

