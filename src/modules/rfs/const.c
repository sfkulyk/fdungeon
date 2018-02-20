/* $Id: const.c,v 1.1.1.1 2001/01/22 12:45:28 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

struct cdir root = { {NULL}, NULL };

static fattr std_dirattr = {
 /*type*/       NFDIR,
 /*mode*/       0755,
 /*nlink*/      1,	
 /*uid*/        0,	
 /*gid*/        0,	
 /*size*/       512,	
 /*blksize*/    512,	
 /*rdev*/       -1,	
 /*blocks*/     1,	
 /*fsid*/       -1,	
 /*fileid*/     0,	
 /*atime*/      {0, 0},	
 /*mtime*/      {0, 0},	
 /*ctime*/      {0, 0},	
};

attrstat * std_dir_getattr(nfs_fh *fh, struct svc_req *req) {
	static attrstat result;

	result.attrstat_u.attributes = std_dirattr;
	result.attrstat_u.attributes.fileid=(u_int)RFS_FH(fh->data)->fh_address;

	return (&result);
}

