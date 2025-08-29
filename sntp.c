/* sntp.c - Chris Young 2025 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

/* TODO: Convert to NTP fraction - this is in 1/50ths */
#define ZX_TIMER_NTP_F (ZX_TIMER % 50)

void sntp_sync(void)
{
	struct ntp_pkt *pkt = calloc(sizeof(struct ntp_pkt), 1);
	unsigned char *rpkt = (unsigned char *)pkt;
	struct tm tms;

	if(pkt == NULL) exit((int)err_mem);
	
	SET_NTP_VN(pkt, 4); /* Version 4 */
	SET_NTP_MODE(pkt, 3); /* Client */

	pkt->originate_time_s = ZX_TIMER_NTP_S;
	
	printf("ZX timer: %u.%lu\n", ZX_TIMER / 50, ZX_TIMER_NTP_F);

	net_send_data(pkt, sizeof(struct ntp_pkt));
	
	net_recv_data(pkt, sizeof(struct ntp_pkt));
	
	printf("pkt recvd:\n");
	for(unsigned int i = 0; i < sizeof(struct ntp_pkt); i++) {
		printf("%x ", rpkt[i]);
	}

	printf("\nrxtime: %lu\n", SWAP_ENDIAN(pkt->receive_time_s));
	printf("txtime: %lu\n", SWAP_ENDIAN(pkt->transmit_time_s));
	printf("stratum: %u\n", pkt->stratum);
	printf("ZX time: %u.%lu\n", ZX_TIMER / 50, ZX_TIMER_NTP_F);

	mini_gmtime_r((int32_t)NTP_TO_UNIX_EPOCH(pkt->transmit_time_s), &tms);
	printf("%04u-%02u-%02u %02u:%02u:%02u\n", 1900+tms.tm_year, 1+ tms.tm_mon, tms.tm_mday, tms.tm_hour, tms.tm_min, tms.tm_sec);


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

