// $Id: interp.c,v 1.132 2004/03/25 14:46:39 wagner Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
#include "magic.h"

DECLARE_SPELL_FUN( spell_null              );
bool fLogAll = FALSE;

struct  cmd_type        cmd_table       [] =
{
 // Common movement command
 { "north",  "север",  "північ", do_north, POS_STANDING,  0,  LOG_NEVER,  NOLOG|FREEZE },
 { "east",   "восток", "схід",   do_east,  POS_STANDING,  0,  LOG_NEVER,  NOLOG|FREEZE },
 { "south",  "юг",     "південь",do_south, POS_STANDING,  0,  LOG_NEVER,  NOLOG|FREEZE },
 { "west",   "запад",  "захід",  do_west,  POS_STANDING,  0,  LOG_NEVER,  NOLOG|FREEZE },
 { "up",     "вверх",  "вгору",  do_up,    POS_STANDING,  0,  LOG_NEVER,  NOLOG|FREEZE },
 { "down",   "вниз",   "вниз",   do_down,  POS_STANDING,  0,  LOG_NEVER,  NOLOG|FREEZE },

 // Common other commands
 { "at",          "ат",          "ат",          do_at,          POS_DEAD,    102,  WIZ_SECURE, NOMOB|HIDE },
 { "auction",     "аукцион",     "аукціон",     do_auction,     POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|NOMOB|SPAM|NOARMY|FREEZE|NOLOG },
 { "account",     "счет",        "рахунок",     do_account,     POS_SLEEPING,  0,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "cast",        "колдовать",   "чаклувати",   do_cast,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOZRIT|NOALL|FREEZE|NOLOG },
 { "chat",        "чат",         "чат",         do_chat,        POS_DEAD,      0,  WIZ_GSPEAKS,SHOW|MORPH|HIDE|SPAM|NOMOB },
 { "censored",    "цензура",     "цензура",     do_censored,    POS_DEAD,      1,  WIZ_GSPEAKS,SHOW|MORPH|HIDE|SPAM|NOMOB|FREEZE },
 { "buy",         "купить",      "купити",      do_buy,         POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|NOLOG },
 { "bounty",      "награда",     "нагорода",    do_bounty,      POS_DEAD,      0,  WIZ_SKILLS, SHOW|MORPH|NOMOB|SPAM|FREEZE|SAVE },
 { "channels",    "каналы",      "канали",      do_channels,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "deaf",        "тишина",      "тиша",        do_deaf,        POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "exits",       "выходы",      "выходи",      do_exits,       POS_RESTING,   0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "get",         "взять",       "взяти",       do_get,         POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|NOLOG },
 { "goto",        "гото",        "гото",        do_goto,        POS_DEAD,    102,  WIZ_SECURE, SHOW|FREEZE },
 { "group",       "группа",      "група",       do_group,       POS_SLEEPING,  0,  LOG_NEVER,  SHOW|HIDE|NOLOG },
 { "guild",       "гильдия",     "гільдія",     do_guild,       POS_DEAD,      1,  WIZ_SECURE, SHOW|MORPH|NOMOB|NOORDER|FREEZE },
 { "hit",         "напасть",     "напасти",     do_kill,        POS_FIGHTING,  0,  WIZ_SKILLS, NOZRIT|FREEZE|NOLOG },
 { "inventory",   "рюкзак",      "рюкзак",      do_inventory,   POS_DEAD,      0,  WIZ_SECURE, SHOW|HIDE|NOLOG|NOMOB },
 { "kill",        "убить",       "вбити",       do_kill,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOZRIT|FREEZE|NOLOG },
 { "look",        "смотреть",    "дивитись",    do_look,        POS_RESTING,   0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOLOG },
 { "leader",      "лидер",       "лідер",       do_leader,      POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|NOMOB|HIDE|NOORDER|FREEZE },
 { "clan",        "склан",       "склан",       do_clantalk,    POS_SLEEPING,  0,  WIZ_SPEAKS, SHOW|MORPH|HIDE|SPAM },
 { "clanwork",    "кланворк",    "кланворк",    do_clanwork,    POS_DEAD,    107,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|FREEZE|NOPUB },
 { "clanrecall",  "кланрекал",   "кланрекал",   do_clan_recall, POS_FIGHTING,  0,  WIZ_SECURE, SHOW|NOADR|FREEZE|SAVE|NOLOG },
 { "sprecal",     "спрекал",     "спрекал",     do_sprecall,    POS_FIGHTING,  0,  WIZ_SECURE, SHOW|NOADR|FREEZE|SAVE|NOLOG|NOMOB },
 { "petition",    "заявление",   "заява",       do_petition,    POS_SLEEPING,  0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE|SAVE },
 { "cleader",     "кланлидер",   "кланлідер",   do_cleader,     POS_DEAD,    107,  WIZ_SECURE, SHOW|OLC|NOMOB|FREEZE },
 { "music",       "петь",        "співати",     do_music,       POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|MORPH|SPAM|NOMOB },
 { "oauction",    "оаукцион",    "оаукціон",    do_oauction,    POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|SPAM|NOMOB },
 { "order",       "приказ",      "наказ",       do_order,       POS_RESTING,   0,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "practice",    "практиковать","практикувати",do_practice,    POS_SLEEPING,  0,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|SAVE|NOLOG },
 { "rest",        "отдыхать",    "відпочити",   do_rest,        POS_SLEEPING,  0,  LOG_NEVER,  SHOW|FREEZE|NOLOG },
 { "sit",         "сесть",       "сісти",       do_sit,         POS_SLEEPING,  0,  LOG_NEVER,  SHOW|FREEZE|NOLOG },
 { "sockets",     "сокет",       "сокет",       do_sockets,     POS_DEAD,    104,  WIZ_SECURE, SHOW|MORPH|NOLOG|OLC|FREEZE|NOPUB|NOMOB },
 { "rape",        "насиловать",  "насилувати",  do_rape,        POS_FIGHTING,  2,  WIZ_SKILLS, SHOW|NOMOB|NOZRIT|SPAM|FREEZE|NOLOG },
 { "mlove",       "секс",        "секс",        do_rape,        POS_RESTING,   2,  WIZ_SPEAKS, SHOW|NOMOB|NOZRIT|SPAM|FREEZE|NOLOG },
 { "stand",       "встать",      "встати",      do_stand,       POS_SLEEPING,  0,  LOG_NEVER,  SHOW|FREEZE|NOLOG },
 { "stopfoll",    "остановить",  "зупинити",    do_stopfoll,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|NOMOB|HIDE|FREEZE|NOLOG },
 { "tell",        "сказать",     "сказати",     do_tell,        POS_RESTING,   0,  WIZ_SPEAKS, SHOW|MORPH|HIDE|SPAM },
 { "unlock",      "отпереть",    "відімкнути",  do_unlock,      POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "wield",       "вооружиться", "вооружиться", do_wear,        POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|SAVE|NOLOG },
 { "wizhelp",     "иммсправка",  "иммсправка",  do_wizhelp,     POS_DEAD,    102,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "spec",        "спец",        "спец",        do_spec,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOMOB|FREEZE|NOLOG },
 { "spam",        "спам",        "спам",        do_spam,        POS_DEAD,      0,  WIZ_CONFIG, SHOW|NOMOB|HIDE|MORPH|NOORDER|NOLOG },
 { "affects",     "аффекты",     "афекти",      do_affects,     POS_DEAD,      0,  LOG_NEVER,  SHOW|NOMOB|MORPH|HIDE|NOLOG },
 { "areas",       "арии",        "арії",        do_areas,       POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "changes",     "изменения",   "зміни",       do_changes,     POS_DEAD,      0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "commands",    "команды",     "команди",     do_commands,    POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "compare",     "сравнить",    "порівняти",   do_compare,     POS_RESTING,   0,  LOG_NEVER,  SHOW|NOMOB|FREEZE|NOLOG },
 { "complain",    "жалоба",      "жалоба",      do_complain,    POS_SLEEPING,  0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "consider",    "определить",  "визначити",   do_consider,    POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|NOLOG },
 { "count",       "сколько",     "скільки",     do_count,       POS_SLEEPING,  0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "credits",     "автора",      "автора",      do_credits,     POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG },
 { "equipment",   "снаряжение",  "спорядження", do_equipment,   POS_DEAD,      0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOLOG },
 { "examine",     "изучить",     "обстежити",   do_examine,     POS_RESTING,   0,  LOG_NEVER,  SHOW|NOMOB|NOLOG },
 { "help",        "справка",     "справка",     do_help,        POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG },
 { "idea",        "идея",        "ідея",        do_idea,        POS_DEAD,      0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "bugreport",   "багрепорт",   "багрепорт",   do_bugreport,   POS_DEAD,      0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "inform",      "информация",  "інформация",  do_inform,      POS_SLEEPING,  0,  LOG_NEVER,  SHOW|HIDE|NOLOG|NOMOB },
 { "motd",        "мотд",        "мотд",        do_motd,        POS_DEAD,      0,  LOG_NEVER,  MORPH|NOMOB|NOLOG },
 { "news",        "новости",     "новини",      do_news,        POS_DEAD,      0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "bulletin",    "бюллетень",   "бюллетень",   do_news,        POS_DEAD,      0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "report",      "рапорт",      "рапорт",      do_report,      POS_RESTING,   0,  WIZ_SECURE, SHOW|NOLOG|FREEZE },
 { "magazine",    "газета",      "газета",      do_newspaper,   POS_DEAD,      0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "run",         "бежать",      "бігти",       do_run,         POS_STANDING,  0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE|NOLOG },
 { "rules",       "правила",     "правила",     do_rules,       POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG },
 { "scan",        "осмотреться", "озирнутися",  do_scan,        POS_RESTING,   0,  WIZ_SECURE, SHOW|HIDE|NOLOG },
 { "oscore",      "остатистика", "остатистика", do_oscore,      POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "online",      "онлайн",      "онлайн",      do_online,      POS_DEAD,      0,  WIZ_SECURE, SHOW|NOMOB|MORPH|HIDE|NOLOG },
 { "score",       "статистика",  "статистика",  do_score,       POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "skills",      "навыки",      "навички",     do_skills,      POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "socials",     "социалы",     "соціали",     do_socials,     POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOZRIT|SPAM|FREEZE|NOLOG },
 { "gsocials",    "глобально",   "глобально",   do_gsocial,     POS_DEAD,      3,  WIZ_GSPEAKS,SHOW|MORPH|SPAM|FREEZE|NOMOB|NOLOG },
 { "sedit",       "седит",       "седит",       do_sedit,       POS_DEAD,    103,  WIZ_SECURE, SHOW|OLC|NOMOB },
 { "spells",      "заклинания",  "заклинання",  do_spells,      POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|NOLOG|NOMOB },
 { "time",        "время",       "час",         do_time,        POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "tournament",  "tournament",  "tournament",  do_tournament,  POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|NOORDER|NOMOB|FREEZE|NOPUB },
 { "weather",     "погода",      "погода",      do_weather,     POS_RESTING,   0,  LOG_NEVER,  SHOW|HIDE|NOLOG|NOMOB },
 { "who",         "кто",         "хто",         do_who,         POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "whois",       "показать",    "хтосаме",     do_whois,       POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "wizlist",     "боги",        "боги",        do_wizlist,     POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOLOG|NOMOB },
 { "worth",       "отчет",       "звіт",        do_worth,       POS_SLEEPING,  0,  WIZ_SECURE, SHOW|HIDE|NOLOG|NOMOB },
 { "charge",      "charge",      "charge",      do_charge,      POS_STANDING,  0,  WIZ_SKILLS, SHOW|NOMOB|NOORDER|NOLOG|FREEZE },

 // Configuration commands
 { "alias",       "алиас",         "аліас",          do_alias,       POS_DEAD,      0,  WIZ_CONFIG, MORPH|HIDE|FULL|NOMOB|NOORDER|FREEZE|SAVE|NOFORCE },
 { "autolist",    "автосписок",    "автосписок",     do_autolist,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "autoassist",  "автопомощь",    "автодопомога",   do_autoassist,  POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|FREEZE|SAVE|NOLOG },
 { "autogold",    "автозолото",    "автозолото",     do_autogold,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "autoloot",    "автоснаряжение","автоспорядження",do_autoloot,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "autosac",     "автожертвы",    "автожертви",     do_autosac,     POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "brief",       "кратко",        "коротко",        do_brief,       POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "colour",      "цвета",         "кольори",        do_colour,      POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "description", "описание",      "опис",           do_description, POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "remove",      "снять",         "зняти",          do_remove,      POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "delete",      "удалить",       "видалити",       do_delete,      POS_STANDING,  0,  WIZ_SECURE, SHOW|FULL|NOMOB|NOORDER|FREEZE|NOFORCE },
 { "remort",      "переродится",   "переродитися",   do_remort,      POS_STANDING,101,  WIZ_SECURE, SHOW|FULL|NOMOB|NOORDER|FREEZE },
 { "nofollow",    "непускать",     "непускати",      do_nofollow,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|NOLOG },
 { "nosummon",    "непризывать",   "непризивати",    do_nosummon,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOORDER|NOLOG },
 { "nodelete",    "ноделете",      "ноделіт",        do_nodelete,    POS_DEAD,    109,  WIZ_PENALT, SHOW|MORPH|HIDE|NOMOB|FULL },
 { "nocancel",    "ноканцел",      "ноканцел",       do_nocancel,    POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOORDER|NOLOG },
 { "nosend",      "непринимать",   "неприймати",     do_nosend,      POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOORDER|FREEZE|NOLOG },
 { "outfit",      "оутфит",        "оутфит",         do_outfit,      POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|SAVE },
 { "offline",     "оффлайн",       "офлайн",         do_offline,     POS_DEAD,      0,  WIZ_SECURE, SHOW|HIDE|NOMOB|OLC|NOPUB },
 { "offer",       "заказ",         "заказ",          do_offer,       POS_DEAD,      1,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE },
 { "password",    "пароль",        "пароль",         do_password,    POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOORDER|SAVE|NOLOG|NOFORCE },
 { "prompt",      "строка",        "строка",         do_prompt,      POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "scroll",      "скроллинг",     "скроллинг",      do_scroll,      POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "title",       "титл",          "тітл",           do_title,       POS_DEAD,      1,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "unalias",     "уналиас",       "уналиас",        do_unalias,     POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|FULL|NOORDER|FREEZE|SAVE },
 { "wimpy",       "трусость",      "боягузтво",      do_wimpy,       POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|FREEZE|NOLOG },

