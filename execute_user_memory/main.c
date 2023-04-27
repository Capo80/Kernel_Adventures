#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include <linux/wait.h>
#include <linux/mman.h>
#include <linux/syscalls.h>

#include "libs/symbol_lookup.h"
#include "libs/memory_helpers.h"
#include "libs/allocator.h"
#include "libs/kernel_patcher.h"
#include "libs/syscall_patcher.h"

struct mm_struct *old_mm;
static unsigned char ksys_mmap_pgoff_signature[] = {
0xf,	0x1f,	0x44,	0x0,	0x0,	0x55,	0x4d,	0x89,
0xca,	0x48,	0x89,	0xe5,	0x41,	0x57,	0x49,	0x89,
0xd7,	0x41,	0x56,	0x49,	0x89,	0xfe,	0x41,	0x55,
0x41,	0x54,	0x49,	0x89,	0xf4,	0x53,	0x48,	0x89,
};

static unsigned char entry_syscall_signature[] = {
0xf,	0x1,	0xf8,	0x65,	0x48,	0x89,	0x24,	0x25,
0x14,	0x60,	0x0,	0x0,	0xeb,	0x12,	0xf,	0x20,
0xdc,	0x48,	0xf,	0xba,	0xec,	0x3f,	0x48,	0x81,
0xe4,	0xff,	0xe7,	0xff,	0xff,	0xf,	0x22,	0xdc,
};
unsigned long entry_syscall;
void* orig_func;
size_t orig_len;


asmlinkage unsigned long (*_ksys_mmap_pgoff)(unsigned long,  unsigned long,  
                    unsigned long,  unsigned long,  unsigned long,  
                    unsigned long); 

void jmp_code(unsigned long code)
{
    asm volatile(
        "call %%r15\n"
          : //not output
          : "a" (code) 
		: // No Clobbers
		);
	
    //should never reach here - we are calling exit
    pr_err("failed exit");
	return;
}


void jmp_code_stack(unsigned long code, unsigned long stack)
{
    asm volatile(
        "movq %0, %%rsp\n"
        "call %%rax\n"
          : //not output
          : // Input Data
        "r" (stack), "a" (code) 
		: // No Clobbers
		);
	
    //should never reach here - we are calling exit
    pr_err("failed exit");
	return;
}

int to_user_kthread(void* data) {

    void *shellcode_mem, *stack, *shellcode_mem_usr, *stack_usr;
    int i;
    unsigned long populate;
    unsigned long text_addr, stack_addr, rodata_addr;
    #include "small_blob_header.h"

    pr_info("Creating a private memory view...\n");
    old_mm = change_mm();
    if (IS_ERR(old_mm)) {
        printk("failed to allocate new mm\n");
        return 0;
    }
    reserve_current_address_space();
    update_page_table();

    //patch syscall out of code
    for (i = 0; i < ARRAY_SIZE(syscalls_addr); i++) {
        patch_syscall(text, syscalls_addr[i]);
    }
    
    // printk("seraching ksys_mmap_pgoff\n");

    // _ksys_mmap_pgoff = (void*) lookup_signature((void*) ksys_mmap_pgoff_signature, sizeof(ksys_mmap_pgoff_signature));
    
    // printk("Found ksys_mmap_pgoff: %lx\n", (unsigned long)_ksys_mmap_pgoff);
    // //allocate user memory areas
    // mmap_write_lock(current->active_mm);
    
    // printk("Alloc adresses\n");

    // text_addr = _ksys_mmap_pgoff(0x40100, sizeof(text), PROT_WRITE | PROT_READ | PROT_EXEC, MAP_ANONYMOUS, 0, 0);
    // stack_addr = _ksys_mmap_pgoff(0, 0x20000, PROT_WRITE | PROT_READ , MAP_ANONYMOUS, 0, 0);
    
    // printk("addresses: %lx, %lx\n", text_addr, stack_addr);

    // mmap_write_unlock(current->active_mm);
    
    // printk("addresses: %lx, %lx\n", text_addr, stack_addr);

    // copy_to_user( (void*) text_addr, text, sizeof(text));
    
    // do_exit(1);
    // jmp_code(text_addr, stack_addr);


    pr_info("Allocating executable memory...\n");
    text_addr = alloc_executable_mem(sizeof(text));
    memcpy(text_addr, text, sizeof(text));

    pr_info("Allocating user stack...\n");
    stack = kmalloc(200, GFP_KERNEL);

    pr_info("Mapping to user...\n");
    shellcode_mem_usr = map_user_address(shellcode_mem);
    stack_usr = map_user_address(stack);

    pr_info("Returning to user...\n");
    jmp_code_stack(text_addr, stack_usr);


    //should never reach here
    pr_err("Exectution failed\n");

    do_exit(42);
}

/*
0:  66 41 8c cf             mov    r15w,cs
4:  66 41 83 e7 03          and    r15w,0x3
9:  66 41 83 ff 00          cmp    r15w,0x0
e:  0f 84 00 00 00 00       je     14 <_main+0x14> ; wrong, will not owrk
14: 0f 01 f8                swapgs
17: 65 48 89 24 25 14 60    mov    QWORD PTR gs:0x6014,rsp
1e: 00 00
20: 49 c7 c7 20 00 e0 81    mov    r15,0xffffffff81e00020
27: 41 ff e7                jmp    r15

000000000000015a <init_module>:
 15a:	e8 00 00 00 00       	call   15f <init_module+0x5>
 15f:	55                   	push   %rbp
 160:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
 167:	48 89 e5             	mov    %rsp,%rbp
 16a:	e8 00 00 00 00       	call   16f <init_module+0x15>
 16f:	31 c0                	xor    %eax,%eax
 171:	5d                   	pop    %rbp
 172:	c3                   	ret    

*/
char syscall_hook[] = "\x66\x41\x8C\xCF\x66\x41\x83\xE7\x03\x0F\x01\xF8\x65\x48\x89\x24\x25\x14\x60\x00\x00\x49\xC7\xC7\x20\x00\xE0\x81\x41\xFF\xE7";

unsigned long funcs[1] = {(unsigned long) kfree};

int init_module(void)
{

    //init_allocator();

    //void* hook = alloc_executable_mem(sizeof(syscall_hook));
    //memcpy(hook, syscall_hook, sizeof(syscall_hook));

    //here return dynamically, too lazy for me now

    //entry_syscall = lookup_signature(entry_syscall_signature, sizeof(entry_syscall_signature));

    //hijack_function((void*) entry_syscall, hook, &orig_func, &orig_len);
    
    printk("Hijack complete %lx\n", funcs[0]);
    //kthread_run(to_user_kthread, NULL, "user_kthread");
	return 0;
}

void cleanup_module(void) 

{ 

    pr_info("Goodbye world 1.\n"); 

} 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pasquale Caporaso");
