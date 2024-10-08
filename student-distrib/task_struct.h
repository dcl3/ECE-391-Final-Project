/* task_struct.h - Defines used in tasks
 * vim:ts=4 noexpandtab
 */
#ifndef _task_struct_H
#define _task_struct_H

#include "types.h"

#define MAX_FD 8
#define MIN_FD 2
#define MAX_TASK 6
#define MAX_F_NAME_LENGTH 32

typedef struct f_op_tbl {
    int32_t (*open)(const uint8_t* filename);
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close) (int32_t fd);
} f_op_tbl_t;

typedef struct f_array {
    f_op_tbl_t* f_op_tbl_ptr;
    uint32_t inode;
    uint32_t f_pos;
    uint32_t flags;
} f_array_t;

typedef struct pcb {
    uint32_t id;
    uint32_t parent_id;
    f_array_t f_array[MAX_FD];
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint32_t active;
    uint32_t tss_esp0;
    uint8_t args[MAX_F_NAME_LENGTH];
} pcb_t;

pcb_t* pcb_ptr[MAX_TASK];

f_op_tbl_t f_op_tbl_stdin;
f_op_tbl_t f_op_tbl_stdout;
f_op_tbl_t f_op_tbl_rtc;
f_op_tbl_t f_op_tbl_dir;
f_op_tbl_t f_op_tbl_file;

#endif /* _task_struct_H */
