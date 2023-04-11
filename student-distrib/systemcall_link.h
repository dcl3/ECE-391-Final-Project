#ifndef SYSTEMCALL_LINK_H
#define SYSTEMCALL_LINK_H

#include "system_call.h"

extern void systemcall_handler();

extern void jump_usermode();

extern void flush_tlb();

uint32_t test_user_function;

uint32_t eip_arg;

uint32_t user_cs_arg;

uint32_t esp_arg;

uint32_t user_ds_arg;

#endif
