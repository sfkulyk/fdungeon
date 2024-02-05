// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"

extern  int     _filbuf         args((FILE *));
int             favour_string   ( CHAR_DATA *ch);
int max_on = 0;

// Local functions.
bool  check_social( CHAR_DATA *ch, char *command, const char *argument );
char *soc_group_name(int64 group);
int   min_level(CHAR_DATA *ch,int sn);
char *act_name(int64 act_flag);
void  act_toggle(CHAR_DATA *ch,int64 act_flag);
char *format_obj_to_char  args((OBJ_DATA *obj, CHAR_DATA *ch,bool fShort));
void  show_list_to_char   args((OBJ_DATA *list, CHAR_DATA *ch,bool fShort, bool fShowNothing));
void  show_char_to_char_0 args((CHAR_DATA *victim, CHAR_DATA *ch));
void  show_char_to_char_1 args((CHAR_DATA *victim, CHAR_DATA *ch));
void  show_char_to_char   args((CHAR_DATA *list, CHAR_DATA *ch));
bool  check_blind         args((CHAR_DATA *ch));
char *do_show_flag(CHAR_DATA *ch,CHAR_DATA *victim,bool fshort);
char *do_show_iflag(CHAR_DATA *ch,OBJ_DATA *obj, bool fshort);

#define MAX_CFG 19

struct cfg_type
{
  char *name;
  char *showname;
  int64 bit;
  bool change;
  bool on;
};

const struct cfg_type cfg_table[] =
{
  { "zritel    ","Статус Зрителя на Арене            ",CFG_ZRITEL   ,TRUE ,FALSE },
  { "showkills ","Выводить статистику убиств всем    ",CFG_SHOWKILLS,TRUE ,FALSE },
  { "autodamage","Выводить повреждения в цифрах      ",CFG_AUTODAM  ,FALSE ,FALSE },
  { "autoexit  ","Выводить выходы из комнаты         ",CFG_AUTOEXIT ,TRUE ,FALSE },
  { "autotitle ","Не менять титл                     ",CFG_AUTOTITLE,TRUE ,TRUE  },
  { "shortflag ","Выводить сокращенные флаги         ",CFG_SHORTFLAG,TRUE ,FALSE },
  { "getexp    ","Получать опыт                      ",CFG_GETEXP   ,TRUE ,TRUE  },
  { "getall    ","Подбирать предметы при смерти      ",CFG_GETALL   ,TRUE ,TRUE  },
  { "autorecal ","Авторекал в битве при лост линке   ",CFG_AUTORECAL,TRUE ,FALSE },
  { "autosplit ","Делить деньги в группе             ",CFG_AUTOSPLIT,TRUE ,FALSE },
  { "runshow   ","Выводить описание комнаты в RUN    ",CFG_RUNSHOW  ,TRUE ,TRUE  },
  { "nolisten  ","Не слышать вопли смертных(for imms)",CFG_NOIMMS   ,TRUE ,FALSE },
  { "affshort  ","Выводить аффекты кратко            ",CFG_AFFSHORT ,TRUE ,FALSE },
  { "autotick  ","Выводить строку autotick каждый тик",CFG_TICK     ,TRUE ,FALSE },
  { "autolook  ","Заглядывать в труп после убийства  ",CFG_AUTOLOOK ,TRUE ,FALSE },
  { "autovote  ","Сообщать о голосованиях при входе  ",CFG_AUTOVOTE ,TRUE ,FALSE },
  { "autonote  ","Сообщать о непрочитанных при входе ",CFG_AUTONOTE ,TRUE ,FALSE },
  { "autoplr   ","Сообщать об играющих при входе     ",CFG_AUTOPLR  ,TRUE ,FALSE },
  { "autopeek  ","Смотреть имущество жертвы при look ",CFG_AUTOPEEK ,TRUE ,FALSE }
};


char *format_obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch, bool fShort)
{
  static char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH], first_name[MAX_STRING_LENGTH];
  buf[0] = '\0';
   
  if (!obj) return buf;

  if (!can_see_obj(ch , obj))
  {
    strcat(buf, "нечто.");
    return buf;
  };

  one_argument(obj->name,first_name);
  do_printf(buf2,"{g({y%s{g){x",first_name);

  if ((fShort && (!obj->short_descr || EMPTY(obj->short_descr)))
    || (!obj->description || EMPTY(obj->description))) return buf;

  if (IS_CFG(ch,CFG_SHORTFLAG)) strcat(buf,do_show_iflag(ch,obj,TRUE));
  else                          strcat(buf,do_show_iflag(ch,obj,FALSE));

  if (fShort)
  {
    if (obj->short_descr) 
    {  
      strcat(buf, "{G");
      strcat(buf, get_obj_desc(obj,'1'));
      strcat(buf, "{x");
    }
    if (obj->pIndexData->area->security==8) strcat(buf, buf2);
  }
  else
  {
    if (obj->description)
    {
      strcat(buf, "{G");
      strcat(buf, obj->description);
      strcat(buf, "{x");
    }  
    if (obj->pIndexData->area->security==8) strcat(buf, buf2);
  }
  if (strlen(buf)<=0) strcat(buf,"Bug - NO DESCRIPTION! Report to IMM.");
  strcat(buf, "{w(");
  if (IS_CFG(ch,CFG_SHORTFLAG)) strcat(buf, get_obj_cond(obj,2));
  else                          strcat(buf, get_obj_cond(obj,0));
  strcat(buf, "{w){x");
  return buf;
}

// Show a list to a character. Can coalesce duplicated items.
void show_list_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing)
{
  char buf[MAX_STRING_LENGTH];
  BUFFER *output;
  const char **prgpstrShow, *pstrShow;
  int *prgnShow;
  OBJ_DATA *obj;
  int nShow, iShow, count;
  bool fCombine;

  if (!ch->desc) return;

  output=new_buf();

  count=0;
  for (obj = list; obj != NULL; obj = obj->next_content) count++;
  prgpstrShow = alloc_mem(count * sizeof(char *));
  prgnShow    = alloc_mem(count * sizeof(int)   );
  nShow       = 0;

  for (obj=list;obj;obj=obj->next_content)
  {
    if (obj->wear_loc==WEAR_NONE && can_see_obj(ch,obj))
    {
      pstrShow = format_obj_to_char(obj, ch, fShort);

      fCombine = FALSE;

      // Look for duplicates, case sensitive.
      // Matches tend to be near end so run loop backwords.
      for (iShow = nShow - 1; iShow >= 0; iShow--)
      {
        if (!strcmp(prgpstrShow[iShow], pstrShow))
        {
          prgnShow[iShow]++;
          fCombine = TRUE;
          break;
        }
      }

      // Couldn't combine, or didn't want to.
      if (!fCombine)
      {
        prgpstrShow[nShow]=str_dup(pstrShow);
        prgnShow   [nShow]=1;
        nShow++;
      }
    }
  }

  // Output the formatted list.
  for (iShow = 0; iShow < nShow; iShow++)
  {
    if (EMPTY(prgpstrShow[iShow]))
    {
      free_string(prgpstrShow[iShow]);
      continue;
    }

    if (prgnShow[iShow] != 1)
    {
      do_printf(buf, "(%4d) ", prgnShow[iShow]);
      add_buf(output,buf);
    }
    else add_buf(output,"       ");
    add_buf(output,prgpstrShow[iShow]);
    add_buf(output,"\n\r");
    free_string(prgpstrShow[iShow]);
  }

  if (fShowNothing && nShow == 0) stc("     Ничего.\n\r", ch);
  page_to_char(buf_string(output),ch);

  free_buf(output);
  free_mem((void*)prgpstrShow, count * sizeof(char *));
  free_mem(prgnShow, count * sizeof(int));
}

void show_char_to_char_0(CHAR_DATA *victim, CHAR_DATA *ch)
{
  char buf[MAX_STRING_LENGTH],buf2[MAX_STRING_LENGTH],
      message[MAX_STRING_LENGTH],first_name[MAX_STRING_LENGTH];

  buf[0] = '\0';

  if (IS_CFG(ch,CFG_SHORTFLAG)) strcat(buf,do_show_flag(ch,victim,TRUE));
  else                          strcat(buf,do_show_flag(ch,victim,FALSE));

  one_argument(victim->name,first_name);
  do_printf(buf2,"{g({w%s{g){x",first_name);

  if (victim->position == victim->start_pos && victim->long_descr[0] != '\0')
  {
    if (IS_NPC(victim) && victim->pIndexData->area->security==8) strcat(buf, buf2);
    strcat(buf, "{y");
    strcat(buf, victim->long_descr); 
    strcat(buf, "{x");
    stc(buf, ch);
    return;
  }

  strcat(buf, PERS(victim, ch));
  if (!IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF)
   && victim->position == POS_STANDING && !ch->on)
  {
    strcat(buf, " ");
    if( can_see(ch,victim,CHECK_LVL) ) strcat(buf, victim->pcdata->title);
  }

  switch (victim->position)
  {
    case POS_DEAD:    strcat(buf, " {RМЕРТВ!!{x");break;
    case POS_MORTAL:  strcat(buf, " {Rсмеpтельно pанен.{x");break;
    case POS_INCAP:   strcat(buf, " {Mлежит без сознания.{x");break;
    case POS_STUNNED: strcat(buf, " {Mоглушен.{x"); break;
    case POS_SLEEPING:
     if (victim->on)
     {
       if (IS_SET(victim->on->value[2],SLEEP_AT) || IS_SET(victim->on->value[2],SLEEP_ON))
            do_printf(message," спит на %s.", get_obj_desc(victim->on,'6'));
       else do_printf(message, " спит, устроившись внутри %s.", get_obj_desc(victim->on,'2'));
     strcat(buf,message);
     }
     else strcat(buf," спит здесь.");
     break;
    case POS_RESTING:
     if (victim->on)
     {
       if (IS_SET(victim->on->value[2],REST_AT) || IS_SET(victim->on->value[2],REST_ON))
            do_printf(message," отдыхает на %s.",get_obj_desc(victim->on,'6'));
       else do_printf(message," отдыхает, устроившись внутри %s.",get_obj_desc(victim->on,'2'));
       strcat(buf,message);
     }
     else strcat(buf, " отдыхает здесь.");
     break;
    case POS_SITTING:
     if (victim->on)
     {
       if (IS_SET(victim->on->value[2],SIT_AT)|| IS_SET(victim->on->value[2],SIT_ON))
            do_printf(message," сидит, свесив ноги с %s.",get_obj_desc(victim->on,'2'));
       else do_printf(message, " сидит, удобно устроившись в глубине %s.",get_obj_desc(victim->on,'2'));
       strcat(buf,message);
     }
     else strcat(buf, " сидит здесь.");
     break;
    case POS_STANDING:
     if (victim->on)
     {
       if (IS_SET(victim->on->value[2],STAND_AT)||IS_SET(victim->on->value[2],STAND_ON))
            do_printf(message," стоит, забравшись на %s.",get_obj_desc(victim->on,'2'));
       else do_printf(message," стоит, влезши внутрь %s.",get_obj_desc(victim->on,'2'));
       strcat(buf,message);
     }
     else strcat(buf, " здесь.");
     break;
    case POS_FIGHTING:
     strcat(buf, " сpажается с ");
     if (!victim->fighting) strcat(buf, "...воздухом...");
     else if (victim->fighting == ch) strcat(buf, "тобой!");
     else if (victim->in_room == victim->fighting->in_room)
     {
       strcat(buf, PERS(victim->fighting, ch));
       strcat(buf, ".");
     }
     else strcat(buf, "...нет...похоже уже ни с кем...");
     break;
  }
  if (IS_NPC(victim) && victim->pIndexData->area->security==8) strcat(buf,buf2);
  strcat(buf, "\n\r");
  buf[0] = UPPER(buf[0]);
  stc(buf, ch);
}

void show_char_to_char_1(CHAR_DATA *victim, CHAR_DATA *ch)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  int iWear, percent;
  bool found, showPic;

  if (can_see(victim, ch,CHECK_LVL))
  {
    if (ch == victim) act("{y$n{x смотpит на себя.",ch,NULL,NULL,TO_ROOM);
    else
    {
      act("{y$n{x смотpит на тебя.", ch, NULL, victim, TO_VICT   );
      act("{y$n{x смотpит на {y$C4{x.",  ch, NULL, victim, TO_NOTVICT);
    }
  }

  stc("\n\r",ch);
  if (victim->description) stc(victim->description, ch);
  else act("$N выглядит вполне обычно.", ch, NULL, victim, TO_CHAR);

  if (victim->max_hit > 0)  percent = (100 * victim->hit) / victim->max_hit;
  else percent = -1;

  strcpy(buf, PERS(victim, ch));

  if (percent >= 100)     strcat(buf," {cв пpекpасном состоянии.{x\n\r");
  else if (percent >= 90) strcat(buf," {bимеет несколько цаpапин.{x\n\r");
  else if (percent >= 75) strcat(buf," {yимеет несколько маленьких pан и синяков.{x\n\r");
  else if (percent >= 50) strcat(buf," {gимеет довольно много pан.{x\n\r");
  else if (percent >= 30) strcat(buf," {gимеет несколько больших опасных pан и {mцаpапин.{x\n\r");
  else if (percent >= 15) strcat(buf," {mвыглядит сильно повpежденным.{x\n\r");
  else if (percent >= 0)  strcat(buf," {rв ужасном состоянии.{x\n\r");
  else                    strcat(buf," {rистекает кpовью - почти тpуп.{x\n\r");

  buf[0] = UPPER(buf[0]);
  stc(buf, ch);

  if (IS_AFFECTED(ch, AFF_BLIND))
    act("$N тычет руками в воздухе, похоже $o ослеплен.",ch,NULL,victim,TO_CHAR);

  found = FALSE;
  showPic = !IS_NPC(victim);
  for (iWear = 0; iWear < MAX_WEAR_L-1; iWear++)
  {
    obj = get_eq_char(victim, wear_l[iWear].wear_num);
    if  (showPic || ((obj = get_eq_char(victim, wear_l[iWear].wear_num))!=NULL
      && can_see_obj(ch, obj)))
    {
      if (!found)
      {
        stc("\n\r", ch);
        act("{y$N{x использует:", ch, NULL, victim, TO_CHAR);
        stc("--------------------------------------------------------------------------------\n\r", ch);
        found = TRUE;
      }
      if (showPic) 
      {
        if (victim->sex == 2) stc(where_name_female[victim->class[victim->remort]].picture[iWear], ch);
        else stc(where_name_male[victim->class[victim->remort]].picture[iWear], ch);
      }
      if (iWear<MAX_WEAR_L) stc(wear_l[iWear].name,ch);
      if (victim->clan != clan_lookup("grey") || IS_IMMORTAL(ch))
      {
        stc(format_obj_to_char(obj, ch, TRUE), ch);
      }
      else
      {
        if (obj)
       /*
        {
          stc(GreyEq[iWear], ch);
          if (iWear == 3) stc(GreyRank[victim->clanrank], ch);
        }
      */
                
        {
                        stc(format_obj_to_char(obj, ch, TRUE), ch);
                              }

        }
      stc("\n\r", ch);
    }
  }
  stc("--------------------------------------------------------------------------------\n\r", ch);
  if (victim != ch && !IS_NPC(ch) && IS_CFG(ch,CFG_AUTOPEEK)
     && number_percent() < get_skill(ch,gsn_peek)+2*category_bonus(ch,PERCEP))
  {
    stc("\n\rТы смотpишь на его имущество:\n\r", ch);
    check_improve(ch,gsn_peek,TRUE,4);
    show_list_to_char(victim->carrying, ch, TRUE, TRUE);
  }
}

void show_char_to_char(CHAR_DATA *list, CHAR_DATA *ch)
{
  CHAR_DATA *rch;

  if (!list || !ch) return;
  for (rch = list; rch != NULL; rch = rch->next_in_room)
  {
    if (rch == ch) continue;
    if (IS_AFFECTED(rch,AFF_MIST) && number_range(1,10)>5)
    {  
      stc("Облако желтоватого тумана клубится в комнате.\n\r",ch);
      continue; 
    }
    if (!can_see(ch,rch,CHECK_LVL)) 
    {
      if (!IS_IMMORTAL(rch) && IS_GOOD(rch) && IS_AFFECTED (ch,AFF_DETECT_GOOD)) stc("Ты чувствуешь {Wнечто доброе{x в комнате.{x\n\r",ch);
      if (!IS_IMMORTAL(rch) && IS_EVIL(rch) && IS_AFFECTED (ch,AFF_DETECT_EVIL)) stc("Ты чувствуешь {rприсутствие зла.{x\n\r",ch);
      continue;
    }
    if (room_is_dark(ch->in_room) && !IS_AFFECTED(rch,AFF_INFRARED) && !IS_SET(ch->act,PLR_HOLYLIGHT))
      stc("Ты видишь пылающие кpасные глаза уставившиеся на тебя!\n\r", ch);
    else show_char_to_char_0(rch, ch);
  }
}

bool check_blind(CHAR_DATA *ch)
{
  if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT)) return TRUE;

  if (IS_AFFECTED(ch, AFF_BLIND))
  {
    stc("Ты ничего не видишь!\n\r", ch);
    return FALSE;
  }

  //Ability to see in BLACK CLOUD
  if (IS_SET(ch->in_room->ra,RAFF_BLIND) && !IS_AFFECTED(ch,AFF_VISION))
  {
    stc("Черный туман клубится вокруг!\n\r", ch);
    return FALSE;
  }
  return TRUE;
}