 /*** Communication commands. ***/
 { "afk",         "афк",       "афк",         do_afk,         POS_SLEEPING,  0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|FREEZE|SAVE|NOLOG },
 { "answer",      "ответ",     "відповідь",   do_answer,      POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|SPAM|NOMOB },
 { "emote",       "эмоция",    "эмоція",      do_emote,       POS_RESTING,   0,  WIZ_SPEAKS, SHOW|SPAM|NOZRIT },
 { "pmote",       "пмоте",     "пмоте",       do_pmote,       POS_RESTING,   0,  WIZ_SPEAKS, SHOW|SPAM|NOZRIT },
 { ".",           "кричать",   "кричати",     do_gossip,      POS_SLEEPING,  0,  WIZ_GSPEAKS,SPAM|NOARMY },
 { "gossip",      "кричать",   "кричати",     do_gossip,      POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|SPAM|NOARMY },
 { ",",           "эмоция",    "эмоція",      do_emote,       POS_RESTING,   0,  WIZ_SPEAKS, SPAM|NOZRIT },
 { "newbiechat",  "ньюьбие",   "ньюьбічат",   do_newbiechat,  POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|HIDE|SPAM|NOMOB },
 { "grats",       "поздравить","поздоровити", do_grats,       POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|HIDE|SPAM|NOMOB },
 { "gtell",       "сказгруппе","казатигрупі", do_gtell,       POS_DEAD,      0,  WIZ_SPEAKS, SHOW|HIDE|SPAM },
 { ";",           ";",         ";",           do_gtell,       POS_DEAD,      0,  WIZ_SPEAKS, HIDE|SPAM },
 { "note",        "сообщение", "повідомлення",do_note,        POS_SLEEPING,  0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOORDER|NOLOG },
 { "offtopic",    "сообщение", "офтопік",     do_offtopic,    POS_SLEEPING,  0,  WIZ_NOTES,  SHOW|MORPH|HIDE|NOMOB|NOORDER|NOLOG },
 { "quest",       "задание",   "завдання",    do_quest,       POS_SLEEPING,  0,  WIZ_SECURE, SHOW|NOMOB|HIDE|FREEZE|SAVE|NOLOG },
 { "question",    "вопрос",    "питання",     do_question,    POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|SPAM },
 { "quenia",      "изречь",    "промовити",   do_quenia,      POS_RESTING,   0,  WIZ_SPEAKS, SHOW|FREEZE|NOMOB },
 { "quote",       "цитировать","цитувати",    do_quote,       POS_SLEEPING,  0,  WIZ_GSPEAKS,SHOW|SPAM|NOMOB },
 { "reply",       "ответить",  "відповісти",  do_reply,       POS_SLEEPING,  0,  WIZ_SPEAKS, SHOW|HIDE|SPAM },
 { "replay",      "повторить", "повторити",   do_replay,      POS_SLEEPING,  0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB },
 { "say",         "произнести","вимовити",    do_say,         POS_RESTING,   0,  WIZ_SPEAKS, SHOW|SPAM },
 { "'",           "'",         "'",           do_say,         POS_RESTING,   0,  WIZ_SPEAKS, SPAM },
 { "shout",       "крикнуть",  "крикнути",    do_shout,       POS_RESTING,   3,  WIZ_GSPEAKS,SHOW|SPAM },
 { "alli",        "союзники",  "союзники",    do_alli,        POS_SLEEPING,  0,  WIZ_SPEAKS, SHOW|SPAM|HIDE|NOMOB|MORPH },
 { "yell",        "орать",     "репетувати",  do_yell,        POS_RESTING,   0,  WIZ_SPEAKS, SHOW|SPAM },

 { "showprac",    "показпрак", "показпрак",   do_showprac,    POS_SLEEPING,  0,  LOG_NEVER,  SHOW|HIDE|NOMOB|NOLOG },
 { "showskill",   "показнавык","показнавычки",do_showskill,   POS_DEAD,      0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "unread",      "нечитанных","нечитаних",   do_unread,      POS_SLEEPING,  0,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|NOLOG },

 /*** Object manipulation commands. ***/
 { "brandish",    "взмахнуть", "взмахнути", do_brandish,    POS_RESTING,   0,  WIZ_SKILLS, SHOW|NOZRIT|NOALL|FREEZE|SAVE|NOLOG },
 { "close",       "закрыть",   "зачинити",  do_close,       POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "drink",       "выпить",    "випити",    do_drink,       POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|SAVE|NOLOG },
 { "drop",        "бросить",   "бросити",   do_drop,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|NOLOG },
 { "eat",         "съесть",    "з'їсти",    do_eat,         POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "envenom",     "отравить",  "отравити",  do_envenom,     POS_RESTING,   0,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "fill",        "наполнить", "наповнити", do_fill,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|SAVE|NOLOG },
 { "give",        "дать",      "дати",      do_give,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|NOLOG },
 { "send",        "послать",   "послати",   do_send,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|NOZRIT|FREEZE |NOLOG},
 { "heal",        "лечить",    "лікувати",  do_heal,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|SAVE|NOLOG },
 { "hold",        "одеть",     "вдягнути",  do_wear,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|SAVE|NOLOG },
 { "list",        "список",    "список",    do_list,        POS_RESTING,   0,  WIZ_SECURE, SHOW|HIDE|FREEZE|NOLOG },
 { "lock",        "запереть",  "замкнути",  do_lock,        POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "open",        "открыть",   "відкрити",  do_open,        POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "pick",        "взломать",  "взломати",  do_pick,        POS_RESTING,   0,  WIZ_SKILLS, SHOW|NOZRIT|FREEZE|NOLOG },
 { "pour",        "опорожнить","опорожнити",do_pour,        POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|SAVE|NOLOG },
 { "push",        "пнуть",     "пнути",     do_push,        POS_STANDING,  0,  WIZ_SKILLS, SHOW|NOZRIT|FREEZE|NOLOG },
 { "put",         "положить",  "покласти",  do_put,         POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|NOLOG },
 { "quaff",       "глотнуть",  "глотнути",  do_quaff,       POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|SAVE|NOLOG },
 { "recite",      "прочесть",  "прочитати", do_recite,      POS_RESTING,   0,  WIZ_SKILLS, SHOW|NOZRIT|NOALL|FREEZE|NOLOG },
 { "sell",        "продать",   "продати",   do_sell,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|NOLOG },
 { "take",        "взять",     "взяти",     do_get,         POS_RESTING,   0,  WIZ_SECURE, SHOW|NOZRIT|FREEZE|NOLOG },
 { "talks",       "разговоры", "розмови",   do_talk,        POS_DEAD,      0,  WIZ_CONFIG, SHOW|NOMOB|NOORDER|MORPH|HIDE|FREEZE|SAVE },
 { "pose",        "поза",      "поза",      do_pose,        POS_RESTING,   0,  WIZ_CONFIG, SHOW|NOMOB|NOORDER|MORPH|HIDE|NOLOG },
 { "sacrifice",   "уничтожить","знищити",   do_sacrifice,   POS_RESTING,   0,  WIZ_SACCING,SHOW|NOZRIT|FREEZE|NOLOG },
 { "junk",        "жертвоприношение", "жертвопринесення", do_sacrifice,POS_RESTING,0, WIZ_SACCING,NOZRIT|FREEZE|NOLOG },
 { "tap",         "уничтожить","знищити",   do_sacrifice,   POS_RESTING,   0,  WIZ_SACCING,NOZRIT|FREEZE|NOLOG },
 { "value",       "оценить",   "оцінити",   do_value,       POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|NOLOG },
 { "wear",        "надеть",    "надягнути", do_wear,        POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "zap",         "махнуть",   "махнути",   do_zap,         POS_RESTING,   0,  WIZ_SKILLS, SHOW|NOZRIT|NOALL|FREEZE|NOLOG },
 { "exchange",    "обменять",  "обміняти",  do_exchange,    POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|NOLOG },
 { "clanfit",     "кланобнов", "кланобнов", do_clanfit,     POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|SAVE },
 { "clanbank",    "кланбанк",  "кланбанк",  do_clanbank,    POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|NOORDER|OLC|NOARMY|FREEZE|SAVE },
 { "repair",      "чинить",    "лагодити",  do_repair,      POS_STANDING,  0,  WIZ_SKILLS, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "reward",      "наградить", "нагородити",do_reward,      POS_DEAD,    106,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "damage",      "повредить", "пошкодити", do_damage,      POS_DEAD,    107,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "confiscate",  "забрать",   "забрати",   do_seize,       POS_DEAD,    105,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "cfix",        "cfix",      "cfix",      do_cfix,        POS_DEAD,    109,  WIZ_CONFIG, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "gfix",        "gfix",      "gfix",      do_gfix,        POS_DEAD,    109,  WIZ_CONFIG, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "forge",       "ковать",    "ковати",    do_forge,       POS_STANDING,109,  WIZ_SKILLS, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "ldefend",     "узащита",   "узахист",   do_ldefend,     POS_STANDING,110,  WIZ_SKILLS, SHOW|NOMOB|NOORDER|FREEZE|SAVE },

 // Combat commands
 { "backstab",    "бэкстаб",    "бекстаб",    do_backstab,    POS_STANDING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "bash",        "сбить",      "збити",      do_bash,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "throw",       "метнуть",    "метнути",    do_throw,       POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOZRIT|NOMOB|FREEZE|NOLOG },
 { "bs",          "бс",         "бс",         do_backstab,    POS_FIGHTING,  0,  WIZ_SKILLS, NOALL|FREEZE|NOLOG },
 { "berserk",     "ярость",     "лють",       do_berserk,     POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "cleave",      "разбить",    "розбити",    do_cleave,      POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "dirt",        "ослепить",   "осліпити",   do_dirt,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "disarm",      "обезоружить","обеззброїти",do_disarm,      POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|SAVE|NOLOG },
 { "flee",        "сбежать",    "збігти",     do_flee,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "kick",        "пнуть",      "пнути",      do_kick,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "murder",      "убить",      "вбити",      do_murder,      POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|FULL|HIDE|FREEZE|NOLOG },
 { "rescue",      "спасти",     "врятувати",  do_rescue,      POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOZRIT|FREEZE|NOLOG },
 { "trip",        "подножка",   "підніжка",   do_trip,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "pray",        "молиться",   "молитися",   do_pray,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOORDER|FREEZE|NOLOG },
 { "shock",       "врезать",    "врізати",    do_shock_hit,   POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "strangle",    "придушить",  "придушити",  do_strangle,    POS_STANDING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },
 { "frame",       "подставить", "підставити", do_frame,       POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOALL|FREEZE|NOLOG },

 // Miscellaneous commands
 { "polyanarecall","polyanare",   "полянарекал",    do_polyanarecall,POS_SLEEPING,  0,  LOG_NEVER,  SHOW|HIDE|NOMOB|NOFORCE },
 { "config",      "конфигурация", "конфігурация", do_config,      POS_DEAD,      0,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOORDER|SAVE|NOLOG },
 { "seen",        "сиин",         "сиин",         do_seen,        POS_DEAD,      0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOPUB },
 { "backup",      "архив",        "архів",        do_backup,      POS_DEAD,      0,  WIZ_SECURE, SHOW|FULL|NOMOB|NOORDER|OLC|FREEZE|NOPUB },
 { "backup2",     "архив2",       "архів2",       do_backup2,     POS_DEAD,    102,  WIZ_SECURE, SHOW|FULL|NOMOB|NOORDER|OLC|FREEZE|NOPUB },
 { "blink",       "мерцать",      "мерцати",      do_blink,       POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|SAVE|NOLOG|FREEZE },
 { "enter",       "войти",        "увійти",       do_enter,       POS_STANDING,  0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "follow",      "следовать",    "слідувати",    do_follow,      POS_STANDING,  0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "gain",        "учить",        "вчити",        do_gain,        POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|SAVE },
 { "go",          "зайти",        "зайти",        do_enter,       POS_STANDING,  0,  WIZ_SECURE, NOORDER|FREEZE|NOLOG },
 { "hide",        "спрятаться",   "сховатись",    do_hide,        POS_RESTING,   0,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "play",        "проиграть",    "програти",     do_play,        POS_RESTING,   2,  WIZ_SECURE, SHOW|FREEZE|NOMOB },
 { "mob",         "моб",          "моб",          do_mob,         POS_DEAD,      0,  LOG_NEVER,  NOORDER|FREEZE|NOFORCE },
 { "quit",        "выйти",        "вийти",        do_quit,        POS_DEAD,      0,  WIZ_LOGINS, SHOW|FULL|NOMOB|NOORDER|FREEZE },
 { "quiet",       "глухота",      "глухота",      do_quiet,       POS_DEAD,      0,  WIZ_CONFIG, SHOW|FULL|NOMOB|NOORDER|FREEZE|NOLOG },
 { "recall",      "рекал",        "рекал",        do_recall,      POS_FIGHTING,  0,  WIZ_SECURE, SHOW|FREEZE|SAVE|NOLOG },
 { "/",           "/",            "/",            do_recall,      POS_FIGHTING,  0,  WIZ_SECURE, FREEZE|NOLOG },
 { "save",        "сохранить",    "зберегти",     do_save,        POS_DEAD,      0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE },
 { "sleep",       "спать",        "спати",        do_sleep,       POS_SLEEPING,  0,  WIZ_SECURE, SHOW|FREEZE|SAVE|NOLOG },
 { "sneak",       "красться",     "крастись",     do_sneak,       POS_STANDING,  0,  WIZ_SKILLS, SHOW|HIDE|FREEZE|NOLOG },
 { "split",       "поделить",     "поділити",     do_split,       POS_RESTING,   0,  WIZ_SECURE, SHOW|FREEZE|NOLOG },
 { "steal",       "украсть",      "вкрасти",      do_steal,       POS_STANDING,  0,  WIZ_SKILLS, SHOW|HIDE|FREEZE },
 { "train",       "тренировать",  "тренувати",    do_train,       POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE|NOLOG },
 { "visible",     "видимый",      "видимий",      do_visible,     POS_SLEEPING,  0,  LOG_NEVER,  SHOW|FREEZE|NOLOG },
 { "wake",        "проснуться",   "прокинутись",  do_wake,        POS_SLEEPING,  0,  LOG_NEVER,  SHOW|FREEZE|NOLOG },
 { "where",       "где",          "де",           do_where,       POS_RESTING,   0,  WIZ_SECURE, SHOW|HIDE|NOLOG|NOMOB },
 { "crimereport", "криминал",     "кримінал",     do_crimereport, POS_RESTING,   0,  WIZ_SECURE, SHOW|NOMOB|NOORDER|FREEZE },
 { "ahelp",       "асправка",     "асправка",     do_ahelp,       POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|NOMOB|HIDE|NOLOG },
 
