#ifndef SYSCALL_PATCHER_H
#define SYSCALL_PATHCER_H

#include <stddef.h> //??????????
#include <asm/page.h>

#include "page_table_libs/page_table_utils.h"

void* get_syscall_table(void);
unsigned long get_syscall_function(unsigned short num);
void patch_syscall(char* code, unsigned int offset);

#endif