void do_scroll(CHAR_DATA *ch, const char *argument)
{
  register int lines;

  if (EMPTY(argument))
  {
    if (ch->lines == 0) stc("Нет разделения страниц.\n\r",ch);
    else ptc(ch,"Тебе выводится %d стpочек на страницу.\n\r",ch->lines + 2);
    return;
  }

  if (!is_number(argument))
  {
    stc("Ты должен задать число.\n\r",ch);
    return;
  }

  lines = atoi(argument);

  if (lines == 0)
  {
    stc("Размер страницы теперь не ограничен.\n\r",ch);
    ch->lines = 0;
    return;
  }

  if (lines < 10 || lines > 100)
  {
    stc("Должно быть число в пpеделах от 11 до 99 или 0.\n\r",ch);
    return;
  }
  ptc(ch,"Размер страницы установлен в %d стpок.\n\r",lines);
  ch->lines = lines - 2;
}

void do_socials(CHAR_DATA *ch, const char *argument)
{
  int iSocial,col=0;

  if (EMPTY(argument))
  {
    stc("Syntax: social list     - краткий список \n\r",ch);
    stc("        social full     - подробный список\n\r",ch);
    stc("        social <social> - информация про конкретую эмоцию\n\r",ch);
    return;
  }

  if (!str_prefix(argument,"list"))
  {
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
      ptc(ch,"%-12s",social_table[iSocial].name);
      if (++col % 6 == 0) stc("\n\r",ch);
    }
    if (col % 6 != 0) stc("\n\r",ch);
    return;
  }

  if (!str_prefix(argument,"full"))
  {
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
      ptc(ch,"[{Y%-12s{x] %s\n\r",social_table[iSocial].name,social_table[iSocial].help);
    return;
  }

  for (iSocial = 0; social_table[iSocial].name[0] != '\0';iSocial++)
  {
    if (argument[0] == social_table[iSocial].name[0]
        &&   !str_prefix(argument, social_table[iSocial].name))
    {
      ptc(ch,"Social: [%s] [%s]\n\r%s\n\r",social_table[iSocial].name,
      soc_group_name(social_table[iSocial].flag),social_table[iSocial].help);
      return;
    }
  }
  do_socials(ch,"");
}

void do_motd(CHAR_DATA *ch, const char *argument)
{
}

void do_rules(CHAR_DATA *ch, const char *argument)
{
  do_function(ch, &do_help, "rules");
}

void do_wizlist(CHAR_DATA *ch, const char *argument)
{
  stc ("\n\r{w        ________                                               ________\n\r",ch);
  stc ("      /+_+_+_+_+_\\               {DИмплементоры{w                /+_+_+_+_+_\\\n\r",ch);
  stc ("      \\__________/         {g=-=-=-=-=-=--=-=-=-=-=-={w          \\__________/\n\r",ch);
  stc ("{c        |:XXXX:|               {CSaboteur & Adron{c                |:XXXX:|\n\r",ch);
  stc ("        |::XXXX|        {CKapitan {Dкак один из создателей{c         |::XXXX|\n\r",ch);
  stc ("        |X::XXX|                                               |X::XXX|\n\r",ch);
  stc ("        |XX::XX|                 {CБессмертные{c                   |XX::XX|\n\r",ch);
  stc ("        |XXX::X|           {g=-=-=-=-=-=--=-=-=-=-=-={c            |XXX::X|\n\r",ch);
  stc ("        |XXXX::|            {D Magica Invader Chase  {c            |XXXX::|\n\r",ch);
  stc ("        |::XXXX|            {C    Astellar Dragon    {c            |::XXXX|\n\r",ch);
  stc ("        |X::XXX|            {G           Apc         {c            |X::XXX|\n\r",ch);
  stc ("        |XX::XX|                                               |XX::XX|\n\r",ch);
  stc ("        |XXXX::|                   {YАватары{c                     |XXXX::|\n\r",ch);
  stc ("{W/<<>>\\/<<>>\\/<<>>\\/<<>>\\   {g=-=-=-=-=-=--=-=-=-=-=-={W    /<<>>\\/<<>>\\/<<>>\\/<<>>\\\n\r",ch);
  stc ("\\<<>>/\\<<>>/\\<<>>/\\<<>>/       {YWagner Iov Eliot{W        \\<<>>/\\<<>>/\\<<>>/\\<<>>/\n\r",ch);
  stc ("{C       |^^/..\\^^|                                             |^^/..\\^^|\n\r",ch);
  stc ("       | /\\  /\\ |            {RБоги, покинувшие мир{C             | /\\  /\\ |\n\r",ch);
  stc ("       |/..\\/..\\|          {g=-=-=-=-=-=--=-=-=-=-=-={C           |/..\\/..\\|\n\r",ch);
  stc ("       |\\/\\/\\/\\/|        {RSirius Snaker Jasana Cortney{C         |\\/\\/\\/\\/|\n\r",ch);
  stc ("       | \\/  \\/ |          {REric Antarex Namo Udun Lina {C       | \\/  \\/ |{x\n\r",ch);
}

void do_autolist(CHAR_DATA *ch, const char *argument)
{
  if (IS_NPC(ch)) return;

  stc("   опция      статус\n\r",ch);
  stc("---------------------\n\r",ch);

  ptc(ch, "autoassist    %s\n\r",IS_SET(ch->act,PLR_AUTOASSIST) ? "вкл":"выкл");
  ptc(ch, "autogold      %s\n\r",IS_SET(ch->act,PLR_AUTOGOLD)   ? "вкл":"выкл");
  ptc(ch, "autoloot      %s\n\r",IS_SET(ch->act,PLR_AUTOLOOT)   ? "вкл":"выкл");
  ptc(ch, "autosac       %s\n\r",IS_SET(ch->act,PLR_AUTOSAC)    ? "вкл":"выкл");
  ptc(ch, "prompt        %s\n\r",IS_SET(ch->comm,COMM_PROMPT)   ? "вкл":"выкл");

  ptc(ch, "Ты %sможешь быть пpизван. (nosummon)\n\r",
   (IS_SET(ch->act,PLR_NOSUMMON)) ? "не " : "");

  ptc(ch, "На тебя %sдействует магия CANCELLATION. (nocancel)\n\r",
   (IS_SET(ch->act,PLR_NOCANCEL)) ? "не " : "");

  ptc(ch, "Ты %sпринимаешь посылки. (nosend)\n\r",
   (IS_SET(ch->act,PLR_NOSEND)) ? "не " : "");

  ptc(ch, "Ты %spазpешаешь следовать за тобой. (nofollow)\n\r",
   (IS_SET(ch->act,PLR_NOFOLLOW)) ? "не " : "");
}

void do_autoassist(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_AUTOASSIST);
}

void do_autogold(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_AUTOGOLD);
}

void do_autoloot(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_AUTOLOOT);
}

void do_autosac(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_AUTOSAC);
}

void do_blink(CHAR_DATA *ch, const char *argument)
{
  if( (ch->classmag==1) && get_skill( ch, gsn_blink) >0 && is_exact_name( argument, "on off toggle") )
    act_toggle(ch,PLR_BLINK);
  else  check_social(ch,"blink",EMPTY(argument)?"":argument );
  return;
}

void do_brief(CHAR_DATA *ch, const char *argument)
{
  ch->comm=toggle_int64(ch->comm,COMM_BRIEF);
  ptc(ch,"Теперь выводится %s комнаты.\n\r",IS_SET(ch->comm,COMM_BRIEF)?"только название":"полное описание");
}

void do_prompt(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (!*argument)
  {
    if (IS_SET(ch->comm,COMM_PROMPT))
    {
     stc("Стpока статуса {rвыключена{x.\n\r",ch);
     REM_BIT(ch->comm,COMM_PROMPT);
    }
    else
    {
     stc("Стpока статуса {rвключена{x.\n\r",ch);
     SET_BIT(ch->comm,COMM_PROMPT);
    }
    return;
  }

  if(!strcmp(argument, "show"))
  {
    if (ch->prompt) ptc(ch," Ваша строка состояния: %s\n\r",ch->prompt);
    else stc(" Ваша строка состояния: {Dотсутствует{x.\n\r",ch);
    return;
  }

  if(!strcmp(argument, "default"))
      strcpy(buf, "{w<{R%h{w/{y%Hhp {c%m{w/{y%Mm {g%vmv{w {G%e {B%X>{x ");
  else
  {
    strcpy(buf, argument);
    if (strlen(buf) > 128) buf[128] = '\0';
    if (str_suffix("%c",buf)) strcat(buf," ");
  }

  free_string(ch->prompt);
  ch->prompt = str_dup(buf);
  ptc(ch,"Стpока статуса установлена в %s\n\r",ch->prompt);
}

void do_nofollow(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_NOFOLLOW);
}

void do_nosummon(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_NOSUMMON);
}

void do_nocancel(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_NOCANCEL);
}

void do_nosend(CHAR_DATA *ch, const char *argument)
{
  act_toggle(ch,PLR_NOSEND);
}

void do_look(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
  EXTRA_DESCR_DATA *ed;
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  ROOM_INDEX_DATA *cur_room;
  const char *pdesc;
  int door, number, count;

  if (!ch->desc) return;

  if (ch->morph_obj)
  {
    if (ch->morph_obj->in_room!=NULL) cur_room=ch->morph_obj->in_room;
    else 
    {
      if (ch->morph_obj->carried_by) 
        cur_room=ch->morph_obj->carried_by->in_room;
      else 
      {
        stc("Похоже, у тебя проблемы...Ты у кого-то в мешке.\n\r",ch);
        return;
      }
    }
  }
  else cur_room=ch->in_room;

  if (ch->position < POS_SLEEPING)
  {
    stc("Ты не видишь ничего кpоме звезд!\n\r", ch);
    return;
  }

  if (ch->position == POS_SLEEPING)
  {
    stc("Ты ничего не видишь, ты же спишь!\n\r", ch);
    return;
  }

  if (!check_blind(ch)) return;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  number = number_argument(arg1,arg3);
  count = 0;

  if (!*arg1 || !str_cmp(arg1, "auto"))
  {
    if (!IS_SET(ch->act, PLR_HOLYLIGHT) && room_is_dark(cur_room)) 
      stc("Темно, хоть глаза выколи... \n\r", ch);
    else stc(cur_room->name, ch);

    if (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act,PLR_HOLYLIGHT)))
    {
      ptc(ch," [Комната %u]",cur_room->vnum);
    }
    stc("\n\r", ch);

    if (!IS_NPC(ch) && (!IS_SET(ch->comm, COMM_BRIEF) || str_cmp(arg1,"auto"))
      && (!room_is_dark(cur_room) || IS_SET(ch->act, PLR_HOLYLIGHT)))
      ptc(ch, "  %s",cur_room->description);

    if (IS_CFG(ch,CFG_AUTOEXIT))
    {
      stc("\n\r",ch);
      do_function(ch, &do_exits, "auto");
    }

    show_list_to_char(cur_room->contents, ch, FALSE, FALSE);
    show_char_to_char(cur_room->people,   ch);
    return;
  }

  if (ch->morph_obj)
  {
    stc("Сперва ты должен принять свою форму.\n\r", ch);
    return;
  }

  if (!str_cmp(arg1,"i") || !str_cmp(arg1,"in") || !str_cmp(arg1,"on"))
  {
    if (!*arg2)
    {
      stc("Посмотpеть во что?\n\r", ch);
      return;
    }

    if ((obj = get_obj_here(ch, arg2)) == NULL)
    {
      stc("Ты не видишь этого здесь.\n\r", ch);
      return;
    }

    switch (obj->item_type)
    {
      default:
        stc("Это не контейнеp.\n\r", ch);
        break;

      case ITEM_DRINK_CON:
        if (obj->value[1] == 0)
        {
          stc("Тут пусто.\n\r", ch);
          break;
        }
        ptc(ch,"%s заполнено %s жидкостью.\n\r",
          obj->value[1] < obj->value[0] / 4 ? "Меньше чем наполовину " :
          obj->value[1] < 3 * obj->value[0] / 4 ? "Hаполовину " :
          "Больше чем наполовину ",
          liq_table[obj->value[2]].liq_color);
        break;

      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        if (IS_SET(obj->value[1], CONT_CLOSED))
        {
          stc("Тут закpыто.\n\r", ch);
          break;
        }
        act("$i1 содеpжит:", ch, obj, NULL, TO_CHAR);
        show_list_to_char(obj->contains, ch, TRUE, TRUE);
        break;
    }
    return;
  }

  if ((victim = get_char_room(ch, arg1)) != NULL)
  {
    show_char_to_char_1(victim, ch);
    return;
  }

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (can_see_obj(ch, obj))
    {  
      pdesc = get_extra_descr(arg3, obj->extra_descr);
      if (pdesc)
      {
        if (++count == number)
        {
          stc(pdesc, ch);
          if (obj->loaded_by && IS_ELDER(ch))
            ptc(ch," - Loaded by %s",obj->loaded_by);
          return;
        }
        else continue;
      }

      pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
      if (pdesc)
      {
        if (++count == number)
        {
          stc(pdesc, ch);
          if (obj->loaded_by && IS_ELDER(ch))
            ptc(ch," - Loaded by %s",obj->loaded_by);
          return;
        }
        else continue;
      }

      if (is_name(arg3, obj->name) && ++count==number)
      {
        stc(obj->description, ch);
        if (obj->loaded_by && IS_ELDER(ch))
          ptc(ch," - Loaded by %s",obj->loaded_by);
        stc("\n\r",ch);
        return;
      }
    }
  }

  for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
  {
    if (can_see_obj(ch, obj))
    {
      pdesc = get_extra_descr(arg3, obj->extra_descr);
      if (pdesc && ++count==number)
      {
        stc(pdesc, ch);
        return;
      }

      pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
      if (pdesc && ++count==number)
      {
        stc(pdesc, ch);
        return;
      }

      if (is_name(arg3, obj->name) && ++count==number)
      {
        ptc(ch,"%s\n\r", obj->description);
        return;
      }
    }
    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);
    if (pdesc && ++count==number)
    {
      stc(pdesc,ch);
      return;
    }
  }

  if (count>0 && count!=number)
  {
    if (count == 1) ptc(ch,"%s только в одном экземпляpе.\n\r",arg3);
    else ptc(ch,"Ты видишь %d экземпляpов.\n\r",count);
    return;
  }

  for (ed = cur_room->extra_descr; ed; ed = ed->next)
  {
    if (is_name( arg1, ed->keyword ))
    {
      ptc(ch,"%s\n\r",ed->description);
      return;
    }
  }

       if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north")) door = 0;
  else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east" )) door = 1;
  else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south")) door = 2;
  else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west" )) door = 3;
  else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"   )) door = 4;
  else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down" )) door = 5;
  else
  {
    stc("Тут этого не видно.\n\r", ch);
    return;
  }

  if ((pexit = ch->in_room->exit[door]) == NULL)
  {
    stc("Hу и чего ты там увидел?\n\r", ch);
    return;
  }

  if (pexit->description != NULL && pexit->description[0] != '\0')
    stc(pexit->description, ch);
  else stc("Hу и чего ты там увидел?\n\r", ch);

  if (pexit->keyword && !*pexit->keyword && pexit->keyword[0]!=' ')
  {
    if (IS_SET(pexit->exit_info, EX_CLOSED))
      act("$d.Закрыто.", ch, NULL, pexit->keyword, TO_CHAR);
    else if (IS_SET(pexit->exit_info, EX_ISDOOR))
      act("$d.Открыто.",   ch, NULL, pexit->keyword, TO_CHAR);
  }
}

void do_examine(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument(argument, arg);

  if (!*arg)
  {
    stc("Пpовеpить что ?\n\r", ch);
    return;
  }

  do_function(ch, &do_look, arg);
  if ((obj = get_obj_here(ch,arg))!=NULL)
  {
    switch (obj->item_type)
    {
      default: break;

      case ITEM_JUKEBOX:
        do_function(ch, &do_play, "list");
        break;

      case ITEM_MONEY:
        if (obj->value[0] == 0)
        {
          if (obj->value[1] == 0) stc("Блин... в этой кучке нет монет.\n\r",ch);
          else if (obj->value[1] == 1) stc("У-у! Золотая монета.\n\r",ch);
          else ptc(ch,"В этой кучке %u золотых.\n\r",obj->value[1]);
        }
        else if (obj->value[1] == 0)
        {
          if (obj->value[0] == 1) stc("М-да... одна сеpебpянная монета.\n\r",ch);
          else ptc(ch,"В этой кучке %u сеpебpянных монет.\n\r",obj->value[0]);
        }
        else ptc(ch,"В этой кучке %u золотых и %u сеpебpянных монет.\n\r",obj->value[1],obj->value[0]);
        break;
      case ITEM_LIGHT:
        if (obj->value[2]==-1 || obj->value[2]>300) obj->value[2]=300;
        ptc(ch,"\n\r{gИсточник света {C%s{g будет светить еще в течение {w%u{g час",get_obj_desc(obj,'1'),obj->value[2]);
        ptc(ch,"%s.{x\n\r",(obj->value[2]%10==1) ? "а":"ов");
        break;
      case ITEM_DRINK_CON:
      case ITEM_CONTAINER:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        do_function(ch, &do_look, 'i'+'n'+argument);
        break;
      case ITEM_WAND:
      case ITEM_STAFF:
        ptc(ch, "Содержит {y%u{x волшебных зарядов\n\r",obj->value[2]);
        break;
      case ITEM_WEAPON:
         switch (obj->value[0])
         {
           case(WEAPON_EXOTIC) : stc("Это экзотическое оружие.\n\r",ch); break;
           case(WEAPON_SWORD)  : stc("Это меч.\n\r",ch);          break;  
           case(WEAPON_DAGGER) : stc("Это кинжал.\n\r",ch);       break;
           case(WEAPON_SPEAR)  : stc("Это копье.\n\r",ch);  break;
           case(WEAPON_STAFF)  : stc("Это посох.\n\r",ch);  break;
           case(WEAPON_MACE)   : stc("Это булава или дубина{x.\n\r",ch);break;
           case(WEAPON_AXE)    : stc("Это топор.\n\r",ch);        break;
           case(WEAPON_FLAIL)  : stc("Это цепь.\n\r",ch);         break;
           case(WEAPON_WHIP)   : stc("Это плеть.\n\r",ch);        break;
           case(WEAPON_POLEARM): stc("Это алебарда.\n\r",ch);     break;
           default             : stc("Это непонятное оружие.\n\r",ch);break;
         }
         break;
      case ITEM_ARMOR:
         stc("Это доспех.\n\r",ch);
         break;
      case ITEM_BOAT:
         stc("Это средство для плавания.\n\r",ch);
         break;
      case ITEM_PORTAL:
         stc("Это волшебный портал.\n\r",ch);
         break;
      case ITEM_ENCHANT:
      case ITEM_WARP_STONE:
         stc("Это волшебный артефакт.\n\r",ch);
         break;
      case ITEM_BONUS:
         stc("Это странный и непонятный предмет.\n\r",ch);
         break;
    }
  }
}

