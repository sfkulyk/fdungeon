/* $Id: room.c,v 1.2 2002/07/21 18:31:34 saboteur Exp $
 *
 * 2000, unicorn
 */

#include "room.h"
#include "roomlist.h"
#include "const.h"

#include <sys/types.h>
#include <stdio.h>

#include <merc.h>

#define IDT_ABS		1
#define IDT_DELTA	2
#define IDT_GET		3


#define ABS(x)		IDT_ABS, { (void*)(x) }
#define DELTA(x)	IDT_DELTA, { ((void*)&((ROOM_INDEX_DATA*)0)->x) }
#define GET(x)		IDT_GET, { x }

extern const struct flag_type room_flags[], raff_flags[], sector_flags[];

static struct node_op vmt_room_flags = VMT_FLAGS_INIT(&room_flags);
static struct node_op vmt_ra = VMT_FLAGS_INIT(&raff_flags);
static struct node_op vmt_sector_enum = VMT_ENUM_INIT(&sector_flags);
static struct node_op vmt_link_dir = VMT_LINK_INIT("%s/to_room");
extern struct node_op vmt_exit;

static void *get_this(ROOM_INDEX_DATA *room) {
	return room;
}

static void *get_north(ROOM_INDEX_DATA *room) {
	return room->exit[0];
}

static void *get_east(ROOM_INDEX_DATA *room) {
	return room->exit[1];
}

static void *get_south(ROOM_INDEX_DATA *room) {
	return room->exit[2];
}

static void *get_west(ROOM_INDEX_DATA *room) {
	return room->exit[3];
}

static void *get_up(ROOM_INDEX_DATA *room) {
	return room->exit[4];
}

static void *get_down(ROOM_INDEX_DATA *room) {
	return room->exit[5];
}

static struct {
	const char *name;
	struct node_op *vmt;
	int ident_type;
	union {
		void *abs_ptr;
		u_int delta_ptr;
		void *(*get_ptr)(ROOM_INDEX_DATA *);
	} obj;
} leafs[] = {
	{ ".", 		&vmt_room,		GET(get_this)		},
	{ "..",		&vmt_roomlist,		ABS(&vmt_roomlist)	}, 
	/* directions shouldn't be last! */
	{ "north",	&vmt_exit,		GET(get_north)		},
	{ "east",	&vmt_exit,		GET(get_east)		},
	{ "south",	&vmt_exit,		GET(get_south)		},
	{ "west",	&vmt_exit,		GET(get_west)		},
	{ "up",		&vmt_exit,		GET(get_up)		},
	{ "down",	&vmt_exit,		GET(get_down)		},
	/* aliases for short navigation */
	{ "n",		&vmt_link_dir,		ABS("north")		},
	{ "e",		&vmt_link_dir,		ABS("east")		},
	{ "s",		&vmt_link_dir,		ABS("south")		},
	{ "w",		&vmt_link_dir,		ABS("west")		},
	{ "u",		&vmt_link_dir,		ABS("up")		},
	{ "d",		&vmt_link_dir,		ABS("down")		},
	/* misc room properties */
	{ "name",	&vmt_string,		DELTA(name)		},
	{ "description",&vmt_string,		DELTA(description)	},
	{ "owner",	&vmt_string,		DELTA(owner)		},
	{ "vnum",	&vmt_int64,		DELTA(vnum)		},
	{ "room_flags",	&vmt_room_flags,	DELTA(room_flags)	},
	{ "light",	&vmt_int,		DELTA(light)		},
	{ "sector_type",&vmt_sector_enum,	DELTA(sector_type)	},
	{ "heal_rate",	&vmt_int,		DELTA(heal_rate)	},
	{ "mana_rate",	&vmt_int,		DELTA(mana_rate)	},
	{ "ra",		&vmt_ra,		DELTA(ra)		},
};

#define COUNT(x) sizeof(x)/sizeof(*x)

static
void *get_ptr(ROOM_INDEX_DATA *room, int i) {
	void *ptr = 0;

	switch(leafs[i].ident_type)  {
		case IDT_ABS:
			ptr = leafs[i].obj.abs_ptr;
			break;
		case IDT_DELTA:
			ptr = ((char*)room) + 
				leafs[i].obj.delta_ptr;
			break;
		case IDT_GET:
			ptr = leafs[i].obj.get_ptr(room);
			break;
		default:
			/*XXX? (:*/
	}

	return ptr;
}

static
diropres *room_lookup(diropargs *argp, struct svc_req *req) {
	static diropres result;
	rfs_fh *fh=RFS_FH(&argp->dir);
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *)fh->fh_address;
	int i;
	
	result.status = NFSERR_NOENT;

	if(!strcmp(argp->name, ".")) {
		result.diropres_u.diropres.file=argp->dir;

		result.diropres_u.diropres.attributes=
			vmt_room.getattr(
				&result.diropres_u.diropres.file, req
				)->attrstat_u.attributes;

		result.status = NFS_OK;
		return (&result);
	} else for(i=0;i<COUNT(leafs);i++)
		if(get_ptr(room, i) && !strcmp(argp->name, leafs[i].name)) {
			
			FH_SET(result.diropres_u.diropres.file.data,
					leafs[i].vmt, get_ptr(room, i), room);

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
readdirres *room_readdir(readdirargs *argp, struct svc_req *req) {
	static readdirres result;
	static entry e;
	
	struct rfs_fh *fh=RFS_FH(&argp->dir);
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *)fh->fh_address;
	u_int cookie=*(u_int *)argp->cookie;
	int i;
	
	/*paranoid?*/
	if(cookie >= COUNT(leafs)) {
		result.status = NFSERR_PERM;
		return (&result);
	}
	
	do {
		e.name = (char*)leafs[cookie].name;
		e.fileid = (u_int)get_ptr(room, cookie);
		result.readdirres_u.reply.entries = &e;
		*(u_int *)e.cookie = ++cookie;
		result.readdirres_u.reply.eof = (cookie == COUNT(leafs));
	} while(!e.fileid && !result.readdirres_u.reply.eof);

	result.status = e.fileid ? NFS_OK : NFSERR_NOENT;
	return (&result);
}

struct node_op vmt_room = {
	room_lookup,
	std_dir_getattr,
	0,
	0,
	room_readdir,
	0
};

