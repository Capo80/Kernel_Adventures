#ifndef H_LOAD_HELP
#define H_LOAD_HELP

#include <linux/module.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <linux/notifier.h>
#include <linux/livepatch.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/moduleloader.h>
#include <linux/dynamic_debug.h>

#define COPY_CHUNK_SIZE (16*PAGE_SIZE)
#define DEBUG 1

struct load_info {
	const char *name;
	/* pointer to module in temporary copy, freed at end of load_module() */
	struct module *mod;
	void *hdr; //changed pointer type dont care
	unsigned long len;
	void *sechdrs; //changed pointer dont care
	char *secstrings, *strtab;
	unsigned long symoffs, stroffs, init_typeoffs, core_typeoffs;
	struct _ddebug *debug;
	unsigned int num_debug;
	bool sig_ok;
    /* more stuff down here ... dont care*/
	char padding[10000];
};

void init_load_symbols(void);
int copy_chunked_from_kernel(void *dst, const void *usrc, unsigned long len);

// copy load_module from kernel source
int load_module(struct load_info *info, const char *uargs, int flags);


#endif /* H_LOAD_HELP */