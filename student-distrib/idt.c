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

    SET_IDT_ENTRY(idt[0], de);

    printf("size: %d\n", idt[0].size);
}

void de(void) {
    printf("Divide Error Exception\n");
    while(1){};
}
