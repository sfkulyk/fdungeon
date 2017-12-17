// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"

// Local functions.
void    raw_kill        args( ( CHAR_DATA *victim ) );

void    affect_modify   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
int min_level(CHAR_DATA *ch,int sn);
int get_detect_level   (CHAR_DATA *ch, int cast_type, int sn);
int get_undetect_level (CHAR_DATA *victim, int cast_type, int sn);
int deity_char_power   ( CHAR_DATA *ch, int type, int subtype);

// friend stuff -- for NPC's mostly
bool is_friend(CHAR_DATA *ch,CHAR_DATA *victim)
{
  if (is_same_group(ch,victim)) return TRUE;
  if (!IS_NPC(ch)) return FALSE;

  if (!IS_NPC(victim))
  {
    if (IS_SET(ch->off_flags,ASSIST_PLAYERS)) return TRUE;
    else return FALSE;
  }

  if (IS_AFFECTED(ch,AFF_CHARM)) return FALSE;
  if (IS_SET(ch->off_flags,ASSIST_ALL)) return TRUE;
  if (ch->group && ch->group == victim->group) return TRUE;
  if (IS_SET(ch->off_flags,ASSIST_VNUM) 
    &&  ch->pIndexData == victim->pIndexData) return TRUE;

  if (IS_SET(ch->off_flags,ASSIST_RACE) && ch->race == victim->race) return TRUE;
     
  if (IS_SET(ch->off_flags,ASSIST_ALIGN)
    &&  !IS_SET(ch->act,ACT_NOALIGN) && !IS_SET(victim->act,ACT_NOALIGN)
    &&  ((IS_GOOD(ch) && IS_GOOD(victim))
    ||   (IS_EVIL(ch) && IS_EVIL(victim))
    ||   (IS_NEUTRAL(ch) && IS_NEUTRAL(victim)))) return TRUE;

  return FALSE;
}

// returns number of people on an object
int count_users(OBJ_DATA *obj)
{
  CHAR_DATA *fch;
  int count = 0;

  if (obj->in_room == NULL) return 0;

  for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
        if (fch->on == obj) count++;
  return count;
}

int weapon_type (const char *name)
{ 
  int type;
 
  for (type = 0; weapon_table[type].name != NULL; type++)
  {
    if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
      &&  !str_prefix(name,weapon_table[type].name))
        return weapon_table[type].type;
  }
  return WEAPON_EXOTIC;
}

int ch_skill_nok_nomessage( CHAR_DATA *ch , int skill )
{
 if ( !IS_NPC(ch) && CLEVEL_OK(ch,skill)) return 0; 
 return 1;
}

int ch_skill_nok( CHAR_DATA *ch, int skill )
{
 if (ch_skill_nok_nomessage(ch,skill))
 {
   stc("Ась?\n\r",ch);
   return 1;
 }
 return 0;
}


const char *item_name(int item_type)
{
  int type;

  for (type = 0; item_table[type].name != NULL; type++)
        if (item_type == item_table[type].type)
            return item_table[type].name;
  return "none";
}

const char *weapon_name( int weapon_type)
{
  int type;
  for (type = 0; weapon_table[type].name != NULL; type++)
        if (weapon_type == weapon_table[type].type)
            return weapon_table[type].name;
  return "exotic";
}

// for immunity, vulnerabiltiy, and resistant
// the 'globals' (magic and weapons) may be overriden
// three other cases -- wood, silver, and iron -- are checked in fight.c */
int check_immune(CHAR_DATA *ch, int dt, OBJ_DATA *obj)
{
  //bool immune=FALSE;
  int  bit=0;

  if (IS_SET(ch->imm_flags,IMM_ALL)) return IS_IMMUNE;
  if (dt == DAM_NONE) return IS_NORMAL;


  switch (dt)
  {
    case(DAM_BASH):     bit = IMM_BASH;      break;
    case(DAM_PIERCE):   bit = IMM_PIERCE;    break;
    case(DAM_SLASH):    bit = IMM_SLASH;     break;
    case(DAM_FIRE):     bit = IMM_FIRE;      break;
    case(DAM_COLD):     bit = IMM_COLD;      break;
    case(DAM_LIGHTNING):bit = IMM_LIGHTNING; break;
    case(DAM_ACID):     bit = IMM_ACID;      break;
    case(DAM_POISON):   bit = IMM_POISON;    break;
    case(DAM_NEGATIVE): bit = IMM_NEGATIVE;  break;
    case(DAM_HOLY):     bit = IMM_HOLY;      break;
    case(DAM_ENERGY):   bit = IMM_ENERGY;    break;
    case(DAM_MENTAL):   bit = IMM_MENTAL;    break;
    case(DAM_DISEASE):  bit = IMM_DISEASE;   break;
    case(DAM_DROWNING): bit = IMM_DROWNING;  break;
    case(DAM_LIGHT):    bit = IMM_LIGHT;     break;
    case(DAM_CHARM):    bit = IMM_CHARM;     break;
    case(DAM_SOUND):    bit = IMM_SOUND;     break;
    case(DAM_HARM):     bit = IMM_HARM;      break;
    default:            break;
  }

  //if (IS_SET(ch->imm_flags, bit)) return TRUE;
  if (IS_SET(ch->vuln_flags, bit)) return IS_VULNERABLE;
  if (IS_SET(ch->res_flags, bit )) return IS_RESISTANT;
  if (IS_SET(ch->imm_flags, bit )) return IS_IMMUNE;
  
  if (dt<= 3 && IS_SET(ch->vuln_flags,IMM_WEAPON)) return IS_VULNERABLE;
  if (dt>  3 && IS_SET(ch->vuln_flags,IMM_MAGIC )) return IS_VULNERABLE;
  if (dt<= 3 && IS_SET(ch->res_flags,IMM_WEAPON )) return IS_RESISTANT;
  if (dt>  3 && IS_SET(ch->res_flags,IMM_MAGIC  )) return IS_RESISTANT;
  if (dt<= 3 && IS_SET(ch->imm_flags,IMM_WEAPON )) return IS_IMMUNE;
  if (dt>  3 && IS_SET(ch->imm_flags,IMM_MAGIC  )) return IS_IMMUNE;

  return FALSE;
}

bool is_same_clan(CHAR_DATA *ch, CHAR_DATA *vch)
{ 
 if (!ch->clan || IS_SET(ch->clan->flag, CLAN_LONER)) return FALSE;
 else return (ch->clan == vch->clan);
}

// for returning skill information
int get_skill(CHAR_DATA *ch, int sn)
{
  int skill;

  if (sn == -1) skill = ch->level * 5 / 2;
  else if (sn < -1 || sn >= max_skill)
  {
    bug("Bad sn %d in get_skill.",sn);
    skill = 0;
  }
  else if (!IS_NPC(ch))
  {
    if (ch->level < min_level(ch,sn)) skill = 0;
    else skill = ch->pcdata->learned[sn];
    if (!check_clanspell(sn,ch->clan)) skill=0;

    if (IS_DEVOTED_ANY(ch) && ch->pcdata->favour < 0) 
      skill+=skill*(ch->pcdata->favour/100)/100; //0 favour - 100%, -5000 - 50%
  }
  else
  {
    if (skill_table[sn].spell_fun != spell_null)
      skill = 40 + 2 * ch->level;

    else if (sn == gsn_sneak || sn == gsn_hide)
      skill = ch->level * 2 + 20;

    else if ((sn == gsn_dodge && IS_SET(ch->off_flags,OFF_DODGE))
          || (sn == gsn_parry && IS_SET(ch->off_flags,OFF_PARRY))
          || (sn == gsn_blink && IS_SET(ch->off_flags,OFF_BLINK)))
        skill = ch->level * 2;

    else if (sn == gsn_shield_block) skill = 10 + 2 * ch->level;

    else if (sn == gsn_second_attack 
        && (IS_SET(ch->act,ACT_WARRIOR) || IS_SET(ch->act,ACT_THIEF)))
            skill = 10 + 3 * ch->level;

    else if (sn == gsn_third_attack && IS_SET(ch->act,ACT_WARRIOR))
            skill = 4 * ch->level - 40;

    else if (sn == gsn_hand_to_hand)
            skill = 40 + 2 * ch->level;

    else if (sn == gsn_trip && IS_SET(ch->off_flags,OFF_TRIP))
            skill = 10 + 3 * ch->level;

    else if (sn == gsn_bash && IS_SET(ch->off_flags,OFF_BASH))
            skill = 10 + 3 * ch->level;

    else if (sn == gsn_disarm 
             &&  (IS_SET(ch->off_flags,OFF_DISARM) 
             ||   IS_SET(ch->act,ACT_WARRIOR)
             ||   IS_SET(ch->act,ACT_THIEF)))
            skill = 20 + 3 * ch->level;

    else if (sn == gsn_berserk && IS_SET(ch->off_flags,OFF_BERSERK))
            skill = 3 * ch->level;

    else if (sn == gsn_kick) skill = 10 + 3 * ch->level;

    else if (sn == gsn_backstab && IS_SET(ch->act,ACT_THIEF))
            skill = 20 + 2 * ch->level;

    else if (sn == gsn_rescue) skill = 40 + ch->level; 

    else if (sn == gsn_recall) skill = 40 + ch->level;

    else if (sn == gsn_sword ||  sn == gsn_dagger
        ||  sn == gsn_spear  ||  sn == gsn_staff
        ||  sn == gsn_mace   ||  sn == gsn_axe
        ||  sn == gsn_flail  ||  sn == gsn_whip
        ||  sn == gsn_polearm) skill = 40 + 5 * ch->level / 2;

    else skill = 100;
  }

  if (ch->daze > 0)
  {
    bool psy=IS_SET(race_table[ch->race].spec, SPEC_PSY)?TRUE:FALSE;

    if (skill_table[sn].spell_fun != spell_null)
    {
      if (psy) skill=(skill*9)/10;
      else skill=(skill*2)/3;
    }
    else
    {
      if (psy) skill=(skill*9)/10;
      skill = (skill*3)/4;
    }
  }
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]>10) skill=9*skill/10;
  if (ch->godcurse > 0) skill = UMAX(1,skill - ch->godcurse);
  return URANGE(0,skill,100);
}

// for returning weapon information
int get_weapon_sn(CHAR_DATA *ch, bool rhand)
{
  OBJ_DATA *wield;
  int sn;

  wield = (rhand) ? get_eq_char( ch, WEAR_RHAND ):get_eq_char( ch, WEAR_LHAND ) ;

  if (wield == NULL || wield->item_type != ITEM_WEAPON) sn = gsn_hand_to_hand;
  else switch (wield->value[0])
  {
    default :               sn = -1;          break;
    case(WEAPON_SWORD):     sn = gsn_sword;   break;
    case(WEAPON_DAGGER):    sn = gsn_dagger;  break;
    case(WEAPON_SPEAR):     sn = gsn_spear;   break;
    case(WEAPON_STAFF):     sn = gsn_staff;   break;
    case(WEAPON_MACE):      sn = gsn_mace;    break;
    case(WEAPON_AXE):       sn = gsn_axe;     break;
    case(WEAPON_FLAIL):     sn = gsn_flail;   break;
    case(WEAPON_WHIP):      sn = gsn_whip;    break;
    case(WEAPON_POLEARM):   sn = gsn_polearm; break;
  }
  return sn;
}

int get_weapon_skill(CHAR_DATA *ch, int sn)
{
  int skill;

  /* -1 is exotic */
  if (IS_NPC(ch))
  {
    if (sn == -1) skill = 3 * ch->level;
    else if (sn == gsn_hand_to_hand) skill = 40 + 2 * ch->level;
    else skill = 40 + 5 * ch->level / 2;
  }
  else
  {
    if (sn == -1) skill = 3 * ch->level;
    else skill = ch->pcdata->learned[sn];
  }
  return URANGE(0,skill,100);
} 


