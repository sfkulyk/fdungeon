/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include <stdio.h>

#include "rfs.h"

#ifndef lint
/*static char sccsid[] = "from: @(#)nfs_prot.x 1.2 87/10/12 Copyr 1987 Sun Micro";*/
/*static char sccsid[] = "from: @(#)nfs_prot.x	2.1 88/08/01 4.0 RPCSRC";*/
static const char rcsid[] =
  "$FreeBSD: src/include/rpcsvc/nfs_prot.x,v 1.7 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */
  
void *
nfsproc_null_2(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static char * result;

	/*
	 * insert server code here
	 */

	return((void *) &result);
}

attrstat *
nfsproc_getattr_2(argp, rqstp)
	nfs_fh *argp;
	struct svc_req *rqstp;
{
	static struct attrstat result;
	rfs_fh *fh=(rfs_fh *)argp;

	if(check_fh(fh)) {
		result.status = NFSERR_PERM;
		return (&result);
	} else if(fh->fh_nodeop && fh->fh_nodeop->getattr)
		return fh->fh_nodeop->getattr(argp, rqstp);
	else {
		result.status = NFSERR_NXIO;
		return (&result);
	}
}

attrstat *
nfsproc_setattr_2(argp, rqstp)
	sattrargs *argp;
	struct svc_req *rqstp;
{
	static attrstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

void *
nfsproc_root_2(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static char * result;

	/*
	 * insert server code here
	 */

	return((void *) &result);
}

diropres *
nfsproc_lookup_2(argp, rqstp)
	diropargs *argp;
	struct svc_req *rqstp;
{
	static struct diropres result;
	rfs_fh *fh=(rfs_fh *)&argp->dir;

	if(check_fh(fh)) {
		result.status = NFSERR_PERM;
		return (&result);
	} else if(fh->fh_nodeop && fh->fh_nodeop->lookup)
		return fh->fh_nodeop->lookup(argp, rqstp);
	else {
		result.status = NFSERR_NXIO;
		return (&result);
	}
}

readlinkres *
nfsproc_readlink_2(argp, rqstp)
	nfs_fh *argp;
	struct svc_req *rqstp;
{
	static struct readlinkres result;
	rfs_fh *fh=(rfs_fh *)argp;

	if(check_fh(fh)) {
		result.status = NFSERR_PERM;
		return (&result);
	} else if(fh->fh_nodeop && fh->fh_nodeop->readlink)
		return fh->fh_nodeop->readlink(argp, rqstp);
	else {
		result.status = NFSERR_NXIO;
		return (&result);
	}
}

readres *
nfsproc_read_2(argp, rqstp)
	readargs *argp;
	struct svc_req *rqstp;
{
	static struct readres result;
	rfs_fh *fh=(rfs_fh *)&argp->file;

	if(check_fh(fh)) {
		result.status = NFSERR_PERM;
		return (&result);
	} else if(fh->fh_nodeop && fh->fh_nodeop->read)
		return fh->fh_nodeop->read(argp, rqstp);
	else {
		result.status = NFSERR_NXIO;
		return (&result);
	}
}

void *
nfsproc_writecache_2(argp, rqstp)
	void *argp;
	struct svc_req *rqstp;
{
	static char * result;

	/*
	 * insert server code here
	 */

	return((void *) &result);
}

attrstat *
nfsproc_write_2(argp, rqstp)
	writeargs *argp;
	struct svc_req *rqstp;
{
	static attrstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

diropres *
nfsproc_create_2(argp, rqstp)
	createargs *argp;
	struct svc_req *rqstp;
{
	static diropres  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

nfsstat *
nfsproc_remove_2(argp, rqstp)
	diropargs *argp;
	struct svc_req *rqstp;
{
	static nfsstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

nfsstat *
nfsproc_rename_2(argp, rqstp)
	renameargs *argp;
	struct svc_req *rqstp;
{
	static nfsstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

nfsstat *
nfsproc_link_2(argp, rqstp)
	linkargs *argp;
	struct svc_req *rqstp;
{
	static nfsstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

nfsstat *
nfsproc_symlink_2(argp, rqstp)
	symlinkargs *argp;
	struct svc_req *rqstp;
{
	static nfsstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

diropres *
nfsproc_mkdir_2(argp, rqstp)
	createargs *argp;
	struct svc_req *rqstp;
{
	static diropres  result;

	/*
	 * insert server code here
	 */

	return (&result);
}

nfsstat *
nfsproc_rmdir_2(argp, rqstp)
	diropargs *argp;
	struct svc_req *rqstp;
{
	static nfsstat  result;

	/*
	 * insert server code here
	 */

	return (&result);
}



readdirres *
nfsproc_readdir_2(argp, rqstp)
	readdirargs *argp;
	struct svc_req *rqstp;
{
	static struct readdirres result;
	rfs_fh *fh=(rfs_fh *)&argp->dir;

	if(check_fh(fh)) {
		result.status = NFSERR_PERM;
		return (&result);
	} else if(fh->fh_nodeop && fh->fh_nodeop->readdir)
		return fh->fh_nodeop->readdir(argp, rqstp);
	else {
		result.status = NFSERR_NXIO;
		return (&result);
	}
}

statfsres *
nfsproc_statfs_2(argp, rqstp)
	nfs_fh *argp;
	struct svc_req *rqstp;
{
	static statfsres  result;

	/*
	 * insert server code here
	 */

	return (&result);
}
