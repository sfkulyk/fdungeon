// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

bool    remove_obj      args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void    say_spell       args( ( CHAR_DATA *ch, int sn ) ); 
void add_pkiller(CHAR_DATA *ch, CHAR_DATA *killer); 
void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target);

extern char *target_name;

void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;

  if ( ( victim = get_char_world( ch, target_name ) ) == NULL
  || victim == ch
  || ch->fighting != NULL
  || victim->in_room == NULL
  || !can_see_room(ch,victim->in_room) 
  || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
  || IS_SET(victim->in_room->ra, RAFF_SAFE_PLC)
  || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
  || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
  || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
  || IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
  || IS_SET(ch->act, PLR_ARMY)
  || (!IS_NPC(victim) && (IS_SET(victim->act, PLR_ARMY)))
  || (!IS_NPC(victim) &&  (victim->level>LEVEL_HERO))
  || (IS_NPC(victim) && (IS_SET(victim->imm_flags,IMM_SUMMON)
  || is_gqmob(victim->pIndexData->vnum)))
  || (IS_NPC(victim) && victim->level>=level+3)
  || IS_AFFECTED(ch, AFF_CURSE)
  || IS_SET(ch->in_room->ra, RAFF_EVIL_PR)
  || IS_SET(victim->in_room->ra, RAFF_EVIL_PR)
  || (IS_NPC(victim) && victim->fighting != NULL)
  || ((!IS_NPC(victim) || IS_SET(victim->act, ACT_PET)) && !is_same_clan(ch,victim)))
  {
    stc( "Неудача.\n\r", ch );
    return;
  }   

  if (!IS_NPC(victim) && !IS_MARRY(ch,victim) && 
   (
   victim->fighting != NULL
   || victim->level>=level+3
   || saves_spell(level,victim,DAM_OTHER)
   || (victim->clan && ch->clan && !is_same_clan(ch,victim) && IS_SET(victim->act,PLR_NOSUMMON) && !is_exact_name(victim->clan->name, ch->clan->alli))))
  {
    stc( "Неудача.\n\r", ch );
    return;
  }   

    stone = get_eq_char(ch,WEAR_LHAND);
    if (stone==NULL) stone = get_eq_char(ch, WEAR_RHAND);

    if (!IS_IMMORTAL(ch) 
    &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
    {
        stc("У тебя не хватает важных компонентов.\n\r",ch);
        return;
    }

    if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
    {
        act("Ты берешь энергию $i2, направляя ее в новое русло.",ch,stone,NULL,TO_CHAR);
        act("$i1 ярко загорается и исчезает!",ch,stone,NULL,TO_CHAR);
        extract_obj(stone);
    }

    if((portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0)))
    {
     portal->timer = 2 + level / 25; 
     portal->value[3] = victim->in_room->vnum;

     obj_to_room(portal,ch->in_room);

     act("$i1 вырастает из под земли.",ch,portal,NULL,TO_ROOM);
     act("$i1 вырастает перед тобой.",ch,portal,NULL,TO_CHAR);
    }
   else stc("{RBUG! Unable to create object! Report to Imms NOW!{x\n\r",ch);  
}

void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;
    ROOM_INDEX_DATA *to_room, *from_room;

    from_room = ch->in_room;
 
  if ( ( victim = get_char_world( ch, target_name ) ) == NULL
  || victim == ch
  || ch->fighting != NULL
  || (to_room = victim->in_room) == NULL
  || !can_see_room(ch,victim->in_room) 
  || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
  || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
  || IS_SET(victim->in_room->ra, RAFF_SAFE_PLC)
  || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
  || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
  || IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
  || IS_SET(ch->act, PLR_ARMY)
  || (!IS_NPC(victim) && (IS_SET(victim->act, PLR_ARMY)))
  || (!IS_NPC(victim) &&  (victim->level>LEVEL_HERO))
  || (IS_NPC(victim) && (IS_SET(victim->imm_flags,IMM_SUMMON)
  || is_gqmob(victim->pIndexData->vnum)))
  || (IS_NPC(victim) && victim->level>=level+3)
  || IS_AFFECTED(ch, AFF_CURSE)
  || IS_SET(ch->in_room->ra, RAFF_EVIL_PR)
  || IS_SET(victim->in_room->ra, RAFF_EVIL_PR)
  || (IS_NPC(victim) && victim->fighting != NULL)
  || ((!IS_NPC(victim) || IS_SET(victim->act, ACT_PET)) && !is_same_clan(ch,victim)))
  {
    stc( "Неудача.\n\r", ch );
    return;
  }   

  if (!IS_NPC(victim) && !IS_MARRY(ch,victim) && 
   (
   victim->fighting != NULL
   || victim->level>=level+3
   || saves_spell(level,victim,DAM_OTHER)
   || (victim->clan && ch->clan && !is_same_clan(ch,victim) && IS_SET(victim->act,PLR_NOSUMMON) && !is_exact_name(victim->clan->name, ch->clan->alli))))
  {
    stc( "Неудача.\n\r", ch );
    return;
  }  
  
    stone = get_eq_char(ch,WEAR_LHAND);
    if (!IS_IMMORTAL(ch)
    &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
    {
        stc("У тебя не хватает важных компонентов.\n\r",ch);
        return;
    }
 
    if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
    {
        act("Ты берешь энергию $i2, направляя ее в новое русло.",ch,stone,NULL,TO_CHAR);
        act("$i1 ярко загорается и исчезает!",ch,stone,NULL,TO_CHAR);
        extract_obj(stone);
    }

    /* portal one */ 
    if((portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0)))
    {
     portal->timer = 1 + level / 10;
     portal->value[3] = to_room->vnum;
 
     obj_to_room(portal,from_room);
 
     act("$i1 вырастает из-под земли.",ch,portal,NULL,TO_ROOM);
     act("$i1 вырастает перед тобой.",ch,portal,NULL,TO_CHAR);

     /* no second portal if rooms are the same */
     if (to_room == from_room)
        return;

     /* portal two */
     if((portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0)))
     {
      portal->timer = 1 + level/10;
      portal->value[3] = from_room->vnum;

      obj_to_room(portal,to_room);

      if (to_room->people != NULL)
      {
       act("$i1 вырастает из-под земли.",to_room->people,portal,NULL,TO_ROOM);
       act("$i1 вырастает из-под земли.",to_room->people,portal,NULL,TO_CHAR);
      }
     }
     else stc("{RBUG! Unable to create object! Report to Imms NOW!{x\n\r",ch);  
    }
   else stc("{RBUG! Unable to create object! Report to Imms NOW!{x\n\r",ch);  
}

void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim, *tmp;
  OBJ_DATA *obj;
  bool gate_pet;
  char buf[MAX_INPUT_LENGTH];

  if ( ( victim = get_char_world( ch, target_name ) ) == NULL
  || victim == ch
  || ch->fighting != NULL
  || victim->in_room == NULL
  || !can_see_room(ch,victim->in_room) 
  || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
  || IS_SET(victim->in_room->ra, RAFF_SAFE_PLC)
  || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
  || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
  || (IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
      && !IS_SET(victim->in_room->ra, RAFF_LIFE_STR))
  || (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
      && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR))
  || IS_SET(ch->act, PLR_ARMY)
  || (!IS_NPC(victim) && (IS_SET(victim->act, PLR_ARMY)))
  || (!IS_NPC(victim) &&  (victim->level>LEVEL_HERO))
  || (IS_NPC(victim) && (IS_SET(victim->imm_flags,IMM_SUMMON)
  || is_gqmob(victim->pIndexData->vnum)))
  || (IS_NPC(victim) && victim->level>=level+3)
  || IS_AFFECTED(ch, AFF_CURSE)
  || IS_SET(ch->in_room->ra, RAFF_EVIL_PR)
  || IS_SET(victim->in_room->ra, RAFF_EVIL_PR)
  || (IS_NPC(victim) && victim->fighting != NULL)
  || ((!IS_NPC(victim) || IS_SET(victim->act, ACT_PET)) && !is_same_clan(ch,victim)))
  {
    stc( "Неудача.\n\r", ch );
    return;
  }   

  if (!IS_NPC(victim) && !IS_MARRY(ch,victim) && 
   (
   victim->fighting != NULL
   || victim->level>=level+3
   || saves_spell(level,victim,DAM_OTHER)
   || (victim->clan && ch->clan && !is_same_clan(ch,victim) && IS_SET(victim->act,PLR_NOSUMMON) && !is_exact_name(victim->clan->name, ch->clan->alli))))
  {
    stc( "Неудача.\n\r", ch );
    return;
  }   

  if (!IS_NPC(ch) && ch->pcdata->condition[COND_ADRENOLIN] > 0 && 
      IS_SET(victim->in_room->room_flags,ROOM_SAFE) 
      && IS_SET(victim->in_room->ra,RAFF_SAFE_PLC)
      && get_trust(ch)<102)
  {
   stc( "{rТы слишком взволнован.{x\n\r", ch );
   return;
  }

  if (ch->pet != NULL
  &&  ch->in_room == ch->pet->in_room
  &&  !IS_SET(victim->in_room->room_flags, ROOM_NO_MOB))
   gate_pet = TRUE;
  else
   gate_pet = FALSE;

  act("$c1 ступает в портал и исчезает.",ch,NULL,NULL,TO_ROOM);
  stc("Ты ступаешь в портал и исчезаешь.\n\r",ch);

  for (obj=ch->carrying; obj; obj=obj->next_content)
  {
    if (obj->morph_name)
    {
      ptc(obj->morph_name,"{y%s{x переместился в %s.\n\r",ch->name,ch->in_room->name);
      do_function(obj->morph_name,&do_look,"auto");
    }
  }

  for (tmp=ch->in_room->people;tmp;tmp=tmp->next_in_room)
  {
    if (IS_NPC(tmp) || tmp==ch || !IS_AFFECTED(tmp,AFF_DETECT_MAGIC)) continue;
    if (number_percent()>(50+ch->level-tmp->level) ||
         (GUILD(tmp,ASSASIN_GUILD) && is_offered(ch) ) )
    {
      do_printf(buf,"Ты {CВИДИШЬ{x, как $c1 переместился в %s.",victim->in_room->name);
      act(buf,ch,NULL,tmp,TO_VICT);
    }
  }
  char_from_room(ch);
  char_to_room(ch,victim->in_room);

  act("$c1 появляется из портала.",ch,NULL,NULL,TO_ROOM);
  do_function(ch, &do_look, "auto");

  if (gate_pet)
  {
    act("$c1 ступает в портал и исчезает.",ch->pet,NULL,NULL,TO_ROOM);
    stc("Ты ступаешь в портал и исчезаешь.\n\r",ch->pet);
    char_from_room(ch->pet);
    char_to_room(ch->pet,victim->in_room);
    act("$c1 появляется из портала.",ch->pet,NULL,NULL,TO_ROOM);
    do_function(ch->pet, &do_look, "auto");
  }
}

void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim, *tmp;
  OBJ_DATA *obj;
  char buf[MAX_INPUT_LENGTH];

  victim=get_char_world(ch,target_name);
  if (!victim || victim == ch || victim->in_room == NULL
  ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
  ||   IS_SET(victim->in_room->ra, RAFF_SAFE_PLC)
  ||   IS_SET(ch->in_room->ra, RAFF_SAFE_PLC)
  ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
  ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
  ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
  ||   (IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
        && !IS_SET(victim->in_room->ra, RAFF_LIFE_STR))
  ||   IS_SET(ch->in_room->room_flags, ROOM_NO_MOB)
  ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
  ||   IS_AFFECTED(victim, AFF_CURSE)
  ||   IS_SET(victim->in_room->ra,RAFF_EVIL_PR)
  ||   ((victim->in_room->vnum>=12401) && (victim->in_room->vnum<=12990))
  ||   ((ch->in_room->vnum>=12401) && (ch->in_room->vnum<=12990)))
  {
      stc( "Неудача.\n\r", ch );
      return;
  }

  if (IS_NPC(victim) && (
   (IS_SET(victim->act,ACT_AGGRESSIVE) && IS_SET(ch->in_room->room_flags,ROOM_LAW))
   || IS_SET(victim->imm_flags,IMM_SUMMON) || victim->pIndexData->pShop != NULL
   || is_gqmob(victim->pIndexData->vnum) || saves_spell( level, victim,DAM_OTHER)))
  {
      stc( "Неудача.\n\r", ch );
      return;
  }

  if (victim->level>=level+3 || victim->fighting!=NULL
   || (!IS_NPC(victim) && (victim->level >= LEVEL_IMMORTAL ||
       IS_SET(victim->act,PLR_NOSUMMON) || IS_SET(ch->act,PLR_ARMY))))
  {
      stc( "Неудача.\n\r", ch );
      return;
  }
  if (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE)) ISORDEN(ch);

  act( "$c1 внезапно исчезает.", victim, NULL, NULL, TO_ROOM );

  for (obj=victim->carrying; obj; obj=obj->next_content)
  {
    if (obj->morph_name)
    {
      ptc(obj->morph_name,"{y%s{x переместился в %s.\n\r",victim->name,victim->in_room->name);
      do_function(obj->morph_name,&do_look,"auto");
    }
  }

  for (tmp=victim->in_room->people;tmp;tmp=tmp->next_in_room)
  {
    if (IS_NPC(tmp) || tmp==victim || !IS_AFFECTED(tmp,AFF_DETECT_MAGIC)) continue;
    if (number_percent()>(50+ch->level-tmp->level))
    {
      do_printf(buf,"Ты {CВИДИШЬ{x, как $c1 переместился в %s.",ch->in_room->name);
      act(buf,victim,NULL,tmp,TO_VICT);
    }
  }
  char_from_room( victim );
  char_to_room( victim, ch->in_room );
  act( "$c1 внезапно появляется.", victim, NULL, NULL, TO_ROOM );
  act( "$c1 призывает тебя!", ch, NULL, victim,   TO_VICT );
  do_function(victim, &do_look, "auto" );
}

void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo, *tmp;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    if ( victim->in_room == NULL
    || (IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL))
