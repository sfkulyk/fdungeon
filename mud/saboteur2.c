// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#include <sys/types.h> 
#include <sys/stat.h>
#include <ctype.h> 
#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <stdlib.h> 
#include "merc.h" 
#include "recycle.h" 
#include "tables.h" 
#include "interp.h" 

void load_races();
void write_skill_table( CHAR_DATA *ch, const char *argument );
void pban( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument );
char *flag_string( const struct flag_type *flag_table, int64 bits );
int64 flag_value( const struct flag_type *flag_table, const char *argument);

void do_rlist( CHAR_DATA *ch, const char *argument )
{
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 ROOM_INDEX_DATA *room;
 int64 r_from,r_to,i;

 argument = one_argument( argument, arg1 );
 argument = one_argument( argument, arg2 );

 if (arg1[0]=='\0' || !is_number(arg1) || arg2[0] == '\0' || !is_number(arg2))
 {
   stc( " {RСинтаксис: rlist нач_Vnum кол-во.\n\r                {CПример:{x rlist 1 100\n\r", ch );
   return;
 }

 r_from=atoi64(arg1);
 r_to=atoi64(arg2);

 if (r_from<1 || r_from>1000000)
 {
   stc("{RRooms ranges: 1 - 1000000.{x\n\r",ch);
   return;
 }

 if (r_to-r_from>100)  r_to=r_from+100;

 stc("{C|{GVnum {C|{x Название\n\r=-----------------------------------------------------------------------------={x\n\r",ch);

 for (i=r_from;i<=r_to;i++)
 {
   if ( (room= get_room_index(i)) == NULL )continue;

   ptc(ch, "{C|{G%5u{C|{x%s\n\r",room->vnum,room->name );
 }
 stc("{C=-----------------------------------------------------------------------------={x\n\r",ch);
}

void do_vlist( CHAR_DATA *ch, const char *argument )
{
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 OBJ_DATA *obj;
 OBJ_INDEX_DATA *obji;
 int64 i,i_from,i_to;

 argument = one_argument( argument, arg1 );
 argument = one_argument( argument, arg2 );

 if (arg1[0]=='\0' || !is_number(arg1) || arg2[0] == '\0' || !is_number(arg2))
  {
   stc( " {RСинтаксис: vlist нач_Vnum кол-во.\n\r                {CПример:{x vlist 1 100\n\r", ch );
   return;
  }

 i_from=atoi64(arg1);
 i_to=atoi64(arg2);

 if (i_from<1 || i_from>100000)
 {
   stc("{RVnum ranges: 1 - 100000.{x\n\r",ch);
   return;
 }

 if (i_to-i_from>200)  i_to=i_from+200;

 stc("{C|{GVnum {C| {GТип      {C|{GИмя{C\n\r=-----------------------------------------------------------------------------={x\n\r",ch);

 for (i=i_from;i<=i_to;i++)
  {
   if ( (obji= get_obj_index(i)) == NULL )continue;
   obj = create_object( obji, 110 );
   ptc(ch, "{C|{G%5u{C|{G%10s{C|{x%s\n\r",obji->vnum, item_name(obj->item_type),get_obj_desc(obj,'1'));
  }
 stc("{C=-----------------------------------------------------------------------------={x\n\r",ch);
}

void do_srcwrite( CHAR_DATA *ch, const char *argument )
{
  if (EMPTY(argument))
  {
    ptc(ch,"Use srcwrite with next parameters:\n\r");
    ptc(ch,"  skilltab  - write skill_table to const.str\n\r");
    return;
  }
  
  if (!str_prefix(argument,"skilltable")) write_skill_table(ch,"test");
}

