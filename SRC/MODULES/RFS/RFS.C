/* $Id: rfs.c,v 1.2 2001/01/28 04:12:49 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "rfs.h"

#include "const.h"

MOD(rfs, 0, "NFS emulation.");

char * new_fh(node_op *nodeop, void *address, void *parent) {
	int i;
	static char tmp[NFS_FHSIZE];
	static int secret = 0x11181118; /*XXX*/

	RFS_FH(tmp)->fh_nodeop = nodeop;
	RFS_FH(tmp)->fh_address = address;
	RFS_FH(tmp)->fh_parent = parent;
	RFS_FH(tmp)->fh_crc = 0; /*XXX !insecure!*/

	return tmp;
}

int check_fh(rfs_fh *fh) {
	/*XXX !insecure! */

	return 0;
}

