// $Id: fight.c,v 1.209 2004/04/29 14:47:45 ghost Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h> 
#include "merc.h" 
#include "interp.h" 
#include "tables.h"
#include "recycle.h"
#include "magic.h"
 
// Local functions.struct corpse_type
void do_stop_fighting( CHAR_DATA *ch, ROOM_INDEX_DATA *room);
void add_pkiller(CHAR_DATA *ch, CHAR_DATA *killer); 
void remove_pkiller(CHAR_DATA *ch, char *name); 
void check_assist       args( (CHAR_DATA *ch,CHAR_DATA *victim ) ); 
void cant_mes           args( (CHAR_DATA *ch )); 
bool check_blink        args( (CHAR_DATA *ch,CHAR_DATA *victim,OBJ_DATA *obj));
bool check_dodge        args( (CHAR_DATA *ch,CHAR_DATA *victim,OBJ_DATA *obj)); 
void check_criminal     args( (CHAR_DATA *ch,CHAR_DATA *victim, int level ) ); 
bool check_parry        args( (CHAR_DATA *ch,CHAR_DATA *victim,OBJ_DATA *obj)); 
bool check_shield_block args( (CHAR_DATA *ch,CHAR_DATA *victim,OBJ_DATA *obj)); 
void dam_message        args( (CHAR_DATA *ch,CHAR_DATA *victim, int dam, 
                               int dt, bool immune, OBJ_DATA *obj) ); 
void death_cry          args( ( CHAR_DATA *ch ) ); 
void group_gain         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) ); 
int  xp_compute         args( ( CHAR_DATA *gch, CHAR_DATA *victim,  
                                int total_levels ) ); 
int  get_ac_modifier    args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int  get_vuln_modifier  args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt) );
bool is_safe            args( ( CHAR_DATA *ch, CHAR_DATA *victim ) ); 
void make_corpse        args( ( CHAR_DATA *ch ) ); 
void one_hit            args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, OBJ_DATA *obj, bool rgh) ); 
void new_one_hit        args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, OBJ_DATA *obj, bool rgh) );
void mob_hit            args( ( CHAR_DATA *ch, CHAR_DATA *victim ) ); 
void raw_kill           args( ( CHAR_DATA *victim ) ); 
void set_fighting       args( ( CHAR_DATA *ch, CHAR_DATA *victim ) ); 
void disarm             args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool fLeft ) ); 
bool check_skill        args( ( CHAR_DATA *ch, int gsn_skill) ); 
void do_ear             args( ( CHAR_DATA *victim, CHAR_DATA *ch) ); 
int  min_level          args( ( CHAR_DATA *ch, int sn) ); 
void char_death         args( ( CHAR_DATA *ch) ); 
void damage_both_objs   args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *c_obj, 
                                OBJ_DATA *v_obj, int reason) );
void damage_eq_char     args( ( CHAR_DATA *ch) );
void tatoo_works(CHAR_DATA *ch, CHAR_DATA *victim);


// function for exp calculating 
int check_victim     args( ( CHAR_DATA *ch, CHAR_DATA *victim) ); 


struct corpse_list
{
  char *name;
  char *short_n;
  char *long_n;
};

const struct corpse_list corpse_table[]=
{
 { "Utka",   "жарен|ая|ой|ой|ую|ой|ой утятин|а|ы|е|у|ой|не",
             "Жареная утятина из Utka обалденно пахнет поджаренными лапками." },
 { "Lion",   "жарен|ая|ой|ой|ую|ой|ой ветчин|а|ы|е|у|ой|е",
             "Жареная ветчина из Льва отдает густой паленой шерстью." },
 { "Belka",  "жарен|ая|ой|ой|ую|ой|ой бельчатин|а|ы|е|у|ой|е",
             "Жареная бельчатина из Belka с подсмаленым хвостиком." },
 { "Doomer", "Зелен|ый|ого|ому|ого|ым|ом труп||а|у||ом|е Doomer-a",
             "Труп Думера зеленого цвета, вернее его остатки словно после выстрела BFG." },
 { NULL,NULL,NULL }
};

void damage_both_objs( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *c_obj, OBJ_DATA *v_obj, int reason)
{
  int d_dam_diff, dtemp, codam = 1, vodam = 1;
  bool inf_char, cgreater = 0;

  if( !IS_SET(global_cfg,CFG_DAMAGEOBJ) ) return;

  d_dam_diff = ( material_table[material_num(c_obj->material)].d_dam -
                 material_table[material_num(v_obj->material)].d_dam );

  if( d_dam_diff == 0 )
  {
    damage_obj(     ch, c_obj, codam, 0);
    damage_obj( victim, v_obj, vodam, 0);
    return;
  }
  else if( d_dam_diff > 0 ) cgreater = 1;
  else
  {
    d_dam_diff = - d_dam_diff;
    cgreater = 0;
  }

  if( reason == DAM_CLEAVE )
  {
    if( cgreater)
    {
      codam += d_dam_diff/16;
      vodam += d_dam_diff/12;
    }
    else
    {
      dtemp = codam;
      codam = vodam;
      vodam = dtemp;
    }
    inf_char = 1;
  }
  else
  {
    if( cgreater)
    {
      codam += d_dam_diff/24;
      vodam += d_dam_diff/19;
    }
    else
    {
      dtemp = codam;
      codam = vodam;
      vodam = dtemp;
    }
    inf_char = 0;
  }

  damage_obj(     ch, c_obj, codam, inf_char);
  damage_obj( victim, v_obj, vodam, inf_char);
};

void damage_eq_char ( CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   OBJ_DATA *c_weap;
   int i, w_skill, dam_v;
   bool critical_hit=FALSE;

   if( !IS_SET(global_cfg,CFG_DAMAGEOBJ) ) return;

   if( ch==NULL || (victim=ch->fighting)==NULL ) return;

   c_weap = get_eq_char( ch, WEAR_RHAND );
   if( !c_weap || c_weap->item_type != ITEM_WEAPON ) return;
   ptc( ch, "{RWeapon recieved: %s\n\r", c_weap->short_descr);

   w_skill = get_weapon_skill( ch, get_weapon_sn(ch, TRUE) )*3/4;
   ptc( ch,"{RRecieving ch's weapon skill: \n\r{Y%d\n\r", w_skill);

   if( w_skill < number_range(1, 90) ) return;
                                              
   if((obj = get_eq_char(victim, wear_l[number_range( 0, MAX_WEAR_L-1)].wear_num))==NULL)
   {
     stc("{RNull obj recieved!\n\r", ch);
     return;
   }

//   obj = get_eq_char( victim, wear_l[number_range( 1, (MAX_WEAR_L-1))].wear_num);
//   if( obj != NULL || number_range(0,4) == 0 ) return;

   ptc( ch, "{RRecieving random victim object: \n\r{Y%s\n\r", obj->short_descr);

   if( obj != NULL || obj->item_type == ITEM_LIGHT || number_range(0,4) == 0)
   {
     stc("{RExistant(?), Item type light filtered...\n\r", ch);
     return;
   }

   if( (get_curr_stat( ch, STAT_STR) > 30) && number_range(0,1) == 0 )
       critical_hit = TRUE;

   if( critical_hit && obj->item_type == ITEM_ARMOR )
     for (i = 0; i < 4; i++) obj->value[i] -= 1;
   stc("{Rac checked on damage...\n\r", ch);
 
   dam_v = ( material_table[material_num(c_weap->material)].d_dam -
             material_table[material_num(obj->material)].d_dam ) / 30;

   if( dam_v <= 0 ) return;
   if( critical_hit ) dam_v = dam_v*4/3;

   damage_obj( victim, obj, dam_v, 1);
   ptc( ch, "%s {Ris damaged by %d!!!\n\r", get_obj_desc( obj, '1'), dam_v);
}; 

// Control the fights going on. Called periodically by update_handler.
void violence_update( void ) 
{ 
  CHAR_DATA *ch; 
  CHAR_DATA *victim; 
 
  for ( ch = char_list;ch; ch = ch->next ) 
  { 
    if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL ) continue; 
    if ( IS_AWAKE(ch) && ch->in_room == victim->in_room ) multi_hit( ch, victim);
    else stop_fighting( ch, FALSE ); 
    if ( ( victim = ch->fighting ) == NULL ) continue; 
    check_assist(ch,victim); 
  } 
} 
 
void check_assist(CHAR_DATA *ch,CHAR_DATA *victim) 
{ 
  CHAR_DATA *rch, *rch_next; 
 
  for (rch = ch->in_room->people; rch != NULL; rch = rch_next) 
  { 
    rch_next = rch->next_in_room; 
         
    if (IS_AWAKE(rch) && rch->fighting == NULL) 
    { 
      if (!IS_NPC(ch) && IS_NPC(rch)  
       && IS_SET(rch->off_flags,ASSIST_PLAYERS) 
       && victim->questmob == NULL 
       &&  rch->level + 6 > victim->level) 
      { 
        do_function(rch, &do_emote, "вскpикивает и атакует!"); 
        multi_hit(rch,victim); 
        continue; 
      } 
      /* PCs next */ 
      if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM)) 
      { 
        if ( ( (!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST)) 
         || IS_AFFECTED(rch,AFF_CHARM))  
         && is_same_group(ch,rch)  
         && !is_safe(rch, victim)) multi_hit (rch,victim); 
        continue; 
      } 
         
      /* now check the NPC cases */ 
      if ( IS_NPC( ch ) ) 
      { 
        if ( HAS_TRIGGER( ch, TRIG_FIGHT ) ) 
         mp_percent_trigger( ch, victim, NULL, NULL, TRIG_FIGHT ); 
        if ( HAS_TRIGGER( ch, TRIG_HPCNT ) ) 
         mp_hprct_trigger( ch, victim ); 
      } 
             
      if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM)) 
      { 
        if ( (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL)) 
          || (IS_NPC(rch) && rch->group && rch->group == ch->group) 
          || (IS_NPC(rch) && rch->race == ch->race  
          && IS_SET(rch->off_flags,ASSIST_RACE)) 
          || (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN) 
          && ((IS_GOOD(rch)    && IS_GOOD(ch)) 
          || (IS_EVIL(rch)    && IS_EVIL(ch)) 
          || (IS_NEUTRAL(rch) && IS_NEUTRAL(ch))))  
          || (rch->pIndexData == ch->pIndexData  
          && IS_SET(rch->off_flags,ASSIST_VNUM))) 
        { 
          CHAR_DATA *vch; 
          CHAR_DATA *target; 
          int number; 
 
          if (number_bits(1) == 0) continue; 
                 
          target = NULL; 
          number = 0; 
          for (vch = ch->in_room->people; vch; vch = vch->next) 
          { 
            if (can_see(rch,vch,CHECK_LVL) 
             &&  is_same_group(vch,victim) 
             && vch->questmob==NULL 
             &&  number_range(0,number) == 0) 
            { 
              target = vch; 
              number++; 
            } 
          } 
          if (target != NULL) 
          { 
            do_function(rch, &do_emote, "вскpикивает и атакует!"); 
            multi_hit(rch,target); 
          } 
        }        
      } 
    } 
  } 
} 
 
// Do one group of attacks.
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim)
{ 
  int chance; 
  OBJ_DATA *wield; 
 
  // decrement the wait
  if (ch->desc == NULL) ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE); 
  if (ch->desc == NULL) ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE);  
 
  // no attacks for stunnies -- just a check
  if (ch->position < POS_RESTING) return; 
 
  if (IS_NPC(ch)) 
  { 
    mob_hit(ch,victim); 
    return; 
  } 
 
  wield=get_eq_char(ch,WEAR_RHAND);

//shans prohojdeniya spella, unikal'nogo dlya kajdogo deity
  if (IS_DEVOTED_ANY(ch))
  {
    chance=ch->pcdata->favour;
    if (IS_DEVOTED_ANY(victim) && ch->pcdata->dn == victim->pcdata->dn)
      chance-=victim->pcdata->favour/2;

//     0,   100,  "Последователь"      6%  100      3%  0
//   101,   500,  "Младший аколит"     9%  500      6%  100
//   501,  1000,  "Старший аколит"     12% 1000     9%  500
//  1001,  2500,  "Аббат"              15% 2.5K     12% 1000
//  2501,  4949,  "Жрец"               18% 5K       15% 2500
//  4950,  5000,  "Верховный Жрец"     

    if      (chance>0    && chance<=100 ) chance=3+chance/33;
    else if (chance>100  && chance<=500 ) chance=6+(chance-100)/133;
    else if (chance>500  && chance<=1000) chance=9+(chance-500)/166;
    else if (chance>1000 && chance<=2500) chance=12+(chance-1000)/500;
    else if (chance>2500 && chance<=5000) chance=15+(chance-2500)/833;
    else chance=0;

    if (number_percent() < chance) tatoo_works(ch, victim);
  }

  one_hit( ch, victim, TYPE_UNDEFINED, NULL, TRUE ); 
  if (IS_AFFECTED(ch,AFF_HASTE)) one_hit(ch,victim,TYPE_UNDEFINED, NULL, TRUE  ); 
 
  chance = get_skill(ch,gsn_second_attack)/2; 
  if (IS_AFFECTED(ch,AFF_SLOW)) chance /= 2; 
  if (!(wield != NULL && wield->item_type != ITEM_WEAPON)
    && number_percent( ) < chance ) 
  { 
    one_hit( ch, victim, TYPE_UNDEFINED, NULL, TRUE ); 
    check_improve(ch,gsn_second_attack,TRUE,5); 
  } 
 
  chance = get_skill(ch,gsn_third_attack)/4; 
  if (IS_AFFECTED(ch,AFF_SLOW)) chance = 0; 
  if (!(wield != NULL && wield->item_type != ITEM_WEAPON)
    && number_percent( ) < chance ) 
  { 
    one_hit( ch, victim, TYPE_UNDEFINED, NULL, TRUE ); 
    check_improve(ch,gsn_third_attack,TRUE,6); 
  } 
 
  if (wield != NULL 
   && wield->item_type == ITEM_WEAPON
   && (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS)
    && (!IS_SET(race_table[ch->race].spec,SPEC_TWOHAND)))) return;

  wield=get_eq_char(ch,WEAR_LHAND); 
  if (wield==NULL || wield->item_type!=ITEM_WEAPON) return; 
  chance = get_skill (ch,gsn_dual); 
  if (IS_AFFECTED(ch,AFF_SLOW)) chance /= 3; 
  if ( number_percent( ) < chance ) 
  { 
    one_hit( ch, victim, TYPE_UNDEFINED, NULL, FALSE ); 
    if (IS_AFFECTED(ch,AFF_HASTE) && ch->classwar &&
        number_percent( ) < chance) one_hit(ch,victim,TYPE_UNDEFINED, NULL, FALSE  );
    if (wield) check_improve(ch,gsn_dual,TRUE,6); 
  } 
 
  chance = get_skill(ch,gsn_dual_sec_attack); 
  if (IS_AFFECTED(ch,AFF_SLOW)) chance /= 10; 
  if (IS_AFFECTED(ch,AFF_HASTE)) chance+=(chance/3); 
  if ( number_percent( ) < chance ) 
  { 
    one_hit( ch, victim, TYPE_UNDEFINED, NULL, FALSE ); 
    if (wield!=NULL) check_improve(ch,gsn_dual_sec_attack,TRUE,6); 
  } 
} 
 
// procedure for all mobile attacks
void mob_hit (CHAR_DATA *ch, CHAR_DATA *victim) 
{ 
  int chance,number; 
  CHAR_DATA *vch, *vch_next; 
 
  one_hit(ch,victim,TYPE_UNDEFINED, NULL, TRUE); 
  if (ch->fighting != victim) return; 

  // Area attack -- BALLS nasty!
  if (IS_SET(ch->off_flags,OFF_AREA_ATTACK)) 
  { 
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) 
    { 
      vch_next = vch->next; 
      if ((vch != victim && vch->fighting == ch)) one_hit(ch,vch,TYPE_UNDEFINED, NULL, TRUE); 
    } 
  } 
  if (IS_AFFECTED(ch,AFF_HASTE) ||  (IS_SET(ch->off_flags,OFF_FAST) 
   && !IS_AFFECTED(ch,AFF_SLOW))) one_hit(ch,victim,TYPE_UNDEFINED, NULL, TRUE); 
 
  chance = get_skill(ch,gsn_second_attack)/2; 
 
  if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST)) chance /= 2; 
 
  if (number_percent() < chance) 
  { 
    one_hit(ch,victim,TYPE_UNDEFINED, NULL, TRUE); 
    if (ch->fighting != victim) return; 
  } 
 
  chance = get_skill(ch,gsn_third_attack)/4; 
  if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST)) chance = 0; 
 
  if (number_percent() < chance) 
  { 
    one_hit(ch,victim,TYPE_UNDEFINED, NULL, TRUE); 
    if (ch->fighting != victim) return; 
  }  
 
  if (ch->wait > 0) return; 

//  number = number_range(0,2); 
//  if (number == 1 && IS_SET(ch->act,ACT_MAGE)) { mob_cast_mage(ch,victim); return; } 
//  if (number == 2 && IS_SET(ch->act,ACT_CLERIC)) { mob_cast_cleric(ch,victim); return; } 
 
  /* now for the skills */ 
  if (IS_SET(ch->off_flags, OFF_BACKSTAB) && ! IS_AFFECTED(ch,AFF_CHARM) &&
      ! IS_SET(ch->in_room->room_flags,ROOM_NOFLEE))
  {
    int nDir;
    int nBackDirs[DIR_DOWN+1] =
      {DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST, DIR_DOWN, DIR_UP};
    OBJ_DATA *obj;
    CHAR_DATA *vict;
    char *cDirs[DIR_DOWN+1] =
      {"N", "E", "S", "W", "U", "D"};

    vict = (! IS_NPC(victim) || ! victim->master) ? victim :
      (victim->in_room == victim->master->in_room ? victim->master : NULL);

  if ((obj=get_eq_char(ch,WEAR_RHAND))!=NULL)
  {
    if (obj->item_type!=ITEM_WEAPON ||
    (obj->value[0]!=WEAPON_SWORD && obj->value[0]!=WEAPON_DAGGER && obj->value[0]!=WEAPON_SPEAR)) obj=NULL;
  }
  if (!obj)
  {
    obj=get_eq_char(ch,WEAR_LHAND);
    if (!obj || obj->item_type!=ITEM_WEAPON ||
    (obj->value[0]!=WEAPON_SWORD && obj->value[0]!=WEAPON_DAGGER && obj->value[0]!=WEAPON_SPEAR)) obj=NULL;
  }
    if (vict && obj && can_see(ch, vict,CHECK_LVL))

      for (nDir = DIR_NORTH; nDir <= DIR_DOWN; nDir++)
      {
        if (ch->in_room->exit[nDir] && ch->in_room->exit[nDir]->u1.to_room &&
            ch->in_room->exit[nDir]->u1.to_room->exit[nBackDirs[nDir]] &&
            ch->in_room->exit[nDir]->u1.to_room->exit[nBackDirs[nDir]]->u1.to_room == ch->in_room &&
                        ! IS_SET(ch->in_room->exit[nDir]->exit_info, EX_CLOSED) &&
            ! IS_SET(ch->in_room->exit[nDir]->u1.to_room->exit[nBackDirs[nDir]]->exit_info, EX_CLOSED))
        {
          do_flee(ch, cDirs[nDir]);
          do_move_char(ch, nBackDirs[nDir], FALSE, FALSE);
          do_function(ch, &do_backstab, vict->name);
          one_hit(ch,vict,TYPE_UNDEFINED, NULL, TRUE); 
          return;
        }
      }
  }
  number = number_range(0,8); 
 
  switch(number)  
  { 
  case (0) : 
    if (IS_SET(ch->off_flags,OFF_BASH)) do_function(ch, &do_bash, ""); 
    break; 
  case (1) : 
    if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK)) 
      do_function(ch, &do_berserk, ""); 
    break; 
  case (2) : 
    if (IS_SET(ch->off_flags,OFF_DISARM) ||
       (get_weapon_sn(ch, TRUE) != gsn_hand_to_hand 
     &&(IS_SET(ch->act,ACT_WARRIOR) ||  IS_SET(ch->act,ACT_THIEF))))
      do_function(ch, &do_disarm, ""); 
    break; 
  case (3) : 
     if (IS_SET(ch->off_flags,OFF_KICK)) do_function(ch, &do_kick, ""); 
     break; 
  case (4) : 
     if (IS_SET(ch->off_flags,OFF_KICK_DIRT)
      || IS_SET(ch->act,ACT_THIEF)) do_function(ch, &do_dirt, ""); 
     break;
  case (5) : 
     if (IS_SET(ch->off_flags,OFF_TAIL)) do_function(ch, &do_tail, ""); 
     break;  
  case (6) : 
     if (IS_SET(ch->off_flags,OFF_TRIP)) do_function(ch, &do_trip, ""); 
     break; 
  case (7) : 
     if (IS_SET(ch->off_flags,OFF_CRUSH)) do_function(ch, &do_crush, "") ; 
     break; 
  case (8) : break; 
  } 
} 
 
void one_hit(CHAR_DATA *ch,CHAR_DATA *victim,int dt,OBJ_DATA *obj,bool rgh)
{ 
  OBJ_DATA *wield=NULL;  int victim_ac;
  int thac0, thac0_00, thac0_32,dam,diceroll,skill,dam_type,sn=-1;
  bool result; 
 
  if (IS_SET(global_cfg,CFG_NEWHIT))
  {
    new_one_hit(ch,victim,dt,obj,rgh);
    return;
  }
  if (!ch || !victim || victim==ch) return; 
  if (ch->position < POS_RESTING) return; //no attacks for stunnies -just a check
  if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return; 

  if (is_safe(ch,victim)) return;
  if (victim->fighting == NULL ) set_fighting( victim, ch );

  if (!IS_NPC(ch) && !IS_NPC(victim)) check_criminal(ch,victim,0); 

  if (rgh) wield = get_eq_char( ch, WEAR_RHAND );
  else wield = get_eq_char( ch, WEAR_LHAND );

  if (wield!=NULL && wield->item_type!=ITEM_WEAPON) return;
 
  if(IS_AFFECTED(victim, AFF_FIRESHIELD) && number_percent() >= 93) //was 95
  {
   act("{y$c1{x обжигается о твой огненный щит, и с визгом отскакивает!",ch,NULL,victim,TO_VICT);
   act("Ты обжигаешься об огненный щит, окружающий {y$C4{x, и отскакиваешь!",ch,NULL,victim,TO_CHAR);
   act("{y$c1{x обжигается об огненный щит, окружающий {y$C4{x, и с визгом отскакивает.",ch,NULL,victim,TO_NOTVICT);
   damage(victim,ch,victim->level/2,37,DAM_FIRE,TRUE, FALSE, NULL);
   return;
  }

  if (!obj)
  {
    if (dt==TYPE_UNDEFINED)
    {
      if (wield!=NULL) dam_type = (int)wield->value[3];
      else
      {
        if (ch->race==RACE_UNIQUE) dam_type = ch->dam_type;
        else dam_type = attack_lookup(race_table[ch->race].hand_dam);
        if (GUILD(ch,VAMPIRE_GUILD) && (time_info.hour>19 || time_info.hour<6))
        dam_type=attack_lookup("drain");
      }
      dt=attack_table[dam_type].damage;
    }
    else if (dt==gsn_backstab && wield) dam_type = (int)wield->value[3];
    else dam_type=attack_lookup(race_table[ch->race].hand_dam);

    sn = get_weapon_sn(ch,rgh);
    skill = 20+get_weapon_skill(ch,sn);
  }
  else
  {
    sn = gsn_missile;
    dt = gsn_missile;
    skill = 20 + get_skill(ch,sn);
    if (obj->item_type==ITEM_WEAPON) dam_type=(int)obj->value[3];
    else dam_type = 1;
  }

  if (IS_NPC(ch))
  {
    thac0_00 = 20*(1+ch->level/20);
    thac0_32 = -4*(1+ch->level/20); // as good as a thief
    if (IS_SET(ch->act,ACT_WARRIOR))    thac0_32 *= 2;
    else if (IS_SET(ch->act,ACT_THIEF)) thac0_32 *= 3;
    else if (IS_SET(ch->act,ACT_MAGE))  thac0_32 /= 2;
  }
  else
  { 
    thac0_00 = 0;
    if (ch->classwar==1) thac0_32 = -10*(1+ch->level/20);
    else if (ch->classthi==1) thac0_32 = -4*(1+ch->level/20);
    else if (ch->classcle==1) thac0_32 = 2*(1+ch->level/20);
    else thac0_32 = 6*(1+ch->level/20);
  }

  thac0 = interpolate( ch->level, thac0_00, thac0_32 );
  if (thac0 < 0) thac0 = thac0/2;
  if (thac0 < -5) thac0 = -5 + (thac0 + 5) / 2;
  thac0 -= GET_HITROLL(ch) * skill/100;
  thac0 += 5 * (100 - skill) / 100;

  if (dt==gsn_backstab) 
   thac0-=10*(100-get_skill(ch,gsn_backstab)-5*(category_bonus(ch,OFFENCE)-category_bonus(victim,PROTECT)));

  switch(dt)
  {
    case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/10; break;
    case(DAM_BASH):  victim_ac = GET_AC(victim,AC_BASH)/10;   break;
    case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/10;  break;
    default:         victim_ac = GET_AC(victim,AC_EXOTIC)/10; break;
  };

  if (victim_ac < -15) victim_ac = (victim_ac + 15) / 5 - 15;
  if (!can_see( ch, victim,CHECK_LVL ) ) victim_ac -= victim->level;
  if (victim->position < POS_FIGHTING) victim_ac +=victim->level/10;
  if (victim->position < POS_RESTING) victim_ac += victim->level;

  while ( ( diceroll = number_bits( 5 ) ) >= 20 );


  if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
  {
    damage( ch, victim, 0, dam_type+1000, dt, TRUE, FALSE, (obj)?obj:wield );
/*
    if (IS_CFG(victim,CFG_AUTODAM) && IS_IMMORTAL(victim))
      ptc(victim,"VF:Diceroll %d, Thac0 %d, AC %d, thac0-ac %d\n\r",diceroll, thac0, victim_ac, thac0-victim_ac);
    if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch))
      ptc(ch,"CF:Diceroll %d, Thac0 %d, AC %d, thac0-ac %d\n\r",diceroll, thac0, victim_ac, thac0-victim_ac);
*/
    return;
  }
/*
  if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch))
    ptc(ch,"CP:Diceroll %d, Thac0 %d, AC %d, thac0-ac %d\n\r",diceroll, thac0, victim_ac, thac0-victim_ac);
  if (IS_CFG(victim,CFG_AUTODAM) && IS_IMMORTAL(victim))
     ptc(victim,"VP:Diceroll %d, Thac0 %d, AC %d, thac0-ac %d\n\r",diceroll, thac0, victim_ac, thac0-victim_ac);
*/
   if ( diceroll !=19 && dt!=gsn_backstab && dt!=gsn_missile)
  {
    if ( check_shield_block(ch, victim, (obj)?obj:wield)) return;
    if ( check_parry(ch, victim, wield)) return;
    if ( check_dodge(ch, victim, (obj)?obj:wield)) return;
    if ( check_blink(ch, victim, (obj)?obj:wield)) return;
  }

  if (IS_NPC(ch) && !wield)
  {
    dam = number_range( ch->level / 2, ch->level*3/2);
  } 
  else 
  {
    if (sn != -1) check_improve(ch,sn,TRUE,5);
    if (obj && (obj->item_type == ITEM_WEAPON && IS_WEAPON_STAT(obj, WEAPON_MISSILE))) wield=obj;
    if (wield!=NULL)
    {
      OBJ_DATA *shield;

      dam = dice((int)wield->value[1],(int)wield->value[2]) * skill/100;

      shield=get_eq_char(ch,WEAR_RHAND);
      if (!shield || !CAN_WEAR(shield,WEAR_SHIELD)) 
        shield=get_eq_char(ch,WEAR_LHAND);
      if (!shield || !CAN_WEAR(shield,WEAR_SHIELD)) dam = dam * 11/10;

      if (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS))
      {
        dam += dam / 10; 
      }
      if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
      {
        int percent;
        if ((percent = number_percent()) <= (skill / 8))
           dam = 2 * dam + (dam * 2 * percent / 100);
      }
    }
    else dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
  }
  if ((obj!=NULL) && (obj->item_type != ITEM_WEAPON)
   && (obj->item_type == ITEM_WEAPON && !IS_WEAPON_STAT(obj, WEAPON_MISSILE)))
   dam = number_range( 1, (obj->level/3)*skill/100) + obj->weight;

  if ( get_skill(ch,gsn_enhanced_damage) > 0 ) 
  { 
    diceroll = number_percent(); 
    if (diceroll <= get_skill(ch,gsn_enhanced_damage)) 
    { 
      check_improve(ch,gsn_enhanced_damage,TRUE,6); 
      dam += 2 * ( dam * diceroll/300); 
    } 
  } 
  if (!IS_AWAKE(victim)) dam *= 2;
  else if (victim->position < POS_FIGHTING) dam = dam * 3 / 2;
   
  if (dt==gsn_backstab && wield)
  {
    if ( wield->value[0] != 2 ) dam *= 2 + (ch->level / 10);
    else dam *= 2 + (ch->level / 8);
  
    if (IS_SET(race_table[ch->race].spec,SPEC_BACKSTAB)) dam+=dam/14;
    dam += (dam * (category_bonus(ch, OFFENCE)-category_bonus(victim,PROTECT))/20);
  }
