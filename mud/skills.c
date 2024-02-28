// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"

// command procedures needed
DECLARE_DO_FUN2(do_groups        );
DECLARE_DO_FUN2(do_help          );
DECLARE_DO_FUN2(do_showskill     );
DECLARE_DO_FUN2(do_say           );
DECLARE_DO_FUN2(do_ahelp         );
int min_level(CHAR_DATA *ch,int sn);
int skill_cost(CHAR_DATA *ch,int sn);
int group_cost(CHAR_DATA *ch,int gn);
int gain_skill_cost(CHAR_DATA *ch,int sn);
int gain_group_cost(CHAR_DATA *ch,int gn);

// used to Gain new skills
void do_gain(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *trainer;
  int gn = 0, sn = 0;

  // find a trainer
  for ( trainer = ch->in_room->people; trainer != NULL; 
        trainer = trainer->next_in_room)
   if (IS_NPC(trainer) && IS_SET(trainer->act,ACT_GAIN)) break;

  if (trainer == NULL || !can_see(ch,trainer,NOCHECK_LVL))
  {
    stc("Ты не можешь сделать это тут.\n\r",ch);
    return;
  }

  one_argument(argument,arg);
  if (EMPTY(arg))
  {
    stc ("Use:gain list    - вывести список доступного для изучения\n\r",ch);
    stc ("    gain <name>  - изучить навык или группу заклинаний\n\r",ch);
    stc ("    gain convert - конвертировать 10 практик в 1 тренировку\n\r",ch);
    stc ("    gain revert  - конвертировать 1 тренировку в 10 практик\n\r",ch);
    stc ("    gain points  - уменьшить кол-во опыта на уровень(минимум 1000)\n\r",ch);
    return;
  }
  if (!str_prefix(arg,"convert"))
  {
    if (ch->practice < 10)
    {
      act("$C1 говорит тебе 'Ты еще не готов.'",ch,NULL,trainer,TO_CHAR);
      return;
    }
    act("$C1 помогает тебе перевести практики в тренировку.",ch,NULL,trainer,TO_CHAR);
    ch->practice -= 10;
    ch->train +=1 ;
    return;
  }

  if (!str_prefix(arg,"revert"))
  {
    if (ch->train < 1)
    {
      act("$C1 говорит тебе 'Ты еще не готов.'",ch,NULL,trainer,TO_CHAR);
      return;
    }

    act("$C1 помогает тебе перевести тренировку в практики.",ch,NULL,trainer,TO_CHAR);
    ch->train -= 1;
    ch->practice +=10 ;
    return;
  }

  if (!str_prefix(arg,"list"))
  {
    int col;

    col = 0;
    ptc(ch, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
                   "group","cost","group","cost","group","cost");

    for (gn = 0; gn < MAX_GROUP; gn++)
    {
      if (group_table[gn].name == NULL) break;

      if (!ch->pcdata->group_known[gn] && gain_group_cost(ch,gn) > 0)
      {
        ptc(ch,"%-18s %-5d ", group_table[gn].name,gain_group_cost(ch,gn));
        if (++col % 3 == 0) stc("\n\r",ch);
      }
    }
    if (col % 3 != 0) stc("\n\r",ch);
    stc("\n\r",ch);         
    col = 0;
    ptc(ch, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
                 "skill","cost","skill","cost","skill","cost");

    for (sn = 0; sn < max_skill; sn++)
    {
      if (skill_table[sn].name == NULL) break;
      if (!ch->pcdata->learned[sn] &&  gain_skill_cost(ch,sn) > 0
            && skill_table[sn].spell_fun == spell_null
            && !CLANSPELL(sn))
      {
        ptc(ch,"%-18s %-5d ", skill_table[sn].name,gain_skill_cost(ch,sn));
        if (++col % 3 == 0) stc("\n\r",ch);
      }
    }
    if (col % 3 != 0) stc("\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"points"))
  {
    if (ch->remort>0)
    {
      stc("MLT не могут использовать эту команду",ch);
      return;
    }

    if (ch->train < 2)
    {
      act("$C1 говорит тебе 'Ты еще не готов.'", ch,NULL,trainer,TO_CHAR);
      return;
    }

    if (ch->pcdata->points <= 40)
    {
      act("$C1 говорит тебе 'Ниже спустить уже нельзя.'",ch,NULL,trainer,TO_CHAR);
      return;
    }

    act("$C1 провел с тобой занятие, снимая бремя умений с твоих плеч.",ch,NULL,trainer,TO_CHAR);

    ch->train -= 2;
    ch->pcdata->points -= 1;
    ch->exp = exp_per_level(ch,ch->pcdata->points) * ch->level;
    return;
  }

  // else add a group/skill
  sn = skill_lookup(argument);
  if ( IS_SET(skill_table[sn].flag,S_CLAN) && !IS_IMMORTAL(ch) )
   return;
  gn = group_lookup(argument);
  if (gn > 0)
  {
    if (ch->pcdata->group_known[gn])
    {
      act("$C1 говорит тебе 'Ты уже знаешь эту группу!'",ch,NULL,trainer,TO_CHAR);
      return;
    }

    if ( gain_group_cost(ch,gn) <= 0)
    {
      act("$C1 говорит тебе 'Ты не можешь это изучить эту группу.'",ch,NULL,trainer,TO_CHAR);
      return;
    }
    if (ch->train < gain_group_cost(ch,gn))
    {
      act("$C1 говорит тебе 'Ты еще не готов к обучению.'",ch,NULL,trainer,TO_CHAR);
      return;
    }

    /* add the group */
    gn_add(ch,gn);
    act("$C1 обучает тебя искусству $t",ch,group_table[gn].name,trainer,TO_CHAR);
    ch->train -= gain_group_cost(ch,gn);
    return;
  }

  sn = skill_lookup(argument);
  if (sn > -1)
  {
    if (skill_table[sn].spell_fun != spell_null)
    {
      act("$C1 говорит тебе 'Ты должен выучить всю группу.'",ch,NULL,trainer,TO_CHAR);
      return;
    }
    if (ch->pcdata->learned[sn])
    {
      act("$C1 говорит тебе 'Ты уже владеешь этим!'",ch,NULL,trainer,TO_CHAR);
      return;
    }
    if (gain_skill_cost(ch,sn) <= 0)
    {
      act("$C1 говорит тебе 'Я не могу обучить тебя этому.'",ch,NULL,trainer,TO_CHAR);
      return;
    }
    if (ch->train < gain_skill_cost(ch,sn))
    {
      act("$C1 говорит тебе 'Ты еще не готов к обучению.'",ch,NULL,trainer,TO_CHAR);
      return;
    }

    /* add the skill */
    ch->pcdata->learned[sn] = 1;
    act("$C1 обучил тебя искусству $t",ch,skill_table[sn].name,trainer,TO_CHAR);
    ch->train -= gain_skill_cost(ch,sn);
    return;
  }
  act("$C1 говорит тебе 'Я не понял...'",ch,NULL,trainer,TO_CHAR);
}
    
// RT spells and skills show the players spells (or skills)
void do_spells(CHAR_DATA *ch, const char *argument)
{
  BUFFER *buffer;
  char arg[MAX_INPUT_LENGTH];
  char spell_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
  char spell_columns[LEVEL_HERO + 1];
  int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
  bool fAll = FALSE, found = FALSE;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  if (argument[0] != '\0')
  {
    fAll = TRUE;

    if (str_prefix(argument,"all"))
    {
      argument = one_argument(argument,arg);
      if (!is_number(arg))
      {
        stc("Аргумент - числовое значение или ALL.\n\r",ch);
        return;
      }
      max_lev = atoi(arg);

      if (max_lev < 1 || max_lev > LEVEL_HERO)
      {
        ptc(ch,"Уровень должен быть между 1 и %d.\n\r",LEVEL_HERO);
        return;
      }

      if (argument[0] != '\0')
      {
        argument = one_argument(argument,arg);
        if (!is_number(arg))
        {
          stc("Аргумент - числовое значение или ALL.\n\r",ch);
          return;
        }
        min_lev = max_lev;
        max_lev = atoi(arg);

        if (max_lev < 1 || max_lev > LEVEL_HERO)
        {
          ptc(ch,"Уровень должен быть между 1 и %d.\n\r",LEVEL_HERO);
          return;
        }
        if (min_lev > max_lev)
        {
          stc("Идиотизм какой-то...\n\r",ch);
          return;
        }
      }
    }
  }

  // initialize data
  for (level = 0; level < LEVEL_HERO + 1; level++)
  {
    spell_columns[level] = 0;
    spell_list[level][0] = '\0';
  }

  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name == NULL )  break;

    if ((level = min_level(ch,sn)) < LEVEL_HERO + 1
    &&  (fAll || level <= ch->level)
    &&  level >= min_lev && level <= max_lev
    &&  skill_table[sn].spell_fun != spell_null
    &&  ch->pcdata->learned[sn] > 0
    && check_clanspell(sn,ch->clan))
    {
      found = TRUE;
      level = min_level(ch,sn);
      if (ch->level < level)
        do_printf(buf,"%-18s {rn/a{x      ", skill_table[sn].name);
      else do_printf(buf,"%-18s  {w%3d маны{x  ",skill_table[sn].name,mana_cost(ch,sn));

      if (spell_list[level][0] == '\0')
        do_printf(spell_list[level],"\n\r{CУровень %2d:{x   %s",level,buf);
      else /* append */
      {
        if ( ++spell_columns[level] % 2 == 0) strcat(spell_list[level],"\n\r              ");
        strcat(spell_list[level],buf);
      }
    }
  }

  /* return results */
  if (!found)
  {
    stc("- заклинаний не обнаружено -\n\r",ch);
    return;
  }

  buffer = new_buf();
  for (level = 0; level < LEVEL_HERO + 1; level++)
    if (spell_list[level][0] != '\0') add_buf(buffer,spell_list[level]);
  add_buf(buffer,"\n\r");
  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

void do_skills(CHAR_DATA *ch, const char *argument)
{
  BUFFER *buffer;
  char arg[MAX_INPUT_LENGTH];
  char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
  char skill_columns[LEVEL_HERO + 1];
  int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
  bool fAll = FALSE, found = FALSE;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  if (argument[0] != '\0')
  {
    fAll = TRUE;

    if (str_prefix(argument,"all"))
    {
      argument = one_argument(argument,arg);
      if (!is_number(arg))
      {
        stc("Аргумент - числовое значение или ALL.\n\r",ch);
        return;
      }
      max_lev = atoi(arg);

      if (max_lev < 1 || max_lev > LEVEL_HERO)
      {
        ptc(ch,"Уровень должен быть между 1 и %d.\n\r",LEVEL_HERO);
        return;
      }

      if (argument[0] != '\0')
      {
        argument = one_argument(argument,arg);
        if (!is_number(arg))
        {
          stc("Аргумент - числовое значение или ALL.\n\r",ch);
          return;
        }
        min_lev = max_lev;
        max_lev = atoi(arg);
        if (max_lev < 1 || max_lev > LEVEL_HERO)
        {
          ptc(ch,"Уровень должен быть между 1 и %d.\n\r",LEVEL_HERO);
          return;
        }
        if (min_lev > max_lev)
        {
          stc("Идиотизм какой-то...\n\r",ch);
          return;
        }
      }
    }
  }
  for (level = 0; level < LEVEL_HERO + 1; level++)
  {
    skill_columns[level] = 0;
    skill_list[level][0] = '\0';
  }

  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name == NULL ) break;

    if ((level = min_level(ch,sn)) < LEVEL_HERO + 1
    &&  (fAll || level <= ch->level)
    &&  level >= min_lev && level <= max_lev
    &&  skill_table[sn].spell_fun == spell_null
    &&  ch->pcdata->learned[sn] > 0
    && check_clanspell(sn,ch->clan))
    {
      found = TRUE;
      level = min_level(ch,sn);
      if (ch->level < level)
           do_printf(buf,"%-18s {rn/a{x      ", skill_table[sn].name);
      else do_printf(buf,"%-18s {w%3d%%{x      ",skill_table[sn].name,ch->pcdata->learned[sn]);

      if (skill_list[level][0] == '\0')
          do_printf(skill_list[level],"\n\r{CУровень %2d:   {x %s",level,buf);
      else /* append */
      {
        if ( ++skill_columns[level] % 2 == 0)
            strcat(skill_list[level],"\n\r               ");
        strcat(skill_list[level],buf);
      }
    }
  }
  if (!found)
  {
    stc("- умений не обнаружено -\n\r",ch);
    return;
  }

  buffer = new_buf();
  for (level = 0; level < LEVEL_HERO + 1; level++)
    if (skill_list[level][0] != '\0') add_buf(buffer,skill_list[level]);
  add_buf(buffer,"\n\r");
  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

