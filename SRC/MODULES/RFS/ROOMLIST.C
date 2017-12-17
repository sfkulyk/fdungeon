/* $Id: roomlist.c,v 1.1.1.1 2001/01/22 12:45:13 cvsuser Exp $
 *
 * 2000, unicorn
 */

#include "const.h"
#include "room.h"

#include <sys/types.h>
#include <stdio.h>
#include <merc.h>

static diropres *roomlist_lookup(diropargs *, struct svc_req *);

#ifdef notyet
static readdirres *roomlist_readdir(readdirargs *, struct svc_req *);
#endif

struct node_op vmt_roomlist = {
	roomlist_lookup,
	std_dir_getattr,
	0,
	0,
	0,/* roomlist_readdir, 
	   * XXX we have no list of rooms. Just hash. 
	   * So, we can lookup, but can't list.
	   * 
	   * TODO skip step-to-step gaps in vnums from area min_vnum, 
	   * to area max_vnum. It's slow, but it's better than NULL.
	   */
};

CNODE(roomlist, root, &vmt_roomlist, &vmt_roomlist)

static
diropres *roomlist_lookup(diropargs *argp, struct svc_req *req) {
	static diropres result;
	rfs_fh *fh=RFS_FH(&argp->dir);
	int i;

	result.status = NFSERR_NOENT;

	if(!strcmp(argp->name, ".")) {
		result.diropres_u.diropres.file=argp->dir;

		result.diropres_u.diropres.attributes=
			vmt_roomlist.getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else if(!strcmp(argp->name, "..")) {
		                                           /*
							    * make sure you
							    * pass to CNODE
							    * the same CDIR
							    */
		FH_SET(result.diropres_u.diropres.file.data,
					&vmt_cdir, &root, 0);
			
		result.diropres_u.diropres.attributes=
			vmt_cdir.getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else {
		long long vnum = 0;
		char *tmp = argp->name;
		ROOM_INDEX_DATA *rid;

		for(;isdigit(*tmp);tmp++)
			vnum = vnum*10 + *tmp - '0';
		
		rid = get_room_index(vnum);
		if(rid && rid->vnum == vnum) {
			FH_SET(result.diropres_u.diropres.file.data,
					&vmt_room, rid, 0);

			result.diropres_u.diropres.attributes=
				vmt_room.getattr(
					&result.diropres_u.diropres.file, req
					)->attrstat_u.attributes;
			
			result.status = NFS_OK;
			return (&result);
		}
	}
	return (&result);
}

