/* newt
 * Chris Young 2025
 */

#pragma output CLIB_MALLOC_HEAP_SIZE = -1

#include <input.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/zxn.h>

#include "error.h"
#include "main.h"
#include "net.h"
#include "sntp.h"
#include "uart.h" // for IO_153B

#define RTM_28MHZ 3 // from manual
#define NEWT_VER "0.1"

static unsigned char old_cpu_speed;
static unsigned char old_uart;

unsigned char err_break[] = "D BREAK - no repea" "\xf4";
unsigned char err_mem[] = "4 Out of memor" "\xf9";
unsigned char err_timeout[] = "Network timeou" "\xf4";

bool quiet = false;
bool verbose = false;

void user_break(void)
{
   if (in_key_pressed(IN_KEY_SCANCODE_SPACE | 0x8000))  // CAPS+SPACE
      exit((int)err_break);
}

static void cleanup(void)
{
	ZXN_NEXTREGA(REG_TURBO_MODE, old_cpu_speed);
	IO_153B = old_uart;
}

static void print_usage(void)
{
	printf(".newt [-qv] <command> [args]\n\n");
	printf("-q            quiet\n");
    printf("-v            verbose\n");
	printf("\nCommands:\n");
	printf("ip            show ip addr\n");
	printf("info          show esp firmware\n");
	printf("lookup <fqdn> lookup ip for fqdn\n");
	printf("sntp [server] get time from server\n");
	exit(0);
}

static void print_wifi(void)
{
	unsigned char at_cwjap[] = "AT+CWJAP?\r\n\0";
	unsigned int bytes_read = 0;
	unsigned char *buffer = NULL;

	if(!quiet) printf("Current AP association:\n");

	buffer = malloc(BUFFER_SIZE);
	if(buffer) {

		net_send(at_cwjap);

		while(1) {
			bytes_read = net_recv_line(buffer, BUFFER_SIZE);
			if(bytes_read == 0) break;
			if(strncmp(buffer, "+CWJAP:", 7) == 0) {
				unsigned int i = 0;
				unsigned char *str = strtok(buffer + 7, ",");
				
				while(str != NULL) {
					switch(i) {
						case 0:
							printf("SSID:    %s\n", str);
						break;
						case 1:
							printf("BSSID:   %s\n", str);
						break;
						case 2:
							printf("Channel: %s\n", str);
						break;
						case 3:
							printf("RSSI:    %s\n", str);
						break;
						/* The rest of the values are for newer firmware */
						default:
							printf("???:     %s\n", str);
						break;
					}

					str = strtok(NULL, ",");
					i++;
				}
				
				exit(0);
			} else {
				continue;
			}
		}
		free(buffer);
	} else {
		exit((int)err_mem);
	}

	exit(0);
}

static void print_ip(void)
{
	unsigned char at_cipsta[] = "AT+CIPSTA?\r\n\0";
	unsigned char strip[] = "+CIPSTA:";
	unsigned int bytes_read = 0;
	unsigned char *buffer = NULL;

	if(!quiet) printf("Current IP:\n");

	buffer = malloc(BUFFER_SIZE);
	if(buffer) {

		net_send(at_cipsta);

		while(1) {
			bytes_read = net_recv_stripped(buffer, BUFFER_SIZE, strip);
			if(bytes_read == 0) break;
			printf("%s", buffer);
		}
		free(buffer);
	} else {
		exit((int)err_mem);
	}

	exit(0);
}

static void print_cmd(unsigned char cmd[])
{
	unsigned int bytes_read = 0;
	unsigned char *buffer = NULL;

	buffer = malloc(BUFFER_SIZE);
	if(buffer) {

		net_send(cmd);

		while(1) {
			bytes_read = net_recv_line(buffer, BUFFER_SIZE);
			if(bytes_read == 0) break;
			printf("%s", buffer);
		}
		free(buffer);
	} else {
		exit((int)err_mem);
	}

	exit(0);
}

int main(int argc, char **argv)
{
	unsigned int command_at = 1;

	unsigned char at_gmr[] = "AT+GMR\r\n\0";
	unsigned char ate0[] = "ATE0\r\n\0";

	old_cpu_speed = ZXN_READ_REG(REG_TURBO_MODE);
	old_uart = IO_153B & 0x40;

	atexit(cleanup);

	ZXN_NEXTREGA(REG_TURBO_MODE, RTM_28MHZ);
	IO_153B = 0x00; // ESP UART
	net_send_wait(ate0); // echo off

	if(argc >= 2) {
		if(argv[1][0] == '-') {
				if(argv[1][1] == 'q') quiet = true;
				if(argv[1][1] == 'v') {
						quiet = false; /* can't be quiet and verbose simultaneously */
						verbose = true;
				}
				command_at = 2;
			}

		if(!quiet) printf("newt %s by Chris Young 2025\nhttps://github.com/chris-y/newt\n\n", NEWT_VER);

		if(argc >= (command_at + 1)) {
			if(stricmp(argv[command_at], "ip") == 0) print_ip();
			if(stricmp(argv[command_at], "wifi") == 0) print_wifi();
			if(stricmp(argv[command_at], "info") == 0) {
				if(!quiet) printf("ESP information:\n");
				print_cmd(at_gmr);
			}
			
			if(stricmp(argv[command_at], "sntp") == 0) {
				if(argc >= (command_at + 2)) {
					sntp_get(argv[command_at + 1]);
				} else {
					sntp_get(NULL);
				}
			}
		}

		if(argc >= (command_at + 2)) {
			if(stricmp(argv[command_at], "lookup") == 0) {
				unsigned char *ip = malloc(20);
				if(ip) {
					if(!quiet) printf("DNS lookup for \"%s\":\n", argv[command_at + 1]);
					if(net_lookup(argv[command_at + 1], ip, 20)) {
						puts(ip);
					} else {
						printf("Error resolving\n");
					}
					free(ip);
					exit(0);
				} else {
					exit((int)err_mem);
				}
			}
			if(stricmp(argv[command_at], "sntp") == 0) sntp_get(argv[command_at + 1]);
		}
	}

	print_usage();

	exit(0);
}
