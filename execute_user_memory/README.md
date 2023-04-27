# Executing user memory

Steps:

|||
|:-|:-:|
|Allocate executable user memory| :heavy_check_mark:|
|Patch syscall out of user code| :wavy_dash:|
|Execute user memory| :heavy_check_mark: |

## Allocate executable user memory

Start with executable memory:

```c
// new vmalloc allocation for executable memory
// https://elixir.bootlin.com/linux/v5.13/source/mm/vmalloc.c#L2865
void *p = __my_vmalloc_node_range(size, 1, VMALLOC_START, VMALLOC_END,
        GFP_KERNEL, PAGE_KERNEL_EXEC, 0,
        NUMA_NO_NODE, __builtin_return_address(0));
```

Then map it to user:

```c
void* map_user_address(void* kernel_addr) {
    
    pgd_t* page_table;
    unsigned long user_addr, pgd_offset;
    unsigned long cast_kernel_addr = (unsigned long) kernel_addr;
    
    page_table = (pgd_t*) phys_to_virt(my_read_cr3() & ADDRESS_MASK);

    pgd_offset = get_free_pdg_offset(page_table, (unsigned long) kernel_addr);

    page_table[pgd_offset] = page_table[PML4(cast_kernel_addr)];
    user_addr = (cast_kernel_addr & 0x7fffffffff) | (pgd_offset << 39);

    return (void*) user_addr;
}
```

## Patch sycall out of code

we cannot use ```syscall``` in ring 0

### Idea 1

patch it out, map every syscall with the address of the actual function, need to do this:

```
0:  48 c7 c0 3c 00 00 00    mov    rax,0x3c    ; exit
7:  48 c7 c7 42 00 00 00    mov    rdi,0x42    ; error_code
e:  0f 05                   syscall

// trasform to

0:  48 c7 c0 c0 ab 0a 81    mov    rax,0xffffffff810aabc0
7:  48 c7 c7 42 00 00 00    mov    rdi,0x42
e:  ff d0                   call   rax 
```

Problem, we need 9 bytes of space to call an absolute address, without controlling compilation, in the worst case we only have 4, example from socat:

```
3fcce:       b0 05                   mov    al, 0x5
3fcd0:       0f 05                   syscall 
```

Maybe, add a stub in the kernel to do the stuff?

### Execute user memory

We just jump into it and it works.