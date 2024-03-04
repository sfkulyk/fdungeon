// $Id: olc_mpcode.c,v 1.12 2004/07/29 06:39:14 mud Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"

DECLARE_OLC_FUN( mpedit_create );
DECLARE_OLC_FUN( mpedit_code   );
DECLARE_OLC_FUN( mpedit_show   );
DECLARE_OLC_FUN( mpedit_clear  );
DECLARE_OLC_FUN( mpedit_help   );
extern const struct keywords_type fn_evals[];
extern const struct keywords_type fn_keyword[];

char *mprog_type_to_name( int64 type )
{
  switch ( type )
  {
    case TRIG_ACT:    return "ACT";
    case TRIG_SPEECH: return "SPEECH";
    case TRIG_RANDOM: return "RANDOM";
    case TRIG_FIGHT:  return "FIGHT";
    case TRIG_HPCNT:  return "HPCNT";
    case TRIG_DEATH:  return "DEATH";
    case TRIG_ENTRY:  return "ENTRY";
    case TRIG_GREET:  return "GREET";
    case TRIG_GRALL:  return "GRALL";
    case TRIG_GIVE:   return "GIVE";
    case TRIG_BRIBE:  return "BRIBE";
    case TRIG_KILL:   return "KILL";
    case TRIG_DELAY:  return "DELAY";
    case TRIG_SURR:   return "SURRENDER";
    case TRIG_EXIT:   return "EXIT";
    case TRIG_EXALL:  return "EXALL";
    default:          return "ERROR";
  }
}

void mpedit( CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char command[MAX_INPUT_LENGTH];
  int cmd;

  strcpy(arg, argument);
  argument=one_argument(argument, command);

  if (ch->pcdata->security < 2)
  {
    stc("MPEdit: Insufficient security to modify code\n\r",ch); edit_done(ch);
  }

  if (!*command)
  {
    mpedit_show(ch, argument);
    return;
  }
  if (!str_cmp(command, "done") )
  {
    edit_done(ch);
    return;
  }
  for (cmd = 0; mpedit_table[cmd].name; cmd++)
  {
    if (!str_prefix(command, mpedit_table[cmd].name) )
    {
      (*mpedit_table[cmd].olc_fun) (ch, argument);
      return;
    }
  }
  interpret(ch, arg);
}

void do_mpedit(CHAR_DATA *ch, const char *argument)
{
  MPROG_CODE *pMcode;
  char command[MAX_INPUT_LENGTH];
  int code;

  argument = one_argument( argument, command);

  if (EMPTY(command))
  {
    stc(" Mpedit <number> \n\r Mpedit create <number>\n\r Mpedit delete <number>\n\r",ch);
    return;
  }
  if(is_number(command) )
  {
    code=atoi(command);
    if (!(pMcode = get_mprog_index(code)))
    {
      ptc(ch,"MPEdit: программа %d не найдена.\n\r",code);
      return;
    }
    ch->desc->pEdit=(void *)pMcode;
    ch->desc->editor= ED_MPCODE;
    ptc(ch,"Найдена программа %d\n\r",pMcode->vnum);
    return;
  }
  if (!str_cmp(command, "create" ) )
  {
    if (EMPTY(argument))
    {
      stc("Syntax: mpedit create [vnum]\n\r",ch);
      return;
    }
    if (mpedit_create(ch, argument)) ch->desc->editor = ED_MPCODE;
    else stc("Can't create new MobProg !!!\n\r",ch);
    return;
  }
  if (!str_cmp(command, "delete"))
  {
    if (!is_number(argument))
    {
      stc("Укажите правильеый номер программы.\n\r",ch);
      return;
    }
    pMcode=get_mprog_index(atoi(argument));
    if (pMcode)
    {
      free_mpcode(pMcode);
      stc("Программа удалена.\n\r",ch);
    }
    else stc("Программа не найдена.\n\r",ch);
    return;
  }
  do_mpedit(ch,"");
}

MPEDIT (mpedit_create)
{
  MPROG_CODE *pMcode;
  int value;

  value = atoi(argument);

  if (!*argument || value == 0)
  {
    stc("Syntax: mpedit create [vnum]\n\r",ch);
    return FALSE;
  }
  if (get_mprog_index(value))
  {
    stc("MPEdit: Code vnum already exists.\n\r",ch);
    return FALSE;
  }

  pMcode         = new_mpcode();
  pMcode->vnum   = value;
  ch->desc->pEdit= (void *)pMcode;

  ptc(ch,"Program Code %d Created.\n\r",value);
  return TRUE;
}

