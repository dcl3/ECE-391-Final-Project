# paging.S
# vim:ts=4 noexpandtab

#define ASM     1

.text

.globl load_enable_paging

load_enable_paging:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%esp), %eax
    andl $0xfffff000, %eax
    movl %eax, %cr3

    movl %cr4, %eax
    orl $0x10, %eax
    movl %eax, %cr4

    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0

    movl %ebp, %esp
    popl %ebp
    ret
