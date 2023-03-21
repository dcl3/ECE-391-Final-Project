/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

// adapted from lecture 10
/* Initialize the 8259 PIC */
void i8259_init(void) {
    cli();

    // mask all interrupts
    outb(IRQ_ALL_MASK, MASTER_8259_DATA);
    outb(IRQ_ALL_MASK, SLAVE_8259_DATA);

    // init control word sequence for master
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW4, MASTER_8259_DATA);

    // init control word sequence for slave
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    // mask all interrupts
    outb(IRQ_ALL_MASK, MASTER_8259_DATA);
    outb(IRQ_ALL_MASK, SLAVE_8259_DATA);

    // unmask IR2 on master for slave connection
    enable_irq(IR2);
    sti();

}

// adapted from OS Dev, https://wiki.osdev.org/8259_PIC
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;

    // determine which 8259
    if (irq_num < NUM_IR) {
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= NUM_IR;
    }

    // replace irq mask with 0 (unmasked)
    value = inb(port) & ~(1 << irq_num);
    outb(value, port);
}

// adapted from OS Dev, https://wiki.osdev.org/8259_PIC
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;

    // determine which 8259
    if (irq_num < NUM_IR) {
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= NUM_IR;
    }

    // replace irq mask with 1 (masked)
    value = inb(port) | (1 << irq_num);
    outb(value, port);
}

// adapted from OS Dev, https://wiki.osdev.org/8259_PIC
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // determine if slave 8259
    if (irq_num >= NUM_IR) {
        // EOI for both master IR2 and slave irq
        outb(EOI | (irq_num - NUM_IR), SLAVE_8259_PORT);
        outb(EOI | IR2, MASTER_8259_PORT);
    } else {
        // EOI for just master
        printf("here");
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
}
