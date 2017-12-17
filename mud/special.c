// $Id: special.c,v 1.14 2002/10/14 16:30:39 black Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"

// The following special functions are available for mobiles.
void raw_kill        args( ( CHAR_DATA *victim ) );
DECLARE_SPEC_FUN(       spec_breath_any         );
DECLARE_SPEC_FUN(       spec_breath_acid        );
DECLARE_SPEC_FUN(       spec_breath_fire        );
DECLARE_SPEC_FUN(       spec_breath_frost       );
DECLARE_SPEC_FUN(       spec_breath_gas         );
DECLARE_SPEC_FUN(       spec_breath_lightning   );
DECLARE_SPEC_FUN(       spec_cast_adept         );
DECLARE_SPEC_FUN(       spec_cast_cleric        );
DECLARE_SPEC_FUN(       spec_cast_judge         );
DECLARE_SPEC_FUN(       spec_cast_mage          );
DECLARE_SPEC_FUN(       spec_cast_undead        );
DECLARE_SPEC_FUN(       spec_executioner        );
DECLARE_SPEC_FUN(       spec_fido               );
DECLARE_SPEC_FUN(       spec_guard              );
DECLARE_SPEC_FUN(       spec_janitor            );
DECLARE_SPEC_FUN(       spec_mayor              );
DECLARE_SPEC_FUN(       spec_poison             );
DECLARE_SPEC_FUN(       spec_thief              );
DECLARE_SPEC_FUN(       spec_nasty              );
DECLARE_SPEC_FUN(       spec_troll_member       );
DECLARE_SPEC_FUN(       spec_ogre_member        );
DECLARE_SPEC_FUN(       spec_patrolman          );
DECLARE_SPEC_FUN(       spec_questmaster        );
DECLARE_SPEC_FUN(       spec_summoner           );
DECLARE_SPEC_FUN(       spec_summonera          );

const   struct  spec_type    spec_table[] =
{
 {   "spec_breath_any",        spec_breath_any         },
 {   "spec_breath_acid",       spec_breath_acid        },
 {   "spec_breath_fire",       spec_breath_fire        },
 {   "spec_breath_frost",      spec_breath_frost       },
 {   "spec_breath_gas",        spec_breath_gas         },
 {   "spec_breath_lightning",  spec_breath_lightning   },      
 {   "spec_cast_adept",        spec_cast_adept         },
 {   "spec_cast_cleric",       spec_cast_cleric        },
 {   "spec_cast_judge",        spec_cast_judge         },
 {   "spec_cast_mage",         spec_cast_mage          },
 {   "spec_cast_undead",       spec_cast_undead        },
 {   "spec_executioner",       spec_executioner        },
 {   "spec_fido",              spec_fido               },
 {   "spec_guard",             spec_guard              },
 {   "spec_janitor",           spec_janitor            },
 {   "spec_mayor",             spec_mayor              },
 {   "spec_poison",            spec_poison             },
 {   "spec_thief",             spec_thief              },
 {   "spec_nasty",             spec_nasty              },
 {   "spec_troll_member",      spec_troll_member       },
 {   "spec_ogre_member",       spec_ogre_member        },
 {   "spec_patrolman",         spec_patrolman          },
 {   "spec_questmaster",       spec_questmaster        }, // Vassago
 {   "spec_summonera",         spec_summonera          },
 {   NULL,                     NULL                    }
};

// Given a name, return the appropriate spec fun.
SPEC_FUN *spec_lookup( const char *name )
{
  int i;
 
  for ( i = 0; spec_table[i].name != NULL; i++)
  {
    if (LOWER(name[0]) == LOWER(spec_table[i].name[0])
      &&  !str_prefix( name,spec_table[i].name))
      return spec_table[i].function;
  }
  return 0;
}

char *spec_name( SPEC_FUN *function)
{
  int i;

  for (i = 0; spec_table[i].function != NULL; i++)
  {
    if (function == spec_table[i].function) return spec_table[i].name;
  }
  return NULL;
}

