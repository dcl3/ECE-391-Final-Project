/* keyboard.h - keyboard driver
 * vim:ts=4 noexpandtab
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_IRQ_NUM 1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64
#define ALPHA_NUMERIC 0x3A

// initialize the keyboard
void keyboard_init(void);

// handles the interrupt from the keyboard
extern void keyboard_handler(void);

#endif /* KEYBOARD_H */
