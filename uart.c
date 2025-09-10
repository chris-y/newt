/* uart.c - Chris Young 2025 */

#include <arch/zxn.h>

#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "main.h"
#include "timer.h"
#include "uart.h"

/* Timeout in seconds */
#define ESP_TIMEOUT 5

void uart_flush_read_buffer(void)
{
        unsigned char c;

        while (IO_133B & 0x01) {
                c = IO_143B;
                user_break();
        }
}

void uart_tx_string(unsigned char *s)
{
        while (*s) {
                while (IO_133B & 0x02)
                        user_break();
                IO_133B = *s++;
        }
}

void uart_tx_bin(unsigned char *s, unsigned int size)
{
	if(size == 0) return;

        do {
                while (IO_133B & 0x02)
                        user_break();
                IO_133B = *s++;
        } while (size--);
}

unsigned char uart_rx(void)
{
	uint32_t zxtime = ZX_TIMER;

        while (!(IO_133B & 0x01)) {
                user_break();
		if((ZX_TIMER - zxtime) > (50 * ESP_TIMEOUT)) {
			exit((int)err_timeout);
		}
	}

        return(IO_143B);
}

unsigned int uart_rx_line(unsigned char *buffer, unsigned int bufsize)
{
	unsigned int i = 0;
	unsigned int bufsz = bufsize - 1; // take into account NUL-termination

	do {
		buffer[i] = uart_rx();
		i++;
	} while((buffer[i-1] != '\n') && (i < bufsz));

	buffer[i] = '\0';

	return i;
}
