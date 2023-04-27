#include "symbol_lookup.h"

//kernel start addresses
#define START_ADDR		0xffffffff00000000ULL		// use this as starting address --> this is a biased search since does not start from 0xffff000000000000
#define MAX_ADDR		0xfffffffffff00000ULL

/*
 * Search for a function signature in page
 */
unsigned long lookup_signature_in_page(const void* signature, size_t signature_len, unsigned long page_addr) {
	
	int i;
	unsigned long new_page;

	for(i = 0; i < PAGE_SIZE; i+=sizeof(void*)) {

		// If the signature occupies 2 pages check if the second one is materialized in a frame
		new_page = page_addr+i+signature_len;
		if( ( (page_addr+PAGE_SIZE) == ( new_page & ADDRESS_MASK ) ) && is_mapped_addr(new_page) == NO_MAP) 
			break;

		if (memcmp( (void*) (page_addr+i), signature, signature_len) == 0)
			return page_addr+i;
	}

	return 0;
}

/*
 * Search for a function signature
 * Will look from START_ADDR to MAX_ADDR
 */
unsigned long lookup_signature(const void* signature, size_t signature_len) {

	unsigned long candidate_page; // current page
	unsigned long signature_addr;

	for(candidate_page=START_ADDR; candidate_page < MAX_ADDR; candidate_page+=4096){	
		//check if candidate_page is mapped
		if( is_mapped_addr(candidate_page) != NO_MAP ){
			// check if candidate_page contains the signature
			if((signature_addr = lookup_signature_in_page(signature, signature_len, candidate_page)))
				return signature_addr;
		}
	}

	return 0;
	
}
