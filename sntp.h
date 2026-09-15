/* sntp.h - Chris Young 2025 */

#ifndef __NEWT_SNTP_H
#define __NEWT_SNTP_H 1

#define SNTP_OFFSET_GUESS -99

void sntp_get(unsigned char *server, bool rtc, int32_t offset);
#endif
