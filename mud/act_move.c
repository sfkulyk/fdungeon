// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"

// dname - I uses dname becouse dir_name's has different lenght of dir names
char * const dname [] =
{
  "north", "east ", "south", "west ", "up   ", "down "
};

char * const dir_name [] =
{
  "north", "east", "south", "west", "up", "down"
};

char * const dir_name2 [] =
{
  "на север", "на восток", "на юг", "на запад", "вверх", "вниз"
};

const int rev_dir [] =
{
  2, 3, 0, 1, 5, 4
};

const int movement_loss [SECT_MAX] =
{
   1, 2, 2, 3, 4, 8, 4, 8, 3,
   1, 7, 9,10, 1, 1,10, 5, 4, 2
};

/* SECT_INSIDE       0  SECT_AIR            9 
   SECT_CITY         1  SECT_DESERT        10 
   SECT_FIELD        2  SECT_ROCK_MOUNTAIN 12 
   SECT_FOREST       3  SECT_SNOW_MOUNTAIN 13 
   SECT_HILLS        4  SECT_ENTER         14 
   SECT_MOUNTAIN     5  SECT_ROAD          15 
   SECT_WATER_SWIM   6  SECT_SWAMP         16 
   SECT_WATER_NOSWIM 7  SECT_JUNGLE        17 
   SECT_UNUSED       8  SECT_RUINS         18
                        SECT_UWATER        19*/

bool scan_room(CHAR_DATA *ch, const ROOM_INDEX_DATA *room,char *buf)
{
  CHAR_DATA *target = room->people;
  bool found=FALSE;

  while (target != NULL)
  {
    if (can_see(ch,target,CHECK_LVL))
    {
      if (IS_NPC(target))
      {
        if (IS_STATUE(target))
        {
          target = target->next_in_room;
          continue;
        }
        strcat(buf,"\n\r                {W");
        strcat(buf,get_char_desc(target,'1'));
      }
      else
      {
        strcat(buf,"\n\r                {Y");
        strcat(buf,target->name);
      }
      found=TRUE;
    }
    target = target->next_in_room;
  }
  strcat(buf,"{x\n\r");
  return found;
}

bool scan_dir(CHAR_DATA *ch,const ROOM_INDEX_DATA *room,int dir,char *buf)
{
  EXIT_DATA * pexit=NULL;

  pexit = room->exit[dir];
  if (pexit == NULL) return TRUE;
  if (IS_SET(pexit->exit_info,EX_CLOSED))
  {
    strcat(buf,"   {DЗакрытая дверь.{x\n\r");
    stc(buf,ch);
    return TRUE;
  }
  if (pexit->u1.to_room == NULL || !can_see_room(ch,pexit->u1.to_room)) return TRUE;
  if (scan_room(ch,pexit->u1.to_room,buf)) stc(buf,ch);
  return FALSE;
}

void scan(CHAR_DATA *ch,int door)
{
  extern char * const dname[];
  char buf[MAX_STRING_LENGTH*3];
  int dir=0;
  const ROOM_INDEX_DATA *new_room;

  do_printf (buf, "{CЗдесь :{x");
  if (!scan_room(ch,ch->in_room,buf)) strcat (buf,"Никого\n\r");
  stc(buf,ch);

  if (door==100)
  {
    for (dir=0;dir<6;dir++)
    {
      do_printf(buf,"{CНа %s:{x",dname[dir]);
      if (scan_dir(ch,ch->in_room,dir,buf)) continue;
      if (!is_affected(ch,skill_lookup("farsight"))) continue;
      new_room=ch->in_room->exit[dir]->u1.to_room;
      do_printf(buf,"{CРядом на %s:{x",dname[dir]);
      scan_dir(ch,new_room,dir,buf);
    }
    return;
  }
        
  do_printf(buf,"{CНа %s:{x",dname[door]);
  if(scan_dir(ch,ch->in_room,door,buf)) return;
  new_room=ch->in_room->exit[door]->u1.to_room;
  do_printf(buf,"{CРядом на %s:{x",dname[door]);
  if (scan_dir(ch,new_room,door,buf)) return;
  new_room=new_room->exit[door]->u1.to_room;
  if (is_affected(ch,skill_lookup("farsight")))
  {
    do_printf(buf,"{CДалеко на %s:{x",dname[door]);
    if (scan_dir(ch,new_room,door,buf)) return;
  }
}

void do_north( CHAR_DATA *ch, const char *argument )
{
  do_move_char( ch, DIR_NORTH, FALSE, FALSE );
  return;
}

void do_east( CHAR_DATA *ch, const char *argument )
{
  do_move_char( ch, DIR_EAST, FALSE, FALSE );
  return;
}

void do_south( CHAR_DATA *ch, const char *argument )
{
  do_move_char( ch, DIR_SOUTH, FALSE, FALSE );
  return;
}

void do_west( CHAR_DATA *ch, const char *argument )
{
  do_move_char( ch, DIR_WEST, FALSE, FALSE );
  return;
}

void do_up( CHAR_DATA *ch, const char *argument )
{
  do_move_char( ch, DIR_UP, FALSE, FALSE );
  return;
}

void do_down( CHAR_DATA *ch, const char *argument )
{
  do_move_char( ch, DIR_DOWN, FALSE, FALSE );
  return;
}

int find_door( CHAR_DATA *ch, char *arg )
{
  EXIT_DATA *pexit;
  int door;

  if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
  else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
  else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
  else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
  else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
  else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
  else
  {
    for ( door = 0; door <= 5; door++ )
    {
      if ( ( pexit = ch->in_room->exit[door] ) != NULL
        &&   IS_SET(pexit->exit_info, EX_ISDOOR)
        &&   pexit->keyword != NULL
        &&   is_name( arg, pexit->keyword ) )  return door;
    }
    return -1;
  }

  if ( ( pexit = ch->in_room->exit[door] ) == NULL )
  {
    act( "На $T нет дверей.", ch, NULL, arg, TO_CHAR );
    return -1;
  }

  if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
  {
    stc( "Ты не можешь этого сделать.\n\r", ch );
    return -1;
  }
  return door;
}

void open_close( CHAR_DATA *ch, const char *argument,int action )
{
  // action: 1==open, else==close.
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;

  one_argument( argument, arg );

  if (EMPTY(arg))
  {
    ptc(ch,"%sкрыть что?\n\r",action==1?"От":"За");
    return;
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    // open door
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if ( action==1 && !IS_SET(pexit->exit_info, EX_CLOSED) )
      { stc( "Тут уже открыто.\n\r",      ch ); return; }
    if ( action!=1 && IS_SET(pexit->exit_info, EX_CLOSED) )
      { stc( "Тут уже закрыто.\n\r",      ch ); return; }
    if (  IS_SET(pexit->exit_info, EX_LOCKED) )
      { stc( "Заперто.\n\r",            ch ); return; }
    if (IS_SET(pexit->exit_info, EX_DWARVESGUILD) && !GUILD(ch,DWARVES_GUILD))
      { stc( "Ты не можешь сдвинуть это даже на дюйм.\n\r",ch ); return; }

    if (action==1)
    {
      REM_BIT(pexit->exit_info, EX_CLOSED);
      act( "$c1 открывает $d.", ch, NULL, pexit->keyword, TO_ROOM );
    }
    else
    {
      SET_BIT(pexit->exit_info, EX_CLOSED);
      act( "$c1 закрывает $d.", ch, NULL, pexit->keyword, TO_ROOM );
    }
    stc( "Ok.\n\r", ch );

    // open the other side
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
      &&   pexit_rev->u1.to_room == ch->in_room )
    {
      CHAR_DATA *rch;
      if (action==1)
      {
        REM_BIT( pexit_rev->exit_info, EX_CLOSED );
        for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
        act( "$d открывается.", rch, NULL, pexit_rev->keyword, TO_CHAR );
      }
      else
      {
        SET_BIT( pexit_rev->exit_info, EX_CLOSED );
        for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
        act( "$d закрывается.", rch, NULL, pexit_rev->keyword, TO_CHAR );
      }
    }
    return;
  }

  if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
  {
    // open portal
    if (obj->item_type == ITEM_PORTAL)
    {
      if (!IS_SET(obj->value[1], EX_ISDOOR))
      {
        stc("Ты не можешь сделать этого.\n\r",ch);
        return;
      }

      if (action==1 && !IS_SET(obj->value[1], EX_CLOSED))
      {
        stc("Тут уже открыто.\n\r",ch);
        return;
      }

      if (action!=1 && IS_SET(obj->value[1], EX_CLOSED))
      {
        stc("Тут уже закрыто.\n\r",ch);
        return;
      }

      if (IS_SET(obj->value[1], EX_LOCKED))
      {
        stc("Заперто.\n\r",ch);
        return;
      }

      if (IS_SET(obj->value[1], EX_DWARVESGUILD) && !GUILD(ch,DWARVES_GUILD))
      { stc( "Ты не можешь сдвинуть это даже на дюйм.\n\r",ch ); return; }

      if (action==1)
      {
        REM_BIT(obj->value[1], EX_CLOSED);
        act("Ты открываешь $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 открывает $i1.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        SET_BIT(obj->value[1], EX_CLOSED);
        act("Ты закрываешь $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 закрывает $i1.",ch,obj,NULL,TO_ROOM);
      }
      return;
    }

    /* 'open object' */
    if ( obj->item_type != ITEM_CONTAINER)
       { stc( "Это не контейнер.\n\r", ch ); return; }
    if ( action==1 && !IS_SET(obj->value[1], CONT_CLOSED) )
       { stc( "Тут уже открыто.\n\r",      ch ); return; }
    if ( action!=1 && IS_SET(obj->value[1], CONT_CLOSED) )
       { stc( "Тут уже закрыто.\n\r",      ch ); return; }
    if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
       { stc( "Ты не можешь этого сделать.\n\r",      ch ); return; }
    if ( IS_SET(obj->value[1], CONT_LOCKED) )
       { stc( "Заперто.\n\r",            ch ); return; }
    if (IS_SET(obj->value[1], EX_DWARVESGUILD) && !GUILD(ch,DWARVES_GUILD))
       { stc( "Ты не можешь сдвинуть это даже на дюйм.\n\r",ch ); return; }

    if (action==1)
    {
      REM_BIT(obj->value[1], CONT_CLOSED);
     act("Ты открываешь $i1.",ch,obj,NULL,TO_CHAR);
     act( "$c1 открывает $i1.", ch, obj, NULL, TO_ROOM );
    }
    else
    {
      SET_BIT(obj->value[1], CONT_CLOSED);
      act("Ты закрываешь $i1.",ch,obj,NULL,TO_CHAR);
      act( "$c1 закрывает $i1.", ch, obj, NULL, TO_ROOM );
    }
    return;
  }
  act( "Тут нет $T.", ch, NULL, arg, TO_CHAR );
}

