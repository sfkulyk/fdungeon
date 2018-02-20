/* $Id: cmd.h,v 1.8 2002/07/21 18:24:21 saboteur Exp $
 *
 * 2000, unicorn
 */

#ifndef __CMD_H__
#define __CMD_H__

#include "init.h"
#include "sortedlist.h"
#include <sys/types.h>
#include <stdio.h>
// #include <merc.h>

typedef void (*cmd_fn_t)(CHAR_DATA *ch, const char *arg);

struct command;

SORTEDLIST_HEAD(cmds_head, command);

extern struct cmds_head cmds;

struct command {
	SORTEDLIST_ENTRY(command) cm_list;
	int cm_nice;
	struct cmd_type cmd; /* from merc.h */
	char *cm_descr;
};

#define CMDS_EMPTY SORTEDLIST_EMPTY(&cmds)
#define CMDS_FIRST SORTEDLIST_FIRST(&cmds)
#define CMDS_FOREACH(var) SORTEDLIST_FOREACH(var, &cmds, cm_list)
#define CMDS_INSERT(elm) SORTEDLIST_INSERT(&cmds, elm, command, cm_list)
#define CMDS_REMOVE(elm) SORTEDLIST_REMOVE(&cmds, elm, command, cm_list)

/* command definition... */

#define CMD(name, prio, rname, pos, level, log, flag, descr) \
static void __ ## name ## _cmd_do_fn(CHAR_DATA *, const char *); \
static struct command __ ## name ## _cmd = { \
	{NULL}, \
	prio, \
	{ \
		#name, \
		rname, \
		__ ## name ## _cmd_do_fn, \
		pos, \
		level, \
		log, \
		flag \
	}, \
	descr \
}; \
CTOR(name ## _cmd_ctor) { CMDS_INSERT(&__ ## name ## _cmd); } \
DTOR(name ## _cmd_dtor) { CMDS_REMOVE(&__ ## name ## _cmd); } \
static void __ ## name ## _cmd_do_fn(CHAR_DATA *ch, const char *argument)

#define ALIAS(oldname, name, rname, prio, pos, level, log, flag, descr) \
static void __ ## oldname ## _cmd_do_fn(CHAR_DATA *, const char *); \
static struct command __ ## name ## _cmd = { \
	{NULL}, \
	prio, \
	{ \
		#name, \
		rname, \
		__ ## oldname ## _cmd_do_fn, \
		pos, \
		level, \
		log, \
		flag \
	}, \
	descr \
}; \
CTOR(name ## _cmd_ctor) { CMDS_INSERT(&__ ## name ## _cmd); } \
DTOR(name ## _cmd_dtor) { CMDS_REMOVE(&__ ## name ## _cmd); }

#endif /* __CMD_H__ */
