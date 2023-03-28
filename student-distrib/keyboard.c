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

/* 
 * Keyboard_init
 *   DESCRIPTION: initialize the keyboard by enable interrupts
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   REFERENCE:
 *      https://wiki.osdev.org
 */
void keyboard_init() {
    // Enable interrupts for the keyboard
    clear();
    enable_irq(KEYBOARD_IRQ_NUM);
}

/* 
 * keyboard_handler
 *   DESCRIPTION: handler for keyboard, send a byte through a port whenever there is a typed character, and look up tables for characters
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   REFERENCE:
 *      https://wiki.osdev.org
 */
extern void keyboard_handler() {
    // Set critical section
    // cli();
    uint8_t scancode;

    // Read the scancode from the keyboard
    scancode = inb(KEYBOARD_DATA_PORT);

    // Handle the scancode
    if (scancode >= ALPHA_NUMERIC) {
        send_eoi(KEYBOARD_IRQ_NUM);
        return;  // Invalid scancode
    } else {
        // Print the character
        putc(keyboard_scancode_set1[scancode]);
    }

    // Exit critical section
    // sti();

    send_eoi(KEYBOARD_IRQ_NUM);

    return;
}
