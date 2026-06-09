/* newt by Chris Youbg 2026 */

/* time.c */
#include <stdio.h>
#include <sys/time.h>

#include "c_gmtime.h"

void time_print(time_t s)
{
	struct tm tms;

	mini_gmtime_r((int32_t)s, &tms);
	printf("%02u/%02u/%04u %02u:%02u:%02u\n", tms.tm_mday, 1+tms.tm_mon, 1900+tms.tm_year, tms.tm_hour, tms.tm_min, tms.tm_sec);

}

