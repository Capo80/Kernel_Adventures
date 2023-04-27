#ifndef PAGE_UTILS_H
#define PAGE_UTILS_H

#include <linux/io.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>

//stuff for actual service operations
#define ADDRESS_MASK 0xfffffffffffff000
#define PAGE_TABLE_ADDRESS phys_to_virt(my_read_cr3() & ADDRESS_MASK)
#define PT_ADDRESS_MASK 0x7ffffffffffff000
#define VALID 0x1
#define MODE 0x100
#define LH_MAPPING 0x80
#define IS_USER 0x4

#define PML4(addr) (unsigned int)(((long long)(addr) >> 39) & 0x1ff)
#define PDP(addr)  (unsigned int)(((long long)(addr) >> 30) & 0x1ff)
#define PDE(addr)  (unsigned int)(((long long)(addr) >> 21) & 0x1ff)
#define PTE(addr)  (unsigned int)(((long long)(addr) >> 12) & 0x1ff)


//#define NO_MAP (-1) //this is a baseline with no linkage to classical error code numbers
#define NO_MAP (-EFAULT)
#define MAP    1

uint64_t my_read_cr3(void);
void my_write_cr3(uint64_t val);

int is_mapped_addr(unsigned long vaddr);

pgd_t* alloc_page_table(void);
void free_page_table(pgd_t* to_free);

void update_page_table(void); 
void* map_user_address(void* kernel_addr);

#endif /* PAGE_UTILS_H */