/*
  if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch)) 
     {ptc (ch, "one_hit damage1: %d\n\r",dam);}
*/   
  if (IS_NPC(ch)) dam += number_range(ch->level/2,ch->level);
  else if (!IS_SET(global_cfg,CFG_NODR)) {dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;}

  if IS_SET(ch->act,PLR_ARMY) dam*=2;
  if ( dam <= 0 ) dam = 0;
/*
  if (!IS_NPC(ch) && get_skill(ch,gsn_death_blow) > 1 &&
      ch->level >= 20 && wield && wield->item_type == ITEM_WEAPON )
      {
        if (number_percent() < 0.2 * get_skill(ch,gsn_death_blow))
     {
      stc("{RТы наносишь удар с невероятной силой!{X\n\r",ch);
      ptc(victim,"{W%s {Rнаносит удар с невероятной силой!{X\n\r",ch->short_descr);
      dam =(dam*ch->level)/20;
      check_improve(ch,gsn_death_blow,TRUE,1);
     }
        else check_improve(ch,gsn_death_blow,FALSE,2);
      }

  if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch)) 
     {ptc (ch, "one_hit damage2: %d\n\r",dam);}
*/
  
  result=damage( ch, victim, dam, dam_type+1000, dt, TRUE, FALSE, (obj) ? obj : wield );

  if (IS_AFFECTED(victim,AFF_FIRESHIELD) && number_percent()>93)
  {
    act("$c1 обжигается об огненный щит.",ch,NULL,NULL,TO_ROOM);
    act("Ты обжигается об огненный щит, окружающий $C1.",ch,NULL,victim,TO_CHAR);
    damage( victim, ch, dam/10, 37, DAM_FIRE,TRUE, FALSE, NULL);
  }

//  Damaging objects: Buggy...
//    if( ch && ch->fighting) damage_eq_char(ch);

// handling weapon flag effects (poison, etc...)
// Handling weapon flag specifics (used in fight.c/one_hit() )
  
  if (result && wield != NULL) 
  {  
    int dam; 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON)) 
    { 
      int level; 
      AFFECT_DATA *poison, af; 
  
      if ((poison = affect_find(wield->affected,gsn_poison)) == NULL) level = wield->level; 
      else level = poison->level; 

      if( !IS_ELDER(ch) )
        if ( !saves_spell(level / 2,victim,DAM_POISON ) )
        { 
          stc("Ты чувствуешь что в твоих венах течет яд.\n\r", victim); 
          act("{y$c1{x отpавлен ядом {w$i2{x.",victim,wield,NULL,TO_ROOM); 
  
          af.where     = TO_AFFECTS; 
          af.type      = gsn_poison; 
          af.level     = level * 3/4; 
          af.duration  = level / 2; 
          af.location  = APPLY_STR; 
          af.modifier  = -1; 
          af.bitvector = AFF_POISON; 
          affect_join( victim, &af ); 
        } 
  
      /* weaken the poison if it's temporary */ 
      if (poison != NULL) 
      { 
        poison->level = UMAX(0,poison->level - 2); 
        poison->duration = UMAX(0,poison->duration - 1); 
        if (poison->level == 0 || poison->duration == 0) 
         act("Отpавление {w$i2{x слабеет.",ch,wield,NULL,TO_CHAR); 
      } 
    } 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC)) 
    { 
      dam = number_range(1, wield->level / 5 + 1); 
      doact("{w$i1{x вытягивает жизнь из {y$c2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF);
      doact("Ты чувствуешь как {w$i1{x вытягивает из тебя жизнь.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF);
      damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE,FALSE, NULL);
      if (!IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE)) ch->alignment = UMAX(-1000,ch->alignment - 1);
      ch->hit += dam/2;
// hp>max ne budet
      if (ch->hit>ch->max_hit) ch->hit=ch->max_hit;
    }

    else if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMP_MANA)) 
    { 
      dam = number_range(1, wield->level / 5 + 1); 
      victim->mana -= dam;
      doact("{w$i1{x вытягивает энергию из {y$c2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF);
      doact("Ты чувствуешь как {w$i1{x вытягивает из тебя энергию.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF);
      if (!IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE)) ch->alignment = UMAX(-1000,ch->alignment - 1);
      ch->mana += dam/2;
// mana>max ne budet
      if (ch->mana>ch->max_mana) ch->mana=ch->max_mana;
    }
    
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
    {
      dam = number_range(1,wield->level / 4 + 1);
      doact("{w$i1{x обжигает {y$c2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF);
      doact("{w$i1{x обжигает твое тело.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF);
      fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
      damage(ch,victim,dam,0,DAM_FIRE,FALSE,FALSE, NULL);
    } 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST)) 
    { 
      dam = number_range(1,wield->level / 6 + 2); 
      doact("{w$i1{x обмоpаживает {y$c4{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF); 
      doact("Холодное пpикосновение {w$i2{x окpужает тебя льдом.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF); 
      cold_effect(victim,wield->level/2,dam,TARGET_CHAR); 
      damage(ch,victim,dam,0,DAM_COLD,FALSE,FALSE, NULL); 
    } 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING)) 
    { 
      dam = number_range(1,wield->level/5 + 2);
      doact("{y$c1{x шокирован световым удаpом из {w$i2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF); 
      doact("Ты шокирован {w$i5{x.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF); 
      shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
      damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE,FALSE, NULL);
    } 
  } 

  tail_chain( ); 
} 


bool damage(CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dam_type,int dt,bool show, bool breath, OBJ_DATA *obj)
{ 
  char buf[MAX_STRING_LENGTH],buf1[MAX_STRING_LENGTH];
  OBJ_DATA *corpse; 
  OBJ_DATA *shield;
  bool immune=FALSE;
  bool dam_hit=FALSE;
  int number;
  int dam_temp=dam,str_bonus;

 if (dam >=0)
 {
  if (dam_type>=1000)
  {
    dam_type-=1000;
    dam_hit=TRUE;
  }
 
  if ( victim->position == POS_DEAD ) return FALSE; 
 
  if ( !IS_IMMORTAL(ch) )
  {
   if ( dam > 2000 && dt != gsn_backstab)
   { 
    dam = 1200;
    if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && obj)
    { 
      stc("Смертный не может держать такое оружие!.\n\r",ch); 
      obj_from_char(obj);
      extract_obj(obj); 
    } 
   } 
 
   if (!breath)
   { 
    if (!IS_SET(global_cfg,CFG_NODR))
    {
     if ( dam > 35) dam = (dam - 35)/2 + 35; 
     if ( dam > 80) dam = (dam - 80)/2 + 80;  
     if ( dt != gsn_backstab  &&  dam > 150 ) dam = (dam - 150)/2 + 150;
    }
   } 
   else
   {
    shield = get_eq_char(victim, WEAR_RHAND);
    if (shield && CAN_WEAR(shield, ITEM_WEAR_SHIELD)) dam = (dam * 4)/5;
        
    shield = get_eq_char(victim, WEAR_LHAND);
    if (shield && CAN_WEAR(shield, ITEM_WEAR_SHIELD)) dam = (dam * 4)/5;
   }
  }
 }  
 
  if (victim && ch && victim!=ch && !IS_NPC(victim) && !IS_NPC(ch))
  {
    if (victim->pcdata->condition[COND_ADRENOLIN] == 0) 
      stc("{rАдреналин бушует у тебя в крови!{x\n\r",victim); 
    if (ch->pcdata->condition[COND_ADRENOLIN] == 0) 
      stc("{rАдреналин бушует у тебя в крови!{x\n\r",ch); 
    add_pkiller(victim,ch); 
    victim->pcdata->condition[COND_ADRENOLIN] = 2; 
    ch->pcdata->condition[COND_ADRENOLIN]=2;
  } 

  if ( victim != ch )
  { 
    // Certain attacks are forbidden. Most other attacks are returned.
    if ( is_safe( ch, victim ) ) return FALSE; 
    if (!IS_NPC(ch) && !IS_NPC(victim)) check_criminal(ch,victim,60); 

    if ( (ch->race==RACE_ZOMBIE) && ( (ch->position != POS_FIGHTING) || (victim->position != POS_FIGHTING) ) )
    {
      DAZE_STATE(victim,number_range(1,3) * PULSE_VIOLENCE); 
    }
    if ( (victim->race==RACE_ZOMBIE) && ( (ch->position != POS_FIGHTING) || (victim->position != POS_FIGHTING) ) )
    {
      DAZE_STATE(ch,number_range(1,3) * PULSE_VIOLENCE); 
    }
    if ( victim->position > POS_STUNNED ) 
    { 
      if (victim->fighting == NULL ) set_fighting( victim, ch ); 
      if (victim->timer <= 4) victim->position = POS_FIGHTING; 
    } 
 
    if ( victim->position > POS_STUNNED && ch->fighting == NULL ) set_fighting( ch, victim ); 
    // More charm stuff.
    if ( victim->master == ch ) stop_follower( victim ); 
  } 
  // Inviso attacks ... not.
//  if (ch->race!=RACE_SPRITE)  // even for sprites...
  { 
    if ( IS_AFFECTED(ch, AFF_INVISIBLE) || 
         IS_AFFECTED(ch, AFF_SNEAK)) 
    { 
      affect_strip( ch, gsn_sneak ); 
      affect_strip( ch, gsn_invis ); 
      affect_strip( ch, gsn_mass_invis ); 
      REM_BIT( ch->affected_by, AFF_INVISIBLE ); 
      REM_BIT( ch->affected_by, AFF_SNEAK );
      act( "{y$c1{x появляется из ниоткуда.", ch, NULL, NULL, TO_ROOM ); 
    } 
  } 

  if (IS_AFFECTED(ch,AFF_MIST))
  {
    stc ("Желтоватый туман, окружающий тебя исчезает.\n\r",ch);
    REM_BIT(ch->affected_by,AFF_MIST);
  }

  if (IS_AFFECTED(ch,AFF_HIDE)) 
  { 
   affect_strip( ch, gsn_hide); 
   REM_BIT(ch->affected_by,AFF_HIDE); 
   stc("Ты больше не спрятан.\n\r",ch); 
  } 

  //some immune check. 
  //didn't place returns only 'cause immune is needed later (c) Apc
  immune = FALSE; 
  if ( check_immune(victim,dt,obj) == IS_IMMUNE ) immune=TRUE;
  if (IS_NPC(victim) 
   && (
         victim->pIndexData->pShop 
      || victim->pIndexData->vnum == 3011
      )
     ) 
  immune=TRUE;

  //Weapons with magic damtypes deal 0 damage
  if (dam_hit && !IS_NPC(ch) && dt>3 && dt != gsn_backstab && dt!=gsn_missile) dam = 0;


  if (!immune && dam > 0)
  { 
    // Damage modifiers.
    dam+=dam*get_ac_modifier(ch,victim,dt)/100;

    // Balance purposes only
    if (dt<=3) dam=dam*9/10;
    else dam*=21/20;

    if (!IS_NPC(victim))
    {
      if (victim->pcdata->condition[COND_DRUNK]  > 10 ) dam = 9 * dam / 10; 
    }
 
    if ((dt<=3 || dt==gsn_backstab) && IS_AFFECTED(victim, AFF_SANCTUARY))
    {
      if (IS_ORDEN(victim)) dam -= dam*6/10;
      else dam /= 2;
    }
    if (dt>3 && IS_AFFECTED(victim, AFF_SHIELD))
    {
      if (IS_ORDEN(victim)) dam -= dam*6/10;
      else dam /= 2;
    }
    if (dt>3 && is_affected(victim,skill_lookup("vbite"))) dam +=dam/10;

    if (((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch) && !BAD_ORDEN(ch) )
    || (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) && !BAD_VAMP(ch)))) 
      dam -= dam / 4; 
 
    if (!breath) dam+=dam*get_vuln_modifier(ch,victim,dt)/100;
    else         dam+=dam*get_vuln_modifier(ch,victim,dt)/200;

    if (IS_AFFECTED(victim, AFF_FIRESHIELD) &&
    (dam_type==DAM_COLD || dam_type==DAM_FIRE)) dam+=dam*20/100;

    if (obj)
    {
      dam+=dam/100*get_material_modifier(victim,obj);

      if (material_table[material_num(obj->material)].hard<100)
       dam-=dam/200*material_table[material_num(obj->material)].hard;
      if (material_table[material_num(obj->material)].hard>100)
       dam+=dam/200*material_table[material_num(obj->material)].hard;
    }
    if (dam<=0) dam=0; 

    // prayed clerics have a kind of resist
    if( (victim->classcle==1) && is_affected(victim,skill_lookup("pray")))
    {
      if (dam_hit) dam-=dam*(3+victim->level/10)/100; //slash-pierce-bash
      else dam-=dam*victim->level/1200;               //fire-holy-energy etc.
    }
  
    // strength & con bonus
    if (dt!=gsn_backstab && dam_hit)
    { 
      if (IS_NPC(victim))  str_bonus=get_curr_stat(ch,STAT_STR)-23;
      else                 str_bonus=get_curr_stat(ch,STAT_STR)-get_curr_stat(victim,STAT_CON);
      dam=UMAX(0,dam+dam*str_bonus/20);
    }

    // race size modifier in damage calculation
    if (dt!=gsn_backstab && dam_hit) 
      dam += (dam * (ch->size - victim->size)) / 25;   

    if (dam_hit) dam = (dam * (200 - 3*category_bonus(victim,FORTITUDE)))/200;

    // weapon bonus
    if ( dam_hit && obj && obj->item_type==ITEM_WEAPON )
      dam+=dam*race_table[ch->race].weapon_bonus[obj->value[0]]/100;

    // Aligment modifier..
    if (obj!=NULL)
    {
      if ((IS_OBJ_STAT(obj, ITEM_BLESS) && IS_EVIL(ch)) ||
        (IS_OBJ_STAT(obj, ITEM_EVIL ) && IS_GOOD(ch))) dam -=(dam/20);

      if ((IS_OBJ_STAT(obj, ITEM_BLESS) && IS_GOOD(ch)) ||
        (IS_OBJ_STAT(obj, ITEM_EVIL ) && IS_EVIL(ch))) dam +=(dam/20);

      if ((IS_OBJ_STAT(obj, ITEM_BLESS) && IS_EVIL(victim)) ||
       (IS_OBJ_STAT(obj, ITEM_EVIL ) && IS_GOOD(victim))) dam +=(dam/20);
    }

    if ( dt==DAM_HARM ) dam=dam_temp;
  }
 
  if (dam<=0) dam=0; 

  if (show)
  {
    if (dt==gsn_backstab || dt==gsn_missile)
      dam_message( ch, victim, dam, dt, immune, obj);
    else
      dam_message( ch, victim, dam, dam_hit?dam_type+1000:dam_type, immune, obj);
  }
  if (immune || dam == 0) return FALSE;
  
  
  // updating the damage indicator 
  if (!IS_NPC(ch)) 
  { 
   number = check_victim(ch,victim); 
   if (number < MAX_VICT) ch->pcdata->victims[number].dampool+=dam; 
  } 
  else if (IS_AFFECTED(ch, AFF_CHARM)) 
  { 
   if (ch->master && ch->master->pet==ch && !IS_NPC(ch->master))
   { 
    number = check_victim(ch->master,victim); 
    if (number < MAX_VICT) ch->master->pcdata->victims[number].dampool+=dam*2/3; 
   } 
   else if (ch->master && !IS_NPC(ch->master))
   { 
    number = check_victim(ch->master,victim); 
    if (number < MAX_VICT) ch->master->pcdata->victims[number].dampool+=dam/3; 
   } 
  }

  // Hurt the victim. Inform the victim of his new state.
  victim->hit -= dam; 
  if (victim->hit < -15) victim->hit = -12;
  update_pos( victim ); 
 
  switch( victim->position ) 
  { 
    case POS_MORTAL: 
      act( "{y$c1{x cмеpтельно pанен, и вскоpе умpет, если ему не помогут.",victim, NULL, NULL, TO_ROOM ); 
      stc( "Ты {rсмеpтельно pанен{x, и вскоpе умpешь, если тебе не помогут.\n\r",victim ); 
      break; 
 
    case POS_INCAP: 
      act( "{y$c1{x {mне может двигаться {x и вскоре умpет, если ему никто не поможет.",victim, NULL, NULL, TO_ROOM ); 
      stc("Ты {mне можешь двигаться{x и скоро умpешь, если тебе не помогут.\n\r",victim ); 
      break; 
 
    case POS_STUNNED: 
      act( "{y$c1{x {mоглушен{x, но еще может встать.",victim, NULL, NULL, TO_ROOM ); 
      stc("Ты {mоглушен{x, но еще можешь встать.\n\r",victim ); 
      break; 
 
    case POS_DEAD: 
      act( "{y$c1{x {rУБИТ!!{x", victim, 0, 0, TO_ROOM ); 
      stc( "Ты {rУБИТ!!{x\n\r\n\r", victim ); 
      if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH) ) 
      { 
        victim->position = POS_STANDING; 
        mp_percent_trigger( victim, ch, NULL, NULL, TRIG_DEATH ); 
        if (victim) victim->position=POS_DEAD;
        else return 0;
      }
      break; 
 
    default: 
      if ( dam > victim->max_hit / 4 ) 
        stc( "{rЭто было ДЕЙСТВИТЕЛЬНО БОЛЬНО!{x\n\r", victim ); 
      if ( victim->hit < victim->max_hit / 4 ) 
        stc( "Ты истекаешь {rКРОВЬЮ!{x\n\r", victim ); 
      break; 
  } 
 
  if (!IS_NPC(ch) && !IS_NPC(victim)) check_criminal( ch, victim , 60); 
  // Sleep spells and extremely wounded folks.
  if (!IS_AWAKE(victim) ) stop_fighting( victim, FALSE ); 
 
  // Payoff for killing things.
  if (victim->position == POS_DEAD)
  {
    group_gain( ch, victim );

    if (!IS_NPC(victim) && !IS_SET(victim->act, PLR_ARMY)
        && !IS_SET(victim->in_room->room_flags,ROOM_ARENA)
        && victim->remort!=0)
    {
      if (!IS_NPC(ch)) victim->pcdata->deathcounter+=5;
      else if ( (ch->level-victim->level)<20) victim->pcdata->deathcounter+=1;

      if (victim->pcdata->deathcounter>=15) 
      { 
         if (victim->remort) 
         { 
           stc("{R{*Ты умиpал слишком часто, твое здоpовье ухудшается!{x\n\r", victim); 
           victim->max_hit-=10; 
         } 
         victim->pcdata->deathcounter=0; 
         WILLSAVE(ch);
      } 
    } 
 
    if (!IS_NPC(victim)) 
    { 
      if (IS_SET(ch->in_room->room_flags, ROOM_ARENA) && !IS_NPC(ch))
      {
        victim->death_pc_arena++;
        ch->vic_pc_arena++;
        ch->pcdata->condition[COND_ADRENOLIN] = 0;
        victim->pcdata->condition[COND_ADRENOLIN] = 0;
        do_printf(buf,"В жестокой схватке %s {Y%s{x(lvl.%d) победил на арене ",clasname(ch),ch->name,ch->level);
        do_printf(buf1,"%s%s {Y%s{x(lvl.%d)",buf,clasname(victim),victim->name,victim->level);
        send_news(buf1,NEWS_VICTORY);
      }
      else 
      {
        if (ch!=victim && !IS_NPC(ch))
        {
          do_printf(buf,"Ария: [{Y%s{x], %s {Y%s{x(lvl.%d) убил ",ch->in_room->area->name,clasname(ch),ch->name,ch->level);
          do_printf(buf1,"%s%s {Y%s{x(lvl.%d)",buf,clasname(victim),victim->name,victim->level);
          strcat(buf1,(victim->desc !=NULL || victim->pcdata->condition[COND_ADRENOLIN] > 0)?"":" {RLOSTLINK KILL{x");
          send_news(buf1,NEWS_MURDER);
        }
        else if (ch!=victim && ch->master && !IS_NPC(ch->master))
        {
          do_printf(buf,"Ария: [{Y%s{x], %s {Y%s{x(lvl.%d) убил ",ch->in_room->area->name,clasname(ch->master),ch->master->name,ch->master->level);
          do_printf(buf1,"%s%s {Y%s{x(lvl.%d) с помощью своих подручных",
          buf,clasname(victim),victim->name,victim->level);
          strcat(buf1,(victim->desc !=NULL || victim->pcdata->condition[COND_ADRENOLIN] > 0)?"":" {RLOSTLINK KILL{x");
          send_news(buf1,NEWS_MURDER);
        }  
      } 
      
      if (!IS_NPC(ch)) do_ear(victim,ch);
//      else if (ch->master)  do_ear(victim,ch->master); 
     
      // bounty
      if (victim->pcdata->bounty!=0 && ch!=victim && (!IS_SET(victim->act,PLR_ARMY))) 
       { 
        if (ch->master && !IS_NPC(ch->master))
        {
         ptc(ch->master,"Ты получаешь {Y%u{x золотых монет за убийство %s!\n\r",victim->pcdata->bounty,victim->name); 
         ch->master->gold+=victim->pcdata->bounty; 
         victim->pcdata->bounty=0; 
         do_printf(buf,"{CНаграда за голову {Y%s{C выплачена.",victim->name); 
         info (NULL,1,4,buf,"{x"); 
        }
        else if (!IS_NPC(ch))
        {
         ptc(ch,"Ты получаешь {Y%u{x золотых монет за убийство %s!\n\r",victim->pcdata->bounty,victim->name); 
         ch->gold+=victim->pcdata->bounty; 
         victim->pcdata->bounty=0; 
         do_printf(buf,"{CНаграда за голову {Y%s{C выплачена.",victim->name); 
         info (NULL,1,4,buf,"{x"); 
        }
      } 
    } 
 
    do_printf(log_buf, "В %s [%u], %s убил ",ch->in_room->name,ch->in_room->vnum,
    get_char_desc(ch,'1'));
    strcat(log_buf,get_char_desc(victim,'4'));
    if (!IS_NPC(victim)) strcat(log_buf,(victim->desc !=NULL)?"":" !!! LOSTLINK KILL !!!");
    log_string( log_buf );

    if (IS_NPC(victim)) wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0); 
    else wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0); 

    // Dying penalty: 2/3 way back to previous level.
    // 1/5 of exp_per_level, if exp<=exp_per_level*level.
    if (!IS_NPC(victim) && !IS_SET(victim->act,PLR_ARMY) && !IS_SET(race_table[victim->race].spec,SPEC_NOLOSTEXP))
    { 
      if (victim->exp > exp_per_level(victim,victim->pcdata->points) * victim->level) 
       gain_exp(victim,(2 * (exp_per_level(victim,victim->pcdata->points) * victim->level - victim->exp)/3)+50); 
    }      
 
    // statistics for killing
    if (ch!=victim)
    {
      if (!IS_NPC(ch) && IS_NPC(victim)) ch->vic_npc++;
      else if (IS_NPC(ch) && IS_NPC(victim)) ch->vic_npc++;
      else if (IS_NPC(ch) && !IS_NPC(victim))
      {
        ch->vic_pc_total++;
        victim->death_npc++;
      }
      else if (!IS_NPC(ch) && !IS_NPC(victim))
      {
        if( IS_DEVOTED_ANY(ch) && IS_DEVOTED_ANY(victim) 
         && ( ch->pcdata->dn == victim->pcdata->dn ) 
         && !IS_SET(ch->in_room->room_flags,ROOM_ARENA) )
              change_favour( ch, -50);
        ch->vic_pc_total++;
        victim->death_pc_total++;
      }
    } 
     
    raw_kill( victim ); 
    if (IS_CFG(ch,CFG_AUTOLOOK)) do_look(ch,"in corpse");
    // dump the flags

    if (ch != victim && victim->criminal>0)
    { 
     if (!IS_NPC(ch)
     || ch->spec_fun==spec_lookup("spec_executioner"))
      victim->criminal-=200; 
      else victim->criminal-=50; 

     if (victim->criminal<0) 
      { 
       victim->criminal=0; 
       REM_BIT(victim->act,PLR_WANTED);
       stc("{YТы больше не в розыске.{x\n\r",victim);
      } 
    } 
    // RT new auto commands
    if (!IS_NPC(ch) 
      &&  (corpse = get_obj_list(ch,"corpse",ch->in_room->contents)) != NULL 
      &&  corpse->item_type == ITEM_CORPSE_NPC && can_see_obj(ch,corpse)) 
    { 
      OBJ_DATA *coins; 
 
      corpse = get_obj_list( ch, "corpse", ch->in_room->contents );  
 
      // exists and not empty
      if ( IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains) 
      { 
        do_function(ch, &do_get, "all corpse"); 
      } 
 
      if (IS_SET(ch->act,PLR_AUTOGOLD) && corpse && corpse->contains &&  
           !IS_SET(ch->act,PLR_AUTOLOOT)) 
      { 
        if ((coins = get_obj_list(ch,"gcash",corpse->contains))!= NULL) 
        { 
          do_function(ch, &do_get, "all.gcash corpse"); 
        } 
      } 
             
      if (IS_SET(ch->act, PLR_AUTOSAC)) 
      { 
        if (IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains) 
         return TRUE;  /* leave if corpse has treasure */ 
        else do_function(ch, &do_sacrifice, "corpse"); 
      } 
      WILLSAVE(ch);
      return TRUE; 
    } 
  } 
 
  if ( victim == ch ) return TRUE; 
 
  /* Take care of link dead people. */ 
  if ( !IS_NPC(victim) && victim->desc == NULL ) 
  { 
    if ( number_range( 0, victim->wait ) == 0 && IS_CFG(victim, CFG_AUTORECAL))
    { 
      do_function(victim, &do_recall, "auto" ); 
      WILLSAVE(ch);
      return TRUE; 
    } 
  } 
 
  /* Wimp out? */ 
  if ( IS_NPC(victim)) 
  { 
    if (IS_AFFECTED(victim, AFF_CHARM) && victim->master!=NULL 
     && victim->master->in_room != victim->in_room) 
     do_function(victim, &do_flee, "" ); 
    else if ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0 
     && victim->hit < victim->max_hit / 5 
     && victim->wait < PULSE_VIOLENCE / 2) do_function(victim, &do_flee, "" ); 
  } 
 
  if ( !IS_NPC(victim) &&  victim->hit > 0 
   &&   victim->hit <= victim->wimpy 
   &&   victim->wait < PULSE_VIOLENCE / 2 ) 
  { 
    do_function (victim, &do_flee, "" );
    if ( number_percent() < 33 ) victim->wimpy = 0;
    if ( !IS_IMMORTAL(victim) 
     &&  !IS_AFFECTED(victim,AFF_BERSERK) 
     &&  number_percent() < 33 )
    {
       AFFECT_DATA af;
       af.where     = TO_AFFECTS;
       af.type      = skill_lookup("fear");
       af.level     = victim->level;
       af.duration  = 0;                    
       af.location  = APPLY_NONE;             
       af.modifier  = 0;
       af.bitvector = 0;
       affect_to_char( victim, &af );
       stc("Ты паникуешь!\n\r",victim);
    }
  } 
  tail_chain( ); 
  return TRUE; 
} 

int get_ac_modifier(CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
  int ac;
  if (/*IS_NPC(victim) ||*/ dt>3 || dt == gsn_backstab) return 0;
    
  switch(dt)
  {
    case DAM_PIERCE: ac=GET_AC(victim,AC_PIERCE); break;
    case DAM_SLASH : ac=GET_AC(victim,AC_SLASH ); break;
    case DAM_BASH  : ac=GET_AC(victim,AC_BASH  ); break;
    default        : ac=GET_AC(victim,AC_EXOTIC); break;
  }
//old
/*  if ( victim->level <= 10 )      return URANGE(-10,ac/10,10);
  else if ( victim->level <= 20 ) return URANGE(-11,ac/12,10);
  else if ( victim->level <= 30 ) return URANGE(-12,(ac+120)/18,11);
  else if ( victim->level <= 40 ) return URANGE(-13,(ac+150)/20,12);
  else if ( victim->level <= 50 ) return URANGE(-13,(ac+200)/20,14);
  else if ( victim->level <= 60 ) return URANGE(-14,(ac+250)/20,15);
  else if ( victim->level <= 70 ) return URANGE(-15,(ac+280)/20,15);
  else if ( victim->level <= 80 ) return URANGE(-15,(ac+300)/20,16);
  else if ( victim->level <= 88 ) return URANGE(-15,(ac+320)/22,17);
  else if ( victim->level <= 95 ) return URANGE(-15,(ac+600)/25,18);
  return URANGE(-17,(ac+1200)/25,25);
*/
//new
  if ( victim->level <= 10 )      return URANGE(-19,ac/10,10);
  else if ( victim->level <= 20 ) return URANGE(-20,ac/12,10);
  else if ( victim->level <= 30 ) return URANGE(-21,(ac+120)/18,11);
  else if ( victim->level <= 40 ) return URANGE(-22,(ac+150)/20,12);
  else if ( victim->level <= 50 ) return URANGE(-22,(ac+200)/20,14);
  else if ( victim->level <= 60 ) return URANGE(-23,(ac+250)/20,15);
  else if ( victim->level <= 70 ) return URANGE(-24,(ac+280)/20,15);
  else if ( victim->level <= 80 ) return URANGE(-24,(ac+300)/20,16);
  else if ( victim->level <= 88 ) return URANGE(-24,(ac+320)/25,17);
  else if ( victim->level <= 95 ) return URANGE(-25,(ac+600)/40,18);
  return URANGE(-25,(ac+1000)/50,20);
}

int get_vuln_modifier ( CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
  if (!dt || dt>=DAM_MAX) return 0;
  return race_table[victim->race].dambonus[dt];
}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim) 
{ 
  if (victim->in_room == NULL || ch->in_room == NULL) return TRUE; 

  if (!IS_NPC(ch) && !IS_NPC(victim) 
      && victim->pcdata->condition[COND_ADRENOLIN] <= 0
      && victim->desc==NULL
     )
  { 
    stc("Он в лостлинке!\n\r",ch); 
    return TRUE; 
  } 
  
/* remmed out - not used (c) Apc
  if (IS_AFFECTED( ch, AFF_GASEOUS_FORM ))
  {
    stc("Ты в неподходящей для нападения форме...\n\r",ch);
    return TRUE;
  }
  if (IS_AFFECTED( victim, AFF_GASEOUS_FORM )) 
  {
    stc("Ты вдруг понимаешь, что ганяешься за призраком...\n\r",ch);
    return TRUE;
  }
  if ( IS_SET(ch->in_room->ra,RAFF_VIOLENCE) ) return FALSE; 
*/
 
  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WANTED)) return FALSE;
  
  if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) && 
     (IS_NPC(victim) || victim->pcdata->condition[COND_ADRENOLIN]<=0))
  { 
    stc("Hе в этой комнате.\n\r",ch); 
    return TRUE; 
  } 
 
  if (victim->fighting == ch || victim == ch) return FALSE; 
 
  // killing mobiles
  if (IS_NPC(victim)) 
  { 
   if (IS_SET(victim->in_room->room_flags,ROOM_SAFE)
       || IS_SET(victim->in_room->ra, RAFF_SAFE_PLC))
    { 
      stc("Hе в этой комнате.\n\r",ch); 
      return TRUE; 
    } 
 
    // no killing healers, trainers, etc
    if (IS_SET(victim->act,ACT_TRAIN) 
     || IS_SET(victim->act,ACT_PRACTICE) 
     || IS_SET(victim->act,ACT_IS_HEALER) 
     || IS_SET(victim->act,ACT_IS_CHANGER)) 
    { 
      stc("Я думаю БОГИ этого не одобpят.\n\r",ch); 
      return TRUE; 
    } 

    // attacking someone's pet is equvalent to attacking it's master
    if (IS_SET(victim->act,ACT_PET) && victim->master != NULL
       && !PK_RANGE(ch,victim->master) && !IS_NPC(ch))
    {
      if (ch->pcdata->pkillers!=NULL && is_exact_name(victim->master->name,ch->pcdata->pkillers)) return FALSE;
      stc("Выбеpи кого-то себе по силам.\n\r",ch);
      return TRUE;
    }
  } 
  /* killing players */ 
  else 
  { 
    /* NPC doing the killing */ 
    if (IS_NPC(ch)) 
    { 
      if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
       if (IS_NPC(victim) || victim->pcdata->condition[COND_ADRENOLIN]<=0) 
      { 
//        stc("Hе в этой комнате.\n\r",ch); 
        return TRUE; 
      } 
 
      /* charmed mobs and pets cannot attack players while owned */ 
      if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL 
       &&  ch->master->fighting != victim) 
      { 
//        stc("Игpоки твои дpузья!\n\r",ch);  // idiotizm?
        return TRUE; 
      } 
    } 
    /* player doing the killing */ 
    else 
    { 
      if (IS_SET(ch->in_room->room_flags,ROOM_ARENA) && IS_CFG(ch,CFG_ZRITEL))
      { 
        stc("Ты сейчас Зритель.\n\r",ch); 
        return TRUE; 
      } 
 
      if (IS_SET(victim->in_room->room_flags,ROOM_ARENA) && IS_CFG(victim,CFG_ZRITEL))
      { 
        stc("Этот персонаж всего лишь зевака на арене.\n\r",ch); 
        return TRUE; 
      } 
 
      if (ch->clan==NULL && !IS_SET(ch->in_room->room_flags,ROOM_ARENA)) 
      { 
        stc("Вступи в клан если ты хочешь убивать игpоков.\n\r",ch); 
        return TRUE; 
      } 
      if (victim->clan==NULL && !IS_SET(ch->in_room->room_flags,ROOM_ARENA)) 
      { 
        stc("Этот персонаж не в клане.\n\r",ch); 
        return TRUE; 
      } 

      if (IS_SET(victim->act,PLR_WANTED)) return FALSE;
      if (!PK_RANGE(ch,victim)) 
      { 
        if (ch->pcdata->pkillers!=NULL && is_exact_name(victim->name,ch->pcdata->pkillers)) return FALSE;
        if (IS_SET(victim->act,PLR_RAPER) && IS_MARRY(ch,victim)) return FALSE;
        stc("Выбеpи кого-то себе по силам.\n\r",ch); 
        return TRUE; 
      } 
    } 
  } 
  return FALSE; 
} 
  
bool is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area ) 
{ 
  if (!victim->in_room || !ch->in_room ) return TRUE;

/*
  if (IS_AFFECTED( ch, AFF_GASEOUS_FORM ))
  {
    stc("Ты в неподходящей для нападения форме...\n\r",ch);
    return TRUE;
  }
  if (IS_AFFECTED( victim, AFF_GASEOUS_FORM))
  {
    stc("Ты вдруг понимаешь, что ганяешься за призраком...\n\r",ch);
    return TRUE;
  }
  if (IS_AFFECTED(victim,AFF_MAGIC_BAR))
  {
    act("$t защищает $C2 от магии.",ch,SabAdron,victim,TO_CHAR);
    return TRUE;
  }
*/
  if ((victim == ch || is_same_group(ch, victim)) && area) return TRUE;
  if (victim->fighting == ch || victim == ch) return FALSE;
  if (IS_SET(ch->in_room->ra, RAFF_VIOLENCE) ) return FALSE;
  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WANTED)) return FALSE;
  if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL && !area) return FALSE; 
  if (IS_SET(victim->in_room->room_flags,ROOM_SAFE) &&
       (IS_NPC(victim) || victim->pcdata->condition[COND_ADRENOLIN]<=0)) return TRUE;      
  if (IS_SET(victim->in_room->ra,RAFF_SAFE_PLC)) return TRUE;
  if (IS_SET(ch->in_room->room_flags,ROOM_ARENA) && IS_CFG(ch,CFG_ZRITEL)) return TRUE; 
  if (IS_SET(victim->in_room->room_flags,ROOM_ARENA) && IS_CFG(victim,CFG_ZRITEL)) return TRUE; 

  if (!IS_NPC(ch) && !IS_NPC(victim) 
      && victim->pcdata->condition[COND_ADRENOLIN] <= 0
      && victim->desc==NULL
     )
  { 
    stc("Он в лостлинке!\n\r",ch); 
    return TRUE; 
  } 

  if (IS_NPC(victim)) 
  { 
    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE)) return TRUE; 
 
    if (IS_SET(victim->act,ACT_TRAIN) || 
        IS_SET(victim->act,ACT_PRACTICE) || 
        IS_SET(victim->act,ACT_IS_HEALER) || 
        IS_SET(victim->act,ACT_IS_CHANGER)) 
     return TRUE; 
 
  } 
  else 
  { 
    if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL) return TRUE; 
 
    if (IS_NPC(ch)) 
    { 
      if (IS_AFFECTED(ch,AFF_CHARM) && ch->master
          && ch->master->fighting != victim) return TRUE; 
       
      if (IS_SET(victim->in_room->room_flags,ROOM_SAFE)) return TRUE; 
    } 
    else 
    { 
      if (!ch->clan|| !victim->clan) return TRUE; 
 
      if (IS_SET(victim->act,PLR_WANTED)) return FALSE; 
 
      if (!PK_RANGE(ch,victim)) 
      { 
        if (ch->pcdata->pkillers!=NULL && is_exact_name(victim->name,ch->pcdata->pkillers)) return FALSE; 
        if (IS_SET(victim->act,PLR_RAPER) && IS_MARRY(ch,victim)) return FALSE;
        stc("Выбеpи кого-то себе по силам.\n\r",ch); 
        return TRUE; 
      } 
    } 
  } 
  return FALSE; 
} 
 
void check_criminal( CHAR_DATA *ch, CHAR_DATA *victim, int level )
{
  if (!ch || !victim || ch==victim 
      || IS_NPC(ch)  || IS_NPC(victim) 
      || IS_SET(victim->act, PLR_WANTED)
     ) 
   return;
  if (ch->in_room && IS_SET(ch->in_room->room_flags, ROOM_ARENA)) return;
  while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master) victim = victim->master;
  while ( IS_AFFECTED(ch, AFF_CHARM) && ch->master) ch= ch->master;


  if ( victim->pcdata->condition[COND_ADRENOLIN]>0 
        && ch->pcdata->condition[COND_ADRENOLIN]>0) return;

  if ( victim->pcdata->condition[COND_ADRENOLIN]<1)
  {
    victim->pcdata->condition[COND_ADRENOLIN] = 2;
    ch->pcdata->condition[COND_ADRENOLIN]=3;
//    if( IS_GOOD_DEITY(ch) ) change_favour(ch, -40);
//    if( IS_EVIL_DEITY(ch) ) change_favour(ch, 40);
//    if( IS_FANATIC_DEITY(ch) ) change_favour(ch, 10);
    stc("{rАдреналин бушует у тебя в крови!{x\n\r",victim);
    stc("{rАдреналин бушует у тебя в крови!{x\n\r",ch);
  }
  else
  {
    victim->pcdata->condition[COND_ADRENOLIN] = 2;
    ch->pcdata->condition[COND_ADRENOLIN]=3;
  }

  if ( victim->criminal>50) return;

  if ( !IS_SET(victim->in_room->room_flags,ROOM_LAW) && ch->clan && victim->clan && is_exact_name(ch->clan->name, victim->clan->war)) return;
  if ( is_same_clan(ch,victim)) return;

  if ( IS_SET(ch->affected_by, AFF_CHARM) )
  {
    if ( ch->master == NULL )
    {
       char buf[MAX_STRING_LENGTH];

       do_printf( buf, "Killer check: %s bad AFF_CHARM",
                IS_NPC(ch) ? get_char_desc(ch,'1') : ch->name );
       bug( buf, 0 );       affect_strip( ch, gsn_charm_person );
       REM_BIT( ch->affected_by, AFF_CHARM );
       return;
    }
    stop_follower( ch );
    return;
  }

  if ( IS_NPC(ch) || ch == victim || ch->level >= LEVEL_IMMORTAL
    || ch->clan==NULL || ch->fighting == victim
    || IS_SET(ch->in_room->room_flags,ROOM_ARENA)) return;
 
  ch->criminal+=level; 
  if (ch->criminal>1000) ch->criminal=1000; 
  WILLSAVE(ch); 
} 
 
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ) 
{ 
  OBJ_DATA *wrh; 
  OBJ_DATA *wlh; 
  int chance, chance_lh;
 
  if ( !IS_AWAKE(victim) ) return FALSE; 
  if (obj && obj->value[0] == WEAPON_WHIP) return FALSE;
  if (victim->position <= POS_RESTING) return FALSE;
 
  chance = get_skill(victim,gsn_parry); 

  if (chance==0) return FALSE;
  chance/=2;

  chance_lh = (get_skill(victim,gsn_parry) / 10) * (1+get_skill(victim,gsn_dual)/100); 
 
  wrh=get_eq_char( victim, WEAR_RHAND ); 
  wlh=get_eq_char( victim, WEAR_LHAND ); 
  if ( wrh == NULL && wlh==NULL ) 
  { 
    if (IS_NPC(victim)) chance /= 2; 
    else return FALSE; 
  } 
 
  if (wrh && wrh->item_type==ITEM_WEAPON && wrh->value[0] == WEAPON_WHIP) chance = 0; 
  if (wlh && wlh->item_type==ITEM_WEAPON && wlh->value[0] != WEAPON_WHIP) chance+=chance_lh; 
  if (chance == 0) return FALSE;

  if ((can_see(ch,victim,CHECK_LVL))&&(!can_see(victim,ch,CHECK_LVL))) chance /= 2;
  chance+=(get_curr_stat(victim,STAT_DEX)-get_curr_stat(ch,STAT_DEX)) *3/2;

  chance-=str_app[get_curr_stat(ch,STAT_STR)].tohit*2;
  chance+=str_app[get_curr_stat(victim,STAT_STR)].tohit;
  
  chance += victim->level + 3*(category_bonus(victim,PROTECT) -category_bonus(ch,OFFENCE))- ch->level;
  if (obj && IS_WEAPON_STAT(obj, WEAPON_VORPAL)) chance = chance * 4/5;
  if (is_affected(victim,skill_lookup("ensnare"))) chance -=5;
  if (is_affected(victim,skill_lookup("slow"))) chance -=5;

  if ( number_percent( ) >= chance) return FALSE;
  doact( "Ты {wпаpиpуешь{x атаку {y$c2{x.",  ch, NULL, victim, TO_VICT,SPAM_MISS);
  doact( "{y$C1{x {wпаpиpует{x твою атаку.", ch, NULL, victim, TO_CHAR,SPAM_MISS);
  check_improve(victim,gsn_parry,TRUE,6);
//  if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 1);
  return TRUE;
}

bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj)
{
  int chance,col=0;
  OBJ_DATA *sh1;
  OBJ_DATA *sh2;
  char buf1[MAX_INPUT_LENGTH];
  char buf2[MAX_INPUT_LENGTH];

  if ( !IS_AWAKE(victim) ) return FALSE;
  if (victim->position < POS_RESTING) return FALSE;

  if ( (chance = get_skill(victim,gsn_shield_block))==0) return FALSE;
  chance=chance/4+5;

  sh1=get_eq_char( victim, WEAR_RHAND );
  if (sh1!=NULL && CAN_WEAR (sh1, ITEM_WEAR_SHIELD)) col++;
  else sh1=NULL;

  sh2=get_eq_char( victim, WEAR_LHAND );
  if (sh2!=NULL && CAN_WEAR (sh2, ITEM_WEAR_SHIELD)) col++;
  else sh2=NULL;

  if (col==0) return FALSE;
  chance += col*25 + victim->level - ch->level + (category_bonus(victim,PROTECT)-category_bonus(ch,OFFENCE));
  chance = UMIN(chance, 90);
  if (obj && IS_WEAPON_STAT(obj, WEAPON_VORPAL)) chance = chance * 4/5;
  if (obj && obj->value[0] == WEAPON_FLAIL) chance /= 2;

  if (number_percent() >= chance) return FALSE;
  if (col<2) {
    do_printf(buf1,"Ты {wблокиpуешь{x атаку {y$c2{x %s.",get_obj_desc(sh1 ? sh1:sh2,'5'));
    do_printf(buf2,"{y$C1{x {wблокиpует{x твою атаку %s.",get_obj_desc(sh1 ? sh1:sh2,'5'));
  }
  else {
    if (number_percent() >= 50) {
      do_printf(buf1,"Ты {wблокиpуешь{x атаку {y$c2{x %s.",get_obj_desc(sh2,'5'));
      do_printf(buf2,"{y$C1{x {wблокиpует{x твою атаку %s.",get_obj_desc(sh2,'5'));
    }
    else {
      do_printf(buf1,"Ты {wблокиpуешь{x атаку {y$c2{x %s.",get_obj_desc(sh1,'5'));
      do_printf(buf2,"{y$C1{x {wблокиpует{x твою атаку %s.",get_obj_desc(sh1,'5'));
    }
  }
  doact( buf1, ch, NULL, victim, TO_VICT,SPAM_MISS);
  doact( buf2, ch, NULL, victim, TO_CHAR,SPAM_MISS);
  check_improve(victim,gsn_shield_block,TRUE,6);
//  if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 1);
  return TRUE;
}

// victim dodges ch's attack
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
  int chance;

  if ( !IS_AWAKE(victim) ) return FALSE;
  if (victim->position <= POS_RESTING) return FALSE;
  if (IS_SET(victim->act,PLR_BLINK) && victim->classmag) return FALSE;
  if ( (chance = get_skill(victim,gsn_dodge)/2)==0) return FALSE;
  if (can_see(ch,victim,CHECK_LVL) && !can_see(victim,ch,CHECK_LVL)) chance /= 2;

  chance+= (ch->size - victim->size)*4;
  chance+= (get_curr_stat(victim,STAT_DEX)-get_curr_stat(ch,STAT_DEX))*3/2;
  if (IS_AFFECTED(ch,AFF_HASTE)) chance-=10;
  if (IS_AFFECTED(victim,AFF_HASTE)) chance+=10;
  if (IS_SET(race_table[victim->race].spec,SPEC_DODGE)) chance+=5;
  if (is_affected(victim,skill_lookup("ensnare"))) chance -=5;
  if (is_affected(victim,skill_lookup("slow"))) chance -=5;
  
  if ( !IS_NPC(ch) && !IS_NPC(victim) )
  {
    if ( victim->level >= 40 )
      chance+=URANGE(-10,(GET_HITROLL(victim)-GET_HITROLL(ch))*8/victim->level,10);
    else 
      chance+=URANGE(-8,(GET_HITROLL(victim)-GET_HITROLL(ch))/7,8);
  }
  
  if ( number_percent( ) >= (chance + victim->level + 2*(category_bonus(victim,PROTECT)-category_bonus(ch,OFFENCE))- ch->level)) return FALSE; 
  doact( "Ты {wуклоняешься{x от атаки {y$c2{x.", ch, NULL, victim, TO_VICT,SPAM_MISS);
  doact( "{y$C1{x {wуклоняется{x от твоей атаки.", ch, NULL, victim, TO_CHAR,SPAM_MISS);
  check_improve(victim,gsn_dodge,TRUE,6); 
//  if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 1);
  return TRUE; 
} 
 
bool check_blink( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
  int skill=get_skill(victim,gsn_blink);
  int chance=0;

  if(IS_NPC(victim) && (!IS_SET(victim->act,PLR_BLINK)
   || !IS_SET(victim->off_flags,OFF_BLINK))) return FALSE;

  if (!IS_NPC(victim) && (!IS_SET(victim->act,PLR_BLINK) 
   || skill<1 || victim->mana<1)) return FALSE;

  if (victim->position < POS_RESTING) return FALSE;

  chance=skill/2;
  if (!can_see(ch,victim,CHECK_LVL)) chance*=2;
  if (!can_see(victim,ch,CHECK_LVL)) chance/=2;

  chance+=(ch->size-victim->size)*4;
  chance+=(get_curr_stat(victim,STAT_INT)-get_curr_stat(ch,STAT_DEX))*2;

  if (IS_AFFECTED(ch,AFF_HASTE)) chance-=10;
  if (IS_AFFECTED(victim,AFF_HASTE)) chance+=10;
  if (is_affected(victim,skill_lookup("slow"))) chance -=5;
  
  if (!IS_NPC(ch) && !IS_NPC(victim) )
  {
    if ( victim->level >= 40 )
     chance+=URANGE(-10,(GET_HITROLL(victim)-GET_HITROLL(ch))*8/victim->level,10);
    else 
     chance+=URANGE(-8,(GET_HITROLL(victim)-GET_HITROLL(ch))/7,8);
  }
  chance+=victim->level-ch->level;
  chance+=(category_bonus(victim,PROTECT)-category_bonus(ch,OFFENCE))*4;

  if (number_percent() >= chance) return FALSE; 
  doact( "Ты {Cмерцаешь{x и атака {y$c2{x не причиняет тебе вреда{x.", ch, NULL, victim, TO_VICT,SPAM_MISS);
  doact( "{y$C1{x {Cмерцает{x и твое оружие проходит сквозь него, не причинив вреда.", ch, NULL, victim, TO_CHAR,SPAM_MISS);
  victim->mana -= 5-(skill/50);
  check_improve(victim,gsn_blink,TRUE,6);
//  if( IS_MAGIC_DEITY(ch) ) change_favour(ch, 1);
  return TRUE;
} 

void update_pos( CHAR_DATA *victim ) 
{ 
  if ( victim->hit > 0 ) 
  { 
    if ( victim->position < POS_STUNNED && victim->fighting == NULL) 
       victim->position = POS_STANDING; 
    return; 
  } 
 
  if ( IS_NPC(victim) && victim->hit < 0 ) 
  { 
    victim->position = POS_DEAD; 
    return; 
  } 
 
  if ( victim->hit <= -11 ) 
  { 
    victim->position = POS_DEAD; 
    return; 
  } 
 
  if      ( victim->hit <= -6 ) victim->position = POS_MORTAL; 
  else if ( victim->hit <= -3 ) victim->position = POS_INCAP; 
  else                          victim->position = POS_STUNNED; 
} 
 
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim ) 
{ 
  if ( ch->fighting != NULL ) 
  { 
    bug( "Ты уже сpажаешься", 0 ); 
    return; 
  } 
 
  if (is_affected(ch,gsn_sleep)) affect_strip( ch, gsn_sleep ); 
 
  ch->fighting = victim; 
  if (ch->position > POS_STUNNED) ch->position = POS_FIGHTING; 
 
  if (victim->fighting==NULL)  
  { 
   victim->fighting = ch; 
  if (victim->position > POS_STUNNED) victim->position = POS_FIGHTING; 
  } 
} 
 
void stop_fighting( CHAR_DATA *ch, bool fBoth ) 
{ 
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  ROOM_INDEX_DATA *room;

  room = (ch->in_room!=NULL)? ch->in_room : ch->was_in_room;
  if (room==NULL)
  {
   bug("Char is nowhere",0);
   return;
  }

  for ( fch = room->people; fch != NULL; fch = fch_next ) 
  { 
    fch_next=fch->next_in_room;
    if ( fch == ch || (fBoth && (fch->fighting == ch) ) ) 
    { 
      fch->fighting       = NULL; 
      fch->position       = IS_NPC(fch) ? fch->default_pos : POS_STANDING; 
      update_pos(fch); 
    } 
  } 
} 
 
void death_cry( CHAR_DATA *ch ) 
{ 
  ROOM_INDEX_DATA *was_in_room;
  ROOM_INDEX_DATA *room; 
  char *msg; 
  int door; 
  int64 vnum; 
 
  vnum = 0; 
  msg = "Ты слышишь пpедсмеpтный кpик {y$c2{x."; 
 
  room = (ch->in_room!=NULL)? ch->in_room : ch->was_in_room;
  if (room==NULL)
  {
   bug("Char is nowhere",0);
   return;
  }

  switch ( number_bits(4)) 
  { 
  case  0: 
  case  1:  
    msg  = "{y$c1{x падает на землю и {rУМИРАЕТ{x.";
    break; 
  case  2:                                                     
    if (IS_SET(race_table[ch->race].parts,PART_GUTS)) 
    { 
      msg = "Кишки {y$c2{x pасплескиваются во все стоpоны."; 
      vnum = OBJ_VNUM_GUTS; 
    } 
    break; 
  case  3:  
    if (IS_SET(race_table[ch->race].parts,PART_HEAD)) 
    { 
      msg  = "Отpубленая голова {y$c2{x пpыгает по земле."; 
      vnum = OBJ_VNUM_SEVERED_HEAD;                                
    } 
    break; 
  case  4:  
    if (IS_SET(race_table[ch->race].parts,PART_HEART)) 
    { 
      msg  = "Сеpдце {y$c2{x выpывается из $g гpуди."; 
      vnum = OBJ_VNUM_TORN_HEART;                          
    } 
    break; 
  case  5:  
    if (IS_SET(race_table[ch->race].parts,PART_ARMS)) 
    { 
      msg  = "Рука {y$c2{x отваливается от $g меpтвого тела."; 
      vnum = OBJ_VNUM_SLICED_ARM;                          
    } 
    break; 
  case  6:  
    if (IS_SET(race_table[ch->race].parts,PART_LEGS)) 
    { 
      msg  = "Нога {y$c2{x отваливается от $g меpтвого тела."; 
      vnum = OBJ_VNUM_SLICED_LEG;                          
    } 
    break; 
  case 7: 
    if (IS_SET(race_table[ch->race].parts,PART_BRAINS)) 
    { 
      msg = "Голова {y$c2{x pазлетается, и $g мозги падают к твоим ногам."; 
      vnum = OBJ_VNUM_BRAINS; 
    } 
  } 
 
  act( msg, ch, NULL, NULL, TO_ROOM ); 
  if ( vnum != 0 ) 
  { 
    char buf[MAX_STRING_LENGTH]; 
    OBJ_DATA *obj; 
    const char *name; 
 
    name            = IS_NPC(ch) ? get_char_desc(ch,'2'): ch->name; 
    if((obj             = create_object( get_obj_index( vnum ), 0 )))
    {
     obj->timer      = number_range( 4, 7 ); 
 
     do_printf( buf, obj->short_descr, name ); 
     free_string( obj->short_descr ); 
     obj->short_descr = str_dup( buf ); 
 
     do_printf( buf, obj->description, name ); 
     free_string( obj->description ); 
     obj->description = str_dup( buf ); 
 
     if (obj->item_type == ITEM_FOOD) 
     { 
      if (IS_SET(ch->form,FORM_POISON)) obj->value[3] = 1; 
      else if (!IS_SET(ch->form,FORM_EDIBLE)) obj->item_type = ITEM_TRASH; 
     } 
     obj_to_room( obj, room );
    }  
  } 
 
  if ( IS_NPC(ch) ) msg = "Ты слышишь чей-то пpедсмеpтный кpик."; 
  else msg = "Ты слышишь пpедсмеpтный кpик {y$c2{x."; 
 
  was_in_room = room; 
  for ( door = 0; door <= 5; door++ ) 
  { 
    EXIT_DATA *pexit; 
 
    if ( ( pexit = was_in_room->exit[door] ) != NULL 
     && pexit->u1.to_room != NULL 
     && pexit->u1.to_room != was_in_room ) 
    { 
      ch->in_room = pexit->u1.to_room; 
      act( msg, ch, NULL, NULL, TO_ROOM ); 
    } 
  } 
  ch->in_room = was_in_room; 
} 

void raw_kill( CHAR_DATA *victim )
{
  stop_fighting( victim, TRUE );
  death_cry( victim );
  if ( IS_NPC(victim) )
  {
    make_corpse( victim );
    victim->pIndexData->killed++;
    kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
    extract_char( victim, TRUE );
    return;
  }
  char_death(victim);
  save_char_obj( victim );
}

void group_gain( CHAR_DATA *ch, CHAR_DATA *victim ) 
{ 
  CHAR_DATA *gch; 
  int i;
  int xp; 
  int members, members_pkrange; 
  int dam_sum;
  int group_level; 
  double gbonus;
 
  /* Monsters don't get kill xp's or alignment changes. 
   * P-killing doesn't help either. 
   * Dying of mortal wounds or poison doesn't give xp to anyone! */ 
  if ( victim == ch ) return; 
 
  if (victim->questmob!=NULL && victim->questmob!=ch) 
  { 
    stc("{RТы угодил богам но это было поpучено дpугому.{x\n\r",ch); 
    stc("{RКто-то угодил богам вместо тебя{x",victim->questmob); 
    cancel_quest(victim->questmob, FALSE,6,10);
  } 
         
  if (IS_SET(ch->act, PLR_QUESTOR)&&IS_NPC(victim)) 
  { 
    if (ch->questmob == victim) 
    { 
      stc("Ты почти выполнил свое задание!\n\r",ch); 
      stc("Возвpащайся к тому кто тебе его дал пока есть вpемя!\n\r",ch); 
      ch->questmob = NULL;
      SET_BIT(ch->q_stat, Q_MOB_KILLED);
      save_char_obj( ch ); 
    } 
  } 

  if (!IS_NPC(ch) && IS_NPC(victim) && ch->pcdata->gquest.status==GQ_JOINED)
  {
    int i;
    for (i=0;i<21;i++)
    {
      if(victim->pIndexData->vnum==gquest.target[i]
       && ch->pcdata->gquest.target[i]<gquest.target_counter[i])
       {
         int reward=number_range(1,5);
         ch->pcdata->gquest.target[i]++;
         stc("Ты выполнил часть задания богов!\n\r",ch);
         ptc(ch,"Ты получаешь %d квестовых очков.\n\r",reward);
         ch->questpoints+=reward;
         break;
       }
    }
  }   

  members = 0; 
  members_pkrange = 0;
  dam_sum=0;
  group_level = ch->level; 
  for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room ) 
  { 
    if ( is_same_group( gch, ch ) ) 
    { 
      members++; 
      if (PK_RANGE(gch,ch) && !IS_NPC(gch)) 
      {
        members_pkrange++;
        for(i=0;i<MAX_VICT;i++)
        {
          if (victim==gch->pcdata->victims[i].victim) 
           dam_sum+=gch->pcdata->victims[i].dampool;
        }
      }
      group_level=UMAX(group_level,gch->level);
    } 
  } 

  if ( members == 0 ) 
  { 
    bug( "Group : 0 members.", members ); 
    members = 1; 
    group_level = ch->level ; 
  } 

  if ( members_pkrange == 0 ) 
  { 
    bug( "Group : 0 pkrange members.", members_pkrange ); 
    members_pkrange = 1; 
  } 
 
  for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room ) 
  { 
    OBJ_DATA *obj; 
    OBJ_DATA *obj_next; 
 
    if ( !is_same_group( gch, ch ) || IS_NPC(gch)) continue;

// pc group bonus (c) Ghost
    gbonus=1.0;
    if (members_pkrange>1 && members_pkrange<7 && dam_sum>victim->max_hit*0.9)
      for(i=0;i<MAX_VICT;i++)
      {
        if (victim==gch->pcdata->victims[i].victim) 
        { 
          gbonus=gch->pcdata->victims[i].dampool/(double)victim->max_hit - 1/(double)members_pkrange;
          gbonus=URANGE(-0.5, gbonus*members_pkrange/2, 0.5);
          gbonus*=3.14159265358979323846;
          gbonus=gbonus*gbonus*gbonus*gbonus/24 - gbonus*gbonus/2 + 1.9;
          gbonus=URANGE(0.9, gbonus, 1.9);
        } 
      }
          
    if (!IS_SET(victim->act, ACT_EXTRACT_CORPSE) 
     && !IS_CLAN_ROOM(victim->in_room)
     && !IS_CLAN_ROOM(ch->in_room)) 
     xp = (int)(gbonus*xp_compute( gch, victim, group_level));
    else xp = 0; 
    if (!IS_NPC(gch) && !IS_NPC(victim)) xp=0; 
    ptc( gch, "Ты получаешь %d experience points.\n\r", xp ); 
    gain_exp( gch, xp ); 
 
    for ( obj = ch->carrying; obj != NULL; obj = obj_next ) 
    { 
      obj_next = obj->next_content; 
      if (!IS_NPC(ch) && IS_SET(ch->act,PLR_ARMY)
          && (obj->pIndexData->vnum < 12401 || obj->pIndexData->vnum > 12990 || obj->pIndexData->vnum<102)
          && (obj->item_type != ITEM_FOOD) && (obj->item_type != ITEM_DRINK_CON))
      {
       act("Волосатый дембель, нехорошо ухмыляясь, забирает у тебя {w$i1{x.",ch,obj,NULL,TO_CHAR);
       act("Волосатый дембель, нехорошо ухмыляясь, забирает у {y$c1 {w$i1{x.",ch,obj,NULL,TO_ROOM);
       obj_from_char(obj);
       extract_obj(obj);
      }
      if ( obj->wear_loc == WEAR_NONE ) continue; 
 
      if (  (IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)   ) 
       ||   (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)   ) 
       ||   (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch))
       ||   (IS_SET(obj->extra_flags,ITEM_CLANENCHANT)
         && (ch->clan==NULL || !is_exact_name(ch->clan->name,obj->name))))
      { 
        act("Ты обжигаешься и бросаешь {w$i1{x.", ch, obj, NULL, TO_CHAR); 
        act("{y$c1{x обжигается и бросает {w$i1{x.", ch, obj, NULL, TO_ROOM); 
        obj_from_char(obj); 
        obj_to_room(obj, ch->in_room); 
      } 
    } 
  } 
} 
 
// Compute xp for a kill. Also adjust alignment of killer. 
//  Edit this function to change xp computations.
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels ) 
{ 
  int xp,base_exp; 
  int align,level_range; 
  int change; 
//  int time_per_level; //Remmed by Magic 
  float modifier; 
  int i; 
 
/*
  if (total_levels>gch->level+8)
  {
    stc("Ты слишком маленький для такой группы.\n\r",gch);
    return 0;
  }
*/
  if (IS_CLAN_ROOM(gch->in_room) || IS_CLAN_ROOM(victim->in_room)) return 0;
  
  level_range = victim->level - gch->level; 
  
  // compute the base exp
  switch (level_range) 
  { 
    default : base_exp = 0; break; 
    case -9 : base_exp = 1; break; 
    case -8 : base_exp = 2; break; 
    case -7 : base_exp = 6; break; 
    case -6 : base_exp = 11; break; 
    case -5 : base_exp = 14; break; 
    case -4 : base_exp = 19; break; 
    case -3 : base_exp = 35; break; 
    case -2 : base_exp = 48; break; 
    case -1 : base_exp = 53; break; 
    case  0 : base_exp = 69; break; 
    case  1 : base_exp = 80; break; 
    case  2 : base_exp = 109; break; 
    case  3 : base_exp = 120; break; 
    case  4 : base_exp = 135; break; 
  }  
     
  if (level_range > 4) base_exp = 135 + 15 * (level_range - 4); 
 
  /* do alignment computations */ 
    
  align = victim->alignment - gch->alignment; 
 
  if (IS_SET(victim->act,ACT_NOALIGN)
  || (gch->questmob == victim))
  { 
  } 

  else if (align > 500) /* monster is more good than slayer */ 
  { 
    change = (align - 500) * base_exp / 500 * gch->level/total_levels;
    change = UMAX(1,change); 
    gch->alignment = UMAX(-1000,gch->alignment - change);
  } 
 
  else if (align < -500) /* monster is more evil than slayer */ 
  { 
    change = ( -1 * align - 500) * base_exp/500 * gch->level/total_levels; 
    change = UMAX(1,change); 
    gch->alignment = UMIN(1000,gch->alignment + change); 
  } 
 
  else /* improve this someday */ 
  { 
    change = gch->alignment * base_exp/500 * gch->level/total_levels;   
    gch->alignment -= change; 
  } 
     
  if (IS_ORDEN(gch)) gch->alignment=UMAX(350,gch->alignment);

  /* calculate exp multiplier */ 
  if (IS_SET(victim->act,ACT_NOALIGN)) xp = base_exp; 
 
  else if (gch->alignment > 500)  /* for goodie two shoes */ 
  { 
    if (victim->alignment < -750) xp = (base_exp *4)/3; 
    else if (victim->alignment < -500) xp = (base_exp * 5)/4; 
    else if (victim->alignment > 750) xp = base_exp / 4; 
    else if (victim->alignment > 500) xp = base_exp / 2; 
    else if (victim->alignment > 250) xp = (base_exp * 3)/4;  
    else xp = base_exp; 
  } 
 
  else if (gch->alignment < -500) /* for baddies */ 
  { 
    if (victim->alignment > 750) xp = (base_exp * 5)/4; 
    else if (victim->alignment > 500) xp = (base_exp * 11)/10;  
    else if (victim->alignment < -750) xp = base_exp/2; 
    else if (victim->alignment < -500) xp = (base_exp * 3)/4; 
    else if (victim->alignment < -250) xp = (base_exp * 9)/10; 
    else xp = base_exp; 
  } 
 
  else if (gch->alignment > 200)  /* a little good */ 
  { 
    if (victim->alignment < -500) xp = (base_exp * 6)/5; 
    else if (victim->alignment > 750) xp = base_exp/2; 
    else if (victim->alignment > 0) xp = (base_exp * 3)/4;  
    else xp = base_exp; 
  } 
 
  else if (gch->alignment < -200) /* a little bad */ 
  { 
    if (victim->alignment > 500) xp = (base_exp * 6)/5; 
    else if (victim->alignment < -750) xp = base_exp/2; 
    else if (victim->alignment < 0) xp = (base_exp * 3)/4; 
    else xp = base_exp; 
  } 
 
  else // neutral
  { 
    if (victim->alignment > 500 || victim->alignment < -500) xp = (base_exp * 4)/3; 
    else if (victim->alignment < 200 && victim->alignment > -200) xp = base_exp/2; 
    else xp = base_exp; 
  } 
 
  /* more exp at the low levels */ 
  if (gch->level < 6) xp = 10 * xp / (gch->level + 4); 
 
  /* less at high */ 
  if (gch->level > 35 && gch->level<65) xp = 15 * xp / (gch->level - 25 ); 
 
  // reduce for playing time
/*
  { 
    // compute quarter-hours per level 
    time_per_level = 4 * (gch->played + (int) (current_time - gch->logon))/3600 
                         / gch->level; 
    time_per_level = URANGE(2,time_per_level,12); 
    if (gch->level < 15)  // make it a curve
            time_per_level = UMAX(time_per_level,(15 - gch->level)); 
    xp = xp * time_per_level / 14; 
  } 
*/
    
  /* randomize the rewards */ 
  xp = number_range (xp * 3/4, xp * 5/4); 
 
// adjust for grouping - not need now (C) Jasana 
//  xp = xp * gch->level/( UMAX(1,total_levels -1) ); 
  if (gch->level > 97) xp/=2; 
 
  if (IS_SET(victim->act,ACT_PET)) xp=0; 
 
  //calculating percent modifier 
  if (!IS_NPC(gch)) 
  {
    bool found=FALSE;
    for(i=0;i<MAX_VICT;i++)
    {
      if (victim==gch->pcdata->victims[i].victim) 
      { 
       modifier = gch->pcdata->victims[i].dampool/(float)victim->max_hit;
       modifier = URANGE(0,modifier,1); 
       xp = (int) (xp * modifier);
       gch->pcdata->victims[i].victim = NULL; 
       gch->pcdata->victims[i].dampool = 0; 
       found=TRUE;
      } 
    }
    if (!found) xp=0;
  }
  xp+=xp/50*category_bonus(gch,LEARN);
  return xp; 
} 

void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dam_type,bool immune, OBJ_DATA *obj )
{ 
  char buf1[256], buf2[256], buf3[256]; 
  const char *attack; 
  char punct; 
  int dammsg;
 
  if (!ch || !victim ) return; 


  for (dammsg=0;dam_msg_table[dammsg].to!=-1;dammsg++)
  {
    if (dam>=dam_msg_table[dammsg].from && dam<=dam_msg_table[dammsg].to) break;
  }

  punct   = (dam <= 50) ? '.' : '!'; 
  
  if ( dam_type==1000 )
  { 
    if (ch  == victim) 
    { 
      do_printf( buf1, "{y$c1n{x %s cебя%c",dam_msg_table[dammsg].vp,punct); 
      do_printf( buf2, "Ты %s себя%c",dam_msg_table[dammsg].vs,punct); 
    } 
    else 
    { 
      if ((IS_CFG(ch,CFG_AUTODAM)     && IS_IMMORTAL(ch)) ||
          (IS_CFG(victim,CFG_AUTODAM) && IS_IMMORTAL(victim)))
      { 
        do_printf( buf1, "{y$c1{x %s $C4%c", dam_msg_table[dammsg].vp,punct); 
        do_printf( buf2, "Ты {W%s {y$C4{x%c {G(%d){x", dam_msg_table[dammsg].vs, punct,dam ); 
        do_printf( buf3, "{y$c1{x {W%s{x тебя%c {R(%d){x", dam_msg_table[dammsg].vp, punct, dam); 
      } 
      else 
      { 
        do_printf( buf1, "{y$c1{x %s $C4%c",  dam_msg_table[dammsg].vp, punct ); 
        do_printf( buf2, "Ты {W%s {y$C4{x%c", dam_msg_table[dammsg].vs, punct ); 
        do_printf( buf3, "{y$c1{x {W%s{x тебя%c", dam_msg_table[dammsg].vp, punct); 
      } 
    } 
  } 
  else 
  { 
    if (dam_type>=0 && dam_type< max_skill) attack = skill_table[dam_type].noun_damage;
    else if ( dam_type>=1000 && dam_type<1000+MAX_DAMAGE_MESSAGE)
    {
      dam_type-=1000;
      attack= attack_table[dam_type].noun;
    }
    else 
    { 
      bug( "Dam_message: bad dt %d.", dam_type ); 
      dam_type = 0;
      attack  = attack_table[0].name; 
    } 
    if (immune) 
    { 
      if (ch == victim) 
      { 
        do_printf(buf1,"$o %s бессилен против {y$c2{x.",attack); 
        doact(buf1,ch, obj,NULL,TO_ROOM, SPAM_MISS);
        do_printf(buf2,"К счастью, у тебя иммунитет на это."); 
        doact(buf2,ch, obj,NULL,TO_CHAR, SPAM_MISS);
      }  
      else 
      { 
        do_printf(buf1,"{y$C1{x не боится атаки {x%s!",attack); 
        doact( buf1, ch, obj, victim, TO_NOTVICT, SPAM_OTHERF|SPAM_MISS );
        do_printf(buf2,"{y$C1{x не боится атаки %s!",attack); 
        doact( buf2, ch, obj, victim, TO_CHAR, SPAM_MISS );
        do_printf(buf3,"Атака %s не причиняет тебе вреда.",attack); 
        doact( buf3, ch, obj, victim, TO_VICT, SPAM_MISS );
      } 
    } 
    else 
    { 
      if (ch == victim) 
      { 
        do_printf( buf1, "{y$c1{x %s %s {yсебя{x%c",attack,dam_msg_table[dammsg].vp,punct); 
        act(buf1,ch, obj,NULL,TO_ROOM);
        do_printf( buf2, "Ты {Y%s {W%s{x себя%c",attack,dam_msg_table[dammsg].vs,punct); 
        act(buf2,ch, obj,NULL,TO_CHAR);
      } 
      else 
      { 
        if ((IS_CFG(ch,CFG_AUTODAM)     && IS_IMMORTAL(ch)) ||
            (IS_CFG(victim,CFG_AUTODAM) && IS_IMMORTAL(victim)))
        { 
          do_printf( buf1, "{Y$c1{x %s %s {Y$C4{x%c ",  attack, dam_msg_table[dammsg].vp, punct); 
          doact( buf1, ch, obj, victim, TO_NOTVICT, SPAM_OTHERF );
          do_printf( buf2, "Ты %s %s {Y$C4{x%c {G(%d){x",  attack, dam_msg_table[dammsg].vs, punct,dam ); 
          act( buf2, ch, obj, victim, TO_CHAR );
          do_printf( buf3, "{Y$c1{x %s %s тебя%c {R(%d){x", attack, dam_msg_table[dammsg].vp, punct, dam ); 
          act( buf3, ch, obj, victim, TO_VICT );
        } 
        else 
        { 
          do_printf( buf1, "{Y$c1{x %s %s {Y$C4{x%c",  attack, dam_msg_table[dammsg].vp, punct ); 
          doact( buf1, ch, obj, victim, TO_NOTVICT, SPAM_OTHERF );
          do_printf( buf2, "Ты %s %s {Y$C4{x%c",  attack, dam_msg_table[dammsg].vs, punct ); 
          act( buf2, ch, obj, victim, TO_CHAR );
          do_printf( buf3, "{Y$c1{x %s %s тебя%c", attack, dam_msg_table[dammsg].vp, punct ); 
          act( buf3, ch, obj, victim, TO_VICT );
        }  
      } 
    } 
  } 
} 
 
void disarm( CHAR_DATA *ch, CHAR_DATA *victim, bool fLeft ) 
{ 
  OBJ_DATA *obj; 
     
  if (!fLeft) obj = get_eq_char( victim, WEAR_RHAND);
  if (fLeft || !obj || obj->item_type !=ITEM_WEAPON) obj = get_eq_char( victim, WEAR_LHAND );
  if (!obj || obj->item_type !=ITEM_WEAPON) return; 

  if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE)) 
  { 
    doact("{y$G{x оpужие не пошевельнулось!",ch,NULL,victim,TO_CHAR,SPAM_SKMISS); 
    doact("{y$c1{x пытается тебя обезоpужить, но ты удеpжал свое оpужие!", 
            ch,NULL,victim,TO_VICT,SPAM_SKMISS); 
    doact("{y$c1{x пытается обезоpужить {y$C4{x, но неудачно.",ch,NULL,victim,TO_NOTVICT,SPAM_OTHERF); 
    return; 
  } 
 
  act( "{y$c1{x {wОБЕЗОРУЖИВАЕТ{x тебя и твое оpужие падает на землю!",  
   ch, NULL, victim, TO_VICT    ); 
  act( "Ты обезоpужил {y$C4{x!",  ch, NULL, victim, TO_CHAR    ); 
  act( "{y$c1{x обезоpуживает {y$C4{x!",  ch, NULL, victim, TO_NOTVICT); 
 
  unequip_char( victim, obj ); 
  obj_from_char( obj ); 
  if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY) ) 
        obj_to_char( obj, victim ); 
  else 
  { 
    obj_to_room( obj, victim->in_room ); 
    if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj)) 
    {
      local_get_obj(victim,obj,NULL, TRUE); 
      do_wear (victim,obj->name);
    }
  } 
}
 
void do_berserk( CHAR_DATA *ch, const char *argument) 
{ 
  int chance, hp_percent; 
 
  if ((chance = get_skill(ch,gsn_berserk)) == 0 
   ||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK)) 
   ||  (!IS_NPC(ch) 
   &&   ch->level < min_level(ch,gsn_berserk))) 
  { 
    stc("Твое лицо становится кpасным, но ничего не пpоисходит.\n\r",ch);        cant_mes (ch); 
    return; 
  } 
 
  if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk) 
   ||  is_affected(ch,skill_lookup("frenzy"))) 
  { 
    stc("Ты и так уже на грани сумасшествия.\n\r",ch); 
    return; 
  } 
 
  if (IS_AFFECTED(ch,AFF_CALM)) 
  { 
    stc("Ты слишком спокоен чтобы вспыхнуть гневом.\n\r",ch); 
    return; 
  } 

  if (is_affected(ch,skill_lookup("fear")))
  {
    stc("Ты весь дрожишь от ужаса, ты не способен гневаться.\n\r",ch);
    return;
  }
 
  if (ch->mana < 50) 
  { 
    stc("У тебя не хватает энергии.\n\r",ch); 
    return; 
  } 
 
  // fighting
  if (ch->position == POS_FIGHTING) chance += 10; 
 
  // damage -- below 50% of hp helps, above hurts
  hp_percent = 100 * ch->hit/ch->max_hit; 
  chance += 25 - hp_percent/2; 
 
  if (number_percent() < chance) 
  { 
    AFFECT_DATA af; 
 
    WAIT_STATE(ch,PULSE_VIOLENCE); 
    ch->mana -= 50; 
    ch->move /= 2; 
 
    // heal a little damage
    ch->hit += ch->level * 2; 
    ch->hit = UMIN(ch->hit,ch->max_hit); 
 
    stc("Твой пульс учащается, ты в гневе!\n\r",ch); 
    act("В глазах $c2 вспыхивают дикие огоньки.",ch,NULL,NULL,TO_ROOM); 
    check_improve(ch,gsn_berserk,TRUE,2); 
 
    af.where        = TO_AFFECTS; 
    af.type         = gsn_berserk; 
    af.level        = ch->level+category_bonus(ch,FORTITUDE)*3; 
    af.duration     = number_fuzzy(ch->level / 8); 
    af.modifier     = UMAX(1,3*ch->level/4+category_bonus(ch,FORTITUDE)*3); 
    af.bitvector    = AFF_BERSERK;
    af.location     = APPLY_HITROLL; 
    affect_to_char(ch,&af); 
 
    af.location     = APPLY_DAMROLL; 
    affect_to_char(ch,&af); 
 
    af.modifier     = UMAX(10,10 * (ch->level/2)); 
    af.location     = APPLY_AC; 
    affect_to_char(ch,&af); 
 
    af.location  = APPLY_SAVING_SPELL; 
    af.modifier  = ch->level/5 + category_bonus(ch,FORTITUDE);
    if (af.modifier>0) af.modifier=0-af.modifier;
    affect_to_char(ch,&af); 
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 10);
//    if( IS_FANATIC_DEITY(ch) ) change_favour(ch, 10);
  } 
  else 
  { 
    WAIT_STATE(ch,3 * PULSE_VIOLENCE); 
    ch->mana -= 25; 
    if(ch->move > 1) ch->move /= 2; 
 
    stc("Твой пульс начинает биться быстpее, но ничего не пpоисходит.\n\r",ch); 
    check_improve(ch,gsn_berserk,FALSE,2); 
  } 
} 
 
void do_bash( CHAR_DATA *ch, const char *argument ) 
{ 
  char arg[MAX_STRING_LENGTH]; 
  CHAR_DATA *victim; 
  int chance; 
 
  one_argument(argument,arg); 
 
  if (!can_attack(ch,1)) return; 
 
  if ( (chance = get_skill(ch,gsn_bash)) == 0 
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH)) 
    ||   (!IS_NPC(ch) && !check_skill(ch,gsn_bash))) 
  { 
    stc("Толкнуть? Как?\n\r",ch);        cant_mes (ch); 
    return; 
  } 
  
  if (arg[0] == '\0') 
  { 
    victim = ch->fighting; 
    if (victim == NULL) 
    { 
      stc("Кого толкнуть?\n\r",ch); 
      return; 
    } 
  } 
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Нет здесь таких.\n\r",ch); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Ты сильно толкаешь сам себя...тормоз.\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 
 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Hо {y$C1{x твой коpеш!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
 
  if (IS_SET(ch->act,PLR_TIPSY)) 
   if (tipsy(ch,"bash")) return; 

  if (ch->size<victim->size)  
  {
   stc("Тебе не сдвинуть эту стену...:(\n\r",ch);
   return;
  }
 
  /*** New chance formula by Imp follows this pretty long comment ***/
  chance =  chance*7/10; //Initial = 75% (max)
  chance += (ch->size-victim->size)*4;
  chance += (get_curr_stat(ch,STAT_STR)-get_curr_stat(victim,STAT_DEX))*2;
  chance -= get_skill(victim,gsn_dodge)/20;
  chance += (ch->level - victim->level);
  chance += (is_affected(victim,skill_lookup("ensnare")) ? 10 : 1 );
  if (IS_SET(race_table[victim->race].spec,SPEC_RESBASH)) chance/=2;
  
  check_criminal(ch,victim,60); 
  if (number_percent() < chance ) 
  { 
    if (IS_AFFECTED(victim, AFF_FIRESHIELD) && number_percent()<25) 
    { 
      act("{y$c1{x обжигается о твой огненный щит, и с визгом отскакивает!",ch,NULL,victim,TO_VICT); 
      act("Ты обжигаешься об окружающий {y$C4{x огненный щит, и отскакиваешь!",ch,NULL,victim,TO_CHAR); 
      act("{y$c1{x обжигается об окружающий {y$C4{x огненный щит, и с визгом отскакивает.", 
                 ch,NULL,victim,TO_NOTVICT); 
      damage(ch,victim,0,gsn_bash,DAM_BASH,TRUE, FALSE, NULL);
      WAIT_STATE(ch,skill_table[gsn_bash].beats); 
      return; 
    } 
    act("{y$c1{x сбивает тебя с ног сильным ударом, заставляя тебя ползать!", 
               ch,NULL,victim,TO_VICT); 
    act("Ты швыpяешь {y$C4{x, заставляя $G ползать!",ch,NULL,victim,TO_CHAR); 
    act("{y$c1{x сильным удаpом сбивает {y$C4{x с ног.", 
               ch,NULL,victim,TO_NOTVICT); 
    check_improve(ch,gsn_bash,TRUE,1); 
 
    DAZE_STATE(victim, 3 * PULSE_VIOLENCE); 
    WAIT_STATE(ch,skill_table[gsn_bash].beats); 
    victim->position = POS_RESTING; 
    damage(ch,victim,number_range(2,10 + dice(ch->level, ch->size+1) + chance/4),gsn_bash,DAM_BASH,TRUE, FALSE, NULL);
//   if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 2);
  } 
  else 
  { 
    damage(ch,victim,0,gsn_bash,DAM_BASH,FALSE,FALSE, NULL); 
    doact("Твое лицо вытягивается от удивления! Противник устоял на ногах.", 
           ch,NULL,victim,TO_CHAR,SPAM_SKMISS); 
    doact("{y$c1{x вытягивает от удивления лицо.", 
           ch,NULL,victim,TO_NOTVICT,SPAM_OTHERF); 
    doact("Ты уклоняешься от удаpа $c2, заставляя $g вытянуть от удивления cвое лицо.", 
           ch,NULL,victim,TO_VICT,SPAM_SKMISS); 
    check_improve(ch,gsn_bash,FALSE,1); 
    ch->position = POS_RESTING; 
    WAIT_STATE(ch,skill_table[gsn_bash].beats * 3/2);  
  } 
} 

void do_strangle(CHAR_DATA *ch, const char *argument)
{
  
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  AFFECT_DATA af;
  int offence, defence,percent_=0;
  
  one_argument(argument,arg);
  
  if (!can_attack(ch,1)) return;
  if (! get_skill(ch,gsn_strangle) || (!IS_NPC(ch) &&
      ! check_skill(ch,gsn_strangle)))
  { 
    stc("Усыпить? Как?\n\r",ch);
    cant_mes (ch); 
    return;
  }
 
  if (arg[0] == '\0')
  {
    stc("Кого усыпить?\n\r",ch);
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"strangle")) return;

  if ((victim = get_char_room(ch,arg)) == NULL)
  {
    stc("Нет здесь таких.\n\r",ch);
    return;
  }

  if (victim == ch)
  {
    stc("Ты усыпляешь сам себя...тормоз.\n\r",ch);
    return;
  }

  if (IS_IMMORTAL(victim) && get_trust(ch)<get_trust(victim))
  {
    stc("Ты на кого замахнулся!\n\r",ch);
    return;
  }

  if (IS_NPC(victim) &&
     (victim->spec_fun==spec_lookup( "spec_questmaster" )||
     victim->spec_fun==spec_lookup( "spec_executioner")))
  {
    stc("Ты на кого замахнулся!\n\r",ch);
    return;
  }

  if (is_safe(ch,victim)) return; 

  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
  { 
    act("Hо {y$C1{x твой коpеш!",ch,NULL,victim,TO_CHAR);
    return;
  }

  if ( !IS_AWAKE(victim) )
  { 
    act("$C1 уже спит.",ch,NULL,victim,TO_CHAR);
    return;
  }
  if (victim->fighting != NULL) {
    act("$C1 слишком внимателен.",ch,NULL,victim,TO_CHAR);
    return;
  }
  if ( is_affected(victim,gsn_strangle)) 
  {  
    stc("Ему уже и так нехорошо...",ch);
    return;
  } 
  if (IS_NPC(victim) && IS_SET(victim->act,ACT_UNDEAD))
  {
   stc ("Он к этому не восприимчив.",ch);
   return;
  } 

  WAIT_STATE(ch,skill_table[gsn_strangle].beats);
    
  check_criminal(ch,victim,60);
  offence = get_skill(ch,gsn_strangle)  *  
            (ch->level + get_curr_stat(ch,STAT_DEX)*2/3 + 
            get_curr_stat(ch,STAT_STR)) / 100;
  defence = victim->level + get_curr_stat(victim,STAT_DEX)*2/3
                          + get_curr_stat(ch,STAT_CON);
  percent_ = number_percent();
  if (offence + dice(3,40)  >  defence + dice(3,40))
  {
  {
   if (percent_ < 30)
    {  
     act("{y$c1{x придушивает тебя!", ch,NULL,victim,TO_VICT); 
     act("Ты придушиваешь {y$C4{x.",ch,NULL,victim,TO_CHAR); 
     act("{y$c1{x придушивает {y$C4{x.", ch,NULL,victim,TO_NOTVICT); 
     stc( "Ты засыпаешь....\n\r", victim );
     act( "$c1 засыпает.", victim, NULL, NULL, TO_ROOM );
     victim->position = POS_SLEEPING;
    
     af.where     = TO_AFFECTS;
     af.type      = gsn_sleep;
     af.level     = ch->level;
     af.duration  = 0;
     af.location  = APPLY_NONE;
     af.modifier  = 0;
     af.bitvector = 0;
     affect_to_char( victim, &af );

     check_improve(ch,gsn_strangle,TRUE,1);
//     if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 25);
    }  
   else 
    {
     act("{y$c1{x придушивает тебя! ..но не до конца..",ch,NULL,victim,TO_VICT);
     act("Ты не смог полностью придушить {y$C4{x.",ch,NULL,victim,TO_CHAR);
     act("{y$c1{x почти придушил {y$C4{x. ", ch,NULL,victim,TO_NOTVICT);
     stc( "Ты чувствуешь слабость во всем...\n\r", victim );
     act( "$c1 чувствует некий дискомфорт.", victim, NULL, NULL, TO_ROOM );
   
     if (percent_ < 75)
      {  
       af.where     = TO_AFFECTS;
       af.type      = gsn_strangle;
       af.level     = ch->level;
       af.duration  = 1;
       af.location  = APPLY_HITROLL;
       af.modifier  = -1 * ((percent_ <50 )?(ch->level):(ch->level / 2));
       af.bitvector = 0;
       affect_to_char( victim, &af );
       stc("У тебя, похоже, начались проблемы с меткостью..\n\r",victim);
       act("$c1, похоже, не может точно прицелиться..",victim,NULL,NULL,TO_ROOM);    
//       if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 15);
      }
     if (percent_ < 45)
      {
       af.where     = TO_AFFECTS;
       af.type      = gsn_strangle;
       af.level     = ch->level;
       af.duration  = 1;
       af.location  = APPLY_DEX;
       af.modifier  = -1 - (ch->level >= 18) - 2*(ch->level >= 25) - 3*(ch->level >= 32) - 3*(ch->level >=101);
       af.bitvector = AFF_SLOW;
       affect_to_char( victim, &af );
       stc("У тебя ,похоже, проблемы с координацией движений..\n\r",victim);
       act("$c1 начинает двигаться медленно и глючно..",victim,NULL,NULL,TO_ROOM);
//       if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 15);
      }
     if (percent_ < 30)
      {   
       af.where     = TO_AFFECTS;
       af.type      = gsn_strangle;
       af.level     = ch->level;
       af.duration  = 1;
       af.location  = APPLY_STR;
       af.modifier  = -1 * (ch->level / 5);
       af.bitvector = 0;
       affect_to_char( victim, &af );
       stc( "Ты чувствуешь, как сила уходит из тебя..\n\r", victim );
       act("$c1 выглядит слабым и уставшим..",victim,NULL,NULL,TO_ROOM);
//       if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 10);
      }
     if (percent_ < 35)
      {
       af.where     = TO_AFFECTS;
       af.type      = gsn_strangle;
       af.level     = ch->level;
       af.duration  = 1;
       af.location  = APPLY_INT;
       af.modifier  = -1 * (ch->level / 8) - 1;
       af.bitvector = 0;
       affect_to_char( victim, &af );
       stc( "Похоже, у тебя проблемы с головой..\n\r",ch);
       act( "У $c1, похоже, затуманилась голова..",victim,NULL,NULL,TO_ROOM);
//       if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 15);
      }                                                                                                           
     add_pkiller(victim,ch);
     one_hit( victim, ch, TYPE_UNDEFINED, NULL, TRUE ); 
     check_improve(ch,gsn_strangle,TRUE,1);
    } 
  }
  }
  else 
  {
    add_pkiller(victim,ch);
    act("Твои руки не сходятся на шее $C1.",ch,NULL,victim,TO_CHAR);
    one_hit( victim, ch, TYPE_UNDEFINED, NULL, TRUE ); 
    check_improve(ch,gsn_strangle,FALSE,1);  
  }
}

void do_throw( CHAR_DATA *ch, const char *argument ) 
{ 
 CHAR_DATA *victim,*tmp_vict; 
 OBJ_DATA *obj; 
 char arg1[MAX_STRING_LENGTH],arg2[MAX_STRING_LENGTH]; 
 int chance; 
      
 argument = one_argument(argument,arg1); 
 argument = one_argument(argument,arg2); 
  
 if ((chance = get_skill(ch,gsn_missile)) == 0 )
 {    
  stc("Ты не умеешь метать оружие.\n\r",ch); 
  return; 
 } 
 
 if (arg1[0] == '\0') 
 { 
  stc("Кинуть что (и в кого)?\n\r",ch); 
  return; 
 } 
     
 if ((obj = get_obj_carry(ch,arg1, ch)) == NULL) 
 { 
  stc("У тебя нет этой вещи.\n\r",ch); 
  return; 
 } 
  
 if (get_obj_weight(obj) > (str_app[get_curr_stat(ch,STAT_STR)].wield*10) 
  || obj->level > ch->level) 
 { 
  stc( "Тяжеловато для метания.\n\r", ch ); 
  return; 
 } 
 
 if (IS_OBJ_STAT(obj,ITEM_NODROP)) 
 { 
  stc("Ты не можешь от этого избавиться.\n\r",ch); 
  return; 
 } 
 
 if (obj->item_type !=ITEM_WEAPON 
   || !IS_WEAPON_STAT(( obj ) ,WEAPON_MISSILE))
 { 
  stc("Ты не можешь это метнуть.\n\r",ch); 
  return;          
 } 
 
 if (arg2[0] == '\0') 
 { 
  victim = ch->fighting; 
  if (victim == NULL) 
  { 
   stc("Но ты не сражаешься.\n\r",ch); 
   return; 
  } 
 } 
 else if ((victim = get_char_room(ch,arg2)) == NULL) 
 { 
  stc("Нет здесь таких.\n\r",ch); 
  return; 
 } 
 
 if (victim == ch) 
 { 
  stc("Кинуть в себя - дурная идея.\n\r",ch); 
  return; 
 } 
 
 if (is_safe(ch,victim)) return; 
 
 if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
 { 
  act("Hо {y$C1{x твой коpеш!",ch,NULL,victim,TO_CHAR); 
  return; 
 } 
                         
 /* now the attack */ 
 check_criminal(ch,victim,60); 
 { 
  act("{y$c1{x швыряет в тебя $i1!",ch,obj,victim,TO_VICT); 
  act("Ты швыряешь $i1 в {y$C4{x!",ch,obj,victim,TO_CHAR); 
  act("{y$c1{x швыряет $i1 в {y$C4{x.",ch,obj,victim,TO_NOTVICT); 
  WAIT_STATE(ch,skill_table[gsn_missile].beats); 
  tmp_vict = ch;
  tmp_vict = ch->in_room->people;
  if (obj->item_type == ITEM_WEAPON)
  {
   if (IS_WEAPON_STAT(( obj ) ,WEAPON_ROUND))
   {
    if (can_see( ch, victim,CHECK_LVL )) one_hit( ch, victim, gsn_missile,obj, TRUE );
    while (tmp_vict != NULL)
    {
     if (can_see( ch, tmp_vict,CHECK_LVL )) one_hit(ch , tmp_vict, gsn_missile,obj, TRUE);
     tmp_vict = tmp_vict->next_in_room;
    }
   }
   else if (can_see( ch, victim,CHECK_LVL )) one_hit( ch, victim, gsn_missile,obj, TRUE );
  }
  else if (can_see( ch, victim,CHECK_LVL )) one_hit( ch, victim, gsn_missile,obj, TRUE );
//  if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 3);
//  if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 1);
  if (!IS_WEAPON_STAT(( obj ) ,WEAPON_RETURN))
  {
   if (number_percent( ) > 20)
   {
    obj_from_char( obj);
    obj_to_room( obj, ch->in_room );
    act("$i1 падает на землю.",ch,obj,victim,TO_CHAR);
    act("$i1 падает на землю.",ch,obj,victim,TO_VICT);
    act("$i1 падает на землю.",ch,obj,victim,TO_NOTVICT);
   }
   else
   {
    extract_obj( obj );
    act("$i1 ломается от броска.",ch,obj,victim,TO_CHAR);
    act("$i1 ломается от броска.",ch,obj,victim,TO_VICT);
    act("$i1 ломается от броска.",ch,obj,victim,TO_NOTVICT);
   }
  }
 }
} 
 
void do_dirt( CHAR_DATA *ch, const char *argument ) 
{ 
  OBJ_DATA *rhand, *lhand; 
  char arg[MAX_STRING_LENGTH]; 
  CHAR_DATA *victim; 
  int chance; 
 
  one_argument(argument,arg); 
 
  if (!can_attack(ch,1)) return; 
 
  if ( (chance = get_skill(ch,gsn_dirt)) == 0 
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT)) 
    ||   (!IS_NPC(ch) 
    &&    (!IS_NPC(ch) && (!check_skill(ch,gsn_dirt))))) 
  { 
    stc("Ты только испачкал ногу.\n\r",ch);
    cant_mes (ch); 
    return; 
  } 
 
  if (arg[0] == '\0') 
  { 
    victim = ch->fighting; 
    if (victim == NULL) 
    { 
      stc("Hо ты не сpажаешься!\n\r",ch); 
      return; 
    } 
  } 
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Они не здесь.\n\r",ch); 
    return; 
  } 
 
  if (IS_AFFECTED(victim,AFF_BLIND)) 
  { 
    act("{y$O{x уже ослеплен.",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Очень смешно.\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 
 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Hо {y$C1{x твой хоpоший дpуг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
 
  if (IS_SET(ch->act,PLR_TIPSY)) // tipsy by Dinger 
   if (tipsy(ch,"dirt")) return; 
 
  /* dexterity */ 
  chance += (get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_DEX))*2; 
 
  /* speed  */ 
  if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE)) chance += 10; 
  if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE)) chance -= 25; 
 
  /* level */ 
  chance += (ch->level - victim->level) * 2; 
  
  /* flying fix by Dino */
  if (IS_AFFECTED(ch, AFF_FLYING))
     chance -= 30;
 
  /* check for shield */
  rhand=get_eq_char(ch,WEAR_RHAND); 
  lhand=get_eq_char(ch,WEAR_LHAND); 
  if ((rhand && CAN_WEAR(rhand,ITEM_WEAR_SHIELD)) || (lhand && CAN_WEAR(lhand, ITEM_WEAR_SHIELD)))
    chance -= 5;
  if ((rhand && CAN_WEAR(rhand,ITEM_WEAR_SHIELD)) && (lhand && CAN_WEAR(lhand, ITEM_WEAR_SHIELD)))
    chance -= 30;

  rhand=get_eq_char(victim,WEAR_RHAND); 
  lhand=get_eq_char(victim,WEAR_LHAND); 
  if ((rhand && CAN_WEAR(rhand,ITEM_WEAR_SHIELD)) || (lhand && CAN_WEAR(lhand, ITEM_WEAR_SHIELD)))
    chance -= 20;
  if ((rhand && CAN_WEAR(rhand,ITEM_WEAR_SHIELD)) && (lhand && CAN_WEAR(lhand, ITEM_WEAR_SHIELD)))
    chance -= 5;



  /* sloppy hack to prevent false zeroes */ 
  if (chance % 5 == 0) chance += 1; 
 
  /* terrain */ 
  switch(ch->in_room->sector_type) 
  { 
    case(SECT_INSIDE):      chance -= 5; break; 
    case(SECT_CITY):                     break; 
    case(SECT_FIELD):       chance += 5; break; 
    case(SECT_FOREST):                   break; 
    case(SECT_HILLS):                    break; 
    case(SECT_MOUNTAIN):    chance -= 10;break; 
    case(SECT_WATER_SWIM):  chance  = 0;break; 
    case(SECT_WATER_NOSWIM):chance  = 0;break; 
    case(SECT_AIR):         chance  = 0;break; 
    case(SECT_DESERT):      chance += 10;break; 
  } 

 
  if (chance == 0) 
  { 
    stc("Hе всем можно пользоваться в качестве гpязи(dirt).\n\r",ch); 
    return; 
  } 
 
  /* now the attack */ 
  check_criminal(ch,victim,60); 
  if (number_percent() < chance) 
  { 
    AFFECT_DATA af; 
    act("Грязь залепила глаза {y$c2{x!",victim,NULL,NULL,TO_ROOM); 
    act("{y$c1{x кидает гpязь тебе в глаза!",ch,NULL,victim,TO_VICT); 
    damage(ch,victim,number_range(2,5),gsn_dirt,DAM_NONE,FALSE,FALSE, NULL); 
    stc("Ты ничего не видишь!\n\r",victim); 
    check_improve(ch,gsn_dirt,TRUE,2); 
//    if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 5);
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 1);
    WAIT_STATE(ch,skill_table[gsn_dirt].beats); 
 
    af.where        = TO_AFFECTS; 
    af.type         = gsn_dirt; 
    af.level        = ch->level; 
    af.duration     = 0; 
    af.location     = APPLY_HITROLL; 
    af.modifier     = -4; 
    af.bitvector    = AFF_BLIND; 
    affect_to_char(victim,&af); 
  } 
  else 
  { 
    damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE,FALSE, NULL); 
    check_improve(ch,gsn_dirt,FALSE,2); 
    WAIT_STATE(ch,skill_table[gsn_dirt].beats); 
  } 
} 
 
void do_trip( CHAR_DATA *ch, const char *argument ) 
{ 
  char arg[MAX_STRING_LENGTH]; 
  CHAR_DATA *victim; 
  int chance; 
 
  one_argument(argument,arg); 
 
  if (!can_attack(ch,1)) return; 
 
  if ( (chance = get_skill(ch,gsn_trip)) == 0 
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP)) 
    ||   (!IS_NPC(ch) && (!check_skill(ch,gsn_trip)))) 
  { 
    stc("Подсечь?  Как?\n\r",ch);        cant_mes (ch); 
    return; 
  } 
 
  if (arg[0] == '\0') 
  { 
    victim = ch->fighting; 
    if (victim == NULL) 
    { 
      stc("Hо ты ни с кем не сpажаешься!\n\r",ch); 
      return; 
    } 
  } 
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Они не здесь.\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 
 
    
  if (IS_AFFECTED(victim,AFF_FLYING)) 
  { 
    act("{y$G{x ноги не на земле.",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
 
  if (victim->position < POS_FIGHTING) 
  { 
    act("{y$C1{x уже на земле.",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Твое лицо становится кислым!\n\r",ch); 
    WAIT_STATE(ch,2 * skill_table[gsn_trip].beats); 
    act("{y$c1{x спотыкается об свои собственные ноги!",ch,NULL,NULL,TO_CHAR); 
    return; 
  } 
 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("{y$C1{x твой возлюбленный учитель.",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
 
  if (ch->size < victim->size) 
        chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */ 
 
  chance += (get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_DEX)) * 2; 
 
  if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE)) 
        chance += 10; 
  if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE)) 
        chance -= 20; 
 
  chance += (ch->level - victim->level) * 2; 
 
  check_criminal(ch,victim,60); 
  if (number_percent() < chance) 
  { 
    act("{y$c1{x подсекает тебя и ты падаешь!",ch,NULL,victim,TO_VICT); 
    act("Ты подсекаешь {y$C4{x и {y$O{x падает!",ch,NULL,victim,TO_CHAR); 
    act("{y$c1{x подсекает {y$C4{x, отпpавляя $G на землю.",ch,NULL,victim,TO_NOTVICT); 
//    if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 4);
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 1);
    check_improve(ch,gsn_trip,TRUE,1); 
 
    DAZE_STATE(victim,2 * PULSE_VIOLENCE); 
    WAIT_STATE(ch,skill_table[gsn_trip].beats); 
    WAIT_STATE(victim,PULSE_VIOLENCE/2);
    victim->position = POS_RESTING; 
    if(victim->move>1) victim->move /= 2; 
    damage(ch,victim,number_range(2 ,victim->level/3 + 4 * victim->size),gsn_trip, 
      DAM_BASH,TRUE,FALSE, NULL); 
  } 
  else 
  { 
    damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE,FALSE, NULL); 
    WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3); 
    check_improve(ch,gsn_trip,FALSE,1); 
  }  
} 
 
void do_kill( CHAR_DATA *ch, const char *argument ) 
{ 
  char arg[MAX_STRING_LENGTH]; 
  CHAR_DATA *victim; 
 
  one_argument( argument, arg ); 
 
  if (!can_attack(ch,1)) return; 
 
  if ( arg[0] == '\0' ) 
  { 
    stc( "Убить кого?\n\r", ch ); 
    return; 
  } 
 
  if ( ( victim = get_char_room( ch, arg ) ) == NULL ) 
  { 
    stc( "Они не здесь.\n\r", ch ); 
    return; 
  } 
 
/* Allow player killing */ 
 
  if (!IS_NPC(victim)) 
  { 
    if (!IS_SET(victim->act, PLR_WANTED) || ch->criminal<100) 
    { 
      stc( "Ты должен УБИТЬ(murder) игpока.\n\r", ch ); 
      return; 
    } 
  } 
 
  if ( victim == ch ) 
  { 
    stc( "Ты бьешь сам себя.  Ой!\n\r", ch ); 
    multi_hit( ch, ch); 
    return; 
  } 
 
  if ( is_safe( ch, victim ) ) return; 
 
  if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim ) 
  { 
    act( "{y$C1{x твой любимый учитель.", ch, NULL, victim, TO_CHAR ); 
    return; 
  } 
 
  if ( ch->position == POS_FIGHTING ) 
  { 
    stc( "Лучшее что ты можешь сделать!\n\r", ch ); 
    return; 
  } 
 
  if (IS_SET(ch->act,PLR_TIPSY)) // tipsy by Dinger 
   if (tipsy(ch,"kill")) return; 
 
  WAIT_STATE( ch, 1 * PULSE_VIOLENCE ); 
  check_criminal( ch, victim ,60); 
  

  multi_hit( ch, victim); 
  if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) ) 
        mp_percent_trigger( victim, ch, NULL, NULL, TRIG_KILL ); 
} 
 
void do_murder( CHAR_DATA *ch, const char *argument ) 
{ 
  char buf[MAX_STRING_LENGTH]; 
  char arg[MAX_INPUT_LENGTH]; 
  CHAR_DATA *victim; 
 
  one_argument( argument, arg ); 
 
  if (!can_attack(ch,1)) return; 
 
  if ( arg[0] == '\0' ) 
  { 
    stc( "Убить кого?\n\r", ch ); 
    return; 
  } 
 
  if (IS_AFFECTED(ch,AFF_CHARM) || (IS_NPC(ch) && IS_SET(ch->act,ACT_PET))) return; 
 
  if ( ( victim = get_char_room( ch, arg ) ) == NULL ) 
  { 
    stc( "Тут таких нет.\n\r", ch ); 
    return; 
  } 
 
  if ( victim == ch ) 
  { 
    stc( "Самоубийство это смеpтельный гpех.\n\r", ch ); 
    return; 
  } 
 
  if ( is_safe( ch, victim ) ) return; 
 
  if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim ) 
  { 
    act( "{y$C1{x твой любимый хозяин.", ch, NULL, victim, TO_CHAR ); 
    return; 
  } 
 
  if ( ch->position == POS_FIGHTING ) 
  { 
    stc( "Лучшее что ты можешь сделать!\n\r", ch ); 
    return; 
  } 
 
  WAIT_STATE( ch, 1 * PULSE_VIOLENCE ); 
  
  if (IS_NPC(ch)) 
     do_printf(buf, "{mПомогите!{x На меня напал%s %s!",(ch->sex == 2) ? "a" : "",get_char_desc(ch,'1')); 
  else 
     do_printf( buf, "{mПомогите!{x  На меня напал%s %s!",(ch->sex == 2) ? "a" : "", ch->name ); 
  do_function(victim, &do_yell, buf ); 
  check_criminal( ch, victim , 60); 
  multi_hit( ch, victim); 
} 
 
void do_backstab( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int chance;
  bool rgh=TRUE;

  one_argument( argument, arg );
  if (!can_attack(ch,1)) return;
  if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BACKSTAB))
  {
    cant_mes (ch);
    return;
  }
  if (!IS_NPC(ch) && (!check_skill(ch,gsn_backstab)))
  {
    stc("Ась?\n\r",ch);
    return;
  }
  if (ch->fighting || ch->position==POS_FIGHTING)
  {
    stc ("Ты уже сражаешься!\n\r",ch);
    return;
  }
  if (EMPTY(arg))
  {
    stc("Ударить в спину кого?\n\r",ch);
    return;
  }
  if ((victim = get_char_room(ch,arg)) == NULL)
  {
    stc("Тут таких нет.\n\r",ch);
    return;
  }
  if ( victim == ch )
  {
    stc( "Ты подкрадываешься к себе сзади...и понимаешь, что выглядишь глупо.\n\r", ch );
    return;
  }
  if (is_safe( ch, victim ) ) return;
  if (!IS_NPC(victim)) ISORDEN(ch);


  if ((obj=get_eq_char(ch,WEAR_RHAND))!=NULL)
  {
    if (obj->item_type!=ITEM_WEAPON || attack_table[obj->value[3]].damage!=DAM_PIERCE)
     {
      obj=NULL;
      rgh=FALSE;
     }
  }
  if (!obj)
  {
    obj=get_eq_char(ch,WEAR_LHAND);
    if (!obj || obj->item_type!=ITEM_WEAPON || attack_table[obj->value[3]].damage!=DAM_PIERCE)
    obj=NULL;
  }
  if (!obj)
  {
    if (!IS_NPC(ch)) stc( "Тебе нужно деpжать оpужие в pуках для использования backstab.\n\r",ch );
    return;
  }

  if ( ch->position>POS_SLEEPING && !IS_NPC(ch) && ((victim->hit < victim->max_hit/3) || IS_AFFECTED(ch,AFF_CHARM)))
  {
    act("{y$C1{x пpистально наблюдает за тобой, ты не можешь подкpасться незаметно.",
         ch,NULL,victim,TO_CHAR);
    return;
  }
  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"backstab")) return;

  // check for the zombie's rescue
  if (!IS_NPC(victim) && victim->pet && 
      IS_SET(victim->pet->act,ACT_EXTRACT_CORPSE) && 
      victim->pet->in_room == victim->in_room)
  {
    chance = 20+(victim->pet->level-ch->level);
    if (number_percent() < chance)
    {
      act("Ты заслоняешь собой {y$C4{x", victim->pet, NULL, victim, TO_CHAR);
      act("$c1 заслоняет тебя своим телом!", victim->pet, NULL, victim, TO_VICT);
      act("$c1 заслоняет собой {y$C4{x!", victim->pet, NULL, victim, TO_NOTVICT);
      victim=victim->pet;
    }
  }
  check_criminal(ch,victim,60);
  WAIT_STATE( ch, skill_table[gsn_backstab].beats );

  chance=get_skill(ch,gsn_backstab)/2;
  chance+=(get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_DEX))*5;
  chance+=(victim->size-ch->size)*12;
  chance+=category_bonus(ch,PERCEP)*5;
  if (IS_NPC(victim) && !IS_NPC(ch)) chance+=15+ch->level/2;

  if (number_percent() < chance ||
   (get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim)))
  {
    check_improve(ch,gsn_backstab,TRUE,1);
    one_hit( ch, victim, gsn_backstab, NULL, rgh );
    if (IS_AFFECTED(ch,AFF_HASTE))
    {
      if(IS_AFFECTED(victim, AFF_FIRESHIELD)
        && number_percent()<50)
      {
        act("{y$c1{x обжигается о твой огненный щит, и с визгом отскакивает!",ch,NULL,victim,TO_VICT);
        act("Ты обжигаешься об огненный щит, окружающий {y$C4{x, и отскакиваешь!",ch,NULL,victim,TO_CHAR);
        act("{y$c1{x обжигается об огненный щит, окружающий {y$C4{x, и с визгом отскакивает.",ch,NULL,victim,TO_NOTVICT);
        WAIT_STATE(ch,skill_table[gsn_backstab].beats*3/2);
        return;
      }
      else if (!IS_AWAKE(victim) || !GUILD(ch,ASSASIN_GUILD) || !is_offered(victim))
//      if( IS_STEALTH_DEITY(ch) )
//      {
//        change_favour(ch, 5);
//        if( !IS_NPC(victim) ) change_favour(ch, 10);
//      }
//      if( IS_FANATIC_DEITY(ch) ) change_favour(ch, 15);
      one_hit(ch,victim,gsn_backstab, NULL, rgh  );
    }
  }
  else
  {
    check_improve(ch,gsn_backstab,FALSE,1);
    damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE,FALSE, NULL);
  }
}
 
void do_flee( CHAR_DATA *ch, const char *argument ) 
{ 
  char buf[MAX_STRING_LENGTH]; 
  char arg[MAX_INPUT_LENGTH]; 
  ROOM_INDEX_DATA *was_in; 
  ROOM_INDEX_DATA *now_in; 
  CHAR_DATA *victim; 
  int attempt,door; 
  EXIT_DATA *pexit; 
 
  if ( ( victim = ch->fighting ) == NULL ) 
  { 
    if ( ch->position == POS_FIGHTING ) ch->position = POS_STANDING; 
    stc( "Ты ни с кем не сpажаешься.\n\r", ch ); 
    return; 
  } 
 
  if (IS_SET(ch->in_room->room_flags, ROOM_NOFLEE)) 
  { 
    stc("Ты не можешь убежать отсюда.\n\r",ch); 
    return; 
  } 

  if ( IS_SET(ch->in_room->ra, RAFF_WEB) && (number_range(1,8) < 7) ) 
  { 
    stc("Магическая паутина удерживает твои ноги, не давая убежать!\n\r",ch); 
    WAIT_STATE(ch,12);
    return; 
  } 

  was_in = ch->in_room; 
 
  one_argument(argument,arg); 
  // Thiefs can flee in selected direction
  if ( arg[0] != '\0' && (IS_NPC(ch) || ch->classthi==1)) 
  { 
   if ( !str_prefix( arg, "north" )) door = 0; 
   else if (!str_prefix( arg, "east" )) door = 1; 
   else if (!str_prefix( arg, "south")) door = 2; 
   else if (!str_prefix( arg, "west" )) door = 3; 
   else if (!str_prefix( arg, "up"   )) door = 4; 
   else if (!str_prefix( arg, "down" )) door = 5; 
   else 
   { 
     stc( "Куда ты хочешь сбежать?\n\r", ch ); 
     return; 
   } 
 
   if ( ( pexit = was_in->exit[door] ) == 0 || pexit->u1.to_room == NULL 
     || (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(race_table[ch->race].spec,SPEC_PASSFLEE))
     || number_range(1,ch->daze*2) > 10)
   {
     stc("Ты не можешь убежать сюда.\n\r",ch);
     return;
   }

   do_move_char( ch, door, FALSE, FALSE );
   if ( ( now_in = ch->in_room ) == was_in )
   {
     stc( "Ты В ПАHИКЕ! Попытка сбежать неудалась!\n\r", ch );
     return;
   }
   ch->in_room = was_in;
   if (!IS_NPC(ch) && !IS_SET(ch->pcdata->guild,ASSASIN_GUILD)) do_printf(buf,"{y$c1{x убегает на %s!",dir_name[door]);
    else do_printf(buf,"{y$c1{x убегает!");
    act( buf, ch, NULL, NULL, TO_ROOM );
    ch->in_room = now_in;

   do_stop_fighting(ch,was_in);
   stc( "Ты спасаешься бегством!\n\r", ch );
   if( number_percent() < 3*(ch->level/2))
   {
     stc( "Ты незаметно ускользнул.\n\r", ch);
//     if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 1);
   }
   else
   { 
//     if( IS_STEALTH_DEITY(ch) ) change_favour(ch, -5);
     stc( "Ты теpяешь 10 exp.\n\r", ch);
     gain_exp( ch, -10 ); 
   } 

   if (!IS_IMMORTAL(ch) && !IS_AFFECTED(ch,AFF_BERSERK) && number_percent() > 95 )
   {
      AFFECT_DATA af;
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("fear");
      af.level     = ch->level;
      af.duration  = 0;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      stc("Ты в ужасе!\n\r",ch);
   }
   WAIT_STATE(ch,1);
   return; 
 } 
 
 // if arg==NULL, select randomize direction for flee
 for ( attempt = 0; attempt < 6; attempt++ ) 
 { 
   door = number_door( ); 
   if ( ( pexit = was_in->exit[door] ) == 0 
   || pexit->u1.to_room == NULL 
   || (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(race_table[ch->race].spec,SPEC_PASSFLEE))
   || number_range(0,ch->daze) > 7 
   || (IS_NPC(ch) && IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)))
      continue; 
 
   do_move_char( ch, door, FALSE, FALSE ); 
   if ( ( now_in = ch->in_room ) == was_in ) continue; 
 
   ch->in_room = was_in; 
   if (!IS_AFFECTED(ch, AFF_SNEAK)) do_printf(buf,"{y$c1{x убегает на %s!",dir_name[door]); 
   else do_printf(buf,"{y$c1{x убегает!"); 
   act( buf, ch, NULL, NULL, TO_ROOM ); 
   ch->in_room = now_in; 
   do_stop_fighting(ch,was_in);
 
   if ( !IS_NPC(ch) ) 
   { 
     stc( "Ты спасаешься бегством!\n\r", ch ); 
     if( (ch->classthi == 1) && (number_percent() < 3*(ch->level/2) ) )
     {
       stc( "Ты незаметно ускользнул.\n\r", ch);
//       if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 1);
     }
     else 
     { 
//       if( IS_STEALTH_DEITY(ch) ) change_favour(ch, -5);
       stc( "Ты теpяешь 10 exp.\n\r", ch);  
       gain_exp( ch, -10 ); 
     }

   if (!IS_IMMORTAL(ch))
    if (!IS_AFFECTED(ch,AFF_BERSERK)) 
     if (number_percent()>85)
     {
       AFFECT_DATA af;
       af.where     = TO_AFFECTS;
       af.type      = skill_lookup("fear");
       af.level     = ch->level;
       af.duration  = 0;
       af.location  = APPLY_NONE;
       af.modifier  = 0;
       af.bitvector = 0;
       affect_to_char( ch, &af );
       stc("Ты в ужасе!\n\r",ch);
     }
   }
//   if( IS_FANATIC_DEITY(ch) ) change_favour(ch, -3);
   return; 
 } 
 stc( "Ты ПАHИКУЕШЬ! Тебе некуда бежать!\n\r", ch ); 
} 
 
void do_rescue( CHAR_DATA *ch, const char *argument ) 
{ 
  char arg[MAX_INPUT_LENGTH]; 
  CHAR_DATA *victim; 
  CHAR_DATA *fch; 
 
  one_argument( argument, arg ); 
 
  if ( arg[0] == '\0' ) 
  { 
    stc( "Спасать кого?\n\r", ch ); 
    return; 
  } 
 
  if ( ( victim = get_char_room( ch, arg ) ) == NULL ) 
  { 
    stc( "Тут таких нет.\n\r", ch ); 
    return; 
  } 
 
  if ( victim == ch || ch->fighting == victim ) 
  { 
    stc( "Это смешно.\n\r", ch ); 
    return; 
  } 
 
  if ( ( fch = victim->fighting ) == NULL ) 
  { 
    stc( "Этот пеpсонаж не дерется.\n\r", ch ); 
    return; 
  } 
 
  if ( IS_NPC(fch) && !is_same_group(ch,victim)) 
  { 
    stc("Ты не можешь его спасать.\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,fch)) 
  { 
    stc("Нельзя.\n\r",ch); 
    return; 
  } 

/*
  if (!IS_NPC(fch) && !PK_RANGE(ch,fch))
  {
    stc("Не лезь в детские разборки.\n\r",ch);
    return;
  }
*/

  WAIT_STATE( ch, skill_table[gsn_rescue].beats ); 
  if ( number_percent( ) > get_skill(ch,gsn_rescue)+3*(category_bonus(ch,PROTECT)-category_bonus(ch,OFFENCE))) 
  { 
    stc( "У тебя не получилось.\n\r", ch ); 
    check_improve(ch,gsn_rescue,FALSE,1); 
    return; 
  } 
 
  act( "Ты спасаешь {y$C4{x!",  ch, NULL, victim, TO_CHAR    ); 
  act( "{y$c1{x спасает тебя!", ch, NULL, victim, TO_VICT    ); 
  act( "{y$c1{x спасает {y$C4{x!",  ch, NULL, victim, TO_NOTVICT ); 
  check_improve(ch,gsn_rescue,TRUE,1); 
 
  stop_fighting( fch, FALSE ); 
  stop_fighting( victim, FALSE ); 
  check_criminal( ch, fch , 30); 
  set_fighting( ch, fch ); 
  set_fighting( fch, ch );
//  if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 10);
} 
 
void do_kick( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim=NULL;
 
 
  if (!IS_NPC(ch) && (!check_skill(ch,gsn_kick))) 
  { 
    stc("Твои ноги запутываются и ты падаешь...\n\r", ch ); 
    return; 
  } 
 
  if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK)) 
  { 
    cant_mes (ch); 
    return; 
  } 
 
  if (IS_SET(race_table[ch->race].spec,SPEC_DKICK))
   if (!EMPTY(argument)) victim=get_char_room(ch,argument);
  if (!victim) victim = ch->fighting;
  if (!victim)
  { 
    stc( "Ты ни с кем не сpажаешься.\n\r", ch ); 
    return; 
  } 
 
  WAIT_STATE( ch, skill_table[gsn_kick].beats ); 
  check_criminal( ch, victim , 60); 
  if ( get_skill(ch,gsn_kick) > number_percent()) 
  { 
    int dam_kick =(ch->level)*2; 
    dam_kick+=GET_DAMROLL(ch)*get_skill(ch,gsn_kick)/100;
    dam_kick*=2*(ch->size + 1) / (victim->size + 1);

    if (IS_SET(race_table[ch->race].spec,SPEC_DKICK)) 
    {
        damage(ch,victim,number_range( dam_kick/2, 3*dam_kick/2 ), gsn_kick,DAM_BASH,TRUE,FALSE, NULL);
        check_improve(ch,gsn_kick,TRUE,1);
        damage(ch,victim,number_range( dam_kick/2, 3*dam_kick/2 ), gsn_kick,DAM_BASH,TRUE,FALSE, NULL);
        check_improve(ch,gsn_kick,TRUE,1);
        if( IS_MIGHT_DEITY(ch) ) 
            change_favour(ch, 5);
    }
    else
    {
        damage(ch,victim,number_range( 1, dam_kick ), gsn_kick,DAM_BASH,TRUE,FALSE, NULL);
        check_improve(ch,gsn_kick,TRUE,1);
//        if( IS_MIGHT_DEITY(ch) ) 
//            change_favour(ch, 2);
    }
  }
  else
  { 
    damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE,FALSE, NULL);
    if (IS_SET(race_table[ch->race].spec,SPEC_DKICK)) damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE,FALSE, NULL);
    check_improve(ch,gsn_kick,FALSE,1);
  } 
} 
 
void do_disarm( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim; 
  OBJ_DATA *obj=NULL, *wpn=NULL;
  int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon; 
  bool fLeft=FALSE;
  bool ch_whip=FALSE, vict_whip=FALSE;
  
  hth = 0; 
 
  if ((chance = get_skill(ch,gsn_disarm)) == 0) 
  { 
    stc( "Ты не знаешь как обезоpужить пpотивника.\n\r", ch ); 
    return; 
  } 

  if ( get_eq_char( ch, WEAR_RHAND ) == NULL  
   && ((hth = get_skill(ch,gsn_hand_to_hand)) == 0 
   || (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM)))) 
  { 
    stc( "У тебя должно быть оpужие чтобы обезоpуживать других.\n\r", ch );    cant_mes (ch); 
    return; 
  } 
 
  if ( ( victim = ch->fighting ) == NULL ) 
  { 
    stc( "Ты ни с кем не сpажаешься.\n\r", ch ); 
    return; 
  } 
 
  if (!EMPTY(argument) && !str_prefix(argument,"left")) fLeft=TRUE;

  if (fLeft==FALSE) obj=get_eq_char(victim,WEAR_RHAND);
  if (fLeft==TRUE || obj ==NULL) obj=get_eq_char(victim,WEAR_LHAND);

  if (obj==NULL || obj->item_type!=ITEM_WEAPON)
  { 
    stc( "Твой пpотивник безоpужен.\n\r", ch ); 
    return; 
  } 
 
  // find weapon skills
  ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch, TRUE)); 
  vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim, TRUE)); 
  ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim, TRUE)); 
 
  if ( (get_eq_char(ch,WEAR_RHAND) == NULL)
     && (get_eq_char(ch,WEAR_LHAND)==NULL)) chance = chance * hth/150; 
  else chance = chance * ch_weapon/100; 

  // Випом дизармить проще...
  if ((wpn = get_eq_char(ch,WEAR_RHAND)) && wpn->item_type==ITEM_WEAPON && wpn->value[0] == WEAPON_WHIP) {chance += 11; ch_whip=TRUE;}
  else if ((wpn = get_eq_char(ch,WEAR_LHAND)) && wpn->item_type==ITEM_WEAPON && wpn->value[0] == WEAPON_WHIP) {chance += 11; ch_whip=TRUE;}

  // И вип дизармить проще.. (c) Apc
  if (obj->item_type==ITEM_WEAPON && obj->value[0] == WEAPON_WHIP) {chance += 11; vict_whip=TRUE;}

  if (ch_whip && vict_whip && number_percent() < 23)
  {
    WAIT_STATE( ch, skill_table[gsn_disarm].beats ); 
    WAIT_STATE( victim, skill_table[gsn_disarm].beats ); 

    disarm( ch, victim, fLeft ); 
    disarm( victim, ch, fLeft ); 
    doact("Плети {y$c2 и {y$C2{x переплетаются и выпадают из рук!.",ch,NULL,victim,TO_NOTVICT,SPAM_SKMISS); 
    doact("Твоя плеть переплетается с $i5! и выпадают обе!.",ch,obj,victim,TO_CHAR,SPAM_SKMISS); 
    doact("Твоя плеть переплетается с $i5! и выпадают обе!.",ch,wpn,victim,TO_VICT,SPAM_SKMISS); 
    check_improve(ch,gsn_disarm,FALSE,1); 
    return;
  }
  chance += (ch_vict_weapon/2 - vict_weapon) / 2;  
  chance += get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_STR); 
  chance += (ch->level - victim->level) * 2; 
  
  if (number_percent() < chance) 
  { 
    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    disarm( ch, victim, fLeft );
    check_improve(ch,gsn_disarm,TRUE,1);
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 5);
  } 
  else 
  { 
    WAIT_STATE(ch,skill_table[gsn_disarm].beats); 
    doact("Ты не смог обезоpужить {y$C4{x.",ch,NULL,victim,TO_CHAR,SPAM_SKMISS); 
    doact("{y$c1{x пытается обезоpужить тебя, но безуспешно.",ch,NULL,victim,TO_VICT,SPAM_SKMISS); 
    doact("{y$c1{x пытаеться обезоpужить {y$C4{x, но безуспешно.",ch,NULL,victim,TO_NOTVICT,SPAM_SKMISS); 
    check_improve(ch,gsn_disarm,FALSE,1); 
  } 
  check_criminal( ch, victim , 60); 
} 


// *** Shield cleave absolutely rewritten (C) Astellar & Kardi
// Idea (c) Apc & Thor
void do_cleave( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   int chance,koef;
   OBJ_DATA *weapon,*shield;

   if (IS_NPC(ch)) return;

   if ((victim = ch->fighting) == NULL)
   {
     stc("Но ты ни с кем не сражаешься!\n\r", ch);
     return;
   }

   if ( !get_skill(ch, gsn_cleave) )
   {
     stc("Ты не представляешь, как можно разрубить щит.\n\r", ch);
     return;
   }

  weapon = get_eq_char( ch, WEAR_RHAND );

  if (weapon==NULL || weapon->item_type!=ITEM_WEAPON 
  || ( weapon->value[0]!=WEAPON_AXE && weapon->value[0]!=WEAPON_MACE) )
  { 
    weapon = get_eq_char( ch, WEAR_LHAND );
 
    if( weapon==NULL || weapon->item_type!=ITEM_WEAPON 
    || ( weapon->value[0]!=WEAPON_AXE && weapon->value[0]!=WEAPON_MACE) )
    {
      stc("Ты не можешь разрубить щит этим оружием.\n\r", ch);
      return;
    }
  }
 
  shield = get_eq_char( victim, WEAR_LHAND );  
  if (shield==NULL || !CAN_WEAR(shield, ITEM_WEAR_SHIELD)) 
  {
    shield = get_eq_char( victim, WEAR_RHAND );
    if (shield==NULL || !CAN_WEAR(shield, ITEM_WEAR_SHIELD)) 
    {
      stc ("Твой противник не использует щит.\n\r", ch);
      return;
    }
  }

  koef = get_curr_stat(victim,STAT_DEX)-get_curr_stat(ch,STAT_DEX);
  koef = koef/2;
  chance = get_skill (ch, gsn_cleave);
  chance = chance*3/5;
//  if (weapon->value[0] == WEAPON_MACE) chance -= chance/10;
  chance -= get_skill(victim,gsn_dodge)/20;
  chance -= koef;
  if (IS_SET(race_table[victim->race].spec,SPEC_DODGE)) chance -=5;

  if( IS_CFG( ch, CFG_AUTODAM) && IS_ELDER(ch) )
    ptc( ch,"{RChance to hit: %d.\n\r", chance );

  if( chance < number_percent() )
  {
    if( !IS_ELDER(ch) ) WAIT_STATE(ch,skill_table[gsn_cleave].beats);
    act("Твое оружие проскальзывает по щиту $C2.",ch,NULL,victim,TO_CHAR);
    act("$n попытался разбить твой щит, но промахнулся.",ch,NULL,victim,TO_VICT);
    act("$n попытался разбить щит $C2, но промахнулся.",ch,NULL,victim,TO_NOTVICT);
    damage_obj( ch, weapon, 3, 1 );
    check_improve(ch,gsn_cleave,FALSE,1);
    return;
  } 

  chance = get_curr_stat(ch,STAT_STR) - 23;
  if (weapon->value[0] == WEAPON_AXE)
   chance += race_table[ch->race].weapon_bonus[WEAPON_AXE]*2;
  else chance -=5;

  koef = material_table[material_num(weapon->material)].hard;
  koef = koef - material_table[material_num(shield->material)].hard;
  chance += koef/5;

  koef = material_table[material_num(weapon->material)].d_dam;
  koef = koef - material_table[material_num(shield->material)].d_dam;
  chance += koef/50;

  if (ch->race==RACE_DWARF) chance +=5;

  chance += ( ch->level - victim->level) / 2;

  chance += ( category_bonus(ch, FORTITUDE) 
              + category_bonus(ch, OFFENCE)
              - category_bonus(victim, PROTECT) );
  chance +=10;

  if( chance <= 0 ) chance = 1;

  if( IS_CFG( ch, CFG_AUTODAM) && IS_ELDER(ch) )
     ptc( ch,"{RChance to break: %d.\n\r", chance );

   if ( chance > number_percent() )
   {
      DAZE_STATE( victim, 5*PULSE_VIOLENCE );
      if( !IS_ELDER(ch) ) WAIT_STATE( ch, skill_table[gsn_cleave].beats );
      act("Ты сильным ударом разбиваешь щит $C2.",ch,NULL,victim,TO_CHAR);
      act("$n сильным ударом разбивает твой щит.",ch,NULL,victim,TO_VICT);
      act("$n сильным ударом разбивает щит $C2.",ch,NULL,victim,TO_NOTVICT);
      check_improve(ch,gsn_cleave,TRUE,1);
      damage_obj( victim, shield, shield->condition, 1);
//     if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 8);
   }
   else
   {
     if( !IS_ELDER(ch) ) WAIT_STATE(ch,skill_table[gsn_cleave].beats);
     act("Твоё оружие со звоном отскакивает от щита $C2.",ch,NULL,victim,TO_CHAR);
     act("Оружие $c2 со звоном отскакивает от твоего щита.",ch,NULL,victim,TO_VICT);
     act("Оружие $c2 со звоном отскакивает от щита $C2.",ch,NULL,victim,TO_NOTVICT);
     check_improve(ch,gsn_cleave,FALSE,1);

     chance = 35 - get_curr_stat(ch,STAT_STR);
     chance *= 2;
     if (ch->race==RACE_DWARF) chance -=5;
     damage_both_objs( ch, victim, weapon, shield, DAM_CLEAVE);

     if( !IS_ELDER(ch) && number_percent() < chance )
     {
       act("И вырывается из твоих рук.",ch,NULL,victim,TO_CHAR);
       act("И вырывается из рук $c2.",ch,NULL,victim,TO_VICT);
       act("И вырывается из рук $c2.",ch,NULL,victim,TO_NOTVICT);
       obj_from_char(weapon);
       if ( IS_OBJ_STAT(weapon,ITEM_NODROP) ) obj_to_char( weapon, ch ); 
       else obj_to_room( weapon, victim->in_room ); 
       WAIT_STATE(ch,2*PULSE_VIOLENCE);
     }
   }
   return;
}

void do_crush( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim; 
  char arg[MAX_INPUT_LENGTH]; 
//  int chance=50;
  int damage_crush; 
  int offence, defence;
 
  one_argument(argument,arg); 
 
  if (!can_attack(ch,1)) return; 
 
  if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_CRUSH)) 
  { 
    cant_mes (ch); 
    return; 
  } 
 
  if (!IS_SET(race_table[ch->race].spec,SPEC_CRUSH)) 
  {    
    stc("Ась?\n\r",ch); 
    return; 
  } 
 
  if (arg[0] == '\0') 
  { 
    victim = ch->fighting; 
    if (victim == NULL) 
    { 
      stc("Но ты ни с кем не сражаешься!\n\r",ch); 
      return; 
    } 
  } 
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Tут таких нет.\n\r",ch); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Ты необычайно мощным ударом пытаешься сломать себе кости...мазохист?!\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Но $C1 твой друг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
    
  if (IS_SET(ch->act,PLR_TIPSY)) // tipsy by Dinger 
   if (tipsy(ch,"crush")) return; 
 
  /* size  and weight */ 
  check_criminal(ch,victim,60); 
/*
  chance += ch->carry_weight / 50; 
  chance -= victim->carry_weight / 40; 
 
  if (ch->size != victim->size) chance += (ch->size - victim->size) *10; 
 
  chance += get_curr_stat(ch,STAT_STR)/2;
  chance += (get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_DEX))*5;
 
  if (IS_AFFECTED(victim,AFF_FLYING)) chance -= 10;
  if (IS_SET(ch->off_flags,OFF_FAST)) chance += 10;
  if (IS_SET(victim->off_flags,OFF_FAST)) chance -= 10;
 
  chance += (ch->level - victim->level) * 5;
*/

  offence = ch->level + (get_curr_stat(ch, STAT_STR) + get_curr_stat(ch, STAT_DEX)) * 2 + 15;
  offence += ch->size * 10;
  if (IS_SET(ch->off_flags,OFF_FAST))
    offence += 10;

  defence = victim->level + (get_curr_stat(victim, STAT_CON) + get_curr_stat(victim, STAT_DEX)) * 2;
  defence += victim->size * 10;
  if (IS_SET(ch->off_flags,OFF_FAST))
    defence += 10;
  if (IS_AFFECTED(victim, AFF_FLYING))
    defence += 10;
  if (IS_SET(race_table[victim->race].spec,SPEC_RESBASH))
    defence += 10;

//  ptc(ch, "offence [%d] VS defence [%d]\n\r\n\r", offence, defence);

  if (offence + dice(3,40)  >  defence + dice(3,40))
  {
    if (IS_AFFECTED(victim, AFF_FIRESHIELD) && number_percent()<25)
    {
      act("{y$c1{x обжигается о твой огненный щит, и с визгом отскакивает!",ch,NULL,victim,TO_VICT);
      act("Ты обжигаешься об огненный щит, окружающий {y$C4{x, и отскакиваешь!",ch,NULL,victim,TO_CHAR);
      act("{y$c1{x обжигается об огненный щит, окружающий {y$C4{x, и с визгом отскакивает.",
                 ch,NULL,victim,TO_NOTVICT);
      WAIT_STATE(ch,2*PULSE_VIOLENCE);
      return;
    }
    act("$c1 необычайно мощным ударом ломает тебе кости!",ch,NULL,victim,TO_VICT);
    act("Ты со всей силы врезаешься плечем в $C4, сокрушая $G!",ch,NULL,victim,TO_CHAR);
    act("$c1 необычайно мощным ударом ломает кости $C3.",ch,NULL,victim,TO_NOTVICT);
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 6);
    damage_crush = 10+dice(1+ch->level/30,(ch->damroll<600) ? ch->damroll/2 : 300);
    damage_crush-=damage_crush*victim->size/20;
    if (IS_SET(race_table[victim->race].spec,SPEC_RESBASH)) damage_crush/=2;
    damage(ch,victim,damage_crush,gsn_crush, DAM_BASH, TRUE, FALSE, NULL);
    WAIT_STATE(victim, number_range(1,2) * PULSE_VIOLENCE);
    WAIT_STATE(ch,2*PULSE_VIOLENCE);
  }
  else 
  { 
    damage(ch,victim,0,gsn_crush,DAM_BASH, TRUE, FALSE, NULL); 
    act("Ты промахиваешься и падаешь лицом на пол...",ch,NULL,victim,TO_CHAR); 
    act("$c1 промахивается и падает лицом на пол.",ch,NULL,victim,TO_NOTVICT); 
    act("Ты уходишь от мощного удара $n, заставляя$g упасть на землю.",ch,NULL,victim,TO_VICT); 
    ch->position = POS_RESTING;
    WAIT_STATE(ch, 3*PULSE_VIOLENCE);
  }
} 
 
void do_tail( CHAR_DATA *ch, const char *argument ) 
{ 
  char arg[MAX_INPUT_LENGTH]; 
  CHAR_DATA *victim; 
  int chance, wait; 
  int damage_tail; 
 
  if (!can_attack(ch,1)) return; 
 
  if (!IS_SET(race_table[ch->race].spec,SPEC_TAIL)
    || (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TAIL))) 
  {    
    stc("У тебя нет хвоста.\n\r",ch); 
    cant_mes (ch); 
    return; 
  } 
  
  one_argument(argument,arg); 

  chance = 100;
  if (EMPTY(arg))
  { 
    victim = ch->fighting; 
    if (!victim)
    { 
      stc("Но ты ни с кем не сражаешься!\n\r",ch); 
      return; 
    } 
  } 
  else if (!(victim = get_char_room(ch,arg))) 
  { 
    stc("Tут таких нет.\n\r",ch); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Ты пытаешься ударить себя хвостом, но у тебя ничего не выходит.\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 

  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Но $C1 твой друг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
    
  // size and weight
  chance -= ch->carry_weight / 20;
  chance += victim->carry_weight / 25;
 
  if (ch->size < victim->size) chance += (ch->size - victim->size) * 25;
  else chance += (ch->size - victim->size) * 10;
 
  // stats
  chance += get_curr_stat(ch,STAT_STR) +  get_curr_stat(ch,STAT_DEX); 
  chance -= get_curr_stat(victim,STAT_DEX) * 2; 
 
  if (IS_AFFECTED(ch,AFF_FLYING)) chance -= 10; 
  if (IS_AFFECTED(ch,AFF_HASTE)) chance += 20; 
  if (IS_AFFECTED(victim,AFF_HASTE)) chance -= 20; 
  if (IS_NPC(victim) && IS_SET(victim->off_flags,OFF_FAST)) chance-=30;
  if (IS_NPC(ch) && IS_SET(ch->off_flags,OFF_FAST)) chance+=20;
 
  chance += (ch->level - victim->level) * 2; 
  check_criminal(ch,victim,60); 
 
  if (number_percent() < (chance / 4))
  { 
    act("$c1 посылает тебя на землю взмахом хвоста!!",ch,NULL,victim,TO_VICT); 
    act("Ты посылаешь $C4 на землю взмахом хвоста!",ch,NULL,victim,TO_CHAR); 
    act("$c1 посылает $C4 на землю взмахом хвоста.",ch,NULL,victim,TO_NOTVICT); 
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 5); 
    wait = 1; 
 
    switch(number_bits(2)) 
    { 
      case 0: wait = 2; break; 
      case 1: wait = 3; break; 
      case 2: wait = 3; break; 
      case 3: wait = 2; break; 
    } 
    WAIT_STATE(victim, wait * PULSE_VIOLENCE);
    WAIT_STATE(ch,2*PULSE_VIOLENCE);
    victim->position = POS_RESTING;
    damage_tail = ((ch->damroll < 600) ? ch->damroll*2/3 : 400) + (2 * number_range(4,4 + 10* ch->size + chance/10) );
    damage(ch,victim,damage_tail,0, DAM_BASH, TRUE, FALSE, NULL);
  }
  else
  {
    damage(ch,victim,0,0,DAM_BASH, TRUE, FALSE, NULL);
    act("Ты потерял равновесие и упал на землю!",ch,NULL,victim,TO_CHAR);
    act("$c1 потерял равновесие и упал на землю!",ch,NULL,victim,TO_NOTVICT);
    act("Ты уклоняешься от хвоста $c2, посылая $G на землю.",ch,NULL,victim,TO_VICT);
    ch->position = POS_RESTING;
    WAIT_STATE(ch,2*PULSE_VIOLENCE);
  }
}

void do_slay( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  bool quite=FALSE;
 
  argument=one_argument( argument, arg ); 
  if ( arg[0] == '\0' ) 
  { 
    stc( "slay <{Yимя жертвы{x> [{Gdeaf{x]\n\r", ch );
    return; 
  } 
 
  if ( ( victim = get_char_room( ch, arg ) ) == NULL ) 
  {    stc( "Здесь нет таких.\n\r", ch ); 
    return; 
  } 
 
  if ( ch == victim ) 
  { 
    stc( "Самоубийство это смеpтельный гpех.\n\r", ch ); 
    return; 
  } 
 
  if( !IS_NPC(victim) && ( (victim->level >= get_trust(ch)) 
   || IS_NPC(ch) || (victim->pcdata->protect > get_trust(ch)) ) )
  { 
    stc( "Ты неудачник.\n\r", ch ); 
    return; 
  } 
  if (is_name(argument,"quite deaf hide")) quite=TRUE;

  if (!quite)
  {
    act( "Ты хладнокpовно убиваешь $G!",  ch, NULL, victim, TO_CHAR    ); 
    act( "{y$c1{x хладнокpовно убивает тебя!", ch, NULL, victim, TO_VICT    ); 
    act( "{y$c1{x хладнокpовно убивает $C4!",  ch, NULL, victim, TO_NOTVICT ); 
    log_printf("%s slays %s",ch->name,victim->name); 
  }
  else log_printf("%s quite slays %s",ch->name,victim->name); 

  if (IS_SET(ch->act, PLR_QUESTOR) && IS_NPC(victim)) 
  { 
    if (ch->questmob == victim) 
    { 
      stc("Ты почти выполнил свое задание!\n\r",ch); 
      stc("Веpнись к тому кто тебе его дал пока не поздно!\n\r",ch); 
      ch->questmob = NULL; 
      SET_BIT(ch->q_stat, Q_MOB_KILLED);
      raw_kill( victim );
      return; 
    } 
  } 
 
  if (IS_NPC(victim) && victim->questmob!=NULL) 
  { 
    stc("Некто убил твой квест...\n\r{*",ch);
    cancel_quest(victim->questmob,FALSE,6,10);
  } 
  raw_kill( victim ); 
} 
 
bool check_skill ( CHAR_DATA *ch, int gsn_skill) 
{ 
  if (!IS_NPC(ch) && ch->classmag==1 &&  
      ch->level>=skill_table[gsn_skill].skill_level[0]) return TRUE;       
  if (!IS_NPC(ch) && ch->classcle==1 &&  
      ch->level>=skill_table[gsn_skill].skill_level[1]) return TRUE;       
  if (!IS_NPC(ch) && ch->classthi==1 &&  
      ch->level>=skill_table[gsn_skill].skill_level[2]) return TRUE;       
  if (!IS_NPC(ch) && ch->classwar==1 &&  
      ch->level>=skill_table[gsn_skill].skill_level[3]) return TRUE;       
  return FALSE;  
} 
 
void cant_mes ( CHAR_DATA *ch) 
{ 
 if (IS_AFFECTED(ch,AFF_CHARM) && ch->master!=NULL) 
 { 
  if ( !IS_NPC(ch) || ch->race==RACE_HUMAN 
        || ch->race==RACE_DWARF  || ch->race==RACE_ELF 
        || ch->race==RACE_DROW   || ch->race==RACE_DRUID 
        || ch->race==RACE_HOBBIT || ch->race==RACE_VAMPIRE) 
   ptc(ch->master, "%s говорит тебе '{GИзвини, хозяин, но я не умею этого.{x'\n\r",get_char_desc(ch,'1')); 
   else do_function(ch, &do_emote, "озадаченно вертит головой, не в силах выполнить приказ."); 
 } 
} 
 
int check_victim(CHAR_DATA *ch, CHAR_DATA *victim) 
{ 
 int i,k,rez = MAX_VICT; 
 bool found = FALSE; 
 VICTIM_DATA temp, tmp; 
 temp.victim = NULL; 
 temp.dampool = 0; 
 tmp.victim = NULL; 
 tmp.dampool = 0; 
 
 if (!IS_NPC(victim) || IS_NPC(ch)) return rez; 
 
 for(i=0;i<MAX_VICT;i++) 
  if (ch->pcdata->victims[i].victim==victim) 
  { 
   found = TRUE; 
   break; 
  } 
 if (!found) 
 { 
  if (ch->pcdata->victims[0].victim==NULL) 
  { 
   ch->pcdata->victims[0].victim = victim; 
   ch->pcdata->victims[0].dampool = 0; 
  } 
  else  
  { 
   tmp.victim = victim; 
   tmp.dampool = 0; 
   for(i=0;i<MAX_VICT;i++) 
    if (tmp.victim == NULL) 
     break; 
    else 
    { 
     temp.victim = ch->pcdata->victims[i].victim; 
     temp.dampool = ch->pcdata->victims[i].dampool; 
     ch->pcdata->victims[i].victim = tmp.victim; 
     ch->pcdata->victims[i].dampool = tmp.dampool; 
     tmp.victim = temp.victim; 
     tmp.dampool = temp.dampool; 
    }  
  } 
 } 
 
 // пересортировка массива 
 temp.victim = NULL; 
 temp.dampool = 0; 
 for(i=0;i<MAX_VICT;i++) 
 { 
   if (ch->pcdata->victims[i].victim==ch->fighting) 
   { 
    temp.victim = ch->pcdata->victims[i].victim; 
    temp.dampool = ch->pcdata->victims[i].dampool; 
    for(k=i;k>0;k--) 
    { 
     ch->pcdata->victims[k].victim = ch->pcdata->victims[k-1].victim; 
     ch->pcdata->victims[k].dampool = ch->pcdata->victims[k-1].dampool; 
    } 
    ch->pcdata->victims[0].victim = temp.victim; 
    ch->pcdata->victims[0].dampool = temp.dampool; 
    break; 
   } 
 } 
 
 rez = 10; 
 // ищем чего возвращать 
 for(i=0;i<MAX_VICT;i++) 
  if (ch->pcdata->victims[i].victim==victim) 
  { 
   rez = i; 
   break; 
  } 
 
 return rez; 
} 
 
 
bool can_attack(CHAR_DATA *ch, int type)
{
 if (is_affected(ch,gsn_sleep))
 {
   stc("Ты же спишь!\n\r",ch);
   return FALSE;
 }

 if( is_affected(ch,gsn_gaseous_form) )
 {
   stc(" Бестелесность напоминает о себе...\n\r", ch);
   return FALSE;
 }

 if (IS_AFFECTED(ch,AFF_CALM) && ch->fighting==NULL)
 {
  if (type==1) stc("Ты не в том состоянии, чтобы нападать...\n\r",ch);
  else         stc("Ты не в том состоянии, чтобы колдовать...\n\r",ch);
  return FALSE;
 }

 if (is_affected(ch,skill_lookup("fear")) && ch->fighting==NULL)
 {
   if (type==1) stc("От страха у тебя дрожат руки и ноги, ты не можешь нападать.\n\r",ch);
   else         stc("От страха твой язык заплетается, ты не можешь произнести заклинание.\n\r",ch);
   return FALSE;
 }

 if ( IS_AFFECTED(ch,AFF_PEACE) && ch->fighting == NULL )
  {
    stc("У тебя слишком миролюбивое настроение.\n\r",ch); 
    return FALSE;
  }

 if ( IS_SET(ch->in_room->ra,RAFF_VIOLENCE) )
  return TRUE;

 return TRUE;
}

void char_death(CHAR_DATA *ch)
{
  OBJ_DATA *corpse;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  // AFFECT_DATA *charaf;
  char buf[MAX_STRING_LENGTH];
  int i;

  if (ch==NULL || IS_NPC(ch)) return;

  if (IS_SET(ch->act, PLR_SIFILIS)) REM_BIT(ch->act, PLR_SIFILIS);

  corpse          = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
  corpse->timer   = number_range( 25, 40 );
  corpse->owner = str_dup(ch->name);

  if (ch->gold > 0 || ch->silver > 0)
  {
   obj_to_obj(create_money(ch->gold, ch->silver), corpse);
   ch->gold  = 0;
   ch->silver = 0;
  }
   corpse->cost = 0;
   corpse->level = ch->level;

  // corpse name feature
  for(i=0;corpse_table[i].name!=NULL;i++) if (!str_cmp(ch->name,corpse_table[i].name)) break;

  if (corpse_table[i].name==NULL)
       do_printf(buf, corpse->short_descr, get_char_desc(ch,'2'));
  else do_printf(buf,corpse_table[i].short_n);

  free_string(corpse->short_descr);
  corpse->short_descr = str_dup(buf);

  if (corpse_table[i].name==NULL)
       do_printf(buf, corpse->description, get_char_desc(ch,'2'));
  else do_printf(buf,corpse_table[i].long_n);

  free_string(corpse->description);
  corpse->description = str_dup(buf);

  
  if (ch->race==RACE_ZOMBIE) { 
          /* sigh... replace for strcat(corpse->name, " zombie") (uni) */
          do_printf(buf, "%s zombie", corpse->name);
          free_string(corpse->name);
          corpse->name = str_dup(buf);
  }

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
      bool floating = FALSE;
  
      obj_next = obj->next_content;
  
      if (obj->wear_loc == WEAR_FLOAT)
        {
          floating = TRUE;
          unequip_char (ch, obj);
          obj_from_char (obj);
          act ("{y$p{x падает на землю.", ch, obj, NULL, TO_ROOM);
          obj_to_room (obj, ch->in_room);
        }
  
      if (IS_SET (obj->extra_flags, ITEM_ROT_DEATH))
        {
          obj_from_char (obj);
          act ("{y$p{x покрывается трещинами и распадается на осколки.", ch,
               obj, NULL, TO_ROOM);
          extract_obj (obj);
        }
  
      if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
        extract_obj (obj);
      else if (floating)
        {
          if (IS_OBJ_STAT (obj, ITEM_ROT_DEATH))        /* get rid of it! */
            {
              if (obj->contains != NULL)
                {
                  OBJ_DATA *in, *in_next;
                  act ("{y$p{x pаствоpяется, его содеpжимое вываливается.", ch,
                       obj, NULL, TO_ROOM);
                  for (in = obj->contains; in != NULL; in = in_next)
                    {
                      in_next = in->next_content;
                      obj_from_obj (in);
                      obj_to_room (in, ch->in_room);
                    }
                }
              else
                act ("{y$p{x pаствоpяется.", ch, obj, NULL, TO_ROOM);
              extract_obj (obj);
            }
        }
      else if (obj->wear_loc != WEAR_NONE) /* stop spamming logs with bug in */
        unequip_char (ch, obj);            /* unequip_char messages    (uni) */
    }

 if (ch->in_room) obj_to_room(corpse,ch->in_room);
 else if (ch->was_in_room!=NULL) obj_to_room(corpse,ch->was_in_room);
 else 
 {
   if (IS_SET(ch->act,PLR_ARMY)) obj_to_room( corpse, get_room_index(ROOM_VNUM_ARMY_MORGUE));
   else if (ch->clan==NULL) obj_to_room( corpse,get_room_index(ROOM_VNUM_ALTAR) );
   else obj_to_room(corpse,get_room_index(ch->clan->clandeath) );
 }
 char_from_room(ch);

//  for( charaf = ch->affected; charaf != NULL; charaf = charaf->next )
//   if( is_affected(ch, charaf->type) ) 
 // if( charaf) affect_remove( ch, charaf);
  // ch->affected_by = 0;
  // ch->affected_by = race_table[ch->race].aff; 
     while (ch->affected) affect_remove(ch,ch->affected);  
     ch->affected_by = race_table[ch->race].aff;    
 
//       for(charaf = ch->affected; ch->affected != NULL; ch->affected = charaf->next )
 //   if(ch->affected) affect_remove(ch,ch->affected);
 //   ch->affected_by = race_table[ch->race].aff;

  for (i = 0; i < 4; i++ ) ch->armor[i]= 100;

  ch->position    = POS_RESTING;
  ch->hit         = 1;
  ch->mana        = UMAX(1,ch->mana/4);
  ch->move        = 2;
  if (IS_SET(race_table[ch->race].spec, SPEC_RDEATH))
  {
    ch->hit=ch->max_hit;
    ch->mana=ch->max_mana;
  }

  REM_BIT(ch->act,PLR_SIFILIS);
  ch->pcdata->condition[COND_HUNGER]=20;
  ch->pcdata->condition[COND_THIRST]=20;
  ch->pcdata->condition[COND_DRUNK]=0;
  ch->pcdata->condition[COND_ADRENOLIN]=0;
  
  if (IS_SET(ch->act,PLR_ARMY)) char_to_room( ch, get_room_index(ROOM_VNUM_ARMY_BED));
   else if (ch->clan==NULL) char_to_room( ch,get_room_index(ROOM_VNUM_ALTAR) );
   else char_to_room(ch,get_room_index(ch->clan->clandeath) );  
}

void do_stop_fighting( CHAR_DATA *ch, ROOM_INDEX_DATA *room) 
{ 
  CHAR_DATA *fch;

  ch->fighting = NULL;
  ch->position = IS_NPC(ch) ? ch->default_pos : POS_STANDING;
  update_pos(ch);

  if (ch->in_room==NULL)
  {
    bug("do_stop_fighting: Char is nowhere",0);
    return;
  }

  for ( fch = room->people; fch != NULL; fch=fch->next_in_room) 
  { 
    if (fch->fighting!=ch) continue;
    fch->fighting = NULL; 
    fch->position = IS_NPC(fch) ? fch->default_pos : POS_STANDING; 
    update_pos(fch); 
  } 
} 

void do_shock_hit( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim; 
  char arg[MAX_INPUT_LENGTH]; 
  int chance; 
  AFFECT_DATA af;
 
  one_argument(argument,arg); 
 
  if (!can_attack(ch,1)) return; 
 
  if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_CRUSH)) 
  { 
    cant_mes (ch); 
    return; 
  } 

  if ( !get_skill(ch, gsn_shock_hit))
  {
   stc("Ты не представляешь, как можно оглушить.\n\r", ch);
   return;
  }

  chance = get_skill(ch, gsn_shock_hit);
 
  if (arg[0] == '\0') 
  { 
    victim = ch->fighting; 
    if (victim == NULL) 
    { 
      stc("Но ты ни с кем не сражаешься!\n\r",ch); 
      return; 
    } 
  } 
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Tут таких нет.\n\r",ch); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Ты со всей силы бьешь себе в ухо. Мазохист?!\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Но $C1 твой друг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
    
  if (is_affected(victim,skill_lookup("deaf")))
  {
    do_printf(arg,"Он$r уже оглушен$r!\n\r");
    act(arg,ch,NULL,victim,TO_CHAR);
    return;
  }
  // size  and weight 
  check_criminal(ch,victim,60); 
  chance /= 2;
 
  if (ch->size != victim->size) chance += (ch->size - victim->size) *15;
 
  chance += get_curr_stat(ch,STAT_STR); 
  chance -= get_curr_stat(victim,STAT_DEX); 
 
  if (IS_NPC(ch) && IS_SET(ch->off_flags,OFF_FAST)) chance += 10; 
  if (IS_NPC(ch) && IS_SET(victim->off_flags,OFF_FAST)) chance -= 20; 
 
  if (number_percent() < chance) 
  { 
    act("$c1 сильно бьет тебя по ушам!",ch,NULL,victim,TO_VICT); 
    act("Ты со всей силы бьешь $C4 в ухо!",ch,NULL,victim,TO_CHAR); 
    act("$c1 сильно бьет $C3 по ушам.",ch,NULL,victim,TO_NOTVICT); 
    stc("Ты ничего не слышишь!\n\r",victim);
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("deaf");
    af.level     = ch->level;
    af.duration  = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    WAIT_STATE(ch,2*PULSE_VIOLENCE); 
    damage(ch,victim,ch->level/2+1,gsn_shock_hit, DAM_BASH, TRUE, FALSE, NULL); 
    check_improve(ch,gsn_shock_hit,TRUE,2);
//    if( IS_MIGHT_DEITY(ch) ) change_favour(ch, 3);
  } 
  else 
  { 
    damage(ch,victim,0,gsn_shock_hit,DAM_BASH, TRUE, FALSE, NULL); 
    act("Ты промахиваешься мимо $C4...",ch,NULL,victim,TO_CHAR); 
    act("$c1 промахивается мимо $C4.",ch,NULL,victim,TO_NOTVICT); 
    act("Ты уклоняешься от удара $n.",ch,NULL,victim,TO_VICT); 
    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    check_improve(ch,gsn_shock_hit,FALSE,1);
  } 
} 

