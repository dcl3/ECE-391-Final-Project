#include "rtc.h"
#include "lib.h"



/* 
 * rtc_init
 *   DESCRIPTION: initialize the rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
void rtc_init(void){
    /* To turn on the periodic interrupt */
    cli();                                      // diable interrupts ...
    outb(RTC_REGISTER_B, RTC_IDX_PORT);		    // select register B, and disable NMI
    char prev = inb(RTC_DATA_PORT);	            // read the current value of register B
    outb(RTC_REGISTER_B, RTC_IDX_PORT);		    // set the index again (a read will reset the index to register D)
    outb(prev | RTC_BIT_6, RTC_DATA_PORT);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();                                      // enable interrupts...

    /* Turning on IRQ 8 */
    enable_irq(RTC_IRQ_NUMBER);

    /* "Changing the output divider changes the interrupt rate, without interfering with the RTC's ability to keep proper time." */
    // char rate = RTC_DEFAULT_RATE;               // rate must be above 2 and not over 15
    char rate = RTC_SLOWEST_RATE;               // 15 slowest, and 3 fastest
    
    rate &= RTC_RATE_MASK;			
    cli();                                      // disable interrupts ...
    outb(RTC_REGISTER_A, RTC_IDX_PORT);		    // set index to register A, disable NMI
    prev = inb(RTC_DATA_PORT);	                // get initial value of register A
    outb(RTC_REGISTER_A, RTC_IDX_PORT);		    // reset index to A
    outb((prev & RTC_REG_A_MASK) | rate, RTC_DATA_PORT);    // write only our rate to A. Note, rate is the bottom 4 bits.
    sti();                                      // enable interrupts ...

}
    


/* 
 * rtc_handler
 *   DESCRIPTION: handler for rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
void rtc_handler(void){
    /* What is important is that if register C is not read after an IRQ 8, then the interrupt will not happen again*/
    outb(RTC_REGISTER_C, RTC_IDX_PORT);     // select register C
    inb(RTC_DATA_PORT);                     // just throw away contents

    test_interrupts();                      // given func to test if it works 

    send_eoi(RTC_IRQ_NUMBER);               // send eoi of irq 8
}

