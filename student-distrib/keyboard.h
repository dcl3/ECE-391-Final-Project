/* keyboard.h - keyboard driver
 * vim:ts=4 noexpandtab
 */

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
#define RIGHT_CTRL 0xE01D
#define RIGHT_CTRL_RELEASE 0xE09D
#define LEFT_ALT 0x38
#define LEFT_ALT_RELEASE 0xB8
#define RIGHT_ALT 0xE038
#define RIGHT_ALT_RELEASE 0xE0B8
#define CAPS_LOCK 0x3A
#define CAPS_LOCK_RELEASE 0xBA

uint8_t shift_pressed = 0;
uint8_t ctrl_pressed = 0;
uint8_t alt_pressed = 0;
uint8_t caps_lock_pressed = 0;

// keyboard buffer
char kb_buffer[KEYBOARD_BUFFER_SIZE];
uint32_t kb_buffer_index = 0;

// initialize the keyboard
void keyboard_init(void);

// handles the interrupt from the keyboard
extern void keyboard_handler(void);

#endif /* KEYBOARD_H */