//        && !IS_SET(victim->in_room->ra, RAFF_LIFE_STR))
    || ( victim != ch && IS_SET(victim->imm_flags,IMM_SUMMON))
    || ( !IS_NPC(ch) && victim->fighting != NULL )
    || IS_AFFECTED(ch, AFF_CURSE)
    || IS_SET(ch->act, PLR_ARMY)
    || IS_SET( ch->in_room->ra,RAFF_EVIL_PR)
    || ( victim != ch && ( saves_spell( level - 5, victim,DAM_OTHER))))
    {
        stc( "Неудача.\n\r", ch );
        return;
    }
    pRoomIndex = get_random_room(victim); /* ARMY by Adron */ /* Battlezone by Imperror */
    if ( (pRoomIndex->vnum>=12401 && pRoomIndex->vnum<=12990) || (pRoomIndex->vnum>=26801 && pRoomIndex->vnum<=26899) )
    {
      stc( "Неудача.\n\r", ch );
      return;
    }
    
    if (victim != ch) stc("Ты телепортирован!\n\r",victim);

    act( "$c1 исчезает!", victim, NULL, NULL, TO_ROOM );

    for (obj=victim->carrying; obj; obj=obj->next_content)
    {
      if (obj->morph_name)
      {
        ptc(obj->morph_name,"{y%s{x переместился в %s.\n\r",victim->name,victim->in_room->name);
        do_function(obj->morph_name,&do_look,"auto");
      }
    }

    for (tmp=victim->in_room->people;tmp;tmp=tmp->next_in_room)
    {
      if (IS_NPC(tmp) || tmp==victim || !IS_AFFECTED(tmp,AFF_DETECT_MAGIC)) continue;
      if ( number_percent()>85 ||
         (GUILD(tmp,ASSASIN_GUILD) && is_offered(ch) ) )
      {
        do_printf(buf,"Ты {CВИДИШЬ{x, как $c1 переместился в %s.",pRoomIndex->name);
        act(buf,victim,NULL,tmp,TO_VICT);
      }
    }
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$c1 медленно появляется в поле зрения.", victim, NULL, NULL, TO_ROOM );
    do_function(victim, &do_look, "auto" );
    return;
}

void do_lore( CHAR_DATA *ch, const char *argument )
{
 char buf[MAX_STRING_LENGTH];
 char arg[MAX_INPUT_LENGTH];
 AFFECT_DATA *paf;
 OBJ_DATA *obj;

 one_argument( argument, arg );

 if ( arg[0] == '\0' )
 {
  stc( "Укажите имя предмета.\n\r", ch );
  return;
 }

 if ( (obj=get_obj_world(ch,argument)) == NULL || get_skill(ch,gsn_lore)+category_bonus(ch,LEARN)*3<number_range(1,99))
 {
  stc( "Ты ничего не можешь припомнить про такой предмет...\n\r", ch );
  check_improve(ch,gsn_lore,FALSE,1);
  return;
 }
 check_improve(ch,gsn_lore,TRUE,3);

 if (obj->level>ch->level && ch->remort<1)
 {
  stc("Вроде бы ты слышал про это, но ничего конкретно припомнить не можешь...\n\r",ch);
  WAIT_STATE( ch, PULSE_VIOLENCE/2);
  return;
 }

 ptc( ch,"{CИмя:{x %s   {CТип: {x%s     {CУровень: {x%d\n\r{CСостояние вещи: %s\n\r",obj->name,
        item_name(obj->item_type),obj->level, get_obj_cond(obj,0) );

 if (obj->owner)
 ptc( ch,"{CСобственность:{x %s\n\r",obj->owner);

 if( IS_ELDER(ch) ) 
 {
  if( !obj ) 
  {
      bug("NULL obj is sent as an argument to 'lore'", 0);
      obj->durability = material_table[material_num(obj->material)].d_dam;
      obj->condition = material_table[material_num(obj->material)].d_dam;
  }
//      ptc(ch,"{RГлючный запас прочности у %s!!!\n\r", get_obj_desc(obj,'2') );
//  else 
       ptc(ch,"{DDurability{x: {y%4d{x \n\r{WCondition{x : {y%4d{x\n\r",
          (obj->durability == -1)?1001:obj->durability, 
          (obj->durability == -1)?1001:obj->condition);
 }

 if (get_skill(ch,gsn_lore)>number_range(5,50))
   ptc(ch, "{CМатериал: [{W%s{C]{x\n\r",material_table[material_num(obj->material)].real_name);

 if (get_skill(ch,gsn_lore)>number_range(5,50))
  ptc(ch, "%s {Cодевается на [{W%s{C]{x\n\r",
        get_obj_desc(obj,'1'),wear_bit_name(obj->wear_flags));

 for ( paf = obj->affected; paf != NULL; paf = paf->next )
 {
   if (paf->location==APPLY_SPELL_AFFECT)
   {
     ptc(ch,"Добавляет Spellaffect [%s]\n\r",affect_bit_name(paf->modifier));
     continue;
   }
  if (paf->bitvector && get_skill(ch,gsn_lore)>number_range(50,99))
  {
     switch(paf->where)
    {
        case TO_AFFECTS:
            do_printf(buf,"Добавляет эффект {C%s{x.\n\r",
                affect_bit_name(paf->bitvector));
            break;
        case TO_WEAPON:
            do_printf(buf,"Оружейные флаги : {C%s{x\n\r",
            weapon_bit_name(paf->bitvector));
            break;
        case TO_OBJECT:
            do_printf(buf,"Флаги : {C%s{x.\n\r",extra_bit_name(paf->bitvector));
            break;
        case TO_IMMUNE:
            do_printf(buf,"Добавляет иммунитет к {C%s{x.\n\r",
                imm_bit_name(paf->bitvector));
            break;
        case TO_RESIST:
            do_printf(buf,"Добавляет сопротивляемость к {C%s{x.\n\r",
                imm_bit_name(paf->bitvector));
            break;
        case TO_VULN:
            do_printf(buf,"Добавляет неуязвимость к {C%s{x.\n\r",
                imm_bit_name(paf->bitvector));
            break;
        default:
            do_printf(buf,"Неизвестный эффект.\n\r");
            break;
    }
    stc(buf,ch);
  }
  }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
        if (paf->bitvector && get_skill(ch,gsn_lore)>number_range(50,99))
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    do_printf(buf,"Добавляет эффект {C%s{x.\n\r",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    do_printf(buf,"Добавляет предметный флаг {C%s{x.\n\r",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    do_printf(buf,"Добавляет иммунитет к {C%s{x.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    do_printf(buf,"Добавляет сопротивляемость к {C%s{x.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    do_printf(buf,"Добавляет уязвимость к {C%s{x.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                default:
                    do_printf(buf,"Неизвестный эффект\n\r");
                    break;
            }
            stc(buf,ch);
        }
    }
    WAIT_STATE( ch, PULSE_VIOLENCE);
    return;
}


/* RT recall spell is back */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *location;
    
    if (IS_NPC(victim)) return;
   
    if ((location = get_room_index( ROOM_VNUM_TEMPLE)) == NULL)
    {
        stc("Ты полностью потерялся.\n\r",victim);
        return;
    } 

    if ((IS_SET(victim->in_room->room_flags,ROOM_NO_RECALL)
         && !IS_SET(victim->in_room->ra, RAFF_LIFE_STR)) 
       || IS_AFFECTED(victim,AFF_CURSE) 
       || IS_SET(ch->in_room->ra,RAFF_EVIL_PR))
    {
        stc("Заклинание не сработало.\n\r",victim);
        return;
    }

    if (victim->fighting != NULL) stop_fighting(victim,TRUE);
    
    ch->move /= 2;
    act("$c1 исчезает.",victim,NULL,NULL,TO_ROOM);
    char_from_room(victim);
    char_to_room(victim,location);
    act("$c1 появляется в комнате.",victim,NULL,NULL,TO_ROOM);
    do_function(victim, &do_look, "auto");
}

/* NPC spells. */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam,hp_dam,dice_dam,hpch;

    act("$c1 выдыхает облако кислоты в $C4.",ch,NULL,victim,TO_NOTVICT);
    act("$c1 выдыхает в тебя облако разъедающей кислоты.",ch,NULL,victim,TO_VICT);
    act("Ты выдыхаешь облако кислоты в $C4.",ch,NULL,victim,TO_CHAR);

    hpch = UMAX(12,ch->hit);
    hp_dam = number_range(hpch/11 + 1, hpch/6);
    dice_dam = dice(level,16);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);
    dam += dam*get_int_modifier(ch,victim,skill_lookup("acid breath"),DAM_ACID)/200;
    if (saves_spell(level,victim,DAM_ACID))
    {
        acid_effect(victim,level/2,dam/4,TARGET_CHAR);
        damage(ch,victim,dam/2,sn,DAM_ACID,TRUE, TRUE, NULL);
    }
    else
    {
        acid_effect(victim,level,dam,TARGET_CHAR);
        damage(ch,victim,dam,sn,DAM_ACID,TRUE, TRUE, NULL);
    }
}

void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam,hp_dam,dice_dam;
    int hpch;

    act("$c1 выдыхает клуб огня.",ch,NULL,victim,TO_NOTVICT);
    act("$c1 выдыхает в тебя клуб огня!",ch,NULL,victim,TO_VICT);
    act("Ты выдыхаешь клуб огня.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX( 10, ch->hit );
    hp_dam  = number_range( hpch/9+1, hpch/5 );
    dice_dam = dice(level,20);

    dam = UMAX(hp_dam + dice_dam /10, dice_dam + hp_dam / 10);
    dam += dam*get_int_modifier(ch,victim,skill_lookup("fire breath"),DAM_FIRE)/200;
    fire_effect(victim->in_room,level,dam/2,TARGET_ROOM);

    // prevent crash with firebreath
    if (!victim || !victim->in_room || !victim->in_room->people) return;

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;

        if (is_safe_spell(ch,vch,TRUE) 
        ||  (IS_NPC(vch) && IS_NPC(ch) 
        &&   (ch->fighting != vch || vch->fighting != ch)))
            continue;

        if (vch == victim) /* full damage */
        {
            if (saves_spell(level,vch,DAM_FIRE))
            {
                fire_effect(vch,level/2,dam/4,TARGET_CHAR);
                damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE, TRUE, NULL);
            }
            else
            {
                fire_effect(vch,level,dam,TARGET_CHAR);
                damage(ch,vch,dam,sn,DAM_FIRE,TRUE, TRUE, NULL);
            }
        }
        else /* partial damage */
        {
            if (saves_spell(level - 2,vch,DAM_FIRE))
            {
                fire_effect(vch,level/4,dam/8,TARGET_CHAR);
                damage(ch,vch,dam/4,sn,DAM_FIRE,TRUE, TRUE, NULL);
            }
            else
            {
                fire_effect(vch,level/2,dam/4,TARGET_CHAR);
                damage(ch,vch,dam/2,sn,DAM_FIRE,TRUE, TRUE, NULL);
            }
        }
    }
}

void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam,hp_dam,dice_dam, hpch;

    act("$c1 поражает противников замораживающим дыханием!",ch,NULL,victim,TO_NOTVICT);
    act("$c1 замораживает тебя своим дыханием!",
        ch,NULL,victim,TO_VICT);
    act("Ты выдыхаешь волну холода.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX(12,ch->hit);
    hp_dam = number_range(hpch/11 + 1, hpch/6);
    dice_dam = dice(level,16);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);
    dam += dam*get_int_modifier(ch,victim,skill_lookup("frost breath"),DAM_COLD)/200;
    cold_effect(victim->in_room,level,dam/2,TARGET_ROOM); 

    // prevent crash with frostbreath
    if (!victim || !victim->in_room || !victim->in_room->people) return;

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;

        if (is_safe_spell(ch,vch,TRUE)
        ||  (IS_NPC(vch) && IS_NPC(ch) 
        &&   (ch->fighting != vch || vch->fighting != ch)))
            continue;

        if (vch == victim) /* full damage */
        {
            if (saves_spell(level,vch,DAM_COLD))
            {
                cold_effect(vch,level/2,dam/4,TARGET_CHAR);
                damage(ch,vch,dam/2,sn,DAM_COLD,TRUE, TRUE, NULL);
            }
            else
            {
                cold_effect(vch,level,dam,TARGET_CHAR);
                damage(ch,vch,dam,sn,DAM_COLD,TRUE, TRUE, NULL);
            }
        }
        else
        {
            if (saves_spell(level - 2,vch,DAM_COLD))
            {
                cold_effect(vch,level/4,dam/8,TARGET_CHAR);
                damage(ch,vch,dam/4,sn,DAM_COLD,TRUE, TRUE, NULL);
            }
            else
            {
                cold_effect(vch,level/2,dam/4,TARGET_CHAR);
                damage(ch,vch,dam/2,sn,DAM_COLD,TRUE, TRUE, NULL);
            }
        }
    }
}

    
void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam,hp_dam,dice_dam,hpch;

    act("$c1 выдыхает облако ядовитого газа.",ch,NULL,victim,TO_NOTVICT);
    act("$c1 выдыхает в тебя облако ядовитого газа!",ch,NULL,victim,TO_VICT);
    act("Ты выдыхаешь облако ядовитого газа.",ch,NULL,victim,TO_CHAR);

    hpch = UMAX(16,ch->hit);
    hp_dam = number_range(hpch/15+1,8);
    dice_dam = dice(level,12);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);
    poison_effect(ch->in_room,level,dam,TARGET_ROOM);

    // prevent crash with gasbreath
//    if (!victim || !victim->in_room || !victim->in_room->people) return;

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;

        if (is_safe_spell(ch,vch,TRUE)
        ||  (IS_NPC(ch) && IS_NPC(vch) 
        &&   (ch->fighting == vch || vch->fighting == ch)))
            continue;

        if (saves_spell(level,vch,DAM_POISON))
        {
            poison_effect(vch,level/2,dam/4,TARGET_CHAR);
            damage(ch,vch,dam/2,sn,DAM_POISON,TRUE, TRUE, NULL);
        }
        else
        {
            poison_effect(vch,level,dam,TARGET_CHAR);
            damage(ch,vch,dam,sn,DAM_POISON,TRUE, TRUE, NULL);
        }
    }
}

