// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <time.h>
#include <ctype.h> 
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include "merc.h" 
#include "tables.h"
#include "interp.h"
#include "recycle.h"

void load_deities       ( );
void save_deities       ( );
void panish_effect      ( CHAR_DATA *ch);
void dec_worship        ( CHAR_DATA *ch);
void show_deity_applies ( CHAR_DATA *ch, int deity);

void save_deities()
{
  FILE * fp;
  int counter = 0, i, deity;

  fclose (fpReserve);
  if( (fp = fopen(DEITY_FILE, "w")) == NULL )
  {
    bug ("Write permission denied to 'DEITY_FILE'.", 0);
    fpReserve = fopen (NULL_FILE, "r");
    return;
  }
 
  do_fprintf(fp, "Version 2\n");
  for ( deity = 0;deity<MAX_DEITIES; deity++)
  {
    if ( !deity_table[deity].name) break;
    do_fprintf(fp, "#Deity\n");
    do_fprintf(fp, "Name %s~\n", deity_table[deity].name);
    do_fprintf(fp, "Russian %s~\n", deity_table[deity].russian);
    do_fprintf(fp, "MinAlign %d\n", deity_table[deity].min_align);
    do_fprintf(fp, "MaxAlign %d\n", deity_table[deity].max_align);
    do_fprintf(fp, "Descr %s~\n", deity_table[deity].descr);
    do_fprintf(fp, "Worship %d\n", deity_table[deity].worship);
    do_fprintf(fp, "App");
    for( i = 0; i < MAX_DEITY_APP; i++)
      do_fprintf(fp," %d", deity_table[deity].d_apply[i] );
    do_fprintf(fp,"\nEND\n");
    counter++;
  }
  do_fprintf(fp,"#END\n");
  do_fprintf(fp,"; Detected and saved %d deities.\n", counter);
  log_printf("Saved %d deities.", counter);
  fclose  (fp);

  fpReserve = fopen (NULL_FILE, "r");
}

int get_vacant_deity()
{
  int v_deity;
  for( v_deity = 0; v_deity < MAX_DEITIES ; v_deity++)
  {
    if( deity_table[v_deity].name == NULL && deity_table[v_deity+1].name == NULL ) 
      return v_deity;
  }
  bug("Error in deity data file!", 0);
  return -1;
}

bool may_devote( CHAR_DATA *ch, int dn)
{
  if( IS_NPC(ch) ) return FALSE;
  switch (ch->race)
  {
     case RACE_HUMAN:
     case RACE_UNIQUE:
       break;
     case RACE_ELF:
       if( dn != 0  && dn != 1  && dn != 4 && dn != 9 && dn != 10 
        && dn != 13 && dn != 15 && dn != 16 ) return FALSE;
       break;
     case RACE_DWARF:
       if( dn != 4 && dn != 5  && dn != 6 && dn != 7 && dn != 8  
        && dn != 9 && dn !=11 && dn != 12 && dn != 17) return FALSE;
       break;
     case RACE_GIANT:
       if( dn != 6 && dn != 7 && dn != 8 && dn != 11 && dn != 12 )
           return FALSE;
       break;
     case RACE_VAMPIRE:
       if( dn != 2  && dn != 3 && dn != 5  && dn != 6  && dn != 8 && dn != 14 )
           return FALSE;
       break;
     case RACE_OGRE:
       if( dn != 2  && dn !=  3 && dn != 7 && dn != 8 && dn != 14
        && dn != 17 ) return FALSE;
       break;
     case RACE_HOBBIT:
       if( dn != 3 &&  dn != 9 &&  dn != 13 ) return FALSE;
       break;
     case RACE_ORC:
       if( dn !=  2 && dn != 6 && dn != 7 && dn != 8 && dn != 12 )
           return FALSE;
       break;
     case RACE_DROW:
       if( dn !=  2 && dn != 3  && dn != 5 && dn != 8
        && dn != 13 && dn != 14 && dn != 17 ) 
           return FALSE;
       break;
     case RACE_CENTAUR:
       if( dn != 0 && dn != 1 && dn !=  3 && dn !=  4 && dn != 7 
        && dn != 8 && dn != 9 && dn != 10 && dn != 12 ) return FALSE;
       break;
     case RACE_HGRYPHON:
       if( dn != 0  && dn !=  1 && dn != 2  && dn != 7  && dn != 8 && dn != 9 
        && dn != 12 && dn != 15 && dn !=16  && dn != 17 ) return FALSE;
       break;
     case RACE_LIZARD:
       if( dn !=  1 && dn != 3  && dn != 7 && dn != 9 && dn != 14 && dn != 16)
           return FALSE;
       break;
     case RACE_ETHERIAL:
       if( dn !=  0 && dn != 2  && dn != 4  && dn != 6 && dn != 9 && dn != 14
        && dn != 15 && dn != 16 && dn != 17 ) return FALSE;
       break;
     case RACE_SPRITE:
       if( dn != 1  && dn != 2  && dn != 4  && dn != 5 && dn != 6 && dn != 9 
        && dn != 10 && dn != 13 && dn != 14 && dn != 15 && dn != 16 && dn != 17 ) 
           return FALSE;
       break;
     case RACE_DRUID:
       if( dn != 0  && dn != 1  && dn != 4 && dn != 9 && dn != 10 && dn != 15
        && dn != 16 && dn != 17 ) return FALSE;
       break;
     case RACE_SKELETON:
       if( dn != 2  && dn != 5 && dn != 6 && dn != 7 && dn != 8 && dn != 14 ) 
           return FALSE;
     case RACE_ZOMBIE:
       if( dn != 2  && dn != 5 && dn != 8 && dn != 12 && dn != 14 ) return FALSE;
       break;

  }
  if( deity_table[dn].min_align > ch->alignment
   || deity_table[dn].max_align < ch->alignment ) return FALSE;

  return TRUE;
};

void dec_worship( CHAR_DATA *ch)
{
  if( !IS_DEVOTED_ANY(ch) ) return;
  deity_table[ch->pcdata->dn].worship -= 1;
  if( deity_table[ch->pcdata->dn].worship < 0 )
    deity_table[ch->pcdata->dn].worship = 0;
};

bool favour( CHAR_DATA *ch, int dvalue )
{
  int fcount;

  if( !IS_DEVOTED_ANY(ch) ) return FALSE;
  if( IS_ELDER(ch) ) return TRUE;
  if( IS_SET( ch->act, PLR_HIGHPRIEST ) ) dvalue /= 2;
  else
   for( fcount=0; t_favour[fcount].fav_afstr; fcount++)
     if( (ch->pcdata->favour >= t_favour[fcount].from) && (ch->pcdata->favour <= t_favour[fcount].to) )
     {
       dvalue *= t_favour[fcount].amp;
       break;
     }
  if( dvalue < 0 && (ch->pcdata->favour + dvalue >= -50) )
  {
    ch->pcdata->favour += dvalue;
    return TRUE;
  }
  else if( dvalue > 0 )
  {
    ch->pcdata->favour += dvalue;
    return TRUE;
  }
  ptc( ch, "{C%s{x не слышит твоих молитв...\n\r", get_rdeity( deity_table[ch->pcdata->dn].russian, '1') );
  return FALSE;
}

void change_favour( CHAR_DATA *ch, int cvalue)
{
//  if( !IS_DEVOTED_ANY(ch) ) return;
  if( cvalue < 0 && IS_SET( ch->act, PLR_HIGHPRIEST ) ) cvalue /= 2;
//  if( number_range(1,3) > 2 - (IS_SET( ch->act, PLR_HIGHPRIEST )) ) return;
  ch->pcdata->favour += cvalue;
  if( ch->pcdata->favour < -5000 ) ch->pcdata->favour = -5000;
  if( ch->pcdata->favour >  5000 ) ch->pcdata->favour =  5000;
};

int favour_string( CHAR_DATA *ch)
{
 int fcount;

 for( fcount=0; t_favour[fcount].fav_afstr; fcount++)
   if( (ch->pcdata->favour >= t_favour[fcount].from) && (ch->pcdata->favour <= t_favour[fcount].to) )
     return fcount;
 return MIDDLE_FAVOUR;
};

#define dtab deity_table[ch->pcdata->dn]
int deity_char_power( CHAR_DATA *ch, int type, int subtype)
{
  int sum=0;

  if( !IS_DEVOTED_ANY(ch) ) return sum;
  switch(type)
  {
    case 1:
      if( dtab.d_apply[0] == subtype ) sum += dtab.d_apply[1];
      if( dtab.d_apply[2] == subtype ) sum -= dtab.d_apply[3];
      break;
    case 2:
      if( dtab.d_apply[4] == subtype ) sum += dtab.d_apply[5];
      if( dtab.d_apply[6] == subtype ) sum -= dtab.d_apply[7];
      break;
    case 3:
/*
      if( str_cmp( deity_apply_table[dtab.d_apply[8]].res_flag,
          deity_apply_table[dtab.d_apply[10]].res_flag ) )
      {
        if( deity_apply_table[dtab.d_apply[8]].res_flag )
        {
           do_printf( buf," char %s resist %s", ch->name, 
            deity_apply_table[dtab.d_apply[8]].res_flag );
          do_function( ch, &do_flag, buf);
          free_buf(buf);
        }
        if( deity_apply_table[dtab.d_apply[10]].res_flag )
        {
          do_printf( buf,"char %s vuln %s", ch->name,
            deity_apply_table[dtab.d_apply[10]].res_flag );
          do_function( ch, &do_flag, buf);
        }
      }
*/
    default:
      break;
  }
  return sum;
};
#undef dtab

