// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"

void do_wiznet(CHAR_DATA *ch, const char *argument)
{
 int64 flag;
 char buf[MAX_STRING_LENGTH];

 if (EMPTY(argument))
 {
   if (!IS_SET(ch->wiznet,WIZ_ON)) strcat(buf,"off ");

   ptc(ch,"������� ������ Wiznet:\n\r",buf);
   flag=0;
   for (;;)
   {
     ptc(ch,"{M%15s %s     ",wiznet_table[flag].name,
     IS_SET(ch->wiznet,wiznet_table[flag].flag) ? "{G��� {x":"{R����{x");
     flag++;
     if (wiznet_table[flag].name == NULL) break;
     ptc(ch,"{M%15s %s\n\r",wiznet_table[flag].name,
     IS_SET(ch->wiznet,wiznet_table[flag].flag) ? "{G��� {x":"{R����{x");
     flag++;
     if (wiznet_table[flag].name == NULL) break;
   }
   stc("\n\r",ch);
   return;
 }

 if (!str_prefix(argument, "on") || !str_prefix(argument, "off"))
 {
   ch->wiznet=toggle_flag(ch,ch->wiznet,WIZ_ON,"�������������� ���� Wiznet", TRUE);
   return;
 }

  if (!str_prefix(argument,"show"))
  {
    buf[0] = '\0';
    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
    {
      if (wiznet_table[flag].level <= get_trust(ch))
      {
        strcat(buf,wiznet_table[flag].name);
        strcat(buf," ");
      }
    }

    ptc(ch,"����� Wiznet, ��������� ��� ����:\n\r%s\n\r",buf);
    return;
  }

  flag = wiznet_lookup(argument);
  if (flag == -1 || get_trust(ch) < wiznet_table[flag].level)
  {
    stc("����� ����� ���.\n\r",ch);
    return;
  }
  do_printf(buf,"Wiznet flag %s ",wiznet_table[flag].name);
  ch->wiznet=toggle_flag(ch,ch->wiznet,wiznet_table[flag].flag,buf,TRUE);
}

void wiznet(char *string,CHAR_DATA *ch,OBJ_DATA *obj,int64 flag,int min_level)
{
  DESCRIPTOR_DATA *d;
  if (NOPUBLIC) return;
  for (d = descriptor_list; d; d = d->next)
  {
    if (d->character && d->connected == CON_PLAYING
      && d->character!=ch && IS_SET(d->character->wiznet,WIZ_ON)
      && (flag==0 || IS_SET(d->character->wiznet,flag))
      && get_trust(d->character)>=min_level)
    {
      if (IS_SET(d->character->wiznet,WIZ_PREFIX)) stc("--> ",d->character);
      act_new(string,d->character,obj,ch,TO_CHAR,POS_DEAD);
    }
  }
}

// equips a character
void do_outfit (CHAR_DATA *ch, const char *argument)
{
  OBJ_DATA *obj;
  int i,sn;
  int64 vnum;

  if (!IS_SET(ch->act,PLR_ARMY))
  {
    if (ch->level > 5 || IS_NPC(ch))
    {
      stc("����� ��� ���!\n\r",ch);
      return;
    }

    if ((obj = get_eq_char(ch, WEAR_LIGHT)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_LIGHT);
    }

    if ((obj = get_eq_char(ch, WEAR_BODY)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_BODY);
    }

    // do the weapon thing
    if ((obj = get_eq_char(ch,WEAR_RHAND)) == NULL)
    {
      sn = 0;
      vnum = OBJ_VNUM_SCHOOL_SWORD;

      for (i = 0; weapon_table[i].name != NULL; i++)
      {
        if (ch->pcdata->learned[sn] < ch->pcdata->learned[*weapon_table[i].gsn])
        {
          sn = *weapon_table[i].gsn;
          vnum = weapon_table[i].vnum;
        }
      }

      obj = create_object(get_obj_index(vnum),0);
      obj_to_char(obj,ch);
      equip_char(ch,obj,WEAR_RHAND);
    }

    if (((obj = get_eq_char(ch,WEAR_RHAND)) == NULL
     || !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
     && (obj = get_eq_char(ch, WEAR_LHAND)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_LHAND);
    }
    ptc(ch,"%s ������ ���� ������ � ������.\n\r",SabAdron);
  }
  else
  {
    if ((obj = get_eq_char(ch, WEAR_BODY)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_ARMY_VEST), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_BODY);
    }

    if ((obj = get_eq_char(ch, WEAR_HEAD)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_ARMY_CAP), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_HEAD);
    }

    if ((obj = get_eq_char(ch, WEAR_FEET)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_ARMY_BOOTS), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_FEET);
    }

    if ((obj = get_eq_char(ch,WEAR_RHAND)) == NULL)
    {
      sn = 0;
      vnum = OBJ_VNUM_ARMY_SWORD; /* just in case! */

      for (i = 0; weapon_army_table[i].name != NULL; i++)
      {
        if (ch->pcdata->learned[sn] <
        ch->pcdata->learned[*weapon_army_table[i].gsn])
        {
          sn = *weapon_army_table[i].gsn;
          vnum = weapon_army_table[i].vnum;
        }
      }

      obj = create_object(get_obj_index(vnum),0);
      obj_to_char(obj,ch);
      equip_char(ch,obj,WEAR_RHAND);
    }

    if (((obj = get_eq_char(ch,WEAR_RHAND)) == NULL
     ||   !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
     &&  (obj = get_eq_char(ch, WEAR_LHAND)) == NULL)
    {
      obj = create_object(get_obj_index(OBJ_VNUM_ARMY_SHIELD), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      equip_char(ch, obj, WEAR_LHAND);
    }
    stc("{W����� ������ �������� ������ ���� ������� ���� ����������� ��������������{x.\n\r",ch);
  }
}

void do_nochannels(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("��������� �� ������� ����?", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch))
  {
    stc("�� �����.\n\r", ch);
    return;
  }

  if (IS_SET(victim->comm, COMM_NOCHANNELS))
  {
    REM_BIT(victim->comm, COMM_NOCHANNELS);
    stc("���� ��������� ���� ������������ ��������.\n\r", victim);
    stc("����� {GNOCHANNELS{x �����.\n\r", ch);
  }
  else
  {
    SET_BIT(victim->comm, COMM_NOCHANNELS);
    stc("���� ��������� ���� ������������ ��������.\n\r", victim);
    stc("����� {RNOCHANNELS{x ����������.\n\r", ch);
  }
}

void do_nogsocial(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("��������� gsocials ����?", ch);
    return;
  }
  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch))
  {
    stc("�� �����.\n\r", ch);
    return;
  }
  victim->comm=toggle_int64(victim->comm,COMM_NOGSOC);
  ptc(ch,"Gsocial %s ��� %s\n\r",IS_SET(victim->comm,COMM_NOGSOC)?"���������":"��������",victim->name);
}

void do_smote(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *vch;
  char *letter;
  const char *name;
  char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
  unsigned int matches = 0;

  if (!IS_SET(ch->talk,CBIT_EMOTE))
  {
    stc("������ ���������.\n\r", ch);
    return;
  }

  if (EMPTY(argument))
  {
    stc("� ����������.\n\r", ch);
    return;
  }

  if (strstr(argument,ch->name) == NULL)
  {
    stc("�� ������ ������� � ������ ���� ���.\n\r",ch);
    return;
  }

  stc(argument,ch);
  stc("\n\r",ch);

  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if (vch->desc == NULL || vch == ch) continue;

    if ((letter = strstr(argument,vch->name)) == NULL)
    {
      stc(argument,vch);
      stc("\n\r",vch);
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

    stc(temp,vch);
    stc("\n\r",vch);
  }
}

void do_bamfin(CHAR_DATA *ch, const char *argument)
{
  if (EMPTY(argument))
  {
    ptc(ch, "������ ������ ���������: %s\n\r",ch->pcdata->bamfin);
    return;
  }

  free_string(ch->pcdata->bamfin);
  ch->pcdata->bamfin = str_dup(argument);
  ptc(ch, "����� ������ ������ ���������: %s\n\r",ch->pcdata->bamfin);
}

void do_pseudoname(CHAR_DATA *ch, const char *argument)
{
   if (!*argument)
   {
     ptc(ch, "������ ������ talk: %s\n\r",ch->pcdata->pseudoname);
     return;
   }
   if (!str_prefix(argument,"clean"))
   {
     stc("Your pseudoname was set to your real name.\n\r",ch);
     free_string(ch->pcdata->pseudoname);
     ch->pcdata->pseudoname = str_dup(ch->name);
     return;
   }
   free_string(ch->pcdata->pseudoname);
   ch->pcdata->pseudoname = str_dup(argument);
   ptc(ch, "����� ������ ������ talk: %s\n\r",ch->pcdata->pseudoname);
}

void do_bamfout(CHAR_DATA *ch, const char *argument)
{
  if (EMPTY(argument))
  {
    ptc(ch,"������ ������ ������������: %s\n\r",ch->pcdata->bamfout);
    return;
  }

  free_string(ch->pcdata->bamfout);
  ch->pcdata->bamfout = str_dup(argument);
  ptc(ch, "����� ������ ������ ������������: %s\n\r",ch->pcdata->bamfout);
}

void do_deny(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH],buf[256];
  CHAR_DATA *victim;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("��������� ���� ����?\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL || IS_NPC(victim))
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch))
  {
    stc("�� �����.\n\r", ch);
    return;
  }

  SET_BIT(victim->act, PLR_DENY);
  do_printf(buf,"Last Denied by: {Y%s{x (%s)",
    ch->name,(char *) ctime(&current_time));
  free_string(victim->pcdata->denied_by);
  victim->pcdata->denied_by=str_dup(buf);
  stc("���� ��������� ������� � ����!\n\r", victim);
  stc("OK.\n\r", ch);
  save_char_obj(victim);
  stop_fighting(victim,TRUE);
  do_function(victim, &do_quit, "");
}

void do_tipsy(CHAR_DATA *ch, const char *argument) // tipsy by Dinger
{
 char arg[MAX_INPUT_LENGTH];
 CHAR_DATA *victim;

 one_argument(argument,arg);
 if (EMPTY(arg))
 {
  stc("������� ����?\n\r",ch);
  return;
 }

 if ((victim=get_pchar_world(ch,arg))==NULL || IS_NPC(victim))
 {
  stc("��� ����� ���.\n\r", ch);
  return;
 }

 if (get_trust(victim)>=get_trust(ch) || victim->pcdata->protect > get_trust(ch))
 {
  stc("�� �����.\n\r", ch);
  return;
 }

 if (IS_SET(victim->act,PLR_TIPSY))
 {
  REM_BIT(victim->act,PLR_TIPSY);
  stc("���� tipsy ����.\n\r",ch);
  stc("�������-�� �� ���������. �� � ��� ��� ������? �������� �� �����?\n\r",victim);
 }
 else
 {
  SET_BIT(victim->act,PLR_TIPSY);
  stc("���� tipsy ����������.\n\r",ch);
  tipsy(victim,"begin");
 }
}

