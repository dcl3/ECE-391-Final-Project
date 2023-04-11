#include "system_call.h"
#include "paging.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_link.h"
#include "x86_desc.h"

uint8_t num_processes = 0;

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

    // parse cmd
    dentry_t dentry;
    if (read_dentry_by_name(command, &dentry) == -1) {
        return -1;
    };

    // fill in pcb
    pcb[num_processes - 1].id = num_processes - 1;
    pcb[num_processes - 1].active = 1;

    // fill in file array
    pcb[num_processes - 1].f_array[2].inode = dentry.inode_num;
    pcb[num_processes - 1].f_array[2].f_pos = 0;
    pcb[num_processes - 1].f_array[2].flags = 1;

    if (dentry.f_type == 0) {
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &rtc_open;
        temp_f_op_tbl.read = &rtc_read;
        temp_f_op_tbl.write = &rtc_write;
        temp_f_op_tbl.close = &rtc_close;

        pcb[num_processes - 1].f_array[2].f_op_tbl_ptr = &temp_f_op_tbl;
    } else if (dentry.f_type == 1) {
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &dir_open;
        temp_f_op_tbl.read = &dir_read;
        temp_f_op_tbl.write = &dir_write;
        temp_f_op_tbl.close = &dir_close;

        pcb[num_processes - 1].f_array[2].f_op_tbl_ptr = &temp_f_op_tbl;
    } else if (dentry.f_type == 2) {
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &file_open;
        temp_f_op_tbl.read = &file_read;
        temp_f_op_tbl.write = &file_write;
        temp_f_op_tbl.close = &file_close;

        pcb[num_processes - 1].f_array[2].f_op_tbl_ptr = &temp_f_op_tbl;
    } else {
        return -1;
    }

    load_user(num_processes);
    flush_tlb();
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    pcb[num_processes - 1].saved_ebp = saved_ebp;
    pcb[num_processes - 1].saved_esp = saved_esp;

    load_program(dentry.inode_num, num_processes);

    test_user_function = 0x08048000;

    uint8_t* eip_arg_ptr = (uint8_t*) test_user_function + 24;
    eip_arg = *((uint32_t*)eip_arg_ptr);
    eip_arg = eip_arg;
    user_cs_arg = USER_CS;
    esp_arg = 0x08000000 + FOUR_MB - sizeof(uint32_t);
    user_ds_arg = USER_DS;

    tss.esp0 = (2 * FOUR_MB) - ((2 * FOUR_KB) * (num_processes - 1)) - sizeof(uint32_t);
    tss.ss0 = KERNEL_DS;

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

