// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#if defined (WIN32)
#include <windows.h>
#include <io.h>
#else
#include <dirent.h>
#endif
#include "merc.h"
#include "recycle.h"
#include "interp.h"

#if defined(WIN32)
typedef struct dirent
{
  WIN32_FIND_DATA FindData;
  HANDLE Data;
  char   d_name[MAX_PATH+1]; // file's name
  BOOL   d_firstread;        // flag for 1st time
  int64  attrib;
} DIR;

DIR            *opendir (const char *dirname);
struct  dirent *readdir (DIR *dirstream);
int             closedir(DIR *dirstream);
#endif

char *pflag64(int64 flag)
{
  int count,pos=0;
  int64 tbit=1;
  static char buf[53];

  buf[0] = '\0';
  for (count = 0;count<52; count++)
  {
    if (IS_SET(flag,tbit))
    {
      if (count < 26) buf[pos]='A' + count;
      else            buf[pos]='a' + (count - 26);
      pos++;
    }
    tbit*=2;
  }
  buf[pos]='\0';
  if (EMPTY(buf))
  {
    buf[0]='0';
    buf[1]='\0';
  }
  return buf;
}

int64 fread_flag64( FILE *fp)
{
  int64 number=0;
  char c;
  bool negative = FALSE;

  do c = getc(fp);
  while (isspace(c));

  if (c == '-')
  {
    negative = TRUE;
    c = getc(fp);
  }

  if (!isdigit(c))
  {
    while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
    {
      number+=flag_convert64(c);
      c = getc(fp);
    }
  }

  while (isdigit(c))
  {
    number = number * 10 + c - '0';
    c = getc(fp);
  }

  if (c == '|') number += fread_flag64(fp);
  else if  ( c != ' ') ungetc(c,fp);

  if (negative) return -1 * number;
  return number;
}

