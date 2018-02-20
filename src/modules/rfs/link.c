/* $Id: link.c,v 1.1.1.1 2001/01/22 12:45:28 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

attrstat *link_getattr(nfs_fh *, struct svc_req *);
readlinkres *link_readlink(nfs_fh *, struct svc_req *);

#if 0
struct node_op vmt_link = {
	0,		/*lookup*/
	link_getattr,	/*getattr*/
	0,		/*read*/
	link_readlink,	/*readlink*/
	0,		/*readdir*/
};
#endif

static fattr const_attr = {
		 /*type*/       NFLNK,
		 /*mode*/       0644,
		 /*nlink*/      1,	
		 /*uid*/        0,	
		 /*gid*/        0,	
		 /*size*/       0,	
		 /*blksize*/    1,
		 /*rdev*/       -1,	
		 /*blocks*/     0,	
		 /*fsid*/       -1,	
		 /*fileid*/     0,	
		 /*atime*/      {0, 0},	
		 /*mtime*/      {0, 0},	
		 /*ctime*/      {0, 0},	
	};

attrstat * link_getattr(nfs_fh *argp, struct svc_req *req) {
	static attrstat result;
	struct rfs_fh *fh=RFS_FH(argp);
	
	result.attrstat_u.attributes=const_attr;
	result.attrstat_u.attributes.fileid = 0; //(u_int)fh->fh_address;
	result.attrstat_u.attributes.size = 0; //strlen((char*)fh->fh_address);

	return (&result);
}

readlinkres *link_readlink(nfs_fh *argp, struct svc_req *req) {
	static readlinkres result;
	static char tmp[MAX_STRING_LENGTH];
	char *arg = (char *)RFS_FH(argp)->fh_address;
	char *fmt = (char *)RFS_FH(argp)->fh_nodeop->config;
	
	do_printf(tmp, fmt, arg);

	result.readlinkres_u.data = tmp;

	return (&result);
}