MPEDIT( mpedit_show)
{
  MPROG_CODE *pMcode;

  EDIT_MPCODE(ch, pMcode);
  ptc(ch,"Vnum: {Y%u{x\n\rCode:\n\r{G%s{x\n\r",
    pMcode->vnum, pMcode->code?pMcode->code:"{RЏгбв®© Є®¤{x");
  return FALSE;
}

MPEDIT(mpedit_code)
{
  MPROG_CODE *pMcode;
  EDIT_MPCODE(ch, pMcode);
  if (!*argument)
  {
    string_append(ch, &pMcode->code);
    return TRUE;
  }
  stc(" Syntax: desc\n\r",ch);
  return FALSE;
}

MPEDIT(mpedit_clear)
{
  MPROG_CODE *code;
  EDIT_MPCODE(ch,code);

  if (code)
  {
    edit_done(ch);
    free_mpcode(code);
    stc("Программа очищена.\n\r",ch);
    return TRUE;
  }
  else stc("Программа не найдена.\n\r",ch);
  return FALSE;
}

MPEDIT(mpedit_help)
{
  if (!*argument)
  {
    stc(" show         - show code of current program\n\r",ch);
    stc(" clear        - clear current program\n\r",ch);
    stc(" code         - edit mobprog code\n\r",ch);
    stc(" ?            - show help\n\r",ch);
    stc(" ? <commands> - show avaible commands\n\r",ch);
    stc(" ? <compares> - show avaible compare aliases\n\r",ch);
    stc(" ? <equates>  - show equates sign\n\r",ch);
    stc(" ? <macros>   - show avaible mobprog macros\n\r",ch);
    stc(" ? <triggers> - show avaible mobprog triggers\n\r",ch);
    return FALSE;
  }
  if (!str_prefix(argument,"macros"))
  {
    stc("$i - {Gmob{x name\n\r",ch);
    stc("$I - {Gmob{x short_descr or name, if not NPC\n\r",ch);
    stc("$n - {Ych{x name (can_see check)\n\r",ch);
    stc("$N - {Ych{x short_descr or name, if not NPC, can_see chech\n\r",ch);
    stc("$t - {yvch{x name, can_see check\n\r",ch);
    stc("$T - {yvch{x short_descr or name, can_see check\n\r",ch);
    stc("$r - {Crch{x name, can_see check\n\r",ch);
    stc("$R - {Crch{x short_descr or name, can_see check\n\r",ch);
    stc("$q - {Rmprog_target{x name, can_see check\n\r",ch);
    stc("$Q - {Rmprog_target{x short_desc or name, can_see check\n\r\n\r",ch);
    stc("$j - he_she {Gmob{x               $k - him_her {Gmob{x             \n\r",ch);
    stc("$e - he_she {Ych{x           vis  $m - him_her {Ych{x           vis\n\r",ch);
    stc("$E - he_she {yvch{x          vis  $M - him_her {yvch{x          vis\n\r",ch);
    stc("$J - he_she {Crch{x          vis  $K - hum_her {Crch{x          vis\n\r",ch);
    stc("$X - he_she {Rmprog_target{x vis  $Y - him_her {Rmprog_target{x vis\n\r",ch);
    stc("$l - his_her {Gmob{x\n\r",ch);
    stc("$s - his_her {Ych{x           vis\n\r",ch);
    stc("$S - his_her {yvch{x          vis\n\r",ch);
    stc("$L - his_her {Crch{x          vis\n\r",ch);
    stc("$Z - his_her {Rmprog_target{x vis\n\r\n\r",ch);
    stc("$o - {Mobj1{x name        vis  $p - {Mobj2{x name        vis\n\r",ch);
    stc("$O - {mobj1{x short_descr vis  $P - {mobj2{x short_descr vis\n\r",ch);
  }
  if (!str_prefix(argument,"triggers"))
  {
    int i;
    for (i=0;mprog_flags[i].name;i++)
      ptc(ch,"%2d [{G%s{x]\n\r",i,mprog_flags[i].name);
    return FALSE;
  }
  if (!str_prefix(argument,"compares"))
  {
    int i;
    for (i=0;fn_keyword[i].name[0]!='\n';i++)
      ptc(ch,"[{G%10s{x] %s\n\r",fn_keyword[i].name,fn_keyword[i].help);
    return FALSE;
  }
  if (!str_prefix(argument,"equates"))
  {
    int i;
    for (i=0;fn_evals[i].name[0]!='\n';i++)
      ptc(ch,"[{G%s{x] %s\n\r",fn_evals[i].name,fn_evals[i].help);
    return FALSE;
  }
  if (!str_prefix(argument,"commands"))
  {
    int i;
    for (i=0;mob_cmd_table[i].name[0]!='\0';i++)
      ptc(ch,"[{G%10s{x] %s\n\r",mob_cmd_table[i].name,mob_cmd_table[i].help);
    return FALSE;
  }

  return FALSE;
}