void do_vbite( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim; 
  char arg[MAX_INPUT_LENGTH]; 
  AFFECT_DATA af;
 
  one_argument(argument,arg); 
 
  if (!can_attack(ch,1)) return; 
 
  if (!GUILD(ch,VAMPIRE_GUILD)) 
  { 
    stc ("Твои клыки недостаточно сильны, чтобы укусить.\n\r",ch);
    return;
  } 

  if (ch->fighting)
  {
    stc ("Ты сражаешься, некогда тут кусаться.\n\r",ch);
    return;
  }

  if (ch->mana<100)
  {
    stc("Ты слишком устал, чтобы кусаться.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    stc ("Кого укусить?\n\r",ch);
    return;
  }
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Tут таких нет.\n\r",ch); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Укусить себя?\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Но $C1 твой друг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
    
  if (is_affected(victim,skill_lookup("vbite")))
  {
    do_printf(arg,"Ты чувствуешь, что он$r уже укушен$r!\n\r");
    act(arg,ch,NULL,victim,TO_CHAR);
    return;
  }
  check_criminal(ch,victim,60); 
 
  if (number_percent() > ELDER(ch,VAMPIRE_GUILD) ? 90:45)
  { 
    act("$c1 кусает тебя в шею. Ты чувствуешь легкое головокружение!",ch,NULL,victim,TO_VICT); 
    act("Ты кусаешь $C4 в шею!",ch,NULL,victim,TO_CHAR); 
    act("$c1 кусает $C3 в шею, его .",ch,NULL,victim,TO_NOTVICT); 
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("vbite");
    af.level     = ch->level;
    af.duration  = URANGE(10,ch->level,70);
    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 1+ch->level/10;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    WAIT_STATE(ch,2*PULSE_VIOLENCE); 
    DAZE_STATE(victim, 5 * PULSE_VIOLENCE); 
    ch->mana/=3;
    damage(ch,victim,1,0, DAM_PIERCE, TRUE, FALSE, NULL);
//    if( IS_EVIL_DEITY(ch) ) change_favour(ch, 10);
  }
  else
  {
    damage(ch,victim,0,gsn_shock_hit,DAM_BASH, TRUE, FALSE, NULL); 
    act("Ты попытался укусить, но $C4 увернулся...",ch,NULL,victim,TO_CHAR); 
    act("$c1 пытается укусить $C4.",ch,NULL,victim,TO_NOTVICT); 
    act("$n клацает огромными зубами возле твоего лица, ты с трудом уворачиваешься.",ch,NULL,victim,TO_VICT); 
    ch->mana-=ch->mana/10;
    WAIT_STATE(ch, 2*PULSE_VIOLENCE); 
  } 
} 

void do_lash( CHAR_DATA *ch, const char *argument ) 
{ 
  char arg[MAX_INPUT_LENGTH]; 
  OBJ_DATA *whip;
  CHAR_DATA *victim; 
  int chance=50; 
 
  if (!can_attack(ch,1)) return; 
 
  one_argument(argument,arg); 
 
  whip=get_eq_char(ch,WEAR_RHAND);
  if (!whip || whip->item_type!=ITEM_WEAPON
     || whip->value[0]!=WEAPON_WHIP) whip=get_eq_char(ch,WEAR_LHAND);
  if (!whip || whip->item_type!=ITEM_WEAPON || whip->value[0]!=WEAPON_WHIP)
  {
    stc("Подсекать нужно хлыстом.\n\r",ch);
    cant_mes (ch); 
    return; 
  } 
  
  if (arg[0] == '\0') 
  { 
    victim = ch->fighting; 
    if (victim == NULL) 
    { 
      stc("Но ты ни с кем не сражаешься!\n\r",ch); 
      return; 
    }  
  } 
 
  else if ((victim = get_char_room(ch,arg)) == NULL) 
  { 
    stc("Tут таких нет.\n\r",ch); 
    return; 
  } 
 
  if (victim == ch) 
  { 
    stc("Ты пытаешься подсечь сам себя, но у тебя ничего не выходит.\n\r",ch); 
    return; 
  } 
 
  if (is_safe(ch,victim)) return; 

  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Но $C1 твой друг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
    
  chance += (victim->size - ch->size) * 4;
  chance += get_curr_stat(ch,STAT_DEX) * 9;
  chance -= get_curr_stat(victim,STAT_DEX) * 9;
  chance += get_curr_stat(ch,STAT_STR) * 5;
  chance -= get_curr_stat(victim,STAT_STR) * 5;

  if (IS_AFFECTED(ch,AFF_HASTE)) chance += 20;
  if (IS_AFFECTED(victim,AFF_HASTE)) chance -= 20;
  if (IS_AFFECTED(ch,AFF_SLOW)) chance -= 10;
  if (IS_AFFECTED(victim,AFF_SLOW)) chance += 10;
  if (IS_AFFECTED(victim,AFF_FLYING)) chance += 10;

  chance += (ch->level - victim->level) * 2;
  chance += get_skill(ch,gsn_lash)-100;
  check_criminal(ch,victim,60); 
 
  if (number_range(1,99) < chance )
  { 
    act("$c1 подсекает тебя своим хлыстом!!",ch,NULL,victim,TO_VICT); 
    act("Ты подсекаешь $C4 своим хлыстом!",ch,NULL,victim,TO_CHAR); 
    act("$c1 подсекает $C4 своим хлыстом.",ch,NULL,victim,TO_NOTVICT); 
    check_improve(ch,gsn_lash,TRUE,1); 
 
    WAIT_STATE(victim, number_range(0,1) * PULSE_VIOLENCE); 
    WAIT_STATE(ch,PULSE_VIOLENCE); 
    victim->position = POS_RESTING; 
    if( victim->move > 1 ) victim->move /=2;
    damage(ch,victim,number_range(ch->level/2,ch->level),gsn_lash, DAM_BASH, TRUE, FALSE, NULL); 
  } 
  else 
  { 
    damage(ch,victim,2,gsn_lash,DAM_BASH, TRUE, FALSE, NULL); 
    act("Ты лишь оцарапал $C2.",ch,NULL,victim,TO_CHAR); 
    act("$c1 взмахом хлыста поцарапал $C2!",ch,NULL,victim,TO_NOTVICT); 
    act("Ты уклоняешься от хлыста $C2.",ch,NULL,victim,TO_VICT); 
    check_improve(ch,gsn_lash,FALSE,1);
//    if( IS_EVIL_DEITY(ch) ) change_favour(ch, 1);
//    if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 2);
    WAIT_STATE(ch,2*PULSE_VIOLENCE);  
  } 
} 

void do_frame( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim, *opp;
  int offence, defence;
  
  if (! (opp = ch->fighting))
  {
    stc("Но ты ни с кем не сражаешься!\n\r", ch);
    return;
  }

  if (opp->fighting != ch)
  {
    stc("Но твой противник атакует не тебя!\n\r", ch);
    return;
  }

  if (! get_skill(ch,gsn_frame))
  {
    stc("Подставить? Как?\n\r", ch);
    return;
  }

  one_argument(argument,arg); 

  if (! (victim = get_char_room(ch, arg)))
  {
    stc("Здесь таких нет.\n\r", ch);
    return;
  }
  
  if (victim == ch->fighting)
  {
    stc("Но ты и так с ним сражаешься!\n\r", ch);
    return;
  }
  
  if (victim->fighting == ch)
  {
    stc("Но он уже сражается с тобой!\n\r", ch);
    return;
  }
  
  if (victim == ch)
  {
    stc("Подставить себя???\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
  {
    act("Но $C1 твой друг!",ch,NULL,victim,TO_CHAR);
    return;
  }

  if (is_safe(ch,victim)) return;                
  check_criminal(ch,victim,60);
  
  offence = (ch->level*get_skill(ch,gsn_frame)) / 50 +
            get_curr_stat(ch, STAT_DEX) * 2 + 10;
  defence = opp->level + get_curr_stat(opp, STAT_DEX) +
            victim->level + get_curr_stat(victim, STAT_DEX);

//  if (! IS_NPC(opp) && ! IS_NPC(victim) && victim->level + 8 < opp->level)
  
  add_pkiller(victim,ch);

  if ((IS_NPC(opp) || IS_NPC(victim)) && offence + dice(3,40) > defence + dice(3,40))
  {
//    if( IS_STEALTH_DEITY(ch) ) change_favour(ch, 15);
    act("Ты слегка уворачиваешься..... и твой противник атакует $C4!!!", ch, NULL, victim, TO_CHAR);
    act("$c1 слегка уворачивается..... и его противник атакует $C4!!!", ch, NULL, victim, TO_NOTVICT);
    act("Твой противник слегка уворачивается..... и ты атакуешь $C4!!!", opp, NULL, victim, TO_CHAR);
    opp->fighting = victim;
    multi_hit(opp, victim);
    WAIT_STATE(opp, PULSE_VIOLENCE);
    ch->fighting = NULL;
    ch->position = IS_NPC(ch) ? ch->default_pos : POS_STANDING; 
    update_pos(ch);
    check_improve(ch,gsn_frame,TRUE,1); 
  }
  else
  {
    act("Ты неловко поворачиваешься... и зацепляешь $C4!!!", ch, NULL, victim, TO_CHAR);
    act("$c1 неловко поворачивается..... и зацепляет $C4!!!", ch, NULL, victim, TO_ROOM);
    multi_hit(victim, ch);
    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    check_improve(ch,gsn_frame,FALSE,1); 
  }
}


void do_pacify( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *rch;
  
  if ( ( !IS_ORDEN(ch) || BAD_ORDEN(ch) ) && !IS_IMMORTAL (ch) )
  {
   stc ("Боюсь, ты не сможешь этого сделать..", ch);
   return;
  }    
  
  act( "$c1 взмахивает руками, в попытке остановить кровопролитие.", ch, NULL, ch, TO_ROOM );
  stc( "Ты пытаешься умиротворить монстров\n\r",ch);
            
  for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
  {
   if (   IS_NPC(rch) 
       && !IS_AFFECTED(rch,AFF_BERSERK) 
       && (rch->level <= ch->level)
       && (number_percent() >= 50 - (ch->level - rch->level))
      ) 
   {
    if (IS_SET(rch->act,ACT_AGGRESSIVE)) 
    {
     REM_BIT(rch->act,ACT_AGGRESSIVE);
     if (rch->fighting) stop_fighting( rch, TRUE );
     act( "$c1 попускается.", rch, NULL, NULL, TO_ROOM );
    } 
   }
  }
  stc( "Покойся с {Wмиром!{x.\n\r", ch );
  return;
}
                    
void new_one_hit(CHAR_DATA *ch,CHAR_DATA *victim,int dt,OBJ_DATA *obj,bool rgh)
{ 
  OBJ_DATA *wield=NULL;
  int dam,diceroll,skill,dam_type,sn=-1;
  int vict_ac, char_hr;
  bool result; 
 
  if (!ch || !victim || victim==ch || ch->in_room!=victim->in_room) return; 

  if (ch->position < POS_RESTING) return;
  if (victim->position == POS_DEAD) return; 

  if (is_safe(ch,victim)) return;
  if (!victim->fighting) set_fighting( victim, ch );

  if (!IS_NPC(ch) && !IS_NPC(victim)) check_criminal(ch,victim,0); 

  if (rgh) wield = get_eq_char( ch, WEAR_RHAND );
  else wield = get_eq_char( ch, WEAR_LHAND );

  if (wield && wield->item_type!=ITEM_WEAPON) return;
 
  if(IS_AFFECTED(victim, AFF_FIRESHIELD) && number_percent() >= 93)
  {
    act("{y$c1{x обжигается о твой огненный щит, и с визгом отскакивает!",ch,NULL,victim,TO_VICT);
    act("Ты обжигаешься об огненный щит, окружающий {y$C4{x, и отскакиваешь!",ch,NULL,victim,TO_CHAR);
    act("{y$c1{x обжигается об огненный щит, окружающий {y$C4{x, и с визгом отскакивает.",ch,NULL,victim,TO_NOTVICT);
    damage(victim,ch,victim->level/10,37,DAM_FIRE,TRUE, FALSE, NULL);
    return;
  }

  if (!obj)
  {
    if (dt==TYPE_UNDEFINED)
    {
      if (wield) dam_type = (int)wield->value[3];
      else
      {
        if (ch->race==RACE_UNIQUE) dam_type = ch->dam_type;
        else dam_type = attack_lookup(race_table[ch->race].hand_dam);
        if (GUILD(ch,VAMPIRE_GUILD) && (time_info.hour>19 || time_info.hour<6))
        dam_type=attack_lookup("drain");
      }
      dt=attack_table[dam_type].damage;
    }
    else if (dt==gsn_backstab && wield) dam_type = (int)wield->value[3];
    else dam_type=attack_lookup(race_table[ch->race].hand_dam);

    sn = get_weapon_sn(ch,rgh);
  }
  else
  {
    sn = gsn_missile;
    dt = gsn_missile;
    if (obj->item_type==ITEM_WEAPON) dam_type=(int)obj->value[3];
    else dam_type = 1;
  }
  skill = 20 + get_skill(ch,sn);

  if (IS_NPC(ch))
  {
    char_hr=ch->level;
    if (IS_SET(ch->act,ACT_WARRIOR))    char_hr*=2;
    else if (IS_SET(ch->act,ACT_THIEF)) char_hr*=3;
    else if (IS_SET(ch->act,ACT_MAGE))  char_hr/=2;
  }
  else
  { 
    char_hr=ch->level/2;
    if (ch->classwar==1) char_hr*=2;
    else if (ch->classthi==1) char_hr*=3;
    else if (ch->classmag==1) char_hr/=2;
  }

  char_hr+=GET_HITROLL(ch) * skill/100;

  char_hr+=5*(category_bonus(ch,OFFENCE)-category_bonus(victim,PROTECT));

  if (dt==gsn_backstab) char_hr*=get_skill(ch,gsn_backstab)/100;

  switch(dt)
  {
    case(DAM_PIERCE):vict_ac = GET_AC(victim,AC_PIERCE)/10; break;
    case(DAM_BASH):  vict_ac = GET_AC(victim,AC_BASH)/10;   break;
    case(DAM_SLASH): vict_ac = GET_AC(victim,AC_SLASH)/10;  break;
    default:         vict_ac = GET_AC(victim,AC_EXOTIC)/10; break;
  };

  if (!can_see(ch,victim,CHECK_LVL ) ) vict_ac -= victim->level;
  if (victim->position < POS_FIGHTING) vict_ac += victim->level/10;
  if (victim->position < POS_RESTING) vict_ac += victim->level;

  diceroll=number_range(0,19);
  if ( diceroll == 0 || ( diceroll != 19 && diceroll < (char_hr+vict_ac) ))
  {
    damage( ch, victim, 0, dam_type+1000, dt, TRUE, FALSE, (obj)?obj:wield );
/*
    if (IS_CFG(victim,CFG_AUTODAM) && IS_IMMORTAL(victim))
      ptc(victim,"VF:Diceroll %d, vict_ac %d, char_hr %d\n\r",diceroll, vict_ac, char_hr);
    if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch))
      ptc(ch,"CF:Diceroll %d, vict_ac %d, char_hr %d\n\r",diceroll, vict_ac, char_hr);
*/
    return;
  }
/*
    if (IS_CFG(victim,CFG_AUTODAM) && IS_IMMORTAL(victim))
  ptc(victim,"VP:Diceroll %d, vict_ac %d, char_hr %d\n\r",diceroll, vict_ac, char_hr);
    if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch))
  ptc(ch,"CP:Diceroll %d, vict_ac %d, char_hr %d\n\r",diceroll, vict_ac, char_hr);
*/
  if ( diceroll !=19 && dt!=gsn_backstab && dt!=gsn_missile)
  {
    if ( check_shield_block(ch, victim, (obj)?obj:wield)) return;
    if ( check_parry(ch, victim, wield)) return;
    if ( check_dodge(ch, victim, (obj)?obj:wield)) return;
    if ( check_blink(ch, victim, (obj)?obj:wield)) return;
  }

  if (IS_NPC(ch) && !wield) dam = number_range( ch->level / 2, ch->level*3/2);
  else 
  {
    if (sn != -1) check_improve(ch,sn,TRUE,5);
    if (obj && (obj->item_type == ITEM_WEAPON && IS_WEAPON_STAT(obj, WEAPON_MISSILE))) wield=obj;
    if (wield!=NULL)
    {
      OBJ_DATA *shield;

      dam = dice((int)wield->value[1],(int)wield->value[2]) * skill/100;

      shield=get_eq_char(ch,WEAR_RHAND);
      if (!shield || !CAN_WEAR(shield,WEAR_SHIELD)) 
        shield=get_eq_char(ch,WEAR_LHAND);
      if (!shield || !CAN_WEAR(shield,WEAR_SHIELD)) dam = dam * 11/10;

      if (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS))
      {
        dam += dam / 10; 
      }
      if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
      {
        int percent;
        if ((percent = number_percent()) <= (skill / 8))
           dam = 2 * dam + (dam * 2 * percent / 100);
      }
    }
    else dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
  }
  if (obj && (obj->item_type!=ITEM_WEAPON || IS_WEAPON_STAT(obj,WEAPON_MISSILE)))
    dam = number_range( 1, (obj->level/3)*skill/100) + obj->weight;

  if ( get_skill(ch,gsn_enhanced_damage) > 0 ) 
  { 
    diceroll = number_percent(); 
    if (diceroll <= get_skill(ch,gsn_enhanced_damage)) 
    { 
      check_improve(ch,gsn_enhanced_damage,TRUE,6); 
      dam += 2 * ( dam * diceroll/300); 
    } 
  } 
  if (!IS_AWAKE(victim)) dam *= 2;
  else if (victim->position < POS_FIGHTING) dam = dam * 3 / 2;
   
  if (dt==gsn_backstab && wield)
  {
    if ( wield->value[0] != 2 ) dam *= 2 + (ch->level / 10);
    else dam *= 2 + (ch->level / 8);
  
    if (IS_SET(race_table[ch->race].spec,SPEC_BACKSTAB)) dam+=dam/14;
    dam += (dam * (category_bonus(ch, OFFENCE)-category_bonus(victim,PROTECT))/20);
  }
   
  if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch)) {ptc (ch, "new_hit damage1: %d\n\r",dam);}
  if (IS_NPC(ch)) dam += number_range(ch->level/2,ch->level);
  else if (!IS_SET(global_cfg,CFG_NODR)) {dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;}
  
  if IS_SET(ch->act,PLR_ARMY) dam*=2;
  if (IS_CFG(ch,CFG_AUTODAM) && IS_IMMORTAL(ch)) {ptc (ch, "new_hit damage2: %d\n\r",dam);}
  if ( dam <= 0 ) dam = 0;
/*
  if (!IS_NPC(ch) && get_skill(ch,gsn_death_blow) > 1 &&
      ch->level >= 20 && wield && wield->item_type == ITEM_WEAPON )
  {
    if (number_percent() < 0.2 * get_skill(ch,gsn_death_blow))
    {
      stc("{RТы наносишь удар с невероятной силой!{X\n\r",ch);
      ptc(victim,"{W%s {Rнаносит удар с невероятной силой!{X\n\r",ch->short_descr);
      dam =(dam*ch->level)/20;
      check_improve(ch,gsn_death_blow,TRUE,1);
    }
    else check_improve(ch,gsn_death_blow,FALSE,2);
  }
*/


  result=damage( ch, victim, dam, dam_type+1000, dt, TRUE, FALSE, (obj) ? obj : wield );

  if (IS_AFFECTED(victim,AFF_FIRESHIELD) && number_percent()>93)
  {
    act("$c1 обжигается об огненный щит.",ch,NULL,NULL,TO_ROOM);
    act("Ты обжигается об огненный щит, окружающий $C1.",ch,NULL,victim,TO_CHAR);
    damage( victim, ch, dam/10, 37, DAM_FIRE,TRUE, FALSE, NULL);
  }
 
// handling weapon flag effects (poison, etc...)
// Handling weapon flag specifics (used in fight.c/one_hit() )
  
  if (result && wield)
  {  
    int dam; 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON)) 
    { 
      int level; 
      AFFECT_DATA *poison, af; 
  
      if ((poison = affect_find(wield->affected,gsn_poison)) == NULL) level = wield->level; 
      else level = poison->level; 
          
      if (!saves_spell(level / 2,victim,DAM_POISON))  
      { 
        stc("Ты чувствуешь что в твоих венах течет яд.\n\r", victim); 
        act("{y$c1{x отpавлен ядом {w$i2{x.",victim,wield,NULL,TO_ROOM); 
  
        af.where     = TO_AFFECTS; 
        af.type      = gsn_poison; 
        af.level     = level * 3/4; 
        af.duration  = level / 2; 
        af.location  = APPLY_STR; 
        af.modifier  = -1; 
        af.bitvector = AFF_POISON; 
        affect_join( victim, &af ); 
      } 
  
      /* weaken the poison if it's temporary */ 
      if (poison != NULL) 
      { 
        poison->level = UMAX(0,poison->level - 2); 
        poison->duration = UMAX(0,poison->duration - 1); 
        if (poison->level == 0 || poison->duration == 0) 
         act("Отpавление {w$i2{x слабеет.",ch,wield,NULL,TO_CHAR); 
      } 
    } 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC)) 
    { 
      dam = number_range(1, wield->level / 5 + 1); 
      doact("{w$i1{x вытягивает жизнь из {y$c2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF);
      doact("Ты чувствуешь как {w$i1{x вытягивает из тебя жизнь.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF);
      damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE,FALSE, NULL);
      if (!IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE)) ch->alignment = UMAX(-1000,ch->alignment - 1);
      ch->hit += dam/2;
    }

    else if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMP_MANA)) 
    { 
      dam = number_range(1, wield->level / 5 + 1); 
      victim->mana -= dam;
      doact("{w$i1{x вытягивает энергию из {y$c2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF);
      doact("Ты чувствуешь как {w$i1{x вытягивает из тебя энергию.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF);
      if (!IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE)) ch->alignment = UMAX(-1000,ch->alignment - 1);
      ch->mana += dam/2;
    }
    
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
    {
      dam = number_range(1,wield->level / 4 + 1);
      doact("{w$i1{x обжигает {y$c2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF);
      doact("{w$i1{x обжигает твое тело.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF);
      fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
      damage(ch,victim,dam,0,DAM_FIRE,FALSE,FALSE, NULL);
    } 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST)) 
    { 
      dam = number_range(1,wield->level / 6 + 2); 
      doact("{w$i1{x обмоpаживает {y$c4{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF); 
      doact("Холодное пpикосновение {w$i2{x окpужает тебя льдом.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF); 
      cold_effect(victim,wield->level/2,dam,TARGET_CHAR); 
      damage(ch,victim,dam,0,DAM_COLD,FALSE,FALSE, NULL); 
    } 
  
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING)) 
    { 
      dam = number_range(1,wield->level/5 + 2);
      doact("{y$c1{x шокирован световым удаpом из {w$i2{x.",victim,wield,NULL,TO_ROOM, SPAM_WEAPAF); 
      doact("Ты шокирован {w$i5{x.",victim,wield,NULL,TO_CHAR, SPAM_WEAPAF); 
      shock_effect(victim,wield->level/2,dam,TARGET_CHAR); 
      damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE,FALSE, NULL); 
    } 
  } 
  tail_chain( ); 
}

void tatoo_works(CHAR_DATA *ch, CHAR_DATA *victim)
{
//  AFFECT_DATA af;
  char buf[MAX_STRING_LENGTH];
  int dam=0;

  do_printf( buf, "{C%s{x помогает $c1 в бою!", get_rdeity( deity_table[ch->pcdata->dn].russian,'1'));
  act( buf, ch, NULL, NULL, TO_ROOM);
  ptc(ch,"{C%s{x помогает тебе в бою!\r\n", get_rdeity( deity_table[ch->pcdata->dn].russian,'1'));


  switch (ch->pcdata->dn)
  {
    default:
      bug( "tatoo_works: Unknown deity, %d.", ch->pcdata->dn );
      break;

    case 0: //Astellar
      ptc (ch,"Пространство вокруг тебя размывается, воздух начинает дрожать!\r\n");
      act ("Ты чувствуешь поток магической энергии, исходящий от $c1!", ch, NULL, NULL, TO_ROOM);
      ch->mana+=(ch->level+ch->pcdata->favour/50)*3/5;
      if (ch->mana>ch->max_mana) ch->mana=ch->max_mana;
      break;

    case 1: //Tymothy
      ptc (ch,"Тонкий луч лунного света падает на тебя.\r\n");
      act ("$c1 освещен лунным светом.", ch, NULL, NULL, TO_ROOM);
      ch->hit+=ch->level+ch->pcdata->favour/50;
      if (ch->hit>ch->max_hit) ch->hit=ch->max_hit;
      break;

    case 2: //Irenicus
      act ("Черные тучи сгущаются над $c1!", victim, NULL, NULL, TO_ROOM);
      ptc (victim, "Ты чувствуешь раздирающее тебя абсолютное {DЗЛО{x!\r\n");
      dam=(ch->level+ch->pcdata->favour/100)*3;
      damage (ch,victim, dam, 0, DAM_NEGATIVE, TRUE, FALSE, NULL);
      break;

    case 3: //Ketsalkoathl
      act ("$c1 пытается очистить мир от жизни!", ch, NULL, NULL, TO_ROOM);
      ptc (ch, "Странный голос в голове приказывает тебе уничтожить все живое!\r\n");
      spell_plague(gsn_plague, ch->level , ch, (void *) victim,TARGET_CHAR);
      break;

    case 4: //Mishra
      ptc (ch,"Ты чувствуешь странное мерцание вокруг!\r\n");
      act ("Странное свечение окружает $c1!", ch, NULL, NULL, TO_ROOM);
      if (check_dispel(ch->level, ch, gsn_plague))
      {
        stc("Язвы, покрывавшие тебя, исчезли.\n\r",ch);
        act("$c1 выглядит выздоровевшим, и $g язвы исчезли.",ch,NULL,NULL,TO_ROOM);
      }
      if (check_dispel(ch->level, ch, gsn_poison))
      {
        stc("Тепло проходит сквозь тебя, унося боль.\n\r",ch);
        act("$c1 выглядит значительно лучше.",ch,NULL,NULL,TO_ROOM);  
      }
// if (check_dispel(ch->level, ch, gsn_weaken));
      if (number_percent()<50 && check_dispel(ch->level, ch, gsn_curse))
      {
        stc("Ты чувствуешь себя лучше.\n\r",ch);
        act("$c1 чувствует себя более расслабленно.",ch,NULL,NULL,TO_ROOM);
      }
      break;

    case 5: //Thanatos
      act ("Танатос успокаивает $c1.. Это должно быть больно!", victim, NULL, NULL, TO_ROOM);
      ptc (victim, "Некто появляется из ниоткуда.\r\nНекто чем-то успокаивает тебя.\r\nНекто исчезает куда-то.\r\n");
      dam=(ch->level+ch->pcdata->favour/100)*3;
      damage (ch,victim, dam, 0, DAM_NONE, TRUE, FALSE, NULL);
      break;

    case 6: //Ignissa
      act ("Священное пламя вспыхивает вокруг, причиняя невыносимые страдания $c1!", victim, NULL, NULL, TO_ROOM);
      ptc (victim, "Вокруг тебя возникают странные огни, принося невыносимые муки!\r\n");
      dam=(ch->level+ch->pcdata->favour/50)*3;
      damage (ch,victim, dam, 37/*0*/, DAM_FIRE, TRUE, FALSE, NULL);
      break;

    case 7: //Aikanaro
      if (number_percent()>50)
      {
        act("{y$c1{x бросает тебя через бедро!",ch,NULL,victim,TO_VICT); 
        act("Ты бросаешь {y$C4{x через бедро недавно выученным приемом!",NULL,NULL,victim,TO_CHAR); 
        act("{y$c1{x бросает {y$C4{x через бедро, отпpавляя $G на землю.",ch,NULL,victim,TO_NOTVICT); 
 
        WAIT_STATE(victim,PULSE_VIOLENCE);
        damage(ch,victim,number_range(ch->level/10,ch->level/2),gsn_bash,DAM_BASH,TRUE, FALSE, NULL);
        if (check_social(ch,"flex","")) {};
        if (check_social(victim,"confuse","")) {};
      }
      break;

    case 8: //Eteiros
      do_say(ch, "Скоро ты станешь завидовать мертвым!");
      one_hit( ch, victim, TYPE_UNDEFINED, NULL, TRUE ); 
      break;

    case 9: //Saboteur
      if (number_percent()>50)
      {
        ptc (ch,"Ты видишь мир другими глазами.\r\n");
        act ("Глаза $c1 излучают слабый голубоватый свет", ch, NULL, NULL, TO_ROOM);
        if (is_affected(ch, gsn_blindness)) 
        {
          affect_strip(ch, gsn_blindness);
//      if ( skill_table[gsn_blindness].msg_off ) 
//        ptc(ch,"%s\n\r",skill_table[gsn_blindness].msg_off, ch );
        }
        if (is_affected(ch, gsn_dirt)) 
        {
          affect_strip(ch, gsn_dirt);
//      if ( skill_table[gsn_dirt].msg_off ) 
//        ptc(ch,"%s\n\r",skill_table[gsn_dirt].msg_off, ch );
        }
        if (is_affected(ch, skill_lookup("fire breath"))) 
        {
          affect_strip(ch, skill_lookup("fire breath"));
//      if ( skill_table[skill_lookup("fire breath")].msg_off ) 
//        ptc(ch,"%s\n\r",skill_table[skill_lookup("fire breath")].msg_off, ch );
        }
      }
      break;

    case 10: //Aularyas
      ptc (ch,"Ты пьешь воду из волшебного ручейка.\r\n");
      act ("$c1 пьет воду из волшебного ручейка", ch, NULL, NULL, TO_ROOM);
      ch->hit+=ch->level+ch->pcdata->favour/50;
      if (ch->hit>ch->max_hit) ch->hit=ch->max_hit;
      break;

    case 11: //Votan 
      act ("Земля под ногами $c1 трескается, освобождая подземную мощь!", victim, NULL, NULL, TO_ROOM);
      ptc (victim, "Почва уходит у тебя из под ног!\r\n");
      DAZE_STATE(victim,2 * PULSE_VIOLENCE); 
      if (number_percent()<35) 
      {
        WAIT_STATE(victim,PULSE_VIOLENCE);
      }
      break;

    case 12: // Xetharex
      do_say(ch, "Моя месть требует {rКРОВИ{x!");
      one_hit( ch, victim, TYPE_UNDEFINED, NULL, TRUE ); 
      break;

    case 13: //Ligreff
      act("{y$c1{x ловко подставил тебе подножку, отправляя тебя на землю!",ch,NULL,victim,TO_VICT); 
      act("Ты обманным движением посылаешь {y$C4{x на землю!",NULL,NULL,victim,TO_CHAR); 
      act("{y$c1{x подсекает {y$C4{x, отпpавляя $G на землю.",ch,NULL,victim,TO_NOTVICT); 
 
      DAZE_STATE(victim,2 * PULSE_VIOLENCE); 
      if (number_percent()<33) 
      {
        if (check_social(ch,"smirk","")) {};
        WAIT_STATE(victim,PULSE_VIOLENCE/2);
      }
      else  if (check_social(ch,"giggle","")) {};
      if(victim->move>victim->max_move/4) 
      {
        victim->move -= (ch->level+ch->pcdata->favour/50)/2; 
        if(victim->move<victim->max_move/4) victim->move=victim->max_move/4;
      }
      damage(ch,victim,number_range(2 ,victim->level/3 + 4 * victim->size),gsn_trip, 
       DAM_BASH,TRUE,FALSE, NULL); 
      break;

    case 14: //Emortarys
      act ("Силы тьмы приходят на помощь $c1!", ch, NULL, NULL, TO_ROOM);
      ptc (victim, "Ужасная Эмортарис вытягивает силы из тебя!\r\n");
      ptc (ch, "Твои страдания облегчаются!\r\n");
      dam=(ch->level+ch->pcdata->favour/50)*3/2;
      ch->hit+=(ch->level+ch->pcdata->favour/50)/2;
      if (ch->hit>ch->max_hit) ch->hit=ch->max_hit;
      damage (ch,victim, dam, 44/*0*/, DAM_ENERGY, TRUE, FALSE, NULL);
      break;

    case 15: //Urza
      act ("Внезапный порыв ветра налетел на $c1!", victim, NULL, NULL, TO_ROOM);
      ptc (victim, "Мощный торнадо пронессЯ возле тебя!\r\n");
      dam=(ch->level+ch->pcdata->favour/100)*3;
      damage (ch,victim, dam, 0, DAM_NONE, TRUE, FALSE, NULL);
      break;

    case 16: //Thawneous
      act ("Холод пронизывает тело $c1!", victim, NULL, NULL, TO_ROOM);
      ptc (victim, "Холод пронизывает твое тело насквозь!\r\n");
      dam=(ch->level+ch->pcdata->favour/50)*3;
      damage (ch,victim, dam, 96/*0*/, DAM_COLD, TRUE, FALSE, NULL);
      break;

    case 17: //Zagroxer
      ptc (ch,"Мать-Земля придает тебе силы!\r\n");
      act ("Земля придает силы $c1!", ch, NULL, NULL, TO_ROOM);
      ch->hit+=ch->level+ch->pcdata->favour/50;
      if (ch->hit>ch->max_hit) ch->hit=ch->max_hit;
      break;
  }

  return;
}