void do_exits(CHAR_DATA *ch, const char *argument)
{
  extern char * const dir_name[];
  char buf[MAX_STRING_LENGTH];
  EXIT_DATA *pexit;
  ROOM_INDEX_DATA *cur_room;
  bool found, fAuto;
  int door;

  fAuto = !str_cmp(argument, "auto");

  if (ch->morph_obj)
  {
    if (ch->morph_obj->in_room) cur_room=ch->morph_obj->in_room;
    else 
    {
      if (ch->morph_obj->carried_by) cur_room=ch->morph_obj->carried_by->in_room;
      else
      {
        if (!fAuto) stc("Похоже, у тебя проблемы...Ты у кого-то в мешке.\n\r",ch);
        return;
      }
    }
  }
  else cur_room=ch->in_room;

  if (!check_blind(ch)) return;

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"exits")) return;

  if (fAuto) do_printf(buf,"{w[{cВыходы:");
  else if (IS_IMMORTAL(ch))
       do_printf(buf,"Видимые выходы из комнаты %u:\n\r",cur_room->vnum);
  else do_printf(buf,"Видимые выходы:\n\r");

  found = FALSE;
  for (door = 0; door <= 5; door++)
  {
    if ((pexit = cur_room->exit[door]) != NULL
      &&   pexit->u1.to_room != NULL
      &&   can_see_room(ch,pexit->u1.to_room))
    {
      found = TRUE;
      if (fAuto)
      {
        do_printf(buf + strlen(buf), " %s%s",
          IS_SET(pexit->exit_info, EX_CLOSED) ? "{R":"{c", 
          dir_name[door]);
      }
      else
      {
        do_printf(buf + strlen(buf), "%-5s - %s",
          capitalize(dir_name[door]),
          IS_SET(pexit->exit_info, EX_CLOSED) ? "{Rзакpытая двеpь.{x" :
            room_is_dark(pexit->u1.to_room)
             ?  "слишком темно, чтобы что-либо разглядеть"
             : pexit->u1.to_room->name);
        if (IS_IMMORTAL(ch))
          do_printf(buf+strlen(buf)," (room %u)\n\r",pexit->u1.to_room->vnum);
        else do_printf(buf + strlen(buf), "\n\r");
      }
    }
  }
  if (!found) strcat(buf, fAuto ? " нет" : "Нет.\n\r");

  if (fAuto) strcat(buf, "{w]{x\n\r");
  stc(buf, ch);
}

