/* Header file for pseudo-functions that the compiler will convert to assembler instructions */

/* This simplifies what the following headers pull in */
#define __ASSEMBLER__

#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc_ulp_c.h"

int  adc(int sar_sel, int mux);
void halt();
void i2c_rd(int sub_addr, int high, int low, int slave_sel);
int  i2c_wr(int sub_addr, int value, int high, int low, int slave_sel);
int  reg_rd(int addr, int high, int low);
void reg_wr(int addr, int high, int low, int data);
void sleep(int sleep_reg);
int  tsens(int wait_delay);
void wait(int cycles);
void wake();
#define wake_when_ready() do {while (0 == (READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP) & 1)); wake(); } while(0)

