/* $Id: enum.c,v 1.3 2002/07/21 18:31:34 saboteur Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

#include <sys/types.h>
#include <stdio.h>

#include <merc.h>

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

static const char *lookup_id(struct flag_type *nm, int i) {
	int l;
	
	for(l=0;nm[l].name;l++)
		if(nm[l].bit == i)
			return nm[l].name;

	return "(unknown)";
}

attrstat * enum_getattr(nfs_fh *argp, struct svc_req *req) {
	static attrstat result;
	struct rfs_fh *fh=RFS_FH(argp);
	struct flag_type *nm = (struct flag_type *)fh->fh_nodeop->config;
	u_int *i = (u_int *)fh->fh_address;
	int len;
	

	result.attrstat_u.attributes=const_attr;
	result.attrstat_u.attributes.fileid = (u_int)i;
	result.attrstat_u.attributes.size = strlen(lookup_id(nm, *i));

	return (&result);
}

readres *enum_read(readargs *argp, struct svc_req *req) {
	static readres result;
	struct rfs_fh *fh=RFS_FH(argp);
	struct flag_type *nm = (struct flag_type *)fh->fh_nodeop->config;
	u_int *i = (u_int *)fh->fh_address;
	int rest;
	
	
	result.readres_u.reply.attributes=enum_getattr(
					&argp->file, req
					)->attrstat_u.attributes;

	rest = result.readres_u.reply.attributes.size - argp->offset;
	
	if(rest < 0)
		rest = 0;
			
	result.readres_u.reply.data.data_val = (char*)lookup_id(nm, *i) + argp->offset;
	result.readres_u.reply.data.data_len = 
		rest < argp->count ? rest : argp->count;

	return (&result);
}