// used to de-screw characters
void reset_char(CHAR_DATA *ch)
{
  int loc,mod,stat;
  OBJ_DATA *obj;
  AFFECT_DATA *af;
  int i;

  if (IS_NPC(ch)) return;

  if (ch->pcdata->perm_hit == 0 
    ||  ch->pcdata->perm_mana == 0
    ||  ch->pcdata->perm_move == 0
    ||  ch->pcdata->last_level == 0)
  {
    // do a FULL reset
    for (loc = 0; loc < MAX_WEAR; loc++)
    {  
      obj = get_eq_char(ch,loc);
      if (obj == NULL) continue;
      if (!obj->enchanted)
        for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
        {
          mod = af->modifier;
          switch(af->location)
          {
            case APPLY_SEX:     ch->sex         -= mod;
                                if (ch->sex<0) ch->sex+=2;
                                if (ch->sex>2) ch->sex-=2;
                                break;
            case APPLY_MANA:    ch->max_mana    -= mod;         break;
            case APPLY_HIT:     ch->max_hit     -= mod;         break;
            case APPLY_MOVE:    ch->max_move    -= mod;         break;
          }
        }

      for ( af = obj->affected; af != NULL; af = af->next )
      {
        mod = af->modifier;
        switch(af->location)
        {
          case APPLY_SEX:  ch->sex -= mod;
                           if (ch->sex<0) ch->sex+=2;
                           if (ch->sex>2) ch->sex-=2;
                           break;
          case APPLY_MANA: ch->max_mana    -= mod;         break;
          case APPLY_HIT:  ch->max_hit     -= mod;         break;
          case APPLY_MOVE: ch->max_move    -= mod;         break;
        }
      }
    }
    // now reset the permanent stats
    ch->pcdata->perm_hit    = ch->max_hit;
    ch->pcdata->perm_mana   = ch->max_mana;
    ch->pcdata->perm_move   = ch->max_move;
    ch->pcdata->last_level  = ch->played/3600;
    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
    {
      if (ch->sex > 0 && ch->sex < 3) ch->pcdata->true_sex        = ch->sex;
      else                            ch->pcdata->true_sex        = 0;
    }
  }

  // now restore the character to his/her true condition
  for (stat = 0; stat < MAX_STATS; stat++)
      ch->mod_stat[stat] = 0;

  if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
      ch->pcdata->true_sex = 0; 
  ch->sex      = ch->pcdata->true_sex;
  ch->max_hit  = ch->pcdata->perm_hit;
  ch->max_mana = ch->pcdata->perm_mana;
  ch->max_move = ch->pcdata->perm_move;
 
  for (i = 0; i < 4; i++) ch->armor[i]    = 100;

  ch->hitroll  = 0;
  ch->damroll  = 0;
  ch->saving_throw = 0;

  /* now start adding back the effects */
  for (loc = 0; loc < MAX_WEAR; loc++)
  {
      obj = get_eq_char(ch,loc);
      if (obj == NULL)
          continue;
      for (i = 0; i < 4; i++)
          ch->armor[i] -= apply_ac( obj, loc, i );

      if (!obj->enchanted)
      for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
      {
        mod = af->modifier;
        switch(af->location)
        {
          case APPLY_STR: ch->mod_stat[STAT_STR]  += mod; break;
          case APPLY_DEX: ch->mod_stat[STAT_DEX]  += mod; break;
          case APPLY_INT: ch->mod_stat[STAT_INT]  += mod; break;
          case APPLY_WIS: ch->mod_stat[STAT_WIS]  += mod; break;
          case APPLY_CON: ch->mod_stat[STAT_CON]  += mod; break;

          case APPLY_SEX: ch->sex += mod;
                          if (ch->sex<0) ch->sex+=2;
                          if (ch->sex>2) ch->sex-=2;
                          break;
          case APPLY_MANA:ch->max_mana += mod; break;
          case APPLY_HIT: ch->max_hit  += mod; break;
          case APPLY_MOVE:ch->max_move += mod; break;

          case APPLY_AC:          
              for (i = 0; i < 4; i ++) ch->armor[i] += mod; 
              break;
          case APPLY_HITROLL: ch->hitroll += mod; break;
          case APPLY_DAMROLL: ch->damroll += mod; break;
      
          case APPLY_SAVES:        ch->saving_throw += mod; break;
          case APPLY_SAVING_ROD:   ch->saving_throw += mod; break;
          case APPLY_SAVING_PETRI: ch->saving_throw += mod; break;
          case APPLY_SAVING_BREATH:ch->saving_throw += mod; break;
          case APPLY_SAVING_SPELL: ch->saving_throw += mod; break;
        }
      }

      for ( af = obj->affected; af != NULL; af = af->next )
      {
        mod = af->modifier;
        switch(af->location)
        {
          case APPLY_STR:         ch->mod_stat[STAT_STR]  += mod; break;
          case APPLY_DEX:         ch->mod_stat[STAT_DEX]  += mod; break;
          case APPLY_INT:         ch->mod_stat[STAT_INT]  += mod; break;
          case APPLY_WIS:         ch->mod_stat[STAT_WIS]  += mod; break;
          case APPLY_CON:         ch->mod_stat[STAT_CON]  += mod; break;

          case APPLY_SEX:         ch->sex                 += mod; break;
          case APPLY_MANA:        ch->max_mana            += mod; break;
          case APPLY_HIT:         ch->max_hit             += mod; break;
          case APPLY_MOVE:        ch->max_move            += mod; break;

          case APPLY_AC:
              for (i = 0; i < 4; i ++) ch->armor[i] += mod;
              break;
          case APPLY_HITROLL:     ch->hitroll             += mod; break;
          case APPLY_DAMROLL:     ch->damroll             += mod; break;

          case APPLY_SAVES:         ch->saving_throw += mod; break;
          case APPLY_SAVING_ROD:          ch->saving_throw += mod; break;
          case APPLY_SAVING_PETRI:        ch->saving_throw += mod; break;
          case APPLY_SAVING_BREATH:       ch->saving_throw += mod; break;
          case APPLY_SAVING_SPELL:        ch->saving_throw += mod; break;
        }
      }
  }

  // now add back spell effects
  for (af = ch->affected; af != NULL; af = af->next)
  {
    mod = af->modifier;
    switch(af->location)
    {
      case APPLY_STR: ch->mod_stat[STAT_STR]  += mod; break;
      case APPLY_DEX: ch->mod_stat[STAT_DEX]  += mod; break;
      case APPLY_INT: ch->mod_stat[STAT_INT]  += mod; break;
      case APPLY_WIS: ch->mod_stat[STAT_WIS]  += mod; break;
      case APPLY_CON: ch->mod_stat[STAT_CON]  += mod; break;

      case APPLY_SEX: ch->sex     += mod; break;
      case APPLY_MANA:ch->max_mana+= mod; break;
      case APPLY_HIT: ch->max_hit += mod; break;
      case APPLY_MOVE:ch->max_move+= mod; break;

      case APPLY_AC:
          for (i = 0; i < 4; i ++) ch->armor[i] += mod;
          break;
      case APPLY_HITROLL: ch->hitroll+= mod; break;
      case APPLY_DAMROLL: ch->damroll+= mod; break;
 
      case APPLY_SAVES:        ch->saving_throw += mod; break;
      case APPLY_SAVING_ROD:   ch->saving_throw += mod; break;
      case APPLY_SAVING_PETRI: ch->saving_throw += mod; break;
      case APPLY_SAVING_BREATH:ch->saving_throw += mod; break;
      case APPLY_SAVING_SPELL: ch->saving_throw += mod; break;
    } 
  }

  // make sure sex is RIGHT!!!!
  if (ch->sex < 0 || ch->sex > 2) ch->sex = ch->pcdata->true_sex;
}


// Retrieve a character's trusted level for permission checking.
int get_trust( CHAR_DATA *ch )
{
  if (IS_NPC(ch)) return 101;
  return UMAX(ch->level,ch->trust);
}

// Retrieve a character's age.
int get_age( CHAR_DATA *ch )
{
  return 17 + ( ch->played + (int) (current_time - ch->logon) ) / 72000;
}

// command for retrieving stats
int get_curr_stat( CHAR_DATA *ch, int stat )
{
  int max,mod;

  mod = (ch->race == race_lookup("human")) ? 3 : 2;
  max = race_table[ch->race].high_stats[stat]-mod+dcp(ch,1,stat+1);
  if (ch->classmag && stat==STAT_INT) max += mod;
  if (ch->classcle && stat==STAT_WIS) max += mod;
  if (ch->classthi && stat==STAT_DEX) max += mod;
  if (ch->classwar && stat==STAT_STR) max += mod;
  if (IS_SET(ch->act,PLR_LASTREMORT && stat == STAT_CON)) max += mod;
  if (class_table[ch->class[0]].attr_prime == stat ) max += 1;

  if (IS_SET(race_table[ch->race].spec,SPEC_WATERWALK) && stat==STAT_DEX &&
       ( ch->in_room->sector_type==SECT_WATER_SWIM
      || ch->in_room->sector_type==SECT_WATER_NOSWIM
      || ch->in_room->sector_type==SECT_UWATER) ) max+=3;

  if (IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE) && (time_info.hour<20 && time_info.hour>5))
  {
    if (stat==STAT_STR) max-=5; // Vampires leaks, when sun is shining.
    if (stat==STAT_DEX) max-=5;
  }
  if (IS_NPC(ch))
    return URANGE(1, ch->perm_stat[stat] + ch->level/10 ,36);
  return URANGE(1,ch->perm_stat[stat] + ch->mod_stat[stat]+dcp(ch,1,stat+1), max);
}


// command for returning max training score
int get_max_train( CHAR_DATA *ch, int stat )
{
  int max;

  if (stat<STAT_HP) max = race_table[ch->race].max_stats[stat];
  else
  {
    if (ch->remort==0) return 2*ch->level;
    if (ch->remort==1) return ch->level+ch->level/2;
    return ch->level;
  }

  if (ch->classmag && (class_table[0].attr_prime == stat)) max += 2;
  if (ch->classcle && (class_table[1].attr_prime == stat)) max += 2;
  if (ch->classthi && (class_table[2].attr_prime == stat)) max += 2;
  if (ch->classwar && (class_table[3].attr_prime == stat)) max += 2;
  if (IS_SET(race_table[ch->race].spec,SPEC_TRAIN)) max += 1;
  if (class_table[ch->class[0]].attr_prime == stat ) max += 1;
  return max;
}
        

// Retrieve a character's carry capacity.
int can_carry_n( CHAR_DATA *ch )
{
  if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL ) return 90000;
  return MAX_WEAR +  2 * get_curr_stat(ch,STAT_DEX) + ch->level;
}

// Retrieve a character's carry capacity.
int can_carry_w( CHAR_DATA *ch )
{
  if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL ) return 90000;
  return str_app[get_curr_stat(ch,STAT_STR)].carry * 10 + ch->level * 25;
}

/* See if a string is one of the names of an object. */
bool is_name ( const char *str, const char *namelist )
{
  char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
  const char *list, *string;

  if (namelist == NULL || namelist[0] == '\0') return FALSE;

  if (str[0] == '\0') return FALSE;

  string = str;
  for ( ; ; )
  {
    str = one_argument(str,part);

    if (part[0] == '\0' ) return TRUE;
    list = namelist;
    for ( ; ; )
    {
      list = one_argument(list,name);
      if (name[0] == '\0') return FALSE;
      if (!str_prefix(string,name)) return TRUE;
      if (!str_prefix(part,name)) break;
    }
  }
}

bool is_exact_name(const char *str, const char *namelist )
{
  char name[MAX_STRING_LENGTH];

  if (namelist == NULL) return FALSE;

  for ( ; ; )
  {
    namelist = one_argument( namelist, name );
    if ( name[0] == '\0' ) return FALSE;
    if ( !str_cmp( str, name ) ) return TRUE;
  }
}

/* enchanted stuff for eq */
void affect_enchant(OBJ_DATA *obj)
{
  /* okay, move all the old flags into new vectors if we have to */
  if (!obj->enchanted)
  {
    AFFECT_DATA *paf, *af_new;
    obj->enchanted = TRUE;
    if (!obj->owner)
    {
      obj->owner = str_dup("(public)");
    }

    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      af_new = new_affect();
      af_new->next = obj->affected;
      obj->affected = af_new;
 
      af_new->where       = paf->where;
      af_new->type        = UMAX(0,paf->type);
      af_new->level       = paf->level;
      af_new->duration    = paf->duration;
      af_new->location    = paf->location;
      af_new->modifier    = paf->modifier;
      af_new->bitvector   = paf->bitvector;
    }
  }
}

void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
  OBJ_DATA *wield;
  int mod,i;

  mod = paf->modifier;
  if ( fAdd )
  {
    switch (paf->where)
    {
      case TO_AFFECTS:
        SET_BIT(ch->affected_by, paf->bitvector);
        break;
      case TO_IMMUNE:
        SET_BIT(ch->imm_flags,paf->bitvector);
        break;
      case TO_RESIST:
        SET_BIT(ch->res_flags,paf->bitvector);
        break;
      case TO_VULN:
        SET_BIT(ch->vuln_flags,paf->bitvector);
        break;
    }
  }
  else
  {
    switch (paf->where)
    {
      case TO_AFFECTS:
        REM_BIT(ch->affected_by, paf->bitvector);
        break;
      case TO_IMMUNE:
        REM_BIT(ch->imm_flags,paf->bitvector);
        break;
      case TO_RESIST:
        REM_BIT(ch->res_flags,paf->bitvector);
        break;
      case TO_VULN:
        REM_BIT(ch->vuln_flags,paf->bitvector);
        break;
    }
    mod = 0 - mod;
  }

  switch ( paf->location )
  {
    default:
      bug( "Affect_modify: unknown location %d.", paf->location );
      return;

    case APPLY_NONE:  break;
    case APPLY_STR:   ch->mod_stat[STAT_STR]  += mod; break;
    case APPLY_DEX:   ch->mod_stat[STAT_DEX]  += mod; break;
    case APPLY_INT:   ch->mod_stat[STAT_INT]  += mod; break;
    case APPLY_WIS:   ch->mod_stat[STAT_WIS]  += mod; break;
    case APPLY_CON:   ch->mod_stat[STAT_CON]  += mod; break;
    case APPLY_SEX:   ch->sex                 += mod; break;
    case APPLY_CLASS: break;
    case APPLY_LEVEL: break;
    case APPLY_AGE:   break;
    case APPLY_HEIGHT:break;
    case APPLY_WEIGHT:break;
    case APPLY_MANA:  ch->max_mana            += mod; break;
    case APPLY_HIT:   ch->max_hit             += mod; break;
    case APPLY_MOVE:  ch->max_move            += mod; break;
    case APPLY_GOLD:  break;
    case APPLY_EXP:   break;
    case APPLY_AC: for (i = 0; i < 4; i ++) ch->armor[i] += mod;break;
    case APPLY_HITROLL:      ch->hitroll      += mod; break;
    case APPLY_DAMROLL:      ch->damroll      += mod; break;
    case APPLY_SAVES:        ch->saving_throw += mod; break;
    case APPLY_SAVING_ROD:   ch->saving_throw += mod; break;
    case APPLY_SAVING_PETRI: ch->saving_throw += mod; break;
    case APPLY_SAVING_BREATH:ch->saving_throw += mod; break;
    case APPLY_SAVING_SPELL: ch->saving_throw += mod; break;
    case APPLY_SPELL_AFFECT:
      if ( fAdd ) SET_BIT(ch->affected_by,paf->modifier);
      else REM_BIT(ch->affected_by,paf->modifier);
     break;
  }

  // Check for weapon wielding.
  // Guard against recursion (for weapons with affects).
  if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_RHAND ) ) != NULL
    &&   get_obj_weight(wield) > (str_app[get_curr_stat(ch,STAT_STR)].wield*10))
  {
    static int depth;

    if ( depth == 0 )
    {
      depth++;
      act( "Ты бросаешь $i4.", ch, wield, NULL, TO_CHAR );
      act( "$c1 бросает $i4.", ch, wield, NULL, TO_ROOM );
      obj_from_char( wield );
      obj_to_room( wield, ch->in_room );
      depth--;
    }
  }
}