bool spec_troll_member( CHAR_DATA *ch)
{
  CHAR_DATA *vch, *victim = NULL;
  int count = 0;
  char *message;

  if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL 
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL) return FALSE;

  // find an ogre to beat up
  for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
  {
    if (!IS_NPC(vch) || ch == vch) continue;
    if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN) return FALSE;
    if (vch->pIndexData->group == GROUP_VNUM_OGRES
        &&  ch->level > vch->level - 2 && !is_safe(ch,vch))
    {
      if (number_range(0,count) == 0) victim = vch;
      count++;
    }
  }

  if (victim == NULL) return FALSE;

  /* say something, then raise hell */
  switch (number_range(0,6))
  {
    default:  message = NULL;       break;
    case 0: message = "$c1 громко вскрикивает '{MЯ следил, за тобой, ублюдок!{x'";
      break;
    case 1: message = "С яростным криком $c1 атакует $C4.";
      break;
    case 2: message = "$c1 произносит '{GКакого Адрона ты тут делаешь?{x'";
      break;
    case 3: message = "$c1 хрустнув пальцами, произносит '{GЧувтвуешь удачу?{x'";
      break;
    case 4: message = "$c1 произносит '{GТут нет охранников, ты попал!{x'";
      break;  
    case 5: message = "$c1 произносит '{GВремя присоединиться к остальным, сосунок.{x'";
      break;
    case 6: message = "$c1 произносит '{GДочирикался...{x'";
      break;
  }

  if (message != NULL) act(message,ch,NULL,victim,TO_ROOM);
  multi_hit( ch, victim);
  return TRUE;
}

bool spec_ogre_member( CHAR_DATA *ch)
{
  CHAR_DATA *vch, *victim = NULL;
  int count = 0;
  char *message;
 
  if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL) return FALSE;

  /* find an troll to beat up */
  for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
  {
    if (!IS_NPC(vch) || ch == vch) continue;
 
    if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN) return FALSE;
 
    if (vch->pIndexData->group == GROUP_VNUM_TROLLS
      &&  ch->level > vch->level - 2 && !is_safe(ch,vch))
    {
      if (number_range(0,count) == 0) victim = vch;
      count++;
    }
  }
  if (victim == NULL) return FALSE;
 
  /* say something, then raise hell */
  switch (number_range(0,6))
  {
    default: message = NULL;        break;
    case 0: message = "$c1 громко вскрикивает '{MЯ следил, за тобой, ублюдок!{x'";
      break;
    case 1: message = "С яростным криком $c1 атакует $C4.";
      break;
    case 2: message = "$c1 произносит '{GКакого Адрона ты тут делаешь?{x'";
      break;
    case 3: message = "$c1 хрустнув пальцами, произносит '{GЧувтвуешь удачу?{x'";
      break;
    case 4: message = "$c1 произносит '{GТут нет охранников, ты попал!{x'";
      break;  
    case 5: message = "$c1 произносит '{GВремя присоединиться к остальным, сосунок.{x'";
      break;
    case 6: message = "$c1 произносит '{GДочирикался...{x'";
      break;
  }
  if (message != NULL) act(message,ch,NULL,victim,TO_ROOM);
  multi_hit( ch, victim);
  return TRUE;
}

bool spec_patrolman(CHAR_DATA *ch)
{
  CHAR_DATA *vch,*victim = NULL;
  OBJ_DATA *obj;
  char *message;
  int count = 0;

  if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
  ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL)
      return FALSE;

  /* look for a fight in the room */
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if (vch == ch) continue;

    if (vch->fighting != NULL)  /* break it up! */
    {
      if (number_range(0,count) == 0) victim = (vch->level > vch->fighting->level) 
        ? vch : vch->fighting;
      count++;
    }
  }

  if (victim == NULL || (IS_NPC(victim) && victim->spec_fun == ch->spec_fun))
    return FALSE;

  if (((obj = get_eq_char(ch,WEAR_NECK)) != NULL 
     && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE))
  {
    act("Ты сильно дуешь в $i4.",ch,obj,NULL,TO_CHAR);
    act("$c1 дует в $i4, ***WHEEEEEEEEEEEET***",ch,obj,NULL,TO_ROOM);

    for ( vch = char_list; vch != NULL; vch = vch->next )
    {
      if ( vch->in_room == NULL ) continue;

      if (vch->in_room != ch->in_room && vch->in_room->area == ch->in_room->area)
        stc( "Ты слышишь звук свистка.\n\r", vch );
    }
  }

  switch (number_range(0,6))
  {
    default:        message = NULL;         break;
    case 0: message = "$c1 вскрикивает '{MРуки за голову! По одному на выход!{x'";
      break;
    case 1: message = 
      "$c1 вскрикивает '{MОбщество в опастности!{x'";
      break;
    case 2: message = 
      "$c1 вздыхает '{mДети, погрязшие в разврате..{RЗАСЛУЖИВАЮТ СМЕРТИ!{x'";
      break;
    case 3: message = "$c1 вскрикивает '{MПрекратить немедленно!{x' и атакует.";
      break;
    case 4: message = "$c1 поднимает дубинку и принимается за работу.";
      break;
    case 5: message = 
      "$c1 предъявляет значок и ввязывается в драку.";
      break;
    case 6: message = "$c1 вскрикивает '{MНа пол, сосунки!{x'";
      break;
  }

  if (message != NULL) act(message,ch,NULL,NULL,TO_ROOM);
  multi_hit(ch,victim);
  return TRUE;
}