 // Deity commands
 { "devote",      "посвятить", "посвятити", do_devote,      POS_DEAD,     21,  WIZ_SECURE,  MORPH|HIDE|NOMOB|NOORDER|NOFORCE },
 { "deity",       "божить",    "божити",    do_deity,       POS_DEAD,    109,  WIZ_SECURE,  MORPH|HIDE|NOMOB|NOORDER|NOFORCE },
// { "supplicate",  "молить", "молить",   do_supplicate,  POS_STUNNED,   0,  WIZ_SECURE,  MORPH|HIDE|NOMOB|NOORDER|NOFORCE },

 // Immortal commands
 { "player",      "unknown",   "unknown",   do_player,      POS_DEAD,    109,  WIZ_SECURE, MORPH|NOMOB|OLC|NOORDER|NOFORCE },
 { "pseudoname",  "unknown",   "unknown",   do_pseudoname,  POS_DEAD,    102,  WIZ_CONFIG, SHOW|MORPH|SAVE|NOMOB },
 { "index",       "индекс",    "індекс",    do_index,       POS_DEAD,    109,  WIZ_SECURE, MORPH|NOMOB|OLC|NOORDER|NOFORCE },
 { "family",      "семья",     "сім'я",     do_family,      POS_DEAD,    104,  LOG_NEVER,  MORPH|HIDE|NOMOB|NOORDER },
 { "srcwrite",    "код",       "код",       do_srcwrite,    POS_DEAD,    109,  LOG_NEVER,  MORPH|HIDE|NOMOB|NOPUB|NOORDER },
 { "rename",      "ренейм",    "ренейм",    do_rename,      POS_DEAD,    107,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB|NOPUB|NOORDER },
 { "advance",     "адванс",    "адванс",    do_advance,     POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB|NOFORCE|NOORDER },
 { "dump",        "дамп",      "дамп",      do_dump,        POS_DEAD,    109,  WIZ_SECURE, MORPH|NOPUB|NOMOB|NOFORCE },
 { "trust",       "unknown",   "unknown",   do_trust,       POS_DEAD,    109,  WIZ_SECURE, SHOW|MORPH|NOMOB|NOFORCE|NOORDER },
 { "allow",       "unban",     "unban",     do_allow,       POS_DEAD,    106,  WIZ_SECURE, SHOW|MORPH|OLC|NOFORCE },
 { "ban",         "бан",       "бан",       do_ban,         POS_DEAD,    105,  WIZ_SECURE, SHOW|MORPH|OLC|NOPUB|NOFORCE },
 { "pban",        "пбан",      "пбан",      do_pban,        POS_DEAD,    101,  WIZ_SECURE, SHOW|MORPH|OLC|NOPUB|NOMOB|NOFORCE },
 { "deny",        "unknown",   "unknown",   do_deny,        POS_DEAD,    104,  WIZ_PENALT, SHOW|MORPH|OLC },
 { "tipsy",       "unknown",   "unknown",   do_tipsy,       POS_DEAD,    103,  WIZ_PENALT, SHOW|FULL|MORPH|OLC }, // tipsy by Dinger
 { "disconnect",  "unknown",   "unknown",   do_disconnect,  POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|NOMOB },
 { "flag",        "unknown",   "unknown",   do_flag,        POS_DEAD,    109,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB },
 { "freeze",      "unknown",   "unknown",   do_freeze,      POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|OLC },
 { "protect",     "unknown",   "unknown",   do_protect,     POS_DEAD,    109,  WIZ_SECURE, SHOW|MORPH|NOMOB },
 { "reboot",      "unknown",   "unknown",   do_reboot,      POS_DEAD,      0,  WIZ_SECURE, SHOW|FULL|NOMOB|NOORDER|NOFORCE },
 { "set",         "unknown",   "unknown",   do_set,         POS_DEAD,    106,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB|NOORDER|NOFORCE },
 { "sset",        "unknown",   "unknown",   do_sset,        POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB|NOORDER|NOFORCE },
 { "cset",        "unknown",   "unknown",   do_sset,        POS_DEAD,    109,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB|NOORDER|NOFORCE },
 { "force",       "unknown",   "unknown",   do_force,       POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH|OLC|NOMOB|NOFORCE|NOORDER },
 { "load",        "unknown",   "unknown",   do_load,        POS_DEAD,    105,  WIZ_LOAD,   SHOW|MORPH|OLC|NOMOB|NOORDER },
 { "nochannels",  "unknown",   "unknown",   do_nochannels,  POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|OLC|NOMOB },
 { "noemote",     "unknown",   "unknown",   do_noemote,     POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|NOMOB },
 { "nomlove",     "unknown",   "unknown",   do_nomlove,     POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|NOMOB },
 { "nogsocial",   "unknown",   "unknown",   do_nogsocial,   POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|OLC|NOMOB },
 { "nopost",      "unknown",   "unknown",   do_nopost,      POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|NOMOB|OLC },
 { "notell",      "unknown",   "unknown",   do_notell,      POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|OLC|NOMOB },
 { "pecho",       "unknown",   "unknown",   do_pecho,       POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH },
 { "pardon",      "unknown",   "unknown",   do_pardon,      POS_DEAD,    107,  WIZ_SECURE, SHOW|MORPH|OLC },
 { "purge",       "unknown",   "unknown",   do_purge,       POS_DEAD,    106,  WIZ_SECURE, SHOW|OLC },
 { "restore",     "unknown",   "unknown",   do_restore,     POS_DEAD,    103,  WIZ_RESTORE,SHOW|MORPH|OLC },
 { "slay",        "unknown",   "unknown",   do_slay,        POS_DEAD,    103,  WIZ_SECURE, SHOW|FULL },
 { "transfer",    "unknown",   "unknown",   do_transfer,    POS_DEAD,    103,  WIZ_SECURE, SHOW|MORPH|OLC },
 { "poofin",      "unknown",   "unknown",   do_bamfin,      POS_DEAD,    102,  WIZ_CONFIG, SHOW|MORPH|SAVE|NOMOB|NOLOG },
 { "poofout",     "unknown",   "unknown",   do_bamfout,     POS_DEAD,    102,  WIZ_CONFIG, SHOW|MORPH|SAVE|NOMOB|NOLOG },
 { "gecho",       "unknown",   "unknown",   do_echo,        POS_DEAD,    102,  WIZ_GSPEAKS,SHOW|MORPH },
 { "holylight",   "unknown",   "unknown",   do_holylight,   POS_DEAD,    102,  WIZ_CONFIG, SHOW|MORPH|NOMOB|NOLOG },
 { "incognito",   "unknown",   "unknown",   do_incognito,   POS_DEAD,    102,  WIZ_CONFIG, SHOW|MORPH|NOMOB|NOLOG },
 { "log",         "unknown",   "unknown",   do_log,         POS_DEAD,    109,  WIZ_SECURE, SHOW|MORPH|NOMOB|NOFORCE|NOORDER },
 { "memory",      "unknown",   "unknown",   do_memory,      POS_DEAD,    107,  LOG_NEVER,  SHOW|MORPH|NOPUB|NOMOB },
 { "mwhere",      "unknown",   "unknown",   do_mwhere,      POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH|NOMOB },
 { "owhere",      "unknown",   "unknown",   do_owhere,      POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH|NOMOB },
 { "peace",       "unknown",   "unknown",   do_peace,       POS_DEAD,    102,  WIZ_SECURE, SHOW|OLC|NOORDER|NOFORCE },
 { "penalty",     "unknown",   "unknown",   do_penalty,     POS_DEAD,    103,  WIZ_PENALT, SHOW|MORPH|NOMOB },
 { "echo",        "unknown",   "unknown",   do_recho,       POS_DEAD,    102,  WIZ_SPEAKS, SHOW|MORPH },
 { "snoop",       "unknown",   "unknown",   do_snoop,       POS_DEAD,    106,  WIZ_SNOOPS, SHOW|MORPH|OLC|NOPUB|NOMOB },
 { "stat",        "unknown",   "unknown",   do_stat,        POS_DEAD,    103,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB },
 { "string",      "unknown",   "unknown",   do_string,      POS_DEAD,    104,  WIZ_SECURE, SHOW|MORPH|NOORDER },
 { "wizinvis",    "unknown",   "unknown",   do_invis,       POS_DEAD,    102,  WIZ_SECURE, SHOW|MORPH|NOMOB },
 { "vnum",        "unknown",   "unknown",   do_vnum,        POS_DEAD,    103,  LOG_NEVER,  SHOW|MORPH|NOMOB },
 { "zecho",       "unknown",   "unknown",   do_zecho,       POS_DEAD,    102,  WIZ_SPEAKS, SHOW|MORPH },
 { "clone",       "unknown",   "unknown",   do_clone,       POS_DEAD,    109,  WIZ_LOAD,   SHOW|NOPUB|NOMOB },
 { "wiznet",      "unknown",   "unknown",   do_wiznet,      POS_DEAD,    102,  WIZ_CONFIG, SHOW|MORPH|NOMOB },
 { "immtalk",     "unknown",   "unknown",   do_immtalk,     POS_DEAD,      0,  WIZ_GSPEAKS,SHOW|MORPH|NOORDER|HIDE },
 { ":",           "unknown",   "unknown",   do_immtalk,     POS_DEAD,      0,  WIZ_GSPEAKS,MORPH },
 { "smote",       "unknown",   "unknown",   do_smote,       POS_DEAD,    102,  WIZ_SPEAKS, SHOW|MORPH },
 { "prefix",      "unknown",   "unknown",   do_prefix,      POS_DEAD,    102,  LOG_NEVER,  SHOW|MORPH|FULL },
 { "nuke",        "unknown",   "unknown",   do_nuke,        POS_SLEEPING,  0,  WIZ_SECURE, SHOW|NOMOB|FREEZE|SAVE },
 { "vislist",     "unknown",   "unknown",   do_vislist,     POS_DEAD,    102,  WIZ_SKILLS, SHOW|NOMOB|HIDE|NOORDER},
 { "ignorelist",  "unknown",   "unknown",   do_ignorelist,  POS_DEAD,    106,  WIZ_SKILLS, SHOW|NOMOB|HIDE|NOORDER},
 { "setcurse",    "проклять",  "проклясти", do_setcurse,    POS_DEAD,    107,  WIZ_PENALT, SHOW|NOMOB|OLC|FREEZE|NOPUB|NOORDER },

 // Race Skills
 { "tail",        "хвост",     "хвіст",     do_tail,        POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOORDER|FREEZE|NOLOG },
 { "crush",       "толкнуть",  "толкнути",  do_crush,       POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOORDER|FREEZE|NOLOG },
 { "blacksmith",  "перековать","перекувати",do_blacksmith,  POS_STANDING,  0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE },
 { "vbite",       "укус",      "укусити",   do_vbite,       POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOORDER|NOMOB|FREEZE|NOLOG },
 { "resurrect",   "возродить", "возродити", do_resurrect,   POS_FIGHTING,  0,  WIZ_SKILLS, SHOW|NOMOB|FREEZE|NOLOG },

 { "travel",      "путешествовать", "подорожувати",  do_travel,      POS_STANDING,  0,  WIZ_SECURE, NOMOB|FREEZE },