void spell_lightning_breath(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam,hp_dam,dice_dam,hpch;

    act("$c1 выдыхает шаровую молнию в $C4.",ch,NULL,victim,TO_NOTVICT);
    act("$c1 выдыхает в тебя шаровую молнию!",ch,NULL,victim,TO_VICT);
    act("Ты выдыхаешь шаровую молнию в $C4.",ch,NULL,victim,TO_CHAR);

    hpch = UMAX(10,ch->hit);
    hp_dam = number_range(hpch/9+1,hpch/5);
    dice_dam = dice(level,20);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);
    dam += dam*get_int_modifier(ch,victim,skill_lookup("lightning breath"),DAM_LIGHTNING)/200;
    if (saves_spell(level,victim,DAM_LIGHTNING))
    {
        shock_effect(victim,level/2,dam/4,TARGET_CHAR);
        damage(ch,victim,dam/2,sn,DAM_LIGHTNING,TRUE, TRUE, NULL);
    }
    else
    {
        shock_effect(victim,level,dam,TARGET_CHAR);
        damage(ch,victim,dam,sn,DAM_LIGHTNING,TRUE, TRUE, NULL); 
    }
}

void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_safe(ch,victim)) return;

  if ( victim == ch )
  {
      stc( "Теперь ты любишь себя еще больше!\n\r", ch );
      return;
  }

  if (IS_NPC(victim) && victim->race==RACE_ZOMBIE && IS_SET(victim->act,ACT_AGGRESSIVE))
  {
    stc( "Неудача!\n\r", ch );
    return;
  }

  if (victim->position==POS_SLEEPING)
  {
      stc( "Сначала разбуди его!\n\r", ch );
      return;
  }

  if (IS_SET(race_table[ch->race].spec, SPEC_PSY)) level+=level/20;

  if ((ch->sex+ victim->sex)==3) {
    if ( level < (victim->level+2) || victim->fighting != NULL
      || IS_SET(victim->imm_flags,IMM_CHARM)|| IS_AFFECTED(ch, AFF_CHARM)
      || IS_AFFECTED(victim, AFF_CHARM)
      || saves_spell( level+1, victim,DAM_CHARM))
    {
        stc( "Неудача!\n\r", ch );
        return;
    }

  }
  else {

    if ( level < (victim->level+2) || victim->fighting != NULL
      || IS_SET(victim->imm_flags,IMM_CHARM)|| IS_AFFECTED(ch, AFF_CHARM)
      || IS_AFFECTED(victim, AFF_CHARM)
      || saves_spell( level, victim,DAM_CHARM))
    {
        stc( "Неудача!\n\r", ch );
        return;
    }

  }

  if (IS_SET(victim->in_room->room_flags,ROOM_LAW))
  {
      stc("Мэр не разрешает очаровывать кого-либо в пределах города.\n\r",ch);
      return;
  }

  if ( victim->master )
  stop_follower( victim );
  add_follower( victim, ch );
  victim->leader = ch;
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = number_fuzzy( level / 4 );
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char( victim, &af );
  act( "$c1 выглядит таким очаровательным!", ch, NULL, victim, TO_VICT );
  if ( ch != victim )
      act("$C1 смотрит на тебя влюбленными глазами.",ch,NULL,victim,TO_CHAR);
  return;
}

void spell_enchant_armor( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA *paf; 
  int result, fail;
  int ac_bonus, added;
  bool ac_found = FALSE;

  if (obj->item_type != ITEM_ARMOR)
  {
    stc("Это не броня.\n\r",ch);
    return;
  }

  if (obj->wear_loc != -1)
  {
    stc("Ты должен нести эту вещь, чтобы ее улучшить.\n\r",ch);
    return;
  }

  if (obj->enchanted && !obj->owner)
  {
    stc("Этот предмет не может быть улучшен.\n\r",ch);
    return;
  }

  if (obj->enchanted && obj->owner && str_cmp(obj->owner,"(public)") && strcmp(obj->owner,ch->name))
  {     
     stc( "Нельзя улучшать чужие вещи.\n\r", ch );
     return;
  }

  /* this means they have no bonus */
  ac_bonus = 0;
  fail = 25;  /* base 25% chance of failure */

  /* find the bonuses */

  if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
      if ( paf->location == APPLY_AC )
      {
        ac_bonus = paf->modifier;
        ac_found = TRUE;
        fail += 5 * (ac_bonus * ac_bonus);
      }
      else fail += 20;
    }

  for ( paf = obj->affected; paf != NULL; paf = paf->next )
  {
    if ( paf->location == APPLY_AC )
    {
      ac_bonus = paf->modifier;
      ac_found = TRUE;
      fail += 5 * (ac_bonus * ac_bonus);
    }
    else fail += 20;
  }
  /* apply other modifiers */
  fail -= level;

  if (IS_OBJ_STAT(obj,ITEM_BLESS)) fail -= 15;
  if (IS_OBJ_STAT(obj,ITEM_GLOW)) fail -= 5;

  fail = URANGE(5,fail,85);

  result = number_percent()+5+(ch->race==RACE_DWARF)*number_range(10,20);
  if (ch->level>108) fail=0;
  /* the moment of truth */
  if (result < (fail / 5))  /* item destroyed */
  {
    act("$i1 ярко вспыхивает...и сгорает!",ch,obj,NULL,TO_CHAR);
    act("$i1 ярко вспыхивает...и сгорает!",ch,obj,NULL,TO_ROOM);
    extract_obj(obj);
    return;
  }

  if (result < (fail / 3)) /* item disenchanted */
  {
    AFFECT_DATA *paf_next;

    act("$i1 ярко загорается, затем затухает...дааа...",ch,obj,NULL,TO_CHAR);
    act("$i1 ярко загорается, затем затухает.",ch,obj,NULL,TO_ROOM);
    obj->enchanted = TRUE;
    obj->owner = str_dup(ch->name);

    /* remove all affects */
    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next; 
      free_affect(paf);
    }
    obj->affected = NULL;

    /* clear all flags */
    obj->extra_flags = 0;
    return;
  }

  if ( result <= fail )  /* failed, no bad result */
  {
    stc("Похоже, ничего не случилось.\n\r",ch);
    return;
  }

  /* okay, move all the old flags into new vectors if we have to */
  if (!obj->enchanted)
  {
    AFFECT_DATA *af_new;
    obj->enchanted = TRUE;

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
  obj->owner = str_dup(ch->name);

  if (result <= (90 - level/5))  /* success! */
  {
    act("$i1 {xпокрывается {yзолотой{x аурой.",ch,obj,NULL,TO_CHAR);
    act("$i1 {xпокрывается {yзолотой{x аурой.",ch,obj,NULL,TO_ROOM);
    SET_BIT(obj->extra_flags, ITEM_MAGIC);
    added = -number_range((obj->level / 30),(obj->level / 10));
  }
  
  else  /* exceptional enchant */
  {
    act("$i1 загорается {Wярким {Yзолотым{x свечением!",ch,obj,NULL,TO_CHAR);
    act("$i1 загорается {Wярким {Yзолотым{x свечением!",ch,obj,NULL,TO_ROOM);
    SET_BIT(obj->extra_flags,ITEM_MAGIC);
    SET_BIT(obj->extra_flags,ITEM_GLOW);
    added = -number_range((obj->level / 30),(obj->level / 10)) - (obj->level / 20);
  }

  if ( category_bonus(ch,MAKE) > 5 ) 
    added-= (category_bonus(ch,MAKE)-4)*obj->level/LEVEL_HERO;
              
  /* now add the enchantments */ 

  //if (number_percent() > (4*category_bonus(ch,MAKE))) 
  obj->level++;

  if (ac_found)
  {
    for ( paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if ( paf->location == APPLY_AC)
      {
        paf->type = sn;
        paf->modifier += added;
        paf->level = UMAX(paf->level,level);
      }
    }
  }
  else /* add a new affect */
  {
    paf = new_affect();

    paf->where      = TO_OBJECT;
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_AC;
    paf->modifier   = added;
    paf->bitvector  = 0;
    paf->next       = obj->affected;
    obj->affected   = paf;
  }
  if (category_bonus(ch,MAKE)>2 && number_percent()>75 && obj->level>21)
  {
    paf = new_affect();
    paf->where      = TO_OBJECT;
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_SAVING_SPELL;
    paf->modifier   = (obj->level < 50) ? -1 : (category_bonus(ch,MAKE)>5 ? -2 : -1);
    paf->bitvector  = 0;
    paf->next       = obj->affected;
    obj->affected   = paf;
  }
}

void spell_enchant_weapon(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA *paf; 
  int result, fail;
  int hit_bonus, dam_bonus, added;
  bool hit_found = FALSE, dam_found = FALSE;

  if (obj->item_type != ITEM_WEAPON)
  {
      stc("Это не оружие.\n\r",ch);
      return;
  }

  if (obj->wear_loc != -1)
  {
      stc("Ты должен нести эту вещь, чтобы улучшить.\n\r",ch);
      return;
  }

  /* this means they have no bonus */
  hit_bonus = 0;
  dam_bonus = 0;
  fail = 25;  /* base 25% chance of failure */

  /* find the bonuses */

  if (!obj->enchanted)
      for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
      {
          if ( paf->location == APPLY_HITROLL )
          {
              hit_bonus = paf->modifier;
              hit_found = TRUE;
              fail += 2 * (hit_bonus * hit_bonus);
          }

          else if (paf->location == APPLY_DAMROLL )
          {
              dam_bonus = paf->modifier;
              dam_found = TRUE;
              fail += 2 * (dam_bonus * dam_bonus);
          }

          else  /* things get a little harder */
              fail += 25;
      }

  for ( paf = obj->affected; paf != NULL; paf = paf->next )
  {
      if ( paf->location == APPLY_HITROLL )
      {
          hit_bonus = paf->modifier;
          hit_found = TRUE;
          fail += 2 * (hit_bonus * hit_bonus);
      }

      else if (paf->location == APPLY_DAMROLL )
      {
          dam_bonus = paf->modifier;
          dam_found = TRUE;
          fail += 2 * (dam_bonus * dam_bonus);
      }

      else /* things get a little harder */
          fail += 25;
  }

  /* apply other modifiers */
  fail -= 3 * level/2;

  if (IS_OBJ_STAT(obj,ITEM_BLESS))
      fail -= 15;
  if (IS_OBJ_STAT(obj,ITEM_GLOW))
      fail -= 5;

  fail = URANGE(5,fail,95);

  result = number_percent()+(ch->race==RACE_DWARF)*number_range(15,25);
  if (ch->level>108) fail=0;
  /* the moment of truth */
  if (result < (fail / 5))  /* item destroyed */
  {
      act("$i1 окутывается дымом и взрывается!",ch,obj,NULL,TO_CHAR);
      act("$i1 окутывается дымом и взрывается!",ch,obj,NULL,TO_ROOM);
      extract_obj(obj);
      return;
  }

  if (result < (fail / 2)) /* item disenchanted */
  {
      AFFECT_DATA *paf_next;

      act("$i1 {Wярко{x загорается, затем {Dзатухает...{xнеувязочка!",ch,obj,NULL,TO_CHAR);
      act("$i1 {Wярко{x загорается, затем {Dзатухает{x.",ch,obj,NULL,TO_ROOM);
      obj->enchanted = TRUE;

      /* remove all affects */
      for (paf = obj->affected; paf != NULL; paf = paf_next)
      {
          paf_next = paf->next; 
          free_affect(paf);
      }
      obj->affected = NULL;

      /* clear all flags */
      obj->extra_flags = 0;
      return;
  }

  if ( result <= fail )  /* failed, no bad result */
  {
      stc("Похоже, ничего не произошло.\n\r",ch);
      return;
  }

  /* okay, move all the old flags into new vectors if we have to */
  if (!obj->enchanted)
  {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;

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

  if (result <= (100 - level/5))  /* success! */
  {
      act("$i1 {xзагорается {bсиним{x светом.",ch,obj,NULL,TO_CHAR);
      act("$i1 {xзагорается {bсиним{x светом.",ch,obj,NULL,TO_ROOM);
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
      added = 1;
  }
  
  else  /* exceptional enchant */
  {
      act("$i1 {xокутывается {Cяркой {Bсиней{x аурой!",ch,obj,NULL,TO_CHAR);
      act("$i1 {xокутывается {Cяркой {Bсиней{x аурой!",ch,obj,NULL,TO_ROOM);
      SET_BIT(obj->extra_flags,ITEM_MAGIC);
      SET_BIT(obj->extra_flags,ITEM_GLOW);
      added = 2;
  }
              
  //if (number_percent() > (4*category_bonus(ch,MAKE))) 
  obj->level++;

  /* now add the enchantments */ 
  if (dam_found)
  {
    for ( paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if ( paf->location == APPLY_DAMROLL)
      {
        paf->type = sn;
        paf->modifier += added;
        paf->level = UMAX(paf->level,level);
        if (paf->modifier > 4) SET_BIT(obj->extra_flags,ITEM_HUM);
      }
    }
  }
  else /* add a new affect */
  {
    paf = new_affect();

    paf->where      = TO_OBJECT;
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_DAMROLL;
    paf->modifier   = added;
    paf->bitvector  = 0;
    paf->next       = obj->affected;
    obj->affected   = paf;
  }

  if (hit_found)
  {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
          if ( paf->location == APPLY_HITROLL)
          {
              paf->type = sn;
              paf->modifier += added;
              paf->level = UMAX(paf->level,level);
              if (paf->modifier > 4)
                  SET_BIT(obj->extra_flags,ITEM_HUM);
          }
      }
  }
  else /* add a new affect */
  {
      paf = new_affect();

      paf->type       = sn;
      paf->level      = level;
      paf->duration   = -1;
      paf->location   = APPLY_HITROLL;
      paf->modifier   = added;
      paf->bitvector  = 0;
      paf->next       = obj->affected;
      obj->affected   = paf;
  }

}

