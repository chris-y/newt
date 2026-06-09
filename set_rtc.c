/*
 * set_rtc.c – Set the ZX Spectrum Next DS1307 (time, date & signature)
 *             Callable function for your own dot command.
 *
 * No external I/O headers required – all port access is inline asm.
 *
 * Usage:
 *   #include "set_rtc.h"
 *   set_rtc(2026, 6, 8, 14, 30, 0);
 */

#include <stdint.h>

/* ZX Next I²C bus 0 – the DS1307 */
#define SCL_PORT  0x103B
#define SDA_PORT  0x113B
#define DS1307_ADDR_W  0xD0

/* ------------------------------------------------------------------ */
/* Low‑level I²C helpers (direct port writes, matching the ASM) */

static void i2c_delay(void) {
    __asm__("nop\nnop\nnop\n");
}

static void scl_high(void) {
    __asm__("ld bc, 0x103B\nld a, 1\nout (c), a");
    i2c_delay();
}
static void scl_low(void) {
    __asm__("ld bc, 0x103B\nxor a\nout (c), a");
    i2c_delay();
}
static void sda_high(void) {
    __asm__("ld bc, 0x113B\nld a, 1\nout (c), a");
    i2c_delay();
}
static void sda_low(void) {
    __asm__("ld bc, 0x113B\nxor a\nout (c), a");
    i2c_delay();
}

static void i2c_start(void) {
    sda_high();
    scl_high();
    i2c_delay();
    sda_low();
    i2c_delay();
    scl_low();
}

static void i2c_stop(void) {
    sda_low();
    scl_high();
    i2c_delay();
    sda_high();
    i2c_delay();
}

/* Write one byte – no ACK check (same as the ASM) */
static void i2c_write(uint8_t data) {
    uint8_t i;
    for (i = 0x80; i; i >>= 1) {
        if (data & i) sda_high();
        else          sda_low();
        scl_high();
        i2c_delay();
        scl_low();
    }
    /* Release SDA for ACK */
    sda_high();
    scl_high();
    i2c_delay();
    scl_low();
}

/* ------------------------------------------------------------------ */
/* Decimal to packed BCD */
static uint8_t dec2bcd(int val) {
    return (uint8_t)(((val / 10) << 4) | (val % 10));
}

/* Sakamoto’s algorithm: 0=Sun … 6=Sat */
static int day_of_week(int y, int m, int d) {
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

/* ------------------------------------------------------------------ */
/*
 * set_rtc
 *
 * Write the DS1307 clock and RAM signature exactly as the Next’s
 * original .TIME and .DATE dot commands do.
 *
 * Parameters:
 *   year  – full year, e.g. 2026 (must be 2000‑2099)
 *   month – 1 to 12
 *   day   – 1 to 31
 *   hour  – 0 to 23
 *   min   – 0 to 59
 *   sec   – 0 to 59
 */
void set_rtc(int year, int month, int day,
             int hour, int min, int sec)
{
    /* Compute day‑of‑week for the DS1307 (1=Monday … 7=Sunday) */
    int dow = day_of_week(year, month, day);
    if (dow == 0) dow = 7;

    /* All values in packed BCD */
    uint8_t bcd_sec  = dec2bcd(sec);
    uint8_t bcd_min  = dec2bcd(min);
    uint8_t bcd_hour = dec2bcd(hour) & 0x3F;   /* force 24‑hour mode */
    uint8_t bcd_dow  = (uint8_t)dow;
    uint8_t bcd_date = dec2bcd(day);
    uint8_t bcd_mon  = dec2bcd(month);
    uint8_t bcd_year = dec2bcd(year % 100);

    /* 1. Write time & date registers (0x00 … 0x06) */
    i2c_start();
    i2c_write(DS1307_ADDR_W);
    i2c_write(0x00);            /* seconds register */
    i2c_write(bcd_sec);
    i2c_write(bcd_min);
    i2c_write(bcd_hour);
    i2c_write(bcd_dow);
    i2c_write(bcd_date);
    i2c_write(bcd_mon);
    i2c_write(bcd_year);
    i2c_stop();

    /* 2. Write validation signature 'ZX' into battery‑backed RAM */
    i2c_start();
    i2c_write(DS1307_ADDR_W);
    i2c_write(0x3E);            /* RAM address 0x3E */
    i2c_write('Z');             /* 0x5A */
    i2c_write('X');             /* 0x58 */
    i2c_stop();
}