 // OLC
 { "edit",        "unknown",     "unknown",     do_olc,         POS_DEAD,    103,  WIZ_SECURE, SHOW|OLC|NOMOB },
 { "asave",       "unknown",     "unknown",     do_asave,       POS_DEAD,    103,  WIZ_SECURE, SHOW|OLC|NOMOB|NOFORCE },
 { "alist",       "unknown",     "unknown",     do_alist,       POS_DEAD,    103,  LOG_NEVER,  SHOW|NOMOB|NOLOG },
 { "resets",      "unknown",     "unknown",     do_resets,      POS_DEAD,    103,  LOG_NEVER,  SHOW|NOMOB },
 { "redit",       "unknown",     "unknown",     do_redit,       POS_DEAD,    103,  LOG_NEVER,  SHOW|OLC|NOMOB },
 { "medit",       "unknown",     "unknown",     do_medit,       POS_DEAD,    103,  LOG_NEVER,  SHOW|OLC|NOMOB },
 { "aedit",       "unknown",     "unknown",     do_aedit,       POS_DEAD,    103,  LOG_NEVER,  SHOW|OLC|NOMOB },
 { "oedit",       "unknown",     "unknown",     do_oedit,       POS_DEAD,    103,  LOG_NEVER,  SHOW|OLC|NOMOB },
 { "qstat",       "unknown",     "unknown",     do_qstat,       POS_DEAD,    103,  WIZ_SECURE, SHOW|NOMOB|NOLOG },
 { "pcheck",      "проверить",   "перевірити",  do_check,       POS_DEAD,    109,  WIZ_SECURE, NOMOB|OLC|NOPUB|NOORDER|FULL|NOLOG },
 { "skillstat",   "unknown",     "unknown",     do_skillstat,   POS_DEAD,    103,  LOG_NEVER,  SHOW|NOMOB|NOLOG },
 { "spellstat",   "unknown",     "unknown",     do_spellstat,   POS_DEAD,    103,  LOG_NEVER,  SHOW|NOMOB|NOLOG },
 { "arenarecall", "арена",       "арена",       do_arecall,     POS_FIGHTING,  0,  WIZ_SECURE, SHOW|NOADR|NOORDER|SAVE|NOMOB|NOLOG|FREEZE },
 { "addlag",      "unknown",     "unknown",     do_addlag,      POS_DEAD,    103,  WIZ_PENALT, SHOW|OLC|NOPUB|NOMOB },
 { "itemlist",    "unknown",     "unknown",     do_itemlist,    POS_DEAD,    110,  WIZ_SECURE, SHOW|OLC|NOPUB|NOMOB },
 { "moblist",     "unknown",     "unknown",     do_moblist,     POS_DEAD,    110,  WIZ_SECURE, SHOW|OLC|NOPUB|NOMOB },
 { "mpdump",      "unknown",     "unknown",     do_mpdump,      POS_DEAD,    105,  LOG_NEVER,  SHOW|NOMOB },
 { "mpstat",      "unknown",     "unknown",     do_mpstat,      POS_DEAD,    105,  LOG_NEVER,  SHOW|NOMOB|NOLOG },
 { "mpsave",      "unknown",     "unknown",     do_mpsave,      POS_DEAD,    105,  WIZ_SECURE, SHOW|OLC|NOMOB|NOLOG },
 { "mpedit",      "unknown",     "unknown",     do_mpedit,      POS_DEAD,    105,  WIZ_SECURE, SHOW|OLC|NOMOB },
 { "mplist",      "unknown",     "unknown",     do_mplist,      POS_DEAD,    105,  LOG_NEVER,  SHOW|NOMOB|NOLOG },
 { "vlist",       "unknown",     "unknown",     do_vlist,       POS_DEAD,    103,  LOG_NEVER,  SHOW|NOMOB|OLC|NOLOG },
 { "rlist",       "unknown",     "unknown",     do_rlist,       POS_DEAD,    103,  LOG_NEVER,  SHOW|NOMOB|OLC|NOLOG },
 { "lore",        "узнать",      "дізнатися",   do_lore,        POS_RESTING,   0,  WIZ_SKILLS, SHOW|FREEZE|NOMOB|NOLOG|FREEZE },
 { "morph",       "превратиться","перетворитись",do_polymorph,   POS_RESTING,   0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE },
 { "reform",      "перекинуться","перекинутись",do_reform,      POS_RESTING,   0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|NOORDER|FREEZE|SAVE },
 { "referi",      "рефери",      "рефери",      do_referi,      POS_STANDING,  0,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|FREEZE|NOLOG },
 { "test",        "unknown",     "unknown",     do_test,        POS_RESTING,   1,  LOG_NEVER,  SHOW|MORPH|HIDE|NOMOB|FREEZE|NOPUB },
 { "version",     "версия",      "версия",      do_version,     POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB },
 { "vote",        "голосовать",  "голосувати",  do_vote,        POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "global",      "глобал",      "глобал",      do_global,      POS_DEAD,    103,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|OLC },
 { "suicide",     "умереть",     "суіцид",      do_suicide,     POS_DEAD,      0,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|FULL|NOADR|NOORDER|FREEZE|SAVE },
 { "diplomacy",   "дипломатия",  "дипломатія",  do_diplomacy,   POS_DEAD,      1,  WIZ_CONFIG, SHOW|MORPH|HIDE|NOMOB|NOORDER|FREEZE },
 { "room",        "unknown",     "unknown",     do_room,        POS_DEAD,    103,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|NOLOG },
 { "addpenalty",  "unknown",     "unknown",     do_add_penalty, POS_DEAD,    103,  WIZ_SECURE, SHOW|MORPH|HIDE|NOMOB|OLC },
 { "kazad",       "казад",       "казад",       do_kazad,       POS_DEAD,      0,  WIZ_SPEAKS, SHOW|NOORDER|MORPH|HIDE|NOMOB },
 { "avenge",      "ассасин",     "ассасін",     do_avenge,      POS_DEAD,      0,  WIZ_SPEAKS, SHOW|NOORDER|MORPH|HIDE|NOMOB },
 { "gaccount",    "гсчет",       "грахунок",    do_gaccount,    POS_DEAD,      0,  WIZ_SKILLS, SHOW|NOORDER|MORPH|HIDE|NOMOB|FREEZE|SAVE },
 { "setclass",    "unknown",     "unknown",     do_setclass,    POS_SLEEPING,103,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER|OLC},
 { "dampool",     "unknown",     "unknown",     do_dampool,     POS_SLEEPING,102,  WIZ_SECURE, SHOW|HIDE|NOMOB|NOORDER},
 { "marry",       "предложение", "шлюб",        do_marry,       POS_STANDING,  1,  WIZ_SKILLS, SHOW|NOMOB|NOORDER|NOADR|FREEZE|OLC},
 { "ptalk",       "ссказать",    "ссказати",    do_ptalk,       POS_DEAD,      1,  WIZ_SPEAKS, SHOW|NOMOB|HIDE|MORPH },
 { "divorce",     "развод",      "развод",      do_divorce,     POS_STANDING,  1,  WIZ_SKILLS, SHOW|NOMOB|HIDE|NOORDER|FREEZE|OLC},
 { "race",        "раса",        "раса",        do_race,        POS_DEAD,    106,  WIZ_SECURE, SHOW|NOMOB|HIDE|OLC|NOORDER},
 { "fly",         "взлететь",    "злетіти",     do_fly,         POS_FIGHTING,  1,  WIZ_SKILLS, SHOW|FREEZE|NOLOG},
 { "walk",        "приземлится", "приземлитися",do_walk,        POS_FIGHTING,  1,  WIZ_SKILLS, SHOW|FREEZE|NOLOG},
 { "smoke",       "курить",      "курити",      do_smoke,       POS_RESTING,   1,  WIZ_SPEAKS, HIDE|NOMOB|NOORDER|NOPUB|FREEZE },
 { "mist",        "туман",       "туман",       do_mist,        POS_STANDING,  1,  WIZ_SKILLS, SHOW|FREEZE|NOLOG },
 { "howl",        "вой",         "вой",         do_howl,        POS_STANDING,  1,  WIZ_SKILLS, SHOW|MORPH|NOMOB|FREEZE|NOLOG },
 { "gquest",      "гквест",      "гквест",      do_gquest,      POS_DEAD,      1,  WIZ_SKILLS, HIDE|MORPH|NOMOB|FREEZE|NOARMY|NOORDER },
 { "lash",        "лаш",         "лаш",         do_lash,        POS_FIGHTING,  1,  WIZ_SKILLS, MORPH|NOMOB|FREEZE|NOLOG },
 { "dig",         "закопаться",  "закопатись",  do_dig,         POS_DEAD,      1,  WIZ_SKILLS, MORPH|NOMOB|FREEZE },
 { "pacify",      "от..здить",   "от..здить",   do_pacify,      POS_STANDING,  1,  WIZ_SKILLS, SHOW|NOMOB|FREEZE|NOARMY|NOALL },
 { "",             0,             0,            0,              POS_DEAD,      0,  LOG_NEVER,  HIDE }
};

void interpret( CHAR_DATA *ch, const char *argument )
{
  char command[MAX_STRING_LENGTH];
  char logline[MAX_STRING_LENGTH];
  struct cmd_type *cmd_ptr = NULL;
  int cmd, trust;

  // Strip leading spaces.
  while ( isspace(*argument) ) argument++;
  if ( argument[0] == '\0' )   return;
  smash_tilde((char*)argument);

// Grab the command word. Special parsing so ' can be a command,
//   also no spaces needed after punctuation.
  strcpy( logline, argument );

  if (!IS_LETTER(argument[0]) && !IS_DIGIT(argument[0]))
  {
    command[0] = argument[0];
    command[1] = '\0';
    argument++;
    while ( isspace(*argument) ) argument++;
  }
  else argument = one_argument( argument, command );

  // Look for command in command table

  trust = get_trust( ch );
  if (IS_NPC(ch)) trust=UMIN(101,trust);
  command[0]=LOWER(command[0]);
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
    if(cmd_table[cmd].level > trust) continue;
    if(command[0]!=cmd_table[cmd].name[0]) continue;
    if(IS_SET(cmd_table[cmd].flag,FULL)    ?
     str_cmp( command, cmd_table[cmd].name):
     str_prefix(command,cmd_table[cmd].name)) continue;
    if (IS_SET(global_cfg,CFG_PUBLIC) && IS_SET(cmd_table[cmd].flag,NOPUB)) continue;
    {
      cmd_ptr = &cmd_table[cmd];
      break;
    }
  }
  /* what is it for? To enable non-public commands in public version? */
  if (!cmd_ptr)
  {
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
      if(command[0]==cmd_table[cmd].rname[0] &&
        IS_SET(cmd_table[cmd].flag,FULL)?!str_cmp(command,cmd_table[cmd].rname):
        !str_prefix(command,cmd_table[cmd].rname)
        && cmd_table[cmd].level<=trust)
      {
       cmd_ptr = &cmd_table[cmd];
       break;
      }
    }
  }
  if (!cmd_ptr)
  {
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
      if(command[0]==cmd_table[cmd].uname[0] &&
        IS_SET(cmd_table[cmd].flag,FULL)?!str_cmp(command,cmd_table[cmd].uname):
        !str_prefix(command,cmd_table[cmd].uname)
        && cmd_table[cmd].level<=trust)
      {
       cmd_ptr = &cmd_table[cmd];
       break;
      }
    }
  }

  if ( !cmd_ptr )
  {
    if (!check_social(ch,command,argument)) stc("Ась?\n\r",ch);
    return;
  }

  if (IS_NPC(ch) && IS_SET(cmd_ptr->flag,NOMOB))
  {
   stc("Ты не можешь сделать это.\n\r",ch);
   return;
  }

  if( IS_SET(cmd_ptr->flag, DENY) )
  {
    stc("{RЭта команда ЗАПРЕЩЕНА БОГАМИ!{x\n\r",ch);
    return;
  }

  if (!IS_NPC(ch))
  {
    if (IS_SET(ch->pcdata->cfg,CFG_ZRITEL) && IS_SET(cmd_ptr->flag,NOZRIT))
    {
     stc("Ты не можешь сделать,когда ты Зритель.\n\r",ch);
     return;
    }

    if (IS_SET(ch->act,PLR_FREEZE) && IS_SET(cmd_ptr->flag,FREEZE))
    {
      stc( "Ты полностью заморожен!\n\r", ch );
      return;
    }
    if (ch->morph_obj!=NULL && !IS_SET(cmd_ptr->flag,MORPH))
    {
      stc("Ты должен сначала вернуться в свой нормальный облик.\n\r",ch);
      return;
    };
    if (IS_SET(cmd_ptr->flag,NOADR)&& ch->pcdata->condition[COND_ADRENOLIN]!=0)
    {
      stc("Тебе надо успокоится.\n\r",ch);
      return;
    }
    if (IS_SET(cmd_ptr->flag,NOARMY) && IS_SET(ch->act,PLR_ARMY))
    {
      stc("Ты не можешь делать это в армии.\n\r",ch);
      return;
    };
  }

  if ( strcmp(cmd_ptr->name,"east") ||
       strcmp(cmd_ptr->name,"west") ||
       strcmp(cmd_ptr->name,"north")||
       strcmp(cmd_ptr->name,"south")||
       strcmp(cmd_ptr->name,"up")   ||
       strcmp(cmd_ptr->name,"down") )
   {
     if ( (!IS_SET(cmd_ptr->flag, HIDE) )
       && ( ( !IS_AFFECTED(ch,AFF_HIDE) )
            || ( !IS_AFFECTED(ch,AFF_SNEAK) )
            || (ch->clan == NULL) ))
       REM_BIT( ch->affected_by, AFF_HIDE );
   }
   else if (!IS_SET(cmd_ptr->flag, HIDE)) REM_BIT( ch->affected_by, AFF_HIDE );

  // Character not in position for command?
  if ( ch->position < cmd_ptr->position && ch->trust<111)
  {
    switch( ch->position )
    {
      case POS_DEAD:stc( "Лежи спокойно, ты труп.\n\r", ch );break;
      case POS_MORTAL:
      case POS_INCAP:stc( "Ты слишком изранен.\n\r", ch );break;
      case POS_STUNNED:stc( "Ты оглушен и не можешь этого сделать.\n\r", ch );break;
      case POS_SLEEPING:stc( "Ты же спишь!\n\r", ch );break;
      case POS_RESTING:stc( "Неа...Отдыхать тоже неплохо...\n\r", ch);break;
      case POS_SITTING:stc( "Сначала встань.\n\r",ch);break;
      case POS_FIGHTING:stc( "Ты все еще сражаешься!\n\r", ch);break;
    }
    return;
  }

  if (IS_SET(ch->act, PLR_LOG) || fLogAll || cmd_ptr->log!=LOG_NEVER)
  {
    smash_percent(logline);
    do_printf(log_buf,"%s%s%s",ch->name,get_logchar(cmd_ptr->log),logline);
    if (!NOPUBLIC && !IS_SET(cmd_ptr->flag,NOLOG)) log_string(log_buf);
    if (!IS_SET(ch->comm,COMM_COMSYS))
    {
      smash_dollar(log_buf);
      wiznet(log_buf,ch,NULL,cmd_ptr->log,get_loglevel(cmd_ptr->log));
    }
  }

  if (IS_SET(cmd_ptr->flag,OLC))
  {
    char tmp[MAX_STRING_LENGTH];
    strftime(tmp,19,"%d%m %a %H:%M:%S:",localtime(&current_time));
    do_printf(log_buf,"%s[%s]%s\n",tmp,ch->name,logline);
    ID_FILE=OLC_FILE;
    stf(log_buf,NULL);
  }
  if (IS_SET(cmd_ptr->flag,SAVE)) WILLSAVE(ch);

  if ( ch->desc && ch->desc->snoop_by)
  {
    write_to_buffer( ch->desc->snoop_by, "% ",    2 );
    write_to_buffer( ch->desc->snoop_by, logline, 0 );
    write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
  }
 (*cmd_ptr->do_fun) ( ch, argument );
 tail_chain( );
}

