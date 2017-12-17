/* $Id: unicorn.h,v 1.4 2001/01/28 19:37:41 cvsuser Exp $
 *
 * 2000, unicorn
 */

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

