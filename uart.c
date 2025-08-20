/* uart.c - Chris Young 2025 */

#include <arch/zxn.h>

#include "main.h"
#include "uart.h"

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

unsigned char uart_rx(void)
{
        while (!(IO_133B & 0x01))
                user_break();

        return(IO_143B);
}

unsigned int uart_rx_line(char *buffer, unsigned int bufsize)
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
