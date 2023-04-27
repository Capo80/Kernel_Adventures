#include "kernel_patcher.h"

#define ALIGNMENT_OFFSET_INSURANCE 0x14

static inline void
write_cr0_forced(unsigned long val)
{
    unsigned long __force_order;

    /* __asm__ __volatile__( */
    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(__force_order));
}

static inline void
protect_memory(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    write_cr0_forced(cr0);
}

static inline void
unprotect_memory(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    write_cr0_forced(cr0);
}

void write_address(void *f_addr, char *shellcode)
{
	memcpy(shellcode, &f_addr, 8);
}

/*
 * Split the 16 bytes data pointed by val into two unsigned long 
 * (pointed by val1 and val2)
 * The values are copied as they are in memory:
 * low will contain from *val to *(val +7)
 * high will contain from *(val + 8) through *(val + 15)
 */
void split_value(void *val, unsigned long *low, unsigned long *high)
{
	
	memcpy(low, val, 8);
	memcpy(high, val + 8, 8);
	
}

/* Write atomically 16 bytes */
int atomic_write_mem(void *dst, void *src)
{
	/*
	 * https://www.felixcloutier.com/x86/cmpxchg8b:cmpxchg16b
	 * Compare RDX:RAX with m128. If equal, set ZF and load RCX:RBX into m128. Else, clear ZF and load m128 into RDX:RAX.
	*/ 


	// get old value
	unsigned long old_val1, old_val2;
	split_value(dst, &old_val1, &old_val2);
	// split new value
    unsigned long new_val1, new_val2;
	split_value(src, &new_val1, &new_val2);

	// https://github.com/ivmai/libatomic_ops/blob/master/src/atomic_ops/sysdeps/gcc/x86.h
	
	char result;
	asm volatile("lock; cmpxchg16b %0; setz %1"
		: "=m"(*((unsigned __int128 *) dst)),
		"=a"(result)
		: "m" (*((unsigned __int128 *) dst)),
		/* NB : Arguments inside registers must be passed in reverse order due to endianess */ 
		"d" (old_val2),
		"a" (old_val1),
		"c" (new_val2),
		"b" (new_val1)
	  : "memory");
	

	/*

		"m" input : memory input

		https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html

		"memory" clobber
	
    	The "memory" clobber tells the compiler that the assembly 
    	code performs memory reads or writes to items other 
    	than those listed in the input and output operands (for example, 
    	accessing the memory pointed to by one of the input parameters). 
    	To ensure memory contains correct values, GCC may need to flush 
    	specific register values to memory before executing the asm. Further, 
    	the compiler does not assume that any values read from memory 
    	before an asm remain unchanged after that asm; it reloads them 
    	as needed. Using the "memory" clobber effectively forms a 
    	read/write memory barrier for the compiler. 
    */
	return (int) result;

}


int hijack_function(void *origin_f, void *hook_f, void **original_code_ptr, size_t *len_original_code)
{

	int ret;
	void *_original_code;
	size_t len;
    /*
	 <4 bytes nope>
	mov rax, 0x123456789ab
	jmp rax
	*/
	char shellcode[] = "\x0F\x1F\x40\x00\x48\xB8\xAB\x89\x67\x45\x23\x01\x00\x00\xFF\xE0";
	
    /*
	mov rax, 0x123456789ab
	jmp rax
	*/
	char return_trampoline[] = "\x48\xB8\xAB\x89\x67\x45\x23\x01\x00\x00\xFF\xE0";
    
    printk(KERN_INFO "Hook @ 0x%p\n", hook_f);
	

	// Little endian
	// size = 4 NOP + 12 
	#define SHELLCODE_ADDRESS_OFFSET 6
	
	
	
	// Save ALIGNMENT_OFFSET_INSURANCE bytes to be "sure" instructions will be aligned
	// TODO : really bad way
	len = ALIGNMENT_OFFSET_INSURANCE;
	
	
	/*
	 * Need Executable Mem
	 * len : size of saved instructions
	 * sizeof(shellcode) : jmp original_func
	 *
	 */
	_original_code = alloc_executable_mem(len + sizeof(shellcode));
	*original_code_ptr = _original_code;
	
	if (_original_code == NULL)
	{
		printk(KERN_ERR "[hijack_function] Can't  Allocate Executable Memory!\n");
		return -1;
	}
	
	void **tmp;
	
	// save
	memcpy(_original_code, origin_f, len);

	*len_original_code = len;
	// Little endian
	
	// address offset = 2
	// Write JMP to original code backup
	write_address(origin_f + len, &return_trampoline[2]);

	memcpy(_original_code + len, return_trampoline, sizeof(return_trampoline));

	// Hijack
	write_address(hook_f, &shellcode[6]);
	
	unprotect_memory();
	//memcpy(origin_f, shellcode, sizeof(shellcode));
	ret = atomic_write_mem(origin_f, (void *) shellcode);
	protect_memory();
	
	tmp = origin_f + SHELLCODE_ADDRESS_OFFSET;
	printk(KERN_INFO "[hijack_function] Address of Hook function (IN JMP Instruction) : 0x%p\n", *(tmp));
	
	printk(KERN_INFO "[hijack_function] We Will jump to the original function @ 0x%p\n", origin_f + len);
	
	printk(KERN_INFO "[hijack_function] Hijacked! \n\n");
	return 0;	
}


void restore_original_function(void *origin_f, void *original_code, int len_original_code)
{
	// Restore
	
	printk(KERN_INFO "Restoring function...");

	unprotect_memory();
	atomic_write_mem(origin_f, original_code);
	protect_memory();
	
}