void write_skill_table( CHAR_DATA *ch, const char *argument )
{
  int i;
  char buf[MAX_STRING_LENGTH];

  ID_FILE="const.str";

  for (i=0;;i++)
  {
    do_printf(buf," { \n");
    stf(buf,ch);
    do_printf(buf,"     \"%s\",\n",skill_table[i].name);
    stf(buf,ch);
    do_printf(buf,"     { %3d,%3d,%3d,%3d },\n",skill_table[i].skill_level[0],skill_table[i].skill_level[1],skill_table[i].skill_level[2],skill_table[i].skill_level[3]);
    stf(buf,ch);
    do_printf(buf,"     { %3d,%3d,%3d,%3d },\n",skill_table[i].rating[0],skill_table[i].rating[1],skill_table[i].rating[2],skill_table[i].rating[3]);
    stf(buf,ch);
    do_printf(buf,"     %s,\n",skill_table[i].spell_fun?"some spell":"spell_null");
    stf(buf,ch);
    do_printf(buf,"     %d,\n",skill_table[i].target);
    stf(buf,ch);
    do_printf(buf,"     %d,\n",skill_table[i].minimum_position);
    stf(buf,ch);
    do_printf(buf,"     &gsn_%s,\n",skill_table[i].name);
    stf(buf,ch);
    do_printf(buf,"     %d,\n",skill_table[i].group);
    stf(buf,ch);
    do_printf(buf,"     %d,\n",skill_table[i].min_mana);
    stf(buf,ch);
    do_printf(buf,"     %d,\n",skill_table[i].beats);
    stf(buf,ch);
    do_printf(buf,"     \"%s\",\n",skill_table[i].noun_damage?skill_table[i].noun_damage:"");
    stf(buf,ch);
    do_printf(buf,"     \"%s\",\n",skill_table[i].msg_off?skill_table[i].msg_off:"");
    stf(buf,ch);
    do_printf(buf,"     \"%s\",\n",skill_table[i].msg_obj?skill_table[i].msg_obj:"");
    stf(buf,ch);
    do_printf(buf,"     %s,\n",skill_flag_name(skill_table[i].flag));
    stf(buf,ch);
    do_printf(buf,"     \"%s\"\n",skill_table[i].help);
    stf(buf,ch);
    do_printf(buf," },\n");
    stf(buf,ch);
    if (str_cmp(skill_table[i].name,"end")) break;
  }
  return;
}