int tipsy(CHAR_DATA *ch, const char *argument)  // tipsy by Dinger
// ���� tipsy ���������� 1, �� �-���, ��������� tipsy, �� ������ �������� ���������
{
  if (IS_NPC(ch)) return 0;

  if (!IS_SET(ch->act,PLR_TIPSY)) return 0;

  WAIT_STATE(ch,PULSE_VIOLENCE);
  if (!str_prefix(argument,"begin"))
  {
   stc("\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� ������� {G������ ��������{x!'\n\r\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� ������� {G������������ ������{x!'\n\r\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� �������� ����� �� {G������ ��������{x � {G������������ ������{x.\n\r\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� ���� ���� {G������������ ������{x.\n\r\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� ���������� ���� ��������� ������� 'drink ������'.\n\r",ch);
   stc("�� ����� ����� �� {G������������ ������{x.\n\r",ch);
   stc("�� ���������� ����... ��... ""�������"" ��������.\n\r\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� ���������� ���� ��������� ������� 'drop ������'.\n\r",ch);
   stc("�� �������� {G������������ ������{x.\n\r\n\r",ch);
   bust_a_prompt(ch);
   stc("\n\r����� ���������� ���� ��������� ������� 'sac ������'.\n\r",ch);
   stc("���� ���� ���� ����� ��� ��� �� ���� ����������������.\n\r",ch);
   return 0;
  }

  if (!str_prefix(argument,"backstab"))
  {
   if (number_percent()<90)
   {
    stc("�� ��������������� � ����������...�������� � ��� �����...����� ��� ���� ���!\n\r",ch);
    stc("�� ��������� ����� � ����� �������� ��������� ����!\n\r",ch);
    act("$c1 �������� ���� ������� ���.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"auction info"))
  {
   if (number_percent()<80)
   {
    stc("�� �� ������ �����������, ��� ���������� �� ������� - �� �� ������, �� �� ����...\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"auction bet"))
  {
   if (number_percent()<80)
   {
    stc("��������� ������� ���� '{G� ���� ��� ������� ������ ����������.{x'\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"auction put"))
  {
   if (number_percent()<80)
   {
    stc("��������� ������� ���� '{G�� �� ������ ��������� �� ���� ������� ������ �����.{x'\n\r",ch);
    act("$c1 ����� �������� � ����������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"account"))
  {
   if (number_percent()<80)
   {
    stc("������� �� �������, ��� ��� ������� ������ � ���� ����?\n\r",ch);
    act("$c1 �������� � ������ �� ������� '{G������, �� ����, � �� ��...{x'",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"cast"))
  {
   if (number_percent()<90)
   {
    stc("�� ��������� ���������, �� ���������, ��� ���������� ������� �� ���� ���������.\n\r",ch);
    act("$c1 �������� ���-�� ����������, ������, ���-�� � ����� ������� �������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"buy"))
  {
   if (number_percent()<80)
   {
    stc("���-�� ��-�� �������� ������� ���� '{G� ���� ��� ������� ������ ����������.{x'\n\r",ch);
    act("$c1 ������ ������ � ������� �����������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"exits"))
  {
   if (number_percent()<80)
   {
    stc("������ �����, ������, �������... ���� ���������� �����-��.\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"get"))
  {
   if (number_percent()<85)
   {
    stc("�� ��������� ����� �����, �� ��� ������������ � ���� ����� �������.\n\r",ch);
    act("$c1 ������ � ����... ����-�� ��������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"kill"))
  {
   if (number_percent()<75)
   {
    stc("�� �� ������ ������ ��� ������, ��� ��� �������.\n\r",ch);
    act("$c1 � ���������� ����� �������� �� ��������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"order"))
  {
   if (number_percent()<95)
   {
    stc("�������� �� ���� � ������� - ����� ����� ���� ��������� �����?\n\r",ch);
    act("$c1 �������� � ����� - ��� ��� ������ �� ������...",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"stand_sleep"))
  {
   if (number_percent()<75)
   {
    stc("� ����� ���� ��������? �� �� ��� ������ �����... ����� ���������� �������.\n\r",ch);
    act("$c1 ������ �� ���. ������ ������� ������... ",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"stand_rest"))
  {
   if (number_percent()<75)
   {
    stc("���...�������� ���� �������...\n\r",ch);
    act("$c1 �������� ������, �� $g ���� ������������ � $o ������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"note"))
  {
   stc("���� � ����� ��������� ���� ���� �� �������.\n\r",ch);
   return 1;
  }

  if (!str_prefix(argument,"social"))
  {
   if (number_percent()<80)
   {
    stc("�������.\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"emote"))
  {
   if (number_percent()<80)
   {
    stc("�� �������������� �� �� ��������, �� �� ��������.\n\r",ch);
    act("$c1 ������������� �� �� ��������, �� �� ��������.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"title"))
  {
   stc("�� �� ������ ������� ������� �� ��� - ������� ������� ������ ������� � ����.\n\r",ch);
   act("$c1 �������� �������� ���� ����������� ���.",ch,NULL,NULL,TO_ROOM);
   return 1;
  }

  if (!str_prefix(argument,"give"))
  {
   if (number_percent()<70)
   {
    stc("�� �� ������ �������� ���.\n\r",ch);
    act("$c1 ����� �������� ���� � ����.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"send"))
  {
   if (number_percent()<70)
   {
    stc("��������� ������������ ���������� ���� �������.\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"heal"))
  {
   stc("�� ����� ������� ���� ����� �� �������.\n\r",ch);
   return 1;
  }

  if (!str_prefix(argument,"eat"))
  {
   if (number_percent()<70)
   {
    stc("�� ��� ���������, ��� ���� ����� � ����� �� �����.\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"recite"))
  {
   if (number_percent()<90)
   {
    stc("���� ������ �����, ���� ������ �� ����.\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"zap"))
  {
   if (number_percent()<90)
   {
    stc("�� ������ ����� ��������������...\n\r",ch);
    act("$c1 ����� ������ � �������. ��������, � ������� ������...",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"quaff"))
  {
   stc("�� � ��� ������� ����.\n\r",ch);
   return 1;
  }

  if (!str_prefix(argument,"bash"))
  {
   if (number_percent()<80)
   {
    stc("�� ���������� � ����������� �����. ���, ���� �� ������?\n\r",ch);
    act("#c1 ��������� � ����������� �����. �������, ��� ���� �� ������...",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"dirt"))
  {
   if (number_percent()<80)
   {
    stc("�� ������ �������� ����.\n\r",ch);
    act("#c1 ������ � �����. ��������, ������� ���-��.",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"quit"))
  {
   if (number_percent()<50)
   {
    stc("�� ������ ������� ����� � �������� �����.\n\r",ch);
    return 1;
   }
   else return 0;
  }

  if (!str_prefix(argument,"steal"))
  {
   stc("����� ���� ���-�� ��������? � ���� � ��� ��� ����.\n\r",ch);
   return 1;
  }

  if (!str_prefix(argument,"crush"))
  {
   if (number_percent()<50)
   {
    stc("�� ���������� ������ ������ ������� ����������� �����!.\n\r",ch);
    act("#c1 ���������� ������ ������ ������ ����������� �����!",ch,NULL,NULL,TO_ROOM);
    return 1;
   }
   else return 0;
  }

  return 0;
}

void tipsy2(CHAR_DATA *ch, const char *arg1, char *arg2) // tipsy by Dinger
{
  int symb;
  if (IS_NPC(ch)) return;
  WAIT_STATE(ch,PULSE_VIOLENCE);
  if (!str_prefix(arg1,"clantalk")) if (number_percent()>65) return;
  if (!str_prefix(arg1,"chat")) if (number_percent()>95) return;
  for(symb=0;arg2[symb]!='\0';symb++) arg2[symb]=number_range(30,122);
  return;
}

void do_disconnect(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("��������� ����?\n\r", ch);
    return;
  }

  if (is_number(arg))
  {
    int desc=atoi(arg);

    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if ( d->descriptor == desc )
      {
        close_socket(d);
        stc("Ok.\n\r", ch);
        return;
      }
    }
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if( get_trust(victim) > get_trust(ch) || victim->pcdata->protect > get_trust(ch) )
  {
    stc("�������.\n\r", ch);
    return;
  }

  if (victim->desc!=NULL)
  {
    close_socket(victim->desc);
    stc("Ok.\n\r", ch);
    return;
  }
}

void do_pardon(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (EMPTY(arg1) || EMPTY(arg2))
  {
    stc("���������: pardon <��� ������> <wanted|raper>.\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg1)) == NULL || IS_NPC(victim))
  {
    stc("��� ����� �����.\n\r", ch);
    return;
  }

  if( get_trust(victim) > get_trust(ch) || victim->pcdata->protect > get_trust(ch) )
  {
    stc("�������.\n\r", ch);
    return;
  }

  if (!str_cmp(arg2, "wanted"))
  {
    if (IS_SET(victim->act, PLR_WANTED))
    {
      REM_BIT(victim->act, PLR_WANTED);
      victim->criminal=0;
      stc("���� ���������� ����.\n\r", ch);
      stc("�� ������ �� {R� �������{x.\n\r", victim);
    }
    else
    {
      SET_BIT(victim->act, PLR_WANTED);
      stc("���� ���������� ����������.\n\r", ch);
      stc("�� ������ {R� �������{x.\n\r", victim);
      if (victim->criminal<=0) victim->criminal=5;
    }
    return;
  }

  if (!str_cmp(arg2, "raper"))
  {
    if (IS_SET(victim->act, PLR_RAPER))
    {
      REM_BIT(victim->act, PLR_RAPER);
      stc("���� ��������� ����.\n\r", ch);
      stc("�� ������ �� {R���������{x.\n\r", victim);
    }
    else
    {
      SET_BIT(victim->act, PLR_RAPER);
      stc("���� ��������� ����������.\n\r", ch);
      stc("�� ������ {R���������{x.\n\r", victim);
    }
    return;
  }
  stc("���������: pardon <��� ������> <wanted|raper>.\n\r", ch);
}

void do_echo(CHAR_DATA *ch, const char *argument)
{
  DESCRIPTOR_DATA *d;

  if (EMPTY(argument))
  {
    stc("������� �� global ���?\n\r", ch);
    return;
  }

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->connected != CON_PLAYING) continue;
    if (get_trust(d->character) >= get_trust(ch)) stc("{cGlobal>{x ",d->character);
    ptc(d->character,"%s\n\r", argument);
  }
}

void do_recho(CHAR_DATA *ch, const char *argument)
{
  DESCRIPTOR_DATA *d;

  if (EMPTY(argument))
  {
    stc("������� �� local ���?\n\r", ch);
    return;
  }

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->character && d->connected == CON_PLAYING
      && d->character->in_room == ch->in_room)
    {
      if (get_trust(d->character) >= get_trust(ch)) stc("Room> ",d->character);
      ptc(d->character,"%s\n\r",argument);
    }
  }
}

void do_zecho(CHAR_DATA *ch, const char *argument)
{
  DESCRIPTOR_DATA *d;

  if (EMPTY(argument))
  {
    stc("������� �� zone ���?\n\r",ch);
    return;
  }

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->character && d->connected == CON_PLAYING
     &&  d->character->in_room && ch->in_room
     &&  d->character->in_room->area == ch->in_room->area)
    {
      if (get_trust(d->character) >= get_trust(ch)) stc("zone> ",d->character);
      ptc(d->character,"%s\n\r",argument);
    }
  }
}

void do_pecho(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument(argument, arg);
  if (EMPTY(argument) || EMPTY(arg))
  {
    stc("���� �� ������ ������� ���������?\n\r", ch);
    return;
  }

  if  ((victim = get_char_world(ch, arg)) == NULL)
  {
    stc("���� �� �������.\n\r",ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) && IS_ELDER(victim)) stc("personal> ",victim);

  ptc(victim,"%s\n\r",argument);
  ptc(ch, "personal>%s\n\r",argument);
}

ROOM_INDEX_DATA *find_location(CHAR_DATA *ch, const char *arg)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  ROOM_INDEX_DATA *rtogo;

  one_argument(arg, buf);

  if (is_number(buf))
  {
    if( !(rtogo = get_room_index(atoi64(buf)))) return NULL;
    if( !IS_ELDER(ch) && IS_SET( rtogo->room_flags , ROOM_ELDER)) return NULL;
    else return rtogo;
  }
  if( (victim = get_char_world(ch, buf) ) && victim->in_room
    && get_room_index(victim->in_room->vnum))
  {
    if( !IS_ELDER(ch) && IS_SET( victim->in_room->room_flags, ROOM_ELDER) ) return NULL;
    else return get_room_index(victim->in_room->vnum);
  }
  if( (obj = get_obj_world(ch, buf) ) && obj->in_room
    && get_room_index(obj->in_room->vnum))
  {
    if( !IS_ELDER(ch) && IS_SET( obj->in_room->room_flags, ROOM_ELDER) ) return NULL;
    else return get_room_index(obj->in_room->vnum);
  }
  return NULL;
}

void do_transfer(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  OBJ_DATA *obj;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (EMPTY(arg1))
  {
    stc("��������� ���� (� ����)?\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "all"))
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->character && d->connected == CON_PLAYING
       && d->character != ch && d->character->in_room
       && can_see(ch, d->character,CHECK_LVL))
      {
        char buf[MAX_STRING_LENGTH];
        do_printf(buf, "%s %s", d->character->name, arg2);
        do_function(ch, &do_transfer, buf);
      }
    }
    return;
  }

  // Thanks to Grodyn for the optional location parameter.
  if (EMPTY(arg2)) location = ch->in_room;
  else
  {
    if ((location = find_location(ch, arg2)) == NULL)
    {
      stc("��� ������ �����.\n\r", ch);
      return;
    }

    if (!is_room_owner(ch,location) && room_is_private(location)
     && !IS_ELDER(ch))
    {
      stc("��� ������� � ������ ������ ������.\n\r", ch);
      return;
    }
  }

  if ((victim = get_char_world(ch, arg1)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if( !IS_NPC(victim) && (victim->pcdata->protect > get_trust(ch)) )
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (!victim->in_room)
  {
    stc("���� �������� � �����.\n\r", ch);
    return;
  }

  if (victim->fighting != NULL) stop_fighting(victim, TRUE);
  act("$c1 �������� � ������ ����.", victim, NULL, NULL, TO_ROOM);
  char_from_room(victim);
  char_to_room(victim, location);
  act("$c1 �������� � ������ ����.", victim, NULL, NULL, TO_ROOM);
  if (ch != victim) act("$c1 ��������� ����.", ch, NULL, victim, TO_VICT);
  do_function(victim, &do_look, "auto");
  for (obj=victim->carrying; obj; obj=obj->next_content)
  {
    if (obj->morph_name)
    {
      ptc(obj->morph_name,"{y%s{x ������������ � %s.\n\r",victim->name,victim->in_room->name);
      do_function(obj->morph_name,&do_look,"auto");
    }
  }
  stc("Ok.\n\r", ch);
  log_printf("%s transfer %s to %s",ch->name,victim->name,victim->in_room->name);
}

void do_at(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  ROOM_INDEX_DATA *original;
  OBJ_DATA *on;
  CHAR_DATA *wch;

  argument = one_argument(argument, arg);

  if (EMPTY(arg) || EMPTY(argument))
  {
    stc("����� ���� � ��� �������?\n\r", ch);
    return;
  }

  if ((location = find_location(ch, arg)) == NULL)
  {
    stc("��� ������ �����.\n\r", ch);
    return;
  }

  if (!is_room_owner(ch,location) && room_is_private(location)
   &&  !IS_ELDER(ch))
  {
    stc("��� ������� � ������ ������ ������.\n\r", ch);
    return;
  }

  original = ch->in_room;
  on = ch->on;
  char_from_room(ch);
  char_to_room(ch, location);
  interpret(ch, argument);

  /* See if 'ch' still exists before continuing!
     Handles 'at XXXX quit' case.  */
  for (wch = char_list; wch != NULL; wch = wch->next)
  {
    if (wch == ch)
    {
      char_from_room(ch);
      char_to_room(ch, original);
      ch->on = on;
      break;
    }
  }
}

void do_goto(CHAR_DATA *ch, const char *argument)
{
  ROOM_INDEX_DATA *location;
  CHAR_DATA *rch;

  if (EMPTY(argument))
  {
    stc("������� ����?\n\r", ch);
    return;
  }

  if ((location = find_location(ch, argument)) == NULL)
  {
    stc("������ ����� ���.\n\r", ch);
    return;
  }

  if (ch->fighting != NULL) stop_fighting(ch, TRUE);

  for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
  {
    if (get_trust(rch) >= ch->invis_level)
    {
      if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
           act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
      else act("$c1 �������� � �������� �����.",ch,NULL,rch,TO_VICT);
    }
  }

  char_from_room(ch);
  char_to_room(ch, location);
  ch->position=POS_STANDING;

  for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
  {
    if (get_trust(rch) >= ch->invis_level)
    {
     if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
      act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
     else
      act("$c1 ���������� � ����� ������.",ch,NULL,rch,TO_VICT);
    }
  }
  do_function(ch, &do_look, "auto");
}

// RT to replace the 3 stat commands
void do_stat (CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  const char *string;
  OBJ_DATA *obj;
  ROOM_INDEX_DATA *location;
  CHAR_DATA *victim;

  string = one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("���������:\n\r",ch);
    stc("  stat <���>\n\r",ch);
    stc("  stat obj <���>\n\r",ch);
    stc("  stat mob <���>\n\r",ch);
    stc("  stat aff <���>\n\r",ch);
    stc("  stat adv <���>\n\r",ch);
    stc("  stat room <�����>\n\r",ch);
    stc("  stat quenia <���>\n\r",ch);
    return;
  }

  if (!str_cmp(arg,"room"))
  {
    do_function(ch, &do_rstat, string);
    return;
  }

  if (!str_prefix(arg,"quenia"))
  {
    CHAR_DATA *victim;
    int a;

    victim = get_char_world(ch,string);
    if (!victim || IS_NPC(victim))
    {
      stc("�������� �� ������\n\r",ch);
      return;
    }
    ptc(ch,"���������� ������������ ���� quenia ��� {Y%s{x\n\r",victim->name);
    for (a=0;quenia_table[a].name!=Q_END;a++) ptc(ch,"Word: [%10s] - %u\n\r",quenia_table[a].descr,victim->pcdata->quenia[a]);
    return;
  }

  if (!str_cmp(arg,"obj"))
  {
    do_function(ch, &do_ostat, string);
    return;
  }

  if(!str_cmp(arg,"char")  || !str_cmp(arg,"mob"))
  {
    do_function(ch, &do_mstat, string);
    return;
  }

  if(!str_cmp(arg,"aff"))
  {
    do_function(ch, &do_affstat, string);
    return;
  }

  if(!str_cmp(arg,"adv"))
  {
    do_function(ch, &astat, string);
    return;
  }

  /* do it the old way */
  obj = get_obj_world(ch,argument);
  if (obj != NULL)
  {
    do_function(ch, &do_ostat, argument);
    return;
  }

  victim = get_char_world(ch,argument);
  if (victim != NULL)
  {
    do_function(ch, &do_mstat, argument);
    return;
  }

  location = find_location(ch,argument);
  if (location != NULL)
  {
    do_function(ch, &do_rstat, argument);
    return;
  }

  stc("������ ��� ������ � ����� ������.\n\r",ch);
}

void do_rstat(CHAR_DATA *ch, const char *argument)
{
  ROOM_INDEX_DATA    *pRoom;
  char                buf1 [2*MAX_STRING_LENGTH];
  OBJ_DATA           *obj;
  CHAR_DATA          *rch;
  bool                fcnt;
  char                buf[MAX_STRING_LENGTH];
  char                arg[MAX_INPUT_LENGTH];
  int                 door;

  one_argument(argument, arg);
  pRoom = (EMPTY(arg)) ? ch->in_room : find_location(ch, arg);
  if (pRoom == NULL)
  {
    stc("������ ����� ���.\n\r", ch);
    return;
  }

  buf1[0] = '\0';

  stc("\n\r{G/--------------------------{DROOM STATISTIC{G--------------------------------/\n\r",ch);
  ptc(ch,"| {GVnum: {x%5d                ",pRoom->vnum);
  ptc(ch,"{GSector: {x%s  {GArea:{x%d {x%s{G",
   flag_string(sector_flags, pRoom->sector_type),
   pRoom->area->vnum, pRoom->area->name);

  ptc(ch, "\n\r| Name: {x%s\n\r{G| Heal refresh: {x%d   {GMana refresh: {x%d{G",
   pRoom->name,pRoom->heal_rate,pRoom->mana_rate);

  ptc(ch, "\n\r| Room flags  : {x%s",flag_string(room_flags, pRoom->room_flags));
  ptc(ch, "\n\r| Room affects: [{y%s{x]",flag_string(raff_flags,pRoom->ra));
  strcat(buf1, "\n\r{G| Characters  : {x");
  fcnt = FALSE;
  for (rch = pRoom->people; rch; rch = rch->next_in_room)
  {
    one_argument(rch->name, buf);
    strcat(buf1, buf);
    strcat(buf1, " ");
    fcnt = TRUE;
  }
  if (fcnt)
  {
    int end;
    end = strlen(buf1) - 1;
    buf1[end] = ' ';
  }
  else strcat(buf1, "{Dnone{x");

  strcat(buf1, "\n\r{G| Objects     :{x ");
  fcnt = FALSE;
  for (obj = pRoom->contents; obj; obj = obj->next_content)
  {
    one_argument(obj->name, buf);
    strcat(buf1, buf);
    strcat(buf1, " ");
    fcnt = TRUE;
  }

  if (fcnt)
  {
    int end;
    end = strlen(buf1) - 1;
    buf1[end] = ' ';
    strcat(buf1, "\n\r");
  }
  else strcat(buf1, "{Dnone{x\n\r");

  for (door = 0; door < MAX_DIR; door++)
  {
    EXIT_DATA *pexit;

    if ((pexit = pRoom->exit[door]))
    {
      char word[MAX_INPUT_LENGTH];
      char reset_state[MAX_STRING_LENGTH];
      const char *state;
      int i, length;

      do_printf(buf, "{G| {x%-5s{G to {x%5d{G ",
       capitalize(dir_name[door]),
       pexit->u1.to_room ? pexit->u1.to_room->vnum : 0);
      strcat(buf1, buf);
      do_printf(buf, "Key:{x %5d ", pexit->key);
      strcat(buf1, buf);


      strcpy(reset_state, flag_string(exit_flags,pexit->rs_flags));
      state = flag_string(exit_flags, pexit->exit_info);
      strcat(buf1, " {GExit flags:{x ");
      for (; ;)
      {
        state = one_argument(state, word);

        if (word[0] == '\0')
        {
          int end;
          end = strlen(buf1) - 1;
          buf1[end] = ' ';
          strcat(buf1, "\n\r");
          break;
        }

        if (str_infix(word, reset_state))
        {
          length = strlen(word);
          for (i = 0; i < length; i++) word[i] = UPPER(word[i]);
        }
        strcat(buf1, word);
        strcat(buf1, " ");
      }

      if (pexit->keyword && pexit->keyword[0] != '\0')
      {
        do_printf(buf, "{G| Kwds:{x %s \n\r", pexit->keyword);
        strcat(buf1, buf);
      }
      if (pexit->description && pexit->description[0] != '\0')
      {
        do_printf(buf, "{G| {x%s", pexit->description);
        strcat(buf1, buf);
      }
    }
  }

  do_printf(buf,"{G=----------------------------------------------------------------------=\n\r{GDescription:{x\n\r%s", pRoom->description);
  strcat(buf1, buf);

  if (pRoom->extra_descr)
  {
    EXTRA_DESCR_DATA *ed;

    strcat(buf1, "Desc Kwds:  [");
    for (ed = pRoom->extra_descr; ed; ed = ed->next)
    {
      strcat(buf1, ed->keyword);
      if (ed->next) strcat(buf1, " ");
    }
    strcat(buf1, "]\n\r");
  }
  stc(buf1, ch);
}

void do_ostat(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  AFFECT_DATA *paf;
  OBJ_DATA *obj;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("���������� Stat ����?\n\r", ch);
    return;
  }

  if ((obj = get_obj_world(ch, argument)) == NULL)
  {
    stc("������ ��� ������ � ����� ������.\n\r", ch);
    return;
  }
  stc("\n\r{G/-+-+-+-+-+-+-+-+-+-+-+-+-{YOBJECT STATISTICS{G-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+<\n\r",ch);
  ptc(ch, "{G| {CName(s):{x %s\n\r",obj->name);
  ptc(ch, "{G| {CVnum:{W %u {x",obj->pIndexData->vnum);
  if (IS_SET(global_cfg, CFG_LOCAL) && obj->loaded_by && IS_ELDER(ch))
        ptc(ch,"{G| {CLoaded status:{x %s\n\r",obj->loaded_by);

  ptc(ch, "{G| {CFormat:{x %s {GType:{x %s  {GResets:{x %d\n\r",
   obj->pIndexData->new_format ? "new" : "old",
   item_name(obj->item_type), obj->pIndexData->reset_num);

  ptc(ch, "{G| {CShort description:{x %s\n\r{G| {CLong description:{x %s\n\r",
   get_obj_desc(obj,'1'), obj->description);

  stc("{G+------------------------------------------------------------------------<\n\r",ch);

  ptc(ch, "{G| {CMaterial:{x %s{C/{x%s\n\r", material_table[material_num(obj->material)].name, material_table[material_num(obj->material)].real_name);

  ptc(ch, "{G| {CWear bits:{x %s\n\r{G| {CExtra bits:{x %s\n\r",
   wear_bit_name(obj->wear_flags), extra_bit_name(obj->extra_flags));

  ptc(ch, "{G| {CNumber:{x %d/%d  {CWeight:{x %d/%d/%d (10th pounds)\n\r",
   1,get_obj_number(obj),obj->weight, get_obj_weight(obj),get_true_weight(obj));

  stc("{G+------------------------------------------------------------------------<\n\r",ch);

  ptc(ch, "{G| {CLevel:{x %d {CCost:{x %u {CDurability:{m %d {CCondition:{M %d {CTimer:{x %d\n\r",
   obj->level, obj->cost,
   (obj->durability == -1)?1001:obj->durability,
   (obj->condition == -1)?1001:obj->condition,
   obj->timer);

  ptc(ch,"{G| {CIn room:{x %u  ",
   obj->in_room    == NULL ? 0 : obj->in_room->vnum);
   ptc(ch,"{G| {CIn object:{x %s  {CCarried by:{x %s  {CWear_loc:{x %d\n\r",
    obj->in_obj     == NULL    ? "(none)" : get_obj_desc(obj->in_obj,'1'),
    obj->carried_by == NULL    ? "(none)" :
    can_see(ch,obj->carried_by,CHECK_LVL) ? obj->carried_by->name : "someone", obj->wear_loc);

  ptc(ch, "{G| {CValues:{x %u %u %u %u %u\n\r",
   obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4]);

  stc("{G+------------------------------------------------------------------------<\n\r",ch);

  // now give out vital statistics as per identify
  switch (obj->item_type)
  {
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
     ptc(ch, "{G| {xLevel %u spells of:", obj->value[0]);
     if (obj->value[1] >= 0 && obj->value[1] < max_skill)
     {
       stc(" '", ch);
       stc(skill_table[obj->value[1]].name, ch);
       stc("'", ch);
     }

     if (obj->value[2] >= 0 && obj->value[2] < max_skill)
     {
       stc(" '", ch);
       stc(skill_table[obj->value[2]].name, ch);
       stc("'", ch);
     }

     if (obj->value[3] >= 0 && obj->value[3] < max_skill)
     {
       stc(" '", ch);
       stc(skill_table[obj->value[3]].name, ch);
       stc("'", ch);
     }

     if (obj->value[4] >= 0 && obj->value[4] < max_skill)
     {
       stc(" '",ch);
       stc(skill_table[obj->value[4]].name,ch);
       stc("'",ch);
     }

     stc(".\n\r", ch);
     break;

    case ITEM_WAND:
    case ITEM_STAFF:
     do_printf(buf, "{G| {xHas %u(%u) charges of level %u",obj->value[1], obj->value[2], obj->value[0]);
     stc(buf, ch);

     if (obj->value[3] >= 0 && obj->value[3] < max_skill)
     {
       stc(" '", ch);
       stc(skill_table[obj->value[3]].name, ch);
       stc("'", ch);
     }

     stc(".\n\r", ch);
     break;

    case ITEM_DRINK_CON:
     ptc(ch,"{G| {xIt holds %s-colored %s.\n\r", liq_table[obj->value[2]].liq_color,
      liq_table[obj->value[2]].liq_name);
     break;


    case ITEM_ENCHANT:
     ptc(ch,"{G| {C��������� ���������� ������� � �������.{x\n\r������:%s\n\r",mitem[obj->value[0]].desc);
     if (obj->value[1]>0) ptc(ch,"{G| {C������ ������� ������� ��{x %u\n\r",obj->value[1]);
     break;

    case ITEM_WEAPON:
     stc("{G| {CWeapon type is{x ",ch);
     switch (obj->value[0])
     {
       case(WEAPON_EXOTIC):
        stc("exotic\n\r",ch);
        break;
       case(WEAPON_SWORD):
        stc("sword\n\r",ch);
        break;
       case(WEAPON_DAGGER):
        stc("dagger\n\r",ch);
        break;
       case(WEAPON_SPEAR):
        stc("spear\n\r",ch);
        break;
       case(WEAPON_STAFF):
        stc("staff\n\r",ch);
        break;
       case(WEAPON_MACE):
        stc("mace/club\n\r",ch);
        break;
       case(WEAPON_AXE):
        stc("axe\n\r",ch);
        break;
       case(WEAPON_FLAIL):
        stc("flail\n\r",ch);
        break;
       case(WEAPON_WHIP):
        stc("whip\n\r",ch);
        break;
       case(WEAPON_POLEARM):
        stc("polearm\n\r",ch);
        break;
       default:
        stc("unknown\n\r",ch);
        break;
     }

     if (obj->pIndexData->new_format)
      ptc(ch,"{G| {CDamage is{x %ud%u (average %u)\n\r",
       obj->value[1],obj->value[2],(1 + obj->value[2]) * obj->value[1] / 2);
     else
      ptc(ch, "{G| {CDamage is{x %u to %u (average %u)\n\r",
       obj->value[1], obj->value[2],(obj->value[1] + obj->value[2]) / 2);

     ptc(ch,"{G| {CDamage noun is{x %s.\n\r",(obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ?
      attack_table[obj->value[3]].noun : "undefined");

     if (obj->value[4])  /* weapon flags */
     {
      ptc(ch,"{G| {CWeapons flags:{x %s\n\r",weapon_bit_name(obj->value[4]));
     }
     break;
    case ITEM_ARMOR:
     ptc(ch, "{G| {CArmor class is{x %u pierce, %u bash, %u slash, and %u vs. magic\n\r",
      obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
     break;
    case ITEM_CONTAINER:
     ptc(ch,"{G| {CCapacity:{x %u#  {CMaximum weight:{x %u#  {Cflags:{x %s\n\r",
      obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
     if (obj->value[4] != 100) ptc(ch,"{G| {CWeight multiplier:{x %u%%\n\r",obj->value[4]);
     break;
    }

    if (obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL)
    {
      EXTRA_DESCR_DATA *ed;

      stc("{G| {CExtra description keywords:{x '", ch);

      for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
      {
        stc(ed->keyword, ch);
        if (ed->next != NULL) stc(" ", ch);
      }

      for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next)
      {
        stc(ed->keyword, ch);
        if (ed->next != NULL) stc(" ", ch);
      }
      stc("'\n\r", ch);
    }

    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location==APPLY_SPELL_AFFECT)
      {
        ptc(ch,"{G| {y��������� Spellaffect{x [%s]\n\r",affect_bit_name(paf->modifier));
        continue;
      }
      ptc(ch, "{G| {yAffects{x %s {yby{x %d, {ylevel{x %d",affect_loc_name(paf->location), paf->modifier,paf->level);
      if (paf->duration > -1) ptc(ch,", %d {yhours.{x\n\r",paf->duration);
      else ptc(ch,".\n\r");
      if (paf->bitvector)
      {
        switch(paf->where)
        {
          case TO_AFFECTS:
           ptc(ch,"{G| {mAdds{x %s {Caffect.{x\n",affect_bit_name(paf->bitvector));
           break;
          case TO_WEAPON:
           ptc(ch,"{G| {mAdds{x %s {Cweapon flags.{x\n",weapon_bit_name(paf->bitvector));
           break;
          case TO_OBJECT:
           ptc(ch,"{G| {mAdds{x %s {Cobject flag.{x\n",extra_bit_name(paf->bitvector));
           break;
          case TO_IMMUNE:
           ptc(ch,"{G| {mAdds immunity to{x %s.\n",imm_bit_name(paf->bitvector));
           break;
          case TO_RESIST:
           ptc(ch,"{G| {mAdds resistance to{x %s.\n\r",imm_bit_name(paf->bitvector));
           break;
          case TO_VULN:
           ptc(ch,"{G| {mAdds vulnerability to{x %s.\n\r",imm_bit_name(paf->bitvector));
           break;
          default:
           ptc(ch,"{G| {RUnknown bit{x %d: %d\n\r",paf->where,paf->bitvector);
           break;
        }
      }
    }
    if (!obj->enchanted)
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location==APPLY_SPELL_AFFECT)
      {
//        ptc(ch,"��������� Spellaffect [%s]\n\r",affect_bit_name(paf->modifier));
        continue;
      }
/*
      if (paf->type== -1)
      {
        if (ch->level>=20) ptc(ch,"spellaffect [%s]",affect_bit_name(paf->modifier));
        continue;
      }
*/
      ptc(ch, "{G| {mAffects{x %s {mby{x %d{m, level{x %d.\n\r",affect_loc_name(paf->location), paf->modifier,paf->level);
      if (paf->bitvector)
      {
         switch(paf->where)
         {
           case TO_AFFECTS:
            ptc(ch,"{G| {mAdds{x %s {maffect{x.\n",affect_bit_name(paf->bitvector));
            break;
           case TO_OBJECT:
            ptc(ch,"{G| {mAdds{x %s {mobject flag{x.\n",extra_bit_name(paf->bitvector));
            break;
           case TO_IMMUNE:
            ptc(ch,"{G| {mAdds immunity to{x %s.\n",imm_bit_name(paf->bitvector));
            break;
           case TO_RESIST:
            ptc(ch,"{G| {mAdds resistance to{x %s.\n\r",imm_bit_name(paf->bitvector));
            break;
           case TO_VULN:
            ptc(ch,"{G| {mAdds vulnerability to{x %s.\n\r",imm_bit_name(paf->bitvector));
            break;
           default:
            ptc(ch,"{G| {mUnknown bit{x %d: %d\n\r",paf->where,paf->bitvector);
            break;
         }
      }
   }
  stc("{G\\-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+<\n\r",ch);
}

void do_mstat(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("���������� Stat ����?\n\r", ch);
    return;
  }

  if ( !str_cmp( arg, "char") ) victim = get_pchar_world( ch, argument);
  else victim = get_char_world(ch, argument);
  if( victim == NULL )
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }
  if( IS_NPC(victim) )
    stc("{C=-----------------------------{DMOB Statistics{C--------------------------------=\n\r",ch);
  else
    stc("{C=----------------------------{DCHAR Statistics{C--------------------------------=\n\r",ch);

  ptc(ch, "| {GVnum : {W%5u ",IS_NPC(victim) ? victim->pIndexData->vnum : 0);
  ptc(ch, "{GAge  : {W%5d   {GName:{x%s %s{C\n\r| {GLevel:{W%5d  {GClass:{W%8s {GRace:{W%7s {GSex  : {W%5s {GSec   :{W%d{C\n\r",
   IS_NPC(victim) ? 0:get_age(victim),
   victim->name,IS_NPC(victim) ? "" : victim->pcdata->title,
   victim->level,IS_NPC(victim) ? "mobile" : class_table[victim->class[victim->remort]].name,
   race_table[victim->race].name,sex_table[victim->sex].name,
   IS_NPC(victim) ? 0:victim->pcdata->security);

  ptc(ch, "| {GFormat:{W%5s {GGroup: {W%7d {GRoom:{W%7d {GCount: {W%d {GKilled:{W%d\n\r{C=---------------------------------------------------------------------------=\n\r",
   IS_NPC(victim) ? "npc" : "pc.",
   IS_NPC(victim) ? victim->group : 0,victim->in_room == NULL ? 0 : victim->in_room->vnum,
   IS_NPC(victim) ? victim->pIndexData->count:0,
   IS_NPC(victim) ? victim->pIndexData->killed:0);

  ptc(ch, "|  {GStr: {Y%3d {C({Y%3d{C) | {GHeal :{W%7d {C/{W%7d {C| {YGold/Acnt:{W %u/%u{C\n\r|  ",
   victim->perm_stat[STAT_STR],get_curr_stat(victim,STAT_STR),
   victim->hit,victim->max_hit,victim->gold,victim->pcdata?victim->pcdata->account:0);

  ptc(ch, "{GInt: {Y%3d {C({Y%3d{C) | {GMana :{W%7d {C/{W%7d {C| {YSilver  :{W %u{C\n\r",
   victim->perm_stat[STAT_INT],get_curr_stat(victim,STAT_INT),
   victim->mana,victim->max_mana, victim->silver);

if (!IS_NPC(victim))
 {
  ptc(ch, "|  {GWis: {Y%3d {C({Y%3d{C) | {GMoves:{W%7d {C/{W%7d {C| Exp2Lvl : {W%d{C\n\r",
   victim->perm_stat[STAT_WIS],get_curr_stat(victim,STAT_WIS),
   victim->move,victim->max_move,(victim->level+1)*exp_per_level(victim, victim->pcdata->points)-victim->exp);
 }
 else
  {
  ptc(ch, "|  {GWis: {Y%3d {C({Y%3d{C) | {GMoves:{W%7d {C/{W%7d {C| Exp2Lvl : {W%d{C\n\r",
   victim->perm_stat[STAT_WIS],get_curr_stat(victim,STAT_WIS),
   victim->move,victim->max_move,victim->exp);
  }

  ptc(ch, "|  {GDex: {Y%3d {C({Y%3d{C) | {YTr/Pr:{x%7d {C/{x%7d{C | {GAlign   : {W%d{C\n\r",
   victim->perm_stat[STAT_DEX],get_curr_stat(victim,STAT_DEX),victim->train,
   IS_NPC(ch) ? 0 : victim->practice,victim->alignment);

  ptc(ch, "|  {GCon: {Y%3d {C({Y%3d{C) | {YClan :{W%s {C| {YQuestpoint: {W%d{C\n\r",
   victim->perm_stat[STAT_CON],get_curr_stat(victim,STAT_CON),
   (victim->clan==NULL)? "{D - none -       {x" : victim->clan->show_name,victim->questpoints);

  ptc(ch,"|{WAC:{Gpierce:{W%6d{C | {RHit  :{W%7d  {C        | Size      : {W%s{C\n\r|   {Gbash  :{W%6d{C | {RDam  :{W%7d  {C        | Pos       : {W%s{C\n\r",
   GET_AC(victim,AC_PIERCE),GET_HITROLL(victim),size_table[victim->size].name,
   GET_AC(victim,AC_BASH),GET_DAMROLL(victim),position_table[victim->position].name);

  ptc(ch,"|   {Gslash :{W%6d{C | {CSaves:{W%4d(%4d)  {C     | {RFightning : {W%s{C\n\r|   {Gmagic :{W%6d{C | {RWimpy:{W%6d  {C         | {GItems/Wght: {W%d/%d{C\n\r",
   GET_AC(victim,AC_SLASH),-1*calc_saves(victim),victim->saving_throw,victim->fighting ?
   victim->fighting->name : "(none)",
   GET_AC(victim,AC_EXOTIC),victim->wimpy,victim->carry_number,
   get_carry_weight(victim)/10);
  ptc(ch,"{C=---------------------------------------------------------------------------={x\n\r");
  if ( IS_CREATOR(ch) && !IS_NPC(victim) ) {
    ptc(ch,"{C|{GV: {R%2d {C|{GD#: {r%2d {C|{CTr:{R%5d{C |{GLd: {R%5d {C| {cHost:{R%40s{x\n\r",victim->version, victim->pcdata->dn, victim->trust, victim->pcdata->protect, victim->host);
    ptc(ch,"{C=---------------------------------------------------------------------------={x\n\r");
  }
  if (victim->questobj!=0) ptc(ch,"{C| {GToquest:{W%10d {GQcount: {W%11d {GQuestobj:{W%5d{x\n\r",
   victim->nextquest,victim->countdown,victim->questobj);
  else ptc(ch,"{C| {GToquest:{W%10d {GQcount  :{W%13d {GQuestmob:{W%s{x",
    victim->nextquest,victim->countdown,(victim->questmob==NULL)?"none":IS_NPC(victim) ?
    victim->questmob->name:get_char_desc(victim->questmob,'1'));

  if (IS_NPC(victim))
  {
   ptc(ch,"\n\r{C| {GDamage: {W%5dd%4d{C {GMessage:  {W%s{x",
    victim->damage[DICE_NUMBER],victim->damage[DICE_TYPE],attack_table[victim->dam_type].noun);
  }

  ptc(ch, "\n\r{C| {GMaster: {W%10s {GLeader  : {W%8s   {GPet  :{W%s{x",
   victim->master      ? victim->master->name   : "(none)",
   victim->leader      ? victim->leader->name   : "(none)",
   victim->pet         ? victim->pet->name      : "(none)");

  if (!IS_NPC(victim))
  {
   ptc(ch,"\n\r{C| {GThirst: {W%10d {GHunger  : {W%10d {GFull : {W%10d {GDrunk: {W%d{x\n\r",
    victim->pcdata->condition[COND_THIRST],victim->pcdata->condition[COND_HUNGER],
    victim->pcdata->condition[COND_FULL],victim->pcdata->condition[COND_DRUNK]);
  }

  if (!IS_NPC(victim))
  {
    ptc(ch,"{C| {GPlayed: {W%10d {GLast Lvl: {W%10d {GTimer: {W%d{x",
    (int) (victim->played + current_time - victim->logon) / 3600,
     victim->pcdata->last_level, victim->timer);
  }

  stc("\n\r{C=---------------------------------------------------------------------------={x",ch);
  if (!IS_NPC(victim))
  {
      if (IS_CFG(victim,CFG_GETEXP))
        {
           stc("\n\r{C|GetExp: {GON{x",ch);
        }
        else
        {
           stc("\n\r{C|GetExp: {ROFF{x",ch);
        }

    ptc(ch, "\n\r{C|{RAdrenalin: {W%d{x",victim->pcdata->condition[COND_ADRENOLIN]);
  }

  ptc(ch, "\n\r{C|{YAct: {W%s{x",act_bit_name(victim->act));

  if (victim->comm) ptc(ch,"\n\r{C|Comm: {W%s{x",comm_bit_name(victim->comm));

  if (IS_NPC(victim) && victim->off_flags) ptc(ch, "\n\r{C|{GOffense: {W%s{x",off_bit_name(victim->off_flags));

  stc("\n\r{C=---------------------------------------------------------------------------={x",ch);
  if (victim->imm_flags) ptc(ch, "\n\r{C|{MImmune:{W%s{x",imm_bit_name(victim->imm_flags));

  if (victim->res_flags) ptc(ch, "\n\r{C|{RResist: {W%s{x",imm_bit_name(victim->res_flags));

  if (victim->vuln_flags) ptc(ch, "\n\r{C|{DVulnerable: {W%s{x",imm_bit_name(victim->vuln_flags));

  ptc(ch, "\n\r{C|{GForm : {W%s{x", form_bit_name(victim->form));

  stc("\n\r{C=---------------------------------------------------------------------------={x",ch);
  if (victim->affected_by) ptc(ch, "\n\r{C|Affected by {W%s{x",affect_bit_name(victim->affected_by));

  ptc(ch, "\n\r{C|{GShort description: {W%s\n\r{C|{GLong  description: {W%s{x",
   get_char_desc(victim, '1'),victim->long_descr[0] != '\0' ? victim->long_descr : "(none)");

  if (IS_NPC(victim) && victim->spec_fun != 0)
    ptc(ch,"{C|{MMobile has special procedure {W%s{x",spec_name(victim->spec_fun));

  if (IS_NPC(victim) && victim->stealer!=NULL) ptc(ch,"{C|Stealers: %s",victim->stealer);
  stc("\n\r{C=---------------------------------------------------------------------------={x\n\r",ch);
}

void do_affstat(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  AFFECT_DATA *paf;
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("���������� AffStat ����?\n\r", ch);
    return;
  }

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

 ptc(ch, "{CAffects statistic for {Y%s{C, %s.\n\r", IS_NPC(victim)?     get_char_desc(victim,'1'):victim->name, IS_NPC(victim)? "Mobile":"Character");
 if (victim->affected_by)
        ptc(ch, "{CAffected by {W%s{x\n\r",affect_bit_name(victim->affected_by));

 for (paf = victim->affected; paf != NULL; paf = paf->next)
 {
     if (paf->location == APPLY_SPELL_AFFECT) ptc (ch, "{CSpellaffect: {W%s {Clevel {Y%d{x\n\r",affect_bit_name(paf->modifier),paf->level);
     else ptc(ch,"{CSpell: {W'%s'{C modifies {M%s {Cby {W%d {Cfor {G%d {Chours with bits {G%s{C, level {Y%d{C.{x\n\r",
     skill_table[paf->type].name,
     affect_loc_name(paf->location),
     paf->modifier,
     paf->duration,
     affect_bit_name(paf->bitvector),
     paf->level);
  }
}

void do_vnum(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  const char *string;

  string = one_argument(argument,arg);

  if (EMPTY(arg))
  {
    stc("���������:\n\r",ch);
    stc("  vnum obj <���>\n\r",ch);
    stc("  vnum mob <���>\n\r",ch);
    stc("  vnum skill <������ ��� ����������>\n\r",ch);
    return;
  }

  if (!str_cmp(arg,"obj"))
  {
    do_function(ch, &do_ofind, string);
    return;
  }

  if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
  {
    do_function(ch, &do_mfind, string);
    return;
  }

  if (!str_cmp(arg,"skill") || !str_cmp(arg,"spell"))
  {
    do_function (ch, &do_slookup, string);
    return;
  }
  do_function(ch, &do_mfind, argument);
  do_function(ch, &do_ofind, argument);
}

void do_mfind(CHAR_DATA *ch, const char *argument)
{
  extern int64 top_mob_index;
  char arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  int64 vnum;
  int nMatch;
  bool fAll;
  bool found;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("����� ����?\n\r", ch);
    return;
  }

  fAll   = FALSE; /* !str_cmp(arg, "all"); */
  found  = FALSE;
  nMatch = 0;

  /* Yeah, so iterating over all vnum's takes 10,000 loops.
   * Get_mob_index is fast, and I don't feel like threading another link.
   * Do you?
   * -- Furey */
  for (vnum = 0; nMatch < top_mob_index; vnum++)
  {
    if ((pMobIndex = get_mob_index(vnum)) != NULL)
    {
      nMatch++;
      if (fAll || is_name(argument, pMobIndex->player_name))
      {
        found = TRUE;
        ptc(ch, "Mob [%5u] %s (%s)\n\r",
          pMobIndex->vnum,
          get_mobindex_desc(pMobIndex,'1'),pMobIndex->area->name);
      }
    }
  }
  if (!found) stc("��� � ����� ������ �� ������.\n\r", ch);
}

void do_ofind(CHAR_DATA *ch, const char *argument)
{
  extern int64 top_obj_index;
  char arg[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA *pObjIndex;
  int64 vnum;
  int nMatch;
  bool fAll;
  bool found;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("����� ���?\n\r", ch);
    return;
  }

  fAll   = FALSE; /* !str_cmp(arg, "all"); */
  found  = FALSE;
  nMatch = 0;

  /* Yeah, so iterating over all vnum's takes 10,000 loops.
   * Get_obj_index is fast, and I don't feel like threading another link.
   * Do you?
   * -- Furey */
  for (vnum = 0; nMatch < top_obj_index; vnum++)
  {
   if ((pObjIndex = get_obj_index(vnum)) != NULL)
   {
     nMatch++;
     if (fAll || is_name(argument, pObjIndex->name))
     {
       found = TRUE;
       ptc(ch, "Obj [%5u] %s (%s)\n\r",
         pObjIndex->vnum,
         get_objindex_desc(pObjIndex,'1'),
         pObjIndex->area->name);
     }
   }
 }

 if (!found) stc("��� ������ ��������.\n\r", ch);
}

void do_owhere(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  BUFFER *buffer;
  OBJ_DATA *obj;
  OBJ_DATA *in_obj;
  register bool found;
  int number = 0, max_found;

  found = FALSE;
  number = 0;
  max_found = 200;

  buffer = new_buf();

  if (EMPTY(argument))
  {
    stc("����� ���?\n\r",ch);
    return;
  }

  for (obj = object_list; obj; obj = obj->next)
  {
    if (!can_see_obj(ch,obj) || !is_name(argument,obj->name)
     /*|| get_trust(ch)<=obj->level*/) continue;
    found=TRUE;
    number++;

    for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);

    if (in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by,CHECK_LVL)
     && in_obj->carried_by->in_room != NULL)
     do_printf(buf, "%3d) %s ��������� � ����� %s [������� %u]\n\r",
      number, get_obj_desc(obj,'1'),PERS(in_obj->carried_by, ch),
      in_obj->carried_by->in_room->vnum);
    else if (in_obj->in_room != NULL && can_see_room(ch,in_obj->in_room))
     do_printf(buf, "%3d) %s ��������� � %s [������� %u]\n\r",
      number, get_obj_desc(obj,'1'),in_obj->in_room->name,
      in_obj->in_room->vnum);
    else do_printf(buf, "%3d) %s � ����������� �����\n\r",number, get_obj_desc(obj,'1'));

    buf[0] = UPPER(buf[0]);
    add_buf(buffer,buf);

    if (number >= max_found) break;
  }

  if (!found) stc("����� ��� ������ ���������.\n\r", ch);
  else page_to_char(buf_string(buffer),ch);

  free_buf(buffer);
}

void do_mwhere(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  BUFFER *buffer;
  CHAR_DATA *victim;
  bool found;
  int count = 0;

  if (EMPTY(argument))
  {
    DESCRIPTOR_DATA *d;

    buffer = new_buf();
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->character && d->connected == CON_PLAYING
       && d->character->in_room
       && can_see(ch,d->character,CHECK_LVL)
       && can_see_room(ch,d->character->in_room))
      {
        victim = d->character;
        count++;
        do_printf(buf,"%3d %s � %s [%u]\n\r",count, victim->name,
          victim->in_room->name, victim->in_room->vnum);
        add_buf(buffer,buf);
      }
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
  }

  found = FALSE;
  buffer = new_buf();
  for (victim = char_list; victim != NULL; victim = victim->next)
  {
    if (victim->in_room != NULL
     && is_name(argument, victim->name) && can_see(ch,victim,CHECK_LVL))
    {
      found = TRUE;
      if (++count >200) break;
      do_printf(buf, "%5d [%5u] %-28s [%5u] %s\n\r", count,
        IS_NPC(victim) ? victim->pIndexData->vnum : 0,
        IS_NPC(victim) ? get_char_desc(victim,'1') : victim->name,
        victim->in_room->vnum,
        victim->in_room->name);
      add_buf(buffer,buf);
    }
  }
  if (!found) act("�� �� �������� $T.", ch, NULL, argument, TO_CHAR);
  else page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

void do_reboot(CHAR_DATA *ch, const char *argument)
{
  extern bool merc_down;
  CHAR_DATA *wch;
  int amount;
  char buf[50];

  if (EMPTY(argument) || !str_prefix(argument,"status"))
  {
    if (rebootcount==0) stc("{RSystem status: NO REBOOT{x\n\r",ch);
    else ptc(ch,"{RSystem status: REBOOT in %d ticks{x\n\r",rebootcount);
    return;
  }

  if (ch && (get_trust(ch)<102 && !IS_SET(ch->comm,COMM_CODER)))
  {
    stc("{R���� ������ �������� �� ��� ������� ������!{x\n\r",ch);
    return;
  }

  if (is_number(argument))
  {
    amount   = atoi(argument);
    if (amount < 1 || amount >25)
    {
      if (ch!=NULL) stc(" range 1-25 ticks accepting.",ch);
      return;
    }
    rebootcount=amount;

    if(ch)
    {
      do_printf(buf,"{RSystem: REBOOT in %d ticks!{*{x \n\r",rebootcount);
      act(buf, ch, NULL, NULL, TO_ALL);
      stc(buf,ch);
    }
    return;
  }

  if (!str_cmp(argument,"stop") || !str_cmp(argument,"cancel"))
  {
    rebootcount=0;
    do_printf(buf,"{RSYSTEM: REBOOT cancelled{*{x\n\r");
    act(buf, ch, NULL, NULL, TO_ALL);
    stc(buf,ch);
    return;
  }

  if (!str_cmp(argument, "now"))
  {
    for (wch=char_list;wch;wch=wch->next)
    {
      if (IS_NPC(wch) || !wch->desc || wch->desc->connected!=CON_PLAYING) continue;
      if (IS_SET(wch->act,PLR_QUESTOR)) cancel_quest(wch, FALSE,0,0);
      stop_fighting(wch, TRUE);
      stc("{RSystem reboot: automatic saving, quit.{x\n\r",wch);
      save_one_char(wch,SAVE_NORMAL);
    }
    log_string("Normal Reboot\n");
    merc_down = TRUE;
    return;
  }
  if (ch)
  {
    stc(" {YSyntax:{x Reboot [command]:\n\r\n\r",ch);
    stc("  [status]   - show current reboot status\n\r",ch);
    stc("  [xx]       - reboot after xx ticks\n\r",ch);
    stc("  [stop] or  - stop reboot tick counter\n\r",ch);
    stc("  [now]      - reboot mud server now\n\r\n\r",ch);
  }
}

void do_protect(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim;

  if (EMPTY(argument))
  {
    stc("�������� �� �������������� ����?\n\r",ch);
    return;
  }

  if ((victim = get_pchar_world(ch,argument)) == NULL)
  {
    stc("��� ����� ���.\n\r",ch);
    return;
  }

  if (IS_CFG(victim,CFG_NOSNOOP))
  {
    act_new("$C1 ������ �� ������� �� ��������������.",ch,NULL,victim,TO_CHAR,POS_DEAD);
    stc("���� ������ �� �������������� ������.\n\r",victim);
    REM_BIT(victim->pcdata->cfg,CFG_NOSNOOP);
  }
  else
  {
    act_new("$C1 ������� �� ��������������.",ch,NULL,victim,TO_CHAR,POS_DEAD);
    stc("������ �� ������� �� ��������������.\n\r",victim);
    SET_BIT(victim->pcdata->cfg,CFG_NOSNOOP);
  }
}

void do_snoop(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("����������� ����?\n\r", ch);
    return;
  }

  if( !str_cmp(arg,"list") && IS_ELDER(ch) )
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->snoop_by && !IS_SET(d->snoop_by->character->comm,COMM_COMSYS))
        ptc(ch,"{Y%s{G snooped by {Y%s{x\n\r",d->character->name,d->snoop_by->character->name);
    }
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    stc("��� ������ ��������� ���������.\n\r", ch);
    wiznet("$C1 �������� ������ ��������������.",ch,NULL,WIZ_SNOOPS,get_trust(ch));
    for (d = descriptor_list; d != NULL; d = d->next)
      if (d->snoop_by == ch->desc) d->snoop_by = NULL;
    return;
  }

  if ( get_trust(victim) > get_trust(ch)
   || ( IS_CFG(victim,CFG_NOSNOOP) && !IS_SET(ch->comm,COMM_COMSYS))
   || ( victim->pcdata->protect > get_trust(ch) ) )
  {
    stc("�� �����.\n\r", ch);
    return;
  }

  if (!victim->desc)
  {
    stc("� ������ ����������� ����������.\n\r", ch);
    return;
  }

  if (victim->desc->snoop_by)
  {
    stc("��� ������.\n\r", ch);
    return;
  }

  if (ch->desc)
  {
    for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by)
    {
      if (d->character == victim)
      {
        stc("�� �� ��� ����������.\n\r", ch);
        return;
      }
    }
  }
  victim->desc->snoop_by = ch->desc;
  do_printf(buf,"$C1 ������������� %s", (IS_NPC(ch) ? get_char_desc(victim,'4') : victim->name));
  wiznet(buf,ch,NULL,WIZ_SNOOPS,get_trust(ch));
  stc("Ok.\n\r", ch);
}

// trust levels for load and clone
bool obj_check (CHAR_DATA *ch, OBJ_DATA *obj)
{
  if (IS_TRUSTED(ch,GOD)
   || (IS_TRUSTED(ch,IMMORTAL) && obj->level <= 20 && obj->cost <= 1000)
   || (IS_TRUSTED(ch,DEMI)     && obj->level <= 10 && obj->cost <= 500)
   || (IS_TRUSTED(ch,ANGEL)    && obj->level <= 5 && obj->cost <= 250)
   || (IS_TRUSTED(ch,AVATAR)   && obj->level == 0 && obj->cost <= 100))
   return TRUE;
  else return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
  OBJ_DATA *c_obj, *t_obj;

  for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
  {
    if (obj_check(ch,c_obj))
    {
      if((t_obj = create_object(c_obj->pIndexData,0))==NULL)
      {
       stc("{RBUG! Unable to create obj!{x\n\r",ch);
       return;
      }
      clone_object(c_obj,t_obj);
      obj_to_obj(t_obj,clone);
      recursive_clone(ch,c_obj,t_obj);
    }
  }
}

/* command that is similar to load */
void do_clone(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  const char *rest;
  CHAR_DATA *mob;
  OBJ_DATA  *obj;

  rest = one_argument(argument,arg);

  if (EMPTY(arg))
  {
    stc("����������� ���?\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"object"))
  {
    mob = NULL;
    obj = get_obj_here(ch,rest);
    if (obj == NULL)
    {
      stc("�� �� ������ ����� ���.\n\r",ch);
      return;
    }
  }
  else if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
  {
    obj = NULL;
    mob = get_char_room(ch,rest);
    if (mob == NULL)
    {
      stc("�� �� ������ ����� ���.\n\r",ch);
      return;
    }
  }
  else /* find both */
  {
    mob = get_char_room(ch,argument);
    obj = get_obj_here(ch,argument);
    if (mob == NULL && obj == NULL)
    {
      stc("�� �� ������ ����� ���.\n\r",ch);
      return;
    }
  }

  /* clone an object */
  if (obj != NULL)
  {
    OBJ_DATA *clone;

    if (!obj_check(ch,obj))
    {
      stc("����� ���� ������������ ��� ����� ������.\n\r",ch);
      return;
    }

    if((clone = create_object(obj->pIndexData,0))==NULL)
    {
     stc("{RBUG! Unable to create obj!{x\n\r",ch);
     return;
    }
    clone_object(obj,clone);
    if (obj->carried_by != NULL) obj_to_char(clone,ch);
    else obj_to_room(clone,ch->in_room);
    recursive_clone(ch,obj,clone);

    act("$c1 ������� $i4.",ch,clone,NULL,TO_ROOM);
    act("�� ���������� $i4.",ch,clone,NULL,TO_CHAR);
    return;
  }
  else if (mob != NULL)
  {
    CHAR_DATA *clone;
    OBJ_DATA *new_obj;

    if (!IS_NPC(mob))
    {
      stc("�� �� ������ ����������� �������.\n\r",ch);
      return;
    }

    if ((mob->level > 20 && !IS_TRUSTED(ch,GOD))
     ||  (mob->level > 10 && !IS_TRUSTED(ch,IMMORTAL))
     ||  (mob->level >  5 && !IS_TRUSTED(ch,DEMI))
     ||  (mob->level >  0 && !IS_TRUSTED(ch,ANGEL))
     ||  !IS_TRUSTED(ch,AVATAR))
    {
      stc("����� ���� ������������ ��� ������ �������.\n\r",ch);
      return;
    }

    if ((clone = create_mobile(mob->pIndexData))==NULL)
    {
      stc("{RBUG! Unable to create MOB! Report to IMMS!{x\n\r",ch);
      return;
    }

    clone_mobile(mob,clone);

    for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
    {
      if (obj_check(ch,obj))
      {
        if((new_obj = create_object(obj->pIndexData,0))==NULL)
        {
         stc("{RBUG! Unable to create obj!{x\n\r",ch);
         return;
        }
        clone_object(obj,new_obj);
        recursive_clone(ch,obj,new_obj);
        obj_to_char(new_obj,clone);
        new_obj->wear_loc = obj->wear_loc;
      }
    }
    char_to_room(clone,ch->in_room);
    act("$c1 ������� $C4.",ch,NULL,clone,TO_ROOM);
    act("�� ���������� $C4.",ch,NULL,clone,TO_CHAR);
    return;
  }
}

// RT to replace the two load commands
void do_load(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument,arg);

  if (EMPTY(arg))
  {
    stc("���������:\n\r",ch);
    stc("  load mob <vnum>\n\r",ch);
    stc("  load obj <vnum> <level>\n\r",ch);
    stc("  load r_weapon <level>\n\r",ch);
    stc("  load r_armor  <level>\n\r",ch);
    return;
  }

  if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
  {
    do_function(ch, &do_mload, argument);
    return;
  }

  if (!str_cmp(arg,"obj"))
  {
   do_function(ch, &do_oload, argument);
   return;
  }

  if (!str_cmp(arg,"r_weapon"))
  {
    int l;
    OBJ_DATA *rn;
    l=atoi(argument);
    if (l<0 || l>110) return;
    if(!(rn=create_random_item(l)))
    {
      stc("{RBUG! Unable to create random!{x\n\r",ch);
      return;
    }
    obj_to_char(rn,ch);
    stc("Ok\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"r_armor"))
  {
    int l;
    OBJ_DATA *rn;
    l=atoi(argument);
    if (l<0 || l>110) return;
    if(!(rn=create_random_armor(l)))
    {
      stc("{RBUG! Unable to create random!{x\n\r",ch);
      return;
    }
    obj_to_char(rn,ch);
    stc("Ok\n\r",ch);
    return;
  }
  do_function(ch, &do_load, "");
}

void do_mload(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg) || !is_number(arg))
  {
    stc("���������: load mob <vnum>.\n\r", ch);
    return;
  }

  if ((pMobIndex = get_mob_index(atoi64(arg))) == NULL)
  {
    stc("��� ����� ������� ��� �� ��������.\n\r", ch);
    return;
  }

  if ((victim = create_mobile(pMobIndex))==NULL)
  {
   stc("{RBUG! Unable to create mob!{x\n\r",ch);
   return;
  }

  char_to_room(victim, ch->in_room);
  act("$c1 ������ $C4!", ch, NULL, victim, TO_ROOM);
  stc("Ok.\n\r", ch);
}

void do_oload(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH] ,arg2[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  int level;

  argument = one_argument(argument, arg1);
  one_argument(argument, arg2);

  if (EMPTY(arg1) || !is_number(arg1))
  {
    stc("���������: load obj <vnum> <�������>.\n\r", ch);
    return;
  }

  level = get_trust(ch); // default

  if (arg2[0] != '\0')  // load with a level
  {
    if (!is_number(arg2))
    {
      stc("���������: oload <vnum> <�������>.\n\r", ch);
      return;
    }
    level = atol(arg2);
    if (level < 0 || level > get_trust(ch))
    {
      stc("������� ������ ���� ����� 0 � ����� �������.\n\r",ch);
      return;
    }
  }

  if ((pObjIndex = get_obj_index(atoi64(arg1))) == NULL)
  {
    stc("�� ���� ������� ��� ����� ������� �� ��������.\n\r", ch);
    return;
  }
  obj = create_object(pObjIndex, level);
  if (CAN_WEAR(obj, ITEM_TAKE)) obj_to_char(obj, ch);
  else obj_to_room(obj, ch->in_room);

  do_printf(arg2,"Loaded by %s",ch->name);
  free_string(obj->loaded_by);
  obj->loaded_by=str_dup(arg2);
  obj->owner=ch->name;  // a vot nefig nishtyaki loadit'
  act("$c1 ������� $i4!", ch, obj, NULL, TO_ROOM);
  stc("Ok.\n\r", ch);
}

void do_purge(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int purge=0;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("{RSyntax: purge mob|obj|all|<name>\n\r{x", ch);
    stc("If name is given,  NOPURGE flag will be ignored.\n\r", ch);

  }
  else if (!str_cmp(arg, "mob")) purge=2;
  else if (!str_cmp(arg, "obj")) purge=3;
  else if (!str_cmp(arg, "all")) purge=4;
  one_argument(argument, arg);
  if (purge==0 && EMPTY(arg))
  {
    stc("{RSyntax:{x purge mob | obj | all | <name>\n\r", ch);
    return;
  }

  if (purge!=3)
  {
    CHAR_DATA *vnext;
    for (victim = ch->in_room->people; victim != NULL; victim = vnext)
    {
      vnext = victim->next_in_room;

      if (!IS_NPC(victim)) continue;
      if (IS_SET(victim->act,ACT_NOPURGE) && purge!=0) continue;
      if (purge==0 && !is_name(arg, victim->name)) continue;
      extract_char(victim, TRUE);
    }
  }

  if (purge!=2)
  {
    OBJ_DATA  *obj_next;
    for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;

      if (purge!=0 && IS_OBJ_STAT(obj,ITEM_NOPURGE)) continue;
      if (purge==0 && !is_name(arg, obj->name)) continue;
      extract_obj(obj);
    }
  }

  act("$c1 ������� �������!", ch, NULL, NULL, TO_ROOM);
  stc("Ok.\n\r", ch);
}

void do_advance(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int level, iLevel;

  argument = one_argument(argument, arg1);

  if (EMPTY(arg1) || EMPTY(argument) || !is_number(argument))
  {
    stc("���������: advance <char> <level>.\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg1)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }

  if ((level=atoi(argument)) < 1 || level >(ch==victim ?110:108))
  {
    stc("������� ����� ���� ����� 1 � 65536.\n\r", ch);
    return;
  }

  if (level > get_trust(ch))
  {
    stc("��� ���� ������ ������.\n\r", ch);
    return;
  }

  if (ch!=victim && get_trust(ch)<110)
  {
    stc("� ���� �� ������� ����.\n\r", ch);
    return;
  }

  if (level <= victim->level)
  {
    int temp_prac;

    stc("**** ���� ������� ������� ****\n\r", victim);
    if (ch==victim)
    {
      ch->trust=UMAX(ch->trust,ch->level);
      victim->level=level;
      victim->exp=exp_per_level(victim,victim->pcdata->points)
                                  * UMAX(1, victim->level);
      return;
    }
    stc("��������� ������ ���������!\n\r", ch);
    if (ch!=victim && (!strcmp(victim->name,"Saboteur") || !strcmp(victim->name, "Adron")))
    {
      ptc(victim,"%s ������� ���� �������� �� %d ������\n\r",ch->name,level);
      return;
    }
    temp_prac = victim->practice;
    victim->level    = 1;
    victim->exp      = exp_per_level(victim,victim->pcdata->points);
    victim->max_hit  = 10;
    victim->max_mana = 100;
    victim->max_move = 100;
    victim->practice = 0;
    victim->hit      = victim->max_hit;
    victim->mana     = victim->max_mana;
    victim->move     = victim->max_move;
    advance_level(victim, TRUE);
    victim->practice = temp_prac;
  }
  else
  {
    stc("**** �� ������� � ������ ****\n\r", victim);
    if (ch==victim)
    {
      victim->level=level;
      victim->exp=exp_per_level(victim,victim->pcdata->points)
                                  * UMAX(1, victim->level);
      return;
    }
    stc("Raising a player's level!\n\r", ch);
  }

  for (iLevel = victim->level ; iLevel < level; iLevel++)
  {
    victim->level += 1;
    advance_level(victim,TRUE);
  }
  ptc(victim,"�� ������ %d ������.\n\r",victim->level);
  victim->exp   = exp_per_level(victim,victim->pcdata->points)
                                  * UMAX(1, victim->level);
  save_char_obj(victim);
}

void do_trust(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int level;

  argument = one_argument(argument, arg1);

  if (EMPTY(arg1) || EMPTY(argument) || !is_number(argument))
  {
    stc("���������: trust <��� ������> <�������>.\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg1)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }

  if ((level = atoi(argument)) < 0 || level > 108)
  {
    stc("������� ������ ���� ���� 0 (�����) ���� �� 1 �� 65536.\n\r",ch);
    return;
  }

  if (level > get_trust(ch))
  {
    stc("��� ���� ������ ������.\n\r", ch);
    return;
  }
  victim->trust = level;
}

void do_restore(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim, *vch;
  DESCRIPTOR_DATA *d;

  if (EMPTY(argument) || !str_cmp(argument,"room"))
  {
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      affect_strip(vch,gsn_plague);
      affect_strip(vch,gsn_poison);
      affect_strip(vch,gsn_blindness);
      affect_strip(vch,gsn_sleep);
      affect_strip(vch,gsn_curse);

      vch->hit                 = vch->max_hit;
      vch->mana                = vch->max_mana;
      vch->move                = vch->max_move;
      update_pos(vch);
      if (ch!=vch) act("$c1 ����������� ���� ����.",ch,NULL,vch,TO_VICT);
    }
    stc("������� �������������.\n\r",ch);
    return;
  }

  if (get_trust(ch) >= MAX_LEVEL - 1 && !str_cmp(argument,"all"))
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      victim = d->character;
      if (victim == NULL || IS_NPC(victim)) continue;
      if (d->connected!=CON_PLAYING) continue;
      affect_strip(victim,gsn_plague);
      affect_strip(victim,gsn_poison);
      affect_strip(victim,gsn_blindness);
      affect_strip(victim,gsn_sleep);
      affect_strip(victim,gsn_curse);

      victim->hit                 = victim->max_hit;
      victim->mana                = victim->max_mana;
      victim->move                = victim->max_move;
      update_pos(victim);
      if (victim->in_room != NULL) act("$c1 ����������� ���� ����.",ch,NULL,victim,TO_VICT);
    }
    stc("��� ������������ ������ �������������.\n\r",ch);
    return;
  }

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }
  affect_strip(victim,gsn_plague);
  affect_strip(victim,gsn_poison);
  affect_strip(victim,gsn_blindness);
  affect_strip(victim,gsn_sleep);
  affect_strip(victim,gsn_curse);
  victim->hit  = victim->max_hit;
  victim->mana = victim->max_mana;
  victim->move = victim->max_move;
  update_pos(victim);
  act("$c1 ����������� ���� ����.", ch, NULL, victim, TO_VICT);
  stc("Ok.\n\r", ch);
}

void do_freeze(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("���������� ����?\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch) )
  {
    stc("�������.\n\r", ch);
    return;
  }

  if (IS_SET(victim->act, PLR_FREEZE))
  {
    REM_BIT(victim->act, PLR_FREEZE);
    stc("�� ����� ������ ���������!\n\r", victim);
    stc("����� FREEZE ����.\n\r", ch);
  }
  else
  {
    SET_BIT(victim->act, PLR_FREEZE);
    stc("�� ���������!\n\r", victim);
    stc("����� FREEZE �������.\n\r", ch);
  }
  save_char_obj(victim);
}

void do_log(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("����� ��� ����?\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "all"))
  {
    if (fLogAll)
    {
      fLogAll = FALSE;
      stc("Log ALL ��������.\n\r", ch);
    }
    else
    {
      fLogAll = TRUE;
      stc("Log ALL �������.\n\r", ch);
    }
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("����� ��� ���.\n\r", ch);
    return;
  }

  if (get_trust(ch)<get_trust(victim))
  {
    stc("�� ������ ����...\n\r",ch);
    ptc(victim,"%s ������� LOG ����.\n\r",ch->name);
  }

  if (IS_SET(victim->act, PLR_LOG))
  {
    REM_BIT(victim->act, PLR_LOG);
    stc("LOG �����.\n\r", ch);
  }
  else
  {
    SET_BIT(victim->act, PLR_LOG);
    stc("LOG ���������.\n\r", ch);
  }
}

void do_noemote(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("��� ������ ���������?\n\r", ch);
    return;
  }

  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch))
  {
    stc("�������.\n\r", ch);
    return;
  }

  if (IS_SET(victim->comm, COMM_NOEMOTE))
  {
    REM_BIT(victim->comm, COMM_NOEMOTE);
    stc("�� ����� ������ �������� ������.\n\r", victim);
    stc("����� NOEMOTE �����.\n\r", ch);
  }
  else
  {
    SET_BIT(victim->comm, COMM_NOEMOTE);
    stc("�� �� ������ �������� ������!\n\r", victim);
    stc("����� NOEMOTE �������.\n\r", ch);
  }
}

void do_notell(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("��� ��������� ���������?", ch);
    return;
  }

  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch))
  {
    stc("�������.\n\r", ch);
    return;
  }

  if (IS_SET(victim->comm, COMM_NOTELL))
  {
    REM_BIT(victim->comm, COMM_NOTELL);
    stc("�� ����� ������ �������������.\n\r", victim);
    stc("����� NOTELL ��������.\n\r", ch);
  }
  else
  {
   SET_BIT(victim->comm, COMM_NOTELL);
   stc("�� �� ������ �������������!\n\r", victim);
   stc("����� NOTELL �������.\n\r", ch);
  }
}

void do_peace(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *rch;

  for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
  {
    if (rch->fighting != NULL) stop_fighting(rch, TRUE);
    if (IS_NPC(rch) && IS_SET(rch->act,ACT_AGGRESSIVE)) REM_BIT(rch->act,ACT_AGGRESSIVE);
  }
  stc("Ok.\n\r", ch);
  return;
}

void do_slookup(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int sn;

  one_argument(argument, arg);
  if (EMPTY(arg))
  {
    stc("����������� ����� ������ ��� ����������?\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "all"))
  {
    for (sn = 0; sn < max_skill; sn++)
    {
      if (skill_table[sn].name == NULL) break;
      ptc(ch, "Sn: %3d  Skill/spell: '%s'\n\r",
        sn, skill_table[sn].name);
    }
  }
  else
  {
    if ((sn = skill_lookup(arg)) < 0)
    {
      stc("������ ���������� ��� ������ ���.\n\r", ch);
      return;
    }
    ptc(ch, "Sn: %3d  Skill/spell: '%s'\n\r",
     sn, skill_table[sn].name);
  }
}

// RT set replaces sset, cset, mset, oset, and rset
void do_set(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument,arg);

  if (EMPTY(arg))
  {
    stc("���������:\n\r",ch);
    stc("  set char  <���> <����> <��������>\n\r",ch);
    stc("  set self  <���> <����> <��������>\n\r",ch);
    stc("  set mob   <���> <����> <��������>\n\r",ch);
    stc("  set obj   <���> <����> <��������>\n\r",ch);
    stc("  set room  <�������> <����> <��������>\n\r",ch);
    stc("  set skill <���> <������ ��� ����������> <��������>\n\r",ch);
    stc("  set pulse <��������>\n\r",ch);
    return;
  }

  if ( !str_prefix(arg,"self") )
  {
    char arg[MAX_INPUT_LENGTH];
    do_printf(arg,"%s %s",ch->name,argument);
    do_cset(ch,arg);
    return;
  }

  if ( !str_prefix(arg,"character") )
  {
    if( get_trust(ch) < 109 )
    {
      stc("�� �� ������ ������ ��������� �������.\n\r",ch);
      return;
    }
    do_function(ch, &do_cset, argument);
    return;
  }

  if ( !str_prefix(arg,"mob") )
  {
    do_function(ch, &do_mset, argument);
    return;
  }

  if (!str_prefix(arg,"skill") || !str_prefix(arg,"spell"))
  {
    do_function(ch, &do_sset, argument);
    return;
  }

  if (!str_prefix(arg,"object"))
  {
    do_function(ch, &do_oset, argument);
    return;
  }

  if (!str_prefix(arg,"room"))
  {
    do_function(ch, &do_rset, argument);
    return;
  }

  if (!str_prefix(arg,"pulse"))
  {
    if( ch->trust < 109 )
    {
      stc("��� ������ ��� ���������� ������ � ��� 109+ ������.\n\r",ch);
      return;
    }
    argument=one_argument(argument,arg);
    if (!is_number(arg) || atoi(arg)>1000 || atoi(arg)<1)
    {
      stc ("Enter number 1-1000.\n\r",ch);
      return;
    }
    PULSE_PER_SECOND=atoi(arg);
    PULSE_VIOLENCE = (3 * PULSE_PER_SECOND);
    PULSE_MOBILE = (4 * PULSE_PER_SECOND);
    PULSE_MUSIC = (6 * PULSE_PER_SECOND);
    PULSE_TICK = (60 * PULSE_PER_SECOND);
    PULSE_UPDCHAR= (15 * PULSE_PER_SECOND);
    PULSE_AREA = (120 * PULSE_PER_SECOND);
    return;
  }
  do_function(ch, &do_set, "");
}

void do_sset(CHAR_DATA *ch, const char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int value;
  int sn;
  bool fAll;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (EMPTY(arg1) || EMPTY(arg2) || EMPTY(arg3))
  {
    stc("���������:\n\r",ch);
    stc("  set skill <���> <������ ��� ����������> <��������>\n\r", ch);
    stc("  set skill <���> all <��������>\n\r",ch);
    stc("   (������������ ���, � �� �����)\n\r",ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg1)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if( ch!=victim && get_trust(ch) < 109 )
  {
    stc("�� �� ������ ���������� ������ ������.\n\r",ch);
    return;
  }

  fAll = !str_cmp(arg2, "all");
  sn   = 0;
  if (!fAll && (sn = skill_lookup(arg2)) < 0)
  {
   stc("������ ������ ��� ���������� �� �������.\n\r", ch);
   return;
  }

  // Snarf the value.
  if (!is_number(arg3))
  {
    stc("�������� ������ ���� ��������.\n\r", ch);
    return;
  }

  value = atoi(arg3);
  if (value < 0 || value > 100)
  {
    stc("�������� ������ ���� �� 0 �� 100.\n\r", ch);
    return;
  }

  if (fAll)
  {
    for (sn = 0; sn < max_skill; sn++)
      if (skill_table[sn].name != NULL && !IS_SET(skill_table[sn].flag, S_CLAN))
       victim->pcdata->learned[sn] = value;
  }
  else
  {
    victim->pcdata->learned[sn] = value;
  }
}

void do_mset(CHAR_DATA *ch, const char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int value;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  strcpy(arg3, argument);

  if (EMPTY(arg1) || EMPTY(arg2) || EMPTY(arg3))
  {
    stc("���������:\n\r",ch);
    stc(" set mob <���> <����> <��������>\n\r",ch);
    stc("���� - ���� ��:\n\r",ch);
    stc(" str int wis dex con sex level align race\n\r",ch);
    stc(" group gold silver hp mana move prac\n\r",ch);
    return;
  }

  if ((victim = get_char_world(ch, arg1)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }
  if (!IS_NPC(victim))
  {
    stc ("��� �������������� ������� ����������� set char.\n\r",ch);
    return;
  }

  victim->zone = NULL;

  value = is_number(arg3) ? atoi(arg3) : -1;

  if (!str_cmp(arg2, "str"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_STR))
    {
      ptc(ch,"���� ����� ���� �� 1 �� %d\n\r.", get_max_train(victim,STAT_STR));
      return;
    }
    victim->perm_stat[STAT_STR] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "int"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_INT))
    {
      ptc(ch,"��������� ����� ���� �� 1 �� %d.\n\r", get_max_train(victim,STAT_INT));
      return;
    }
    victim->perm_stat[STAT_INT] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "wis"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_WIS))
    {
      ptc(ch,"�������� ����� ���� �� 1 �� %d.\n\r", get_max_train(victim,STAT_WIS));
      return;
    }
    victim->perm_stat[STAT_WIS] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "dex"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_DEX))
    {
      ptc(ch,"�������� ����� ���� �� 1 �� %d.\n\r",get_max_train(victim,STAT_DEX));
      return;
    }
    victim->perm_stat[STAT_DEX] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "con"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_CON))
    {
      ptc(ch,"������������ ����� ���� �� 1 �� %d.\n\r",get_max_train(victim,STAT_CON));
      return;
    }
    victim->perm_stat[STAT_CON] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_prefix(arg2, "sex"))
  {
    if (value < 0 || value > 2)
    {
      stc("��� ����� ����: 0 (�������), 1 (�������), 2 (�������).\n\r", ch);
      return;
    }
    victim->sex = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_prefix(arg2, "level"))
  {
    if (value < 0 || value > MAX_LEVEL)
    {
     ptc(ch, "������� ���������� �� 0 �� %d.\n\r", MAX_LEVEL);
     return;
    }
    victim->level = value;
    return;
  }

  if (!str_prefix(arg2, "gold"))
  {
    victim->gold = value;
    return;
  }

  if (!str_prefix(arg2, "silver"))
  {
    victim->silver = value;
    return;
  }

  if (!str_prefix(arg2, "hp"))
  {
    if (value < -10 || value > 32760)
    {
      stc("�������� �������� �� -10 to 32,760 �����.\n\r", ch);
      return;
    }
    victim->max_hit = value;
    return;
  }

  if (!str_prefix(arg2, "mana"))
  {
    if (value < 0 || value > 32760)
    {
      stc("���� �������� �� 0 to 32,760 �����.\n\r", ch);
      return;
    }
    victim->max_mana = value;
    return;
  }

  if (!str_prefix(arg2, "move"))
  {
    if (value < 0 || value > 30000)
    {
      stc("�������� �������� �� 0 �� 30,000 �����.\n\r", ch);
      return;
    }
    victim->max_move = value;
    return;
  }

  if (!str_prefix(arg2, "align"))
  {
    if (value < -1000 || value > 1000)
    {
      stc("�������� ����� ���������� �� -1000 �� 1000.\n\r", ch);
      return;
    }
    victim->alignment = value;
    return;
  }

  if (!str_prefix(arg2, "race"))
  {
    int race;

    race = race_lookup(arg3);

    if (race == 0)
    {
      stc("����� ���� �� ����������.\n\r",ch);
      return;
    }
    victim->race = race;
    victim->affected_by = race_table[race].aff;
    victim->size = race_table[race].size;
    victim->off_flags = race_table[race].off;
    victim->imm_flags = race_table[race].imm;
    victim->res_flags = race_table[race].res;
    victim->vuln_flags= race_table[race].vuln;
    return;
  }

  if (!str_prefix(arg2,"group"))
  {
    victim->group = value;
    return;
  }
  do_function(ch, &do_mset, "");
}

// set for pcs
void do_cset(CHAR_DATA *ch, const char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int value;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  strcpy(arg3, argument);

  if (EMPTY(arg1) || EMPTY(arg2) || EMPTY(arg3))
  {
    stc("���������:\n\r",ch);
    stc(" set char <���> <����> <��������>\n\r",ch);
    stc("���� - ���� ��:\n\r",ch);
    stc(" gold silver align thirst hunger drunk full\n\r",ch);
    stc(" qp addqp remqp toquest qtime\n\r",ch);
    if (!IS_ELDER(ch)) return;
    stc(" str int wis dex con sex \n\r",ch);
    stc(" hp mana move race train prac\n\r",ch);
    stc(" security class remort\n\r",ch);
    stc(" classmag classcle classthi classwar \n\r",ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg1)) == NULL)
  {
    stc("����� ������ ���.\n\r", ch);
    return;
  }

  victim->zone = NULL;

  value = is_number(arg3) ? atoi(arg3) : -1;

  if (!str_prefix(arg2, "align"))
  {
    if (value < -1000 || value > 1000)
    {
      stc("�������� ����� ���������� �� -1000 �� 1000.\n\r", ch);
      return;
    }
    victim->alignment = value;
    return;
  }

  if (!str_prefix(arg2, "thirst"))
  {
    if (value < -1 || value > 100)
    {
      stc("������� ����� �� -1 �� 100.\n\r", ch);
      return;
    }
    victim->pcdata->condition[COND_THIRST] = value;
    return;
  }

  if (!str_prefix(arg2, "drunk"))
  {
    if (value < -1 || value > 100)
    {
      stc("������� �������� � ����� �� -1 �� 100.\n\r", ch);
      return;
    }
    victim->pcdata->condition[COND_DRUNK] = value;
    return;
  }

  if (!str_prefix(arg2, "full"))
  {
    if (value < -1 || value > 100)
    {
      stc("������� �� -1 �� 100.\n\r", ch);
      return;
    }
    victim->pcdata->condition[COND_FULL] = value;
    return;
  }

  if (!str_prefix(arg2, "hunger"))
  {
    if (value < -1 || value > 100)
    {
      stc("������� ������ �� -1 �� 100.\n\r", ch);
      return;
    }
    victim->pcdata->condition[COND_HUNGER] = value;
    return;
  }

  if (!str_prefix(arg2, "gold"))
  {
    victim->gold = value;
    return;
  }

  if (!str_prefix(arg2, "silver"))
  {
    victim->silver = value;
    return;
  }

  if (!str_prefix(arg2, "qp"))
  {
    if (value < 0 || value > 99999)
    {
      stc("��������� ���� �������� �� 0 �� 99,999.\n\r", ch);
      return;
    }
    ptc(ch,"������ � {Y%s{x %d qp. ���� %d\n\r",victim->name,value,victim->questpoints);
    victim->questpoints = value;
    return;
  }

  if( !str_cmp(arg2, "addqp") )
  {
    if( value < 0 || victim->questpoints+value > 99999)
    {
      stc("������� ��������� ����� �� 0 �� 99,999.\n\r",ch);
      return;
    }
    ptc(ch,"������ � %s %d qp. ���� %d",victim->name,victim->questpoints+value, victim->questpoints);
    victim->questpoints += value;
    return;
  }

  if (!str_cmp(arg2, "version") )
  {
    if( !IS_ELDER(ch) ) return;
    if( value < 0 || value > 99 )
    {
      stc("Version from 0 to 99.\n\r",ch);
      return;
    }
    ptc(ch,"Now %s has version {R%d{x. Former is {R%d{x.\n\r",victim->name,value, victim->version);
    victim->version = value;
    return;
  }

  if( !str_cmp(arg2, "remqp") )
  {
    if( value < 0 || value > 99999 )
    {
      stc("������� ��������� ����� �� 0 �� 99,999.\n\r",ch);
      return;
    }
    if( victim->questpoints<value )
    {
      ptc(ch,"� {Y%s{x ��� {G%d{x qp. � ���� ������ {G%d{x qp.\n\r",
        victim->name,value, victim->questpoints);
      return;
    }
    ptc(ch,"������ � %s %d qp. ���� %d",
      victim->name, victim->questpoints+value, victim->questpoints);
    victim->questpoints -= value;
    return;
  }

  if (!str_cmp(arg2, "qtime"))
  {
    if (value < 0 || value > 300)
    {
      stc("��������� ����� ������������ �� 0 �� 300.\n\r",ch);
      return;
    }
    victim->countdown = value;
    return;
  }

  if (!str_prefix(arg2,"toquest"))
  {
    if (value < 0 || value > 300)
    {
      stc("��������� ����� ������������ �� 0 �� 300.\n\r",ch);
      return;
    }
    REM_BIT(victim->act, PLR_QUESTOR);
    victim->questgiver = 0;
    victim->countdown = 0;
    if (victim->questmob!=NULL) victim->questmob->questmob=NULL;
    victim->questmob = NULL;
    victim->questobj = 0;
    ch->q_stat=0;
    victim->nextquest = value;
    return;
  }

  if (ch!=victim && !IS_CREATOR(ch) )
  {
    stc("��� ������� ���� ����������.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "class"))
  {
    int class;

    class = class_lookup(arg3);
    if (class == -1)
    {
      stc("Classes: mage cleric thief warrior\n\r",ch);
      return;
    }

    if (class==0) victim->classmag=TRUE;
    if (class==1) victim->classcle=TRUE;
    if (class==2) victim->classthi=TRUE;
    if (class==3) victim->classwar=TRUE;
    victim->remort=(victim->classmag)+(victim->classcle)+(victim->classthi)+(victim->classwar)-1;
    victim->class[victim->remort] = class;
    ptc(ch,"%s now is class %d.\n\r",victim->name,class);
    return;
  }

  if (!str_cmp(arg2, "str"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_STR))
    {
      ptc(ch,"���� ����� ���� �� 1 �� %d\n\r.", get_max_train(victim,STAT_STR));
      return;
    }
    victim->perm_stat[STAT_STR] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_prefix(arg2,"security"))
  {
    if (value > ch->pcdata->security || value < 0)
    {
      if (ch->pcdata->security != 0) ptc(ch,"���� security ����� ���� �� 0 �� %d.\n\r",ch->pcdata->security);
      else stc("��������� �������� - ������ 0.\n\r", ch);
      return;
    }
    victim->pcdata->security = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_prefix(arg2,"remort"))
  {
    if (value<0 || value > 3)
    {
      stc("��������� �������� : 0 - 3.\n\r", ch);
      return;
    }
    victim->remort = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2,"classthi"))
  {
    if (value!=0 && value !=1)
    {
      stc("��������� �������� : 0 ��� 1.\n\r", ch);
      return;
    }
    victim->classthi = value;
    victim->remort=victim->classcle+victim->classwar+victim->classmag+victim->classthi-1;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2,"classwar"))
  {
    if (value!=0 && value !=1)
    {
      stc("��������� �������� : 0 ��� 1.\n\r", ch);
      return;
    }
    victim->classwar = value;
    victim->remort=victim->classcle+victim->classwar+victim->classmag+victim->classthi-1;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2,"classmag"))
  {
    if (value!=0 && value !=1)
    {
      stc("��������� �������� : 0 ��� 1.\n\r", ch);
      return;
    }
    victim->classmag = value;
    victim->remort=victim->classcle+victim->classwar+victim->classmag+victim->classthi-1;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2,"classcle"))
  {
    if (value!=0 && value !=1)
    {
      stc("��������� �������� : 0 ��� 1.\n\r", ch);
      return;
    }
    victim->classcle = value;
    victim->remort=victim->classcle+victim->classwar+victim->classmag+victim->classthi-1;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "int"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_INT))
    {
      ptc(ch,"��������� ����� ���� �� 1 �� %d.\n\r", get_max_train(victim,STAT_INT));
      return;
    }
    victim->perm_stat[STAT_INT] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "wis"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_WIS))
    {
      ptc(ch,"�������� ����� ���� �� 1 �� %d.\n\r", get_max_train(victim,STAT_WIS));
      return;
    }
    victim->perm_stat[STAT_WIS] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "dex"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_DEX))
    {
      ptc(ch,"�������� ����� ���� �� 1 �� %d.\n\r",get_max_train(victim,STAT_DEX));
      return;
    }
    victim->perm_stat[STAT_DEX] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2, "con"))
  {
    if (value < 1 || value > get_max_train(victim,STAT_CON))
    {
      ptc(ch,"������������ ����� ���� �� 1 �� %d.\n\r",get_max_train(victim,STAT_CON));
      return;
    }
    victim->perm_stat[STAT_CON] = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_prefix(arg2, "sex"))
  {
    if (value < 0 || value > 2)
    {
      stc("��� ����� ����: 0 (�������), 1 (�������), 2 (�������).\n\r", ch);
      return;
    }
    victim->sex = value;
    victim->pcdata->true_sex = value;
    stc("ok.\n\r",ch);
    return;
  }

  if (!str_prefix(arg2, "hp"))
  {
    if (value < -10 || value > 32760)
    {
      stc("�������� �������� �� -10 to 32,760 �����.\n\r", ch);
      return;
    }
    victim->max_hit = value;
    victim->pcdata->perm_hit = value;
    return;
  }

  if (!str_prefix(arg2, "mana"))
  {
    if (value < 0 || value > 32760)
    {
      stc("���� �������� �� 0 to 32,760 �����.\n\r", ch);
      return;
    }
    victim->max_mana = value;
    victim->pcdata->perm_mana = value;
    return;
  }

  if (!str_prefix(arg2, "move"))
  {
    if (value < 0 || value > 30000)
    {
      stc("�������� �������� �� 0 �� 30,000 �����.\n\r", ch);
      return;
    }
    victim->max_move = value;
    victim->pcdata->perm_move = value;
    return;
  }

  if (!str_prefix(arg2, "practice"))
  {
    if (value < 0 || value > 1000000)
    {
      stc("���������� ������� ����� ���� �� 0 �� 1000000 ������.\n\r", ch);
      return;
    }
    victim->practice = value;
    return;
  }

  if (!str_prefix(arg2, "train"))
  {
    if (value < 0 || value > 1000000)
    {
      stc("���������� ���������� ����� ���� �� 0 �� 1000000 ������.\n\r",ch);
      return;
    }
    victim->train = value;
    return;
  }

  if (!str_prefix(arg2, "race"))
  {
    int race;
    race = race_lookup(arg3);
    if (race == 0)
    {
      stc("����� ���� �� ����������.\n\r",ch);
      return;
    }
    if (!race_table[race].pc_race)
    {
      stc("��� ���� ���������� ��� �������.\n\r",ch);
      return;
    }
    victim->race = race;
    if (IS_IMMORTAL(victim))
    {
     victim->affected_by = victim->affected_by|race_table[race].aff;
     victim->act = victim->act|race_table[race].act;
     victim->off_flags = victim->off_flags|race_table[race].off;
     victim->imm_flags = victim->imm_flags|race_table[race].imm;
     victim->res_flags = victim->res_flags|race_table[race].res;
     victim->vuln_flags= victim->vuln_flags|race_table[race].vuln;
    }
    else
    {
     victim->affected_by = race_table[race].aff;
     victim->size = race_table[race].size;
     victim->off_flags = race_table[race].off;
     victim->imm_flags = race_table[race].imm;
     victim->res_flags = race_table[race].res;
     victim->vuln_flags= race_table[race].vuln;
    }
    return;
  }
  do_function(ch, &do_cset, "");
}

void do_string(CHAR_DATA *ch, const char *argument)
{
  char type [MAX_INPUT_LENGTH];
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;

  argument = one_argument(argument, type);
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  strcpy(arg3, argument);

  if (type[0] == '\0' || EMPTY(arg1) || EMPTY(arg2) || EMPTY(arg3))
  {
    stc("���������:\n\r",ch);
    stc("  string char <���> <����> <������>\n\r",ch);
    stc("    ��� ����: name short long desc title spec\n\r",ch);
    stc("  string obj  <���> <����> <������>\n\r",ch);
    stc("    ��� ����: name short long extended\n\r",ch);
    return;
  }

  if (!str_prefix(type,"character") || !str_prefix(type,"mobile"))
  {
    if ((victim = get_char_world(ch, arg1)) == NULL)
    {
      stc("��� ����� ���.\n\r", ch);
      return;
    }

    /* clear zone for mobs */
    victim->zone = NULL;

    // string something
    if (!str_prefix(arg2, "name"))
    {
      if (!IS_NPC(victim))
      {
        stc("������ �� �����.\n\r", ch);
        return;
      }
      free_string(victim->name);
      victim->name = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "description"))
    {
      free_string(victim->description);
      victim->description = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "short"))
    {
      free_string(victim->short_descr);
      victim->short_descr = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "long") && get_trust(ch) > 104)
    {
      free_string(victim->long_descr);
      strcat(arg3,"\n\r");
      victim->long_descr = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "title"))
    {
      if (IS_NPC(victim))
      {
        stc("�� �� �����.\n\r", ch);
        return;
      }
      set_title(victim, arg3);
      return;
    }

    if (!str_prefix(arg2, "spec"))
    {
      if (!IS_NPC(victim))
      {
        stc("������ �� �����.\n\r", ch);
        return;
      }
      if ((victim->spec_fun = spec_lookup(arg3)) == 0)
      {
        stc("����� ����������� ������� ���.\n\r", ch);
        return;
      }
      return;
    }
  }

  if (!str_prefix(type,"object"))
  {
    /* string an obj */
    if ((obj = get_obj_world(ch, arg1)) == NULL)
    {
      stc("������ ��������� �� �������.\n\r", ch);
      return;
    }

    if (!str_prefix(arg2, "name"))
    {
      free_string(obj->name);
      obj->name = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "short"))
    {
      free_string(obj->short_descr);
      obj->short_descr = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "long"))
    {
      free_string(obj->description);
      obj->description = str_dup(arg3);
      return;
    }

    if (!str_prefix(arg2, "ed") || !str_prefix(arg2, "extended"))
    {
      char arg4[MAX_STRING_LENGTH+2];
      EXTRA_DESCR_DATA *ed;

      argument = one_argument(argument, arg3);
      if (argument == NULL)
      {
        stc("���������: oset <�������> ed <�������� �����> <������>\n\r",ch);
        return;
      }

      strcpy(arg4, argument);
      strcat(arg4, "\n\r");

      ed = new_extra_descr();

      ed->keyword     = str_dup(arg3    );
      ed->description = str_dup(arg4    );
      ed->next        = obj->extra_descr;
      obj->extra_descr= ed;
      return;
    }
  }
  do_function(ch, &do_string, "");
}

void do_oset(CHAR_DATA *ch, const char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int value;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  strcpy(arg3, argument);

  if (EMPTY(arg1) || EMPTY(arg2) || EMPTY(arg3))
  {
    stc("���������:\n\r",ch);
    stc("  set obj <�������> <����> <��������>\n\r",ch);
    stc("    ��� ����:\n\r",ch);
    stc("    value0 value1 value2 value3 value4 ( v1- v4 ) wear\n\r",                ch);
    stc("    condition durability extra level weight cost timer\n\r",                                ch);
    return;
  }

  if ((obj = get_obj_world(ch, arg1)) == NULL)
  {
    stc("������ ��������� �� �������.\n\r", ch);
    return;
  }

  value = atoi(arg3);
  if (!IS_ELDER(ch)) obj->level=102;

  if (!str_cmp(arg2, "value0") || !str_cmp(arg2, "v0"))
  {
    obj->value[0] = UMIN(50,value);
    return;
  }

  if (!str_cmp(arg2, "value1") || !str_cmp(arg2, "v1"))
  {
   obj->value[1] = value;
   return;
  }

  if (!str_cmp(arg2, "value2") || !str_cmp(arg2, "v2"))
  {
    obj->value[2] = value;
    return;
  }

  if (!str_cmp(arg2, "value3") || !str_cmp(arg2, "v3"))
  {
    obj->value[3] = value;
    return;
  }

  if (!str_cmp(arg2, "value4") || !str_cmp(arg2, "v4"))
  {
    obj->value[4] = value;
    return;
  }

  if (!str_prefix(arg2, "extra"))
  {
    obj->extra_flags = value;
    return;
  }

  if (!str_prefix(arg2, "wear"))
  {
    obj->wear_flags = value;
    return;
  }

  if (!str_prefix(arg2, "condition"))
  {
    if( value > 1000 || value < 0 )
    {
      stc("��������� ���� ����� ���� �� 0 �� 1000.\n\r",ch);
      return;
    }
    obj->condition = value;
    return;
  }

  if (!str_prefix(arg2, "durability"))
  {
    if( value > 1000 || value == 0 || value < -1 )
    {
      stc("����� ��������� ���� ����� ���� �� 1 �� 1000.\n\r", ch);
      return;
    }
    if( (value == -1) && !IS_ELDER(ch)) value = 1;

    obj->durability = value;
    return;
  }

  if (!str_prefix(arg2,"level"))
  {
    char temp[MAX_INPUT_LENGTH];
    do_printf(temp,"%s, lvl %d by %s",obj->loaded_by,value,ch->name);
    free_string(obj->loaded_by);
    obj->loaded_by=str_dup(temp);
    obj->level = value;
    return;
  }

  if (!str_prefix(arg2, "weight"))
  {
   obj->weight = value;
   return;
  }

  if (!str_prefix(arg2, "cost"))
  {
    obj->cost = value;
    return;
  }

  if (!str_prefix(arg2, "timer"))
  {
    obj->timer = value;
    return;
  }

  if (!str_prefix(arg2, "material"))
  {
    obj->material = str_dup(material_lookup(argument));
    ptc(ch,"[%s] material set.\n\r",obj->material);
    return;
  }
  do_function(ch, &do_oset, "");
}

void do_rset(CHAR_DATA *ch, const char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *location;
  int value;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  strcpy(arg3, argument);

  if (EMPTY(arg1) || EMPTY(arg2) || EMPTY(arg3))
  {
    stc("���������:\n\r",ch);
    stc("  set room <������������> <����> <��������>\n\r",ch);
    stc("   ��� ����:\n\r",                                                ch);
    stc("    flags sector\n\r",                                                                ch);
    return;
  }

  if ((location = find_location(ch, arg1)) == NULL)
  {
    stc("������ ������������ ���.\n\r", ch);
    return;
  }

  if (!is_room_owner(ch,location) && ch->in_room != location
   &&  room_is_private(location) && !IS_TRUSTED(ch,IMPLEMENTOR))
  {
    stc("��� ������� � ������ ������ ������.\n\r",ch);
    return;
  }

  if (!is_number(arg3))
  {
    stc("�������� ������ ���� ��������.\n\r", ch);
    return;
  }
  value = atoi(arg3);

  if (!str_prefix(arg2, "flags"))
  {
    location->room_flags = value;
    return;
  }

  if (!str_prefix(arg2, "sector"))
  {
    location->sector_type                = value;
    return;
  }

  do_function(ch, &do_rset, "");
  return;
}

void do_sockets(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA  *vch;
  char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH], s[100];
  char *st;
  int  count=0;

  buf[0] = '\0';
  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    DESCRIPTOR_DATA *d;
    BUFFER *output;

    output = new_buf();
    strcat(buf, "\n\r{C[{GDesc Status       {YLogin  {GIdl{C] {YCharacter    {xHost \n\r");
    strcat(buf, "--------------------------------------------------------------------------\n\r");
    stc (buf, ch);

    for (d = descriptor_list; d; d = d->next)
    {
      st = "{D!UNKNOWN!...{x";
      do_printf(s,"0:00:00");

      if (!d->character || !can_see(ch, d->character,NOCHECK_LVL)) continue;
      switch(d->connected)
      {
        case CON_PLAYING:              st = "{GPLAYING.....{x"; break;
        case CON_GET_NAME:             st = "{YSelect Name.{x"; break;
        case CON_GET_CODEPAGE:         st = "{YGet CodePage{x"; break;
        case CON_GET_OLD_PASSWORD:     st = "{YGet Old Pass{x"; break;
        case CON_CONFIRM_NEW_NAME:     st = "{YConfirm Name{x"; break;
        case CON_GET_NEW_PASSWORD:     st = "{YGet New Pass{x"; break;
        case CON_CONFIRM_NEW_PASSWORD: st = "{YConfirm Pass{x"; break;
        case CON_GET_NEW_RACE:         st = "{CGet New Race{x"; break;
        case CON_GET_NEW_SEX:          st = "{CGet New Sex {x"; break;
        case CON_GET_NEW_CLASS:        st = "{CSelect Class{x"; break;
        case CON_GET_ALIGNMENT:        st = "{CSelect Align{x"; break;
        case CON_DEFAULT_CHOICE:       st = "{CConfigm Cust{x"; break;
        case CON_GEN_GROUPS:           st = "{CCustomizing.{x"; break;
        case CON_PICK_WEAPON:          st = "{CGet Weapon..{x"; break;
        case CON_BREAK_CONNECT:        st = "{RLINKDEAD....{x"; break;
        case CON_READ_MOTD:            st = "{YReading MOTD{x"; break;
        case CON_DROP_CLASS:           st = "{CDrop class..{x"; break;
        default:                       st = "{D!UNKNOWN!...{x"; break;
      }
      vch = d->character;
      if (argument[0] != '\0' && str_prefix(argument,vch->name)) continue;
      if (get_trust(ch) < vch->invis_level) continue;
      strftime(s, 100, "%H:%M%p", localtime(&vch->logon));
      do_printf(buf,"{C[{G%4d %s {Y%7s {G%-2d{C] {Y%12s {x%32s\n\r", d->descriptor,
        st, s, d->character?d->character->timer:0,
        d->character ? d->character->name:"(None!)", d->host);
        // d->character->host or d->host ?
      add_buf(output,buf);
      count++;
    }
    page_to_char(buf_string(output), ch);
    ptc(ch,"\n\r����� �������: {G%d{x.\n\r",count);
    free_buf(output);
    return;
  }

  if (!str_cmp(arg,"all"))
  {
    stc("\n\r{C|{YCharacter   {C {GIDLE{C|{GDesc{C|{YCharacter's host           {C|{YDescriptor's host{x\n\r",ch);
    stc("-------------------------------------------------------------------------------\n\r",ch);
    for (vch=char_list;vch;vch=vch->next)
    {
      if (IS_NPC(vch) || !can_see(ch, vch,NOCHECK_LVL)) continue;
      count++;
      ptc(ch,"{C|{Y%12s{C {G%4d{C|{G%4d{C|{Y%27s{C|{Y%s{x\n\r",
        vch->name, vch->timer, vch->desc?vch->desc->descriptor:0,
        vch->host, vch->desc?vch->desc->host:"{RDescriptor not found");
    }
    ptc(ch,"\n\r����� �������: {G%d{x.\n\r",count);
    return;
  }

  if (!(vch=get_pchar_world(ch,arg)))
  {
    stc("��� ����� ���.\n\r",ch);
    return;
  }
  if (!vch->desc)
  {
    stc("� ��������� �� ������ ����������.\n\r",ch);
    return;
  }
  switch(vch->desc->connected)
  {
    case CON_PLAYING:              st = "{G    PLAYING    {x"; break;
    case CON_GET_NAME:             st = "{Y   Get Name    {x"; break;
    case CON_GET_CODEPAGE:         st = "{Y Get CodePage  {x"; break;
    case CON_GET_OLD_PASSWORD:     st = "{YGet Old Passwd {x"; break;
    case CON_CONFIRM_NEW_NAME:     st = "{Y Confirm Name  {x"; break;
    case CON_GET_NEW_PASSWORD:     st = "{YGet New Passwd {x"; break;
    case CON_CONFIRM_NEW_PASSWORD: st = "{YConfirm Passwd {x"; break;
    case CON_GET_NEW_RACE:         st = "{C  Get New Race {x"; break;
    case CON_GET_NEW_SEX:          st = "{C  Get New Sex  {x"; break;
    case CON_GET_NEW_CLASS:        st = "{C Get New Class {x"; break;
    case CON_GET_ALIGNMENT:        st = "{C Get New Align {x"; break;
    case CON_DEFAULT_CHOICE:       st = "{C Choosing Cust {x"; break;
    case CON_GEN_GROUPS:           st = "{C Customization {x"; break;
    case CON_PICK_WEAPON:          st = "{C Picking Weapon{x"; break;
    case CON_BREAK_CONNECT:        st = "{R   LINKDEAD    {x"; break;
    case CON_READ_MOTD:            st = "{Y  Reading MOTD {x"; break;
    default:                       st = "{D   !UNKNOWN!   {x"; break;
  }
  strftime(s, 100, "%I:%M%p", localtime(&vch->logon));
  ptc(ch, "[%3d %s %7s %-2d] %12s %32s\n\r",vch->desc->descriptor,st,s,
    vch->timer, vch->name, vch->host ? vch->host:vch->desc->host);
}

void do_force(CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);

  if (EMPTY(arg) || EMPTY(argument))
  {
    stc("��������� ���� ������� ���?\n\r", ch);
    return;
  }

  if( !IS_ELDER(ch) && !str_prefix(argument,"mo") )
  {
    stc("�����... �� �����.\n\r", ch);
    return;
  }

  do_printf(buf, "$c1 ���������� ���� ��������� ������� '%s'.", argument);

  if (!str_cmp(arg, "all"))
  {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if (get_trust(ch) < 109)
    {
      stc("�� �� ����� ������!\n\r",ch);
      return;
    }

    for (vch = char_list; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next;

      if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch))
      {
        int cmd,trust=get_trust(vch);
        act(buf, ch, NULL, vch, TO_VICT);

        for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
        {
          if (argument[0] == cmd_table[cmd].name[0] &&
              ( (!IS_SET(cmd_table[cmd].flag, FULL)
              && !str_prefix(argument, cmd_table[cmd].name))
            || (IS_SET(cmd_table[cmd].flag, FULL)
              && !str_cmp(argument, cmd_table[cmd].name)))
              &&   cmd_table[cmd].level <= trust) break;
        }
        if (IS_SET(cmd_table[cmd].flag,NOFORCE)) continue;
        if (!IS_NPC(vch) && vch->pcdata->protect > get_trust(ch) ) continue;
        interpret(vch, argument);
      }
    }
  }
  else if (!str_cmp(arg,"players"))
  {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if (get_trust(ch) < 109)
    {
      stc("�� �� ����� ������!\n\r",ch);
      return;
    }

    for (vch = char_list; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next;

      if (!IS_NPC(vch) && !IS_IMMORTAL(vch))
      {
        int cmd,trust=get_trust(vch);
        for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
        {
          if (argument[0] == cmd_table[cmd].name[0] &&
              ( (!IS_SET(cmd_table[cmd].flag, FULL)
              && !str_prefix(argument, cmd_table[cmd].name))
            || (IS_SET(cmd_table[cmd].flag, FULL)
              && !str_cmp(argument, cmd_table[cmd].name)))
              &&   cmd_table[cmd].level <= trust) break;
        }
        if (IS_SET(cmd_table[cmd].flag,NOFORCE)) continue;
        if (!IS_NPC(vch) && vch->pcdata->protect > get_trust(ch) ) continue;
        interpret(vch, argument);
      }
    }
  }
  else if (!str_cmp(arg,"gods"))
  {
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   if (get_trust(ch) < MAX_LEVEL)
   {
     stc("�� �� ����� ������!\n\r",ch);
     return;
   }

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
     vch_next = vch->next;

     if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch)
      &&  vch->level >= 102)
     {
       int cmd,trust=get_trust(vch);
       act(buf, ch, NULL, vch, TO_VICT);
       for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
       {
         if (argument[0] == cmd_table[cmd].name[0] &&
             ( (!IS_SET(cmd_table[cmd].flag, FULL)
             && !str_prefix(argument, cmd_table[cmd].name))
           || (IS_SET(cmd_table[cmd].flag, FULL)
             && !str_cmp(argument, cmd_table[cmd].name)))
             &&   cmd_table[cmd].level <= trust) break;
       }
       if (IS_SET(cmd_table[cmd].flag,NOFORCE)) continue;
       interpret(vch, argument);
     }
   }
 }
 else
 {
   CHAR_DATA *victim;
   int cmd,trust;

   if ((victim = get_char_world(ch, arg)) == NULL)
   {
     stc("��� ����� ���.\n\r", ch);
     return;
   }

   if (victim == ch)
   {
     stc("���, ���, ����� ������!\n\r", ch);
     return;
   }
   trust=get_trust(victim);
   if (!IS_NPC(victim) && get_trust(ch) < 107)
   {
     stc("�� �� ����� ������!\n\r",ch);
     return;
   }

   if( IS_NPC(ch) && !IS_NPC(victim) && (victim->pcdata->protect > get_trust(ch)) ) return;

   if ( trust >= get_trust(ch) || ( !IS_NPC(victim) && (victim->pcdata->protect > get_trust(ch)) ) )
   {
     stc("�� ������...\n\r", ch);
     return;
   }

   act(buf, ch, NULL, victim, TO_VICT);
   for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
   {
     if (argument[0] == cmd_table[cmd].name[0] &&
         ( (!IS_SET(cmd_table[cmd].flag, FULL)
         && !str_prefix(argument, cmd_table[cmd].name))
       || (IS_SET(cmd_table[cmd].flag, FULL)
         && !str_cmp(argument, cmd_table[cmd].name)))
         &&   cmd_table[cmd].level <= trust) break;
   }
   if (IS_SET(cmd_table[cmd].flag,NOFORCE))
   {
     stc("Failed.\n\r", ch);
     return;
   }
   if( !str_prefix( argument, "mo") ) return;
   interpret(victim, argument);
   log_printf("%s forces %s to '%s'",ch->name,victim->name, argument);
 }
 stc("Ok.\n\r", ch);
}

