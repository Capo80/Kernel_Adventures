#ifndef H_TRAP_CONTROLLER
#define H_TRAP_CONTROLLER

#include <linux/hashtable.h>
#include <linux/kdebug.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include "syscall_dispatcher.h"

void trace_task(struct task_struct* ts);
int teardown_tracer(void);
int setup_tracer(void);


#endif