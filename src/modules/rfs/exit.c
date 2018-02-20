/* $Id: exit.c,v 1.2 2002/07/21 18:31:34 saboteur Exp $
 *
 * 2000, unicorn
 */

#include "room.h"
#include "const.h"

#include <sys/types.h>
#include <stdio.h>

#include <merc.h>

#define IDT_ABS		1
#define IDT_DELTA	2
#define IDT_GET		3


#define ABS(x)		IDT_ABS, { (void*)(x) }
#define DELTA(x)	IDT_DELTA, { ((void*)&((EXIT_DATA*)0)->x) }
#define GET(x)		IDT_GET, { x }

extern const struct flag_type exit_flags[];
static struct node_op vmt_exit_flags = VMT_FLAGS_INIT(&exit_flags);

static const struct flag_type direction_flags[] = {
	{ "north",	0,	TRUE },
	{ "east",	1,	TRUE },
	{ "south",	2,	TRUE },
	{ "west",	3,	TRUE },
	{ "up",		5,	TRUE },
	{ "down",	6,	TRUE },
	
	{ NULL,		-1,	FALSE}
};
static struct node_op vmt_dir_flags = VMT_ENUM_INIT(&exit_flags);

static struct node_op vmt_to_room = VMT_LINK_INIT("../../%d");

static readdirres *exit_readdir(readdirargs *argp, struct svc_req *req);
static diropres *exit_lookup(diropargs *argp, struct svc_req *req);
struct node_op vmt_exit = {
	exit_lookup,
	std_dir_getattr,
	0,
	0,
	exit_readdir,
	0
};

static void *get_this(rfs_fh *fh) {
	return fh->fh_address;
}

static void *get_to_room(rfs_fh *fh) {
	/* better don't try to create vnums over `long' */
	return (void*)(long)((EXIT_DATA *)fh->fh_address)->u1.to_room->vnum;
}

static void *get_parent(rfs_fh *fh) {
	return fh->fh_parent;
}

static struct {
	const char *name;
	struct node_op *vmt;
	int ident_type;
	union {
		void *abs_ptr;
		u_int delta_ptr;
		void *(*get_ptr)(rfs_fh *);
	} obj;
} leafs[] = {
	{ ".", 		&vmt_exit,		GET(get_this)		},
	{ "..",		&vmt_room,		GET(get_parent)		}, 
	{ "to_room",	&vmt_to_room,		GET(get_to_room)	},
//	{ "to_room",	&vmt_room,		GET(get_to_room)	},
	{ "keyword",	&vmt_string,		DELTA(keyword)		},
	{ "description",&vmt_string,		DELTA(description)	},
	{ "exit_info",	&vmt_exit_flags,	DELTA(exit_info)	},/*!*/
	{ "rs_flags",	&vmt_exit_flags,	DELTA(rs_flags)		},/*!*/
	{ "key",	&vmt_int64,		DELTA(key)		},/*?*/
};

#define COUNT(x) sizeof(x)/sizeof(*x)

static
void *get_ptr(rfs_fh *fh, int i) {
	void *ptr = 0;

	switch(leafs[i].ident_type)  {
		case IDT_ABS:
			ptr = leafs[i].obj.abs_ptr;
			break;
		case IDT_DELTA:
			ptr = ((char*)fh->fh_address) + 
				leafs[i].obj.delta_ptr;
			break;
		case IDT_GET:
			ptr = leafs[i].obj.get_ptr(fh);
			break;
		default:
			/*XXX? (:*/
	}

	return ptr;
}

static
diropres *exit_lookup(diropargs *argp, struct svc_req *req) {
	static diropres result;
	rfs_fh *fh=RFS_FH(&argp->dir);
	EXIT_DATA *ex = (EXIT_DATA *)fh->fh_address;
	int i;
	
	result.status = NFSERR_NOENT;

	if(!strcmp(argp->name, ".")) {
		result.diropres_u.diropres.file=argp->dir;

		result.diropres_u.diropres.attributes=
			vmt_exit.getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else for(i=0;i<COUNT(leafs);i++)
		if(!strcmp(argp->name, leafs[i].name)) {

			FH_SET(result.diropres_u.diropres.file.data,
					leafs[i].vmt, get_ptr(fh, i), 0);

			result.diropres_u.diropres.attributes=
				leafs[i].vmt->getattr(
					&result.diropres_u.diropres.file, req
					)->attrstat_u.attributes;

			result.status = NFS_OK;
			return (&result);
		}

	return (&result);
}

static
readdirres *exit_readdir(readdirargs *argp, struct svc_req *req) {
	static readdirres result;
	static entry e;
	
	struct rfs_fh *fh=RFS_FH(&argp->dir);
	EXIT_DATA *ex = (EXIT_DATA *)fh->fh_address;
	u_int cookie=*(u_int *)argp->cookie;
	int i;

	/*paranoid?*/
	if(cookie >= COUNT(leafs)) {
		result.status = NFSERR_PERM;
		return (&result);
	}
	
	e.name = (char*)leafs[cookie].name;
	e.fileid = (u_int)get_ptr(fh, cookie);
	result.readdirres_u.reply.entries = &e;
	*(u_int *)e.cookie = ++cookie;
	result.readdirres_u.reply.eof = (cookie == COUNT(leafs));

	result.status = NFS_OK;
	return (&result);
}

