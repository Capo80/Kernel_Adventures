#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <asm/desc.h>
#include <linux/printk.h>
#include<linux/slab.h>
#include<linux/kprobes.h>

#include "idt_entry_discovery.h"

MODULE_LICENSE("GPL");

#define HML_TO_ADDR(h,m,l)      ((unsigned long) (l) | ((unsigned long) (m) << 16) | ((unsigned long) (h) << 32))

unsigned long audit_counter __attribute__((aligned(64)));
unsigned long cr0;

static unsigned int old_call_operand      = 0x0;
static unsigned int new_call_operand      = 0x0;
static unsigned int *call_operand_address = NULL;

gate_desc old_entry;
gate_desc my_trap_desc;


void my_handler(struct pt_regs *regs) {
	get_cpu();
	atomic_inc((atomic_t*)&audit_counter);
	printk("%s: I'm here\n",MOD_NAME);
	put_cpu();
}


static inline void write_cr0_forced(unsigned long val) {
    unsigned long __force_order;

    asm volatile("mov %0, %%cr0" : "+r"(val), "+m"(__force_order));
}


static inline void protect_memory(void) {
    write_cr0_forced(cr0);
}

static inline void unprotect_memory(void) {
    write_cr0_forced(cr0 & ~X86_CR0_WP);
}


/**
 *
 * @idt:
 * @vector_number:
 *
 */
static unsigned long get_full_offset_by_vector(gate_desc *idt, int vector_number) {
    gate_desc *gate_ptr;
    gate_ptr = (gate_desc *)((unsigned long)idt + vector_number * sizeof(gate_desc));
    return (unsigned long)HML_TO_ADDR(gate_ptr->offset_high,
                                      gate_ptr->offset_middle,
                                      gate_ptr->offset_low);
}


/**
 *
 * @idt:
 *
 */
static unsigned long get_full_offset_spurious_interrput(gate_desc *idt) {    
    unsigned long address;
    address = get_full_offset_by_vector(idt, SPURIOUS_APIC_VECTOR);
    return address;
}


/**
 *
 * @offset_spurious: indirizzo lineare del gestore di primo livello
 *
 * Modificare l'offset della seconda istruzione di CALL. La prima
 * istruzione di CALL consente di raggiungere la procedura error_
 * entry mentre la seconda consente di raggiungere il gestore C
 * dell'evento.
 *
 */
void patch_IDT(unsigned long offset_spurious) {
    unsigned char *byte;
    unsigned int operand;
    unsigned long address;
    int is_first;
    int i;
    struct desc_ptr idtr;

    /* Non devo considerare la prima istruzione di CALL */
    is_first = 1;

    byte = (unsigned char *)offset_spurious;

    store_idt(&idtr);

    for(i=0; i<512; i++) {
        if(byte[i]==0xE8) {
            if(!is_first) {
                
                operand = ( (unsigned int) byte[i+1]       ) |
                          (((unsigned int) byte[i+2]) << 8 ) |
                          (((unsigned int) byte[i+3]) << 16) |
                          (((unsigned int) byte[i+4]) << 24);

                address = (unsigned long) (((long) &byte[i+5]) + (long) operand);

                //TODO: Inserire un meccanismo di controllo per verificare se è la CALL corretta

                old_call_operand = operand;
                new_call_operand = (unsigned int) ((long) my_handler - ((long) &byte[i+5]));
                call_operand_address = (unsigned int *) &byte[i+1];
                cr0 = read_cr0();
                unprotect_memory();
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
                arch_cmpxchg(call_operand_address, old_call_operand, new_call_operand);
#else
                cmpxchg(call_operand_address, old_call_operand, new_call_operand);
#endif
                write_idt_entry((gate_desc*)idtr.address, 255, &my_trap_desc);
                protect_memory();
                break;
            }
        
            if(is_first)    is_first = 0;                   
        }
    }
}


/**
 *
 * @offset_spurious: indirizzo lineare del gestore di primo livello
 * @sysvec_spurious_apic_interrupt_addr: indirizzo lineare del gestore C
 *
 */
static discovery_info * call_counting(unsigned long offset_spurious, unsigned long sysvec_spurious_apic_interrupt_addr) {
    int count;
    unsigned char *byte;
    int i;
    int operand;
    unsigned long address;
    unsigned long first_address;
    int is_first;
    discovery_info *ret;

    count = 0;
    is_first = 1;

    byte = (unsigned char *)offset_spurious;
    for(i=0; i<512; i++) {
        if(byte[i]==0xE8) {

            operand = (int) (((unsigned int) byte[i+1])         |
                      (((unsigned int) byte[i+2]) << 8)  |
                      (((unsigned int) byte[i+3]) << 16) |
                      (((unsigned int) byte[i+4]) << 24));

            address = (unsigned long) (((unsigned long) &byte[i+5]) + (long) operand);
    
            if(is_first) {
                first_address = address;
                is_first = 0;
            }

            if(address == sysvec_spurious_apic_interrupt_addr)  printk("%s: BINGO!\n", MOD_NAME);

           printk("%s: base: %lx address: %lx\toperand: %x\n", MOD_NAME, &byte[i+5], address, operand);
           count++;           
        }
    }

    ret = (discovery_info *)kmalloc(sizeof(discovery_info), GFP_KERNEL);

    if(ret == NULL)     printk("%s: Errore kmalloc\n", MOD_NAME);

    ret->num_calls = count;

    ret->first_address = first_address;

    return ret;
}


