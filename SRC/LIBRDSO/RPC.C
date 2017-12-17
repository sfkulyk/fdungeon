/* $Id: rpc.c,v 1.2 2001/01/27 07:45:55 cvsuser Exp $
 *
 * 2000, unicorn
 */


#include <sys/types.h>
#include <stdio.h>

#include <merc.h>
#include <string.h>

#include "mod.h"
#include "cmd.h"
#include "rpc.h"

#include "rpc.msg"

MOD(rpc, 100, "RPC mapper.");

struct rpc_head rpc_head = LIST_HEAD_INITIALIZER(rpc_head);

SVCXPRT *rpc_transp = 0;

CTOR(rpc_transp) {
	rpc_transp = svcudp_create(RPC_ANYSOCK);
}

CMD(lsrpc, 0, "unknown", POS_DEAD, 0, LOG_NEVER, MORPH|HIDE|NOMOB|NOLOG, "List RPC services") {
	rpc *rpc;

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc(MSG_AUTH_WEEK, ch);
		return;
	}

	stc("Id     Ver    Name       Descr\n\r", ch);
	stc("--------------------------------------\n\r", ch);
	LIST_FOREACH(rpc, &rpc_head, rpc_list)
		ptc(ch, "%6d %6d %10s %40s\n\r",
			rpc->num,
			rpc->ver,
			rpc->name,
			rpc->desc);

	return;
}