bool spec_nasty( CHAR_DATA *ch )
{
  CHAR_DATA *victim, *v_next;
  int64 gold;
 
  if (!IS_AWAKE(ch)) {return FALSE;}
 
  if (ch->position != POS_FIGHTING)
  {
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next)
    {
      v_next = victim->next_in_room;
      if (!IS_NPC(victim)
        && (victim->level > ch->level)
        && (victim->level < ch->level + 10))
      {
        do_function(ch, &do_backstab, victim->name);
        if (ch->position != POS_FIGHTING)
        {
          do_function(ch, &do_murder, victim->name);
        }
        /* should steal some coins right away? :) */
        return TRUE;
      }
    }
    return FALSE;    /*  No one to attack */
  }
  /* okay, we must be fighting.... steal some coins and flee */
  if ( (victim = ch->fighting) == NULL) return FALSE;// let's be paranoid....
 
  switch ( number_bits(2) )
  {
    case 0:  act( "$c1 срезает твой кошелек!",ch, NULL, victim, TO_VICT);
             act( "Ты срезаешь кошелек с $C2.",ch, NULL, victim, TO_CHAR);
             act( "Кто-то срезал кошелек с $C2!",ch, NULL, victim, TO_NOTVICT);
             gold = victim->gold / 2;  /* steal 50% of his gold */
             victim->gold -= gold;
             ch->gold     += gold;
             return TRUE;
    case 1:  do_function(ch, &do_flee, "");
             return TRUE;
    default: return FALSE;
  }
}

bool dragon( CHAR_DATA *ch, char *spell_name )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  int sn;

  if ( ch->position != POS_FIGHTING ) return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 3 ) == 0 )break;
  }
  if ( victim == NULL ) return FALSE;

  if ( ( sn = skill_lookup( spell_name ) ) < 0 )
        return FALSE;
  if ( !IS_NPC(victim) && victim->clan != NULL && IS_SET(victim->clan->flag, CLAN_WARRIOR)
       && IS_AFFECTED(victim, AFF_FORCEFIELD) 
       && number_percent() < 65 )
    {
      stc("{yТвоя ненависть к магии поглощает заклинание!{x\n\r",victim);
      act("{yНенависть {W$c2{y к магии поглощает заклинание!{x",victim,NULL,NULL,TO_ROOM);
     return FALSE; 
    }   
  (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim, TARGET_CHAR);
  return TRUE;
}