// shows skills, groups and costs (only if not bought)
void list_group_costs(CHAR_DATA *ch)
{
  int gn,sn,col;

  if (IS_NPC(ch)) return;

  col = 0;
  ptc(ch,"%-18s %-5s %-18s %-5s %-18s %-5s\n\r","group","cp","group","cp","group","cp");

  for (gn = 0; gn < MAX_GROUP; gn++)
  {
    if (group_table[gn].name == NULL) break;

    if (!ch->gen_data->group_chosen[gn] 
    &&  !ch->pcdata->group_known[gn]
    &&  group_cost(ch,gn) > 0)
    {
        ptc(ch,"%-18s %-5d ",group_table[gn].name,group_cost(ch,gn));
        if (++col % 3 == 0) stc("\n\r",ch);
    }
  }
  if ( col % 3 != 0 ) stc( "\n\r", ch );
  stc("\n\r",ch);
  col = 0;
  ptc(ch,"%-18s %-5s %-18s %-5s %-18s %-5s\n\r","skill","cp","skill","cp","skill","cp");

  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name == NULL) break;

    if (!ch->gen_data->skill_chosen[sn] 
    &&  ch->pcdata->learned[sn] == 0
    &&  skill_table[sn].spell_fun == spell_null
    &&  skill_cost(ch,sn) > 0
    &&  !IS_SET(skill_table[sn].flag,S_CLAN) ) 
    {
        ptc(ch,"%-18s %-5d ",skill_table[sn].name,skill_cost(ch,sn));
        if (++col % 3 == 0) stc("\n\r",ch);
    }
  }
  if ( col % 3 != 0 ) stc( "\n\r", ch );
  stc("\n\r",ch);

  ptc(ch,"Creation points: %d\n\r",ch->pcdata->points);
  ptc(ch,"Experience per level: %d\n\r",
          exp_per_level(ch,ch->gen_data->points_chosen));
}

