#ifndef MEM_HELP_H
#define MEM_HELP_H

#include <linux/module.h>
#include <linux/sched/mm.h>
#include <linux/slab.h>
#include <linux/rbtree.h>
#include <linux/mmap_lock.h>

#include "page_table_libs/page_table_utils.h"

void init_memory_helpers(void);

void inline reserve_current_address_space(void);
void inline release_current_address_space(void);

struct mm_struct* change_mm(void);
void restore_mm(struct mm_struct* old_mm);

#endif /* MEM_HELP_H */