// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#if defined (WIN32)
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "magic.h"

void check_gsocial args( ( CHAR_DATA *ch, char *command, const char *argument ) );
char *comm_name(int64 comm_flags);
void talk( CHAR_DATA *ch, const char *argument, int channel );
void tell(CHAR_DATA *ch,CHAR_DATA *victim, const char *argument);
void dec_worship( CHAR_DATA *ch);

struct pose_table_type
{
  char * message[2*MAX_CLASS];
};

struct talk_type
{
  char *name;
  int   channel;
  bool  npc;
  int64 bit;
  int   level;
  char *showname;
  char *string;
  bool  yourname;
  char *sstring;
};

struct spam_type
{
  char *name;
  char *showname;
  int  bit;
};

#define MAX_SPAM_TYPE 4
const struct spam_type spam_table[] =
{
// "command_name", "show_list_name                   ", spam_bit 
 { "missing     ", "Выводить сообщения о промахе     ", SPAM_MISS   },
 { "otherfight  ", "Выводить чужие боевые действия   ", SPAM_OTHERF },
 { "skillmiss   ", "Выводить сообщ. о промахе навыков", SPAM_SKMISS },
 { "weaponaff   ", "Выводить спец.удары оружия       ", SPAM_WEAPAF }
};

const struct talk_type talk_table[] =
{
  {"yell",    CHAN_YELL,    FALSE,CBIT_YELL,     1,"{RYell                      ","%s гpомко кpичит '{M%s{x'\n\r" ,   FALSE,"%s гpомко кpичишь '{M%s{x'\n\r"   },
  {"auction", CHAN_AUCTION, TRUE ,CBIT_AUCTION,  2,"{YКанал аукционов           ","%s аукционирует '{Y%s{x'\n\r",     FALSE,"%s аукционируешь '{Y%s{x'\n\r"    },
  {"gossip",  CHAN_GOSSIP,  TRUE ,CBIT_GOSSIP,   2,"{RGossip                    ","%s кpичит '{M%s{x'\n\r",           FALSE,"%s кpичишь '{M%s{x'\n\r"          },
  {"shouts",  CHAN_SHOUT ,  TRUE ,CBIT_SHOUT ,   2,"{MКанал Криков              ","%s оpет '{R%s{x'\n\r",             FALSE,"%s оpешь '{R%s{x'\n\r"            },
  {"question",CHAN_QUESTION,TRUE ,CBIT_QUESTION, 2,"{YКанал вопросов и ответов  ","%s спpашивает '{y%s{x'\n\r",       FALSE,"%s спpашиваешь '{Y%s{x'\n\r"      },
  {"answer",  CHAN_ANSWER,  TRUE ,CBIT_ANSWER,   2,"{YКанал вопросов и ответов  ","%s отвечает '{Y%s{x'\n\r",         FALSE,"%s отвечаешь '{Y%s{x'\n\r"        },
  {"music",   CHAN_MUSIC,   FALSE,CBIT_MUSIC,    2,"{CМузыкальный канал         ","{y%s{x поет '{C%s{x'\n\r",         FALSE,"%s поешь '{C%s{x'\n\r"            },
  {"clan",    CHAN_CLAN,    FALSE,CBIT_CLAN,     2,"{CКлановый канал            ","%s говорит клану '{C%s{x'\n\r",    FALSE,"%s говоришь клану '{C%s{x'\n\r"   },
  {"quote",   CHAN_QUOTE,   TRUE ,CBIT_QUOTE,    2,"{WКанал цитирований         ","%s замечает '%s'\n\r",             FALSE,"%s замечаешь '%s'\n\r"            },
  {"censored",CHAN_CENSORED,FALSE,CBIT_CENSORED, 3,"{CКанал без контроля цензуры","{R[Censored] {Y%s: {G%s{x\n\r",    TRUE, "{R[Censored] {Y%s: {G%s{x\n\r"    },
  {"grats",   CHAN_GRAT,    FALSE,CBIT_GRAT,     2,"{CКанал поздравлений        ","%s поздpавляет '{C%s{x'\n\r",      FALSE,"%s поздpавляешь '{C%s{x'\n\r"     },
  {"chat",    CHAN_CHAT,    TRUE ,CBIT_CHAT,     2,"{CChat                      ","{C[Chat] {Y%s{w: {G%s{x\n\r",      TRUE, "{C[Chat] {Y%s{w: {G%s{x\n\r"      },
  {"newbie",  CHAN_NEWBIE,  TRUE ,CBIT_NEWBIE,   1,"{MКанал новичков            ","{R[newbie] {Y%s: {W%s{x\n\r",      TRUE, "{R[newbie] {Y%s: {W%s{x\n\r"      },
  {"alli",    CHAN_ALLI,    FALSE,CBIT_ALLI,     1,"{cКанал союзников           ","{M[Alli] {Y%s{w: {C%s{x\n\r",      TRUE, "{M[Alli] {Y%s{w: {C%s{x\n\r"      },
  {"kazad",   CHAN_KAZAD,   FALSE,CBIT_KAZAD,    1,"{DКанал гномов              ","{c[{yKazad{c] {g%s{w: {G%s{x\n\r",TRUE, "{c[{yKazad{c] {g%s{w: {G%s{x\n\r" },
  {"ptalk",   CHAN_PTALK,   FALSE,CBIT_PTALK,    1,"{DКанал для супругов        ","{C[Private] {Y%s{w: {W%s{x\n\r",   TRUE, "{C[Private] {Y%s{w: {W%s{x\n\r"   },
  {"avenge",  CHAN_AVENGE,  FALSE,CBIT_AVENGE,   1,"Канал гильдии {Rассасинов   ","{w[{RAvenge{w] {w%s{w: {Y%s{x\n\r",TRUE, "{w[{RAvenge{w] {w%s{w: {Y%s{x\n\r"},
  {"gtell",   CHAN_GTELL,   FALSE,CBIT_GTELL,    1,"{GРазговоры группы          ","%s говоpит гpуппе '{G%s{x'\n\r",   FALSE,"%s говоpишь гpуппе '{G%s{x'\n\r"  },
  {"immtalk", CHAN_IMMTALK, FALSE,CBIT_IMMTALK,  1,"{mКанал богов               ","",FALSE,""},
  // not saving in disconnect
  {"info",    CHAN_INFO,    FALSE,CBIT_INFO,     1,"{CИнформационный канал      ","",FALSE,""},
  {"emotes",  CHAN_EMOTE,   TRUE, CBIT_EMOTE,    1,"{DЭмоции                    ","",FALSE,""},
  {"gsocial", CHAN_GSOCIAL, TRUE, CBIT_GSOCIAL,  1,"{DГлобальные эмоции         ","",FALSE,""},
  {NULL,      0,            TRUE, 0,             0,""                            ,"",FALSE,""}
};              

void do_yell( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_YELL);
}
void do_oauction( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_AUCTION);
}
void do_gossip( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_GOSSIP);
}
void do_shout( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_SHOUT);
}
void do_question( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_QUESTION);
}
void do_answer( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_ANSWER);
}
void do_music( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_MUSIC);
}
void do_clantalk( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_CLAN);
}
void do_quote( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_QUOTE);
}
void do_censored(CHAR_DATA *ch, const char *argument)
{
  if(ch->level<3 && ch->remort<1 && !IS_IMMORTAL(ch)) {
    stc("Тебе еще рано использовать этот канал.\n\r",ch);
    return;
  }
  talk(ch,argument,CHAN_CENSORED);
}
void do_grats( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_GRAT);
}
void do_chat( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_CHAT);
}
void do_newbiechat( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_NEWBIE);
}
void do_alli( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_ALLI);
}
void do_kazad( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_KAZAD);
}
void do_ptalk( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_PTALK);
}
void do_avenge( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_AVENGE);
}
void do_gtell( CHAR_DATA *ch, const char *argument )
{
  talk(ch,argument,CHAN_GTELL);
}

void do_spam(CHAR_DATA *ch, const char *argument)
{
  int i, item=-1;

  if ( EMPTY(argument) ) {
   stc(   "Конфигурация подавления SPAM :\n\r",ch);
   stc(   "------------------------------\n\r",ch);
   for (i=0;i<MAX_SPAM_TYPE;i++)
     ptc(ch,"%s [%s] %s\n\r",spam_table[i].showname,spam_table[i].name,
       IS_SET(ch->spam,spam_table[i].bit)?"Нет":"Да");
   return;
  }

  for (i=0;i<MAX_SPAM_TYPE;i++)
   if (!str_prefix(argument,spam_table[i].name)) {
     item=i;
     break;
   }

  if (item==-1) {
    stc("Нет такой опции.",ch);
    return;
  }

  ch->spam=toggle_int(ch->spam,spam_table[item].bit);
  ptc(ch,"{R%s теперь в%sключен.{x\n\r",spam_table[item].showname,
    (IS_SET(ch->spam,spam_table[item].bit) ? "ы":""));
}

void do_talk(CHAR_DATA *ch, const char *argument)
{
  int i,item=-1;

  if (!*argument) {
   stc(   "Конфигурация сохранения разговоров при дисконнекте:\n\r",ch);
   stc(   "---------------------------------------------------\n\r",ch);
   for (i=0;talk_table[i].channel!=CHAN_INFO;i++)
     ptc(ch,"%s [%10s] %s\n\r",talk_table[i].showname,talk_table[i].name, IS_SET(ch->pcdata->comm_save,talk_table[i].bit)?"Нет":"Да");
   return;
  }

  for (i=0;talk_table[i].channel!=CHAN_INFO;i++)
   if (!str_prefix(argument,talk_table[i].name)) {
     item=i;
     break;
   }

  if (item==-1) {
    stc("Нет такой опции.",ch);
    return;
  }

  ch->pcdata->comm_save=toggle_int64(ch->pcdata->comm_save,talk_table[item].bit);
  ptc(ch,"Запись канала {Y%s{x теперь в%sключена.{x\n\r",talk_table[item].name, (IS_SET(ch->pcdata->comm_save,talk_table[item].bit) ? "ы":""));
}

