#include "types.h"


#define EIGHT_MB 0x0800000
#define EIGHT_KB 0x2000

#define USER_MODE 0x08000000
#define USER_MODE_OFF 0x08048000

#define EIP_OFF 24
#define FOUR_B 4

uint32_t halt_jump_ptr;
uint32_t halt_status;

// reference from appendex B, the ten system calls that we need to implement, numbered 1 through 10
int32_t syscall_halt(uint8_t status);                                        //1
int32_t syscall_execute(const uint8_t* command);                             //2
int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes);                 //3
int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes);          //4
int32_t syscall_open(const uint8_t* filename);                               //6
int32_t syscall_close(int32_t fd);                                           //5
int32_t syscall_getargs(uint8_t* buf, int32_t nbytes);                       //7
int32_t syscall_vidmap(uint8_t** screen_start);                              //8
int32_t syscall_set_handler(int32_t signum, void* handler_address);          //9
int32_t syscall_sigreturn(void);                                             //10
