/* date.h - Chris Young 2026 */

#ifndef __NEWT_DATE_H
#define __NEWT_DATE_H 1
#include <sys/time.h>

void datetime_to_tm(char *date, char *time, struct tm *tms);
#endif
