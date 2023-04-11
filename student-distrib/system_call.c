#include "system_call.h"
#include "paging.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_link.h"
#include "x86_desc.h"
#include "terminal.h"
#include "types.h"
#include "lib.h"

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
    int cmd_idx = 0;
    int arg_idx = 0;

    uint8_t cmd[MAX_F_NAME_LENGTH] = { '\0' };
    uint8_t arg[MAX_F_NAME_LENGTH] = { '\0' };
    cli();
    // parse cmd
    if (command == NULL)
        return -1;
    int i;
    for(i = 0 ; i < strlen((const int8_t*) command) ; i++){
        if(command[i] == ' '){
            if(cmd_idx > 0)
                break;
        }
        else{
            cmd[cmd_idx] = command[i];
            cmd_idx++;
        }
    }

    for(; i < strlen((const int8_t*) command) ; i++){
         if(command[i] == ' '){
            if(arg_idx > 0)
                break;
        }
        else{
            arg[arg_idx] = command[i];
            arg_idx++;
        }
    }

    dentry_t dentry;
    if (read_dentry_by_name(cmd, &dentry) == -1) {
        return -1;
    };
    // paging setup
    num_processes += 1;

    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - ((num_processes) * EIGHT_KB));

    // fill in pcb
    temp_pcb->id = num_processes - 1;
    temp_pcb->active = 1;

    // fill in file array
    // temp_pcb->f_array[2].inode = dentry.inode_num;
    // temp_pcb->f_array[2].f_pos = 0;
    // temp_pcb->f_array[2].flags = 1;

    f_op_tbl_t temp_f_op_tbl;

    for(i = 0 ; i < MAX_FD ; i++){
        temp_pcb->f_array[i].flags = 0;
        temp_pcb->f_array[i].f_pos = 0;
        temp_pcb->f_array[i].inode = 0;
    }

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

    test_user_function = USER_MODE_OFF;

    // int i;
    // for (i = 0; i < 4; i++) {
    //     printf("magic num %s", *((uint32_t*) test_user_function + i));
    // }

    uint8_t* eip_arg_ptr = (uint8_t*) test_user_function + EIP_OFF;
    eip_arg = *((uint32_t*)eip_arg_ptr);
    user_cs_arg = USER_CS;
    esp_arg = USER_MODE + FOUR_MB - FOUR_B;
    user_ds_arg = USER_DS;

    tss.esp0 = EIGHT_MB - ((num_processes - 1) * EIGHT_KB) - FOUR_B;
    tss.ss0 = KERNEL_DS;

    pcb_ptr[num_processes - 1] = temp_pcb;

    sti();

    // jump to usermode
    jump_usermode();
    // asm volatile("movw %%bx, %%ds     \n\
    //     pushl %%ebx         \n\
    //     pushl %%edx         \n\
    //     pushfl              \n\
    //     popl %%edx          \n\
    //     orl $0x0200, %%edx  \n\
    //     pushl %%edx         \n\
    //     pushl %%ecx         \n\
    //     pushl %%eax         \n\
    //     iret                \n\
    //     "
    //     :
    //     : "a"(USER_DS), "b"(esp_arg), "c"(USER_CS), "d"(eip_arg)
    //     : "memory"
    // );

    return 0;
}

/* 
 * system_read
 *   DESCRIPTION: The read system call reads data from the keyboard, a file, device (RTC), or directory. 
 *   INPUTS: fd - file descriptor
*           buf - buffer to read into
*           nbytes - number of bytes to read
 *   OUTPUTS: number of bytes read
 *   RETURN VALUE: -1 for fail, otherwise success
 *   REFERENCE: ECE391 MP3 Documentation
 */
int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes){
    pcb_t* pcb;
    // check if the any of the inputs are valid
    if(fd < 0 || buf == NULL || nbytes < 0){
       return -1;
    }

    return (pcb[num_processes].f_array[fd]).f_op_tbl_ptr->read(fd, buf, nbytes);
}

/* 
 * system_write
 *   DESCRIPTION: The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should 
 * be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
 * integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly.
 *   INPUTS: fd - file descriptor
*           buf - buffer to read into
*           nbytes - number of bytes to read
 *   OUTPUTS: number of bytes write
 *   RETURN VALUE: -1 for fail, otherwise success
 *   REFERENCE: ECE391 MP3 Documentation
 */
int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes){
    pcb_t* pcb;
    // check if the any of the inputs are valid
    if(fd < 0 || buf == NULL || nbytes < 0){
       return -1;
    }
    
    return (pcb[num_processes].f_array[fd]).f_op_tbl_ptr->write(fd, buf, nbytes);
}

/* 
 * system_open
 *   DESCRIPTION: The open system call provides access to the file system.
 *   INPUTS: filename - name of file to open
 *   OUTPUTS: none
 *   RETURN VALUE:-1 for fail, otherwise success
 *   REFERENCE: ECE391 MP3 Documentation 
 */
int32_t syscall_open(const uint8_t* filename){
    // find the directory entry corresponding to the named file
    if(filename == NULL){
        return -1;
    }
    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) == -1){
        return -1;
    }

    // allocate an unused file descriptor
    int curr = -1;
    int i; 
    for(i = 0; i < 8; i++){
        if( pcb_ptr[i]->active == 0){
            curr = i;
        }
    }
    if(curr == -1){
        return -1;
    }
    int j;
    for(j = 0; j < 8; j++){
        if(pcb_ptr[curr]->f_array[j].flags == 0){
            pcb_ptr[curr]->f_array[j].flags = 1; 
            pcb_ptr[curr]->f_array[j].inode = dentry.inode_num;
            pcb_ptr[curr]->f_array[j].f_pos = 0;
            break;
        }
    }
    // and set up any data necessary to handle the given type of file (directory, RTC device, or regular file).
    if ((dentry.f_type) == 0){
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &rtc_open;
        temp_f_op_tbl.read = &rtc_read;
        temp_f_op_tbl.write = &rtc_write;
        temp_f_op_tbl.close = &rtc_close;

        pcb_ptr[curr]->f_array[j].f_op_tbl_ptr = &temp_f_op_tbl;
    }
    else if ((dentry.f_type) == 1){
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &dir_open;
        temp_f_op_tbl.read = &dir_read;
        temp_f_op_tbl.write = &dir_write;
        temp_f_op_tbl.close = &dir_close;

        pcb_ptr[curr]->f_array[j].f_op_tbl_ptr = &temp_f_op_tbl;
    }
    else if ((dentry.f_type) == 2){
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &file_open;
        temp_f_op_tbl.read = &file_read;
        temp_f_op_tbl.write = &file_write;
        temp_f_op_tbl.close = &file_close;

        pcb_ptr[curr]->f_array[j].f_op_tbl_ptr = &temp_f_op_tbl;
    }
    else{
        return -1;
    }

    return curr;
}

/* 
 * system_close
 *   DESCRIPTION: The close system call closes the specified file descriptor and makes it available for return from later calls to open.
 *   INPUTS: fd - file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: -1 for fail, 0 for success
 *   REFERENCE: ECE391 MP3 Documentation
 */
int32_t syscall_close(int32_t fd){
    pcb_t* pcb;
    if(pcb[num_processes].f_array[fd].flags == 0) return -1;

    pcb[num_processes].f_array[fd].flags = 0;
    pcb[num_processes].f_array[fd].f_pos = 0;
    pcb[num_processes].f_array[fd].inode = 0;

    pcb[num_processes].f_array[fd].f_op_tbl_ptr->close(fd);

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

