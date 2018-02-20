/* $Id: rfs.h,v 1.1.1.1 2001/01/22 12:45:12 cvsuser Exp $
 *
 * 2000, unicorn
 * 
 */

/* 
 * file handle format:
 * (long) vmt (node operations ptr)
 * (long) obj (realy void *data)
 * (long) obj-parent (realy void *data)
 * (long) packet+secure_seed crc (crypt/des/md5/...other one-way hash?)
 *
 * Accepting address from rpc client can cause crash in two cases:
 *   -  things we expect there has been freed:
 *	it's not realy so disapointing. We guaranteed the memmory is still 
 *	with us beacause of recycling mechanism forebides core size to be grown
 *	down (; 
 *	(but not so funny - social editor seems to use realloc(3) )=
 *   -	improper use (spoofed rpc client, etc) but who cares?
 *  	secure_seed + md5 CRC should aviod us from such a tricky things.
 *
 * XXX: in order to keep client's name cache in touch, one should coarsly set
 * special attribute on a particular vnodes to disable namei caching. This
 * schem is requires modifications in standard nfs kernel driver, but this can
 * be avoided this by using `coda' rpc2 protocol, which allows transmission of 
 * such an attrubute.
 */

/* Each node can imply this methods:
 * 
 * readdir lookup getattr read readlink
 * 
 * NOTYET:
 * link symlink setattr write mkdir rmdir create remove rename
 */

#ifndef __RFS_H__
#define __RFS_H__

#include "nfs_prot.h"

typedef readdirres *(*readdir_t)(readdirargs *, struct svc_req *);
typedef diropres *(*lookup_t)(diropargs *, struct svc_req *);
typedef attrstat *(*getattr_t)(nfs_fh *, struct svc_req *);
typedef readres *(*read_t)(readargs *, struct svc_req *);
typedef readlinkres *(*readlink_t)(nfs_fh *, struct svc_req *);

typedef struct node_op {
	lookup_t lookup;
	getattr_t getattr;
	read_t read;
	readlink_t readlink;
	readdir_t readdir;
	void *config;
} node_op;

typedef struct rfs_fh {
	node_op *fh_nodeop;
	void *fh_address;
	void *fh_parent;

	int fh_crc;
} rfs_fh;

char *new_fh(node_op *nodeop, void *address, void *parent); 
int check_fh(rfs_fh *);

#define FH_SET(t, m, a, p) bcopy(new_fh(m, a, p), t, NFS_FHSIZE)
#define RFS_FH(x) ((rfs_fh *)(x))

	
#endif /* __RFS_H__ */
