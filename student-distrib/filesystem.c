/* filesystem.c
 * vim:ts=4 noexpandtab
 */

#include "filesystem.h"
#include "lib.h"

// pointers to filesystem structures
boot_block_t* boot_block_ptr;
inode_t* inode_ptr;
dblock_t* dblock_ptr;

/* 
 * filesystem_init
 *   DESCRIPTION: initialize the filesystem
 *   INPUTS: filesystem_ptr - pointer to the beginning of the filesystem
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void filesystem_init(uint32_t* filesystem_ptr) {
    boot_block_ptr = (boot_block_t*) filesystem_ptr;

    // first inode is 1 away from boot block
    inode_ptr = (inode_t*) (boot_block_ptr + 1);

    // first data block is number of inodes away from first inode
    dblock_ptr = (dblock_t*) (inode_ptr + (boot_block_ptr->num_inodes));
}

/* 
 * read_dentry_by_name
 *   DESCRIPTION: fill dentry_t block based on fname
 *   INPUTS: fname - file name to use to fill dentry_t block
 *   OUTPUTS: dentry - dentry_t block to be filled
 *   RETURN VALUE: 0 on success, -1 on failure
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i, j;

    // loop through every directory entry
    for (i = 0; i < boot_block_ptr->num_dentries; i++) {
        // check if directory entry with corresponding file name found
        if (strncmp((int8_t*) fname, (int8_t*) boot_block_ptr->dentries[i].f_name, FILE_NAME_LENGTH) == 0) {
            // fill in the dentry_t block
            for (j = 0; j < FILE_NAME_LENGTH; j++) {
                dentry->f_name[j] = boot_block_ptr->dentries[i].f_name[j];
            }
            dentry->f_type = boot_block_ptr->dentries[i].f_type;
            for (j = 0; j < DENTRY_RES; j++) {
                dentry->reserved[j] = boot_block_ptr->dentries[i].reserved[j];
            }
            dentry->inode_num = boot_block_ptr->dentries[i].inode_num;

            return 0;
        }
    }

    return -1;
};

/* 
 * read_dentry_by_index
 *   DESCRIPTION: fill dentry_t block based on index
 *   INPUTS: fname - file name to use to fill dentry_t block
 *   OUTPUTS: dentry - dentry_t block to be filled
 *   RETURN VALUE: 0 on succes, -1 on failure
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    int i;

    // check if index is within valid range
    if (index < 0 || index >= boot_block_ptr->num_dentries) {
        return -1;
    }
    
    // fill in the dentry_t block
    for (i = 0; i < FILE_NAME_LENGTH; i++) {
        dentry->f_name[i] = boot_block_ptr->dentries[index].f_name[i];
    }
    dentry->f_type = boot_block_ptr->dentries[index].f_type;
    for (i = 0; i < DENTRY_RES; i++) {
        dentry->reserved[i] = boot_block_ptr->dentries[index].reserved[i];
    }
    dentry->inode_num = boot_block_ptr->dentries[index].inode_num;

    return 0;
};

/* 
 * read_data
 *   DESCRIPTION: fill buffer with data based on inode number
 *   INPUTS: inode - inode to get data from
 *           offset - offset in bytes to start gettig data
 *           length - number of bytes of data to get
 *   OUTPUTS: buf - buffer to place data
 *   RETURN VALUE: 0 if reach end of file, number of bytes on success, -1 on failure
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    int i;
    uint32_t inode_idx;
    uint32_t dblock;
    uint32_t data_idx;

    // printf("grep size: %d\n", (inode_ptr + inode)->length);

    // loop for amount of bytes requested
    for (i = 0; i < length; i++) {

        // check if we have reached end of file
        if (i + offset > (inode_ptr + inode)->length) {
            return 0;
        }

        // set index within inode block to get correct data block number
        inode_idx = (i + offset) / (MAX_DATA);
        dblock = (inode_ptr + inode)->dblock_num[inode_idx];

        // index within the data block
        data_idx = (i + offset) % (MAX_DATA);

        // memcpy(buf[i], (dblock_ptr + dblock)->data[data_idx], 1);

        // fill in buffer with data
        buf[i] = (dblock_ptr + dblock)->data[data_idx];
    }

    return length;
};

// keeps track of number of times read from directory
int num_read = 0;

/* 
 * dir_read
 *   DESCRIPTION: reads files from a directory
 *   INPUTS: fd - function descriptor to read from
 *           nbytes - not used
 *   OUTPUTS: buf - buffer to place file names
 *   RETURN VALUE: number of characters in file name, 0 when reached end of directory
 */
int32_t dir_read (int32_t fd, void* buf, int32_t nbytes) {
    int i;
    dentry_t dentry;
    // printf("num_read: %d\n", num_read);
    
    // checks if reached end of directory
    if (num_read == boot_block_ptr->num_dentries) {
        // printf("1\n");
        // printf("num_read: %d\n", num_read);
        num_read = 0;
        // printf("num_read: %d\n", num_read);
        return 0;
    // checks if we are reading directory
    } else if (num_read == 0) {
        // printf("2\n");
        read_dentry_by_index(fd, &dentry);
        num_read += 1;
    // checks if we are reading a file
    } else {
        // printf("3\n");
        read_dentry_by_index(num_read, &dentry);
        num_read += 1;
    }

    // adds file name to buffer
    for (i = 0; i < FILE_NAME_LENGTH; i++) {
        *((uint8_t*) buf + i) = dentry.f_name[i];
    }
    // fixes smiley face at end of long file name
    *((uint8_t *) buf + FILE_NAME_LENGTH) = '\0';

    return strlen((int8_t*) dentry.f_name);
}

/* 
 * dir_write
 *   DESCRIPTION: does nothing because read only
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure
 */
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* 
 * dir_open
 *   DESCRIPTION: does nothing until process control block is implemented
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t dir_open (const uint8_t* filename) {
    return 0;
}

/* 
 * dir_close
 *   DESCRIPTION: does nothing until process control block is implemented
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t dir_close (int32_t fd) {
    return 0;
}

/* 
 * file_read
 *   DESCRIPTION: reads data from a file
 *   INPUTS: fd - function descriptor to read from
 *           nbytes - number of bytes to read
 *   OUTPUTS: buf - buffer to place data
 *   RETURN VALUE: 0 if reach end of file, number of bytes on success, -1 on failure
 */
int32_t file_read (int32_t fd, void* buf, int32_t nbytes) {
    dentry_t dentry;
    // printf("num_read: %d\n", num_read);

    // fill dentry based on file
    read_dentry_by_index(fd, &dentry);

    // fill buffer with data
    return read_data(dentry.inode_num, 0, (uint8_t*) buf, nbytes);
}

/* 
 * file_write
 *   DESCRIPTION: does nothing because read only
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure
 */
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* 
 * file_open
 *   DESCRIPTION: does nothing until process control block is implemented
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t file_open (const uint8_t* filename) {
    return 0;
}

/* 
 * file_close
 *   DESCRIPTION: does nothing until process control block is implemented
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */
int32_t file_close (int32_t fd) {
    return 0;
}