// calls from customize chars
void list_group_chosen(CHAR_DATA *ch)
{
  int gn,sn,col;

  if (IS_NPC(ch)) return;

  col = 0;
  ptc(ch,"%-18s %-5s %-18s %-5s %-18s %-5s","group","cp","group","cp","group","cp\n\r");

  for (gn = 0; gn < MAX_GROUP; gn++)
  {
   if (group_table[gn].name == NULL) break;

   if (ch->gen_data->group_chosen[gn] && group_cost(ch,gn) > 0)
      {
          ptc(ch,"%-18s %-5d ",group_table[gn].name,group_cost(ch, gn));
          if (++col % 3 == 0) stc("\n\r",ch);
      }
  }
  if ( col % 3 != 0 ) stc( "\n\r", ch );
  stc("\n\r",ch);

  col = 0;
  ptc(ch,"%-18s %-5s %-18s %-5s %-18s %-5s","skill","cp","skill","cp","skill","cp\n\r");
  for (sn = 0; sn < max_skill; sn++)
  {
   if (skill_table[sn].name == NULL) break;

   if (ch->gen_data->skill_chosen[sn] &&  skill_cost(ch, sn) > 0
              && skill_table[sn].spell_fun == spell_null)
      {
          ptc(ch,"%-18s %-5d ",skill_table[sn].name,skill_cost(ch,sn));
          if (++col % 3 == 0) stc("\n\r",ch);
      }
  }
  if ( col % 3 != 0 ) stc( "\n\r", ch );
  stc("\n\r",ch);

  ptc(ch,"Creation points: %d\n\r",ch->gen_data->points_chosen);
  ptc(ch,"Experience per level: %d\n\r",
          exp_per_level(ch,ch->gen_data->points_chosen));
}

