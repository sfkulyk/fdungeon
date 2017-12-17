/* $Id: mod.h,v 1.7 2002/07/21 18:55:29 saboteur Exp $
 *
 * 2000, unicorn
 */

#ifndef __MOD_H__
#define __MOD_H__

#include "init.h"

#include "sortedlist.h"

#include <sys/types.h>
#include <sys/queue.h>
#include <stdio.h>
// #include <merc.h>

/* Defenitions for modules list */
struct module;

SORTEDLIST_HEAD(mods_head, module);

extern struct mods_head mods;

struct module {
 SORTEDLIST_ENTRY(module) md_list;
	
 const char *md_fname;
 const char *md_name;
 int md_nice;
	
 void *md_handle;
 const char *md_descr;
};

extern struct module *last_module;

#define MODS_EMPTY SORTEDLIST_EMPTY(&mods)
#define MODS_FIRST SORTEDLIST_FIRST(&mods)
#define MODS_FOREACH(var) SORTEDLIST_FOREACH(var, &mods, md_list)
#define MODS_INSERT(elm) SORTEDLIST_INSERT(&mods, elm, module, md_list)
#define MODS_REMOVE(elm) SORTEDLIST_REMOVE(&mods, elm, module, md_list)

/* module definition... */

#define MOD(name, prio, descr) \
static struct module __ ## name ## _mod = { \
	{NULL}, \
	NULL, \
	#name, \
	prio, \
	0, \
	descr \
}; \
CTOR(name ## _mod_ctor) { \
	MODS_INSERT(&__ ## name ## _mod); \
	mod_env.module = &__ ## name ## _mod; \
} \
DTOR(name ## _mod_dtor) { \
	MODS_REMOVE(&__ ## name ## _mod); \
}

extern struct mod_env {
 struct module *module;
 CHAR_DATA *actor;
 int ok;
} mod_env;

struct module *mods_lookup(const char *name);
int dl_load(const char *, CHAR_DATA *);
int dl_unload(const char *, CHAR_DATA *);
int dl_reload(const char *, CHAR_DATA *);

#endif /* __MOD_H__ */

