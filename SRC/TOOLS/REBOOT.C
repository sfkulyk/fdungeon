/* $Id: reboot.c,v 1.1 2001/01/28 19:33:03 cvsuser Exp $
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
	reboot_args a;
	
	if (argc < 2) {
		printf("Usage: %s ticks [reason]\n", *argv);
		return 1;
	}
	argv++;	argc--;

	clnt = clnt_create("127.0.0.1", RRPROG, RRVERS, "udp");
	if(!clnt) {
		fputs("Failed to connect.\n", stderr);
		return 1;
	}
	
	a.count = atoi(*argv);
	argc--; argv++;
	
	if(argc > 0)
		a.reason = atoi(*argv);
	else
		a.reason = 0;
	
	if(!reboot_1(&a, clnt)) {
		fputs("Error, sending data to server.\n", stderr);
		return 1;
	}

	clnt_destroy(clnt);
	return 0;
}

