/*
 * $Id: PluginSocket.h,v 1.3 2004/03/29 17:24:02 crc_canada Exp $
 */

#ifndef __pluginSocket_h__
#define __pluginSocket_h__


#ifndef AQUA

#include <unistd.h>

#endif /* AQUA */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "pluginUtils.h"


#ifdef __cplusplus
extern "C" {
#endif

/* what Browser are we running under? eg, netscape, mozilla, konqueror, etc */

#define MAXNETSCAPENAMELEN 256
extern char NetscapeName[MAXNETSCAPENAMELEN];
	                                                                                
char *requestUrlfromPlugin(int sockDesc, unsigned int plugin_instance, const char *url);

int
receiveUrl(int sockDesc, urlRequest *request);


#ifdef __cplusplus
}
#endif

	
#endif /*__pluginSocket_h__ */
