#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <asm/desc.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");

#define HML_TO_ADDR(h,m,l)      ((unsigned long) (l) | ((unsigned long) (m) << 16) | ((unsigned long) (h) << 32))
#define MOD_NAME "Change Level"
#define IDT_INFO

gate_desc old_entry;
gate_desc my_trap_desc;

unsigned long cr0;


static inline void
write_cr0_forced(unsigned long val)
{
    unsigned long __force_order;

    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(__force_order));
}

static inline void
protect_memory(void)
{
    write_cr0_forced(cr0);
}

static inline void
unprotect_memory(void)
{
    write_cr0_forced(cr0 & ~X86_CR0_WP);
}


static unsigned long get_full_offset_by_vector(gate_desc *idt, int vector_number) {
    gate_desc *gate_ptr;

    gate_ptr = (gate_desc *)((unsigned long)idt + vector_number * sizeof(gate_desc));

    return (unsigned long)HML_TO_ADDR(gate_ptr->offset_high,
                                      gate_ptr->offset_middle,
                                      gate_ptr->offset_low);
}

static unsigned long get_full_offset_spurious_interrput(gate_desc *idt) {    
    unsigned long address;

    address = get_full_offset_by_vector(idt, SPURIOUS_APIC_VECTOR);
    return address;
}

int idt_entry_discovery_init(void) {

    struct desc_ptr dtr;                                                    // Pointer to the IDT info
    int num_idt_entries;                                                    // Number of IDT entries
    
    unsigned long addr_spurious_first_handler;
    
    gate_desc *idt = NULL;                                                  // Pointer to the IDT table

    /* Get IDT info */
    store_idt(&dtr);
    idt = (gate_desc *)dtr.address;
    num_idt_entries = (dtr.size / sizeof(gate_desc)) + 1;

#ifdef IDT_INFO
    pr_info("%s: IDT is located at address %lx\nThe number of IDT entries is equal to %d\nSPURIOUS_APIC_VECTOR = %d\n",MOD_NAME,
                                                                                                                       (unsigned long)idt,
                                                                                                                       num_idt_entries,
                                                                                                                       SPURIOUS_APIC_VECTOR);
#endif

    addr_spurious_first_handler = get_full_offset_spurious_interrput(idt);

#ifdef IDT_INFO
    printk("%s: spurious first handler address at entry %d --> %lx\n", MOD_NAME, SPURIOUS_APIC_VECTOR, addr_spurious_first_handler);
#endif

	/* copy the old content of the target vector entry */
    memcpy(&old_entry, (void*)(dtr.address + SPURIOUS_APIC_VECTOR * sizeof(gate_desc)), sizeof(gate_desc));   

	/* pack a new gate for access from user land */
    pack_gate(&my_trap_desc, GATE_TRAP, (unsigned long)addr_spurious_first_handler, 0x3, 0, 0);
 
    cr0 = read_cr0();

    unprotect_memory();
    write_idt_entry((gate_desc*)dtr.address, 255, &my_trap_desc);
    protect_memory();

    return 0;

}

void idt_entry_discovery_exit(void) {
	struct desc_ptr idtr;

    store_idt(&idtr);

    cr0 = read_cr0();

	unprotect_memory();
	write_idt_entry((gate_desc*)idtr.address, 255, &old_entry);
	protect_memory();

    printk("%s: exit...\n", MOD_NAME);
}

module_init(idt_entry_discovery_init);
module_exit(idt_entry_discovery_exit);