void do_open( CHAR_DATA *ch, const char *argument )
{
  open_close(ch, argument, 1);
}

void do_close( CHAR_DATA *ch, const char *argument )
{
  open_close(ch, argument, 0);
}

bool has_key( CHAR_DATA *ch, int64 key )
{
  OBJ_DATA *obj;

  for (obj=ch->carrying;obj;obj=obj->next_content)
    if (obj->pIndexData->vnum==key) return TRUE;
  return FALSE;
}

void lock_unlock( CHAR_DATA *ch, const char *argument, int action )
{
  /* action : 1==LOCK, else UNLOCK */
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;

  one_argument( argument, arg );

  if ( EMPTY(arg))
  {
    ptc(ch,"%sпереть что?\n\r",action==1?"За":"От");
    return;
  }

  if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
  {
    // portal stuff
    if (obj->item_type == ITEM_PORTAL)
    {
      if (!IS_SET(obj->value[1],EX_ISDOOR)
       ||  IS_SET(obj->value[1],EX_NOCLOSE))
      {
        stc("Ты не можешь сделать этого.\n\r",ch);
        return;
      }

      if (!IS_SET(obj->value[1],EX_CLOSED))
      {
        stc("Дверь открыта нараспашку.\n\r",ch);
        return;
      }

      if (obj->value[4] < 0 || IS_SET(obj->value[1],EX_NOLOCK))
      {
        stc("Это не может быть заперто.\n\r",ch);
        return;
      }

      if (!has_key(ch,obj->value[4]))
      {
        stc("У тебя нет ключа.\n\r",ch);
        return;
      }

      if (action==1 && IS_SET(obj->value[1],EX_LOCKED))
      {
        stc("Тут уже заперто.\n\r",ch);
        return;
      }

      if (action!=1 && !IS_SET(obj->value[1],EX_LOCKED))
      {
        stc("Тут не заперто.\n\r",ch);
        return;
      }

      if (action==1)
      {
        SET_BIT(obj->value[1],EX_LOCKED);
        act("Ты запираешь $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 запирает $i1.",ch,obj,NULL,TO_ROOM);
        return;
      }
      else
      {
        REM_BIT(obj->value[1],EX_LOCKED);
        act("Ты отпираешь $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 отпирает $i1.",ch,obj,NULL,TO_ROOM);
        return;
      }
    }

    /* 'lock object' */
    if ( obj->item_type != ITEM_CONTAINER )
       { stc( "Это не контейнер.\n\r", ch ); return; }
    if ( !IS_SET(obj->value[1], CONT_CLOSED) )
       { stc( "Оно открыто.\n\r",        ch ); return; }
    if ( obj->value[2] < 0 )
       { stc( "Это не может быть закрыто.\n\r",     ch ); return; }
    if ( !has_key( ch, obj->value[2] ) )
       { stc( "У тебя нет ключа.\n\r",       ch ); return; }
    if ( action==1 && IS_SET(obj->value[1], CONT_LOCKED) )
       { stc( "Тут уже заперто.\n\r",    ch ); return; }
    if ( action!=1 && !IS_SET(obj->value[1], CONT_LOCKED) )
       { stc( "Тут уже не заперто.\n\r",    ch ); return; }
  
    if (action==1)
    {
       SET_BIT(obj->value[1], CONT_LOCKED);
       act("Ты запираешь $i1.",ch,obj,NULL,TO_CHAR);
       act( "$c1 запирает $i1.", ch, obj, NULL, TO_ROOM );
       return;
    }
    else
    {
      REM_BIT(obj->value[1], CONT_LOCKED);
      act("Ты отпираешь $i1.",ch,obj,NULL,TO_CHAR);
      act( "$c1 отпирает $i1.", ch, obj, NULL, TO_ROOM );
      return;
    }
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    /* 'lock door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
  
    pexit   = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
       { stc( "Сначала закрой это.\n\r",        ch ); return; }
    if ( pexit->key < 0 )
       { stc( "Это не может быть закрыто.\n\r",     ch ); return; }
    if ( !has_key( ch, pexit->key) )
       { stc( "У тебя нет ключа.\n\r",       ch ); return; }
    if ( action==1 && IS_SET(pexit->exit_info, EX_LOCKED) )
       { stc( "Тут уже заперто.\n\r",    ch ); return; }
    if ( action!=1 && !IS_SET(pexit->exit_info, EX_LOCKED) )
       { stc( "Тут уже не заперто.\n\r",    ch ); return; }

    if (action==1)
    {
      SET_BIT(pexit->exit_info, EX_LOCKED);
      act( "$c1 запирает $d.", ch, NULL, pexit->keyword, TO_ROOM );
    }
    else
    {
      REM_BIT(pexit->exit_info, EX_LOCKED);
      act( "$c1 отпирает $d.", ch, NULL, pexit->keyword, TO_ROOM );
    }
    stc( "*Щелк*\n\r", ch );

    // lock the other side
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
      && ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
      && pexit_rev->u1.to_room == ch->in_room )
    {
      if (action==1) SET_BIT( pexit_rev->exit_info, EX_LOCKED );
      else  REM_BIT( pexit_rev->exit_info, EX_LOCKED );
    }
  }
}

void do_lock( CHAR_DATA *ch, const char *argument )
{
  lock_unlock(ch, argument, 1);
}

void do_unlock( CHAR_DATA *ch, const char *argument )
{
  lock_unlock(ch, argument, 0);
}

void do_pick( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *gch;
  OBJ_DATA *obj;
  int door, chance;

  if (IS_NPC(ch) && !IS_SET(ch->act,ACT_THIEF))
  {
    cant_mes(ch);
    return;
  }
  one_argument( argument, arg );

  if (EMPTY(arg))
  {
    stc( "Взломать что?\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 + 2*category_bonus(ch,PERCEP)< gch->level )
    {
      act( "$C1 стоит слишком близко к замку.",ch, NULL, gch, TO_CHAR );
      return;
    }
  }
  chance = get_skill(ch,gsn_pick_lock)+ 2*get_curr_stat(ch,STAT_INT) +2*get_curr_stat(ch,STAT_DEX) -104;
  if ( !IS_NPC(ch) && number_percent( ) > chance+3*category_bonus(ch,PERCEP))
  {
    stc( "Неудача.\n\r", ch);
    check_improve(ch,gsn_pick_lock,FALSE,2);
    return;
  }

  if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
  {
    // portal stuff
    if (obj->item_type == ITEM_PORTAL)
    {
      if (!IS_SET(obj->value[1],EX_ISDOOR))
      {
        stc("Ты не можешь этого сделать.\n\r",ch);
        return;
      }

      if (!IS_SET(obj->value[1],EX_CLOSED))
      {
        stc("Тут не заперто.\n\r",ch);
        return;
      }

      if (obj->value[4] < 0)
      {
        stc("Ты не можешь отпереть это.\n\r",ch);
        return;
      }

      if (IS_SET(obj->value[1],EX_PICKPROOF))
      {
        stc("Слишком хороший замок...\n\r",ch);
        return;
      }

      REM_BIT(obj->value[1],EX_LOCKED);
      act("Ты взламываешь замок на $i1.",ch,obj,NULL,TO_CHAR);
      act("$c1 взламывает замок $i1.",ch,obj,NULL,TO_ROOM);
      check_improve(ch,gsn_pick_lock,TRUE,2);
      return;
    }

    // 'pick object'
    if ( obj->item_type != ITEM_CONTAINER )
    {
      stc( "Это не контейнер.\n\r", ch );
      return;
    }
    if ( !IS_SET(obj->value[1], CONT_CLOSED) )
    {
      stc( "Тут открыто.\n\r",ch );
      return;
    }
    if ( obj->value[2] < 0 )
    {
      stc( "Ты не можешь это отпереть.\n\r", ch );
      return;
    }
    if ( !IS_SET(obj->value[1], CONT_LOCKED) )
    {
      stc( "Тут не заперто.\n\r", ch );
      return;
    }
    if (IS_SET(obj->value[1], CONT_PICKPROOF) )
    {
      stc( "Слишком хороший замок...\n\r", ch );
      return;
    }

    REM_BIT(obj->value[1], CONT_LOCKED);
    act("Ты взламываешь замок на $i6.",ch,obj,NULL,TO_CHAR);
    act("$c1 взламывает замок на $i6.",ch,obj,NULL,TO_ROOM);
    check_improve(ch,gsn_pick_lock,TRUE,2);
    return;
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    // 'pick door'
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
    {
      stc( "Тут открыто.\n\r", ch );
      return;
    }
    if ( pexit->key < 0 && !IS_IMMORTAL(ch))
    {
      stc( "Ты не можешь взломать это.\n\r", ch );
      return;
    }
    if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
    {
      stc( "Тут не заперто.\n\r", ch );
      return;
    }
    if (IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
    {
      stc( "Слишком хороший замок.\n\r", ch );
      return;
    }

    REM_BIT(pexit->exit_info, EX_LOCKED);
    stc( "*Щелк*\n\r", ch );
    act( "$c1 взламывает $d.", ch, NULL, pexit->keyword, TO_ROOM );
    check_improve(ch,gsn_pick_lock,TRUE,2);

    // pick the other side
    if ( (to_room = pexit->u1.to_room) != NULL
      && (pexit_rev = to_room->exit[rev_dir[door]])!=NULL
      && pexit_rev->u1.to_room == ch->in_room )
    {
      REM_BIT( pexit_rev->exit_info, EX_LOCKED );
    }
  }
}

void do_stand( CHAR_DATA *ch, const char *argument )
{
  OBJ_DATA *obj = NULL;

  if (!EMPTY(argument))
  {
    if (ch->position == POS_FIGHTING)
    {
      stc("Ага...А потом твой враг встанет на твой труп.\n\r",ch);
      return;
    }
    obj = get_obj_list(ch,argument,ch->in_room->contents);
    if (!obj)
    {
      stc("Ты не видишь этого тут.\n\r",ch);
      return;
    }
    if (obj->item_type != ITEM_FURNITURE
     ||  (!IS_SET(obj->value[2],STAND_AT)
     &&   !IS_SET(obj->value[2],STAND_ON)
     &&   !IS_SET(obj->value[2],STAND_IN)))
    {
      stc("Ты не находишь места, чтобы стать на него.\n\r",ch);
      return;
    }
    if (ch->on != obj && count_users(obj) >= obj->value[0])
    {
      act_new("На $i6 нет для тебя места.", ch,obj,NULL,TO_CHAR,POS_DEAD);
      return;
    }
    if (obj != ch->on && obj->level > ch->level)
    {
     stc("Тут могут стоять только старшие!\n\r",ch);
     return;
    }
    ch->on = obj;
  }
    
  switch ( ch->position )
  {
    case POS_SLEEPING:
      if (is_affected(ch,gsn_sleep))
      { stc( "Ты не можешь проснуться!\n\r", ch ); return; }
        
      if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"stand_sleep")) return;

      if (!obj)
      {
        stc( "Ты просыпаешься и встаешь.\n\r", ch );
        act( "$c1 просыпается и встает.", ch, NULL, NULL, TO_ROOM );
        if (ch->on && IS_SET(ch->on->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL(ch))
        {
         act_new ("... И понимаешь, что не можешь сдвинуться с $i2!..",ch,ch->on,NULL,TO_CHAR,POS_DEAD);
         act ("... и понимает, что он не может сойти с этого места!",ch,NULL,NULL, TO_ROOM);
        }
        else {ch->on = NULL;}
      }
      else if (IS_SET(obj->value[2],STAND_AT))
      {
        act_new("Ты просыпаешься и встаешь возле $i2.",ch,obj,NULL,TO_CHAR,POS_DEAD);
        act("$c1 просыпается и встает возле $i2.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],STAND_ON))
      {
        act_new("Ты просыпаешься и встаешь на $i1.",ch,obj,NULL,TO_CHAR,POS_DEAD);
        act("$c1 просыпается и встает на $i1.",ch,obj,NULL,TO_ROOM);
      }
      else 
      {
        act_new("Ты просыпаешься и становишься в $i1.",ch,obj,NULL,TO_CHAR,POS_DEAD);
        act("$c1 просыпается и становится в $i1.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_STANDING;
      do_function(ch, &do_look, "auto");
      break;

    case POS_RESTING: 
    case POS_SITTING:
      if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"stand_rest")) return;

      if (!obj)
      {
        stc( "Ты встаешь.\n\r", ch );
        act( "$c1 встает.", ch, NULL, NULL, TO_ROOM );
        if (ch->on && IS_SET(ch->on->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL(ch))
        {
         act_new ("... И понимаешь, что не можешь сдвинуться с $i2!..",ch,ch->on,NULL,TO_CHAR,POS_DEAD);
         act ("... и понимает, что он не может сойти с этого места!",ch,NULL,NULL, TO_ROOM);
        }
        else {ch->on = NULL;}
      }
      else if (IS_SET(obj->value[2],STAND_AT))
      {
        act("Ты становишься на $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 становится на $i1.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],STAND_ON))
      {
        act("Ты становишься на $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 становится на $i1.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act("Ты становишься в $i1.",ch,obj,NULL,TO_CHAR);
        act("$c1 становится в $i1.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_STANDING;
      break;

    case POS_STANDING:
      stc( "Ты уже стоишь.\n\r", ch );
      break;

    case POS_FIGHTING:
      stc( "А как бы ты сражался лежа?\n\r", ch );
      break;
  }
}

void do_rest( CHAR_DATA *ch, const char *argument )
{
  OBJ_DATA *obj = NULL;

  if (ch->position == POS_FIGHTING)
  {
    stc("Отдыхать будешь на том свете.\n\r",ch);
    return;
  }

  // okay, now that we know we can rest, find an object to rest on
  if (!EMPTY(argument))
  {
    obj = get_obj_list(ch,argument,ch->in_room->contents);
    if (obj == NULL)
    {
      stc("Ты не видишь этого тут.\n\r",ch);
      return;
    }
  }
  else obj = ch->on;

  if (obj)
  {
    if (obj->item_type != ITEM_FURNITURE
     || (!IS_SET(obj->value[2],REST_ON)
     &&  !IS_SET(obj->value[2],REST_IN)
     &&  !IS_SET(obj->value[2],REST_AT)))
    {
      stc("Ты не можешь отдыхать на этом.\n\r",ch);
      return;
    }

    if (ch->on!=obj && count_users(obj) >= obj->value[0])
    {
      act_new("На $i6 не хватает места для тебя.",ch,obj,NULL,TO_CHAR,POS_DEAD);
      return;
    }

    if (obj != ch->on && obj->level > ch->level)
    {
     stc("Тут могут отдыхать только старшие!\n\r",ch);
     return;
    }

    ch->on = obj;
  }

  switch ( ch->position )
  {
    case POS_SLEEPING:
      if (is_affected(ch,gsn_sleep))
      {
        stc("Ты не можешь проснуться!\n\r",ch);
        return;
      }

      if (obj == NULL)
      {
        stc( "Ты просыпаешься и садишься отдыхать.\n\r", ch );
        act ("$c1 просыпается и садится отдыхать.",ch,NULL,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],REST_AT))
      {
        act_new("Ты просыпаешься и садишься отдыхать на $i4.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
        act("$c1 просыпается и садится отдыхать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],REST_ON))
      {
        act_new("Ты просыпаешься и садишься отдыхать на $i4.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
        act("$c1 просыпается и садится отдыхать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act_new("Ты просыпаешься и садишься отдыхать в $i4.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
        act("$c1 просыпается и садится отдыхать в $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;

    case POS_RESTING:
      stc( "Ты уже и так отдыхаешь.\n\r", ch );
      break;

    case POS_STANDING:
      if (obj == NULL)
      {
        stc( "Ты садишься отдыхать.\n\r", ch );
        act( "$c1 садится отдыхать.", ch, NULL, NULL, TO_ROOM );
      }
      else if (IS_SET(obj->value[2],REST_AT))
      {
        act("Ты садишься отдыхать на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится отдыхать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],REST_ON))
      {
        act("Ты садишься отдыхать на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится отдыхать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act("Ты садишься отдыхать в $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится отдыхать в $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;

    case POS_SITTING:
      if (obj == NULL)
      {
        stc("Ты садишься отдыхать.\n\r",ch);
        act("$c1 садится отдыхать.",ch,NULL,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],REST_AT))
      {
        act("Ты садишься отдыхать на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится отдыхать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],REST_ON))
      {
        act("Ты садишься отдыхать $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится отдыхать $i4.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act("Ты садишься отдыхать в $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится отдыхать в $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;
  }
}

void do_sit (CHAR_DATA *ch, const char *argument )
{
  OBJ_DATA *obj = NULL;

  if (ch->position == POS_FIGHTING)
  {
    stc("Сражаться сидя? Оригинальная мысль!\n\r",ch);
    return;
  }

  if (!EMPTY(argument))
  {
    obj = get_obj_list(ch,argument,ch->in_room->contents);
    if (!obj)
    {
      stc("Ты не видишь этого тут.\n\r",ch);
      return;
    }
  }
  else obj = ch->on;

  if (obj)
  {
    if (obj->item_type != ITEM_FURNITURE
     || (!IS_SET(obj->value[2],SIT_ON)
     &&  !IS_SET(obj->value[2],SIT_IN)
     &&  !IS_SET(obj->value[2],SIT_AT)))
    {
      stc("Ты не можешь сесть на это.\n\r",ch);
      return;
    }

    if (obj && ch->on != obj && count_users(obj) >= obj->value[0])
    {
      act_new("На $i6 нет места для тебя.",ch,obj,NULL,TO_CHAR,POS_DEAD);
      return;
    }

    if (obj != ch->on && obj->level > ch->level)
    {
     stc("Тут могут сидеть только старшие!\n\r",ch);
     return;
    }
    ch->on = obj;
  }
  switch (ch->position)
  {
    case POS_SLEEPING:
      if (is_affected(ch,gsn_sleep))
      {
        stc("Ты не можешь проснуться!\n\r",ch);
        return;
      }

      if (!obj)
      {
        stc( "Ты просыпаешься и садишься.\n\r", ch );
        act( "$c1 просыпается и садится.", ch, NULL, NULL, TO_ROOM );
      }
      else if (IS_SET(obj->value[2],SIT_AT))
      {
        act_new("Ты просыпаешься и садишься на $i4.",ch,obj,NULL,TO_CHAR,POS_DEAD);
        act("$c1 просыпается и садится на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],SIT_ON))
      {
        act_new("Ты просыпаешься и садишься на $i4.",ch,obj,NULL,TO_CHAR,POS_DEAD);
        act("$c1 просыпается и садится на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act_new("Ты просыпаешься и садишься в $i4.",ch,obj,NULL,TO_CHAR,POS_DEAD);
        act("$c1 просыпается и садится в $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
    case POS_RESTING:
      if (!obj) stc("Ты перестаешь отдыхать.\n\r",ch);
      else if (IS_SET(obj->value[2],SIT_AT))
      {
        act("Ты садишься на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится на $i4.",ch,obj,NULL,TO_ROOM);
      }

      else if (IS_SET(obj->value[2],SIT_ON))
      {
        act("Ты садишься на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится на $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
    case POS_SITTING:
      stc("Ты уже сидишь.\n\r",ch);
      break;
    case POS_STANDING:
      if (!obj)
      {
        stc("Ты садишься.\n\r",ch);
        act("$c1 садится на землю.",ch,NULL,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],SIT_AT))
      {
        act("Ты садишься на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],SIT_ON))
      {
        act("Ты садишься на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act("Ты садишься в $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 садится в $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
  }
}

void do_sleep( CHAR_DATA *ch, const char *argument )
{
  OBJ_DATA *obj = NULL;

  switch ( ch->position )
  {
  case POS_SLEEPING:
    stc( "Ты и так спишь.\n\r", ch );
    break;

  case POS_RESTING:
  case POS_SITTING:
  case POS_STANDING: 
    if (EMPTY(argument) && !ch->on)
    {
      stc( "Ты засыпаешь.\n\r", ch );
      act( "$c1 засыпает.", ch, NULL, NULL, TO_ROOM );
      ch->position = POS_SLEEPING;
    }
    else // find an object and sleep on it
    {
      if (EMPTY(argument)) obj = ch->on;
      else obj = get_obj_list( ch, argument,  ch->in_room->contents );

      if (!obj)
      {
        stc("Тут этого нет.\n\r",ch);
        return;
      }
      if (obj->item_type != ITEM_FURNITURE
       ||  (!IS_SET(obj->value[2],SLEEP_ON) 
       &&   !IS_SET(obj->value[2],SLEEP_IN)
       &&   !IS_SET(obj->value[2],SLEEP_AT)))
      {
        stc("Ты не можешь спать на этом!\n\r",ch);
        return;
      }

      if (ch->on != obj && count_users(obj) >= obj->value[0])
      {
        act_new("На $i6 нет для тебя места.", ch,obj,NULL,TO_CHAR,POS_DEAD);
        return;
      }

     if (obj != ch->on && obj->level > ch->level)
     {
      stc("Тут могут отдыхать только старшие!\n\r",ch);
      return;
     }
     ch->on = obj;
  
     if (IS_SET(obj->value[2],SLEEP_AT))
      {
        act("Ты ложишься спать на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 ложится спать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else if (IS_SET(obj->value[2],SLEEP_ON))
      {
        act("Ты ложишься спать на $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 ложится спать на $i4.",ch,obj,NULL,TO_ROOM);
      }
      else
      {
        act("Ты ложишься спать в $i4.",ch,obj,NULL,TO_CHAR);
        act("$c1 ложится спать в $i4.",ch,obj,NULL,TO_ROOM);
      }
      ch->position = POS_SLEEPING;
    }
    break;

  case POS_FIGHTING:
    stc( "Спать в бою? Не смеши людей!\n\r", ch );
    break;
  }
}

void do_wake( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );
  if ( EMPTY(arg))
  {
    do_function(ch, &do_stand, "");
    return;
  }

  if ( !IS_AWAKE(ch) )
  {
    stc( "Ты сам спишь!\n\r", ch );
    return;
  }

  if ((victim=get_char_room(ch,arg))==NULL)
  {
    stc( "Тут таких нет.\n\r", ch );
    return;
  }

  if ( IS_AWAKE(victim) )
  {
    act("$C1 не спит.",ch,NULL,victim,TO_CHAR);
    return;
  }

  if (is_affected(ch,gsn_sleep))
  {
    act( "Ты не можешь разбудить $G!", ch, NULL, victim, TO_CHAR );
    return;
  }

  act_new( "$c1 будит тебя.", ch, NULL, victim, TO_VICT,POS_SLEEPING );
  act_new( "Ты будишь $C4.", ch, NULL, victim, TO_CHAR,POS_SLEEPING );
  act_new( "$c1 будит $C4.", ch, NULL, victim, TO_ROOM,POS_SLEEPING );
}

void do_sneak( CHAR_DATA *ch, const char *argument )
{
  int chance;
  AFFECT_DATA af;

  if (IS_NPC(ch))
  {
    cant_mes(ch);
    return;
  }

  if ( !IS_AFFECTED(ch, AFF_SNEAK) 
    && IS_SET(race_table[ch->race].spec,SPEC_SNEAK))
  {
    SET_BIT(ch->affected_by,AFF_SNEAK);
    stc( "Теперь ты двигаешься бесшумно.\n\r", ch );
    return;
  }

  affect_strip( ch, gsn_sneak );
  
  chance = number_percent ();
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_ADRENOLIN]!=0 ) chance *= 4;
  
  if ( chance < get_skill(ch,gsn_sneak) )
  {
    check_improve(ch,gsn_sneak,TRUE,3);
    af.where     = TO_AFFECTS;
    af.type      = gsn_sneak;
    af.level     = ch->level; 
    af.duration  = ch->level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char( ch, &af );
    stc("Теперь ты двигаешься бесшумно.\n\r",ch);
    return;
  }
  check_improve(ch,gsn_sneak,FALSE,3);
  stc( "Ты стараешься двигаться более бесшумно.\n\r", ch );
}

void do_hide( CHAR_DATA *ch, const char *argument )
{
  int chance;
  
  if (IS_NPC(ch))
  {
    cant_mes(ch);
    return;
  }

  if (!IS_AFFECTED(ch, AFF_HIDE)
   && IS_SET(race_table[ch->race].spec,SPEC_HIDE)) 
  { 
    SET_BIT(ch->affected_by,AFF_HIDE); 
    stc( "Ты спрятался.\n\r", ch ); 
    return; 
  } 

  if ( IS_AFFECTED(ch, AFF_HIDE) && ch->race!=RACE_SPRITE)
       REM_BIT(ch->affected_by, AFF_HIDE);
  WAIT_STATE(ch,skill_table[gsn_hide].beats); 
  
  chance = number_percent ();
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_ADRENOLIN]!=0 ) chance *= 4;
  
  if ((chance < get_skill(ch,gsn_hide))
  &&  (ch->position == POS_STANDING))
  {
    SET_BIT(ch->affected_by, AFF_HIDE);
    check_improve(ch,gsn_hide,TRUE,3);
    stc( "Ты спрятался.\n\r", ch );
    return;
  }
  else check_improve(ch,gsn_hide,FALSE,3);
  stc( "Ты стараешься спрятаться.\n\r", ch );
  return;
}

// Contributed by Alander.
void do_visible( CHAR_DATA *ch, const char *argument )
{
  affect_strip (ch, gsn_invis);
  affect_strip ( ch, gsn_mass_invis);
  affect_strip ( ch, gsn_sneak);
  REM_BIT ( ch->affected_by, AFF_HIDE);
  REM_BIT (ch->affected_by, AFF_INVISIBLE);
  REM_BIT ( ch->affected_by, AFF_SNEAK);
  stc( "Ok.\n\r", ch );
}

void do_recall( CHAR_DATA *ch, const char *argument )
{
  ROOM_INDEX_DATA *location;
  int chance;

/*
  if (IS_NPC(ch) && !IS_SET(ch->act,ACT_PET))
  {
    stc("Только игpоки могут делать recall.\n\r",ch);
    return;
  }
*/

// Say "No!" to avengers near Hassan
  if ( IS_NPC(ch) && IS_SET(ch->act,ACT_AGGRESSIVE) ) return;

  if( IS_DEVOTED_ANY(ch) )
  {
    char buf[MAX_STRING_LENGTH];

    do_printf( buf, "{y    $c1{x пpосит %s пеpенести его!", 
       get_rdeity( deity_table[deity_lookup(ch->deity)].rname, '2') );
    act( buf, ch, 0, NULL, TO_ROOM );
  }
  else  
  {
    act( "{y   $c1{x пpосит Богов пеpенести его!", ch, 0, 0, TO_ROOM );
    if (!IS_IMMORTAL(ch) && (ch->level>5 || ch->remort>0) )
    {
      stc( "Боги отказываются от тебя...\n\r", ch);
      if (!IS_NPC(ch)) act( "Боги отказываются от {Y$c1{x...", ch, 0, 0, TO_ROOM );
      if ( ( number_range(1,2) == 2) && (!IS_IMMORTAL(ch) || !IS_NPC(ch)) ) return;
    }
  }   

  if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
  {
    stc( "Боги не услышали тебя.\n\r", ch );
    return;
  }

  if (EMPTY(argument) && !str_cmp(argument, "auto"))
     location=get_room_index(ROOM_VNUM_ALTAR);

  if ( ch->in_room == location ) return;

  if ( !IS_IMMORTAL(ch) && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR)
      && (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) 
      ||  IS_AFFECTED(ch, AFF_CURSE)
      ||  IS_SET(ch->in_room->ra,RAFF_EVIL_PR)
      ||  (!IS_NPC(ch) && IS_SET(ch->act, PLR_ARMY))))
  {
    stc( "Боги не услышали тебя.\n\r", ch );
    return;
  }

  if (ch->move<2)
  {
    stc("Ты слишком устал.\n\r",ch);
    return;
  }
     chance = get_skill(ch, skill_lookup("recall"));

  if( !IS_DEVOTED_ANY(ch) && ch->level > 20 && !IS_NPC(ch)) 
     chance /= 4;

  if ( !IS_IMMORTAL(ch) && (number_percent() > chance) && !IS_NPC(ch) 
/**/    && ((ch->level < 5) && (ch->remort < 1))
     )
  {
    stc( "Никакой реакции от Богов...\n\r", ch );
    WAIT_STATE( ch, skill_table[gsn_recall].beats );
    return;
  }

  if ( ch->fighting != NULL )
  {
    if ( number_range(1,50+ch->daze*2) > 25)
    {
      check_improve(ch,gsn_recall,FALSE,1);
      WAIT_STATE( ch, skill_table[gsn_recall].beats );
      stc( "Hеудача.\n\r", ch );
      return;
    }
    ch->exp-=100;
    stop_fighting(ch, TRUE);
    if (!IS_IMMORTAL(ch))
    stc ("Боги переносят тебя из битвы! Ты теряешь 100 опыта.\n\r",ch);
  }

  ch->move /= 2;
  act( "{y$c1{x исчезает.", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, location );
  act( "{y$c1{x появляется в комнате.", ch, NULL, NULL, TO_ROOM );
  do_function(ch, &do_look, "auto" );
  check_improve(ch,gsn_recall,TRUE,1);
  WAIT_STATE( ch, skill_table[gsn_recall].beats );
    
  if (ch->pet)  
   {
     act( "{y   $c1{x пpосит Богов пеpенести его!", ch->pet, 0, 0, TO_ROOM );
     char_from_room( ch->pet );
     char_to_room( ch->pet, location );    
     act( "{y$c1{x появляется в комнате.", ch->pet, NULL, NULL, TO_ROOM );
   }