// function to keep argument safe in all commands -- no static strings
void do_function (CHAR_DATA *ch, DO_FUN *do_fun, const char *argument)
{
 const char *command_string;

 command_string = str_dup(argument);
 (*do_fun) (ch, command_string);
 free_string(command_string);
}

bool check_social( CHAR_DATA *ch, char *command, const char *argument )
{
 char arg[MAX_STRING_LENGTH];
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


 if ( !found ) return FALSE;

 if (IS_NPC(ch) && IS_SET(social_table[cmd].flag,SOC_NOMOB))
 {
   do_function(ch,&do_emote,"озадаченно вертит головой, не понимая приказа.");
   return TRUE;
 }

 if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
 {
  stc( "Ты не можешь этого сделать!\n\r", ch );
  return TRUE;
 }

 switch ( ch->position )
 {
    case POS_DEAD: stc( "Лежи смирно, ты труп.\n\r", ch ); return TRUE;
    case POS_INCAP:
    case POS_MORTAL:stc( "Ты слишком изранен.\n\r", ch );return TRUE;
    case POS_STUNNED:stc( "Ты оглушен и не можешь этого сделать.\n\r", ch );return TRUE;
    case POS_SLEEPING:
        if ( !str_cmp( social_table[cmd].name, "snore" ) ) break;
        stc( "Ты же спишь!\n\r", ch );
        return TRUE;
  }

 if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"social")) return TRUE;

 if (IS_SET(social_table[cmd].flag,SOC_CULTURAL))
 {
   if (ch->move<3)
   {
     stc("Ты слишком устал.\n\r",ch);
     return TRUE;
   }
   ch->move-=3;
 }

 one_argument( argument, arg );
 victim = NULL;
 if ( arg[0] == '\0' )
 {
  act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
  act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
  return TRUE;
 }

 victim=get_char_room(ch, arg);
 if (victim==NULL) victim = get_pchar_world(ch, arg);
 if (victim==NULL)
 {
  stc("Тут таких нет.\n\r", ch);
  return TRUE;
 }
 if (victim->position==POS_SLEEPING)
 {
   act( "$O спит и не видит тебя.", ch, NULL, victim, TO_CHAR);
   return TRUE;
 }
 if ( victim == ch )
 {
  act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM );
  act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR );
 }
 else
 {
  act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
  act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
  act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    ); 

  if ( !IS_NPC(ch) && IS_NPC(victim) && !IS_AFFECTED(victim, AFF_CHARM)
     && IS_AWAKE(victim) && victim->desc == NULL)
   {
    switch ( number_bits( 4 ) )
    {
     case 0:

     case 1: case 2: case 3: case 4:
     case 5: case 6: case 7: case 8:
       act( social_table[cmd].others_found,victim, NULL, ch, TO_NOTVICT );
       act( social_table[cmd].char_found,victim, NULL, ch, TO_CHAR    ); 
       act( social_table[cmd].vict_found,victim, NULL, ch, TO_VICT    ); 
       break;

     case 9: case 10: case 11: case 12:
       act( "$c1 дает пощечину $C2.",  victim, NULL, ch, TO_NOTVICT );
       act( "Ты даешь пощечину $C2.",  victim, NULL, ch, TO_CHAR    );
       act( "$c1 дает тебе пощечину.", victim, NULL, ch, TO_VICT    );
       break;
    }
   }
  }
 return TRUE;
}

// Return true if an argument is completely numeric.
bool is_number ( const char *arg )
{
 if ( *arg == '\0' ) return FALSE;
 if ( *arg == '+' || *arg == '-' ) arg++;

 for ( ; *arg != '\0'; arg++ )
 {
  if ( !isdigit( *arg ) ) return FALSE;
 }
 return TRUE;
}

// Given a string like 14.foo, return 14 and 'foo'
int number_argument( char *argument, char *arg )
{
 char *pdot;
 int number;

 for ( pdot = argument; *pdot != '\0'; pdot++ )
 {
  if ( *pdot == '.' )
  {
   *pdot = '\0';
   number = atoi( argument );
   *pdot = '.';
   strcpy( arg, pdot+1 );
   return number;
  }
 }
 strcpy( arg, argument );
 return 1;
}

// Given a string like 14*foo, return 14 and 'foo'
int mult_argument(char *argument, char *arg)
{
 char *pdot;
 int number;

 for ( pdot = argument; *pdot != '\0'; pdot++ )
 {
  if ( *pdot == '*' )
  {
   *pdot = '\0';
   number = atoi( argument );
   *pdot = '*';
   strcpy( arg, pdot+1 );
   return number;
  }
 }
 
 strcpy( arg, argument );
 return 1;
}

// Pick off one argument from a string and return the rest. Understands quotes.
const char *one_argument( const char *argument, char *arg_first )
{
 char cEnd;

 while ( isspace(*argument) ) argument++;

 cEnd = ' ';
 if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

 while ( *argument != '\0' )
 {
   if ( *argument == cEnd )
   {
    argument++;
    break;
   }
   *arg_first = LOWER(*argument);
   arg_first++;
   argument++;
 }
 *arg_first = '\0';
 while ( isspace(*argument) ) argument++;
 return argument;
}

// Contributed by Alander.
void do_commands( CHAR_DATA *ch, const char *argument )
{
  int cmd=0, col=0;
 

  stc("Список доступных команд:\n",ch);
  for (;cmd_table[cmd].name[0]!= '\0'; cmd++ )
  {
    if (cmd_table[cmd].level<LEVEL_HERO
      && cmd_table[cmd].level<=get_trust(ch)
      && IS_SET(cmd_table[cmd].flag,SHOW))
    {
     ptc(ch,"{G[{Y%-12s %12s %12s{G] ",cmd_table[cmd].name,cmd_table[cmd].rname,cmd_table[cmd].uname);
     if (++col % 2==0) stc( "{x\n\r", ch );
    }
  }

  if (col % 3!=0) stc("{x\n\r",ch);
  return;
}

void do_wizhelp( CHAR_DATA *ch, const char *argument )
{
  int level,cmd,col=0;
 
  for (level=102;level<=get_trust(ch);level++)
  {
    col=0;
    ptc(ch,"{C%3d:{x",level);
      
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {                                            
      if ( cmd_table[cmd].level == level
/*        && IS_SET(cmd_table[cmd].flag, SHOW)*/)
      {
        if (col == 6)
        {
          col=0;
          stc("    ",ch);
        }
        ptc( ch, "%-12s", cmd_table[cmd].name );
        if (++col % 6 == 0 ) stc("\n\r", ch );
      }                                   
    }                                      
    if (col % 6 != 0) stc("{x\n\r", ch);
  }
  return;
}

void do_stopfoll( CHAR_DATA *ch, const char *argument )
{
 if (IS_AFFECTED(ch, AFF_CHARM) && ch->master!=NULL)
 {
   stc("Покинуть любимого хозяина??\n\r",ch);
   return;
 }
 if ( IS_SET(ch->affected_by, AFF_CHARM))
  {
      REM_BIT(ch->affected_by, AFF_CHARM);
  }

 die_follower(ch);
}