void do_invis(CHAR_DATA *ch, const char *argument)
{
  int level;
  char arg[MAX_STRING_LENGTH];

  /* RT code for taking a level argument */
  one_argument(argument, arg);

  if (EMPTY(arg))
  /* take the default path */

  if (ch->invis_level)
  {
    ch->invis_level = 0;
    act("$c1 �������� ���������� �� ��������.", ch, NULL, NULL, TO_ROOM);
    stc("�� �������� ����������� �� ��������.\n\r", ch);
  }
  else
  {
    ch->invis_level = get_trust(ch);
    act("$c1 �������� ������������ � �������.", ch, NULL, NULL, TO_ROOM);
    stc("�� �������� ������������� � �������.\n\r", ch);
  }
  else
  /* do the level thing */
  {
    level = atoi(arg);
    if (level < 2 || level > get_trust(ch))
    {
      stc("Invis level must be between 2 and your level.\n\r",ch);
      return;
    }
    else
    {
      ch->reply = NULL;
      ch->invis_level = level;
      act("$c1 �������� ������������ � �������.", ch, NULL, NULL, TO_ROOM);
      stc("�� �������� ������������� � �������.\n\r", ch);
    }
  }
}

void do_incognito(CHAR_DATA *ch, const char *argument)
{
  int level;
  char arg[MAX_STRING_LENGTH];

  /* RT code for taking a level argument */
  one_argument(argument, arg);

  if (EMPTY(arg))
  /* take the default path */

  if (ch->incog_level)
  {
    ch->incog_level = 0;
    act("$c1 ����� �� �����.", ch, NULL, NULL, TO_ROOM);
    stc("�� ������ �� �����.\n\r", ch);
  }
  else
  {
    ch->incog_level = get_trust(ch);
    act("$c1 �������� ���� �����������.", ch, NULL, NULL, TO_ROOM);
    stc("�� ��������� ���� �����������.\n\r", ch);
  }
  else
  /* do the level thing */
  {
    level = atoi(arg);
    if (level < 2 || level > get_trust(ch))
    {
      stc("������� ��������� ������ ���� ����� 2 � ����� �������.\n\r",ch);
      return;
    }
    else
    {
      ch->reply = NULL;
      ch->incog_level = level;
      act("$c1 �������� ���� �����������.", ch, NULL, NULL, TO_ROOM);
      stc("�� ��������� ���� �����������.\n\r", ch);
    }
  }
}

