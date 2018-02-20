/* $Id: tell.c,v 1.1 2001/01/28 19:33:03 cvsuser Exp $
 *
 * 2001, unicorn
 */

#include "remote.h"

#include <stdio.h>
#include <string.h>

CLIENT *clnt;

int
main(argc, argv)
	int argc;
	char *argv[];
{
	char buf[1024], names[1024], *s;
	tell_args a;
	
	argc--;
	argv++;

	clnt = clnt_create("127.0.0.1", RRPROG, RRVERS, "udp");
	if(!clnt) {
		fputs("Failed to connect.\n", stderr);
		return 1;
	}

	for(*names=0;argc;argv++, argc--) {
		strcat(names, *argv);
		strcat(names, " ");
	}

	a.to = names;
	a.text = buf;
	
	while(fgets(buf, sizeof(buf), stdin)) {
		if(s=strchr(buf, '\n')) *s = 0;
		if(s=strchr(buf, '\r')) *s = 0;
		if(!tell_1(&a, clnt)) {
			fputs("Error, sending message to mud.\n", stderr);
			return 1;
		}
	}

	clnt_destroy(clnt);
	return 0;
}