/* Special procedures for mobiles. */
bool spec_breath_any( CHAR_DATA *ch )
{
  if ( ch->position != POS_FIGHTING ) return FALSE;

  switch ( number_bits( 3 ) )
  {
    case 0: return spec_breath_fire             ( ch );
    case 1:
    case 2: return spec_breath_lightning        ( ch );
    case 3: return spec_breath_gas              ( ch );
    case 4: return spec_breath_acid             ( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost            ( ch );
  }
  return FALSE;
}

bool spec_breath_acid( CHAR_DATA *ch )
{
  return dragon( ch, "acid breath" );
}

bool spec_breath_fire( CHAR_DATA *ch )
{
  return dragon( ch, "fire breath" );
}

bool spec_breath_frost( CHAR_DATA *ch )
{
  return dragon( ch, "frost breath" );
}

bool spec_breath_gas( CHAR_DATA *ch )
{
  int sn;

  if ( ch->position != POS_FIGHTING ) return FALSE;

  if ( ( sn = skill_lookup( "gas breath" ) ) < 0 ) return FALSE;
  (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL,TARGET_CHAR);
  return TRUE;
}

bool spec_breath_lightning( CHAR_DATA *ch )
{
  return dragon( ch, "lightning breath" );
}

bool spec_cast_adept( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;

  if ( !IS_AWAKE(ch) ) return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim != ch && can_see( ch, victim,NOCHECK_LVL ) && number_bits( 1 ) == 0 
        && !IS_NPC(victim) && victim->level < 11) break;
  }
  if ( victim == NULL ) return FALSE;

  switch ( number_bits( 4 ) )
  {
    case 0:
      act( "$c1 бормочет 'abrazak'.", ch, NULL, NULL, TO_ROOM );
      spell_armor( skill_lookup( "armor" ), ch->level,ch,victim,TARGET_CHAR);
      return TRUE;

    case 1:
      act( "$c1 бормочет 'fido'.", ch, NULL, NULL, TO_ROOM );
      spell_bless( skill_lookup( "bless" ), ch->level,ch,victim,TARGET_CHAR);
      return TRUE;

    case 2:
      act("$c1 бормочет 'judicandus noselacri'.",ch,NULL,NULL,TO_ROOM);
      spell_cure_blindness( skill_lookup( "cure blindness" ),
            ch->level, ch, victim,TARGET_CHAR);
      return TRUE;

    case 3:
      act("$c1 бормочет 'judicandus dies'.", ch,NULL, NULL, TO_ROOM );
      spell_cure_light( skill_lookup( "cure light" ),
            ch->level, ch, victim,TARGET_CHAR);
      return TRUE;

    case 4:
      act( "$c1 бормочет 'judicandus sausabru'.",ch,NULL,NULL,TO_ROOM);
      spell_cure_poison( skill_lookup( "cure poison" ),
            ch->level, ch, victim,TARGET_CHAR);
      return TRUE;

    case 5:
      act("$c1 бормочет 'candusima'.", ch, NULL, NULL, TO_ROOM );
      spell_refresh( skill_lookup("refresh"),ch->level,ch,victim,TARGET_CHAR);
      return TRUE;

    case 6:
      act("$c1 бормочет 'judicandus eugzagz'.",ch,NULL,NULL,TO_ROOM);
      spell_cure_disease(skill_lookup("cure disease"),
            ch->level,ch,victim,TARGET_CHAR);
    }
  return FALSE;
}

bool spec_cast_cleric( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  char spell[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if ( ch->position != POS_FIGHTING ) return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 ) break;
  }

  if ( victim == NULL || victim==ch) return FALSE;

  for ( ;; )
  {
    int min_level;

    switch ( number_bits( 4 ) )
    {
      case  0: min_level = 0; do_printf(spell,"'blindness'");      break;        
      case  1: min_level = 3; do_printf(spell,"'cause serious'");  break;
      case  2: min_level = 7; do_printf(spell,"'earthquake'");     break;
      case  3: min_level = 9; do_printf(spell,"'cause critical'"); break;
      case  4: min_level = 10; do_printf(spell,"'spirit lash'");    break;
      case  5: min_level = 12; do_printf(spell,"'curse'");          break;
      case  6: min_level = 12; do_printf(spell,"'change sex'");     break;
      case  7: min_level = 13; do_printf(spell,"'flamestrike'");    break;
      case  8: min_level = 29; do_printf(spell,"'cure serious'"); victim = ch; break;
      case  9: min_level = 45; do_printf(spell,"'cure critical'"); victim = ch; break;
      case 10: min_level = 25; do_printf(spell,"'harm'");           break;
      case 11: min_level = 15; do_printf(spell,"'plague'");        break;
      case 12: min_level = 25; 
        if (IS_EVIL(ch)) {do_printf(spell,"'demonfire'");}
        else if (IS_GOOD(ch)) {do_printf(spell,"'ray of truth'");}
        else {do_printf(spell,"'harm'");}
        break;
      default: min_level = 16; do_printf(spell,"'dispel magic'");   break;
    }
    if ( ch->level >= min_level ) break;
  }
  do_printf(buf, " %s", victim->name);
  strcat(spell, buf);
  do_function(ch, &do_cast, spell);
  return TRUE;
}

bool spec_cast_judge( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  char *spell;
  int sn;
 
  if ( ch->position != POS_FIGHTING ) return FALSE;
 
  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 ) break;
  }
  if ( victim == NULL ) return FALSE;
  spell = "high explosive";
  if ( ( sn = skill_lookup( spell ) ) < 0 ) return FALSE;
  (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
  return TRUE;
}

