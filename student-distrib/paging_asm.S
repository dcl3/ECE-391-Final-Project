# paging.S
# vim:ts=4 noexpandtab

#define ASM     1

.text

.globl load_enable_paging

load_enable_paging:
    push %ebp
    mov %esp, %ebp

    mov 8(%esp), %eax
    and $0xFFFFF000, %eax
    mov %eax, %cr3

    mov %cr4, %eax
    or $0x10, %eax
    mov %eax, %cr4

    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    mov %ebp, %esp
    pop %ebp
    ret