int exp_per_level(CHAR_DATA *ch, int points)
{
  int expl,inc;

  if (IS_NPC(ch)) return 1000; 
  expl = 1000;
  inc = 500;

  if (IS_SET(ch->act,PLR_LASTREMORT)) return 10000;
  if (IS_SET(ch->act,PLR_5REMORT)) return 1000;
  if (points <= 40 && ch->remort!=0)  return 1000+1000*ch->remort;

  if (points <= 40)
    return 1000*ch->remort + 1000 * (race_table[ch->race].class_mult[ch->class[ch->remort]] ?
                   race_table[ch->race].class_mult[ch->class[ch->remort]]/100 : 1); 

  points -= 40;

  while (points > 9)
  {
    expl += inc;
    points -= 10;
    if (points > 9)
    {
      expl += inc;
      inc *= 2;
      points -= 10;
    }
  }
  expl += points * inc / 10;  
  return (expl * race_table[ch->race].class_mult[ch->class[ch->remort]]/100)+1000*ch->remort;
}

// this procedure handles the input parsing for the skill generator
bool parse_gen_groups(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int gn,sn,i;

  if (argument[0] == '\0') return FALSE;

  argument = one_argument(argument,arg);

  if (!str_prefix(arg,"help"))
  {
    if (argument[0] == '\0')
    {
      do_help(ch,"group help");
      return TRUE;
    }
    do_help(ch,argument);
    return TRUE;
  }

  if (!str_prefix(arg,"showskill"))
  {
    if (argument[0] == '\0')
    {
      do_showskill(ch,"");
      return TRUE;
    }
    do_showskill(ch,argument);
    return TRUE;
  }

  if (!str_prefix(arg,"ahelp"))
  {
    if (argument[0] == '\0')
    {
      do_ahelp(ch,"");
      return TRUE;
    }
    do_ahelp(ch,argument);
    return TRUE;
  }

  if (!str_prefix(arg,"add"))
  {
    if (argument[0] == '\0')
    {
      stc("Ты должен указать название умения.\n\r",ch);
      return TRUE;
    }

    gn = group_lookup(argument);
    if (gn != -1)
    {
      if (ch->gen_data->group_chosen[gn])
      {
        stc("Ты уже знаешь эту группу!\n\r",ch);
        return TRUE;
      }

      if (group_cost(ch,gn) < 1)
      {
        stc("Эта группа тебе недоступна.\n\r",ch);
        return TRUE;
      }

      ptc(ch,"Группа %s добавлена к списку\n\r",group_table[gn].name);
      ch->gen_data->group_chosen[gn] = TRUE;
      ch->gen_data->points_chosen += group_cost(ch,gn);
      gn_add(ch,gn);
      group_add(ch,argument,FALSE);
      ch->pcdata->points += group_cost(ch,gn);
      ptc(ch,"Creation points: %d\n\r",ch->pcdata->points);
      ptc(ch,"Experience per level: %d\n\r",
        exp_per_level(ch,ch->gen_data->points_chosen));
      return TRUE;
    }

    sn = skill_lookup(argument);
    if (sn != -1)
    {
     if ( IS_SET(skill_table[sn].flag,S_CLAN) ) 
      return TRUE;
     if (ch->gen_data->skill_chosen[sn])
     {
       stc("Ты уже владеешь этим навыком!\n\r",ch);
       return TRUE;
     }
     if (skill_cost(ch,sn) < 1 || skill_table[sn].spell_fun != spell_null)
     {
      stc("Тебе это умение недоступно.\n\r",ch);
      return TRUE;
     }
     ptc(ch, "Навык %s добавлено к списку\n\r",skill_table[sn].name);
     ch->gen_data->skill_chosen[sn] = TRUE;
     ch->gen_data->points_chosen += skill_cost(ch,sn);
     ch->pcdata->learned[sn] = 1;
     ch->pcdata->points += skill_cost(ch,sn);
     return TRUE;
    }

    stc("Такого навыка или группы не существует.\n\r",ch);
    return TRUE;
  }

  if (!strcmp(arg,"drop"))
  {
    if (argument[0] == '\0')
    {
      stc("Ты должен указать название группы или навыка.\n\r",ch);
      return TRUE;
    }
    gn = group_lookup(argument);
    if (gn != -1 && ch->gen_data->group_chosen[gn]
        && group_cost(ch,gn)>0 && group_cost(ch,gn)<40)
    {
     stc("Группа убрана.\n\r",ch);
     ch->gen_data->group_chosen[gn] = FALSE;
     ch->gen_data->points_chosen -= group_cost(ch,gn);
     gn_remove(ch,gn);
     group_remove(ch,argument);
     for (i = 0; i < MAX_GROUP; i++)
      if (ch->gen_data->group_chosen[gn]) gn_add(ch,gn);
     ch->pcdata->points -= group_cost(ch,gn);
     ptc(ch,"Creation points: %d\n\r",ch->pcdata->points);
     ptc(ch,"Experience per level: %d\n\r",
       exp_per_level(ch,ch->gen_data->points_chosen));
     return TRUE;
    }

    sn = skill_lookup(argument);
    if (sn != -1 && ch->gen_data->skill_chosen[sn] 
            && skill_table[sn].spell_fun == spell_null)
    {
      stc("Навык убран.\n\r",ch);
      ch->gen_data->skill_chosen[sn] = FALSE;
      ch->gen_data->points_chosen -= skill_cost(ch,sn);
      ch->pcdata->learned[sn] = 0;
      ch->pcdata->points -= skill_cost(ch,sn);
      return TRUE;
    }
    stc("У тебя нет ничего с подобным названием.\n\r",ch);
    return TRUE;
  }

  if (!str_prefix(arg,"premise"))
  {
    do_help(ch,"premise");
    return TRUE;
  }

  if (!str_prefix(arg,"list"))
  {
    list_group_costs(ch);
    return TRUE;
  }

  if (!str_prefix(arg,"learned"))
  {
    list_group_chosen(ch);
    return TRUE;
  }

  if (!str_prefix(arg,"info"))
  {
    do_groups(ch,argument);
    return TRUE;
  }
  return FALSE;
}
            
