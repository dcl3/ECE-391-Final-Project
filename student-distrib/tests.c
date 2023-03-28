#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"

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

// add more tests here

/* Checkpoint 2 tests */
int rtc_test(){
	TEST_HEADER;

	if(rtc_open() != 0){
		return FAIL;
	}
	int valid_freq[15] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
	int i;
	int sec;
	for(i = 14; i >= 0; i--){
		clear();
		sec = (32768 >> (i-1));
		if(rtc_write(valid_freq[i]) == -1){
			return FAIL;
		}
		while (sec != 0){
			if (rtc_read() == 0){
				putc('i');
			}
			sec--;
		}
		
	}
	// test_interrupts();

	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
    clear();
	TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("divison_test", divison_test());
    // TEST_OUTPUT("system_call_test", system_call_test());
	// launch your tests here
	TEST_OUTPUT("rtc_test", rtc_test());
}
