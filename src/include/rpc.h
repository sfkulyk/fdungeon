/* $Id: rpc.h,v 1.2 2001/01/28 19:32:58 cvsuser Exp $
 *
 * 2000, unicorn
 */

#ifndef __RPC_H__
#define __RPC_H__

#include <sys/queue.h>
#include <rpc/rpc.h>
#include <errno.h>

typedef struct rpc {
	LIST_ENTRY(rpc) rpc_list;

	int num;
	int ver;
	const char *name;
	const char *desc;
} rpc;

extern LIST_HEAD(rpc_head, rpc) rpc_head;

extern SVCXPRT *rpc_transp;

#define RPC_PROG(p, v, n, d) \
static void __ ## n ## _rpc_prog (struct svc_req *, register SVCXPRT *); \
RPC(p, v, __ ## n ## _rpc_prog, n, d) \
static void __ ## n ## _rpc_prog (struct svc_req *rqstp, register SVCXPRT *transp)

#define RPC(p, v, f, n, d) \
static struct rpc __ ## n ## _rpc = { \
	{NULL}, p, v, #n, d \
}; \
CTOR( __ ## n ## _rpc ) { \
	if(svc_register(rpc_transp, p, v, f, IPPROTO_UDP)) \
		LIST_INSERT_HEAD(&rpc_head, &__ ## n ## _rpc, rpc_list); \
	else { \
		if(mod_env.actor) { \
			mod_env.ok = 0; \
			ptc(mod_env.actor, "svc_register: %s: ", strerror(errno)); \
		} \
		log_printf("svc_register: %s: ", strerror(errno)); \
	} \
} \
DTOR( __ ## n ## _rpc ) { \
	(void)svc_unregister(p, v); \
	if(__ ## n ## _rpc . rpc_list . le_prev) \
		LIST_REMOVE(&__ ## n ## _rpc, rpc_list); \
}

#endif /* __RPC_H__ */