//    do_function(ch->pet,&do_recall,(EMPTY(argument))?"":"auto");
}

void do_train( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *mob;
  char *buf= NULL;
  int stat;

  WAIT_STATE(ch,PULSE_VIOLENCE);
  for (mob=ch->in_room->people;mob;mob=mob->next_in_room)
    if (IS_NPC(mob) && IS_SET(mob->act,ACT_TRAIN)) break;

  if (!mob)
  {
    stc("Ты не можешь сделать этого тут.\n\r",ch);
    return;
  }

  if (EMPTY(argument))
  {
    ptc(ch,"У тебя %d сессий тренировки.\n\r", ch->train );
    ptc(ch,"Ты тренировал {Ghp{x: {G%d{x/{g%d{x раз, {Cmana{X: {C%d{x/{c%d{x раз.\n\r",
             ch->pcdata->hptrained, get_max_train(ch,STAT_HP), ch->pcdata->manatrained, get_max_train(ch,STAT_MANA));
    stc( "Ты можешь тренировать:",ch);
    if ( ch->perm_stat[STAT_STR] < get_max_train(ch,STAT_STR)) stc(" str,",ch);
    if ( ch->perm_stat[STAT_INT] < get_max_train(ch,STAT_INT)) stc(" int,",ch);
    if ( ch->perm_stat[STAT_WIS] < get_max_train(ch,STAT_WIS)) stc(" wis,",ch);
    if ( ch->perm_stat[STAT_DEX] < get_max_train(ch,STAT_DEX)) stc(" dex,",ch);
    if ( ch->perm_stat[STAT_CON] < get_max_train(ch,STAT_CON)) stc(" con,",ch);
    if ( ch->pcdata->hptrained   < get_max_train(ch,STAT_HP))  stc(" hp,", ch);
    if ( ch->pcdata->manatrained < get_max_train(ch,STAT_MANA))stc(" mana,",ch);
    return;
  }

  if ( ch->train<1 )
  {
    stc("У тебя не осталось сессий тренировки.\n\r",ch);
    return;
  }

  if (!str_cmp(argument,"str"))
  {
    stat=STAT_STR;
    buf="силу";
  }
  else if (!str_cmp(argument,"int"))
  {
    stat=STAT_INT;
    buf="интеллект";
  }
  else if (!str_cmp(argument,"wis"))
  {
    stat=STAT_WIS;
    buf="мудрость";
  }
  else if (!str_cmp(argument,"dex"))
  {
    stat= STAT_DEX;
    buf="ловкость";
  }
  else if (!str_cmp(argument,"con"))
  {
    stat= STAT_CON;
    buf="телосложение";
  }
  else if (!str_cmp(argument,"hp"))
  {
    stat=STAT_HP;
    buf="здоровье";
  }
  else if (!str_cmp(argument,"mana"))
  {
    stat=STAT_MANA;
    buf="ману";
  }
  else
  {
    stc("Такого параметра нет.\n\r",ch);
    return;
  }

  if(stat<STAT_HP)
  {
    if (ch->perm_stat[stat] >= get_max_train(ch,stat))
    {
      ptc(ch,"Ты не можешь тренировать %s - этот параметр на максимуме.",buf);
      return;
    }
    ch->train--;
    ch->perm_stat[stat]++;
    act("Ты тренируешь $T.",ch,NULL,buf,TO_CHAR);
    act("$c1 тренирует $T.",ch,NULL,buf,TO_ROOM);
    return;
  }
  if (stat==STAT_HP)
  {
    if (ch->pcdata->hptrained>=get_max_train(ch,STAT_HP))
    {
      stc("Ты больше не можешь тренировать Здоровье.\n\r", ch);
      return;
    }
    ch->pcdata->perm_hit += 10;
    ch->max_hit += 10;
    ch->hit +=10;
    ch->pcdata->hptrained++;
  }
  else if (stat==STAT_MANA)
  {
    if (ch->pcdata->manatrained>=get_max_train(ch,STAT_MANA))
    {
      stc("Ты больше не можешь тренировать Ману.\n\r", ch);
      return;
    }
    ch->pcdata->perm_mana += 10;
    ch->max_mana += 10;
    ch->mana += 10;
    ch->pcdata->manatrained++;
  }
  else
  {
    stc("Нет такого параметра.\n\r", ch);
    return;
  }
  ch->train--;
  act( "Ты повышаешь $T.",ch,NULL,buf,TO_CHAR);
  act( "$c1 повышает $T.",ch,NULL,buf,TO_ROOM);
}

