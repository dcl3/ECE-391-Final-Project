/* keyboard.c - keyboard driver
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

static const unsigned short keyboard_scancode_set1[ALPHA_NUMERIC] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',
    0, /* All other keys are undefined */
};

void init_keyboard() {
    // Enable interrupts for the keyboard
    enable_irq(KEYBOARD_IRQ_NUM);
}

extern void keyboard_interrupt_handler() {
    // Set critical section
    cli();

    // Read the scancode from the keyboard
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // Handle the scancode
    if (scancode >= ALPHA_NUMERIC) return;  // Invalid scancode

    // Print the character
    putc(keyboard_scancode_set1[scancode]);


    send_eoi(KEYBOARD_IRQ_NUM);
    // Exit critical section
    sti();

}