// find an effect in an affect list
AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn)
{
  AFFECT_DATA *paf_find;
  
  for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
  {
    if ( paf_find->type == sn )
    return paf_find;
  }
  return NULL;
}

// fix object affects when removing one
void affect_check(CHAR_DATA *ch,int where,int64 vector)
{
  AFFECT_DATA *paf;
  OBJ_DATA *obj;

  if (where == TO_OBJECT || where == TO_WEAPON || vector == 0) return;

  for (paf = ch->affected; paf != NULL; paf = paf->next)
    if (paf->where == where && paf->bitvector == vector)
    {
      switch (where)
      {
        case TO_AFFECTS:
          SET_BIT(ch->affected_by,vector);
          break;
        case TO_IMMUNE:
          SET_BIT(ch->imm_flags,vector);   
          break;
        case TO_RESIST:
          SET_BIT(ch->res_flags,vector);
          break;
        case TO_VULN:
          SET_BIT(ch->vuln_flags,vector);
          break;
      }
      return;
    }

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == -1) continue;

    for (paf = obj->affected; paf != NULL; paf = paf->next)
    if (paf->where == where && paf->bitvector == vector)
    {
      switch (where)
      {
        case TO_AFFECTS:
          SET_BIT(ch->affected_by,vector);
          break;
        case TO_IMMUNE:
          SET_BIT(ch->imm_flags,vector);
          break;
        case TO_RESIST:
          SET_BIT(ch->res_flags,vector);
          break;
        case TO_VULN:
          SET_BIT(ch->vuln_flags,vector);
      }
      return;
    }

    if (obj->enchanted) continue;

    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      if (paf->where == where && paf->bitvector == vector)
      {
        switch (where)
        {
          case TO_AFFECTS:
            SET_BIT(ch->affected_by,vector);
            break;
          case TO_IMMUNE:
            SET_BIT(ch->imm_flags,vector);
            break;
          case TO_RESIST:
            SET_BIT(ch->res_flags,vector);
            break;
          case TO_VULN:
            SET_BIT(ch->vuln_flags,vector);
            break;
        }
        return;
      }
  }
}

// Give an affect to a char.
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
  AFFECT_DATA *paf_new;
  paf_new = new_affect();
  *paf_new       = *paf;
  VALIDATE(paf);      /* in case we missed it when we set up paf */
  paf_new->next  = ch->affected;
  ch->affected   = paf_new;
  affect_modify( ch, paf_new, TRUE );
}

/* give an affect to an object */
void affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf)
{
  AFFECT_DATA *paf_new;

  paf_new = new_affect();
  *paf_new      = *paf;
  VALIDATE(paf); // in case we missed it when we set up paf
  paf_new->next = obj->affected;
  obj->affected = paf_new;
  // apply any affect vectors to the object's extra_flags
  if (paf->bitvector)
    switch (paf->where)
    {
      case TO_OBJECT:
        SET_BIT(obj->extra_flags,paf->bitvector);
        break;
      case TO_WEAPON:
        if (obj->item_type == ITEM_WEAPON) SET_BIT(obj->value[4],paf->bitvector);
         break;
    }
}



// Remove an affect from a char.
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
  int where;
  int64 vector;

  if ( ch->affected == NULL )
  {
    bug( "Affect_remove: no affect.", 0 );
    return;
  }

  affect_modify( ch, paf, FALSE );
  where = paf->where;
  vector = paf->bitvector;

  if ( paf == ch->affected ) ch->affected = paf->next;
  else
  {
    AFFECT_DATA *prev;
    for ( prev = ch->affected; prev != NULL; prev = prev->next )
    {
      if ( prev->next == paf )
      {
        prev->next = paf->next;
        break;
      }
    }
    if ( prev == NULL )
    {
      bug( "Affect_remove: cannot find paf.", 0 );
      return;
    }
  }
  free_affect(paf);
  affect_check(ch,where,vector);
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf)
{
  int where;
  int64 vector;

  if ( obj->affected == NULL )
  {
    bug( "Affect_remove_object: no affect.", 0 );
    return;
  }
  if (obj->carried_by != NULL && obj->wear_loc != -1)
      affect_modify( obj->carried_by, paf, FALSE );
  where = paf->where;
  vector = paf->bitvector;
  // remove flags from the object if needed
  if (paf->bitvector)
    switch( paf->where)
    {
      case TO_OBJECT:
        REM_BIT(obj->extra_flags,paf->bitvector);
        break;
      case TO_WEAPON:
        if (obj->item_type == ITEM_WEAPON) REM_BIT(obj->value[4],paf->bitvector);
         break;
    }
  if ( paf == obj->affected )
  {
    obj->affected = paf->next;
  }
  else
  {
    AFFECT_DATA *prev;
    for ( prev = obj->affected; prev != NULL; prev = prev->next )
    {
      if ( prev->next == paf )
      {
        prev->next = paf->next;
        break;
      }
    }
    if ( prev == NULL )
    {
      bug( "Affect_remove_object: cannot find paf.", 0 );
      return;
    }
  }
  free_affect(paf);
  if (obj->carried_by != NULL && obj->wear_loc != -1)
      affect_check(obj->carried_by,where,vector);
}

// Strip all affects of a given sn.
void affect_strip( CHAR_DATA *ch, int sn )
{
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  for ( paf = ch->affected; paf != NULL; paf = paf_next )
  {
    paf_next = paf->next;
    if ( paf->type == sn ) affect_remove( ch, paf );
  }
}

// Return true if a char is affected by a spell.
bool is_affected( CHAR_DATA *ch, int sn )
{
  AFFECT_DATA *paf;
  if (!ch) return FALSE;
  for ( paf = ch->affected; paf; paf = paf->next ) if ( paf->type == sn ) return TRUE;
  return FALSE;
}

// Add or enhance an affect.
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
  AFFECT_DATA *paf_old;
  bool found;

  found = FALSE;
  for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
  {
    if ( paf_old->type == paf->type )
    {
      paf->level = (paf->level += paf_old->level) / 2;
      paf->duration += paf_old->duration;
      paf->modifier += paf_old->modifier;
      affect_remove( ch, paf_old );
      break;
    }
  }
  affect_to_char( ch, paf );
}

// Move a char out of a room.
void char_from_room( CHAR_DATA *ch )
{
  OBJ_DATA *obj;

  if (!ch->in_room)
  {
    bug( "Char_from_room: NULL.", 0 );
    return;
  }

   if ( !IS_NPC(ch) ) --ch->in_room->area->nplayer;

   if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room->light > 0 )
        --ch->in_room->light;

  if ( ch == ch->in_room->people ) ch->in_room->people = ch->next_in_room;
  else
  {
    CHAR_DATA *prev;

    for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
    {
      if ( prev->next_in_room == ch )
      {
        prev->next_in_room = ch->next_in_room;
        break;
      }
    }
//    Sometimes ch->in_room are not in room->people.
//    if (!prev) bug( "Char_from_room: ch not found.", 0 );
  }
  ch->in_room      = NULL;
  ch->next_in_room = NULL;
  ch->on           = NULL;
}

// Move a char into a room.
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
  OBJ_DATA *obj;

  if ( pRoomIndex == NULL )
  {
    ROOM_INDEX_DATA *room;
    bug( "Char_to_room: NULL.", 0 );
    if ((room = get_room_index(ROOM_VNUM_TEMPLE)) != NULL) char_to_room(ch,room);
    return;
  }

  ch->in_room         = pRoomIndex;
  ch->next_in_room    = pRoomIndex->people;
  pRoomIndex->people  = ch;

  if ( !IS_NPC(ch) )
  {
    if (ch->in_room->area->empty)
    {
      ch->in_room->area->empty = FALSE;
      ch->in_room->area->age = 0;
    }
    ++ch->in_room->area->nplayer;
  }

  if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0 )
        ++ch->in_room->light;
        
  if (IS_AFFECTED(ch,AFF_PLAGUE))
  {
    AFFECT_DATA *af, plague;
    CHAR_DATA *vch;
        
    for ( af = ch->affected; af != NULL; af = af->next )
    {
      if (af->type == gsn_plague) break;
    }
        
    if (af == NULL)
    {
      REM_BIT(ch->affected_by,AFF_PLAGUE);
      return;
    }
        
    if (af->level == 1) return;
        
    plague.where            = TO_AFFECTS;
    plague.type             = gsn_plague;
    plague.level            = af->level - 1; 
    plague.duration         = number_range(1,2 * plague.level);
    plague.location         = APPLY_STR;
    plague.modifier         = -5;
    plague.bitvector        = AFF_PLAGUE;
        
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (!saves_spell(plague.level - 2,vch,DAM_DISEASE) 
        && !IS_IMMORTAL(vch)
        && !IS_AFFECTED(vch,AFF_PLAGUE)
        && number_bits(6) == 0)
      {
        stc("Ты чувствуешь жар и боль.\n\r",vch);
        act("$c1 дрожит и выглядит очень больным.",vch,NULL,NULL,TO_ROOM);
        affect_join(vch,&plague);
      }
    }
  }
}

/* Give an obj to a char. */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
  obj->next_content    = ch->carrying;
  ch->carrying         = obj;
  obj->carried_by      = ch;
  obj->in_room         = NULL;
  obj->in_obj          = NULL;
  if (!IS_EAR(obj))   ch->carry_number += get_obj_number( obj );
  ch->carry_weight    += get_obj_weight( obj );
  WILLSAVE(ch);
}

// Take an obj from its character.
void obj_from_char( OBJ_DATA *obj )
{
  CHAR_DATA *ch;

  if ( ( ch = obj->carried_by ) == NULL )
  {
      bug( "Obj_from_char: null ch.", 0 );
      return;
  }   

  if ( obj->wear_loc != WEAR_NONE )
      unequip_char( ch, obj );

  if ( ch->carrying == obj )
  {
      ch->carrying = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }
    if ( prev == NULL ) bug( "Obj_from_char: obj not in list.", 0 );
  }

  obj->carried_by      = NULL;
  obj->next_content    = NULL;
  if (!IS_EAR(obj))   ch->carry_number -= get_obj_number( obj ); //ear weights 0
  ch->carry_weight    -= get_obj_weight( obj );
  WILLSAVE(ch);
}

// Find the ac value of an obj, including position effect.
int apply_ac( OBJ_DATA *obj, int iWear, int type )
{
  if ( obj->item_type != ITEM_ARMOR ) return 0;

  switch ( iWear )
  {
  case WEAR_BODY:     return  3 * (int)obj->value[type];
  case WEAR_HEAD:     return  2 * (int)obj->value[type];
  case WEAR_LEGS:     return  2 * (int)obj->value[type];
  case WEAR_FEET:     return (int) obj->value[type];
  case WEAR_HANDS:    return (int) obj->value[type];
  case WEAR_ARMS:     return (int) obj->value[type];
  case WEAR_SHIELD:   return (int) obj->value[type];
  case WEAR_NECK:     return (int) obj->value[type];
  case WEAR_ABOUT:    return  2 * (int)obj->value[type];
  case WEAR_WAIST:    return (int) obj->value[type];
  case WEAR_WRIST_L:  return (int) obj->value[type];
  case WEAR_WRIST_R:  return (int) obj->value[type];
  case WEAR_HOLD:     return (int) obj->value[type];
  }
  return 0;
}

// Find a piece of eq on a character.
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
  OBJ_DATA *obj;

  if (ch == NULL) return NULL;

  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
  {
    if ( obj->wear_loc == iWear ) return obj;
  }
  return NULL;
}

// Equip a char with an obj.
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
  AFFECT_DATA *paf;
  int i;

  if ( get_eq_char( ch, iWear ) != NULL )
  {
    char buf[MAX_STRING_LENGTH];

    do_printf(buf, "Equip_char: %u already equipped (%d).",IS_NPC(ch)?ch->pIndexData->vnum:0, iWear );
    bug(buf, 0);
    return;
  }

  if ( !IS_IMMORTAL(ch) && 
        (( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) )
    ||   (IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE) && !strcmp(material_lookup(obj->material),"silver")))
      )
  {
    // Thanks to Morgenes for the bug fix here!
    act( "Ты обжигаешься и бросаешь $i4 на пол.", ch, obj, NULL, TO_CHAR );
    act( "$c1 обжигается и бросает $i4 на пол.",  ch, obj, NULL, TO_ROOM );
    obj_from_char( obj );
    obj_to_room( obj, ch->in_room );
    return;
  }

  for (i = 0; i < 4; i++) ch->armor[i] -= apply_ac( obj, iWear,i );
  obj->wear_loc = iWear;

  if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
      if ( paf->location != APPLY_SPELL_AFFECT ) affect_modify( ch, paf, TRUE );
  for ( paf = obj->affected; paf != NULL; paf = paf->next )
      if ( paf->location == APPLY_SPELL_AFFECT ) affect_to_char ( ch, paf );
      else affect_modify( ch, paf, TRUE );

  if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0
      && ch->in_room != NULL ) ++ch->in_room->light;

  WILLSAVE(ch);
}

