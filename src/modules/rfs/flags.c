/* $Id: flags.c,v 1.4 2002/07/21 18:31:34 saboteur Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

#include <sys/types.h>
#include <stdio.h>

#include <merc.h>

static char *stub_str = "Nothing here!";

#ifdef BIT
#undef BIT
#endif
#define BIT(x) ( ((unsigned long long int)(1)) << (x) )

diropres *flags_lookup(diropargs *argp, struct svc_req *req) {
	static diropres result;
	rfs_fh *fh=RFS_FH(&argp->dir);
	int64 f = *(int64 *)fh->fh_address;
	struct flag_type *nm = (struct flag_type *)fh->fh_nodeop->config;
	int i;
	
	result.status = NFSERR_NOENT;

	if(!strcmp(argp->name, ".")) {
		result.diropres_u.diropres.file=argp->dir;

		result.diropres_u.diropres.attributes=
			fh->fh_nodeop->getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else for(i=0;nm[i].name;i++)
		if((f & nm[i].bit) && !strcmp(argp->name, nm[i].name)) {
			FH_SET(result.diropres_u.diropres.file.data,
					&vmt_string, &stub_str, 0);

			result.diropres_u.diropres.attributes=
				vmt_string.getattr(
					&result.diropres_u.diropres.file, req
					)->attrstat_u.attributes;
			
			result.status = NFS_OK;
			return (&result);
		}

	return (&result);
}

readdirres *flags_readdir(readdirargs *argp, struct svc_req *req) {
	static entry dot    = {0, ".",  {0, 0, 0, 0}, 0};
	static readdirres result;
	static entry e;
	
	struct rfs_fh *fh=RFS_FH(&argp->dir);
	int64 *f = (int64 *)fh->fh_address, i64;
	struct flag_type *nm = (struct flag_type *)fh->fh_nodeop->config;
	u_int cookie=*(u_int *)argp->cookie;
	int i;
	
	if(cookie) {
		/*paranoid?*/
		if(cookie > 64) {
			result.status = NFSERR_PERM;
			return (&result);
		}
		
		e.name = (char*)nm[cookie - 1].name;
		e.fileid = (u_int)&stub_str;
		
		result.readdirres_u.reply.entries = &e;
		
		*(u_int *)e.cookie = 0;
		for(i=cookie;nm[i].name;i++)
			if(*f & nm[i].bit) {
				*(u_int *)e.cookie=i + 1;
				break;
			}
	} else {
		dot.fileid = (u_int)f;
		result.readdirres_u.reply.entries = &dot;

		*(u_int *)dot.cookie = 0;
		for(i=0;nm[i].name;i++)
			if(*f & nm[i].bit) {
				*(u_int *)dot.cookie=i + 1;
				break;
			}
	}

	result.readdirres_u.reply.eof = 
		!*(u_int *)result.readdirres_u.reply.entries->cookie;

	result.status = NFS_OK;
	return (&result);
}

