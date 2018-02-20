/* $Id: special.c,v 1.1 2001/01/28 04:12:47 cvsuser Exp $
 * 
 * 2000, unicorn
 */

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <merc.h>

#include "special.h"
#include "mod.h"


MOD(spec, 100, "Base for loadable specials.");

struct specs_head specs = {0};

