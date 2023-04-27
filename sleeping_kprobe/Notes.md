# Sleping kprobes

TODO: 
- add standard kprobe
- add a way to modify other registers?

kretprobe_trampoline:
https://elixir.bootlin.com/linux/v5.13/source/arch/x86/kernel/kprobes/core.c#L1027

trampoline_handler:
https://elixir.bootlin.com/linux/latest/source/arch/arm/probes/kprobes/core.c#L417

kretprobe_trampoline_handler:
https://elixir.bootlin.com/linux/latest/source/include/linux/kprobes.h#L235

__kretprobe_trampoline_handler:
https://elixir.bootlin.com/linux/latest/source/kernel/kprobes.c#L2007

restore_registers:
https://elixir.bootlin.com/linux/v5.13/source/arch/x86/kernel/kprobes/common.h#L32

kretprobe struct:
https://elixir.bootlin.com/linux/latest/source/include/linux/kprobes.h#L147