// shows all groups, or the sub-members of a group
void do_groups(CHAR_DATA *ch, const char *argument)
{
  int gn,sn,col;

  if (IS_NPC(ch)) return;

  col = 0;
  if (argument[0] == '\0')
  {
    for (gn = 0; gn < MAX_GROUP; gn++)
    {
      if (group_table[gn].name == NULL) break;
      if (ch->pcdata->group_known[gn])
      {
        ptc(ch,"%-20s ",group_table[gn].name);
        if (++col % 3 == 0) stc("\n\r",ch);
      }
    }
    if ( col % 3 != 0 ) stc( "\n\r", ch );
    ptc(ch,"Creation points: %d\n\r",ch->pcdata->points);
    return;
  }

  if (!str_cmp(argument,"all"))
  {
    for (gn = 0; gn < MAX_GROUP; gn++)
    {
      if (group_table[gn].name == NULL) break;
      ptc(ch,"%-20s ",group_table[gn].name);
      if (++col % 3 == 0) stc("\n\r",ch);
    }
    if ( col % 3 != 0 ) stc( "\n\r", ch );
    return;
  }
      
  // show the sub-members of a group
  gn = group_lookup(argument);
  if (gn == -1)
  {
    stc("Группы с таким названием нет.\n\r",ch);
    stc("Набери 'groups all' или 'inform all' для полного списка.\n\r",ch);
    return;
  }

  for (sn = 0; sn < MAX_IN_GROUP; sn++)
  {
    if (group_table[gn].spells[sn] == NULL)  break;
    ptc(ch,"%-20s ",group_table[gn].spells[sn]);
    if (++col % 3 == 0) stc("\n\r",ch);
  }
  if ( col % 3 != 0 ) stc( "\n\r", ch );
}

