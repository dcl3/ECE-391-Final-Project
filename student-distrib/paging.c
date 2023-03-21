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
    page_table[0] = (VIDEO_ADDR & UPPER_TEN) | READ_WRITE_P;

    // fille page directories for first four mb and kernel
    page_directory[0] = (((unsigned int) page_table) & UPPER_TWENTY) | READ_WRITE_P;
    page_directory[1] = (KERNEL_ADDR & UPPER_TEN) | READ_WRITE_P_FOUR_MB;

    // load and enable paging
    load_enable_paging(page_directory);
}
