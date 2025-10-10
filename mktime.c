/* mktime.c - modified by Chris Young 2025 for z88dk */

/*
 * mktime.c
 * Original Author:     G. Haley
 *
 * Converts the broken-down time, expressed as local time, in the structure
 * pointed to by tim_p into a calendar time value. The original values of the
 * tm_wday and tm_yday fields of the structure are ignored, and the original
 * values of the other fields have no restrictions. On successful completion
 * the fields of the structure are set to represent the specified calendar
 * time. Returns the specified calendar time. If the calendar time can not be
 * represented, returns the value (time_t) -1.
 *
 * Modifications:       Fixed tm_isdst usage - 27 August 2008 Craig Howland.
 */

/*
FUNCTION
<<mktime>>---convert time to arithmetic representation

INDEX
        mktime

SYNOPSIS
        #include <time.h>
        time_t mktime(struct tm *<[timp]>);

DESCRIPTION
<<mktime>> assumes the time at <[timp]> is a local time, and converts
its representation from the traditional representation defined by
<<struct tm>> into a representation suitable for arithmetic.

<<localtime>> is the inverse of <<mktime>>.

RETURNS
If the contents of the structure at <[timp]> do not form a valid
calendar time representation, the result is <<-1>>.  Otherwise, the
result is the time, converted to a <<time_t>> value.

PORTABILITY
ANSI C requires <<mktime>>.

<<mktime>> requires no supporting OS subroutines.
*/

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define _SEC_IN_MINUTE 60L
#define _SEC_IN_HOUR 3600L
#define _SEC_IN_DAY 86400L

static const int DAYS_IN_MONTH[12] =
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define _DAYS_IN_MONTH(x) ((x == 1) ? days_in_feb : DAYS_IN_MONTH[x])

static const int _DAYS_BEFORE_MONTH[12] =
{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define _ISLEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || (((y)+1900) % 400) == 0))
#define _DAYS_IN_YEAR(year) (_ISLEAP(year) ? 366 : 365)

time_t mktime (struct tm *tim_p)
{
  time_t tim = 0;
  long days = 0;
  int year, isdst=0;

  /* compute hours, minutes, seconds */
  tim += tim_p->tm_sec + (tim_p->tm_min * _SEC_IN_MINUTE) +
    (tim_p->tm_hour * _SEC_IN_HOUR);

  /* compute days in year */
  days += tim_p->tm_mday - 1;
  days += _DAYS_BEFORE_MONTH[tim_p->tm_mon];
  if (tim_p->tm_mon > 1 && _DAYS_IN_YEAR (tim_p->tm_year) == 366)
    days++;

  /* compute day of the year */
  tim_p->tm_yday = days;

  if (tim_p->tm_year > 10000 || tim_p->tm_year < -10000)
      return (time_t) -1;

  /* compute days in other years */
  if ((year = tim_p->tm_year) > 70)
    {
      for (year = 70; year < tim_p->tm_year; year++)
        days += _DAYS_IN_YEAR (year);
    }
  else if (year < 70)
    {
      for (year = 69; year > tim_p->tm_year; year--)
        days -= _DAYS_IN_YEAR (year);
      days -= _DAYS_IN_YEAR (year);
    }

  /* compute total seconds */
  tim += (time_t)days * _SEC_IN_DAY;
  
  return(tim);
}
