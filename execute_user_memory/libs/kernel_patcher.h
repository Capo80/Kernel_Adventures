#ifndef H_KPATCHER
#define H_KPATCHER

#include <linux/kernel.h>
#include <linux/string.h>
#include "allocator.h"

int hijack_function(void *origin_f, void *hook_f, void **original_code_ptr, size_t *len_original_code);
void restore_original_function(void *origin_f, void *original_code, int len_original_code);

#endif