/* http.h - Chris Young 2026 */

#ifndef __NEWT_HTTP_H
#define __NEWT_HTTP_H 1

#include <stdbool.h>

/* Returns false on error */
bool http_get(unsigned char *req, unsigned char *buf, unsigned int buf_size);

char *http_strip_header(unsigned char *buf);
#endif
