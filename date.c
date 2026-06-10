/* date.c - Chris Young 2026 */

#include <string.h>
#include <stdio.h>
#include <time.h>

static int str_to_num(char **str, char del)
{
	char *p = *str;
	int t[3];
	int i = 0;
	int n = 0;

	if(p == NULL) return 0;

    while ((*p != del) && (*p != '\0')) {
		t[i] = *p - '0';
		i++;
		p++;
		
		if(i >= 3) return 0;
	}
	
	for(int x=0; x < i; x++) {
		n = (n * 10) + t[x];
	}
	
	p++;
	*str = p;
	return n;
}

void datetime_to_tm(char *date, char *time, struct tm *tms)
{
	char *p = date;
	int t[3];
	int i = 0;
	int day = 0;
	int mon = 0;
	int yr = 0;
	int h = 0;
	int m = 0;
	int s = 0;

	day = str_to_num(&p, '/');
	mon = str_to_num(&p, '/');
	yr = str_to_num(&p, '/');
	
	tms->tm_mday = day;
	tms->tm_mon = mon - 1;
	tms->tm_year = yr + 100; /* accept dates 2000-2099 */
	
	p = time;
	
	h = str_to_num(&p, ':');
	m = str_to_num(&p, ':');
	s = str_to_num(&p, ':');

	tms->tm_hour = h;
	tms->tm_min = m;
	tms->tm_sec = s;
}

#if 0
void main(void)
{
	struct tm tms;
	char *date = "01/02/2003";
	datetime_to_tm(date, date, &tms);
}
#endif