bool spec_cast_mage( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  char spell[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if ( ch->position != POS_FIGHTING ) return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 ) break;
  }

  if ( victim == NULL || victim==ch) return FALSE;

  for ( ;; )
  {
    int min_level;

    switch ( number_bits( 4 ) )
    {
      case  0: min_level = 0; do_printf(spell,"'blindness'");      break;
      case  1: min_level = 3; do_printf(spell,"'chill touch'");    break;
      case  2: min_level = 7; do_printf(spell,"'weaken'");         break;
      case  3: min_level = 8; do_printf(spell,"'magic missile'");  break;
      case  4: min_level = 11; do_printf(spell,"'colour spray'");  break;
      case  5: min_level = 12; do_printf(spell,"'change sex'");    break;
      case  6: min_level = 13; do_printf(spell,"'energy'");        break;
      case  7: min_level = 15; do_printf(spell, "'fireball'");     break;
      case  8: min_level = 20; do_printf(spell, "'plague'");       break;
      case  9: min_level = 40; do_printf(spell, "'ensnare'");      break;      
      default: min_level = 20; do_printf(spell, "'acid'");         break;
    }
    if ( ch->level >= min_level )        break;
  }
  do_printf(buf, " %s", victim->name);
  strcat(spell, buf);
//  bug(spell, 0);
  do_function(ch, &do_cast, spell);
  return TRUE;
}

bool spec_cast_undead( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  char spell[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if ( ch->position != POS_FIGHTING ) return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 ) break;
  }

  if ( victim == NULL || victim==ch) return FALSE;

  for ( ;; )
  {
    int min_level;

    switch ( number_bits( 4 ) )
    {
      case  0: min_level = 0; do_printf(spell, "'curse'");          break;
      case  1: min_level = 3; do_printf(spell, "'weaken'");         break;
      case  2: min_level = 6; do_printf(spell, "'chill touch'");    break;
      case  3: min_level = 9; do_printf(spell, "'blindness'");      break;
      case  4: min_level = 12; do_printf(spell, "'poison'");        break;
      case  5: min_level = 15; do_printf(spell, "'energy drain'");  break;
      case  6: min_level = 18; do_printf(spell, "'harm'");          break;
      case  7: min_level = 21; do_printf(spell, "'teleport'");      break;
      case  8: min_level = 20; do_printf(spell, "'plague'");        break;
      case  9: min_level = 40; do_printf(spell, "'ensnare'");       break;
      default: min_level = 18; do_printf(spell, "'harm'");          break;
    }
    if ( ch->level >= min_level ) break;
  }
  do_printf(buf, " %s", victim->name);
  strcat(spell, buf);
  do_function(ch, &do_cast, spell);
  return TRUE;
}

bool spec_executioner( CHAR_DATA *ch )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int crime;

  if ( !IS_AWAKE(ch) || ch->fighting != NULL ) return FALSE;

  crime = 0;

  for ( victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
  {
    if (!can_see(ch,victim,NOCHECK_LVL)) continue;

    if ( !IS_NPC(victim) && (IS_SET(victim->act, PLR_WANTED) || victim->criminal>250 ))
    { crime = C_CRIMINAL; break; }

    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_RAPER))
    { crime = C_RAPER; break; }
        
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_ARMY))
    { crime = C_DEMBEL; break; }
  }

  if (!victim) return FALSE;

  if    (crime == C_RAPER && !IS_IMMORTAL(victim))
  {
    do_printf( buf, "%s НАСИЛЬНИК! {RГОТОВЬСЯ К РАСПЛАТЕ!!!{x", victim->name );
    do_function(ch, &do_yell, buf );      
    act("$c1 срывает с тебя одежду и {rжестоко насилует{x!!!",
                                           ch,NULL,victim,TO_VICT);
    act("$c1 срывает одежду c $C2 и {rжестоко насилует{x!!!",
                                           ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE(victim, 5*PULSE_VIOLENCE);
    act("$c1 с жестокой ухмылкой повторяет экзекуцию...{rты не можешь пошевелиться{x!",
                                           ch,NULL,victim,TO_VICT);
    act("$c1 жестоко мучает $C4 {rво второй раз{x...",                                             ch,NULL,victim,TO_NOTVICT);
    REM_BIT( victim->act, PLR_RAPER );

    victim->hit=-2;
    update_pos( victim );
    return TRUE;
  }

  if (crime == C_DEMBEL && victim->level>=21)
  {
    do_printf(buf, "Рядовой %s! Поздравляю, Вы уволены в запас! Можете идти.", victim->name);
    do_function(ch, &do_say, buf);
    do_printf(buf, "Вы закалились в нашей армии, будьте в мире таким же мужественным и бесстрашным.", victim->name);
    do_function(ch, &do_say, buf);
    do_printf(buf, "Получите от меня лично этот значок - он поможет вам в этой трудной жизни.", victim->name);
    do_function(ch, &do_say, buf);
    victim->gold += 5000;
    victim->questpoints += 250;
    obj_to_char(create_object(get_obj_index(OBJ_VNUM_ARMYARTEFACT),21),victim);
    REM_BIT( victim->act, PLR_ARMY );
    char_from_room(victim);
    char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR) );
    do_function(victim, &do_look, "auto" );
    return TRUE;
  } 

  do_printf( buf, "%s ПРЕСТУПНИК! {RКРОВИ И ОТМЩЕНИЯ!!!{x",victim->name );
  do_function(ch, &do_yell, buf );
  SET_BIT(victim->act,PLR_WANTED);
  multi_hit( ch, victim);
  return TRUE;
}