void show_deity_applies( CHAR_DATA *ch, int deity)
{
  int number;
  char *incdec;

  if( ch == NULL ) return;

  for( number=0; number < MAX_DEITY_APP; number +=2 )
  {
    if( number % 4 == 0) incdec = "{GУвеличивает{x";
    else incdec = "{RУменьшает{x";
    if( deity_table[deity].d_apply[number] == 0 ) continue;
    else 
     switch(number)
     {
       case 0 :
       case 2 :
         if( deity_apply_table[deity_table[deity].d_apply[number]].param )
         {
           ptc( ch,"%s {m%s{x на {Y%d{x.\n\r", incdec,
             deity_apply_table[deity_table[deity].d_apply[number]].param,
             deity_table[deity].d_apply[number+1] );
         }
         break;
       case 4 :
       case 6 :
         if( deity_apply_table[deity_table[deity].d_apply[number]].inform )
         {
           ptc( ch,"%s знание {m%s{x на {Y%d{x.\n\r", incdec,
             deity_apply_table[deity_table[deity].d_apply[number]].inform,
             deity_table[deity].d_apply[number+1] );
         }
         break;
       case 8 :
       case 10:
         if( deity_apply_table[deity_table[deity].d_apply[number]].resist )
         {
           ptc( ch,"%s защиту от {m%s{x на {Y%d{x.\n\r", incdec,
             deity_apply_table[deity_table[deity].d_apply[number]].resist,
             deity_table[deity].d_apply[number+1] );
         }
         break;
       case 12:
       case 14:
       case 16:
       case 18:
       default:
         break;
     }
  }
};

void punish_effect( CHAR_DATA *ch )
{
  if( ch && ch->deity)
  {
    ch->pcdata->dc = 1000;
    ch->pcdata->dr = 0;
    ch->godcurse = 5;
    SET_BIT(ch->act, PLR_DISAVOWED);
    stc( "Мир пред твоими глазами теряет краски...\n\r", ch );
    act("Боги проклинают $c1.",ch,NULL,NULL,TO_ROOM);
    save_char_obj(ch);
  }
  WAIT_STATE( ch, 3 * PULSE_VIOLENCE);
};

void do_gfix( CHAR_DATA *ch, const char *argument)
{
  int gvalue, gdef, i;

  if( EMPTY(argument) || !str_cmp( argument, "help") )
  {
    stc("{cGlobal fix command for defined conception.\n\r", ch);
    stc("{cThis command is quiet {Rdangerouse{c. Use it wisely!\n\r", ch);
    stc("{cUsage:\n\r", ch);
    stc("{G  gfix <parameter>\n\r", ch);
    stc("{cAvailiable parameters:{G clearworships clearapplies{x.\n\r", ch);
    return;
  }

  if( !IS_ELDER(ch) )
  {
    stc("{RYou have no access to this command.{x\n\r", ch);
  }

  if( !IS_SET(global_cfg, CFG_GTFIX) )
  {
    stc("GTFix global configuration has to be toggled 'on' to use this option.\n\r", ch);
    return;
  }

  if( !str_cmp( argument,"clearworships") )
  {
    gdef = gvd();
    if (gdef==-1)
    {
      stc("Error - Max Deities reached.\n\r",ch);
      return;
    }
    for( gvalue=0; gvalue < gdef; gvalue++)
      deity_table[gvalue].worship=0;
    stc("Путем загрузки файла {Rможна отменить обнуление{x последователей deity{x.\n\r", ch);
    REM_BIT( global_cfg, CFG_GTFIX);
    return;
  }

  if( !str_cmp( argument,"clearapplies") )
  {
    gdef = gvd();
    if (gdef==-1)
    {
      stc("Error - max Deities reached.\n\r",ch);
      return;
    }
    for( gvalue=0; gvalue < gdef; gvalue++)
      for( i=0; i < MAX_DEITY_APP; i++)
        deity_table[gvalue].d_apply[i] = 0;
    stc("Путем загрузки файла {Rможна отменить обнуление{x эффектов deity{x.\n\r", ch);
    REM_BIT( global_cfg, CFG_GTFIX);
    return;
  }
  stc("Type 'gfix help' for extra information.\n\r", ch);
};

void do_cfix( CHAR_DATA *ch, const char *argument)
{
  int dtemp;

  if( ch == NULL ) return;

//  if( IS_SET(ch->act, PLR_HIGHPRIEST) ) ch->pcdata->favour = 1000;

  if( ch->version > 10 || ch->version < 1 ) ch->version = 1;
  if( ch->version < 5) 
  {
    ch->long_descr = str_dup(ch->name);
    if( IS_IMMORTAL(ch) && ch->pcdata->pseudoname == NULL )
      do_function( ch, &do_pseudoname, "clean");
    ch->pcdata->protect = 0;
    if( IS_DEVOTED_ANY(ch) )
    {
      free_string(ch->deity);
      ch->deity = NULL;
      ch->pcdata->dn = 25;
      ch->pcdata->dc = 0;
      ch->pcdata->dr = 0;
      ch->pcdata->carma = 0;
      ch->pcdata->favour = 0;
    }
    ch->version = 5;
    save_char_obj(ch);
    return;
  }
  else if( ch->version >= 5 )
  {
    dtemp = gvd();
    if (dtemp==-1)
    {
      stc("Error - max Deities reached.\n\r",ch);
      return;
    }
    if( ch->pcdata->dn >= dtemp ) ch->deity = NULL;
    else ch->deity = str_dup(deity_table[ch->pcdata->dn].name);
    return;
  }
  save_char_obj(ch);
  if( IS_ELDER(ch) ) stc("Fixed.\n\r", ch);
}

void do_polyanarecall( CHAR_DATA *ch, const char *argument)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];

  if( EMPTY(argument) || !str_cmp(argument,"help") )
  {
    stc("{cКоманда 'звания' и 'ухода'(с бонус-параметром 'out') на поляну.\n\r", ch);
    stc("{cCинтаксис:\n\r", ch);
    stc("{G   polyanarecall <parameter>\n\r", ch);
    stc("{GActual parameters: help, beer, pepsi, talk{x\n\r", ch);
    stc("{GBonus parameters : out\n\r", ch);
    stc("{G (polyanarecall pepsi out){x\n\r", ch);
    return;
  }

  argument = one_argument( argument, arg);

  if( ch->fighting != NULL || ch->pcdata->condition[COND_ADRENOLIN] > 0 )
  {
    stc( "Отдышись, успокойся, а потом поляна... :)\n\r", ch);
    return;
  }

  if (EMPTY(arg)) return;
  else if( !str_prefix(arg,"beer") )
    do_printf(buf, "{RP{Wo{RL{Wy{RA{Wn{RA{Wr{RE{Wc{RA{Wl{RL {B!{G!{B! {DPiFfO {BBeEr {mCeRvEsA {RBiEr {G!{Y!{W!{x");
  else if( !str_prefix(arg,"pepsi") )
    do_printf(buf, "{W!{R!{W! {YG{Go{YT{Go{YP{Go{YL{Gy{YA{Gn{YA {RP{BE{RP{BS{RI {D-d.r.i.n.k.i.n.g {W!{R!{W!{x");
  else if( !str_prefix(arg,"talk") )
    do_printf( buf, "{DНу пойдемте, други, посидим у мыслях...{x");
  else do_printf( buf, "\0");

  if( EMPTY(buf) )
  {
    ptc( ch, "Неизвестный аргумент %s!\n\r", argument);
    stc( "Набери 'polyanarecall help' для справки :)\n\r", ch);
    return;
  }

  for( d = descriptor_list; d; d = d->next )
  {
    if ( !d->character || d->connected!=CON_PLAYING) continue;
    ptc( d->character, "{Y%s{x без заднiх мыслiв предлагаiть товариству: %s\n\r",PERS(d->character,ch), buf);
  }

  if( !IS_ELDER(ch) ) WAIT_STATE( ch, 4);

  if( !str_cmp( argument,"out") )
  {
    save_char_obj( ch);
    do_function( ch, &do_quit, "");
  }
  return;
}

