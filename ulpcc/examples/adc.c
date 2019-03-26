/* ULP Example: using ADC in deep sleep

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This file contains assembly code which runs on the ULP.

   ULP wakes up to run this code at a certain period, determined by the values
   in SENS_ULP_CP_SLEEP_CYCx_REG registers. On each wake up, the program
   measures input voltage on the given ADC channel 'adc_oversampling_factor'
   times. Measurements are accumulated and average value is calculated.
   Average value is compared to the two thresholds: 'low_thr' and 'high_thr'.
   If the value is less than 'low_thr' or more than 'high_thr', ULP wakes up
   the chip from deep sleep.
*/

#include <ulp_c.h>

/* ADC1 channel 6, GPIO34 */
#define adc_channel 6

/* Configure the number of ADC samples to average on each measurement.
    For convenience, make it a power of 2. */
#define adc_oversampling_factor_log 2
#define adc_oversampling_factor (1 << adc_oversampling_factor_log)
	
/* Low threshold of ADC reading.
   Set by the main program. */
unsigned low_thr;

/* High threshold of ADC reading.
   Set by the main program. */
unsigned high_thr;

/* Counter of measurements done */
unsigned sample_counter;

unsigned last_result;

void entry()
{
    unsigned i, acc = 0;
	
	sample_counter++;
	
    for (i = 0; i < adc_oversampling_factor; i++) {
        /* measure ADC and add value to accumulator */
        acc += adc(0, adc_channel + 1);
    }

    /* take average */
    acc >>= adc_oversampling_factor_log;
    last_result = acc;

    if (acc < low_thr || acc > high_thr)
    {
        if (READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP) & 1)
        {
            wake();
            WRITE_RTC_FIELD(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN, 0);
        }
    }
}
