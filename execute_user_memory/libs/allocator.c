#include "allocator.h"

asmlinkage void* (*__my_vmalloc_node_range)(unsigned long, unsigned long,
			unsigned long, unsigned long, gfp_t,
			pgprot_t, unsigned long, int,
			const void*);  

static unsigned char vmalloc_range_signature[] = {
	0xf,	0x1f,	0x44,	0x0,	0x0,	0x55,	0x48,	0x89,
	0xe5,	0x41,	0x57,	0x41,	0x56,	0x44,	0x8b,	0x75,
	0x18,	0x41,	0x55,	0x41,	0x54,	0x53,	0x48,	0x85,
	0xff,	0xf,	0x84,	0x93,	0x0,	0x0,	0x0,	0x49,
};

void init_allocator() {
    __my_vmalloc_node_range = (void*) lookup_signature((void*) vmalloc_range_signature, sizeof(vmalloc_range_signature));
    printk(KERN_INFO "Found __vmalloc: %lx", (unsigned long) __my_vmalloc_node_range);
}

void *alloc_executable_mem(unsigned long size) {

    // new vmalloc allocation for executable memory
    // https://elixir.bootlin.com/linux/v5.13/source/mm/vmalloc.c#L2865
	void *p = __my_vmalloc_node_range(size, 1, VMALLOC_START, VMALLOC_END,
			GFP_KERNEL, PAGE_KERNEL_EXEC, 0,
			NUMA_NO_NODE, __builtin_return_address(0));

	return p;    
}