void do_repair( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *victim;
  OBJ_DATA *robj=NULL;
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
  int64 g_delta;

  argument=one_argument( argument, arg1 );
  argument=one_argument( argument, arg2 );

  if( EMPTY(arg1) || !str_prefix( arg1, "help" ) )
  {
    stc("{cКоманда починки предметов вооружения и брони.", ch);
    stc("{cЧинить могут только мастера в комнате-кузнице.\n\r", ch);
    stc("{cПри починке другому деньги сразу платятся кузнецу.\n\r", ch);
    stc("{cПочинка себе - бесплатна.\n\r", ch);
    stc("{cСинтаксис:\n\r", ch);
    stc("{G  repair [help]               - Эта справка;{x\n\r", ch);
    stc("{G  repair <object> [self]      - Починить предмет у себя в инвентаре{x\n\r", ch);
    stc("{G  repair <object> <char>      - Починить предмет в инвентаре у другого{x\n\r", ch);
    stc("{G  repair <object> <mob>       - Починить предмет у моба-кузнеца в комнате{x\n\r", ch);
    stc("{G  repair <object> <mob> value - Поинтересоваться стоимостью починки вещи.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->in_room->room_flags,ROOM_RFORGE) && !IS_IMMORTAL(ch) )
  {
    stc("Сначала зайди в кузницу.\n\r", ch);
    return;
  }
  if( EMPTY(arg2) || !str_cmp(arg2,"self") ) victim=ch; 
  else victim=get_char_room(ch, arg2);
  if (!victim)
  {
    stc("Тут нет таких.\n\r", ch);
    return;
  }

  if( IS_NPC(victim) )
  {
    if( IS_SET(victim->act, ACT_FORGER) )
    {
      if ( !(robj = get_obj_carry( ch, arg1, ch)) )
      {
        stc( "У тебя нет этой вещи.\n\r", ch );
        return;
      }
      if( !IS_ELDER(ch) ) switch ( robj->item_type )
      {
        case ITEM_WEAPON:
        case ITEM_ARMOR:
        case ITEM_WAND:
        case ITEM_STAFF:
          break;
        default:
          stc("Чинить можна только оружие, вещи с броней, жезлы и посохи.\n\r",ch);
          return;
      }
      if( robj->durability == 0 ) robj->durability = material_table[material_num(robj->material)].d_dam;
      if( robj->durability == -1 || (robj->condition)*100/((robj->durability < 1)?1:robj->durability) >= 100) 
      {
        ptc(ch,"Эта вещь не нуждается в починке...\n\r");
        return;
      }

      if( (g_delta = ( robj->durability - robj->condition) * robj->cost / ((robj->durability==-1)?1:(robj->durability)) * victim->level / 10000 ) > ch->gold )
      {
        ptc(ch,"У тебя не хватит денег на починку %s!\n\r", get_obj_desc(robj,'2') );
        return;
      }
      
      if( !str_prefix( argument, "value") && !EMPTY(argument) )
      {
        if( g_delta == 0) ptc( ch, "%s произносит: '{GЯ починю тебе {C%s {Gнашару!{x'\n\r", get_obj_desc( robj, '2'));
        else
        {
          do_printf( buf1, "{GПочинка {C%s {Gбудет стоить {Y%d{G золотых монет.{x",
             get_obj_desc( robj, '2'),
             g_delta );
          do_function( victim, &do_say, buf1);
        }
        return;
      }
      else
      {
        robj->condition = robj->durability;
        ptc(ch, "%s {Gчинит для тебя {C%s{x за {Y%d {Gзолотых монет.{x\n\r",
            get_char_desc( victim, '1'),
            get_obj_desc( robj, '4'),
            g_delta );                      
        ch->gold -= g_delta;
      }
      ptc(ch, "Теперь {C%s {xв %s состоянии.\n\r",
          get_obj_desc( robj, '1'),
          get_obj_cond( robj, 1 ) );

      do_printf(buf, "{Y$c1{x ремонтирует {C%s{x для {Y$C2{x.",
          get_obj_desc(robj,'4') );
      act(buf, victim, NULL, ch, TO_NOTVICT);
      if (!IS_IMMORTAL(ch)) WAIT_STATE(ch,3*PULSE_VIOLENCE);
      return;
    }
    else
    { 
      stc("Сначала найди кузнеца!\n\r",ch);
      return; 
    }
  }
  else if( !IS_SET( race_table[ch->race].spec, SPEC_BLACKSMITH) && !IS_IMMORTAL(ch) )
  {
    stc( "Ты не владеешь кузнечным делом!\n\r", ch);
    return;
  }

  if( !(robj=get_obj_carry( victim, arg1, ch)) )
  {
    stc( "Ты не нашел этого.\n\r", ch );
    return;
  }

  if( !IS_ELDER(ch) ) switch ( robj->item_type )
  {
    case ITEM_WEAPON:
    case ITEM_ARMOR:
    case ITEM_WAND:
    case ITEM_STAFF:
      break;
    default:
      stc("Чинить можна только оружие, вещи с броней, жезлы и посохи.\n\r",ch);
      return;
  }
  if( robj->durability == 0 ) robj->durability = material_table[material_num(robj->material)].d_dam;
  if( robj->durability == -1 || (robj->condition)*100/((robj->durability < 1)?1:robj->durability) >= 100) 
  {
    ptc(ch,"Эта вещь не нуждается в починке...\n\r");
    return;
  }

  if( victim != ch && (victim->gold < (g_delta = (robj->durability - robj->condition) * robj->cost*victim->level / 80 / (robj->durability + 2))) )
  {
    stc( "У клиента не хватит денег на оплату твоей работы...\n\r", ch );
    ptc( victim, "У тебя не хватит денег на оплату работы %s...\n\r", ch->name);
    return;
  }

/*
  if( !str_prefix( argument, "value") && !IS_NPC(victim) )
  {
    do_printf( buf1,"{GПочинка %s будет стоить {Y%d{x золотых монет.{x\n\r", 
       get_obj_desc( robj,'2'),
       g_delta );
    do_function( victim, &do_say, buf1);
    return;
  }
*/

  if( victim != ch)
  {
    ptc(ch, "%s {Gчинит для тебя {C%s {Gза {Y%d {Gзолотых монет.{x\n\r{GТеперь {C%s {Gв %s {Gсостоянии.{x\n\r",
         get_char_desc( victim, '1'),
         get_obj_desc( robj, '4'),
         g_delta,
         get_obj_desc( robj, '1'),
         get_obj_cond( robj, 1 ) );  

    victim->gold -= g_delta;
    ch->gold += g_delta;
  }

  robj->condition = robj->durability; 
  do_printf(buf, "{Y$c1{x ремонтирует для {Y%s{x {y%s{x.",
    (ch==victim)?"себя":get_char_desc(victim, 2), get_obj_desc(robj,'4'));
  act(buf, ch, NULL, victim, TO_NOTVICT);
  if( robj->condition > robj->durability) robj->condition = robj->durability;
  ptc( ch, "Ты чинишь {C%s{x%s%s.\n\rТеперь эта вещь в %s состоянии.\n\r",
      get_obj_desc(robj, '4'), (ch==victim)?"":" для ",
      (ch==victim)?"":get_char_desc(victim, '2'),
      get_obj_cond( robj, 1) );
  if (!IS_IMMORTAL(ch)) WAIT_STATE(ch,2*PULSE_VIOLENCE);
}

void do_reward( CHAR_DATA *ch, const char *argument )
{
 CHAR_DATA *victim;
 char buf[MAX_INPUT_LENGTH];
 char arg1[MAX_STRING_LENGTH];
 char arg2[MAX_STRING_LENGTH];
 char arg3[MAX_STRING_LENGTH];
 int value;

 argument=one_argument( argument, arg1);
 argument=one_argument( argument, arg2);
 argument=one_argument( argument, arg3);

 if( EMPTY(arg1) || !str_prefix(arg1,"help") )
 {
   stc("{cВыплачивание награды чарам.\n\r",ch);
   stc("{cСинтаксис:{x\n\r",ch);
   stc("{Greward <char_name> <reward_type> <reward_value>\n\r",ch);
   stc("{G  Поля reward_type(указывается полностью): qp, gold, silver\n\r",ch);
   stc("{G  Поле reward_value является числовым значением награды.\n\r",ch);
   return;
 }

 if( !(victim=get_pchar_world(ch,arg1)) )
 {
   stc("Таких в мире нет.\n\r",ch);
   return;
 }

   if(EMPTY(arg3))
   {
     stc("Отсутствует параметр размера награды.\n\r",ch);
     return;
   }

 value=atoi(arg3);

 if( !str_cmp( arg2,"qp") )
 {
   if( value > 5000 || value < 1 )
   {
     stc("Значение награды в квестовых очках может быть от 1 до 5000.\n\r",ch);
     return;
   }
   victim->questpoints += value;
   if ( ch != victim && !IS_NPC(ch) )
   {
     do_printf( buf,"{Y%s{x получил%s от {c%s{x награду в размере {G%d{x квестовых очков.\n\r",victim->name,victim->sex==1?"":"а",ch->name,value);
     if( value) ptc( ch, "{Y%s {xполучает от тебя награду в размере {R%d {c%s{x! (В сумме теперь: {G%d{x).\n\r", get_char_desc( victim, '1'), value, arg2, victim->questpoints);
     ptc( victim, "Ты получаешь в награду от {C%s {G%d{x квестовых очков!\n\r", ch->name, value);
     send_news( buf, NEWS_REWARD);
   }
   return;
 }
 if( !str_cmp( arg2, "gold") )
 {
   if( value > 10000 || value < 1 )
   {
     stc("Значение награды в золотых монетах может быть от 1 до 10000.\n\r",ch);
     return;
   }
   victim->gold += value;
   if( victim != ch && !IS_NPC(ch) ) 
   {
     do_printf( buf,"{Y%s{x получил%s от {c%s{x награду в размере {Y%d{x золотых.",victim->name,victim->sex==1?"":"а",ch->name,value);
     if( victim != NULL && value) ptc( ch, "{Y%s {xполучает от тебя награду в размере {R%d {c%s{x! (В сумме теперь: {w%d{x).\n\r", get_char_desc( victim, '1'), value, arg2, victim->gold);
     ptc( victim, "Ты получаешь в награду от {C%s {Y%d{x золотых монет!\n\r", ch->name, value);
     send_news( buf, NEWS_REWARD);
   }
   return;
 }
 if( !str_cmp( arg2, "silver") )
 {
   if( value > 50000 || value < 1 )
   {
     stc("Значение награды в серебряных монетах может быть от 1 до 50000.\n\r",ch);
     return;
   }
   victim->silver += value;
   if ( ch != victim && !IS_NPC(ch) )
   {
     do_printf( buf,"{Y%s{x получил%s от {c%s{x награду в размере {W%d{x серебряных монет!",victim->name,victim->sex==1?"":"а",ch->name,value);
     if( value) ptc( ch, "{Y%s {xполучает от тебя награду в размере {R%d {c%s{x! (В сумме теперь: {G%d{x).\n\r", get_char_desc( victim, '1'), value, arg2, victim->silver);
     ptc( victim, "Ты получаешь в награду от {C%s {W%d{x серебряных монет! (В сумме теперь: {G%d{x).\n\r", ch->name, value, victim->silver);
     send_news( buf, NEWS_REWARD);
   }
 }

}

void do_damage( CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  int value;

  argument=one_argument( argument, arg1);
  argument=one_argument( argument, arg2);

  if(EMPTY(arg1) || !str_cmp( arg1,"help") )
  {
    stc("{cУхудшает состояния вещи.{x\n\r", ch);
    stc("{cПоиск вещи проводится у чара, если параметр [character] пуст,{x\n\r", ch);
    stc("{cи у другого персонажа (чара, моба), если не пуст.{x\n\r", ch);
    stc("{cСинтаксис:\n\r", ch);
    stc("{G  damage <object_name> <damage_value> [character]{x\n\r", ch);
    return;
  }
  if ( get_trust(ch) < 109) return;

  if ( EMPTY(argument) ) victim=ch;
  else victim = get_char_world( ch, argument);

  if(!victim)
  {
    stc("Таких нет в мире.\n\r",ch);
    return;
  }

  if( (obj=get_obj_wear( victim, arg1)) != NULL );
  else if( !(obj=get_obj_victim( ch, victim, arg1)) )
  {
    stc("Предмет не найден!\n\r", ch);
    return;
  }

  if( !(value=atoi(arg2)) ) value=1;

  if( obj->durability == -1 ) 
  {
    stc("Ты не можешь повредить эту вещь.\n\r", ch);
    return;
  }

  if( obj->condition < 1) 
  {
    stc("Эта вещь и так сломана.\n\r", ch);
    return;
  }
  if( obj->condition < value ) value = obj->condition;
  obj->condition -= value;
  ptc(ch, "Ты повредил {c%s{x у {Y%s{x на {R%d{x единиц. Осталось {R%d{x.\n\r",
  get_obj_desc( obj,'4'), (victim==ch)?"себя":get_char_desc(victim,'2'), value, 
  obj->condition );
}

void do_seize( CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1);
  argument = one_argument( argument, arg2);

  if(EMPTY(arg1) || !str_cmp( arg1,"help") )
  {
    stc("{cЗабрать указаный предмет у персонажа.{x\n\r", ch);
    stc("{cУдобно использовать для конфискации ништяков.{x\n\r", ch);
    stc("{cСинтаксис:\n\r", ch);
    stc("{G  seize <obj_name> <char_name> [quiet]{x\n\r", ch);
    stc("{G     <obj_name>  - название вещи;{x\n\r", ch);
    stc("{G     <char_name> - имя жертвы;{x\n\r", ch);
    if( IS_ELDER(ch) )
    {
      stc("{cElders' Синтаксис:\n\r", ch);
      stc("{G     [quiet]     - бонус-параметр(без сообщения жертве){x\n\r", ch);
    }
    return;
  }

  if( ( victim = get_char_room( ch, arg2)) == NULL ) 
  {
    stc("Этого персонажа нет в комнате!\n\r", ch);
    return;
  }

  if( (obj=get_obj_wear( victim, arg1)) != NULL );
  else if( !(obj=get_obj_victim( ch, victim, arg1)) )
  {
    stc("Предмет с таким именем у персонажа не найден!\n\r", ch);
    return;
  }
  
  if( get_trust(ch) < get_trust(victim) )
  {
     stc("Хамство наказуемо...\n\r", ch);
     ptc( victim,"{R%s{x пытался конфисковать у тебя {R%s{x!\n\r",
          ch->name, get_obj_desc( obj, '4') );
     return;
  }
  if( victim == ch )
  {
    stc("Может лучше просто снять?\n\r", ch);
    return;
  }

  obj_from_char(obj);
  obj_to_char( obj, ch);
  ptc( ch, "Ты конфисковываешь {c%s{x у {Y%s{x.\n\r", 
       get_obj_desc( obj, '4'), get_char_desc( victim, '2') );

  if( !str_cmp(argument,"quiet") && IS_ELDER(ch) && !IS_ELDER(victim) ) return;
  else
  {
    ptc( victim, "{Y%s{x конфисковывает у тебя {c%s{x.\n\r", 
         get_char_desc( ch, '1'), get_obj_desc( obj, '4') );
  }
};

#define dtab deity_table[deity]
void do_devote( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_STRING_LENGTH];
  int deity = 0;

   if( EMPTY(argument) || !str_cmp( argument, "help") )
   {
     stc("{cПосвятить свою жизнь и деяния Божеству.\n\r", ch);
     stc("{cПоследствия отказа от пути признанного Божества могут быть весьма различными.\n\r", ch);
     stc("{cСинтаксис:\n\r", ch);
     stc("{G   devote, devote help    - эта справка\n\r", ch);
     stc("{G   devote list            - список Богов\n\r", ch);
     stc("{G   devote show            - твой статус поклонения\n\r", ch);
     stc("{G   devote stat <deity>    - краткая справка о Божестве\n\r", ch);
     if( !IS_DEITY(ch) )
     stc("{G   devote <deity>         - посвятить себя Божеству\n\r", ch);
//     stc("{G   devote disavow         - сойти с пути посвященного\n\r", ch);
     if( IS_DEITY(ch) )
     {
       stc("{G   devote <char> stat     - посмотреть статистику посвященного\n\r{x", ch);
       stc("{G   devote <char> clear    - сделать персонажа атеистом\n\r{x", ch);
       stc("{G   devote <char> <deity>  - посвятить персонажа Божеству\n\r{x", ch);
     }
     if( IS_CREATOR(ch) )
     {
       stc("{G   devote <char> excuse    - простить грехи персонажа (disavow)\n\r{x", ch);
       stc("{G   devote <char> pardon    - простить грехи персонажа (carma,favour)\n\r{x", ch);
       stc("{G   devote <char> reward    - поощрить за следование канонам (+favour)\n\r{x", ch);
       stc("{G   devote <char> punish    - наказать за неследование канонам (-favour)\n\r{x", ch);
     }
     return;
   }

   if ( !str_prefix( argument, "list") )
   {
     stc("{D Имя Божества {GРусское имя    {cОписание\n\r", ch);
     for( deity = 0; dtab.name && deity<MAX_DEITIES; deity++)
     { 
       ptc( ch, "{C %12s [{Y%12s{C] %s\n\r", dtab.name, get_rdeity( dtab.russian, '1' ), dtab.descr);
     }
     return;
   }

   if( ch->deity) deity = ch->pcdata->dn;

   if( !str_prefix( argument, "show") && !IS_NPC(ch) )
   {
     if( IS_DEITY(ch) )
     {
       ptc( ch, "Ты сам%s являешься Божеством.\n\r", (ch->sex==2)?"а":"");
       return;
     }
     else ptc( ch, "Ты %sпосвящен%s %s.\n\r", (ch->deity)?"":"не ", (ch->sex==2)?"а":"",
             (ch->deity)?get_rdeity(dtab.russian,'3'):"Божеству");
     if( IS_DEVOTED_ANY(ch) ) ptc( ch, "Ты являешься {y%s{W {R%s{x.\n\r",
        ( ch->sex==2)?t_favour[favour_string(ch)].fav_afstr:t_favour[favour_string(ch)].fav_amstr,
        get_rdeity( dtab.russian, '2') );
     return;
   }
/*
   if( !str_cmp( argument, "disavow") && !IS_NPC(ch) )
   {
     if( ch->deity )
     {
       deity = deity_lookup(ch->deity);
       dec_worship(ch);
       if( get_trust(ch) < 102 )
       {
         ptc( ch, "Ты отрекаешься от поклонения %s...\n\r", get_rdeity(dtab.russian,'3') );
         stc("{RТЫ ВЫБРАЛ ЭТО САМ, СМЕРТНЫЙ...{x\n\r", ch);
         WAIT_STATE( ch, 2*PULSE_VIOLENCE);
         ch->pcdata->carma -= 10;
         ch->pcdata->favour = -50;
         punish_effect( ch);
       }
       else ptc( ch, "Ты отрекаешься от покровительства %s...\n\r", get_rdeity(dtab.russian,'2') );
       ch->pcdata->dn = 49;
       free_string(ch->deity);
       ch->deity = NULL;
       save_deities();
       return;
     }
     stc("У тебя нет покровителя!\n\r", ch);
     return;
   }
*/
   argument = one_argument( argument, arg);

   if( !str_cmp(arg,"stat"))
   {
    deity = deity_lookup(argument);
     if( !EMPTY(argument) )
       if( !str_cmp( argument, dtab.name) )
       {
         ptc( ch, "{c%s{x, %s{x.\n\rПризнаваемый характер: от {D%d {xдо {W%d{x.\n\r", get_rdeity( dtab.russian, '1'),
           dtab.descr, dtab.min_align, dtab.max_align );
         show_deity_applies( ch, deity);
         ptc( ch,"{C%s %sсклонен{x принять твое поклонение.{x\n\r",
             get_rdeity( dtab.russian,'1'),
             may_devote(ch,deity)?"{G":"{Rне " );
         return;
       }
     stc("Такого Божества не существует.\n\r", ch);
     return;
   }

   if( ch->deity && !IS_DEITY(ch) )
   {
     ptc( ch, "Ты являешься последовател%s %s... Сначала отрекись от своего божества...\n\r", (ch->sex==2)?"ьницей":"ем", get_rdeity(deity_table[deity_lookup(ch->deity)].russian,'2'));
     return;
   }

    if( !EMPTY(arg) && !IS_DEITY(ch) && !str_cmp( arg, deity_table[deity_lookup(arg)].name) )
    {
      deity = deity_lookup(arg);
      if( !may_devote( ch, deity) )
      {
        ptc( ch, "Ты не удовлетворяешь требованиям %s.\n\r", get_rdeity(dtab.russian,'2') );
        return;
      }

      if( IS_SET(ch->act,PLR_DISAVOWED) )
      {
        stc("Ты проклят Богами...\n\r", ch);
        return;
      }

      free_string(ch->deity);
      arg[0] = UPPER(arg[0]);
      ch->deity = str_dup(arg);
      ch->pcdata->dn = deity_lookup(arg);
      deity_table[deity_lookup(arg)].worship++;
      if( !IS_NPC(ch) )
      {
        ch->pcdata->carma += 5;
        ch->pcdata->favour += 50;
        save_deities();
      }
      ptc(ch, "Ты посвятил%s себя %s.\n\r", (ch->sex==2)?"а":"", get_rdeity(deity_table[deity_lookup(ch->deity)].russian,'3') );
      return;
    }

    if( EMPTY(argument) || EMPTY(arg) ) 
    {
       stc("{cСинтаксис: {Gdevote help {cдля информации.{x\n\r", ch);
       return;
    }

   if( IS_DEITY(ch) )
   {
     CHAR_DATA *victim;

     if( (victim = get_pchar_world( ch, arg)) == NULL )
     {
       stc("В мире нет таких.\n\r", ch);
       return;
     }

     if( victim->deity) deity = victim->pcdata->dn;

     if( !str_cmp( argument, "clear") )
     {
       if( victim->deity )
       {
         dec_worship(victim);
         ptc( ch, "%s теперь атеист%s.\n\r", victim->name, (victim->sex==2)?"ка":"" );
         ptc( victim, "Ты теперь {Rатеист%s{x!\n\r", (victim->sex==2)?"ка":"" );
         victim->pcdata->dn = 25;
         free_string(victim->deity);
         victim->deity = NULL;
         if( !IS_NPC(victim) )
         {
           victim->pcdata->favour = 0;
           save_deities();
         }
         return;
       }
       ptc( ch, "%s не имеет покровителя...\n\r", victim->name );
       return;
     }

     if( !str_cmp( argument, "highpriest") && IS_ELDER(ch) )
     {
       if( !IS_NPC(victim) && victim->deity )
       {
         if( IS_SET( victim->act, PLR_HIGHPRIEST ) ) REM_BIT( victim->act, PLR_HIGHPRIEST );
         else
         {
           SET_BIT( victim->act, PLR_HIGHPRIEST );
           ptc( ch, "%s теперь {W%s {R%s{x!\n\r", victim->name, (victim->sex==2)?t_favour[favour_string(victim)].fav_nfstr:t_favour[favour_string(victim)].fav_nmstr, get_rdeity( deity_table[victim->pcdata->dn].russian,'2'));
           ptc( victim, "Ты теперь {W%s {R%s{x!\n\r", (victim->sex==2)?t_favour[favour_string(victim)].fav_nfstr:t_favour[favour_string(victim)].fav_nmstr, get_rdeity( deity_table[victim->pcdata->dn].russian,'2'));
         }
         return;
       }
       ptc( ch, "%s не имеет покровителя...\n\r", victim->name );
       return;
     }

     if( !str_cmp( argument, "increment") && IS_ELDER(ch) )
     {
       if( str_cmp( arg, deity_table[deity_lookup(arg)].name) )
       {
         stc("Проверь правильность написания deity!\n\r", ch);
         return;
       }
       deity_table[deity_lookup(arg)].worship +=1;
       ptc( ch, "Теперь у %s %d посвященных.\n\r", 
            get_rdeity( deity_table[deity_lookup(arg)].russian, '2'),
            deity_table[deity_lookup(arg)].worship );
       return;
     }

     if( !str_cmp( argument, "pardon") && !IS_NPC(victim) && IS_ELDER(ch) )
     {
      if( victim->pcdata->carma < 0) victim->pcdata->carma = 0;
      if( victim->pcdata->favour < 0) victim->pcdata->favour = 0;
      ptc( ch, "Ты отпускаешь грехи %s.\n\r", victim->name);
      ptc( victim, "%s отпускает твои грехи.\n\r", PERS(ch,victim));
      return;
     }

     if( !str_cmp( argument, "excuse") && !IS_NPC(victim) && IS_ELDER(ch) )
     {
       if( IS_SET( victim->act, PLR_DISAVOWED) ) REM_BIT( victim->act, PLR_DISAVOWED);
       else ptc( ch, "%s не проклят Богами.\n\r", victim->name);
       if( victim->godcurse >0 ) victim->godcurse = 0;
       else ptc( ch, "%s не проклят Богами.\n\r", victim->name);
       ptc( ch, "Ты прощаешь проступки %s.\n\r", victim->name);
       if( can_see( victim, ch, CHECK_LVL) ) ptc( victim, "%s прощает твои проступки.\n\r", PERS(ch,victim));
       return;
     }

     if( !str_prefix( argument, "reward") && IS_CREATOR(ch) )
     {
       if( !IS_NPC(victim) ) change_favour( victim, 50);
       else stc("Только для игровых персонажей!{x\n\r", ch);
       return;
     }

     if( !str_prefix( argument, "punish") && IS_CREATOR(ch) )
     {
       if( !IS_NPC(victim) ) change_favour( victim, -50);
       else stc("Только для игровых персонажей!{x\n\r", ch);
       return;
     }

     if( !str_cmp( argument, "stat") && !IS_NPC(victim) )
     {
        if( victim->deity != NULL )
        {
          stc( "{c+-------------------------------------------------------------+\n\r", ch);
          ptc( ch, "{c| {GСтатистика Deity для {Y%s{G, %s\n\r", victim->name, IS_NPC(victim)?"none":victim->pcdata->title);
          stc( "{c+-------------------------------------------------------------+\n\r", ch);
          ptc( ch, "{c| {GПосвящен{W: {R%12s{x {GЛояльность %s{W: {Y%5d {GКарма{W: {w%d\n\r",
             get_rdeity(dtab.russian, '3'), get_rdeity( dtab.russian,'2'),
             IS_NPC(ch)?0:victim->pcdata->favour, IS_NPC(ch)?0:victim->pcdata->carma );
          ptc( ch, "{c| {GОписание Deity{W: {x%s{G\n\r", dtab.descr);
          ptc( ch, "{c| {GКоличество последователей у %s{W: {m%d\n\r",
             get_rdeity( dtab.russian,'2'), dtab.worship );
          ptc( ch, "{c| {GРазрешенный характер{W: {D%5d{c/{W%5d{G, Характер персонажа{W: {R%5d{x\n\r",
             dtab.min_align, dtab.max_align, victim->alignment );
          ptc( ch, "{c| {YDeityNumber{w:{R%d {YDeityRank{w: {R%d {DDeityCurseCounter{w: {R%d{x\n\r",
             victim->pcdata->dn, victim->pcdata->dr, victim->pcdata->dc );
          stc( "{c+-------------------------------------------------------------+\n\r", ch);
          return;
        }
        stc( "{c+-------------------------------------------------------------+\n\r", ch);
        ptc( ch, "{c| {GСтатистика Deity для {Y%s{G, %s\n\r", victim->name, victim->pcdata->title);
        stc( "{c+-------------------------------------------------------------+\n\r", ch);
        ptc( ch, "{c| {Y%s {Gне имеет покровителя.{x\n\r", victim->name );
        ptc( ch, "{c| {YCarma{c: {W%6d {YFavour{c: {m%6d {wGodCurse{c: {D%2d {YAlign{c: {y%5d{x\n\r",
             victim->pcdata->carma, victim->pcdata->favour, victim->godcurse, victim->alignment);
        ptc( ch, "{c| {YDeityNumber{w:{R%2d {YDeityRank{w: {R%d {DDeityCurseCounter{w: {R%3d {x\n\r",
             victim->pcdata->dn, victim->pcdata->dr, victim->pcdata->dc );
        stc( "{c+-------------------------------------------------------------+\n\r", ch);
        return;
     }

     deity = deity_lookup(argument);

     if( !str_cmp( argument, dtab.name) )
     {
       if( !IS_NPC(ch) && IS_SET(victim->act,PLR_DISAVOWED) )
       {
         ptc( ch, "%s проклят Богами...\n\r", victim->name);
         return;
       }
       if( !may_devote( victim, deity_lookup(argument) ) && !IS_ELDER(ch) )
       { 
          stc("No match.\n\r", ch); 
          return; 
       }
       else
       {
         if( !IS_NPC(victim) ) dec_worship(victim);
         free_string(victim->deity);
       }
       victim->deity = str_dup(argument);
       victim->pcdata->dn = deity_lookup(argument);
       if( !IS_NPC(victim) ) deity_table[victim->pcdata->dn].worship++;
       ptc( ch, "Ты посвящаешь %s %s.\n\r", victim->name, get_rdeity(dtab.russian,'3') );
       ptc( victim, "Теперь твоя жизнь посвящена %s.\n\r", get_rdeity(dtab.russian,'3') );
       if( !IS_NPC(victim) )
       {
         victim->pcdata->favour += 5;
         victim->pcdata->carma +=5;
         save_deities();
       }
       return;
     }
    stc("{CНебеса{x не слышат твоих молитв...{x\n\r", ch);
    stc("Набери '{Gdevote help{x'для информации.{x\n\r", ch);
  }
}
#undef dtab