void do_clan_recall( CHAR_DATA *ch, const char *argument )
{
  ROOM_INDEX_DATA *location;
  char buf[MAX_STRING_LENGTH];
  int chance;

  if ( (IS_NPC(ch) && !IS_SET(ch->act,ACT_PET)) || ch->clan==NULL)
  {
    stc("Только клановые игроки могут перемещатся в клан.\n\r",ch);
    return;
  }

  do_printf(buf, ch->clan->recalmsg1);
  act( buf, ch, 0, 0, TO_ROOM );

  if ((location=get_room_index(ch->clan->clanrecal))==NULL)
  {
    stc( "Ты заблудился в пространстве...\n\r", ch );
    return;
  }

  if (!ch->in_room)
  {
    bug("Char in NULL room",0);
    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
  }

  if (ch->in_room == location) return;

  if (!IS_IMMORTAL(ch) && ch->in_room->sector_type!=SECT_CITY
      && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR))
  {
    stc("Путь в клан открыт только из города.\n\r", ch);
    return;
  }

  if ( !IS_IMMORTAL(ch) && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR)
      && (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) 
      ||  IS_AFFECTED(ch, AFF_CURSE)
      ||  IS_SET(ch->in_room->ra,RAFF_EVIL_PR)
      ||  (!IS_NPC(ch) && IS_SET(ch->act, PLR_ARMY))))
  {
    stc( "Боги не слышат тебя.\n\r", ch );
    return;
  }

  if (IS_SET(ch->act,PLR_WANTED) || ch->criminal>50)
  {
    stc( "Боги не помогают преступникам.\n\r", ch );
    return;
  }

  if (ch->move<2)
  {
    stc("Ты слишком устал.\n\r",ch);
    return;
  }

  chance = get_skill(ch, skill_lookup("recall"));
  if ( number_percent() > chance )
  {
    stc( "Никакой реакции от Богов...\n\r", ch );
    WAIT_STATE( ch, skill_table[gsn_recall].beats );
    return;
  }

  if ( ch->fighting != NULL )
  {
    if ( number_range(1,50+ch->daze*2) > 25)
    {
      stc( "Hеудача.\n\r", ch );
      return;
    }
    ch->exp-=130;
    stop_fighting(ch, TRUE);
    stc ("Боги переносят тебя из битвы! Ты теряешь 130 опыта.\n\r",ch);
  }

  ch->move /= 2;

  do_printf(buf, ch->clan->recalmsg2);
  act( buf, ch, NULL, NULL, TO_ROOM );

  char_from_room( ch );
  char_to_room( ch, location );
  act( "$c1 внезапно появляется в комнате.", ch, NULL, NULL, TO_ROOM );
  do_look( ch, "auto" );
 
  if (ch->pet) do_clan_recall(ch->pet,"");
}

void do_arecall( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location;
  int chance;

  if (IS_NPC(ch) && !IS_SET(ch->act,ACT_PET))
  {
    stc("Только игpоки могут делать recall.\n\r",ch);
    return;
  }
  
  act( "{y$c1{x пpосит Богов пеpенести его на Арену!", ch, 0, 0, TO_ROOM );

  if ( ( location = get_room_index( ROOM_VNUM_ARENA ) ) == NULL )
  {
    stc( "Боги не услышали тебя.\n\r", ch );
    return;
  }

  // Lina and Saboteur
  if (!IS_IMMORTAL(ch))
  {
    if ((!ch->in_room || ch->in_room->sector_type!=SECT_CITY) && !IS_IMMORTAL(ch))
    {
      stc("Путь на арену открыт только из города.\n\r", ch);
      return;
    }
    if ((IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
        && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR))
       || IS_AFFECTED(ch, AFF_CURSE)
       || IS_SET(ch->in_room->ra,RAFF_EVIL_PR)
       || (!IS_NPC(ch) && IS_SET(ch->act, PLR_ARMY)))
    {
      stc( "Боги отказываются от тебя.\n\r", ch );
      return;
    }
  }
  if ( ( victim = ch->fighting ) != NULL )
  {
    stc( "Но ведь ты сражаешься!", ch );
  }
  if (ch->move<2)
  {
    stc("Ты слишком устал.\n\r",ch);
    return;
  }

  chance = get_skill(ch, skill_lookup("recall"));
  if ( number_percent() > chance )
  {
    stc( "Никакой реакции от Богов...\n\r", ch );
    WAIT_STATE( ch, skill_table[gsn_recall].beats );
    return;
  }

  ch->move /= 2;
  act( "{y$c1{x исчезает.", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, location );
  act( "{y$c1{x появляется в комнате.", ch, NULL, NULL, TO_ROOM );
  do_function(ch, &do_look, "auto" );
    
  if (ch->pet != NULL)
  do_function(ch->pet, &do_arecall, "");
}

void do_rape( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  ISORDEN(ch);
  if (!ch->in_room)
  {
    bug("Char in NULL room",0);
    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
  }

  if (ch->in_room == get_room_index(ROOM_VNUM_ALTAR))
  {
    stc("Разврат в храме Богов недопустим!\n\r",ch);
    return;
  }

  if (IS_SET(ch->act,PLR_NOMLOVE))
  {
    stc("Тебе запрещено заниматься сексом.\n\r", ch);  return ;
  }

  one_argument( argument, arg );

  if (EMPTY(arg))
  {
    stc("Ты гоняешься с похотливым взглядом за каждым, кого видишь!\n\r", ch);
    act("$c1 гоняется за всеми с похотливым взглядом. Берегись!",ch,NULL,NULL,TO_ROOM);
    return;
  }

  if (!str_cmp(arg,"self")) victim=ch;
  else victim = get_char_room(ch,arg);

  if (!victim)
  {
    stc( "Тут таких нет.\n\r",ch );
    return;
  }
  if (ch==victim)
  {
    stc( "Тихо сам с собою, левою рукою...\n\r", ch);
    act( "$c1 занимается онанизмом у всех на виду...",ch,NULL,NULL,TO_ROOM);
    return;
  }

  if (!IS_MARRY(ch,victim) && !IS_LOVER(ch,victim)
   && !IS_NPC(victim) && !IS_SET(ch->act,PLR_RAPER)
   && !is_exact_name(ch->pcdata->lovers,"ALL"))
  {
    SET_BIT(ch->act, PLR_RAPER);
    stc( "Грязный насильник...\n\r", ch );
    save_char_obj( ch );
  }

  act("$n срывает с $N одежду и бурно занимаетсЯ с $F любовью....", ch, NULL,victim, TO_NOTVICT);
  act("Ты срываешь одежду с $N и бурно занимаешьсЯ с $F любовью.",ch,NULL,victim,TO_CHAR);
  act("$n срывает с тебЯ одежду...Да! Еще! Ахх...",ch,NULL,victim,TO_VICT);

  // заражение / самовозникающий сифилис - юзайте презики :)
  if (!IS_MARRY(ch,victim) && !IS_LOVER(ch,victim) 
     && !is_exact_name(ch->pcdata->lovers,"ALL") &&
     ((!IS_NPC(victim) && IS_SET(victim->act,PLR_SIFILIS)
     && number_percent()>30) || number_percent()<95))
  {
    OBJ_DATA *condom = get_obj_carry(ch,"condom",ch);
    if (condom)
    {
      stc("Твой презерватив покрывается какой-то мерзостью и растворяется в ней.\n\r",ch);
      unequip_char(ch,condom);
      extract_obj(condom);
    }
    else 
    {
      SET_BIT(ch->act, PLR_SIFILIS);
      stc("Ты чувствуешь себя нехорошо...похоже, ты начинаешь гнить заживо.",ch);
      act("$n, похоже, приболел...",ch,NULL,NULL,TO_ROOM);
    }
  }
}

