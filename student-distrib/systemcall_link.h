#ifndef SYSTEMCALL_LINK_H
#define SYSTEMCALL_LINK_H

#include "system_call.h"

extern void systemcall_handler();

extern void jump_usermode();

uint32_t test_user_function;

#endif