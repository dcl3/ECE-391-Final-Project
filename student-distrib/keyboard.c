/* keyboard.c - keyboard driver
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"


uint8_t shift_pressed = 0;
uint8_t ctrl_pressed = 0;
uint8_t alt_pressed = 0;
uint8_t caps_lock_pressed = 0;


// keyboard buffer
char kb_buffer[KEYBOARD_BUFFER_SIZE];
uint32_t kb_buffer_index = 0;

static const unsigned short keyboard_scancode_set1[ALPHA_NUMERIC] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',
    0, /* All other keys are undefined */
};

static const unsigned short keyboard_scancode_modified[ALPHA_NUMERIC] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
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
    cli();
    uint8_t scancode;

    // Read the scancode from the keyboard
    scancode = inb(KEYBOARD_DATA_PORT);

    unsigned short character;

    if(check_for_modifier(scancode)) {
        send_eoi(KEYBOARD_IRQ_NUM);
        sti();
        return;
    }

    // Handle the scancode
    if(ctrl_pressed && keyboard_scancode_set1[scancode] == 'l') {
        clear();
        send_eoi(KEYBOARD_IRQ_NUM);
        sti();
        return;
    }
    if (keyboard_scancode_set1[scancode] >= 'a' && keyboard_scancode_set1[scancode] <= 'z') {
        if(caps_lock_pressed ^ shift_pressed) 
            character = keyboard_scancode_modified[scancode];
        else
            character = keyboard_scancode_set1[scancode];
    } else {
        if(shift_pressed)
            character = keyboard_scancode_modified[scancode];
        else
            character = keyboard_scancode_set1[scancode];
    }

    // Send the character to the terminal
    terminal_write((uint8_t*)&character, 1);
    kb_buffer[kb_buffer_index] = character;
    kb_buffer_index++;

    // Exit critical section
    sti();

    send_eoi(KEYBOARD_IRQ_NUM);

    return;
}


int32_t check_for_modifier(uint8_t scancode) {
    if(scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) {
        shift_pressed = 1;
        return 1;
    }
    if(scancode == LEFT_CTRL) {
        ctrl_pressed = 1;
        return 1;
    }
    if(scancode == LEFT_ALT) {
        alt_pressed = 1;
        return 1;
    }
    if(scancode == BACKSPACE) {
        if(kb_buffer_index > 0) {
            kb_buffer_index--;
            putc('\b');
        }
        return 1;
    }
    if(scancode == CAPS_LOCK) {
        if(caps_lock_pressed)
            caps_lock_pressed = 0;
        else
            caps_lock_pressed = 1;
        return 1;
    }
    if(scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) {
        shift_pressed = 0;
        return 1;
    }
    if(scancode == LEFT_CTRL_RELEASE) {
        ctrl_pressed = 0;
        return 1;
    }
    if(scancode == LEFT_ALT_RELEASE) {
        alt_pressed = 0;
        return 1;
    }
    return 0;
}