void do_family(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *tch;

  argument=one_argument(argument,arg1);
  argument=one_argument(argument,arg2);

  if (EMPTY(arg1))
  {
    stc("Syntax:\n\r",ch);
    stc(" Look Info  : family {Y<name>{x\n\r",ch);
    stc(" Set room   : family {Y<name>{x room    {G<vnum>{x|{Dnone{x\n\r",ch);
    stc(" set mother : family {Y<name>{x mother  {Y<name>{x|{Dnone{x\n\r",ch);
    stc(" set father : family {Y<name>{x father  {Y<name>{x|{Dnone{x\n\r",ch);
    stc(" set kins   : family {Y<name>{x kins    {Ystring (74 symbols max){x\n\r",ch);
    stc(" set marry  : family {Y<name>{x marry   {Y<name>{x|{Dnone{x\n\r",ch);
    stc(" set lover  : family {Y<name>{x lovers  {Y<name>{x|{Dnone{x\n\r",ch);
    stc(" set propose: family {Y<name>{x propose {Y<name>{x|{Dnone{x\n\r",ch);
    return;
  }

  tch=get_char_world(ch,arg1);
  if (!tch || IS_NPC(tch))
  {
    ptc(ch,"Персонаж %s не найден.\n\r",arg1);
    return;
  }

  if (EMPTY(arg2))
  {
    ROOM_INDEX_DATA *proom;

    // show information
    ptc(ch,"Информация о персонаже: {Y%s{x\n\r",tch->name);
  
    proom=get_room_index(tch->pcdata->proom);
    if (proom)
         ptc(ch," Место проживания: %s {G[%u]{x\n\r",proom->name,tch->pcdata->proom);
    else ptc(ch," Место проживания: {Dнигде не прописан{x\n\r");
  
    if (EMPTY(tch->pcdata->marry)) 
         ptc(ch," Семейное положение:{Dне %s{x\n\r",tch->sex==2?"замужем":"женат");
    else ptc(ch," Семейное положение:%sа {Y%s{x\n\r",tch->sex==2?"замужем з":"женат н",tch->pcdata->marry);
  
    if (EMPTY(tch->pcdata->lovers)) 
         ptc(ch," Любовники: {Dотсутствуют (ни с кем не обвенчан%s){x\n\r",tch->sex==2?"a":"");
    else ptc(ch," Любовники(обвенчание): {Y%s{x\n\r",tch->pcdata->lovers);
  
    if (EMPTY(tch->pcdata->propose_marry)) 
         ptc(ch," Предложение брака: {Dнет заявок{x\n\r");
    else ptc(ch," Предложена рука и сердце для {Y%s{x\n\r",tch->pcdata->propose_marry);

    if (EMPTY(tch->pcdata->kins)) 
         ptc(ch," Родственники: {Dнет{x\n\r");
    else ptc(ch," {Y%s{x\n\r",tch->pcdata->kins);
  
    if (EMPTY(tch->pcdata->mother) && EMPTY(tch->pcdata->father))
         ptc(ch," Родители: {Dнеизвестны{x\n\r");
    else ptc(ch," Родители: папа:{Y%s{x мама:{Y%s{x\n\r",
       EMPTY(tch->pcdata->father)?"{Dнеизвестен":tch->pcdata->father,
       EMPTY(tch->pcdata->mother)?"{Dнеизвестна":tch->pcdata->mother);
    return;
  }

  if (!str_prefix(arg2,"room"))
  {
    ROOM_INDEX_DATA *proom;

    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      ptc(ch,"{Y%s{x выселен.\n\r",tch->name);
      tch->pcdata->proom=0;
      return;
    }
    if (!is_number(argument) || (proom=get_room_index(atoi64(argument)))==NULL
     || ( !IS_SET( global_cfg,CFG_GTFIX) && !IS_ELDER(ch) && IS_SET(proom->room_flags,ROOM_ELDER) ))
    {
      stc("Такой комнаты не существует",ch);
      return;
    }
    tch->pcdata->proom=atoi64(argument);
    ptc(ch,"Ты поселяешь {Y%s{x в комнате %s [%u].\n\r",tch->name,proom->name,proom->vnum);
    return;
  }

  if (!str_prefix(arg2,"mother"))
  {
    free_string(tch->pcdata->mother);
    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      tch->pcdata->mother=&str_empty[0];
      ptc(ch,"Поле {Yмама {xочищено у  {Y%s{x{x.\n\r",tch->name);
      return;
    }
    ((char*)argument)[0] = UPPER(argument[0]);
    tch->pcdata->mother=str_dup(argument);
    ptc(ch,"Теперь {Y%s{x является мамой {Y%s{x.\n\r",argument,tch->name);
    return;
  }

  if (!str_prefix(arg2,"kins"))
  {
    free_string(tch->pcdata->kins);
    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      tch->pcdata->kins=&str_empty[0];
      ptc(ch,"Поле {YРодственники {xочищено у  {Y%s{x{x.\n\r",tch->name);
      return;
    }
    ((char*)argument)[0] = UPPER(argument[0]);
    tch->pcdata->kins=str_dup(argument);
    ptc(ch,"Теперь у {Y%s{x есть:\n\r{Y%s{x.\n\r",tch->name,argument);
    return;
  }

  if (!str_prefix(arg2,"father"))
  {
    free_string(tch->pcdata->father);
    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      tch->pcdata->father=&str_empty[0];
      ptc(ch,"Поле {Yпапа {xочищено у  {Y%s{x{x.\n\r",tch->name);
      return;
    }
    ((char*)argument)[0] = UPPER(argument[0]);
    tch->pcdata->father=str_dup(argument);
    ptc(ch,"Теперь {Y%s{x является папой {Y%s{x.\n\r",argument,tch->name);
    return;
  }

  if (!str_prefix(arg2,"marry"))
  {
    free_string(tch->pcdata->marry);
    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      tch->pcdata->marry=&str_empty[0];
      ptc(ch,"Поле {Yсупруг {xочищено у  {Y%s{x{x.\n\r",tch->name);
      return;
    }
    ((char*)argument)[0] = UPPER(argument[0]);
    tch->pcdata->marry=str_dup(argument);
    ptc(ch,"Теперь {Y%s{x %s {Y%s{x.\n\r",tch->name,
     tch->sex==2?"замужем за":"женат на",argument);
    return;
  }

  if (!str_prefix(arg2,"lovers"))
  {
    free_string(tch->pcdata->lovers);
    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      tch->pcdata->lovers=&str_empty[0];
      ptc(ch,"Поле {Yлюбовники {xочищено у  {Y%s{x{x.\n\r",tch->name);
      return;
    }
    ((char*)argument)[0] = UPPER(argument[0]);
    tch->pcdata->lovers=str_dup(argument);
    ptc(ch,"Теперь {Y%s{x обвенчан на%s {Y%s{x.\n\r",tch->name,
     tch->sex==2?"а":"",argument);
    return;
  }

  if (!str_prefix(arg2,"propose"))
  {
    free_string(tch->pcdata->propose_marry);
    if (EMPTY(argument) || !str_cmp(argument,"none"))
    {
      tch->pcdata->propose_marry=&str_empty[0];
      ptc(ch,"Поле {Yпредложение брака{xочищено у  {Y%s{x{x.\n\r",tch->name);
      return;
    }
    ((char*)argument)[0] = UPPER(argument[0]);
    tch->pcdata->propose_marry=str_dup(argument);
    ptc(ch,"Теперь {Y%s{x предлагает руку и сердце {Y%s{x.\n\r",tch->name);
    return;
  }
}

