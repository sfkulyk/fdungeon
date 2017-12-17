/* $Id: mod.c,v 1.5 2002/07/21 18:55:29 saboteur Exp $
 *
 * 2000, unicorn
 */

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <merc.h>

#include "mod.h"
#include "cmd.h"

#include "mod.msg"

MOD(mod, 100, "DSO handling module.");

struct mod_env mod_env = { 0, 0, 0 };

int mod_cmp(struct module *key1, struct module *key2) {
	if(key1->md_nice < key2->md_nice)
		return -1;
	else
		if(key1->md_nice > key2->md_nice)
			return 1;
		else
			return strcasecmp(key1->md_name, key2->md_name);
}

struct mods_head mods = { &mod_cmp, 0};

int dl_load(const char *pathname, CHAR_DATA *ch) {
	void *a;

	mod_env.module = NULL;
	mod_env.actor = ch;
	mod_env.ok = 1;

	a = dlopen(pathname, RTLD_LAZY);
	if(!a) {
		const char *e = dlerror();
		
		log_printf("dlopen: %s", e);
		if(ch)
			ptc(ch, "dlopen: %s\n\r", e);
		return -1;
	}
	
	if(!mod_env.module) {
		log_printf("dl_load: module does not appeared to be registered.");
		if(ch)
			ptc(ch, "dl_load: module does not appeared to be registered.\n\r");
		dlclose(a);
		return -1;
	}
	
	mod_env.module->md_handle = a;
	mod_env.module->md_fname = strdup(pathname);
	
	if(!mod_env.ok) {
		log_printf("insmod: module init failed.");
		if(ch)
			ptc(ch, "insmod: module init failed.\n\r");
		dlclose(a);
		return -1;
	}

	log_printf("insmod: module %s loaded.", mod_env.module->md_name);
	if(ch)
		ptc(ch, "insmod: module %s loaded.\n\r", mod_env.module->md_name);

	mod_env.module = 0;
	mod_env.ok = 0;
	mod_env.actor = 0;

	return 0;
}

int dl_unload(const char *modname, CHAR_DATA *ch) {
	struct module *mod;

	mod = mods_lookup(modname);

	mod_env.actor = ch;

	if(mod) {
		if(mod->md_handle) {
			log_printf("Unloading %s.", mod->md_name);
			if(ch)
				ptc(ch, "Unloading %s.\n\r", mod->md_name);
			
			/*
			 * Carefull with order.
			 * After dlclose `mod' is undefined.
			 */
			free((void*)mod->md_fname);
			dlclose(mod->md_handle);
			
			mod_env.actor = 0;
			return 0;
		} else {
			log_printf("unload: %s: is a static module.", mod->md_name);
			if(ch)
				ptc(ch, "unload: %s: is a static module.\n\r", mod->md_name);
		}
	} else {
		log_printf("unload: %s: not loaded.", modname);
		if(ch)
			ptc(ch, "unload: %s: not loaded.\n\r", modname);
	}

	mod_env.actor = 0;
	return -1;
}

int dl_reload(const char *modname, CHAR_DATA *ch) {
	struct module *mod;
	char *modpath = NULL;
	int error = 0;

	mod = mods_lookup(modname);
	if(mod)
		modpath = strdup(mod->md_fname);

	if(modpath)
		error = (dl_unload(modname, ch) < 0 || dl_load(modpath, ch) < 0);
	else {
		log_printf("dl_reload: can't reload %s.", modname);
		if(ch)
			ptc(ch, "dl_reload: can't reload %s.\n\r", modname);
		error = 1;
	}

	free(modpath);
	return error ? -1 : 0;
}

struct module *mods_lookup(const char *name) {
	struct module *mod;

	MODS_FOREACH(mod) {
		if((mod->md_nice < 100 && !str_prefix(name, mod->md_name))
			|| !str_cmp(name, mod->md_name))
			return mod;
	}

	return 0;
}

CMD(lsmod, 50, "unknown", POS_DEAD, 0, LOG_NEVER, MORPH|HIDE|NOMOB|NOLOG, "List loaded modules.") {
	struct module *mod;

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc(MSG_AUTH_WEEK, ch);
		return;
	}
	
	ptc(ch, "DSO nice name       description                  \n\r");
	ptc(ch, "-------------------------------------------------\n\r");
	MODS_FOREACH(mod)
		ptc(ch, "%-3s %4d %-10s %-40s\n\r", 
				mod->md_handle ? "Yes" : "No", 
				mod->md_nice,
				mod->md_name,
				mod->md_descr);
}

/* this should be synchronized in case of threaded version */
CMD(insmod, 99, "unknown", POS_DEAD, 0, WIZ_NOTES, MORPH|HIDE|NOMOB|NOLOG,  "Load module.") {
	char pathname[MAX_INPUT_LENGTH];

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc(MSG_AUTH_WEEK, ch);
		return;
	}
	
	argument = one_argument(argument, pathname);
	
	if(!*pathname) {
		ptc(ch, "Usage: insmod <file_name>\n\r");
		return;
	}

	dl_load(pathname, ch);

	return;
}

CMD(rmmod, 99, "unknown", POS_DEAD, 0, WIZ_NOTES, MORPH|HIDE|NOMOB|NOLOG,  "Load module.") {
	char modname[MAX_INPUT_LENGTH];

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc(MSG_AUTH_WEEK, ch);
		return;
	}
	
	argument = one_argument(argument, modname);

	if(!*modname) {
		ptc(ch, "Usage: rmmod <module_name>\n\r");
		return;
	}

	dl_unload(modname, ch);

	return;
}

CMD(rlmod, 99, "unknown", POS_DEAD, 0, WIZ_NOTES, MORPH|HIDE|NOMOB|NOLOG,  "ReLoad module.") {
	char modname[MAX_INPUT_LENGTH];

	if( get_trust(ch) < 109 && !IS_SET(ch->comm, COMM_CODER) ) {
		stc(MSG_AUTH_WEEK, ch);
		return;
	}
	
	argument = one_argument(argument, modname);

	if(!*modname) {
		ptc(ch, "Usage: rlmod <module_name>\n\r");
		return;
	}

	dl_reload(modname, ch);

	return;
}
