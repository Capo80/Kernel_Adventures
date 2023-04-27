#include "smart_api_finder.h"

unsigned long (*_kallsyms_lookup_name)(const char *name) = NULL;

extern unsigned long smart_call(unsigned long addr, unsigned char param_num, va_list argptr);

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

void init_kallsyms(void) {

    register_kprobe(&kp);

    _kallsyms_lookup_name = (unsigned long (*)(const char* name)) (((unsigned long)kp.addr) + 5) ;

    //printk("kallsyms_lookup_name: %lx\n", (unsigned long) _kallsyms_lookup_name);
	unregister_kprobe(&kp);

}

unsigned long smart_api_call(char* func_name, unsigned char param_num, ...) {

    va_list argptr;
    unsigned long func_addr, ret;
    
    if (_kallsyms_lookup_name == NULL)
        init_kallsyms();

    func_addr = _kallsyms_lookup_name(func_name);

    //printk("func address: %lx\n", (unsigned long) func_addr);

    va_start(argptr, param_num);
    ret = smart_call(func_addr, param_num, argptr);
    va_end(argptr);

    return ret;
} 