bool spec_fido( CHAR_DATA *ch )
{
  OBJ_DATA *corpse;
  OBJ_DATA *c_next;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  if ( !IS_AWAKE(ch) ) return FALSE;

  for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
  {
    c_next = corpse->next_content;
    if ( corpse->item_type != ITEM_CORPSE_NPC ) continue;

    act( "$c1 мерзко чавкая, пожирает труп.", ch, NULL, NULL, TO_ROOM );
    for ( obj = corpse->contains; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      obj_from_obj( obj );
      obj_to_room( obj, ch->in_room );
    }
    extract_obj( corpse );
    return TRUE;
  }
  return FALSE;
}

bool spec_guard( CHAR_DATA *ch )
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  CHAR_DATA *ech;
  char *crime;
  int max_evil;

  if ( !IS_AWAKE(ch) || ch->fighting != NULL ) return FALSE;
  if (IS_SET(ch->in_room->room_flags,ROOM_SAFE)) return FALSE;
  if (IS_SET(ch->in_room->ra,RAFF_SAFE_PLC)) return FALSE;

  max_evil = 300;
  ech      = NULL;
  crime    = "";

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( !IS_NPC(victim) && (IS_SET(victim->act, PLR_WANTED) || victim->criminal>50) 
    &&   can_see(ch,victim,CHECK_LVL))
    { crime = "ПРЕСТУПНИК"; break; }

    if ( victim->fighting != NULL
    &&   victim->fighting != ch
    &&   victim->fighting->questmob == NULL
    &&   victim->alignment < max_evil )
    {
      max_evil = victim->alignment;
      ech      = victim;
    }
  }

  if ( victim != NULL )
  {
    do_printf( buf, "%s %s! ЗАЩИТИТЬ НЕВИННЫХ! БАНЗАЙ!!",victim->name, crime );
    do_function(ch, &do_yell, buf );
    multi_hit( ch, victim);
    return TRUE;
  }

  if ( ech != NULL )
  {
    act( "$c1 вскрикивает 'ЗАЩИТИТЬ НЕВИННЫХ! БАНЗАЙ!!", ch, NULL, NULL, TO_ROOM );
    multi_hit( ch, ech);
    return TRUE;
  }
  return FALSE;
}

bool spec_janitor( CHAR_DATA *ch )
{
  OBJ_DATA *trash;
  OBJ_DATA *trash_next;

  if ( !IS_AWAKE(ch) ) return FALSE;

  for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
  {
    trash_next = trash->next_content;
    if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) || !can_loot(ch,trash)) continue;
    if ( trash->item_type == ITEM_DRINK_CON
      ||   trash->item_type == ITEM_TRASH
      ||   trash->cost < 10 )
    {
      act( "$c1 подбирает мусор.", ch, NULL, NULL, TO_ROOM );
      obj_from_room( trash );
      obj_to_char( trash, ch );
      return TRUE;
    }
  }
  return FALSE;
}