#define dtab deity_table[deity]
void do_deity ( CHAR_DATA *ch, const char *argument)
{
   char   buf[MAX_INPUT_LENGTH],  arg[MAX_STRING_LENGTH];
   char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
   char arg3[MAX_STRING_LENGTH];
   int  deity, value, i;

   if( EMPTY(argument) || !str_prefix( argument, "help") )
   {
     stc("{cКоманда редактирования Божеств для поклонения.{x\n\r", ch);
     stc("{cАргументы к команде следует набирать полностью!{x\n\r", ch);
     stc("{RУдаление {cиспользовать {RТОЛЬКО{c для неудачно созданного Божества!{x\n\r", ch);
     stc("{cИзменения {Rавтоматически не сохраняются {cв файле!{x\n\r", ch);
     stc("{cСинтаксис:{x\n\r", ch);
     stc("{G  deity [help]           - эта справка{x\n\r", ch);
     stc("{G  deity <stat>           - просмотреть статистику deity.{x\n\r", ch);
     stc("{G  deity <stat> [deity]   - просмотреть информацию о конкретном Божестве.{x\n\r", ch);
     stc("{G  deity <save>           - сохранить текущий DEITY_FILE{x\n\r", ch);
     stc("{G  deity <load>           - загрузить DEITY_FILE{x\n\r", ch);
     stc("{G  deity <deity> create   - создать новое Божество{x\n\r", ch);
     stc("{G  deity <deity> delete   - удалить Божество{x\n\r", ch);
     stc("{G  deity <deity> <name>     <new_name>    - сменить имя Божества{x\n\r", ch);
     stc("{G  deity <deity> <russian>  <new_russian> - сменить русское имя Божества{x\n\r", ch);
     stc("{G  deity <deity> <descr>    <new_descr>   - сменить описание Божества{x\n\r", ch);
     stc("{G  deity <deity> <alignmin> <value> - выставить минимальный требуемый характер{x\n\r", ch);
     stc("{G  deity <deity> <alignmax> <value> - выставить максимальный позволимый характер{x\n\r", ch);
     stc("{G  deity <deity> apply   <what> <value> - выставление бонусов Божества{x\n\r", ch);
     stc("{G  deity <deity> penalty <what> <value> - выставление пенальти Божества{x\n\r", ch);
     return;
   }

   if( !str_cmp( argument, "save") )
   {
     stc( "{RSaving deity data file.{x\n\r",ch);
     save_deities();
     stc( "{RDone.{x\n\r", ch);
     return;
   }

   if( !str_cmp(  argument, "load") )
   {
     stc( "{RLoading new deity list...\n\r", ch);
     load_deities();
     stc( "{RDone.{x\n\r", ch);
     return;
   }

   argument = one_argument( argument, arg);

   if( !str_cmp( arg, "stat") )
   {
     if( EMPTY(argument) )
     {
       stc("{w ___   ____   ____________   ____________   ___________{x\n\r", ch);
       stc("{w[{g#No{w] [{rWsh {w] [{CName        {w] [{yRussianName {w] [{DDescription{w]{x\n\r", ch);
       stc("{w >-<   >--<   >----------<   >----------<   >---------<{x\n\r", ch);
       for( deity = 0; dtab.name && deity<MAX_DEITIES; deity++)
         ptc( ch, "{w[{G%3d{w] [{R%4d{w] [{c%12s{w] [{Y%12s{w] [{x%s{x\n\r", deity, dtab.worship, 
             dtab.name, get_rdeity( dtab.russian,'1'), dtab.descr);
       return;
     }
     else 
       if( !str_cmp( argument, deity_table[deity_lookup(argument)].name ) )
       {
         stc( "{w[{DNum{w] [{DName        {w] [{DR_Name      {w] [{DWorship{w] [{DMinAlign{w] [{DMaxAlign{w]{x\n\r", ch);
         ptc( ch, "{w[{w%3d{w] [{y%12s{w] [{Y%12s{w] [  {R%5d{w] [  {G%5d {w] [  {R%5d {w]\n\r{r >{w%s\n\r",
              deity_lookup(argument),
              deity_table[deity_lookup(argument)].name,
              get_rdeity(deity_table[deity_lookup(argument)].russian,'1'),
              deity_table[deity_lookup(argument)].worship,
              deity_table[deity_lookup(argument)].min_align,
              deity_table[deity_lookup(argument)].max_align,
              deity_table[deity_lookup(argument)].descr);
         show_deity_applies( ch, deity_lookup(argument) );
         return;
       }
     stc("Такого Божества не существует!{x\n\r", ch);
     return;
   }

   if( !str_cmp( arg, "astat") && IS_ELDER(ch) )
   {
     stc("{D+--+{y---------+--+-----+-----+-----------+-----------+-----------+-----------+-----------+{x\n\r{x", ch);
     stc("{D|{wN#{D|{BName     {y|{MWs{y|{galMin{y|{ralMax{y| {G+{cParams{R-  {y| {G+{cInform{R-  {y| {G+{cResists{R- {y| {G+{cSkills{R-  {y| {G+{cBonus{R-   {y|{x\n\r", ch);
     stc("{D+--+{y---------+--+-----+-----+-----------+-----------+-----------+-----------+-----------+{x\n\r{x", ch);
     for( deity = 0; dtab.name && deity<MAX_DEITIES ; deity++)
     {
       ptc( ch, "{D|{W%2d{D|{Y%9s{y|{c%2d{y|{G%5d{y|{m%5d{y|", deity, dtab.name, dtab.worship,
            dtab.min_align, dtab.max_align);
       for( i=0; i < MAX_DEITY_APP; i++)
       {
         switch( dtab.d_apply[i])
         {
          case 0:  stc("{w", ch); break;
          case 1:  stc("{G", ch); break;
          case 2:  stc("{Y", ch); break;
          case 3:  stc("{c", ch); break;
          case 4:  stc("{m", ch); break;
          case 5:  stc("{R", ch); break;
          default: stc("{r", ch); break;
         }
         ptc( ch, "%2d", dtab.d_apply[i] );
         if( (i+1)%4 == 0 ) stc("{y|", ch);
         else stc(" ", ch);
       }
       stc("{x\n\r", ch);
     }
     stc("{D+--+{y---------+--+-----+-----+-----------+-----------+-----------+-----------+-----------+{x\n\r{x", ch);
     return;
   }

   if( !str_cmp( argument, "create") && !EMPTY(arg) )
   {
     if( !str_cmp( arg, deity_table[deity_lookup(arg)].name) )
     {
       ptc( ch, "Божество %s уже существует!\n\r", arg );
       return;
     }
     deity = gvd();
     if (deity==-1)
     {
       ptc(ch,"Достигнуто максимальное количество божеств. (%d)\n\r",MAX_DEITIES);
       return;
     }
     arg[0]=UPPER(arg[0]);
     dtab.name = str_dup(arg);
     dtab.russian = "Божество";
     dtab.min_align = -1000;
     dtab.max_align = 1000;
     dtab.descr = "Описание Нового Божества";
     dtab.worship = 0;
     for( i=0; i < MAX_DEITY_APP; i++)
       dtab.d_apply[i] = 0;
     ptc( ch, "Новое deity {y%s{x успешно создано!\n\r", dtab.name);
     return;
   }

   if(  !str_cmp( arg, deity_table[deity_lookup(arg)].name ) && !EMPTY(argument) )
   {
     if( !str_cmp( argument, "delete") && !str_cmp(arg, deity_table[deity_lookup(arg)].name) )
     {
       deity = deity_lookup(arg); 

       ptc( ch, "Удаляем божество {y%s{x...", deity_table[deity_lookup(arg)].name);
       for( ; deity < MAX_DEITIES; deity++)
       {
         if (!deity_table[deity+1].name) break;
         dtab.name=deity_table[deity+1].name;
         dtab.russian   = deity_table[deity+1].russian;
         dtab.descr     = deity_table[deity+1].descr;
         dtab.min_align = deity_table[deity+1].min_align;
         dtab.max_align = deity_table[deity+1].max_align;
         dtab.worship   = deity_table[deity+1].worship;
         for( i=0; i < MAX_DEITY_APP; i++)
           dtab.d_apply[i] = deity_table[deity+1].d_apply[i];
         if( deity < 0 ) return;
       }
       free_string(dtab.russian);
       free_string(dtab.name);
       free_string(dtab.descr);
       dtab.name = NULL;
       dtab.min_align = 0;
       dtab.max_align = 0;
       dtab.worship = 0;
       for( i=0; i < MAX_DEITY_APP; i++) dtab.d_apply[i] = 0;
       stc (" Готово\n\r",ch);
       return;
     }

     argument = one_argument( argument, arg1);

     if ( !str_prefix( arg1, "name") && !EMPTY(argument) )
     {
         ptc(ch,"Имя Божества {W%s{x изменено на '{Y%s{x'.\n\r",
               deity_table[deity_lookup(arg)].name,
               str_dup( argument) );
         do_printf( buf, "%s", argument);
         buf[0]=UPPER(buf[0]);
         free_string( deity_table[deity_lookup(arg)].name);
         deity_table[deity_lookup(arg)].name = str_dup(buf);
         return;
     }

     if ( !str_prefix( arg1, "russian") && !EMPTY(argument) )
     {
         ptc(ch,"Русское Имя {W%s{x изменено на '{Y%s{x'.\n\r",
              get_rdeity( deity_table[deity_lookup(arg)].russian,'2'),
              str_dup( argument) );
         do_printf( buf, "%s", argument);
         buf[0]=UPPER(buf[0]);
         free_string( deity_table[deity_lookup(arg)].russian);
         deity_table[deity_lookup(arg)].russian = (char *)str_dup(buf);
         return;
     }

     if ( !str_prefix( arg1, "descr") && !EMPTY(argument) )
     {
         ptc(ch,"Описание {W%s{x изменено на '{Y%s{x'.\n\r",
             get_rdeity( deity_table[deity_lookup(arg)].russian, '2'), 
             str_dup( argument) );
         do_printf( buf, "%s", argument);
         buf[0]=UPPER(buf[0]);
         free_string( deity_table[deity_lookup(arg)].descr);
         deity_table[deity_lookup(arg)].descr = str_dup(buf);
         return;
     }

     argument = one_argument( argument, arg2);

     if ( !str_cmp( arg1, "alignmin"))
     {
         if( !(value = atoi(arg2)) ) value = 0;
         if( value < -1000) value = -1000;
         if( value > 1000 ) value = 1000;
 
         if( value >= deity_table[deity_lookup(arg)].max_align )
         {
           stc( "Минимальный характер не может быть больше максимального!\n\r", ch);
           return;
         }
         ptc(ch,"{GMinalign {W%s{x изменено с '{y%d{x' на '{Y%d{x'.\n\r",
             get_rdeity( deity_table[deity_lookup(arg)].russian, '2'),
             deity_table[deity_lookup(arg)].min_align, value);
         deity_table[deity_lookup(arg)].min_align = value;
         return;
     }

     if ( !str_cmp( arg1, "alignmax"))
     {
         if( !(value = atoi(arg2)) ) value = 0;
         if( value < -1000) value = -1000;
         if( value > 1000 ) value = 1000;
 
         if( value <= deity_table[deity_lookup(arg)].min_align )
         {
           stc( "Максимальный характер не может быть меньше минимального!\n\r", ch);
           return;
         }
         ptc(ch,"{GMaxalign {W%s{x изменено с '{y%d{x' на '{Y%d{x'.\n\r",
             get_rdeity( deity_table[deity_lookup(arg)].russian, '2'), 
             deity_table[deity_lookup(arg)].max_align, value);
         deity_table[deity_lookup(arg)].max_align = value;
         return;
     }

     argument = one_argument( argument, arg3);

     if( !str_cmp( arg1, "apply") || !str_cmp( arg1, "penalty") )
     {
         int iarg3;
         bool isapply=TRUE;

         deity = deity_lookup(arg);
         if( EMPTY(arg2) )
         {
           stc("{cПозволенные значения Божественных бонусов и пенальти:{x\n\r", ch);
           stc("{cПараметры:{x\n\r", ch);
           stc("{G  str, int, wis, dex, con{x\n\r", ch);
           stc("{cСопротивляемости\\Уязвимости:{x\n\r", ch);
           stc("{G  rslash,  rpierce, rbash,     rfire, rpoison,{x\n\r", ch);
           stc("{G  rmental, rlight,  rnegative, racid, rholy{x\n\r", ch);
           stc("{cБонусы\\Пенальти:{x\n\r", ch);
           stc("{G  water,      air,      earth,   fire,       spirit{x\n\r", ch);
           stc("{G  mind,       light,    dark,    fortitude,  curative{x\n\r", ch);
           stc("{G  perception, learning, offence, protection, makemastery{x\n\r", ch);
           return;
         }

         if( str_cmp( arg2, "spell") && str_cmp( arg2, "skill") )
         {
           if( EMPTY(arg3) || (iarg3=atoi(arg3)) < 0 ) iarg3=0;
           if( iarg3 < 0 || iarg3 > 10 ) iarg3 = 0; 
           if( is_exact_name( arg2,"water rslash str" ) ) value = 1;
           else if( is_exact_name( arg2,"air rpierce int" ) ) value = 2;
           else if( is_exact_name( arg2,"earth rbash wis" ) ) value = 3;
           else if( is_exact_name( arg2,"fire rfire dex" ) ) value = 4;
           else if( is_exact_name( arg2,"spirit rpoison con" ) ) value = 5;
           else if( is_exact_name( arg2,"mind rmental" ) ) value = 6;
           else if( is_exact_name( arg2,"light rlight" ) ) value = 7;
           else if( is_exact_name( arg2,"dark rnegative" ) ) value = 8;
           else if( is_exact_name( arg2,"fortitude racid" ) ) value = 9;
           else if( is_exact_name( arg2,"curative rholy" ) ) value = 10;
           else if( !str_cmp( arg2,"perception") ) value=11; 
           else if( !str_cmp( arg2,"learning" ) ) value = 12;
           else if( !str_cmp( arg2,"offence") ) value = 13;
           else if( !str_cmp( arg2,"protection" ) ) value = 14;
           else if( !str_cmp( arg2,"makemastery" ) ) value = 15;
           else { stc("Такого бонуса не существует!\n\r", ch); return; }

           i = 0;
           if( !str_cmp( arg1, "penalty" ) )
           {
             i += 2;
             isapply = FALSE;
           }
           if( is_exact_name(arg2,"str int wis dex con") 
            && deity_apply_table[value].param )
           {
             i += 0;
             if( iarg3 == 0 )
             {
               ptc( ch, "Теперь {C%s{x не меняет {G%s{x.\n\r", get_rdeity( dtab.russian,'1'),
                    deity_apply_table[value].param );
               dtab.d_apply[i]=0;
               dtab.d_apply[i+1]=0;
               return;
             }
             dtab.d_apply[i]=value;
             dtab.d_apply[i+1]=iarg3;
             ptc(ch,"Теперь поклонение {C%s{x %s {G%s{x на {R%d{x.\n\r",
               get_rdeity(dtab.russian,'3'),isapply?"увеличивает":"уменьшает", deity_apply_table[value].param,iarg3);
             return;
           }
           if( (is_exact_name(arg2,"water air earth fire spirit mind light dark fortitude")
            || is_exact_name(arg2,"curative perception learning offence protection makemastery"))
            && deity_apply_table[value].inform )
           {
             i += 4;
             if( iarg3 == 0 )
             {
               ptc( ch, "Теперь {C%s{x не меняет бонус {G%s{x.\n\r", get_rdeity( dtab.russian,'1'),
                    deity_apply_table[value].inform );
               dtab.d_apply[i]=0;
               dtab.d_apply[i+1]=0;
               return;
             }
             dtab.d_apply[i]=value;
             dtab.d_apply[i+1]=iarg3;
             ptc(ch,"Теперь поклонение {C%s{x %s бонус {Y%s{x на {R%d{x.\n\r",
               get_rdeity(dtab.russian,'3'),isapply?"увеличивает":"уменьшает",deity_apply_table[value].inform,iarg3);
             return;
           }
           if( (is_exact_name(arg2,"rslash rpierce rbash rfire rpoison rmental")
            || is_exact_name(arg2,"rlight rnegative racid rholy"))
            && deity_apply_table[value].resist )
           {
             i += 8;
             if( iarg3 == 0 )
             {
               ptc( ch, "Теперь {C%s{x не меняет защиту от {D%s{x.\n\r", get_rdeity( dtab.russian,'1'),
                    deity_apply_table[value].resist );
               dtab.d_apply[i]=0;
               dtab.d_apply[i+1]=0;
               return;
             }
             dtab.d_apply[i]=value;
             dtab.d_apply[i+1]=iarg3;
             ptc(ch,"Теперь поклонение {C%s{x %s защиту от {G%s{x на {R%d{x.\n\r",
               get_rdeity(dtab.russian,'3'),isapply?"увеличивает":"уменьшает",deity_apply_table[value].resist,iarg3);
             return;
           }
           return;
         }
         if( !str_cmp( arg2, "spell") || !str_cmp( arg2, "skill") )
         {
           return;
         }

         if( !str_cmp( arg2, "bonus") )
         {
           return;
         }
       }
     }
   stc("Не указаны правильно все параметры.\n\r", ch);
   stc("Набери '{Gdeity help{x' для справки.\n\r", ch);
   return;
}
#undef dtab

