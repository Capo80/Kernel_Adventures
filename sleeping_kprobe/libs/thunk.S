/* Thunks ("mediator functions") used to call the handlers. */

#include <linux/linkage.h>
#include <asm/asm.h>

.extern kproble_tail_call

#define SK_SAVE_SCRATCH_BUT_AX \
	push %rcx; \
	push %rdx; \
	push %rsi; \
	push %rdi; \
	push %r8;  \
	push %r9;  \
	push %r10; \
	push %r11;

#define SK_RESTORE_SCRATCH_BUT_AX \
	pop %r11; \
	pop %r10; \
	pop %r9;  \
	pop %r8;  \
	pop %rdi; \
	pop %rsi; \
	pop %rdx; \
	pop %rcx;
	
#define SK_SAVE_CALEE_REG \
	push %rbx; \
	push %r12; \
	push %r13; \
	push %r14; \
	push %r15; 

#define SK_RESTORE_CALEE_REG \
	pop %rbx; \
	pop %r12; \
	pop %r13; \
	pop %r14; \
	pop %r15;

#define SK_PUSHF pushfq
#define SK_POPF popfq
	

/* Creates a thunk ("_thunk_name") that calls a given function of RaceHound
 * ("_func") preserving the registers and flags. 
 * "_func" must be a normal function, i.e. with the default calling 
 * convention / linkage. 
 * It must have the following type:
 *    unsigned long _func(void);
 * 
 * For kprobes:
 * "_func" is expected to return the address to pass control to. 
 * The address will be in %eax/%rax after "call _func". We place it on top 
 * of the stack while restoring the old value of %eax/%rax and use "ret" to 
 * pass control there. 
 *
 * For kretprobes:
 * the correct return address should be passed in the rdx register
 * the asm will push it onto the stack before calling the tail func to return in the correct place
 * also the return value of the tail is going to be the return value if the function
 */
#define SK_MAKE_THUNK(_thunk_name, _func) \
	.global _thunk_name; \
	.type _thunk_name,@function; \
	_thunk_name: \
		push %_ASM_AX; \
		SK_SAVE_SCRATCH_BUT_AX; \
		SK_PUSHF; \
		call _func; \
		SK_POPF; \
		SK_RESTORE_SCRATCH_BUT_AX; \
		xchg %_ASM_AX, (%_ASM_SP); \
		ret; \
	.size _thunk_name, .-_thunk_name;

#define SK_MAKE_RET_THUNK(_thunk_name, _func) \
	.global _thunk_name; \
	.type _thunk_name,@function; \
	_thunk_name: \
		push %_ASM_DX; \
		push %_ASM_AX; \
		call _func; \
        pop %_ASM_AX;\
		ret; \
	.size _thunk_name, .-_thunk_name;

.text
SK_MAKE_RET_THUNK(sk_thunk_tail, kproble_tail_call)