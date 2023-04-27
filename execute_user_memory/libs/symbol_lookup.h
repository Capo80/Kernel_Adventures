#ifndef SYMBOL_LOOKUP_H
#define SYMBOL_LOOKUP_H

#include <linux/kprobes.h>
#include "page_table_libs/page_table_utils.h"

unsigned long lookup_signature(const void* signature, size_t signature_len);

#endif