// Unequip a char with an obj.
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
  AFFECT_DATA *paf = NULL;
  AFFECT_DATA *lpaf = NULL;
  AFFECT_DATA *lpaf_next = NULL;
  int i;

  if ( obj->wear_loc == WEAR_NONE )
  {
    bug( "Unequip_char: already unequipped.", 0 );
    return;
  }

  for (i = 0; i < 4; i++)
      ch->armor[i]    += apply_ac( obj, obj->wear_loc,i );
  obj->wear_loc        = -1;

  if (!obj->enchanted)
  {
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
      if ( paf->location == APPLY_SPELL_AFFECT )
      {
        for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
        {
          lpaf_next = lpaf->next;
          if ((lpaf->type == paf->type) 
              && (lpaf->level == paf->level) 
              && (lpaf->location == APPLY_SPELL_AFFECT)
              )
          {
            affect_remove( ch, lpaf );
            lpaf_next = NULL;
          }
        }
      }
      else
      {
        affect_modify( ch, paf, FALSE );
        affect_check(ch,paf->where,paf->bitvector);
      }
  } 
  for ( paf = obj->affected; paf != NULL; paf = paf->next )
    if ( paf->location == APPLY_SPELL_AFFECT )
    {
//      bug ( "Norm-Apply: %d", 0 );
      for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
      {
        lpaf_next = lpaf->next;
        if ((lpaf->type == paf->type) 
          && (lpaf->level == paf->level) 
          && (lpaf->location == APPLY_SPELL_AFFECT)
          )
        {
//          bug ( "location = %d", lpaf->location );
//          bug ( "type = %d", lpaf->type );
          affect_remove( ch, lpaf );
          lpaf_next = NULL;
        }
      }
    }
    else
    {
      affect_modify( ch, paf, FALSE );
      affect_check(ch,paf->where,paf->bitvector); 
    }

  if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0
  &&   ch->in_room != NULL && ch->in_room->light > 0 ) --ch->in_room->light;

  WILLSAVE(ch);
}

// Count occurrences of an obj in a list.
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
  OBJ_DATA *obj;
  int nMatch;

  nMatch = 0;
  for ( obj = list; obj != NULL; obj = obj->next_content )
  {
    if ( obj->pIndexData == pObjIndex ) nMatch++;
  }
  return nMatch;
}

// Move an obj out of a room.
void obj_from_room( OBJ_DATA *obj )
{
  ROOM_INDEX_DATA *in_room;
  CHAR_DATA *ch;

  if ( ( in_room = obj->in_room ) == NULL )
  {
    bug( "obj_from_room: NULL.", 0 );
    return;
  }

  for (ch = in_room->people; ch != NULL; ch = ch->next_in_room)
   if (ch->on == obj) ch->on = NULL;

  if ( obj == in_room->contents )
  {
    in_room->contents = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for ( prev = in_room->contents; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }
    if ( prev == NULL )
    {
      bug( "Obj_from_room: obj not found.", 0 );
      return;
    }
  }
  obj->in_room      = NULL;
  obj->next_content = NULL;
}

// Move an obj into a room.
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
  if (pRoomIndex==NULL)
  {
   bug("Obj to NULL room",0);
   pRoomIndex = get_room_index(ROOM_VNUM_ALTAR);
  }
  obj->next_content           = pRoomIndex->contents;
  pRoomIndex->contents        = obj;
  obj->in_room                = pRoomIndex;
  obj->carried_by             = NULL;
  obj->in_obj                 = NULL;
}

// Move an object into an object.
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
  obj->next_content           = obj_to->contains;
  obj_to->contains            = obj;
  obj->in_obj                 = obj_to;
  obj->in_room                = NULL;
  obj->carried_by             = NULL;
  if (obj_to->pIndexData->vnum == OBJ_VNUM_PIT) obj->cost = 0; 

  for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
  {
    if ( obj_to->carried_by != NULL )
    {
      obj_to->carried_by->carry_number += get_obj_number( obj );
      obj_to->carried_by->carry_weight += get_obj_weight( obj )
          * (int)WEIGHT_MULT(obj_to) / 100;
    }
  }
}

// Move an object out of an object.
void obj_from_obj( OBJ_DATA *obj )
{
  OBJ_DATA *obj_from;

  if ( ( obj_from = obj->in_obj ) == NULL )
  {
    bug( "Obj_from_obj: null obj_from.", 0 );
    return;
  }

  if ( obj == obj_from->contains )
  {
    obj_from->contains = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for ( prev = obj_from->contains; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }
    if ( prev == NULL )
    {
      bug( "Obj_from_obj: obj not found.", 0 );
      return;
    }
  }

  obj->next_content = NULL;
  obj->in_obj       = NULL;

  for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
  {
    if ( obj_from->carried_by != NULL )
    {
      obj_from->carried_by->carry_number -= get_obj_number( obj );
      obj_from->carried_by->carry_weight -= get_obj_weight( obj ) 
              * (int)WEIGHT_MULT(obj_from) / 100;
    }
  }
}

// Extract an obj from the world.
void extract_obj( OBJ_DATA *obj )
{
  OBJ_DATA *obj_content;
  OBJ_DATA *obj_next;

  if (obj==NULL)
  {
    bug("BUG - extract NULL object", 0);
    return;
  }

  if ( obj->in_room != NULL ) obj_from_room( obj );
  else if ( obj->carried_by != NULL ) obj_from_char( obj );
  else if ( obj->in_obj != NULL ) obj_from_obj( obj );

  if (obj->morph_name!=NULL)
  {
    char_from_room( obj->morph_name );
    char_to_room( obj->morph_name, get_room_index(ROOM_VNUM_ALTAR) );
    obj->morph_name->hit=1;
    obj->morph_name->mana=1;
    obj->morph_name->move=1;
    if (IS_SET(obj->morph_name->act, PLR_WANTED)
      ||  obj->morph_name->criminal>50)
    {
      raw_kill(obj->morph_name);
      stc("Ты {RУБИТ!{x\n\r",obj->morph_name);
    }            
    else stc("Ты принимаешь свою первоначальную форму. Tебе плохо...\n\r",obj->morph_name);
    obj->morph_name->morph_obj->is_morphed=FALSE;
    obj->morph_name->morph_obj = NULL;
  }

  for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
  {
    obj_next = obj_content->next_content;
    extract_obj( obj_content );
  }

  if ( object_list == obj ) object_list = obj->next;
  else
  {
    OBJ_DATA *prev;

    for ( prev = object_list; prev != NULL; prev = prev->next )
    {
      if ( prev->next == obj )
      {
        prev->next = obj->next;
        break;
      }
    }
    if ( prev == NULL )
    {
      bug( "Extract_obj: obj %u not found.", obj->pIndexData->vnum );
      return;
    }
  }
  --obj->pIndexData->count;
  free_obj(obj);
}

// Extract a char from the world.
void extract_char( CHAR_DATA *ch, bool fPull )
{
  CHAR_DATA *wch;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  DESCRIPTOR_DATA *plr;
  int i;

  if (!ch)
  {
    bug( "Extract_char: NULL.", 0 );
    return;
  }

  if (IS_NPC(ch))
  {
   //чистим списки жертв
   for(plr=descriptor_list;plr;plr=plr->next)
    if (plr->character && !IS_NPC(plr->character))
    {
      for(i=0;i<MAX_VICT;i++)
      if (plr->character->pcdata->victims[i].victim==ch)
      {
        plr->character->pcdata->victims[i].victim = NULL;
        plr->character->pcdata->victims[i].dampool = 0;
      }
    }
  }
  nuke_pets(ch);

  if ( fPull ) die_follower( ch );
  
  stop_fighting( ch, TRUE );

  for ( obj = ch->carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    extract_obj( obj );
  }
  
  if (ch->in_room)
  {
    if (ch->on)
    {
      CHAR_DATA *victim;
      OBJ_DATA *obj_on;

      obj_on=ch->on;

      for (victim=ch->in_room->people;victim;victim=victim->next_in_room)
        if (!IS_NPC(victim) && victim!=ch && victim->on==obj_on) obj_on=NULL;

      if(obj_on && CAN_WEAR(obj_on,ITEM_TAKE)) extract_obj(obj_on);
    }
    char_from_room( ch );
  }

  // Death room is set in the clan table now
  if ( !fPull )
  {
    if (IS_SET(ch->act,PLR_ARMY)) char_to_room(ch,get_room_index(ROOM_VNUM_ARMY_BED));
    else if (!ch->clan) char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
    else char_to_room(ch,get_room_index(ch->clan->clandeath));
    return;
  }

  if (IS_NPC(ch)) --ch->pIndexData->count;

  for (wch=char_list;wch;wch=wch->next)
  {
    if (wch->reply==ch) wch->reply=NULL;
    if (ch->mprog_target==wch) wch->mprog_target=NULL;
  }
  if (ch==char_list) char_list = ch->next;
  else
  {
    CHAR_DATA *prev;

    for ( prev = char_list; prev; prev = prev->next )
    {
      if ( prev->next == ch )
      {
         prev->next = ch->next;
         break;
      }
    }
  }
  if (ch->desc) ch->desc->character = NULL; //BUGGY ? Sab
  free_char( ch );
}

// Find a char in the room.
CHAR_DATA *get_char_room( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *rch;
  int number;
  int count;

  number = number_argument( (char*)argument, arg );
  count  = 0;
  if ( !str_cmp( arg, "self" ) ) return ch;
  for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
  {
    if (!can_see(ch,rch,CHECK_LVL) || !is_name(arg,rch->name)) continue;
    if (IS_AFFECTED(rch,AFF_MIST) && !IS_IMMORTAL(ch))
    {
      int chance=GUILD(rch,VAMPIRE_GUILD)?35:10;
      chance+=rch->level-ch->level;
      if (number_range(1,100)<chance) return NULL;
    }
    if ( ++count == number ) return rch;
  }
  return NULL;
}

// Find a char in the world
CHAR_DATA *get_char_world( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *wch;
  int number;
  int count;

  if ((wch = get_char_room(ch,argument)) != NULL) return wch;
  number = number_argument( (char *)argument, arg );
  count  = 0;

  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( wch->in_room == NULL || !is_name(arg,wch->name)
    || !can_see(ch,wch,CHECK_LVL)) continue;
    if (ch->desc && ch->desc->connected!=CON_PLAYING) continue;

    if (++count==number) return wch;
  }
  return NULL;
}

CHAR_DATA *get_pchar_world( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *wch;
  int number;
  int count;

  number = number_argument( (char *)argument, arg );
  count  = 0;
  if ( !str_cmp( arg, "self" ) ) return ch;
  for ( wch = char_list; wch != NULL ; wch = wch->next )
  {
    if ( wch->in_room == NULL || !is_name( arg, wch->name ) ||
      !can_see( ch, wch,CHECK_LVL ) || IS_NPC(wch)) continue;
    if (ch->desc && ch->desc->connected!=CON_PLAYING) continue;
    if ( ++count == number ) return wch;
  }
  return NULL;
}

CHAR_DATA *get_online_char( CHAR_DATA *ch, const char *argument )
{
  DESCRIPTOR_DATA *d;

  if (!str_cmp(argument,"self")) return ch;
  for ( d=descriptor_list;d!=NULL;d=d->next)
  {
    if ( !d->character || d->connected !=CON_PLAYING || !d->character->in_room
      || !can_see( ch, d->character,CHECK_LVL)
      || !is_name( argument, d->character->name ) ) continue;
      return d->character;
  }
  return NULL;
}

/* Find some object with a given index data.
 * Used by area-reset 'P' command. */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
  OBJ_DATA *obj;

  for ( obj = object_list; obj != NULL; obj = obj->next )
  {
      if ( obj->pIndexData == pObjIndex ) return obj;
  }
  return NULL;
}

// Find an obj in a list.
OBJ_DATA *get_obj_list( CHAR_DATA *ch, const char *argument, OBJ_DATA *list )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument( (char *)argument, arg );
  count  = 0;
  for ( obj = list; obj != NULL; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
    {
      if ( ++count == number ) return obj;
    }
  }
  return NULL;
}

/* Find an obj in player's inventory. */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, const char *argument, CHAR_DATA *viewer )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;
  number = number_argument( (char *)argument, arg );
  count  = 0;
  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
  {
    if ( obj->wear_loc == WEAR_NONE
    &&   (can_see_obj( viewer, obj ) )
    &&   is_name( arg, obj->name ) )
    {
      if ( ++count == number ) return obj;
    }
  }
  return NULL;
}

/* Find an obj in player's equipment. */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument( (char *)argument, arg );
  count  = 0;
  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
  {
      if ( obj->wear_loc != WEAR_NONE
      &&   can_see_obj( ch, obj )
      &&   is_name( arg, obj->name ) )
      {
          if ( ++count == number )
              return obj;
      }
  }

  return NULL;
}

/* Find an obj in the room or in inventory. */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, const char *argument )
{
 OBJ_DATA *obj;

 if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL ) return obj;
 if ( ( obj = get_obj_wear( ch, argument ) ) != NULL ) return obj;
 if ( (obj=get_obj_list(ch,argument,ch->in_room->contents)) !=NULL) return obj;

 return NULL;
}

/* Find an obj in the room or in inventory. */
OBJ_DATA *get_obj_victim( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument )
{
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int number, count;

  number = number_argument( (char *)argument, arg );
  count  = 0;
  for ( obj = victim->carrying; obj != NULL; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) &&  is_name( arg, obj->name ) )
    {
      if ( ++count == number ) return obj;
    }  
  }
  return NULL;
}

/* Find an obj in the world. */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;

  if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
      return obj;

  number = number_argument( (char *)argument, arg );
  count  = 0;
  for ( obj = object_list; obj != NULL; obj = obj->next )
  {
      if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
      {
          if ( ++count == number )
              return obj;
      }
  }

  return NULL;
}

/* deduct cost from a character */
void deduct_cost(CHAR_DATA *ch, int64 cost)
{
  int64 silver = 0, gold = 0;

  silver = UMIN(ch->silver,cost); 

  if (silver < cost)
  {
      gold = ((cost - silver + 99) / 100);
      silver = cost - 100 * gold;
  }

  ch->gold -= gold;
  ch->silver -= silver;

  if (ch->gold < 0)
  {
      bug("deduct costs: gold %u < 0",ch->gold);
      ch->gold = 0;
  }
  if (ch->silver < 0)
  {
      bug("deduct costs: silver %u < 0",ch->silver);
      ch->silver = 0;
  }
}   

