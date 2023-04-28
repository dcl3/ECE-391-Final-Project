#define ASM

.global jump_usermode

.global halt_return

.global terminal_switch_return

.extern test_user_function

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
    pushl %ebx

    cmpl $0, %eax
    jle  invalid_call
    cmpl $10, %eax
    jg  invalid_call

    pushl %edx
    pushl %ecx
    pushl %ebx

    call *systemcall_table(,%eax,4)

    popl %ebx
    popl %ecx
    popl %edx

    jmp systemcall_return

invalid_call:
    movl $-1, %eax

systemcall_return:
    popl %ebx
    popl %edi
    popl %esi
    iret

jump_usermode_unused:
    movw user_ds_arg, %ds
    pushl user_ds_arg
    pushl esp_arg
    pushfl
    pushl user_cs_arg
    pushl eip_arg
    iret

jump_usermode:
    movw user_ds_arg, %ds
    pushl user_ds_arg
    pushl esp_arg
    pushl $0x202
    pushl user_cs_arg
    pushl eip_arg
    iret

.globl flush_tlb
flush_tlb:
    movl %cr3, %eax
    movl %eax, %cr3
    ret

halt_return_0:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %eax
    movl 16(%ebp), %esp
    movl 12(%ebp), %ebp

    leave
    ret

halt_return_1:
    movl %eax, %esp
    movl %ecx, %ebp

    jmp *%edx

halt_return:
    movl halt_esp_arg, %esp
    movl halt_ebp_arg, %ebp

    jmp *halt_jump_ptr_arg

terminal_switch_return:
    movl terminal_switch_esp_arg, %esp
    movl terminal_switch_ebp_arg, %ebp

    jmp *terminal_switch_jump_ptr_arg
