#ifndef H_ALLOCATOR
#define H_ALLOCATOR

#include <linux/slab.h>
#include <linux/numa.h>
#include <linux/vmalloc.h>
#include "symbol_lookup.h"

void init_allocator(void);
void *alloc_executable_mem(unsigned long size);

#endif