bool spec_mayor( CHAR_DATA *ch )
{
  static const char open_path[] =
        "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";
  static const char close_path[] =
        "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

  static const char *path;
  static int pos;
  static bool move;

  if ( !move )
  {
    if ( time_info.hour == 6 )
    {
      path = open_path;
      move = TRUE;
      pos  = 0;
    }
    if ( time_info.hour == 20 )
    {
      path = close_path;
      move = TRUE;
      pos  = 0;
    }
  }

  if ( ch->fighting != NULL ) return spec_cast_mage( ch );
  if ( !move || ch->position < POS_SLEEPING ) return FALSE;

  switch ( path[pos] )
  {
    case '0':
    case '1':
    case '2':
    case '3':
        do_move_char( ch, path[pos] - '0', FALSE, FALSE );
        break;
    case 'W':
        ch->position = POS_STANDING;
        act( "$c1 просыпается и громко зевает.", ch, NULL, NULL, TO_ROOM );
        break;
    case 'S':
        ch->position = POS_SLEEPING;
        act( "$c1 ложится и засыпает.", ch, NULL, NULL, TO_ROOM );
        break;
    case 'a':
        act( "$c1 произносит '{GПривет, дорогая!{x'", ch, NULL, NULL, TO_ROOM );
        break;
    case 'b':
        act( "$c1 произносит '{GЧто за вид...Надо что-то с этим сделать!{x'",
            ch, NULL, NULL, TO_ROOM );
        break;
    case 'c':
        act( "$c1 произносит '{GВандалы! Ничего святого в молодежи!{x'",
            ch, NULL, NULL, TO_ROOM );
        break;
    case 'd':
        act( "$c1 произносит '{GДобрый день, горожане!{x'", ch, NULL, NULL, TO_ROOM );
        break;
    case 'e':
        act( "$c1 произносит '{GОбьявляю город Мидгаард открытым!{x'",
            ch, NULL, NULL, TO_ROOM );
        break;
    case 'E':
        act( "$c1 произносит '{GОбьявляю город Мидгаард закрытым!{x'",
            ch, NULL, NULL, TO_ROOM );
        break;
    case 'O':
        do_function(ch, &do_open, "gate" );
        break;
    case 'C':
        do_function(ch, &do_close, "gate" );
        break;
    case '.' :
        move = FALSE;
        break;
    }
    pos++;
    return FALSE;
}

bool spec_poison( CHAR_DATA *ch )
{
  CHAR_DATA *victim;

  if ( ch->position != POS_FIGHTING
    || ( victim = ch->fighting ) == NULL
    ||   number_percent( ) > 2 * ch->level ) return FALSE;

  act( "Ты кусаешь $C4!",  ch, NULL, victim, TO_CHAR    );
  act( "$c1 кусает $C4!",  ch, NULL, victim, TO_NOTVICT );
  act( "$c1 кусает тебя!", ch, NULL, victim, TO_VICT    );
  spell_poison( gsn_poison, ch->level, ch, victim,TARGET_CHAR);
  return TRUE;
}

bool spec_summoner (CHAR_DATA *ch)
{
  if (ch->fighting !=NULL)
  return spec_cast_mage(ch);
  return FALSE;
}

bool spec_summonera (CHAR_DATA *ch)
{
  return spec_summoner(ch);
}

bool spec_thief( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  int64 gold,silver;

  if ( ch->position != POS_STANDING ) return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;

    if ( IS_NPC(victim)
    ||   victim->level >= LEVEL_IMMORTAL
    ||   number_bits( 5 ) != 0 
    ||   !can_see(ch,victim,CHECK_LVL))
        continue;

    if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
    {
      act( "Ты чувствуешь пальцы $c2 у себя в карманах!",ch, NULL, victim, TO_VICT );
      act( "$C1 чувствует пальцы $c2 в своих карманах!",
          ch, NULL, victim, TO_NOTVICT );
      return TRUE;
    }
    else
    {
      gold = victim->gold * UMIN(number_range(1,20),ch->level / 2) / 100;
      gold = UMIN(gold, ch->level * ch->level * 10 );
      ch->gold     += gold;
      victim->gold -= gold;
      silver = victim->silver * UMIN(number_range(1,20),ch->level/2)/100;
      silver = UMIN(silver,ch->level*ch->level * 25);
      ch->silver  += silver;
      victim->silver -= silver;
      return TRUE;
    }
  }
  return FALSE;
}

bool spec_questmaster (CHAR_DATA *ch)
{
  if (ch->fighting != NULL) return spec_cast_mage( ch );
  return FALSE;
}
