#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"

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
 * Coverage: dir_read
 * Files: filesystem.c/h
 */
int dir_read_test(){
	TEST_HEADER;

	int32_t fd = 0;
    uint8_t buf[32];
    int32_t nbytes = 32;

    // printf("start\n");
    while (dir_read(fd, &buf, nbytes) != 0) {
        printf("filename: %s\n", buf);
    }

	return PASS;
}

/* Small File Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_read for small file
 * Files: filesystem.c/h
 */
int small_file_read_test(){
	TEST_HEADER;
	int result = PASS;

	int32_t fd = 10; // frame0.txt
    uint8_t buf[200];
    int32_t nbytes = 200;

    // printf("start\n");
    file_read(fd, &buf, nbytes);

    printf("%s\n", buf);

	return result;
}

/* Executable File Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_read for executable file
 * Files: filesystem.c/h
 */
int exec_file_read_test(){
	TEST_HEADER;
	int result = PASS;
    int i;

	int32_t fd = 3; // grep
    uint8_t buf[6149]; // length of grep file
    int32_t nbytes = 6149;

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

	return result;
}

/* File Read Test
 * 
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: file_read for large file
 * Files: filesystem.c/h
 */
int large_file_read_test(){
	TEST_HEADER;
	int result = PASS;

	int32_t fd = 11; // verylargetextwithverylongname.tx(t)
    uint8_t buf[10000];
    int32_t nbytes = 10000;

    // printf("start\n");
    file_read(fd, &buf, nbytes);

    printf("%s\n", buf);

	return result;
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
	// TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("divison_test", divison_test());
    // TEST_OUTPUT("system_call_test", system_call_test());
	// launch your tests here
    // TEST_OUTPUT("page_fault_test", page_fault_test());
    // TEST_OUTPUT("dir_read_test", dir_read_test());
    // TEST_OUTPUT("small_file_read_test", small_file_read_test());
    // TEST_OUTPUT("exec_file_read_test", exec_file_read_test());
    TEST_OUTPUT("large_file_read_test", large_file_read_test());
}