void do_ahelp(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  bool found=FALSE;

  argument=one_argument(argument,arg1);
  argument=one_argument(argument,arg2);

  if (EMPTY(arg1))
  {
    stc("{C Advanced Help показывает информацию, извлекая ее из текущего кода.\n\r",ch);
    stc("Эта информация является корректной и не требует ручного обновления.\n\r",ch);
    stc("Advanced Help синтаксис:\n\r{x",ch);
    stc(" ahelp race\n\r",ch);
    stc(" ahelp skill\n\r",ch);
    stc(" ahelp class\n\r",ch);
    stc(" ahelp stat <race>\n\r",ch);
    stc(" ahelp group\n\r",ch);
    stc(" ahelp damage\n\r",ch);
    stc(" ahelp material\n\r",ch);
    return;
  }
  
  if (!str_prefix(arg1,"race"))
  {
    int race;
    int64 spec;
    if (EMPTY(arg2))
    {
      stc("Syntax: ahelp race <имя расы>\n\r\n\r",ch);
      stc("Список доступных рас:\n\r",ch);
      stc("{W#No {CИмя расы     {GДоступна {W#No {CИмя расы     {GДоступна\n\r",ch);
      for ( race = 1; race_table[race].name; race+=2 )
      {
       ptc(ch,"{W%2d {C%15s %s",race,race_table[race].name,
         (race_table[race].pc_race==TRUE) ? "{GДа{x " : "{RНет{x");
       if (race_table[race+1].name) ptc(ch,"     {W%2d {C%15s %s\n\r",race+1,race_table[race+1].name,
         (race_table[race+1].pc_race==TRUE) ? "{GДа{x " : "{RНет{x");
       else stc("\n\r",ch);
      }
      return;
    }

    for ( race = 1; race_table[race].name != NULL; race++ )
    {
      if (!str_prefix(arg2,race_table[race].name))
      {
        found=TRUE;
        break;
      }
    }
    if (!found)
    {
      stc("Нет такой расы.\n\r",ch);
      return;
    }
    stc("{C=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r",ch);
    ptc(ch,"| Advanced help: Раса{Y %s  {x( %s )\n\r",
      race_table[race].name,race_table[race].who_name);
    stc("{C=-------------------------------------------------------------------=\n\r",ch);
      ptc(ch,"{CCreation Points:{Y %3d {CРазмер:{Y %s\n\r",race_table[race].points,size_table[race_table[race].size].name);
      ptc(ch,"Тип удара: %s\n\r",race_table[race].hand_dam);
      ptc(ch,"{CВрожденные навыки:{G %s %s %s %s %s\n\r",race_table[race].skills[0],
        race_table[race].skills[1],race_table[race].skills[2],
        race_table[race].skills[3],race_table[race].skills[4]);
      ptc(ch,"{CНачальные параметры   :{Y%3d %3d %3d %3d %3d\n\r",race_table[race].stats[0],
        race_table[race].stats[1],race_table[race].stats[2],
        race_table[race].stats[3],race_table[race].stats[4]);
      ptc(ch,"{CМакс. свои параметры  :{R%3d %3d %3d %3d %3d\n\r",race_table[race].max_stats[0],
        race_table[race].max_stats[1],race_table[race].max_stats[2],
        race_table[race].max_stats[3],race_table[race].max_stats[4]);
      ptc(ch,"{CМаксимальные параметры:{R%3d %3d %3d %3d %3d\n\r",race_table[race].high_stats[0],
        race_table[race].high_stats[1],race_table[race].high_stats[2],
        race_table[race].high_stats[3],race_table[race].high_stats[4]);
      stc("Максимальные параметры указаны для Имморталов. У смертных они меньше. (+2 к primary class stat)\n\r",ch);
      ptc(ch,"{CClass Mult            :{MMag:[{Y%3d{M] Cle:[{Y%3d{M] Thi:[{Y%3d{M] War:[{Y%3d{M]{x\n\r",race_table[race].class_mult[0],
        race_table[race].class_mult[1],race_table[race].class_mult[2],
        race_table[race].class_mult[3]);
    stc("{C=-------------------------------------------------------------------=\n\r",ch);
    ptc(ch,"{CПостоянные аффекты:{c %s\n\r",affect_bit_name(race_table[race].aff));
    ptc(ch,"{CУязвимости        :{R %s\n\r",imm_bit_name(race_table[race].vuln));
    ptc(ch,"{CСопротивляемость  :{G %s\n\r",imm_bit_name(race_table[race].res));
    ptc(ch,"{CИммунитет         :{W %s\n\r{x",imm_bit_name(race_table[race].imm));
    stc("{C=-------------------------------------------------------------------=\n\r",ch);
    if (race_table[race].c_pen!=0)
    {                                                   
      stc("{RПенальти для категорий: [",ch);
      if (IS_SET(race_table[race].c_pen,WATER))       stc("Water ",ch);
      if (IS_SET(race_table[race].c_pen,AIR))         stc("Air ",ch);
      if (IS_SET(race_table[race].c_pen,EARTH))       stc("Earth ",ch);
      if (IS_SET(race_table[race].c_pen,FIRE))        stc("Fire ",ch);
      if (IS_SET(race_table[race].c_pen,SPIRIT))      stc("Spirit ",ch);
      if (IS_SET(race_table[race].c_pen,MIND))        stc("Mind ",ch);
      if (IS_SET(race_table[race].c_pen,PROTECT))     stc("Protection ",ch);
      if (IS_SET(race_table[race].c_pen,LIGHT))       stc("Light ",ch);
      if (IS_SET(race_table[race].c_pen,DARK))        stc("Dark ",ch);
      if (IS_SET(race_table[race].c_pen,FORTITUDE))   stc("Fortitude ",ch);
      if (IS_SET(race_table[race].c_pen,CURATIVE))    stc("Curative ",ch);
      if (IS_SET(race_table[race].c_pen,PERCEP))      stc("Perception ",ch);
      if (IS_SET(race_table[race].c_pen,LEARN))       stc("Learning ",ch);
      if (IS_SET(race_table[race].c_pen,MAKE))        stc("MakeMastery",ch);
      stc ("]{x\n\r",ch);
    }
    if (race_table[race].c_bon!=0)
    {
      stc("{GБонус для категорий: [",ch);
      if (IS_SET(race_table[race].c_bon,WATER))       stc("Water ",ch);       
      if (IS_SET(race_table[race].c_bon,AIR))         stc("Air ",ch);         
      if (IS_SET(race_table[race].c_bon,EARTH))       stc("Earth ",ch);        
      if (IS_SET(race_table[race].c_bon,FIRE))        stc("Fire ",ch);        
      if (IS_SET(race_table[race].c_bon,SPIRIT))      stc("Spirit ",ch);      
      if (IS_SET(race_table[race].c_bon,PROTECT))     stc("Protection ",ch);
      if (IS_SET(race_table[race].c_bon,MIND))        stc("Mind ",ch);        
      if (IS_SET(race_table[race].c_bon,LIGHT))       stc("Light ",ch);       
      if (IS_SET(race_table[race].c_bon,DARK))        stc("Dark ",ch);        
      if (IS_SET(race_table[race].c_bon,FORTITUDE))   stc("Fortitude ",ch);
      if (IS_SET(race_table[race].c_bon,CURATIVE))    stc("Curative ",ch);
      if (IS_SET(race_table[race].c_bon,PERCEP))      stc("Perception ",ch);  
      if (IS_SET(race_table[race].c_bon,LEARN))       stc("Learning ",ch);    
      if (IS_SET(race_table[race].c_bon,MAKE))        stc("MakeMastery",ch); 
      stc("]\n\r{C=-------------------------------------------------------------------=\n\r",ch);
    }
    ptc(ch,"{CОсобенности    :{Y %s\n\r",spec_bit_name(race_table[race].spec));
    for(spec=0;rspec_table[spec].bit!=0;spec++)
      if (IS_SET(race_table[race].spec, rspec_table[spec].bit))
        ptc(ch, "{G%s{x\n\r", rspec_table[spec].info);
    stc("{C=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r",ch);
    return;
  }
  

  if (!str_prefix(arg1,"damage"))
  {
    int temp;

    if (IS_IMMORTAL(ch)) stc("\n\r{C#No. {D[{C  От{D - {C До {D] [{xMessage{D]{x\n\r",ch);
    else stc("\n\r{C#No. {D[{xmessage{D]{x\n\r",ch);
    for (temp=0;;temp++)
    {
      if (IS_IMMORTAL(ch))
      ptc(ch,"{C%3d. {D[{C%4d{D - {C%4d{D] [{x%s{D]{x\n\r",temp,dam_msg_table[temp].from,dam_msg_table[temp].to,dam_msg_table[temp].vs);
      else ptc(ch,"{C%3d. {D[{x%s{D]{x\n\r",temp,dam_msg_table[temp].vs);
      if (dam_msg_table[temp].to==-1) break;
    }
    return;
  }

  if( !str_prefix( arg1, "material") )
  {
    int mIndex, cols=0;

    if (EMPTY(arg2) )
    {
      stc("ahelp material list       : list of materials\n\r", ch);
      stc("ahelp material durability : list of durability messages\n\r", ch);
      stc("ahelp material <material> : extra info on material\n\r", ch);
      return;
    }

    if (!str_prefix(arg2,"list"))
    {
      ptc( ch,"{C+------------------------------------------------------------------+{x\n\r");
      ptc( ch,"{C[{x%24s%s%24s{C]{x\n\r", "", "Таблица материалов", "");
      ptc( ch,"{C+------------------------------------------------------------------+{x\n\r");
      for( mIndex=0; material_table[mIndex].d_dam != 0 ; mIndex++ )
      {
        ptc( ch,"{C[{Y%15s{C]{x", material_table[mIndex].real_name);
        cols++;
        if( cols == 4 )
        {
          stc("\n\r", ch);
          cols=0;
        }
      }
      ptc( ch,"{C+------------------------------------------------------------------+{x\n\r");
      if( strcmp(material_table[mIndex].name,"NULL") )  return;
    }

    else if (!str_prefix(arg2,"durability"))
    {
      if( IS_ELDER(ch) )
      {
        ptc( ch, "{cFrom   To {GПрочность материалов в порядке возростания{w:{x\n\r");
        for( mIndex=0; item_durability_table[mIndex].to != -1; mIndex++ )
        {
          ptc( ch,"{m%4d {R%4d{x ",item_durability_table[mIndex].from, (item_durability_table[mIndex].to - 1) );
          ptc( ch, "{C[{Y%s{C]{x\n\r", item_durability_table[mIndex].d_message);
        }
      }
      else 
      {
        ptc( ch, "{GПрочность материалов в порядке возростания.{x\n\r");
        for( mIndex=0; item_durability_table[mIndex].to != -1; mIndex++ )
          ptc( ch, "{C[{Y%s{C]{x\n\r", item_durability_table[mIndex].d_message);
      }
    }

    else if ( !str_cmp(arg2, material_lookup(arg2)) )
    {
      int mn_arg2;

      if( !str_prefix(arg2,"bug") ) 
      {
        bug("Kakogo-to hrena gluchit esli arg2 == bug v ahelp material:(", 0);
        if( !IS_ELDER(ch) ) WAIT_STATE( ch, 10);
        return;
      }

      if( (mn_arg2 = material_num(arg2)) < MATERIAL_NONE )
      {
        stc("{RBUG in reading material table data.{x", ch);
        return;
      }
      else 
      {
        ptc( ch, "{GМатериал{w: {Y[{R%s{Y][{R%s{Y]{x\n\r", material_table[mn_arg2].real_name, arg2);
        for( mIndex = 0; item_durability_table[mIndex].to != -1; mIndex++)
          if( material_table[mn_arg2].d_dam > item_durability_table[mIndex].from && material_table[mn_arg2].d_dam <= item_durability_table[mIndex].to)
          {
            ptc( ch, "{GБазовая прочность для даного материала{w:{x %s", item_durability_table[mIndex].d_message );
            if( IS_ELDER(ch) ) 
            {
              ptc( ch, " {Y[{R%d{Y]{x.\n\r", material_table[mn_arg2].d_dam );
              ptc( ch, "{GОтносится к диапазону прочностей {Y[{m%d{Y]-[{R%d{Y]{x.\n\r", item_durability_table[mIndex].from, item_durability_table[mIndex].to );
            }
            else stc( "{x.\n\r", ch);
            return;
          }
      }
    }

    else if ( !str_cmp(arg2, materialrus_lookup(arg2)) )
    {
      int mnr_arg2;

      if( !str_prefix(arg2,"bug") ) 
      {
        bug("Kakogo-to hrena gluchit esli arg2 == bug v ahelp material:(", 0);
        if( !IS_ELDER(ch) ) WAIT_STATE( ch, 10);
        return;
      }

      if( (mnr_arg2 = materialrus_num(arg2)) < MATERIAL_NONE )
      {
        stc("{RBUG :({x", ch);
        return;
      }
      else 
      {
        ptc( ch, "{GМатериал{w: {Y[{R%s{Y][{R%s{Y]{x\n\r", arg2, material_table[mnr_arg2].name );
        for( mIndex = 0; item_durability_table[mIndex].to != -1; mIndex++)
          if( material_table[mnr_arg2].d_dam > item_durability_table[mIndex].from && material_table[mnr_arg2].d_dam <= item_durability_table[mIndex].to)
          {
            ptc( ch, "{GБазовая прочность для даного материала{w:{x %s", item_durability_table[mIndex].d_message );
            if( IS_ELDER(ch) ) 
            {
              ptc( ch, " {Y[{R%d{Y]{x.\n\r", material_table[mnr_arg2].d_dam );
              ptc( ch, "{GОтносится к диапазону прочностей {Y[{m%d{Y]-[{R%d{Y]{x.\n\r", item_durability_table[mIndex].from, item_durability_table[mIndex].to );
            }
            else stc( "{x.\n\r", ch);
            return;
          }
      }
    }
  else return;
  return;
  }

  if( !str_prefix(arg1,"group") )
  {
    int gn,i;

    if (EMPTY(arg2))
    {
      stc("ahelp group list\n\r",ch);
      stc("ahelp group <group>\n\r",ch);
      return;
    }

    if (!str_prefix(arg2,"list"))
    {
      int col=0;

      for(i=0;i<MAX_GROUP;i++)
      {
        if (group_table[i].name == NULL) break;
        ptc(ch,"%3d %15s  ",i+1,group_table[i].name);
        col++;
        if (col==4)
        {
          col=0;
          stc("\n\r",ch);
        }
      }
      return;
    }
    gn=group_lookup(arg2);
    if (gn == -1)
    {
      stc("Такой группы не найдено.\n\r",ch);
      return ;
    }
    ptc(ch,"Группа : [{C%s{x]\n\r",group_table[gn].name);
    if (group_cost(ch,gn) >0) ptc(ch,"{CСтоимость: %d{x\n\r",group_cost(ch,gn));
    ptc(ch,"{CСтоимость для: Mag:[%3d]  Cle:[%3d]  Thi:[%3d]  War:[%3d]{x\n\r",
      group_table[gn].rating[0],group_table[gn].rating[1],
      group_table[gn].rating[2],group_table[gn].rating[3]);
    stc("{gВ группу входят следующие навыки и заклинания:{x\n\r",ch);
    for(i=0;group_table[gn].spells[i]!=NULL;i++)
    {
      ptc(ch,"{c[%2d]{Y %s{x\n\r",i+1,group_table[gn].spells[i]);
    }
    return;
  }

  if (!str_prefix(arg1,"class"))
  {
    if (EMPTY(arg2))
    {
      stc("Syntax: ahelp class bonus\n\r\n\r",ch);
      stc("Syntax: ahelp class list\n\r\n\r",ch);
      return;
    }
    if (!str_prefix(arg2,"bonus"))
    {
      stc("Бонусы на классы. (Сохраняются при перерождении):\n\r",ch);
      stc("{wMage   {x:              +2 Learning   +2 Mind\n\r",ch);
      stc("{wCleric {x:              +2 Spirit     +2 Curative\n\r",ch);
      stc("{wThief  {x:              +3 Perception +1 Offence\n\r",ch);
      stc("{wWarrior{x:              +2 Fortitude  +2 Offence\n\r",ch);
                                     
      stc("{GBattleMage{x [M+W]:     +1 Fortitude  +2 Learning   +1 MakeMastery +1 Offence\n\r",ch);
      stc("{GWizard    {x [M+C]:     +2 Fire,Air,Water,Earth\n\r",ch);
      stc("{GNightBlade{x [M+T]:     +2 Perception +2 Mind       +1 Learn\n\r",ch);
      stc("{GHeretic   {x [C+T]:     +2 Perception +2 Spirit     +1 Fire\n\r",ch);
      stc("{GPaladin   {x [C+W]:     +2 Spirit     +1 Curative   +2 Protection\n\r",ch);
      stc("{GNinja     {x [W+T]:     +1 Fortitude  +2 Perception +2 Offence\n\r",ch);

      stc("{RSpellSword{x [M+W+T]:   +1 Offence    +1 Learning   +1 Perception\n\r",ch);
      stc("{RTemplar   {x [C+W+T]:   +1 Spirit     +1 Perception +1 Protection\n\r",ch);
      stc("{RMonk      {x [M+C+W]:   +1 Protection +1 Curative   +1 Fortitude   +1 Spirit\n\r",ch);
      stc("{RBard      {x [M+C+T]:   +1 Mind       +1 Learning   +1 Perception\n\r",ch);

      stc("Race bonus/penalty: 4\n\r",ch);
      return;
    }
    if (!str_prefix(arg2,"list"))
    {
      stc("Список доступных классов:\n\r",ch);
      stc("Основные классы:\n\r",ch);
      stc("{GWarrior - Сила      (Str)\n\r",ch);
      stc("{GMage    - Интеллект (Int)\n\r",ch);
      stc("{GThief   - Ловкость  (Dex)\n\r",ch);
      stc("{GCleric  - Мудрость  (Wis)\n\r{x",ch);
      return;
    }
    do_ahelp (ch,"class");
    return;
  }

  if (!str_prefix(arg1,"skill") || !str_prefix(arg1,"spell"))
  {
    int sn;

    if (EMPTY(arg2))
    {
      stc("Syntax: ahelp <skill> <имя навыка или заклинания>\n\r",ch);
      stc("        ahelp <skill> list\n\r",ch);
      stc("        ahelp <skill> list <имя группы или групп>\n\r",ch);
      stc("Пример: ahelp kick\n\r",ch);
      stc("        ahelp 'dispel go\n\r",ch);
      stc("        ahelp list fortitude\n\r",ch);
      stc("        ahelp list fire air\n\r",ch);
      return;
    }

    if ( !str_prefix(arg2,"list"))
    {
      int col=0;
  
      if (EMPTY(argument))
      {
        stc("{CЗаклинания: {x\n\r",ch);
        for ( sn = 0; sn < max_skill; sn++ )
        {
          if ( skill_table[sn].name == NULL ) break;
          if ( IS_SET(skill_table[sn].group,SPELL))
          {
            ptc( ch, "%18s ", skill_table[sn].name);
            if ((col++)==3) {stc("\n\r",ch);col=0;}
          }
        }
        stc("\n\r{GНавыки:{x\n\r",ch);
        for ( sn = 0; sn < max_skill; sn++ )
        {
          if ( skill_table[sn].name == NULL ) break;
          if ( IS_SET(skill_table[sn].group,SKILL))
          {
            ptc( ch, "%18s ", skill_table[sn].name);
            if ((col++)==3) {stc("\n\r",ch);col=0;}
          }
        }
        return;
      }
      else
      {
        bool fortitude=FALSE,curative=FALSE;
        bool dark=FALSE,light=FALSE,mind=FALSE,spirit=FALSE,percep=FALSE,make=FALSE,learn=FALSE;
        bool fire=FALSE,water=FALSE,earth=FALSE,air=FALSE;
        bool protect=FALSE;

        if (is_name("fortitude",argument))  fortitude=TRUE;
        if (is_name("curative",argument))   curative=TRUE;
        if (is_name("dark",argument))       dark=TRUE;
        if (is_name("protection",argument)) protect=TRUE;
        if (is_name("mind",argument))       mind=TRUE;
        if (is_name("light",argument))      light=TRUE;
        if (is_name("spirit",argument))     spirit=TRUE;
        if (is_name("earth",argument))      earth=TRUE;
        if (is_name("air",argument))        air=TRUE;
        if (is_name("fire",argument))       fire=TRUE;
        if (is_name("water",argument))      water=TRUE;
        if (is_name("learning",argument))   learn=TRUE;
        if (is_name("perception",argument)) percep=TRUE;
        if (is_name("makemastery",argument))make=TRUE;
        for(sn=0;sn<max_skill;sn++)
        {
          if ( skill_table[sn].name == NULL ) break;
          if ( fortitude && !IS_SET(skill_table[sn].group,FORTITUDE)) continue;
          if ( curative && !IS_SET(skill_table[sn].group,CURATIVE)) continue;
          if ( air && !IS_SET(skill_table[sn].group,AIR)) continue;
          if ( fire && !IS_SET(skill_table[sn].group,FIRE)) continue;
          if ( water && !IS_SET(skill_table[sn].group,WATER)) continue;
          if ( earth && !IS_SET(skill_table[sn].group,EARTH)) continue;
          if ( protect && !IS_SET(skill_table[sn].group,PROTECT)) continue;
          if ( learn && !IS_SET(skill_table[sn].group,LEARN)) continue;
          if ( percep && !IS_SET(skill_table[sn].group,PERCEP)) continue;
          if ( make && !IS_SET(skill_table[sn].group,MAKE)) continue;
          if ( dark && !IS_SET(skill_table[sn].group,DARK)) continue;
          if ( light && !IS_SET(skill_table[sn].group,LIGHT)) continue;
          if ( spirit && !IS_SET(skill_table[sn].group,SPIRIT)) continue;
          if ( mind && !IS_SET(skill_table[sn].group,MIND)) continue;
          ptc (ch,"{C%18s {G[{M%s{G]{x\n\r",skill_table[sn].name,spell_group_name(skill_table[sn].group));
        }
        return;
      }
    }

    sn = skill_lookup(arg2);
    if (sn==-1)
    {
      stc("Такого заклинания или навыка не найдено.\n\r",ch);
      return;
    }

    if (skill_table[sn].spell_fun==spell_null)
    {
      ptc(ch,"{GНавык: {Y%s{x\n\r",skill_table[sn].name);

      if (skill_table[sn].rating[CLASS_MAG]!=0 && skill_table[sn].skill_level[CLASS_MAG]<102)
        ptc(ch,"{GМаг{x   : уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_MAG],
        skill_table[sn].rating[CLASS_MAG]);
      else stc("{GМаг{x   : {RN/A{x\n\r",ch);

      if (skill_table[sn].rating[CLASS_CLE]!=0 && skill_table[sn].skill_level[CLASS_CLE]<102)
        ptc(ch,"{GКлерик{x: уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_CLE],
        skill_table[sn].rating[CLASS_CLE]);
      else stc("{GКлерик{x: {RN/A{x\n\r",ch);

      if (skill_table[sn].rating[CLASS_THI]!=0 && skill_table[sn].skill_level[CLASS_THI]<102)
        ptc(ch,"{GВор{x   : уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_THI],
        skill_table[sn].rating[CLASS_THI]);
      else stc("{GВор{x   : {RN/A{x\n\r",ch);

      if (skill_table[sn].rating[CLASS_WAR]!=0 && skill_table[sn].skill_level[CLASS_WAR]<102)
        ptc(ch,"{GВоин{x  : уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_WAR],
        skill_table[sn].rating[CLASS_WAR]);
      else stc("{GВоин{x  : {RN/A{x\n\r",ch);
    }
    else
    {
      ptc(ch,"{CЗаклинание: {Y%s{x\n\r",skill_table[sn].name);

      if (skill_table[sn].rating[CLASS_MAG]!=0 && skill_table[sn].skill_level[CLASS_MAG]<102)
        ptc(ch,"{GМаг{x   : уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_MAG],
        skill_table[sn].rating[CLASS_MAG]);
      else stc("{GМаг{x   : {RN/A{x\n\r",ch);

      if (skill_table[sn].rating[CLASS_CLE]!=0 && skill_table[sn].skill_level[CLASS_CLE]<102)
        ptc(ch,"{GКлерик{x: уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_CLE],
        skill_table[sn].rating[CLASS_CLE]);
      else stc("{GКлерик{x: {RN/A{x\n\r",ch);

      if (skill_table[sn].rating[CLASS_THI]!=0 && skill_table[sn].skill_level[CLASS_THI]<102)
        ptc(ch,"{GВор{x   : уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_THI],
        skill_table[sn].rating[CLASS_THI]);
      else stc("{GВор{x   : {RN/A{x\n\r",ch);

      if (skill_table[sn].rating[CLASS_WAR]!=0 && skill_table[sn].skill_level[CLASS_WAR]<102)
        ptc(ch,"{GВоин{x  : уровень {Y%3d{x, способность {Y%d{x\n\r",
        skill_table[sn].skill_level[CLASS_WAR],
        skill_table[sn].rating[CLASS_WAR]);
      else stc("{GВоин{x  : {RN/A{x\n\r",ch);

      ptc(ch,"{YМинимальное количество маны: {C%d{x\n\r",skill_table[sn].min_mana);
      if (IS_SET(skill_table[sn].flag,C_NODUAL)) stc("{RВо время использование second weapon использовать нельзя.{x\n\r",ch);
    }
      ptc(ch,"{YПринадлежность к группам: [{M%s{Y]{x\n\r",spell_group_name(skill_table[sn].group));
      ptc(ch,"%s{x\n\r",skill_table[sn].help);
  }
  
  if (!str_prefix(arg1,"stat"))
  {
    int i,j;
    bool is_found=FALSE;
    if (EMPTY(arg2))
    {
      ptc(ch,"Syntax: ahelp stat <race>\n\r");
      return;
    }
    i=race_lookup(arg2);
    if (i==0)
    {
      ptc(ch,"Такой расы не существует.\n\r");
      return;
    }      
    ptc(ch,"{GСопротивляемости и {Rуязвимости {xдля {W%s{x:\n\r",race_table[i].name);
    for ( j = 0;j < DAM_MAX;j++)
    {
      if ( race_table[i].dambonus[j] < 0)
      {
        ptc(ch,"{C%s{x: {G%d%%\n\r{x",bonus_dam_types[j],race_table[i].dambonus[j]);
        is_found=TRUE;
      }
    }
    for ( j = 0;j < DAM_MAX;j++)
    {
      if ( race_table[i].dambonus[j] > 0 )
      {
        ptc(ch,"{C%s{x: {R+%d%%\n\r{x",bonus_dam_types[j],race_table[i].dambonus[j]);
        is_found=TRUE;
      }
    }
    if (!is_found) stc("Нет.\n\r",ch);
    is_found=FALSE;
    stc("{wWeapon bonuses{x:\n\r",ch);
    for ( j = 0; j < WEAPON_MAX; j++)
    {
      if ( race_table[i].weapon_bonus[j] > 0 )
      {
        ptc(ch,"{c%s{x: {G+%d%%\n\r",weapon_bonus_types[j],race_table[i].weapon_bonus[j]);
        is_found=TRUE;
      }
    }
    for ( j = 0; j < WEAPON_MAX; j++)
    {
      if ( race_table[i].weapon_bonus[j] < 0 )
      {
        ptc(ch,"{c%s{W: {R%d%%\n\r",weapon_bonus_types[j],race_table[i].weapon_bonus[j]);
        is_found=TRUE;
      }
    }
    if (!is_found) stc("Нет.\n\r",ch);
  }
  
}

