#include "rtc.h"
#include "lib.h"


// decleared global varibales:

volatile int RTC_INTERRUPT = 0;                 // 0 - no interrupts, 1 - some interrupts

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

    // rtc_set_freq(15);

    RTC_INTERRUPT = 0;
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

    // test_interrupts();                      // given func to test if it works 
    RTC_INTERRUPT = 1;

    send_eoi(RTC_IRQ_NUMBER);               // send eoi of irq 8
}



/* 
 * rtc_open
 *   DESCRIPTION: initializes RTC frequency to 2HZ（when rate = 15). freq = 32768 >> (rate-1) = 32768 >> (15-1)=2
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
int rtc_open(void){
    int freq = RTC_2Hz_FREQ;                // set the frequency to the corresponding value as explained in hte Description
    rtc_set_freq(freq);

    return 0;                               // return 0 for success
}


/* 
 * rtc_close
 *   DESCRIPTION: probably does nothing, unless you virtualize RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
int rtc_close(void){
    // does nothing and return 0
    return 0;
}


/* 
 * rtc_read
 *   DESCRIPTION: should block until the next interrupt.
 *              must only return once the RTC interrupt occurs. You might want to use some sort of 
 *              flag here (you will not need spinlocks).
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
int rtc_read(void){
    while (RTC_INTERRUPT == 1)
    {
        /* do nothing and wait for it */
    }

    RTC_INTERRUPT = 0;

    return 0;
}


/* 
 * rtc_write
 *   DESCRIPTION: must be able to change frequency, and frequencies must be power of 2.
 *                  must get its input parameter through a buffer and not read the value directly.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, 
 *                 -1 for fail
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
int rtc_write(int freq){
    if(!(((freq & (freq-1)) == 0) && (freq > 0))){      // check if the freq is power of 2
        return -1;                                      // if not, return -1
    }
    int valid_freq[15] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
    char rate;
    int i;
    for(i =0; i < 15; i++){
        if(valid_freq[i] == freq){
            rate = i + 1;
        }
    }
     
    if(rate <= 2 && rate > 15){
        return -1;
    }
    rate &= RTC_RATE_MASK;		

    cli();                                      // disable interrupts ...
    outb(RTC_REGISTER_A, RTC_IDX_PORT);		    // set index to register A, disable NMI
    char prev = inb(RTC_DATA_PORT);	            // get initial value of register A
    outb(RTC_REGISTER_A, RTC_IDX_PORT);		    // reset index to A
    outb((prev & RTC_REG_A_MASK) | rate, RTC_DATA_PORT);    // write only our rate to A. Note, rate is the bottom 4 bits.
    sti(); 

    return 0;
}


/* 
 * rtc_set_freq
 *   DESCRIPTION: "Changing the output divider changes the interrupt rate, without interfering with the RTC's ability to keep proper time."
 *   INPUTS: freq - the frequency value that we want to set to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   REFERENCE:
 *      https://wiki.osdev.org/RTC
 */
void rtc_set_freq(int freq){
    // char rate = RTC_DEFAULT_RATE;               // rate must be above 2 and not over 15
    // char rate = RTC_SLOWEST_RATE;               // 15 slowest, and 3 fastest, set for demo purposes
    char rate = (char)(freq & 0xff);            // Use bitwise AND to get the lowest 8 bits of integer
    
    rate &= RTC_RATE_MASK;			
    cli();                                      // disable interrupts ...
    outb(RTC_REGISTER_A, RTC_IDX_PORT);		    // set index to register A, disable NMI
    char prev = inb(RTC_DATA_PORT);	                // get initial value of register A
    outb(RTC_REGISTER_A, RTC_IDX_PORT);		    // reset index to A
    outb((prev & RTC_REG_A_MASK) | rate, RTC_DATA_PORT);    // write only our rate to A. Note, rate is the bottom 4 bits.
    sti();                                      // enable interrupts ...
}