void do_channels( CHAR_DATA *ch, const char *argument)
{
  register int i=0;

  if (EMPTY(argument)) {
    for (i=0;talk_table[i].name;i++)
    {
      if (talk_table[i].channel==CHAN_KAZAD && !GUILD(ch,DWARVES_GUILD)) continue;
      if (talk_table[i].channel==CHAN_AVENGE && !GUILD(ch,ASSASIN_GUILD)) continue;
      if (talk_table[i].channel==CHAN_PTALK && (IS_NPC(ch) || !*ch->pcdata->marry)) continue;
      if ((talk_table[i].channel==CHAN_ALLI || talk_table[i].channel==CHAN_CLAN) && (!ch->clan || IS_SET(ch->clan->flag, CLAN_LONER))) continue;
      ptc(ch,"%s{x [%10s] : в%sкл\n\r",talk_table[i].showname,talk_table[i].name,
        IS_SET(ch->talk,talk_table[i].bit)?"":"ы");
    }
    if (IS_SET(ch->comm,COMM_AFK))  stc("Режим AFK включен.\n\r",ch);
    if (IS_SET(ch->comm,COMM_DEAF)) stc("Каналы выключены (deaf mode).\n\r",ch);
    if (IS_SET(ch->comm,COMM_QUIET))stc("Режим тишины включен (quiet mode).\n\r",ch);
    if (ch->lines != PAGELEN) {
      if (ch->lines) ptc(ch," Вывод %d строк на экран.\n\r",ch->lines+2);
      else stc(" Буферизация вывода отключена.\n\r",ch);
    }
    if (ch->prompt != NULL) ptc(ch," Ваша строка состояния: %s\n\r",ch->prompt);
    return;
  }

  if (!str_cmp(argument,"help")) {
    stc("Для того, чтобы включить или выключить канал. наберите его название.\n\r",ch);
    stc("Для вывода списка каналов и опций наберите channel без аргументов.\n\r",ch);
    stc("Пример: {Cchannel newbie{x\n\r",ch);
    return;
  }

  for (i=0;talk_table[i].name;i++) {
    if (talk_table[i].channel==CHAN_KAZAD && !GUILD(ch,DWARVES_GUILD)) continue;
    if (talk_table[i].channel==CHAN_AVENGE && !GUILD(ch,ASSASIN_GUILD)) continue;
    if (talk_table[i].channel==CHAN_CENSORED && ch->level<5) continue;
    if (!str_prefix(argument,talk_table[i].name)) {
      ch->talk=toggle_int64(ch->talk,talk_table[i].bit);
      ptc(ch,"%s{x %s.{x\n\r",talk_table[i].showname,
        IS_SET(ch->talk,talk_table[i].bit)? "{Gвключ":"{Rвыключ");
      return;
    }
  }
  stc("Такой опции нет.\n\r",ch);
}

void do_afk ( CHAR_DATA *ch, const char * argument)
{
  char buf[MAX_STRING_LENGTH];

  ch->comm=toggle_int64(ch->comm,COMM_AFK);
  ptc(ch,"Режим AFK в%sключен",IS_SET(ch->comm,COMM_AFK)?"":"ы");
  do_printf(buf,"{Y%s{x %s {CAFK{x.",ch->name,IS_SET(ch->comm,COMM_AFK)?"ушел в":"вернулся из");
  act(buf,ch,NULL,NULL,TO_ROOM);
}

// All channels
void do_deaf( CHAR_DATA *ch, const char * argument)
{
  ch->comm=toggle_int64(ch->comm,COMM_DEAF);
  ptc(ch,"Режим глухоты (Dead) в%sключен",IS_SET(ch->comm,COMM_DEAF)?"":"ы");
}

// All tells, except reply
void do_quiet( CHAR_DATA *ch, const char * argument)
{
  ch->comm=toggle_int64(ch->comm,COMM_QUIET);
  ptc(ch,"Режим тишины (quiet) в%sключен",IS_SET(ch->comm,COMM_QUIET)?"":"ы");
}

void do_replay (CHAR_DATA *ch, const char *argument)
{
  if (EMPTY(buf_string(ch->pcdata->buffer))) {
    stc("Нечего повторять.\n\r",ch);
    return;
  }
  page_to_char(buf_string(ch->pcdata->buffer),ch);
  clear_buf(ch->pcdata->buffer);
}

void do_immtalk( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *vch;

  if (EMPTY(argument)) return;

  if (!IS_SET(ch->talk,CBIT_IMMTALK)) {
    stc("Этот канал у тебя выключен.\n\r",ch);
    return;
  }
  if (!IS_IMMORTAL(ch)) {
    ptc(ch,"{MТы говоришь богам {x'%s{x'\n\r",argument);
    do_printf(buf,"{Y%s {Mговорит богам '%s'{x\n\r",ch->name, argument);
  }
  else do_printf( buf, "{c[{y%s{c]:{M %s{x\n\r",ch->name, argument );

  for ( vch= char_list; vch != NULL; vch = vch->next ) {
    if (IS_NPC(vch) || !IS_IMMORTAL(vch) || !IS_SET(vch->talk,CBIT_IMMTALK)) continue;
    if (!IS_IMMORTAL(ch) && IS_CFG(vch,CFG_NOIMMS)) continue;
    if (is_exact_name(ch->name,vch->pcdata->ignorelist)) continue;
    if (!vch->desc) add_buf(vch->pcdata->buffer,buf);
    else if (vch->desc->connected==CON_PLAYING) stc(buf, vch);
  }
}

void do_say( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *wch;
  char buf[MAX_STRING_LENGTH];

  if (EMPTY(argument)) {
    stc( "Сказать что?\n\r", ch );
    return;
  }
  if (IS_SET(ch->act,PLR_TIPSY)) tipsy2(ch,"chat", (char*)argument);
  ptc(ch,"Ты пpоизносишь '{G%s{x'\n\r", argument);
  for(wch=ch->in_room->people;wch;wch=wch->next_in_room)
  {
    if (wch==ch || (IS_IMMORTAL(wch) && is_exact_name(ch->name,wch->pcdata->ignorelist)) || (is_affected(wch,skill_lookup("deaf")) && !IS_IMMORTAL(wch))) continue;
    if (!IS_NPC(wch) && wch->desc == NULL)
    {
      // Replace Imm's name with it's alias when say something
      if ( IS_IMMORTAL(ch) && !IS_IMMORTAL(wch) && ch->pcdata->pseudoname) { 
        do_printf(buf,"%s произносит '{G%s{x'\n\r",ch->pcdata->pseudoname,argument);
        add_buf(wch->pcdata->buffer,buf);
      } 
      else {
        do_printf(buf,"%s произносит '{G%s{x'\n\r",can_see(wch,ch,CHECK_LVL)?get_char_desc(ch,'1'):"Некто",argument);
        add_buf(wch->pcdata->buffer,buf);
      }
    }
    else if (IS_IMMORTAL(ch) && !IS_IMMORTAL(wch) && !EMPTY(ch->pcdata->pseudoname))
         ptc(wch,"%s произносит '{G%s{x'\n\r",can_see(ch, wch, CHECK_LVL)?ch->pcdata->pseudoname:"Некто",argument);
    else act_new("$n произносит '{G$t{x'",ch,argument,wch,TO_VICT,POS_RESTING);
  }
}

void do_tell( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  if (!IS_NPC(ch) && IS_SET(ch->comm,COMM_QUIET)) {
    stc( "Ты выключил разговоры.\n\r", ch );
    return;
  } 

  argument = one_argument( argument, arg );

  if (EMPTY(arg) || EMPTY(argument)) {
    stc( "Сказать кому и что?\n\r", ch );
    return;
  }

  if ( (victim=get_char_room(ch,arg))==NULL) {
    if ((victim=get_pchar_world(ch,arg))==NULL) {
      stc( "Таких здесь нет.\n\r", ch );
      return;
    }
  }
  tell(ch,victim,argument);
}

void do_reply( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *victim;

  if ( ( victim = ch->reply ) == NULL ) {
    stc( "Некому отвечать.\n\r", ch );
    return;
  }
  tell(ch,victim,argument);
}

void do_emote( CHAR_DATA *ch, const char *argument )
{
  if (!IS_NPC(ch) && !IS_SET(ch->talk, CBIT_EMOTE)) {
    stc( "Ты выключил эмоции.\n\r", ch );
    return;
  }
  if (IS_SET(ch->comm,COMM_NOEMOTE)) {
    stc( "Боги запрещают тебе волноваться.\n\r", ch );
    return;
  }
  if (EMPTY(argument))
  {
    stc( "С параметром.\n\r", ch );
    return;
  }
  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"emote")) return;

  MOBtrigger = FALSE; 
  act( "$n $T", ch, NULL, argument, TO_ROOM );
  act( "$n $T", ch, NULL, argument, TO_CHAR );
  MOBtrigger = TRUE; 
}

