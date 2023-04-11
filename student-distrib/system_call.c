#include "system_call.h"
#include "paging.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_link.h"
#include "x86_desc.h"
#include "terminal.h"

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

    pcb_t* temp_pcb = (pcb_t*)(0x0800000 - ((num_processes) * 2 * FOUR_KB));

    // fill in pcb
    temp_pcb->id = num_processes - 1;
    temp_pcb->active = 1;

    // fill in file array
    temp_pcb->f_array[2].inode = dentry.inode_num;
    temp_pcb->f_array[2].f_pos = 0;
    temp_pcb->f_array[2].flags = 1;

    f_op_tbl_t temp_f_op_tbl;

    temp_f_op_tbl.open = &terminal_open;
    temp_f_op_tbl.read = &terminal_read;
    temp_f_op_tbl.write = &terminal_write;
    temp_f_op_tbl.close = &terminal_close;

    temp_pcb->f_array[0].f_op_tbl_ptr = &temp_f_op_tbl;
    temp_pcb->f_array[0].flags = 1;
    temp_pcb->f_array[1].f_op_tbl_ptr = &temp_f_op_tbl;
    temp_pcb->f_array[1].flags = 1;

    // if (dentry.f_type == 0) {
    //     f_op_tbl_t temp_f_op_tbl;

    //     temp_f_op_tbl.open = &rtc_open;
    //     temp_f_op_tbl.read = &rtc_read;
    //     temp_f_op_tbl.write = &rtc_write;
    //     temp_f_op_tbl.close = &rtc_close;

    //     temp_pcb->f_array[2].f_op_tbl_ptr = &temp_f_op_tbl;
    // } else if (dentry.f_type == 1) {
    //     f_op_tbl_t temp_f_op_tbl;

    //     temp_f_op_tbl.open = &dir_open;
    //     temp_f_op_tbl.read = &dir_read;
    //     temp_f_op_tbl.write = &dir_write;
    //     temp_f_op_tbl.close = &dir_close;

    //     temp_pcb->f_array[2].f_op_tbl_ptr = &temp_f_op_tbl;
    // } else if (dentry.f_type == 2) {
    //     f_op_tbl_t temp_f_op_tbl;

    //     temp_f_op_tbl.open = &file_open;
    //     temp_f_op_tbl.read = &file_read;
    //     temp_f_op_tbl.write = &file_write;
    //     temp_f_op_tbl.close = &file_close;

    //     temp_pcb->f_array[2].f_op_tbl_ptr = &temp_f_op_tbl;
    // } else {
    //     return -1;
    // }

    load_user(num_processes);
    flush_tlb();
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    temp_pcb->saved_ebp = saved_ebp;
    temp_pcb->saved_esp = saved_esp;

    load_program(dentry.inode_num, num_processes);

    test_user_function = 0x08048000;

    // int i;
    // for (i = 0; i < 4; i++) {
    //     printf("magic num %s", *((uint32_t*) test_user_function + i));
    // }

    uint8_t* eip_arg_ptr = (uint8_t*) test_user_function + 24;
    eip_arg = *((uint32_t*)eip_arg_ptr);
    eip_arg = eip_arg;
    user_cs_arg = USER_CS;
    esp_arg = 0x08000000 + FOUR_MB - 4;
    user_ds_arg = USER_DS;

    tss.esp0 = 0x0800000 - ((num_processes - 1) * MAX_FD * 0x400) - 4;
    tss.ss0 = KERNEL_DS;

    pcb_ptr[num_processes - 1] = temp_pcb;

    // cli();

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

