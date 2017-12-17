/* $Id: cmd.c,v 1.1.1.1 2001/01/22 12:41:33 cvsuser Exp $
 * 
 * 2000, unicorn
 */

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <merc.h>

#include "cmd.h"
#include "mod.h"


MOD(cmd, 100, "Command interpreter.");

int cmd_cmp(struct command *key1, struct command *key2) {
	if(key1->cm_nice < key2->cm_nice)
		return -1;
	else
		if(key1->cm_nice > key2->cm_nice)
			return 1;
		else
			return strcasecmp(key1->cmd.name, key2->cmd.name);
}

struct cmds_head cmds = { &cmd_cmp, 0};