// checks for skill improvement
void check_improve( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
  int chance;
  int rll;

  rll = 100;

  if (IS_NPC(ch))
      return;

  if (ch->level < min_level(ch,sn)
  ||  ch->pcdata->learned[sn] == 0 /* dodelat'? */
  ||  ch->pcdata->learned[sn] == 100)
      return;  /* skill is not known */ 

                      
  if ((ch->classmag) && (rll > skill_table[sn].rating[0]) && (skill_table[sn].rating[0] != 0)) 
                            rll = skill_table[sn].rating[0];     
  if ((ch->classcle) && (rll > skill_table[sn].rating[1]) && (skill_table[sn].rating[1] != 0)) 
                            rll = skill_table[sn].rating[1];     
  if ((ch->classthi) && (rll > skill_table[sn].rating[2]) && (skill_table[sn].rating[2] != 0)) 
                            rll = skill_table[sn].rating[2];     
  if ((ch->classwar) && (rll > skill_table[sn].rating[3]) && (skill_table[sn].rating[3] != 0)) 
                            rll = skill_table[sn].rating[3];     

      if (rll == 100) return; 

  chance = 10 * int_app[get_curr_stat(ch,STAT_INT)].learn;
  chance /= ( multiplier * rll * 4);
  chance += ch->level;

  if (number_range(1,1000) > chance) return;

  /* now that the character has a CHANCE to learn, see if they really have */ 
  if (success)
  {
    chance = URANGE(5,100 - ch->pcdata->learned[sn], 95);
    if (number_percent() < chance)
    {
      ptc(ch,"Ты повышаешь навык {C%s{x!\n\r", skill_table[sn].name);
      ch->pcdata->learned[sn]++;
      gain_exp(ch,2 * rll );
      ptc(ch, "Ты теперь знаешь {C%s{x на {Y%d{x%%.\n\r", skill_table[sn].name, ch->pcdata->learned[sn]);
    }
  }
  else
  {
    chance = URANGE(5,ch->pcdata->learned[sn]/2,30);
    if (number_percent() < chance)
    {
      ptc(ch,"Ты учел свои ошибки, и твой навык {C%s{x повысился.\n\r",
          skill_table[sn].name);
      ch->pcdata->learned[sn] += number_range(1,3);
      ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn],100);
      gain_exp(ch,2 * rll);
      ptc(ch, "Ты теперь знаешь {C%s{x на {Y%d{x%%.\n\r", skill_table[sn].name, ch->pcdata->learned[sn]);
    }
  }
}

