/* paging.c
 * vim:ts=4 noexpandtab
 */

#include "paging.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t page_table[1024] __attribute__((aligned(4096)));

void paging_init() {
    unsigned int i;

    // Clear the page directory and page table entries
    for (i = 0; i < 1024; i++) {
        page_directory[i] = 0;
        page_table[i] = 0;
    }

    page_directory[0] = ((unsigned int) page_table) | 0x3;
    page_directory[1] = (KERNEL_ADDR & 0xffc00000) | 0x83;

    // Map the first 4MB of physical memory to virtual address 0
    // page_table[0] = 0x00000007; // Present, Read/Write, User/Supervisor

    // Enable paging
    load_enable_paging(&page_directory);
}
