/* uart.h - Chris Young 2025 */

#ifndef __ZXNET_UART_H
#define __ZXNET_UART_H 1

__sfr __banked __at 0x153b IO_153B; // not in headers yet

/* Flush the UART read buffer */
void uart_flush_read_buffer(void);

/* Transmit a NULL-terminated string */
void uart_tx_string(unsigned char *s);

/* Transmit arbitrary data */
void uart_tx_bin(unsigned char *s, unsigned int size);

/* Receive a single character */
unsigned char uart_rx(void);

/* Receive a line - up to \n or until buffer is full
 * Returns number of bytes read
 */
unsigned int uart_rx_line(unsigned char *buffer, unsigned int bufsize);

#endif
