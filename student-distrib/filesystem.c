/* filesystem.c
 * vim:ts=4 noexpandtab
 */

#include "filesystem.h"
#include "lib.h"
#include "task_struct.h"

// pointers to filesystem structures
boot_block_t* boot_block_ptr;
inode_t* inode_ptr;
dblock_t* dblock_ptr;

uint8_t num_processes = 0;
uint8_t curr_proc = 0;

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
    // for (i = 0; i < FILE_NAME_LENGTH; i++) {
    //     dentry->f_name[i] = boot_block_ptr->dentries[index].f_name[i];
    // }
    strncpy((int8_t*) dentry->f_name, (int8_t*) boot_block_ptr->dentries[index].f_name, MAX_F_NAME_LENGTH);
    // dentry->f_name[FILE_NAME_LENGTH] = '\0';
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
    uint32_t num_bytes_read = 0;

    if(offset >= (inode_ptr + inode)->length)
        return 0;

    // printf("grep size: %d\n", (inode_ptr + inode)->length);

    // loop for amount of bytes requested
    for (i = 0; i < length; i++) {

        // check if we have reached end of file
        if (i + offset > (inode_ptr + inode)->length) {
            return num_bytes_read;
        }

        // set index within inode block to get correct data block number
        inode_idx = (i + offset) / (MAX_DATA);
        dblock = (inode_ptr + inode)->dblock_num[inode_idx];

        // index within the data block
        data_idx = (i + offset) % (MAX_DATA);

        // memcpy(buf + i, &(dblock_ptr + dblock)->data[data_idx], 1);

        // fill in buffer with data
        buf[i] = (dblock_ptr + dblock)->data[data_idx];
        num_bytes_read++;
    }

    return num_bytes_read;
};

/* 
 * load_program
 *   DESCRIPTION: check if num_proc is valid based on inode number
 *   INPUTS: inode - inode to get data from
 *           num_proc - number of process in pcb
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success, -1 on failure
 */
int32_t load_program(uint32_t inode_num, uint32_t num_proc) {
    inode_t* temp_inode_ptr = inode_ptr + inode_num;

    uint8_t buf[4];

    read_data(inode_num, 0, (uint8_t*) &buf, 4);

    if (buf[0] != 0x7f || buf[1] != 0x45 || buf[2] != 0x4c || buf[3] != 0x46) {
        return -1;
    }

    read_data(inode_num, 0, (uint8_t*) 0x08048000, temp_inode_ptr->length);

    // memcpy((uint32_t*) 0x848000 + (num_proc * 0x400000), &buf, temp_inode_ptr->length);

    return 0;
}

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

    uint32_t f_pos = pcb_ptr[curr_proc]->f_array[fd].f_pos;
    
    // checks if reached end of directory
    if (f_pos == boot_block_ptr->num_dentries) {
        return 0;
    // checks if we are reading a file
    } else {
        read_dentry_by_index(f_pos, &dentry);
        pcb_ptr[curr_proc]->f_array[fd].f_pos += 1;
    }

    // adds file name to buffer
    for (i = 0; i < FILE_NAME_LENGTH; i++) {
        *((uint8_t*) buf + i) = dentry.f_name[i];
    }
    // fixes smiley face at end of long file name
    // *((uint8_t *) buf + FILE_NAME_LENGTH) = '\0';

    // printf("%d", strlen((int8_t*) &dentry.f_name));

    // printf("%x", dentry.f_name[33]);

    uint32_t str_len = strlen((int8_t*) dentry.f_name);

    if (str_len > (uint32_t) 32) {
        str_len = 32;
    }

    // return strlen((int8_t*) dentry.f_name);
    return str_len;
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
    uint32_t i;
    dentry_t dentry;
    // printf("num_read: %d\n", num_read);

    // find dentry based on inode
    for (i = 0; i < boot_block_ptr->num_dentries; i++) {
        if (boot_block_ptr->dentries[i].inode_num == pcb_ptr[curr_proc]->f_array[fd].inode) {
            break;
        }
    }

    // fill dentry based on file
    read_dentry_by_index(i, &dentry);

    int32_t num_bytes_read;

    num_bytes_read = read_data(dentry.inode_num, pcb_ptr[curr_proc]->f_array[fd].f_pos, (uint8_t*) buf, nbytes);

    pcb_ptr[curr_proc]->f_array[fd].f_pos += num_bytes_read;

    // fill buffer with data
    return num_bytes_read;
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
    int i;
    dentry_t* temp_dentry;
    f_op_tbl_t temp_f_op_tbl;

    for (i = 0; i < MAX_FD; i++) {
        if (pcb_ptr[curr_proc]->f_array[i].flags == 0) {
            break;
        }
    }

    read_dentry_by_name(filename, temp_dentry);

    temp_f_op_tbl.open = &file_open;
    temp_f_op_tbl.read = &file_read;
    temp_f_op_tbl.write = &file_write;
    temp_f_op_tbl.close = &file_close;

    pcb_ptr[curr_proc]->f_array[i].f_op_tbl_ptr = &temp_f_op_tbl;
    pcb_ptr[curr_proc]->f_array[i].f_pos = 0;
    pcb_ptr[curr_proc]->f_array[i].flags = 1;
    pcb_ptr[curr_proc]->f_array[i].inode = temp_dentry->inode_num;
    
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
    pcb_ptr[curr_proc]->f_array[fd].f_op_tbl_ptr = NULL;
    pcb_ptr[curr_proc]->f_array[fd].f_pos = NULL;
    pcb_ptr[curr_proc]->f_array[fd].flags = 0;
    pcb_ptr[curr_proc]->f_array[fd].inode = NULL;
    
    return 0;
}
