// Copyrights (C) 1998-2003, Forgotten Dungeon team.
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

void web_update()
{
  DESCRIPTOR_DATA * d ;
  FILE *fp;
  static char web_class_names[10][36] = {
    "<font color=blue>Hero  </font>\0",
    "<font color=blue>Avatar  </font>\0",
    "<font color=blue>Angel   </font>\0",
    "<font color=blue>ArhAngel</font>\0",
    "<font color=cyan>Immortal</font>\0",
    "<font color=green>DemiGod </font>\0",
    "<font color=green>GOD     </font>\0",
    "<font color=magenta>SUPERIOR</font>\0",
    "<font color=red>CREATOR </font>\0",
    "<font color=red>IMPLEMENTOR </font>\0"};

  if (fname_who)
  {
    if ( ( fp = fopen( fname_who, "w" ) ) == NULL )
    {
      perror( fname_who );
    }
    do_fprintf(fp,"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><title> то онлайн</title></head><body bgcolor=BLACK text=WHITE vlink=BLUE link=BLUE><hr><pre>");

    for (d = descriptor_list ; d != NULL ; d = d->next)
    {
      CHAR_DATA * wch   ;
      char      * class ;

      if (d->connected != CON_PLAYING) continue ; // skip disconnected

      // don't use trust as that exposes trusted mortals
      if (!d->character) continue ;
      wch = d->character ;

      if (wch->level < 102) class=classname(wch);
      else class = web_class_names[wch->level-101];

      do_fprintf (fp, "%3d %12s %s</font>] %s",
               wch->level, race_wname (wch), class,
               wch->clan ? wch->clan->show_name : "                ");

      do_fprintf (fp, "%s%s%s %s %s</font>\n\r",
               IS_SET(wch->act, PLR_WANTED) ? "<font color=red>(W)</font>" : "",
               IS_SET(wch->act, PLR_RAPER)  ? "<font color=red>(Н)</font>" : "",
               wch->godcurse                ? "<font color=grey>(C)</font>" : "",
               wch->name, IS_NPC(wch) ? "" : wch->pcdata->title) ;
    }
    do_fprintf(fp,"</pre><hr>");
    fclose (fp);
  }
}
