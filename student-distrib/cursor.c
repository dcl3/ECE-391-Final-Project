/* cursor.c - cursor of text mode
 * vim:ts=4 noexpandtab */

#include "cursor.h"
#include "lib.h"

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x0A, 0x3D4);
	outb((inb(0x3D5) & 0xC0) | cursor_start, 0x3D5);
 
	outb(0x0B, 0x3D4);
	outb((inb(0x3D5) & 0xE0) | cursor_end, 0x3D5);
}

void disable_cursor()
{
	outb(0x0A, 0x3D4);
	outb(0x20, 0x3D5);
}

void update_cursor(int x, int y)
{
	terminals[curr_terminal_id].screen_x = x;
	terminals[curr_terminal_id].screen_y = y;
	
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}

uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    outb(0x0F, 0x3D4);
    pos |= inb(0x3D5);
    outb(0x0E, 0x3D4);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}
