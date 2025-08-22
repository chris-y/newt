/* net.c - Chris Young 2025 */

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "main.h"
#include "net.h"
#include "uart.h"

static bool check_end(unsigned char *buffer, unsigned int buffer_size)
{
        if(buffer_size < 7) return false; // just in case
        
        if((strncmp(buffer, "OK\r\n", 4) == 0) ||
                (strncmp(buffer, "ERROR\r\n", 7) == 0))
                return true;
                
        return false;
}

void net_send(unsigned char cmd[])
{
        uart_flush_read_buffer();
	if(verbose) printf(">> %s", cmd);
        uart_tx_string(cmd);
}

static void net_send_bin(unsigned char *data, unsigned int size)
{
	unsigned char buf[BUFFER_SIZE];
	unsigned int bytes;

        uart_flush_read_buffer();
	if(verbose) {
		printf(">> ");
		unsigned int i = size;
		while(i < size) {
			printf("%x ", data[i]);
			i++;
		}
	}
        uart_tx_bin(data, size);

	while(bytes = net_recv_line(buf, BUFFER_SIZE)) {
		if(verbose) printf("\n<< [%u] %s", bytes, buf);
		if(strncmp("SEND OK\r\n", buf, 9) == 0) return;
	}
}

void net_send_wait(unsigned char cmd[])
{
        unsigned int bytes_read = 0;
        unsigned char *buffer = NULL;
        
        buffer = malloc(BUFFER_SIZE);
	if(buffer) {

		net_send(cmd);

		while(1) {
			bytes_read = uart_rx_line(buffer, BUFFER_SIZE);
			if(verbose) printf("<< [%d] %s", bytes_read, buffer);

			// check for errors?
			if(check_end(buffer, BUFFER_SIZE)) break;
		}
		free(buffer);
	} else {
		exit((int)err_mem);
	}   
}

unsigned int net_recv_line(unsigned char *buffer, unsigned int buffer_size)
{
        unsigned int bytes_read = 0;
        if(buffer == NULL) return 0;

        bytes_read = uart_rx_line(buffer, buffer_size);
        if(verbose) printf("<< [%d] %s", bytes_read, buffer);

        // check for errors?
        if(check_end(buffer, buffer_size)) return 0;

        return bytes_read;
}

unsigned int net_recv_stripped(unsigned char *result, unsigned int result_size, unsigned char strip[])
{
        unsigned int bytes_read = 0;
        unsigned char *buffer = NULL;

        buffer = malloc(BUFFER_SIZE);
        if(buffer) {
                while(1) {
                        bytes_read = uart_rx_line(buffer, BUFFER_SIZE);
			if(verbose) printf("<< [%d] %s", bytes_read, buffer);
                        if(strncmp(buffer, strip, strlen(strip)) == 0) {
				strlcpy(result, buffer+strlen(strip), result_size);
				bytes_read -= strlen(strip);
				break;
			}
			// check for errors?
                        if(check_end(buffer, BUFFER_SIZE)) {
                                strlcpy(result, buffer, bytes_read);
				bytes_read = 0;
                                break;
                        }
                }
                free(buffer);
		return(bytes_read);
        } else {
                exit((int)err_mem);
        }
	return 0;
}

bool net_lookup(unsigned char *hostname, unsigned char *ip, unsigned int ip_size)
{
        unsigned char cmd_opt[100] = "AT+CIPDOMAIN=\"";
	unsigned char strip[] = "+CIPDOMAIN:";
	unsigned int bytes = 0;
	
        strcat(cmd_opt, hostname);
        strcat(cmd_opt, "\"\r\n\0");

	net_send(cmd_opt);
        bytes = net_recv_stripped(ip, ip_size, strip);
	
	if(bytes) {
		ip[strlen(ip)-2] = '\0';
		return true;
	} else {
		return false;
	}
}

bool net_connect_udp(unsigned char *ip, unsigned int port)
{
	unsigned char buf[64];
	unsigned int bytes_read;
	
	if(!quiet) printf("Connect to %s:%u...\n", ip, port);
	
	snprintf(buf, 64, "AT+CIPSTART=\"UDP\",\"%s\",%u\r\n", ip, port);
	
	net_send(buf);
	bytes_read = net_recv_line(buf, 64);
	if(bytes_read != 0) {
		if(strncmp(buf, "CONNECT", 7) == 0) {
			return(true);
		} else {
			return(false);
		}
	}
	
	return(false);
}

void net_close(void)
{
	unsigned char at_cipclose[] = "AT+CIPCLOSE\r\n\0";

	net_send_wait(at_cipclose);
}

void net_send_data(unsigned char *data, unsigned int size)
{
	char cmd[20];
	
	snprintf(cmd, 20, "AT+CIPSEND=%u\r\n", size);
	
	net_send_wait(cmd);
	
	net_send_bin(data, size);
}

unsigned int net_recv_data(unsigned char *data, unsigned int size)
{
	unsigned int i = 0;
	unsigned int bytes_rx = 0;

	if(verbose) printf("<< ");
	while(i < 5) {
		data[i] = uart_rx();
		if(verbose) printf("%c", data[i]);
		if(data[0] != '+') continue;
		i++;
	};
	
	if(strncmp("+IPD,", data, 5) != 0) return 0;

	do {
		data[i] = uart_rx();
		if(verbose) printf("%c", data[i]);
		i++;
	} while(data[i-1] != ':');

	bytes_rx = atoi(data + 5); // convert ascii to decimal
	
	if(verbose) printf("\nReceiving %u bytes...\n", bytes_rx);
	
	i = 0;
	
	/* NB: we may get issues if our data isn't at least as big as bytes_rx */
	
	if(verbose) printf("<<");
	do {
		data[i] = uart_rx();
		if(verbose) printf(" %x", data[i]);
		i++;
	} while((i < bytes_rx) && (i < size));

	if(verbose) printf("\n");

	return i;
}
