/* $Id: cdir.c,v 1.1.1.1 2001/01/22 12:43:39 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "const.h"

static diropres *cdir_lookup(diropargs *, struct svc_req *);
static readdirres *cdir_readdir(readdirargs *, struct svc_req *);

struct node_op vmt_cdir = {
	cdir_lookup,
	std_dir_getattr,
	0,
	0,
	cdir_readdir,
};

static
diropres *cdir_lookup(diropargs *argp, struct svc_req *req) {
	static diropres result;
	rfs_fh *fh=RFS_FH(&argp->dir);
	struct cdir *cd = (struct cdir *)fh->fh_address;
	struct cnode *cn;
	int i;
	
	result.status = NFSERR_NOENT;

	if(!strcmp(argp->name, ".")) {
		result.diropres_u.diropres.file=argp->dir;

		result.diropres_u.diropres.attributes=
			vmt_cdir.getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else if(!strcmp(argp->name, "..")) {
		if(cd->parent)
			FH_SET(result.diropres_u.diropres.file.data,
					&vmt_cdir, cd->parent, 0);
		else
			result.diropres_u.diropres.file=argp->dir;
			
		result.diropres_u.diropres.attributes=
			vmt_cdir.getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else SLIST_FOREACH(cn, &cd->nodes, cn_list)
		if(!strcmp(argp->name, cn->cn_name)) {
			FH_SET(result.diropres_u.diropres.file.data,
					cn->cn_vmt, cn->cn_obj, cd);

			result.diropres_u.diropres.attributes=
				cn->cn_vmt->getattr(
					&result.diropres_u.diropres.file, req
					)->attrstat_u.attributes;
			
			result.status = NFS_OK;
			return (&result);
		}

	return (&result);
}

static
readdirres *cdir_readdir(readdirargs *argp, struct svc_req *req) {
	static entry dotdot = {0, "..", {0, 0, 0, 0}, 0};
	static entry dot    = {0, ".",  {0, 0, 0, 0}, 0};
	static readdirres result;
	static entry e;
	
	struct rfs_fh *fh=RFS_FH(&argp->dir);
	struct cdir *cd = (struct cdir *)fh->fh_address;
	struct cnode *cn;
	struct cnode *cookie=*(struct cnode **)argp->cookie;
	
	if(argp->count < 2) {
		result.status = NFSERR_NXIO;
		return (&result);
	}

	if(cookie) {
		/*paranoid?*/
		SLIST_FOREACH(cn, &cd->nodes, cn_list)
			if(cn == cookie)
				break;

		if(cn != cookie) {
			result.status = NFSERR_PERM;
			return (&result);
		}
	} else {
		dot.fileid = (u_int)cd;
		dotdot.fileid = (u_int)cd->parent;
		if(!dotdot.fileid)
			dotdot.fileid++;
		dot.nextentry = &dotdot;
		result.readdirres_u.reply.entries = &dot;
		cookie=SLIST_FIRST(&cd->nodes);
		*(u_int*)dotdot.cookie = (u_int)cookie;
		result.readdirres_u.reply.eof = !cookie;
		
		result.status = NFS_OK;
		return (&result);
	}

	e.fileid = (u_int)cookie->cn_obj;
	e.name = (char *)cookie->cn_name;
	cookie = SLIST_NEXT(cookie, cn_list);
	*(u_int*)e.cookie = (u_int) cookie;

	result.readdirres_u.reply.entries = &e;
	result.readdirres_u.reply.eof = (cookie == NULL);

	return (&result);
}

