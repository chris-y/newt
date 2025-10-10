#ifndef __NEWT_MKTIME_H
#define __NEWT_MKTIME_H 1

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

time_t mktime (struct tm *tim_p);
#endif
