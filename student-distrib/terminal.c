
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

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

    sti();

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
