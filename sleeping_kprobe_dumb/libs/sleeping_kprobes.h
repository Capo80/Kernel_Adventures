#ifndef SLEEP_KPROBES_H
#define SLEEP_KPROBES_H

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

int kprobe_sleep_header(struct kprobe *ri);
int kprobe_sleep_footer(struct kprobe *ri);
int kretprobe_sleep_header(struct kretprobe_instance *ri);
int kretprobe_sleep_footer(struct kretprobe_instance *ri);

#endif