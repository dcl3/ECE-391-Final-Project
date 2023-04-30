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

    page_table[(VIDEO_ADDR / PAGE_SIZE) + 1] = ((VIDEO_ADDR + (1 * FOUR_KB)) & UPPER_TWENTY) | READ_WRITE_P;
    page_table[(VIDEO_ADDR / PAGE_SIZE) + 2] = ((VIDEO_ADDR + (2 * FOUR_KB)) & UPPER_TWENTY) | READ_WRITE_P;
    page_table[(VIDEO_ADDR / PAGE_SIZE) + 3] = ((VIDEO_ADDR + (3 * FOUR_KB)) & UPPER_TWENTY) | READ_WRITE_P;

    // filled page directories for first four mb and kernel
    page_directory[0] = (((unsigned int) page_table) & UPPER_TWENTY) | READ_WRITE_P;
    page_directory[1] = (KERNEL_ADDR & UPPER_TEN) | READ_WRITE_P_FOUR_MB;

    // load and enable paging
    load_enable_paging(page_directory);
}

// load into user space
void load_user(uint32_t curr_proc) {
    // 32 * 4MB = 128 MB
    page_directory[USER_DIR_OFF] = ((USER_ADDR + (curr_proc * FOUR_MB)) & UPPER_TEN) | USER_READ_WRITE_P_FOUR_MB;
}

// maps to user video memory
uint32_t map_user_vid() {
    // 32 * 4MB = 128 MB
    vid_page_table[0] = (VIDEO_ADDR & UPPER_TWENTY) | USER_RW_PTE;
    page_directory[USER_VID_DIR_OFF] = (((unsigned int) vid_page_table) & UPPER_TWENTY) | USER_RW_PDE;

    return USER_VID_DIR_OFF * FOUR_MB;
}
