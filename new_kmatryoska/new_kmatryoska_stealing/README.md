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

