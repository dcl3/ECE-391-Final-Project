/* paging.h
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "lib.h"

#define NUM_ENTRIES 1024
#define PAGE_SIZE 4096

#define KERNEL_ADDR 0x00400000
#define FOUR_KB 0x00001000
#define FOUR_MB 0x00400000

#define UPPER_TEN 0xffc00000
#define UPPER_TWENTY 0xfffff00

#define READ_WRITE_P 0x3
#define READ_WRITE_P_FOUR_MB 0x83
#define USER_READ_WRITE_P_FOUR_MB 0x87

#define VIDEO_ADDR 0xb8000

#define USER_ADDR 0x00800000

uint32_t page_directory[NUM_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

uint32_t page_table[NUM_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

// initialize paging
extern void paging_init();

// assembly code for modfying control registers
extern void load_enable_paging(unsigned int*);
