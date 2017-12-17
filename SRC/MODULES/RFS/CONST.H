/* $Id: const.h,v 1.1.1.1 2001/01/22 12:45:13 cvsuser Exp $
 *
 * 2000, unicorn
 */

#ifndef __CONST_H__
#define __CONST_H__

#include "rfs.h"
#include "mod.h"

struct cnode {
	SLIST_ENTRY(cnode) cn_list;
	char *cn_name;
	struct node_op *cn_vmt;
	void *cn_obj;
};

extern struct cdir {
	SLIST_HEAD(, cnode) nodes;
	struct cdir *parent;
} root;

extern struct node_op vmt_string;
extern struct node_op vmt_int;
extern struct node_op vmt_int64;
extern struct node_op vmt_cdir;
extern struct node_op vmt_link;

#define ADD_LEAF(d, ptr) \
SLIST_INSERT_HEAD(&d.nodes, ptr, cn_list);

#define DEL_LEAF(d, ptr) \
SLIST_REMOVE(&d.nodes, ptr, cnode, cn_list);

#define CNODE(name, parent, vmt, ptr) \
static struct cnode __rfs_##name##_cnode = { {0}, #name, vmt, ptr }; \
CTOR(__rfs_##name##_cnode) { ADD_LEAF(parent, &__rfs_##name##_cnode); } \
DTOR(__rfs_##name##_cnode) { DEL_LEAF(parent, &__rfs_##name##_cnode); }

#define CSTRING(name, parent, ptr) \
CNODE( name, parent, &vmt_string, &ptr);

#define CDIR(name, parent) \
struct cdir name = {{0}, &parent}; \
CNODE( name, parent, &vmt_cdir, &name);



#define VMT_FLAGS_INIT(x) { \
	flags_lookup, std_dir_getattr, 0, 0, flags_readdir, x \
}

#define VMT_ENUM_INIT(x) { \
	0, enum_getattr, enum_read, 0, 0, x \
}

#define VMT_LINK_INIT(x) { \
	0, link_getattr, 0, link_readlink, 0, x \
}

attrstat * std_dir_getattr(nfs_fh *fh, struct svc_req *req);

diropres *flags_lookup(diropargs *argp, struct svc_req *req);
readdirres *flags_readdir(readdirargs *argp, struct svc_req *req);

attrstat *enum_getattr(nfs_fh *, struct svc_req *);
readres *enum_read(readargs *, struct svc_req *);

attrstat *link_getattr(nfs_fh *, struct svc_req *);
readlinkres *link_readlink(nfs_fh *, struct svc_req *);
#endif /* __CONST_H__ */