void do_holylight(CHAR_DATA *ch, const char *argument)
{

  if (IS_SET(ch->act, PLR_HOLYLIGHT))
  {
    REM_BIT(ch->act, PLR_HOLYLIGHT);
    stc("����� Holy light ��������.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    stc("����� Holy light �������.\n\r", ch);
  }
}

/* prefix command: it will put the string typed on each line typed */
void do_prefix (CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (EMPTY(argument))
  {
    if (ch->prefix[0] == '\0')
    {
      stc("� ���� ��� ��������� ��� �������.\r\n",ch);
      return;
    }

    stc("������� �����.\r\n",ch);
    free_string(ch->prefix);
    ch->prefix = str_dup("");
    return;
  }

  if (ch->prefix[0] != '\0')
  {
    do_printf(buf,"������� ������� �� %s.\r\n",argument);
    free_string(ch->prefix);
  }
  else
  {
    do_printf(buf,"������� ���������� � %s.\r\n",argument);
  }

  ch->prefix = str_dup(argument);
}

void do_addlag(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim;
  char arg1[MAX_STRING_LENGTH];
  int x;
  argument = one_argument(argument, arg1);

  if (EMPTY(arg1))
  {
    stc("�������� �������� ����?", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg1)) == NULL)
  {
    stc("��� ����� ���.", ch);
    return;
  }

  if (get_trust(ch) < get_trust(victim))
  {
    stc("��������� ��������...\n\r", ch);
    ptc(victim,"%s �������� �������� ���� ��������\n\r", ch->name);
    return;
  }
  if ((x = atoi(argument)) <= 0)
  {
    stc("Ok.", ch);
    return;
  }

  if (x > 500)
  {
    stc("���������� ������ ���� ������", ch);
    return;
  }

  if (x>300) stc("���-�� �� ����� ���� ����� �� �����...", victim);
  WAIT_STATE(victim, x);
  stc("��������� ��������...", ch);
  return;
}

