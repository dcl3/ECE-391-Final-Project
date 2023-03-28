
#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"

#define KEYBOARD_BUFFER_SIZE 128

extern volatile int32_t keyboard_flag;
// extern char* kb_buffer;
extern char kb_buffer[KEYBOARD_BUFFER_SIZE];
extern uint32_t kb_buffer_index;

int32_t terminal_open (const uint8_t* filename);
int32_t terminal_close (int32_t fd);
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes);

#endif /* TERMINAL_H */