void do_offline( CHAR_DATA *ch, const char *argument )
{
  DESCRIPTOR_DATA d;
  bool found = FALSE;
  char name[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int value;

  argument = one_argument(argument, name);
  name[0] = UPPER(name[0]);
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (EMPTY(name) || EMPTY(arg1))
  {
    stc("syntax: OFFLINE <name> <command>\n\r", ch);
    stc("Commands avaible:\n\r", ch);
    stc("   {CInfo\n\r",ch);

    if ( get_trust(ch)>101 )
    {
      stc("   {Cremoveqp    - забрать у персонажа qp\n\r",ch);
      stc("   deny        - изменить статутс deny\n\r",ch);
      stc("   tipsy       - изменить статус TIPSY\n\r", ch);
      stc("   nopost      - изменить статус NOPOST\n\r", ch);
      stc("   nochannels  - изменить статуст NOCHANNEL\n\r", ch);
      stc("   password    - узнать пароль персонажа\n\r", ch);
      stc("   stat        - узнать подробную информацию\n\r", ch);
      stc("   {Cclearpban   - очистить personal ban статус\n\r", ch);
      stc("   {Runload      - не пользоваться (для отладки)\n\r", ch);
      stc("   load        - не пользоваться (для отладки){x\n\r", ch);
    }
    if ( get_trust(ch)>106 )
    {
      stc("   {Caddqp       - добавить qp персонажу{x\n\r", ch);
    }
    if (get_trust(ch)>=107)
    {
      stc("   {Rdelete      - удалить персонаж{x\n\r", ch);
    }
    return;
  }

  if ( (get_trust(ch)) < 102) WAIT_STATE(ch,PULSE_VIOLENCE*3);
  if ( !IS_LETTER (name[0]) )
  {
    stc("Неправильные символы в имени.\n\r", ch);
    return;
  }

  if((victim=get_pchar_world(ch,name)))
  {
    if (!str_cmp(arg1,"unload"))
    {
      save_char_obj(victim);
      char_from_room(victim);
      extract_char(victim, TRUE);
      return;
    }
    else
    {
      stc("Этот персонаж сейчас в игре.\n\r",ch);
      return;
    }
  }

  found = load_char_obj(&d, name, SAVE_NORMAL);

  if (!found)
  {
    stc("Такого имени не зарегистрировано.\n\r", ch);
    return;
  }

  victim=d.character;

  if (!str_prefix(arg1,"info") || !IS_IMMORTAL (ch))
  {
    if ((IS_IMMORTAL(victim) && !IS_IMMORTAL (ch)) ||
        (IS_IMMORTAL(ch) && (get_trust(victim) > get_trust(ch))))
    {
      stc("Эта информация закрыта для разглашения :E.\n\r",ch);
      return;
    }
    ptc(ch, "{WВ последний раз этот персонаж заходил {Y%s{W{x\n\r",ctime(&victim->lastlogin));
    if (IS_IMMORTAL(ch) && get_trust(victim) < get_trust(ch))
      ptc(ch,"{CStatus: {RDeny:%s {RTipsy:%s {RNopost:%s {RNochannel:%s{x\n\r{CHost:%s{x\n\r",
      IS_SET(victim->act,PLR_DENY) ? "{Gon" : "{Doff",
      IS_SET(victim->act,PLR_TIPSY) ? "{Gon" : "{Doff",
      IS_SET(victim->act,PLR_NOPOST) ? "{Gon" : "{Doff",
      IS_SET(victim->comm,COMM_NOCHANNELS) ? "{Gon" : "{Doff",
      victim->host);
    if (IS_ELDER(ch) && victim->pcdata->denied_by)
      ptc(ch,"%s\n\r",victim->pcdata->denied_by);
    whois_info(ch,victim);
    extract_char(victim, TRUE );
    return;
  }

  // only info allowed for non-immortal characters. (C) Sab
  if ( !IS_IMMORTAL(ch)) return;

  if ( !str_cmp(arg1,"delete") && IS_DEITY(ch))
  {
#if defined(unix)
    char buf  [MAX_STRING_LENGTH];
    char buf1 [MAX_STRING_LENGTH];

    do_printf( buf, "%s%s", PLAYER_DIR , capitalize(victim->name) );
    do_printf( buf1,"%s%s", PLAYER_DIR2, capitalize(victim->name) );
    if( IS_DEVOTED_ANY(victim) ) dec_worship(victim);
    save_one_char(victim, SAVE_DELETED );
    extract_char(victim, TRUE );
    unlink(buf);
    unlink(buf1);
    ptc(ch,"{Y%s {Rполностью {xудален{x.\n\r",victim->name);
    do_printf(buf,"%s полностью удалил персонажа %s\n\r",ch->name,victim->name);
    send_note("{RSystem{x","elder","Character was offline deleted",buf,2);
#else
    stc("Эта команда работает только под Unix.\n\r",ch);
#endif
    return;
  }

  if (!str_prefix(arg1,"deny"))
  {
   char buf[256];
   if (IS_SET(victim->act, PLR_DENY))
   {
     REM_BIT(victim->act, PLR_DENY);
     ptc( ch, "{GRemoved Deny status from %s.{x\n\r", name );
   }
   else
   {
     SET_BIT(victim->act, PLR_DENY);
     ptc( ch, "{RSet Deny status to %s.{x\n\r", name );
     do_printf(buf,"Last Denied offline by %s (%s)",
       ch->name,ctime(&current_time));
     free_string(victim->pcdata->denied_by);
     victim->pcdata->denied_by=str_dup(buf);
   }
  }
/* offline guild will be appear someday
void do_guild( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CLAN_DATA *clan;
  int gn_old;
  int gn_new;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (IS_IMMORTAL(ch) && !str_prefix(arg2,"none"))
  {
    stc("Этот персонаж теперь вне клана.\n\r",ch);
    stc("Ты теперь вне клана!\n\r",victim);
    if (victim->clan && str_cmp(victim->clan->name,"loner"))
    {
      gn_old=group_lookup(victim->clan->name);
      if ( gn_old > -1) gn_remove(victim,gn_old);
     }
    victim->clan = NULL;
    return;
  }

  if ((IS_IMMORTAL(ch) || ELDER(ch,DWARVES_GUILD)) && !str_prefix(arg2, "dwarves"))
  {
    argument=one_argument(argument, arg2);
    if (victim->race!=RACE_DWARF)
    {
      stc("Только Гномы могут состоять в гильдии!\n\r",ch);
      return;
    }

    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,DWARVES_GUILD);
      if (ELDER(victim, DWARVES_GUILD))
      {
        ptc(ch, "%s теперь старейшина в Гильдии Гномов.\n\r",victim->name);
        SET_BIT(victim->pcdata->guild,DWARVES_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из старейшин Гильдии Гномов.\n\r",victim->name);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,DWARVES_GUILD);
    if (GUILD(victim, DWARVES_GUILD))
    {
      ptc(ch,"%s теперь состоит в Гильдии Гномов.\n\r",victim->name);
    }
    else
    {
      ptc(ch,"%s исключен из Гильдии Гномов.\n\r",victim->name);
      REM_BIT(victim->pcdata->elder,DWARVES_GUILD);
    }
    return;
  }

  if ((IS_IMMORTAL(ch) || ELDER(ch,DRUIDS_GUILD)) && !str_prefix(arg2, "druids"))
  {
    argument=one_argument(argument, arg2);
    if (victim->race!=RACE_DRUID)
    {
      stc("Только Друиды могут состоять в гильдии!\n\r",ch);
      return;
    }

    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,DRUIDS_GUILD);
      if (ELDER(victim, DRUIDS_GUILD))
      {
        ptc(ch, "%s теперь старейшина в Гильдии Друидов.\n\r",victim->name);
        SET_BIT(victim->pcdata->guild,DRUIDS_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из старейшин Гильдии Друидов.\n\r",victim->name);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,DRUIDS_GUILD);
    if (GUILD(victim, DRUIDS_GUILD))
    {
      ptc(ch,"%s теперь послушник в Гильдии Друидов.\n\r",victim->name);
    }
    else
    {
      ptc(ch,"%s исключен из Гильдии Друидов.\n\r",victim->name);
      REM_BIT(victim->pcdata->elder,DRUIDS_GUILD);
    }
    return;
  }

  if ( (IS_IMMORTAL(ch) || ELDER(ch,VAMPIRE_GUILD)) && !str_prefix(arg2, "vampire"))
  {
    argument=one_argument(argument, arg2);
    if (victim->race!=RACE_VAMPIRE)
    {
      stc("Только Вампиры могут состоять в гильдии!\n\r",ch);
      return;
    }
    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,VAMPIRE_GUILD);
      if (ELDER(victim, VAMPIRE_GUILD))
      {
        ptc(ch, "%s теперь Высший Вампир.\n\r",victim->name);
        SET_BIT(victim->pcdata->guild,VAMPIRE_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из Высших Вампиров.\n\r",victim->name);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,VAMPIRE_GUILD);
    if (GUILD(victim,VAMPIRE_GUILD))
    {
      ptc(ch, "%s теперь член гильдии Вампиров.\n\r",victim->name);
    }
    else
      ptc(ch, "%s исключен из гильдии Вампиров.\n\r",victim->name);

    if (!GUILD(victim,VAMPIRE_GUILD)) REM_BIT(victim->pcdata->elder,VAMPIRE_GUILD);
    return;
  }

  if ((clan = clan_lookup(arg2)) == NULL)
  {
    stc("Такого клана нет.\n\r",ch);
    return;
  }

  if (IS_SET(clan->flag, CLAN_LONER))
  {
    ptc(ch,"%s теперь %s.\n\r",victim->name,clan->name);
  }
  else
  {
    ptc(ch,"%s теперь член клана %s.\n\r",victim->name,capitalize(clan->name));
  }

  if ( (victim->clan == NULL) && (clan->name !="loner") )
   {
    gn_new=group_lookup(clan->name);
    if ( gn_new > -1) gn_add(victim,gn_new);
   }
  else if ( (victim->clan->name == "loner") && (clan->name !="loner") )
   {
    gn_new=group_lookup(clan->name);
    if ( gn_new > -1) gn_add(victim,gn_new);
   }
  else if ( (victim->clan->name != "loner") && (clan->name != "loner") )
   {
    gn_old=group_lookup(victim->clan->name);
    if (gn_old > -1) gn_remove(victim,gn_old);
    gn_new=group_lookup(clan->name);
    if (gn_new > -1) gn_add(victim,gn_new);
   }
  else if ( (victim->clan->name !="loner") && (clan->name == "loner") )
   {
    gn_old=group_lookup(victim->clan->name);
    if (gn_old > -1) gn_remove(victim,gn_old);
   }

  victim->clan = clan;
  victim->clanrank = 0;
  victim->clanpet=NULL;
}

*/

  if (!str_prefix(arg1,"nopost"))
  {
   if (IS_SET(victim->act, PLR_NOPOST))
   {
     REM_BIT(victim->act, PLR_NOPOST);
     ptc( ch, "{GRemoved Nopost status from %s.{x\n\r", name );
   }
   else
   {
     SET_BIT(victim->act, PLR_NOPOST);
     ptc( ch, "{RSet Nopost status to %s.{x\n\r", name );
   }
  }

  if (!str_prefix(arg1,"nochannels"))
  {
   if (IS_SET(victim->comm, COMM_NOCHANNELS))
   {
     REM_BIT(victim->comm, COMM_NOCHANNELS);
     ptc( ch, "{GRemoved Nochannels status from %s.{x\n\r", name );
   }
   else
   {
     SET_BIT(victim->comm, COMM_NOCHANNELS);
     ptc( ch, "{RSet Nochannels status to %s.{x\n\r", name );
   }
  }

  if (!str_prefix(arg1,"tipsy"))
  {
   if (IS_SET(victim->act, PLR_TIPSY))
   {
     REM_BIT(victim->act, PLR_TIPSY);
     ptc( ch, "{GRemoved Tipsy status from %s.{x\n\r", name );
   }
   else
   {
     SET_BIT(victim->act, PLR_TIPSY);
     ptc( ch, "{RSet Tipsy status to %s.{x\n\r", name );
   }
  }

  if (!str_prefix(arg1,"addqp") && get_trust(ch)>106)
  {
    if (is_number(arg2)) value=atoi(arg2);
    else {
           stc("Введите количество квест поинтов.\n\r",ch);
           return;
         }
    if ( value < 0 || victim->questpoints+value > 30000 )
    {
      stc("ты можешь добавить от 0 до 30000 за раз.\n\r",ch);
      return;
    }
    ptc(ch,"Персонаж {Y%s. Было: {Y%d {xquest points. Теперь {Y%d{x.\n\r",victim->name,victim->questpoints,victim->questpoints+value);
    victim->questpoints += value;
  }

  if (!str_prefix(arg1,"removeqp"))
  {
    if (is_number(arg2)) value=atoi(arg2);
    else {
           stc("Введите количество квест поинтов.\n\r",ch);
           return;
         }
    if ( value < 0 || victim->questpoints-value <0 )
    {
      stc("Ты не можешь снять больше чем есть.\n\r",ch);
      return;
    }
    ptc(ch,"Персонаж {Y%s {xбыло {Y%d {xquest points. Теперь {Y%d{x.\n\r",victim->name, victim->questpoints,victim->questpoints-value);
    victim->questpoints -= value;
  }

  if (!str_prefix(arg1,"password"))
  {
    ptc(ch,"Пароль у {Y%s{x: %s\n\r", victim->name, (get_trust(victim)<102)?(victim->pcdata->pwd):"FIG_TEBE_(C)Sab");
    extract_char(victim, TRUE );
    return;
  }

  if (!str_prefix(arg1,"stat"))
  {
    if (get_trust(victim)<get_trust(ch)) mstat_info(ch,victim);
    extract_char(victim, TRUE );
    return;
  }

  if (!str_cmp(arg1,"clearpban"))
  {
   ptc(ch,"Personal Ban очищен для {Y%s{x.\n\r",victim->name);
   free_string(victim->pcdata->deny_addr);
   save_char_obj(victim);
   extract_char(victim, TRUE );
   return;
  }

  if (!str_prefix(arg1,"load"))
  {
    stc("ЭТА КОМАНДА КРАШИТ МУД. ЗАРЭЖУ ТОГО, КТО ЕЕ СНОВА ЗАКОДИТ С ГЛЮКАМИ. (c)Sab.\n\r",ch);
    extract_char(victim, TRUE);
    return;
  }

  save_char_obj(victim);
  extract_char(victim, TRUE );
}

void add_pkiller(CHAR_DATA *ch, CHAR_DATA *killer)
{
 char buf[MAX_STRING_LENGTH];

 if (!ch || !killer) return;
 if (IS_NPC(ch) || IS_NPC(killer)) return;
 if (PK_RANGE(ch,killer) || IS_SET(killer->act,PLR_WANTED)) return;
 if (ch->pcdata->pkillers && is_exact_name(killer->name,ch->pcdata->pkillers)) return;

 if (ch->pcdata->pkillers==NULL) do_printf(buf,killer->name);
  else
  {
   do_printf(buf,ch->pcdata->pkillers);
   strcat (buf," ");
   strcat (buf,killer->name);
  }
 free_string(ch->pcdata->pkillers);
 ch->pcdata->pkillers=str_dup(buf);
}

void remove_pkiller(CHAR_DATA *ch, char *name)
{
 const char *arg;
 char arg1[MAX_INPUT_LENGTH];
 char arg2[MAX_INPUT_LENGTH];
 bool found=FALSE;

 arg2[0]='\0';
 if (!ch || IS_NPC(ch) ) return;
 if (!ch->pcdata->pkillers) return;
 if (!is_exact_name(name,ch->pcdata->pkillers)) return;

 arg=ch->pcdata->pkillers;

 for (;;)
 {
  arg=one_argument(arg, arg1);
  if (EMPTY(arg1)) break;
  if (str_cmp(arg1,name))
  {
    strcat(arg2,arg1);
    found=TRUE;
  }
 }
 free_string(ch->pcdata->pkillers);
 if (found) ch->pcdata->pkillers = str_dup(arg2);
 else ch->pcdata->pkillers = str_empty;
}

bool PK_RANGE(CHAR_DATA *ch, CHAR_DATA *victim)
{
// if (ch->level-victim->level > pk_range_table[ch->remort][victim->remort]) return FALSE;
 if (ch->level - victim->level > (MAX_PK_RANGE + 2*(victim->remort > 0)) ) return FALSE;
 return TRUE;
}

void add_stealer(CHAR_DATA *ch, CHAR_DATA *stealer)
{
 char buf[MAX_STRING_LENGTH];

 buf[0]='\0';
 if (ch==NULL || stealer==NULL ) return;
 if (!IS_NPC(ch) || IS_NPC(stealer)) return;
 if (ch->pIndexData==NULL || ch->pIndexData->pShop == NULL) return;
 if (ch->stealer!=NULL && is_exact_name(stealer->name,ch->stealer)) return;

 if (ch->stealer==NULL)
 {
   do_printf(buf,stealer->name);
   ch->stealer=str_dup(buf);
   return;
 }
 else
 {
   strcpy(buf,ch->stealer);
   strcat(buf," ");
   strcat(buf,stealer->name);
 }
   free_string(ch->stealer);
   ch->stealer=str_dup(buf);
}

void remove_one_stealer(CHAR_DATA *ch)
{
 const char *arg;
 char arg1[MAX_INPUT_LENGTH];

 if (!ch || !IS_NPC(ch) || !ch->stealer) return;

 arg=ch->stealer;
 arg=one_argument(arg, arg1);

 free_string(ch->stealer);
 if (!EMPTY(arg1)) ch->stealer=str_dup(arg);
}

void do_global( CHAR_DATA *ch, const char *argument )
{
 char arg[MAX_INPUT_LENGTH];
 register int cmd;

 if (EMPTY(argument))
 {
   ptc (ch,"Wizlock      :%s\n\r",IS_SET(global_cfg,CFG_WIZLOCK)? "{RYes{x":"{GNo{x");
   ptc (ch,"Newlock      :%s\n\r",IS_SET(global_cfg,CFG_NEWLOCK)? "{RYes{x":"{GNo{x");
   ptc (ch,"Random items :%s\n\r",(IS_SET(global_cfg,CFG_RANDOM)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"Global Quests:%s\n\r",(IS_SET(global_cfg,CFG_GQUEST)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"Birthday     :%s\n\r",(IS_SET(global_cfg,CFG_BIRTHDAY)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"New Year     :%s\n\r",(IS_SET(global_cfg,CFG_NEWYEAR)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"Debug Log    :%s\n\r",(IS_SET(global_cfg,CFG_DLOG)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"DamageObj    :%s\n\r",(IS_SET(global_cfg,CFG_DAMAGEOBJ)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"NewOneHit    :%s\n\r",(IS_SET(global_cfg,CFG_NEWHIT)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"NoDamroll    :%s\n\r",(IS_SET(global_cfg,CFG_NODR)) ? "{Gon{x" : "{Doff{x");
#ifdef WITH_ANTICRASH
   ptc (ch,"Anticrash    :%s\n\r",(IS_SET(global_cfg,CFG_ANTICRASH)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"Ignorecrash  :%s\n\r",(IS_SET(global_cfg,CFG_IGNORECRASH)) ? "{Gon{x" : "{Doff{x");
   ptc (ch,"Bugtrace     :%s\n\r",(IS_SET(global_cfg,CFG_BUGTRACE)) ? "{Gon{x" : "{Doff{x");
#endif
   for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    if (IS_SET(cmd_table[cmd].flag, DENY)) ptc (ch, "Deny  :{R%s{x\n\r", cmd_table[cmd].name);
   return;
 }

 argument=one_argument(argument,arg);

 if (!str_prefix(arg,"help"))
 {
   stc ("Global options usage:\n\r",ch);
   stc ("global save            - saving global configuration\n\r",ch);
   stc ("global wizlock         - disable login for non-immortal chars\n\r",ch);
   stc ("global newlock         - disable creating new chars\n\r",ch);
   stc ("global random          - enable or disable random items generating\n\r",ch);
   stc ("global gquest          - enable or disable automated global quests\n\r",ch);
   stc ("global newyear         - enable New Year mode\n\r",ch);
   stc ("global birthday        - enable birthday mode\n\r",ch);
   stc ("global commit          - commit areas\n\r",ch);
   stc ("global quenia          - show current quenia words\n\r",ch);
#ifdef WITH_ANTICRASH
   stc ("global anticrash       - enable anticrash mode\n\r",ch);
   stc ("global ignorecrash     - enable delayed reboot on crash\n\r",ch);
   stc ("global bugtrace        - enable crash report on bug\n\r",ch);
#endif
   stc ("global deny <command>  - disable command\n\r",ch);
   stc ("global update <list>   - force tick for events (empty arg for list)\n\r",ch);
   stc ("global bd              - edit birthday messages (empty arg for list)\n\r",ch);
   return;
 }

  if (!str_prefix(arg,"bd"))
  {
    int counter;

    argument=one_argument(argument,arg);
    if (EMPTY(arg) || !is_number(arg) )
    {
      for (counter=0;counter<10;counter++) ptc(ch,"%d %s\n\r",counter,bdmsg[counter]);
      return;
    }
    counter=atoi(arg);
    free_string(bdmsg[counter]);
    bdmsg[counter]=str_dup(argument);
    return;
  }

  if (!str_prefix(arg,"save"))
  {
    save_config();
    stc("{RКонфигурация сохранена.{x\n\r", ch);
    return;
  }

  if (!str_prefix(arg,"quenia"))
  {
    int i;

    argument=one_argument(argument,arg);
    if (!EMPTY(arg) && !str_prefix(arg,"reset"))
    {
      for (i=0;quenia_table[i].name!=Q_END;i++)
      {
        quenia_table[i].word[0]='\0';
        strcat(quenia_table[i].word,create_word());
        quenia_table[i].counter=quenia_table[i].start_counter;
      }
      stc("Words resets\n\r",ch);
    }
    for (i=0;quenia_table[i].start_counter!=255;i++)
      ptc(ch,"%d. %s [%3d] [%3d] [%s]\n\r",i+1,quenia_table[i].long_name,
       quenia_table[i].start_counter,quenia_table[i].counter,quenia_table[i].word);
    return;
  }

  if (!str_prefix(arg,"wizlock"))
  {
    global_cfg=toggle_int64(global_cfg,CFG_WIZLOCK);
    if (IS_SET(global_cfg,CFG_WIZLOCK))
    {
      wiznet("$C1 закрыл игру.",ch,NULL,0,0);
      stc( "Игра закрыта.\n\r", ch );
    }
    else
    {
      wiznet("$C1 открыл доступ к игре.",ch,NULL,0,0);
      stc( "Игра открыта для доступа.\n\r", ch );
    }
    return;
  }

  if (!str_prefix(arg,"debuglog"))
  {
    global_cfg=toggle_int64(global_cfg,CFG_DLOG);
    if (IS_SET(global_cfg,CFG_DLOG))
         wiznet("$C1 включил отладочный лог.",ch,NULL,0,0);
    else wiznet("$C1 выключил отладочный лог.",ch,NULL,0,0);
    ptc(ch,"Отладочный лог в%sключен.\n\r",IS_SET(global_cfg,CFG_DLOG)?"":"ы");
    return;
  }

  if (!str_prefix(arg,"newhit"))
  {
    global_cfg=toggle_int64(global_cfg,CFG_NEWHIT);
    ptc(ch, "Новая функция one_hit в%sключена.\n\r", IS_SET(global_cfg,CFG_NEWHIT)?"":"ы");
    return;
  }

  if (!str_prefix(arg,"nodr") || !str_prefix(arg,"nodamroll"))
  {
    global_cfg=toggle_int64(global_cfg,CFG_NODR);
    ptc(ch, "Влияние дамролла и зарезание физ. дамаджа в%sключены.\n\r", IS_SET(global_cfg,CFG_NODR)?"":"ы");
    return;
  }

  if (!str_prefix(arg,"damageobj"))
  {
    global_cfg=toggle_int64(global_cfg,CFG_DAMAGEOBJ);
    ptc(ch, "Повреждение объектов в%sключенo.\n\r", IS_SET(global_cfg,CFG_DAMAGEOBJ)?"":"ы");
    return;
  }

  if (!str_prefix(arg,"gquest"))
  {
    global_cfg=toggle_int64(global_cfg,CFG_GQUEST);
    ptc(ch,"Automated global quests turned %s.\n\r", IS_SET(global_cfg,CFG_GQUEST)?"on":"off");
    return;
  }

  if (!str_prefix(arg,"newlock"))
  {
    if (!IS_SET(global_cfg,CFG_NEWLOCK))
    {
      SET_BIT(global_cfg,CFG_NEWLOCK);
      wiznet("$C1 закрыл доступ новым персонажам.",ch,NULL,0,0);
      stc( "Игра закрыта для новых персонажей.\n\r", ch );
    }
    else
    {
      REM_BIT(global_cfg,CFG_NEWLOCK);
      wiznet("$C1 позволяет новым персонажам входить в игру.",ch,NULL,0,0);
      stc( "Игра открыта для новых персонажей.\n\r", ch );
    }
    return;
  }

  if (!str_prefix(arg,"birthday"))
  {
   if (IS_SET(global_cfg,CFG_BIRTHDAY)) REM_BIT(global_cfg,CFG_BIRTHDAY);
   else SET_BIT(global_cfg,CFG_BIRTHDAY);
   ptc(ch,"Birthday mode is %s\n\r",(IS_SET(global_cfg,CFG_BIRTHDAY)) ? "on" : "off");
   return;
  }

#ifdef WITH_ANTICRASH
  if (!str_prefix (arg, "anticrash"))
    {
      if (IS_SET (global_cfg, CFG_ANTICRASH))
        REM_BIT (global_cfg, CFG_ANTICRASH);
      else
        SET_BIT (global_cfg, CFG_ANTICRASH);

      death_hook();

      ptc (ch, "Anticrash mode is %s\n\r",
         (IS_SET (global_cfg, CFG_ANTICRASH)) ? "on" : "off");
      return;
    }

  if (!str_prefix (arg, "bugtrace"))
    {
      if (IS_SET (global_cfg, CFG_BUGTRACE))
        REM_BIT (global_cfg, CFG_BUGTRACE);
      else
        SET_BIT (global_cfg, CFG_BUGTRACE);

      death_hook();

      ptc (ch, "Bugtrace mode is %s\n\r",
         (IS_SET (global_cfg, CFG_BUGTRACE)) ? "on" : "off");
      return;
    }

  if (!str_prefix (arg, "ignorecrash"))
    {
      if (IS_SET (global_cfg, CFG_IGNORECRASH))
        REM_BIT (global_cfg, CFG_IGNORECRASH);
      else
        SET_BIT (global_cfg, CFG_IGNORECRASH);

      ptc (ch, "Ignorecrash mode is %s\n\r",
         (IS_SET (global_cfg, CFG_IGNORECRASH)) ? "on" : "off");
      return;
    }
#endif

  if (!str_prefix(arg,"newyear"))
  {
   if (IS_SET(global_cfg,CFG_NEWYEAR)) REM_BIT(global_cfg,CFG_NEWYEAR);
   else SET_BIT(global_cfg,CFG_NEWYEAR);
   ptc(ch,"New Year mode is %s\n\r",(IS_SET(global_cfg,CFG_NEWYEAR)) ? "on" : "off");
   return;
  }

  if (!str_prefix(arg,"random"))
  {
   if (IS_SET(global_cfg,CFG_RANDOM)) REM_BIT(global_cfg,CFG_RANDOM);
   else SET_BIT(global_cfg,CFG_RANDOM);
   ptc(ch,"Random items mode is %s\n\r",(IS_SET(global_cfg,CFG_RANDOM)) ? "on" : "off");
   return;
  }

  if (!str_prefix(arg,"update"))
  {
    CHAR_DATA *victim;
    argument=one_argument(argument,arg);
    if (EMPTY(arg))
    {
      stc("{Ckeys:{x weather char obj quest raffect area song mobile violence auction\n\r",ch);
      return;
    }
    victim=get_char_world(ch,argument);
    if (is_name(arg,"weather")) weather_update ();
    if (is_name(arg,"char"))    char_update    ();
    if (is_name(arg,"obj"))     obj_update     ();
    if (is_name(arg,"quest"))   quest_update   ();
    if (is_name(arg,"gquest"))  gquest_update  ();
    if (is_name(arg,"clan"))    clan_update    ();
    if (is_name(arg,"raffect")) raffect_update ();
    if (is_name(arg,"area"))    area_update    ();
    if (is_name(arg,"song"))    song_update    ();
    if (is_name(arg,"mobile"))  mobile_update  ();
    if (is_name(arg,"violence"))violence_update();
    if (is_name(arg,"auction")) auction_update ();
    if (is_name(arg,"penalty")) penalty_update (victim);
    stc("Updating..\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"deny"))
  {
    bool found = FALSE;
    char command[MAX_INPUT_LENGTH];

    argument=one_argument(argument, command);

    if (EMPTY(command))
    {
       for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
         REM_BIT(cmd_table[cmd].flag, DENY);
       stc("Все команды разблокированы.\n\r", ch);
       return;
    }


    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
      if ( command[0] == cmd_table[cmd].name[0] &&
          (  ( !IS_SET(cmd_table[cmd].flag, FULL)
          && !str_prefix( command, cmd_table[cmd].name) )
        || ( IS_SET(cmd_table[cmd].flag, FULL)
          && !str_cmp( command, cmd_table[cmd].name) ) )
          &&   cmd_table[cmd].level <= 110 )
      {
        found = TRUE;
        break;
      }
    }

    if (!found || !str_cmp(cmd_table[cmd].name, "global"))
    {
      stc("Команда не обнаружена.\n\r", ch);
    }
    else
    {
      if (IS_SET(cmd_table[cmd].flag, DENY)) REM_BIT(cmd_table[cmd].flag, DENY);
      else SET_BIT(cmd_table[cmd].flag, DENY);
      ptc(ch, "Команда %s %sблокирована.\n\r", cmd_table[cmd].name,
      IS_SET(cmd_table[cmd].flag, DENY)?"за":"раз");
    }
    return;
  }
}

#if defined( WIN32 )
void bcopy(char *s,char *d,register int n)
{
  while ((n--) > 0) *(d++) = *(s++);
}

DIR *opendir (const char *dirname)
{
  char path[MAX_PATH];
  int  cnt, dlen = strlen (dirname);
  DIR *dir;

  if (dlen==0) return NULL;
  dir = (DIR *) malloc (sizeof (DIR));
  if (!dir) return NULL;

  path[0]='\0';
  strcat(path, dirname);

  for (cnt=0; cnt<dlen; cnt++) if (path[cnt]=='/') path[cnt]='\\';

  dir->d_name[0]='\0';
  strcat(dir->d_name,path);

  if (path[dlen-1]=='\\') strcat (path, "*.*");
  else                    strcat (path, "\\*.*");

  dir->d_firstread=TRUE;
  dir->Data = FindFirstFile((LPCTSTR)(path), &(dir->FindData));

  if (dir->Data == INVALID_HANDLE_VALUE) // This may be don't correct...
  {
    free (dir);
    return NULL;
  }
  return dir;
}

struct dirent* readdir  (DIR *dirstream)
{
  if (!dirstream) return NULL;

  if (dirstream->d_firstread) dirstream->d_firstread=FALSE;
  else
  {
    if (!FindNextFile (dirstream->Data, &(dirstream->FindData))) return NULL;
  }
  strcpy (dirstream->d_name, dirstream->FindData.cFileName);
  dirstream->attrib=dirstream->FindData.dwFileAttributes;
  return (struct dirent *) dirstream;
}

int closedir (DIR *dirstream)
{
  if (!dirstream) return -1;
  FindClose (dirstream->Data);
  free (dirstream);
  return 0;
}
#endif

char *do_printf(char *buf, const char *fmt,...)
{
  char *p;
  char tmp[16];
  register int format;
  bool left=FALSE;
  bool fmtenable=FALSE;
  bool fMatch=FALSE;
  va_list data;
  va_start(data,fmt);

#include "printf.c"

  *buf = 0;
  va_end (data);
  return buf;
}

void do_fprintf(FILE *fp, const char *fmt,...)
{
  char buff [MAX_STRING_LENGTH];
  char * buf;
  bool left=FALSE;
  bool fMatch=FALSE;
  bool fmtenable=FALSE;
  char *p;
  char tmp[16];
  register int format;
  va_list data;
  va_start(data,fmt);
  memset (buff, 0, MAX_STRING_LENGTH) ;
  buf = buff ;

#include "printf.c"

  *buf = 0;
  va_end (data);
  fprintf(fp,"%s",buff);
}

void dlog(const char *fmt,...)
{
  FILE *fp;
  char logname[32];
  char ttime[MAX_STRING_LENGTH];
  char buff [MAX_STRING_LENGTH];
  char *buf=buff;
  char *strtime=ttime;
  bool left=FALSE;
  bool fMatch=FALSE;
  bool fmtenable=FALSE;
  char *p;
  char tmp[16];
  register int format;
  va_list data;
  va_start(data,fmt);
  memset (buff, 0, MAX_STRING_LENGTH) ;

  if (!IS_SET(global_cfg,CFG_DLOG)) return;

  strftime(logname,28,"../log/full_log_%y%m%d.log",localtime(&current_time));
  strftime(ttime,21,"%y%m%d %a %H:%M:%S:",localtime(&current_time));
#include "printf.c"
  *buf = 0;
  va_end (data);
  strcat(strtime,buff);
  strcpy(log_buf,buff); /* @#$... (unicorn) */
  strcat(strtime,"\n");

  fclose(logReserve);
  fp = fopen (logname, "a+b");
  if (fp != NULL)
  {
    fprintf(fp,"%s", strtime);
    fclose(fp);
  }
  logReserve = fopen (LOGR_FILE, "r");
}

void log_printf(const char *fmt,...)
{
  FILE *fp;
  char logname[32];
  char buff [MAX_STRING_LENGTH];
  char ttime[MAX_STRING_LENGTH];
  char *buf=buff;
  char *strtime=ttime;
  bool left=FALSE;
  bool fMatch=FALSE;
  bool fmtenable=FALSE;
  char *p;
  char tmp[16];
  register int format;
  va_list data;
  va_start(data,fmt);
  memset (buff, 0, MAX_STRING_LENGTH) ;

  strftime(logname,28,"../log/full_log_%y%m%d.log",localtime(&current_time));
  strftime(ttime,21,"%y%m%d %a %H:%M:%S:",localtime(&current_time));
#include "printf.c"

  *buf = 0;
  va_end (data);
  strcat(strtime,buff);
  strcpy(log_buf,buff); /* @#$... (unicorn) */
  strcat(strtime,"\n");

  fclose(logReserve);
  fp = fopen (logname, "a+b");
  if (fp != NULL)
  {
    fprintf(fp,"%s", strtime);
    fclose(fp);
  }
  logReserve = fopen (LOGR_FILE, "r");
}

char *number_string(int64 number)
{
  static char buf[MAX_STRING_LENGTH];
  char *p;

  p=buf;
  if (number < 0)
  {
    *(p++) = '-';
    number = -number;
  }
  do
  {
    *(p++) = '0' + (char)(number%10);
    number/=10;
  } while(number);
  return buf;
}

int toggle_int(int flag,int bit)
{
  if (IS_SET(flag,bit)) REM_BIT(flag,bit);
  else SET_BIT(flag,bit);
  return flag;
}

int64 toggle_int64(int64 flag,int64 bit)
{
  if (IS_SET(flag,bit)) REM_BIT(flag,bit);
  else SET_BIT(flag,bit);
  return flag;
}

void do_player(CHAR_DATA *ch, const char *argument)
{
  DIR *folder=opendir(PLAYER_DIR);
  struct dirent *dir;
  CHAR_DATA *vch;
  DESCRIPTOR_DATA d;
  int cnt=0;

  if (!folder)
  {
    ptc(ch,"Player directory not found!\n\r",argument);
    return;
  }
  ptc(ch,"Opening {G%s{x.\n\r",PLAYER_DIR);
  for (dir=readdir(folder);dir;dir=readdir(folder))
  {
#if defined(WIN32)
    if (IS_SET(dir->attrib,SUB_DIR)) continue;
#else
    if (dir->d_type == DT_DIR) continue;
#endif
    if (!load_char_obj(&d,dir->d_name,SAVE_NORMAL))
    {
      ptc(ch,"{RBAD{x.{Y%12s{x  ",dir->d_name);
      extract_char(d.character,TRUE);
      cnt=0;
      continue;
    }
    vch=d.character;
    ptc(ch,  "{G%3d{x.{Y%12s{x  ",vch->level,vch->name);
    if (++cnt>=5)
    {
      stc("\n\r",ch);
      cnt=0;
    }
    extract_char(vch,TRUE);
  }
  closedir(folder);
}

void do_version( CHAR_DATA *ch, const char *argument )
{
  stc("\n\r{CForgotten Dungeon. {YCurrent version: {G4.90.00 (Saboteur){x\n\r",ch);
}
