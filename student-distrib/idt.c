/* idt.c - Functions to interact with the idt
 * vim:ts=4 noexpandtab
 */

#include "idt.h"
#include "lib.h"
#include "x86_desc.h"

/* Initialize the idt */
void idt_init(void) {
    int i;

    for (i = 0; i < NUM_VEC; i++) {
        // traps then interrupts
        if (i < 0x20) {
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4 = 0;
            idt[i].reserved3 = 1;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1;
            idt[i].size = 1;
            idt[i].reserved0 = 0;
            idt[i].dpl = DPL_KERNEL;
            idt[i].present = 1;
        } else {
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4 = 0;
            idt[i].reserved3 = 0;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1;
            idt[i].size = 1;
            idt[i].reserved0 = 0;
            idt[i].dpl = DPL_KERNEL;
            idt[i].present = 1;
        }
    }

    SET_IDT_ENTRY(idt[DE], de);
    SET_IDT_ENTRY(idt[DB], db);
    SET_IDT_ENTRY(idt[NMI], nmi);
    SET_IDT_ENTRY(idt[BP], bp);
    SET_IDT_ENTRY(idt[OF], of);
    SET_IDT_ENTRY(idt[BR], br);
    SET_IDT_ENTRY(idt[UD], ud);
    SET_IDT_ENTRY(idt[NM], nm);
    SET_IDT_ENTRY(idt[DF], df);
    SET_IDT_ENTRY(idt[CSO], cso);
    SET_IDT_ENTRY(idt[TS], ts);
    SET_IDT_ENTRY(idt[NP], np);
    SET_IDT_ENTRY(idt[SFE], sfe);
    SET_IDT_ENTRY(idt[GP], gp);
    SET_IDT_ENTRY(idt[PF], pf);
    SET_IDT_ENTRY(idt[MF], mf);
    SET_IDT_ENTRY(idt[AC], ac);
    SET_IDT_ENTRY(idt[MC], mc);
    SET_IDT_ENTRY(idt[XF], xf);

    // modify idt entry for system call
    idt[SYS_CALL].reserved3 = 1;
    idt[SYS_CALL].dpl = DPL_USER;
    SET_IDT_ENTRY(idt[SYS_CALL], sys_call);

}

void de(void) {
    printf("Divide Error Exception\n");
    while(1){};
}

void db(void) {
    printf("Debug Exception\n");
    while(1){};
}

void nmi(void) {
    printf("NMI Interrupt\n");
    while(1){};
}

void bp(void) {
    printf("Breakpoint Exception\n");
    while(1){};
}

void of(void) {
    printf("Overflow Exception\n");
    while(1){};
}

void br(void) {
    printf("BOUND Range Exceeded Exception\n");
    while(1){};
}

void ud(void) {
    printf("Invalid Opcode Exception\n");
    while(1){};
}

void nm(void) {
    printf("Device Not Available Exception\n");
    while(1){};
}

void df(void) {
    printf("Double Fault Exception\n");
    while(1){};
}

void cso(void) {
    printf("Coprocessor Segment Overrun\n");
    while(1){};
}

void ts(void) {
    printf("Invalid TSS Exception\n");
    while(1){};
}

void np(void) {
    printf("Segment Not Present\n");
    while(1){};
}

void sfe(void) {
    printf("Stack Fault Exception\n");
    while(1){};
}

void gp(void) {
    printf("General Protection Exception\n");
    while(1){};
}

void pf(void) {
    printf("Page-Fault Exception\n");
    while(1){};
}

void mf(void) {
    printf("x87 FPU Floating-Point Error\n");
    while(1){};
}

void ac(void) {
    printf("Alignment Check Exception\n");
    while(1){};
}

void mc(void) {
    printf("Machine-Check Exception\n");
    while(1){};
}

void xf(void) {
    printf("SIMD Floating-Point Exception\n");
    while(1){};
}

void sys_call(void) {
    printf("System Call");
    while(1){};
}
