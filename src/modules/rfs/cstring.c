/* $Id: cstring.c,v 1.1.1.1 2001/01/22 12:43:59 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

static attrstat *cstring_getattr(nfs_fh *, struct svc_req *);
static readres *cstring_read(readargs *, struct svc_req *);

struct node_op vmt_string = {
	0,			/*lookup*/
	cstring_getattr,	/*getattr*/
	cstring_read,		/*read*/
	0,			/*readlink*/
	0,			/*readdir*/
};

static fattr const_attr = {
		 /*type*/       NFREG,
		 /*mode*/       0644,
		 /*nlink*/      1,	
		 /*uid*/        0,	
		 /*gid*/        0,	
		 /*size*/       1,	
		 /*blksize*/    1,
		 /*rdev*/       -1,	
		 /*blocks*/     0,	
		 /*fsid*/       -1,	
		 /*fileid*/     0,	
		 /*atime*/      {0, 0},	
		 /*mtime*/      {0, 0},	
		 /*ctime*/      {0, 0},	
	};

static
attrstat * cstring_getattr(nfs_fh *argp, struct svc_req *req) {
	static attrstat result;
	struct rfs_fh *fh=RFS_FH(argp);
	
	result.attrstat_u.attributes=const_attr;
	result.attrstat_u.attributes.fileid = (u_int)fh->fh_address;
	result.attrstat_u.attributes.size = strlen(*(char**)fh->fh_address);

	return (&result);
}

static
readres *cstring_read(readargs *argp, struct svc_req *req) {
	static readres result;
	struct rfs_fh *fh=RFS_FH(argp);
	char *s = *(char **)fh->fh_address;
	int rest;
	
	result.readres_u.reply.attributes=cstring_getattr(
					&argp->file, req
					)->attrstat_u.attributes;
	
	rest = result.readres_u.reply.attributes.size - argp->offset;
	
	if(rest < 0)
		rest = 0;
			
	result.readres_u.reply.data.data_val = s + argp->offset;
	result.readres_u.reply.data.data_len = 
		rest < argp->count ? rest : argp->count;

	return (&result);
}

