#include "page_table_utils.h"

inline uint64_t my_read_cr3(void) {
    uint64_t ret = 0;

    asm volatile (
        "movq %%cr3, %0\n"
        :"=r"(ret)
    );

    return ret;
}

inline void my_write_cr3(uint64_t val) {
    asm volatile (
        "movq %0, %%cr3\n"
        :
        :"r"(val)
    );
}

int is_mapped_addr(unsigned long vaddr) {

    void* target_address;
        
	pud_t *pdp;
	pmd_t *pde;
	pte_t *pte;
	pgd_t *pml4;


	target_address = (void*)vaddr; 
        /* fixing the address to use for page table access */       

	pml4  = PAGE_TABLE_ADDRESS;

	if (!(((ulong)(pml4[PML4(target_address)].pgd)) & VALID))
		return NO_MAP;

	pdp = __va((ulong)(pml4[PML4(target_address)].pgd) & PT_ADDRESS_MASK);           

	if (!((ulong)(pdp[PDP(target_address)].pud) & VALID))
		return NO_MAP;

	pde = __va((ulong)(pdp[PDP(target_address)].pud) & PT_ADDRESS_MASK);

	if (!((ulong)(pde[PDE(target_address)].pmd) & VALID))
        return NO_MAP;
	
	if ((ulong)pde[PDE(target_address)].pmd & LH_MAPPING) 
        return MAP;

    pte = __va((ulong)(pde[PDE(target_address)].pmd) & PT_ADDRESS_MASK);           

 	if (!((ulong)(pte[PTE(target_address)].pte) & VALID))
		return NO_MAP;

	return MAP;
}

pgd_t* alloc_page_table(void) {

	pgd_t* new_page_table;

	new_page_table = (pgd_t*) __get_free_pages((GFP_KERNEL | __GFP_ZERO), 0);
	if (new_page_table == NULL)
		return NULL;

	//just copy the whole thing - don't care
	memcpy(new_page_table, current->active_mm->pgd, sizeof(pgd_t)*PTRS_PER_PGD);
	
	return new_page_table;
}

void free_page_table(pgd_t* to_free) {
	free_pages((unsigned long) to_free, 0);
}

void update_page_table(void) {
    my_write_cr3(virt_to_phys(current->active_mm->pgd));
}

unsigned int get_free_pdg_offset(pgd_t* pgd, unsigned long kernel_addr) {

    int i;
    unsigned long pgd_value;
    unsigned long pud_phys = ((ulong)(pgd[PML4(kernel_addr)].pgd) & PT_ADDRESS_MASK);           
;

    for (i = 0; i < 0xff; i++) {
        pgd_value = (unsigned long)(pgd[i].pgd);
        if (pgd_value & VALID)
            if ((pgd_value & PT_ADDRESS_MASK) == pud_phys)
                return i;
    }

    for (i = 0; i < 0xff; i++) {
        if (!(((unsigned long)(pgd[i].pgd)) & VALID))
            break;
    }


    return i;
}


void* map_user_address(void* kernel_addr) {
    
    pgd_t* page_table;
    unsigned long user_addr, pgd_offset;
    unsigned long cast_kernel_addr = (unsigned long) kernel_addr;
    
    page_table = (pgd_t*) phys_to_virt(my_read_cr3() & ADDRESS_MASK);

    pgd_offset = get_free_pdg_offset(page_table, (unsigned long) kernel_addr);

    page_table[pgd_offset] = page_table[PML4(cast_kernel_addr)];
    user_addr = (cast_kernel_addr & 0x7fffffffff) | (pgd_offset << 39);

    return (void*) user_addr;

}