void do_pmote( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *vch;
  char *letter;
  const char *name;
  char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
  unsigned int matches = 0;

  if (!IS_SET(ch->talk, CBIT_EMOTE))
  {
    stc( "Ты выключил канал эмоций.\n\r", ch );
    return;
  }
  if (IS_SET(ch->comm,COMM_NOEMOTE))
  {
    stc( "Боги запрещают тебе волноваться.\n\r", ch );
    return;
  }  
  if (EMPTY(argument))
  {
    stc( "С параметром.\n\r", ch );
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"emote")) return;

  MOBtrigger = FALSE;   
  act( "$n $t", ch, argument, NULL, TO_CHAR );
  MOBtrigger = TRUE; 
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if (vch->desc == NULL || vch == ch) continue;

    if ((letter = strstr(argument,vch->name)) == NULL)
    {
      act("$N $t",vch,argument,ch,TO_CHAR);
      continue;
    }

    strcpy(temp,argument);
    temp[strlen(argument) - strlen(letter)] = '\0';
    last[0] = '\0';
    name = vch->name;
        
    for (; *letter != '\0'; letter++)
    { 
      if (*letter == '\'' && matches == strlen(vch->name))
      {
        strcat(temp,"r");
        continue;
      }

      if (*letter == 's' && matches == strlen(vch->name))
      {
        matches = 0;
        continue;
      }
            
      if (matches == strlen(vch->name)) matches = 0;

      if (*letter == *name)
      {
        matches++;
        name++;
        if (matches == strlen(vch->name))
        {
          strcat(temp,"you");
          last[0] = '\0';
          name = vch->name;
          continue;
        }
        strncat(last,letter,1);
        continue;
      }

      matches = 0;
      strcat(temp,last);
      strncat(temp,letter,1);
      last[0] = '\0';
      name = vch->name;
    }
    MOBtrigger = FALSE; 
    act("$N $t",vch,temp,ch,TO_CHAR);
    MOBtrigger = TRUE; 
  }
}

// (1) A ~ A
// (2) if A ~ B then B ~ A
// (3) if A ~ B  and B ~ C, then A ~ C
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
  if ( ach == NULL || bch == NULL) return FALSE;

  if ( ach->leader != NULL ) ach = ach->leader;
  if ( bch->leader != NULL ) bch = bch->leader;
  return ach == bch;
}

// Colour setting and unsetting, way cool, Lope Oct '94
void do_colour( CHAR_DATA *ch, const char *argument )
{
  ch->act=toggle_int64(ch->act,PLR_COLOUR);
  if(IS_SET(ch->act, PLR_COLOUR))
       stc( "{bЦ{rв{yе{cт{mа{x {Gвключены{x!\n\r", ch );
  else send_to_char_bw( "Цвета выключены.\n\r", ch );
}

//info types: void info(char|NULL, min level, int type, name, fraze)
//            1 - char_name char_fraze   3 fraze name (frazoy)
//            2 - fraze name             4- name fraze (frazoy)
void info(CHAR_DATA * ch, int level, int mes, const char *name, const char *fraze)
{
  DESCRIPTOR_DATA * d;

  for (d=descriptor_list;d;d=d->next)
  {
    if ( d->character && d->connected == CON_PLAYING
        && d->character!=ch &&  get_trust(d->character) >= level
        && IS_SET(d->character->talk,CBIT_INFO))
    {
      if (mes==4)
        ptc(d->character,"{CИнформация: {G%s {C%s{x\n\r",name,fraze);
      else if (mes==3)
        ptc(d->character,"{CИнформация: %s {G%s{x\n\r",fraze,name);
      else if (mes==2 && ch)
        ptc(d->character,"{CИнформация: {G%s{C %s{x\n\r",fraze,can_see(d->character,ch,NOCHECK_LVL) ? name: "Некто");
      else if (mes==1 && ch)
        ptc(d->character,"{CИнформация: %s {G%s{x\n\r",ch->name ,fraze);
    }
  }
}

