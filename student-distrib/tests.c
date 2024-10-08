#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "filesystem.h"
#include "terminal.h"
#include "system_call.h"
#include "paging.h"
#include "task_struct.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Division Test
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int divison_test(){
	TEST_HEADER;
    int test = 0;
	int result = PASS;

    test = 0 / test;

	return result;
}

/* System Call Test
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int system_call_test(){
	TEST_HEADER;

	int result = PASS;

    asm volatile("int $0x80");

	return result;
}

/* Page Fault Test
 * 
 * Check if dereferencing 0 causes a page fault
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging
 * Files: paging_asm.S/paging.c/h
 */
int page_fault_test(){
	TEST_HEADER;
	int result = PASS;
	int *test;
	int *i = 0;

    memcpy(test, i, 4);


	return result;
}

/* Directory Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: dir_read, dir_write, dir_open, dir_close
 * Files: filesystem.c/h
 */
int dir_read_test(){
	TEST_HEADER;

	int32_t fd = 0;
    uint8_t buf[32];
    int32_t nbytes = 32;

    const uint8_t* filename;

    dir_open(filename);
    dir_write(fd, buf, nbytes);

    // printf("start\n");
    while (dir_read(fd, &buf, nbytes) != 0) {
        printf("filename: %s\n", buf);
    }

    dir_close(fd);

	return PASS;
}

/* Small File Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_read for small file, file_write, file_open, file_close
 * Files: filesystem.c/h
 */
int small_file_read_test(){
	TEST_HEADER;
	int result = PASS;

	int32_t fd = 10; // frame0.txt
    uint8_t buf[200];
    int32_t nbytes = 200;

    const uint8_t* filename;

    file_open(filename);
    file_write(fd, buf, nbytes);

    // printf("start\n");
    file_read(fd, &buf, nbytes);

    printf("%s\n", buf);

    file_close(fd);

	return result;
}

/* Executable File Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_read for executable file, file_write, file_open, file_close
 * Files: filesystem.c/h
 */
int exec_file_read_test(){
	TEST_HEADER;
	int result = PASS;
    int i;

	int32_t fd = 3; // grep
    uint8_t buf[6149]; // length of grep file
    int32_t nbytes = 6149;

    const uint8_t* filename;

    file_open(filename);
    file_write(fd, buf, nbytes);

    // printf("start\n");
    i = file_read(fd, &buf, nbytes);

    // printf("return val: %d\n", i);

    // printf("%s\n", buf);

    // only print first few and last few (ELF and magic number)
    for (i = 0; i < 6149; i++) {
        if (i < 50 || i > 6100) {
            putc(buf[i]);
        }
    }

    printf("\n");

    file_close(fd);

	return result;
}

/* File Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_read for large file, file_write, file_open, file_close
 * Files: filesystem.c/h
 */
int large_file_read_test(){
	TEST_HEADER;
	int result = PASS;

	int32_t fd = 11; // verylargetextwithverylongname.tx(t)
    uint8_t buf[10000];
    int32_t nbytes = 10000;

    const uint8_t* filename;

    file_open(filename);
    file_write(fd, buf, nbytes);

    // printf("start\n");
    file_read(fd, &buf, nbytes);

    printf("%s\n", buf);

    file_close(fd);

	return result;
}

// add more tests here

/* Checkpoint 2 tests */
// int rtc_test(){
// 	TEST_HEADER;

// 	/* check open works */
// 	if(rtc_open() != 0){
// 		return FAIL;
// 	}

// 	/* interates through valid frequencies */
// 	int valid_freq[15] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
// 	int i;
// 	int sec;
// 	for(i = 14; i >= 0; i--){
// 		clear();
// 		sec = (32768 >> (i-1));

// 		/* check if write works */
// 		if(rtc_write(valid_freq[i]) == -1){
// 			break;
// 		}

// 		/* check if read works*/
// 		int count = 0;
// 		while (sec != 0){
// 			if (count >= 2048){
// 				putc('x');
// 				break;
// 			}
// 			if (rtc_read() == 0){
// 				putc('i');
// 				count++;
// 			}
// 			sec--;
// 			// count++;
// 		}
		
// 	}

// 	clear();

// 	/* return Pass if successful */
// 	return PASS;
// }

/* Terminal Read Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read
 * Files: terminal.c/h
 */
int terminal_read_test(){
	TEST_HEADER;
	int result = PASS;

	int32_t fd;
    uint8_t buf[128];
    int32_t nbytes = 128;

    kb_buffer_index = 4;
    kb_buffer[0] = 't';
    kb_buffer[1] = 'e';
    kb_buffer[2] = 's';
    kb_buffer[3] = 't';
    // ...

    keyboard_flag = 1;
    terminal_read(fd, buf, nbytes);

    printf("%s\n", buf);

	return result;
}

/* Terminal Write Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_write
 * Files: terminal.c/h
 */
int terminal_write_test(){
	TEST_HEADER;
	int result = PASS;

	int32_t fd;
    uint8_t buf[20] = "test";
    int32_t nbytes = 20;

    terminal_write(fd, buf, nbytes);

    // printf("%s\n", buf);

	return result;
}

/* Checkpoint 3 tests */

/* Systhem Call Write Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_write
 * Files: terminal.c/h
 */
int system_call_open_test(){
    TEST_HEADER;
    int result = PASS;

    pcb_t* temp_pcb = (pcb_t*)(0x0800000 - ((1) * 2 * FOUR_KB));

    // fill in pcb
    temp_pcb->id = 0;
    temp_pcb->active = 0;

    f_op_tbl_t temp_f_op_tbl;
    int i;
    for(i = 0 ; i < MAX_FD ; i++){
        temp_pcb->f_array[i].flags = 0;
        temp_pcb->f_array[i].f_pos = 0;
        temp_pcb->f_array[i].inode = 0;
    }

    temp_f_op_tbl.open = &terminal_open;
    temp_f_op_tbl.read = &terminal_read;
    temp_f_op_tbl.write = &terminal_write;
    temp_f_op_tbl.close = &terminal_close;

    temp_pcb->f_array[0].f_op_tbl_ptr = &temp_f_op_tbl;
    temp_pcb->f_array[0].flags = 1;
    temp_pcb->f_array[1].f_op_tbl_ptr = &temp_f_op_tbl;
    temp_pcb->f_array[1].flags = 1;

    pcb_ptr[0] = temp_pcb;

    // (const uint8_t) f_name = "frame0.txt";
    int32_t sys_open = syscall_open((const uint8_t*)"frame0.txt");
    if (sys_open == -1) result = FAIL;
    
    uint8_t buf[40];
    int32_t nbytes = 40;

    if (syscall_read(sys_open, buf, nbytes) == -1) result = FAIL;

    printf("%s", buf);

    if (syscall_write(sys_open, buf, nbytes) == -1) result = FAIL;
    return result;
}

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
	// TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("divison_test", divison_test());
    // TEST_OUTPUT("system_call_test", system_call_test());
	// launch your tests here

	// TEST_OUTPUT("rtc_test", rtc_test());
    // TEST_OUTPUT("dir_read_test", dir_read_test());
    // TEST_OUTPUT("small_file_read_test", small_file_read_test());
    // TEST_OUTPUT("exec_file_read_test", exec_file_read_test());
    // TEST_OUTPUT("large_file_read_test", large_file_read_test());
    // TEST_OUTPUT("terminal_read_test", terminal_read_test());
    // TEST_OUTPUT("terminal_write_test", terminal_write_test());
    TEST_OUTPUT("system_call_open_test", system_call_open_test());
}
