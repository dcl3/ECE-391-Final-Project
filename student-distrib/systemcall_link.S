#define ASM

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