// returns a group index number given the name
int group_lookup( const char *name )
{
  int gn;

  for ( gn = 0; gn < MAX_GROUP; gn++ )
  {
    if (!group_table[gn].name) break;
    if ( LOWER(name[0]) == LOWER(group_table[gn].name[0])
      && !str_prefix( name, group_table[gn].name ) )  return gn;
  }
  return -1;
}

// recursively adds a group given its number -- uses group_add
void gn_add( CHAR_DATA *ch, int gn)
{
  int i;
  
  ch->pcdata->group_known[gn] = TRUE;
  for ( i = 0; i < MAX_IN_GROUP; i++)
  {
    if (!group_table[gn].spells[i]) break;
    group_add(ch,group_table[gn].spells[i],FALSE);
  }
}

// recusively removes a group given its number -- uses group_remove
void gn_remove( CHAR_DATA *ch, int gn)
{
  int i;

  ch->pcdata->group_known[gn] = FALSE;
  for ( i = 0; i < MAX_IN_GROUP; i ++)
  {
    if (group_table[gn].spells[i] == NULL) break;
    group_remove(ch,group_table[gn].spells[i]);
  }
}
        
// use for processing a skill or group for addition
void group_add( CHAR_DATA *ch, const char *name, bool deduct)
{
  int sn,gn;

  if (IS_NPC(ch)) return;

  sn = skill_lookup(name);
  if (sn != -1)
  {
    if (ch->pcdata->learned[sn] == 0) // i.e. not known
    {
      ch->pcdata->learned[sn] = 1;
      if (deduct) ch->pcdata->points += skill_table[sn].rating[ch->class[ch->remort]]; 
    }
    return;
  }
      
  gn = group_lookup(name);

  if (gn != -1)
  {
    if (ch->pcdata->group_known[gn] == FALSE)  
    {
      ch->pcdata->group_known[gn] = TRUE;
      if (deduct) ch->pcdata->points += group_table[gn].rating[ch->class[ch->remort]];
    }
    gn_add(ch,gn); // make sure all skills in the group are known
  }
}

/* used for processing a skill or group for deletion -- no points back! */
void group_remove(CHAR_DATA *ch, const char *name)
{
  int sn, gn;
  
  sn = skill_lookup(name);
  if (sn != -1)
  {
      ch->pcdata->learned[sn] = 0;
      return;
  }

  /* now check groups */
  gn = group_lookup(name);

  if (gn != -1 && ch->pcdata->group_known[gn] == TRUE)
  {
      ch->pcdata->group_known[gn] = FALSE;
      gn_remove(ch,gn);  /* be sure to call gn_add on all remaining groups */
  }
}

