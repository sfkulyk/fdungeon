
/* $Id: cint64.c,v 1.1.1.1 2001/01/22 12:45:13 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

#include <sys/types.h>
#include <stdio.h>
#include <merc.h>

static attrstat *cint64_getattr(nfs_fh *, struct svc_req *);
static readres *cint64_read(readargs *, struct svc_req *);

struct node_op vmt_int64 = {
	0,
	cint64_getattr,
	cint64_read,
	0,
	0,
};

static fattr const_attr = {
	NFREG, 0644, 1,0,0,1,512,-1,0,-1,0,{0, 0},{0, 0},{0, 0}
};

static
attrstat * cint64_getattr(nfs_fh *argp, struct svc_req *req) {
	static attrstat result;
	struct rfs_fh *fh=RFS_FH(argp);
	int64 s = *(int64 *)fh->fh_address;
	char val[50];
	
	do_printf(val, "%u", s);
	result.attrstat_u.attributes=const_attr;
	result.attrstat_u.attributes.fileid = (u_int)fh->fh_address;
	result.attrstat_u.attributes.size = strlen(val);

	return (&result);
}

#define min(x, y) (x < y ? x : y)

static
readres *cint64_read(readargs *argp, struct svc_req *req) {
	static readres result;
	struct rfs_fh *fh=RFS_FH(argp);
	int64 s = *(int64 *)fh->fh_address;
	static char val[50];
	
	result.readres_u.reply.attributes=cint64_getattr(
					&argp->file, req
					)->attrstat_u.attributes;
	
	if(argp->offset) {
		result.status = NFSERR_NXIO;
		return (&result);
	}
	
	do_printf(val, "%u", s);
	result.readres_u.reply.data.data_val = val;
	result.readres_u.reply.data.data_len = 
		min(argp->count, result.readres_u.reply.attributes.size);

	result.status = NFS_OK;
	return (&result);
}

