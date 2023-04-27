# Kmatrioska for kernel > 5.10

Moving the address space limit is not possible anymore for some architectures, articles:

https://lwn.net/Articles/722267/

https://lwn.net/Articles/832121/

Some API have been updated but the init_module is not supposed to be called from the kernel so it stayed the same.

## Original

https://github.com/milabs/kmatryoshka

## User address limit checks

The address limit check is done with the access_ok macro, defined for every architecture: 

https://elixir.bootlin.com/linux/v5.13/C/ident/access_ok

## Limit value

The actual limit is arhcitecture dependent, for some is fixed, some use the same macro to hold the address, TASK_SIZE_MAX:

https://elixir.bootlin.com/linux/v5.13/C/ident/TASK_SIZE_MAX

This are architectures where it is not possible to change the address are:
- x86_64
- arm64
- powerpc
- s390
- mips

The list may not be exhaustive.

Mips does not use the TASK_SIZE_MAX name but the adddress is still fixed:

https://elixir.bootlin.com/linux/v5.13/source/arch/mips/include/asm/uaccess.h#L79

## Examples of limit check

### x86_64

Limit is TASK_SIZE_MAX:

https://elixir.bootlin.com/linux/v5.13/source/arch/x86/include/asm/page_64_types.h#L64

default is:

https://elixir.bootlin.com/linux/v5.13/source/arch/x86/include/asm/page_64_types.h#L61

```
PAGE_SIZE = 4096
__VIRTUAL_MASK_SHIFT = 47
((_AC(1,UL) << __VIRTUAL_MASK_SHIFT) - PAGE_SIZE) = 0x7ffffffff000
```

Hard coded in header file, not changeble

### Arm 32 bits

Limit is in the thread_info struct which is on top of the stack:

https://elixir.bootlin.com/linux/v5.13/source/arch/arm/include/asm/thread_info.h#L91

https://elixir.bootlin.com/linux/v5.13/source/arch/arm/include/asm/thread_info.h#L57

Can be changed

## Module loading

Syscall is resolved here:

https://elixir.bootlin.com/linux/v5.13/source/kernel/module.c#L4116

### Idea #1 - let's call load_module

The load_module fuction loads a module from kernel memory, the symbol is exported so we can search it with ```kallsyms_lookup_name``` and call it.

Unfortunately this does not work beacuse the function still wants the user arguments of the module in a user space address.

The call to load_module fails here when it checks that uargs is in user memory:

https://elixir.bootlin.com/linux/v5.13/source/kernel/module.c#L4019

### Idea #2 - just rewrite load_module

Impossible some symbols are not exported.

### Idea #3 - just bruteforce a valid user space address

Impossible address space is too big.

### Idea #4 - steal a user level string from someone else

Impossible cannot steal from our user address space only from others

### Idea #5 - probe the strndup_user functions to pass the checks

It works, we pass a special address (0x4242) as uargs to the load_module function, the probe on strndup_user recognizes it and modifies the return value with a kernel string allocated by us with a valid uargs string.

working prototype for kernel 5.13 in the ```new_kmatryoska_probes``` folder

## Hardening the prototype

### kallsyms --> bruteforce from known symbol

Kallsyms is not available on every system, we can try to search our function signature from the closest exported symbol.

Two symbols needed in this prototype:
- load_module, can start the search from "module_put";
- __vmalloc_node_range, can start the search from "__vmalloc";

In both cases the symbol is close (MAX_DISTANCE < 0xfffff) and a signature of ~32 byte is enough to avoid false positives.

### kprobes --> binary patching

Kprobes are nore available on every system, instead we can patch the function directly in memory.

Done for the strndup.

The function is pretty small so we don't need to go back to the original one, we just reimplement the whole thing, and put it back when we are done.