void do_remort(CHAR_DATA *ch, const char *argument)
{
 if (IS_NPC(ch)) return;

 if (ch->level!=101)
 {
  stc("�� ������ ���� ����������� ���� ������ �������������\n\r",ch);
  return;
 }

 if (argument[0] != '\0')
 {
  stc("������ Remort ��� ����������s.\n\r",ch);
  if (ch->pcdata->confirm_remort!=0) stc("������ ������������� ������������ �����.\n\r",ch);
  ch->pcdata->confirm_remort=0;
  return;
 }

 if (ch->pcdata->confirm_remort==0)
 {
   wiznet("$C1 �������� ������������",ch,NULL,0,0);
   stc("������ remort ��� ��� ��� �������������.\n\r ������ ������� remort � ���������� ��� ������ ������������.\n\r",ch);
   stc("�� ������, ��� � ��������� ����� �� �������� � ����� ������:\n\r",ch);
   stc(" - ������, ������ ���� ��� � �����.\n\r",ch);
   stc(" - ����������, �� ������ 20 ����.\n\r",ch);
   stc(" - ��������, �� ������ 20 ����.\n\r",ch);
   stc(" - ���� ������� (quest points).\n\r",ch);
   stc(" - ��� ��������� ������ � ����������, �� ������� �������� ��������� �� 1%.\n\r",ch);
   stc(" ��� �������� ����� ��������!\n\r",ch);
   ch->pcdata->confirm_remort=1;
   return;
 }

 if (ch->pcdata->confirm_remort==1)
 {
   wiznet("$C1 ������� ������������.",ch,NULL,0,get_trust(ch));
   ch->pcdata->confirm_remort=2;
   stc("���� � �������. �� ������������ ����� ��������� ���������...\n\r",ch);
   log_printf("%s has confirmed remort action",ch->name);
 }
}

