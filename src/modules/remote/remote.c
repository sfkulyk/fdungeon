/* $Id: remote.c,v 1.4 2002/04/26 07:53:59 saboteur Exp $
 * 2001, unicorn
 * 2002, upgraded by Saboteur
 */

#include <sys/types.h>
#include <stdio.h>

#include <merc.h>

#include <mod.h>
#include <rpc.h>

#include "remote.h"

extern int reboot_reason;

void rrprog_1(struct svc_req *, register SVCXPRT *);

MOD(remote, 50, "RPC Remote controll.")

RPC(RRPROG, RRVERS, rrprog_1, remote, "Remote Control")

void *
reboot_1_svc(reboot_args *argp, struct svc_req *rqstp)
{
  static char * result = "Nothing.";
  DESCRIPTOR_DATA *d;

  if( rqstp->rq_xprt->xp_raddr.sin_addr.s_addr != 0x100007f )
  {
    svcerr_auth(rqstp->rq_xprt, AUTH_TOOWEAK);
    return NULL;
  }

  reboot_reason = argp->reason;

  if(argp->count)
  {
    rebootcount = argp->count;

    for(d = descriptor_list; d ;d = d->next)
      if( d->connected == CON_PLAYING )
      {
        ptc(d->character, "{RSystem: REBOOT in %d ticks!{x{*\n\r", rebootcount);
      }
  }
  else do_reboot(NULL, "now");

  return((void *) &result);
}
void *
tell_1_svc(tell_args *argp, struct svc_req *rqstp)
{
  static char * result = "Nothing.";
  DESCRIPTOR_DATA *d;

  if( rqstp->rq_xprt->xp_raddr.sin_addr.s_addr != 0x100007f )
  {
    svcerr_auth(rqstp->rq_xprt, AUTH_TOOWEAK);
    return NULL;
  }

  for(d = descriptor_list; d ;d = d->next)
  {
    if (d->connected!=CON_PLAYING) continue;
    if (!d->character) continue;
    if (!*argp->to || is_name(d->character->name,argp->to))
    {
      ptc(d->character, "%s%s\r\n",
        get_trust(d->character)>101?"Romtell: ":"", argp->text);
    }
    else
    {
      if (is_name(argp->to,"imms") && get_trust(d->character)>101)
        ptc(d->character,"Romtell: %s\n\r",argp->text);
    }
  }
  return((void *) &result);
}
