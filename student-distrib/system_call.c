#include "system_call.h"
#include "paging.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_link.h"

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_halt(uint8_t status){
    return -1;
}

/* 
 * system_execute
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_execute(const uint8_t* command){
    // paging setup
    num_processes += 1;
    load_user(num_processes);

    // parse cmd
    dentry_t* dentry;
    if (read_dentry_by_name(command, dentry) == -1) {
        return -1;
    };

    // fill in pcb
    pcb[num_processes - 1].id = num_processes - 1;
    pcb[num_processes - 1].active = 1;

    // fill in file array
    pcb[num_processes - 1].f_array.inode = dentry->inode_num;
    pcb[num_processes - 1].f_array.f_pos = 0;
    pcb[num_processes - 1].f_array.flags = 1;

    if (dentry->f_type == 0) {
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->open = (int32_t*) &rtc_open;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->read = (int32_t*) &rtc_read;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->write = (int32_t*) &rtc_write;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->close = (int32_t*) &rtc_close;
    } else if (dentry->f_type == 1) {
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->open = (int32_t*) &dir_open;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->read = (int32_t*) &dir_read;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->write = (int32_t*) &dir_write;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->close = (int32_t*) &dir_close;
    } else if (dentry->f_type == 2) {
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->open = (int32_t*) &file_open;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->read = (int32_t*) &file_read;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->write = (int32_t*) &file_write;
        pcb[num_processes - 1].f_array.f_op_tbl_ptr->close = (int32_t*) &file_close;
    } else {
        return -1;
    }

    load_program(dentry->inode_num, num_processes);

    register uint32_t test_user_function asm("eip");

    // jump to usermode
    jump_usermode();

    return 0;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_open(const uint8_t* filename){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_close(int32_t fd){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_getargs(uint8_t* buf, int32_t nbytes){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_vidmap(uint8_t** screen_start){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_set_handler(int32_t signum, void* handler_address){
    return -1;
}

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_sigreturn(void){
    return -1;
}

