#include "system_call.h"
#include "paging.h"
#include "filesystem.h"
#include "rtc.h"
#include "systemcall_link.h"
#include "x86_desc.h"
#include "terminal.h"
#include "lib.h"
#include "task_struct.h"
#include "idt.h"

/* 
 * system_halt
 *   DESCRIPTION: e halt system call terminates a process, returning the specified value to its parent process.
 *   INPUTS: status
 *   OUTPUTS: None
 *   RETURN VALUE: This call should never return to the caller (-1 if successful).
 *   REFERENCE: MP3 Documentation
 */
int32_t syscall_halt(uint8_t status){
    // get parent process
    uint32_t parent_id = pcb_ptr[curr_proc]->parent_id;

    // set inactive
    pcb_ptr[curr_proc]->active = 0;

    // close files
    int i;
    for (i = 0; i < MAX_FD; i++) {
        pcb_ptr[curr_proc]->f_array[i].f_op_tbl_ptr = NULL;
        pcb_ptr[curr_proc]->f_array[i].f_pos = NULL;
        pcb_ptr[curr_proc]->f_array[i].flags = NULL;
        pcb_ptr[curr_proc]->f_array[i].inode = NULL;
    }

    // restore the information of that particular process
    if (parent_id == -1) {
        num_processes -= 1;
        curr_proc = parent_id;

        syscall_execute((const uint8_t*) "shell");
    }

    // set tss for parent
    tss.esp0 = EIGHT_MB - ((parent_id) * EIGHT_KB) - FOUR_B;
    tss.ss0 = KERNEL_DS;

    // map user page
    load_user(parent_id);
    flush_tlb();

    // mark the pcb active
    pcb_ptr[parent_id]->active = 1;
    terminals[curr_terminal_id].active_proc_id = parent_id;

    halt_jump_ptr_arg = halt_jump_ptr;
    halt_esp_arg = pcb_ptr[curr_proc]->saved_esp;
    // halt_esp_arg = 0x7fffcc;
    halt_ebp_arg = pcb_ptr[curr_proc]->saved_ebp;

    // decrease number of processes
    num_processes -= 1;
    curr_proc = parent_id;

    // halt_return((uint32_t) halt_jump_ptr, pcb_ptr[parent_id]->saved_esp, pcb_ptr[parent_id]->saved_ebp);

    // if the exception flag is on, turn it off
    if (exception_flag) {
        exception_flag = 0;
    }
    
    // set the halt status
    halt_status = (uint32_t) status;

    // return from halt
    halt_return();

    return -1;
}

/* 
 * system_execute
 *   DESCRIPTION: The execute system call attempts to load and execute a new program, handing off the processor to the new program
 * until it terminates.
 *   INPUTS: command - commands coming from user input
 *   OUTPUTS: based off the commands user give
 *   RETURN VALUE: -1 if unseccusful, otherwise successful
 *   REFERENCE: MP3 Documentation
 */