bool do_move_char( CHAR_DATA *ch, int door, bool follow, bool run )
{
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  ROOM_INDEX_DATA *in_room;
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit;
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  bool found=FALSE;
  int move;

  if ( door < 0 || door > 5 ) return FALSE;
  in_room = ch->in_room;
  if (( pexit   = in_room->exit[door] ) == NULL
   || ( to_room = pexit->u1.to_room   ) == NULL 
   || !can_see_room(ch,pexit->u1.to_room))
  {
    stc( "К сожалению, ты не можешь туда идти.\n\r", ch );
    return FALSE;
  }

  if (IS_SET(in_room->ra,RAFF_WEB) && !IS_IMMORTAL(ch) 
       && raffect_level(in_room,RAFF_WEB) > ch->level)
  {
    stc("Магическая паутина оплела все выходы вокруг.\n\r",ch);
    return FALSE;
  }

  if (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_TRUSTED(ch,ANGEL))
  {
    if(!IS_AFFECTED(ch, AFF_PASS_DOOR))
    {
      if (!run)
      {
        stc("Здесь закрыто.\n\r",ch);
        return FALSE;
      }
      // try to open door
      open_close(ch, dir_name[door], 1);
      if (IS_SET(pexit->exit_info, EX_LOCKED) ) lock_unlock(ch, dir_name[door],0);
      if ( IS_SET(pexit->exit_info, EX_LOCKED) )
      {
        stc("Здесь заперто.\n\r",ch);
        return FALSE;
      }
    }
    else if (IS_SET(pexit->exit_info,EX_NOPASS))
    {
      // try to open door
      open_close(ch, dir_name[door], 1);
      if (IS_SET(pexit->exit_info, EX_LOCKED) ) lock_unlock(ch, dir_name[door],0);
      if ( IS_SET(pexit->exit_info, EX_LOCKED) )
      {
        stc("Здесь заперто.Дверь околдована.\n\r",ch);
        return FALSE;
      }
      stc("Здесь закрыто.Дверь околдована.\n\r",ch);
      return FALSE;
    }
  }

  if ( !IS_NPC(ch) && mp_exit_trigger( ch, door ) ) return FALSE;

  if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL
    && in_room == ch->master->in_room )
  {
    stc( "Что? Покинуть любимого хозяина?\n\r", ch );
    return FALSE;
  }

  if ( !is_room_owner(ch,to_room) && room_is_private( to_room ) )
  {
    stc( "Эта комната в данный момент занята.\n\r", ch );
    return FALSE;
  }

  if (
         IS_NPC(ch)  && !IS_SET(ch->act,ACT_PET)
      && (
            IS_SET(to_room->room_flags,ROOM_MAG_ONLY)
         || IS_SET(to_room->room_flags,ROOM_CLE_ONLY)
         || IS_SET(to_room->room_flags,ROOM_THI_ONLY)
         || IS_SET(to_room->room_flags,ROOM_WAR_ONLY)
         )
     )
  return FALSE;
  

  if ( !IS_NPC(ch) )
  {
    if (
           ((ch->classmag!=1 || ch->pcdata->condition[COND_ADRENOLIN]>0)
                            && IS_SET(to_room->room_flags,ROOM_MAG_ONLY))
        || ((ch->classcle!=1 || ch->pcdata->condition[COND_ADRENOLIN]>0)
                            && IS_SET(to_room->room_flags,ROOM_CLE_ONLY))
        || ((ch->classthi!=1 || ch->pcdata->condition[COND_ADRENOLIN]>0)
                            && IS_SET(to_room->room_flags,ROOM_THI_ONLY))
        || ((ch->classwar!=1 || ch->pcdata->condition[COND_ADRENOLIN]>0)
                            && IS_SET(to_room->room_flags,ROOM_WAR_ONLY))
       )
    {
      stc("Тебе туда нельзя.\n\r",ch);
      return FALSE;
    }
  }
 
  if (ch->on) 
  {
   stc ("Сначала сойди на землю.\n\r",ch);
   return FALSE;
  }
 
  if ( !IS_NPC(ch) && ch->gold < 1 && ch->pcdata->account < 1 && 
      IS_SET(to_room -> room_flags,ROOM_DWARVES_RENT))
  {
    stc("{yГном-охранник{x на входе пристально смотрит на твои карманы...",ch);
    stc("Не найдя там ничего {Yзолотого{x, он разворачивает тебя взад. ",ch);
    return FALSE;
  }

  if ( (in_room->sector_type != SECT_UWATER && to_room->sector_type == SECT_UWATER)
    && !is_affected(ch,skill_lookup("wbreath"))
    && !IS_SET(race_table[ch->race].spec,SPEC_UWATER))
  {
    OBJ_DATA *obj;
    bool found=FALSE;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_SCUBA )
      {
        found = TRUE;
        break;
      }
    }
    if ( !found ) stc( "Ты задерживаешь дыхание и НЫРЯЕШЬ!\n\r", ch );
  }

  if ( (in_room->sector_type == SECT_UWATER && to_room->sector_type != SECT_UWATER)
    && !is_affected(ch,skill_lookup("wbreath"))
    && !IS_SET(race_table[ch->race].spec,SPEC_UWATER))
  stc( "Теперь можно выдохнуть.\n\r", ch );

  if ( (in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR)
    && (!IS_AFFECTED(ch, AFF_FLYING) && !IS_IMMORTAL(ch) ) )
  {
    stc( "Ты не умеешь летать.\n\r", ch );
    return FALSE;
  }

  if ( ( in_room->sector_type == SECT_WATER_SWIM
      || to_room->sector_type == SECT_WATER_SWIM
      || to_room->sector_type == SECT_WATER_NOSWIM
      || to_room->sector_type == SECT_WATER_NOSWIM )
    && ( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_SWIM) ) )
  {
    OBJ_DATA *obj;
    bool found=FALSE;

    if (IS_IMMORTAL(ch)) found = TRUE;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_BOAT )
      {
        found = TRUE;
        break;
      }
    }
    if ( !found )
    {
      stc( "Тебе нужна лодка.\n\r", ch );
      return FALSE;
    }
  }

    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
       + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)];
    move /= 2;
  
    if (IS_AFFECTED(ch,AFF_SLOW)) move *= 2;
    if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE)) move /= 2;
  
    if (ch->move < move )
    {
      stc( "Ты слишком устал.\n\r", ch );
      return FALSE;
    }

    WAIT_STATE( ch, 1 );
    ch->move -= move;

  // moving char
  if ( !IS_AFFECTED(ch, AFF_SNEAK) &&  ch->invis_level < LEVEL_HERO)
  {
    if ( !IS_AFFECTED(ch, AFF_FLYING))
         act( "$c1 уходит $T.",  ch, NULL, dir_name2[door], TO_ROOM );
    else act( "$c1 улетает $T.", ch, NULL, dir_name2[door], TO_ROOM );
  }

  char_from_room( ch );
  char_to_room( ch, to_room );

  if ( !IS_AFFECTED(ch, AFF_SNEAK) && ch->invis_level < LEVEL_HERO)
  {
      if (!IS_AFFECTED(ch, AFF_FLYING))
         act( "Входит $c1.",  ch, NULL, NULL, TO_ROOM );
    else act( "Влетает $c1.", ch, NULL, NULL, TO_ROOM );
  }

  if (run && IS_CFG(ch,CFG_RUNSHOW)) ptc(ch,"%s\n\r",to_room->name);
  else do_function(ch, &do_look, "auto" );

  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
  {
    if (obj->morph_name!=NULL )
    {
      ptc(obj->morph_name,"{y%s{x уносит тебя %s.\n\r", ch->name, dir_name2[door]);
      if (run && IS_CFG(obj->morph_name,CFG_RUNSHOW)) ptc(obj->morph_name,"%s\n\r",to_room->name);
      else do_function(obj->morph_name, &do_look, "auto" );
    }
  }

  if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
    mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_ENTRY );

  if ( !IS_NPC( ch ) ) mp_greet_trigger( ch );

  if (in_room == to_room) return TRUE; /* no circular follows */
  for ( fch = in_room->people; fch != NULL; fch = fch_next )
  {
    fch_next = fch->next_in_room;
    if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM) 
     && fch->position < POS_STANDING) do_function(fch, &do_stand, "");
    if ( fch->master == ch && fch->position == POS_STANDING 
     && can_see_room(fch,to_room))
    {
      if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
        && (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
      {
        act("Ты не можешь провести $C4 в город.", ch,NULL,fch,TO_CHAR);
        act("Тебе нельзя в город.", fch,NULL,NULL,TO_CHAR);
        continue;
      }

      if (IS_SET(ch->in_room->room_flags,ROOM_NO_MOB) && IS_NPC(fch))
      {
        act("Ты не можешь провести $C4 в эту комнату.", ch,NULL,fch,TO_CHAR);
        act("Тебе туда нельзя.", fch,NULL,NULL,TO_CHAR);
        continue;
      }
      
      act( "Ты следуешь за $C5.", fch, NULL, ch, TO_CHAR );
      do_move_char( fch, door, TRUE, FALSE );

      if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
       mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_ENTRY );

      if ( !IS_NPC( ch ) ) mp_greet_trigger( ch );

      found=FALSE;
      for ( fch = ch->in_room->people; fch; fch=fch->next_in_room )
      {
        if ( fch!=NULL && IS_NPC(fch) && fch->pIndexData->pShop != NULL )
        {
          found=TRUE;
          break;
        }
      }

      if (found==TRUE && !IS_NPC(ch) && fch->stealer!=NULL 
          && is_exact_name(ch->name,fch->stealer))
      {
        do_printf(buf,"{Y%s{m грязный {RВОР{m! ДЕРЖИТЕ ВОРА!{x\n\r",ch->name);
        SET_BIT(fch->talk,CBIT_SHOUT);
        do_yell(fch, buf);
        multi_hit( fch, ch);
      }
    }
  }
   if ( (!IS_AFFECTED(ch,AFF_HIDE) )
        || (!IS_AFFECTED(ch, AFF_SNEAK))
        || (IS_NPC(ch))
        || (ch->clan == NULL) )
    REM_BIT(ch->affected_by,AFF_HIDE);
   return TRUE;
}

// random room generation procedure
ROOM_INDEX_DATA  *get_random_room(CHAR_DATA *ch)
{
  ROOM_INDEX_DATA *room;
  int64 index;

  for ( ; ; )
  {
    index=number_range(1,512)+number_range(1,512)+1000*number_range(0,30);
    room=get_room_index(index);
    if (room==NULL) continue;
    if ( can_see_room(ch,room)
     && !IS_SET(room->room_flags, ROOM_PRIVATE)
     && !IS_SET(room->room_flags, ROOM_SOLITARY)
     && !IS_SET(room->room_flags, ROOM_SAFE)
     && !IS_SET(room->ra, RAFF_SAFE_PLC)
     && !IS_SET(room->ra, RAFF_VIOLENCE)
     && !room_is_private(room)
     && (!IS_NPC(ch) || (!IS_SET(ch->act,ACT_AGGRESSIVE)
        || !IS_SET(room->room_flags,ROOM_LAW)))) break;
  }
  return room;
}

// RT Enter portals
void do_enter( CHAR_DATA *ch, const char *argument)
{
  ROOM_INDEX_DATA *location, *old_room;
  OBJ_DATA *portal;
  CHAR_DATA *fch, *fch_next;
  bool local=FALSE;

  if ( ch->fighting != NULL )
  {
    stc("Ты сначала должен перестать драться.\n\r",ch);
    return;
  }

  // nifty portal stuff
  if (EMPTY(argument))
  {
    stc("Куда войти?\n\r",ch);
    return;
  }

  old_room = ch->in_room;
  portal = get_obj_list( ch, argument,  ch->in_room->contents );
  if (!portal)
  {
    portal=get_obj_here(ch,argument);
    local=TRUE;
  }

  if (portal == NULL)
  {
    stc("Ты не видишь здесь этого.\n\r",ch);
    return;
  }

  if (portal->item_type != ITEM_PORTAL || (IS_SET(portal->value[1],EX_CLOSED)
       && !IS_TRUSTED(ch,ANGEL)))
  {
    stc("Ты не видишь пути туда.\n\r",ch);
    return;
  }

  if (!IS_TRUSTED(ch,ANGEL) && !IS_SET(portal->value[2],GATE_NOCURSE)
   && (IS_AFFECTED(ch,AFF_CURSE) || IS_SET(ch->in_room->ra,RAFF_EVIL_PR) 
   || (IS_SET(old_room->room_flags,ROOM_NO_RECALL)
   && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR))))
  {
    stc("Что-то не пускает тебя...\n\r",ch);
    return;
  }

  if (IS_SET(portal->value[2],GATE_RANDOM) || portal->value[3] == -1)
  {
    location = get_random_room(ch);
    portal->value[3] = location->vnum; /* for record keeping :) */
  }
  else if (IS_SET(portal->value[2],GATE_BUGGY) && (number_percent() < 5))
       location = get_random_room(ch);
  else  location = get_room_index(portal->value[3]);

  if (location == NULL || location == old_room || !can_see_room(ch,location)
  || (room_is_private(location) && !IS_TRUSTED(ch,IMPLEMENTOR)))
  {
    act("$i1, похоже, никуда не ведет.",ch,portal,NULL,TO_CHAR);
    return;
  }

  if (IS_NPC(ch) && (IS_SET(location->room_flags,ROOM_NO_MOB) 
      || (IS_SET(ch->act,ACT_AGGRESSIVE) 
      && IS_SET(location->room_flags,ROOM_LAW)))) 
  {
    stc("Что-то не пускает тебя...\n\r",ch);
    return;
  }

  act("$n входит в $i1.",ch,portal,NULL,TO_ROOM);
  if (IS_SET(portal->value[2],GATE_NORMAL_EXIT))
       act("Ты входишь в $i1.",ch,portal,NULL,TO_CHAR);
  else act("Ты входишь в $i1 и оказываешься где-то в...", ch,portal,NULL,TO_CHAR);

  char_from_room(ch);
  char_to_room(ch, location);
 
  if (IS_SET(portal->value[2],GATE_GOWITH) && !local)
  {
   obj_from_room(portal);
   obj_to_room(portal,location);
  }
 
  if (IS_SET(portal->value[2],GATE_NORMAL_EXIT))
        act("$n появляется в комнате.",ch,portal,NULL,TO_ROOM);
  else   act("$n проходит через $i1 и оказывается в комнате.",ch,portal,NULL,TO_ROOM);
 
  do_function(ch, &do_look, "auto");
  // If someone is following the char, these triggers get activated
  // for the followers before the char, but it's safer this way...
  if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
    mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_ENTRY );
  if ( !IS_NPC( ch ) ) mp_greet_trigger( ch );
 
  // charges (-1 - no charges) (0 = unlimited!)
  if (portal->value[0] > 0)
  {
    portal->value[0]--;
    if (portal->value[0] == 0) portal->value[0]--;
  }
 
  // protect against circular follows
  if (old_room == location) return;
 
  for ( fch = old_room->people; fch != NULL; fch = fch_next )
  {
   fch_next = fch->next_in_room;
 
   if (portal == NULL || portal->value[0] == -1) continue;
 
   if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM)
     && fch->position < POS_STANDING) do_function(fch, &do_stand, "");
 
   if ( fch->master == ch && fch->position == POS_STANDING)
   {
    if (IS_SET(ch->in_room->room_flags,ROOM_NO_MOB) && IS_NPC(fch))
     {
      act("Ты не можешь провести $C4 с собой.",ch,NULL,fch,TO_CHAR);
      act("Тебе туда нельзя.",fch,NULL,NULL,TO_CHAR);
      continue;
     }
    if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
      &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
     {
      act("Ты не можешь провести $C4 в город.",ch,NULL,fch,TO_CHAR);
      act("Тебе туда нельзя.",fch,NULL,NULL,TO_CHAR);
      continue;
     }
    act( "Ты следуешь за $N.", fch, NULL, ch, TO_CHAR );
    do_function(fch, &do_enter, argument);
   }
  }
 
  if (portal != NULL && portal->value[0] == -1)
  {
    act("$i1{Y исчезает в {Dмежизмеренческом вихре.{x",ch,portal,NULL,TO_CHAR);
    if (ch->in_room == old_room) act("$i1 {Yисчезает в {Dмощном вихре.{x",ch,portal,NULL,TO_ROOM);
    else if (old_room->people != NULL)
    {
      act("$i1 {Yисчезает в {Dмощном вихре.{x" ,old_room->people,portal,NULL,TO_CHAR);
      act("$i1 {Yисчезает в {Dмощном вихре.{x" ,old_room->people,portal,NULL,TO_ROOM);
    }
    extract_obj(portal);
  }
}

