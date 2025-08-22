#ifndef __NEWT_C_GMTIME_H
#define __NEWT_C_GMTIME_H 1

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

struct tm *mini_gmtime_r(time_t ts, struct tm *tm);
#endif
