/* net.h - Chris Young 2025 */

#ifndef __ZXNET_NET_H
#define __ZXNET_NET_H 1

#include <stdbool.h>

#define BUFFER_SIZE 64

void net_send(unsigned char cmd[]);
void net_send_wait(unsigned char cmd[]);
unsigned int net_recv_line(unsigned char *buffer, unsigned int buffer_size);
unsigned int net_recv_stripped(unsigned char *result, unsigned int result_size, unsigned char strip[]);

/* Resolves "hostname" to "ip"
 * ip - user buffer
 * returns true on success
 */
bool net_lookup(unsigned char *hostname, unsigned char *ip, unsigned int ip_size);

#endif