/**
 *
 */
static kallsyms_lookup_name_t get_kallsyms_lookup_name(void) {
    kallsyms_lookup_name_t kallsyms_lookup_name;

    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);

    return kallsyms_lookup_name;
}


int idt_entry_discovery_init(void) {

    struct desc_ptr dtr;                                                    // Pointer to the IDT info
    int num_idt_entries;                                                    // Number of IDT entries

    kallsyms_lookup_name_t kallsyms_lookup_name;                            // kallsyms_lookup_name() address
    discovery_info *spurious_info;
    
    unsigned long asm_sysvec_spurious_apic_interrupt_addr;                  // C Handler Function
    unsigned long addr_spurious_first_handler;
    unsigned long addr_target;
    //unsigned long offset;

    gate_desc *idt = NULL;                                                  // Pointer to the IDT table
    
    audit_counter = 0;

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
    pr_info("%s: spurious first handler address at entry %d --> %lx\n", MOD_NAME, SPURIOUS_APIC_VECTOR, addr_spurious_first_handler);
#endif

	/* copy the old content of the target vector entry */
    memcpy(&old_entry, (void*)(dtr.address + SPURIOUS_APIC_VECTOR * sizeof(gate_desc)), sizeof(gate_desc));    

	/* pack a new gate for access from user land */
    pack_gate(&my_trap_desc, GATE_TRAP, (unsigned long)addr_spurious_first_handler, 0x3, 0, 0);

    //TODO: vedere da quale versione del kernel la funzione kallsyms_lookup_name() non è più esportata

    /* Get kallsyms_lookup_name()'s address */
    kallsyms_lookup_name = get_kallsyms_lookup_name();

#ifdef IDT_INFO
    pr_info("%s: the function kallsyms_lookup_name() is at address %lx\n", MOD_NAME, (unsigned long)kallsyms_lookup_name);
#endif

    /* Get address of C Handler Function */
    asm_sysvec_spurious_apic_interrupt_addr = kallsyms_lookup_name("asm_sysvec_spurious_apic_interrupt");

#ifdef IDT_INFO
    pr_info("%s: asm_sysvec_spurious_apic_interrupt() is at address %lx\n", MOD_NAME, asm_sysvec_spurious_apic_interrupt_addr);
#endif

    if(asm_sysvec_spurious_apic_interrupt_addr == addr_spurious_first_handler)  printk("%s: Spurious C handler is asm_sysvec_spurious_apic_interrupt()\n", MOD_NAME);
    else
    {
        pr_err("%s: Spurious First handler is unknown\n", MOD_NAME);
        return 1;
    }

    addr_target = kallsyms_lookup_name("sysvec_spurious_apic_interrupt");

#ifdef IDT_INFO
    pr_info("%s: sysvec_spurious_apic_interrupt --> %lx\n",MOD_NAME, addr_target);
#endif

    spurious_info = call_counting(addr_spurious_first_handler, addr_target);

#ifdef IDT_INFO
    pr_info("%s: Number of CALL instructions the spurious handler --> %d\n", MOD_NAME, spurious_info->num_calls);
    pr_info("%s: First CALL address in the spurious handler --> %lx\n", MOD_NAME, spurious_info->first_address);
#endif  

    patch_IDT(addr_spurious_first_handler);

    addr_target = kallsyms_lookup_name("error_entry");

#ifdef IDT_INFO
    pr_info("%s: error_entry --> %lx\n",MOD_NAME, addr_target);
#endif 

    return 0;

}

void idt_entry_discovery_exit(void) {
	struct desc_ptr idtr;

    store_idt(&idtr);

    cr0 = read_cr0();
	unprotect_memory();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
    arch_cmpxchg(call_operand_address, new_call_operand, old_call_operand);
#else
    cmpxchg(call_operand_address, new_call_operand, old_call_operand);
#endif

	write_idt_entry((gate_desc*)idtr.address, 255, &old_entry);
	protect_memory();
    printk("%s: exit...\n", MOD_NAME);
}

module_init(idt_entry_discovery_init);
module_exit(idt_entry_discovery_exit);