void damage_obj( CHAR_DATA *ch, OBJ_DATA *obj, int dam_v, bool inf_char )
{
  if( !IS_SET(global_cfg,CFG_DAMAGEOBJ) ) return;

  if( ch == NULL || !obj ) return;

  if( !str_cmp( get_obj_cond(obj, 0), SHOW_COND_ETERNAL ) ) return;

  if( obj->durability != -1)
  {
    obj->condition -= dam_v;

    if( obj->condition < 0 ) obj->condition = 0;

    if( number_range(1,2) == 1 && inf_char )
      act("С $i2 сыпятся искры!",ch,obj,NULL,TO_ROOM);

    if( obj->condition == 0 )
    {
      if( IS_IMMORTAL(ch) )
      {
        ptc( ch,"{RCondition of {x%s {Rrestored!\n\r", get_obj_desc( obj, '1') );
        obj->condition = obj->durability;
        return;
      }
      act("$i1 с треском ломается!",ch,obj,ch->fighting,TO_ROOM);
      if( obj->carried_by != NULL)
        if( material_table[material_num(obj->material)].d_dam > 450 )
        {
          unequip_char( ch, obj);
          return;
        }
      obj_from_char(obj);
      extract_obj(obj);
    }
  }
};

void do_pk ( CHAR_DATA *ch, const char *argument)
{
};

