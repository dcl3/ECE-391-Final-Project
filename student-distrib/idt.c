/* idt.c - Functions to interact with the idt
 * vim:ts=4 noexpandtab
 */

#include "idt.h"
#include "lib.h"
#include "x86_desc.h"
// #include "keyboard.h"
#include "interrupt_link.h"
#include "rtc.h"
#include "systemcall_link.h"

/* 
 * idt_init
 *   DESCRIPTION: initialize the idt
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   REFERENCE:
 *      https://wiki.osdev.org
 */
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
            idt[i].present = 0;
        } else {
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4 = 0;
            idt[i].reserved3 = 0;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1;
            idt[i].size = 1;
            idt[i].reserved0 = 0;
            idt[i].dpl = DPL_KERNEL;
            idt[i].present = 0;
        }
    }

    // make the idt values present, and set the idt entries
    idt[DE].present = 1;
    SET_IDT_ENTRY(idt[DE], de);
    idt[DB].present = 1;
    SET_IDT_ENTRY(idt[DB], db);
    idt[NMI].present = 1;
    SET_IDT_ENTRY(idt[NMI], nmi);
    idt[BP].present = 1;
    SET_IDT_ENTRY(idt[BP], bp);
    idt[OF].present = 1;
    SET_IDT_ENTRY(idt[OF], of);
    idt[BR].present = 1;
    SET_IDT_ENTRY(idt[BR], br);
    idt[UD].present = 1;
    SET_IDT_ENTRY(idt[UD], ud);
    idt[NM].present = 1;
    SET_IDT_ENTRY(idt[NM], nm);
    idt[DF].present = 1;
    SET_IDT_ENTRY(idt[DF], df);
    idt[CSO].present = 1;
    SET_IDT_ENTRY(idt[CSO], cso);
    idt[TS].present = 1;
    SET_IDT_ENTRY(idt[TS], ts);
    idt[NP].present = 1;
    SET_IDT_ENTRY(idt[NP], np);
    idt[SFE].present = 1;
    SET_IDT_ENTRY(idt[SFE], sfe);
    idt[GP].present = 1;
    SET_IDT_ENTRY(idt[GP], gp);
    idt[PF].present = 1;
    SET_IDT_ENTRY(idt[PF], pf);
    idt[MF].present = 1;
    SET_IDT_ENTRY(idt[MF], mf);
    idt[AC].present = 1;
    SET_IDT_ENTRY(idt[AC], ac);
    idt[MC].present = 1;
    SET_IDT_ENTRY(idt[MC], mc);
    idt[XF].present = 1;
    SET_IDT_ENTRY(idt[XF], xf);

    // modify idt entry for system call
    idt[SYS_CALL].dpl = DPL_USER;
    idt[SYS_CALL].present = 1;
    SET_IDT_ENTRY(idt[SYS_CALL], systemcall_handler);

    // modify idt entry for keyboard
    idt[0x21].present = 1;
    SET_IDT_ENTRY(idt[KEYBOARD_ENTRY], keyboard_handler_linkage);

    // modify idt entry for rtc
    idt[0x28].present = 1;
    SET_IDT_ENTRY(idt[RTC_ENTRY], rtc_handler_linkage);
}

/* Our approaches to IDT exceptions is to create separate exception functions */

// function for Divide Error
void de(void) {
    printf("Divide Error Exception\n");
    while(1){};
}

// function for Debug Exception
void db(void) {
    printf("Debug Exception\n");
    while(1){};
}

// function for NMI
void nmi(void) {
    printf("NMI Interrupt\n");
    while(1){};
}

// function for Breakpoint exception
void bp(void) {
    printf("Breakpoint Exception\n");
    while(1){};
}

// function for Overflow Exception
void of(void) {
    printf("Overflow Exception\n");
    while(1){};
}

// function for BOUND Range Exceed Exception
void br(void) {
    printf("BOUND Range Exceeded Exception\n");
    while(1){};
}

// function for Invalid Opcode
void ud(void) {
    printf("Invalid Opcode Exception\n");
    while(1){};
}

// function for Device Not Available
void nm(void) {
    printf("Device Not Available Exception\n");
    while(1){};
}

// function for Double Fault
void df(void) {
    printf("Double Fault Exception\n");
    while(1){};
}

// function for CSO
void cso(void) {
    printf("Coprocessor Segment Overrun\n");
    while(1){};
}

// function for Invalid TSS
void ts(void) {
    printf("Invalid TSS Exception\n");
    while(1){};
}

// function for Segment Not Present
void np(void) {
    printf("Segment Not Present\n");
    while(1){};
}

// function for Stack Fault
void sfe(void) {
    printf("Stack Fault Exception\n");
    while(1){};
}

// function for  General Protection
void gp(void) {
    printf("General Protection Exception\n");
    while(1){};
}

// function for Page Fault
void pf(void) {
    printf("Page-Fault Exception\n");
    while(1){};
}

// function for Floating Error
void mf(void) {
    printf("x87 FPU Floating-Point Error\n");
    while(1){};
}

// function for Alignment Check
void ac(void) {
    printf("Alignment Check Exception\n");
    while(1){};
}

// function for Machine Check
void mc(void) {
    printf("Machine-Check Exception\n");
    while(1){};
}

// function for SIMD Floating ponit exception
void xf(void) {
    printf("SIMD Floating-Point Exception\n");
    while(1){};
}
