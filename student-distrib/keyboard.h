/* keyboard.h - keyboard driver
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_IRQ_NUM 1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64
#define ALPHA_NUMERIC 0x3A

#define KEYBOARD_BUFFER_SIZE 128

#define LEFT_SHIFT 0x2A
#define LEFT_SHIFT_RELEASE 0xAA
#define RIGHT_SHIFT 0x36
#define RIGHT_SHIFT_RELEASE 0xB6
#define LEFT_CTRL 0x1D
#define LEFT_CTRL_RELEASE 0x9D
#define LEFT_ALT 0x38
#define LEFT_ALT_RELEASE 0xB8
#define CAPS_LOCK 0x3A
#define CAPS_LOCK_RELEASE 0xBA
#define BACKSPACE 0x08
#define ENTER 0x1C

extern volatile int32_t keyboard_flag = 0;
extern char* kb_buffer;
extern uint32_t kb_buffer_index;

// initialize the keyboard
void keyboard_init(void);

// handles the interrupt from the keyboard
extern void keyboard_handler(void);

int32_t check_for_modifier(uint8_t scancode);

#endif /* KEYBOARD_H */

