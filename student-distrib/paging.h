/* paging.h
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "lib.h"

#define KERNEL_ADDR 0x400000


// typedef struct page_directory_entry {
//     uint32_t p : 1;       
//     uint32_t rw : 1;            
//     uint32_t us : 1;            
//     uint32_t pwt : 1; 
//     uint32_t pcd : 1;       
//     uint32_t a : 1;      
//     uint32_t d : 1;      
//     uint32_t ps : 1;     
//     uint32_t g : 1;             
//     uint32_t avl : 3;       // bit 9:11
//     uint32_t pat : 20;      // bit 12:31
// } __attribute__ ((packed)) page_dir;


// typedef struct page_table_entry {
//     uint32_t p : 1;       
//     uint32_t rw : 1;            
//     uint32_t us : 1;            
//     uint32_t pwt : 1; 
//     uint32_t pcd : 1;       
//     uint32_t a : 1;      
//     uint32_t d : 1;      
//     uint32_t ps : 1;     
//     uint32_t g : 1;             
//     uint32_t avl : 3;       // bit 9:11
//     uint32_t pat : 20;      // bit 12:31
// } __attribute__ ((packed)) page_table;

extern void load_enable_paging(unsigned int*);

extern void paging_init();
