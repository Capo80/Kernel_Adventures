#include "syscall_patcher.h"

#define ADDRESS_MASK 0xfffffffffffff000//to migrate

#define START 			0xffffffff00000000ULL		// use this as starting address --> this is a biased search since does not start from 0xffff000000000000
#define MAX_ADDR		0xfffffffffff00000ULL
#define FIRST_NI_SYSCALL	134
#define SECOND_NI_SYSCALL	174
#define THIRD_NI_SYSCALL	182 
#define FOURTH_NI_SYSCALL	183
#define FIFTH_NI_SYSCALL	214	
#define SIXTH_NI_SYSCALL	215	
#define SEVENTH_NI_SYSCALL	236	

#define ENTRIES_TO_EXPLORE 256

// NOT THREAD SAFE

unsigned long* syscall_table = NULL;


int good_area(unsigned long * addr){

	int i;
	
	for(i=1;i<FIRST_NI_SYSCALL;i++){
		if(addr[i] == addr[FIRST_NI_SYSCALL]) goto bad_area;
	}	

	return 1;

bad_area:

	return 0;

}

int validate_page(unsigned long *addr) {
	int i = 0;
	unsigned long page 	= (unsigned long) addr;
	unsigned long new_page 	= (unsigned long) addr;
	for(; i < PAGE_SIZE; i+=sizeof(void*)){		
		new_page = page+i+SEVENTH_NI_SYSCALL*sizeof(void*);
			
		// If the table occupies 2 pages check if the second one is materialized in a frame
		if( 
			( (page+PAGE_SIZE) == (new_page & ADDRESS_MASK) )
			&& is_mapped_addr(new_page) == NO_MAP
		) 
			break;
		// go for patter matching
		addr = (unsigned long*) (page+i);
		if(
			   ( (addr[FIRST_NI_SYSCALL] & 0x3  ) == 0 )		
			   && (addr[FIRST_NI_SYSCALL] != 0x0 )			// not points to 0x0	
			   && (addr[FIRST_NI_SYSCALL] > 0xffffffff00000000 )	// not points to a locatio lower than 0xffffffff00000000	
	//&& ( (addr[FIRST_NI_SYSCALL] & START) == START ) 	
			&&   ( addr[FIRST_NI_SYSCALL] == addr[SECOND_NI_SYSCALL] )
			&&   ( addr[FIRST_NI_SYSCALL] == addr[THIRD_NI_SYSCALL]	 )	
			&&   ( addr[FIRST_NI_SYSCALL] == addr[FOURTH_NI_SYSCALL] )
			&&   ( addr[FIRST_NI_SYSCALL] == addr[FIFTH_NI_SYSCALL] )	
			&&   ( addr[FIRST_NI_SYSCALL] == addr[SIXTH_NI_SYSCALL] )
			&&   ( addr[FIRST_NI_SYSCALL] == addr[SEVENTH_NI_SYSCALL] )	
			&&   (good_area(addr))

		){
            syscall_table = addr;
			return 1;
		}
	}
	return 0;

}	

void init_syscall_patcher(void) {

    unsigned long k; // current page
	unsigned long candidate; // current page

	for(k=START; k < MAX_ADDR; k+=4096){	
		candidate = k;
		if(
			(is_mapped_addr(candidate) != NO_MAP) 	
		){
			// check if candidate maintains the syscall_table
			if(validate_page((unsigned long *)(candidate)) ){
				printk("Syscall table found at %lx\n",candidate);
				break;
			}
		}
	}

}


void *get_syscall_table(void) {

    if (syscall_table == NULL)
        init_syscall_patcher();
    
    return syscall_table;

}

unsigned long get_syscall_function(unsigned short num) {

    if (syscall_table == NULL)
        init_syscall_patcher();

    return syscall_table[num];
}

/*
0:  48 c7 c7 42 00 00 00    mov    rdi,0x42    ; error_code
7:  48 c7 c0 3c 00 00 00    mov    rax,0x3c    ; exit
e:  0f 05                   syscall
*/
// trasform to
/*
0:  48 c7 c7 42 00 00 00    mov    rdi,0x42
7:  48 c7 c0 c0 ab 0a 81    mov    rax,0xffffffff810aabc0
e:  ff d0                   call   rax 
*/
void patch_syscall(char* code, unsigned int mov_offset) {


    unsigned short sys_num;
    unsigned long sys_func;
    int i;
    sys_num = *((unsigned short*) (code+mov_offset+3));


    sys_func = get_syscall_function(sys_num);
    
    printk("patching syscall number: %x - %u - %lx\n", sys_num, mov_offset, sys_func);

    memcpy(code+mov_offset+3, &sys_func, 4);  
    memcpy(code+mov_offset+7, "\xff\xd0", 2); // syscall -> call
    //for (i = 0; i < 9; i++)
    //    printk("new code %x\n", code[mov_offset+i]);
}