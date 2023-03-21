/* paging.h
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "lib.h"

#define NUM_ENTRIES 1024

#define KERNEL_ADDR 0x00400000
#define FOUR_KB 0x00001000

#define UPPER_TEN 0xffc00000
#define UPPER_TWENTY 0xfffff00

#define READ_WRITE_P 0x3
#define READ_WRITE_P_FOUR_MB 0x83

#define VIDEO_ADDR 0xb8000

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t page_table[1024] __attribute__((aligned(4096)));

// initialize paging
extern void paging_init();

// assembly code for modfying control registers
extern void load_enable_paging(unsigned int*);