#define rdeity deity_table[ch->pcdata->dn].russian
void do_supplicate ( CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_INPUT_LENGTH];
  if( ch->deity == NULL || IS_SET( ch->act, PLR_DISAVOWED) )
  {
    stc("Боги не слышат тебя.\n\r", ch);
    return;
  }
  if( EMPTY(argument) || !str_prefix( argument, "help") )
  {
    stc("{cПросить свое Божество о помощи.{x\n\r", ch);
    stc("{cСинтаксис:{x\n\r", ch);
    stc("{G  supplicate <blessing>{x\n\r", ch);
    stc("{cДоступные блага:{x\n\r", ch);
    stc("{G  recall  - перенестись в Храм.{x\n\r", ch);
    stc("{G  peace   - остановить бой в комнате.{x\n\r", ch);
    stc("{G  aid     - подвосстановить силенки.{x\n\r", ch);
    stc("{G  uprise  - снять вредные заклинания ( не на арене).{x\n\r", ch);
//    stc("{G  protect - защититься на время от внешнего мира.{x\n\r", ch);
//    stc("{cНекоторые Боги дают дополнительные параметры.{x\n\r", ch);
//    stc("{cСмотри информацию о своем Божестве.{x\n\r", ch);
    return;
  }
  if( ch->deity == NULL )
  {
    stc("Боги не слышат тебя.\n\r", ch);
    return;
  }
  if( ch->pcdata->favour < 1 )
  {
    ptc( ch, "{C%s{x не слышит твоих молитв...\n\r", get_rdeity( rdeity, '0') );
    return;
  }
  if( IS_SET(ch->in_room->room_flags, ROOM_ARENA) || IS_SET( ch->act, PLR_ARMY) )
  {
    ptc( ch, "{C%s{x не будет помогать тебе здесь...\n\r", get_rdeity( rdeity,'0') );
    return;
  }
  if (ch->pcdata->condition[COND_ADRENOLIN]!=0 && ch->level<102)
  {
    stc( "{rТы слишком взволнован для молитвы.{x\n\r", ch );
    return;
  }

  if( is_affected(ch,skill_lookup("pray")) )
  {
    ptc( ch, "{C%s{x устал от твоих молитв...\n\r",
         get_rdeity( deity_table[ch->pcdata->dn].russian, '0') );
    change_favour(ch, -20);
    return;
  }
  else if( !IS_ELDER(ch) )
  {
    AFFECT_DATA paf;

    paf.where        = TO_AFFECTS;
    paf.type         = skill_lookup("pray");
    paf.level        = ch->level;
    paf.duration     = ch->level / 15+5;
    paf.location     = APPLY_NONE;
    paf.modifier     = 0;
    paf.bitvector    = 0;
    affect_to_char( ch, &paf);
  }

  if( !IS_ELDER(ch) ) WAIT_STATE( ch, 2-(IS_SET(ch->act,PLR_HIGHPRIEST)?1:0) );

  if( !str_prefix( argument, "recall") )
  {
    if( ch->move < 2 ) return;
    if( ROOM_EVIL(ch) && !favour(ch, -350) ) return;
    if( is_affected( ch, AFF_CURSE ) ) return;
    if( ch->fighting != NULL )
    {
      if( !favour(ch, -30) ) return;
      stop_fighting( ch, TRUE);
    }
    if( ch->pcdata->condition[COND_ADRENOLIN] != 0 && !favour(ch,-550) ) return;
    do_printf(buf,"{Y$c1{x молит {C%s{x перенести его!{/{C%s{x забирает {Y$c2{x!",
      get_rdeity( rdeity ,'4'), get_rdeity( rdeity, '0') );
    act(buf,ch,NULL,NULL,TO_ROOM);
    ptc( ch, "Ты молишь {C%s{x перенести тебя!{x\n\r", get_rdeity( rdeity,'4') );
    ptc( ch, "{C%s{x переносит тебя в храм!{x\n\r", get_rdeity( rdeity,'0') );
    char_from_room(ch);
    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE) );
    do_function( ch, &do_look, "");
    return;
  }

  if( !str_prefix( argument, "peace") )
  {
     CHAR_DATA *vch;

     if( ROOM_EVIL(ch) && !favour(ch, -50) ) return;

     for(vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
     {
       if( vch->fighting != NULL )
       {
         if( !IS_NPC( vch) && !favour(ch, -35) ) continue;
         stop_fighting( vch, TRUE);
       }
       if( IS_NPC(vch) && IS_SET( vch->act, ACT_AGGRESSIVE) )
       {
         if( !favour( ch, -25) ) continue;
         REM_BIT( vch->act, ACT_AGGRESSIVE);
       }
       if( !IS_CREATOR(ch) ) DAZE_STATE( ch, 2);
     }
    do_printf(buf,"{Y%s{x молит {C%s{x остановить бой!{/Благосный туман окутывает комнату, прекращая насилие...",
      ch->name, get_rdeity( rdeity ,'4') );
    act(buf,ch,NULL,NULL,TO_ROOM);
    ptc(ch,"Ты молишь {C%s{x остановить бой!{x\n\r", get_rdeity(rdeity,'4') );
    ptc(ch,"Благосный свет окутывает комнату, прекращая насилие!{x\n\r");
    return;
  }

  if( !str_cmp( argument, "aid") )
  {
    int heal;

    if( !favour( ch, -55) ) return;
    else
    {
      heal = number_range( ch->level, 5 * ch->level);
      ch->mana = UMIN( ch->mana + heal, ch->max_mana );
      heal = number_range( ch->level, 4 * ch->level);
      ch->hit =   UMIN( ch->hit + heal, ch->max_hit );
      heal = number_range( ch->level, 3 * ch->level);
      ch->move = UMIN( ch->move + heal, ch->max_move );
      update_pos(ch);
      do_printf(buf,"{Y%s{x молит {C%s{x о новых силах!{/В глазах {Y%s{x вспыхивают лучи энергии...", ch->name,
      get_rdeity( rdeity ,'4'), ch->name );
      act(buf,ch,NULL,NULL,TO_ROOM);
      ptc(ch,"Ты молишь {C%s{x о новых силах!{x\n\r", get_rdeity(rdeity,'4') );
      stc("Ты чувствуешь, как сквозь твое тело проходит энергия!{x\n\r", ch);
      return;
    }
    return;
  }

  if( !str_cmp( argument, "uprise") )
  {
    int dfav=0;

    if( ch->fighting != NULL ) dfav -= 45;
    if( ch->pcdata->condition[COND_ADRENOLIN] != 0 ) dfav -= 200;
    if( ROOM_EVIL(ch) ) dfav -= 45;

    if( is_affected( ch, gsn_blindness) )
      if( favour( ch, dfav -= 45) )
      {
        affect_strip( ch, gsn_blindness );
        REM_BIT(ch->affected_by,AFF_BLIND);
        ptc( ch, "%s проясняет твой взгляд.\n\r", get_rdeity( rdeity,'0') );
        act("$c1 больше не ослеплен.",ch,NULL,NULL,TO_ROOM);
      }
    if( is_affected( ch, gsn_curse) )
      if( favour( ch, dfav -= 65) )
      {
        affect_strip( ch, gsn_curse );
        REM_BIT(ch->affected_by,AFF_CURSE);
        stc("Ты чувствуешь себя лучше.\n\r", ch);
        act("$c1 чувствует себя более расслабленно.",ch,NULL,NULL,TO_ROOM);
      }
    if( is_affected( ch, gsn_poison) )
      if( favour( ch, dfav -= 40) )
      {
        affect_strip( ch, gsn_poison );
        REM_BIT(ch->affected_by,AFF_POISON);
        stc("Тепло проходит сквозь тебя, унося боль.\n\r", ch);
        act("$c1 выглядит значительно лучше.",ch,NULL,NULL,TO_ROOM);
      }
    if( is_affected( ch, gsn_plague) )
      if( favour( ch, dfav -= 85) )
      {
        affect_strip( ch, gsn_plague );
        REM_BIT(ch->affected_by,AFF_PLAGUE);
        stc("Язвы, покрывавшие тебя, исчезли.\n\r", ch);
        act("$c1 выглядит выздоровевшим, и $g язвы исчезли.",ch,NULL,NULL,TO_ROOM);
      }
    do_printf( buf,"На мгновенье над тобой появляется туманный лик {C%s{x", get_rdeity( rdeity,'2') );
    act( buf,ch,NULL,NULL,TO_ALL_IN_ROOM);
    return;
  }

  if (!str_prefix(argument,"protect") && ch->pcdata->condition[COND_ADRENOLIN] == 0 && !ch->fighting)
  {
    if( favour( ch, -50) )
    {
      AFFECT_DATA caff;

      if( is_affected( ch, gsn_gaseous_form) )
      {
        stc("Ты уже в сфере эмпирея...\n\r", ch);
        return;
      }
      caff.where     = TO_AFFECTS;
      caff.type      = skill_lookup("gaseous form");
      caff.level     = 120;
      caff.duration  = deity_table[ch->pcdata->dn].worship / 3 + 1;
      caff.location  = APPLY_NONE;
      caff.modifier  = 0;
      caff.bitvector = AFF_GASEOUS_FORM;
      affect_to_char( ch, &caff );
      do_printf( buf,"На мгновенье над тобой появляется туманный лик {C%s{x", get_rdeity( rdeity,'2') );
      act( buf,ch,NULL,NULL,TO_ALL_IN_ROOM);
      stc( "Ты чувствуешь как воздух начинает проходить сквозь твое тело.\n\r", ch);
      return;
    }
    ptc( ch, "{C%s{x отказывается от тебя...\n\r", get_rdeity( deity_table[ch->pcdata->dn].russian,'1') );
  }
#undef rdeity
}

