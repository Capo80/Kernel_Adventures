#include <linux/linkage.h>

.text
.intel_syntax noprefix
.global smart_call

# https://blog.nelhage.com/2010/10/amd64-and-va_arg/

smart_call:
    cmp rdi, 0                  # check that func address is not NULL
    je error

    mov rax, rdi                # mov jump addr to rax
    mov bl, sil                 # mov param number to rbx
    mov r14, [rdx+0x8]          # save va_list->overflow_arg_area
    mov r13, [rdx+0x10]         # save va_list->reg_save_area
    
    mov r12b, 6
    mov rdi, 0x10
stack_loop_start:               # check how many params we have and fix stack
    cmp bl, r12b
    jle end_loop
    mov r11, [r14+rdi]
    push r11
    add r12b, 1
    add rdi, 0x8
    jmp stack_loop_start

end_loop:
    cmp bl, 0                   # check how many params we have and fix registers
    je jump_check
    mov rdi, [r13+0x10]
    cmp bl, 1
    je jump_check
    mov rsi, [r13+0x18]
    cmp bl, 2
    je jump_check
    mov rdx, [r13+0x20]
    cmp bl, 3
    je jump_check
    mov r10, [r13+0x28]
    cmp bl, 4
    je jump_check
    mov r8, [r14] 
    cmp bl, 5
    je jump_check
    mov r9, [r14+0x8]   

jump_check:
    lea r15, [rip+return]
    push r15                    # push return address
    mov r11b, [rax]             # check where to jump
    cmp r11b, 0x0f              # NOP instruction
    je jump_offset
    cmp r11b, 0xe8              # CALL instruction
    je jump_offset
    jmp rax                     # its not NOP or CALL so we jmp to function start
jump_offset:
    add rax, 5
    jmp rax                     # its NOP or CALL we jump over the first instruction

return:
    mov r12b, 6
stack_clean_start:              # get the parameters out of the stack
    cmp bl, r12b
    jle end_stack_clean
    pop r9
    add r12b, 1
    jmp stack_clean_start

end_stack_clean:
    ret

error:                          # NULL as func address
    mov rax, -1
    ret