int min_level(CHAR_DATA *ch,int sn)
{
 int maglevel=102,thilevel=102,warlevel=102,clelevel=102,level;

 if (ch->classmag) maglevel=skill_table[sn].skill_level[0];
 if (ch->classcle) clelevel=skill_table[sn].skill_level[1];
 if (ch->classthi) thilevel=skill_table[sn].skill_level[2];
 if (ch->classwar) warlevel=skill_table[sn].skill_level[3];

 level=maglevel;
 if (level>clelevel) level=clelevel;
 if (level>thilevel) level=thilevel;
 if (level>warlevel) level=warlevel;
 return level;
}

/* 0 - group unavaible for this class*/
int skill_cost(CHAR_DATA *ch,int sn)
{
 int cost,cost1;

 cost1=skill_table[sn].rating[ch->class[ch->remort]];
 if (ch->remort==0) return cost1;

 cost=100;
 cost1=skill_table[sn].rating[0];
 if (ch->classmag && (cost>cost1 && cost1!=0)) cost=cost1;
 cost1=skill_table[sn].rating[1];
 if (ch->classcle && (cost>cost1 && cost1!=0)) cost=cost1;
 cost1=skill_table[sn].rating[2];
 if (ch->classthi && (cost>cost1 && cost1!=0)) cost=cost1;
 cost1=skill_table[sn].rating[3];
 if (ch->classwar && (cost>cost1 && cost1!=0)) cost=cost1;

 if (cost==100) return 0;
 return cost;
}

int group_cost(CHAR_DATA *ch,int gn)
{
 int cost,cost1;

 cost1=group_table[gn].rating[ch->class[ch->remort]];
 if (cost1==-1) cost1=0;
 if ( ch->remort==0 ) return cost1;

 cost=100;
 cost1=group_table[gn].rating[0];
 if (ch->classmag && (cost>cost1 && cost1!=-1)) cost=cost1;
 cost1=group_table[gn].rating[1];
 if (ch->classcle && (cost>cost1 && cost1!=-1)) cost=cost1;
 cost1=group_table[gn].rating[2];
 if (ch->classthi && (cost>cost1 && cost1!=-1)) cost=cost1;
 cost1=group_table[gn].rating[3];
 if (ch->classwar && (cost>cost1 && cost1!=-1)) cost=cost1;

 if (cost==100) return 0;
 return cost;
}

int gain_group_cost(CHAR_DATA *ch,int gn)
{
 int cost,cost1;

 cost1=group_table[gn].rating[ch->class[ch->remort]];
 if (cost1==-1) cost1=0;
 if ( ch->remort==0 ) return cost1;

 cost=100;
 cost1=group_table[gn].rating[0];
 if (ch->classmag && (cost>cost1 && cost1!=-1)) cost=cost1;
 cost1=group_table[gn].rating[1];
 if (ch->classcle && (cost>cost1 && cost1!=-1)) cost=cost1;
 cost1=group_table[gn].rating[2];
 if (ch->classthi && (cost>cost1 && cost1!=-1)) cost=cost1;
 cost1=group_table[gn].rating[3];
 if (ch->classwar && (cost>cost1 && cost1!=-1)) cost=cost1;

 if (cost==100) return 0;
 return cost;
}

int gain_skill_cost(CHAR_DATA *ch,int sn)
{
 int cost,cost1;

 cost1=skill_table[sn].rating[ch->class[ch->remort]];
 if (ch->remort==0) return cost1;
 cost=100;

 cost1=skill_table[sn].rating[0];
 if (ch->classmag==1 && (cost>cost1 && cost1!=0)) cost=cost1;
 cost1=skill_table[sn].rating[1];
 if (ch->classcle==1 && (cost>cost1 && cost1!=0)) cost=cost1;
 cost1=skill_table[sn].rating[2];
 if (ch->classthi==1 && (cost>cost1 && cost1!=0)) cost=cost1;
 cost1=skill_table[sn].rating[3];
 if (ch->classwar==1 && (cost>cost1 && cost1!=0)) cost=cost1;

 if (cost==100) return 0;
 return cost;
}
