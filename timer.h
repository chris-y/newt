/* timer.h - Chris Young 2025 */
#ifndef _NEWT_TIMER_H
#define _NEWT_TIMER_H 1
#include <z80.h>

#define ZX_TIMER ((z80_bpeek(23674)*65536) + (z80_bpeek(23673)*256) + z80_bpeek(23672))
#endif