// Проверяем входит ли f2 в f1
bool is_set(int64 f1,int64 f2)
{
 int f1h,f2h,f1l,f2l;
 if (!f1 || !f2) return FALSE;
 if (f1 < f2) return FALSE;
 f1h = (int)((f1 >> 32) & 0xFFFFFFFF);
 f2h = (int)((f2 >> 32) & 0xFFFFFFFF);
 f1l = (int)f1;
 f2l = (int)f2;
 return ((f1l & f2l) == f2l) && ((f1h & f2h) == f2h);
}

void set_bit(int64 flag,int64 bit)
{
 int flagh,bith,flagl,bitl;
 if (!flag || !bit) return;
 flagh = (int)((flag >> 32) & 0xFFFFFFFF);
 bith = (int)((bit >> 32) & 0xFFFFFFFF);
 flagl = (int)flag;
 bitl = (int)bit;
 if (!flagh && !bith)
 {
  flagl |= bitl;
  flag = flagl;
 }
 else
 {
  flagh |= bith;
  flagl |= bitl;
  flag = ((int64)flagh << 32) + flagl;
 }
}

void rem_bit(int64 flag,int64 bit)
{
 int flagh,bith,flagl,bitl;
 if (!flag || !bit) return;
 flagh = (int)((flag >> 32) & 0xFFFFFFFF);
 bith = (int)((bit >> 32) & 0xFFFFFFFF);
 flagl = (int)flag;
 bitl = (int)bit;
 if (!flagh && !bith)
 {
  flagl &= ~(bitl);
  flag = flagl;
 }
 else 
 {
  flagh &= ~(bith);
  flagl &= ~(bitl);
  flag = ((int64)flagh << 32) + flagl;
 }
}

// does aliasing and other fun stuff
void substitute_alias(DESCRIPTOR_DATA *d, const char *argument)
{
  CHAR_DATA *ch;
  char buf[MAX_STRING_LENGTH],prefix[MAX_STRING_LENGTH],name[MAX_STRING_LENGTH];
  const char *point;
  int alias;

  ch = d->character;

  // check for prefix
  if (ch->prefix[0] != '\0' && str_prefix("prefix",argument))
  {
    if (strlen(ch->prefix) + strlen(argument) > MAX_INPUT_LENGTH)
      stc("Строка слишком длинная.\r\n",ch);
    else
    {
      do_printf(prefix,"%s %s",ch->prefix,argument);
      argument = prefix;
    }
  }

  if (IS_NPC(ch) || ch->pcdata->alias[0] == NULL
    ||  !str_prefix(argument,"alias") || !str_prefix(argument,"unalias")
    ||  !str_prefix(argument,"prefix"))
  {
    interpret(d->character,argument);
    return;
  }

  strcpy(buf,argument);

  for (alias = 0; alias < MAX_ALIAS; alias++)
  {
    if (ch->pcdata->alias[alias] == NULL) break;

    if (!str_prefix(ch->pcdata->alias[alias],argument))
    {
      point = one_argument(argument,name);
      if (!strcmp(ch->pcdata->alias[alias],name))
      {
        if (( strlen(ch->pcdata->alias_sub[alias])+strlen(point) ) > MAX_INPUT_LENGTH - 6)
        {
          stc("Запись слишком длинная. Обрезано.\r\n",ch);
          buf[MAX_INPUT_LENGTH -1] = '\0';
        }
        else
        {
         buf[0] = '\0';
         strcat(buf,ch->pcdata->alias_sub[alias]);
         strcat(buf," ");
         strcat(buf,point);
        }
        break;
      }
    }
  }
  interpret(d->character,buf);
}

