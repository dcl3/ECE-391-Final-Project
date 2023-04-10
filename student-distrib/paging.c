/* paging.c
 * vim:ts=4 noexpandtab
 */

#include "paging.h"

// initilizes paging
void paging_init() {
    unsigned int i;

    // Clear the page directory and page table entries
    for (i = 0; i < NUM_ENTRIES; i++) {
        page_directory[i] = 0;
        page_table[i] = 0;
    }

    // fill page table entry with video memory
    page_table[VIDEO_ADDR / PAGE_SIZE] = (VIDEO_ADDR & UPPER_TWENTY) | READ_WRITE_P;

    // filled page directories for first four mb and kernel
    page_directory[0] = (((unsigned int) page_table) & UPPER_TWENTY) | READ_WRITE_P;
    page_directory[1] = (KERNEL_ADDR & UPPER_TEN) | READ_WRITE_P_FOUR_MB;

    // load and enable paging
    load_enable_paging(page_directory);
}

void load_user(uint32_t num_proc) {
    // 32 * 4MB = 128 MB
    page_directory[32] = ((USER_ADDR + (num_proc * FOUR_MB)) & UPPER_TEN) | USER_READ_WRITE_P_FOUR_MB;
}
