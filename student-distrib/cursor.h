#ifndef _CURSOR_H
#define _CURSOR_H

#include "types.h"
#include "lib.h"

#define VGA_WIDTH 80
#define OFFSET_8 8

/* Enable the cursor */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
/* Disable the cursor */
void disable_cursor();
/* Update the cursor position */
void update_cursor(int x, int y);
/* Get cursor position */
uint16_t get_cursor_position(void);


#endif /* _CURSOR_H */