void spell_heat_metal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  OBJ_DATA *obj_lose, *obj_next;
  int dam = 0;
  bool fail = TRUE;

 if (!saves_spell(level + 2,victim,DAM_FIRE) 
 &&  !IS_SET(victim->imm_flags,IMM_FIRE))
 {
      for ( obj_lose = victim->carrying;
            obj_lose != NULL; 
            obj_lose = obj_next)
      {
          obj_next = obj_lose->next_content;
          if ( number_range(1,2 * level) > obj_lose->level 
          &&   !saves_spell(level,victim,DAM_FIRE)
          &&   !IS_OBJ_STAT(obj_lose,ITEM_NONMETAL)
          &&   !IS_OBJ_STAT(obj_lose,ITEM_BURN_PROOF))
          {
              switch ( obj_lose->item_type )
              {
              case ITEM_ARMOR:
              if (obj_lose->wear_loc != -1) /* remove the item */
              {
                  if (can_drop_obj(victim,obj_lose)
                  &&  (obj_lose->weight / 10) < 
                      number_range(1,2 * get_curr_stat(victim,STAT_DEX))
                  &&  remove_obj( victim, obj_lose->wear_loc, TRUE ))
                  {
                      act("$c1 вскрикивает и бросает $i4 на землю!",
                          victim,obj_lose,NULL,TO_ROOM);
                      act("Ты снимаешь и бросаешь $i4, пока жар не спалил твое тело.",
                          victim,obj_lose,NULL,TO_CHAR);
                      dam += (number_range(1,obj_lose->level) / 3);
                      obj_from_char(obj_lose);
                      obj_to_room(obj_lose, victim->in_room);
                      fail = FALSE;
                  }
                  else /* stuck on the body! ouch! */
                  {
                      act("$i1 обжигает твою кожу!",
                          victim,obj_lose,NULL,TO_CHAR);
                      dam += (number_range(1,obj_lose->level));
                      fail = FALSE;
                  }

              }
              else /* drop it if we can */
              {
                  if (can_drop_obj(victim,obj_lose))
                  {
                      act("$c1 вскрикивает и бросает $i4 на землю!",
                          victim,obj_lose,NULL,TO_ROOM);
                      act("Ты бросаешь $i4, пока жар не спалил тебя.",
                          victim,obj_lose,NULL,TO_CHAR);
                      dam += (number_range(1,obj_lose->level) / 6);
                      obj_from_char(obj_lose);
                      obj_to_room(obj_lose, victim->in_room);
                      fail = FALSE;
                  }
                  else /* cannot drop */
                  {
                      act("$i1 обжигает твою кожу!",
                          victim,obj_lose,NULL,TO_CHAR);
                      dam += (number_range(1,obj_lose->level) / 2);
                      fail = FALSE;
                  }
              }
              break;
              case ITEM_WEAPON:
              if (obj_lose->wear_loc != -1) /* try to drop it */
              {
                  if (IS_WEAPON_STAT(obj_lose,WEAPON_FLAMING))
                      continue;

                  if (can_drop_obj(victim,obj_lose) 
                  &&  remove_obj(victim,obj_lose->wear_loc,TRUE))
                  {
                      act("$c1 отдергивает руку от $i2, роняя на землю .",
                          victim,obj_lose,NULL,TO_ROOM);
                      stc(
                          "Ты бросаешь раскалившееся оружие на землю!\n\r",
                          victim);
                      dam += 1;
                      obj_from_char(obj_lose);
                      obj_to_room(obj_lose,victim->in_room);
                      fail = FALSE;
                  }
                  else /* YOWCH! */
                  {
                      stc("Твое оружие обжигает твою кожу!\n\r",
                          victim);
                      dam += number_range(1,obj_lose->level);
                      fail = FALSE;
                  }
              }
              else /* drop it if we can */
              {
                  if (can_drop_obj(victim,obj_lose))
                  {
                      act("$c1 отдергивает руку от $i2, роняя на землю!",
                          victim,obj_lose,NULL,TO_ROOM);
                      act("Ты роняешь $i4, пока жар не спалил твою кожу.",
                          victim,obj_lose,NULL,TO_CHAR);
                      dam += (number_range(1,obj_lose->level) / 6);
                      obj_from_char(obj_lose);
                      obj_to_room(obj_lose, victim->in_room);
                      fail = FALSE;
                  }
                  else /* cannot drop */
                  {
                      act("$i1 обжигает твою кожу!",
                          victim,obj_lose,NULL,TO_CHAR);
                      dam += (number_range(1,obj_lose->level) / 2);
                      fail = FALSE;
                  }
              }
              break;
              }
          }
      }
  } 
  if (fail)
  {
      stc("Твое заклинание не подействовало.\n\r", ch);
      stc("Ты чувствуешь себя теплее.\n\r",victim);
  }
  else /* damage! */
  {
      if (saves_spell(level,victim,DAM_FIRE))
          dam = 2 * dam / 3;
      damage(ch,victim,dam,sn,DAM_FIRE,TRUE, FALSE, NULL);
  }
}

void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af,*aff;
  bool found=FALSE;

 if (is_affected(ch,skill_lookup("farsight")))
 {
  for ( aff=ch->affected; aff!=NULL; aff=aff->next )
  if ( aff->type ==skill_lookup("farsight"))
  {
   affect_strip(ch,skill_lookup("farsight"));
   found=TRUE;
  }
 }

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = level/2;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  if (found)  stc( "Зоркость твоего взгляда снова прояснилась.\n\r",ch);
  else        stc( "Острота твоего взгляда увеличилась.\n\r", ch );
  return;
}
void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;

  if (IS_OBJ_STAT(obj,ITEM_NO_IDENT))
  {
    ptc(ch,"Магия этого артефакта подавляет твою попытку познать суть вещи.\n\r");
    if (!IS_IMMORTAL(ch)) return;
  }

  ptc( ch,"Объект '{y%s{x' тип {c%s{x, дополнительные флаги {g%s{x.\n\rВес {m%d{x, стоимость {Y%u{x, уровень {C%d{x.\n\r",
      obj->name,
      item_name(obj->item_type),
      extra_bit_name( obj->extra_flags ),
      obj->weight / 10,
      obj->cost,
      obj->level);

  switch ( obj->item_type )
  {
  case ITEM_SCROLL: 
  case ITEM_POTION:
  case ITEM_PILL:
      ptc(ch, "Магия {C%u{x-го уровня: ", obj->value[0] );

      if ( obj->value[1] >= 0 && obj->value[1] < max_skill )
          ptc( ch, " '{g%s{x'", skill_table[obj->value[1]].name, ch );

      if ( obj->value[2] >= 0 && obj->value[2] < max_skill )
          ptc(ch, " '{g%s{x'", skill_table[obj->value[2]].name, ch );

      if ( obj->value[3] >= 0 && obj->value[3] < max_skill )
          ptc(ch, " '{g%s{x'",skill_table[obj->value[3]].name, ch );

      if (obj->value[4] >= 0 && obj->value[4] < max_skill)
          ptc(ch," '{g%s{x'",skill_table[obj->value[4]].name,ch);
      stc( ".\n\r", ch );
      break;

  case ITEM_WAND: 
  case ITEM_STAFF: 
      ptc(ch, "Содержит {y%u{x зарядов уровня {c%u{x",
          obj->value[2], obj->value[0] );
    
      if ( obj->value[3] >= 0 && obj->value[3] < max_skill )
          ptc(ch, " '{g%s{x'", skill_table[obj->value[3]].name, ch );
      stc( ".\n\r", ch );
      break;

  case ITEM_DRINK_CON:
      ptc(ch,"Заполнено %s жидкостью. По запаху и вкусу - %s.\n\r",
          liq_table[obj->value[2]].liq_color,
          liq_table[obj->value[2]].liq_showname);
      break;

  case ITEM_CONTAINER:
      ptc(ch,"Вместимость: {y%u#{x  ",obj->value[0]);
      ptc(ch,"Максимальный вес: {c%u#{x  ",obj->value[3]);
      ptc(ch,"флаги: {g%s{x\n\r",cont_bit_name(obj->value[1]));
      if (obj->value[4] != 100)
      {
          ptc(ch,"Изменение веса: {c%u{x%%\n\r",
              obj->value[4]);
      }
      break;
              
  case ITEM_WEAPON:
      stc("Тип оружия: {c",ch);
      switch (obj->value[0])
      {
          case(WEAPON_EXOTIC) : stc("экзотическое{x.\n\r",ch);        break;
          case(WEAPON_SWORD)  : stc("меч{x.\n\r",ch); break;  
          case(WEAPON_DAGGER) : stc("кинжал{x.\n\r",ch);      break;
          case(WEAPON_SPEAR)  : stc("копье{x.\n\r",ch); break;
          case(WEAPON_STAFF)  : stc("посох{x.\n\r",ch); break;
          case(WEAPON_MACE)   : stc("булава/дубина{x.\n\r",ch);       break;
          case(WEAPON_AXE)    : stc("топор{x.\n\r",ch);               break;
          case(WEAPON_FLAIL)  : stc("цепь{x.\n\r",ch);        break;
          case(WEAPON_WHIP)   : stc("плеть{x.\n\r",ch);               break;
          case(WEAPON_POLEARM): stc("алебарда{x.\n\r",ch);    break;
          default             : stc("Адрон его знает{x.\n\r",ch);     break;
      }
      if (obj->pIndexData->new_format)
          do_printf(buf,"Повреждение {c%u{xd{c%u{x (в среднем {c%u{x).\n\r",
              obj->value[1],obj->value[2],
              (1 + obj->value[2]) * obj->value[1] / 2);
      else
          do_printf( buf, "Повреждение от {c%u{x до {c%u{x (в среднем {y%u{x).\n\r",
              obj->value[1], obj->value[2],
              (obj->value[1] + obj->value[2]) / 2);
      stc( buf, ch );
      if (obj->value[4])  /* weapon flags */
      {
        ptc(ch,"Флаги оружия: {g%s{x\n\r",weapon_bit_name(obj->value[4]));
      }
      break;

  case ITEM_ARMOR:
      ptc(ch,"Класс защиты: {c%u{x от укола, {c%u{x от удара, {c%u{x от пореза, и {c%u{x против магических ударов.\n\r", 
          obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
      break;

  case ITEM_ENCHANT:
      ptc(ch,"Позволяет переделать предмет у кузнеца.\n\rЭффект:%s\n\r",mitem[obj->value[0]].desc);
      if (obj->value[1]>0) ptc(ch,"Меняет уровень объекта на %u\n\r",obj->value[1]);
      break;
  case ITEM_BONUS:
      if (obj->value[0]==SCROLL_QUENIA)
      {
        if (number_percent()>50)
             stc ("Ты не можешь разобрать надпись. Похоже тут что-то на {Cquenia{x.\n\r",ch);
        else ptc (ch,"Надпись на {Cquenia {xгласит '%s'.\n\r",quenia_table[obj->value[1]].descr); 
      }
      else if (obj->value[0]==SCROLL_CLANSKILL)
      {
        if (number_percent()>50)
             stc ("Магия этого свитка позволит твоему клану стать {CСИЛЬНЕЕ{x.\n\r",ch);
        else if (number_percent()>50)
             ptc (ch,"Строки на свитке гласят '{C%s{x'.\n\r",skill_table[obj->value[1]].name);
        else ptc (ch,"Строки на свитке гласят '%u часов с {C%s{x'.\n\r",obj->value[2],skill_table[obj->value[1]].name);

      }
      break;
  }

  if (!obj->enchanted)
  for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
  {
      if ( paf->location!=APPLY_NONE && paf->location!=APPLY_SPELL_AFFECT && paf->modifier!= 0 )
      {
          ptc(ch, "Изменяет {c%s{x на {y%d{x.\n\r",
              affect_loc_name( paf->location ), paf->modifier );
          if (paf->bitvector)
          {
              switch(paf->where)
              {
                  case TO_AFFECTS:
                      do_printf(buf,"Добавляет аффект {y%s{x.\n",
                          affect_bit_name(paf->bitvector));
                      break;
                  case TO_OBJECT:
                      do_printf(buf,"Добавляет флаг {y%s{x.\n",
                          extra_bit_name(paf->bitvector));
                      break;
                  case TO_IMMUNE:
                      do_printf(buf,"Добавляет иммунитет к {y%s{x.\n",
                          imm_bit_name(paf->bitvector));
                      break;
                  case TO_RESIST:
                      do_printf(buf,"Добавляет устойчивость к {y%s{x.\n\r",
                          imm_bit_name(paf->bitvector));
                      break;
                  case TO_VULN:
                      do_printf(buf,"Добавляет уязвимость к {y%s{x.\n\r",
                          imm_bit_name(paf->bitvector));
                      break;
                  default:
                      do_printf(buf,"Неизвестный бит {y%d{x: {y%d{x\n\r",
                          paf->where,paf->bitvector);
                      break;
              }
              stc( buf, ch );
          }
      }
  }

  for ( paf = obj->affected; paf != NULL; paf = paf->next )
  {
      if ( paf->location!=APPLY_NONE && paf->location!=APPLY_SPELL_AFFECT && paf->modifier!=0 )
      {
          ptc(ch, "Изменяет {c%s{x на {y%d{x",
              affect_loc_name( paf->location ), paf->modifier );
          if ( paf->duration > -1)
               ptc(ch,", {y%d{x часов.\n\r",paf->duration);
          else ptc(ch,".\n\r");
          if (paf->bitvector)
          {
              switch(paf->where)
              {
                  case TO_AFFECTS:
                      do_printf(buf,"Добавляет аффект {y%s{x.\n",
                          affect_bit_name(paf->bitvector));
                      break;
                  case TO_OBJECT:
                      do_printf(buf,"Добавляет флаг {y%s{x.\n",
                          extra_bit_name(paf->bitvector));
                      break;
                  case TO_WEAPON:
                      do_printf(buf,"Добавляет флаг оружия {y%s{x.\n",
                          weapon_bit_name(paf->bitvector));
                      break;
                  case TO_IMMUNE:
                      do_printf(buf,"Добавляет иммунитет к {y%s{x.\n",
                          imm_bit_name(paf->bitvector));
                      break;
                  case TO_RESIST:
                      do_printf(buf,"Добавляет устойчивось к {y%s{x.\n\r",
                          imm_bit_name(paf->bitvector));
                      break;
                  case TO_VULN:
                      do_printf(buf,"Добавляет уязвимость к {y%s{x.\n\r",
                          imm_bit_name(paf->bitvector));
                      break;
                  default:
                      do_printf(buf,"Неизвестный бит {y%d{x: {y%d{x\n\r",
                          paf->where,paf->bitvector);
                      break;
              }
              stc(buf,ch);
          }
      }
  }
}

void spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( is_affected( victim, sn ) )
  {
      if (victim == ch) stc("Ты уже защищен огненным щитом.\n\r",ch);
      else              act("$C1 уже защищен огненным щитом.",ch,NULL,victim,TO_CHAR);
      return;
  }

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = level/4;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_FIRESHIELD;
  affect_to_char( victim, &af );
  act( "$c1 окружается огненным щитом.", victim, NULL, NULL, TO_ROOM );
  stc( "Вокруг тебя вспыхивает огненный круг.\n\r", victim );
}

void spell_bcloud( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  char buf[MAX_INPUT_LENGTH];
  RAFFECT *af;

  if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
   || IS_SET(ch->in_room->room_flags,ROOM_SAFE)
   || IS_SET(ch->in_room->room_flags,ROOM_ARENA))
  {
    stc("Боги защищают это место.\n\r",ch);
    return;
  }
  if (IS_SET(ch->in_room->ra,RAFF_BLIND))
  {
    stc("Облако черного дыма уже покрывает все вокруг сплошной стеной.\n\r",ch);
    return;
  }
  af=new_raffect();
  af->level=level;
  af->duration=number_range(0,2);
  af->bit=RAFF_BLIND;
  raffect_to_room( af,ch->in_room);
  do_printf(buf,"Вокруг {Y%s{x заклубился {Dчерный туман{x, застлавший все вокруг.",get_char_desc(ch,'2'));
  act(buf,ch,NULL,NULL,TO_ROOM);
  stc("Вокруг тебя заклубился {Dчерный туман{x, застлавший все вокруг.\n\r",ch);
}

void spell_magic_web( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  RAFFECT *af;
  CHAR_DATA *nch;

  if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
   || IS_SET(ch->in_room->room_flags,ROOM_SAFE)
   || IS_SET(ch->in_room->ra, RAFF_SAFE_PLC)
   || IS_SET(ch->in_room->room_flags,ROOM_ARENA))
  {
    stc("Боги защищают это место.\n\r",ch);
    return;
  }
  if (IS_SET(ch->in_room->ra, RAFF_WEB))
  {
    stc("Волшебная паутина уже окружает пространство вокруг.\n\r",ch);
    return;
  }

  if (IS_SET(ch->in_room->ra, RAFF_HIDE))
  {
    stc("Искаженное пространство искажает принцип работы сети. :(\n\r",ch);
    return;
  }

  stc("Пространство замерцало.\n\r",ch);
  for(nch=ch->in_room->people;nch;nch=nch->next)
    if (!IS_NPC(nch) && nch->start_pos==CASTING_WEB) break;

  if (!nch || nch->start_pos!=CASTING_WEB || ch == nch)
  {
    SET_BIT(ch->start_pos,CASTING_WEB);
    ch->mprog_delay=1;
    return;
  }
  af=new_raffect();
  af->level=UMIN((ch->level+nch->level+16)/2,102);
  af->duration=number_range(1,ch->level/50+1);
  af->bit=RAFF_WEB;
  raffect_to_room( af,ch->in_room);
  act("Мохнатые космы волшебной паутины окружили пространство вокруг.\n\r",ch,NULL,NULL,TO_ALL_IN_ROOM);
  nch->start_pos=0;
  nch->mprog_delay=0;
}

void acid_effect(void *vo, int level, int dam, int target)
{
  if (target == TARGET_ROOM)
  {
    ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
    OBJ_DATA *obj, *obj_next;

    for (obj = room->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      acid_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_CHAR)
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj, *obj_next;
      
    for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      acid_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_OBJ)
  {
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    OBJ_DATA *t_obj,*n_obj;
    int chance;
    char *msg;

    if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF) || IS_OBJ_STAT(obj,ITEM_INVENTORY)
        || number_range(0,4) == 0) return;

    chance = level / 4 + dam / 10;
    if (chance > 25) chance = (chance - 25) / 2 + 25;
    if (chance > 50) chance = (chance - 50) / 2 + 50;

    if (IS_OBJ_STAT(obj,ITEM_BLESS)) chance -= 5;
    chance -= obj->level * 2;

    switch (obj->item_type)
    {
      default:
        return;
      case ITEM_CONTAINER:
      case ITEM_CORPSE_PC:
      case ITEM_CORPSE_NPC:
        msg = "$i1 растворяется.";
        break;
      case ITEM_ARMOR:
        msg = "$i1 покрывается ржавчиной.";
        break;
      case ITEM_CLOTHING:
        msg = "$i1 разлагается под действием кислоты.";
        break;
      case ITEM_STAFF:
      case ITEM_WAND:
        chance -= 10;
        msg = "$i1 разъедается кислотой и ломается.";
        break;
      case ITEM_SCROLL:
        chance += 10;
        msg = "$i1 сгорает дотла.";
        break; 
    }

    chance = URANGE(3,chance,97);
    if (number_percent() > chance) return;

    if (obj->carried_by) act(msg,obj->carried_by,obj,NULL,TO_ALL_IN_ROOM);
    else if (obj->in_room && obj->in_room->people)
                         act(msg,obj->in_room->people,obj,NULL,TO_ALL_IN_ROOM);

    if (obj->item_type == ITEM_ARMOR)
    {
      AFFECT_DATA *paf;
      bool af_found = FALSE;
      int i;

      affect_enchant(obj);

      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
        if ( paf->location == APPLY_AC)
        {
          af_found = TRUE;
          paf->type = -1;
          paf->modifier += number_range(5,obj->level/4+5);
          paf->level = UMAX(paf->level,level);
          break;
        }
      }

      if (!af_found)
      {
        paf = new_affect();
        paf->type       = -1;
        paf->level      = level;
        paf->duration   = -1;
        paf->location   = APPLY_AC;
        paf->modifier += number_range(5,obj->level/4+5);
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;
      }

      if (obj->carried_by != NULL && obj->wear_loc != WEAR_NONE)
        for (i = 0; i < 4; i++) obj->carried_by->armor[i] += 1;
      return;
    }

    if (obj->contains)
    {
      for (t_obj = obj->contains; t_obj != NULL; t_obj = n_obj)
      {
        n_obj = t_obj->next_content;
        obj_from_obj(t_obj);
        if (obj->in_room != NULL) obj_to_room(t_obj,obj->in_room);
        else if (obj->carried_by != NULL) obj_to_room(t_obj,obj->carried_by->in_room);
        else
        {
          extract_obj(t_obj);
          continue;
        }
        acid_effect(t_obj,level/2,dam/2,TARGET_OBJ);
      }
    }
    extract_obj(obj);
    return;
  }
}

void cold_effect(void *vo, int level, int dam, int target)
{
  if (target == TARGET_ROOM)
  {
    ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
    OBJ_DATA *obj, *obj_next;
 
    for (obj = room->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      cold_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_CHAR)
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj, *obj_next;
        
    if (!saves_spell(level/4 + dam / 20, victim, DAM_COLD))
    {
      AFFECT_DATA af;

      act("$c1 покрывается коркой льда.",victim,NULL,NULL,TO_ROOM);
      act("Холод пронизывает тебя до костей.",victim,NULL,NULL,TO_CHAR);
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("chill touch");
      af.level     = level;
      af.duration  = 6;
      af.location  = APPLY_STR;
      af.modifier  = -1 - (level>=50) - (level>=100);
      af.bitvector = 0;
      affect_join( victim, &af );
    }

    if (!IS_NPC(victim)) gain_condition(victim,COND_HUNGER,dam/20);

    for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      cold_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_OBJ)
  {
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int chance;
    char *msg;

    if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF) || IS_OBJ_STAT(obj,ITEM_INVENTORY)
     || number_range(0,4) == 0) return;

    chance = level / 4 + dam / 10;
    if (chance > 25) chance = (chance - 25) / 2 + 25;
    if (chance > 50) chance = (chance - 50) / 2 + 50;

    if (IS_OBJ_STAT(obj,ITEM_BLESS)) chance -= 5;

    chance -= obj->level * 2;
    switch(obj->item_type)
    {
      default:
        return;
      case ITEM_POTION:
        msg = "$i1 замерзает и лопается!";
        chance += 25;
        break;
      case ITEM_DRINK_CON:
        msg = "$i1 замерзает и лопается!";
        chance += 5;
        break;
    }

    chance = URANGE(5,chance,95);
    if (number_percent() > chance) return;

    if (obj->carried_by) act(msg,obj->carried_by,obj,NULL,TO_ROOM);
    else if (obj->in_room && obj->in_room->people)
                        act(msg,obj->in_room->people,obj,NULL,TO_ROOM);
    extract_obj(obj);
    return;
  }
}

void fire_effect(void *vo, int level, int dam, int target)
{
  if (target == TARGET_ROOM)
  {
    ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
    OBJ_DATA *obj, *obj_next;

    if (!room)
    {
      log_string("BUG:NULL Room in Fire_effect - magic2.c");
      return;
    }

    for (obj = room->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      fire_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }
 
  if (target == TARGET_CHAR)
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj, *obj_next;

    if (!victim)
    {
      log_string("BUG:NULL Victim in Fire_effect - magic2.c");
      return;
    }

    if (!IS_AFFECTED(victim,AFF_BLIND)
     && !saves_spell(level / 4 + dam / 20, victim,DAM_FIRE))
    {
      AFFECT_DATA af;
      act("$c1 ослеплен дымом!",victim,NULL,NULL,TO_ROOM);
      act("Твои глаза ослеплены дымом...ты не можешь видеть!",victim,NULL,NULL,TO_CHAR);
         
      af.where        = TO_AFFECTS;
      af.type         = skill_lookup("fire breath");
      af.level        = level;
      af.duration     = number_range(0,level/10);
      af.location     = APPLY_HITROLL;
      af.modifier     = -level/4;
      af.bitvector    = AFF_BLIND;
 
      affect_to_char(victim,&af);
    }

    if (!IS_NPC(victim)) gain_condition(victim,COND_THIRST,dam/20);

    for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;

      fire_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_OBJ)
  {
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    OBJ_DATA *t_obj,*n_obj;
    int chance;
    char *msg;

    if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF) || IS_OBJ_STAT(obj,ITEM_INVENTORY)
        || number_range(0,4) == 0) return;
 
    chance = level / 4 + dam / 10;
    if (chance > 25) chance = (chance - 25) / 2 + 25;
    if (chance > 50) chance = (chance - 50) / 2 + 50;
    if (IS_OBJ_STAT(obj,ITEM_BLESS)) chance -= 5;
    chance -= obj->level * 2;

    switch ( obj->item_type )
    {
      default:             
        return;
      case ITEM_CONTAINER:
        msg = "$i1 вспыхивает и сгорает!";
        break;
      case ITEM_POTION:
        chance += 25;
        msg = "$i1 вскипает и испаряется!";
        break;
      case ITEM_SCROLL:
        chance += 50;
        msg = "$i1 сгорает дотла!";
        break;
      case ITEM_STAFF:
        chance += 10;
        msg = "$i1 дымится и сгорает!";
        break;
      case ITEM_WAND:
        msg = "$i1 рассыпается искрами!";
        break;
      case ITEM_FOOD:
        msg = "$i1 чернеет и сгорает!";
        break;
      case ITEM_PILL:
        msg = "$i1 рассыпается от жары!";
        break;
    }
    chance = URANGE(5,chance,95);
    if (number_percent() > chance) return;
 
    if (obj->carried_by) act( msg, obj->carried_by, obj, NULL, TO_ROOM );
    else if (obj->in_room && obj->in_room->people)
                         act(msg,obj->in_room->people,obj,NULL,TO_ROOM);

    if (obj->contains)
    {
      for (t_obj = obj->contains; t_obj != NULL; t_obj = n_obj)
      {
        n_obj = t_obj->next_content;
        obj_from_obj(t_obj);
        if (obj->in_room) obj_to_room(t_obj,obj->in_room);
        else if (obj->carried_by) obj_to_room(t_obj,obj->carried_by->in_room);
        else
        {
          extract_obj(t_obj);
          continue;
        }
        fire_effect(t_obj,level/2,dam/2,TARGET_OBJ);
      }
    }
    extract_obj( obj );
    return;
  }
}

void poison_effect(void *vo,int level, int dam, int target)
{
  if (target == TARGET_ROOM)
  {
    ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
    OBJ_DATA *obj, *obj_next;
 
    for (obj = room->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      poison_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }
 
  if (target == TARGET_CHAR)
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj, *obj_next;

    if (!saves_spell(level / 4 + dam / 20,victim,DAM_POISON))
    {
      AFFECT_DATA af;

      stc("Ты чувствуешь, как яд струится по твоим венам.\n\r",victim);
      act("$c1 выглядит очень больным.",victim,NULL,NULL,TO_ROOM);

      af.where     = TO_AFFECTS;
      af.type      = gsn_poison;
      af.level     = level;
      af.duration  = level / 2;
      af.location  = APPLY_STR;
      af.modifier  = -1;
      af.bitvector = AFF_POISON;
      affect_join( victim, &af );
    }

    for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      poison_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_OBJ)
  {
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int chance;
        
    if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF)
     || IS_OBJ_STAT(obj,ITEM_BLESS)
     || number_range(0,4) == 0) return;

    chance = level / 4 + dam / 10;
    if (chance > 25) chance = (chance - 25) / 2 + 25;
    if (chance > 50) chance = (chance - 50) / 2 + 50;

    chance -= obj->level * 2;

    switch (obj->item_type)
    {
      default:
        return;
      case ITEM_FOOD:
        break;
      case ITEM_DRINK_CON:
        if (obj->value[0] == obj->value[1]) return;
      break;
    }
    chance = URANGE(5,chance,95);
    if (number_percent() > chance) return;
    obj->value[3] = 1;
    return;
  }
}

void shock_effect(void *vo,int level, int dam, int target)
{
  if (target == TARGET_ROOM)
  {
    ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
    OBJ_DATA *obj, *obj_next;

    for (obj = room->contents; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      shock_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_CHAR)
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj, *obj_next;

    if (!saves_spell(level/4 + dam/20,victim,DAM_LIGHTNING))
    {
      stc("Твои мускулы перестают тебе повиноваться.\n\r",victim);
      DAZE_STATE(victim,UMAX(12,level/4 + dam/20));
    }
    for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      shock_effect(obj,level,dam,TARGET_OBJ);
    }
    return;
  }

  if (target == TARGET_OBJ)
  {
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int chance;
    char *msg;

    if (IS_OBJ_STAT(obj,ITEM_BURN_PROOF) || IS_OBJ_STAT(obj,ITEM_INVENTORY)
        || number_range(0,4) == 0) return;

    chance = level / 4 + dam / 10;

    if (chance > 25) chance = (chance - 25) / 2 + 25;
    if (chance > 50) chance = (chance - 50) /2 + 50;
    if (IS_OBJ_STAT(obj,ITEM_BLESS)) chance -= 5;
    chance -= obj->level * 2;

    switch(obj->item_type)
    {
      default:
        return;
      case ITEM_WAND:
      case ITEM_STAFF:
        chance += 10;
        msg = "$i1 взрывается!";
        break;
      case ITEM_JEWELRY:
        chance -= 10;
        msg = "$i1 рассыпается на груду осколков.";
    }
        
    chance = URANGE(5,chance,95);
    if (number_percent() > chance) return;
    if (obj->carried_by) act(msg,obj->carried_by,obj,NULL,TO_ROOM);
    else if (obj->in_room && obj->in_room->people)
                         act(msg,obj->in_room->people,obj,NULL,TO_ROOM);
    extract_obj(obj);
    return;
  }
}

void do_mist(CHAR_DATA *ch, const char *argument)
{
  if (!IS_SET(race_table[ch->race].spec,SPEC_MIST))
  {
    stc("Ты не умеешь превращаться в туман\n\r",ch);
    return;
  }
  if (IS_AFFECTED(ch,AFF_MIST))
  {
    stc("Желтоватый туман, окружающий тебя исчезает.\n\r",ch);
    REM_BIT(ch->affected_by,AFF_MIST);
    return;
  }
  if (number_percent()> GUILD(ch,VAMPIRE_GUILD)?50:80)
  {
    stc("Твои очертания скрываются в желтоватом тумане.\n\r",ch);
    SET_BIT(ch->affected_by,AFF_MIST);
  }
  else stc("Не получилось.\n\r",ch);
  WAIT_STATE(ch, PULSE_VIOLENCE);
}

void do_howl( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *victim; 
  int chance=50;
 
  if (!IS_SET(race_table[ch->race].spec,SPEC_HOWL))
  {
    stc("Твой вой никого не испугает...\n\r",ch);
    return;
  }
  if (!can_attack(ch,1)) return; 
  if (ch->fighting!=NULL)
  { 
    stc("Некогда выть, ты сражаешься!\n\r",ch); 
    return; 
  } 
  if ((victim = get_char_room(ch,argument)) == NULL) 
  { 
    stc("Нет здесь таких.\n\r",ch); 
    return; 
  } 
  if (is_affected(victim,skill_lookup("fear"))) 
  { 
    act("{y$O{x уже дрожит от страха.",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
  if (victim == ch) 
  { 
    stc("Ты жутко взвываешь и твои волосы встают дыбом от страха 8/\n\r",ch); 
    return; 
  } 
  if (is_safe(ch,victim)) return; 
  if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim) 
  { 
    act("Hо {y$C1{x твой хоpоший дpуг!",ch,NULL,victim,TO_CHAR); 
    return; 
  } 
  if (is_affected(victim,skill_lookup("deaf")))
  {
    act("Ты жутко {Gвзвываешь{x!!! Но {y$O{x ничего не слышит...",ch,NULL,victim,TO_CHAR);
    return;
  }
 
  chance += (ch->level - victim->level) * 2; 
  
  switch(ch->in_room->sector_type) 
  { 
    case(SECT_INSIDE):      chance +=15;break; 
    case(SECT_FOREST):      chance += 5;break; 
    case(SECT_MOUNTAIN):    chance +=30;break; 
    case(SECT_WATER_SWIM):  chance -=10;break; 
    case(SECT_WATER_NOSWIM):chance  = 0;break; 
  } 
 
  if (number_percent() < chance) 
  { 
    AFFECT_DATA af; 
    act("От жуткого вопля {Y$c2{x, волосу на голове у {Y$C2{x встают дыбом!",ch,NULL,victim,TO_ROOM);
    act("{Y$c1{x жутко взвывает, его вопль продирает тебя до костей!",ch,NULL,victim,TO_VICT);
    act("От твоего жуткого вопля {Y$c2{x едва непадает в обморок!",ch,NULL,victim,TO_CHAR);
    stc("Ты в ужасе!\n\r",victim);
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("fear");
    af.level     = ch->level;
    af.duration  = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    WAIT_STATE(ch,2*PULSE_VIOLENCE);
    DAZE_STATE(victim,2*PULSE_VIOLENCE);
  } 
  else
  {
    stc("Никто не испугался твоего вопля.. может надо погромче?.\n\r",ch);
    WAIT_STATE(ch,2*PULSE_VIOLENCE);
  }
} 

void spell_distortion( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  char buf[MAX_INPUT_LENGTH];
  RAFFECT *raf;
  AFFECT_DATA af;

  if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
   || IS_SET(ch->in_room->room_flags,ROOM_SAFE)
   || IS_SET(ch->in_room->ra, RAFF_SAFE_PLC)
   || IS_SET(ch->in_room->room_flags,ROOM_ARENA))
  {
    stc("Боги защищают это место.\n\r",ch);
    return;
  }
  if(ch->in_room && IS_CLAN_ROOM(ch->in_room))
  {
   stc("Не в кланах\n\r",ch);
   return;
  }
  if (IS_SET(ch->in_room->ra,RAFF_HIDE))
  {
    stc("Пространство вокруг тебя уже искажено.\n\r",ch);
    return;
  }
  if (is_affected(ch,skill_lookup("distortion")))
  {
   stc("Ты слишком истощен\n\r",ch);
   return;
  }
  
  raf=new_raffect();
  raf->level=level + 20;
  raf->duration=number_range(level/15,level/7);
  raf->bit=RAFF_HIDE;
  raffect_to_room( raf,ch->in_room);

  af.where     = TO_AFFECTS;
  af.type      = skill_lookup("distortion");
  af.level     = 120;
  af.duration  = ch->level/10;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  
  do_printf(buf,"Пространство вокруг {Y%s{x исказилось.",get_char_desc(ch,'2'));
  act(buf,ch,NULL,NULL,TO_ROOM);
  stc("Пространство вокруг тебя исказилось.\n\r",ch);
}

void spell_oasis( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  char buf[MAX_INPUT_LENGTH];
  bool new=FALSE;
  RAFFECT *raf;

  if (raffect_level(ch->in_room,RAFF_OASIS)>level)
  {
    stc("Ты пыжишься, но твои попытки ни к чему не приводят.\n\r",ch);
    act("{Y$c1{x пыжится, но его попытка ни к чему не приводит.",ch,NULL,NULL,TO_ROOM);
    return;
  }
  
  raf=get_raffect(ch->in_room,RAFF_OASIS);
  if (!raf)
  {
    new=TRUE;
    raf=new_raffect();
  }
  raf->level=level+number_range(1,10);
  raf->duration=number_range(level/10,level/5);
  raf->bit=RAFF_OASIS;

  if (new)
  {
    raffect_to_room( raf,ch->in_room);
    do_printf(buf,"Яркий свет быстро заполнил все вокруг {Y%s{x",get_char_desc(ch,'2'));
    act(buf,ch,NULL,NULL,TO_ROOM);
    stc("Вокруг тебя разлился яркий приятный свет.\n\r",ch);
  }
  else
  {
    act("{Y$c1{x ослепительно вспыхнул, обновляя живительную силу пространства.",ch,NULL,NULL,TO_ROOM);
    stc("Ты обновил живительную силу пространства вокруг.\n\r",ch);
  }
}

void spell_mind_ch( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  char buf[MAX_INPUT_LENGTH];
  bool new=FALSE;
  RAFFECT *raf;

  if (raffect_level(ch->in_room,RAFF_MIND_CH)>=level)
  {
    stc("Ты пыжишься, но твои попытки ни к чему не приводят.\n\r",ch);
    act("{Y$c1{x пыжится, но его попытка ни к чему не приводит.",ch,NULL,NULL,TO_ROOM);
    return;
  }
  
  raf=get_raffect(ch->in_room,RAFF_MIND_CH);
  if (!raf)
  {
    new=TRUE;
    raf=new_raffect();
  }
  raf->level=level+number_range(1,10);
  raf->duration=number_range(level/10,level/5);
  raf->bit=RAFF_MIND_CH;

  if (new)
  {
    raffect_to_room( raf,ch->in_room);
    do_printf(buf,"Яркий луч света вырвался из рук {Y%s{x и устремился в небо,{/ освещая все вокруг {cмягким голубым {xсветом",get_char_desc(ch,'2'));
    act(buf,ch,NULL,NULL,TO_ROOM);
    stc("Ты создал энергетический канал.\n\r",ch);
  }
  else
  {
    act("{Y$c1{x коснулся рукой столба света, заставляя его ярко мерцать.",ch,NULL,NULL,TO_ROOM);
    stc("Ты обновил энергетический канал.\n\r",ch);
  }
}