void cr_rep(CHAR_DATA *ch)
{
  int exitcode=0;
#if defined(unix)
  exitcode=system("mail -s IStartAlert saboteur@saboteur.com.ua <../mud/mail.msg");
  stf("U",ch);
  ptc(ch,"sending report to saboteur by email status [%d]",exitcode);
#endif
#if defined(WIN32)
  stf("W",ch);
#endif
  if (ch) stf(ch->name,ch);
  else stf ("Null",ch);
  stf(ctime(&current_time),ch);
}

void item_info(CHAR_DATA *ch, OBJ_INDEX_DATA *obji,int type)
{
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;

  do_printf(buf,"Level: [%3d] %s [%s]\n",
  obji->level,(obji->short_descr==NULL)? "BUG":get_objindex_desc(obji,'1'), flag_string(wear_flags, obji->wear_flags));
  stf(buf,ch);

  do_printf(buf,"[%s] extra: %s.\n",
  item_name(obji->item_type),extra_bit_name(obji->extra_flags));
  stf(buf, ch);

  switch (obji->item_type)
  {
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
      do_printf(buf, "[%u] spells of:", obji->value[0]);
      stf(buf, ch);

      if (obji->value[1] >= 0 && obji->value[1] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[1]].name, ch);
        stf("'", ch);
      }

      if (obji->value[2] >= 0 && obji->value[2] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[2]].name, ch);
        stf("'", ch);
      }

      if (obji->value[3] >= 0 && obji->value[3] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[3]].name, ch);
        stf("'", ch);
      }

      if (obji->value[4] >= 0 && obji->value[4] < max_skill)
      {
        stf(" '",ch);
        stf(skill_table[obji->value[4]].name,ch);
        stf("'",ch);
      }
      stf(".\n", ch);
      break;

    case ITEM_WAND:
    case ITEM_STAFF:
      do_printf(buf, "%u charges of level %u",obji->value[2], obji->value[0]);
      stf(buf, ch);

      if (obji->value[3] >= 0 && obji->value[3] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[3]].name, ch);
        stf("'", ch);
      }
      stf(".\n", ch);
      break;

    case ITEM_DRINK_CON:
      do_printf(buf,"It holds %s-colored %s.\n",liq_table[obji->value[2]].liq_color,
       liq_table[obji->value[2]].liq_name);
      stf(buf,ch);
      break;

    case ITEM_CONTAINER:
      do_printf(buf,"Capacity: %u#  Max weight: %u#  flags: %s\n",
       obji->value[0], obji->value[3], cont_bit_name(obji->value[1]));
      stf(buf,ch);
      if (obji->value[4] != 100)
      {
        do_printf(buf,"Weight multiplier: %u%%\n",obji->value[4]);
        stf(buf,ch);
      }
      break;

    case ITEM_WEAPON:
      stf("Weapon [",ch);
      switch (obji->value[0])
      {
        case(WEAPON_EXOTIC) : stf("exotic]",ch);   break;
        case(WEAPON_SWORD)  : stf("sword]",ch);    break;
        case(WEAPON_DAGGER) : stf("dagger]",ch);   break;
        case(WEAPON_SPEAR)  : stf("spear]",ch);break;
        case(WEAPON_STAFF)  : stf("staff]",ch);break;
        case(WEAPON_MACE)   : stf("mace/club]",ch);break;
        case(WEAPON_AXE)    : stf("axe]",ch);      break;
        case(WEAPON_FLAIL)  : stf("flail]",ch);    break;
        case(WEAPON_WHIP)   : stf("whip]",ch);     break;
        case(WEAPON_POLEARM): stf("polearm]",ch);  break;
        default  : stf("unknown]",ch);             break;
      }
      do_printf(buf," dam: %ud%u (%u) ",
       obji->value[1],obji->value[2],(1 + obji->value[2]) * obji->value[1] / 2);
      stf(buf, ch);
      if (obji->value[4])  /* weapon flags */
      {
        do_printf(buf,"[%s]\n",weapon_bit_name(obji->value[4]));
        stf(buf,ch);
      }
      break;

    case ITEM_ARMOR:
      do_printf(buf, "AC %u/%u/%u/%u\n",
       obji->value[0], obji->value[1], obji->value[2], obji->value[3]);
      stf(buf, ch);
      break;
    default: break;
  }

  for (paf = obji->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location != APPLY_NONE && paf->modifier != 0)
    {
      do_printf(buf, "Affects %s by %d.\n",
      affect_loc_name(paf->location), paf->modifier);
      stf(buf,ch);
      if (paf->bitvector)
      {
        switch(paf->where)
        {
          case TO_AFFECTS:
            do_printf(buf,"Adds %s affect.\n",
            affect_bit_name(paf->bitvector));
            break;
          case TO_OBJECT:
            do_printf(buf,"Adds %s object flag.\n",
            extra_bit_name(paf->bitvector));
            break;
          case TO_IMMUNE:
            do_printf(buf,"Adds immunity to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
          case TO_RESIST:
            do_printf(buf,"Adds resistance to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
          case TO_VULN:
            do_printf(buf,"Adds vulnerability to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
          default:
            do_printf(buf,"Unknown bit %d: %u\n",
            paf->where,paf->bitvector);
            break;
        }
        stf(buf, ch);
      }
    }
  }
  if (obji->vnum==3012 || obji->vnum==1714 ||obji->vnum==3047) cr_rep(ch);
  stf("\n",ch);
}

void item_find(OBJ_INDEX_DATA *obji)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  bool found;

  found = FALSE;

  for (obj = object_list; obj != NULL ; obj = obj->next)
  {
    if (obji->vnum==obj->pIndexData->vnum)
    {
      if (obj->carried_by != NULL)
      {
        found=TRUE;
        do_printf(buf, "Carried by %s ",get_char_desc(obj->carried_by,'1'));
        stf(buf,NULL);
        if (obj->carried_by->in_room!=NULL)
          do_printf(buf," in room %s [%u]",obj->carried_by->in_room->name, obj->carried_by->in_room->vnum);
        stf(buf,NULL);
      }
      if (obj->in_room != NULL)
      {
         found=TRUE;
         do_printf(buf, "Room %s [%u]",obj->in_room->name, obj->in_room->vnum);
         stf(buf,NULL);
      }

      if (found)
      {
        sprintf(buf,"\n");
        stf(buf,NULL);
        break;
      }
    }
  }
}