void do_quenia( CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *och,*victim=NULL;
  int    chance,chance2=0,i;
  char arg1[MAX_INPUT_LENGTH];
  bool found=FALSE;
  int64 phraze=0;
 
  if((chance = get_skill(ch,gsn_quenia)) == 0) 
  {
    stc("Ты не умеешь разговаривать на Квенье.\n\r",ch);
    return; 
  }

  if(EMPTY(argument))
  {
    stc("Что произнести?\n\r",ch);
    return; 
  }

  if (IS_SET(ch->act,PLR_TIPSY)) tipsy2(ch,"clantalk", (char*)argument);

  if(number_percent( ) < chance+category_bonus(ch,LEARN)*3)
  {
    act("Ты {CИзрекаешь{x {G'$t'{x",ch,argument,NULL,TO_CHAR);
     
    for(och = ch->in_room->people; och != NULL;och = och->next_in_room  )
      if((chance2 = get_skill(och,gsn_quenia))+3*category_bonus(och,LEARN) >number_percent( ))
      {
        act_new("$n {CИзрекает {G'$t'{x",ch,argument,och,TO_VICT,POS_DEAD);
        check_improve(och,gsn_quenia,TRUE,4);
      }
      else act("$n что-то произносит на странном языке.",ch,NULL,och,TO_VICT);

    // parsing the quenia phraze
    for (;!EMPTY(argument);)
    {
      argument=one_argument(argument,arg1);
 
      for (i=0;quenia_table[i].name!=Q_END;i++)
      {
        if (victim==NULL && (victim=get_char_room(ch,arg1))!=NULL) continue;

        if (!str_cmp(quenia_table[i].descr,arg1) && !IS_NPC(ch) && ch->pcdata->quenia[i]>0)
        {
          ch->pcdata->quenia[i]--;
          SET_BIT(phraze,quenia_table[i].name);
          found=TRUE;
          continue;
        }

        if (quenia_table[i].word[0]==arg1[0] && quenia_table[i].word[1]==arg1[1])
        {
          found=TRUE;
          if (!str_cmp(quenia_table[i].word,arg1))
          {
            SET_BIT(phraze,quenia_table[i].name);
            quenia_table[i].counter--;
            if (quenia_table[i].counter==0)
            {
              quenia_table[i].counter=quenia_table[i].start_counter;
              ptc(ch,"{DСлово {C%s{D утрачивает свою силу.{x\n\r",quenia_table[i].word);
              quenia_table[i].word[0]='\0';
              strcat(quenia_table[i].word,create_word());
            }
          }
        }
      }
    }

    if (found) // magic words detected in phraze, begin to interpret
    {
      if (IS_SET(phraze,Q_RESTORE))
      {
        if (IS_SET(phraze,Q_POWER))
        {
          act("{Y$n {Cвосстановил тебя.{x",ch,NULL,NULL,TO_ROOM);
          stc("{CТы восстановил всех вокруг.{x\n\r",ch);
          do_restore(ch,"");
        }
        else
        {
          if (victim!=NULL)
          {
            act("{Y$n{C восстановил {Y$N{C.{x",ch,victim,NULL,TO_ROOM);
            act("{CТы восстановил силы {Y$N{C.{x\n\r",ch,NULL,victim,TO_CHAR);
            act("{Y$n{C восстановил твои силы.{x\n\r",ch,NULL,victim,TO_VICT);
            do_restore(ch,victim->name);
          }
          else
          {
            act("{CСилы {Y$n{C восстановлены.{x",ch,NULL,NULL,TO_ROOM);
            stc("{CТвои силы восстановлены.{x\n\r",ch);
            do_restore(ch,ch->name);
          }
        }
      }
      if (IS_SET(phraze,Q_PEACE))
      {
        CHAR_DATA *vch;

        for (vch=ch->in_room->people;vch;vch=vch->next_in_room)
        {
          if (IS_SET(phraze,Q_POWER))
          {
            if (IS_NPC(vch) && IS_SET(ch->act, ACT_AGGRESSIVE))
            {
              REM_BIT(vch->act,ACT_AGGRESSIVE);
              act("$C1 выглядит умиротворенно.",ch,NULL,vch,TO_CHAR);
            }
            if (!IS_NPC(vch) && vch->pcdata->condition[COND_ADRENOLIN]!=0)
            {
              vch->pcdata->condition[COND_ADRENOLIN]=0;
              act("$C1 выглядит умиротворенно.",ch,NULL,vch,TO_CHAR);
            }
            if (vch->fighting) stop_fighting (vch,TRUE);
          }
          else if (vch->fighting==ch) stop_fighting (vch,TRUE);
        }
        if (!IS_SET(phraze,Q_POWER))
          act("$c1 {Gостановил{x сражение.",ch,NULL,NULL,TO_ROOM);
        else
          act("$c1 {GОСТАНОВИЛ{x сражение.",ch,NULL,NULL,TO_ROOM);
        ptc(ch,"Ты {G%s {xсражение.\n\r",
          IS_SET(phraze,Q_POWER)?"ОСТАНОВИЛ":"остановил");
      }
      if (IS_SET(phraze,Q_FIREPROOF))
      {
        OBJ_DATA *obj;
        AFFECT_DATA af;
        CHAR_DATA *tmp;

        if (victim) tmp=victim;
        else tmp=ch;

        for (obj=tmp->carrying;obj;obj=obj->next)
        {
          if (obj->wear_loc!=WEAR_NONE && !IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
          {
            af.where     = TO_OBJECT;
            af.type      = skill_lookup("fireproof");
            af.level     = IS_SET(phraze,Q_POWER)? 200:ch->level;
            af.duration  = IS_SET(phraze,Q_POWER) ? 200: number_fuzzy(ch->level)/4+category_bonus(ch,MAKE)*3+category_bonus(ch,FIRE)*3;
            af.location  = APPLY_NONE;
            af.modifier  = 0;
            af.bitvector = ITEM_BURN_PROOF;
            affect_to_obj(obj,&af);
          }
        }
        act("{CОбмундирование на {Y$n{x вспыхивает ослепительным блеском.{x",tmp,NULL,NULL,TO_ROOM);
        stc("{CТвое обмундирование вспыхивает ослепительным блеском.{x\n\r",tmp);
      }
      if (IS_SET(phraze,Q_BLESS))
      {
        OBJ_DATA *obj;
        AFFECT_DATA af;
        CHAR_DATA *tmp;

        if (victim) tmp=victim;
        else tmp=ch;

        for (obj=tmp->carrying;obj;obj=obj->next_content)
        {
          if (obj->wear_loc!=WEAR_NONE && !IS_OBJ_STAT(obj,ITEM_BLESS))
          {
            af.where    = TO_OBJECT;
            af.type     = skill_lookup("bless");
            af.level    = IS_SET(phraze,Q_POWER)?200:ch->level;
            af.duration = 6 + IS_SET(phraze,Q_POWER)?200:ch->level;
            af.location = APPLY_SAVES;
            af.modifier = -1;
            af.bitvector= ITEM_BLESS;
            affect_to_obj(obj,&af);
            tmp->saving_throw -= 1;
         }
        }
        act("{CОбмундирование на {Y$n{x на мгновение загорается священным огнем.{x",tmp,NULL,NULL,TO_ROOM);
        stc("{CТвое обмундирование на мгновение загорается священным огнем.{x\n\r",tmp);
      }

      if (IS_SET(phraze,Q_GOODSPELL))
      {
        if (victim==NULL) victim=ch;
        spell_sanctuary( skill_lookup("sanctuary"), IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
        spell_haste(skill_lookup("haste"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
        spell_giant_strength(skill_lookup("giant strength"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
        spell_shield(skill_lookup("shield"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
        spell_bless(skill_lookup("bless"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
        spell_stone_skin(skill_lookup("stone skin"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
      }
      if (IS_SET(phraze,Q_BADSPELL))
      {
        if (!victim) victim=ch;
        if (!victim->in_room 
          || (( IS_SET(victim->in_room->room_flags,ROOM_SAFE)
       || IS_SET(victim->in_room->ra,RAFF_SAFE_PLC))
           && !IS_SET(victim->in_room->ra, RAFF_VIOLENCE)))
          stc("Злые заклинания не могут повредить в святом месте.\n\r",ch);
        else
        {
          spell_curse(gsn_curse, IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
          spell_poison(gsn_poison,IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
          spell_plague(gsn_plague,IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
          spell_blindness(gsn_blindness,IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
          spell_slow(skill_lookup("slow"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
          spell_weaken(skill_lookup("weaken"),IS_SET(phraze,Q_POWER)?120:ch->level+5,ch, victim,TARGET_CHAR);
        }
      }
      if (phraze==0)
      {
        act("{CНа мгновенье все вокруг стихло.{x",ch,NULL,NULL,TO_ALL_IN_ROOM);
      }
    }
  }
  else stc ("Ты не можешь этого произнести.\n\r",ch);
  check_improve(ch,gsn_quenia,FALSE,4);
}

void talk_auction (char *argument)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  do_printf (buf,"{RАукцион:{x {Y%s{x", argument);
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if ( d->character && d->connected == CON_PLAYING
        && IS_SET(d->character->talk,CBIT_AUCTION)
        && !IS_SET(d->character->comm,COMM_QUIET))
      act (buf, d->character, NULL, NULL, TO_CHAR);
  }
}

void do_gsocial( CHAR_DATA *ch, const char *argument )
{
  char command[MAX_INPUT_LENGTH];

  if (!*argument)
  {
    stc("{YСинтаксис:{x gsocial <social>\n\r",ch);
    return;
  }
  if(!IS_SET(ch->talk,CBIT_GSOCIAL))
  {
    stc("Глобальные социалы выключены.\n\r",ch);
    return;
  }
  if(IS_SET(ch->comm,COMM_NOGSOC))
  {
    stc("{RБоги запретили тебе глобально выражаться и сувать магические дули. :)).\n\r",ch);
    return;
  }
  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"social")) return;

  argument=one_argument( argument, command );
  check_gsocial( ch,command,argument );
}

void check_gsocial( CHAR_DATA *ch, char *command, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int cmd;
  bool found;

  found  = FALSE;
  for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
  {
    if ( command[0] == social_table[cmd].name[0]
     &&   !str_prefix( command, social_table[cmd].name ) )
    {
      found = TRUE;
      break;
    }
  }

  if ( !found )
  {
    stc("Ась?\n\r",ch);
    return;
  }

  if (IS_NPC(ch) && IS_SET(social_table[cmd].flag,SOC_NOMOB))
  {
    do_function(ch,&do_emote,"озадаченно вертит головой, не понимая приказа.");
    return;
  }

  if ( !IS_SET(ch->talk, CBIT_GSOCIAL) || IS_SET(social_table[cmd].flag, SOC_NOGLOBAL))
  {
    stc( "Ты не можешь этого сделать!\n\r", ch );
    return;
  }

  if (IS_SET(social_table[cmd].flag,SOC_CULTURAL))
  {
    if (ch->move<5)
    {
      stc("Ты слишком устал.\n\r",ch);
      return;
    }
    ch->move-=5;
  }

  switch ( ch->position )
  {
    case POS_DEAD:
    case POS_INCAP:
    case POS_MORTAL:
    case POS_STUNNED:
    case POS_SLEEPING:
     if ( !str_cmp( social_table[cmd].name, "snore" ) ) break;
     stc( "Ты же спишь!!\n\r", ch );
     return;
  }

  victim = NULL;
  if (EMPTY(argument))
  {
    do_printf(buf,"{cGlobal:{x%s",social_table[cmd].others_no_arg);
    act( buf, ch, NULL, victim, TO_ALL );
    do_printf(buf,"{cGlobal:{x%s",social_table[cmd].char_no_arg);
    act( buf, ch, NULL, victim, TO_CHAR    ); 
    return;
  }

  victim = get_pchar_world(ch, argument);
  if (victim==NULL || is_exact_name(ch->name,victim->pcdata->ignorelist))
  {
    stc("Тут таких нет.\n\r", ch);
    return;
  }

  if ( victim == ch )
  {
    do_printf(buf,"{cGlobal:{x%s",social_table[cmd].others_auto);
    act(buf, ch, NULL, victim, TO_ALL);
    do_printf(buf,"{cGlobal:{x%s",social_table[cmd].char_auto);
    act(buf, ch, NULL, victim, TO_CHAR);
  }
  else
  {
    do_printf(buf,"{cGlobal:{x%s",social_table[cmd].others_found);
    act(buf, ch, NULL, victim, TO_ALL);

    do_printf(buf,"{cGlobal:{x%s",social_table[cmd].char_found);
    act(buf, ch, NULL, victim, TO_CHAR );
    if (IS_SET(victim->talk,CBIT_GSOCIAL))
    {
      do_printf(buf,"{cGlobal:{x%s",social_table[cmd].vict_found);
      act(buf, ch, NULL, victim, TO_VICT );
    }
  }
}

// New version of act with Spam protection. (C) Saboteur
void do_act( const char *format, CHAR_DATA *ch, const void *arg1,
              const void *arg2, int type, int min_pos, int spam )
{
  static char * const he_she   [] = { "it",  "he",  "she" };
  static char * const him_her  [] = { "it",  "him", "her" };
  static char * const his_her  [] = { "its", "his", "her" };
  static char * const on_ona   [] = { "оно", "он",  "она" };
  static char * const ego_ee   [] = { "его", "его", "ее" };
  static char * const emu_ei   [] = { "ему", "ему", "ей" };
  static char * const sam_sama [] = { "сам", "сам", "сама" };
  static char * const samomu_samoj[] = { "самому", "самому", "самой" };
  static char * const nim_nej  [] = { "ним", "ним", "ней" };
  static char * const nemu_nej [] = { "нему", "нему", "ней" };
  static char * const nemu_nee [] = { "нему", "нему", "неe" };
  static char * const sa_as    [] = { "ся", "ся", "ась" };
  static char * const a_a      [] = { "", "", "a" };
  static char * const ij_aja   [] = { "ый", "ый", "ая" };
  static char * const im_oy    [] = { "ым", "ым", "ой" };

  CHAR_DATA           *to;
  CHAR_DATA           *vch = ( CHAR_DATA * ) arg2;
  OBJ_DATA            *obj1 = ( OBJ_DATA  * ) arg1;
  OBJ_DATA            *obj2 = ( OBJ_DATA  * ) arg2;
  const       char    *str;
  char                temp [MAX_STRING_LENGTH];
  const char                *i = NULL;
  char                *point;
  char                *pbuff;
  char                buffer[ MAX_STRING_LENGTH ];
  char                buf[ MAX_STRING_LENGTH   ];
  // bool                fColour = FALSE;

  // Discard null and zero-length messages.
  if( !format || !*format ) return;

  // discard null rooms and chars
  if( !ch || !ch->in_room ) return;

  if( type == TO_VICT && (!vch || !vch->in_room))
  {
    bug( "Act: null vch with TO_VICT.", 0 );
    return;
  }

  to = char_list;

  for( ; to ; to = to->next )
  {
    if (!IS_NPC(to) && to->desc == NULL ) continue;
    if (IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT)) continue;
    if (to->position < min_pos) continue;
    if (to->desc && to->desc->connected!=CON_PLAYING) continue;

    if (type == TO_CHAR && to != ch) continue;
    if (type == TO_VICT && (to!=vch || to==ch)) continue;
    if (type == TO_ROOM && (to==ch || ch->in_room!=to->in_room)) continue;
    if (type == TO_NOTVICT && (to==ch || to==vch || ch->in_room != to->in_room)) continue;
    if (type == TO_ALL && (to==ch || to==vch)) continue;
    if (type == TO_ALL_IN_ROOM && (ch->in_room!=to->in_room)) continue;
    if (IS_SET(to->spam,spam)) continue;

    point   = buf;
    str     = format;
    while( *str != '\0' )
    {
      if( *str != '$' )
      {
        *point++ = *str++;
        continue;
      }

      // fColour = TRUE;
      ++str;
      i = " <   > ";
      if( !arg2 && *str >= 'A' && *str <= 'Z' )
      {
        bug( "Act: missing arg2 for code %d.", *str );
        i = " <   > ";
      }
      else
      {
        switch (*str)
        {
          default:  bug ( "act_output: unknown code",0);
                    i = "<   >";
                    break;
                    if ( *str=='o' || *str=='O' || *str=='c' || *str=='C' )
                    if ( str[1]< '1' || str[1]> '8' )
                    {
                     bug ( "act_output: unknown option.",0);
                     continue;
                    }
          case 't': i = (char *) arg1;                          break;
          case 'T': i = (char *) arg2;                          break;
          case 'n': i = PERS( ch,  to  );                       break;
          case 'N': i = PERS( vch, to  );                       break;
          case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];      break;
          case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];      break;
          case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];      break;
          case 'M': i = him_her [URANGE(0, vch ->sex, 2)];      break;
          case 's': i = his_her [URANGE(0, ch  ->sex, 2)];      break;
          case 'S': i = his_her [URANGE(0, vch ->sex, 2)];      break;
          case 'o': i = on_ona  [URANGE(0, ch  ->sex, 2)];      break;
          case 'O': i = on_ona  [URANGE(0, vch ->sex, 2)];      break;
          case 'g': i = ego_ee  [URANGE(0, ch  ->sex, 2)];      break;
          case 'G': i = ego_ee  [URANGE(0, vch ->sex, 2)];      break;
          case 'u': i = emu_ei  [URANGE(0, ch  ->sex, 2)];      break;
          case 'U': i = emu_ei  [URANGE(0, vch ->sex, 2)];      break;
          case 'j': i = sam_sama  [URANGE(0, ch  ->sex, 2)];      break;
          case 'J': i = sam_sama  [URANGE(0, vch ->sex, 2)];      break;
          case 'l': i = samomu_samoj  [URANGE(0, ch  ->sex, 2)];      break;
          case 'L': i = samomu_samoj  [URANGE(0, vch ->sex, 2)];      break;
          case 'f': i = nim_nej  [URANGE(0, ch  ->sex, 2)];      break;
          case 'F': i = nim_nej  [URANGE(0, vch ->sex, 2)];      break;
          case 'a': i = nemu_nej  [URANGE(0, ch  ->sex, 2)];      break;
          case 'A': i = nemu_nej  [URANGE(0, vch ->sex, 2)];      break;
          case 'q': i = nemu_nee  [URANGE(0, ch  ->sex, 2)];      break;
          case 'Q': i = nemu_nee  [URANGE(0, vch ->sex, 2)];      break;
          case 'r': i = a_a  [URANGE(0, ch  ->sex, 2)];      break;
          case 'R': i = a_a  [URANGE(0, vch ->sex, 2)];      break;
          case 'z': i = sa_as  [URANGE(0, ch  ->sex, 2)];      break;
          case 'Z': i = sa_as  [URANGE(0, vch ->sex, 2)];      break;
          case 'y': i = ij_aja  [URANGE(0, ch  ->sex, 2)];      break;
          case 'Y': i = ij_aja  [URANGE(0, vch ->sex, 2)];      break;
          case 'b': i = im_oy  [URANGE(0, vch ->sex, 2)];      break;
          case 'B': i = im_oy  [URANGE(0, vch ->sex, 2)];      break;
          case '$': strcpy (temp, "$"); i = temp; break;
          case 'p': i = act_parse_obj (temp, to, obj1, '1'); break;
          case 'P': i = act_parse_obj (temp, to, obj2, '1'); break;
          case 'd': if (arg2==NULL||((char *) arg2)[0] == '\0' ) i = "дверь";
                    else {
                      one_argument( (char *) arg2, temp );
                      i = temp;
                    }
                    break;
          case 'w': str = act_ending (temp, ch->sex, str); i = temp; break;
          case 'W': str = act_ending (temp, vch->sex, str); i = temp; break;
          case 'i': i = act_parse_obj (temp, to, obj1, str[1]); str++; break;
          case 'I': i = act_parse_obj (temp, to, obj2, str[1]); str++; break;
          case 'c': i = act_parse_name (temp, ch, to, str[1]); str++; break;
          case 'C': i = act_parse_name (temp, vch, to, str[1]); str++; break;
        }
      }
      str++;
      if (i)
      {
        while (*i != '\0')
        *point++ = *i++;
      }
    }
    *point++ = '{';
    *point++ = 'x';
    *point++ = '\n';
    *point++ = '\r';
    *point   = '\0';
    buf[0]   = UPPER(buf[0]);
    pbuff    = buffer;
    colourconv( pbuff, buf, to );
    if (to->desc != NULL) write_to_buffer( to->desc, buffer, 0 );
    else if ( MOBtrigger ) mp_act_trigger( buf, to, ch, arg1, arg2, TRIG_ACT );
  }
}


void talk( CHAR_DATA *ch, const char *argument, int channel )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];

  if (EMPTY(argument)) return;
  if (!talk_table[channel].npc && IS_NPC(ch)) return;
  if (IS_SET(ch->act,PLR_TIPSY)) tipsy2(ch,"chat", (char*)argument); 

  if (ch->level < talk_table[channel].level && ch->remort==0)
  {
    stc("Ты слишком мал. Используй комманду {WNEWBIECHAT{x.\n\r",ch);
    return;
  }
  if ( channel!=CHAN_CLAN && IS_SET(ch->comm,COMM_NOCHANNELS))
  {
    stc("Боги запрещают тебе пользоваться каналами.\n\r",ch);
    return;
  }
  if (IS_SET(ch->comm,COMM_QUIET))
  {
    stc("Сперва выключи режим тишины(quiet).\n\r",ch);
    return;
  }
  if (!IS_SET(ch->talk,talk_table[channel].bit))
  {
    stc("Этот канал у тебя выключен.\n\r",ch);
    return;
  }
  if (channel!=CHAN_PTALK && channel!=CHAN_YELL && channel!=CHAN_GTELL && !IS_NPC(ch) && IS_SET(ch->act,PLR_ARMY))
  {
    stc("Разговорчики в строю!\n\r",ch);
    return;
  }
  if (channel==CHAN_ALLI && (!ch->clan || IS_SET(ch->clan->flag, CLAN_LONER)))
  {
    stc("У тебя нет союзников.\n\r",ch);
    return;
  }
  if (channel==CHAN_CLAN && (!ch->clan || IS_SET(ch->clan->flag, CLAN_LONER)))
  {
    stc("Ты не в клане.\n\r",ch);
    return;
  }
  if (channel==CHAN_KAZAD && !GUILD(ch,DWARVES_GUILD))
  {
    stc("Ась?\n\r",ch);
    return;
  }
  if (channel==CHAN_AVENGE && !GUILD(ch,ASSASIN_GUILD))
  {
    stc("Ась?\n\r",ch);
    return;
  }

  ptc(ch,talk_table[channel].sstring,talk_table[channel].yourname ? get_char_desc(ch,'1'):"Ты",argument);
  for ( victim=char_list;victim; victim=victim->next)
  {
    if (!victim->desc || victim->desc->connected!=CON_PLAYING) continue;
    if (ch==victim) continue;
    if (!IS_SET(victim->talk,talk_table[channel].bit)) continue;
    if (is_affected(victim,skill_lookup("deaf"))) continue;
    if (!IS_NPC(victim) && is_exact_name(ch->name,victim->pcdata->ignorelist)) continue;
    switch (channel)
    {
      case CHAN_GTELL:
       if (!is_same_group( ch, victim )) continue;
       break;
      case CHAN_YELL:
       if (!ch->in_room || ch->in_room->area!=victim->in_room->area) continue;
       break;
      case CHAN_PTALK:
       if (IS_NPC(victim) || !ch->pcdata->marry || str_cmp(ch->pcdata->marry,victim->name)) continue;
       break;
      case CHAN_ALLI:
       if (!victim->clan || (!is_exact_name(ch->clan->name, victim->clan->alli)
           && !is_same_clan(ch,victim))) continue;
       break;
      case CHAN_CLAN:
       if (!is_same_clan(ch,victim) || ch==victim) continue;
       break;
      case CHAN_KAZAD:
       if (!GUILD(victim,DWARVES_GUILD)) continue;
       break;
      case CHAN_AVENGE:
       if (!GUILD(victim,ASSASIN_GUILD)) continue;
       break;
    }
    smash_beep( (char*)argument,0 );
    if (!victim->desc && (!IS_NPC(victim) && !IS_SET(victim->pcdata->comm_save,talk_table[channel].bit)))
    {
       //Replace Imm's name with it's pseudoname when use channels
       if ( IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) && ch->pcdata->pseudoname[0] != '\0' 
            && channel != CHAN_CHAT )
        {
          do_printf(buf,talk_table[channel].string,ch->pcdata->pseudoname,argument);
          add_buf(victim->pcdata->buffer,buf);
        }
       else
       { 
         do_printf(buf,talk_table[channel].string,get_char_desc(ch,'1'),argument);
         add_buf(victim->pcdata->buffer,buf);
       }
    }
     else if ( IS_IMMORTAL(ch) && !IS_IMMORTAL(victim)
       && ch->pcdata->pseudoname && channel != CHAN_CHAT )
       ptc(victim,talk_table[channel].string,ch->pcdata->pseudoname,argument);
     else
       ptc(victim,talk_table[channel].string,get_char_desc(ch,'1'),argument);
  }
}

void tell(CHAR_DATA *ch,CHAR_DATA *victim, const char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (!IS_IMMORTAL(ch) && ch->in_room->area!=victim->in_room->area &&
     (IS_SET(victim->act,PLR_ARMY) || IS_SET(ch->act,PLR_ARMY)))
  {
    stc( "Гражданские могут общаться с солдатами только в комнате свиданий.\n\r",ch);
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) tipsy2(ch,"chat", (char*)argument);
  smash_beep( (char*)argument,0 );

  if ( victim->desc == NULL && !IS_NPC(victim))
  {
    act("{y$N{x, похоже, потерял$R связь...попробуй позже.",ch,NULL,victim,TO_CHAR);
    do_printf(buf,"{y%s{x говоpит тебе '{G%s{x'\n\r",PERS(ch,victim),argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer,buf);
    return;
  }

  if (!IS_AWAKE(victim) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
  {
    act( "$O не слышит тебя.", ch, 0, victim, TO_CHAR );
    return;
  }

  if (is_affected(victim,skill_lookup("deaf")) && !IS_IMMORTAL(ch))
  {
    act( "$O не слышит тебя.", ch, 0, victim, TO_CHAR );
    return;
  }
  
  if (IS_SET(victim->comm,COMM_QUIET) && !IS_IMMORTAL(ch))
  {
    act( "$O не слышит разговоров.", ch, 0, victim, TO_CHAR );
    return;
  }

  if (IS_SET(victim->comm,COMM_AFK))
  {
    if (IS_NPC(victim))
    {
      act("$O в AFK режиме, и не слышит разговоров.",ch,NULL,victim,TO_CHAR);
      return;
    }
    act("$O в AFK режиме, но $O прочитает твое сообщение, когда вернется.",
            ch,NULL,victim,TO_CHAR);
    do_printf(buf,"%s говоpит тебе '{G%s{x'\n\r",PERS(ch,victim),argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer,buf);
    return;
  }
  act_new("Ты говоpишь $N '{G$t{x'", ch,argument,victim,TO_CHAR,POS_DEAD);

  if (IS_IMMORTAL(victim) && is_exact_name(ch->name,victim->pcdata->ignorelist)) return;
  else
  {
    //Replace Imm's name with it's pseudoname when tell
    if (IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) && ch->pcdata->pseudoname)
    { 
      ptc(victim,"%s говоpит тебе '{G%s{x'\n\r",ch->pcdata->pseudoname,argument);
      victim->reply = ch;
    }
    else 
    {
      act_new("$n говоpит тебе '{G$t{x'",ch,argument,victim,TO_VICT,POS_DEAD);
      victim->reply       = ch;
    }
  }
}

void do_delete( CHAR_DATA *ch, const char *argument)
{
  char strsave[MAX_INPUT_LENGTH];

  if( IS_CREATOR(ch) )
  {
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

    one_argument(argument,arg);
    if(EMPTY(arg))
    {
      stc("Кого ты собираешься удалить?...\n\r",ch);
      return;
    }
    else if ((victim = get_char_world(ch,arg)) == NULL)
    {
      stc("Здесь таких нет.\n\r", ch);
      return;
    }

    if(IS_IMMORTAL(ch) && get_trust(ch) > get_trust(victim))
    {
      save_one_char(victim,SAVE_DELETED);
      save_char_obj(victim);
      victim->pcdata->confirm_delete = TRUE;
      stop_fighting(victim,TRUE);
      dec_worship(victim);
      sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
      do_printf(buf,"\n\r{Y%s{x удалил персонажа {R%s{x.\n\r",ch->name,victim->name);
      unlink(strsave);
      if (victim->desc) close_socket(victim->desc);
      extract_char(victim,TRUE);
      send_note("{RSystem{x","elder","Character was deleted.",buf,2);
      return;
    }
  }

  if (IS_CFG(ch,CFG_NODELETE))
  {
    stc("Боги не разрешают тебе удалить своего персонажа...\n\r",ch);
    stc("Может сначала отдoхнешь, поспишь.. выпьешь пепси..:)) \n\r",ch);
    return;
  }
  if (ch->pcdata->confirm_delete)
  {
    if (!EMPTY(argument))
    {
      stc("Статус подтверждения удаления персонажа снят.\n\r",ch);
      ch->pcdata->confirm_delete = FALSE;
      WAIT_STATE(ch,20);
      return;
    }
    if (get_age(ch)>25) save_one_char(ch,SAVE_DELETED);
    wiznet("$N удалил$R себя",ch,NULL,0,0);
    stop_fighting(ch,TRUE);
    dec_worship(ch);
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    unlink(strsave);
    if (ch->desc) close_socket(ch->desc);
    extract_char(ch,TRUE);
    return;
  }

  if (!EMPTY(argument))
  {
    stc("Просто набери delete без аргументов.\n\r",ch);
    return;
  }
  stc("Набери {rdelete{x еще раз для подтверждения.\n\r",ch);
  stc("ВНИМАНИЕ:эта команда полностью удалит этот персонаж!\n\r",ch);
  stc("Наберите {rdelete{x с аргументом для снятия статуса подтверждения команды.\n\r", ch);
  save_char_obj(ch);
  ch->pcdata->confirm_delete = TRUE;
  WAIT_STATE(ch,2 * PULSE_VIOLENCE);
  wiznet("$N собирается удалить свой персонаж.",ch,NULL,0,get_trust(ch));
}
            
void do_backup( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *keeper;
  DESCRIPTOR_DATA d;

  if ( EMPTY(argument) || !IS_IMMORTAL(ch)) {
    for ( keeper=ch->in_room->people; keeper; keeper=keeper->next_in_room ) {
      if (IS_NPC(keeper) && IS_SET(keeper->act,ACT_IS_KEEPER)) {
        save_one_char( ch, SAVE_BACKUP );
        do_function(keeper, &do_emote, "открывает один из толстых томов, лежащих на столе.");
        do_function(keeper, &do_emote, "долго водит пером, записывая всю информацию.");
        WAIT_STATE(ch,4 * PULSE_VIOLENCE);
        do_function(keeper, &do_emote, "произносит '{GГотово!{x'.");
        return;
      }
    }
    stc("Тут нет {wХранителя Архивов{x...\n\r",ch);
    return;
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (!str_cmp(arg1, "show"))
  {
    char time_buf[25];
    do_printf( buf, "%s%s", PLAYER_DIR2, capitalize( arg2 ) );

    if (load_char_obj(&d, arg2, SAVE_BACKUP)) {
      ptc(ch, "В архиве найден следующий персонаж: %s\n\r", d.character->name);
      strftime(time_buf,25,"%y%m%d %a %H:%M:%S:",localtime(&d.character->lastlogin));
      ptc(ch, "{WВ последний раз этот персонаж заходил {Y%s{W{x\n\r",time_buf);
      ptc(ch, "Уровень %d,  QuestPoints: %d\n\r",d.character->level, d.character->questpoints);
      extract_char(d.character, TRUE);
      return;
    }
    stc("Этого персонажа нет в архивах.\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "restore")) {
    CHAR_DATA *wch;

    if ((wch=get_pchar_world(ch, arg2))) {
      ptc(ch, "Персонаж %s сейчас в мире.\n\r",wch->name);
      return;
    }

    do_printf( buf, "%s%s", PLAYER_DIR2, capitalize( arg2 ) );

    if (load_char_obj(&d, arg2, SAVE_BACKUP)) {
      if (get_trust(d.character)>109 && get_trust(ch)<110) {
        ptc(ch,"{RAccess Denied.{x\n\r");
        extract_char(d.character, TRUE);
        return;
      }

      save_one_char(d.character, SAVE_NORMAL);
      ptc(ch, "Персонаж восстановлен из архива.\n\r", d.character->name);
      extract_char(d.character, TRUE);
      return;
    }
    stc("Этого персонажа нет в архивах.\n\r", ch);
    return;
  }
  stc("Syntax: backup show    <name>\n\r", ch);
  stc("        backup restore <name>\n\r", ch);
}

void do_quit( CHAR_DATA *ch, const char *argument )
{
  AFFECT_DATA *paf;
  DESCRIPTOR_DATA *d,*d_next;
  int id;
  char buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *room;

  if (!ch) return;
  save_char_obj(ch);
  
  room = (ch->in_room) ? ch->in_room : ch->was_in_room;
  
  if (room==NULL) {
    bug("Char in NULL room",0);
    room = get_room_index(ROOM_VNUM_ALTAR);
    char_to_room(ch,room);
  }

  if (room!=NULL && IS_SET(room->room_flags,ROOM_ARENA)) {
   stc( "Боги не позволяют тебе покидать игру в этой комнате.\n\r", ch );
   return;
  }
  
  if ( ch->pcdata->tournament ) {
      stc("Сначала турнир должен закончиться.\n\r",ch);
      return;
  }

  if ( ch->position == POS_FIGHTING ) {
    stc( "Сначала битва должна закончиться.\n\r", ch );
    return;
  }

  if (ch->pcdata->condition[COND_ADRENOLIN]!=0 && ch->level<102) {
    stc( "{rТы слишком взволнован.{x\n\r", ch );
    return;
  }

  if (ch->position < POS_STUNNED) {
    stc( "Ты еще не ТРУП.\n\r", ch );
    return;
  }

  if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller)) ) {
    stc ("Сначала аукцион должен закончиться.\n\r",ch);
    return;
  }
  
  if (is_affected(ch,gsn_sleep)) {
    stc ("Ты спишь нездоровым сном...\n\r",ch);
    return;
  }

  if (IS_AFFECTED(ch,AFF_CHARM)) {
    stc ("Ты не хочешь покидать своего лучшего друга...\n\r",ch);
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"quit")) return;
   
  for (paf = ch->affected; paf != NULL; paf = paf->next) {                       
    if ( !str_cmp(skill_table[paf->type].name, "aid")) {
          ch->hit=UMAX(1,ch->hit - paf->modifier);
          ch->mana=UMAX(1,ch->mana - paf->modifier);
          affect_strip(ch,skill_lookup("aid"));
          break;
       }
  }

  if (IS_SET(ch->act, PLR_QUESTOR)) 
    ptc(ch, "Вы прерываете ваш квест и теряете {R%d{x quest points.\n\r", cancel_quest(ch, TRUE, 16,25));
  stc("Все хорошее рано или поздно заканчивается.\n\r",ch);
  act( "{y$n{x покинул$r игру.", ch, NULL, NULL, TO_ROOM );
  log_printf("%s has quit.", ch->name );
  wiznet("$N покинул$R игру.",ch,NULL,WIZ_LOGINS,get_trust(ch));

  if (ch->level>101) info (ch,ch->level,1,ch->name,"покинул игру.");
  else {
    do_printf(buf,"Весь мир скорбит об уходе {y$c2{x.");
    if (ch->level<90 ) do_printf(buf,"Протяжный звон возвещает об уходе {y$c2{x." );
    if (ch->level<60 ) do_printf(buf,"Протяжный звон возвещает об уходе {y$c2{x." );
    if (ch->level<40 ) do_printf(buf,"Могущественн$y {y$n{x неспеша покидает этот мир.");
    if (ch->level<25 ) do_printf(buf,"{y$n{x покинул$r этот мир.");

    for ( d = descriptor_list; d != NULL; d = d->next ) {
       if ( d->character && d->connected == CON_PLAYING
         && d->character != ch && ch->invis_level <= get_trust(d->character))
         act_new(buf,ch,argument,d->character,TO_VICT,POS_SLEEPING);
    }
  }

  // After extract_char the ch is no longer valid!
  if (ch->pcdata->confirm_delete) ch->pcdata->confirm_delete=FALSE;
  if (!ch->in_room || ch->in_room==get_room_index(ROOM_VNUM_LIMBO)) {
    char_from_room( ch );
    if (!ch->was_in_room || ch->was_in_room == get_room_index(ROOM_VNUM_LIMBO))
         char_to_room( ch, get_room_index(ROOM_VNUM_ALTAR));
    else char_to_room( ch, ch->was_in_room);
    ch->was_in_room=NULL;
  }
  if (room->area->clan && strcmp(room->area->clan,"none")) {
    if (ch->clan==NULL || str_cmp(ch->clan->name,room->area->clan)) {
      char_from_room(ch);
      char_to_room(ch,get_room_index( ROOM_VNUM_TEMPLE ));
    }
  }
  save_char_obj( ch );
  id = ch->id;
  extract_char( ch, TRUE );
  if (ch->desc) close_socket( ch->desc );

  for (d = descriptor_list; d != NULL; d = d_next) {
    d_next = d->next;
    if (d->character && d->character->id == id) {
      close_socket(d);
      extract_char(d->character,TRUE);
    } 
  }
}

void do_save( CHAR_DATA *ch, const char *argument )
{
  stc("{WХранители Знаний Забытого Подземелья записали сведенья о тебе в свои манускрипты, дабы твое имя не было забыто{x.\n\r", ch);
  WAIT_STATE(ch,1 * PULSE_VIOLENCE);
  save_char_obj( ch );
}

void do_follow( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if (EMPTY(arg)) {
    stc( "Следовать за кем?\n\r", ch );
    return;
  }
  if ( ( victim=get_char_room(ch,arg) ) == NULL ) {
    stc( "Tут таких нет.\n\r", ch );
    return;
  }
  if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL ) {
    act( "Но ведь ты уже следуешь за $N!", ch, NULL, ch->master, TO_CHAR );
    return;
  }
  if ( victim == ch )
  {
    if ( ch->master == NULL )
    {
      stc( "Ты и так следуешь за собой.\n\r", ch );
      return;
    }
    stop_follower(ch);
    return;
  }
  if (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOFOLLOW) && !IS_IMMORTAL(ch)) {
    act("$N запрещает следовать за собой.\n\r",ch,NULL,victim, TO_CHAR);
    return;
  }
  if ( ch->master != NULL ) stop_follower( ch );
  add_follower( ch, victim );
}

void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
  if ( ch->master != NULL ) {
    bug( "Add_follower: non-null master.", 0 );
    return;
  }
  ch->master        = master;
  ch->leader        = NULL;

  if (can_see(master,ch,NOCHECK_LVL)) act( "$n теперь следует за тобой.", ch, NULL, master, TO_VICT );
  act( "Теперь ты следуешь за $N.",  ch, NULL, master, TO_CHAR );
}

void stop_follower( CHAR_DATA *ch )
{
  if ( ch->master == NULL ) {
    bug( "Stop_follower: null master.", 0 );
    return;
  }

  if ( can_see( ch->master, ch,NOCHECK_LVL ) && ch->in_room != NULL) {
    act("$n больше не следует за тобой.", ch, NULL, ch->master, TO_VICT);
    act("Ты больше не следуешь за $N.", ch, NULL, ch->master, TO_CHAR);
  }
  if (ch->master->pet == ch) ch->master->pet = NULL;
  ch->master = NULL;
  ch->leader = NULL;
  if ( IS_SET(ch->affected_by, AFF_CHARM)) REM_BIT(ch->affected_by, AFF_CHARM);
}

void do_nuke( CHAR_DATA *ch, const char *argument )
{
  nuke_pets(ch);
}

void nuke_pets( CHAR_DATA *ch )
{    
  CHAR_DATA *pet;

  if ((pet = ch->pet) != NULL) {
    stop_follower(pet);
    if (pet->in_room != NULL) act("$N медленно уходит прочь.",ch,NULL,pet,TO_NOTVICT);
    extract_char(pet,TRUE);
  }
  ch->pet = NULL;
}

void die_follower( CHAR_DATA *ch )
{
  CHAR_DATA *fch;

  if ( ch->master != NULL ) {
    if (ch->master->pet == ch) ch->master->pet = NULL;
    stop_follower( ch );
  }

  ch->leader = NULL;
  for ( fch = char_list; fch != NULL; fch = fch->next ) {
    if ( fch->master == ch ) stop_follower( fch );
    if ( fch->leader == ch ) fch->leader = fch;
  }
}

void do_order( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *och;
  CHAR_DATA *och_next;
  bool found;
  bool fAll;
  int cmd,trust;
  struct cmd_type *cmd_ptr = NULL;
  
  argument = one_argument( argument, arg );
  one_argument(argument,arg2);

  trust=ch->trust; // I think so (c) Jasana

  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ ) {
    if ( arg2[0] == cmd_table[cmd].name[0]
      && (( !IS_SET(cmd_table[cmd].flag, FULL) && !str_prefix(arg2, cmd_table[cmd].name))
      || ( IS_SET(cmd_table[cmd].flag, FULL) && !str_cmp( arg2, cmd_table[cmd].name)))
      && cmd_table[cmd].level <= trust ) {
     cmd_ptr=&cmd_table[cmd];
     break;
    }
  }
  
#ifdef WITH_DSO
  if (!cmd_ptr) {
    struct command *c;
    CMDS_FOREACH (c)
      if (arg2[0] == c->cmd.name[0] && c->cmd.level <= trust &&
        ((c->cm_nice < 100 && !str_prefix (arg2, c->cmd.name))
           || !str_cmp (arg2, c->cmd.name))) {
        cmd_ptr = &c->cmd;
        break;
      }
    }
#endif
  
  if (cmd_ptr && (IS_SET(cmd_ptr->flag,NOORDER) || IS_SET(cmd_ptr->flag,NOFORCE))) {
    stc("Это НЕ БУДЕТ выполнено.\n\r",ch);
    return;
  }

  if ( EMPTY(arg) || EMPTY(argument)) {
    stc( "Скомандовать кому? Что?\n\r", ch );
    return;
  }

  if (IS_AFFECTED( ch, AFF_CHARM )) {
    stc( "Ты чувствуешь себя способным только выполнять приказы.\n\r", ch );
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"order")) return;

  if ( !str_cmp( arg, "all" ) ) {
    if (cmd_ptr && IS_SET(cmd_ptr->flag,NOALL)) {
      stc("Это нельзя командовать всем сразу.\n\r",ch);
      return;
    }
    fAll   = TRUE;
    victim = NULL;
  }
  else {
    fAll   = FALSE;
    if ((victim = get_char_room(ch,arg)) == NULL) {
      stc( "Тут таких нет.\n\r", ch );
      return;
    }

    if (victim == ch) {
      stc( "Да, да...прямо сейчас!\n\r", ch );
      return;
    }

    if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch 
      ||  (IS_IMMORTAL(victim) && victim->trust >= ch->trust)) {
      stc( "Сделай это сам!\n\r", ch );
      return;
    }
  }

  found = FALSE;
  for (och = ch->in_room->people; och != NULL; och = och_next) {
    och_next = och->next_in_room;

    if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch && ( fAll || och == victim )) {
      found = TRUE;
      smash_dollar((char*)argument); // may be wrong. Sab.
      do_printf( buf, "$n командует '%s'.", argument );
      act( buf, ch, NULL, och, TO_VICT );
      interpret( och, argument );
    }
  }

  if ( found ) WAIT_STATE(ch,PULSE_VIOLENCE);
  else stc( "У тебя нет попутчиков.\n\r", ch );
}

