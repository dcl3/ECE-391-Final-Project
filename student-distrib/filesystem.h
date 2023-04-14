/* filesystem.h
 * vim:ts=4 noexpandtab
 */

#ifndef _filesystem_H
#define _filesystem_H

#include "types.h"

#define FILE_NAME_LENGTH 32
#define DENTRY_RES 24
#define BOOT_BLOCK_RES 52
#define NUM_DENTRIES 63
#define MAX_DBLOCK_NUM 1023
#define MAX_DATA 4096

extern uint8_t num_processes;
extern uint8_t curr_proc;

// structure for directory entry
typedef struct dentry {
    uint8_t f_name[FILE_NAME_LENGTH];
    uint32_t f_type;
    uint32_t inode_num;
    uint8_t reserved[DENTRY_RES];
} dentry_t;

// structure for boot block
typedef struct boot_block {
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t num_dblocks;
    uint8_t reserved[BOOT_BLOCK_RES];
    dentry_t dentries[NUM_DENTRIES];
} boot_block_t;

// structure for index node
typedef struct inode {
    uint32_t length;
    uint32_t dblock_num[MAX_DBLOCK_NUM];
} inode_t;

// structure for data block
typedef struct dblock {
    uint8_t data[MAX_DATA];
} dblock_t;

void filesystem_init(uint32_t* filesystem_ptr);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t load_program(uint32_t inode, uint32_t num_proc);

int32_t dir_read (int32_t fd, void* buf, int32_t nbytes);

int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open (const uint8_t* filename);

int32_t dir_close (int32_t fd);

int32_t file_read (int32_t fd, void* buf, int32_t nbytes);

int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);

int32_t file_open (const uint8_t* filename);

int32_t file_close (int32_t fd);

#endif /* _filesystem_H */