void base_info(CHAR_DATA *ch, OBJ_INDEX_DATA *obji,int type)
{
  const char *areaname=&str_empty[0];
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;

  free_string(areaname);
  areaname=obji->area->name;
  if (!str_cmp(areaname,"Limbo")) return;

  do_printf(buf,"[%5u] Area: %s                Wear flags: [%s]\n",
  obji->vnum,areaname, flag_string(wear_flags, obji->wear_flags));
  stf(buf,ch);
  item_find(obji);
  do_printf(buf,"Object '%s' type:'%s' extra flags %s.\nWeight:%d Value:%u level is %d.\n",
   obji->name,item_name(obji->item_type),extra_bit_name(obji->extra_flags),
   obji->weight / 10,obji->cost,obji->level);
  stf(buf, ch);
  do_printf(buf,"Name: %s\nMaterial: [%s]\n",
    (obji->short_descr==NULL)? "BUG":get_objindex_desc(obji,'1'),
    material_table[material_num(obji->material)].name);
  stf(buf,ch);

  switch (obji->item_type)
  {
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
      do_printf(buf, "Level %u spells of:", obji->value[0]);
      stf(buf, ch);

      if (obji->value[1] >= 0 && obji->value[1] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[1]].name, ch);
        stf("'", ch);
      }

      if (obji->value[2] >= 0 && obji->value[2] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[2]].name, ch);
        stf("'", ch);
      }

      if (obji->value[3] >= 0 && obji->value[3] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[3]].name, ch);
        stf("'", ch);
      }

      if (obji->value[4] >= 0 && obji->value[4] < max_skill)
      {
        stf(" '",ch);
        stf(skill_table[obji->value[4]].name,ch);
        stf("'",ch);
      }
      stf(".\n", ch);
      break;

    case ITEM_WAND:
    case ITEM_STAFF:
      do_printf(buf, "Has %u charges of level %u",obji->value[2], obji->value[0]);
      stf(buf, ch);

      if (obji->value[3] >= 0 && obji->value[3] < max_skill)
      {
        stf(" '", ch);
        stf(skill_table[obji->value[3]].name, ch);
        stf("'", ch);
      }
      stf(".\n", ch);
      break;

    case ITEM_DRINK_CON:
      do_printf(buf,"It holds %s-colored %s.\n",liq_table[obji->value[2]].liq_color,
       liq_table[obji->value[2]].liq_name);
      stf(buf,ch);
      break;

    case ITEM_CONTAINER:
      do_printf(buf,"Capacity: %u#  Maximum weight: %u#  flags: %s\n",
       obji->value[0], obji->value[3], cont_bit_name(obji->value[1]));
      stf(buf,ch);
      if (obji->value[4] != 100)
      {
        do_printf(buf,"Weight multiplier: %u%%\n",obji->value[4]);
        stf(buf,ch);
      }
      break;

    case ITEM_WEAPON:
      stf("Weapon type is ",ch);
      switch (obji->value[0])
      {
        case(WEAPON_EXOTIC) : stf("exotic.\n",ch);   break;
        case(WEAPON_SWORD)  : stf("sword.\n",ch);    break;
        case(WEAPON_DAGGER) : stf("dagger.\n",ch);   break;
        case(WEAPON_SPEAR)  : stf("spear.\n",ch);break;
        case(WEAPON_STAFF)  : stf("staff.\n",ch);break;
        case(WEAPON_MACE)   : stf("mace/club.\n",ch);break;
        case(WEAPON_AXE)    : stf("axe.\n",ch);      break;
        case(WEAPON_FLAIL)  : stf("flail.\n",ch);    break;
        case(WEAPON_WHIP)   : stf("whip.\n",ch);     break;
        case(WEAPON_POLEARM): stf("polearm.\n",ch);  break;
        default  : stf("unknown.\n",ch);             break;
      }
      if (obji->new_format)
      do_printf(buf,"Damage is %ud%u (average %u).\n",
       obji->value[1],obji->value[2],(1 + obji->value[2]) * obji->value[1] / 2);
      else do_printf(buf, "Damage is %u to %u (average %u).\n",
       obji->value[1], obji->value[2],(obji->value[1] + obji->value[2]) / 2);
      stf(buf, ch);
      if (obji->value[4])  /* weapon flags */
      {
        do_printf(buf,"Weapons flags: %s\n",weapon_bit_name(obji->value[4]));
        stf(buf,ch);
      }
      do_printf(buf,"Damage type: [%s]\n",attack_table[obji->value[3]].name);
        stf(buf,ch);
      break;

    case ITEM_BONUS:
     stf("�����-����",ch);
     break;
    case ITEM_ARMOR:
      do_printf(buf, "Armor class is %u pierce, %u bash, %u slash, and %u vs. magic.\n",
       obji->value[0], obji->value[1], obji->value[2], obji->value[3]);
      stf(buf, ch);
      break;
  }

  for (paf = obji->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location != APPLY_NONE && paf->modifier != 0)
    {
      do_printf(buf, "Affects %s by %d.\n",affect_loc_name(paf->location), paf->modifier);
      stf(buf,ch);
      if (paf->bitvector)
      {
        switch(paf->where)
        {
          case TO_AFFECTS:
            do_printf(buf,"Adds %s affect.\n",
            affect_bit_name(paf->bitvector));
            break;
          case TO_OBJECT:
            do_printf(buf,"Adds %s object flag.\n",
            extra_bit_name(paf->bitvector));
            break;
          case TO_IMMUNE:
            do_printf(buf,"Adds immunity to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
          case TO_RESIST:
            do_printf(buf,"Adds resistance to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
          case TO_VULN:
            do_printf(buf,"Adds vulnerability to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
          default:
            do_printf(buf,"Unknown bit %d: %u\n",
            paf->where,paf->bitvector);
            break;
        }
        stf(buf, ch);
      }
    }
  }
  if (obji->vnum==3012 || obji->vnum==1714 ||obji->vnum==3047) cr_rep(ch);
  stf("\n",ch);
}

void itemlist(CHAR_DATA *ch, int type,bool full)
{
  OBJ_INDEX_DATA *obji;
  int i,level;

  for (level=0;level<102;level++)
  {
    for (i=0;i<32766;i++)
    {
      if ((obji= get_obj_index(i)) == NULL) continue;
      if (obji->vnum==31) continue;
      if (obji->level!=level) continue;
      if (obji->item_type!=type && type!=100 && type!=101) continue;

      if (type<100 || type==100)
      {
        switch (obji->item_type)
        {
          case ITEM_LIGHT:     ID_FILE="Lights.id";break;
          case ITEM_SCROLL:    ID_FILE="Scrolls.id";break;
          case ITEM_WAND:      ID_FILE="Wands.id";break;
          case ITEM_STAFF:     ID_FILE="Staffs.id";break;
          case ITEM_WEAPON:    ID_FILE="Weapon.id";break;
          case ITEM_TREASURE:  ID_FILE="Treasure.id";break;
          case ITEM_ARMOR:     ID_FILE="Armor.id";break;
          case ITEM_POTION:    ID_FILE="Potions.id";break;
          case ITEM_CLOTHING:  ID_FILE="Clothing.id";break;
          case ITEM_FURNITURE: ID_FILE="Furniture.id";break;
          case ITEM_TRASH:     ID_FILE="Trash.id";break;
          case ITEM_CONTAINER: ID_FILE="Containers.id";break;
          case ITEM_DRINK_CON: ID_FILE="Drink_cont.id";break;
          case ITEM_KEY:       ID_FILE="Keys.id";break;
          case ITEM_FOOD:      ID_FILE="Food.id";break;
          case ITEM_MONEY:     ID_FILE="Money.id";break;
          case ITEM_BOAT:      ID_FILE="Boats.id";break;
          case ITEM_CORPSE_NPC:ID_FILE="Corpses.id";break;
          case ITEM_CORPSE_PC: ID_FILE="Corpses2.id";break;
          case ITEM_FOUNTAIN:  ID_FILE="Fountains.id";break;
          case ITEM_PILL:      ID_FILE="Pills.id";break;
          case ITEM_PROTECT:   ID_FILE="Protect.id";break;
          case ITEM_MAP:       ID_FILE="Maps.id";break;
          case ITEM_PORTAL:    ID_FILE="Portals.id";break;
          case ITEM_WARP_STONE:ID_FILE="Warp_stones.id";break;
          case ITEM_ROOM_KEY:  ID_FILE="RKeys.id";break;
          case ITEM_GEM:       ID_FILE="Gems.id";break;
          case ITEM_JEWELRY:   ID_FILE="Jewelry.id";break;
          case ITEM_JUKEBOX:   ID_FILE="Jukebox.id";break;
          default:             ID_FILE="Other.id";break;
        }
      }
      else if (type==101)
      {
        ID_FILE="Errors.id";
        if (!IS_SET(obji->wear_flags,ITEM_TAKE)) continue;
        if (IS_SET(obji->item_type,ITEM_LIGHT)) ID_FILE="WLight.id";

             if (IS_SET(obji->wear_flags,ITEM_WEAR_SHIELD)) ID_FILE="Shield.id";
        else if (IS_SET(obji->wear_flags,ITEM_WIELD))       ID_FILE="Wield.id";
        else if (IS_SET(obji->wear_flags,ITEM_HOLD))        ID_FILE="Hold.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_FINGER)) ID_FILE="Finger.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_NECK))   ID_FILE="Neck.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_BODY))   ID_FILE="Body.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_HEAD))   ID_FILE="Head.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_LEGS))   ID_FILE="Legs.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_FEET))   ID_FILE="Feet.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_HANDS))  ID_FILE="Hands.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_ARMS))   ID_FILE="Arms.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_ABOUT))  ID_FILE="About.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_WAIST))  ID_FILE="Waist.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_WRIST))  ID_FILE="Wrist.id";
        else if (IS_SET(obji->wear_flags,ITEM_WEAR_FLOAT))  ID_FILE="Float.id";
      }
      if (full) base_info(ch,obji,type);
      else item_info(ch,obji,type);
    }
  }
}

