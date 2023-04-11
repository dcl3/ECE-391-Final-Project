#define ASM

.global jump_usermode
extern test_user_function

.align 4
systemcall_table:
    .long 0x0
    .long syscall_halt
    .long syscall_execute
    .long syscall_read
    .long syscall_write
    .long syscall_open
    .long syscall_close
    .long syscall_getargs
    .long syscall_vidmap
    .long syscall_set_handler
    .long syscall_sigreturn

.align 4
.globl systemcall_handler
systemcall_handler:
    pushl %esi
    pushl %edi
    pushl %ebp
    pushfl

    pushl %edx
    pushl %ecx
    pushl %ebx

    cmp $0, %eax
    jle  invalid_call
    cmp $10, %eax
    jg  invalid_call

    call *systemcall_table(,%eax,4)
    jmp systemcall_return

invalid_call:
    movl $-1, %eax

systemcall_return:
    popl %ebx
    popl %ecx
    popl %edx

    popfl
    popl %ebp
    popl %edi
    popl %esi
    iret

jump_usermode:
	movw (4 * 8) | 3, ax ; ring 3 data with bottom 2 bits set for ring 3
	movw ax, ds
	movw ax, es
	movw ax, fs
	movw ax, gs ; SS is handled by iret
 
	; set up the stack frame iret expects
	movl esp, eax
	pushl (4 * 8) | 3 ; data selector
	pushl eax ; current esp
	pushf ; eflags
	pushl (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	pushl test_user_function ; instruction address to return to
	iret

flush_tlb:
    movl %cr3, %eax
    movl %eax, %cr3
    ret