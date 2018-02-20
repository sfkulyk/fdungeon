/* $Id: nfs_prot_svc.c,v 1.1.1.1 2001/01/22 12:45:13 cvsuser Exp $
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

RPC_PROG(NFS_PROGRAM, NFS_VERSION, rfs, "NFS service (NFS emulation)")
{
	union {
		nfs_fh nfsproc_getattr_2_arg;
		sattrargs nfsproc_setattr_2_arg;
		diropargs nfsproc_lookup_2_arg;
		nfs_fh nfsproc_readlink_2_arg;
		readargs nfsproc_read_2_arg;
		writeargs nfsproc_write_2_arg;
		createargs nfsproc_create_2_arg;
		diropargs nfsproc_remove_2_arg;
		renameargs nfsproc_rename_2_arg;
		linkargs nfsproc_link_2_arg;
		symlinkargs nfsproc_symlink_2_arg;
		createargs nfsproc_mkdir_2_arg;
		diropargs nfsproc_rmdir_2_arg;
		readdirargs nfsproc_readdir_2_arg;
		nfs_fh nfsproc_statfs_2_arg;
	} argument;
	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();

	switch (rqstp->rq_proc) {
	case NFSPROC_NULL:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) nfsproc_null_2;
		break;

	case NFSPROC_GETATTR:
		xdr_argument = xdr_nfs_fh;
		xdr_result = xdr_attrstat;
		local = (char *(*)()) nfsproc_getattr_2;
		break;

	case NFSPROC_SETATTR:
		xdr_argument = xdr_sattrargs;
		xdr_result = xdr_attrstat;
		local = (char *(*)()) nfsproc_setattr_2;
		break;

	case NFSPROC_ROOT:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) nfsproc_root_2;
		break;

	case NFSPROC_LOOKUP:
		xdr_argument = xdr_diropargs;
		xdr_result = xdr_diropres;
		local = (char *(*)()) nfsproc_lookup_2;
		break;

	case NFSPROC_READLINK:
		xdr_argument = xdr_nfs_fh;
		xdr_result = xdr_readlinkres;
		local = (char *(*)()) nfsproc_readlink_2;
		break;

	case NFSPROC_READ:
		xdr_argument = xdr_readargs;
		xdr_result = xdr_readres;
		local = (char *(*)()) nfsproc_read_2;
		break;

	case NFSPROC_WRITECACHE:
		xdr_argument = xdr_void;
		xdr_result = xdr_void;
		local = (char *(*)()) nfsproc_writecache_2;
		break;

	case NFSPROC_WRITE:
		xdr_argument = xdr_writeargs;
		xdr_result = xdr_attrstat;
		local = (char *(*)()) nfsproc_write_2;
		break;

	case NFSPROC_CREATE:
		xdr_argument = xdr_createargs;
		xdr_result = xdr_diropres;
		local = (char *(*)()) nfsproc_create_2;
		break;

	case NFSPROC_REMOVE:
		xdr_argument = xdr_diropargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_remove_2;
		break;

	case NFSPROC_RENAME:
		xdr_argument = xdr_renameargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_rename_2;
		break;

	case NFSPROC_LINK:
		xdr_argument = xdr_linkargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_link_2;
		break;

	case NFSPROC_SYMLINK:
		xdr_argument = xdr_symlinkargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_symlink_2;
		break;

	case NFSPROC_MKDIR:
		xdr_argument = xdr_createargs;
		xdr_result = xdr_diropres;
		local = (char *(*)()) nfsproc_mkdir_2;
		break;

	case NFSPROC_RMDIR:
		xdr_argument = xdr_diropargs;
		xdr_result = xdr_nfsstat;
		local = (char *(*)()) nfsproc_rmdir_2;
		break;

	case NFSPROC_READDIR:
		xdr_argument = xdr_readdirargs;
		xdr_result = xdr_readdirres;
		local = (char *(*)()) nfsproc_readdir_2;
		break;

	case NFSPROC_STATFS:
		xdr_argument = xdr_nfs_fh;
		xdr_result = xdr_statfsres;
		local = (char *(*)()) nfsproc_statfs_2;
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

