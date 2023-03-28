
#include "terminal.h"
#include "lib.h"
#include "keyboard.h"


int32_t terminal_open (const uint8_t* filename){
    // do nothing
    return -1;
}

int32_t terminal_close (int32_t fd){
    // do nothing
    return -1;
}

int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
    if (buf==NULL){return -1;}
    if (nbytes < 0){return -1;}

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

int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
    int i;
    for(i = 0; i < nbytes; ++i) {
        putc(((char*) buf)[i]);
    }
    return nbytes;
}
