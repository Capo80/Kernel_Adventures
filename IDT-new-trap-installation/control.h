#ifndef _CONTROL_H
#define _CONTROL_H

#define MOD_NAME "IDT ENTRY INSTALLER"

extern unsigned my_trap_vector;

int setup_my_irq(void);

void cleanup_my_irq(void);

#endif
