#ifndef KPROBES_H
#define KPROBES_H

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/kprobes.h>
#include <linux/slab.h>
#include <linux/dcache.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/time64.h>
#include <linux/timex.h>
#include <linux/limits.h>
#include <linux/uaccess.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/preempt.h>
#include <linux/delay.h>
#include <linux/hashtable.h>
#include <linux/percpu.h>


int test_handler(struct kprobe *ri, struct pt_regs *regs);

int init_kprobes(void);
void remove_probes(void);

#endif