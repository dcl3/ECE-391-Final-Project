#ifndef SYSTEMCALL_LINK_H
#define SYSTEMCALL_LINK_H

#include "types.h"

extern void systemcall_handler();

extern void jump_usermode();

extern void flush_tlb();

// extern void halt_return(uint32_t halt_return_ptr, uint32_t esp, uint32_t ebp);

extern void halt_return();

extern void terminal_switch_return();

uint32_t test_user_function;
uint32_t eip_arg;
uint32_t user_cs_arg;
uint32_t esp_arg;
uint32_t user_ds_arg;

uint32_t halt_jump_ptr_arg;
uint32_t halt_esp_arg;
uint32_t halt_ebp_arg;

uint32_t terminal_switch_jump_ptr_arg;
uint32_t terminal_switch_esp_arg;
uint32_t terminal_switch_ebp_arg;

#endif
