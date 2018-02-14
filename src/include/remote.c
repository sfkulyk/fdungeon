/* $Id: remote.x,v 1.1 2001/01/28 19:32:57 cvsuser Exp $
 *
 * 2001, unicorn
 */
%
%/* $Id: remote.x,v 1.1 2001/01/28 19:32:57 cvsuser Exp $
% * 
% * 2001, unicorn
% */
%
%/*
% * Remote controll for merc.
% */
%
#ifndef RPC_HDR
%static const char *rcsid = "$Id: remote.x,v 1.1 2001/01/28 19:32:57 cvsuser Exp $";
#endif

const MAX_NAME		= 1024;
const MAX_MSGLEN	= 1024;

typedef string msg_t<MAX_MSGLEN>;
typedef string name_t<MAX_NAME>;

struct tell_args {
	name_t to;
	msg_t text;
};

struct reboot_args {
	int count;
	int reason;
};

program RRPROG {
	version RRVERS {
		void tell(tell_args) = 1;
		void reboot(reboot_args) = 2;
	} = 1;
} = 110000;

