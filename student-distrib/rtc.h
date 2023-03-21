#include "types.h"
#include "i8259.h"

#define RTC_IRQ_NUMBER      8

#define RTC_IDX_PORT        0x70
#define RTC_DATA_PORT       0x71

#define RTC_REGISTER_A      0x8A
#define RTC_REGISTER_B      0x8B
#define RTC_REGISTER_C      0x8C

#define RTC_DEFAULT_RATE    0x06


/* initialize the rtc processor*/
void rtc_init(void);

/* set up the rtc handler*/
void rtc_handler(void);

