/* This is a C version of the assembly example shipped in the ESP IDF at $IDF_PATH/examples/system/ulp */
#include "ulp_c.h"

int next_edge, debounce_counter, debounce_max_count, edge_count, edge_count_to_wake_up, io_number;

void entry()
{ 
	int rtc;
	if (io_number >= 16)
	{
		/* read_io_high */
		rtc = READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S + 16, 2) >> (io_number - 16);
	}
	else
	{
		rtc = READ_RTC_REG(RTC_GPIO_IN_REG, RTC_GPIO_IN_NEXT_S, 16) >> io_number;
	}

	/* read_done */
	if (((rtc & 1) + next_edge) & 1)
		debounce_counter = debounce_max_count;
	else
	{
		/* changed */
		if (debounce_counter)
			debounce_counter--;
		else
		{
			/* edge_detected */
			debounce_counter = debounce_max_count;
			next_edge = !next_edge;
			if (++edge_count == edge_count_to_wake_up)
				wake_when_ready();
		}
	}
}


