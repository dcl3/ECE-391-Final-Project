# paging.S
# vim:ts=4 noexpandtab

#define ASM     1

.text

.globl loadPageDirectory
.type loadPageDirectory, %function

.globl enablePaging
.type enablePaging, %function

loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    mov %ebp, %esp
    pop %ebp
    ret

enablePaging:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret