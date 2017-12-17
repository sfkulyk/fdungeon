// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"
#include "interp.h"
#include "recycle.h" 

// Local functions.
void  raw_kill      args((CHAR_DATA *victim));
bool  remove_obj    args((CHAR_DATA *ch, int iWear, bool fReplace));
void  wear_obj      args((CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool w_left));
void  obj_to_keeper args((OBJ_DATA *obj, CHAR_DATA *ch ));
bool  check_skill   args((CHAR_DATA *ch, int gsn_skill));
int64 get_cost      args((CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ));
void  add_stealer(CHAR_DATA *ch, CHAR_DATA *stealer);
void  add_pkiller(CHAR_DATA *ch, CHAR_DATA *killer);
void  do_give    (CHAR_DATA *ch, const char *argument );
void  sac_obj    (CHAR_DATA *ch, OBJ_DATA *obj);
bool  give_one   (CHAR_DATA *ch,CHAR_DATA *victim,OBJ_DATA *obj);
void  do_ear     (CHAR_DATA *victim,CHAR_DATA *ch);
bool  is_number  (const char *arg );
bool  is_same_obj(OBJ_DATA *obj, OBJ_DATA *nextobj);
void  talk_auction   (char *argument);
char * local_outtext ( int64 count, char *string_one, char *string_many);
void  show_list_to_char   args((OBJ_DATA *list, CHAR_DATA *ch,bool fShort, bool fShowNothing));
CHAR_DATA * find_keeper   args((CHAR_DATA *ch ));
OBJ_DATA * get_obj_keeper args((CHAR_DATA *ch,CHAR_DATA *keeper, const char *argument));
static void do_sacrifice1 args((CHAR_DATA *ch, OBJ_DATA *obj, bool isOne, int count));
bool add_clanskill(CLAN_DATA *clan, int sn, int64 time);
int64 iObjCount;
bool  bFlushText;

// RT part of the corpse looting code
bool can_loot(CHAR_DATA *ch, OBJ_DATA *obj)
{
  CHAR_DATA *owner=NULL, *wch;
  if (IS_IMMORTAL(ch)) return TRUE;
  if (IS_NPC(ch) && (obj->pIndexData->vnum==26 || obj->pIndexData->vnum==120
    || obj->pIndexData->vnum==121 || obj->pIndexData->vnum==122 
    || obj->pIndexData->vnum==123)) return FALSE; 
 
  if (!obj->owner) return TRUE;

  for(wch=char_list;wch;wch=wch->next)
   if (!str_cmp(wch->name,obj->owner)) owner = wch;

  if (!owner) return TRUE;
  if (!str_cmp(ch->name,owner->name)) return TRUE;
  if (!IS_NPC(owner)) return TRUE;
  if (is_same_group(ch,owner)) return TRUE;
  return FALSE;
}

char* can_get_obj (CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container, bool fSelf)
{
  CHAR_DATA *gch;

  if (!ch || !obj)
  {
    bug ("can_get_obj: invalid...",0);
    return "BUG - please report to imm";
  }
  if (!can_see_obj(ch,obj))
    return (fSelf?"Ты не видишь этого." : "$C1 не видит $i4.");

  if (!CAN_WEAR(obj,ITEM_TAKE))
    return (fSelf?"Ты не можешь взять $i4." : "$C1 не может взять $i4.");

  /* А может он хочет украсть чужой quest item? */
  if ( obj->owner && obj->item_type!=ITEM_CORPSE_PC && !IS_IMMORTAL(ch)
    && (IS_NPC(ch) || strcmp(obj->owner,ch->name)))
    return (fSelf ? "Ты не владелец $i2!" : "$C1 не хозяин $i2!" );

  /* А не хочет ли он спереть чужой corpse? */
  if (!can_loot(ch, obj))
    return (fSelf?"Тебе нельзя трогать этот труп.":"$C1 не может взять $i4.");

  if (container)
  {
    if ( container->item_type != ITEM_CONTAINER
      && container->item_type != ITEM_CORPSE_NPC
      && container->item_type != ITEM_CORPSE_PC)
      return "$I1 не контейнер.";

  if (IS_SET(container->value[1],CONT_CLOSED)) 
    return "Для начала открой $i4.";

  if (container->pIndexData->vnum == OBJ_VNUM_PIT && !IS_IMMORTAL(ch))
    return "Не стоит жадничать!";

  if (container->item_type == ITEM_CORPSE_PC && !can_loot(ch, container) )
    return (fSelf ? "Тебе нельзя трогать этот труп." : "$C1 не может взять $i4." );

    return NULL;
  }

  if (obj->in_room != NULL)
  {
    for (gch=obj->in_room->people;gch!=NULL;gch=gch->next_in_room)
    {
      if (gch->on==obj) return "Hо $C1 использует $i4!";
    }
  }

  if (!IS_EAR(obj))
  if (ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    return (fSelf ? "Твои карманы полны, $i1 в них уже не помещается."
                                  : "Руки $C2 полны." );

  if ( (!obj->in_obj || obj->in_obj->carried_by != ch)
    &&  (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)))
    return (fSelf ? "$i1 весит для тебя слишком много."
                  : "$i1 весит для $C2 слишком много." );
  return NULL;
}

int local_get_obj ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container, bool IsOne )
{
  CHAR_DATA *gch;
  char temp[MAX_STRING_LENGTH];
  int members;
  char buffer[100];

  if ( !CAN_WEAR(obj, ITEM_TAKE) )
  {
    stc( "Ты не можешь это взять.\n\r", ch );
    return 0;
  }


  if (obj->owner && obj->enchanted && !IS_IMMORTAL(ch) && str_cmp(obj->owner,"(public)") && strcmp(obj->owner,ch->name))
  {     
     stc( "Нельзя брать чужие вещи.\n\r", ch );
     return 0;
  }

  if (IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE) && !strcmp(material_lookup(obj->material),"silver"))
  {
    stc("Ты не смеешь коснуться этого предмета.",ch);
    return 0;
  }

  /* Added to fix 'get all.diamong rock', where diamond count = 5000, TY  */
  bFlushText=TRUE;
  if (is_same_obj (obj, obj->next_content) && IsOne==0)
  {
    iObjCount++;
    bFlushText=FALSE;
  }

  if (!IS_EAR(obj))
  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
  {
    if (bFlushText)
    {
      act (local_outtext (iObjCount, "$d: ты не можешь нести так много вещей.",
           "$d {C[%d]{x: ты не можешь нести так много вещей."), 
           ch, NULL, obj->name, TO_CHAR); iObjCount=1; }
      return 0;
    }

  if ((!obj->in_obj || obj->in_obj->carried_by != ch)
   && (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)))
  {
    if (bFlushText)
    {
      act (local_outtext (iObjCount, "$d: ты не можешь нести такой большой вес.",
        "$d {C[%d]{x: ты не можешь нести такой большой вес."), 
        ch, NULL, obj->name, TO_CHAR); iObjCount=1; }
      return 0;
    }

    // А не хочет ли он спереть чужой corpse?
    if ( !can_loot(ch, obj) )
    {
      act("Нельзя воровать чужие трупы.",ch,NULL,NULL,TO_CHAR );
      return 0;
    }

    if (obj->in_room != NULL)
    {
      for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
      {
        if (gch->on == obj)
        {
          act("$C1, похоже, использует $i4.",ch,obj,gch,TO_CHAR);
          return 0;
        }
      }
    }
                
    if ( container)
    {
      if (container->pIndexData->vnum == OBJ_VNUM_PIT
       && get_trust(ch) < obj->level)
      {
        stc("Ты не в силах использовать это.\n\r",ch);
        return 0;
      }

    if (container->pIndexData->vnum == OBJ_VNUM_PIT
     && !CAN_WEAR(container, ITEM_TAKE)
     && !IS_OBJ_STAT(obj,ITEM_HAD_TIMER)) obj->timer=0; 

    if (bFlushText)
    {
      if (iObjCount!=1)
      { 
        do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
        act ("Ты берешь $t из $I2.", ch, temp, container, TO_CHAR);
      }
      else 
      act ("Ты берешь $i4 из $I2.", ch, obj, container, TO_CHAR);
      act (local_outtext (iObjCount, "$c1 берет $i4 из $I2.",
           "$c1 берет несколько $i8 из $I2.{x"),
           ch, obj, container, TO_ROOM);
      if (obj->morph_name)
      {
        ptc(obj->morph_name,"{y%s{x достает тебя из {c%s{x.\n\r",
          ch->name,get_obj_desc(container,'2'));
      }
      iObjCount=1;
    }

    REM_BIT(obj->extra_flags,ITEM_HAD_TIMER);
    obj_from_obj( obj );
    }
    else
    {
      if (bFlushText)
      {
        do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
        act (local_outtext(iObjCount,"Ты поднимаешь с земли $I4.",
             "Ты поднимаешь с земли $t."), ch, temp, obj, TO_CHAR);
        act (local_outtext (iObjCount, "$c1 поднимает с земли $i4.",
             "$c1 поднимает с земли несколько $i8.{x"), ch, obj, container, TO_ROOM);
        if (obj->morph_name!=NULL)
           ptc(obj->morph_name,"{y%s{x поднимает тебя с земли.\n\r", ch->name);
        iObjCount=1;
      }

      obj_from_room( obj );
    }

    if ( obj->item_type == ITEM_MONEY)
    {
      ch->silver += obj->value[0];
      ch->gold += obj->value[1];
      if (IS_CFG(ch,CFG_AUTOSPLIT))
      {
        members = 0;
        for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
      {
        if (!IS_AFFECTED(gch,AFF_CHARM) && is_same_group( gch, ch ) ) members++;
      }

      if ( members > 1 && (obj->value[0] > 1 || obj->value[1] >1))
      {
        do_printf(buffer,"%u %u",obj->value[0],obj->value[1]);
        do_function(ch, &do_split, buffer); 
      }
    }
    extract_obj( obj );
  }
  else  obj_to_char( obj, ch );
  return 1;
}

void do_get( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  OBJ_DATA *container;
  bool found;
  int number, count=0;
    
  iObjCount=1; /* Init object counter */
    
  number = mult_argument((char *)argument, arg);
  if (number==1) number=MAX_OBJS_VALUE;

  argument = one_argument( arg, arg1 );
  argument = one_argument( argument, arg2 );

  if (!str_cmp(arg2,"from")) argument = one_argument(argument,arg2);

  /* Get type. */
  if (EMPTY(arg1))
  {
    stc( "Взять что?\n\r", ch );
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"get")) return; 
 
  if ( arg2[0] == '\0' )
  {
    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) && number==MAX_OBJS_VALUE )
    {
      /* 'get obj' */
      obj = get_obj_list( ch, arg1, ch->in_room->contents );
      if ( obj == NULL )
      {
        act( "Тут нет $T.", ch, NULL, arg1, TO_CHAR );
        return;
      }
      local_get_obj( ch, obj, NULL, TRUE );
      return;
    }

    // 'get all' or 'get all.obj'
    found = FALSE;
    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
      obj_next = obj->next_content;
      if ( ( ( number==MAX_OBJS_VALUE  && ( arg1[3] == '\0' || is_name( &arg1[4], obj->name )))
       || ( number!=MAX_OBJS_VALUE && is_name (arg1, obj->name) ) )
         &&   can_see_obj( ch, obj ) )
      {
        found = TRUE;
        if (count+1==number) count+=local_get_obj( ch, obj, NULL, TRUE );
         else count+=local_get_obj( ch, obj, NULL, FALSE );
        if (number!=MAX_OBJS_VALUE && count>=number) return;
      }
    }

    if (!found) act(arg1[3] == '\0' ? "Тут ничего нет." : "Тут нет $T.",
       ch, NULL, &arg1[4], TO_CHAR);
    return;
  }

  // 'get ... container'
  if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
  {
    stc( "Ты не можешь этого сделать.\n\r", ch );
    return;
  }

  if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
  {
    act( "Тут нету $T.", ch, NULL, arg2, TO_CHAR );
    return;
  }

  switch ( container->item_type )
  {
    case ITEM_CONTAINER:  break;
    case ITEM_CORPSE_NPC: break;
    case ITEM_CORPSE_PC:
      if (!can_loot(ch,container))
      {
        stc( "Ты не можешь этого сделать.\n\r", ch );
        return;
      }
      break;
    default:
      stc( "Это не контейнер.\n\r", ch );
      return;
  }

  if ( IS_SET(container->value[1], CONT_CLOSED) )
  {
    stc("Сначала это нужно открыть.\n\r",ch);
    return;
  }

  if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) && number==MAX_OBJS_VALUE )
  {
    /* 'get obj container' */
    obj = get_obj_list( ch, arg1, container->contains );
    if ( obj == NULL ) act( "В $T нет ничего похожего.", ch, NULL, arg2, TO_CHAR );
     else local_get_obj( ch, obj, container, TRUE );
    return;
  }

  // 'get all container' or 'get all.obj container'
  found = FALSE;
  for ( obj = container->contains; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if ( ( ( number==MAX_OBJS_VALUE  && ( arg1[3] == '\0' || is_name( &arg1[4], obj->name )))
     || ( number!=MAX_OBJS_VALUE && is_name (arg1, obj->name) ) )
     &&   can_see_obj( ch, obj ) )
    {
      found = TRUE;
      if (count+1==number) count+=local_get_obj( ch, obj, container, TRUE );
       else count+=local_get_obj( ch, obj, container, FALSE );
      if (number!=MAX_OBJS_VALUE && count>=number) return;
    }
  }
  if (!found) act(arg1[3] == '\0' ? "В $T пусто." : "В $T нет ничего похожего.",ch, NULL, arg2, TO_CHAR );
}

void do_put( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char temp[MAX_STRING_LENGTH];
  OBJ_DATA *container;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  int number=1, foundobjs=0;
  int bFlush=TRUE; 
  int64 iObjCount=1; 

  number = mult_argument((char *)argument, arg);
  argument = one_argument( arg, arg1 );
  argument = one_argument( argument, arg2 );

  if (number==1) number=MAX_OBJS_VALUE;

  if (!str_cmp(arg2,"in") || !str_cmp(arg2,"on")) argument = one_argument(argument,arg2);

  if ( EMPTY(arg1) || EMPTY(arg2))
  {
    stc( "Положить что и куда?\n\r", ch );
    return;
  }

  if (!str_cmp(arg2,"all") || !str_prefix("all.",arg2))
  {
    stc( "Ты не можешь этого сделать.\n\r", ch );
    return;
  }

  if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
  {
    act( "Ты не видишь тут $T.", ch, NULL, arg2, TO_CHAR );
    return;
  }

  if ( container->item_type != ITEM_CONTAINER )
  {
   stc( "Это не контейнер.\n\r", ch );
   return;
  }

  if ( IS_SET(container->value[1], CONT_CLOSED) )
  {
    stc("Сначала это нужно открыть.\n\r",ch);
    return;
  }

  if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) && number==MAX_OBJS_VALUE)
  {
    /* 'put obj container' */
    if (!(obj=get_obj_carry(ch,arg1,ch)))
    {
      stc( "У тебя этого нет.\n\r", ch );
      return;
    }

    if (obj==container)
    {
      stc( "Вещь сама в себя? Оригинально.\n\r", ch );
      return;
    }

    if (WEIGHT_MULT(obj) != 100)
    {
      stc("Плохая идея.\n\r",ch);
      return;
    }

    if (!can_drop_obj( ch, obj ))
    {
      stc( "Ты не можешь избавиться от этого.\n\r", ch );
      return;
    }

    if (get_obj_weight( obj ) + get_true_weight( container ) > (container->value[0] * 10) 
        || get_obj_weight(obj) > (container->value[3] * 10))
    {
      stc("Не лезет.\n\r",ch);
      return;
    }
    
    if (container->pIndexData->vnum==OBJ_VNUM_PIT && !CAN_WEAR(container,ITEM_TAKE))
    {
      if (obj->timer) SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
      else obj->timer=number_range(100,200);
    }

    obj_from_char(obj);
    obj_to_obj(obj, container);

    if (IS_SET(container->value[1],CONT_PUT_ON))
    {
      act("$c1 кладет $i4 на $I4.",ch,obj,container, TO_ROOM);
      act("Ты кладешь $i4 на $I4.",ch,obj,container, TO_CHAR);
      if (obj->morph_name!=NULL) 
        ptc(obj->morph_name,"{y%s{x кладет тебя на {c%s{x.\n\r", 
          ch->name, get_obj_desc(container,'1'));
    }
    else
    {
      act( "$c1 кладет $i4 в $I4.", ch, obj, container, TO_ROOM );
      act( "Ты кладешь $i4 в $I4.", ch, obj, container, TO_CHAR );
      if (obj->morph_name!=NULL)
        ptc(obj->morph_name,"{y%s{x кладет тебя в {c%s{x.\n\r",
          ch->name, get_obj_desc(container,'1'));
    }
    return;
  }

  // 'put all container' or 'put all.obj container'
  for ( obj = ch->carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;

    if (can_see_obj(ch,obj)
      && ((number==MAX_OBJS_VALUE
      &&  ( arg1[3] == '\0' || is_name( &arg1[4], obj->name )))
      ||  (number!=MAX_OBJS_VALUE && is_name (arg1, obj->name)))
      && WEIGHT_MULT(obj) == 100
      && obj->wear_loc == WEAR_NONE
      && obj != container
      && can_drop_obj( ch, obj ))
    {
      if ( get_obj_weight( obj ) + get_true_weight( container ) > (container->value[0] * 10) 
        ||   get_obj_weight(obj) > (container->value[3] * 10))
      {
        act ("$i1 не помещается.", ch, obj, container, TO_CHAR);
        continue;
      }
      foundobjs++; /* object found */
      bFlush=TRUE;
      if (is_same_obj (obj, obj->next_content) && foundobjs<number )
      {
        bFlush=FALSE;
        iObjCount++;
      }

      if (container->pIndexData->vnum == OBJ_VNUM_PIT && !CAN_WEAR(obj, ITEM_TAKE) )
      {
        if (obj->timer) SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
        else obj->timer = number_range(100,200);
      }

      obj_from_char( obj );
      obj_to_obj( obj, container );

      if (IS_SET(container->value[1],CONT_PUT_ON))
      {
        if (bFlush)
        {
          act (local_outtext (iObjCount, "$c1 кладет $i4 на $I4.",
            "$c1 кладет несколько $i8 на $I4."), ch, obj, container, TO_ROOM);
          do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
          act (local_outtext (iObjCount, "Ты кладешь $t на $I4.",
            "Ты кладешь $t в $I4."), ch, temp, container, TO_CHAR);
          if (obj->morph_name!=NULL)
          {
            ptc(obj->morph_name,"{y%s{x кладет тебя в {c%s{x.\n\r", ch->name, get_obj_desc(container,'1'));
          }
          iObjCount=1;
        }
      }
      else
      {
        if (bFlush)
        {
          act (local_outtext (iObjCount, "$c1 кладет $i4 в $I4.",
            "$c1 кладет несколько $i8 в $I4."), ch, obj, container, TO_ROOM);
          do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
          act (local_outtext (iObjCount, "Ты кладешь $t в $I4.",
            "Ты кладешь $t в $I4."), ch, temp, container, TO_CHAR);
          if (obj->morph_name!=NULL)
          {
            ptc(obj->morph_name,"{y%s{x кладет тебя в {c%s{x.\n\r", ch->name, get_obj_desc(container,'1'));
          }
          iObjCount=1;
        }
      }
      if (foundobjs>=number) return;
    }
  }
}

void do_drop( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char temp[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  bool found;
  int bFlush, number = 1, count=0;
  int64 iObjCount=1; 
  
  number = mult_argument((char *)argument, arg2);
  argument = one_argument( arg2, arg );

  if (number==1) number=MAX_OBJS_VALUE;

  if ( arg[0] == '\0' )
  {
    stc( "Бросить что?\n\r", ch );
    return;
  }

  if (is_number( arg ))
  {
    // 'drop NNNN coins'
    int64 amount; 
    int64 gold = 0, silver = 0;

    amount   = atoi64(arg);
    argument = one_argument( argument, arg );
    if ( amount <= 0  || ( str_cmp( arg, "coins" ) && str_cmp( arg, "coin" )
      && str_cmp( arg, "gold"  ) && str_cmp( arg, "silver") ) )
    {
      stc( "Ты не можешь этого сделать.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg, "coins") || !str_cmp(arg,"coin") 
      ||   !str_cmp( arg, "silver"))
    {
      if (ch->silver < amount)
      {
        stc("У тебя нет столько серебра.\n\r",ch);
        return;
      }
      ch->silver = ch->silver-amount;
      silver = amount;
    }
    else
    {
      if (ch->gold < amount)
      {
        stc("У тебя нет столько золота.\n\r",ch);
        return;
      }
      ch->gold = ch->gold-amount;
      gold = amount;
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
      obj_next = obj->next_content;

      switch ( obj->pIndexData->vnum )
      {
        case OBJ_VNUM_SILVER_ONE:
          silver++;
          extract_obj(obj);
          break;
        case OBJ_VNUM_GOLD_ONE:
          gold++;
          extract_obj( obj );
          break;
        case OBJ_VNUM_SILVER_SOME:
          silver = silver+obj->value[0];
          extract_obj(obj);
          break;
        case OBJ_VNUM_GOLD_SOME:
          gold = gold+obj->value[1];
          extract_obj( obj );
          break;
        case OBJ_VNUM_COINS:
          silver = silver+obj->value[0];
          gold = gold+obj->value[1];
          extract_obj(obj);
          break;
      }
    }
    obj_to_room( create_money( gold, silver ), ch->in_room );
    act( "$c1 бросает несколько монет.", ch, NULL, NULL, TO_ROOM );
    stc( "OK.\n\r", ch );
    return;
  }

  if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) && number==MAX_OBJS_VALUE )
  {
    // 'drop obj'
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
      stc( "У тебя этого нет.\n\r", ch );
      return;
    }

    if ( !can_drop_obj( ch, obj ) || ((obj->morph_name!=NULL) && IS_SET(ch->in_room->room_flags,ROOM_ARENA)))
    {
      stc( "Ты не можешь избавиться от этого.\n\r", ch );
      return;
    }

    obj_from_char( obj );
    obj_to_room( obj, ch->in_room );
    act( "$c1 бросает $i4.", ch, obj, NULL, TO_ROOM );
    act( "Ты бросаешь $i4.", ch, obj, NULL, TO_CHAR );
    if (obj->morph_name!=NULL)
       ptc(obj->morph_name,"{y%s{x бросает тебя на землю.\n\r", ch->name);
    if (IS_OBJ_STAT(obj, ITEM_MELT_DROP))
    {
      act("$i1 испаряется в дымке.",ch,obj,NULL,TO_ROOM);
      act("$i1 испаряется в дымке.",ch,obj,NULL,TO_CHAR);
      extract_obj(obj);
    }
    return;
  }

  // 'drop all' or 'drop all.obj'
  found = FALSE;
  for ( obj = ch->carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;

    if ( ( ( number==MAX_OBJS_VALUE  && ( arg[3] == '\0' || is_name( &arg[4], obj->name )))
      || ( number!=MAX_OBJS_VALUE && is_name (arg, obj->name) ) )
      && can_see_obj( ch, obj )
      && obj->wear_loc == WEAR_NONE
      && can_drop_obj( ch, obj ) 
      && !((obj->morph_name!=NULL) && IS_SET(ch->in_room->room_flags,ROOM_ARENA)))
    {
      count++;
      bFlush=TRUE;
      if (is_same_obj (obj, obj->next_content) && count<number)
      {
        iObjCount++;
        bFlush=FALSE;
      }
      found = TRUE;
      obj_from_char( obj );
      obj_to_room( obj, ch->in_room );
      if (obj->morph_name!=NULL)
               ptc(obj->morph_name,"{y%s{x бросает тебя на землю.\n\r", ch->name);
      if (bFlush )
      {
        act (local_outtext (iObjCount, "$c1 бросает $i4.",
          "$c1 бросает несколько $i8."), ch, obj, NULL, TO_ROOM);
        do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
        act ("Ты бросаешь $t.", ch, temp, NULL, TO_CHAR);
          iObjCount=1;
      }

      if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
      {
        if (bFlush)
        {
          act("$i1 испаряется в дымке.",ch,obj,NULL,TO_ROOM);
                      act("$i1 испаряется в дымке.",ch,obj,NULL,TO_CHAR);
          if (obj->morph_name!=NULL)
          {
            do_printf(buf,"Ты испаряешься в дымке.\n\r");
            stc(buf,obj->morph_name);
          }
        }
        extract_obj(obj);
      }
      if (number!=MAX_OBJS_VALUE && count>=number) return;
    }
  }
  if ( !found )
  {
    if ( arg[3] == '\0' )  act( "Ты ничего подобного не несешь.",  ch, NULL, arg, TO_CHAR );
    else act( "Ты не несешь $T.",  ch, NULL, &arg[4], TO_CHAR );
  }
}

void do_give( CHAR_DATA *ch, const char *argument )
{
  char arg  [MAX_INPUT_LENGTH];
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char temp[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA  *obj;
  OBJ_DATA *obj_next;
  bool all;
  bool found;
  int bFlush, number, count=0;
  int64 iObjCount=1; 
  char *check;

  number = mult_argument((char *)argument, arg);
  argument = one_argument( arg, arg1 );
  argument = one_argument( argument, arg2 );

  if (number==1) number=MAX_OBJS_VALUE;

  if ( arg1[0] == '\0' )
  {
    stc( "Что дать?\n\r", ch );
    return;
  }

  if ( arg2[0] == '\0' )
  {
    stc( "Кому дать?\n\r", ch );
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) // tipsy by Dinger 
   if (tipsy(ch,"give")) return; 
 
  if ( is_number( arg1 ) )
  {
    // 'give NNNN coins victim'
    int64 amount;
    bool silver;

    amount   = atoi64(arg1);
    if ( amount <= 0
      || ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" )
      && str_cmp( arg2, "gold"  ) && str_cmp( arg2, "silver")) )
    {
      stc( "Ты не можешь этого сделать.\n\r", ch );
      return;
    }

    silver = str_cmp(arg2, "gold");

    argument = one_argument( argument, arg2 );
    if ( arg2[0] == '\0' )
    {
      stc( "Кому дать?\n\r", ch );
      return;
    }

    victim = get_char_room(ch, arg2);
    if ( victim == NULL ) return;

    if ( (!silver && ch->gold < amount) || (silver && ch->silver < amount) )
    {
      stc( "У тебя нет достаточного количества.\n\r", ch );
      return;
    }

    if (silver)
    {
      ch->silver      -= amount;
      victim->silver  += amount;
    }
    else
    {
      ch->gold     -= amount;
      victim->gold += amount;
    }

    do_printf(buf,"$c1 дает тебе %u %s.", amount, silver ? "серебра (silver)" : "золота (gold)");
    act( buf, ch, NULL, victim, TO_VICT);
    act( "$c1 дает денег $C3.",  ch, NULL, victim, TO_NOTVICT );
    do_printf(buf,"Ты даешь $C3 %u %s.", amount, silver ? "серебра (silver)" : "золота (gold)");
    act( buf, ch, NULL, victim, TO_CHAR    );

    // Bribe trigger
    if ( IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_BRIBE) )
      mp_bribe_trigger(victim, ch, silver ? amount : amount * 100);

    if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER))
    {
      int64 change;

      change = (silver ? 95 * amount / 100 / 100 : 95 * amount);

      if (!silver && change > victim->silver) victim->silver += change;

      if (silver && change > victim->gold) victim->gold += change;

      if (change < 1 && can_see(victim,ch,NOCHECK_LVL))
      {
        act("$c1 говорит тебе '{GПрошу прощения, этого не достаточно для обмена.{w'{x",
                    victim, NULL, ch, TO_VICT);
        /* ch->reply = victim; */
        do_printf(buf,"%u %s %s", amount, silver ? "серебра (silver)" : "золота (gold)",
                    ch->name);
        do_function(victim, &do_give, buf);
      }
      else if (can_see(victim, ch,NOCHECK_LVL))
      {
        do_printf(buf,"%u %s %s", change, silver ? "gold" : "silver",ch->name);
          do_function(victim, &do_give, buf);
        if (silver)
        {
          do_printf(buf,"%u silver %s", (95 * amount / 100 - change * 100),ch->name);
          do_function(victim, &do_give, buf);
        }
        act("$c1 говорит тебе 'Спасибо, приходите еще.'", victim,NULL,ch,TO_VICT);
        // ch->reply = victim;
      }
    }
    return;
  }

  all = !str_cmp( arg1, "all" ) || !str_prefix( "all.", arg1);

  found = FALSE;
  victim = get_char_room(ch, arg2);
  if ( victim == NULL ) return;

  if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
  {
    act("$C1 говорит тебе 'Простите, вы это уже продали.'",
    ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_NPC(victim)) if (victim->pIndexData->vnum>23079 && victim->pIndexData->vnum<23098)
  {
    act("$C1 говорит тебе 'Не надо мне ваших подачек!'",
    ch, NULL, victim, TO_CHAR);
    return;
  }

  for ( obj = ch->carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;

    if ( ( ( number==MAX_OBJS_VALUE  && ( (all && arg1[3] == '\0' )
      || is_name( all ? &arg1[4] : arg1, obj->name )))
      || ( number!=MAX_OBJS_VALUE && is_name (arg1, obj->name) ) )
      &&   can_see_obj( ch, obj )
      &&   obj->wear_loc == WEAR_NONE )
    {
      count++;
      found = TRUE;
      bFlush=TRUE;
      if (is_same_obj (obj, obj->next_content) && count<number
        && !(!all && number==MAX_OBJS_VALUE))
      {
        iObjCount++;
        bFlush=FALSE;
      }

      if ( obj->wear_loc != WEAR_NONE )
      {
        stc( "Да, но это экипировка, снимите (remove) для начала.\n\r", ch );
        return;
      }

      if ( !can_drop_obj( ch, obj ) )
      {
        stc( "Ты не можешь избавиться от этого.\n\r", ch );
        return;
      }

      check = can_get_obj (victim, obj, NULL, FALSE);
      if (check)
      {
        act_new ( check, ch, obj, victim, TO_CHAR, POS_DEAD);
        continue;
      }

      obj_from_char( obj );
      obj_to_char( obj, victim );

      if (bFlush)
      {
        act( local_outtext (iObjCount,"$c1 дает $i4 в руки $C2.",
          "$c1 передает несколько $i8 в руки $C2."),
          ch, obj, victim, TO_NOTVICT );
        do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
        act("$c1 дает тебе $t.", ch, temp, victim, TO_VICT );
        act("Ты даешь $t в руки $C2.", ch, temp, victim, TO_CHAR );
        if (obj->morph_name!=NULL)
        {
          do_printf(buf,"{y%s{x передает тебя в руки {y%s{x.\n\r", get_char_desc(ch,'1'), get_char_desc(victim,'2'));
          stc(buf,obj->morph_name);
        }
        iObjCount=1;
      }
      // Give trigger
      if ( IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_GIVE) )
        mp_give_trigger(victim, ch, obj);

      if ( count>=number || (!all && number==MAX_OBJS_VALUE) ) return;
    }
  }

  if ( !found )
  {
    if ( arg1[3] == '\0' ) stc( "Тут этого нет.\n\r", ch );
    else act( "Тут нет $T.", ch, NULL, &arg1[4], TO_CHAR );
  }
}

void do_envenom(CHAR_DATA *ch, const char *argument)
{
  OBJ_DATA *obj;
  AFFECT_DATA af;
  int percent,skill;

  if (EMPTY(argument))
  {
    stc("Отравить что?\n\r",ch);
    return;
  }

  obj = get_obj_list(ch,argument,ch->carrying);

  if (obj== NULL)
  {
    stc("У тебя нет этой вещи.\n\r",ch);
    return;
  }

  if ((skill = get_skill(ch,gsn_envenom)) < 1)
  {
    stc("Ты идиот! Ты отравишь себя!\n\r",ch);
    return;
  }

  if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
  {
    if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
    {
      act("У тебя не выйдет отравить $i4.",ch,obj,NULL,TO_CHAR);
      return;
    }

    if (number_percent() < skill)  /* success! */
    {
      act("$c1 наполняет $i4 сильным ядом.",ch,obj,NULL,TO_ROOM);
      act("Ты наполняешь $i4 сильным ядом.",ch,obj,NULL,TO_CHAR);
      if (obj->morph_name!=NULL)
        ptc(obj->morph_name,"{y%s{x наполняет тебя сильным ядом.\n\r", ch->name);
      if (!obj->value[3])
      {
        obj->value[3] = 1;
        check_improve(ch,gsn_envenom,TRUE,4);
      }
      WAIT_STATE(ch,skill_table[gsn_envenom].beats);    return;
    }

    act("У тебя не вышло отравить $i4.",ch,obj,NULL,TO_CHAR);
    if (!obj->value[3]) check_improve(ch,gsn_envenom,FALSE,4);
    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
    return;
  }

  if (obj->item_type == ITEM_WEAPON)
  {
   if ( IS_WEAPON_STAT(obj,WEAPON_FLAMING) || IS_WEAPON_STAT(obj,WEAPON_FROST)
     || IS_WEAPON_STAT(obj,WEAPON_VORPAL) || IS_OBJ_STAT(obj,ITEM_BLESS))
   {
     act("У тебя, похоже, не выйдет отравить $i4.",ch,obj,NULL,TO_CHAR);
     return;
   }

   if (obj->value[3] < 0 
     ||  attack_table[obj->value[3]].damage == DAM_BASH)
   {
     stc("Ты можешь отравить только оружие с острыми гранями.\n\r",ch);
     return;
   }

   if (IS_WEAPON_STAT(obj,WEAPON_POISON))
   {
     act("Кто-то уже отравил $i4.",ch,obj,NULL,TO_CHAR);
     return;
   }

   percent = number_percent();
   if (percent < skill)
   {
     af.where     = TO_WEAPON;
     af.type      = gsn_poison;
     af.level     = ch->level * (50 + percent/2) / 100 + category_bonus(ch,OFFENCE|MAKE);
     af.duration  = ch->level/5 * (50 + percent/2) / 100 + category_bonus(ch,MAKE);;
     af.location  = 0;
     af.modifier  = 0;
     af.bitvector = WEAPON_POISON;
     affect_to_obj(obj,&af);
 
     act("$c1 смазывает $i4 сильным ядом.",ch,obj,NULL,TO_ROOM);
     act("Ты смазываешь $i4 сильным ядом.",ch,obj,NULL,TO_CHAR);
     check_improve(ch,gsn_envenom,TRUE,3);
     WAIT_STATE(ch,skill_table[gsn_envenom].beats);
     return;
   }
   else
   {
     act("У тебя не вышло отравить $i4.",ch,obj,NULL,TO_CHAR);
     check_improve(ch,gsn_envenom,FALSE,3);
     WAIT_STATE(ch,skill_table[gsn_envenom].beats);
     return;
   }
 }
 act("Ты не можешь отравить $i4.",ch,obj,NULL,TO_CHAR);
}

void do_fill( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *fountain;
  bool found;

  argument = one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    stc( "Наполнить что?\n\r", ch );
    return;
  }

  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
  {
    stc( "У тебя нет этой вещи.\n\r", ch );
    return;
  }

  if (argument[0]!='\0') one_argument(argument,arg2);
  else arg2[0]='\0';
  found = FALSE;
  for ( fountain = ch->in_room->contents; fountain != NULL; fountain = fountain->next_content )
  {
    if ( fountain->item_type == ITEM_FOUNTAIN )
    {
     if ((arg2[0]!='\0' && is_name(argument,fountain->name)) || arg2[0]=='\0')
     {
      found = TRUE;
      break;
     }
    }
  }

  if ( !found )
  {
    stc( "Тут нет источника!\n\r", ch );
    return;
  }

  if ( obj->item_type != ITEM_DRINK_CON )
  {
    stc( "Ты не можешь наполнить это.\n\r", ch );
    return;
  }

  if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
  {
    stc( "Тут уже что-то есть.\n\r", ch );
    return;
  }

  if ((obj->value[0] != 0 && obj->value[1] == obj->value[0]))
  {
    stc( "Твой контейнер полон.\n\r", ch );
    return;
  }

  do_printf(buf,"Ты наполняешь $i4, наливая туда %s из $I2.",
    liq_table[fountain->value[2]].liq_showname);
  act( buf, ch, obj,fountain, TO_CHAR );
  do_printf(buf,"$c1 наполняет $i4, наливая туда %s из $I2.",
    liq_table[fountain->value[2]].liq_showname);
  act(buf,ch,obj,fountain,TO_ROOM);
  if (obj->morph_name!=NULL)
  {
    ptc(obj->morph_name,"{y%s{x наполняет тебя, наливая в тебя %s из {c%s{x.\n\r", ch->name, liq_table[fountain->value[2]].liq_showname, 
      get_obj_desc(fountain,'2'));
  }
  obj->value[2] = fountain->value[2];
  obj->value[1] = obj->value[0];
}
 
void do_pour (CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_STRING_LENGTH],buf[MAX_STRING_LENGTH];
  OBJ_DATA *out, *in;
  CHAR_DATA *vch = NULL;
  int64 amount;

  argument = one_argument(argument,arg);
    
  if (arg[0] == '\0' || argument[0] == '\0')
  {
    stc("Опорожнить что куда?\n\r",ch);
    return;
  }
    
  if ((out = get_obj_carry(ch,arg, ch)) == NULL)
  {
    stc("У тебя нет этой вещи.\n\r",ch);
    return;
  }

  if (out->item_type != ITEM_DRINK_CON)
  {
    stc("Это не емкость.\n\r",ch);
    return;
  }

  if (!str_cmp(argument,"out"))
  {
    if (out->value[1] == 0)
    {
      stc("Тут уже пусто.\n\r",ch);
      return;
    }
    out->value[1] = 0;
    out->value[3] = 0;
    do_printf(buf,"Ты переворачиваешь $i4, выливая %s на землю.",
                liq_table[out->value[2]].liq_showname);
    act(buf,ch,out,NULL,TO_CHAR);
        
    do_printf(buf,"$c1 переворачивает $i4, выливая %s на землю.",
                liq_table[out->value[2]].liq_showname);
    act(buf,ch,out,NULL,TO_ROOM);
    if (out->morph_name!=NULL)
      ptc(out->morph_name,"{y%s{x опорожняет тебя, выливая %s на землю.\n\r", ch->name, liq_table[out->value[2]].liq_showname);
    out->value[2] = 0;
    return;
  }

  if ((in = get_obj_here(ch,argument)) == NULL)
  {
    vch = get_char_room(ch,argument);

    if (vch == NULL)
    {
      stc("Опорожнить куда?\n\r",ch);
      return;
    }

    in = get_eq_char(vch,WEAR_HOLD);

    if (in == NULL)
    {
      stc("Они ничего нe несут.",ch);
      return;
    }
  }

  if (in->item_type != ITEM_DRINK_CON)
  {
    stc("Ты не можешь перелить жидкoсть сюда.\n\r",ch);
    return;
  }
    
  if (in == out)
  {
    stc("Ты не можешь изменить законы физики!\n\r",ch);
    return;
  }

  if (in->value[1] != 0 && in->value[2] != out->value[2])
  {
    stc("Коктейли смешиваешь? Не выйдет!\n\r",ch);
    return;
  }

  if (out->value[1] == 0)
  {
    act("В $i6 пусто.",ch,out,NULL,TO_CHAR);
    return;
  }

  if (in->value[1] >= in->value[0] && in->value[0] !=-1)
  {
    act("В $i6 уже под завязку.",ch,in,NULL,TO_CHAR);
    return;
  }

  if (in->value[0]==-1)
  {
    if (out->value[0] > 0) out->value[1] = 0;
    in->value[1]=-1;
  }
  else 
  {
    amount = UMIN(out->value[1],in->value[0] - in->value[1]);
    in->value[1] += amount;
    if (out->value[0] > 0)
    {
       out->value[1] -= amount;
       if (out->value[1]<0) out->value[1]=0;
    }
  }

  in->value[2] = out->value[2];
    
  if (vch == NULL)
  {
    do_printf(buf,"Ты переливаешь %s из $i2 в $I4.",
            liq_table[out->value[2]].liq_showname);
    act(buf,ch,out,in,TO_CHAR);
    do_printf(buf,"$c1 переливает %s из $i2 в $I8.",
            liq_table[out->value[2]].liq_showname);
        
    act(buf,ch,out,in,TO_ROOM);
  }
  else
  {
    do_printf(buf,"Ты наливаешь %s в %s $C3.",
       liq_table[out->value[2]].liq_showname,
       get_obj_desc(in,'4'));
    act(buf,ch,NULL,vch,TO_CHAR);
    do_printf(buf,"$c1 наливает тебе %s из %s.",
       liq_table[out->value[2]].liq_showname,
       get_obj_desc(out,'2'));
    act(buf,ch,NULL,vch,TO_VICT);
    do_printf(buf,"$c1 отливает %s в %s $C3.",
       liq_table[out->value[2]].liq_showname,
       get_obj_desc(in,'4'));
    act(buf,ch,NULL,vch,TO_NOTVICT);
  }
  if (out->value[1] == 0)
  {
   out->value[2] = 0;
   out->value[3] = 0;
  } 
}

void do_drink( CHAR_DATA *ch, const char *argument )
{ 
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  int64 amount;
  int64 liquid;

  one_argument( argument, arg );

  if ( ch->fighting && !IS_IMMORTAL(ch) ) 
  { 
    stc( "Пока будешь водку пьянствовать, тебе отрежут чего-то лишнее.\n\r", ch ); 
    stc( "Например голову.\n\r", ch ); 
    WAIT_STATE(ch,PULSE_VIOLENCE);
    return; 
  } 

  if ( arg[0] == '\0' )
  {
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_FOUNTAIN ) break;
    }

    if ( obj == NULL )
    {
      stc( "Что выпить?\n\r", ch );
      return;
    }
  }
  else
  {
    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
      stc( "Тут этого нет.\n\r", ch );
      return;
    }
  }

  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
  {
    stc( "Ты напился по самые уши.  *Ик*\n\r", ch );
    return;
  }

  switch ( obj->item_type )
  {
    default:
      stc( "Ты не можешь пить из этого.\n\r", ch );
      return;

    case ITEM_FOUNTAIN:
      if ( ( liquid = obj->value[2] )  < 0 )
      { 
        bug( "Do_drink: bad liquid number %d.", liquid );
        liquid = obj->value[2] = 0;
      }
      amount = liq_table[liquid].liq_affect[4] * 3;
      break;
    case ITEM_DRINK_CON: 
      if ( obj->value[1] ==0 )
      {
        stc( "Тут уже пусто.\n\r", ch );
        return;
      }

      if ( ( liquid = obj->value[2] )  < 0 )
      {
        bug( "Do_drink: bad liquid number %d.", liquid );
        liquid = obj->value[2] = 0;
      }

      amount = liq_table[liquid].liq_affect[4];
      if (obj->value[0] > 0) amount = UMIN(amount, obj->value[1]);
      break;
  }

  if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && ch->pcdata->condition[COND_FULL] > 45)
  {
    stc("Ты не можешь влить в себя ни капли.\n\r",ch);
    return;
  }

  act("$c1 пьет $T из $i2.",ch,obj,liq_table[liquid].liq_showname,TO_ROOM);
  act("Ты пьешь $T из $i2.",ch,obj,liq_table[liquid].liq_showname,TO_CHAR);
  if (obj->morph_name!=NULL)
  {
    do_printf(buf,"{y%s{x пьет из тебя.\n\r", ch->name);                                       stc(buf,obj->morph_name);
  }
  if (IS_SET(ch->act,PLR_MUSTDRINK) )
  {
    stc("Ты проглатываешь сухой комок в горле\n\r", ch);
    REM_BIT(ch->act,PLR_MUSTDRINK);
  }
  gain_condition( ch, COND_DRUNK, 
      amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
  gain_condition( ch, COND_FULL,
      amount * liq_table[liquid].liq_affect[COND_FULL] / 4 );
  gain_condition( ch, COND_THIRST,
      amount * liq_table[liquid].liq_affect[COND_THIRST] / 10 );
  gain_condition(ch, COND_HUNGER,
      amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
   stc( "Ты опьянел.\n\r", ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > 40)
   stc( "В тебя больше не влезет ни капли.\n\r", ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
   stc( "Ты больше не хочешь пить.\n\r", ch );
        
  if ( obj->value[3] != 0 )
  {
    // The drink was poisoned!
    AFFECT_DATA af;

    act( "$c1 корчится в судорогах, держась за живот.", ch, NULL, NULL, TO_ROOM );
    stc( "Ты корчишься в судорогах! Тебя отравили!\n\r", ch );
    af.where     = TO_AFFECTS;
    af.type      = gsn_poison;
    af.level     = number_fuzzy((int)amount); 
    af.duration  = 3 * (int)amount;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_POISON;
    affect_join( ch, &af );
  }
 
  if (obj->value[0] > 0)
  {
    obj->value[1] -= amount;
    if (obj->value[1]<0) obj->value[1]=0;
  }

  if (IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE)) 
  { 
    if (!str_cmp(liq_table[liquid].liq_name,"{rкровь{x")
     || !str_cmp(liq_table[liquid].liq_name,"кровь")
     || !str_cmp(liq_table[liquid].liq_name,"blood"))
    { 
      stc("Свежая {Rкpовь{x стpуится по твоим жилам!\n\r", ch); 
      ch->hit+=liq_table[liquid].liq_affect[COND_THIRST]*(ch->level/20+1)*2/3; 
    } 
  }
}
 
void do_eat( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  char buf[MAX_STRING_LENGTH];
  one_argument( argument, arg );

  if ( ch->fighting && !IS_IMMORTAL(ch)) 
  { 
    stc( "Пока ты будешь пирожки жевать, тебе отрежут чего-то лишнее.\n\r", ch ); 
    stc( "Например голову.\n\r", ch ); 
    WAIT_STATE(ch,PULSE_VIOLENCE);
    return; 
  } 

  if ( IS_SET(ch->act,PLR_MUSTDRINK) ) 
  { 
    stc( "Комок стоит у тебя поперек горла.\n\r", ch ); 
    return; 
  } 

  if ( arg[0] == '\0' ) 
  {
    stc( "Съесть что?\n\r", ch );
    return;
  }

  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL ) 
  {
    stc( "У тебя этого нет.\n\r", ch );
    return;
  }

  if ( !IS_IMMORTAL(ch) )
  {
    if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
    {
      stc( "Это несъедобно.\n\r", ch );
      return;
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > 40 )
    {   
      stc( "Ты больше не съешь ни кусочка.\n\r", ch );
      return;
    }

    if ( obj->level > ch->level )
    {
      stc( "Ты не можешь это проглотить...\n\r", ch );
      return;
    }
  }

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"eat")) return; 
 
  act( "$c1 ест $i4.",  ch, obj, NULL, TO_ROOM ); 
  act( "Ты ешь $i4.", ch, obj, NULL, TO_CHAR );
  if (obj->morph_name!=NULL) 
  { 
    do_printf(buf,"{y%s{x съел тебя... нувязочка...\n\r", ch->name);
    stc(buf,obj->morph_name);
  }

  switch ( obj->item_type ) 
  {
    case ITEM_FOOD:
      if ( !IS_NPC(ch) )
      {
        int condition;

        condition = ch->pcdata->condition[COND_HUNGER];
        gain_condition( ch, COND_FULL, obj->value[0] );
        gain_condition( ch, COND_HUNGER, obj->value[1]);
        if ( condition == 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
          stc( "Ты больше не голоден.\n\r", ch );
        else if ( ch->pcdata->condition[COND_FULL] > 40 )
          stc( "Ты больше не съешь ни кусочка.\n\r", ch );
      }
      if ( obj->value[3] != 0 )
      {
        // The food was poisoned!
        AFFECT_DATA af;

        act("$c1 корчится в судорогах, держась за живот", ch, 0, 0, TO_ROOM);
        stc("Ты корчишься в судорогах! Тебя отравили!\n\r", ch );

        af.where     = TO_AFFECTS;
        af.type      = gsn_poison;
        af.level     = number_fuzzy((int)obj->value[0]);
        af.duration  = 2 * (int)obj->value[0];
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_POISON;
        affect_join( ch, &af );
      }
      break;

    case ITEM_PILL:
      if (IS_SET(ch->in_room->room_flags,ROOM_ARENA)) 
      { 
        stc( "Мрачное местечко... таблетка выпала из твоих трясущихся рук...\n\r", ch ); 
        act( "$c1 пытается сьесть таблетку, но роняет ее. $i1 закатывается в щель.", ch, obj, NULL, TO_ROOM ); 
        extract_obj( obj ); 
        return; 
      } 
      obj_cast_spell( (int)obj->value[1], (int)obj->value[0], ch, ch, NULL ); 
      obj_cast_spell( (int)obj->value[2], (int)obj->value[0], ch, ch, NULL );
      obj_cast_spell( (int)obj->value[3], (int)obj->value[0], ch, ch, NULL );
      if (number_range(1,100) <20)
      {
        SET_BIT(ch->act,PLR_MUSTDRINK);
        stc("Ты подавился! Тебе нужно срочно глотнуть воды!\n\r",ch);
      }
      break;
  }
  extract_obj( obj );
}

// Remove an object.
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];

  if ( ( obj = get_eq_char( ch, iWear ) ) == NULL ) return TRUE;

  if ( !fReplace ) return FALSE;

  if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL(ch))
  {
    act( "Ты не можешь снять $i4.", ch, obj, NULL, TO_CHAR );
    return FALSE;
  }

  if (CAN_WEAR( obj, ITEM_WEAR_FINGER) && get_eq_char( ch, WEAR_HANDS )!= NULL)
  {
    stc("Сначала сними перчатки.\n\r",ch);
    return FALSE;
  }
  if ((CAN_WEAR( obj, ITEM_WEAR_BODY) || CAN_WEAR( obj, ITEM_WEAR_LEGS)
    || CAN_WEAR( obj, ITEM_WEAR_FEET)) &&  ch->fighting!=NULL)
  {
    stc("Ты не можешь одеть это в бою.\n\r",ch);
    return FALSE;
  }

  unequip_char( ch, obj );  act( "$c1 снимает с себя $i4.", ch, obj, NULL, TO_ROOM );
  act( "Ты снимаешь с себя $i4.", ch, obj, NULL, TO_CHAR );
  if (obj->morph_name!=NULL)
  {
    do_printf(buf,"{y%s{x снимает тебя с себя.\n\r", ch->name);
    stc(buf,obj->morph_name);
  }

  if (ch->fighting!=NULL && !CAN_WEAR( obj, ITEM_WIELD))  WAIT_STATE(ch,3*PULSE_VIOLENCE);
  if (ch->fighting!=NULL && iWear==WEAR_RHAND)  WAIT_STATE(ch,4*PULSE_VIOLENCE);
  if (ch->fighting!=NULL && iWear==WEAR_LHAND)  DAZE_STATE(ch,4*PULSE_VIOLENCE);
  return TRUE;
}

// Wear one object. * Optional replacement of existing objects.
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool w_left)
{
  char buf[MAX_STRING_LENGTH];
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];

  if ( ch->level < obj->level )
  {
    ptc( ch, "Ты должен достигнуть %d уровня чтобы использовать это.\n\r", obj->level );
    act( "$c1 вертит $i4 и так, и сяк, но опыта явно не хватает.", ch, obj, NULL, TO_ROOM );
    return;
  }

  if (IS_SET(obj->extra_flags,ITEM_CLANENCHANT)
  &&  (ch->clan==NULL || !is_exact_name(ch->clan->name,obj->name))
  && !IS_IMMORTAL(ch))
  {
    ptc(ch,"Ты не вправе носить вещи другого клана.\n\r");
    return;
  }

  if (IS_SET(race_table[ch->race].spec,SPEC_VAMPIRE) && !strcmp(material_lookup(obj->material),"silver"))
  {
    stc("Ты не смеешь надеть это.\n\r",ch);
    return;
  }

  if ( obj->item_type == ITEM_LIGHT )
  {
    if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) ) return;
    act( "$c1 зажигает $i4 и берет в руки.", ch, obj, NULL, TO_ROOM );
    act( "Ты зажигаешь $i4 и берешь в руки.",  ch, obj, NULL, TO_CHAR );
    if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
    if (obj->morph_name!=NULL)
    {
      ptc(obj->morph_name,"{y%s{x зажигает тебя и берет в руки.\n\r", ch->name);
    }
    equip_char( ch, obj, WEAR_LIGHT ); 
    return;
  }

  if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD) ||
       CAN_WEAR( obj, ITEM_WIELD ) ||
       CAN_WEAR( obj, ITEM_HOLD ))
  {
   OBJ_DATA *weapon;
   int sn,skill;

   if ( !w_left &&   get_eq_char( ch, WEAR_RHAND ) != NULL
     && get_eq_char( ch, WEAR_LHAND ) != NULL
     && !remove_obj( ch, WEAR_RHAND, fReplace ) 
     && !remove_obj( ch, WEAR_LHAND, fReplace ) )  return;
   else 
   if ( w_left &&   get_eq_char( ch, WEAR_LHAND ) != NULL 
     &&   !remove_obj( ch, WEAR_LHAND, fReplace )) return;        

   if ( get_eq_char( ch, WEAR_RHAND ) == NULL && !w_left)
   {
     weapon = get_eq_char(ch,WEAR_LHAND);
     if (weapon && CAN_WEAR(weapon, ITEM_WIELD))
     {
       if ( IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
        && (!IS_SET(race_table[ch->race].spec,SPEC_TWOHAND))
        && !IS_IMMORTAL(ch))
       {
         stc("Твои руки заняты оружием!\n\r",ch);
         return;
       }
     }
     if (weapon && CAN_WEAR(obj, ITEM_WIELD)
      && IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
      && !IS_SET(race_table[ch->race].spec,SPEC_TWOHAND))
     { 
       stc("Освободи сначала левую руку.\n\r",ch); 
       return; 
     } 
     if (CAN_WEAR (obj, ITEM_WEAR_SHIELD))
     {
     if (get_eq_char(ch,WEAR_LHAND) 
     && CAN_WEAR(get_eq_char(ch,WEAR_LHAND), ITEM_WEAR_SHIELD))
     {
       stc("Нельзя одеть два щита одновременно.\n\r",ch);
       return;
     }
     else
     {
       do_printf(buf1,"$c1 прикрывается $i5 в правой руке.");
       do_printf(buf2,"Ты прикрываешься $i5 в правой руке.");
       do_printf(buf3,"{y%s{x прикрывается, зажав тебя в правой руке.\n\r", ch->name);
     }
     }
     if (CAN_WEAR (obj, ITEM_WIELD))
     {
       if (IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)) ISORDEN(ch);
       do_printf(buf1,"$c1 берет $i4 наизготовку в правую руку.");
       do_printf(buf2,"Ты берешь $i4 наизготовку в правую руку.");
       do_printf(buf3,"{y%s{x берет тебя наизготовку в правую руку.\n\r", ch->name);
     }
     if (CAN_WEAR (obj, ITEM_HOLD))
     {
       do_printf(buf1,"$c1 зажимает $i4 в правой руке.");
       do_printf(buf2,"Ты зажимаешь $i4 в правой руке.");
       do_printf(buf3,"{y%s{x зажимает тебя в правой руке.\n\r", ch->name);
     }
     act( buf1,ch, obj, NULL, TO_ROOM );
     act( buf2,ch, obj, NULL, TO_CHAR );
     if (obj->morph_name!=NULL) stc(buf3,obj->morph_name);
     equip_char( ch, obj, WEAR_RHAND );
     if (CAN_WEAR (obj, ITEM_WIELD))
     {
       sn = get_weapon_sn(ch, TRUE);
       if (sn == gsn_hand_to_hand) return;
       skill = get_weapon_skill(ch,sn);

       if (skill >= 100) act("$i1 выглядит как часть тебя!",ch,obj,NULL,TO_CHAR);
       else if (skill > 85) act("Ты чувствуешь себя достаточно уверенно с $i5.",ch,obj,NULL,TO_CHAR);
       else if (skill > 70) act("Ты хорошо знаком с $i5.",ch,obj,NULL,TO_CHAR);
       else if (skill > 50) act("Ты неплохо знаешь $i4.",ch,obj,NULL,TO_CHAR);
       else if (skill > 25) act("Ты чувствуешь себя немного неловко с $i5 в руках.",ch,obj,NULL,TO_CHAR);
       else if (skill > 1)  act("Ты слабо знаком с $i5.",ch,obj,NULL,TO_CHAR);
       else act("Ты даже не знаешь с какого конца $i4 берут.", ch,obj,NULL,TO_CHAR);
     }
     return;
   }

   if (get_eq_char( ch, WEAR_LHAND ) == NULL)
   {
     weapon = get_eq_char(ch,WEAR_RHAND);
     if (weapon != NULL && CAN_WEAR(weapon, ITEM_WIELD)
      && IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
      && (!IS_SET(race_table[ch->race].spec,SPEC_TWOHAND))
      && !IS_IMMORTAL(ch))
     {
       stc("Твои руки заняты оружием!\n\r",ch);
       return;
     }
        
     weapon=obj;
     if (weapon != NULL && CAN_WEAR(weapon, ITEM_WIELD))
     {
       if (!check_skill(ch,gsn_dual))
       {
         stc("Ты не умеешь использовать оружие в левой руке.\n\r",ch);
         return;
       }
       if (IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
         && (!IS_SET(race_table[ch->race].spec,SPEC_TWOHAND))
         && !IS_IMMORTAL(ch))
       {
         stc("Твои руки заняты оружием!\n\r",ch);
         return;
       }
       if (IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
         && ch->size<SIZE_LARGE)
       {
         stc("Ты не можешь вооружиться этим в левой руке.\n\r",ch);
         return; 
       }
       if (!IS_NPC(ch) && ch->classwar==0 && obj->value[0] != WEAPON_DAGGER)
       {
         stc("Пока не станешь воином, носить тебе в левой руке только кинжалы...\n\r",ch);
         return;
       }
     }
     if (CAN_WEAR (obj, ITEM_WEAR_SHIELD))
     {
     if (get_eq_char(ch,WEAR_RHAND) 
     && CAN_WEAR(get_eq_char(ch,WEAR_RHAND), ITEM_WEAR_SHIELD))
     {
       stc("Нельзя одеть два щита одновременно.\n\r",ch);
       return;
     }
     else
     {
       do_printf(buf1,"$c1 прикрывается $i5 в левой руке.");
       do_printf(buf2,"Ты прикрываешься $i5 в левой руке.");
       do_printf(buf3,"{y%s{x прикрывается, зажав тебя в левой руке.\n\r", ch->name);
     }
     }
     if (CAN_WEAR (obj, ITEM_WIELD))
     {
       if (IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)) ISORDEN(ch);
       do_printf(buf1,"$c1 берет $i4 наизготовку в левую руку.");
       do_printf(buf2,"Ты берешь $i4 наизготовку в левую руку.");
       do_printf(buf3,"{y%s{x берет тебя наизготовку в левую руку.\n\r", ch->name);
     }
     if (CAN_WEAR (obj, ITEM_HOLD))
     {
       do_printf(buf1,"$c1 зажимает $i4 в левой руке.");
       do_printf(buf2,"Ты зажимаешь $i4 в левой руке.");
       do_printf(buf3,"{y%s{x зажимает тебя в левой руке.\n\r", ch->name);
     }

     act( buf1,ch, obj, NULL, TO_ROOM );
     act( buf2,ch, obj, NULL, TO_CHAR );
     if (obj->morph_name!=NULL) stc(buf3,obj->morph_name); 
     equip_char( ch, obj, WEAR_LHAND );
     if (CAN_WEAR (obj, ITEM_WIELD))
     {
       sn = get_weapon_sn(ch, FALSE);
       if (sn == gsn_hand_to_hand) return;
       skill = get_weapon_skill(ch,sn);

       if (skill >= 100) act("$i1 выглядит как часть тебя!",ch,obj,NULL,TO_CHAR);
       else if (skill > 85)  act("Ты чувствуешь себя достаточно уверенно с $i5.",ch,obj,NULL,TO_CHAR);
       else if (skill > 70)  act("Ты хорошо знаком с $i5.",ch,obj,NULL,TO_CHAR);
       else if (skill > 50)  act("Ты неплохо знаешь $i4.",ch,obj,NULL,TO_CHAR);
       else if (skill > 25)  act("Ты чувствуешь себя немного неловко с $i5 в руках.",ch,obj,NULL,TO_CHAR);
       else if (skill > 1)  act("Ты слабо знаком с $i5.",ch,obj,NULL,TO_CHAR);
       else  act("Ты даже не знаешь с какого конца $i4 берут.", ch,obj,NULL,TO_CHAR);
     }
     return;
   }
   bug( "Wear_obj: no free hands.", 0 );
   stc( "Руки полные.\n\r", ch );
   return;
 }

 if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
 {
   if ( !w_left &&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
     &&   get_eq_char( ch, WEAR_FINGER_L ) != NULL
     &&   !remove_obj( ch, WEAR_FINGER_R, fReplace )
     &&   !remove_obj( ch, WEAR_FINGER_L, fReplace ) )  return;
   else if ( w_left &&   get_eq_char( ch, WEAR_FINGER_L ) != NULL 
     &&   !remove_obj( ch, WEAR_FINGER_L, fReplace )) return; 

   if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL && !w_left)
   {
     if (get_eq_char( ch, WEAR_HANDS )!= NULL)
     {
       stc("Как ты собираешься одевать кольцо, не сняв перчаток?\n\r",ch);
       return;
     }
     act( "$c1 надевает $i4 на палец правой руки.",   ch, obj, NULL, TO_ROOM );
     act( "Ты надеваешь $i4 на палец правой руки.", ch, obj, NULL, TO_CHAR );
     if (ch->fighting!=NULL)  WAIT_STATE(ch,2*PULSE_VIOLENCE);
     if (obj->morph_name!=NULL)
     {
       ptc(obj->morph_name,"{y%s{x одевает тебя на палец правой руки.\n\r", ch->name);
     }
     equip_char( ch, obj, WEAR_FINGER_R );
     return;
   }

   if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL)
   {
     if (get_eq_char( ch, WEAR_HANDS )!= NULL)
     {
       stc("Как ты собираешься одевать кольцо, не сняв перчаток?\n\r",ch);
       return;
     }
     act( "$c1 надевает $i4 на палец левой руки.",    ch, obj, NULL, TO_ROOM );
     act( "Ты надеваешь $i4 на палец левой руки.",  ch, obj, NULL, TO_CHAR );
     if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
     if (obj->morph_name!=NULL)
     {
       ptc(obj->morph_name,"{y%s{x одевает тебя на палец левой руки.\n\r", ch->name);
       stc(buf,obj->morph_name);
     }

     equip_char( ch, obj, WEAR_FINGER_L );
     return;
   }
   bug( "Wear_obj: no free finger.", 0 );
   stc( "Тебе двух колец мало?\n\r", ch );
   return;
 }

 if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
 {
   if ( !remove_obj( ch, WEAR_NECK, fReplace ) )  return;

   act( "$c1 одевает $i4 на шею.",   ch, obj, NULL, TO_ROOM );
   act( "Ты одеваешь $i4 на шею.", ch, obj, NULL, TO_CHAR );

   if (obj->morph_name!=NULL)
   {
     ptc(obj->morph_name,"{y%s{x одевает тебя на шею.\n\r", ch->name);
   }
   equip_char( ch, obj, WEAR_NECK );
   return;
 }
 if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
 {
   if ( !remove_obj( ch, WEAR_BODY, fReplace ) )  return;

   if (ch->fighting!=NULL)
   {
     stc("Ты не можешь одеть это в бою.\n\r",ch);
     return;
   }
   act( "$c1 облачается в $i4.",   ch, obj, NULL, TO_ROOM );
   act( "Ты облачаешься в $i4.", ch, obj, NULL, TO_CHAR );

   if (obj->morph_name!=NULL)
   {
     do_printf(buf,"{y%s{x облачается в тебя.\n\r", ch->name);
     stc(buf,obj->morph_name);
   }
   equip_char( ch, obj, WEAR_BODY );
   return;
 }

 if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
 {
   if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )  return;
   act( "$c1 одевает $i4 на голову.",   ch, obj, NULL, TO_ROOM );
   act( "Ты одеваешь $i4 на голову.", ch, obj, NULL, TO_CHAR );
   if (ch->fighting!=NULL)  WAIT_STATE(ch,2*PULSE_VIOLENCE);
   if (obj->morph_name!=NULL)
   {
     do_printf(buf,"{y%s{x одевает тебя на голову.\n\r", ch->name);
     stc(buf,obj->morph_name);
   }

   equip_char( ch, obj, WEAR_HEAD );
   return;
 }

 if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
 {
   if ( !remove_obj( ch, WEAR_LEGS, fReplace ) ) return;
   if (ch->fighting!=NULL)
   {
     stc("Ты не можешь одеть это в бою.\n\r",ch);
     return;
   }
   act( "$c1 одевает на ноги $i4.",   ch, obj, NULL, TO_ROOM );
   act( "Ты одеваешь на ноги $i4.", ch, obj, NULL, TO_CHAR );

   if (obj->morph_name!=NULL)
   {
     do_printf(buf,"{y%s{x одевает тебя на ногу.\n\r", ch->name);
     stc(buf,obj->morph_name);
   }

   equip_char( ch, obj, WEAR_LEGS );
   return;
 }

 if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
 {
   if ( !remove_obj( ch, WEAR_FEET, fReplace ) )  return;
   if (ch->fighting!=NULL)
   {
     stc("Ты не можешь одеть это в бою.\n\r",ch);
     return;
   }
   act( "$c1 обувается в $i4.",   ch, obj, NULL, TO_ROOM );
   act( "Ты обуваешься в $i4.", ch, obj, NULL, TO_CHAR );

   if (obj->morph_name!=NULL)
   {
     do_printf(buf,"{y%s{x обувается в тебя.\n\r", ch->name);
     stc(buf,obj->morph_name);
   }
   equip_char( ch, obj, WEAR_FEET );
   return;
 }

 if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
 {
   if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )  return;
   act( "$c1 одевает на руки $i4.",   ch, obj, NULL, TO_ROOM );
   act( "Ты одеваешь на руки $i4.", ch, obj, NULL, TO_CHAR );
   if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
   if (obj->morph_name!=NULL)
     ptc(obj->morph_name,"{y%s{x одевает тебя на руки.\n\r", ch->name);
   equip_char( ch, obj, WEAR_HANDS );
   return;
 }

  if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
  {
    if ( !remove_obj( ch, WEAR_ARMS, fReplace ) ) return;
    act( "$c1 просовывает руки в $i4.",   ch, obj, NULL, TO_ROOM );
    act( "Ты просовываешь руки в $i4.", ch, obj, NULL, TO_CHAR );
    if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
    if (obj->morph_name!=NULL)
    {
      do_printf(buf,"{y%s{x просовывает руки в тебя.\n\r", ch->name);
      stc(buf,obj->morph_name);
    }
    equip_char( ch, obj, WEAR_ARMS );
    return;
  }

  if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT) )
  {
    if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) ) return;
    act( "$c1 одевает $i4 поверх доспехов.",   ch, obj, NULL, TO_ROOM );
    act( "Ты одеваешь $i4 поверх доспехов.", ch, obj, NULL, TO_CHAR );
    if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
    if (obj->morph_name!=NULL)
    {
      do_printf(buf,"{y%s{x одевает тебя поверх доспехов.\n\r", ch->name);
      stc(buf,obj->morph_name);
    }
    equip_char( ch, obj, WEAR_ABOUT );
    return;
  }

  if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
  {
    if ( !remove_obj( ch, WEAR_WAIST, fReplace ) ) return;
    act( "$c1 одевает $i4 вокруг пояса.",   ch, obj, NULL, TO_ROOM );
    act( "Ты одеваешь $i4 вокруг пояса.", ch, obj, NULL, TO_CHAR );
    if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
    if (obj->morph_name!=NULL)
    {
      do_printf(buf,"{y%s{x одевает тебя вокруг пояса.\n\r", ch->name);
      stc(buf,obj->morph_name);
    }
    equip_char( ch, obj, WEAR_WAIST );
    return;
  }

  if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
  {
    if ( !w_left 
      &&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
      &&   get_eq_char( ch, WEAR_WRIST_L ) != NULL
      &&   !remove_obj( ch, WEAR_WRIST_R, fReplace )
      &&   !remove_obj( ch, WEAR_WRIST_L, fReplace ) )
         return;

    else if ( w_left 
      &&   get_eq_char( ch, WEAR_WRIST_L ) != NULL 
      &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )) 
         return; 

    if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL && !w_left)
    {
      act( "$c1 одевает $i4 вокруг правого запястья.",ch, obj, NULL, TO_ROOM );
      act( "Ты одеваешь $i4 вокруг правого запястья.",ch, obj, NULL, TO_CHAR );
      if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
      if (obj->morph_name!=NULL)
      {
        do_printf(buf,"{y%s{x одевает тебя вокруг правого запястья.\n\r", ch->name);
        stc(buf,obj->morph_name);
      }
      equip_char( ch, obj, WEAR_WRIST_R );
      return;
    }

    if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL ) 
    { 
      act( "$c1 одевает $i4 вокруг левого запястья.", ch, obj, NULL, TO_ROOM ); 
      act( "Ты одеваешь $i4 вокруг левого запястья.", ch, obj, NULL, TO_CHAR ); 
      if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE); 
      if (obj->morph_name!=NULL) 
      { 
        do_printf(buf,"{y%s{x одевает тебя вокруг левого запястья.\n\r", ch->name); 
        stc(buf,obj->morph_name); 
      } 
      equip_char( ch, obj, WEAR_WRIST_L );
      return;
    }
    bug( "Wear_obj: no free wrist.", 0 );
    stc( "Да...Запястья закончились - попробуй на голени.\n\r", ch );
    return;
  }

  if ( CAN_WEAR(obj,ITEM_WEAR_FLOAT) )
  {
    if (!remove_obj(ch,WEAR_FLOAT, fReplace) ) return;
    act("$c1 отпускает $i4 в полет вокруг себя.",ch,obj,NULL,TO_ROOM);
    act("Ты отпускаешь $i4 в полет вокруг себя.",ch,obj,NULL,TO_CHAR);
    if (ch->fighting!=NULL) WAIT_STATE(ch,2*PULSE_VIOLENCE);
    if (obj->morph_name!=NULL)
    {
      do_printf(buf,"{y%s{x отпускает тебя и ты начинаешь кружиться вокруг.\n\r", ch->name);
      stc(buf,obj->morph_name);
    }
    equip_char(ch,obj,WEAR_FLOAT);
    return;
  }

  if ( fReplace ) stc( "Ты не можешь одеть, взять в руки или вооружиться этим.\n\r", ch );
}
 
void do_wear( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool w_left=FALSE; 
  OBJ_DATA *obj;

  argument = one_argument( argument, arg1 ); 
  one_argument(argument,arg2); 
  
  if ( arg1[0] == '\0' )
  {
    stc( "Одеть что?\n\r", ch );
    return;
  }

  if ( !str_cmp( arg1, "all" ) )
  {
    OBJ_DATA *obj_next;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
      obj_next = obj->next_content;
      if( obj->wear_loc != WEAR_NONE ) continue;
      if( !can_see_obj( ch, obj )) continue;
      if( obj->durability!=-1 && obj->condition < 1 )
      {
        ptc( ch,"%s слишком испорчен, чтобы ты мог%s его использовать.\n\r", get_obj_desc( obj, '1'),(ch->sex==2)?"ла":"" );
        continue;
      }
      if (obj->enchanted && str_cmp(obj->owner,"(public)") && obj->item_type == ITEM_ARMOR)
      {
        if (!obj->owner)
        {
          stc( "Эту вещь нельзя одевать.\n\r", ch );
          continue;
        }
        if(strcmp(ch->name,obj->owner))
        {
          stc( "Это не твоя вещь.\n\r", ch );
          continue;
        }
      }
      wear_obj( ch, obj, FALSE, w_left);
    }
    return;
  }
  else
  {
    if (!str_cmp(arg2,"left")) w_left=TRUE; 
    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
      stc( "У тебя нет этой вещи.\n\r", ch );
      return;
    }
    if( obj->durability!=-1 && obj->condition <= 0 )
    {
      ptc( ch,"%s слишком испорчен, чтобы ты мог%s его использовать.\n\r", get_obj_desc( obj, '1'),(ch->sex==2)?"ла":"" );
      return;
    }
    if (obj->enchanted && str_cmp(obj->owner,"(public)") && obj->item_type == ITEM_ARMOR)
    {
      if (!obj->owner)
      {
        stc( "Эту вещь нельзя одевать.\n\r", ch );
        return;
      }
      if(strcmp(ch->name,obj->owner))
      {
        stc( "Это не твоя вещь.\n\r", ch );
        return;
      }
    }
    wear_obj( ch, obj, TRUE, w_left);
  }
}

void do_remove( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    stc( "Снять что?\n\r", ch );
    return;
  }
  if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
  {
    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
      stc( "У тебя нет этой вещи.\n\r", ch );
      return;
    }
    remove_obj( ch, obj->wear_loc, TRUE );
    return;
  }
  else
  {
    obj=get_eq_char( ch, WEAR_HANDS );
    if (( obj != NULL) &&
          can_see_obj( ch, obj ) &&
          obj->wear_loc != WEAR_NONE)
        remove_obj (ch, obj->wear_loc, TRUE);

    for (obj = ch->carrying; obj; obj = obj_next)
    {
      obj_next = obj->next_content;
      if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
        &&   can_see_obj( ch, obj )
        &&   obj->wear_loc != WEAR_NONE) remove_obj (ch, obj->wear_loc, TRUE);
    }
  }
}

void do_sacrifice( CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  bool found, isOne=TRUE;
  char objname[50];
  OBJ_DATA *tobj;
  int position = 0, count = 0;
  int same = 1; 

  if (ch->in_room->contents == NULL)
  {
    stc( "Ты не можешь это найти.\n\r", ch );
    return;
  }

  one_argument( argument, arg );

  if ( (arg[0] == '\0') || (!str_cmp( arg, ch->name )) )
  {
    act( "$c1 предлагает себя богам, но они отказываются.",ch, NULL, NULL, TO_ROOM );
    stc( "После смерти...\n\r", ch );
    return;
  }

  memset(objname,0,50);

  if (!str_cmp(arg, "all")) strcpy(objname, "all");
  else if (!str_prefix("all.", arg)) strncpy(objname, &arg[4], 49);
  else position = number_argument(arg, objname);

  objname[49] = '\0';

  found = 0;

  for (obj = ch->in_room->contents; obj != NULL; obj = tobj)
  {
    tobj = obj->next_content;
 
    if (can_see_obj(ch, obj) && ((is_name(objname, obj->name)
     && (!position || ++count == position)) || !str_cmp(objname, "all")))
    {
      found++; 
      if (isOne) same=1; 
      if (is_same_obj(obj,tobj)) { same++; isOne=FALSE;} 
      else isOne=TRUE; 
      do_sacrifice1(ch, obj,isOne,same);
      if (position && count == position) break;
    }
  }

  if (!found) stc( "Ты не можешь найти это.\n\r", ch );    
}

static void do_sacrifice1 (CHAR_DATA *ch, OBJ_DATA *obj, bool isOne, int count )
{
  CHAR_DATA *gch;
  int members;
  char buffer[100];
  int64 silver=0;
  char buf [MAX_INPUT_LENGTH];
  OBJ_DATA *obj1;
  OBJ_DATA *obj_next;

  if ( obj->item_type == ITEM_CORPSE_PC )
  {
    if (obj->contains)
    {
      stc("Боги не примут этого.\n\r",ch);
      return;
    }
        else 
        {
         ptc(ch,"Боги дают тебе 0 серебрянных монет за твое жертвоприношение.\n\r", silver);
     extract_obj(obj);
         return;
        }
  }

  if (obj->morph_name !=NULL)
  {
    act("$c1 {rприносит{w $C4 {rв жертву Богам.{x", ch, NULL, obj->morph_name, TO_ROOM );
    act("Ты {rприносишь{w $C4 {rв жертву Богам.{x", ch, NULL, obj->morph_name, TO_CHAR );
    ptc(ch,"Боги дают тебе %u золотых монет за твое жертвоприношение.\n\r", silver);
    char_from_room( obj->morph_name );
    char_to_room( obj->morph_name, obj->in_room );
    obj->morph_name->morph_obj->is_morphed=FALSE;
    obj->morph_name->morph_obj = NULL;
    stc("{rТебя принесли в жертву...{x\n\r",obj->morph_name);
    stc("Ты {rУБИТ!{x...{x\n\r",obj->morph_name);
    silver=obj->morph_name->level*2;
    raw_kill(obj->morph_name);
    do_printf(buf,"В темном уголке в %s, {Y%s (lvl.%d){x принес в жертву {Y%s (lvl.%d){x",ch->in_room->area->name,ch->name,ch->level,obj->morph_name->name,obj->morph_name->level);
    send_news(buf,NEWS_DEATH);
    do_ear(obj->morph_name,ch);
    ch->silver=ch->silver+silver;
    obj->morph_name=NULL;
    extract_obj( obj );
    return;
  }

  if ( !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC) )
  {
    act("$i1 не совсем подходит для жертвоприношения.",ch,obj,0,TO_CHAR );
    return;
  }

  if (obj->in_room != NULL)
  {
    for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
      if (gch->on == obj)
      {
        act("$C1, похоже, использует $i4.",ch,obj,gch,TO_CHAR);
        return;
      }
    }
  }

  if (isOne) 
  { 
   silver = UMAX(1,obj->level * 3)*count;
   if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
    silver = UMIN(silver,obj->cost * count);

   if (silver == 1) stc("Боги дают тебе одну серебрянную монету за твое жертвоприношение.\n\r", ch);
   else ptc(ch,"Боги дают тебе %u серебрянных монет за твое жертвоприношение.\n\r", silver);

   ch->silver = ch->silver+silver;

   if (IS_CFG(ch,CFG_AUTOSPLIT))
   { 
     members = 0;
     for (gch=ch->in_room->people;gch!=NULL;gch=gch->next_in_room)
       if ( is_same_group( gch, ch ) ) members++;
   
     if ( members > 1 && silver > 1)
     {
       do_printf(buffer,"%u",silver);
       do_function(ch, &do_split, buffer); 
     }
   }
 
   if (count==1) 
   { 
    act( "$c1 {wприносит{x $i4 {wв жертву Богам.{x", ch, obj, NULL, TO_ROOM );
    wiznet("$C1 {wприносит{x $i4 в жертву Богам.", ch,obj,WIZ_SACCING,0); 
   } 
   else 
   { 
    act( "$c1 {wприносит несколько{x $i8 {wв жертву Богам.{x", ch, obj, NULL, TO_ROOM );
    wiznet("$C1 {wприносит несколько{x $i8 в жертву Богам.", ch,obj,WIZ_SACCING,0); 
   } 
  }

  if ((obj->item_type == ITEM_CORPSE_NPC) && (obj->contains))
  {
    act("Несколько вещей рассыпаются вокруг.", ch, NULL, NULL, TO_ROOM );
    act("Несколько вещей рассыпаются вокруг.", ch, NULL, NULL, TO_CHAR );
    for ( obj1 = obj->contains; obj1; obj1 = obj_next )
    {
      obj_next = obj1->next_content;
      obj_from_obj( obj1 );
      obj_to_room( obj1, ch->in_room );
    }
    extract_obj( obj );
  }
  else
    extract_obj( obj );
}

void do_quaff( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    stc( "Осушить что?\n\r", ch );
    return;
  }

  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
  {
    stc( "У тебя нет этого эликсира.\n\r", ch );
    return;
  }

  if ( obj->item_type != ITEM_POTION )
  {
    stc( "Залпом можно пить только эликсиры.\n\r", ch );
    return;
  }

  if (ch->level < obj->level)
  {
    stc("Мал еще.Выпьешь-козленочком станешь.\n\r",ch);
    return;
  }

  if (IS_SET(ch->in_room->room_flags,ROOM_ARENA)) 
  { 
   stc( "Мрачное местечко... от страха ты проливаешь зелье на землю..\n\r", ch ); 
   act( "$c1 пытается выпить $i4, но проливает зелье на землю.", ch, obj, NULL, TO_ROOM ); 
   extract_obj( obj ); 
   return; 
  } 
 
  if (IS_SET(ch->act,PLR_TIPSY)) // tipsy by Dinger 
   if (tipsy(ch,"quaff")) return; 
 
  act( "$c1 одним глотком выпивает $i4.", ch, obj, NULL, TO_ROOM );
  act( "Ты одним глотком выпиваешь $i4.", ch, obj, NULL ,TO_CHAR );
  if (obj->morph_name!=NULL)
  {
    ptc(obj->morph_name,"{y%s{x выпивает тебя одним глотком... неувязочка...\n\r", ch->name);
  }

  obj_cast_spell( (int)obj->value[1], (int)obj->value[0], ch, ch, NULL );
  obj_cast_spell( (int)obj->value[2], (int)obj->value[0], ch, ch, NULL );
  obj_cast_spell( (int)obj->value[3], (int)obj->value[0], ch, ch, NULL );

  extract_obj( obj );
  WAIT_STATE(ch,PULSE_VIOLENCE); 
}

void do_recite( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *scroll;
  OBJ_DATA *obj;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( ( scroll = get_obj_carry( ch, arg1, ch ) ) == NULL )
  {
    stc( "У тебя нет этого свитка.\n\r", ch );
    return;
  }

  if (scroll->item_type==ITEM_BONUS && !IS_NPC(ch))
  {
    // item bonus for add reserved QUENIA words
    // value0 = 1 (SCROLL_QUENIA)
    // value1 = type of word
    // value2 = quantity of use
    if (scroll->value[0]==SCROLL_QUENIA)
    { 
      if (scroll->value[1] >= MAX_QUENIA) 
      {
        stc("Ты не можешь понять что это такое...Похоже боги перемудрили.\n\r",ch);
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        return;
      }
      ch->pcdata->quenia[scroll->value[1]]+=scroll->value[2];
      ptc(ch,"Прочев древний текст, ты осознаешь {CСИЛУ{x, заключенную в слове %s.\n\r",quenia_table[scroll->value[1]].descr);
      act( "$c1 прочитывает $i4.", ch, scroll, NULL, TO_ROOM );
      extract_obj( scroll );
      return;
    }
    // item bonus for add clan skill
    // value0 = 2 (SCROLL_CLANSKILL)
    // value1 = sn of skill in skill_table (buggy?)
    // value2 = ticks of using (-1 - forever)
    if (scroll->value[0]==SCROLL_CLANSKILL)
    {
      int sn=(int)scroll->value[1];
      if (!ch->clan)
      {
        stc("Сперва вступи в клан.\n\r",ch);
        return;
      }
      if (sn<1 || sn > MAX_SKILL)
      {
        stc("Ты не можешь понять что это такое...Похоже боги перемудрили.\n\r",ch);
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        return;
      }
      if (add_clanskill(ch->clan,sn,scroll->value[2]))
        ptc(ch,"Ты добавляешь навык {G%s{x для клана %s на %u тиков.\n\r",skill_table[sn].name,ch->clan->name,scroll->value[2]);
      else stc("В клане нет места для новых навыков.\n\r",ch);
      act( "$c1 прочитывает $i4 вслух.", ch, scroll, NULL, TO_ROOM );
      extract_obj(scroll);
      return;
    }
    stc("Ты не можешь понять что это такое...\n\r",ch);
    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
    return;
  }

  if ( scroll->item_type != ITEM_SCROLL)
  {
    stc( "Ты можешь читать только свитки.\n\r", ch );
    return;
  }

  if ( ch->level + 2*category_bonus(ch,LEARN) < scroll->level)
  {
    stc("Этот свиток выше твоего понимания.\n\r",ch);
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) // tipsy by Dinger 
   if (tipsy(ch,"recite")) return; 
 
  obj = NULL;
  if ( arg2[0] == '\0' )
  {
    victim = ch;
  }
  else
  {
    if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
      &&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
    {
      stc( "Ты не можешь найти этого.\n\r", ch );
      return;
    }
  }

  act( "$c1 прочитывает $i4 вслух.", ch, scroll, NULL, TO_ROOM );
  act( "Ты прочитываешь $i4 вслух.", ch, scroll, NULL, TO_CHAR );
  if (scroll->morph_name!=NULL)
    ptc(scroll->morph_name,"{y%s{x прочитывает тебя вслух... неувязочка...\n\r", ch->name);

  if (number_percent() >= 20 + get_skill(ch,gsn_scrolls) * 4/5)
  {
    stc("Дефекты дикции тебя подвели...\n\r",ch);
    check_improve(ch,gsn_scrolls,FALSE,2);
  }
  else
  {
    obj_cast_spell( (int)scroll->value[1],(int) scroll->value[0], ch, victim, obj );
    obj_cast_spell( (int)scroll->value[2], (int)scroll->value[0], ch, victim, obj );
    obj_cast_spell( (int)scroll->value[3], (int)scroll->value[0], ch, victim, obj );
    check_improve(ch,gsn_scrolls,TRUE,2);
  }

  if (ch->level < scroll->value[0]) WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
  else WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
  extract_obj( scroll );
  return;
}

void do_brandish( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  OBJ_DATA *staff;
  char buf[MAX_STRING_LENGTH];
  int sn;

  if ( ( staff = get_eq_char( ch, WEAR_LHAND ) ) == NULL )
  {
    stc( "У тебя в руках ничего нет.\n\r", ch );
    return;
  }

  if ( staff->item_type != ITEM_STAFF )
  {
    stc( "Размахивать ты можешь только посохом.\n\r", ch );
    return;
  }

  if ( ( sn = (int)staff->value[3] ) < 0
  ||   sn >= max_skill
  ||   skill_table[sn].spell_fun == 0 )
  {
    bug( "Do_brandish: bad sn %d.", sn );
    return;
  }

  WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

  if ( staff->value[2] > 0 )
  {
    act( "$c1 взмахивает $i5.", ch, staff, NULL, TO_ROOM );
    act( "Ты взмахиваешь $i5.",  ch, staff, NULL, TO_CHAR );
    if (staff->morph_name!=NULL)
    {
      do_printf(buf,"{y%s{x взмахивает тобой.\n\r", ch->name);
      stc(buf,staff->morph_name);
    }

    if ( ch->level+2*category_bonus(ch,MAKE) < staff->level 
     || number_percent() >= 20 + get_skill(ch,gsn_staves) * 4/5)
    {
      act ("Силы $i2 отказались служить тебе.",ch,staff,NULL,TO_CHAR);
      act ("...И ничего не произошло.",ch,NULL,NULL,TO_ROOM);
      check_improve(ch,gsn_staves,FALSE,2);
    }
      
    else for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
      vch_next    = vch->next_in_room;

      switch ( skill_table[sn].target )
      {
        default:
          bug( "Do_brandish: bad target for sn %d.", sn );
          return;
        case TAR_IGNORE:
          if ( vch != ch ) continue;
          break;
        case TAR_CHAR_OFFENSIVE:
          if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) ) continue;
          break;
        case TAR_CHAR_DEFENSIVE:
          if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) ) continue;
          break;
        case TAR_CHAR_SELF:
          if ( vch != ch ) continue;
          break;
      }
      obj_cast_spell( (int)staff->value[3], (int)staff->value[0], ch, vch, NULL );
      check_improve(ch,gsn_staves,TRUE,2);
    }
  }

  if ( --staff->value[2] <= 0 )
  {
    act( "$c1 отдергивает руки, $i1 вспыхивает и исчезает.", ch, staff, NULL, TO_ROOM );
    act( "В твоих руках $i1 вспыхивает и исчезает.", ch, staff, NULL, TO_CHAR );
    if (staff->morph_name!=NULL)
    {
      do_printf(buf,"Уупс... ты вспыхиваешь и исчезаешь...\n\r");
      stc(buf,staff->morph_name);
    }
    extract_obj( staff );
  }
}

void do_zap( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *wand;
  OBJ_DATA *obj;

  one_argument( argument, arg );
  if ( arg[0] == '\0' && ch->fighting == NULL )
  {
    stc( "Взмахнуть чем?\n\r", ch );
    return;
  }

 //   if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL ) //-old version
  if ((wand = get_eq_char(ch, WEAR_LHAND)) == NULL) 
  {
    stc( "У тебя в руке ничего нет.\n\r", ch );
    return;
  }
  
  if ( wand->item_type != ITEM_WAND )
  {
    stc( "Взмахнуть можно только жезлом.\n\r", ch );
    return;
  }

  obj = NULL;
  if ( arg[0] == '\0' )
  {
    if ( ch->fighting != NULL ) victim = ch->fighting;
    else
    {
      stc( "Взмахнуть чем?\n\r", ch );
      return;
    }
  }
  else
  {
    if ( ( victim = get_char_room ( ch, arg ) ) == NULL
      &&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
    {
      stc( "Тут этого нет. it.\n\r", ch );
      return;
    }
  }

  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"zap")) return;// tipsy by Dinger
 
  WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

  if ( wand->value[2] > 0 )
  {
    if ( victim != NULL )
    {
      act( "$c1 стукнул $C4 по голове $i5.", ch, wand, victim, TO_NOTVICT );
      act( "Ты стукнул $C4 по голове $i5.", ch, wand, victim, TO_CHAR );
      act( "$c1 трахнул тебя по голове $i5.",ch, wand, victim, TO_VICT );
    }
    else
    {
      act( "$c1 стукнул $I4 $i5.", ch, wand, obj, TO_ROOM );
      act( "Ты стукнул $I4 $i5.", ch, wand, obj, TO_CHAR );
    }

    if (ch->level+2*category_bonus(ch,MAKE) < wand->level 
      ||  number_percent() >= 20 + get_skill(ch,gsn_wands) * 4/5) 
    {
      act( "Ты добился от $i2 только дыма и искр.", ch,wand,NULL,TO_CHAR);
      act( "Попытки $c2 использовать $i4 окончились дымом и искрами.", ch,wand,NULL,TO_ROOM);
      check_improve(ch,gsn_wands,FALSE,2);
    }
    else
    {
      obj_cast_spell( (int)wand->value[3], (int)wand->value[0], ch, victim, obj );
      check_improve(ch,gsn_wands,TRUE,2);
    }
  }
  if ( --wand->value[2] <= 0 )
  {
    act( "$c1 бросает $i4 на землю, разбивая на осколки.", ch, wand, NULL, TO_ROOM );
    act( "Ты бросаешь $i4 на землю, разбивая на осколки.", ch, wand, NULL, TO_CHAR );
    if (wand->morph_name!=NULL)
    {
      ptc(wand->morph_name,"{y%s{x бросает тебя на землю, разбивая на осколки.\n\r", ch->name);
    }
    extract_obj( wand );
  }
}

void do_steal( CHAR_DATA *ch, const char *argument )
{
  char buf  [MAX_STRING_LENGTH];char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int percent;

//  if (IS_NPC(ch))
//  {
//    cant_mes(ch);
//    return;
//  }

  if (!can_attack(ch,1)) return; 

  ISORDEN(ch)

  argument = one_argument( argument, arg1 );argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    stc( "Украсть что у кого?\n\r", ch );
    return;
  }

  if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
  {
    stc( "Тут таких нет.\n\r", ch );
    return;
  }

  if ( victim == ch || (IS_NPC(victim) && IS_SET(victim->act,ACT_NOSTEAL)))
  {
    stc( "Неподходящая цель.\n\r", ch );
    return;
  }

  if (is_safe(ch,victim)) return;

  if ( victim->position == POS_FIGHTING)
  {
    stc("Беги лучше, а то и тебе попадет.\n\r",ch);
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) 
   if (tipsy(ch,"steal")) return; 
 
  WAIT_STATE( ch, skill_table[gsn_steal].beats );
  percent  = number_percent();

  if (!IS_AWAKE(victim)) percent -= 100;
  else if (!can_see(victim,ch,CHECK_LVL)) percent -= 15;
  else percent += 30;
  if ( ch->race == skill_lookup("hobbit") ) percent-=15;
  if(victim->pIndexData!=NULL && victim->pIndexData->pShop!=NULL) percent+=30;
  percent-=3*(get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim,STAT_DEX));
  percent-=5*category_bonus(ch,PERCEP);
//  percent+=get_obj_carry( victim, arg1, ch )->weight/10;

  if ( ((ch->level + 15 + category_bonus(ch,PERCEP) < victim->level) && !IS_NPC(victim) && !IS_NPC(ch))
   || ( !IS_NPC(ch) && percent > get_skill(ch,gsn_steal))
   || (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)))
  {
    /* Failure. */
    add_pkiller(victim,ch);
    stc( "Опа, менты....\n\r", ch );
    affect_strip(ch,gsn_sneak);
    REM_BIT(ch->affected_by,AFF_SNEAK);

    act( "$c1 пытается обокрасть тебя.\n\r", ch, NULL, victim, TO_VICT    );
    act( "$c1 пытается обокрасть $C4.\n\r",  ch, NULL, victim, TO_NOTVICT );
    switch(number_range(0,3))
    {
      case 0 :
        do_printf( buf, "%s грязный вор!", ch->name );
        break;
      case 1 :
        do_printf( buf, "%s ворюга! Хватай %s !",ch->name,(ch->sex == 2) ? "ее" : "его");
        break;
      case 2 :
        do_printf( buf,"%s пытается ограбить меня!",ch->name );
        break;
      case 3 :
        do_printf(buf,"%s! Убери от меня свои руки, ворюга!",ch->name);
        break;
    }
    if (!IS_AWAKE(victim)) do_wake(victim,"");
    SET_BIT(victim->talk,CBIT_YELL);
    if (IS_AWAKE(victim)) do_yell( victim, buf );
    if ( !IS_NPC(ch) )
    {
      if ( IS_NPC(victim) )
      {
        check_improve(ch,gsn_steal,FALSE,2);
        multi_hit( victim, ch);
        if(victim->pIndexData!=NULL && victim->pIndexData->pShop!=NULL) add_stealer(victim,ch);
      }
      else
      {
        do_printf(buf,"$C1 пытается обокрасть %s.",get_char_desc(victim,'4'));
        wiznet(buf,ch,NULL,WIZ_FLAGS,0);
        check_criminal(ch,victim,80);
      }
    }
    return;
  }

  if ( !str_cmp( arg1, "coin" ) || !str_cmp( arg1, "coins" )
   ||  !str_cmp( arg1, "gold" ) || !str_cmp( arg1, "silver"))
  { 
    int64 gold, silver;
    gold = victim->gold * number_range(1, ch->level) / 60;
    silver = victim->silver * number_range(1,ch->level) / 60;
    if ( gold <= 0 && silver <= 0)
    {
      stc( "Тебе не удасться украсть ни одной монеты...Их там нет.\n\r", ch );
      return;
    }

    while (gold>ch->level*5) gold=(int)gold/2;
    if (gold > victim->gold) gold=victim->gold;
    while (silver>ch->level*50) silver=(int)silver/2;
    if (silver > victim->silver) silver=victim->silver;
    ch->gold        += gold;
    ch->silver      += silver;
    victim->silver  -= silver;
    victim->gold    -= gold;
    if (silver <= 0) ptc(ch, "Повезло! %u золотых в кармане.\n\r", gold );
    else if (gold <= 0) ptc(ch,"Неплохо! Спер %u серебрянных монет.\n\r",silver);
    else ptc(ch,"Ты крут!  За раз %u серебрянных и %u золотых монет!\n\r", silver,gold);
    check_improve(ch,gsn_steal,TRUE,2);
    return;
  }

  if ( ( obj = get_obj_carry( victim, arg1, ch ) ) == NULL )
  {
    stc( "Ты не нашел этого.\n\r", ch );
    return;
  }
        
  if ( !can_drop_obj( ch, obj ) || obj->level > ch->level )
  {
    stc( "Ты не можешь стащить это.\n\r", ch );
    return;
  }

  if (!IS_EAR(obj))
  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
  {
    stc( "У тебя руки заняты.\n\r", ch );
    return;
  }

  if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    stc( "Ты не можешь нести такую тяжесть.\n\r", ch );
    return;
  }

  if (!IS_SET(obj->extra_flags, ITEM_INVENTORY)) obj_from_char(obj);
  else
  {
    if((obj = create_object( obj->pIndexData, obj->level ))==NULL)
    {
     stc("{RBUG! Unable to create object! Report to Imms NOW!{x\n\r",ch);
     return;
    }
    if (obj->timer > 0 && !IS_OBJ_STAT(obj,ITEM_HAD_TIMER)) obj->timer = 0;
    REM_BIT(obj->extra_flags, ITEM_INVENTORY);
    REM_BIT(obj->extra_flags,ITEM_HAD_TIMER);
  }

  obj_to_char( obj, ch );
  act("Ты аккуратно засовываешь руку, подцепляя $i4.",ch,obj,NULL,TO_CHAR);
  check_criminal(ch,victim,5);
  check_improve(ch,gsn_steal,TRUE,2);
  stc( "Есть!\n\r", ch );
}

 

/* Shopping commands. */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
  /*char buf[MAX_STRING_LENGTH];*/
  CHAR_DATA *keeper;
  SHOP_DATA *pShop;

  pShop = NULL;
  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL ) break;
  }

  if ( pShop == NULL )
  {
    stc( "Ты не можешь сделать этого здесь.\n\r", ch );
    return NULL;
  }
  /* Shop hours. */
  if ( time_info.hour < pShop->open_hour )
  {
    do_say( keeper, "Извините, тут закрыто. Зайдите позже." );
    return NULL;
  }
  
  if ( time_info.hour > pShop->close_hour )
  {
    do_say( keeper, "Извините, тут закрыто. Зайдите завтра." );
    return NULL;
  }

  // Invisible or hidden people.
  if ( !can_see( keeper, ch,NOCHECK_LVL ) && ch->race!=RACE_SPRITE && ch->race!=5)
  {
    do_say( keeper, "С невидимками я не торгую." );
    return NULL;
  }
  return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
  OBJ_DATA *t_obj, *t_obj_next;

  /* see if any duplicates are found */
  for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
  {
    t_obj_next = t_obj->next_content;

    if (obj->pIndexData == t_obj->pIndexData 
      &&  !str_cmp(obj->short_descr,t_obj->short_descr))
    {
      /* if this is an unlimited item, destroy the new one */
      if (IS_OBJ_STAT(t_obj,ITEM_INVENTORY))
      {
        extract_obj(obj);
        return;
      }
      obj->cost = t_obj->cost; /* keep it standard */
      break;
    }
  }

  if (t_obj == NULL)
  {
    obj->next_content = ch->carrying;
    ch->carrying = obj;
  }
  else
  {
    obj->next_content = t_obj->next_content;
    t_obj->next_content = obj;
  }

  obj->carried_by      = ch;
  obj->in_room         = NULL;
  obj->in_obj          = NULL;
  ch->carry_number    += get_obj_number( obj );
  ch->carry_weight    += get_obj_weight( obj );
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int number;
  int count;
 
  number = number_argument( (char*)argument, arg );
  count  = 0;
  for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
  {
    if (obj->wear_loc == WEAR_NONE
     &&  can_see_obj( keeper, obj )
     &&  can_see_obj(ch,obj)
     &&  is_name( arg, obj->name ) )
    {
    if ( ++count == number )
    {
//     if (!obj->owner) obj->owner = str_dup("(public)");
     return obj;
    }  

    /* skip other objects of the same name */
    while (obj->next_content != NULL
      && obj->pIndexData == obj->next_content->pIndexData
      && !str_cmp(obj->short_descr,obj->next_content->short_descr))
        obj = obj->next_content;
    }
  }
  return NULL;
}

int64 get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
  SHOP_DATA *pShop;
  int64 cost;

  if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL ) return 0;

  if ( fBuy )
  {
    cost = obj->cost * pShop->profit_buy  / 100;
  }
  else
  {
    OBJ_DATA *obj2;
    int itype;

    cost = 0;
    for ( itype = 0; itype < MAX_TRADE; itype++ )
    {
      if ( obj->item_type == pShop->buy_type[itype] )
      {
        cost = obj->cost * pShop->profit_sell / 100;
        break;
      }
    }

    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
      for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
      {
        if ( obj->pIndexData == obj2->pIndexData
          &&   !str_cmp(obj->short_descr,obj2->short_descr) )
        {
          if (IS_OBJ_STAT(obj2,ITEM_INVENTORY))  cost /= 2;
          else  cost = cost * 3 / 4;
        }
       }
  } 
  if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
  {
    if (obj->value[1] == 0) cost /= 4;
    else cost = cost * obj->value[2] / obj->value[1];
  }
  return cost;
}

void do_buy( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char temp[MAX_STRING_LENGTH];
  int64 cost; 
  int roll;

  if ( argument[0] == '\0' )
  {
      stc( "Что купить?\n\r", ch );
      return;
  }

  if (IS_SET(ch->act,PLR_TIPSY)) 
   if (tipsy(ch,"buy")) return; // tipsy by Dinger 


  if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
  {
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *pet;
    ROOM_INDEX_DATA *pRoomIndexNext;
    ROOM_INDEX_DATA *in_room;

    if ( IS_NPC(ch) ) return;

    argument = one_argument(argument,arg);

    /* hack to make new thalos pets work */
    if (ch->in_room->vnum == 9621)
        pRoomIndexNext = get_room_index(9706);
    else
        pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
    if ( pRoomIndexNext == NULL )
    {
        bug( "Do_buy: bad pet shop at vnum %u.", ch->in_room->vnum );
        stc( "Извините, но вы не можете тут это купить.\n\r", ch );
        return;
    }

    in_room     = ch->in_room;
    ch->in_room = pRoomIndexNext;
    pet         = get_char_room( ch, arg );
    ch->in_room = in_room;
    if ( pet == NULL || !IS_NPC(pet) || !IS_SET(pet->act, ACT_PET) )
    {
        stc( "Извините, но вы не можете тут это купить.\n\r", ch );
        return;
    }

    if ( ch->pet != NULL )
    {
        stc("У вас уже есть животное.\n\r",ch);
        return;
    }

    cost = 10 * pet->level * pet->level;

    if ( (ch->silver + 100 * ch->gold) < cost )
    {
        stc( "У тебя не хватает денег.\n\r", ch );
        return;
    }

    if ( ch->level < pet->level )
    {
        stc( "Эта зверушка съест тебя на обед.\n\r", ch );
        return;
    }

    /* haggle */
    roll = number_percent();
    if (roll < get_skill(ch,gsn_haggle))
    {
      cost -= cost / 2 * roll / 100;
      ptc(ch,"Ты сбил цену на %u монет.\n\r",cost);
      check_improve(ch,gsn_haggle,TRUE,4);
    }

    deduct_cost(ch,cost);
    pet = create_mobile( pet->pIndexData );
    if (!pet)
    {
      stc ("{/{RBUG! unable to create mob! Report to imms!{x\n\r",ch);
      return;
    }
  
    SET_BIT(pet->act, ACT_PET);
    SET_BIT(pet->affected_by, AFF_CHARM);
    pet->clan = ch->clan;

    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    {
        do_printf( buf, "%s %s", pet->name, arg );
        free_string( pet->name );
        pet->name = str_dup( buf );
    }

    do_printf( buf, "%s на ошейнике написано 'Собственность %s. Кусается'.\n\r",
        pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );

    char_to_room( pet, ch->in_room );
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;
    stc( "Получи зверька!\n\r", ch );
    act( "$c1 покупает $C4.", ch, NULL, pet, TO_ROOM );
    return;
  }
  else
  {
    CHAR_DATA *keeper;
    OBJ_DATA *obj,*t_obj;
    char arg[MAX_INPUT_LENGTH];
    int number, count = 1;

    if ( ( keeper = find_keeper( ch ) ) == NULL )
        return;

    number = mult_argument((char *)argument,arg);
    obj  = get_obj_keeper( ch,keeper, arg );
    cost = get_cost( keeper, obj, TRUE );

    if (number < 1 || number >100)
    {
        act("$c1 говорит тебе 'А не многовато ли?'",keeper,NULL,ch,TO_VICT);
        return;
    }

    if ( cost <= 0 || !can_see_obj( ch, obj ) )
    {
      act( "$c1 говорит тебе 'Я не продаю этого - посмотри 'list''.",
          keeper, NULL, ch, TO_VICT );
      ch->reply = keeper;
      return;
    }

    if (!IS_OBJ_STAT(obj,ITEM_INVENTORY))
    {
      for (t_obj = obj->next_content; count < number && t_obj != NULL; 
             t_obj = t_obj->next_content) 
      {
        if (t_obj->pIndexData == obj->pIndexData
          &&  !str_cmp(t_obj->short_descr,obj->short_descr)) count++;
        else  break;
      }

      if (count < number)
      {
        act("$c1 говорит тебе 'У меня нет такого количества.'", keeper,NULL,ch,TO_VICT);
        ch->reply = keeper;
        return;
      }
    }

    if ( (ch->silver + ch->gold * 100) < cost * number )
    {
      if (number > 1) act("$c1 говорит тебе 'У тебя на хватит денег на столько'.",
                  keeper,obj,ch,TO_VICT);
      else act( "$c1 говорит тебе 'У тебя не хватит денег на $i4'.",
                  keeper, obj, ch, TO_VICT );
      ch->reply = keeper;
      return;
    }
      
    if ( obj->level > ch->level )
    {
      act( "$c1 говорит тебе 'Ты еще мал, чтобы использовать $i4'.",
              keeper, obj, ch, TO_VICT );
      ch->reply = keeper;
      return;
    }

    if ( obj->owner && obj->enchanted && obj->item_type==ITEM_ARMOR && strcmp(obj->owner,ch->name) &&  !IS_IMMORTAL(ch))  
     {
        stc("Ты не можешь купить чужую вещь.\n\r",ch);  
        return;
     }

    if (ch->carry_number +  number * get_obj_number(obj) > can_carry_n(ch))
    {
      stc( "Ты не можешь нести так много вещей.\n\r", ch );
      return;
    }

    if ( ch->carry_weight + number * get_obj_weight(obj) > can_carry_w(ch))
    {
      stc( "Ты не можешь нести такую тяжесть.\n\r", ch );
      return;
    }

    /* haggle */
    roll = number_percent();
    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT)
      && roll < get_skill(ch,gsn_haggle)
      && cost>500)
    {
      cost -= obj->cost / 7 * roll / 100;
      act("Ты выбиваешь скидку у $C2.",ch,NULL,keeper,TO_CHAR);
      check_improve(ch,gsn_haggle,TRUE,4);
    }

    if (number > 1)
    {
      do_printf(temp,"{C%d{x %s",number, get_obj_desc(obj,num_char(number)));
      act("$c1 покупает $t",ch,temp,NULL,TO_ROOM);
      do_printf(buf,"Ты покупаешь $t за %u серебрянных монет.",cost * number);
      act(buf,ch,temp,NULL,TO_CHAR);
    }
    else
    {
      act( "$c1 покупает $i4.", ch, obj, NULL, TO_ROOM );
      do_printf(buf,"Ты покупаешь $i4 за %u серебянных монет.",cost);
      act( buf, ch, obj, NULL, TO_CHAR );
    }
    deduct_cost(ch,cost * number);
    keeper->gold += cost * number/100;
    keeper->silver += cost * number - (cost * number/100) * 100;

    for (count = 0; count < number; count++)
    {
      if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
      {        if((t_obj = create_object( obj->pIndexData, obj->level ))==NULL)
               {
                stc("{RBUG! Unable to create obj! Report to Imms NOW!{x\n\r",ch);
                return;
               }} 
      else
      {
        t_obj = obj;
        obj = obj->next_content;
        obj_from_char( t_obj );
      }

      if (t_obj->item_type == ITEM_SCUBA)
      {
         t_obj->timer=7;
      }

      if (t_obj->timer > 0 && !IS_OBJ_STAT(t_obj,ITEM_HAD_TIMER) && (t_obj->item_type != ITEM_SCUBA))
              t_obj->timer = 0;
      REM_BIT(t_obj->extra_flags,ITEM_HAD_TIMER);
      obj_to_char( t_obj, ch );
      if (cost < t_obj->cost) t_obj->cost = cost;
    }
  }
}
 
void do_list( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];char buf2[MAX_STRING_LENGTH],first_name[MAX_STRING_LENGTH];

  if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
  {
    ROOM_INDEX_DATA *pRoomIndexNext;
    CHAR_DATA *pet;
    bool found;

    /* hack to make new thalos pets work */
    if (ch->in_room->vnum == 9621) pRoomIndexNext = get_room_index(9706);
    else pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );

    if ( pRoomIndexNext == NULL )
    {
      bug( "Do_list: bad pet shop at vnum %u.", ch->in_room->vnum );
      stc( "Ты не можешь сделать этого тут.\n\r", ch );
      return;
    }

    found = FALSE;
    for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
    {
      if ( IS_SET(pet->act, ACT_PET) )
      {
        if ( !found )
        {
          found = TRUE;
          stc( "Зверушки на продажу:\n\r", ch );
        }
        do_printf( buf, "[%3d] %9d - %s (%s)\n\r",
          pet->level, 10 * pet->level * pet->level,
          get_char_desc(pet,'1'), pet->name );
        stc( buf, ch );
      }
    }
    if ( !found ) stc( "Извините, но у нас сейчас нет зверей.\n\r", ch );
    return;
  }
  else
  {
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int64 cost; 
    int count;
    bool found;
    char arg[MAX_INPUT_LENGTH];

    if ( ( keeper = find_keeper( ch ) ) == NULL ) return;
    one_argument(argument,arg);

    found = FALSE;
    for ( obj = keeper->carrying; obj; obj = obj->next_content )
    {
      if ( obj->wear_loc == WEAR_NONE
        &&   can_see_obj( ch, obj )
        &&   ( cost = get_cost( keeper, obj, TRUE ) ) > 0 
        &&   ( arg[0] == '\0'  
        ||  is_name(arg,obj->name) ))
      {
        if ( !found )
        {
          found = TRUE;
          stc( "[Ур  Цена    К-во] Назв.\n\r", ch );
        }

        if (IS_OBJ_STAT(obj,ITEM_INVENTORY))
        { 
          do_printf(buf,"[%3d %7d -- ]", obj->level,cost); 
          strcat(buf,get_obj_desc(obj,'1')); 
        } 
        else
        {
           count = 1;

           while (obj->next_content != NULL 
             && obj->pIndexData == obj->next_content->pIndexData
             && !str_cmp(obj->short_descr,
             obj->next_content->short_descr))
           {
             obj = obj->next_content;
             count++;
           }
           do_printf(buf,"[%3d %7d", obj->level,cost);
           do_printf(buf2," %3d] %s", count,get_obj_desc(obj,'1')); 
           strcat(buf, buf2); 
        }
        one_argument(obj->name,first_name); 
        do_printf(buf2,"{g({y%s{g){x\n\r",first_name); 
        if (obj->pIndexData->area->security==8) strcat( buf, buf2);
        else strcat(buf, "\n\r");
        stc( buf, ch );
      }
    }
    if ( !found ) stc( "В продаже ничего нет.\n\r", ch );
    return;
  }
}

void do_sell( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *keeper;
  OBJ_DATA *obj;
  int64 cost; 
  int roll;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    stc( "Продать что?\n\r", ch );
    return;
  }

  if ( ( keeper = find_keeper( ch ) ) == NULL ) return;

  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
  {
    act( "$c1 говорит тебе 'У тебя нет этой вещи'.", keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    stc( "Ты не можешь избавиться от этого.\n\r", ch );
    return;
  }

  if (!can_see_obj(keeper,obj))
  {
    act("$c1 не видит того, что ты ему предлагаешь.",keeper,NULL,ch,TO_VICT);
    return;
  }

  if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 || obj->morph_name)
  {
    act( "$c1 не выглядит заинтересованным в $i6.", keeper, obj, ch, TO_VICT );
    return;
  }

  if ( cost > (keeper-> silver + 100 * keeper->gold) )
  {
    act("$c1 говорит тебе 'Боюсь, у меня не хватит денег чтобы купить $i4'.",
            keeper,obj,ch,TO_VICT);
    return;
  }

  act( "$c1 продает $i4.", ch, obj, NULL, TO_ROOM );
  /* haggle */
  roll = number_percent();
  if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT) && roll < get_skill(ch,gsn_haggle) && cost>500)
  {
    stc("Ты торгуешься с лавочником.\n\r",ch);
    cost += obj->cost / 5 * roll / 100;
    cost = UMAX(cost,95 * get_cost(keeper,obj,TRUE) / 100);
    cost = UMIN(cost,(int)(keeper->silver + 100 * keeper->gold));
    check_improve(ch,gsn_haggle,TRUE,4);
  }

  do_printf( buf, "Ты продаешь $i4 за %u серебрянных и %u золотых монет.",
        cost - (cost/100) * 100, cost/100);
  act( buf, ch, obj, NULL, TO_CHAR );
  ch->gold     += cost/100;
  ch->silver   += cost - (cost/100) * 100;
  deduct_cost(keeper,cost);
  if ( keeper->gold < 0 ) keeper->gold = 0;
  if ( keeper->silver< 0) keeper->silver = 0;

  if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
        extract_obj( obj );
  else
  {
    obj_from_char( obj );
    if (obj->timer) SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
    else obj->timer = number_range(50,100);
    obj_to_keeper( obj, keeper );
  }
}

void do_value( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *keeper;
  OBJ_DATA *obj;
  int64 cost;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    stc( "Оценить что?\n\r", ch );
    return;
  }

  if ( ( keeper = find_keeper( ch ) ) == NULL ) return;

  if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
  {
    act( "$c1 говорит тебе 'У тебя нет этой вещи'.", keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
  }

  if (!can_see_obj(keeper,obj))
  {
    act("$c1 не видит того, что ты ему предлагаешь.",keeper,NULL,ch,TO_VICT);
    return;
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    stc( "Ты не можешь избавиться от этого.\n\r", ch );
    return;
  }

  if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
  {
    act( "$c1 не выглядит заинтересованным в $i6.", keeper, obj, ch, TO_VICT );
    return;
  }

  do_printf( buf, "$c1 говорит тебе 'Я дам тебе %u серебрянных и %u золотых монет за $i4'.", 
        cost - (cost/100) * 100, cost/100 );
  act( buf, keeper, obj, ch, TO_VICT );
  ch->reply = keeper;
}

void sac_obj( CHAR_DATA *ch, OBJ_DATA *obj)
{
  int64 silver;
  CHAR_DATA *gch;
  int members;
  char buffer[100];

  if ( obj->item_type == ITEM_CORPSE_PC )
  {
    if (obj->contains)
    {
      stc("Боги не примут этого жертвоприношения.\n\r",ch);
      return;
    }
  }

  if (!CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC))
  {
    act("$i1 не подходящий объект для приношения в жертву.",ch,obj,0,TO_CHAR);
    return;
  }

  if (obj->in_room != NULL)
  {
    for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
    if (gch->on == obj)
    {
      act("$C1, похоже, использует $i4.",ch,obj,gch,TO_CHAR);
      return;
    }
  }
                
  silver = UMAX(1,obj->level * 3);
  if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
    silver = UMIN(silver,obj->cost);

  if (silver == 1)
    ptc(ch,"%s дает тебе одну серебряную монету.\n\r",SabAdron);
  else
    ptc(ch,"%s дает тебе %u серебряных монет.\n\r",SabAdron,silver);
    
  ch->silver += silver;
    
  if (IS_CFG(ch,CFG_AUTOSPLIT))
  { 
    members = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
      if ( is_same_group( gch, ch ) ) members++;

    if ( members > 1 && silver > 1)
    {
      do_printf(buffer,"%u",silver);
      do_split(ch,buffer);      
    }
  }

  act("$c1 приносит $i4 в жертву Богам.", ch, obj, NULL, TO_ROOM );
  wiznet("$C1 приносит $i4 в жертву.", ch,obj,WIZ_SACCING,0);
  extract_obj( obj );
}

void do_exchange( CHAR_DATA *ch, const char *arg )
{
  OBJ_DATA *obj;
  OBJ_DATA *obj1;
  CHAR_DATA *changer;
  bool find;
  int i;

  find=FALSE;
  for ( changer=ch->in_room->people; changer; changer=changer->next_in_room )
  {
    if (IS_NPC(changer) && IS_SET(changer->act,ACT_IS_CHANGER))
    {
      find=TRUE;
      break;
    }
  }

  if(find==FALSE)
  {
    stc("Но тут не видно обменщика...\n\r",ch);
    return;
  }

  // get 100 diamonds for 1 crystal (str_cmp or str_prefix)
  if (!str_prefix(arg,"CRYSTAL") || !str_prefix(arg,"crystal"))
  {

    find=FALSE;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
      if ( obj->pIndexData->vnum==OBJ_VNUM_CRYSTAL && can_see_obj( ch, obj )
        && obj->wear_loc == WEAR_NONE)
      {
        find=TRUE;
        break;
      }
    }

    if(find)
    {
      extract_obj(obj);
      for (i=0;i!=100;i++)
      {
        if(!(obj = create_object( get_obj_index(OBJ_VNUM_DIAMOND), 0 )))
        {
          stc("{RBUG!Unable to create object!Report to Imms NOW!{x\n\r{x",ch);
          return;
        }
        obj_to_char(obj,ch);
      }
      stc("Ты меняешь {DКристалл{x на 100 бриллиантов.\n\r",ch);
      act("$c1 меняет {DКристалл{x на 100 бриллиантов.",ch,NULL,NULL,TO_ROOM);
    }
    else stc("Но у тебя нет {Dкристаллов{x в инвентори!\n\r",ch);
    return;
  }

  // get 1 crystal for 100 diamonds
  if (!str_cmp(arg,"diamond") || !str_cmp(arg,"DIAMOND"))
  {
    i=0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
      if ( obj->pIndexData->vnum==OBJ_VNUM_DIAMOND && can_see_obj(ch,obj)
        && obj->wear_loc == WEAR_NONE) i++;
      if (i==100) break;
    }
    if (i==100)
    {
      obj = ch->carrying;
      while (i!=0)
      {
        if (obj->pIndexData->vnum==OBJ_VNUM_DIAMOND && can_see_obj(ch,obj)
          && obj->wear_loc==WEAR_NONE)
        {
          obj1 = obj->next_content;
          extract_obj(obj);
          obj = obj1;
          i--;
        }
        else obj=obj->next_content;
        if (!obj) break;
      }
      if(!(obj = create_object( get_obj_index(OBJ_VNUM_CRYSTAL), 0 )))
      {
        stc("{RBUG!Unable to create object!Report to Imms NOW!{x\n\r{x",ch);
        return;
      }
      obj_to_char(obj,ch);
      stc("Ты меняешь 100 бриллиантов на 1 Кристалл.\n\r",ch);
      act("$c1 меняет 100 бриллиантов на {DКристалл{x.",ch,NULL,NULL,TO_ROOM);
    }
    else stc("У тебя нет 105 бриллиантов в инвентори!\n\r",ch);
    return;
  }
  stc( "Ты можешь обменять здесь:\n\r\n\r",ch);
  stc( "{Cdiamond{x - 100 бриллиантов на 1 Кристалл\n\r",ch);
  stc( "{Ccrystal{x - 1 кристалл на 100 бриллиантов\n\r",ch);
  return;
}

void do_auction (CHAR_DATA *ch, const char *argument)
{
  OBJ_DATA *obj;
  char arg1[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int64 startbet;

  argument = one_argument (argument, arg1);

  if (arg1[0] == '\0' )
  {
    stc("Auction info|bet|stop|<item> ?\n\r",ch);
    return;
  }

  if (!str_cmp(arg1,"help"))
  {
    stc("{GAUCTION{x - команда управления аукционом.\n\r",ch);
    stc("{CСинтаксис{x\n\r",ch);
    stc(" {Gauction help    - этот текст.\n\r",ch);
    stc(" {Gauction info    - получить краткую информацию про предмет на аукционе\n\r",ch);
    stc(" {Gauction bet <сумма> - сделать свою ставку.\n\r",ch);
    stc(" {Gauction <item> [стартовая цена]  - выставить предмет на аукцион.\n\r",ch);
    stc(" {Gauction <stop>  - остановить аукцион (только бог или аукционер).\n\n\r",ch);
    stc(" Новая ставка должна быть {Rминимум{x на {Y5 золотых{x монет больше предыдущей.\n\r",ch);
    stc(" {RНельзя выставить на аукцион морфленого персонажа.{x\n\r",ch);
    stc(" {RНе участвуйте в аукционе при уходе в армию или при выходе из игры.{x\n\r",ch);
    return;
  }

  if (!str_cmp(arg1,"info"))
  {
    if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"auction info")) return;
 
    if (auction->item != NULL)
    {
      if (auction->bet > 0)
       {if (auction->buyer) ptc(ch, "Текущая ставка %u золота.\n\r",auction->bet);
           else             ptc(ch, "Начальная ставка %u золота.\n\r",auction->bet);
       }
      else                  ptc(ch, "Ставок еще не было\n\r");
      ptc(ch, "Уpовень:%d   %s\n\r",auction->item->level,auction->item->name);
      if (IS_IMMORTAL(ch))
      {
        ptc(ch,"Аукционер:{Y%s{x   Покупатель:{Y%s{x   Предмет:%s{x\n\r",
         auction->seller?auction->seller->name:"{Dнет",
         auction->buyer?auction->buyer->name:"{Dнет",
         auction->item?item_name(auction->item->item_type):"{Dнет");
      }
      return;
    }
    stc ("Hет пpедметов на аукционе.\n\r",ch);
    return;
  }

  if (!str_cmp(arg1,"stop"))
  {
  if (!auction || !auction->item)
  {
    stc ("Hечего останавливать.\n\r",ch);
    return;
  }
  else
  {
    if (ch && auction->seller!=ch && !IS_IMMORTAL(ch)) return;
    do_printf (buf,"Аукцион на %s был остановлен, предмет конфискован.",get_obj_desc(auction->item,'1'));
    talk_auction (buf);
    if (ch) obj_to_char (auction->item, ch);
    auction->item = NULL;
    if (auction->buyer)
    {
      auction->buyer->gold += auction->bet;
      stc ("Ваши деньги возвpащены.\n\r",auction->buyer);
    }
    auction->seller = NULL;
    auction->buyer  = NULL;
    return;
  }
  }

  if (!str_cmp(arg1,"bet") ) 
  {
    if (auction->item != NULL)
    {
      int64 newbet;

      /* make - perhaps - a bet now */
      if (argument[0] == '\0'|| !is_number(argument))
      {
        stc ("Сколько вы хотите поставить на аукцион?\n\r",ch);
        return;
      }

      newbet = atoi64 (argument);

      if (newbet < (auction->bet + 5))
      {
        stc ("Ваша ставка должна быть как минимум на 5 монет больше текущей.\n\r",ch);
        return;
      }

      if (newbet > ch->gold)
      {
        stc ("У тебЯ нет столько золота!\n\r",ch);
        return;
      }

      /* the actual bet is OK! */
      if (IS_SET(ch->act,PLR_TIPSY)) 
        if (tipsy(ch,"auction bet")) return; // tipsy by Dinger 
 
      /* return the gold to the last buyer, if one exists */
      if (auction->buyer != NULL) auction->buyer->gold += auction->bet;

      ch->gold -= newbet; /* substract the gold - important :) */
      auction->buyer = ch;
      auction->bet   = (int)newbet;
      auction->going = 0;
      auction->pulse = PULSE_AUCTION; /* start the auction over again */
      do_printf (buf,"Ставка в %u золота была получена за %s.\n\r",newbet,get_obj_desc(auction->item,'1'));
      talk_auction (buf);
      return;
    }
    else
    {
      stc ("Сейчас ничего не выставлено.\n\r",ch);
      return;
    }
   }

    if ( (obj = get_obj_carry( ch, arg1, ch )) == NULL)
    {
      stc ("У тебя нет этого.\n\r",ch);
      return;
    }
                        
    if (obj->morph_name!=NULL)
    {
      stc ("Ты не можешь выставить это на аукцион.\n\r",ch);
      return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      stc( "Ты не можешь избавиться от этого.\n\r", ch );
      return;
    }

    if ( ((120<=obj->pIndexData->vnum) && (obj->pIndexData->vnum<=123)) || (obj->pIndexData->vnum==26))
    {
       ptc(ch, "Вы не можете выставить на аукцион квестовый предмет!\n\r");
       return;
    }

    if (obj->owner && obj->enchanted && !strcmp(obj->owner,ch->name))
    {
      stc( "Ты не можешь продать личную вещь.\n\r", ch );
      return; 
    }

    if (auction->item == NULL) 
      switch (obj->item_type)
    {

      default: 
       ptc(ch, "Вы не можете выставить на аукцион %s",item_name(obj->item_type));
       return;
      case ITEM_ARMOR:
      case ITEM_BOAT:
      case ITEM_BONUS:
      case ITEM_CLOTHING:
      case ITEM_CONTAINER:
      case ITEM_DRINK_CON:
      case ITEM_ENCHANT:
      case ITEM_FURNITURE:
      case ITEM_GEM:
      case ITEM_JEWELRY:
      case ITEM_LIGHT:
      case ITEM_MAP:
      case ITEM_PILL:
      case ITEM_POTION:
      case ITEM_SCROLL:
      case ITEM_STAFF:
      case ITEM_TREASURE:
      case ITEM_WAND:
      case ITEM_WARP_STONE:
      case ITEM_WEAPON:
 
      if (argument[0] == '\0'|| !is_number(argument))
        startbet = 0;
      else
        startbet = atoi64 (argument);
      
      if (startbet<0) startbet=0;

      if (IS_SET(ch->act,PLR_TIPSY)) 
       if (tipsy(ch,"auction put")) return; // tipsy by Dinger 
      obj_from_char (obj);
      auction->item = obj;
      auction->bet = startbet;
      auction->buyer = NULL;
      auction->seller = ch;
      auction->pulse = PULSE_AUCTION;
      auction->going = 0;

      if (startbet > 0)
        do_printf (buf, "Hа аукцион поступил новый пpедмет: %s.{/         Стартовая цена: %u.", get_obj_desc(obj,'1'),startbet);
      else
        do_printf (buf, "Hа аукцион поступил новый пpедмет: %s.", get_obj_desc(obj,'1'));

      talk_auction (buf);
      return;
   } /* switch */
   else act ("Попpобуй позже - сейчас на аукционе $i1!",ch,auction->item,NULL,TO_CHAR);
}

void do_send( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  char arg[MAX_INPUT_LENGTH],  buf[MAX_STRING_LENGTH];
  char temp[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA  *obj;
  OBJ_DATA *obj_next;
  int64 toll, iObjCount=1;
  bool all, found;
  int bFlush, number = 1, count=0;
    
  number = mult_argument((char *)argument, arg);
  argument = one_argument( arg, arg1 );
  argument = one_argument( argument, arg2 );

  if (number==1) number=MAX_OBJS_VALUE;

  if ( EMPTY(arg1) || EMPTY(arg2))
  {
    stc( "Послать (send) что? кому?\n\r", ch );
    return;
  }

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"send")) return; 
 
  if ( is_number( arg1 ) )
  {
    // send NNNN coins victim
    int64 amount;
    bool silver;

    amount   = atoi64(arg1);
    if ( amount <= 0 || ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" )
      && str_cmp( arg2, "gold"  ) && str_cmp( arg2, "silver")))
    {
      stc( "Ты не можешь этого сделать.\n\r", ch );
      return;
    }

    silver = str_cmp(arg2, "gold");
                
    argument = one_argument( argument, arg2 );
    if (EMPTY(arg2))
    {
      stc( "Кому дать?\n\r", ch );
      return;
    }
                 
    if ( (victim=get_char_room(ch, arg2))!=NULL )
    {
      stc( "Может лучше попробовать просто отдать? (give).\n\r", ch );
      return;
    }

    if (!(victim=get_pchar_world(ch, arg2)))
    {
      stc( "Получатель не обнаружен.\n\r", ch );
      return;
    }

    if ( IS_SET (victim->act, PLR_NOSEND) )
    {
      stc( "Получатель отказывается принимать твой перевод.\n\r", ch);
      return;
    }

    if ( IS_SET(victim->act, PLR_ARMY) ) 
    {
      stc( "Получатель служит на благо Отчизны.\n\r", ch);
      return;
    }

    if ( (!silver && ch->gold < amount) || (silver && ch->silver < amount) )
    {
      stc( "У тебя нет достаточного количества.\n\r", ch );
      return;
    }

    // Get 5% as a toll
    toll = amount/20;

    if (silver)
    {
      ch->silver -= amount;
      victim->silver  += amount-toll;
    }
    else
    {
      ch->gold -= amount;
      victim->gold += amount-toll;
    }

    do_printf(buf, "$c1 посылает тебе %u %s. Оплата пересылки: %u.",
      amount, silver?"серебра (silver)":"золота (gold)", toll);

    act( buf, ch, NULL, victim, TO_VICT );
    do_printf(buf,"Ты посылаешь $C3 %u %s.",amount,silver?"серебра (silver)":"золота (gold)");
    act( buf, ch, NULL, victim, TO_CHAR );
    return;
  }

  if ( (victim=get_char_room(ch,arg2))!=NULL )
  {
    stc( "Может лучше попробовать просто отдать? (give).\n\r", ch );
    return;
  }
                        
  if (!(victim=get_pchar_world(ch,arg2)))
  {
    stc( "Получатель не обнаружен.\n\r", ch );
    return;
  }

  if ( IS_SET (victim->act, PLR_NOSEND) )
  {
    stc( "Получатель отказывается принимать твою посылку.\n\r", ch);
    return;
  }

  all = !str_cmp( arg1, "all" ) || !str_prefix( "all.", arg1 );

  found = FALSE;
  for (obj=ch->carrying;obj;obj=obj_next)
  {
    obj_next = obj->next_content;
    if (((number==MAX_OBJS_VALUE && ((all && arg1[3]=='\0')
      || is_name(all?&arg1[4]:arg1,obj->name)))
      || (number!=MAX_OBJS_VALUE && is_name (arg1,obj->name) ) )
      && can_see_obj( ch, obj )
      && obj->wear_loc == WEAR_NONE)
    {
      found = TRUE;
      if ( obj->wear_loc != WEAR_NONE )
      {
        stc( "Да, но это экипировка, снимите (remove) для начала.\n\r", ch );
        continue;
      }
      if ( !can_drop_obj( ch, obj ) )
      {
        stc( "Ты не можешь избавиться от этого.\n\r", ch );
        if (all) continue;
        else return;
      }
 
      if (obj->morph_name)
      {
        if (obj->morph_name->level>ch->level || IS_SET(victim->act,PLR_ARMY) ||
         IS_SET(ch->act,PLR_ARMY))
        {
          stc("Ты не можешь этого сделать.\n\r", ch);
          return;
        }
        toll=obj->morph_name->level*ch->level*100*2/3;

        if (ch->gold*100+ch->silver < toll)
        {
          stc("У тебя не хватает денег для оплаты этой посылки.\n\r", ch);
          return;
        }

        if (ch->silver >= toll) ch->silver -= toll;
        else
        {
          // Convert some gold coins into silver
          toll-=ch->silver;
          ch->silver=100;
          ch->gold--;
          ch->gold -= toll/100;
          toll %= 100;
          ch->silver -= toll;
        }
      }

      if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
      {
        act( "Руки $C2 полны.", ch, NULL, victim, TO_CHAR );
        return;
      }

      if (get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w( victim ) )
      {
        act( "$C1 не может нести еще и эту тяжесть.", ch, NULL, victim, TO_CHAR );
        return;
      }

      if ( IS_SET (victim->act, PLR_ARMY) &&
       obj->item_type != ITEM_FOOD && 
       obj->item_type != ITEM_DRINK_CON)
      {
        stc( "В военную часть можно передавать только еду и выпивку.\n\r", ch);
        return;
      }

      /* We are going to get 10% of cost as toll */
      if (!obj->morph_name)
      {
        toll = obj->cost/10;

        if (ch->gold*100+ch->silver < toll)
        {
          stc("У тебя не хватает денег для оплаты этой посылки.\n\r", ch);
          return;
        }

        if (ch->silver >= toll) ch->silver -= toll;
        else
        {
          // Convert some gold coins into silver
          toll-=ch->silver;
          ch->silver=100;
          ch->gold--;
          ch->gold -= toll/100;
          toll %= 100;
          ch->silver -= toll;
        }
      }
      bFlush=TRUE;
      count++;
      if (is_same_obj (obj, obj->next_content) && count<number
         && !(!all && number==MAX_OBJS_VALUE))
      {
        iObjCount++;
        bFlush=FALSE;
      }

      obj_from_char( obj );
      obj_to_char( obj, victim );
                        
      act( "$c1 посылает $i4 в чьи-то руки.", ch, obj, victim, TO_NOTVICT );
      if (bFlush)
      {
        do_printf(temp,"{C%u{x %s",iObjCount, get_obj_desc(obj,num_char64(iObjCount)));
        act ("Ты посылаешь $t в руки $C3.", ch, temp, victim, TO_CHAR);
        act ("$c1 посылает тебе $t.", ch, temp, victim, TO_VICT);
        if (obj->morph_name)
          ptc(obj->morph_name,"{y%s{x посылает тебя {y%s{x.", ch->name, victim->name);
        iObjCount=1;
      }
      if (count>=number || (!all && number==MAX_OBJS_VALUE)) return;
     }
    }

    if ( !found ) {
    if ( all && (arg1[3] == '\0') )
      act( "Ты ничего не несешь.", ch, NULL, arg1, TO_CHAR );
    else
      act( "Ты не несешь $T.", ch, NULL, all?&arg1[4]:arg1, TO_CHAR );
   }
}

bool is_same_obj (OBJ_DATA *obj, OBJ_DATA *next)
{
  if (next==NULL || obj==NULL) return FALSE;
  if (strcmp (next->short_descr, obj->short_descr)) return FALSE;
  if (strcmp (next->description, obj->description)) return FALSE;
  if (next->pIndexData->vnum != obj->pIndexData->vnum) return FALSE;
        
  return TRUE;
}

char *  local_outtext ( int64 count, char *string_one, char *string_many) 
{ 
 static char     textbuf [MAX_STRING_LENGTH]; 
 
 if (count==1) return string_one; 
 
 do_printf (textbuf, string_many, count); 
 return textbuf; 
} 
 
void do_clanfit(CHAR_DATA *ch, const char *argument) 
{ 
 CHAR_DATA *keeper; 
 OBJ_DATA *obj; 
 AFFECT_DATA *paf, *apply; 
 int coeff; 
 char buff[MAX_STRING_LENGTH]; 
 long cost;

 if (argument[0]=='\0') 
 { 
  stc("Переделать что?\n\r",ch); 
  return; 
 } 
 if (ch==NULL) return; 
 if (ch->clan==NULL || ch->clan==clan_lookup("loner")) 
 { 
  stc("Ты не в клане.\n\r",ch); 
  return; 
 } 
 
 // searching for clanfitter 
 for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
 {
  if (!IS_NPC(keeper)) continue; 
  if (IS_SET(keeper->act,ACT_CLANENCHANTER)) break;
 }
 if (keeper==NULL) 
 { 
  stc("Здесь нет никого, кто переделал бы твою вещь.\n\r",ch); 
  return; 
 } 
 
 // seeking for object 
 obj = get_obj_carry(ch,argument,ch); 
 if (obj==NULL) 
 { 
  stc("У тебя нет такой вещи.\n\r",ch); 
  return; 
 } 
 
 if (obj->item_type == ITEM_WEAPON
  || obj->item_type == ITEM_WAND
  || obj->item_type == ITEM_STAFF)
 {
  stc("Такие вещи не могут быть перекованы.\n\r",ch);
  return;
 }

 if (IS_SET(obj->extra_flags,ITEM_CLANENCHANT))
 { 
  stc("Эта вещь уже переделана согласно нормам клана.\n\r",ch); 
  return; 
 } 
 
 if (ch->clan->wear_loc==-1) 
 { 
  stc("Твой клан не умеет переделывать вещи. Посоветуй своему Лидеру обратиться к Богам...\n\r",ch); 
  return; 
 } 
 
 if  ((ch->clan->wear_loc == ITEM_TAKE && obj->item_type != ITEM_LIGHT)
  || (!IS_SET(obj->wear_flags,ch->clan->wear_loc))) 
 { 
  stc("Твой клан не умеет переделывать такие вещи.\n\r",ch); 
  return; 
 } 
 
 cost = obj->level*100; //in gold
 if (ch->clan->wear_loc==ITEM_WEAR_WRIST
  || ch->clan->wear_loc==ITEM_WEAR_FINGER)
   cost /= 2;

  if (ch->pcdata->account < cost)
  {
   stc("У тебя недостаточно денег на счету для оплаты.\n\r",ch);
   return;
  }

  ch->pcdata->account -= cost;
 // we can fix it! :) 
 act_new("$C1 переделывает $i4 согласно нормам твоего клана.",ch,obj,keeper,TO_CHAR,POS_RESTING); 
 SET_BIT(obj->extra_flags,ITEM_CLANENCHANT); 
 one_argument(obj->name,buff);
 strcat(buff," clan ");
 strcat(buff,ch->clan->name);
 free_string(obj->name);
 obj->name = str_dup(buff);

 if (ch->clan->short_desc[0]!='\0')
 {
  free_string(obj->short_descr);
  obj->short_descr = str_dup(ch->clan->short_desc);
 }

 if (ch->clan->long_desc[0]!='\0')
 {
  free_string(obj->description);
  obj->description = str_dup(ch->clan->long_desc);
 }

 coeff = (obj->level>9)  + (obj->level>24) + (obj->level>39) 
       + (obj->level>54) + (obj->level>69) + (obj->level>84)
       + (obj->level>99); 
 if (coeff > 0)
  for(apply=ch->clan->mod;apply!=NULL;apply=apply->next) 
  { 
   paf             = new_affect(); 
   paf->location   = apply->location; 
   paf->modifier   = apply->modifier*coeff; 
   paf->type       = 0; 
   paf->duration   = -1; 
   paf->bitvector  = 0; 
   paf->level      = obj->level; 
   paf->next       = obj->affected; 
   obj->affected   = paf; 
  } 
} 
/* End pack code by TY */
// 'Split' originally by Gnort, God of Chaos.
void do_split( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *gch;
  int members;
  int64 amount_gold = 0, amount_silver = 0;
  int64 share_gold, share_silver;
  int64 extra_gold, extra_silver;

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( arg1[0] == '\0' )
  {
    stc( "Сколько разделить?\n\r", ch );
    return;
  }
  
  amount_silver = atoi64( arg1 );

  if (arg2[0] != '\0') amount_gold = atoi64(arg2);

  if ( amount_gold < 0 || amount_silver < 0)
  {
    stc( "Твоя группа этого не одобрит.\n\r", ch );
    return;
  }

  if ( amount_gold == 0 && amount_silver == 0 )
  {
    stc( "У тебя нет денег...Но никто этого не заметил.\n\r", ch );
    return;
  }

  if ( ch->gold <  amount_gold || ch->silver < amount_silver)
  {
    stc( "У тебя нет столько денег.\n\r", ch );
    return;
  }

  members = 0;
  for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
  {
    if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM)) members++;
  }

  if ( members < 2 )
  {
    stc( "Оставь их себе.\n\r", ch );
    return;
  }
          
  share_silver = amount_silver / members;
  extra_silver = amount_silver % members;

  share_gold = amount_gold / members;
  extra_gold   = amount_gold % members;

  if ( share_gold == 0 && share_silver == 0 )
  {
    stc( "Не утруждай себя, жадина.\n\r", ch );
    return;
  }

  ch->silver  -= amount_silver;
  ch->silver  += share_silver + extra_silver;
  ch->gold    -= amount_gold;
  ch->gold    += share_gold + extra_gold;

  if (share_silver > 0)
  {
    ptc(ch,"Ты делишь %u серебрянных монет. Тебе достается %u.\n\r",
            amount_silver,share_silver + extra_silver);
  }

  if (share_gold > 0)
  {
    ptc(ch,"Ты делишь %u золотых. Тебе достается %u.\n\r",
             amount_gold,share_gold + extra_gold);
  }

  if (share_gold == 0)
  {
    do_printf(buf,"$n делит %u серебрянных монет. Тебе достается %u.",
                amount_silver,share_silver);
  }
  else if (share_silver == 0)
  {
    do_printf(buf,"$n делит %u золотых. Тебе достается %u.",
                amount_gold,share_gold);
  }
  else
  {
    do_printf(buf,
  "$n делит %u серебрянных и %u золотых, твоя доля %u серебра и %u золота.\n\r",
         amount_silver,amount_gold,share_silver,share_gold);
  }

  for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
  {
    if ( gch != ch && is_same_group(gch,ch) && !IS_AFFECTED(gch,AFF_CHARM))
    {
      act( buf, ch, NULL, gch, TO_VICT );
      gch->gold += share_gold;
      gch->silver += share_silver;
    }
  }
}

