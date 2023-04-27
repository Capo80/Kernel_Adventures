#include "probe_helpers.h"
#define MAX_CALLS 		10
#define HASH_SIZE		100
#define MD5_HEX_SIZE	32

char calls;
char* func_name	= "close_fd";
struct list_head process_info_list;

struct sdesc {
    struct shash_desc shash;
    char ctx[];
};

static struct sdesc *init_sdesc(struct crypto_shash *alg)
{
    struct sdesc *sdesc;
    int size;

    size = sizeof(struct shash_desc) + crypto_shash_descsize(alg);
    sdesc = kmalloc(size, GFP_KERNEL);
    if (!sdesc)
        return ERR_PTR(-ENOMEM);
    sdesc->shash.tfm = alg;
    return sdesc;
}

static int calc_hash(struct crypto_shash *alg,
             const unsigned char *data, unsigned int datalen,
             unsigned char *digest)
{
    struct sdesc *sdesc;
    int ret;

    sdesc = init_sdesc(alg);
    if (IS_ERR(sdesc)) {
        pr_info("can't alloc sdesc\n");
        return PTR_ERR(sdesc);
    }

    ret = crypto_shash_digest(&sdesc->shash, data, datalen, digest);
    kfree(sdesc);
    return ret;
}

//return hex string
static int do_md5(const unsigned char *data, unsigned char *out_digest, unsigned long size)
{
    struct crypto_shash *alg;
	int i;
    char *hash_alg_name = "md5";
    unsigned int datalen = size; // remove the null byte

    alg = crypto_alloc_shash(hash_alg_name, 0, 0);
    if(IS_ERR(alg)){
        pr_info("can't alloc alg %s\n", hash_alg_name);
        return PTR_ERR(alg);
    }

    unsigned char hash[32];
    calc_hash(alg, data, datalen, hash);

    // Very dirty print of 8 first bytes for comparaison with sha256sum
    //printk(KERN_INFO "HASH(%s, %i): %02x%02x%02x%02x%02x%02x%02x%02x\n",
    //      data, datalen, hash[0] & 0xFFu, hash[1] & 0xFFu, hash[2] & 0xFFu, hash[3] & 0xFFu, hash[4] & 0xFFu, 
    //      hash[5] & 0xFFu, hash[6] & 0xFFu, hash[7] & 0xFFu);

    char c[3];
    for (i = 0; i < 16; i++)
    {
        sprintf( c, "%02x", hash[i] & 0xFFu);
        memcpy(out_digest+i*2, c, 2);
    }

    crypto_free_shash(alg);
    return 0;
}

struct nameidata {
	struct path	path;
};

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	calls = 0;
	if (calls < MAX_CALLS) {

		char *pathname,*p;
		struct mm_struct* mm = current->mm;
		if (mm) {

			struct process_info* pi = kmalloc(sizeof(struct process_info), GFP_ATOMIC);

			if (mm->exe_file) {

				//save pathname
				pathname = kmalloc(PATH_MAX, GFP_ATOMIC);
				if (pathname) {
					p = d_path(&mm->exe_file->f_path, pathname, PATH_MAX);
					/*Now you have the path name of exe in p*/
					pi->pathname = kmalloc( strlen(p) + 1 , GFP_ATOMIC);
					strcpy(pi->pathname, p);
					printk("function called by: %s\n", p);

					INIT_LIST_HEAD(&pi->list);
					list_add(&pi->list, &process_info_list);
					calls++;
				}
				kfree(pathname);
			}

			// save map count
			pi->vm_area_number = mm->map_count;

			//init vmarea list
			INIT_LIST_HEAD(&pi->vm_area_list);

			struct vm_area_struct* curr = mm->mmap;
			while (curr != NULL) {	
				if (curr->vm_file != NULL) {
					struct vm_area_info* curr_vm = kmalloc(sizeof(struct vm_area_info), GFP_ATOMIC);;
					INIT_LIST_HEAD(&curr_vm->list);
					
					//save pathname
					char *pathname,*p;
					pathname = kmalloc(PATH_MAX, GFP_ATOMIC);
					p = d_path(&curr->vm_file->f_path, pathname, PATH_MAX);
					curr_vm->path = kmalloc( strlen(p) + 1 , GFP_ATOMIC);
					strcpy(curr_vm->path, p);
					kfree(pathname);

					if (strcmp(p, "/usr/bin/bash") == 0 && curr->vm_flags & VM_EXEC) {
						//save size
						curr_vm->size = curr->vm_end - curr->vm_start;

						//get md5
						curr_vm->md5 = kmalloc(MD5_HEX_SIZE + 1, GFP_ATOMIC);
						char* test = kmalloc(HASH_SIZE, GFP_ATOMIC);
						copy_from_user(test, (unsigned long*) curr->vm_start, HASH_SIZE);
						//printk("test: %lx\n", *((unsigned long*)test));
						do_md5(test, curr_vm->md5, HASH_SIZE);
						curr_vm->md5[MD5_HEX_SIZE] = 0;

						printk("%lx - %lx: %s\n", curr->vm_start, curr->vm_end, curr_vm->md5);
					}
				}			
				curr = curr->vm_next;
			}
		}
/*
55e260675000-55e2606a4000 r--p 00000000 fc:03 1048675                    /usr/bin/bash
55e2606a4000-55e260783000 r-xp 0002f000 fc:03 1048675                    /usr/bin/bash
55e260783000-55e2607bd000 r--p 0010e000 fc:03 1048675                    /usr/bin/bash
55e2607bd000-55e2607c1000 r--p 00147000 fc:03 1048675                    /usr/bin/bash
55e2607c1000-55e2607ca000 rw-p 0014b000 fc:03 1048675                    /usr/bin/bash
*/
	}
	return 0;
}
NOKPROBE_SYMBOL(entry_handler);


static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{

	//pr_info("[KPROBE] A printk call is ending...\n");
	return 0;
}
NOKPROBE_SYMBOL(ret_handler);

static struct kretprobe my_kretprobe = {
	.handler			= ret_handler,
	.entry_handler		= entry_handler,
	/* Probe up to 20 instances concurrently. */
	.maxactive		= 20,
};

int init_probe() {

    int ret;

	calls = 0;
	my_kretprobe.kp.symbol_name = func_name;

	//init list
	INIT_LIST_HEAD(&process_info_list);

    ret = register_kretprobe(&my_kretprobe);
	if (ret < 0) {
		pr_err("register_kretprobe failed, returned %d\n", ret);
		return ret;
	}

	pr_info("kprobe ready\n");

    return 0;
}

void remove_probe() {

	unregister_kretprobe(&my_kretprobe);

}