/* Create a 'money' obj. */
OBJ_DATA *create_money( int64 gold, int64 silver )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;

  if ( gold < 0 || silver < 0 || (gold == 0 && silver == 0) )
  {
      bug( "Create_money: zero or negative money.",UMIN(gold,silver));
      gold = UMAX(1,gold);
      silver = UMAX(1,silver);
  }

  if (gold == 0 && silver == 1)
  {
      obj = create_object( get_obj_index( OBJ_VNUM_SILVER_ONE ), 0 );
  }
  else if (gold == 1 && silver == 0)
  {
      obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE), 0 );
  }
  else if (silver == 0)
  {
     if((obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 )))
     {
      do_printf( buf, obj->short_descr, gold );
      free_string( obj->short_descr );
      obj->short_descr        = str_dup( buf );
      obj->value[1]           = gold;
      obj->cost               = gold;
      obj->weight             = (int)(gold/5);
     }
  }
  else if (gold == 0)
  {
    if((obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 )))
     { 
      do_printf( buf, obj->short_descr, silver );
      free_string( obj->short_descr );
      obj->short_descr        = str_dup( buf );
      obj->value[0]           = silver;
      obj->cost               = silver;
      obj->weight             = (int)(silver/20);
     } 
  }
  else
  {
    if((obj = create_object( get_obj_index( OBJ_VNUM_COINS ), 0 )))
     {
      do_printf( buf, obj->short_descr, silver, gold );
      free_string( obj->short_descr );
      obj->short_descr        = str_dup( buf );
      obj->value[0]           = silver;
      obj->value[1]           = gold;
      obj->cost               = (100 * gold + silver);
      obj->weight             = (int)(gold / 5 + silver / 20);
     } 
  }

  return obj;
}


/* Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here. */
int get_obj_number( OBJ_DATA *obj )
{
  int number;

  if (obj->item_type == ITEM_CONTAINER 
   || obj->item_type == ITEM_MONEY
   || obj->item_type == ITEM_GEM 
   || obj->item_type == ITEM_JEWELRY
   || IS_EAR(obj))
      number = 0;
  else
      number = 1;

  for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
      if (!IS_EAR(obj)) number += get_obj_number( obj );

  return number;
}


/* Return weight of an object, including weight of contents. */
int get_obj_weight( OBJ_DATA *obj )
{
  int weight;
  OBJ_DATA *tobj;

  weight = obj->weight;
  for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
      weight += get_obj_weight( tobj ) * (int)WEIGHT_MULT(obj) / 100;

  return weight;
}

int get_true_weight(OBJ_DATA *obj)
{
  int weight;

  weight = obj->weight;
  for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
      weight += get_obj_weight( obj );

  return weight;
}

/* True if room is dark. */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
  if (!pRoomIndex) return TRUE;
  if ( pRoomIndex->light > 0 ) return FALSE;

  if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK) ) return TRUE;

  if ( pRoomIndex->sector_type == SECT_INSIDE
  ||   pRoomIndex->sector_type == SECT_CITY )
      return FALSE;

  if ( weather_info.sunlight == SUN_SET
  ||   weather_info.sunlight == SUN_DARK )
      return TRUE;

  return FALSE;
}

bool is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
  if (room->owner == NULL || room->owner[0] == '\0') return FALSE;

  return is_name(ch->name,room->owner);
}

/* True if room is private. */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
  CHAR_DATA *rch;
  int count;


  if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0') return TRUE;

  count = 0;
  for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
      count++;

  if ( IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)  && count >= 2 )
      return TRUE;

  if ( IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1 )
      return TRUE;
  
  return FALSE;
}


// get char's detect level (suitable for detect invis/hidden/good/evil/poison (?) 
int get_detect_level   (CHAR_DATA *ch, int cast_type, int sn)
{
 int detect_level;
 AFFECT_DATA *af;
 
 detect_level = 0;
 
 for ( af = ch->affected;af != NULL;af = af->next ) 
   if ( af->type == sn ) detect_level = af->level;

 if (detect_level<=1 && IS_NPC(ch) && IS_AFFECTED(ch,cast_type))
    detect_level=ch->level;

 if (IS_SET(race_table[ch->race].aff,cast_type)) 
       detect_level =
       UMAX(detect_level, ch->level + category_bonus(ch,skill_table[sn].group));

 return detect_level;
} 

// get victim's hide/sneak/invis level
int get_undetect_level (CHAR_DATA *victim, int cast_type, int sn)
{  
 int  cast_level;     
 AFFECT_DATA *af;
 
 cast_level = 0;
 
 for ( af = victim->affected;af != NULL;af = af->next ) 
  if ( af->type == sn) cast_level = af->level;
 
 if (cast_level<=1 && IS_NPC(victim) && IS_AFFECTED(victim,cast_type))
    cast_level=victim->level;

 if (
     ( IS_SET(race_table[victim->race].spec,SPEC_INVIS) && cast_type == AFF_INVISIBLE) ||
     ( IS_SET(race_table[victim->race].spec,SPEC_SNEAK) && cast_type == AFF_SNEAK) ||
     ( IS_SET(race_table[victim->race].spec,SPEC_HIDE) && cast_type == AFF_HIDE) 
    )    
     cast_level = 
     UMAX(cast_level, victim->level + category_bonus(victim,skill_table[sn].group));
 
 return cast_level;
}



/* visibility on a room -- for entering and exits */
bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
  if (IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY)
  &&  !IS_IMMORTAL(ch))
      return FALSE;

  if (IS_SET(pRoomIndex->room_flags, ROOM_ELDER)
  &&  !IS_ELDER(ch) )
      return FALSE;

  if (IS_SET(pRoomIndex->room_flags, ROOM_HEROES_ONLY)
  &&  ch->level<LEVEL_HERO)
      return FALSE;

  if (IS_SET(pRoomIndex->room_flags,ROOM_NEWBIES_ONLY)
  &&  ch->level > 5 && !IS_IMMORTAL(ch))
      return FALSE;

  if (IS_SET(pRoomIndex->room_flags,ROOM_DWARVES_GUILD)
   && !GUILD(ch, DWARVES_GUILD) && !IS_IMMORTAL(ch)) return FALSE;

  if (IS_SET(pRoomIndex->area->area_flags,AREA_WIZLOCK) && !IS_IMMORTAL(ch))
      return FALSE;

  if (!IS_IMMORTAL(ch) && ch->level < raffect_level(pRoomIndex,RAFF_HIDE))
      return FALSE;
  return TRUE;
}

bool can_see_sneak (CHAR_DATA *ch, CHAR_DATA *victim, int check_level)
{
  int det_lev, cast_lev;
  
//  if(IS_SET(victim->in_room->room_flags,ROOM_ALL_VIS)) return TRUE;
  
  if (  IS_AFFECTED(victim, AFF_SNEAK) && victim->fighting == NULL )
   {
    if (!IS_AFFECTED(ch, AFF_DETECT_HIDDEN)) return FALSE;
    else if (check_level == CHECK_LVL)
    {
     det_lev  = get_detect_level (ch,AFF_DETECT_HIDDEN, skill_lookup("detect hidden"));
     cast_lev = get_undetect_level(victim, AFF_SNEAK, gsn_sneak);
      
      if ( number_percent() <
      ((cast_lev - det_lev) * 100 / MAX_PK_RANGE) ) return FALSE;
    }
   }  
  return TRUE; 
}

// True if char can see victim.
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim, int check_level )
{
  int det_lev, cast_lev,chance,perc;

//  if (IS_NPC(victim) && victim->pIndexData->vnum > 23079 && victim->pIndexData->vnum < 23098)
  if (IS_STATUE(victim))
  {
    if (IS_NPC(ch)) return FALSE;
    if (IS_IMMORTAL(ch)) return TRUE;
    if (IS_DEVOTED_ANY(ch) && ch->pcdata->dn+23080==victim->pIndexData->vnum) return TRUE;
    else return FALSE;
  }

/* sho za marazmatichesky uchastok coda? Sab.
// statuyu vidyat tol'ko immi i devotnutie
// ifov stol'ko dlya togo, chtobi izbejat' crusha pri ch==NULL, etc.
  if (IS_NPC(victim))
    if ( ((victim->pIndexData->vnum)>23079) && ((victim->pIndexData->vnum)<23098))

      if (!IS_NPC(ch)) 
      {
        if (IS_IMMORTAL(ch)) return TRUE;
        if (IS_DEVOTED_ANY(ch))
        {
// vnumi statuy idut po poryadku, deities toje. dn Astellara - 0, vnum ego statui - 23080
          if ((ch->pcdata->dn + 23080) == (victim->pIndexData->vnum)) return TRUE;
          else return FALSE;
        }
        else return FALSE;
      }
      else return FALSE;
*/

  if ( (ch == victim) && !IS_AFFECTED(ch, AFF_BLIND) ) return TRUE;
  
  if (!IS_NPC(victim))
  {
// if ((get_trust(victim) > get_trust(ch)) && is_exact_name(ch->name,victim->pcdata->ignorelist)) return FALSE;
   if (victim->invis_level>get_trust(ch) && !is_exact_name(ch->name,victim->pcdata->vislist)) return FALSE;
   if (victim->incog_level>get_trust(ch) && ch->in_room!=victim->in_room
       && !is_exact_name(ch->name,victim->pcdata->vislist)) return FALSE;
  }
  
  if ( (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) 
      ||(IS_NPC(ch) && IS_IMMORTAL(ch))) return TRUE;

  if (!IS_NPC(victim) && IS_CFG(victim,CFG_ZRITEL) && victim->in_room
   && IS_SET(victim->in_room->room_flags,ROOM_ARENA)) return FALSE;

//  if(victim->in_room && IS_SET(victim->in_room->room_flags,ROOM_ALL_VIS)) return TRUE;

  if ( IS_AFFECTED(ch, AFF_BLIND) 
       || (ch->in_room && IS_SET(ch->in_room->ra,RAFF_BLIND)
           && !IS_AFFECTED(ch, AFF_VISION) )) return FALSE;
  if (victim->in_room && IS_SET(victim->in_room->ra,RAFF_ALL_VIS)) return TRUE;

  if ( room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_INFRARED) &&
       !IS_AFFECTED(ch, AFF_DARK_VISION))  return FALSE;


  if ( IS_AFFECTED(victim, AFF_INVISIBLE))
  {       
    if (!IS_AFFECTED(ch, AFF_DETECT_INVIS)) return FALSE;
    else if (check_level==CHECK_LVL)
    {
      det_lev = get_detect_level(ch,AFF_DETECT_INVIS, skill_lookup("detect invis"));
      cast_lev = get_undetect_level(victim, AFF_INVISIBLE, gsn_invis);
  
      perc = category_bonus(ch,PERCEP);
      chance = 80  - (cast_lev - det_lev) * (7+ UMIN (3, perc));    
      chance = UMAX (perc,chance);
      chance += victim->size*5;

      if (number_percent() > chance) return FALSE;
    }
  }
   
  if (  IS_AFFECTED(victim, AFF_HIDE) && victim->fighting == NULL )
  {
    if (!IS_AFFECTED(ch, AFF_DETECT_HIDDEN)) return FALSE;
    else if (check_level==CHECK_LVL)
    {
      det_lev  = get_detect_level (ch,AFF_DETECT_HIDDEN, skill_lookup("detect hidden"));
      cast_lev = get_undetect_level(victim, AFF_HIDE, gsn_hide);
      if (!IS_NPC(ch) && ch->clan != NULL && IS_SET(ch->clan->flag, CLAN_THIEF) )
       cast_lev+=number_range(5,10);
      
      perc = category_bonus(ch,PERCEP);
      chance = 70  - (cast_lev - det_lev) * (6+ UMIN (4, perc));    
      chance = UMAX (perc,chance);
      chance += victim->size*5;

      if (number_percent() > chance) return FALSE;
  
    }
  }
  return TRUE;
}

// True if char can see obj.
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
  if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) ) return TRUE;
  if ( IS_SET(obj->extra_flags,ITEM_VIS_DEATH)) return FALSE;

  if (IS_AFFECTED(ch,AFF_BLIND) 
      || (ch->in_room && IS_SET(ch->in_room->ra,RAFF_BLIND)
          && !IS_AFFECTED(ch,AFF_VISION) ))
  {
    if (  obj->item_type!=ITEM_POTION 
        && (   !IS_OBJ_STAT(obj,ITEM_HUM) 
             || is_affected(ch,skill_lookup("deaf"))
           )
       ) return FALSE;
  }

  if (ch->in_room && IS_SET(ch->in_room->ra,RAFF_ALL_VIS) && !obj->carried_by) return TRUE;

  if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
        return TRUE;

  if ( IS_SET(obj->extra_flags, ITEM_INVIS) &&
      (!IS_AFFECTED(ch, AFF_DETECT_INVIS) ||
       (get_detect_level (ch,AFF_DETECT_INVIS, skill_lookup("detect invis")) + number_range(0,ch->level/8+1) 
       < obj->level))
     )
     return FALSE;

  if ( IS_OBJ_STAT(obj,ITEM_GLOW)) return TRUE;

  if ( room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_DARK_VISION) )
        return FALSE;

  return TRUE;
}

/* True if char can drop obj. */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
  if ( !IS_SET(obj->extra_flags, ITEM_NODROP) ) return TRUE;

  if ( !IS_NPC(ch) && IS_IMMORTAL(ch) ) return TRUE;
  return FALSE;
}

