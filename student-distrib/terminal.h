
#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"

#define KEYBOARD_BUFFER_SIZE 128

#define MAX_TERMINALS 3

extern volatile int32_t keyboard_flag;
// extern char* kb_buffer;
extern char kb_buffer[KEYBOARD_BUFFER_SIZE];
extern uint32_t kb_buffer_index;

uint32_t curr_terminal_id;

typedef struct terminal {
    uint32_t active;
    uint32_t active_proc_id;
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t kb_buffer[KEYBOARD_BUFFER_SIZE];
    uint32_t screen_x;
    uint32_t screen_y;
} terminal_t;

terminal_t terminals[MAX_TERMINALS];

// opens terminal
int32_t terminal_open (const uint8_t* filename);
// closes terminal
int32_t terminal_close (int32_t fd);
// reads from terminal
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);
// writes to terminal
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes);

#endif /* TERMINAL_H */