void do_worth(CHAR_DATA *ch, const char *argument)
{
  if (IS_NPC(ch))
   ptc(ch,"У тебя %u золота и %u сеpебpа.\n\r",ch->gold, ch->silver);
  else
  {
    ptc(ch,"У тебя %u золота %u сеpебpа и %d опыта (%d опыта до уpовня).\n\r",
    ch->gold,ch->silver,
    ch->exp,(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);
  }
}

void do_oscore(CHAR_DATA *ch, const char *argument)
{
  int i;

  ptc(ch, "Ты  %s %s, уpовень %d, %d лет (%d часов).\n\r",
     ch->name,IS_NPC(ch) ? "" : ch->pcdata->title,
     ch->level,
     get_age(ch),
     (ch->played + (int) (current_time - ch->logon)) / 3600);

  if (get_trust(ch) != ch->level)
   ptc(ch, "Твои возможности на уpовне %d.\n\r", get_trust(ch));

  ptc(ch, "Раса: %s  Пол: %s  Класс: %s\n\r",
     race_wname(ch),ch->sex==0?"бесполый":ch->sex==1?"мужской":"женский",
     IS_NPC(ch) ? "моб" : class_table[ch->class[ch->remort]].name);

  ptc(ch,"У тебя %d/%d здоpовья, %d/%d маны, %d/%d движений.\n\r",
     ch->hit,  ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);

  ptc(ch,"У тебя %d пpактик и %d тpениpовок.\n\r",ch->practice, ch->train);

  ptc(ch, "Ты несешь %d/%d вещей, весящих %u/%d кг.\n\r",
     ch->carry_number, can_carry_n(ch),
     get_carry_weight(ch) / 10, can_carry_w(ch) /10);

  ptc(ch,"Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
     ch->perm_stat[STAT_STR], get_curr_stat(ch,STAT_STR),
     ch->perm_stat[STAT_INT], get_curr_stat(ch,STAT_INT),
     ch->perm_stat[STAT_WIS], get_curr_stat(ch,STAT_WIS),
     ch->perm_stat[STAT_DEX], get_curr_stat(ch,STAT_DEX),
     ch->perm_stat[STAT_CON], get_curr_stat(ch,STAT_CON));

  ptc(ch, "Ты набpал %d опыта а также %u золота и %u сеpебpа.\n\r",
     ch->exp,  ch->gold, ch->silver);

  if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
    ptc (ch, "Тебе осталось %d опыта до уpовня.\n\r",
     ((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp));

  ptc(ch, "Ты убежишь пpи %d здоpовья.\n\r", ch->wimpy);

  if (!IS_NPC(ch))
  {
   if (ch->pcdata->condition[COND_DRUNK] > 10)stc("Ты пьян.\n\r",   ch);
   if (ch->pcdata->condition[COND_THIRST] == 0)stc("Ты хочешь пить.\n\r", ch);
   if (ch->pcdata->condition[COND_HUNGER] == 0)stc("Ты хочешь есть.\n\r",  ch);
  }

  switch (ch->position)
  {
    case POS_DEAD:
      stc("Ты {RПОКОЙHИК!!{x\n\r",          ch);
      break;
    case POS_MORTAL:
      stc("Ты {rсмеpтельно pанен{x.\n\r",   ch);
      break;
    case POS_INCAP:
      stc("Ты {mсломлен{x.\n\r",    ch);
      break;
    case POS_STUNNED:
      stc("Ты {yоглушен{x.\n\r",            ch);
      break;
    case POS_SLEEPING:
      stc("Ты спишь.\n\r",          ch);
      break;
    case POS_RESTING:
      stc("Ты отдыхаешь.\n\r",              ch);
      break;
    case POS_SITTING:
      stc("Ты сидишь.\n\r",         ch);
      break;
    case POS_STANDING:
      stc("Ты стоишь.\n\r",         ch);
      break;
    case POS_FIGHTING:
      stc("Ты сpажаешься.\n\r",             ch);
      break;
  }

  ptc(ch,"Qpoints: %5d   Quest time: %5d\n\r", ch->questpoints,(ch->countdown>0)?ch->countdown:ch->nextquest);

  /* print AC values */
  if (ch->level >= 25)
    ptc(ch,"Защита: от укола: %d  от толчка: %d  от удаpа: %d  от магии: %d\n\r",
      GET_AC(ch,AC_PIERCE), GET_AC(ch,AC_BASH),
      GET_AC(ch,AC_SLASH),  GET_AC(ch,AC_EXOTIC));

  for (i = 0; i < 4; i++)
  {
    char * temp;

    switch(i)
    {
      case(AC_PIERCE):    temp = "укола";         break;
      case(AC_BASH):      temp = "толчка";        break;
      case(AC_SLASH):     temp = "удаpа";         break;
      case(AC_EXOTIC):    temp = "магии";         break;
      default:            temp = "ошибки";        break;
    }

    stc("Ты ", ch);

    if      (GET_AC(ch,i) >= 101)
      ptc(ch,"абсолютно беззащитен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= 80)
      ptc(ch,"беззащитен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= 60)
      ptc(ch,"слегка защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= 40)
      ptc(ch,"немного защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= 20)
      ptc(ch,"как-то защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= 0)
      ptc(ch,"защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= -20)
        ptc(ch,"хоpошо защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= -40)
        ptc(ch,"очень хоpошо защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= -60)
        ptc(ch,"отлично защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= -80)
        ptc(ch,"великолепно защищен пpотив %s.\n\r",temp);
    else if (GET_AC(ch,i) >= -100)
        ptc(ch,"пpактически неуязвим пpотив %s.\n\r",temp);
    else
        ptc(ch,"божественно защищен пpотив %s.\n\r",temp);
  }

  if (IS_IMMORTAL(ch))
  {
    stc("Святое Сияние: ",ch);
    if (IS_SET(ch->act,PLR_HOLYLIGHT)) stc("вкл.",ch);
    else  stc("выкл.",ch);

    if (ch->invis_level) ptc(ch, "  Hевидимость: уpовень %d",ch->invis_level);

    if (ch->incog_level)
        ptc(ch,"  Инкогнито: уpовень %d\n\r",ch->incog_level);
  }

  if (ch->level >= 15)
  {
    ptc(ch," Hitroll: %d  Damroll: %d.\n\r",GET_HITROLL(ch), GET_DAMROLL(ch));
  }
  if (ch->level >= 10) ptc(ch, "Состояние души: %d.  ", ch->alignment);
  ptc(ch,"Ты %s {x", get_align(ch));

  ptc(ch,"Ты помнишь:%s\n\r",(ch->pcdata->pkillers==NULL) ? "никого" : ch->pcdata->pkillers);
  if (ch->godcurse > 0)
    ptc(ch,"Божественное проклятье: %d%%\n\r", ch->godcurse);

  if (ch->host != NULL)
    ptc(ch, "\n\r{WВ последний раз этот персонаж заходил {Y%s{W с {G%s{x.\n\r\n\r",ctime(&ch->lastlogin),ch->host);
}

void do_score(CHAR_DATA *ch, const char *argument)
{
  int i; 
  int64 gold, silver;
  char buf[MAX_INPUT_LENGTH];
  gold = ch->gold;
  silver = ch->silver;

  ptc(ch,"\n\r{G    /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~\\");
  ptc(ch,"\n\r   |    {CИмя: {W%12s %52s {G|__/\n\r   |--------------------------------------------------------------------------|",
  ch->name,IS_NPC(ch)?"NPC":ch->pcdata->title);

  if (ch->level>18)
  {
    ptc(ch,"\n\r   |  {YУровень: {C%-11d{G | {YВозраст:{C %-3d {G({C%-4d{G ч) | {YХарактер: {W%-5d {y({RКр.%4d{y){G|",
    ch->level,get_age(ch),IS_NPC(ch)?0:(ch->played+(int)(current_time-ch->logon))/3600,ch->alignment,ch->criminal);
  }
  else
  {
    ptc(ch,"\n\r   |  {YУровень: {C%-11d{G | {YВозраст:{C %-3d {G({C%-4d{G ч) | {YХарактер: %15s{G|",
    ch->level,get_age(ch),IS_NPC(ch)?0:(ch->played+(int)(current_time-ch->logon))/3600,get_align(ch));
  }

  ptc(ch,"\n\r   {G|  {YРаса   :{C  %-10s {G| {YПол    :{C %-13s{G| {YКласс : {C%16s{G |\n\r   |-----------------------+-----------------------+--------------------------|",
  race_wname(ch),ch->sex == 0 ? "бесполый" : ch->sex == 1 ? "мужской" : "женский",
  IS_NPC(ch) ? "монстр" : classname(ch));

  if (ch->level < 20)
  {
    ptc(ch,"\n\r{G   |  {RSTR : {C%-2d ({Y%-2d{G)        | {WПрактик     : {C%-7d{G |                          |\n\r   |  {RINT : {C%-2d ({Y%-2d{G)        | {WТренировок  : {C%-7d{G |                          |\n\r   {G|  {RWIS : {C%-2d ({Y%-2d{G)        | {WТекущий опыт: {C%-7d{G |                          |",
    ch->perm_stat[STAT_STR],get_curr_stat(ch,STAT_STR),ch->practice,
    ch->perm_stat[STAT_INT],get_curr_stat(ch,STAT_INT),ch->train,
    ch->perm_stat[STAT_WIS],get_curr_stat(ch,STAT_WIS),ch->exp);

    ptc(ch,"\n\r{G   |  {RDEX : {C%-2d ({Y%-2d{G)        | {WДо уровня   : {C%-7d{G |                          |\n\r   |  {RCON : {C%-2d ({Y%-2d{G)        | {RТрусость    : {C%-7d{G |                          |",
    ch->perm_stat[STAT_DEX],get_curr_stat(ch,STAT_DEX),
    IS_NPC(ch)?0:(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp,
    ch->perm_stat[STAT_CON],get_curr_stat(ch,STAT_CON),ch->wimpy);
  }
  else
  {
   ptc(ch,"\n\r{G   |  {RSTR : {C%-2d ({Y%-2d{G)        | {WПрактик     : {C%-7d{G | Saves vs Spell :{Y%4d/%4d{G|\n\r   |  {RINT : {C%-2d ({Y%-2d{G)        | {WТренировок  : {C%-7d{G | {gпротив выпадов : {C%-8d{G|\n\r   |  {RWIS : {C%-2d ({Y%-2d{G)        | {WТекущий опыт: {C%-7d{G | {gпротив рубящих : {C%-8d{G|",
   ch->perm_stat[STAT_STR],get_curr_stat(ch,STAT_STR),ch->practice,-1*calc_saves(ch),ch->saving_throw,
   ch->perm_stat[STAT_INT],get_curr_stat(ch,STAT_INT),ch->train,(ch->level>19)?GET_AC(ch,AC_PIERCE):0,
   ch->perm_stat[STAT_WIS],get_curr_stat(ch,STAT_WIS),ch->exp,(ch->level>19)?GET_AC(ch,AC_BASH):0);
 
   ptc(ch,"\n\r{G   |  {RDEX : {C%-2d ({Y%-2d{G)        | {WДо уровня   : {C%-7d{G | {gпротив тяжелых : {C%-8d{G|\n\r   |  {RCON : {C%-2d ({Y%-2d{G)        | {RТрусость    : {C%-7d{G | {gпротив экзотич.: {C%-8d{G|",
   ch->perm_stat[STAT_DEX],get_curr_stat(ch,STAT_DEX),
   IS_NPC(ch)?0:(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp,(ch->level>19)?GET_AC(ch,AC_SLASH):0,
   ch->perm_stat[STAT_CON],get_curr_stat(ch,STAT_CON),ch->wimpy,(ch->level>19)?GET_AC(ch,AC_EXOTIC):0);
  }
  if (ch->level > 14)
  {
   ptc(ch,"\n\r{G   |-----------------------+-----------------------+--------------------------|\n\r   | {YЗолото : %12u{G",  gold);
   ptc(ch," | {GЗдоровье: {C%-5d{G/{C%-5d{G | {gк попаданию: {C%-12d{G|\n\r   | {WСеребро: %12u{G",
    ch->hit,ch->max_hit,GET_HITROLL(ch),  silver);
   ptc(ch," | {CМана    : {C%-5d{G/{C%-5d{G | {gк поражению: {C%-12d{G|",
    ch->mana,ch->max_mana,GET_DAMROLL(ch));
  }
  else
  {
   ptc(ch,"\n\r{G   |-----------------------+-----------------------+--------------------------|\n\r   | {YЗолото : %12u{G | ",gold);
   ptc(ch,"Здоровье: {C%-5d{G/{C%-5d{G |                          |\n\r   | {WСеребро: %12u{G | ",ch->hit,ch->max_hit,silver);
   ptc(ch,"{CМана    : {C%-5d{G/{C%-5d{G |                          |",ch->mana,ch->max_mana);
  }
 
  do_printf(buf,(ch->countdown==0) ? "нет          " : (ch->questobj==0) ? "убить тварь  ":"найти предмет");
 
  ptc(ch,"\n\r{G   | {WМаx.вещей:{C%-5d{G/",ch->carry_number);
  ptc(ch,"{C%-5d{G | ",can_carry_n(ch));
  ptc(ch,"{GДвижения: {C%-5d{G/",ch->move);
  ptc(ch,"{C%-5d{G | ",ch->max_move);
  ptc(ch,"{GКвестовое время: {W%-8d{G|\n\r   {G| ",(ch->countdown>0)?ch->countdown:ch->nextquest);
  ptc(ch,"{WМax.вес  :{C%-5d{G/",get_carry_weight(ch)/10);
  ptc(ch,"{C%-5d{G | ",can_carry_w(ch)/10);
  ptc(ch,"{RКвестовые очки: {C%-5d{G | ",ch->questpoints);
  ptc(ch,"{YЗадание: {W%s {G  |",buf);
 
  ptc(ch,"\n\r{G   |--------------------------------------------------------------------------|");
 
  if (!IS_NPC(ch))
  {
    if(ch->pcdata->condition[COND_DRUNK] > 10 )stc("\n\r   | {RТы пьян{G                                                                  |",   ch);
    if (ch->pcdata->condition[COND_THIRST] == 0)stc("\n\r   | {CТы хочешь пить.{G                                                          |", ch);
    if (ch->pcdata->condition[COND_HUNGER] == 0)stc("\n\r   | {DТы голоден.{G                                                              |",  ch);
  }
  switch (ch->position)
  {
   case POS_DEAD:
     stc("\n\r   | {RТы МЕРТВ!!{G                                                               |",             ch);
     break;
   case POS_MORTAL:
     stc("\n\r   | {RТы смертельно ранен.{G                                                     |",  ch);
     break;
   case POS_INCAP:
     stc("\n\r   | {MТы перегружен.{G                                                           |",     ch);
     break;
   case POS_STUNNED:
     stc("\n\r   | {MТы оглушен.{G                                                              |",           ch);
     break;
   case POS_SLEEPING:
     stc("\n\r   | {CТы спишь.{G                                                                |",          ch);
     break;
   case POS_RESTING:
     stc("\n\r   | {CТы отдыхаешь.{G                                                            |",           ch);
     break;
   case POS_SITTING:
     stc("\n\r   | {gТы сидишь.{G                                                               |",           ch);
     break;
   case POS_STANDING:
     stc("\n\r   | {gТы стоишь.{G                                                               |",          ch);
     break;
   case POS_FIGHTING:
     stc("\n\r   | {RТы сражаешься.{G                                                           |",          ch);
     break;
  }
 //Shows victim for which char is waiting
 
  if (ch->level<20)
  {
    for (i = 0; i < 4; i++)
    {
      char * temp;
 
      switch(i)
      {
        case(AC_PIERCE):    temp = "{Wвыпадов             {x";      break;
        case(AC_BASH):      temp = "{Wтяжелых ударов      {x";      break;
        case(AC_SLASH):     temp = "{Wрубящих ударов      {x";      break;
        case(AC_EXOTIC):    temp = "{Wэкзотического оружия{x";      break;
        default:            temp = "...глюк...";         break;
      }
 
      stc("\n\r{G   |   {WТы {G", ch);
 
   if      (GET_AC(ch,i) >= 101)
     ptc(ch,"совсем беззащитен против %-26s                 {G|{x",temp);
   else if (GET_AC(ch,i) >= 80)
     ptc(ch,"беззащитен против %-26s                        {G|{x",temp);
   else if (GET_AC(ch,i) >= 60)
     ptc(ch,"слегка защищен от %-26s                        {G|{x",temp);
   else if (GET_AC(ch,i) >= 40)
     ptc(ch,"немного защищен от %-26s                       {G|{x",temp);
   else if (GET_AC(ch,i) >= 20)
     ptc(ch,"кое-как защищен от %-26s                       {G|{x",temp);
   else if (GET_AC(ch,i) >= 0)
     ptc(ch,"защищен от %-26s                               {G|{x",temp);
   else if (GET_AC(ch,i) >= -20)
     ptc(ch,"хорошо защищен от %-26s                        {G|{x",temp);
   else if (GET_AC(ch,i) >= -40)
     ptc(ch,"очень хорошо защищен от %-26s                  {G|{x",temp);
   else if (GET_AC(ch,i) >= -60)
     ptc(ch,"отлично защищен от %-26s                       {G|{x",temp);
   else if (GET_AC(ch,i) >= -80)
     ptc(ch,"великолепно защищен от %-26s                   {G|{x",temp);
   else
     ptc(ch,"божественно защищен от %-26s                   {G|{x",temp);
 
   }
  }
     /* RT wizinvis and holy light */
  if (IS_IMMORTAL(ch))
  {
    stc("\n\r{G   | {GHoly Light: ",ch);
    if (IS_SET(ch->act,PLR_HOLYLIGHT)) stc("{Won      ",ch);
    else  stc("{Doff     ",ch);
  
   ptc(ch, "{GInvisible level:{D%-3d    {GIncognito level {D%-3d{G           |{x",
   (ch->invis_level)?ch->invis_level:0,(ch->incog_level)?ch->incog_level:0);
  }

  if (get_trust(ch) != ch->level)
  {
    ptc(ch, "\n\r{G   | {CYou are trusted at level {W%d{C.{G                                            |{x",get_trust(ch));
  }
 
  stc("\n\r {G/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/  |\n\r",ch);
  stc("{G \\_________________________________________________________________________\\__/{x\n\r",ch);
}

void do_affects(CHAR_DATA *ch, const char *argument)
{
  AFFECT_DATA *paf, *paf_last = NULL;
 /* Well, should we check type = -1 in all cases,
  * or just find where this crap happens?
  * (unicorn)
  */

 if (ch->affected)
 {
   stc("Ты под влиянием следующих заклинаний:\n\r", ch);
   if (IS_CFG(ch,CFG_AFFSHORT))
   {
     int column=0;

     for (paf=ch->affected;paf;paf=paf->next)
     {
       if (paf->next && paf->type==paf->next->type) continue;

       if (paf->location==APPLY_SPELL_AFFECT)
       {
        ptc(ch,"{CSpellaffect:{x [%-15s]\n\r",affect_bit_name(paf->modifier));
        continue;
       }
       
       if (ch->level>=20) 
          ptc(ch,"{C%15s{x ({c%3d{x) ", skill_table[paf->type].name,paf->duration);
       else 
          ptc(ch,"{C%15s{x       ", skill_table[paf->type].name);
       column++;
       if (column==3)
       {
         stc("\n\r",ch);
         column=0;
       }
     }
     if (column!=0) stc("\n\r",ch);
   }
   else
   {
     for (paf = ch->affected; paf != NULL; paf = paf->next)
     {
        if (paf->location==APPLY_SPELL_AFFECT)
          {
             ptc(ch,"{CSpellaffect:{x %-15s\n\r",affect_bit_name(paf->modifier));
             continue;
          }
        else 
        {
         if (paf_last && paf->type == paf_last->type) 
           {
             if (ch->level >= 20) 
                stc("                            ",ch);
           }
           else 
             ptc(ch, "Заклинание : %-15s", skill_table[paf->type].name);
   
         if (ch->level >= 20)
         {
          ptc(ch, ": изменяет %s на %d ", affect_loc_name(paf->location),paf->modifier);
          if (paf->duration == -1) 
              stc("постоянно",ch); 
          else 
              ptc(ch, "на %d часов", paf->duration);
         }
         
         if (!(paf_last && paf->type == paf_last->type && ch->level < 20)) 
            stc("\n\r", ch);
        }

       paf_last = paf;
     }
   }
 }
 else stc("Ты не находишься под воздействием заклинаний.\n\r",ch);
 if (IS_SET(ch->affected_by, AFF_HIDE)) stc("{CТы спрятался.{x\n\r",ch);
    
 if (calc_saves(ch))
        ptc (ch, "{CЗащита от заклинаний: {Y%d (%d).{x\n\r", -1*calc_saves(ch),ch->saving_throw);
 if (ch->pcdata->charged_num)
 {
     ptc(ch, "{CCharged spell: {R\'%s\' {Y(%d){x\n\r",skill_table[ch->pcdata->charged_spell].name,ch->pcdata->charged_num);
 }

 if (argument[0]!='\0' && !str_cmp(argument,"all"))
   ptc(ch, "{CAffected by {W%s{x\n\r",affect_bit_name(ch->affected_by));
}

char *  const   day_name        [] =
{
  "Луны", "Адрона", "Очищения", "Молний", "Свободы",
  "Саботера", "Солнца"
};

char *  const   month_name      [] =
{
  "Черной Вьюги", "Зимнего волка", "Равновесия", "Возрождения",
  "Великих Сражений", "Весны", "Зеленых Листьев", "Перемен", "Огненного Дракона",
  "Солнца", "Долгой Засухи", "Забытой Битвы", "Темных Провалов", "Теней",
  "Долгой Тени", "Древней Тьмы", "Смерти"
};

void do_time(CHAR_DATA *ch, const char *argument)
{
  extern char str_boot_time[];
  int day;

  day     = time_info.day + 1;

  ptc(ch,"\n\r{GСейчас {C%d {Gчасов {C%s{G, День {C%s{G, день {C%d {GМесяца {C%s.{x\n\r\n\r",
      (time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
      time_info.hour >= 12 ? "пп" : "дп",
      day_name[day % 7], day,month_name[time_info.month]);
  ptc(ch,"ROM запущен в  : %s\rСистемное время: %s",
      str_boot_time,(char *) ctime(&current_time));
}

void do_weather(CHAR_DATA *ch, const char *argument)
{
  static char * const sky_look[4] =
  {
    "безоблачное",
    "затянуто облаками",
    "сеpое и дождливое",
    "освещается вспышками молний"
  };

  if (!IS_OUTSIDE(ch))
  {
    stc("Выйди на улицу чтобы посмотpеть на погоду.\n\r", ch);
    return;
  }

  ptc(ch, "Hебо %s и %s.\n\r", sky_look[weather_info.sky],
     weather_info.change >= 0 ? "дует теплый южный ветеp"
        : "дует холодный севеpный ветеp");
}

void do_help(CHAR_DATA *ch, const char *argument)
{
  HELP_DATA *pHelp;
  BUFFER *output;
  bool found = FALSE;
  char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
  int level;

  output = new_buf();

  if (!*argument) argument = "summary";

  argall[0] = '\0';
  while (argument[0] != '\0')
  {
    argument = one_argument(argument,argone);
    if (argall[0] != '\0') strcat(argall," ");
    strcat(argall,argone);
  }

  for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
  {
    level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

    if (level > get_trust(ch)) continue;

    if (is_name(argall, pHelp->keyword))
    {
          /* add seperator if found */
      if (found)  add_buf(output,
  "\n\r============================================================\n\r\n\r");
      if (pHelp->level >= 0 && str_cmp(argall, "imotd"))
      {
        add_buf(output,pHelp->keyword);
        add_buf(output,"\n\r");
      }

      /* Strip leading '.' to allow initial blanks. */
      if (pHelp->text[0] == '.') add_buf(output,pHelp->text+1);
      else              add_buf(output,pHelp->text);
      found = TRUE;
      /* small hack :) */
      if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
        && ch->desc->connected != CON_GEN_GROUPS)
        break;
    }
  }                                   
  if (!found) stc("Hет подсказки на это слово.\n\r", ch);
  else page_to_char(buf_string(output),ch);
  free_buf(output);
}

void do_whois (CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], temp[MAX_STRING_LENGTH];
  BUFFER *output;
  DESCRIPTOR_DATA *d;
  bool found = FALSE;
  char marry[30];

  one_argument(argument,arg);

  if (!*arg)
  {
    stc("Введи имя.\n\r",ch);
    return;
  }

  output = new_buf();

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;
    char *class;

    if (!d->character || d->connected != CON_PLAYING) continue;

    wch = d->character;

    if (IS_NPC(wch))
    {
      stc("Нет в мире таких\n\r",ch);
      return;
    }

    if (!can_see(ch,wch,NOCHECK_LVL)) continue;

    if (!str_prefix(arg,wch->name))
    {
      found = TRUE;
      class = classname(wch);
      switch(wch->level)
      {
        case MAX_LEVEL - 0 : class = "{DImplementor{x";     break;
        case MAX_LEVEL - 1 : class = "{D  Creator  {x";     break;
        case MAX_LEVEL - 2 : class = "{C   Deity   {x";     break;
        case MAX_LEVEL - 3 : class = "{C Elder God {x";     break;
        case MAX_LEVEL - 4 : class = "{c    God    {x";     break;
        case MAX_LEVEL - 5 : class = "{r  DemiGod  {x";     break;
        case MAX_LEVEL - 6 : class = "{r Immortal  {x";     break;
        case MAX_LEVEL - 7 : class = "{W ArchAngel {x";     break;
        case MAX_LEVEL - 8 : class = "{w   Angel   {x";     break;
      }

      do_printf(buf,"\n\r{C /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/{x\n\r{C| {x%12s %s %-54s {C|{x\n\r{C| Уровень: {Y%3d  {CРаса: {Y%s {CКласс: {Y%15s  {CПол: {Y%s         {C|{x\n\r{C| Клан:{x%s        {CКриминал:%s %s             {C|\n\r{x",
      wch->name, IS_SET(wch->comm, COMM_AFK) ? "{c[AFK]{x" : "     ",
      wch->pcdata->title, wch->level,
      race_wname(wch),
      class,wch->sex==0 ? "потерян":wch->sex == 1 ? "мужской":"женский",
      (wch->clan) ? wch->clan->show_name: "   не в клане   ",
      IS_SET(wch->act,PLR_WANTED) ?"{RПРЕСТУПНИК{x   ":"             ",
      IS_SET(wch->act,PLR_RAPER) ? "{RНАСИЛЬНИК{x":"         ");
      add_buf(output,buf);


      if( !IS_DEITY(wch) )
      {
       if( !IS_DEVOTED_ANY(wch) )
         do_printf(buf,"{C| {cБезбожн%s {Rатеист%s                                                        {C|\n\r",
           (wch->sex==2)?"ая":"ый", (wch->sex==2)?"ка":"  ");
       else
         {
           do_printf(temp,"{c%s {R%s",
            (wch->sex==2)?t_favour[favour_string(wch)].fav_nfstr:t_favour[favour_string(wch)].fav_nmstr,
            get_rdeity( deity_table[wch->pcdata->dn].russian, '2') );
            do_printf(buf,"{C| %72s  {C|\n\r",temp);
         }
       add_buf(output,buf);
      }

      if (!EMPTY(wch->pcdata->marry)) do_printf(marry,"%s%s",wch->sex==2?"{Mзамужем за {Y":"{Mженат на {Y",wch->pcdata->marry);
      else do_printf(marry,"%s",wch->sex==2?"{Dне замужем{x":"{Dхолост{x");
      do_printf(buf,"{C| ClanRank: %s{C          Семейное положение: %20s{C     |{x\n\r",get_clan_rank(wch),marry);
      add_buf(output,buf);

      if (!EMPTY(wch->pcdata->mother) || !EMPTY(wch->pcdata->father))
      {
        if (EMPTY(wch->pcdata->mother))      do_printf(buf,"{C| {CПапа  : {Y%12s                                                      {C|\n\r",wch->pcdata->father);
        else if (EMPTY(wch->pcdata->father)) do_printf(buf,"{C| {CМама  : {Y%12s                                                      {C|\n\r",wch->pcdata->mother);
        else                                 do_printf(buf,"{C| {CПапа  : {Y%12s         {CМама  : {Y%12s                         {C|\n\r",wch->pcdata->father,wch->pcdata->mother);
        add_buf(output,buf);
      }

      if (!EMPTY(wch->pcdata->kins))
      {
        do_printf(buf,"{C| %74s{C|\n\r",wch->pcdata->kins);
        add_buf(output,buf);
      }

      if (IS_IMMORTAL(ch))
      {
        do_printf(buf,"{C| {DКоличество убитых:           Количество смертей от:                       |\n\r| мобов:   {R%-7ld             {Dмобов  :{R%-7ld                              {D|\n\r| игроков: {R%-7ld             {Dигроков:{R%-7ld                              {D|\n\r{C| Побед на арене:{G%-7ld{C       Поражений на арене:{G%-7ld{C                   |\n\r{x",
        wch->vic_npc,wch->death_npc,wch->vic_pc_total,wch->death_pc_total,
        wch->vic_pc_arena, wch->death_pc_arena);
        add_buf(output,buf);
      }
      else if (wch==ch || IS_CFG(ch,CFG_SHOWKILLS))
      { 
        do_printf(buf,"{C| Побед на арене:{G%-7ld{C       Поражений на арене:{G%-7ld{C                   |\n\r{x",
        wch->vic_pc_arena, wch->death_pc_arena);
        add_buf(output,buf);
      }
    
      if (ch == wch || IS_IMMORTAL(ch))
      {
        do_printf(buf,"{C| {DУчастие в квестах : {W%-7ld  {Cвыполнено: {W%-7ld  {Dна этом уровне: {W%-7ld  {C|{x\n\r",wch->qcounter,wch->qcomplete[0],wch->qcomplete[1]);
        add_buf(output,buf);
      } 

      if (ch == wch || IS_IMMORTAL(ch))
      {
        do_printf(buf,"{C| {DУчастие в Гквестах: {W%-7ld  {Dвыполнено: {W%-7ld                           {C|{x\n\r",wch->gqcounter,wch->gqcomplete);
        add_buf(output,buf);
      } 

      if (GUILD(wch,DWARVES_GUILD))
      {
        if (ELDER(wch,DWARVES_GUILD))
          do_printf(buf,"{C| Является {GСтарейшиной {C в {GГильдии Гномов                                   {C |{x\n\r");
        else do_printf(buf,"{C| Является {Gкузнецом{C из {GГильдии Гномов                                      {C |{x\n\r");
         add_buf(output,buf);
      }
      if (GUILD(wch,DRUIDS_GUILD))
      {
        if (ELDER(wch,DRUIDS_GUILD))
          do_printf(buf,"{C| Является {GСтарейшиной {C в {GГильдии Друидов                                  {C |{x\n\r");
        else do_printf(buf,"{C| Является {Gпослушником{C из {GГильдии Друидов                                  {C |{x\n\r");
         add_buf(output,buf);
      }
      if (GUILD(wch,VAMPIRE_GUILD))
      {
        if (ELDER(wch,VAMPIRE_GUILD))
             do_printf(buf,"{C| Является {RВысшим Вампиром{C из {RГильдии Вампиров                              {C|{x\n\r");
        else do_printf(buf,"{C| Является {RВампиром {Cиз {RГильдии Вампиров                                     {C|{x\n\r");
        add_buf(output,buf);
      }
/*      if (GUILD(ch,ASSASIN_GUILD) || IS_IMMORTAL(ch))
      {
        if (GUILD(wch,ASSASIN_GUILD))
        {
          if (ELDER(wch,ASSASIN_GUILD))
               do_printf(buf,"{C| Является {DМастер Ассасином{C из {RГильдии Ассасинов                            {C|{x\n\r");
          else do_printf(buf,"{C| Является {Dнаемником {Cиз {RГильдии Ассасинов                                   {C|{x\n\r");
          add_buf(output,buf);
        }
      }
*/
      add_buf(output,"{C=---------------------------------------------------------------------------=\n\r{x");
    }
  }

  if (!found)
  {
    stc("Игpока с таким именем нет.\n\r",ch);
    return;
  }

  page_to_char(buf_string(output),ch);
  free_buf(output);
}

// Lists players currently in the game, invisible players and immortals
// are reported only for players able to see them.
void do_who (CHAR_DATA * ch, const char * argument)
{
  char buf[MAX_STRING_LENGTH], clans[MAX_STRING_LENGTH] ;
  DESCRIPTOR_DATA * d ;
  BUFFER          * output ;
  CLAN_DATA       * clan ;
  int maxlevel, minlevel, argnum, i;
  bool showclass [MAX_CLASS];

  #define SHOW_IMMONLY  0x001
  #define SHOW_CRIMINAL 0x002
  #define SHOW_CLAN     0x004
  #define SHOW_MALE     0x008
  #define SHOW_IT       0x010
  #define SHOW_FEMALE   0x020
  #define SHOW_PK       0x040
  #define SHOW_MLT      0x080
  #define SHOW_ASSASIN  0x100
  #define SHOW_CLASSES  0x200
  #define SHOW_RACES    0x400

  short int showflag = 0 ;

  minlevel = 0 ;
  maxlevel = MAX_LEVEL ;

  for (i = 0 ; i < MAX_CLASS ; i++) showclass [i] = FALSE ;
  for (i = 0 ; race_table[i].name!=NULL; i++) race_table[i].temp= FALSE;

  memset (clans, 0, MAX_STRING_LENGTH) ;

  // parse who arguments
  for (argnum = 0 ;;)
  {
    char arg [MAX_STRING_LENGTH] ;
    argument = one_argument (argument, arg) ;

    // no argument specified
    if (EMPTY (arg)) break ;

    // level range <min> <max>
    if (is_number (arg))
    {
      switch (++argnum)
      {
        case 1: minlevel = atoi (arg) ; break ;
        case 2: maxlevel = atoi (arg) ; break ;
        default: stc ("Разрешено только два значения.\n\r", ch) ; return ;
      }
    }
    else
    {
      // look for classes to turn on
      if (GUILD(ch, ASSASIN_GUILD) &&
         (!str_prefix (arg, "assasins"))) showflag |= SHOW_ASSASIN  ; else 
      if (!str_prefix (arg, "immortals")) showflag |= SHOW_IMMONLY  ; else
      if (!str_prefix (arg, "male"))      showflag |= SHOW_MALE     ; else
      if (!str_prefix (arg, "it"))        showflag |= SHOW_IT       ; else
      if (!str_prefix (arg, "female"))    showflag |= SHOW_FEMALE   ; else
      if (!str_prefix (arg, "pk"))        showflag |= SHOW_PK       ; else
      if (!str_prefix (arg, "mlt"))       showflag |= SHOW_MLT      ; else 
      if (!str_prefix (arg, "criminal"))  showflag |= SHOW_CRIMINAL ; else
      if (!str_prefix (arg, "clan"))      showflag |= SHOW_CLAN     ; else
      if ((i = class_lookup (arg)) >= 0)
      {
        showflag     |= SHOW_CLASSES ;
        showclass [i] = TRUE ;
      }
      else
      if ((i = race_lookup (arg)) > 0)
      {
        showflag    |= SHOW_RACES ;
        race_table[i].temp=TRUE ;
      }
      else
      if ((clan = clan_lookup (arg)) != NULL)
      {
        if (EMPTY (clans)) strcpy (clans, clan->name) ;
        else               strcat (clans, clan->name) ;

        strcat (clans, " ") ;
      }
      else
      {
        stc ("Это не pаса, не класс и не клан.\n\r", ch) ;
        return ;
      }
    }
  }

  // now show matching players
  buf[0]  = 0 ;
  output  = new_buf () ;

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

    // check level here, so invisible players are undetectable!
    if (!can_see(ch, wch, CHECK_LVL)) continue ;
    if ((wch->level < minlevel      || wch->level > maxlevel)         ||
        ((showflag & SHOW_IMMONLY)  && 
        (wch->level<LEVEL_IMMORTAL  || get_trust(wch)<LEVEL_IMMORTAL))||
        ((showflag & SHOW_CLAN)     && wch->clan == NULL)             ||
        ((showflag & SHOW_IT)       && wch->sex != 0)                 ||
        ((showflag & SHOW_MALE)     && wch->sex != 1)                 ||
        ((showflag & SHOW_FEMALE)   && wch->sex != 2)                 ||
        ((showflag & SHOW_MLT)      && wch->remort < 1)               ||
        ((showflag & SHOW_CRIMINAL) && !IS_SET(wch->act, PLR_WANTED)) ||
        ((showflag & SHOW_ASSASIN)  && !GUILD(wch, ASSASIN_GUILD))    ||
        ((showflag & SHOW_RACES)    && !race_table[wch->race].temp)   ||
        ((showflag & SHOW_CLASSES)  && !showclass[wch->class[wch->remort]])  ||
        ((showflag & SHOW_PK) && (!PK_RANGE(ch, wch) || !PK_RANGE(wch, ch))) ||
        (!EMPTY (clans) && (wch->clan == NULL || !is_name (wch->clan->name,
                                                           clans)))) continue ;

    // figure out what to print for class
    switch (wch->level)
    {
      default:            class = classname(wch) ; break ;
      case MAX_LEVEL - 0: class = "{D ТВОРЕЦ {x" ; break ;
      case MAX_LEVEL - 1: class = "{RCREATOR {x" ; break ;
      case MAX_LEVEL - 2: class = "{MSUPERIOR{x" ; break ;
      case MAX_LEVEL - 3: class = "{GGOD     {x" ; break ;
      case MAX_LEVEL - 4: class = "{GDemiGod {x" ; break ;
      case MAX_LEVEL - 5: class = "{cImmortal{x" ; break ;
      case MAX_LEVEL - 6: class = "{CArhAngel{x" ; break ;
      case MAX_LEVEL - 7: class = "{CAngel   {x" ; break ;
      case MAX_LEVEL - 8: class = "{CAvatar  {x" ; break ;
    }

    do_printf (buf, "[%3d %s %10s{x] %s%s%s%s",
               wch->level, race_wname (wch), class,
               wch->clan ? wch->clan->show_name : "                ",
               wch->incog_level >= LEVEL_HERO ? "{w(I){x" : "",
               wch->invis_level >= LEVEL_HERO ? "{w(W){x" : "",
               IS_SET(wch->comm, COMM_AFK)    ? "{c[A]{x" : "");

    add_buf (output, buf) ;

    do_printf (buf, "%s%s%s %s %s{x\n\r",
               IS_SET(wch->act, PLR_WANTED) ? "{r(W){x" : "",
               IS_SET(wch->act, PLR_RAPER)  ? "{R(Н){x" : "",
               wch->godcurse                ? "{D(C){x" : "",
               wch->name, IS_NPC(wch) ? "" : wch->pcdata->title) ;

    add_buf (output, buf) ;
  }
  page_to_char (buf_string (output), ch) ;
  free_buf (output) ;
   do_function(ch, &do_count, "");
}

// Count number of active players in the game, do not include immortals
// into invisible count so players cant see if they're online
void do_count (CHAR_DATA * ch, const char * argument)
{
  register int count     = 0 ;
  register int count_vis = 0 ;
  DESCRIPTOR_DATA * d ;

  for (d = descriptor_list ; d != NULL ; d = d->next)
  {
    if (!d->character || d->connected != CON_PLAYING) continue;

    if (can_see (ch, d->character, CHECK_LVL))
    {
      count++ ;
      count_vis++ ;
    }
    else /*if (IS_IMMORTAL(d->character))*/ count++ ;
  }

  max_on = UMAX (count, max_on) ;

  if (max_on == count)
    ptc (ch, "\n\rВсего игроков %d, видимых %d, максимум на сегодня.\n\r",
         count, count_vis) ;
  else
    ptc (ch, "\n\rВсего игроков %d, видимых %d, максимум на сегодня было %d.\n\r",
         count, count_vis, max_on) ;
}

void do_inventory(CHAR_DATA *ch, const char *argument)
{
  stc("Ты несешь:\n\r", ch);
  show_list_to_char(ch->carrying, ch, TRUE, TRUE);
}

void do_equipment(CHAR_DATA *ch, const char *argument)
{
  OBJ_DATA *obj;
  int iWear;
/*
  stc("{RDISABLED BY GODS!\n\r{x",ch);
  return;
*/
  stc("Ты используешь:\n\r", ch);
  stc("--------------------------------------------------------------------------------\n\r", ch);
  for (iWear = 0; iWear < MAX_WEAR_L-1; iWear++)
  {
   if( ( obj = get_eq_char(ch, wear_l[iWear].wear_num)) == NULL )
   {
    if (!IS_NPC(ch))
      stc((ch->sex==2) ? where_name_female[ch->class[UMIN(ch->remort,3)]].picture[iWear]
        : where_name_male[ch->class[UMIN(ch->remort,3)]].picture[iWear], ch);
    stc( wear_l[iWear].name, ch);
    stc("\n\r", ch);
    continue;
   }
   if (!IS_NPC(ch))
     stc((ch->sex==2) ? where_name_female[ch->class[UMIN(ch->remort,3)]].picture[iWear]
       : where_name_male[ch->class[UMIN(ch->remort,3)]].picture[iWear], ch);
   stc( wear_l[iWear].name, ch);
   ptc(ch, "%s\n\r", format_obj_to_char(obj, ch, TRUE));
  }
  stc("--------------------------------------------------------------------------------\n\r", ch);
}

void do_compare(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], *msg;
  OBJ_DATA *obj1, *obj2;
  int64 value1, value2;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  if (!*arg1)
  {
   stc("Сpавнить что с чем?\n\r", ch);
   return;
  }

  if ((obj1 = get_obj_carry(ch, arg1, ch)) == NULL)
  {
   stc("У тебя этого нет.\n\r", ch);
   return;
  }

  if (!*arg2)
  {
    for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
    {
      if (obj2->wear_loc != WEAR_NONE && can_see_obj(ch,obj2)
        && obj1->item_type == obj2->item_type
        && (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
            break;
    }
    if (!obj2)
    {
     stc("Hа тебе нет ничего с чем это можно было бы сpавнить.\n\r",ch);
     return;
    }
  }

  else if ((obj2 = get_obj_carry(ch,arg2,ch)) == NULL)
  {
    stc("У тебя этого нет.\n\r",ch);
    return;
  }

  msg    = NULL;
  value1 = 0;
  value2 = 0;

  if (obj1 == obj2)
  {
    msg = "Ты сpавниваешь $i1 с $i5.  Стpанно... они одинаковые...";
  }
  else if (obj1->item_type != obj2->item_type)
  {
    msg = "Ты не можешь сpавнить $i1 и $I1.";
  }
  else
  {
    switch (obj1->item_type)
    {
      default:
          msg = "Ты не можешь сpавнить $i1 и $I1.";
          break;

      case ITEM_ARMOR:
          value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
          value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
          break;

      case ITEM_WEAPON:
          if (obj1->pIndexData->new_format)
              value1 = (1 + obj1->value[2]) * obj1->value[1];
          else
              value1 = obj1->value[1] + obj1->value[2];

          if (obj2->pIndexData->new_format)
              value2 = (1 + obj2->value[2]) * obj2->value[1];
          else
              value2 = obj2->value[1] + obj2->value[2];
          break;
    }
  }

  if (!msg)
  {
    if (value1 == value2)      msg = "$i1 и $I1 выглядят похожими.";
    else if (value1  > value2) msg = "$i1 выглядит лучше, чем $I1.";
    else                         msg = "$i1 выглядит хуже, чем $I1.";
  }
  act(msg, ch, obj1, obj2, TO_CHAR);
}

void do_credits(CHAR_DATA *ch, const char *argument)
{
  do_function(ch, &do_help, "diku");
}

void do_where(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  bool found;

  one_argument(argument, arg);

  if (ch->in_room && !IS_AFFECTED(ch, AFF_BLIND))
   ptc (ch,"{YТы находишься в арии '%s'{x.\n\r",ch->in_room->area->name);

  if (EMPTY(arg))
  {
   stc("Игpоки pядом с тобой:\n\r", ch);
   found = FALSE;
   for (d = descriptor_list; d; d = d->next)
   {
    if (d->connected == CON_PLAYING
    && (victim = d->character) != NULL
    &&   !IS_NPC(victim)
    &&   victim->in_room != NULL
    &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
    &&   !IS_AFFECTED(victim,AFF_HIDE)
    &&   (is_room_owner(ch,victim->in_room)
    ||    !room_is_private(victim->in_room))
    &&   victim->in_room->area == ch->in_room->area
    &&   can_see(ch, victim,CHECK_LVL))
    {
     found = TRUE;
     ptc(ch, "%28s %s\n\r", victim->name,
      IS_SET(victim->in_room->ra,RAFF_NOWHERE) ? "где-то.":victim->in_room->name);
    }
   }
   if (!found) stc("Hет.\n\r", ch);
   return;
  }

  found = FALSE;
  for (victim = char_list; victim != NULL; victim = victim->next)
  {
   if (victim->in_room != NULL
   &&   victim->in_room->area == ch->in_room->area
   &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE) 
   &&   !IS_AFFECTED(victim,AFF_HIDE)  
   &&   can_see(ch, victim,CHECK_LVL)
   &&   is_name(arg, victim->name))
   {
//     statui nevidimi po where
     if (IS_STATUE(victim)) continue;

     found = TRUE;
     ptc(ch, "%-28s %s\n\r", PERS(victim, ch),
       IS_SET(victim->in_room->ra,RAFF_NOWHERE) ? "в пустоте.":victim->in_room->name);
   }
  }
  if (!found) act("Ты не находишь $T.", ch, NULL, arg, TO_CHAR);
}

void do_consider(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim;
  char *msg;
  int diff;

  if (EMPTY(argument))
  {
    stc("Оценить убийство кого?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, argument)) == NULL)
  {
    stc("Тут такого нет.\n\r", ch);
    return;
  }

  if (is_safe(ch,victim))
  {
    stc("Даже не думай об этом.\n\r",ch);
    return;
  }

  diff = victim->level - ch->level;
       if (diff <= -10) msg = "Ты можешь убить $C4 одним пинком.";
  else if (diff <= -5) msg = "$N не сопеpник тебе.";
  else if (diff <= -2) msg = "Похоже ты легко убьешь $C4.";
  else if (diff <=  1) msg = "Пpекpасный поединок!";
  else if (diff <=  4) msg = "$N говоpит 'Чувствуешь удачу, сопляк?'.";
  else if (diff <=  9) msg = "$N смеется над твоей беспомощностью.";
  else                    msg = "Твоя смеpть будет быстpой, но болезненной.";

  act(msg, ch, NULL, victim, TO_CHAR);
}

void set_title(CHAR_DATA *ch, const char *title)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  if (title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?')
  {
    buf[0] = ' ';
    strcpy(buf+1, title);
  }
  else strcpy(buf, title);
  smash_flash(buf);
  smash_beep(buf,1);
  smash_newline(buf);
  
  free_string(ch->pcdata->title);
  ch->pcdata->title = str_dup(buf);
}

void do_title(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int len;


  strcpy(buf, argument);

  if (!*buf)
  {
    stc("Синтаксис: title <новый титл|default>\n\r", ch);
    return;
  }

  len = strlen(buf);
  if ((buf[len-1]=='{' || buf[len-1]=='`')
   && buf[len-2] !='{' && buf[len-2] != '`') buf[len-1] = '\0'; 

  if(len>60)
  {
    stc("Слишком длинный титл. Выбери другой.\n\r",ch);
    return;
  }
  if (!str_cmp(buf,"default"))
   do_printf(buf, " %s",title_table [ch->class[ch->remort]] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0]);
  strcat(buf,"{x");

  set_title(ch, buf);
  stc("Ok.\n\r", ch);
}
  
void do_description(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (argument)
  {
    buf[0] = '\0';

    if (argument[0] == '-')
    {
      int len;
      bool found = FALSE;

      if (!ch->description || ch->description[0] == '\0')
      {
        stc("Описание отсутствует.\n\r",ch);
        return;
      }

      strcpy(buf,ch->description);

      for (len = strlen(buf); len > 0; len--)
      {
        if (buf[len] == '\r')
        {
          if (!found)  /* back it up */
          {
            if (len > 0)len--;
            found = TRUE;
          }
          else /* found the second one */
          {
            buf[len + 1] = '\0';
            free_string(ch->description);
            ch->description = str_dup(buf);
            stc("Твое описание :\n\r", ch);
            stc(ch->description ? ch->description : "(Hет).\n\r", ch);
            return;
          }
        }
      }
      buf[0] = '\0';
      free_string(ch->description);
      ch->description = str_dup(buf);
      stc("Описание очищено.\n\r",ch);
      return;
    }

    if (argument[0] == '+')
    {
      int64 position;
      if (ch->description) strcat(buf, ch->description);
      argument++;
      while (isspace(*argument)) argument++;

      if (strlen(buf) >= 1536) // 1.5 kb 
      {
        stc("Описание слишком длинное.\n\r", ch);
        return;
      }
      strcat(buf, argument);
      strcat(buf, "\n\r");
      for (position=0; position < strlen(buf); position++)
      {
         if (buf[position] == '~')
         {
            buf[position] = '-';
         }
      }
      free_string(ch->description);
      ch->description = str_dup(buf);
    }
  }
  ptc(ch,"Твое описание :\n\r%s", ch->description ? ch->description : "(Отсутствует).\n\r", ch);
}

void do_report(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_INPUT_LENGTH];

  do_printf(buf, "'У меня %d/%d здоровья %d/%d маны %d/%d дв. %d опыта.'",
    ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
    ch->move, ch->max_move,ch->exp);
  act("Ты произносишь $t", ch, buf, NULL, TO_CHAR);
  act("{Y$n{x произносит $t", ch, buf, NULL, TO_ROOM);
}

void do_practice(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *mob;
  int sn=0, rll=100, level, minx=0, maxx=110, adept;
  char arg[MAX_INPUT_LENGTH];

  if (EMPTY(argument) || is_number(argument))
  {
    for(;;)
    {
      argument=one_argument(argument,arg);
      if (EMPTY(arg)) break;
      if (arg[0]=='-') minx=atoi(arg+1);
      else maxx=atoi(arg);
    }

    for (level=0;level<=ch->level;level++)
    {
      int col=0;
      char clr='C';
      bool found=FALSE;

      for (sn = 0; sn < max_skill; sn++)
      {
        int sn_level=min_level(ch,sn);

        if (skill_table[sn].name == NULL) break;
        if (level!=sn_level || ch->level<sn_level 
         || ch->pcdata->learned[sn] < 1) continue;
        if (ch->pcdata->learned[sn]>maxx
         || ch->pcdata->learned[sn]<minx) continue;
        if (!check_clanspell(sn,ch->clan)) continue;

        if (!found)
        {
          ptc(ch,"{C%3d:",level);
          found=TRUE;
        }
        else stc("    ",ch);
        if (ch->pcdata->learned[sn] <=   1) clr='R';
        if (ch->pcdata->learned[sn] <=  30) clr='w';
        if (ch->pcdata->learned[sn] <=  50) clr='y';
        else if (ch->pcdata->learned[sn] <= class_table[ch->class[ch->remort]].skill_adept) clr='g';
        else if (ch->pcdata->learned[sn] <=  70) clr='G';
        else if (ch->pcdata->learned[sn] <=  80) clr='b';
        else if (ch->pcdata->learned[sn] <=  90) clr='B';
        else if (ch->pcdata->learned[sn] <=  99) clr='W';
        else if (ch->pcdata->learned[sn] == 100) clr='C';
          
        ptc(ch, " {Y%15s{%c %3d%%",
          skill_table[sn].name, clr,ch->pcdata->learned[sn]);
        if (++col % 3 == 0) stc("\n\r", ch);
      }
      if (col % 3 != 0) stc("{x\n\r", ch);
    }
    ptc(ch, "{xУ тебя осталось {G%d{x пpактик.\n\r",ch->practice);
    return;
  }

  if (!str_prefix(argument,"help"))
  {
    stc("Синтаксис:\n\r",ch);
    stc("  prac [x] - список навыков отпрактикованых до Х%\n\r",ch);
    stc("  prac [-x] - список навыков отпрактикованых более Х%\n\r",ch);
    stc("  prac <skill>   - практиковать навык\n\r",ch);
    return;
  }

  // Now we can practice skill or spell
  if (!IS_AWAKE(ch))
  {
   stc("Пpоснись для начала.\n\r", ch);
   return;
  }

  for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
   if (IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE)) break;

  if (!mob)
  {
    stc("Hе здесь.\n\r", ch);
    return;
  }

  if (ch->practice <= 0)
  {
    stc("У тебя не осталось пpактик.\n\r", ch);
    return;
  }

  if (!IS_NPC(ch))
  {
    rll = 100;

    if ((ch->classmag) && (rll > skill_table[sn].rating[0]) && (skill_table[sn].rating[0] != 0))
                        rll = skill_table[sn].rating[0];
    if ((ch->classcle) && (rll > skill_table[sn].rating[1]) && (skill_table[sn].rating[1] != 0))
                        rll = skill_table[sn].rating[1];
    if ((ch->classthi) && (rll > skill_table[sn].rating[2]) && (skill_table[sn].rating[2] != 0))
                        rll = skill_table[sn].rating[2];
    if ((ch->classwar) && (rll > skill_table[sn].rating[3]) && (skill_table[sn].rating[3] != 0))
                        rll = skill_table[sn].rating[3];
  }

  sn=find_spell(ch,argument);

  if (sn < 0 || (!IS_NPC(ch) && (ch->level < min_level(ch,sn) 
             || ch->pcdata->learned[sn] < 1 || rll == 0)))
  {
      stc("Ты не можешь пpактиковаться этому.\n\r", ch);
      return;
  }

  if (!check_clanspell(sn,ch->clan))
  {
    stc("Твоему клану это сейчас не доступно.\n\r", ch);
    return;
  }


  adept = IS_NPC(ch) ? 100 : class_table[ch->class[ch->remort]].skill_adept;
  if (ch->pcdata->learned[sn] >= adept)
    ptc(ch, "Ты уже достаточно знаешь %s.\n\r",skill_table[sn].name);
  else
  {
    ch->practice--;
    rll = 100;

    if ((ch->classmag) && (rll > skill_table[sn].rating[0]) && (skill_table[sn].rating[0] != 0))
                        rll = skill_table[sn].rating[0];
    if ((ch->classcle) && (rll > skill_table[sn].rating[1]) && (skill_table[sn].rating[1] != 0))
                        rll = skill_table[sn].rating[1];
    if ((ch->classthi) && (rll > skill_table[sn].rating[2]) && (skill_table[sn].rating[2] != 0))
                        rll = skill_table[sn].rating[2];
    if ((ch->classwar) && (rll > skill_table[sn].rating[3]) && (skill_table[sn].rating[3] != 0))
                        rll = skill_table[sn].rating[3];
    if (rll == 100 || rll == 0) rll = 1;
    ch->pcdata->learned[sn] += (int_app[get_curr_stat(ch,STAT_INT)].learn*3) / (rll*4);
    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn],adept);

    act("Ты пpактикуешься в {w$T{x.",ch, NULL, skill_table[sn].name, TO_CHAR);
    act("{y$n{x пpактикуется в {w$T{x.",ch, NULL, skill_table[sn].name, TO_ROOM);
    ptc(ch,"Теперь ты знаешь %s на %d%%\n\r",skill_table[sn].name,ch->pcdata->learned[sn]);
  }
}

void do_wimpy(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int wimpy;

  one_argument(argument, arg);
  if (!*arg) wimpy = ch->max_hit / 5;
  else wimpy = atoi(arg);

  if (wimpy < 0)
  {
    stc("Твоя отвага пpевосходит твою мудpость.\n\r", ch);
    return;
  }

  if (wimpy > ch->max_hit/2)
  {
    stc("Hу и тpус же ты, батенька.\n\r", ch);
    return;
  }
  ch->wimpy = wimpy;
  ptc(ch, "Ты будешь убегать пpи %d здоpовья.\n\r", wimpy);
  WAIT_STATE(ch,PULSE_VIOLENCE);
}

void do_password(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char *pArg, *pwdnew, *p;
  char cEnd;

  // Can't use one_argument here because it smashes case.
  // So we just steal all its code. Bleagh.
  pArg = arg1;
  while (isspace(*argument)) argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"') cEnd = *argument++;

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\0';

  pArg = arg2;
  while (isspace(*argument)) argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"') cEnd = *argument++;

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\0';

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    stc("Cинтаксис: password <старый пароль> <новый пароль>.\n\r", ch);
    return;
  }

  /* if (strcmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd)) */
  if (strcmp(arg1 , ch->pcdata->pwd))
  {
    stc("Hевеpный паpоль. Подожди 10 секунд.\n\r", ch);
    WAIT_STATE(ch, 40);
    return;
  }

  if (strlen(arg2) < 5)
  {
    stc("Паpоль должен быть не менее 5 символов.\n\r", ch);
    return;
  }

  /* No tilde allowed because of player file format. */
  pwdnew = arg2;
  /* pwdnew = crypt(arg2, ch->name); */
  for (p = pwdnew; *p != '\0'; p++)
  {
    if (*p == '~')
    {
      stc("Hевеpный символ, повтоpи.\n\r", ch);
      return;
    }
  }

  free_string(ch->pcdata->pwd);
  ch->pcdata->pwd = str_dup(pwdnew);
  save_char_obj(ch);
  stc("Ok.\n\r", ch);
}

void do_skillstat(CHAR_DATA *ch, const char *argument)
{
  BUFFER *buffer;
  char skill_list[MAX_LEVEL][MAX_STRING_LENGTH], skill_columns[MAX_LEVEL];
  int sn, level, min_lev = 1, max_lev = MAX_LEVEL;
  bool found = FALSE;
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  if (!*argument)
  {
    stc("{RCинтаксис:{x skillstat <имя игрока>\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, argument)) == NULL)
  {
    stc("Здесь нет таких.\n\r", ch);
    return;
  }

  for (level = 0; level < MAX_LEVEL; level++)
  {
    skill_columns[level] = 0;
    skill_list[level][0] = '\0';
  }

  for (sn = 0; sn < max_skill; sn++)
  {
   if (skill_table[sn].name == NULL) break;

   if ((level = min_level(victim,sn)) <LEVEL_HERO + 1
   && level >= min_lev && level <= max_lev
   && (skill_table[sn].spell_fun == spell_null)
   && victim->pcdata->learned[sn] > 0
   && check_clanspell(sn,victim->clan))
   {
     found = TRUE;
     level = min_level(victim,sn);
     if (victim->level < level)
    do_printf(buf,"%-18s n/a      ", skill_table[sn].name);
     else do_printf(buf,"%-18s %3d%%      ",skill_table[sn].name,
    victim->pcdata->learned[sn]);

     if (skill_list[level][0] == '\0')
    do_printf(skill_list[level],"\n\r{CУpовень {G%3d{x: %s",level,buf);
     else /* append */
     {
      if (++skill_columns[level] % 2 == 0)
    strcat(skill_list[level],"\n\r             ");
      strcat(skill_list[level],buf);
     }
   }
  }

  if (!found)
  {
    stc("{RУмений не найдено.{x\n\r",ch);
    return;
  }

  buffer = new_buf();
  for (level = 0; level < MAX_LEVEL; level++)
    if (skill_list[level][0] != '\0') add_buf(buffer,skill_list[level]);

  add_buf(buffer,"\n\r");
  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

void do_spellstat(CHAR_DATA *ch, const char *argument)
{
  BUFFER *buffer;
  char spell_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
  char spell_columns[LEVEL_HERO + 1];
  int sn, gn, col, level, min_lev = 1, max_lev = LEVEL_HERO;
  bool found = FALSE;
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  if (!*argument)
  {
    stc("Чьи заклинания пеpечислить?\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, argument)) == NULL)
  {
    stc("Нет здесь таких.\n\r", ch);
    return;
  }

  col = 0;

  for (gn = 0; gn < MAX_GROUP; gn++)
  {
    if (group_table[gn].name == NULL) break;
    if (victim->pcdata->group_known[gn])
    {
      ptc(ch,"%-20s ",group_table[gn].name);
      if (++col % 3 == 0) stc("\n\r",ch);
    }
  }
  if (col % 3 != 0) ptc(ch,"\n\r{GCreation points:{x%d\n\r",victim->pcdata->points);

  // initialize data
  for (level = 0; level < LEVEL_HERO + 1; level++)
  {
    spell_columns[level] = 0;
    spell_list[level][0] = '\0';
  }
  stc("{GLevel      {YName of Spell      {y % {x/{CMan {YName of Spell      {y % {x/{CMan{x",ch);
  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name == NULL) break;

    if ((level = min_level(victim,sn)) <LEVEL_HERO + 1
      && level >= min_lev && level <= max_lev
      && skill_table[sn].spell_fun != spell_null
      && victim->pcdata->learned[sn] > 0
      && check_clanspell(sn,victim->clan))
    {
      found = TRUE;
      level = min_level(victim,sn);

      if (victim->level < level)
        do_printf(buf,"{Y%-18s {Rn/a{x     ",skill_table[sn].name);
      else
      {
        do_printf(buf,"{Y%-18s {y%3d{x/{C%3d{x ",skill_table[sn].name,victim->pcdata->learned[sn],mana_cost(victim,sn));
      }

      if (spell_list[level][0] == '\0')
        do_printf(spell_list[level],"\n\r{GLevel {x%3d: %s",level,buf);
      else /* append */
      {
        if (++spell_columns[level] % 2 == 0)
        strcat(spell_list[level],"\n\r           ");
        strcat(spell_list[level],buf);
      }
    }
  }

  // return results
  if (!found)
  {
    stc("Заклинаний не найдено.\n\r",ch);
    return;
  }
  buffer = new_buf();
  for (level = 0; level < LEVEL_HERO + 1; level++)
      if (spell_list[level][0] != '\0') add_buf(buffer,spell_list[level]);
  add_buf(buffer,"\n\r");
  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

void do_qstat(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  MOB_INDEX_DATA *questman;
  one_argument(argument, arg);

  if (!*arg)
  {
    stc("Укажите имя\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch,argument)) == NULL)
  {
    stc("Нет здесь таких.\n\r", ch);
    return;
  }

  stc("\n\r{G/--------------------------{CCHAR QuestStatistic{G------------------------------/\n\r", ch);
  ptc(ch, "{G| {YVnum : {x%u    ",
   IS_NPC(victim) ? victim->pIndexData->vnum : 0);
  ptc(ch, "{YAge  : {x%d   {YName:{x%s %s\n\r{G| {YLevel: {x%3d  {YClass: {x%s   {YRace:{x%s     {YSex  : {x%s   {YSec   :{x%d\n\r",
   IS_NPC(victim) ? get_age(victim):0, victim->name,
   IS_NPC(victim) ? "" : victim->pcdata->title, victim->level,
   IS_NPC(victim) ? "mobile" :class_table[victim->class[victim->remort]].name,
   race_table[victim->race].name,
   ch->sex == 0 ? "бесполый" : ch->sex == 1 ? "мужской" : "женский",
   IS_NPC(victim) ? 0:victim->pcdata->security);
  ptc(ch, "{G| {YFormat:{x%s  {YGroup: {x%d      ",
   IS_NPC(victim) ? "npc" : "pc.", IS_NPC(victim) ? victim->group : 0);
  ptc(ch,"{YRoom:{x%u     ",(victim->in_room) ? 0 : victim->in_room->vnum);
  ptc(ch,"{YCount: {x%d        {YKilled:{x%d\n\r", 
   IS_NPC(victim) ? victim->pIndexData->count:0,
   IS_NPC(victim) ? victim->pIndexData->killed:0);
  stc("{G=---------------------------------------------------------------------------=\n\r",ch);
  ptc(ch,"{G| {YQuestpoint: {x%d\n\r",victim->questpoints);
  ptc(ch,"{G| {YTimeToQuest :{x%d       {G| {YTimeForQuest : {x%d\n\r",victim->nextquest,victim->countdown);

  if (victim->questobj!=0)
   ptc(ch,"{G| {YQuestObjRoom: {Gxx      | {YQuestObjVnum: {x%u\n\r",victim->questobj);
  else
  {
    ptc(ch,"{G| {YQuestMobRoom:{x%7u  {G\n\r",(victim->questmob==NULL)? 0 :
      (victim->questmob->in_room==NULL) ? 0 : victim->questmob->in_room->vnum);
    ptc(ch,"| {YName:{x%s\n\r",(victim->questmob==NULL)?"none":IS_NPC(victim) ? victim->questmob->name:get_char_desc(victim->questmob,'1'));
  }

  if(victim->questgiver!=0)
  {
    questman=get_mob_index(victim->questgiver);
    ptc(ch,"{G| {YQuestGiver  : {x%s (%u)\n\r",get_mobindex_desc(questman,'1'),victim->questgiver);
  }
  else ptc(ch,"{G| {YQuestGiver : {D-none-{x\n\r");
  ptc(ch, "{G| {YMaster: {x%s       {G| {YLeader  : {x%s    {G| {YPet  : {x%s\n\r",
    victim->master      ? victim->master->name   : "(none)",
    victim->leader      ? victim->leader->name   : "(none)  ",
    victim->pet         ? victim->pet->name      : "(none)");
  stc("{G=---------------------------------------------------------------------------={x\n\r",ch);
}

void do_bounty (CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int64 amount;

  argument=one_argument(argument, arg);

  if (!*arg)
  {
    DESCRIPTOR_DATA *d;

    stc("Текущий список вознаграждений:\n\r", ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
     if (d->character && d->connected == CON_PLAYING
      && d->character->pcdata->bounty>0 && can_see(ch,d->character,CHECK_LVL))
      ptc (ch,"{Y%u {xзолотых монет за голову {C%s{x\n\r",d->character->pcdata->bounty,d->character->name);
    }
    return;
  }

  if (!is_number(argument))
  {
    stc("{RСинтаксис:{x bounty <имя> <золото>\n\r",ch);
    return;
  }

  amount=atoi(argument);
  if (amount<=0 || amount>ch->gold)
  {
    stc("У вас нет столько золота.\n\r",ch);
    return;
  }

  victim=get_pchar_world(ch, arg);
  if (victim==NULL || (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)))
  {
     stc("Нету здесь таких\n\r",ch);
     return;
  }
  victim->pcdata->bounty+=amount;
  ch->gold-=amount;
  do_printf(buf,"{CНазначена награда в {Y%u {Cзолота за голову {Y%s{C.",victim->pcdata->bounty,victim->name);
  info (NULL,1,4,buf,"{x");
}

void do_reform(CHAR_DATA *ch, const char *argument)
{
  ROOM_INDEX_DATA *cur_room;

  if(!ch->morph_obj)
  {
    stc("Ты и так в своем облике.\n\r",ch);
    return;
  }
 
  if (ch->morph_obj->in_room!=NULL) cur_room=ch->morph_obj->in_room;
  else 
  {
    if (ch->morph_obj->carried_by!=NULL) cur_room=ch->morph_obj->carried_by->in_room;
    else 
    {
      stc("Похоже, у тебя проблемы...Ты у кого-то в мешке.\n\r",ch);
      return;
    }
  }

  if (IS_SET(cur_room->area->area_flags,AREA_NOREFORM) || IS_SET(cur_room->room_flags, ROOM_NOMORPH))
  {
    stc("{RБоги не позволяют тебе принять свой настоящий облик здесь.{x\n\r",ch);
    return;
  }
  if (is_affected(ch,skill_lookup("mummy")))
  {
    stc("В этом облике ты не по своей воле. Ты не можешь вернуться.{x\n\r",ch);
    return;
  }
  char_from_room(ch);
  char_to_room(ch, cur_room);
  act("$i1 исчезает в облаке тумана.{/$n появляется из ниоткуда.\n\r",ch,ch->morph_obj,NULL,TO_ROOM);
  stc("Ты принимаешь свой облик.\n\r",ch);
  ch->morph_obj->morph_name = NULL;
  ch->was_in_room = NULL;
  extract_obj(ch->morph_obj);
  ch->morph_obj = NULL;
}

void do_polymorph(CHAR_DATA *ch, const char *argument)
{
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int skill;

  if ((skill = get_skill(ch,skill_lookup("morph"))) < 1)
  {
    stc("Ты не умеешь принимать другую форму.\n\r",ch);
    return;
  }
   
  if (IS_SET(ch->in_room->room_flags, ROOM_NOMORPH))
  {
    stc("Ты не можешь принять другую форму здесь.\n\r", ch);
    return;
  }

  if (!*argument)
  {
    stc("Принять облик чего?\n\r",ch);
    return;
  }

  argument = one_argument(argument, arg);

  if (ch->fighting)
  {
    stc("Ты не можешь менять форму во время битвы.\n\r",ch);
    return;
  }

  if ((obj = get_obj_carry(ch, arg, ch))==NULL || !can_see_obj(ch,obj))
  {
    stc("У тебя этого нет.\n\r",ch);
    return;
  }  
    
  if (ch->level + 2*category_bonus(ch,MAKE|MIND) < obj->level)
  {
    ptc(ch,"У тебя ничего не выходит, {c%s{x имеет слишком сложную форму.\n\r",get_obj_desc(obj,'1'));
    return;
  }       

  if (obj->morph_name)
  {
    ptc(ch,"У тебя ничего не выходит, {c%s{x имеет слишком сложную форму.\n\r",get_obj_desc(obj,'1'));
    return;
  }

  if (number_percent() < get_skill(ch,skill_lookup("morph")))
  {
    check_improve(ch,skill_lookup("morph"),TRUE,3);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    obj->is_morphed=TRUE;
    obj->morph_name=ch;
    ch->morph_obj = obj;
    act("Очертания {y$c2{x расплываются, принимая форму {c$i2{x",ch,obj,NULL,TO_ROOM);
    ch->was_in_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
    ptc(ch,"Ты принимаешь форму {c%s{x.\n\r",get_obj_desc(obj,'2'));
  }
  else
  {
    do_printf(arg,"У тебя не получается принять форму {c%s{x.\n\r",get_obj_desc(obj,'2'));
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    extract_obj(obj);
    stc(arg,ch);
    check_improve(ch,skill_lookup("morph"),FALSE,3);
  }
}

void do_seen(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *keeper;
  bool found=FALSE;

  for (keeper=ch->in_room->people; keeper; keeper=keeper->next_in_room)
   if (IS_NPC(keeper) && IS_SET(keeper->act,ACT_IS_KEEPER))
   {
     found=TRUE;
     break;
   }
     
  if(!found)
  {
    stc("Тут нет {wХранителя Архивов{x...\n\r",ch);
    return;
  }
  do_printf(buf,"%s info",argument);
  do_offline(ch,buf); 
}

void do_config(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int i;
  ROOM_INDEX_DATA *room;


  if (EMPTY(argument))
  {
    stc("{CВаша конфигурация:\n\r",ch);
    for (i=0;i<MAX_CFG;i++)
    {
      if (cfg_table[i].change==FALSE && IS_CFG(ch,cfg_table[i].bit)) continue;
      ptc(ch,"%s{x (%s) : %s\n\r",cfg_table[i].showname,cfg_table[i].name,
      IS_CFG(ch,cfg_table[i].bit) ? (cfg_table[i].on ? "выкл":"вкл") :(cfg_table[i].on?"вкл":"выкл"));
    }
    ptc(ch,"{CВыход в limbo при лостлинке {x        (autologout): %d ticks\n\r",ch->settimer);
    if (ch->pcdata->auto_online) ptc(ch,"{YAutoOnline {x: %s\n\r",ch->pcdata->auto_online);
     else stc("{YAutoOnline{x : {Rвыкл{x\n\r",ch);
    if (ch->pcdata->tickstr) ptc(ch,"{YTickstring {x: %s\n\r",ch->pcdata->tickstr);
    ptc(ch,"{YMaxRun {x    : %d\n\r",ch->maxrun[0]);
   return;
  }

  argument=one_argument(argument,arg);

  room = (ch->in_room) ? ch->in_room : ch->was_in_room;

 if (!IS_SET(room->room_flags,ROOM_ARENA) || IS_IMMORTAL(ch))
  {
   for (i=0;i<MAX_CFG;i++)
   {
    if (!str_prefix(arg,cfg_table[i].name))
    {
        ch->pcdata->cfg=toggle_int64(ch->pcdata->cfg,cfg_table[i].bit);
        ptc(ch,"%s{x %s{x\n\r",cfg_table[i].showname,
            IS_CFG(ch,cfg_table[i].bit)? (cfg_table[i].on ? "{Rвыкл":"{Gвкл"):(cfg_table[i].on?"{Gвкл":"{Rвыкл"));
      return;
    }
   }
  }
  else
   {
      stc("{RНа арене люди сражаются, а не настройками балуются!{x\n\r",ch);
      WAIT_STATE(ch,PULSE_VIOLENCE);
      return;
   } 

  if (!str_prefix(arg,"autoonline"))
  {
    if (argument[0]!='\0')
    {
      free_string(ch->pcdata->auto_online);
      ch->pcdata->auto_online=str_dup(argument);
      ptc(ch,"Теперь при входе ты будешь видеть в онлайне таких игроков:\n\r%s\n\r",ch->pcdata->auto_online);
      return;
    }
    free_string(ch->pcdata->auto_online);
    ch->pcdata->auto_online=NULL;
    stc("Авто-онлайн выключен\n\r.",ch);
    return;

  }
 
  if (!str_prefix(arg,"autologout") || !str_prefix(arg,"logout"))
  {
    argument=one_argument(argument,arg);
    if (!is_number(arg) || (i=atoi(arg))<2 || i>30)
      stc("Укажите число в пределах 2 - 30.\n\r",ch);
    else ch->settimer=i;
    ptc(ch,"Текущее значение autologout %d.\n\r",ch->settimer);
    return; 
  }

  if (!str_prefix(arg,"maxrun"))
  {
    argument=one_argument(argument,arg);
    if (!is_number(arg) || (i=atoi(arg))<5 || i>150)
      stc("Укажите число в пределах 5 - 150.\n\r",ch);
    else ch->maxrun[0]=i;
    ptc(ch,"Текущее значение для непреревной пробежки - %d.\n\r",ch->maxrun[0]);
    return; 
  }

  if (!str_prefix(arg,"tickstring"))
  {
    char buf[MAX_STRING_LENGTH];

    if (EMPTY(argument))
    {
      free_string(ch->pcdata->tickstr);
      ch->pcdata->tickstr=&str_empty[0];
    }
    strcpy(buf,argument);
    if (strlen(buf)>128) buf[128] = '\0';
    if (str_suffix("%c",buf)) strcat(buf," ");
    free_string(ch->pcdata->tickstr);
    ch->pcdata->tickstr=str_dup(buf);
    ptc(ch,"Стpока AutoTick в %s\n\r",ch->pcdata->tickstr);
    return;
  }
  stc("Такой опции нет.\n\r",ch);
}

void do_showskill(CHAR_DATA *ch, const char *argument)
{
  int sn;
  bool found=FALSE;

  if (!*argument)
  {
    stc("Syntax:showskill <skill or spell name>\n\r",ch);
    return;
  }

  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name == NULL) break;
    if (LOWER(argument[0]) == LOWER(skill_table[sn].name[0]) &&  !str_prefix(argument,skill_table[sn].name))
    {
      found=TRUE;
      break;
    }
  }
  if(!found) stc("Такого заклинания или умения нет.\n\r",ch);
  else
  {
    ptc(ch,"{M[{G%s{M]\n\r{CТвои способности: {M[{G%d{M] {C(%d + %d){x\n\r",skill_table[sn].name,category_bonus(ch,skill_table[sn].group)+get_skill_bonus(ch, sn),category_bonus(ch,skill_table[sn].group),get_skill_bonus(ch, sn));
    stc("{GЭтот навык или заклинание доступно с таких уровней:{x\n\r",ch);
    if (skill_table[sn].skill_level[0]<102) ptc(ch,"  Mag: {C%d{Y",skill_table[sn].skill_level[0]);
    else stc("  Mag: {RN/A{Y",ch);
    if (skill_table[sn].skill_level[1]<102) ptc(ch,"  Cle: {C%d{Y",skill_table[sn].skill_level[1]);
    else stc("  Cle: {RN/A{Y",ch);
    if (skill_table[sn].skill_level[2]<102) ptc(ch,"  Thi: {C%d{Y",skill_table[sn].skill_level[2]);
    else stc("  Thi: {RN/A{Y",ch);
    if (skill_table[sn].skill_level[3]<102) ptc(ch,"  War: {C%d{Y\n\r",skill_table[sn].skill_level[3]);
    else stc("  War: {RN/A{Y\n\r",ch);
  }
}

void do_crimereport(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];
  CHAR_DATA *crimer=NULL, *victim;

  one_argument(argument, arg);

  for (victim = ch->in_room->people; victim != NULL; victim= victim->next_in_room)
  {
    if (IS_NPC(victim) && victim->spec_fun==spec_lookup("spec_executioner"))
    {
      crimer=victim;
      break;
    }
  }

  if (!crimer)
  {
    stc("Тут некому рассказать о преступлении.\n\r",ch);
    return;
  }

  if (!*arg)
  {
    stc("Кого ты хочешь обвинить в совершении преступления?\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("Ты ошибся, этого человека сейчас нет.\n\r", ch);
    return;
  }

  if (ch==victim)
  {
    stc("На себя стучать? Ну-ну...\n\r", ch);
    return;
  }

  if (victim->criminal>50)
  {
    SET_BIT(crimer->talk,CBIT_SHOUT);
    do_printf(buf, "{yРазыскивается {Rопасный преступник{x %s!{x", get_char_desc(victim,'1'));
    do_function(crimer, &do_yell, buf);
    SET_BIT(victim->act,PLR_WANTED);
  }
  else 
  {
    do_printf(buf, "{y%s {rподлый лгун{x! Правосудие нельзя обмануть!{x", get_char_desc(ch,'1'));
    SET_BIT(crimer->talk,CBIT_SHOUT);
    do_function(crimer, &do_yell, buf);
    SET_BIT(ch->act,PLR_WANTED);
    ch->criminal+=50;
    multi_hit(crimer,ch);
  }
}

void act_toggle(CHAR_DATA *ch,int64 act_flag)
{
  if (IS_SET(ch->act,act_flag))
  {
    ptc(ch,"%s {Rвыключ.{x\n\r",act_name(act_flag));
    REM_BIT(ch->act,act_flag);
    return;
  }
  ptc(ch,"%s {Gвключ.{x\n\r",act_name(act_flag));
  SET_BIT(ch->act,act_flag);
}

char *act_name(int64 act_flags)
{
  static char buf[4096];

  buf[0] = '\0';

  if (act_flags & PLR_AUTOASSIST) strcat(buf, " Режим Автоматического вступления в битву (autoassist)");
  if (act_flags & PLR_AUTOLOOT ) strcat(buf, " Автоматическое подбирание трофеев из трупа (autoloot)");
  if (act_flags & PLR_AUTOSAC  ) strcat(buf, " Автоматическое принесения трупа в жертву (autosacrifice)");
  if (act_flags & PLR_AUTOGOLD ) strcat(buf, " Автоматическое подбирание денег из трупов (autogold)");
  if (act_flags & PLR_HOLYLIGHT) strcat(buf, " Святой свет");
  if (act_flags & PLR_NOSUMMON ) strcat(buf, " Тебя нельзя призвать заклинанием SUMMON (nosummon)");
  if (act_flags & PLR_NOCANCEL ) strcat(buf, " Ты не разрешаешь себя расколдовывать заклинанием CANCEL (nocancel)");
  if (act_flags & PLR_NOFOLLOW ) strcat(buf, " Ты не разрешаешь следовать за тобой (nofollow)");
  if (act_flags & PLR_COLOUR   ) strcat(buf, " Цвет");
  if (act_flags & PLR_NOSEND   ) strcat(buf, " Ты не принимаешь посылки (nosend)");
  if (act_flags & PLR_BLINK    ) strcat(buf, " Blink: ");

  return (buf[0] != '\0') ? buf+1 : "none";
}

void do_showprac(CHAR_DATA *ch, const char *argument)
{
  int sn=find_spell(ch,argument), rll=100;

  if (sn==-1)
  {
    stc("\n\rТы не знаешь такого заклинания или навыка.\n\r", ch);
    return;
  }

  if ((ch->classmag) && (rll > skill_table[sn].rating[0]) && (skill_table[sn].rating[0] != 0))
                       rll = skill_table[sn].rating[0];
  if ((ch->classcle) && (rll > skill_table[sn].rating[1]) && (skill_table[sn].rating[1] != 0))
                       rll = skill_table[sn].rating[1];
  if ((ch->classthi) && (rll > skill_table[sn].rating[2]) && (skill_table[sn].rating[2] != 0))
                       rll = skill_table[sn].rating[2];
  if ((ch->classwar) && (rll > skill_table[sn].rating[3]) && (skill_table[sn].rating[3] != 0))
                       rll = skill_table[sn].rating[3];

  if (sn < 0 || (!IS_NPC(ch) &&  (ch->level < min_level(ch,sn)
    || ch->pcdata->learned[sn] < 1 || rll == 0)) || !check_clanspell(sn,ch->clan))
  stc("\n\rТы не знаешь такого заклинания или навыка.\n\r", ch);
  else
  {
    ptc(ch,"{M[{G%s{M]\n\r{CТвои способности: {M[{G%d{M] {C(%d + %d){x\n\r",skill_table[sn].name,category_bonus(ch,skill_table[sn].group)+get_skill_bonus(ch, sn),category_bonus(ch,skill_table[sn].group),get_skill_bonus(ch, sn));
    if (skill_table[sn].spell_fun == spell_null)  ptc(ch,"\n\rТы знаешь {C%s{x на {Y%d%%{x\n\r",skill_table[sn].name,ch->pcdata->learned[sn]);
    else ptc(ch,"\n\rТы знаешь {C%s{x на {Y%d%% (требует %d mana){x\n\r",skill_table[sn].name,ch->pcdata->learned[sn],mana_cost(ch,sn));
  }
}

char *do_show_flag(CHAR_DATA *ch,CHAR_DATA *victim,bool fshort)
{
  static char buf[4096];

  buf[0] = '\0';

  if (IS_CFG(victim,CFG_ZRITEL)) strcat(buf,(fshort)?"{B(З){x":"{B(Зритель){x");
  if (IS_NPC(victim))
  {
    if (victim->max_hit > victim->hit) strcat(buf,(fshort)?"{r(Р){x":"{M(ранен){x");
    if (ch->questmob==victim) strcat(buf,"{R[ЦЕЛЬ]{x");
    if (victim->questmob!=NULL && IS_IMMORTAL(ch)) strcat(buf, "{G[qst]{x");
  }
  if (IS_SET(victim->comm,COMM_AFK    ) )strcat(buf,"[AFK]");
  if (victim->invis_level >= LEVEL_HERO  )strcat(buf,"{w[Wiz]{x");
  if (IS_AFFECTED(victim, AFF_INVISIBLE) )strcat(buf,(fshort)?"{D(H){x":"{w(Невидимо){x");
  if (IS_AFFECTED(victim, AFF_HIDE)      )strcat(buf,(fshort)?"{D(C){x":"{w(Скрыто){x");
  if (IS_AFFECTED(victim, AFF_CHARM)     )strcat(buf,(fshort)?"{G(О){x":"{w(Очаpовано){x");
  if (IS_AFFECTED(victim, AFF_PASS_DOOR) )strcat(buf,(fshort)?"{D(П){x":"{w(Пpозрачно){x");
  if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))strcat(buf,(fshort)?"{M(Р){x":"{m(Розовая Аура){x");
  if (is_affected(victim,skill_lookup("ensnare")))strcat(buf,(fshort)?"{G(C){x":"{g(Сеть){x");
  if (IS_AFFECTED(victim, AFF_SANCTUARY) )strcat(buf,(fshort)?"{W(Б){x":"{W(Белая Аура){x");
  if (IS_AFFECTED(victim, AFF_SHIELD)    )strcat(buf,(fshort)?"{C(M){x":"{C(Магический щит){x");

  if (((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) || IS_AFFECTED(ch, AFF_DETECT_EVIL)) 
     && IS_EVIL(victim))
    strcat(buf,(fshort)?"{R(К){x":"{r(Красная Аура){x");

  if (((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) || IS_AFFECTED(ch, AFF_DETECT_GOOD)) 
     && IS_GOOD(victim))
    strcat(buf,(fshort)?"{Y(З){x":"{y(Золотая Аура){x");

  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WANTED))
    strcat(buf,(fshort)?"{R[П]{x":"{r(ПРЕСТУПНИК){x");

  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_RAPER))
    strcat(buf,(fshort)?"{R[Н]{x":"{r(НАСИЛЬНИК){x");

  return (buf[0] != '\0') ? buf : "";
}

char *do_show_iflag(CHAR_DATA *ch,OBJ_DATA *obj, bool fshort)
{
  static char buf[4096];

  buf[0] = '\0';

  if (IS_OBJ_STAT(obj,ITEM_GLOW))   strcat(buf,(fshort)?"{M(Пыл){x":"{w({MПылает{w){x");
  if (IS_OBJ_STAT(obj,ITEM_INVIS))  strcat(buf,(fshort)?"{D(Нев){x":"{D(Невидимо){x");
  if (((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) || IS_AFFECTED(ch, AFF_DETECT_EVIL)) 
  && IS_OBJ_STAT(obj,ITEM_EVIL))
    strcat(buf,(fshort)?"{r(Кр){x":"{w({rКрасная{w Аура){x");
  if (((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) || IS_AFFECTED(ch, AFF_DETECT_GOOD)) 
   && IS_OBJ_STAT(obj,ITEM_BLESS))   strcat(buf,(fshort)?"{c(Гол){x":"{w({cГолубая{w Аура){x");
  if (IS_AFFECTED(ch,AFF_DETECT_MAGIC)
   && IS_OBJ_STAT(obj,ITEM_MAGIC))   strcat(buf,(fshort)?"{C(Маг){x":"{w({cМагическое{w){x");
  if (IS_OBJ_STAT(obj,ITEM_HUM))    strcat(buf,(fshort)?"{C(Гуд){x":"{w({CГудит{w){x");
  return (buf[0] != '\0') ? buf : "";
}

void whois_info(CHAR_DATA* ch, CHAR_DATA * victim)
{
  BUFFER *output;
  char buf[MAX_STRING_LENGTH], marry[30], *class;

  if (!victim) return;
  output = new_buf();

  class = classname(victim);
  switch(victim->level)
  {
    case MAX_LEVEL - 0 : class = "{DIMPLEM. {x";     break;
    case MAX_LEVEL - 1 : class = "{CCREATOR {x";     break;
    case MAX_LEVEL - 2 : class = "{CSUPERIOR{x";     break;
    case MAX_LEVEL - 3 : class = "{C DEITY  {x";     break;
    case MAX_LEVEL - 4 : class = "{c  БОГ   {x";     break;
    case MAX_LEVEL - 5 : class = "{rImmortal{x";     break;
    case MAX_LEVEL - 6 : class = "{rDemi God{x";     break;
    case MAX_LEVEL - 7 : class = "{rArhAngel{x";     break;
    case MAX_LEVEL - 8 : class = "{r Angel  {x";     break;
  }

  do_printf(buf,"\n\r{C /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/{x\n\r{C| {x%12s %s %-54s {C|{x\n\r{C| Уровень: {Y%3d  {CРаса: {Y%8s {CКласс: {Y%15s  {CПол: {Y%s         {C|{x\n\r{C| Клан:{x%s        {CКриминал:%s %s             {C|\n\r{x",
  victim->name, IS_SET(victim->comm, COMM_AFK) ? "{c[AFK]{x" : "     ",
  victim->pcdata->title, victim->level,
  race_wname(victim),
  class,victim->sex==0 ? "потерян":victim->sex == 1 ? "мужской":"женский",
  (victim->clan) ? victim->clan->show_name: "   не в клане   ",
  IS_SET(victim->act,PLR_WANTED) ?"{RПРЕСТУПНИК{x   ":"             ",
  IS_SET(victim->act,PLR_RAPER) ? "{RНАСИЛЬНИК{x":"         ");
  add_buf(output,buf);

  if (!EMPTY(victim->pcdata->marry)) do_printf(marry,"%s%s",victim->sex==2?"{Mзамужем за {Y":"{Mженат на {Y",victim->pcdata->marry);
  else do_printf(marry,"%s",victim->sex==2?"{Dне замужем{x":"{Dхолост{x");
  do_printf(buf,"{C| ClanRank: %s{C          Семейное положение: %20s{C     |{x\n\r",get_clan_rank(victim),marry);
  add_buf(output,buf);

  if (IS_IMMORTAL(ch))
  { 
    do_printf(buf,"{C| Количество убитых:           Количество смертей от:                       |\n\r| мобов:   {R%-7ld             {Cмобов  :{R%-7ld                              {C|\n\r| игроков: {R%-7ld             {Cигроков:{R%-7ld                              {C|\n\r| Побед на арене:{G%-7ld{C       Поражений на арене:{G%-7ld{C                   |\n\r{x",
    victim->vic_npc,victim->death_npc,victim->vic_pc_total,victim->death_pc_total,
    victim->vic_pc_arena, victim->death_pc_arena);
    add_buf(output,buf);
  }

  if (ch == victim || IS_IMMORTAL(ch))
  {
   do_printf(buf,"{C| Участвовал в квестах: {W%-7ld{Cвыполнил: {W%-7ld  {Cна этом уровне: {W%-7ld   {C|{x\n\r",victim->qcounter,victim->qcomplete[0],victim->qcomplete[1]);
   add_buf(output,buf);
  } 

  if (GUILD(victim,DWARVES_GUILD))
  {
    if (ELDER(victim,DWARVES_GUILD))
      do_printf(buf,"{C| Является {GСтарейшиной {C в {GГильдии Гномов                                   {C |{x\n\r");
    else do_printf(buf,"{C| Является {Gкузнецом{C из {GГильдии Гномов                                      {C |{x\n\r");
     add_buf(output,buf);
  }
  if (GUILD(victim,DRUIDS_GUILD))
  {
    if (ELDER(victim,DRUIDS_GUILD))
      do_printf(buf,"{C| Является {GСтарейшиной {C в {GГильдии Друидов                                  {C |{x\n\r");
    else do_printf(buf,"{C| Является {Gпослушником{C из {GГильдии Друидов                                  {C |{x\n\r");
     add_buf(output,buf);
  }
  if (GUILD(victim,VAMPIRE_GUILD))
  {
    if (ELDER(victim,VAMPIRE_GUILD))
         do_printf(buf,"{C| Является {RВысшим Вампиром{C из {RГильдии Вампиров                              {C|{x\n\r");
    else do_printf(buf,"{C| Является {RВампиром {Cиз {RГильдии Вампиров                                     {C|{x\n\r");
    add_buf(output,buf);
  }

  add_buf(output,"{C=---------------------------------------------------------------------------=\n\r{x");

  page_to_char(buf_string(output),ch);
  free_buf(output);
}

void mstat_info(CHAR_DATA *ch, CHAR_DATA *victim)
{

  stc("{C/---------------------------{DCHAR/MOBStatistic{C-------------------------------/\n\r",ch);

  if (!IS_NPC(victim))
  {
   ptc(ch, "{C|{GAge : {W%5d   {GName:{x%s %s{C\n\r| {GLevel:{W%5d  {GClass:{W%8s {GRace:{W%7s {GSex  : {W%5s {GSec   :{W%d{C\n\r",
    get_age(victim), victim->name, victim->pcdata->title,
    victim->level, class_table[victim->class[victim->remort]].name,
    race_table[victim->race].name,
    ch->sex == 0 ? "бесполый" : ch->sex == 1 ? "мужской" : "женский",
    victim->pcdata->security);

   ptc(ch, "|{GGroup: {W%7d {GRoom:{W%7d                                                   \n\r=---------------------------------------------------------------------------=\n\r",
    (victim->group)?victim->group:0 , (victim->in_room) ? victim->in_room->vnum:0);

   ptc(ch, "|  {GStr: {Y%3d {C({Y%3d{C) | {GHeal :{W%7d {C/{W%7d {C| {YЗолото  :{W %u{C\n\r",
    victim->perm_stat[STAT_STR],get_curr_stat(victim,STAT_STR),
    victim->hit,victim->max_hit,victim->gold);

   ptc(ch, "|  {GInt: {Y%3d {C({Y%3d{C) | {GMana :{W%7d {C/{W%7d {C| {WCеребро :{W %u{C\n\r",
    victim->perm_stat[STAT_INT],get_curr_stat(victim,STAT_INT),
    victim->mana,victim->max_mana, victim->silver);

   ptc(ch, "|  {GWis: {Y%3d {C({Y%3d{C) | {GMoves:{W%7d {C/{W%7d {C| To level: {W%d{C\n\r",
    victim->perm_stat[STAT_WIS],get_curr_stat(victim,STAT_WIS),
    victim->move,victim->max_move, (victim->level+1)*exp_per_level(victim, victim->pcdata->points)-victim->exp);

   ptc(ch, "|  {GDex: {Y%3d {C({Y%3d{C) | {YTr/Pr:{x%7d {C/{x%7d{C | {GAlign : {W%d{C\n\r",
    victim->perm_stat[STAT_DEX],get_curr_stat(victim,STAT_DEX),victim->train,
    victim->practice,victim->alignment);

   ptc(ch, "|  {GCon: {Y%3d {C({Y%3d{C) | {YClan :{W%s {C| {YQuestpoint: {W%d{C\n\r",
    victim->perm_stat[STAT_CON],get_curr_stat(victim,STAT_CON),
    (victim->clan==NULL)? "{D    - none -   {x " : victim->clan->show_name,victim->questpoints);

  }
  else
  {
   ptc(ch, "| {GVnum : {W%5u ",victim->pIndexData->vnum);
   ptc(ch, "{GName:{x%s %s{C\n\r| {GLevel:{W%5d {GRace:{W%7s {GSex  : {W%5s                \n\r",
    victim->name, victim->level, race_table[victim->race].name,
    ch->sex == 0 ? "бесполый" : ch->sex == 1 ? "мужской" : "женский");

   ptc(ch, "|{GGroup: {W%7d {GRoom:{W%7d {GCount: {W%d {GKilled:{W%d{C                     \n\r=---------------------------------------------------------------------------=\n\r",
    (victim->group)?victim->group:0 , (victim->in_room) ? victim->in_room->vnum:0,
    victim->pIndexData->count, victim->pIndexData->killed);

   ptc(ch, "|  {GStr: {Y%3d {C({Y%3d{C) | {GHeal :{W%7d {C/{W%7d {C|            \n\r",
    victim->perm_stat[STAT_STR],get_curr_stat(victim,STAT_STR),
    victim->hit,victim->max_hit);

   ptc(ch, "{GInt: {Y%3d {C({Y%3d{C) | {GMana :{W%7d {C/{W%7d {C|            \n\r",
    victim->perm_stat[STAT_INT],get_curr_stat(victim,STAT_INT),
    victim->mana,victim->max_mana);

   ptc(ch, "|  {GWis: {Y%3d {C({Y%3d {C) | {GMoves:{W%7d {C/{W%7d {C|         \n\r",
    victim->perm_stat[STAT_WIS],get_curr_stat(victim,STAT_WIS),
    victim->move,victim->max_move);

   ptc(ch, "|  {GDex: {Y%3d {C({Y%3d {C) |                          | {GAlign : {W%d{C\n\r",
    victim->perm_stat[STAT_DEX],get_curr_stat(victim,STAT_DEX),victim->alignment);
   
   ptc(ch, "|  {GCon: {Y%3d {C({Y%3d {C) | {YClan :{W%s     {C|                \n\r",
    victim->perm_stat[STAT_CON],get_curr_stat(victim,STAT_CON),
    (victim->clan==NULL)? "{D   - none -  {x " : victim->clan->show_name);
 
  }

  ptc(ch,"|{WAC:{Gpierce:{W%6d{C | {RHit  :{W%7d  {C        | Size      : {W%s{C\n\r",
   GET_AC(victim,AC_PIERCE),GET_HITROLL(victim),size_table[victim->size].name);

  ptc(ch,"|   {Gbash  :{W%6d{C | {RDam  :{W%7d  {C        | Pos       : {W%s{C\n\r",
   GET_AC(victim,AC_BASH),GET_DAMROLL(victim),position_table[victim->position].name);

  ptc(ch,"|   {Gslash :{W%6d{C | {CSaves:{W%4d(%4d)  {C     | {RFightning : {W%s{C\n\r|   {Gmagic :{W%6d{C | {RWimpy:{W%6d  {C         | {GItems/Wght: {W%d/%d{C\n\r",
   GET_AC(victim,AC_SLASH),-1*calc_saves(victim),victim->saving_throw,victim->fighting ?
   victim->fighting->name : "(none)",
   GET_AC(victim,AC_EXOTIC),victim->wimpy,victim->carry_number,
   get_carry_weight(victim)/10);

  ptc(ch,"{C=---------------------------------------------------------------------------={x\n\r");

  if (!IS_NPC(victim))
  {
    ptc(ch,"  {Gtoquest:{W%5d {Gqcount  :{W%5d",
        victim->nextquest,victim->countdown);

    if (victim->questobj) 
     ptc(ch,"{Gquestobj:{W%5d{x\n\r",victim->questobj);
    if (victim->questmob) 
     ptc(ch,"{Gquestmob:{W%s\n\r{x",get_char_desc(victim->questmob,'1'));
  }

  if (IS_NPC(victim))
  {
   ptc(ch,"  {GDamage: {W%5dd%5d{C  {GMessage:  {W%s{x\n\r",
    victim->damage[DICE_NUMBER],victim->damage[DICE_TYPE],attack_table[victim->dam_type].noun);
   if (victim->questmob) ptc(ch,"{CQuesmob for: {W%s\n\r",victim->questmob->name);
  }

  ptc(ch, "  {GMaster: {W%10s {GLeader  : {W%10s {GPet  :{W%s{x\n\r",
   victim->master      ? victim->master->name   : "(none)",
   victim->leader      ? victim->leader->name   : "(none)",
   victim->pet         ? victim->pet->name      : "(none)");

  if (!IS_NPC(victim))
  {
   ptc(ch,"  {GThirst: {W%10d {GHunger  : {W%10d {GFull : {W%10d {GDrunk: {W%d{x\n\r",
    victim->pcdata->condition[COND_THIRST],victim->pcdata->condition[COND_HUNGER],
    victim->pcdata->condition[COND_FULL],victim->pcdata->condition[COND_DRUNK]);

  ptc(ch,"  {GPlayed: {W%10d {GLast Lvl: {W%10d {GTimer: {W%d{x\n\r",
    (int) (victim->played + current_time - victim->logon) / 3600, 
     victim->pcdata->last_level, victim->timer);
  ptc(ch,"{C=---------------------------------------------------------------------------={x\n\r");
  
  if (IS_CFG(victim,CFG_GETEXP))
  {
   stc("GetExp: {GON{x\n\r",ch);
  }
  else
  {
   stc("GetExp: {ROFF{x\n\r",ch);
  }

  ptc(ch, "{RAdrenalin: {W%d{x",victim->pcdata->condition[COND_ADRENOLIN]);
  }

  ptc(ch, "\n\r{YAct: {W%s{x",act_bit_name(victim->act));
       
  if (victim->comm) ptc(ch,"\n\r{CComm: {W%s{x",comm_bit_name(victim->comm));

  if (IS_NPC(victim) && victim->off_flags) ptc(ch, "\n\r{GOffense: {W%s{x",off_bit_name(victim->off_flags));

  if (victim->imm_flags) ptc(ch, "\n\r{MImmune:{W%s{x",imm_bit_name(victim->imm_flags));
 
  if (victim->res_flags) ptc(ch, "\n\r{RResist: {W%s{x",imm_bit_name(victim->res_flags));

  if (victim->vuln_flags) ptc(ch, "\n\r{DVulnerable: {W%s{x",imm_bit_name(victim->vuln_flags));

  ptc(ch, "\n\r{GForm : {W%s{x", form_bit_name(victim->form));

  if (victim->affected_by) ptc(ch, "\n\r{CAffected by {W%s{x",affect_bit_name(victim->affected_by));

  if (IS_NPC(ch))
  {
   ptc(ch, "\n\r{GShort description: {W%s\n\r{GLong  description: {W%s{x",
   get_char_desc(victim, '1'),victim->long_descr[0] != '\0' ? victim->long_descr : "(none)");
  }
  if (IS_NPC(victim) && victim->spec_fun != 0)
    ptc(ch,"\n\r{MMobile has special procedure {W%s{x",spec_name(victim->spec_fun));

  if (IS_NPC(victim) && victim->stealer!=NULL) ptc(ch,"Stealers: %s",victim->stealer);
  stc("\n\r{D=---------------------------------------------------------------------------={x\n\r",ch);
}
