// $Id$
// Copyrights (C) 2006, Saboteur
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "fdweb.h"


void write_who()
{
  DESCRIPTOR_DATA * d ;
  FILE *fp;

  if ( ( fp = fopen( fname_who, "w" ) ) == NULL )
  {
    perror( fname_who );
  }

  do_fprintf(fp,"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><title>Кто онлайн</title></head><body bgcolor=BLACK text=WHITE vlink=BLUE link=BLUE><hr><pre>");

  // loop over all descriptors
  for (d = descriptor_list ; d != NULL ; d = d->next)
  {
    CHAR_DATA * wch   ;
    char      * class ;

    // do not account players in disconnect
    if (d->connected != CON_PLAYING) continue ;

    // check for match against restrictions
    // don't use trust as that exposes trusted mortals
    wch = d->character ;

    // check for null pointer
    if (!wch) continue ;

    switch (wch->level)
    {
      default:            class = classname(wch) ; break ;
      case 110 - 0: class = "<font color=grey> ’‚Ћђ…– </font>" ; break ;
      case MAX_LEVEL - 1: class = "<font color=red>CREATOR </font>" ; break ;
      case MAX_LEVEL - 2: class = "<font color=magenta>SUPERIOR</font>" ; break ;
      case MAX_LEVEL - 3: class = "<font color=green>GOD     </font>" ; break ;
      case MAX_LEVEL - 4: class = "<font color=green>DemiGod </font>" ; break ;
      case MAX_LEVEL - 5: class = "<font color=cyan>Immortal</font>" ; break ;
      case MAX_LEVEL - 6: class = "<font color=blue>ArhAngel</font>" ; break ;
      case MAX_LEVEL - 7: class = "<font color=blue>Angel   </font>" ; break ;
      case MAX_LEVEL - 8: class = "<font color=blue>Avatar  </font>" ; break ;
    }

    do_fprintf (fp, "%3d %12s %15s</font>] %s",
               wch->level, race_wname (wch), class,
               wch->clan ? wch->clan->show_name : "                ");

    do_fprintf (fp, "%s%s%s %s %s</font>\n\r",
               IS_SET(wch->act, PLR_WANTED) ? "<font color=red>(W)</font>" : "",
               IS_SET(wch->act, PLR_RAPER)  ? "<font color=red>(Ќ)</font>" : "",
               wch->godcurse                ? "<font color=grey>(C)</font>" : "",
               wch->name, IS_NPC(wch) ? "" : wch->pcdata->title) ;

  }
  do_fprintf(fp,"</pre><hr>");
  fclose (fp);
}

void web_update()
{
  if (fname_who)
  {
    write_who();
    log_printf("Updated who file on web [%s].", fname_who);
  }

}