/* $Id: mount_svc.c,v 1.1.1.1 2001/01/22 12:45:12 cvsuser Exp $
 * 
 * 2000, unicorn
 * 
 * based on generated sources using rpcgen.
 */

#include "mount.h"
#include "nfs_prot.h"

#include <stdio.h>
#include <stdlib.h> /* getenv, exit */
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <mod.h>
#include <rpc.h>

#ifdef DEBUG
#define	RPC_SVC_FG
#endif
#ifndef lint
/*static char sccsid[] = "from: @(#)mount.x 1.2 87/09/18 Copyr 1987 Sun Micro";*/
/*static char sccsid[] = "from: @(#)mount.x	2.1 88/08/01 4.0 RPCSRC";*/
static const char rcsid[] =
  "$FreeBSD: src/include/rpcsvc/mount.x,v 1.6 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */

RPC_PROG(MOUNTPROG, MOUNTVERS, mount, "Mount service (NFS emulation)")
{
	union {
		dirpath mountproc_mnt_1_arg;
		dirpath mountproc_umnt_1_arg;
	} argument;
	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();

	switch (rqstp->rq_proc) {
	case MOUNTPROC_NULL:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) mountproc_null_1;
		break;

	case MOUNTPROC_MNT:
		xdr_argument = xdr_dirpath;
		xdr_result = xdr_fhstatus;
		local = (char *(*)()) mountproc_mnt_1;
		break;

	case MOUNTPROC_DUMP:
		xdr_argument = xdr_void;
		xdr_result = xdr_mountlist;
		local = (char *(*)()) mountproc_dump_1;
		break;

	case MOUNTPROC_UMNT:
		xdr_argument = xdr_dirpath;
		xdr_result = xdr_void;
		local = (char *(*)()) mountproc_umnt_1;
		break;

	case MOUNTPROC_UMNTALL:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) mountproc_umntall_1;
		break;

	case MOUNTPROC_EXPORT:
		xdr_argument = xdr_void;
		xdr_result = xdr_exports;
		local = (char *(*)()) mountproc_export_1;
		break;

	case MOUNTPROC_EXPORTALL:
		xdr_argument = xdr_void;
		xdr_result = xdr_exports;
		local = (char *(*)()) mountproc_exportall_1;
		break;

	default:
		svcerr_noproc(transp);
		return;
	}
	(void) memset((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs(transp, xdr_argument, (char *)&argument)) {
		svcerr_decode(transp);
		return;
	}
	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, xdr_result, result)) {
		svcerr_systemerr(transp);
	}
	if (!svc_freeargs(transp, xdr_argument, (char *)&argument)) {
		fprintf(stderr, "unable to free arguments");
		exit(1);
	}
	return;
}