void do_pban( CHAR_DATA *ch, const char *argument )
{
  pban(ch, ch, argument);
}

void pban( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument )
{
  char arg1[MAX_STRING_LENGTH];

  if (EMPTY(argument))
  { 
    const char *temp ;

    temp=victim->pcdata->deny_addr;
    ptc(ch,"Список запрещенных для {Y%s{x адресов:\n\r------------------------------------\n\r",victim->name);
    for (;;)
    {
      temp=one_argument(temp,arg1);
      if (!arg1 || EMPTY(arg1)) break;
      ptc(ch,"%s\n\r",arg1);
    }
    return;
  }

  argument=one_argument(argument,arg1);
  if (!str_cmp(arg1,"add"))
  {
    char temp[MAX_STRING_LENGTH];

    one_argument(argument,arg1);

    if (EMPTY(victim->pcdata->deny_addr))
      sprintf(temp,arg1);
    else
    {
      sprintf(temp,victim->pcdata->deny_addr);
      strcat(temp," ");
      strcat(temp,arg1);
    }
    ptc(ch,"адрес %s добавлен в список недозволеных.\n\r",arg1);
    free_string(victim->pcdata->deny_addr);
    victim->pcdata->deny_addr=str_dup(temp);
    return;
  }

  if (!str_cmp(arg1,"rem"))
  {
    bool first=FALSE;
    const char *temp ;
    char arg2[MAX_STRING_LENGTH];
    char temp2[MAX_STRING_LENGTH];

    temp=victim->pcdata->deny_addr;
    one_argument(argument,arg1);
    temp2[0]='\0';

    for (;;)
    {
      temp=one_argument(temp,arg2);
      if (!arg2 || EMPTY(arg2)) break;

      if (first) strcat(temp2," ");
      if (str_cmp(arg1,arg2)) {strcat(temp2,arg2);first=TRUE;}
    }
    free_string(victim->pcdata->deny_addr);
    victim->pcdata->deny_addr=str_dup(temp2);
    ptc(ch,"Адрес %s удален из списка.\n\r",arg1);
    return;
  }

  if (!str_cmp(arg1,"clean") || !str_cmp(arg1,"clear"))
  {
    free_string(victim->pcdata->deny_addr);
    victim->pcdata->deny_addr=str_dup("");
    stc("Список был очищен.\n\r",ch);
    return;
  }

  if (!str_cmp(arg1,"char") && IS_ELDER(ch))
  {
    CHAR_DATA *target;
    argument=one_argument(argument,arg1);
    target=get_pchar_world(ch,arg1);
    if (!target)
    {
      stc("Персонаж не найден.\n\r",ch);
      return;
    }
    pban(ch, target, argument);
    return;
  }

  stc("Personal Ban - Запрещенные для персонажа адреса:\n\r",ch);
  stc("  pban             - просмотреть текущий список\n\r",ch);
  stc("  pban add <addr>  - добавить адрес\n\r",ch);
  stc("  pban rem <addr>  - убрать адрес\n\r",ch);
  stc("  pban clear       - очистить весь список\n\r",ch);
  if (IS_ELDER(ch)) stc("  pban char <name> - выполнить pban для чара <name>\n\r",ch);
}

