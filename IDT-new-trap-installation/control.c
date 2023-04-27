
/*
* 
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.
* 
* This module is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* @file control.c 
* @brief This is a source for the Linux Kernel Module which sets up
* 	 a not used IDT entry 
* 	 the actual isytalle handler works with PTI-off setting of the kernel 
*
* @author Francesco Quaglia
*
* @date November 22, 2020
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/ptrace.h>
#include <linux/smp.h>

#include <asm/apicdef.h>
#include <asm/apic.h>
#include <asm/nmi.h>
#include <linux/slab.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>
#include <asm/irq.h>		
#include <asm/hw_irq.h>
#include <linux/interrupt.h>	
#include <linux/version.h>


#include "control.h"


unsigned my_trap_vector = 0xffU;//unsigned notation for 255 - let's try to find a free IDT entry starting from this vector value

unsigned long audit_counter __attribute__((aligned(64)));//we can count the number of times this handler has been fired
module_param(audit_counter, ulong, S_IRUSR | S_IRGRP | S_IROTH);

gate_desc old_entry;

//this is the actual handler - it receives the reference to the CPU snaphsot
void my_handler(struct pt_regs *regs){
	pr_info("eflags %lx\n",  native_save_fl());
	//eflags: 46
	//Context: 0
	printk("Handler has been called\n");
}

//the fist level handler emulates a classical one with creation of the CPU snapshot onto the stack
void my_trap_entry(void);
asm("    .globl my_trap_entry\n"
    "my_trap_entry:\n"
    "    testq $3,8(%rsp)\n"
    "    jz    1f\n"//am I already kernel mode?
    "    swapgs\n"
    "1:\n"
    "    pushq $0\n" //fictitious error code 
    "    pushq %rdi\n"
    "    pushq %rsi\n"
    "    pushq %rdx\n"
    "    pushq %rcx\n"
    "    pushq %rax\n"
    "    pushq %r8\n"
    "    pushq %r9\n"
    "    pushq %r10\n"
    "    pushq %r11\n"
    "    pushq %rbx\n"
    "    pushq %rbp\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    mov %rsp,%rdi\n"
    "1:  call my_handler\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbp\n"
    "    popq %rbx\n"
    "    popq %r11\n"
    "    popq %r10\n"
    "    popq %r9\n"
    "    popq %r8\n"
    "    popq %rax\n"
    "    popq %rcx\n"
    "    popq %rdx\n"
    "    popq %rsi\n"
    "    popq %rdi\n"
    "    addq $8,%rsp\n" //error code removal 
    "    testq $3,8(%rsp)\n"
    "    jz 2f\n"
    "    swapgs\n"
    "2:\n"
    "    iretq");


unsigned long cr0;

static inline void write_cr0_forced(unsigned long val) {
    unsigned long __force_order;

    /* __asm__ __volatile__( */
    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(__force_order));
}

static inline void protect_memory(void){
    write_cr0_forced(cr0);
}

static inline void unprotect_memory(void) {
    write_cr0_forced(cr0 & ~X86_CR0_WP);
}


static int setup_idt_entry (void) {
	
	struct desc_ptr idtr;
	gate_desc my_trap_desc;

	// read the idtr register 
	store_idt(&idtr);

	// copy the old entry before overwriting it 
	memcpy(&old_entry, (void*)(idtr.address + my_trap_vector * sizeof(gate_desc)), sizeof(gate_desc));
	
	pack_gate(&my_trap_desc, GATE_INTERRUPT, (unsigned long)my_trap_entry, 0x3, 0, 0);
	
	// the IDT id is read only memory 
	cr0 = read_cr0();
	unprotect_memory();
	// update the IDT entry
	write_idt_entry((gate_desc*)idtr.address, my_trap_vector, &my_trap_desc);
	protect_memory();

	return 0;
}// setup_idt

static int acquire_free_vector(void) {

	struct desc_ptr idtr;
	unsigned search_index;

	store_idt(&idtr);

	while (my_trap_vector) {

		search_index = my_trap_vector - 1;
		for(;search_index > 0x40 && my_trap_vector >0x40; search_index--){
			if (memcmp((void*)idtr.address + my_trap_vector * sizeof(gate_desc),(void*)idtr.address + search_index * sizeof(gate_desc), sizeof(gate_desc)) == 1){
				printk("%s: match at trap vector %d\n",MOD_NAME,my_trap_vector);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,18,0)
				if (!test_bit(my_trap_vector, used_vectors)) {
					printk("%s: test bit confirms trap vector %d\n",MOD_NAME,my_trap_vector);
				}
				else{
					printk("%s: false positive with trap vector %d\n",MOD_NAME,my_trap_vector);
					return -1;
				}
#endif
				goto done;
			}	
		}
		my_trap_vector--;
		if (my_trap_vector <= 0x40) {//below they are reserved
			printk("%s: no free vector found\n",MOD_NAME);
			return -1;
		}
	}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,18,0)
	set_bit(my_trap_vector, used_vectors);
#endif

done:
	printk("%s: got vector #%x - displacement %u\n",MOD_NAME, my_trap_vector,my_trap_vector);
	return 0;
}

int setup_my_irq(void) {

	int err = 0;
	
	audit_counter = 0;

	printk("%s: setting up one IRQ\n",MOD_NAME);

	err = acquire_free_vector();
	if (err) goto out;

	err = setup_idt_entry();
	if (err) goto out;
	
out:
	return err;
}

static void restore_idt(void) {
	struct desc_ptr idtr;

	// read the idtr register 
	store_idt(&idtr);

	// the IDT id is read only memory 
	cr0 = read_cr0();
	unprotect_memory();
	// update the target entry
	write_idt_entry((gate_desc*)idtr.address, my_trap_vector, &old_entry);
	protect_memory();
	
}// restore_idt

void cleanup_my_irq(void) {

	// release the trap entry 
	restore_idt();

#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,18,0)
	clear_bit(my_trap_vector, used_vectors);	
#endif
	
	pr_info("%s: IDT restored\n",MOD_NAME);
}

