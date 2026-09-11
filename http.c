/* http.c - Chris Young 2026 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "net.h"

bool http_get(unsigned char *req, unsigned char *buf, unsigned int buf_size)
{
	char cmd[101];
	
	if((req[0] == 'h') &&
		(req[1] == 't') &&
		(req[2] == 't') &&
		(req[3] == 'p') &&
		(req[4] == ':') &&
		(req[5] == '/') &&
		(req[6] == '/')) req += 7;

	char *path = strchr(req, '/');
	
	if(path != NULL) {
		*path = '\0';
		path++;
		snprintf(cmd, 100, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, req);
	} else {
		snprintf(cmd, 100, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", req);
	}
	
	printf("%s, %s, %s\n", req, path, cmd);

	/* ensure no open connections */
	net_close();
	
	if(net_connect_tcp(req, 80)) {
		net_send_data(cmd, strlen(cmd));
		net_recv_data(buf, buf_size);
		net_close();
	} else {
		return false;
	}
	
	return true;
}