int32_t syscall_execute(const uint8_t* command){
    // parse the user input into command and argument
    int cmd_idx = 0;
    int arg_idx = 0;

    // if the exceed the max tasks, reutrn fail
    if (num_processes == MAX_TASK) {
        return -1;
    }

    uint8_t cmd[MAX_F_NAME_LENGTH] = { '\0' };
    uint8_t arg[MAX_F_NAME_LENGTH] = { '\0' };
    cli();

    // parse cmd
    if (command == NULL) {
        sti();
        return -1;
    }

    // get the command
    int len = strlen((const int8_t*) command);
    int i;
    for(i = 0 ; i < len ; i++){
        if (i == MAX_F_NAME_LENGTH) {
            sti();
            return -1;
        }
        if(command[i] == ' '){
            if(cmd_idx > 0)
                break;
        }
        else{
            cmd[cmd_idx] = command[i];
            cmd_idx++;
        }
    }

    // get the argument
    for(; i < len ; i++){
        if (i - cmd_idx - 1 == MAX_F_NAME_LENGTH) {
            sti();
            return -1;
        }
        if(command[i] == ' '){
            if(arg_idx > 0)
                break;
        }
        else{
            arg[arg_idx] = command[i];
            arg_idx++;
        }
    }

    // check if is valid command
    dentry_t dentry;
    if (read_dentry_by_name(cmd, &dentry) == -1) {
        sti();
        return -1;
    };

    // update the current pcb
    num_processes += 1;
    for (i = 0; i < MAX_TASK; i++) {
        if (pcb_ptr[i] == NULL) {
            curr_proc = i;
            break;
        } else if (pcb_ptr[i]->active == 0) {
            curr_proc = i;
            break;
        }
    }

    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - ((curr_proc + 1) * EIGHT_KB));

    strcpy((int8_t*)(temp_pcb->args), (int8_t*)arg);

    // set up ebp and esp property
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    temp_pcb->saved_ebp = saved_ebp;
    temp_pcb->saved_esp = saved_esp;

    uint32_t temp_active_proc_id = terminals[curr_terminal_id].active_proc_id;

    // update the parent_id
    // if (num_processes == 1) {
    if (curr_proc == 0 || curr_proc == 1 || curr_proc == 2) {
        temp_pcb->parent_id = -1;
        temp_pcb->id = curr_proc;
    } else {
        // temp_pcb->parent_id = pcb_ptr[curr_proc - 1]->id;
        temp_pcb->parent_id = temp_active_proc_id;
        terminals[curr_terminal_id].active_proc_id = curr_proc;
        temp_pcb->id = curr_proc;
    }

    // fill in pcb
    temp_pcb->active = 1;

    // f_op_tbl_t temp_f_op_tbl;

    // initialize file array properties
    for(i = 0 ; i < MAX_FD ; i++){
        temp_pcb->f_array[i].flags = 0;
        temp_pcb->f_array[i].f_pos = 0;
        temp_pcb->f_array[i].inode = 0;
    }

    // function pointers fot stdin and stdout
    f_op_tbl_stdin.open = &terminal_open;
    f_op_tbl_stdin.read = &terminal_read;
    f_op_tbl_stdin.write = NULL;
    f_op_tbl_stdin.close = &terminal_close;

    f_op_tbl_stdout.open = &terminal_open;
    f_op_tbl_stdout.read = NULL;
    f_op_tbl_stdout.write = &terminal_write;
    f_op_tbl_stdout.close = &terminal_close;

    // fill in file array for stdin and stdout
    temp_pcb->f_array[0].f_op_tbl_ptr = &f_op_tbl_stdin;
    temp_pcb->f_array[0].flags = 1;
    temp_pcb->f_array[1].f_op_tbl_ptr = &f_op_tbl_stdout;
    temp_pcb->f_array[1].flags = 1;

    load_user(curr_proc);
    flush_tlb();

    // check if we load the process correctly
    if (load_program(dentry.inode_num, num_processes) == -1) {
        num_processes -= 1;
        curr_proc = temp_active_proc_id;
        terminals[curr_terminal_id].active_proc_id = temp_active_proc_id;

        load_user(curr_proc);
        flush_tlb();

        sti();
        return -1;
    };

    // turn off the user mode
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
    if(fd < 0 || fd > MAX_FD || buf == NULL || nbytes < 0 || fd == 1){
       return -1;
    }

    // check if we can read from an empty file
    if (pcb_ptr[curr_proc]->f_array[fd].flags == 0) {
        return -1;
    }

    // functionalites for read
    return pcb_ptr[curr_proc]->f_array[fd].f_op_tbl_ptr->read(fd, buf, nbytes);
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
    if(fd < 0 || fd > MAX_FD || buf == NULL || nbytes < 0 || fd == 0){
       return -1;
    }

    // check if we can write from an empty file
    if (pcb_ptr[curr_proc]->f_array[fd].flags == 0) {
        return -1;
    }

    // pcb_t* temp_pcb = pcb_ptr[curr_proc];
    // f_array_t temp_fa = pcb_ptr[curr_proc]->f_array[fd];
    // f_op_tbl_t* temp_fop = pcb_ptr[curr_proc]->f_array[fd].f_op_tbl_ptr;
    
    // perfrom the functionalities for write
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

    // check if the filename is valid
    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) == -1){
        return -1;
    }

    // allocate an unused file descriptor
    // int curr = -1;
    // int i; 
    // for(i = 0; i < 8; i++){
    //     if( pcb_ptr[i]->active == 0){
    //         curr = i;
    //         break;
    //     }
    // }
    // if(curr == -1){
    //     return -1;
    // }
    // setup the pcb with this filename
    int j;
    for(j = 0; j <= MAX_FD; j++){
        if(j == MAX_FD)
            return -1;
        if(pcb_ptr[curr_proc]->f_array[j].flags == 0){
            pcb_ptr[curr_proc]->f_array[j].flags = 1; 
            pcb_ptr[curr_proc]->f_array[j].inode = dentry.inode_num;
            pcb_ptr[curr_proc]->f_array[j].f_pos = 0;
            break;
        }
    }
    // and set up any data necessary to handle the given type of file (directory, RTC device, or regular file).
    if ((dentry.f_type) == 0){
        // f_op_tbl_t temp_f_op_tbl;

        f_op_tbl_rtc.open = &rtc_open;
        f_op_tbl_rtc.read = &rtc_read;
        f_op_tbl_rtc.write = &rtc_write;
        f_op_tbl_rtc.close = &rtc_close;

        pcb_ptr[curr_proc]->f_array[j].f_op_tbl_ptr = &f_op_tbl_rtc;
    }
    else if ((dentry.f_type) == 1){
        // f_op_tbl_t temp_f_op_tbl;

        f_op_tbl_dir.open = &dir_open;
        f_op_tbl_dir.read = &dir_read;
        f_op_tbl_dir.write = &dir_write;
        f_op_tbl_dir.close = &dir_close;

        pcb_ptr[curr_proc]->f_array[j].f_op_tbl_ptr = &f_op_tbl_dir;
    }
    else if ((dentry.f_type) == 2){
        // f_op_tbl_t temp_f_op_tbl;

        f_op_tbl_file.open = &file_open;
        f_op_tbl_file.read = &file_read;
        f_op_tbl_file.write = &file_write;
        f_op_tbl_file.close = &file_close;

        pcb_ptr[curr_proc]->f_array[j].f_op_tbl_ptr = &f_op_tbl_file;
    }
    else{
        return -1;
    }

    // anything other than -1 is success.
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
    // check if fd is within required bond 
    if(fd < MIN_FD || fd > MAX_FD){
       return -1;
    }

    // check is fd is valid
    if (pcb_ptr[curr_proc]->f_array[fd].flags == 0) {
        return -1;
    }
    
    // clean up that pcb
    pcb_ptr[curr_proc]->f_array[fd].f_op_tbl_ptr = NULL;
    pcb_ptr[curr_proc]->f_array[fd].flags = 0;
    pcb_ptr[curr_proc]->f_array[fd].f_pos = NULL;
    pcb_ptr[curr_proc]->f_array[fd].inode = NULL;

    // pcb_ptr[curr_proc]->f_array[fd].f_op_tbl_ptr->close(fd);

    // return successful
    return 0;
}