void spell_evil_presence( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{ 
//  bool first=FALSE;
  RAFFECT *roomaf;
  AFFECT_DATA charaf;

  if (is_affected(ch,skill_lookup("evil presence")))
  {
     stc( "Силы {RАДА{x - не игры, смертный...\n\r", ch );
     return;
  }

  if (IS_SET(ch->in_room->room_flags, ROOM_LAW)
   || IS_SET(ch->in_room->sector_type, SECT_CITY)
   || IS_SET(ch->in_room->room_flags, ROOM_SAFE)
   || IS_SET(ch->in_room->ra, RAFF_SAFE_PLC)
   || IS_SET(ch->in_room->ra, RAFF_LIFE_STR))
  {
    stc("Воля Богов защищает это место.\n\r",ch);
    return;
  }
  else if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
  {
    stc("Воздух тут и так пропитан злом.\n\r",ch);
    return;
  }

  if (raffect_level(ch->in_room,RAFF_EVIL_PR) > ch->level)
  {
    stc("Ты лишь чувствуешь, как это место втягивает в себя твою магию.\n\r",ch);
    act("{Y$c1{x поднимает руки но тёмная аура этого места втягивает в себя $g магию.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch, skill_lookup("evil presence"), FALSE, 1);
    return;
  }

  act( "Из поднятых рук $c2 вырывается поток холодного чёрного мрака.", ch, NULL, NULL, TO_ROOM );
  stc( "Мрак, исходящий от твоих ладоней, заполняет комнату.\n\r", ch );

  roomaf=get_raffect(ch->in_room, RAFF_EVIL_PR);

  if (!roomaf)
  {
   roomaf=new_raffect();
   roomaf->level=level;
   roomaf->duration=number_range(1,level/20);
   roomaf->bit=RAFF_EVIL_PR;
   raffect_to_room( roomaf,ch->in_room);

   if( !IS_ELDER(ch) ) {
    charaf.where     = TO_AFFECTS;
    charaf.type      = skill_lookup("evil presence");
    charaf.level     = ch->level;
    charaf.duration  = number_range( ch->level/50 + 1, ch->level/20);
    charaf.location  = APPLY_NONE;
    charaf.modifier  = 0;
    charaf.bitvector = 0;
    affect_to_char( ch, &charaf );
   }
  }

//  if (first) 
//  {
    act("Аура тёмной Силы окутывает окрестности.",ch,NULL,NULL,TO_ROOM);
    stc("От выпущенных тобой Сил свет вокруг мернет.\n\r",ch);
    check_improve(ch, skill_lookup("evil presence"), TRUE, 1);
/*  }
  else 
  {
   roomaf->level+=level/7+2;
   roomaf->duration+=number_range(1,2);
   act("{Y$c1{x обновил мрачную ауру комнаты свежей силой.",ch,NULL,NULL,TO_ROOM);
   stc("Ты придал мраку комнаты новых сил.\n\r",ch);
  }  
*/
} 

void spell_life_stream( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{ 
  RAFFECT *roomaf;
  AFFECT_DATA charaf;

  if (is_affected(ch,skill_lookup("life stream")))
  {
     stc( "Не злоупотребляй вниманием {WБогов{x, смертный...\n\r", ch );
     return;
  }
 
  if(ch->in_room && IS_CLAN_ROOM(ch->in_room))
  {
   stc("Не в кланах\n\r",ch);
   return;
  }

  if( IS_SET( ch->in_room->ra, RAFF_EVIL_PR))
  {
   roomaf=get_raffect(ch->in_room, RAFF_EVIL_PR);

   if( ((ch->level + category_bonus(ch, LIGHT)/3) > roomaf->level) || IS_ELDER(ch) )
   {
     raffect_from_room(roomaf);
     stc( "Ты чувствуешь, как силы Зла покидают эту комнату.\n\r", ch );
     act( "Мрак комнаты медленно рассеивается.",ch,NULL,NULL,TO_ROOM);
     return;
   }
   stc( "Силы Зла оказались сильнее тебя...\n\r", ch );
   check_improve(ch, skill_lookup("life stream"), FALSE, 1);
   return;

  }
  if( IS_SET( ch->in_room->ra, RAFF_WEB))
  {
   roomaf=get_raffect(ch->in_room, RAFF_WEB);

   if( (ch->level + category_bonus(ch, LIGHT)/3) > roomaf->level)
   {
     raffect_from_room(roomaf);
     act( "Косматая паутина расплетается.",ch,NULL,NULL,TO_ROOM);
     check_improve(ch, skill_lookup("life stream"), TRUE, 1);
     return;
   }
   stc( "Тебе не под силу разрушить Сеть...\n\r", ch );
   check_improve(ch, skill_lookup("life stream"), FALSE, 1);
   return;
  }
  else if( IS_SET(ch->in_room->ra, RAFF_LIFE_STR))
  {
   stc( "Комната и так озарена присутствием Светлых Сил..\n\r", ch );
   check_improve(ch, skill_lookup("life stream"), FALSE, 1);
   return;
  }

   roomaf=new_raffect();
   roomaf->level=level;
   roomaf->duration=0;
   roomaf->bit=RAFF_LIFE_STR;
   raffect_to_room( roomaf,ch->in_room);

  if( !IS_ELDER(ch) ) {
    charaf.where     = TO_AFFECTS;
    charaf.type      = skill_lookup("life stream");
    charaf.level     = ch->level;
    charaf.duration  = number_range( ch->level/50 + 1, ch->level/20);
    charaf.location  = APPLY_NONE;
    charaf.modifier  = 0;
    charaf.bitvector = 0;
    affect_to_char( ch, &charaf );
   }

   check_improve(ch, skill_lookup("life stream"), TRUE, 1);

   stc( "Ты взываешь к небесам и мягкий свет озаряет комнату.\n\r", ch);
   act( "Мягкий свет, призванный {Y$c5{x, заполняет комнату.",ch,NULL,NULL,TO_ROOM);
} 

void spell_gaseous_form( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  AFFECT_DATA af;

  if ( !IS_IMMORTAL(ch) && get_curr_stat(ch,STAT_INT) < 27 )
  {
    stc("Твоих духовных сил недостаточно для единения с эмпиреем.\n\r",ch);
    return;
  }
  if (is_affected(ch, sn))
  {
    stc("Твоё тело уже принадлежит эмпирею.\n\r",ch);
    return;
  }
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_ADRENOLIN] > 0)
  {
    stc("Ты должен успокоиться, помедитировать...\n\r",ch);
    return;
  }
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = level/100 + category_bonus(ch,MIND)*2/5;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_GASEOUS_FORM;
  affect_to_char( ch, &af );
  act( "Очертания $c2 тают в легкой дымке.", ch, NULL, NULL, TO_ROOM );
  stc( "Ты чувствуешь как воздух начинает проходить сквозь твое тело.\n\r",ch);

  check_improve(ch, gsn_gaseous_form, TRUE, 1);
}

void spell_safty_place( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  RAFFECT *raf;
  AFFECT_DATA af;
  CHAR_DATA *vch;
  
  if (IS_AFFECTED(ch,AFF_SAFE_PLACE))
  {
    stc( "У тебя не хватает сил.\n\r",ch);
    return;
  }

  if (IS_SET(ch->in_room->ra, RAFF_SAFE_PLC)
    || IS_SET(ch->in_room->room_flags,ROOM_SAFE))
  {
    stc( "Здесь уже и так безопасно.\n\r", ch );
    return;
  }

  if (IS_SET(ch->in_room->ra,RAFF_VIOLENCE))
  {
    raf=get_raffect(ch->in_room,RAFF_VIOLENCE);
    raffect_from_room(raf);
 
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SAFE_PLACE;
    affect_to_char( ch, &af );
    return;
  }

  raf=new_raffect();
  raf->level=level;
  raf->duration=level/10;
  raf->bit=RAFF_SAFE_PLC;
  raffect_to_room( raf,ch->in_room);

  for (vch = ch->in_room->people;vch != NULL;vch = vch->next_in_room)
  {
    stop_fighting(vch,FALSE);
    gain_condition( vch, COND_ADRENOLIN, -50);
  }

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 24;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_SAFE_PLACE;
  affect_to_char( ch, &af );
  stc("Теперь здесь безопасно.\n\r", ch);
  act("{W$c2{x сделал тут безопасное место.\n\r",ch,NULL,NULL,TO_ROOM);
}

void spell_iron_skin( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 20;
   af.location  = APPLY_AC;
   af.modifier  = -10*level;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   stc( "Твоя кожа становится очень твердой.\n\r", ch);
   act( "Кожа {W$c2's{x становится очень твердой.\n\r",ch,NULL,NULL,TO_ROOM);
}

void spell_inspire( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 20;
   af.location  = APPLY_SAVING_SPELL;
   af.modifier  = -3*level/4;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   stc( "Ты чувствуешь вдохновение {WЖизненной Силы{X.\n\r", ch);
   act( "{W$c2{x вдохновлен {WЖизненной Силой{X.\n\r",ch,NULL,NULL,TO_ROOM);
}

void spell_rejuvinate( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;

  if (IS_AFFECTED(ch,AFF_REJUVINATE) )
   {
     stc("У тебя не хватает сил.\n\r",ch);
     return;
   }

   victim->hit = victim->max_hit;
   victim->move = victim->max_move;
   update_pos ( victim );

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = ch->level;
   af.duration  = 10;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_REJUVINATE;
   affect_to_char( ch, &af );
 
   stc("Ты восстановил силы.\n\r",ch);
   act("{W$c2{x восстановил твои силы.\n\r",ch,NULL,NULL,TO_ROOM);

}

void spell_peace( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  CHAR_DATA *vch;

  if ( IS_AFFECTED(ch,AFF_PEACE) )
  {
    stc("У тебя не хватает сил.\n\r",ch);
    return;
  }

  if (IS_SET(ch->in_room->ra,RAFF_VIOLENCE) )
  {
    stc("Дух насилия здесь слишком силен.\n\r",ch);
    return;
  }
  do_function(ch, &do_say, "Мир вам, Братья и Сестры!");

  for (vch = ch->in_room->people;vch != NULL;vch = vch->next_in_room)
  {
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.modifier  = 0;
    af.bitvector = AFF_PEACE;
    af.location  = APPLY_NONE;
    if (vch==ch) af.duration  = 10;
    else if (!IS_IMMORTAL(vch))
    {
      af.duration  = UMAX(1,(ch->level-vch->level+10)/10);
      stc("Ты чувсвуешь себя очень мирным.\n\r",vch);
    }
    affect_to_char( vch, &af );
    gain_condition( vch, COND_ADRENOLIN, -50);
    stop_fighting(vch,FALSE);
  }      
}

void spell_violence( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  RAFFECT *raf;
  AFFECT_DATA af;
  
  if ( IS_AFFECTED(ch,AFF_VIOLENCE) )
  {
    stc( "У тебя не хватает сил.\n\r", ch );
    return;
  }

  if ( IS_AFFECTED(ch,AFF_PEACE) )
  {
    stc("У тебя слишком мирное настроение для этого.\n\r",ch);
    return;
  }   

  if( IS_SET(ch->in_room->ra, RAFF_VIOLENCE))
  {
    stc( "Насилие и так уже царит здесь.\n\r", ch );
    return;
  }

  if ( IS_SET(ch->in_room->ra, RAFF_SAFE_PLC))
  {
    raf=get_raffect(ch->in_room,RAFF_SAFE_PLC);
    raffect_from_room(raf);
  
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_VIOLENCE;
    affect_to_char( ch, &af );
    return;
  }
   
  raf=new_raffect();
  raf->level=level;
  raf->duration=level/10;
  raf->bit=RAFF_VIOLENCE;
  raffect_to_room( raf,ch->in_room);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 24;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_VIOLENCE;
  affect_to_char( ch, &af );

  stc( "Теперь насилие царит здесь!\n\r", ch);
  act( "{W$c2{x выпустил дух насилия здесь.\n\r",ch,NULL,NULL,TO_ROOM);
}

void spell_spook( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  CHAR_DATA *vch;

  if ( IS_AFFECTED(ch,AFF_CANT_FEAR) )
  {
    stc( "У тебя не хватает сил чтоб напугать других.\n\r", ch );
    return;
  }

  stc("Ты до смерти пугаешь окружающих.\n\r",ch);
  for (vch = ch->in_room->people;vch != NULL;vch = vch->next_in_room)
  {
    if ( vch == ch )
    {
      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = ch->level;
      af.duration  = 2;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_CANT_FEAR;
      affect_to_char( vch, &af );
    } 
    if ( vch != ch && number_percent() < 50 && !IS_IMMORTAL(vch) 
      && PK_RANGE(ch,vch) && PK_RANGE(vch,ch) 
      && vch->fighting != NULL && vch->fighting == ch 
      && ch->pcdata->pkillers!=NULL 
      && is_exact_name(vch->name,ch->pcdata->pkillers) )
    {
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("fear");
      af.level     = ch->level;
      af.duration  = 0;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char( vch, &af );
      do_function(vch,&do_flee,"");
    }
  }
}    

void spell_madness( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  
  if (IS_AFFECTED(victim,AFF_MADNESS))
  {
    stc( "Ты не можешь дважды навесить заклинание безумия.\n\r", ch );
    return;
  }

  if (IS_AFFECTED(victim,AFF_PEACE))
  {
    stc("У тебя слишком мирное настроение для этого.\n\r",ch);
    return;
  }

  if ((victim!=ch && !IS_IMMORTAL(victim))
   || (!PK_RANGE(ch,victim) && !PK_RANGE(victim,ch)
   && (!ch->pcdata->pkillers || !is_exact_name(victim->name,ch->pcdata->pkillers))))
  {
    stc("Выбери кого-то себе по силам.\n\r",ch);
    return;
  }
  
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = level/10;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_MADNESS;

  if (victim==ch)
  {
    affect_to_char( ch, &af );
        
    af.location  = APPLY_HITROLL;
    af.modifier  = 3*ch->level/4;
    affect_to_char( ch, &af );

    af.location  = APPLY_DAMROLL;
    af.modifier  = 3*ch->level/4;
    affect_to_char( ch, &af );
   
    stc("Безумие затмевает твой разум!\n\r",ch);
    act("Безумие затмевает разум {W$c1{x!\n\r",ch,NULL,NULL,TO_ROOM);
  }    
  if ( victim != ch && !saves_spell(level,victim,DAM_OTHER ) )
  {
    affect_to_char( victim, &af );
   
    af.location  = APPLY_HITROLL;
    af.modifier  = ch->level/5;
    affect_to_char( victim, &af );

    af.location  = APPLY_DAMROLL;
    af.modifier  = ch->level/5;
    affect_to_char( victim, &af );
    stc("Безумие затмевает твой разум!\n\r",victim);
    act("Безумие затмевает разум {W$c1{x!\n\r",victim,NULL,NULL,TO_ROOM);
  }
}
   
void spell_breath_death( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice ( level, 15 );
  if ( saves_spell(level, victim, DAM_NEGATIVE ) ) dam /=2;
  damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE, FALSE,NULL);

  (*skill_table[gsn_plague].spell_fun)
    (gsn_plague,level,ch,vo, TAR_CHAR_OFFENSIVE);

  (*skill_table[skill_lookup("slow")].spell_fun) (skill_lookup("slow"),level,ch,vo, TAR_CHAR_OFFENSIVE);

  (*skill_table[skill_lookup("chill touch")].spell_fun) (skill_lookup("chill touch"),level,ch,vo, TAR_CHAR_OFFENSIVE);
}

void spell_eyes_death( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *ch_room; 
    
  if (is_affected(ch,skill_lookup("death eyes")))
  {
    stc("У тебя не хватает сил.\n\r",ch);
    return;
  }    

  victim = get_char_world(ch,target_name);

  if (!victim || !can_see(ch,victim,CHECK_LVL))
  {
    stc("Ты не смог найти это сознание в ментальном поле.\n\r",ch);
    return;
  }
  
  if (ch==victim)
  {
    stc("Ты и так себя неплохо видишь.\n\r",ch);
    return;
  }

  if (!IS_NPC(victim))
  {
    if (IS_IMMORTAL(victim) || !PK_RANGE(victim,ch))
    {
      stc("Боги защищают это сознание в ментальном поле.\n\r",ch);
      return;
    }
    if (!PK_RANGE(ch,victim) && (!ch->pcdata->pkillers
     || !is_exact_name(victim->name,ch->pcdata->pkillers)))
    {
      stc("Боги защищают это сознание в ментальном поле.\n\r",ch);
      return;
    }
  }

  if ( number_percent() < 70 )
  {
    act("На мгновенье ты увидел мир глазами $C2",ch,NULL,victim,TO_CHAR);
    ch_room = ch->in_room;
    char_from_room(ch);
    char_to_room(ch, victim->in_room);
    do_look(ch,"auto"); 
    char_from_room(ch);
    char_to_room(ch, ch_room);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = ch->level;
    af.duration  = 1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
  }
  else stc("Не вышло.\n\r",ch);
}

void spell_pain( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
    
  if (victim==ch)
  {
    stc("Не будь мазохистом...\n\r",ch);
    return;
  }

  if (IS_IMMORTAL(victim) && get_trust(ch) < get_trust(victim))
  {
    stc("Даже и не пытайся...\n\r",ch);
    return;
  }

  if (!PK_RANGE(ch,victim) || !PK_RANGE(victim,ch) || !ch->pcdata->pkillers
       || !is_exact_name(victim->name,ch->pcdata->pkillers))
  {
    stc("Выбери кого-то себе по силам.\n\r",ch);
    return;
  }

  if (!can_see(ch, victim, CHECK_LVL) )
  {
    stc("Таких тут нет.\n\r",ch);
    return;
  }

  if (!saves_spell(level,victim,DAM_OTHER) && victim->hit>victim->max_hit*0.85)
  {
    stc("{rУжасная боль пронизывает твоего противника!{X\n\r",ch);
    stc("{rТы чувствуешь ужасную боль во всем теле!{X\n\r",victim);
    act("{W$c2 {rпричиняет ужасную боль {W$c1{X!",ch,NULL,NULL,TO_NOTVICT);
    victim->hit = victim->hit - victim->max_hit/2;
    WAIT_STATE(ch,3*PULSE_VIOLENCE);
  }
  else WAIT_STATE(ch,1*PULSE_VIOLENCE);
}

