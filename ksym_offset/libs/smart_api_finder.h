#ifndef __H_API_FINDER
#define __H_API_FINDER
#include <stdarg.h>
#include <linux/kprobes.h>

unsigned long smart_api_call(char* func_name, unsigned char param_num, ...);

#endif