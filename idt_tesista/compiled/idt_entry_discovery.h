#ifndef _CONTROL_H
#define _CONTROL_H

#define MOD_NAME "IDT ENTRY DISCOVERY"

#define IDT_INFO

int idt_entry_discovery_init(void);
void idt_entry_discovery_exit(void);

struct idt_discovery_entry_info {
    unsigned long first_address;
    int num_calls;
};

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

typedef struct idt_discovery_entry_info discovery_info;

typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);

#endif 