void do_alias(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *rch;
  char arg[MAX_STRING_LENGTH];
  int pos;

  if (ch->desc == NULL) rch = ch;
  else rch = ch;

  argument = one_argument(argument,arg);

  if (arg[0] == '\0')
  {
    if (rch->pcdata->alias[0] == NULL)
    {
      stc("У тебя нет ни одной зарезервированной команды.\n\r",ch);
      return;
    }
    stc("Твои зарезервированные команды:\n\r",ch);

    for (pos = 0; pos < MAX_ALIAS; pos++)
    {
      if (rch->pcdata->alias[pos] == NULL
        ||  rch->pcdata->alias_sub[pos] == NULL) break;

      ptc(ch,"    %s:  %s\n\r",rch->pcdata->alias[pos],rch->pcdata->alias_sub[pos]);
    }
    return;
  }

  if (!str_prefix("una",arg) || !str_cmp("alias",arg))
  {
    stc("Это слово зарезервировано.\n\r",ch);
    return;
  }

  if (argument[0] == '\0')
  {
    for (pos = 0; pos < MAX_ALIAS; pos++)
    {
      if (rch->pcdata->alias[pos] == NULL
        ||  rch->pcdata->alias_sub[pos] == NULL) break;

      if (!str_cmp(arg,rch->pcdata->alias[pos]))
      {
        ptc(ch, "%s теперь означает '%s'.\n\r",rch->pcdata->alias[pos],
                        rch->pcdata->alias_sub[pos]);
        return;
      }
    }
    stc("Зарезервированные слова не определены.\n\r",ch);
    return;
  }

  if (!str_prefix(argument,"delete") || !str_prefix(argument,"prefix"))
  {
    stc("Это не будет сделано.\n\r",ch);
    return;
  }

  for (pos = 0; pos < MAX_ALIAS; pos++)
  {
    if (rch->pcdata->alias[pos] == NULL) break;

    if (!str_cmp(arg,rch->pcdata->alias[pos])) /* redefine an alias */
    {
      free_string(rch->pcdata->alias_sub[pos]);
      rch->pcdata->alias_sub[pos] = str_dup(argument);
      ptc(ch, "%s переназначено на '%s'.\n\r",arg,argument);
      return;
    }
  }

  if (pos >= MAX_ALIAS)
  {
    stc("У тебя уже достаточно зарезервированных команд.\n\r",ch);
    return;
  }
  rch->pcdata->alias[pos]            = str_dup(arg);
  rch->pcdata->alias_sub[pos]        = str_dup(argument);
  ptc(ch, "%s теперь означает '%s'.\n\r",arg,argument);
}

void do_unalias(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *rch;
  char arg[MAX_STRING_LENGTH];
  int pos;
  bool found = FALSE;

  if (ch->desc == NULL) rch = ch;
  else  rch = ch;

  argument = one_argument(argument,arg);

  if (arg[0] == '\0')
  {
    stc("Убрать какую команду?\n\r",ch);
    return;
  }

  for (pos = 0; pos < MAX_ALIAS; pos++)
  {
    if (rch->pcdata->alias[pos] == NULL) break;

    if (found)
    {
      rch->pcdata->alias[pos-1]           = rch->pcdata->alias[pos];
      rch->pcdata->alias_sub[pos-1]       = rch->pcdata->alias_sub[pos];
      rch->pcdata->alias[pos]             = NULL;
      rch->pcdata->alias_sub[pos]         = NULL;
      continue;
    }

    if(!strcmp(arg,rch->pcdata->alias[pos]))
    {
      stc("Зарезервированная команда убрана.\n\r",ch);
      free_string(rch->pcdata->alias[pos]);
      free_string(rch->pcdata->alias_sub[pos]);
      rch->pcdata->alias[pos] = NULL;
      rch->pcdata->alias_sub[pos] = NULL;
      found = TRUE;
    }
  }
  if (!found) stc("Эта зарезервированная команда неизвестна.\n\r",ch);
}

void do_dampool( CHAR_DATA *ch, const char *argument )
{
 int count;
 CHAR_DATA *victim=NULL;

 victim=get_pchar_world(ch,argument);
 if (!victim) victim=ch;

 ptc(ch,"Dampool for {Y%s{x:\n\r",victim->name);
 for (count=0;count<MAX_VICT;count++)
  ptc(ch,"Name: %s Dampool: %d\n\r",
  !victim->pcdata->victims[count].victim ? "none":get_char_desc(victim->pcdata->victims[count].victim,'1'),
   victim->pcdata->victims[count].dampool);
}

// Pick off one argument from a string and return the rest. Understands quotes.
char *one_arg( char *argument, char *arg_first )
{
 char cEnd;

 while ( isspace(*argument) ) argument++;

 cEnd = ' ';
 if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

 while ( *argument != '\0' )
 {
   if ( *argument == cEnd )
   {
    argument++;
    break;
   }
   *arg_first =*argument;
   arg_first++;
   argument++;
 }
 *arg_first = '\0';
 while ( isspace(*argument) ) argument++;
 return argument;
}

void do_tournament( CHAR_DATA *ch, const char *argument )
{
  char arg[256];
  char chn1[256],chn2[256];
  CHAR_DATA *ch1,*ch2;
  TOURNAMENT_DATA *t=tournament;
  BET_DATA *b,*b1;
  char str[256];
  int i,k;

  if (EMPTY(argument))
  {
    stc("tournament bet <имя> <qp> - сделать ставку\n\r",ch);
    stc("           info           - вывести информацию\n\r",ch);
    if (get_trust(ch) > 101)
    {
      stc("           propose <char1> <char2> <prize> - объявить турнир\n\r",ch);
      stc("           startbet - разрешить делать ставки\n\r",ch);
      stc("           fight    - начать бой\n\r",ch);
      stc("           cancel   - отменить турнир\n\r",ch);
      stc("           complete <имя> - выплатить призы, закончить турнир\n\r",ch);
    }
    return;
  }
    
  argument=one_argument(argument,arg);

  if (!str_prefix(arg,"info"))
  {
    if (!t)
    {
      stc("No active tournament found.\n\r",ch);
      return;
    }
       
    if ( get_trust(ch)< 102 )
    {
      ptc(ch,"{DTournament: {C%s {rvs. {C%s{x\n\r",t->ch1->name,t->ch2->name);
      return;
    }
    ptc(ch,"{DTournament: {C%s {rvs. {C%s{x, prize {Y%d qp\n\n\r{RBets:{x\n\r",t->ch1->name,t->ch2->name,t->prize);
    for ( b1=t->bet; b1; b1=b1->next )
      ptc(ch,"{G%s {xпоставил {Y%d qp{x на {C%s\n\r",b1->chname,b1->prize,b1->victimname);
    return;
  }

  if (!str_prefix(arg,"bet"))
  {
    int r;
        
    if (!t) {stc("Нет активных турниров.\n\r",ch); return;}
        
    if ( !t->isbet )
    {
      stc("Букмекер вышел покурить.\n\r",ch);
      return;
    }
       
    if ( ch->level < 5 && !ch->remort )
    {
      stc("Дорасти до 5 уровня.\n\r",ch);
      return;
    }
       
    argument=one_argument(argument,chn1);
    if ( !(ch1=get_pchar_world(ch,chn1)) )
    {
      stc("Нет такого игрока.\n\r",ch);
      return;
    }
       
    if ( !ch1->pcdata->tournament )
    {
      stc("Он не участник турнира.\n\r",ch);
      return;
    }
       
    if (!argument || !argument[0] || !is_number(argument))
    {
      stc("Сколько ты хочешь поставить?",ch);
      return;
    }
       
    r=atoi(argument);
    if ( ch->questpoints < 10 )
    {
      ptc(ch,"You got much-much qp, right?\n\r");
      return;
    }
       
    if ( !r || r > t->prize || r > ch->questpoints || r < 10 )
    {
      ptc(ch,"Ты можешь делать ставки от 10 до %d qp.\n\r",UMIN(t->prize,ch->questpoints));
      return;
    }
       
    if ( ch == ch1 || ch == t->ch2 )
    {
      stc("Букмекер вышел покурить. А Рефери уже ждет!\n\r",ch);
      return;
    }
       
    for ( b1=t->bet; b1; b1=b1->next )
      if ( !strcmp(b1->chname,ch->name) )
      {
        stc("Ты уже сделал ставку.",ch);
        return;
      }
       
/*
    if (!strcmp(ch->name,"Belka"))
    {
      ptc(ch,"{R{+WARNING{-{x{w: one {MBelka{c found in do_tournament()\n\r");
      stc("Ты полностью заморожен!\n\r",ch);
      //return; :)))))
    }
*/
    b=malloc(sizeof(BET_DATA));
    if (!b) return;
    b->next=t->bet;
    t->bet=b;
    strcpy(b->chname,ch->name);
    strcpy(b->victimname,ch1->name);
    b->prize=r;
    ch->questpoints-=r;
    t->bank+=r;
       
    ch->pcdata->bet=b;
       
    ptc(ch,"{wУдачи, {W%s. {wТы поставил {Y%d qp {xна {C%s{x",b->chname,b->prize,b->victimname);
    return;
  }

  if (!str_prefix(arg,"propose"))
  {
    if ( get_trust(ch)<102) return;

    if (t)
    {
      stc("Уже есть один турнир.\n\r",ch);
      return;
    }
        
    argument=one_argument(argument,chn1);
    argument=one_argument(argument,chn2);

    if (EMPTY(chn1) || EMPTY(chn2))
    {
      stc("Укажите имена игроков.\n\r",ch);
      return;
    }

    ch1=get_pchar_world(ch,chn1);
    ch2=get_pchar_world(ch,chn2);

    if (!ch1 || !ch2)
    {
      stc("Указанные игроки не найдены.\n\r",ch);
      return;
    }
        
    if (EMPTY(argument) || !is_number(argument))
    {
      stc("Не объявлен приз турнира.\n\r",ch);
      return;
    }
        
    if (ch1==ch2)
    {
      stc("Fatal error 0xFFA7. Call to Borland.\n\r",ch);
      return;
    }
        
    if ( ch1->pcdata->tournament || ch2->pcdata->tournament )
    {
      stc("Один из игроков уже принимает участие в турнире.\n\r",ch);
      return;
    }

    if ( ch1->level < 5 || ch2->level < 5 )
    {
      stc("Игроки должны достигнуть 5 уровня, чтобы принять участие в турнире.\n\r",ch);
      return;
    }
        
    if ( atoi(argument) < 10 )
    {
      ptc(ch,"А если тебе такой приз ?\n\r");
      return;
    }
        
    t=malloc(sizeof(TOURNAMENT_DATA));
    if (!t)
    {
      stc("Fatal error 0x00A7. Call to Borland.\n\r",ch);
      return;
    }
        
    t->next=tournament;
    tournament=t;
    t->bet=0;
    t->ch1=ch1;
    t->ch2=ch2;
    t->prize=URANGE(0,atoi(argument),1000);
    t->bank=0;
    t->vnum=1;
    t->isbet=0;
    t->isfight=0;
        
    ch1->pcdata->tournament=t;
    ch2->pcdata->tournament=t;
        
    do_printf(str,"\n\r{D[Tournament]{w: {xПредлагается турнир: {C%s {rvs. {C%s.{x Приз: {Y%d qp{x\n\r",ch1->name,ch2->name,t->prize);
    gecho(str);
    return;
  }

  if (!str_prefix(arg,"startbet"))
  {
    if ( get_trust(ch)<102) return;
    if (!t) { stc("Нет активных турниров.\n\r",ch); return; }
    t->isbet=1;
    do_printf(str,"\n\r{D[Tournament]{w: {WДелайте Ваши ставки, господа! {C%s {rvs. {C%s.{x Приз: {Y%d qp{x\n\r",t->ch1->name,t->ch2->name,t->prize);
    gecho(str);
    return;
  }

  if (!str_prefix(arg,"fight"))
  {
    if ( get_trust(ch)< 102 ) return;
    t->isbet=0;
    t->isfight=1;
    do_printf(str,"\n\r{D[Tournament]{w: {YСтавки больше не принимаются. {wLet the {RFIGHT {wbegin!!!{x\n\r");
    gecho(str);
    return;
  }

  if (!str_prefix(arg,"cancel"))
  {
    if ( get_trust(ch)< 105 ) return;
       
    if (!t) 
    {
      stc("No tournaments active now.\n\r",ch);
      return;
    }
       
    t->ch1->pcdata->tournament=0;
    t->ch2->pcdata->tournament=0;
    do_printf(str,"\n\r{D[Tournament]{w: {CТурнир {Rотменен.{x\n\r");
    gecho(str);
       
    for ( b1=t->bet; b1; b1=b1->next )
    {
      ch1=get_pchar_world(ch,b1->chname);
      if (!ch1) continue;
           
      ch1->pcdata->bet=0;
      ch1->questpoints+=b1->prize;
      stc("Ты получил назад свою ставку.\n\r",ch1);
    }
    tournament=0;
    return;
  }

  if (!str_prefix(arg,"complete"))
  {
    if ( get_trust(ch)< 105 ) return;
       
    if (!t) 
    {
      stc("No tournaments active now.\n\r",ch);
      return;
    }
       
    argument=one_argument(argument,chn1);
    ch1=get_pchar_world(ch,chn1);
       
    if (!ch1)
    {
      stc("Нет такого игрока.\n\r",ch);
      return;
    }
       
    if ( !ch1->pcdata->tournament )
    {       
      stc("Он не участник турнира.",ch);
      return;
    }
       
    ch1->questpoints+=t->prize;
    ptc(ch1,"{WПоздравляем, {C%s,{W ты выиграл турнир.{c Ты получаешь {Y%d qp{x\n\r",ch1->name,t->prize);
       
    t->ch1->pcdata->tournament=0;
    t->ch2->pcdata->tournament=0;
    do_printf(str,"\n\r{D[Tournament]{w: {C%s {xWINS. {RFatality.{x\n\r",ch1->name);
    gecho(str);
       
    i=0; //total winners' bets
    for ( b1=t->bet; b1; b1=b1->next )
    {
      ch2=get_pchar_world(ch,b1->chname);
      if (!ch2) continue;
      if (!strcmp(b1->victimname,ch1->name)) i+=b1->prize;
    }
       
    for ( b1=t->bet; b1; b1=b1->next )
    {
      ch2=get_pchar_world(ch,b1->chname);
      if (!ch2) continue;
      ch2->pcdata->bet=0;
           
      if (!strcmp(b1->victimname,ch1->name))
      {
        k=b1->prize*t->bank/i;
        ptc(ch2,"{WДа есть же! {cТы выиграл {Y%d qp{x!\n\r",k-(b1->prize));
        ch2->questpoints+=k;
      }
    }
    tournament=0;
    return;
  }
  do_tournament(ch,"");
}

void do_test( CHAR_DATA *ch, const char *argument )
{
  act("[$c1] [$c2] [$c3]",ch,NULL,NULL,TO_CHAR);
}