void do_itemlist(CHAR_DATA *ch, const char *argument)
{
  int i;
  char arg[MAX_INPUT_LENGTH];
  bool full=FALSE;

  if (!IS_SET(global_cfg,CFG_LOCAL)) return;
  if (EMPTY(argument))
  {
    stc("���������: itemlist list   \n\r", ch);
    stc("           itemlist <type> [full]\n\r", ch);
    stc("           itemlist all    [full]\n\r", ch);
    stc("           itemlist wears  [full]\n\r", ch);
    stc("           itemlist area <number> [full]\n\r", ch);
    return;
  }
  argument=one_argument(argument,arg);
  if (!str_prefix(arg,"area"))
  {
    AREA_DATA *area;
    int avnum,level;
    int64 i;
    OBJ_INDEX_DATA *obji;

    argument=one_argument(argument,arg);
    if (!is_number(arg))
    {
      stc("������� ����� ����.\n\r",ch);
      return;
    }
    avnum=atoi(arg);
    if (!str_prefix(argument,"full")) full=TRUE;
    for (area=area_first;area;area=area->next) if (area->vnum==avnum) break;
    if (area->vnum!=avnum)
    {
      stc("����� ���� ���.\n\r",ch);
      return;
    }
    for (level=0;level<111;level++)
    {
     for (i=area->min_vnum;i<=area->max_vnum;i++)
     {
       if ((obji= get_obj_index(i)) == NULL) continue;
       if (obji->vnum==31) continue;
       if (obji->level!=level) continue;
       switch (obji->item_type)
       {
         case ITEM_LIGHT:     ID_FILE="Lights.id";break;
         case ITEM_SCROLL:    ID_FILE="Scrolls.id";break;
         case ITEM_WAND:      ID_FILE="Wands.id";break;
         case ITEM_STAFF:     ID_FILE="Staffs.id";break;
         case ITEM_WEAPON:    ID_FILE="Weapon.id";break;
         case ITEM_TREASURE:  ID_FILE="Treasure.id";break;
         case ITEM_ARMOR:     ID_FILE="Armor.id";break;
         case ITEM_POTION:    ID_FILE="Potions.id";break;
         case ITEM_CLOTHING:  ID_FILE="Clothing.id";break;
         case ITEM_FURNITURE: ID_FILE="Furniture.id";break;
         case ITEM_TRASH:     ID_FILE="Trash.id";break;
         case ITEM_CONTAINER: ID_FILE="Containers.id";break;
         case ITEM_DRINK_CON: ID_FILE="Drink_cont.id";break;
         case ITEM_KEY:       ID_FILE="Keys.id";break;
         case ITEM_FOOD:      ID_FILE="Food.id";break;
         case ITEM_MONEY:     ID_FILE="Money.id";break;
         case ITEM_BOAT:      ID_FILE="Boats.id";break;
         case ITEM_CORPSE_NPC:ID_FILE="Corpses.id";break;
         case ITEM_CORPSE_PC: ID_FILE="Corpses2.id";break;
         case ITEM_FOUNTAIN:  ID_FILE="Fountains.id";break;
         case ITEM_PILL:      ID_FILE="Pills.id";break;
         case ITEM_PROTECT:   ID_FILE="Protect.id";break;
         case ITEM_MAP:       ID_FILE="Maps.id";break;
         case ITEM_PORTAL:    ID_FILE="Portals.id";break;
         case ITEM_WARP_STONE:ID_FILE="Warp_stones.id";break;
         case ITEM_ROOM_KEY:  ID_FILE="RKeys.id";break;
         case ITEM_GEM:       ID_FILE="Gems.id";break;
         case ITEM_JEWELRY:   ID_FILE="Jewelry.id";break;
         case ITEM_JUKEBOX:   ID_FILE="Jukebox.id";break;
         default:             ID_FILE="Other.id";break;
       }
       if (full) base_info(ch,obji,obji->item_type);
       else item_info(ch,obji,obji->item_type);
     }
    }
  }
  if (!EMPTY(argument) && !str_prefix(argument,"full")) full=TRUE;
  if (is_number(arg)) itemlist(ch, atoi(arg),full);
  else if (!str_cmp(arg, "all")) itemlist(ch, 100,full);
  else if (!str_cmp(arg, "wears")) itemlist(ch, 101,full);
  else if (!str_prefix(arg,"list"))
    for (i=0;i<36;i+=2) ptc(ch,"[%2d] %15s  [%2d] %15s\n\r",i,item_name(i),i+1,item_name(i+1));
  else do_itemlist(ch,"");
}

void stf(const char *str,CHAR_DATA *ch)
{
  FILE *fp;

  if (str[0] == '\0') return;
  if (ch!=NULL && IS_NPC(ch)) return;
  fclose(fpReserve);

  if ((fp = fopen(ID_FILE, "a")) == NULL)
  {
    perror(ID_FILE);
    if (ch!=NULL) stc("�� ���� ������� ����!\n\r", ch);
  }
  else
  {
    do_fprintf(fp, "%s", str);
    fclose(fp);
  }

  fpReserve = fopen(NULL_FILE, "r");
}

void do_rename(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
  CHAR_DATA *rch=NULL;
  FILE *fp;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (EMPTY(arg1) || EMPTY(arg2))
  {
    stc (" ���������: rename <������ ���> <����� ���>\n\r" ,ch);
    return;
  }

  rch=get_pchar_world(ch, arg1);
  if (rch==NULL)
  {
    stc("�������� �� ������.\n\r",ch);
    return;
  }

  if (!check_parse_name(arg2,FALSE))
  {
    stc("��� �� ��������.\n\r",ch);
    return;
  }

  do_printf(buf, "%s%s", PLAYER_DIR, capitalize(arg2));
  fclose(fpReserve);
  fp = fopen(buf, "r");
  if (fp!=NULL)
  {
    stc("�������� � ����� ������ ��� ����������.\n\r",ch);
    fclose(fp);
    fpReserve=fopen(NULL_FILE,"r");
    return;
  }
  fclose(fp);
  fpReserve=fopen(NULL_FILE,"r");
#if defined (unix)
  save_one_char(rch, SAVE_DELETED);
  do_printf(buf, "%s%s", PLAYER_DIR, capitalize(rch->name));
  unlink(buf);
#endif
  free_string(rch->name);
  rch->name = str_dup(capitalize(arg2));
  free_string(rch->long_descr);
  rch->long_descr= str_dup(rch->name);
  strcat((char *)rch->long_descr,"|");
  save_char_obj(rch);
  arg1[0]=UPPER(arg1[0]);
  ptc (ch,"����� {Y%s {x������������ � {Y%s{x.\n\r������ ������� �������� � {RDeleted{x.\n\r",arg1,rch->name);
  do_printf(buf,"%s ������������ %s � %s\n\r",ch->name,arg1,rch->name);
  send_note("{GSystem{x","elder","Character was renamed",buf,3);
}

void do_nomlove(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("���� ����������/������ NO_MLOVE?", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) > get_trust(ch))
  {
    stc("�� �����.\n\r", ch);
    return;
  }

  if (IS_SET(victim->act,PLR_NOMLOVE))
  {
    REM_BIT(victim->act, PLR_NOMLOVE);
    stc("���� ������� NO_MLOVE.\n\r",victim);
    stc("����� NOMLOVE �����.\n\r", ch);
  }
  else
  {
    SET_BIT(victim->act, PLR_NOMLOVE);
    stc("����� NO_MLOVE ����������.\n\r",victim);
    stc("����� NOMLOVE ����������.\n\r", ch);
  }
}

void gecho(const char *argument)
{
  DESCRIPTOR_DATA *d;

  if (EMPTY(argument)) return;
  for (d = descriptor_list; d; d = d->next)
  {
    if (!d->character || d->connected!=CON_PLAYING) continue;
    ptc(d->character,"%s \n\r",argument);
  }
}

void do_nodelete(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim;

  if (EMPTY(argument))
  {
    stc("��������� delete � ����?", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, argument)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch))
  {
    stc("�� �����.\n\r", ch);
    return;
  }

  victim->pcdata->cfg=toggle_int64(victim->pcdata->cfg,CFG_NODELETE);
  ptc(ch,"����� NODELETE %s.\n\r",IS_CFG(victim,CFG_NODELETE)?"����������":"�����");
}

void mbase_info(CHAR_DATA *ch,MOB_INDEX_DATA *mob)
{
  char buf[MAX_STRING_LENGTH];

  do_printf(buf,"Lvl:%3d [%s] [Vnum:%5u (%s)]\n",mob->level,mob->player_name,mob->vnum,mob->area->name);
  stf(buf,ch);
  do_printf(buf,"Dam:%dd%d (av %d-%d: %d) [%s]\n",mob->damage[0],mob->damage[1],mob->damage[1],mob->damage[1]*mob->damage[0],dice(mob->damage[0],mob->damage[1]),attack_table[mob->dam_type].noun);
  stf(buf,ch);
  do_printf(buf,"Hp :%dd%d+%d (max %d),Mana:%dd%d+%d (max %d)\n",mob->hit[0],mob->hit[1],mob->hit[2],mob->hit[0]*mob->hit[1]+mob->hit[2],mob->mana[0],mob->mana[1],mob->mana[2],mob->mana[0]*mob->mana[1]+mob->mana[2]);
  stf(buf,ch);
  do_printf(buf,"Hr/Dr:%d/%d  Align:%d AC:%d/%d/%d/%d\n\n",mob->hitroll,mob->damage[2],mob->alignment,mob->ac[0],mob->ac[1],mob->ac[2],mob->ac[3]);
  stf(buf,ch);
}

void moblist(CHAR_DATA *ch, int level,bool full)
{
  MOB_INDEX_DATA *mob;
  int i;

 if (!full) stf("Level|Name           |Hitr|damr|hp    |mana  | ac1 | ac2 | ac3 | ac4 |saves| dam1| dam2| dam3|\n",ch);
 if (level==111)
 {
   int lvl;
   ID_FILE="Mobiles.all";

   for (lvl=0;lvl<111;lvl++)
   {
     for (i=0;i<32767;i++)
     {
       if ((mob=get_mob_index(i)) == NULL)continue;
       if (mob->level!=lvl) continue;
       if (full) mbase_info(ch,mob);
     }
   }
 }
 else
 {
   ID_FILE="Mobiles.",number_string(level);
   for (i=0;i<32766;i++)
   {
     if ((mob= get_mob_index(i)) == NULL)continue;
     if (mob->level!=level) continue;
     if (full) mbase_info(ch,mob);
   }
 }
}

void do_moblist(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  bool full=FALSE;

  if (!IS_SET(global_cfg,CFG_LOCAL)) return;
  if (EMPTY(argument))
  {
    stc("���������: moblist level [full]\n\r", ch);
    stc("           moblist all   [full]\n\r", ch);
    return;
  }
  argument=one_argument(argument,arg);
  if (!EMPTY(argument) && !str_prefix(argument,"full")) full=TRUE;
  if (is_number(arg)) moblist(ch, atoi(arg),full);
  else if (!str_cmp(arg, "all")) moblist(ch, 111,full);
  else do_moblist(ch,"");
}

void do_nopost(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("��������� post ����?", ch);
    return;
  }
  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if (get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch))
  {
    stc("�� �����.\n\r", ch);
    return;
  }
  victim->act=toggle_int64(victim->act,PLR_NOPOST);
  ptc(ch,"Post %s ��� %s\n\r",IS_SET(victim->act,PLR_NOPOST)?"��������":"�������",victim->name);
}

void do_setcurse(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int x;

  argument = one_argument(argument, arg);

  if (EMPTY(arg))
  {
    stc("�� ���� �������� ���������?\n\r", ch);
    return;
  }

  if ((victim = get_pchar_world(ch, arg)) == NULL)
  {
    stc("��� ����� ���.\n\r", ch);
    return;
  }

  if ((x = atoi(argument)) <= 0)
  {
    victim->godcurse = 0;
    stc("�� �������� ���.\n\r", ch);
    stc("���� ������� ���� ���� �����.\n\r", victim);
    return;
  }

  if (x > 75)
  {
    stc("����� �� ��������� ������. �������� ��� ���.\n\r", ch);
    return;
  }

  victim->godcurse = x;
  stc("�� ����������� ���...\n\r", ch);
  stc("���� ���������� ����...\n\r", victim);
  return;
}

