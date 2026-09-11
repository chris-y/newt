/* http.h - Chris Young 2026 */

#ifndef __NEWT_HTTP_H
#define __NEWT_HTTP_H 1

/* Returns false on error */
bool http_get(unsigned char *req, unsigned char *buf, unsigned int buf_size);
#endif