void do_forge( CHAR_DATA *ch, const char *argument)
{
  OBJ_DATA *obj;
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg);

  if( EMPTY(arg) || !str_cmp( argument, "help") )
  {
    stc("{cКоманда перековки предмета.{x\n\r", ch);
    stc("{cПеределывает структуру вещи.{x\n\r", ch);
    stc("{cСинтаксис:{x\n\r", ch);
    stc("{G  forge <obj> <new_material> [char]{x\n\r", ch);
    return;
  }

  if( ch == NULL )
  {
    bug("Null char in do_forge", 0);
    return;
  }

  argument = one_argument( argument, arg1);

  if ( ( victim = get_char_room( ch, argument) ) == NULL) victim = ch;

  if( ( obj = get_obj_wear( victim, arg1)) != NULL );
  else if( !(obj = get_obj_victim( ch, victim, arg)) )
  {
    stc("Предмет не найден!\n\r", ch);
    return;
  }

  if( !str_cmp( arg1, material_table[material_num(arg1)].name ) )
  {
    ptc( ch,"Ты вковываешь в %s %s %s{x.\n\r", material_table[material_num(obj->material)].real_name,
      get_obj_desc( obj, '2'), material_table[material_num(material_lookup(arg1))].real_name );
    if( victim != ch )
      ptc( victim,"%s вковывает в %s %s %s{x.\n\r", ch->name, material_table[material_num(obj->material)].real_name,
           get_obj_desc( obj, '2'), material_table[material_num(material_lookup(arg1))].real_name );
    free_string(obj->material);
    obj->material = str_dup(arg1);
    save_char_obj(victim);
    return;
  }
  stc("Набери '{Gforge help{x' для справки.{x\n\r", ch);
};

