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
#include "cursor.h"

void terminal_init() {
    // set up 3 processes (shell x3)
    dentry_t dentry;
    read_dentry_by_name((const uint8_t*) "shell", &dentry);

    int i;
    for (i = 0; i < MAX_TERMINALS; i++) {
        num_processes += 1;
        curr_proc = num_processes - 1;

        pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - ((num_processes) * EIGHT_KB));

        register uint32_t saved_ebp asm("ebp");
        register uint32_t saved_esp asm("esp");
        temp_pcb->saved_ebp = saved_ebp;
        temp_pcb->saved_esp = saved_esp;

        temp_pcb->parent_id = -1;
        temp_pcb->id = curr_proc;

        temp_pcb->active = 1;

        // initialize file array properties
        int j;
        for (j = 0 ; j < MAX_FD ; j++) {
            temp_pcb->f_array[j].flags = 0;
            temp_pcb->f_array[j].f_pos = 0;
            temp_pcb->f_array[j].inode = 0;
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

        temp_pcb->tss_esp0 = EIGHT_MB - ((curr_proc) * EIGHT_KB) - FOUR_B;

        pcb_ptr[curr_proc] = temp_pcb;

        // store
        memcpy((uint8_t*) VIDEO_ADDR + ((i + 1) * FOUR_KB), (uint8_t*) VIDEO_ADDR, FOUR_KB);
    }

    // set up 3 terminals as shells as active processes
    for (i = 0; i < MAX_TERMINALS; i++) {
        terminals[i].active = 0;
        terminals[i].active_proc_id = i;
        terminals[curr_terminal_id].switch_flag = 0;

        int j;
        for (j = 0; j < KEYBOARD_BUFFER_SIZE; j++) {
            terminals[i].kb_buffer[j] = kb_buffer[j];
        }

        terminals[i].saved_ebp = pcb_ptr[terminals[i].active_proc_id]->saved_ebp;
        terminals[i].saved_esp = pcb_ptr[terminals[i].active_proc_id]->saved_esp;

        terminals[i].screen_x = 0;
        terminals[i].screen_y = 0;
    }

    // jump into first
    curr_proc = 0;
    curr_terminal_id = 0;
    terminals[curr_terminal_id].active = 1;
    terminals[curr_terminal_id].switch_flag = 1;

    load_user(0);
    flush_tlb();

    // turn off the user mode
    test_user_function = USER_MODE_OFF;

    uint8_t* eip_arg_ptr = (uint8_t*) test_user_function + EIP_OFF;
    eip_arg = *((uint32_t*) eip_arg_ptr);
    user_cs_arg = USER_CS;
    esp_arg = USER_MODE + FOUR_MB - FOUR_B;
    user_ds_arg = USER_DS;

    tss.esp0 = EIGHT_MB - ((curr_proc) * EIGHT_KB) - FOUR_B;
    tss.ss0 = KERNEL_DS;


    // jump to usermode
    jump_usermode();
}

void terminal_switch(uint32_t terminal_id) {
    
    if(terminal_id == curr_terminal_id)
        return;

    // printf("switch: %d", terminal_id);
    pcb_ptr[curr_proc]->active = 0;
    terminals[curr_terminal_id].active = 0;

    int i;
    for (i = 0; i < KEYBOARD_BUFFER_SIZE; i++) {
        terminals[curr_terminal_id].kb_buffer[i] = kb_buffer[i];
    }

    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    terminals[curr_terminal_id].saved_ebp = saved_ebp;
    terminals[curr_terminal_id].saved_esp = saved_esp;

    // terminals[curr_terminal_id]->screen_x = screen_x;
    // terminals[curr_terminal_id]->screen_y = screen_y;

    // store
    memcpy((uint8_t*) VIDEO_ADDR + ((curr_terminal_id + 1) * FOUR_KB), (uint8_t*) VIDEO_ADDR, FOUR_KB);

    // restore
    memcpy((uint8_t*) VIDEO_ADDR, (uint8_t*) VIDEO_ADDR + ((terminal_id + 1) * FOUR_KB), FOUR_KB);

    curr_terminal_id = terminal_id;

    terminals[curr_terminal_id].active = 1;
    curr_proc = terminals[curr_terminal_id].active_proc_id;

    for (i = 0; i < KEYBOARD_BUFFER_SIZE; i++) {
        kb_buffer[i] = terminals[curr_terminal_id].kb_buffer[i];
    }

    terminal_coordinate(terminals[curr_terminal_id].screen_x, terminals[curr_terminal_id].screen_y);
    update_cursor(terminals[curr_terminal_id].screen_x, terminals[curr_terminal_id].screen_y);

    // set tss for parent
    tss.esp0 = EIGHT_MB - ((curr_proc) * EIGHT_KB) - FOUR_B;
    tss.ss0 = KERNEL_DS;

    load_user(curr_proc);
    flush_tlb();

    // mark the pcb active
    pcb_ptr[curr_proc]->active = 1;
    terminals[curr_terminal_id].active_proc_id = curr_proc;

    if(!(terminals[curr_terminal_id].switch_flag)){
        terminals[curr_terminal_id].switch_flag = 1;

        uint8_t* eip_arg_ptr = (uint8_t*) USER_MODE_OFF + EIP_OFF;
        eip_arg = *((uint32_t*) eip_arg_ptr);
        user_cs_arg = USER_CS;
        esp_arg = USER_MODE + FOUR_MB - FOUR_B;
        user_ds_arg = USER_DS;
        
        jump_usermode();
        return;
    }

    terminal_switch_esp_arg = terminals[curr_terminal_id].saved_esp;
    terminal_switch_ebp_arg = terminals[curr_terminal_id].saved_ebp;

    terminal_switch_jump_ptr_arg = halt_jump_ptr;

    terminal_switch_return();
    return;
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