void do_group( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if (EMPTY(arg))
  {
    CHAR_DATA *gch;
    CHAR_DATA *leader;

    leader = (ch->leader != NULL) ? ch->leader : ch;
    ptc(ch,"Группа %s:\n\r", PERS(leader, ch) );

    for ( gch = char_list; gch != NULL; gch = gch->next ) {
      if ( is_same_group( gch, ch ) ) {
        ptc(ch,"[%3d %s] %-16s %6d/%6d здоровья %6d/%6d маны %6d/%6d движений %6d опыта\n\r",
          gch->level,IS_NPC(gch) ? "Моб" : (gch->remort>0)? "MLT":class_table[gch->class[gch->remort]].who_name,
          capitalize( PERS(gch, ch) ),
          gch->hit,   gch->max_hit,gch->mana,  gch->max_mana,
          gch->move,  gch->max_move,gch->exp);
      }
    }
    return;
  }

  if ( ( victim = get_char_room( ch, arg ) ) == NULL ) {
    stc( "Тут таких нет.\n\r", ch );
    return;
  }

  if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) ) {
    stc( "Но ты уже следуешь за кем-то другим!\n\r", ch );
    return;
  }

  if ( victim->master != ch && ch != victim ) {
    act_new("$N не следует за тобой.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
    return;
  }
    
  if (IS_AFFECTED(victim,AFF_CHARM)) {
    stc("Ты не можешь убрать очарованных монстров из группы.\n\r",ch);
    return;
  }
    
  if (IS_AFFECTED(ch,AFF_CHARM)) {
    act_new("Ты слишком любишь своего хозяина, чтобы покинуть его!",
            ch,NULL,victim,TO_VICT,POS_SLEEPING);
    return;
  }

  if ( is_same_group( victim, ch ) && ch != victim )
  {                                                                               
    victim->leader = NULL;
    act_new("$n исключает $C2 из своей группы.",
            ch,NULL,victim,TO_NOTVICT,POS_RESTING);
    act_new("$n исключает тебя из своей группы.",
            ch,NULL,victim,TO_VICT,POS_SLEEPING);
    act_new("Ты исключаешь $C2 из своей группы.",
            ch,NULL,victim,TO_CHAR,POS_SLEEPING);
    return;
  }

  victim->leader = ch;
  act_new("$N присоединяется к группе $c2.",ch,NULL,victim,TO_NOTVICT,POS_RESTING);
  act_new("Ты присоединяешься к группе $c2.",ch,NULL,victim,TO_VICT,POS_SLEEPING);
  act_new("$N присоединяется к твоей группе.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
}

const struct pose_table_type pose_table [] =
{
  {
    {
      "Ты энергично сопишь.",
      "$n энергично сопит.",
      "Ты чувствуешь единение с Всевышним.",
      "$n растворяется в небесном свете.",
      "Ты показываешь карточные фокусы.",
      "$n показывает карточные фокусы.",
      "Ты демонстрируешь развитую мускулатуру.",
      "$n демонстрирует $g развитую мускулатуру."
    }
  },
  {
    {
      "Ты превращаешься в бабочку, и, взмахивая крыльями, уносишься в небо.",
      "$n превращается в бабочку, и, взмахивая крыльями, уносится в небо.",
      "Ты с отрешенным видом превращаешь вино в воду.",
      "$n с отрешенным видом превращает вино в воду.",
      "Ты ловко двигаешь ушами.",
      "$n ловко двигает ушами.",
      "Ты меланхолично щелкаешь пальцами орехи.",
      "$n меланхолично щелкает пальцами орехи."
    }
  },
  {
    {
      "Сноп голубых искр слетает с твоих пальцев.",
      "Сноп голубых искр слетает с пальцев $c2.",
      "Твое чело окружает нимб.",
      "Светящийся нимб окружает чело $c2.",
      "Ты с легкостью сворачиваешься в узел.",
      "$n с легкостью сворачивается в узел.",
      "Ты скалишь зубы и свирепо рычишь.",
      "$n скалит зубы и свирепо рычит."
    }
  },
  {
    {
      "В твоих глазах пылают красные огоньки.",
      "В глазах $c2 пылают красные огоньки.",
      "Ты произносишь слова мудрости, заставляя окружающих пасть ниц.",
      "$n произносит слова мудрости, заставляя окружающих пасть ниц.",
      "Ты жонглируешь кинжалами, глазами и срезанными кошельками.",
      "$n жонглирует кинжалами, глазами и срезанными кошельками.",
      "Ты лупишь себы по голове, страшно вращая глазами.",
      "$n лупит себя по голове, страшно вращая глазами."
    }
  },
  {
    {
      "Противная зеленая тварь появляется перед тобой, низко кланяется и исчезает.",
      "Противная зеленая тварь появляется перед $n, низко кланяется и исчезает.",
      "Ты в молитве возносишься над землей.",
      "Клубоко погрузившись в молитву, $n возносится над землей.",
      "Ты от скуки воруешь нижнее белье у окружеющих.",
      "$n украл у тебя нижнее белье!",
      "Хрумм....хрумм...вкусная бутылочка...",
      "Хрумм...хрумм...$n методично пережевывает бутылку."
    }
  },
  {
    {
      "Ты превращаешь окружеющих в розовых слоников.",
      "Магия $c4 превращает тебя в розового слоника.",
      "Ангел спускается к тебе, наставляя в деле Веры.",
      "Ангел спускается к $c3 и ты падаешь на колени.",
      "Кубики переворачиваются ... снова выиграш.",
      "Кубики переворачиваются ... и $n снова выиграл$r.",
      "Ты высыпаешь на пол груду ушей, отрезанных у врагов.",
      "$n высыпает на пол груду ушей, отрезанных у врагов."
    }
  },
  {
    {
      "Маленький шарик света танцует между твоими пальцами.",
      "Маленький шарик света танцует между пальцами $c2.",
      "Твое тело светится неземным светом.",
      "Тело $c2 светится неземным светом.",
      "Ты пересчитываешь деньги в кошельках у окружающих.",
      "Проверь кошелек, $n только что пересчитал$r в нем все деньги.",
      "Глава твоей гильдии гордится тобой.",
      "Глава гильдии воинов ставит $c2 в пример молодым."
    }
  },
  {
    {
      "Ты выдыхаешь дым и пламя подобно драконам.",
      "$n выдыхает дым и пламя подобно драконам.",
      "Яркий луч Божественного света падает на тебя.",
      "Яркий луч Божественного света падает на $c4.",
      "Ты пришпиливаешь муху к потолочной балке метательным ножом.",
      "$n пришпиливает муху к потолочной балке метательным ножом.",
      "Ты жонглируешь гранитными валунами.",
      "$n жонглирует гранитными валунами."
    }
  },
  {
    {
      "Ты зажигаешь магический свет своими словами.",
      "$n произносит странные слова, и зажигает магический свет.",
      "Ты даруешь Божественное благословение грешникам.",
      "Ты чувствуешь, как слова $c2 наполняют тебя Благодатью.",
      "Ты извлекаешь монеты из ушей проходящих мимо.",
      "$n извлекает монету из твоего уха.",
      "Арргх...Ты выжимаешь воду из булыжника.",
      "Невероятно...$n выжимает воду из булыжника."
    }
  },
  {
    {
      "Твоя голова исчезает.",
      "Голова $c2 исчезает.",
      "Прикосновение Всевышнего восстанавливает твои силы.",
      "$n обращает взор к небу, и его раны затягиваются на глазах.",
      "Ты перепрыгиваешь через собственную тень.",
      "$n перепрыгивает через собственную тень.",
      "Ты ковыряешься в зубах копьем.",
      "$n ковыряется в зубах копьем."
    }
  },
  {
    {
      "Дух Огня появляется перед тобой, обдувая жаром твое лицо.",
      "Дух Огня появляется перед $n, обдувая жаром твое лицо.",
      "Солнце пробивается сквозь облака тебе в поддержку.",
      "Солнце пробивается сквозь облака в ответ на молитву $c2.",
      "В твоих глазах светится жадность",
      "В глазах $c2 светится жадность.",
      "Ты подметаешь пол своими соперниками",
      "$n шутя подметает тобой пол."
    }
  },
  {
    {
      "Небо меняет оттенок под цвет твоих глаз.",
      "Небо меняет оттенок под цвет глаз $c2.",
      "Океан расступается перед тобой.",
      "Океан расступается перед $n.",
      "Ты с невинным лицом крадешь оружие окружающих.",
      "$n с невинным лицом крадет твое оружие.",
      "Ты мощным ударом кулака ломаешь в щепки огромный дуб.",
      "$n мощным ударом кулака ломает в щепки огромный дуб.",
    }
  },
  {
    {
      "Камни танцуют по твоей комманде.",
      "Камни танцуют по комманде $c2.",
      "Грозовое облако спешит уплыть с твоего пути.",
      "Грозовые облака расступаются, заслышав мягкую поступь $c2.",
      "Глава Гильдии воров покупает тебе пиво.",
      "Глава Гильдии воров покупает пиво $c3.",
      "Полосы брони лопаются под напором твоей мускулатуры.",
      "Полоса брони $c2 лопается под напором $g мускулатуры."
    }
  },
  {
    {
      "Небеса и трава меняют цвет от твоей улыбки.",
      "Небеса и трава меняют цвет, когда $n улыбается.",
      "Ты воскрешаешь умершего своей молитвой.",
      "$n своей молитвой воскрешает умершего.",
      "Ты подсовываешь фейерверк в карман проходящих мимо.",
      "Твой карман взрывается от подложенной $n бомбы.",
      "Ты ударом кулака крушишь скалы.",
      "$n ударом кулака крушит скалы."
    }
  },
  {
    {
      "Одежда на окружающих становится прозрачной по мановению твоей руки.",
      "$n произносит странные слова, и твоя одежда становится прозрачной!",
      "Всевышний откликается на твои молитвы, наполняя сердце Просветлением.",
      "$n обращается с молитвой к Всевышнему, и его глаза загораются небесным огнем.",
      "Ты останавливаешь кинжал в сантиметре от глаза противника.",
      "Кинжал $c2 внезапно оказывается в сантиметре от твоего глаза.",
      "Наемники всего мира приезжают перенять твой опыт сражений.",
      "Наемники всего мира приезжают перенять военный опыт $c2."
    }
  },
  {
    {
      "Тебя поглощает черная дыра.",
      "Черная дыра поглощает $c2.",
      "Святые принимают тебя в свои ряды.",
      "$n возведен$r в ранг святого.",
      "Ты незаметно пробираешься в любой дом.",
      "$n незаметно пробирается в твой дом.",
      "Четыре стихии несут твою колесницу.",
      "Четыре стихии запряжены в колесницу $c2."
    }
  },
  {
    {
      "Твои слова светятся.",
      "Слова $c2 светятся в темноте.",
      "Великий Адрон дает тебе своим жезлом.",
      "Великий Адрон дает $c3 своим жезлом.",
      "Щелк.",
      "Щелк",
      "Адрон просит тебя помочь ему подержать небо.",
      "Адрон просит $c4 помочь ему подержать небо."
    }
  }
};

void do_pose( CHAR_DATA *ch, const char *argument )
{
  int level;
  int pose;

  level = UMIN( ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1 );
  pose  = number_range(0, level);

  act( pose_table[pose].message[2*ch->class[ch->remort]+0], ch, NULL, NULL, TO_CHAR );
  act( pose_table[pose].message[2*ch->class[ch->remort]+1], ch, NULL, NULL, TO_ROOM );
}
