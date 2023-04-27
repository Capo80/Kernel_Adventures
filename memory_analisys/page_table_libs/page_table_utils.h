#ifndef PAGE_UTILS_H
#define PAGE_UTILS_H

#include <linux/io.h>
#include <linux/mm.h>
#include <linux/sched.h>
//#include <linux/sched/mm.h>

//stuff for actual service operations
#define ADDRESS_MASK 0xfffffffffffff000
#define PAGE_TABLE_ADDRESS phys_to_virt(my_read_cr3() & ADDRESS_MASK)
#define PT_ADDRESS_MASK 0x7ffffffffffff000
#define VALID 0x1
#define P_WRITE 0x2
#define MODE 0x100
#define LH_MAPPING 0x80
#define IS_USER 0x4

#define EXE_DENY_BIT    63
#define ORIG_EXE_BIT    11
#define ORIG_WRITE_BIT  10
#define WRITE_BIT       1
#define PRESENT_BIT     0
#define DIRTY_BIT       6
#define ACCESS_BIT       5

#define PML4(addr) (unsigned int)(((long long)(addr) >> 39) & 0x1ff)
#define PDP(addr)  (unsigned int)(((long long)(addr) >> 30) & 0x1ff)
#define PDE(addr)  (unsigned int)(((long long)(addr) >> 21) & 0x1ff)
#define PTE(addr)  (unsigned int)(((long long)(addr) >> 12) & 0x1ff)
#define IS_EXE(addr)    (unsigned int)(((long long)(addr) >> 63) & 0x1)


//#define NO_MAP (-1) //this is a baseline with no linkage to classical error code numbers
#define NO_MAP (-EFAULT)
#define MAP    1

uint64_t my_read_cr3(void);
void my_write_cr3(uint64_t val);
void manual_tlb_flush(void);

int is_mapped_addr(unsigned long vaddr);

pgd_t* alloc_page_table(void);
void free_page_table(pgd_t* to_free);

void update_page_table(void); 
void* map_user_address(void* kernel_addr);
int print_flags(unsigned long vaddr);
int flip_bit(unsigned long vaddr, unsigned char bit_position, unsigned char set);
int is_bit_set(unsigned long vaddr, unsigned char bit_position);

#endif /* PAGE_UTILS_H */
