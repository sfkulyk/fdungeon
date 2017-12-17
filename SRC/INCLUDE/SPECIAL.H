/* $Id: special.h,v 1.2 2002/04/25 13:18:20 saboteur Exp $
 *
 * 2000, unicorn
 */

#ifndef __SPECIAL_H__
#define __SPECIAL_H__

#include <sys/types.h>
#include <stdio.h>
#include <sys/queue.h>
#include <merc.h>
#include <merc_str.h>

#include "init.h"

typedef void (*spec_fn_t)(CHAR_DATA *ch);

struct special;

SLIST_HEAD(specs_head, special);

extern struct specs_head specs;

struct special {
	SLIST_ENTRY(special) sp_list;
	char *sp_name;
	spec_fn_t sp_fun;
	char *sp_descr;
};

#define SPECIALS_EMPTY SLIST_EMPTY(&specs)
#define SPECIALS_FIRST SLIST_FIRST(&specs)
#define SPECIALS_FOREACH(var) SLIST_FOREACH(var, &specs, sp_list)
#define SPECIALS_INSERT(elm) SLIST_INSERT_HEAD(&specs, elm, sp_list)
#define SPECIALS_REMOVE(elm) SLIST_REMOVE(&specs, elm, special, sp_list)

/* special definition... */

#define SPECIAL(name, descr) \
static void __ ## name ## _spec_do_fn(CHAR_DATA *); \
static struct special __ ## name ## _spec = { \
	{NULL}, \
	#name, \
	__ ## name ## _spec_do_fn, \
	descr \
}; \
CTOR(name ## _spec_ctor) { SPECIALS_INSERT( &__ ## name ## _spec ); } \
DTOR(name ## _spec_dtor) { SPECIALS_REMOVE( &__ ## name ## _spec ); } \
static void __ ## name ## _spec_do_fn(CHAR_DATA *ch)

#endif /* __SPECIAL_H__ */