/* Return ascii name of an affect location. */
char *affect_loc_name( int location )
{
  switch ( location )
  {
  case APPLY_NONE:            return "none";
  case APPLY_STR:             return "strength";
  case APPLY_DEX:             return "dexterity";
  case APPLY_INT:             return "intelligence";
  case APPLY_WIS:             return "wisdom";
  case APPLY_CON:             return "constitution";
  case APPLY_SEX:             return "sex";
  case APPLY_CLASS:           return "class";
  case APPLY_LEVEL:           return "level";
  case APPLY_AGE:             return "age";
  case APPLY_MANA:            return "mana";
  case APPLY_HIT:             return "hp";
  case APPLY_MOVE:            return "moves";
  case APPLY_GOLD:            return "gold";
  case APPLY_EXP:             return "experience";
  case APPLY_AC:              return "armor class";
  case APPLY_HITROLL:         return "hit roll";
  case APPLY_DAMROLL:         return "damage roll";
  case APPLY_SAVES:           return "saves";
  case APPLY_SAVING_ROD:      return "save vs rod";
  case APPLY_SAVING_PETRI:    return "save vs petrification";
  case APPLY_SAVING_BREATH:   return "save vs breath";
  case APPLY_SAVING_SPELL:    return "save vs spell";
  case APPLY_SPELL_AFFECT:    return "spell affect";
  }

  bug( "Affect_location_name: unknown location %d.", location );
  return "(unknown)";
}

