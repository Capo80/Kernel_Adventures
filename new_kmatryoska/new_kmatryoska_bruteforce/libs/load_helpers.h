#ifndef H_LOAD_HELP
#define H_LOAD_HELP

#include <linux/module.h>

#define COPY_CHUNK_SIZE (16*PAGE_SIZE)


struct load_info {
	const char *name;
	/* pointer to module in temporary copy, freed at end of load_module() */
	struct module *mod;
	void *hdr; //changed pointer type dont care
	unsigned long len;
    /* more stuff down here ... dont care*/
	char padding[10000];
};

void init_load_symbols(void);
int copy_chunked_from_kernel(void *dst, const void *usrc, unsigned long len);

// copy load_module from kernel source
//int load_module(struct load_info *info, const char *uargs, int flags);

/* kernel symbols */
//asmlinkage int (*module_sig_check)(struct load_info *, int) = NULL;
//asmlinkage int (*elf_validity_check)(struct load_info *) = NULL;   

#endif /* H_LOAD_HELP */