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
		return true;
	} else {
		return false;
	}
}