void do_check(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  bool found,all=FALSE;
  AFFECT_DATA *af;
  int mod, level;
  int hr, dr, hit, mana, ac, saves;

  if (IS_SET(global_cfg, CFG_LOCAL))
  {
    stc("Ась?\n\r",ch);
    return;
  }

  argument=one_argument(argument,arg);

  victim=get_char_world(ch,arg);

  if (!victim)
  {
    stc("Персонаж не найден.\n\r",ch);
    return;
  }
  if (!str_cmp(argument,"all")) all=TRUE;
  ptc(ch,"Checking player {Y%s{x.\n\r Vnum Name               lvl(rlv)  Hr   Dr   Hit  Man  Ac   Loaded string\n\r",victim->name);
  for (obj=victim->carrying;obj;obj=obj->next_content)
  {
    found=all;
    level=obj->level;
    hr=dr=hit=mana=ac=saves=0;
//    if (level>101) continue;
    if (obj->loaded_by) found=TRUE;
    if (obj->item_type==ITEM_ARMOR)
    {
      if (obj->value[AC_PIERCE]!=obj->pIndexData->value[AC_PIERCE]) found=TRUE;
      if (obj->value[AC_BASH]  !=obj->pIndexData->value[AC_BASH])   found=TRUE;
      if (obj->value[AC_SLASH] !=obj->pIndexData->value[AC_SLASH])  found=TRUE;
      if (obj->value[AC_EXOTIC]!=obj->pIndexData->value[AC_EXOTIC]) found=TRUE;
    }
    if (level!=obj->pIndexData->level) found=TRUE;
    if (obj->enchanted) found=TRUE;
    if (!obj->enchanted)
    for ( af = obj->pIndexData->affected; af ; af = af->next )
    {
      mod = af->modifier;
      switch(af->location)
      {
        case APPLY_STR:
        case APPLY_DEX:
        case APPLY_INT:
        case APPLY_WIS:
        case APPLY_CON:
         if (level <20 && mod > 2) found=TRUE;
         if (level <70 && mod > 3) found=TRUE;
         if (mod > 4) found=TRUE;
         break;

        case APPLY_MANA:
         mana+=mod;
         if (level <20 && mod>10) found=TRUE;
         if (level <50 && mod>20) found=TRUE;
         if (mod>100) found=TRUE;
           break;
        case APPLY_HIT:
         hit+=mod;
         if (level <20 && mod>10) found=TRUE;
         if (level <50 && mod>20) found=TRUE;
         if (mod>100) found=TRUE;
           break;

        case APPLY_AC:
         ac+=mod;
         if (level<10 && mod<-10) found=TRUE;
         if (level<50 && mod<-50) found=TRUE;
         if (mod<-100) found=TRUE;
            break;

        case APPLY_HITROLL:
         hr+=mod;
         if (level<10 && mod>3) found=TRUE;
         if (level<30 && mod>8) found=TRUE;
         if (level<50 && mod>15) found=TRUE;
         if (level<100 && mod>20) found=TRUE;
         if (mod>29) found=TRUE;
           break;
        case APPLY_DAMROLL:
         dr+=mod;
         if (level<10 && mod>3) found=TRUE;
         if (level<30 && mod>8) found=TRUE;
         if (level<50 && mod>15) found=TRUE;
         if (level<100 && mod>20) found=TRUE;
         if (mod>29) found=TRUE;
           break;

        case APPLY_SAVES:
        case APPLY_SAVING_ROD:
        case APPLY_SAVING_PETRI:
        case APPLY_SAVING_BREATH:
        case APPLY_SAVING_SPELL:
         saves+=mod;
         if (level<10 && mod<-3) found=TRUE;
         if (level<30 && mod<-10) found=TRUE;
         if (mod < -20) found=TRUE;
         break;
      }
    }
    for ( af = obj->affected; af ; af = af->next )
    {
      mod = af->modifier;
      switch(af->location)
      {
        case APPLY_STR:
        case APPLY_DEX:
        case APPLY_INT:
        case APPLY_WIS:
        case APPLY_CON:
         if (level <20 && mod > 2) found=TRUE;
         if (level <70 && mod > 3) found=TRUE;
         if (mod > 4) found=TRUE;
         break;

        case APPLY_MANA:
         mana+=mod;
         if (level <20 && mod>10) found=TRUE;
         if (level <50 && mod>20) found=TRUE;
         if (mod>100) found=TRUE;
           break;
        case APPLY_HIT:
         hit+=mod;
         if (level <20 && mod>10) found=TRUE;
         if (level <50 && mod>20) found=TRUE;
         if (mod>100) found=TRUE;
           break;

        case APPLY_AC:
         ac+=mod;
         if (level<10 && mod<-10) found=TRUE;
         if (level<50 && mod<-50) found=TRUE;
         if (mod<-100) found=TRUE;
            break;

        case APPLY_HITROLL:
         hr+=mod;
         if (level<10 && mod>3) found=TRUE;
         if (level<30 && mod>8) found=TRUE;
         if (level<50 && mod>15) found=TRUE;
         if (level<100 && mod>20) found=TRUE;
         if (mod>29) found=TRUE;
           break;
        case APPLY_DAMROLL:
         dr+=mod;
         if (level<10 && mod>3) found=TRUE;
         if (level<30 && mod>8) found=TRUE;
         if (level<50 && mod>15) found=TRUE;
         if (level<100 && mod>20) found=TRUE;
         if (mod>29) found=TRUE;
           break;

        case APPLY_SAVES:
        case APPLY_SAVING_ROD:
        case APPLY_SAVING_PETRI:
        case APPLY_SAVING_BREATH:
        case APPLY_SAVING_SPELL:
         saves+=mod;
         if (level<10 && mod<-3) found=TRUE;
         if (level<30 && mod<-10) found=TRUE;
         if (mod < -20) found=TRUE;
         break;
      }
    }
    if (!found) continue;
    ptc(ch,"{G%5u{x %18s {G%3d{x({G%3d{x) {R%4d %4d {G%4d {C%4d {W%4d {Y%s{x\n\r",
      obj->pIndexData->vnum,obj->name,
      obj->level,obj->pIndexData->level,
      hr, dr, hit, mana, ac, obj->loaded_by?obj->loaded_by:"нет");
  }
}

