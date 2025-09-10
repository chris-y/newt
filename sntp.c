/* sntp.c - Chris Young 2025 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "c_gmtime.h"
#include "error.h"
#include "net.h"
#include "timer.h"

struct ntp_pkt {
	uint8_t li_vn_mode;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	uint32_t root_delay;
	uint32_t root_disp;
	uint32_t reference_id;
	uint32_t reference_time_s;
	uint32_t reference_time_f;
	uint32_t originate_time_s;
	uint32_t originate_time_f;
	uint32_t receive_time_s;
	uint32_t receive_time_f;
	uint32_t transmit_time_s;
	uint32_t transmit_time_f;
};

#define GET_NTP_LI(PKT) ((PKT->li_vn_mode) >> 6) & 0x3
#define GET_NTP_VN(PKT) ((PKT->li_vn_mode) >> 3) & 0x7
#define GET_NTP_MODE(PKT) ((PKT->li_vn_mode)) & 0x7

#define SET_NTP_VN(PKT,VN) PKT->li_vn_mode |= ((VN & 0x7) << 3)
#define SET_NTP_MODE(PKT,MODE) PKT->li_vn_mode |= ((MODE & 0x7))


#define SWAP_ENDIAN(VAL) (((0xFF000000 & VAL) >> 24) | ((0x00FF0000 & VAL) >> 8) | \
						((0x0000FF00 & VAL) << 8) | ((0x000000FF & VAL) << 24))

#define NTP_TO_UNIX_EPOCH(E) (SWAP_ENDIAN(E)-2208988800L)

#define ZX_TIMER_NTP_S SWAP_ENDIAN((uint32_t)(ZX_TIMER / 50))
#define NANO_TO_ZX_TIMER(N) (uint32_t)((uint64_t)(N*50)/10^9)

/* Convert the current time to ZX timer */
#define TIME_TO_ZX_TIMER(H,M,S) (((H*60*60)+(M*60)+(S))*50)

/* Convert to NTP fraction - this is in 1/50ths */
#define ZX_TIMER_NTP_F SWAP_ENDIAN((uint32_t)(((uint64_t)(ZX_TIMER % 50) << 32) / 50))
#define NTP_F_TO_ZX_TIMER(F) (uint32_t)(((uint64_t)SWAP_ENDIAN(F) * 50) >> 32)

#ifdef CALC_RT
#define NTP_F_TO_NANO(F) (uint32_t)(((uint64_t)SWAP_ENDIAN(F) * 10^9 ) >> 32)

struct timespec *sntp_rt_delay(struct ntp_pkt *pkt)
{
	struct timespec o_ts;
	struct timespec r_ts;
	struct timespec t_ts;
	struct timespec d_ts;
	struct timespec *ts = malloc(sizeof(struct timespec));

	if(ts == NULL) exit((int)err_mem);

	d_ts.tv_sec = (uint32_t)(ZX_TIMER / 50);
	d_ts.tv_nsec = (uint32_t)(((ZX_TIMER % 50) * 10e9) / 50);

	o_ts.tv_sec = NTP_TO_UNIX_EPOCH(pkt->originate_time_s);
	o_ts.tv_nsec = NTP_F_TO_NANO(pkt->originate_time_f);

	r_ts.tv_sec = NTP_TO_UNIX_EPOCH(pkt->receive_time_s);
	r_ts.tv_nsec = NTP_F_TO_NANO(pkt->receive_time_f);

	t_ts.tv_sec = NTP_TO_UNIX_EPOCH(pkt->transmit_time_s);
	t_ts.tv_nsec = NTP_F_TO_NANO(pkt->transmit_time_f);

	//((d - o) - (t - r)) / 2
	timersub(&d_ts, &o_ts, &d_ts);
	timersub(&t_ts, &r_ts, &t_ts);
	timersub(&d_ts, &t_ts, ts);

	ts->tv_sec /= 2;
	ts->tv_nsec /= 2;
	return ts;

}
#endif

void sntp_sync(void)
{
	struct ntp_pkt *pkt = calloc(sizeof(struct ntp_pkt), 1);
	unsigned char *rpkt = (unsigned char *)pkt;
	struct tm tms;

	if(pkt == NULL) exit((int)err_mem);
	
	SET_NTP_VN(pkt, 4); /* Version 4 */
	SET_NTP_MODE(pkt, 3); /* Client */

	pkt->transmit_time_s = ZX_TIMER_NTP_S;
	pkt->transmit_time_f = ZX_TIMER_NTP_F;
	printf("ZX timer: %u.%lu\n", ZX_TIMER / 50, ZX_TIMER % 50);

	net_send_data(pkt, sizeof(struct ntp_pkt));
	
	net_recv_data(pkt, sizeof(struct ntp_pkt));
	
	printf("pkt recvd:\n");
	for(unsigned int i = 0; i < sizeof(struct ntp_pkt); i++) {
		printf("%x ", rpkt[i]);
	}

	printf("\nrxtime: %lu\n", SWAP_ENDIAN(pkt->receive_time_s));
	printf("txtime: %lu\n", SWAP_ENDIAN(pkt->transmit_time_s));
	printf("stratum: %u\n", pkt->stratum);
	printf("ZX time: %u.%lu\n", ZX_TIMER / 50, ZX_TIMER % 50);

	if(pkt->stratum == 0) { // Kiss-o'-Death
		unsigned char *kod = (unsigned char *)&pkt->reference_id;
		printf("Kiss-o'-Death: %c%c%c%c\n", kod[0], kod[1], kod[3], kod[4]);
		exit(0);
	}

	mini_gmtime_r((int32_t)NTP_TO_UNIX_EPOCH(pkt->transmit_time_s), &tms);
	printf("%04u-%02u-%02u %02u:%02u:%02u\n", 1900+tms.tm_year, 1+ tms.tm_mon, tms.tm_mday, tms.tm_hour, tms.tm_min, tms.tm_sec);

	uint32_t zxtime = TIME_TO_ZX_TIMER(tms.tm_hour, tms.tm_min, tms.tm_sec) + NTP_F_TO_ZX_TIMER(pkt->transmit_time_f);

	printf("zx: %u\n", zxtime);

#ifdef CALC_RT
	struct timespec *ts = sntp_rt_delay(pkt);

	zxtime += ts->tv_sec * 50;
	zxtime += NANO_TO_ZX_TIMER(ts->tv_nsec);

	printf("rt: %u.%u\n", ts->tv_sec, ts->tv_nsec);
#endif

	zx_timer_set(zxtime);

	free(pkt);
}

void sntp_test(void)
{
	char ip[32];
	unsigned int conn = 0;
	
	/* ensure no open connections */
	net_close();
	
	if(net_lookup("uk.pool.ntp.org", ip, 32)) {
		if(net_connect_udp(ip, 123)) {
				sntp_sync();
			net_close();
		}
	}
}