void spell_cursed_lands( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AREA_DATA *pArea = ch->in_room->area;
  ROOM_INDEX_DATA *room=ch->in_room;
  DESCRIPTOR_DATA *d;
  RAFFECT *raf;
  AFFECT_DATA af;
  int64 i;
  int skill=get_skill(ch,sn);
  
  if (is_affected(ch,sn))
  {
    stc("У тебя не хватает сил.\n\r",ch);
    return;
  }         
   
  if (IS_SET(room->room_flags, ROOM_LAW)|| IS_SET(room->room_flags, ROOM_SAFE))
  {
    stc("Боги защищают это место.\n\r",ch);
    return;
  }

  if (IS_SET(ch->in_room->ra, RAFF_EVIL_PR))
  {
    stc("Найди чистое место. Здесь уже проклято.\n\r",ch);
    return;
  }
  
  stc("Великое проклятие заполняет эти земли.\n\r",ch);
  for (i=pArea->min_vnum;i<pArea->max_vnum;i++)
  {
    room = get_room_index(i);
    if (!room) continue;
    if ( !IS_SET(room->room_flags, ROOM_LAW) 
      && !IS_SET(room->room_flags, ROOM_SAFE)
      && !IS_SET(room->ra, RAFF_EVIL_PR) 
      && number_percent() < skill  )
    {
      if (IS_SET(room->ra, RAFF_LIFE_STR))
      {
        raf=get_raffect(ch->in_room, RAFF_EVIL_PR);
        if (raf->level<level) raffect_from_room(raf);
      }
      else
      {
        raf=new_raffect();
        raf->level=level;
        raf->duration=level/5;
        raf->bit=RAFF_EVIL_PR;
        raffect_to_room( raf,room);
      }
    }    
  }
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 24;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  for (d=descriptor_list;d;d=d->next)
  {
    if (d->character && d->connected==CON_PLAYING 
     && d->character->in_room->area == ch->in_room->area)
     act("$c1 проклинает местность вокруг тебя!",ch,NULL,d->character,TO_VICT);
  }
}

void spell_nightfall( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AREA_DATA *pArea = ch->in_room->area;
  ROOM_INDEX_DATA *room;
  RAFFECT *raf;
  AFFECT_DATA af;
  int64 i;
  int skill=get_skill(ch,sn);
  
  if ( is_affected(ch, sn) )
  {
    stc("У тебя на достаточно сил.\n\r",ch);
    return;
  }         
   
  if ( IS_SET(ch->in_room->ra, RAFF_BLIND) )
  {
    stc("Ночь уже властвует здесь.\n\r",ch);
    return;
  }
  
  stc("{DНочь{X опускается на эту местность.\n\r",ch);
  act("{W$c2{X вызывает {DНочь{X!",ch,NULL,NULL,TO_ROOM);

  for (i=pArea->min_vnum;i<pArea->max_vnum;i++)
  {
    room = get_room_index(i);
    if ( !room ) continue;
    if ( number_percent() < skill 
         && !IS_SET(room->room_flags, ROOM_LAW) 
         && !IS_SET(room->room_flags, ROOM_SAFE) )
     {
       raf=new_raffect();
       raf->level=level;
       raf->duration=level/10;
       raf->bit=RAFF_BLIND;
       raffect_to_room( raf,room);
     }    
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = ch->level;
   af.duration  = 24;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;
   affect_to_char( ch, &af );
}

void spell_vision( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  
  if (IS_AFFECTED(ch,AFF_VISION))
  {
    stc("Ты уже видишь во тьме.\n\r",ch);
    return;
  }
  stc("Теперь ты видишь во Тьме.\n\r",ch);
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = level;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_VISION;
  affect_to_char( ch, &af );
}   


void spell_power_dark( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  
  stc("Ты чувствуешь как {DСила Ночи{X разливается по твоему телу.\n\r",ch);
  act("Сила {DНочи{X наполяет {W$c2{X.",ch,NULL,NULL,TO_ROOM);
   
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = level;
  af.location  = APPLY_DAMROLL;
  af.modifier  = 2*level/5;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  af.location  = APPLY_AC;
  af.modifier  = -2*level;
  affect_to_char( ch, &af );

  vo = (void *) ch;
  sn=skill_lookup("protection good");
  (*skill_table[sn].spell_fun) (sn,level,ch,vo, TAR_CHAR_SELF);
}   

void spell_dark_swarm( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice ( level, 15 );
  if ( saves_spell(level, victim, DAM_POISON ) ) dam /=2;
  damage( ch, victim, dam, sn, DAM_POISON, TRUE, FALSE,NULL);

  (*skill_table[gsn_blindness].spell_fun) (sn,level,ch,vo, TAR_CHAR_OFFENSIVE);
  (*skill_table[gsn_poison].spell_fun) (sn,level,ch,vo, TAR_OBJ_CHAR_OFF);
  (*skill_table[skill_lookup("weaken")].spell_fun) (sn,level,ch,vo, TAR_CHAR_OFFENSIVE);
  (*skill_table[skill_lookup("chill touch")].spell_fun) (sn,level,ch,vo, TAR_CHAR_OFFENSIVE);
}

void spell_daylight( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AREA_DATA *pArea = ch->in_room->area;
  ROOM_INDEX_DATA *room;
  AFFECT_DATA af;
  int64 i;
  
  if ( is_affected(ch, sn) )
  {
    stc("У тебя на достаточно сил.\n\r",ch);
    return;
  }         

  stc("Яркий свет рассеивает {DНочь{X.\n\r",ch);
  act("Яркий свет, вызванный {W$c2{X рассеивает {DНочь{X!",ch,NULL,NULL,TO_ROOM);

  for (i=pArea->min_vnum;i<pArea->max_vnum;i++)
  {
    room = get_room_index(i);
    if (!room) continue;
    raffect_from_room(get_raffect(room,RAFF_BLIND));
  }
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = ch->level;
  af.duration  = 24;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
}

void spell_consecrate( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AREA_DATA *pArea = ch->in_room->area;
  DESCRIPTOR_DATA *d;
  ROOM_INDEX_DATA *room;
  RAFFECT *raf;
  AFFECT_DATA af;
  int64 i;
  int skill=get_skill(ch,sn);
  
  if (is_affected(ch, sn))
  {
    stc("У тебя не достаточно сил.\n\r",ch);
    return;
  }         
  stc("Проклятие отступило.\n\r",ch);
  act("{W$c2{X снимает проклятие с этих мест!",ch,NULL,NULL,TO_ROOM);

  for (i=pArea->min_vnum;i<pArea->max_vnum;i++)
  {
    room = get_room_index(i);
    if (!room) continue;
    if (IS_SET(room->ra, RAFF_EVIL_PR) && number_percent()<skill)
    {
      raf=get_raffect(room,RAFF_EVIL_PR);
      if (raf->level<level) raffect_from_room(raf);
    }  
  }
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 12;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  for (d=descriptor_list;d;d=d->next)
  {
    if (d->character && d->connected==CON_PLAYING 
     && d->character->in_room->area == pArea)
     act("$c1 освящает местность вокруг тебя!",ch,NULL,d->character,TO_VICT);
  }
}

void spell_nimbus( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  
  stc("Маленький {Yнимб{X появляется над твоей головой.\n\r",ch);
  act("Маленький {Yнимб{X появляется над головой {W$c2{X.",ch,NULL,NULL,TO_ROOM);
   
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 10+level/10;
  af.bitvector = AFF_VISION;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  affect_to_char( ch, &af );

  af.location  = APPLY_HITROLL;
  af.modifier  = 2*level/5;
  affect_to_char( ch, &af );

  af.location  = APPLY_AC;
  af.modifier  = -2*level;
  affect_to_char( ch, &af );

  af.location  = APPLY_SAVING_SPELL;
  af.modifier  = -1*level/10;
  affect_to_char( ch, &af );

  vo = (void *) ch;
  sn=skill_lookup("protection evil");
  (*skill_table[sn].spell_fun) (sn,level,ch,vo, TAR_CHAR_SELF);
}   

void spell_mummy( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{ 
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  OBJ_DATA *obj;
  AFFECT_DATA af,*aff;
  bool found=FALSE;
  char buf[MAX_STRING_LENGTH];

  if (!IS_EVIL(ch))
  {
    stc("Ты не можешь применить заклинание {RЗЛА{x.\n\r",ch);
    return;
  }

  for (aff=victim->affected;aff;aff=aff->next)
  { 
    if (aff->type==sn)
    {
      found=TRUE;
      break;
    } 
  } 
  if (!found)
  {
    if (victim->hit > victim->max_hit/20)
    {
      ptc(ch,"%s не может быть подверженным действию твоего заклятья.\n\r",victim->name);
      act("{Y#c1{x произносит {Dчерное заклинание{x, но ничего не происходит.",ch,NULL,NULL,TO_ROOM);
      return;
    }
 
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act("$c1 произносит {DЧерное заклинание{x, ты теряешь сознание.",ch,NULL,victim,TO_VICT);
    ptc(ch,"Ты произносишь {DЧерное заклинание{x, затуманивая сознание {Y%s{x.\n\r",victim->name);
    act("$c1 произносит {DЧерное заклинание{x над телом $C2.",ch,NULL,victim,TO_NOTVICT);
    if (victim->position>=POS_RESTING) act("$c1 падает без сознания.",victim,NULL,NULL,TO_ROOM);
    victim->hit=0;
    update_pos( victim );
    stop_fighting(ch,TRUE);
    return;
  }
  if (aff->duration<=10)
  {
    aff->duration=20;
    aff->level=level;
    stc("{RНечто ПЛОХОЕ происходит с тобой.{x\n\r",victim);
    ptc(ch,"Ты произносишь {DЧерное заклинание{x, иссушая тело {Y%s{x.\n\r",victim->name);
    act("{Y$c1{x произносит {DЧерное заклинание{x, иссушая тело {Y$C2{x.",ch,NULL,victim,TO_NOTVICT);
    victim->hit=0;
    update_pos(victim);
    return;
  }
  aff->duration=level;
  aff->level=level;
  ch->mana=0;
  if (victim->position>=POS_RESTING) act("$c1 произносит {DЧерное Заклинание{x.",ch,NULL,victim,TO_VICT);
  stc("{GТы возвращаешься к жизни... {Rвернее к смерти! Из тебя сделали {Dмумию{R!{x\n\r",victim);
  ptc(ch,"Ты произносишь {DЧерное заклинание{x, превращая {Y%s{x в мумию.\n\r",victim->name);
  act("{Y$c1{x произносит {DЧерное заклинание{x, превращая тело {Y$C2{x в мумию.",ch,NULL,victim,TO_NOTVICT);

  obj = create_object(get_obj_index(OBJ_VNUM_MUMMY), victim->level);
  if (!IS_NPC(victim))
  {
    victim->hit=1;
    update_pos(victim);
    victim->morph_obj = obj;
    victim->was_in_room = victim->in_room;
    obj->is_morphed=TRUE;
    obj->morph_name=victim;
    char_from_room( victim );
    char_to_room(victim, get_room_index(ROOM_VNUM_LIMBO));
  }
  do_printf(buf,"Высушенная {Dмумия{x из тела {Y%s{x.",victim->name);
  free_string(obj->short_descr);
  free_string( obj->description );
  obj->short_descr = str_dup( buf );
  obj->description = str_dup( buf );

  do_printf(buf,"%s mummy мумия",victim->name);
  free_string( obj->name);
  obj->name = str_dup( buf );
  obj_to_room( obj, ch->in_room );
  if (IS_NPC(victim)) extract_char(victim,TRUE);
}

void spell_mind_blast(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (!IS_IMMORTAL(ch))
    {
      if (get_curr_stat(ch,STAT_INT)+get_curr_stat(ch,STAT_WIS)<52)
      {
        ptc(ch,"У тебя недостаточно способностей для мозговой атаки.\r\n");
        return;
      }
      
      if (ch->classwar==1)
      {
        ptc(ch,"Воины не могут использовать мозговую атаку.\r\n");
        return;
      }

      if (get_curr_stat(victim,STAT_WIS)<26 &&
         (get_curr_stat(victim,STAT_INT)+get_curr_stat(victim,STAT_WIS)<52))
      {
        ptc(ch,"Ты не можешь проникнуть в его мозг ввиду отсутствия объекта атаки.\r\n");
        return;
      }
    }

    act("$c1 пытается разрушить мозг $C4.",ch,NULL,victim,TO_NOTVICT);
    act("$c1 проникает в твой мозг!",ch,NULL,victim,TO_VICT);
    act("Ты пытаешься разрушить мозг $C4.",ch,NULL,victim,TO_CHAR);

    dam = dice(level,5);
    dam += dam*get_int_modifier(ch,victim,skill_lookup("mind blast"),DAM_HARM)/200;
    damage(ch,victim,dam,sn,DAM_HARM,TRUE, FALSE, NULL);
}

void spell_shine_of_heaven( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    AFFECT_DATA af;
    int i;

    act("$c1 взывает к небесам и яркий свет заполняет комнату.",ch,NULL,NULL,TO_ROOM);
    act("Ты  взываешь к небесам и яркий свет заполняет комнату.",ch,NULL,NULL,TO_CHAR);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;

        if (is_safe_spell(ch,vch,TRUE)) continue;
        if (IS_GUARD(vch)) continue;

        check_criminal(ch,vch,60);
        if (!PK_RANGE(vch,ch)) add_pkiller(vch,ch);
 
        if ( IS_AFFECTED(vch, AFF_BLIND)) 
        { 
            act("{y$C4{x уже ослеплен.",ch,NULL,vch,TO_CHAR);
            continue;
        } 

        if ( (vch==victim && number_percent()<50) 
          || (vch!=victim && number_percent()<25) )
        {
            i=number_range(1,20);
            if      ( i==1 ) af.duration=4; //5%
            else if ( i<4  ) af.duration=3; //10%
            else if ( i<8  ) af.duration=2; //20%
            else if ( i<17 ) af.duration=1; //45%
            else             af.duration=0; //20%

            af.where     = TO_AFFECTS;
            af.type      = sn;
            af.level     = level;
            af.location  = APPLY_HITROLL;
            af.modifier  = -level/10;
            af.bitvector = AFF_BLIND;
            affect_to_char( vch, &af );
            stc( "Твой противник поражен ярким светом.\n\r", ch );
            stc( "Твои глаза поражены ярким светом!\n\r", vch );
//            act("$c1 поражен ярким светом.",vch,NULL,NULL,TO_ROOM);
            if ( vch->race==RACE_VAMPIRE 
              || vch->race==RACE_SKELETON 
              || vch->race==RACE_ZOMBIE )
              damage( ch, vch, dice(level*3/4, 3), sn, DAM_HOLY, TRUE, FALSE, NULL );

            multi_hit( vch, ch);
        }
    }
}
