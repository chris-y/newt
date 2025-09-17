/* timer.h - Chris Young 2025 */
#ifndef _NEWT_TIMER_H
#define _NEWT_TIMER_H 1
#include <z80.h>

#define ZX_TIMER ((z80_bpeek(23674)*65536) + (z80_bpeek(23673)*256) + z80_bpeek(23672))

static inline void zx_timer_set(uint32_t t)
{
        uint32_t t2 = (t % 65536);
        unsigned char t3 = (t2 % 256);

        z80_bpoke(23674, (unsigned char)(t / 65536));
        z80_bpoke(23673, (unsigned char)(t2 / 256));
        z80_bpoke(23672, t3);
}

#endif
