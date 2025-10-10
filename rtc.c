/* rtc.c - Chris Young 2025
 * This is largely based on https://www.elite.uk.com/mike/posts/2025-08_Time_Machine/
 */

#include <time.h>
#include <stdio.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>

#include "mktime.h"
#include "rtc.h"

static struct esx_drvapi rtc;
static struct dos_tm nrtc;

time_t rtc_get_time(void)
{
   unsigned char ret;
   struct tm rtc_tm;

   rtc.call.function=0;
   rtc.call.driver = 0;
   ret = esx_m_drvapi(&rtc);
   //printf("drvapi: %u\n", (unsigned int)(ret));

   nrtc.time=rtc.de;
   nrtc.date=rtc.bc;

   if((unsigned int)(nrtc.date) == 0) {
      return 0;
   }

   tm_from_dostm(&rtc_tm, &nrtc);

   rtc_tm.tm_sec = (rtc.hl >> 8);

   // l is 1/100 s

   //printf("%04u-%02u-%02u %02u:%02u:%02u", 1900+tm.tm_year, 1+ tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

   return(mktime(&rtc_tm));
}
