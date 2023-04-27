#include "hash_helper.h"

#define PATH_MAX        4096
#define CONTROL_SIZE    4060*100         //in bytes
#define HASH_SIZE       16
char *hash_alg_name = "md5";

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

char* bytes_to_hex(char* bytes, size_t size) {

    int i;
    char* hex = kmalloc(size*2 + 1, GFP_ATOMIC);

    char c[3];
    for (i = 0; i < size; i++)
    {
        sprintf( c, "%02x", bytes[i] & 0xFFu);
        memcpy(hex+i*2, c, 2);
    }

    hex[size*2] = 0;
    return hex;

}

char* get_process_hash(struct task_struct* process_struct) {

    char *pathname,*p;
    int ret;
    struct sdesc *sdesc;
    struct crypto_shash *alg;
    struct mm_struct* mm = process_struct->mm;

    if (!mm) 
        return NULL;
    
    if (!mm->exe_file)
        return NULL;

    //alloc hash 
    alg = crypto_alloc_shash(hash_alg_name, 0, 0);
    if(IS_ERR(alg)){
        pr_info("can't alloc alg %s\n", hash_alg_name);
        return NULL;
    }

    //init struct
    sdesc = init_sdesc(alg);
    if (IS_ERR(sdesc)) {
        pr_info("can't alloc sdesc\n");
        return NULL;
    }

    //init hash
    ret = crypto_shash_init(&sdesc->shash);
    if (ret) {
        pr_info("can't init hash\n");
        return NULL;
    }
    
    //save pathname
    pathname = kmalloc(PATH_MAX, GFP_ATOMIC);
    if (pathname) {
        p = d_path(&mm->exe_file->f_path, pathname, PATH_MAX);
    }
 
    struct vm_area_struct* curr = mm->mmap;
    char* buffer = kmalloc(CONTROL_SIZE, GFP_ATOMIC);
    size_t area_size;
    int copied;
    while (curr != NULL) {	
        if (curr->vm_file != NULL) {
            
            //add to hash if area is exec
            if (curr->vm_flags & VM_EXEC) {
                
                area_size = (CONTROL_SIZE >  curr->vm_end - curr->vm_start) ?  curr->vm_end - curr->vm_start : CONTROL_SIZE;
                
                
                //get from user
                copied = copy_from_user(buffer, (void*) curr->vm_start, area_size);

                //debug
                //printk("%s - %s - %ld - %d\n", p, curr->vm_file->f_path.dentry->d_name.name, area_size, copied);
                
                //debug
                //printk("%lx\n", *((unsigned long*)buffer));

                //add to hash
                crypto_shash_update(&sdesc->shash, buffer, area_size);


            }

        }			
        curr = curr->vm_next;
    }
    kfree(buffer);

    char* md5_bytes = kmalloc(HASH_SIZE, GFP_ATOMIC);
    //get final hash
    crypto_shash_final(&sdesc->shash, md5_bytes);

    //convert to hex string 
    char* md5_hex = bytes_to_hex(md5_bytes, HASH_SIZE);

    //prepare return buffer
    char* out = kmalloc(PATH_MAX + HASH_SIZE*2, GFP_ATOMIC);
    sprintf(out, "%s, %s", p, md5_hex);


    kfree(sdesc);
    crypto_free_shash(alg);
    kfree(md5_bytes);
    kfree(md5_hex);
    kfree(pathname);

    return out;
}

void save_hash_snapshot(char* filename) {

    struct file* info_file;
    size_t to_write;
    loff_t pos = 0;
    struct task_struct* curr;
    time64_t timestamp;

	info_file = filp_open(filename, O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG| S_IRWXO);
    timestamp = ktime_get_real_seconds();


    char* buffer = kmalloc(PATH_MAX + HASH_SIZE*2 + 100, GFP_ATOMIC);
    for_each_process(curr) {
        char* info = get_process_hash(curr);

        if (info) {
            to_write = snprintf(buffer, PATH_MAX + HASH_SIZE*2 + 100, "%s, %lld\n", info, timestamp);
            kernel_write(info_file, buffer, to_write, &pos);
            //printk("%s\n", info);
            kfree(info);
        }

    }
    kfree(buffer);
}