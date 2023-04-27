#include "symbol_lookup.h"

/*
 * Search for a function signature from a start address
 *  direction = 1 -> start_address++
 *  direction = -1 -> start_address--
 * will look up to SEARCH_SIZE distance
 */
unsigned long lookup_signature(const void* signature, size_t signature_len, unsigned long start_address, char direction) {
	
	int i;
	for (i = 0; i < SEARCH_SIZE; i += 1*direction) {
		
		if (memcmp(signature, (void*) (start_address + i), signature_len) == 0)
			return start_address + i;
	}

	return 0;
}