#define rtab race_table[race]
void do_race(CHAR_DATA *ch, const char *argument)
{
  char buf[128];
  char arg[MAX_INPUT_LENGTH];
  int cnt=0,race;

  argument=one_argument(argument,arg);
  if (EMPTY(arg) || !str_prefix(arg,"help"))
  {
    stc("Syntax: race               - show current help\n\r",ch);
    stc("        race list          - list of current races\n\r",ch);
    stc("        race save          - save "RACE_FILE" file\n\r",ch);
    stc("        race load          - load "RACE_FILE" file\n\r",ch);
    stc("        race <name>        - show race info\n\r",ch);
    stc("        race edit          - show race edit help\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"edit"))
  {
    stc("Редактирование рас. Синтакс:\n\r",ch);
    stc("  race <{Gname{x> <{Gcommand{x> [<{Cparameter{x>]\n\r\n\r",ch);
    stc(" в качестве имени расы может быть ее name или номер.\n\r",ch);
    stc(" Текущие {Gкоманды{x:\n\r\n\r",ch);

    stc(" {Gwealth                       {x- toggle wealth\n\r",ch);
    stc(" {Gpcrace                       {x- toggle PC allowing to use\n\r\n\r",ch);

    stc(" {Gname {x<{Cnew name{x>              - new name\n\r",ch);
    stc(" {Gwhoname {x<{Cnew who_name{x>       - new who_name - 8 symbols max\n\r\n\r",ch);

    stc(" {Ghanddam                      {x- list of attack types\n\r",ch);
    stc(" {Ghanddam {x<{Cnew attack type{x>    - new who_name - 8 symbols max\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"list"))
  {
    stc("{W#No {CИмя расы     {GДоступна {W#No {CИмя расы     {GДоступна\n\r",ch);
    for ( race=0; rtab.name; race+=2 )
    {
     ptc(ch,"{W%2d {C%15s [{Y%s{C] %s",race,rtab.name,rtab.who_name,
       rtab.pc_race?"{GДа{x ":"{RНет{x");
     if (race_table[race+1].name) ptc(ch,"     {W%2d {C%15s [{Y%s{x] %s\n\r",race+1,race_table[race+1].name,race_table[race+1].who_name,
       (race_table[race+1].pc_race==TRUE) ? "{GДа{x " : "{RНет{x");
     else stc("\n\r",ch);
    }
    return;
  }

  if (!str_prefix(arg,"save"))
  {
    save_races();
    stc("{RRace file saved.{x\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"load"))
  {
    stc("{RLoading new race list...\n\r",ch);
    load_races();
    stc("{RDone.{x\n\r",ch);
    return;
  }

  if (is_number(arg)) race=atoi(arg);
  else race=race_lookup(arg);
  if (race==0 || !rtab.name)
  {
    stc("Такой расы не обнаружено.\n\r",ch);
    return;
  }

  argument=one_argument(argument,arg);

  if (EMPTY(arg) || !str_prefix(arg,"info"))
  {
    stc("{g=-=-=-=-=-=-=-= Race Information =-=-=-=-=-=-=-=-=-\n\r",ch);
    ptc(ch,"{GName [{Y%s{G]  Who_Name [{x%s{G]  PC_race [%s{x] Size:%d\n\r",
    rtab.name,rtab.who_name,rtab.pc_race?"{GYes":"{RNo",rtab.size);
    ptc(ch,"Points: %d  Class {{ %3d %3d %3d %3d }\n\r",
     rtab.points,rtab.class_mult[0],rtab.class_mult[1],
     rtab.class_mult[2],rtab.class_mult[3]);
    ptc(ch,"Act: [%s]\n\r",pflag64(rtab.act));
    ptc(ch,"Aff: [%s]\n\r",pflag64(rtab.aff));
    ptc(ch,"Off: [%s]\n\r",pflag64(rtab.off));
    ptc(ch,"Imm: [%s]\n\r",pflag64(rtab.imm));
    ptc(ch,"Res: [%s]\n\r",pflag64(rtab.res));
    ptc(ch,"Vuln: [%s]\n\r",pflag64(rtab.vuln));
    ptc(ch,"Form: [%s]\n\r",pflag64(rtab.form));
    ptc(ch,"Skills: [%s] [%s] [%s] [%s] [%s]\n\r",
     rtab.skills[0],rtab.skills[1],rtab.skills[2],rtab.skills[3],rtab.skills[4]);
    ptc(ch,"Stats    : %3d %3d %3d %3d %3d\n\r",
     rtab.stats[0],rtab.stats[1],rtab.stats[2],rtab.stats[3],rtab.stats[4]);
    ptc(ch,"MaxStats : %3d %3d %3d %3d %3d\n\r",
     rtab.max_stats[0],rtab.max_stats[1],rtab.max_stats[2],rtab.max_stats[3],rtab.max_stats[4]);
    ptc(ch,"HighStats: %3d %3d %3d %3d %3d\n\r",
     rtab.high_stats[0],rtab.high_stats[1],rtab.high_stats[2],rtab.high_stats[3],rtab.high_stats[4]);
    ptc(ch,"Spec: %s\n\r",pflag64(rtab.spec));
    ptc(ch,"Cbon: %s\n\r",pflag64(rtab.c_bon));
    ptc(ch,"Cpen: %s\n\r",pflag64(rtab.c_pen));
    ptc(ch,"DamBonus: ");
    for (cnt=0;cnt<DAM_MAX;cnt++) ptc(ch," %d",rtab.dambonus[cnt]);
    ptc(ch,"\n\rWBonus  : ");
    for (cnt=0;cnt<WEAPON_MAX;cnt++) ptc(ch," %d",rtab.weapon_bonus[cnt]);
    ptc(ch,"\n\rDamage type: %s",rtab.hand_dam?rtab.hand_dam:"{R!NULL!{x");
    ptc(ch,"\n\rWealth  : %d",rtab.wealth);
    stc("\n\r{g=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-{x\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"wealth"))
  {
    if (rtab.wealth) rtab.wealth=FALSE;
    else rtab.wealth=TRUE;
    ptc(ch,"Состояние Wealth в%sключено для расы %s\n\r",rtab.wealth?"":"ы",rtab.name);
    return;
  }

  if (!str_prefix(arg,"wealth"))
  {
    if (rtab.pc_race) rtab.pc_race=FALSE;
    else rtab.pc_race=TRUE;
    ptc(ch,"Состояние PC_RACE в%sключено для расы %s\n\r",rtab.pc_race?"":"ы",rtab.name);
    return;
  }

  if (!str_prefix(arg,"size"))
  {
    int size;
    if (EMPTY(argument))
    {
      stc("Укажите новые габариты для расы\n\r",ch);
      return;
    }
    size=size_lookup(argument);
    if (size==-1)
    {
      stc("Какой-какой?\n\r",ch);
      return;
    }
    rtab.size=size;
    ptc(ch,"Габариты для расы %s установлены в %s.\n\r",
      rtab.name,size_table[rtab.size].name);
    return;
  }

  if (!str_prefix(arg,"name"))
  {
    if (EMPTY(argument))
    {
      stc("Укажите новый name для расы\n\r",ch);
      return;
    }
    ptc(ch,"Вы переименовали расу [%s] в [%s].\n\r",rtab.name,argument);
    stc("{RМобы и игроки этой расы должны быть обязательно сохранены\n\r",ch);
    stc("до ребута, иначе они потеряют свою расу.{x{*\n\r",ch);

    do_printf(buf,argument);
    free_string(rtab.name);
    rtab.name=str_dup(buf);
    return;
  }

  if (!str_prefix(arg,"whoname"))
  {
    if (EMPTY(argument))
    {
      stc("Укажите новый who_name для расы\n\r",ch);
      return;
    }
    ptc(ch,"Вы изменили [%s] на [%s] для расы %s.\n\r",rtab.who_name,argument,rtab.name);
    do_printf(buf,"%8s",argument);
    free_string(rtab.who_name);
    rtab.who_name=str_dup(buf);
    return;
  }

#define atab attack_table[cnt]
  if (!str_prefix(arg,"handdam"))
  {
    if (EMPTY(argument))
    {
      for (cnt=0;atab.name;cnt++)
      {
        ptc(ch,"%3d. %10s %s\n\r",cnt,atab.name,atab.noun);
      }
      return;
    }
    cnt=attack_lookup(argument);
    do_printf(buf,atab.name);
    ptc(ch,"Вы изменили удар для расы %s на %s. (Было %s)\n\r",
      rtab.name,buf,rtab.hand_dam);
    free_string(rtab.hand_dam);
    rtab.hand_dam=str_dup(buf);
    return;
  }
#undef atab

  stc("{RПроверьте синтаксис.{x\n\r",ch);
}
#undef rtab

void do_index(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int64 flag;
  int count=0, tmp;
  CHAR_DATA *vch;
  MOB_INDEX_DATA *mob;

  argument=one_argument(argument,arg);

  if (!str_cmp(arg,"help"))
  {
    ptc(ch,"[%3d] %s\n\r",0,flag_string(act_flags,0));
    for (tmp=1;tmp < MAX_FLAG ;tmp*=2) ptc(ch,"[%3d] %s\n\r",tmp,flag_string(act_flags,tmp));
    return;
  }

  flag=flag_lookup(arg,act_flags);
  if (flag==NO_FLAG || flag==0)
  {
    stc("Флаг не указан.\n\r",ch);
    return;
  }

  for (tmp=0;tmp<top_mob_index;tmp++)
  {
    mob=get_mob_index(tmp);
    if (!mob) continue;
    if (IS_SET(mob->act,flag))
    {
      count++;
      ptc(ch,"Found: [%5u] %s\n\r",mob->vnum, mob->short_descr);
    }
  }
  ptc(ch,"Summary: %d mobindexes.\n\r",count);
  count=0;
  for (vch=char_list;vch;vch=vch->next)
  {
    if (IS_SET(vch->act,flag) && IS_NPC(vch))
    {
      count++;
      ptc(ch,"Found: %s\n\r", vch->short_descr);
    }
  }
  ptc(ch,"In use: %d mobiles with flag %s.\n\r",count,flag_string(act_flags,flag));
}

void do_backup2( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA d;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (!str_cmp(arg1, "show") && IS_IMMORTAL(ch))
  {
    do_printf( buf, "%s%s", PLAYER_DIR3, capitalize( arg2 ) );

    if (load_char_obj(&d, arg2, SAVE_BACKUP2))
    {
      ptc(ch, "В архиве 2 найден следующий персонаж: %s\n\r", d.character->name);
      ptc(ch, "{WВ последний раз этот персонаж заходил {Y%s{W{x\n\r",ctime(&d.character->lastlogin));
      ptc(ch, "Уровень %d,  QuestPoints: %d\n\r",d.character->level, d.character->questpoints);
      extract_char(d.character, TRUE);
      return;
    }
    stc("Этого персонажа нет в архиве 2.\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "restore") && IS_IMMORTAL(ch))
  {
    CHAR_DATA *wch;

    if ((wch=get_pchar_world(ch, arg2)))
    {
      ptc(ch, "Персонаж %s сейчас в мире.\n\r",wch->name);
      return;
    }

    do_printf( buf, "%s%s", PLAYER_DIR3, capitalize( arg2 ) );

    if (load_char_obj(&d, arg2, SAVE_BACKUP2))
    {
      if (get_trust(d.character)>109 && get_trust(ch)<110)
      {
        ptc(ch,"{RAccess Denied.{x\n\r");
        extract_char(d.character, TRUE);
        return;
      }
//      d.character->pcdata->dn = 49;
      save_one_char(d.character, SAVE_NORMAL);
      ptc(ch, "Персонаж восстановлен из архива 2.\n\r", d.character->name);
      extract_char(d.character, TRUE);
      return;
    }
    stc("Этого персонажа нет в архиве 2.\n\r", ch);
    return;
  }
  stc("Syntax: backup2 show    <name>\n\r", ch);
  stc("        backup2 restore <name>\n\r", ch);
}
