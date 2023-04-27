# Trap based syscall handling

Idea: patch the syscall instruction in the user program with and int 3

## How to intercept SIGTRAP

When an int 3 is executed the kernel calls the ```do_int3``` function, in here the kernel goes trough the [die notifier callchain](https://elixir.bootlin.com/linux/latest/source/arch/x86/kernel/traps.c#L788) and calls any registered handler.

We register a custom handler on the callchain with:

```c
int register_die_notifier(struct notifier_block *nb);
int unregister_die_notifier(struct notifier_block *nb);
```

In the handler we check if the trapping process is ours, if it is we prepare for a syscall.

TODO: check that this is a patched int 3 and not a natural exception (maybe see rip?)

## How to do syscall

Step 1, find the syscall table location to obtain the real addresses of the syscall funcitons

Step 2, mimic the real [syscall dispatcher](https://elixir.bootlin.com/linux/v4.7/source/arch/x86/entry/common.c#L333) and change anithing that assumes that we are coming from the user

## Problem