void do_scan(CHAR_DATA *ch, const char *argument)
{
  extern char *const dname[];
  char arg1[MAX_INPUT_LENGTH];
  int dir=100;

  argument = one_argument(argument, arg1);

  if (!str_prefix(arg1, "north")) dir=0;
  if (!str_prefix(arg1, "east"))  dir=1;
  if (!str_prefix(arg1, "south")) dir=2;
  if (!str_prefix(arg1, "west"))  dir=3;
  if (!str_prefix(arg1, "up" ))   dir=4;
  if (!str_prefix(arg1, "down"))  dir=5;
  if (EMPTY(arg1)) dir=100;

  if (ch->invis_level < LEVEL_HERO)
  {
    if(dir==100)
    {
      act("$c1 оглядывается по сторонам.", ch, NULL, NULL, TO_ROOM);
      stc("Ты оглядываешься по сторонам, и видишь:\n\r", ch);
    }
    else
    {
      do_printf(arg1,"%s пристально смотрит на %s.",ch->name,dname[dir]);
      act(arg1,ch,NULL,NULL,TO_ROOM);
      ptc(ch,"\n\rТы пристально смотришь на %s и видишь:\n\r",dname[dir]);
    }
  }
  scan(ch, dir);
}

void do_sprecall( CHAR_DATA *ch, const char *argument )
{
  int chance;

  if (IS_NPC(ch))
  {
    stc("Только игpоки могут делать recall.\n\r",ch);
    return;
  }

  act( "{y    $c1{x пpосит Богов пеpенести его!", ch, 0, 0, TO_ROOM );

  if (EMPTY(ch->pcdata->marry) && ch->pcdata->proom==0)
  {
    stc( "Боги не услышали тебя.\n\r", ch );
    return;
  }

 // Lina and Saboteur
 if ((!ch->in_room || (ch->in_room->sector_type!=SECT_CITY
      && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR))) && !IS_IMMORTAL(ch))

 {
   stc("Путь домой открыт только из города.\n\r", ch);
   return;
 }

  if ( !IS_IMMORTAL(ch) && !IS_SET(ch->in_room->ra, RAFF_LIFE_STR)
      && (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) 
      ||  IS_AFFECTED(ch, AFF_CURSE)
      ||  IS_SET(ch->in_room->ra,RAFF_EVIL_PR)
      ||  (!IS_NPC(ch) && IS_SET(ch->act, PLR_ARMY))))
  {
    stc( "Боги не услышали тебя.\n\r", ch );
    return;
  }

  if (ch->move<2)
  {
    stc("Ты слишком устал.\n\r",ch);
    return;
  }

  chance = get_skill(ch, skill_lookup("recall"));
  if ( number_percent() > chance )
  {
   stc( "Никакой реакции от Богов...\n\r", ch );
   WAIT_STATE( ch, skill_table[gsn_recall].beats );
   return;
  }

  if ( ch->fighting != NULL && !IS_IMMORTAL(ch) )
  {
    if ( number_range(1,50+ch->daze*2) > 25)
    {
      stc( "Hеудача.\n\r", ch );
      return;
    }
    ch->exp-=100;
    stop_fighting(ch, TRUE);
    stc ("Боги переносят тебя из битвы! Ты теряешь 100 опыта.\n\r",ch);
  }

  ch->move /= 2;
  act( "{y$c1{x исчезает.", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, get_room_index(ch->pcdata->proom));
  act( "{y$c1{x появляется в комнате.", ch, NULL, NULL, TO_ROOM );
  do_function(ch, &do_look, "auto" );
    
  if (ch->pet) do_function(ch->pet,&do_recall,(EMPTY(argument))?"":"auto");
}

void do_push( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int dir, moves, weight, str_ap;
  ROOM_INDEX_DATA *to_room;
  bool hard=FALSE;

  argument=one_argument(argument,arg);
  if (EMPTY(arg))
  {
    stc("Что ты хочешь подвинуть?\n\r",ch);
    return;
  }

  if (EMPTY(argument))
  {
    stc("Куда ты хочешь подвинуть?\n\r",ch);
    return;
  }

  obj=get_obj_list(ch,arg,ch->in_room->contents);
  if (!obj)
  {
    stc("Ты не видишь здесь этого.\n\r",ch);
    return;
  }

  if (!CAN_WEAR(obj,ITEM_TAKE))
  {
    stc("Ты не можешь это сдвинуть с места.\n\r",ch);
    return;
  }

       if (!str_prefix(argument,"up"))    dir=DIR_UP;
  else if (!str_prefix(argument,"down"))  dir=DIR_DOWN;
  else if (!str_prefix(argument,"west"))  dir=DIR_WEST;
  else if (!str_prefix(argument,"east"))  dir=DIR_EAST;
  else if (!str_prefix(argument,"south")) dir=DIR_SOUTH;
  else if (!str_prefix(argument,"north")) dir=DIR_NORTH;
  else
  {
    stc("Куда-куда??\n\r",ch);
    return;
  }
  if ( !ch->in_room->exit[dir]
    || !(to_room=(ch->in_room->exit[dir]->u1.to_room))
    || !can_see_room(ch,to_room))
  {
    stc( "К сожалению, ты не видишь дороги в этом направлении.\n\r", ch );
    return;
  }

  if (IS_SET(ch->in_room->exit[dir]->exit_info, EX_CLOSED))
  {
    stc("Здесь закрыто.\n\r",ch);
    return;
  }

  weight=get_obj_weight(obj);
  str_ap=str_app[get_curr_stat(ch,STAT_STR)].wield;

  if (weight > str_ap*15)
  {
    stc("У тебя нехватит сил сдвинуть это с места.\n\r",ch);
    return;
  }

  if (weight > str_ap*10) hard=TRUE;

  moves=URANGE(5,obj->level - ch->level,100);       // object level 
  moves+=movement_loss[ch->in_room->sector_type]*2; // sector_type
  if (hard) moves+=weight-str_ap;                   // weight
  if (dir==DIR_UP)   moves*=2;                      // direction up
  if (dir==DIR_DOWN) moves/=2;                      // direction down 

  if (ch->move < moves)
  {
    stc("Ты слишком устал.\n\r",ch);
    return;
  }

  WAIT_STATE( ch, hard?3:1);

  act("Ты пинаешь $i4 и $i1 улетает $T.",ch,obj,dir_name2[dir],TO_CHAR);
  act("$c1 пинает $i4 и $i1 улетает $T.",ch,obj,dir_name2[dir],TO_ROOM);

  obj_from_room(obj);
  obj_to_room(obj,to_room);

  if (obj->morph_name)
  {
    act("$c1 пинает тебя и ты улетаешь $t.",
      ch,dir_name2[dir],obj->morph_name, TO_VICT);
    do_function(obj->morph_name,&do_look,"auto");
  }

  if (to_room->people)
    act("Неожиданно сюда влетает $i1.",to_room->people,obj,NULL,TO_ALL_IN_ROOM);
}