/* 
 * system_getargs
 *   DESCRIPTION: reads the program’s command line arguments into a user-level buffer.
 *   INPUTS: buf- user level buffer 
 *           nbytes- number of bytes for that command line
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, -1 for fail
 *   REFERENCE: MP3 Documentation
 */
int32_t syscall_getargs(uint8_t* buf, int32_t nbytes){
    // if there are no arguments
    if(buf == NULL) return -1;

    // if argument is invalid
    if(pcb_ptr[curr_proc]->args[0] == '\0') return -1;

    // copy the current pcb commands
    strncpy((int8_t*)buf, (int8_t*)pcb_ptr[curr_proc]->args, nbytes);

    // return 0 if success 
    return 0;
}

/* 
 * syscall_vidmap
 *   DESCRIPTION: The vidmap call maps the text-mode video memory into user space at a pre-set virtual address.
 *   INPUTS: secreen_start - pointer to user space
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for successful, -1 for unseccessful.
 *   REFERENCE: MP3 Documentation
 */
int32_t syscall_vidmap(uint8_t** screen_start){
    // verify location is valid
    if ((uint32_t) screen_start < USER_MODE || (uint32_t) screen_start > (USER_MODE + FOUR_MB)) {
        return -1;
    }
    
    // maps the text-mode video memory into user space
    *screen_start = (uint8_t*) map_user_vid();
    flush_tlb();

    // return success
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

