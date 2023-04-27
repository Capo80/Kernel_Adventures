#include <linux/kprobes.h>

#define SEARCH_SIZE 0xfffff

/*
 * Search for a function signature from a start address
 *  direction = 1 -> start_address++
 *  direction = -1 -> start_address--
 * will look up to SEARCH_SIZE distance
 */
unsigned long lookup_signature(const void* signature, size_t signature_len, unsigned long start_address, char direction);