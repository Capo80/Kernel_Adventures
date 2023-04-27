#ifndef H_SYS_DISPATCHER
#define H_SYS_DISPATCHER

#include <linux/preempt.h>
#include <linux/kthread.h>
#include <linux/hardirq.h>
#include <linux/percpu.h>

void kernel_do_syscall_64(struct pt_regs *regs);

#endif