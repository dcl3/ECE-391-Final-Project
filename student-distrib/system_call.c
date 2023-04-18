#include "system_call.h"
#include "paging.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_link.h"
#include "x86_desc.h"
#include "terminal.h"
#include "lib.h"
#include "task_struct.h"

/* 
 * system_halt
 *   DESCRIPTION: 
 *   INPUTS: 
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_halt(uint8_t status){
    // get parent process
    uint32_t parent_id = pcb_ptr[curr_proc]->parent_id;

    // set inactive
    pcb_ptr[curr_proc]->active = 0;

    // close files
    int i;
    for (i = 0; i < MAX_FD; i++) {
        pcb_ptr[curr_proc]->f_array->f_op_tbl_ptr = NULL;
        pcb_ptr[curr_proc]->f_array->f_pos = NULL;
        pcb_ptr[curr_proc]->f_array->flags = NULL;
        pcb_ptr[curr_proc]->f_array->inode = NULL;
    }

    // set tss for parent
    tss.esp0 = EIGHT_MB - ((parent_id) * EIGHT_KB) - FOUR_B;
    tss.ss0 = KERNEL_DS;

    // map user page
    load_user(parent_id);
    flush_tlb();

    pcb_ptr[parent_id]->active = 1;

    halt_jump_ptr_arg = halt_jump_ptr;
    halt_esp_arg = pcb_ptr[curr_proc]->saved_esp;
    // halt_esp_arg = 0x7fffcc;
    halt_ebp_arg = pcb_ptr[curr_proc]->saved_ebp;

    // decrease number of processes
    num_processes -= 1;
    curr_proc = parent_id;

    // halt_return((uint32_t) halt_jump_ptr, pcb_ptr[parent_id]->saved_esp, pcb_ptr[parent_id]->saved_ebp);

    halt_status = (uint32_t) status;

    halt_return();

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
    if (command == NULL) {
        sti();
        return -1;
    }

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
        sti();
        return -1;
    };

    num_processes += 1;
    curr_proc = num_processes - 1;

    strcpy(pcb_ptr[curr_proc]->args, arg);

    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - ((num_processes) * EIGHT_KB));

    // set up ebp and esp property
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    temp_pcb->saved_ebp = saved_ebp;
    temp_pcb->saved_esp = saved_esp;

    if (num_processes == 1) {
        temp_pcb->parent_id = -1;
        temp_pcb->id = curr_proc;
    } else {
        temp_pcb->parent_id = pcb_ptr[curr_proc - 1]->id;
        temp_pcb->id = curr_proc;
    }

    // fill in pcb
    temp_pcb->active = 1;

    f_op_tbl_t temp_f_op_tbl;

    // initialize file array properties
    for(i = 0 ; i < MAX_FD ; i++){
        temp_pcb->f_array[i].flags = 0;
        temp_pcb->f_array[i].f_pos = 0;
        temp_pcb->f_array[i].inode = 0;
    }

    // function pointers fot stdin and stdout
    temp_f_op_tbl.open = &terminal_open;
    temp_f_op_tbl.read = &terminal_read;
    temp_f_op_tbl.write = &terminal_write;
    temp_f_op_tbl.close = &terminal_close;

    // fill in file array for stdin and stdout
    temp_pcb->f_array[0].f_op_tbl_ptr = &temp_f_op_tbl;
    temp_pcb->f_array[0].flags = 1;
    temp_pcb->f_array[1].f_op_tbl_ptr = &temp_f_op_tbl;
    temp_pcb->f_array[1].flags = 1;

    load_user(curr_proc);
    flush_tlb();

    load_program(dentry.inode_num, num_processes);

    test_user_function = USER_MODE_OFF;

    // int i;
    // for (i = 0; i < 4; i++) {
    //     printf("magic num %s", *((uint32_t*) test_user_function + i));
    // }

    uint8_t* eip_arg_ptr = (uint8_t*) test_user_function + EIP_OFF;
    eip_arg = *((uint32_t*) eip_arg_ptr);
    user_cs_arg = USER_CS;
    esp_arg = USER_MODE + FOUR_MB - FOUR_B;
    user_ds_arg = USER_DS;

    // // set up ebp and esp property
    // register uint32_t saved_ebp asm("ebp");
    // register uint32_t saved_esp asm("esp");
    // temp_pcb->saved_ebp = saved_ebp;
    // temp_pcb->saved_esp = saved_esp;

    tss.esp0 = EIGHT_MB - ((curr_proc) * EIGHT_KB) - FOUR_B;
    temp_pcb->tss_esp0 = tss.esp0;
    tss.ss0 = KERNEL_DS;

    pcb_ptr[curr_proc] = temp_pcb;

    halt_jump_ptr = (uint32_t) &&halt_jump;

    sti();

    // jump to usermode
    jump_usermode();

    halt_jump:

    return halt_status;
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
    // check if the any of the inputs are valid
    if(fd < 0 || buf == NULL || nbytes < 0){
       return -1;
    }

    return (pcb_ptr[curr_proc]->f_array[fd]).f_op_tbl_ptr->read(fd, buf, nbytes);
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
    // check if the any of the inputs are valid
    if(fd < 0 || fd > 8 || buf == NULL || nbytes < 0){
       return -1;
    }
    
    return pcb_ptr[curr_proc]->f_array[fd].f_op_tbl_ptr->write(fd, buf, nbytes);
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
            break;
        }
    }
    if(curr == -1){
        return -1;
    }
    int j;
    for(j = 0; j < 8; j++){
        if(pcb_ptr[curr_proc]->f_array[j].flags == 0){
            pcb_ptr[curr_proc]->f_array[j].flags = 1; 
            pcb_ptr[curr_proc]->f_array[j].inode = dentry.inode_num;
            pcb_ptr[curr_proc]->f_array[j].f_pos = 0;
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

        pcb_ptr[curr_proc]->f_array[j].f_op_tbl_ptr = &temp_f_op_tbl;
    }
    else if ((dentry.f_type) == 1){
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &dir_open;
        temp_f_op_tbl.read = &dir_read;
        temp_f_op_tbl.write = &dir_write;
        temp_f_op_tbl.close = &dir_close;

        pcb_ptr[curr_proc]->f_array[j].f_op_tbl_ptr = &temp_f_op_tbl;
    }
    else if ((dentry.f_type) == 2){
        f_op_tbl_t temp_f_op_tbl;

        temp_f_op_tbl.open = &file_open;
        temp_f_op_tbl.read = &file_read;
        temp_f_op_tbl.write = &file_write;
        temp_f_op_tbl.close = &file_close;

        pcb_ptr[curr_proc]->f_array[j].f_op_tbl_ptr = &temp_f_op_tbl;
    }
    else{
        return -1;
    }

    return j;
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
    if(pcb_ptr[num_processes]->f_array[fd].flags == 0) return -1;

    pcb_ptr[num_processes]->f_array[fd].flags = 0;
    pcb_ptr[num_processes]->f_array[fd].f_pos = 0;
    pcb_ptr[num_processes]->f_array[fd].inode = 0;

    pcb_ptr[num_processes]->f_array[fd].f_op_tbl_ptr->close(fd);

    return 0;
}

/* 
 * system_getargs
 *   DESCRIPTION: reads the program’s command line arguments into a user-level buffer.
 *   INPUTS: buf-
 *           nbytes-
 *   OUTPUTS: 
 *   RETURN VALUE: 
 *   REFERENCE:
 */
int32_t syscall_getargs(uint8_t buf, int32_t nbytes){
    // if there are no arguments
    if(buf == NULL) return -1;

    if(pcb_ptr[curr_proc]->args[0] == '\0') return -1;

    // copy the current pcb commands
    strncpy(buf, pcb_ptr[curr_proc]->args, nbytes);

    // return 0 if success 
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

