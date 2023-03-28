#include "types.h"
#include "i8259.h"

#define RTC_IRQ_NUMBER      8

#define RTC_IDX_PORT        0x70
#define RTC_DATA_PORT       0x71

#define RTC_REGISTER_A      0x8A
#define RTC_REGISTER_B      0x8B
#define RTC_REGISTER_C      0x0C

#define RTC_DEFAULT_RATE    0x06
#define RTC_SLOWEST_RATE    15
#define RTC_BIT_6           0x40
#define RTC_RATE_MASK       0x0F
#define RTC_REG_A_MASK      0xF0

#define RTC_2Hz_FREQ        15



/* initialize the rtc processor*/
void rtc_init(void);

/* set up the rtc handler*/
void rtc_handler(void);

/* */
int rtc_open(void);

/* */
int rtc_close(void);

/* */
int rtc_read(void);

/* */
int rtc_write(int);

/* */
void rtc_set_freq(int);