void do_ldefend( CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int lp=0;

  argument = one_argument( argument, arg);
  argument = one_argument( argument, arg1);

  if( EMPTY(arg) || !str_cmp( arg, "help") )
  {
    stc("{cЗащитить персонажа по уровню. Боги младше установленного{x\n\r", ch);
    stc("{cуровня не смогут оказывать давление на защищенного персонажа.{x\n\r", ch);
    stc("{cСинтаксис:\n\r", ch);
    stc("{G   ldefend <char> <level>\n\r", ch);
    return;
  }

  if( (victim = get_pchar_world( ch, arg)) == NULL || IS_NPC(victim) )
  {
    stc("Тут таких нет.\n\r", ch);
    return;
  }

  if( (lp=atoi(arg1)) < 1 || lp > get_trust(ch) || victim->pcdata->protect > get_trust(ch) )
  {
    stc("Не катит.\n\r", ch);
    return;
  }
  ptc( ch, "Ты защищаешь {Y%s{x уровнем {R%d{x.\n\r", victim->name, lp);
  do_printf(buf,"%s сменил защиту персонажа %s со %d на %d.\n\r",
      ch->name,victim->name,victim->pcdata->protect,lp);
  send_note("{YSystem{x","Dragon","Character was protected",buf,3);
  victim->pcdata->protect = lp;
};

