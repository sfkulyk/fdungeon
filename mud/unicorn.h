// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#ifndef __UNICORN_H__
#define __UNICORN_H__


#ifdef unix

#include <cmd.h>
#include <mod.h>

#define WITH_RPC
#define WITH_DSO
#define WITH_ANTICRASH
#endif

#ifdef WITH_ANTICRASH
void death_hook();
char *gdb_ipc(char *arg);
#endif

#endif /* __UNICORN_H__ */