char *skill_flag_name( int64 vector )
{
  static char buf[512];

  buf[0] = '\0';

  if ( vector & C_NODUAL    ) strcat( buf, " C_NODUAL"  );
  if ( vector & S_CLAN    ) strcat( buf, " S_CLAN"  );
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *spec_bit_name( int64 vector )
{
  static char buf[512];

  buf[0] = '\0';

  if ( vector & SPEC_FLY          ) strcat( buf, " Can_fly"           );
  if ( vector & SPEC_SNEAK        ) strcat( buf, " Always_sneak"      );
  if ( vector & SPEC_MANAREGEN    ) strcat( buf, " Mana_regeneration" );
  if ( vector & SPEC_DKICK        ) strcat( buf, " Adv_Double_Kick"   );
  if ( vector & SPEC_CRUSH        ) strcat( buf, " Crush_skill"       );
  if ( vector & SPEC_TAIL         ) strcat( buf, " Tail_skill"        );
  if ( vector & SPEC_VAMPIRE      ) strcat( buf, " Vampire"           );
  if ( vector & SPEC_NOEAT        ) strcat( buf, " No_eat"            );
  if ( vector & SPEC_NODRINK      ) strcat( buf, " No_drink"          );
  if ( vector & SPEC_TWOHAND      ) strcat( buf, " Twohand_adv_using" );
  if ( vector & SPEC_REGENSP      ) strcat( buf, " Adv_regen_spell"   );
  if ( vector & SPEC_BLACKSMITH   ) strcat( buf, " Blacksmith_skill"  );
  if ( vector & SPEC_REGENERATION ) strcat( buf, " Adv_Regeneration"  );
  if ( vector & SPEC_ENERGY       ) strcat( buf, " Adv_Energy_Damage" );
  if ( vector & SPEC_DODGE        ) strcat( buf, " Adv_dodge"         );
  if ( vector & SPEC_INVIS        ) strcat( buf, " Invis_skill"       );
  if ( vector & SPEC_HIDE         ) strcat( buf, " Hide_skill"        );
  if ( vector & SPEC_PSY          ) strcat( buf, " Psionic"           );
  if ( vector & SPEC_PASSFLEE     ) strcat( buf, " PassFlee"          );
  if ( vector & SPEC_MIST         ) strcat( buf, " Mist"              );
  if ( vector & SPEC_HOWL         ) strcat( buf, " Howl"              );
  if ( vector & SPEC_TRAIN        ) strcat( buf, " Extra_train"       );
  if ( vector & SPEC_RESBASH      ) strcat( buf, " Resist_to_Bash"    );
  if ( vector & SPEC_UWATER       ) strcat( buf, " LikeFish"          );
  if ( vector & SPEC_NOLOSTEXP    ) strcat( buf, " NoLostExp"         );
  if ( vector & SPEC_WATERWALK    ) strcat( buf, " WaterWalk"         );
  if ( vector & SPEC_RDEATH       ) strcat( buf, " RDeath"            );
  if ( vector & SPEC_BACKSTAB     ) strcat( buf, " Adv_Backstab"      );
  if ( vector & SPEC_IGNOREALIGN  ) strcat( buf, " Ignore_Align"      );
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

// Return ascii name of an affect bit vector.
char *affect_bit_name( int64 vector )
{
  static char buf[512];

  buf[0] = '\0';
  if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
  if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
  if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
  if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
  if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
  if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
  if ( vector & AFF_DETECT_GOOD   ) strcat( buf, " detect_good"   );
  if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
  if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
  if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
  if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
  if ( vector & AFF_SWIM          ) strcat( buf, " swim"          );
  if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
  if ( vector & AFF_PROTECT_EVIL  ) strcat( buf, " prot_evil"     );
  if ( vector & AFF_PROTECT_GOOD  ) strcat( buf, " prot_good"     );
  if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
  if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
  if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
  if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
  if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
  if ( vector & AFF_HASTE         ) strcat( buf, " haste"         );
  if ( vector & AFF_CALM          ) strcat( buf, " calm"          );
  if ( vector & AFF_PLAGUE        ) strcat( buf, " plague"        );
  if ( vector & AFF_BERSERK       ) strcat( buf, " berserk"       );
  if ( vector & AFF_DARK_VISION   ) strcat( buf, " dark_vision"   );
  if ( vector & AFF_SLOW          ) strcat( buf, " slow"          );
  if ( vector & AFF_REGENERATION  ) strcat( buf, " regeneration"  );
  if ( vector & AFF_FIRESHIELD    ) strcat( buf, " fireshield"    );
  if ( vector & AFF_SHIELD        ) strcat( buf, " shield"        );
  if ( vector & AFF_NOSTALGIA     ) strcat( buf, " nostalgia"     );
  if ( vector & AFF_LIFE_STR      ) strcat( buf, " life_stream"   );
  //if ( vector & AFF_GASEOUS_FORM  ) strcat( buf, " gaseous_form"  );
  //if ( vector & AFF_FORCEFIELD    ) strcat( buf, " forcefield"    );
  //if ( vector & AFF_SAFE_PLACE    ) strcat( buf, " safty place"   );
  //if ( vector & AFF_REJUVINATE    ) strcat( buf, " rejuvinate"    );
  //if ( vector & AFF_CANT_FEAR     ) strcat( buf, " fear"          );
  //if ( vector & AFF_MADNESS       ) strcat( buf, " madness"       );
  //if ( vector & AFF_PEACE         ) strcat( buf, " peace"         );
  //if ( vector & AFF_VIOLENCE      ) strcat( buf, " violence"      );
  //if ( vector & AFF_VISION        ) strcat( buf, " vision"        );
  //if ( vector & AFF_MAGIC_BAR     ) strcat( buf, " magic barrier" );
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

// Return ascii name of extra flags vector.
char *extra_bit_name( int64 extra_flags )
{
  static char buf[512];

  buf[0] = '\0';
  if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
  if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
  if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
  if ( extra_flags & ITEM_LOCK         ) strcat( buf, " lock"         );
  if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
  if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
  if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
  if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
  if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
  if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
  if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
  if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
  if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
  if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
  if ( extra_flags & ITEM_NOPURGE      ) strcat( buf, " nopurge"      );
  if ( extra_flags & ITEM_VIS_DEATH    ) strcat( buf, " vis_death"    );
  if ( extra_flags & ITEM_ROT_DEATH    ) strcat( buf, " rot_death"    );
  if ( extra_flags & ITEM_NOLOCATE     ) strcat( buf, " no_locate"    );
  if ( extra_flags & ITEM_SELL_EXTRACT ) strcat( buf, " sell_extract" );
  if ( extra_flags & ITEM_BURN_PROOF   ) strcat( buf, " burn_proof"   );
  if ( extra_flags & ITEM_NOUNCURSE    ) strcat( buf, " no_uncurse"   );
  if ( extra_flags & ITEM_NO_IDENT     ) strcat( buf, " noident"      );
  if ( extra_flags & ITEM_CLANENCHANT  ) strcat( buf, " clanenchant"  );

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

// return ascii name of an act vector
char *act_bit_name( int64 act_flags )
{
  static char buf[MAX_STRING_LENGTH];

  buf[0] = '\0';

  if (IS_SET(act_flags,ACT_IS_NPC))
  { 
    strcat(buf," npc");
    if (act_flags & ACT_STAY_PLAIN    ) strcat(buf, " stay_plain");
    if (act_flags & ACT_ACCOUNTER     ) strcat(buf, " account");
    if (act_flags & ACT_REFERI        ) strcat(buf, " referi");
    if (act_flags & ACT_STOPDOOR      ) strcat(buf, " stopdoor");
    if (act_flags & ACT_SENTINEL      ) strcat(buf, " sentinel");
    if (act_flags & ACT_NOSTEAL       ) strcat(buf, " nosteal");
    if (act_flags & ACT_NOQUEST       ) strcat(buf, " noquest");
    if (act_flags & ACT_SCAVENGER     ) strcat(buf, " scavenger");
    if (act_flags & ACT_AGGRESSIVE    ) strcat(buf, " aggr");
    if (act_flags & ACT_STAY_AREA     ) strcat(buf, " stay_area");
    if (act_flags & ACT_WIMPY         ) strcat(buf, " wimpy");
    if (act_flags & ACT_PET           ) strcat(buf, " pet");
    if (act_flags & ACT_TRAIN         ) strcat(buf, " train");
    if (act_flags & ACT_PRACTICE      ) strcat(buf, " prac");
    if (act_flags & ACT_UNDEAD        ) strcat(buf, " undead");
    if (act_flags & ACT_CLERIC        ) strcat(buf, " cleric");
    if (act_flags & ACT_MAGE          ) strcat(buf, " mage");
    if (act_flags & ACT_THIEF         ) strcat(buf, " thief");
    if (act_flags & ACT_WARRIOR       ) strcat(buf, " warrior");
    if (act_flags & ACT_NOALIGN       ) strcat(buf, " no_align");
    if (act_flags & ACT_NOPURGE       ) strcat(buf, " no_purge");
    if (act_flags & ACT_IS_HEALER     ) strcat(buf, " healer");
    if (act_flags & ACT_IS_CHANGER    ) strcat(buf, " changer");
    if (act_flags & ACT_IS_KEEPER     ) strcat(buf, " keeper");
    if (act_flags & ACT_GAIN          ) strcat(buf, " skill_train");
    if (act_flags & ACT_EXTRACT_CORPSE) strcat(buf, " extract_crp");
    if (act_flags & ACT_UPDATE_ALWAYS ) strcat(buf, " upd_always");
    if (act_flags & ACT_CLANENCHANTER ) strcat(buf, " clanench");
    if (act_flags & ACT_ASSASIN_MASTER) strcat(buf, " assasin_m");
    if (act_flags & ACT_FORGER  )       strcat(buf, " forger");
  }
  else
  {
    strcat(buf," player");
    if (act_flags & PLR_AUTOASSIST  ) strcat(buf, " autoassist");
    if (act_flags & PLR_AUTOLOOT    ) strcat(buf, " autoloot");
    if (act_flags & PLR_AUTOSAC     ) strcat(buf, " autosac");
    if (act_flags & PLR_AUTOGOLD    ) strcat(buf, " autogold");
    if (act_flags & PLR_HOLYLIGHT   ) strcat(buf, " holy_light");
    if (act_flags & PLR_NOSUMMON    ) strcat(buf, " no_summon");
    if (act_flags & PLR_NOCANCEL    ) strcat(buf, " no_cancel");
    if (act_flags & PLR_NOFOLLOW    ) strcat(buf, " no_follow");
    if (act_flags & PLR_FREEZE      ) strcat(buf, " frozen");
    if (act_flags & PLR_COLOUR      ) strcat(buf, " colour");
    if (act_flags & PLR_WANTED      ) strcat(buf, " wanted");
    if (act_flags & PLR_RAPER       ) strcat(buf, " raper");
    if (act_flags & PLR_ARMY        ) strcat(buf, " army");
    if (act_flags & PLR_TIPSY       ) strcat(buf, " tipsy"); // tipsy by Dinger
    if (act_flags & PLR_SIFILIS     ) strcat(buf, " sifilis");
    if (act_flags & PLR_DISAVOWED   ) strcat(buf, " disavowed");
    if (act_flags & PLR_HIGHPRIEST  ) strcat(buf, " priest");
    if (act_flags & PLR_HIGHPRIEST  ) strcat(buf, " priest");

  }
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *comm_bit_name(int64 comm_flags)
{
  static char buf[512];

  buf[0] = '\0';

  if (comm_flags & COMM_QUIET        ) strcat(buf, " quiet");
  if (comm_flags & COMM_WILLSAVE     ) strcat(buf, " willsave");
  if (comm_flags & COMM_DEAF         ) strcat(buf, " deaf");
  if (comm_flags & COMM_BRIEF        ) strcat(buf, " brief");
  if (comm_flags & COMM_PROMPT       ) strcat(buf, " prompt");
  if (comm_flags & COMM_NOEMOTE      ) strcat(buf, " no_emote");
  if (comm_flags & COMM_NOTELL       ) strcat(buf, " no_tell");
  if (comm_flags & COMM_RUSSIAN      ) strcat(buf, " russian");
  if (comm_flags & COMM_NOCHANNELS   ) strcat(buf, " no_channels");
  if (comm_flags & COMM_TELNET_GA    ) strcat(buf, " no_telnet_ga");
  if (comm_flags & COMM_AFK          ) strcat(buf, " afk");
  
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *imm_bit_name(int64 imm_flags)
{
  static char buf[512];

  buf[0] = '\0';

  if (imm_flags & IMM_ALL       ) strcat(buf, " all");
  if (imm_flags & IMM_SUMMON    ) strcat(buf, " summon");
  if (imm_flags & IMM_CHARM     ) strcat(buf, " charm");
  if (imm_flags & IMM_MAGIC     ) strcat(buf, " magic");
  if (imm_flags & IMM_WEAPON    ) strcat(buf, " weapon");
  if (imm_flags & IMM_BASH      ) strcat(buf, " blunt");
  if (imm_flags & IMM_PIERCE    ) strcat(buf, " piercing");
  if (imm_flags & IMM_SLASH     ) strcat(buf, " slashing");
  if (imm_flags & IMM_FIRE      ) strcat(buf, " fire");
  if (imm_flags & IMM_COLD      ) strcat(buf, " cold");
  if (imm_flags & IMM_LIGHTNING ) strcat(buf, " lightning");
  if (imm_flags & IMM_ACID      ) strcat(buf, " acid");
  if (imm_flags & IMM_POISON    ) strcat(buf, " poison");
  if (imm_flags & IMM_NEGATIVE  ) strcat(buf, " negative");
  if (imm_flags & IMM_HOLY      ) strcat(buf, " holy");
  if (imm_flags & IMM_ENERGY    ) strcat(buf, " energy");
  if (imm_flags & IMM_MENTAL    ) strcat(buf, " mental");
  if (imm_flags & IMM_DISEASE   ) strcat(buf, " disease");
  if (imm_flags & IMM_DROWNING  ) strcat(buf, " drowning");
  if (imm_flags & IMM_LIGHT     ) strcat(buf, " light");
  if (imm_flags & VULN_IRON     ) strcat(buf, " iron");
  if (imm_flags & VULN_WOOD     ) strcat(buf, " wood");
  if (imm_flags & VULN_SILVER   ) strcat(buf, " silver");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *wear_bit_name(int64 wear_flags)
{
  static char buf[512];

  buf [0] = '\0';
  if (wear_flags & ITEM_TAKE        ) strcat(buf, " take");
  if (wear_flags & ITEM_WEAR_FINGER ) strcat(buf, " finger");
  if (wear_flags & ITEM_WEAR_NECK   ) strcat(buf, " neck");
  if (wear_flags & ITEM_WEAR_BODY   ) strcat(buf, " torso");
  if (wear_flags & ITEM_WEAR_HEAD   ) strcat(buf, " head");
  if (wear_flags & ITEM_WEAR_LEGS   ) strcat(buf, " legs");
  if (wear_flags & ITEM_WEAR_FEET   ) strcat(buf, " feet");
  if (wear_flags & ITEM_WEAR_HANDS  ) strcat(buf, " hands");
  if (wear_flags & ITEM_WEAR_ARMS   ) strcat(buf, " arms");
  if (wear_flags & ITEM_WEAR_SHIELD ) strcat(buf, " shield");
  if (wear_flags & ITEM_WEAR_ABOUT  ) strcat(buf, " body");
  if (wear_flags & ITEM_WEAR_WAIST  ) strcat(buf, " waist");
  if (wear_flags & ITEM_WEAR_WRIST  ) strcat(buf, " wrist");
  if (wear_flags & ITEM_WIELD       ) strcat(buf, " wield");
  if (wear_flags & ITEM_HOLD        ) strcat(buf, " hold");
  if (wear_flags & ITEM_NO_SAC      ) strcat(buf, " nosac");
  if (wear_flags & ITEM_WEAR_FLOAT  ) strcat(buf, " float");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *form_bit_name(int64 form_flags)
{
  static char buf[512];

  buf[0] = '\0';
  if (form_flags & FORM_POISON        ) strcat(buf, " poison");
  else if (form_flags & FORM_EDIBLE   ) strcat(buf, " edible");
  if (form_flags & FORM_MAGICAL       ) strcat(buf, " magical");
  if (form_flags & FORM_INSTANT_DECAY ) strcat(buf, " instant_rot");
  if (form_flags & FORM_OTHER         ) strcat(buf, " other");
  if (form_flags & FORM_ANIMAL        ) strcat(buf, " animal");
  if (form_flags & FORM_SENTIENT      ) strcat(buf, " sentient");
  if (form_flags & FORM_UNDEAD        ) strcat(buf, " undead");
  if (form_flags & FORM_CONSTRUCT     ) strcat(buf, " construct");
  if (form_flags & FORM_MIST          ) strcat(buf, " mist");
  if (form_flags & FORM_INTANGIBLE    ) strcat(buf, " intangible");
  if (form_flags & FORM_BIPED         ) strcat(buf, " biped");
  if (form_flags & FORM_CENTAUR       ) strcat(buf, " centaur");
  if (form_flags & FORM_INSECT        ) strcat(buf, " insect");
  if (form_flags & FORM_SPIDER        ) strcat(buf, " spider");
  if (form_flags & FORM_CRUSTACEAN    ) strcat(buf, " crustacean");
  if (form_flags & FORM_WORM          ) strcat(buf, " worm");
  if (form_flags & FORM_BLOB          ) strcat(buf, " blob");
  if (form_flags & FORM_MAMMAL        ) strcat(buf, " mammal");
  if (form_flags & FORM_BIRD          ) strcat(buf, " bird");
  if (form_flags & FORM_REPTILE       ) strcat(buf, " reptile");
  if (form_flags & FORM_SNAKE         ) strcat(buf, " snake");
  if (form_flags & FORM_DRAGON        ) strcat(buf, " dragon");
  if (form_flags & FORM_AMPHIBIAN     ) strcat(buf, " amphibian");
  if (form_flags & FORM_FISH          ) strcat(buf, " fish");
  if (form_flags & FORM_COLD_BLOOD    ) strcat(buf, " cold_blooded");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *part_bit_name(int64 part_flags)
{
  static char buf[512];

  buf[0] = '\0';
  if (part_flags & PART_HEAD        ) strcat(buf, " head");
  if (part_flags & PART_ARMS        ) strcat(buf, " arms");
  if (part_flags & PART_LEGS        ) strcat(buf, " legs");
  if (part_flags & PART_HEART       ) strcat(buf, " heart");
  if (part_flags & PART_BRAINS      ) strcat(buf, " brains");
  if (part_flags & PART_GUTS        ) strcat(buf, " guts");
  if (part_flags & PART_HANDS       ) strcat(buf, " hands");
  if (part_flags & PART_FEET        ) strcat(buf, " feet");
  if (part_flags & PART_FINGERS     ) strcat(buf, " fingers");
  if (part_flags & PART_EAR         ) strcat(buf, " ears");
  if (part_flags & PART_EYE         ) strcat(buf, " eyes");
  if (part_flags & PART_LONG_TONGUE ) strcat(buf, " long_tongue");
  if (part_flags & PART_EYESTALKS   ) strcat(buf, " eyestalks");
  if (part_flags & PART_TENTACLES   ) strcat(buf, " tentacles");
  if (part_flags & PART_FINS        ) strcat(buf, " fins");
  if (part_flags & PART_WINGS       ) strcat(buf, " wings");
  if (part_flags & PART_TAIL        ) strcat(buf, " tail");
  if (part_flags & PART_CLAWS       ) strcat(buf, " claws");
  if (part_flags & PART_FANGS       ) strcat(buf, " fangs");
  if (part_flags & PART_HORNS       ) strcat(buf, " horns");
  if (part_flags & PART_SCALES      ) strcat(buf, " scales");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *weapon_bit_name(int64 weapon_flags)
{
  static char buf[512];

  buf[0] = '\0';
  if (weapon_flags & WEAPON_FLAMING   ) strcat(buf, " flaming");
  if (weapon_flags & WEAPON_FROST     ) strcat(buf, " frost");
  if (weapon_flags & WEAPON_VAMPIRIC  ) strcat(buf, " vampiric");
  if (weapon_flags & WEAPON_SHARP     ) strcat(buf, " sharp");
  if (weapon_flags & WEAPON_VORPAL    ) strcat(buf, " vorpal");
  if (weapon_flags & WEAPON_TWO_HANDS ) strcat(buf, " two-handed");
  if (weapon_flags & WEAPON_SHOCKING  ) strcat(buf, " shocking");
  if (weapon_flags & WEAPON_POISON    ) strcat(buf, " poison");
  if (weapon_flags & WEAPON_VAMP_MANA ) strcat(buf, " vamp_mana");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *cont_bit_name( int64 cont_flags)
{
  static char buf[512];

  buf[0] = '\0';

  if (cont_flags & CONT_CLOSEABLE ) strcat(buf, " closable");
  if (cont_flags & CONT_PICKPROOF ) strcat(buf, " pickproof");
  if (cont_flags & CONT_CLOSED    ) strcat(buf, " closed");
  if (cont_flags & CONT_LOCKED    ) strcat(buf, " locked");

  return (buf[0] != '\0' ) ? buf+1 : "none";
}

char *off_bit_name(int64 off_flags)
{
  static char buf[512];

  buf[0] = '\0';

  if (off_flags & OFF_AREA_ATTACK ) strcat(buf, " area attack");
  if (off_flags & OFF_BACKSTAB    ) strcat(buf, " backstab");
  if (off_flags & OFF_BASH        ) strcat(buf, " bash");
  if (off_flags & OFF_BERSERK     ) strcat(buf, " berserk");
  if (off_flags & OFF_DISARM      ) strcat(buf, " disarm");
  if (off_flags & OFF_DODGE       ) strcat(buf, " dodge");
  if (off_flags & OFF_FADE        ) strcat(buf, " fade");
  if (off_flags & OFF_FAST        ) strcat(buf, " fast");
  if (off_flags & OFF_KICK        ) strcat(buf, " kick");
  if (off_flags & OFF_KICK_DIRT   ) strcat(buf, " kick_dirt");
  if (off_flags & OFF_PARRY       ) strcat(buf, " parry");
  if (off_flags & OFF_RESCUE      ) strcat(buf, " rescue");
  if (off_flags & OFF_TAIL        ) strcat(buf, " tail");
  if (off_flags & OFF_TRIP        ) strcat(buf, " trip");
  if (off_flags & OFF_CRUSH       ) strcat(buf, " crush");
  if (off_flags & ASSIST_ALL      ) strcat(buf, " assist_all");
  if (off_flags & ASSIST_ALIGN    ) strcat(buf, " assist_align");
  if (off_flags & ASSIST_RACE     ) strcat(buf, " assist_race");
  if (off_flags & ASSIST_PLAYERS  ) strcat(buf, " assist_players");
  if (off_flags & ASSIST_GUARD    ) strcat(buf, " assist_guard");
  if (off_flags & ASSIST_VNUM     ) strcat(buf, " assist_vnum");
  if (off_flags & OFF_ASSASIN_ACCOUNTER)strcat(buf," assasin_a");
  if (off_flags & OFF_DWARVES_ACCOUNTER)strcat(buf," dwarves_a");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

const char *get_char_desc ( CHAR_DATA *ch, char needcase )
{
  static char buf[1024];

  buf[0] = '\0';

  if (!ch) return "<{Rnone{x>";
  if (IS_NPC(ch))
  {   
    if (strchr(ch->short_descr,'|')) gram_newformat (buf, ch->short_descr, needcase);
    else return ch->short_descr;
  }
  else return ch->name;

  if ( buf[0]=='{') return (buf+2);
  return buf;
}

const char *get_mobindex_desc ( MOB_INDEX_DATA *pIndex, char needcase )
{
  static char buf[MAX_STRING_LENGTH];
  buf[0] = '\0';

  if (!pIndex) return "<{Rnone{x>";
  if ( strchr (pIndex->short_descr, '|' ) != NULL )
  {
    gram_newformat ( buf, pIndex->short_descr, needcase);       
    return buf;
  }
  else return pIndex->short_descr;
}

const char *get_obj_desc ( OBJ_DATA *obj, char needcase )
{
  static char buf[MAX_STRING_LENGTH];
  buf[0]='\0';

  if (!obj || !obj->short_descr) return "<{Rnone{x>";

  if ( strchr ( obj->short_descr, '|' ) != NULL )
        gram_newformat (buf, obj->short_descr, needcase);
  else  return obj->short_descr;
  return buf;
}

const char *get_rdeity ( const char *rdeity, char needcase )
{
  static char buf[MAX_STRING_LENGTH];
  buf[0]='\0';

  if ( rdeity == NULL ) return "<{Rnone{x>";

  if ( strchr ( rdeity, '|' ) != NULL )
        gram_newformat (buf, rdeity, needcase);
  else  return rdeity;
  return buf;
}

const char *get_objindex_desc ( OBJ_INDEX_DATA *pIndex, char needcase )
{
  static char buf[MAX_STRING_LENGTH];
  buf[0]='\0';

  if (!pIndex || !pIndex->short_descr) return "<{Rnone{x>";

  if ( strchr (pIndex->short_descr, '|' ) != NULL )
  {
    gram_newformat ( buf, pIndex->short_descr, needcase);       
    return buf;
  }
  else return pIndex->short_descr;
}

char num_char ( int n )
{  n = n - (n/100) * 100;
  if (n/10!=1 && n-(n/10)*10==1) return '1';
  if (n/10!=1 && n-(n/10)*10>=2 && n-(n/10)*10<=4) return '7';
  return '8';
}

char num_char64 ( int64 n )
{  n = n - (n/100) * 100;
  if (n/10!=1 && n-(n/10)*10==1) return '1';
  if (n/10!=1 && n-(n/10)*10>=2 && n-(n/10)*10<=4) return '7';
  return '8';
}

int64 atoi64(const char *arg)
{
  const char *buf=arg;
  int64 rez=0;

  while (*buf)
  {
    if (!isdigit(*buf)) break;
    rez*=10;
    rez+=(int)(*buf)-48;
    buf++;
  }
  return rez;
}

int str_len(const char *arg)
{
 const char *buf;
 int rez;

 rez = 0;
 buf = arg;
 while(*buf != '\0')
 {
  if (*buf=='{' || *buf=='`')
  {
   buf++;
   if (*buf=='{' || *buf=='`')
    rez++;
  }
  else rez++;
  buf++;
 }
 if (rez < 0)
 {
  bug("Wrong string length",0);
  rez = 0;
 }

 return rez;
}

const char *str_cut(char *arg,int len)
{
 char *buf;
 int rez;

 rez = 0;
 buf = arg;
 while(*arg != '\0')
 {
  if (*arg=='{' || *arg=='`')
  {
   arg++;
   if (*arg=='{' || *arg=='`')
    rez++;
  }
  else rez++;
  arg++;
  if (rez > len)
  {
   arg--;
   if (*arg=='{' || *arg=='`') *arg='\0';
   else *(++arg)='\0';
   break;
  }
 }
 return str_dup(buf);
}

// returns material number
int material_num(const char *name)
{
  int i;
  for (i=0; material_table[i].name != NULL; i++)
    if ( !str_cmp( material_table[i].name, name)) return i;

  return MATERIAL_NONE;
}

int materialrus_num(const char *name)
{
  int i;
  for (i=0; material_table[i].name != NULL; i++)
    if (! str_cmp( material_table[i].real_name, name)) return i;

  return MATERIAL_NONE;
}


int item_cond_num( OBJ_DATA *o )
{
  int i;

  for ( i=0; item_cond_table[i].oIndex != -1; i++)
  {
    if( !o ) return OBJECT_BROKEN;

    if( o->durability == -1 ) return OBJECT_ETERNAL;

    if( (o->condition*100 / ((o->durability==0)?1:o->durability) ) <= item_cond_table[i].calccon)
          return i;
  }
  return OBJECT_BROKEN;
}

const char *get_obj_cond( OBJ_DATA *o, int rtype )
{
  switch( rtype)
  {
    case 0:
      if( !o ) return item_cond_table[1].showcon;
      if( o->durability == -1 ) return item_cond_table[OBJECT_ETERNAL].showcon;
      return item_cond_table[item_cond_num(o)].showcon;
    case 1:
      if( !o ) return item_cond_table[1].gshowcon;
      if( o->durability == -1 ) return item_cond_table[OBJECT_ETERNAL].gshowcon;
      return item_cond_table[item_cond_num(o)].gshowcon;
    case 2:
    default:
      if( !o ) return item_cond_table[1].showshortcon;
      if( o->durability == -1 ) return item_cond_table[OBJECT_ETERNAL].showshortcon;
      return item_cond_table[item_cond_num(o)].showshortcon;
  }
  return item_cond_table[1].showshortcon;
}

/*
int *get_material_cond( const char *argument, int d_h_number )
{
 switch( d_h_number )
 {
   case '1':
   {
     if( !o ) return material_table[1].d_dam;
     if( o->durability == -1 ) return item_cond_table[OBJECT_ETERNAL].gshowcon;
     return item_cond_table[item_cond_num(o)].gshowcon;
   }
   case '2':
  else
  {
    if( !o ) return item_cond_table[1].showcon;
    if( o->durability == -1 ) return item_cond_table[OBJECT_ETERNAL].showcon;
    return item_cond_table[item_cond_num(o)].showcon;
  }
}
*/

void raffect_to_room(RAFFECT *ra,ROOM_INDEX_DATA *room)
{
  if (!ra || !room)
  {
    log_string("BUG: raffect_to_room: NULL raffect or room");
    return;
  }
  ra->next_in_room=room->raffect;
  room->raffect=ra;
  ra->room=room;
  SET_BIT(room->ra,ra->bit);
}

RAFFECT *get_raffect(ROOM_INDEX_DATA *room, int bit)
{
  RAFFECT *ra;
  
  if (!room || !room->raffect) return NULL;
  for (ra=room->raffect;ra;ra=ra->next_in_room)
  {
    if (ra->bit==bit) return ra;
  }
  return NULL;
}

int raffect_level( ROOM_INDEX_DATA *room, int ra_bit)
{
  RAFFECT *ra;

  if (!room || !IS_SET(room->ra,ra_bit) || !room->raffect) return 0;

  for (ra=room->raffect;ra;ra=ra->next_in_room)
  {
    if (ra->bit==ra_bit) return ra->level;
  }
  return 0;
}

void raffect_from_room(RAFFECT *raffect)
{
  char buf[MAX_STRING_LENGTH];
  RAFFECT *ra;
  ROOM_INDEX_DATA *room;
  bool found=FALSE;

  if (!raffect || !(room=raffect->room)) return;
  if (IS_SET(room->ra,raffect->bit))
  {
    REM_BIT(room->ra,raffect->bit);
    switch(raffect->bit)
    {
      default:
        do_printf(buf,"Ты чувствуешь какое-то изменение вокруг.");
        break;
      case RAFF_BLIND:
        do_printf(buf,"Черный туман рассеялся без следа.");
        break;
      case RAFF_EVIL_PR:
        do_printf(buf,"Мрачная аура местности исчезает.");
        break;
      case RAFF_LIFE_STR:
        do_printf(buf,"Благословенный свет озаряющий комнату тускнеет.");
        break;
      case RAFF_SAFE_PLC:
        do_printf(buf,"Здесь больше не безопасно.");
        break;
      case RAFF_VIOLENCE:
        do_printf(buf,"Это место больше не вызывает у тебя желание убивать.");
        break;  
    break;  

    }
    if (room->people) act(buf,room->people,NULL,NULL,TO_ALL_IN_ROOM);
  }

  if (room->raffect==raffect)
  {
    found=TRUE;
    room->raffect=raffect->next_in_room;
  }
  else for (ra=room->raffect;ra;ra=ra->next_in_room)
  {
    if (ra->next_in_room==raffect)
    {
      ra->next_in_room=raffect->next_in_room;
      found=TRUE;
      break;
    }
  }
  if (found==FALSE)
  {
    log_string("BUG: raffect_from_room - raffect not in this room");
    return;
  }
  raffect->next_in_room=NULL;
  raffect->room=NULL;
}

char *raff_name( int64 vector )
{
  static char buf[512];

  buf[0] = '\0';
  if ( vector & RAFF_BLIND   ) strcat( buf, " Black Cloud"  );
  if ( vector & RAFF_HIDE    ) strcat( buf, " Distortion"   );
  if ( vector & RAFF_NOWHERE ) strcat( buf, " Nowhere"      );
  if ( vector & RAFF_WEB     ) strcat( buf, " Web"          );
  if ( vector & RAFF_ALL_VIS ) strcat( buf, " All visible"  );
  if ( vector & RAFF_OASIS   ) strcat( buf, " Oasis"        );
  if ( vector & RAFF_MIND_CH ) strcat( buf, " Mind Channel" );
  if ( vector & RAFF_EVIL_PR ) strcat( buf, " Evil Presence");
  if ( vector & RAFF_LIFE_STR) strcat( buf, " Life Stream"  );
  if ( vector & RAFF_SAFE_PLC) strcat( buf, " Safty Place"  );
  if ( vector & RAFF_VIOLENCE) strcat( buf, " Violence"     );
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

int64 flag_lookup(const char *name, const struct flag_type *flag_table)
{
  int flag;

  for (flag = 0; flag_table[flag].name != NULL; flag++)
  {
    if (LOWER(name[0]) == LOWER(flag_table[flag].name[0])
    &&  !str_prefix(name,flag_table[flag].name)) return flag_table[flag].bit;
  }
  return NO_FLAG;
}

CLAN_DATA *clan_lookup(const char *name)
{
  CLAN_DATA *clan;

  for (clan = clan_list; clan!=NULL; clan=clan->next)
  {
    if (LOWER(name[0]) == LOWER(clan->name[0])
    &&  !str_prefix(name,clan->name)) return clan;
  }
  return NULL;
}

int position_lookup (const char *name)
{
  int pos;

  for (pos = 0; position_table[pos].name != NULL; pos++)
  {
    if (LOWER(name[0]) == LOWER(position_table[pos].name[0])
    &&  !str_prefix(name,position_table[pos].name)) return pos;
  }
  return -1;
}

int sex_lookup (const char *name)
{
  int sex;
  
  for (sex = 0; sex_table[sex].name != NULL; sex++)
  {
     if (LOWER(name[0]) == LOWER(sex_table[sex].name[0])
     &&  !str_prefix(name,sex_table[sex].name)) return sex;
  }
  return -1;
}

int size_lookup (const char *name)
{
  int size;

  for ( size = 0; size_table[size].name != NULL; size++)
  {
     if (LOWER(name[0]) == LOWER(size_table[size].name[0])
     &&  !str_prefix( name,size_table[size].name)) return size;
  }
  return -1;
}

// returns race number
int race_lookup (const char *name)
{
  int race;

  for ( race = 0; race_table[race].name != NULL; race++)
  {
    if (LOWER(name[0]) == LOWER(race_table[race].name[0])
      &&  !str_prefix( name,race_table[race].name)) return race;
  }
  return 0;
} 

int deity_lookup (const char *name)
{
  int deity;

  for ( deity = 0; deity_table[deity].name != NULL; deity++)
  {
    if (LOWER(name[0]) == LOWER(deity_table[deity].name[0])
      &&  !str_prefix( name,deity_table[deity].name)) return deity;
  }
  return 0;
} 

int liq_lookup (const char *name)
{
  int liq;

  for ( liq = 0; fix_liq_table[liq].name; liq++)
  {
    if (name[0] == fix_liq_table[liq].name[0]
      && !str_prefix(name,fix_liq_table[liq].name))
     {
       name=fix_liq_table[liq].newname;
       break;
     }
  }

  for ( liq = 0; liq_table[liq].liq_name; liq++)
  {
    if (LOWER(name[0]) == LOWER(liq_table[liq].liq_name[0])
      && !str_prefix(name,liq_table[liq].liq_name)) return liq;
  }
  return 0;
}

int weapon_lookup (const char *name)
{
  int type;

  for (type = 0; weapon_table[type].name != NULL; type++)
  {
    if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
      &&  !str_prefix(name,weapon_table[type].name)) return type;
  }

  return -1;
}
// returns material
char *material_lookup (const char *name)
{
  int i;
  for ( i=0; material_table[i].name != NULL; i++)
  {
//    if (material_table[i].name==NULL) return "none";
    if (!strcmp(material_table[i].name,name)) break;
  }
  if( material_table[i].name ) return material_table[i].name;
  return "BUG";
}

char *materialrus_lookup (const char *name)
{
  int i;
  for ( i=0; material_table[i].name != NULL; i++)
  {
    if (!strcmp(material_table[i].real_name,name)) break;
  }
  if( material_table[i].real_name ) return material_table[i].real_name;
  return "БАГ";
}

int item_lookup(const char *name)
{
 int type;

 for (type = 0; item_table[type].name != NULL; type++)
 {
   if (LOWER(name[0]) == LOWER(item_table[type].name[0])
     &&  !str_prefix(name,item_table[type].name))
       return item_table[type].type;
  }
  return -1;
}

int attack_lookup  (const char *name)
{
  int att;

  for ( att = 0; attack_table[att].name != NULL; att++)
  {
    if (LOWER(name[0]) == LOWER(attack_table[att].name[0])
        &&  !str_prefix(name,attack_table[att].name))
            return att;
  }
  return 0;
}

// returns a flag for wiznet
int64 wiznet_lookup (const char *name)
{
  int64 flag;

  for (flag = 0; wiznet_table[flag].name != NULL; flag++)
  {
        if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0])
        && !str_prefix(name,wiznet_table[flag].name))
            return flag;
  }
  return -1;
}

// returns class number
int class_lookup (const char *name)
{
  int class;
 
  for ( class = 0; class < MAX_CLASS; class++)
  {
     if (LOWER(name[0]) == LOWER(class_table[class].name[0])
     &&  !str_prefix( name,class_table[class].name))
         return class;
  }
  return -1;
}

char *chan_bit_name(int64 comm_flags)
{
  static char buf[512];

  buf[0] = '\0';

  if (comm_flags & CBIT_IMMTALK  ) strcat(buf, " immt");
  if (comm_flags & CBIT_AUCTION  ) strcat(buf, " auction");
  if (comm_flags & CBIT_GOSSIP   ) strcat(buf, " gossip");
  if (comm_flags & CBIT_SHOUT    ) strcat(buf, " shout");
  if (comm_flags & CBIT_QUESTION ) strcat(buf, " question");
  if (comm_flags & CBIT_ANSWER   ) strcat(buf, " answer");
  if (comm_flags & CBIT_MUSIC    ) strcat(buf, " music");
  if (comm_flags & CBIT_CLAN     ) strcat(buf, " clan");
  if (comm_flags & CBIT_QUOTE    ) strcat(buf, " quote");
  if (comm_flags & CBIT_INFO     ) strcat(buf, " info");
  if (comm_flags & CBIT_CENSORED ) strcat(buf, " censored");
  if (comm_flags & CBIT_GRAT     ) strcat(buf, " grats");
  if (comm_flags & CBIT_GSOCIAL  ) strcat(buf, " gsocial");
  if (comm_flags & CBIT_EMOTE    ) strcat(buf, " emote");
  if (comm_flags & CBIT_CHAT     ) strcat(buf, " chat");
  if (comm_flags & CBIT_NEWBIE   ) strcat(buf, " newbie");
  if (comm_flags & CBIT_ALLI     ) strcat(buf, " alli");
  if (comm_flags & CBIT_KAZAD    ) strcat(buf, " kazad");
  if (comm_flags & CBIT_PTALK    ) strcat(buf, " ptalk");
  if (comm_flags & CBIT_AVENGE   ) strcat(buf, " avenge");
  if (comm_flags & CBIT_FD       ) strcat(buf, " game");
  
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

bool check_clanspell(int sn, CLAN_DATA *clan)
{
  int i;
  if (!IS_SET(skill_table[sn].flag,S_CLAN)) return TRUE;
  if (!clan || clan->clansn[0]==0) return FALSE;

  for (i=0;i<20;i++)
  {
    if (sn==clan->clansn[i]) return TRUE;
    if (clan->clansn[i]==0) break;
  }
  return FALSE;
}
