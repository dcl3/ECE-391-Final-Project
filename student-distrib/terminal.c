
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "system_call.h"
#include "filesystem.h"
#include "lib.c"
#include "paging.c"
#include "task_struct.h"
#include "systemcall_link.h"

void terminal_init() {
    // set up 3 processes (shell x3)
    dentry_t dentry;
    read_dentry_by_name((const uint8_t*) "shell", &dentry);

    int i;
    for (i = 0; i < MAX_TERMINALS; i++) {
        num_processes += 1;
        curr_proc = num_processes - 1;

        pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - ((num_processes) * EIGHT_KB));\

        register uint32_t saved_ebp asm("ebp");
        register uint32_t saved_esp asm("esp");
        temp_pcb->saved_ebp = saved_ebp;
        temp_pcb->saved_esp = saved_esp;

        temp_pcb->parent_id = -1;
        temp_pcb->id = curr_proc;

        temp_pcb->active = 1;

        // initialize file array properties
        for (i = 0 ; i < MAX_FD ; i++) {
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

        load_program(dentry.inode_num, num_processes);

        test_user_function = USER_MODE_OFF;
    }

    // set up 3 terminals as shells as active processes

    // jump into first

    curr_terminal_id = 0;

    syscall_execute("shell");



    terminal_switch(1);

    syscall_execute("shell");

    terminal_switch(2);

    syscall_execute("shell");

    terminal_switch(2);

}

void terminal_switch(uint32_t terminal_id) {
    terminals[curr_terminal_id]->active = 0;

    terminals[curr_terminal_id]->active_proc_id = curr_proc;

    int i;
    for (i = 0; i < KEYBOARD_BUFFER_SIZE; i++) {
        terminals[curr_terminal_id]->kb_buffer[i] = kb_buffer[i];
    }

    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    terminals[curr_terminal_id]->saved_ebp = saved_ebp;
    terminals[curr_terminal_id]->saved_esp = saved_esp;

    terminals[curr_terminal_id]->screen_x = screen_x;
    terminals[curr_terminal_id]->screen_y = screen_y;

    memcpy(VIDEO + ((curr_terminal_id + 1) * FOUR_KB), VIDEO, FOUR_KB);

    

}

/*
*  terminal_open: opens the terminal
*  Inputs: filename - name of file to open
*  Outputs: -1
*/
int32_t terminal_open (const uint8_t* filename){
    // do nothing
    return -1;
}

/*
*   terminal_close: closes the terminal
*   Inputs: fd - file descriptor
*   Outputs: -1
*/
int32_t terminal_close (int32_t fd){
    // do nothing
    return -1;
}

/*
*   terminal_read: reads from the keyboard buffer
*   Inputs: fd - file descriptor
*           buf - buffer to read into
*           nbytes - number of bytes to read
*   Outputs: number of bytes read
*   Side Effects: none    
*/
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
    if (buf==NULL){return -1;}
    if (nbytes < 0){return -1;}

    // sti();

    while(keyboard_flag == 0){}

    cli();

    int i;

    if(nbytes < KEYBOARD_BUFFER_SIZE){
        for(i = 0; i < nbytes; i++){
            ((char*) buf)[i] = kb_buffer[i];
            kb_buffer[i] = ' ';
            if(((char*) buf)[i] == '\n'){
                i++;
                break;
            }
        }
    }
    else{
        for(i = 0; i < KEYBOARD_BUFFER_SIZE; i++){
            ((char*) buf)[i] = kb_buffer[i];
            kb_buffer[i] = ' ';
            if(((char*) buf)[i] == '\n'){
                i++;
                break;
            }
        }
    }
    keyboard_flag = 0;
    kb_buffer_index = 0;
    sti();

    return i;
}

/*
 *  terminal_write: writes to the screen
*  Inputs: fd - file descriptor
*       buf - buffer to write from
*      nbytes - number of bytes to write
* Outputs: number of bytes written
* Side Effects: none
*/
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
    int i;
    for(i = 0; i < nbytes; ++i) {
        putc(((char*) buf)[i]);
    }
    return nbytes;
}
