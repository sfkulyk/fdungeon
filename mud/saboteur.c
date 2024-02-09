// $Id: saboteur.c,v 1.125 2003/12/06 10:30:49 wagner Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h> 
#include <ctype.h> 
#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <stdlib.h> 
#include "merc.h" 
#include "recycle.h" 
#include "tables.h" 
#include "interp.h" 
 
int64 flag_lookup args( ( const char *name, const struct flag_type *flag_table));
char *flag_string( const struct flag_type *flag_table, int64 bits );
int   deity_char_power   ( CHAR_DATA *ch, int type, int subtype);
int  favour_string      ( CHAR_DATA *ch);
AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn);

void raw_kill     args( ( CHAR_DATA *victim ) ); 
int  getnumname   args( ( CHAR_DATA *ch ) ); 

void add_penalty(CHAR_DATA *ch, PENALTY_DATA *penalty, int value);

char *char_vote(CHAR_DATA *ch,VOTE_DATA *vote);
void char_voting(CHAR_DATA *ch,VOTE_DATA *vote,int type);
void remove_voting_char(CHAR_DATA *ch,VOTE_DATA *vote);
void save_vote();

void append_note(NOTE_DATA *pnote);
char *newspaper_subject(int type);

void travel(int room, int cost, CHAR_DATA *ch, CHAR_DATA *summoner);
CHAR_DATA * find_summoner       args( ( CHAR_DATA *ch ) );

DECLARE_DO_FUN2( do_say  );
DECLARE_DO_FUN2( do_look );

char * const vote_table[] =
{
  "{xне голосовал","{Gза{x", "{Rпротив{x", "{Dвоздержался{x"
};

void do_account( CHAR_DATA *ch, const char *argument ) 
{ 
  CHAR_DATA *keeper;
  CHAR_DATA *victim; 
  char arg1[MAX_INPUT_LENGTH]; 
  char arg2[MAX_INPUT_LENGTH]; 
  int64 amount; 
  int tmp=0;
 
  if (argument[0]=='\0') 
  { 
    ptc(ch,"{CУ тебя на счету: {Y%u {Cзолота{x\n\r", ch->pcdata->account); 
    return; 
  } 
 
  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"account")) return;
 
  for ( keeper=ch->in_room->people; keeper; keeper=keeper->next_in_room ) 
  { 
    if (IS_NPC(keeper) && IS_SET(keeper->act,ACT_ACCOUNTER)) break; 
  } 

  if (keeper==NULL || !IS_NPC(keeper) || !IS_SET(keeper->act,ACT_ACCOUNTER)) 
  { 
    stc("{RСначала зайди в банк.\n\r{x",ch); 
    return; 
  } 
 
  argument = one_argument(argument, arg1); 
  argument = one_argument(argument, arg2); 
 
  if (is_number(arg2)) 
     amount=atoi(arg2); 
  else
  { 
    stc("{GACCOUNT{x - комманда управления счетом.\n\r",ch);
    stc("{CСинтаксис: {x\n\r",ch);
    stc("{G account get ХХХ [gold] - снять ХХХ золота со счета.{x\n\r",ch); 
    if (IS_IMMORTAL(ch)) 
        stc("{c      account get ХХХ <charname> - снять ХХХ золота со счета <charname>.{x\n\r",ch); 
    stc("{G account put ХХХ [gold] - положить ХХХ золота на счет.{x\n\r",ch); 
    stc("{G account put ХХХ silver - положить ХХХ серебра на счет.{x\n\r",ch); 
    stc("{G account put ХХХ diamonds - положить ХХХ бриллиантов на счет.{x\n\r",ch); 
    stc("{G account put ХХХ crystals - положить ХХХ кристаллов на счет.{x\n\r",ch); 
    stc("{G account transfer XXX <имя получателя> - перевести ХХХ золота на счет <имя получателя>.{x\n\r",ch); 
    return; 
  } 
 
  if (!str_prefix(arg1,"help"))
  { 
    stc("{GACCOUNT{x - комманда управления счетом.\n\r",ch);
    stc("{CСинтаксис: {x\n\r",ch);
    stc("{G account get ХХХ [gold] - снять ХХХ золота со счета.{x\n\r",ch); 
    if (IS_IMMORTAL(ch)) 
        stc("{c      account get ХХХ [gold] <charname> - снять ХХХ золота со счета <charname>.{x\n\r",ch); 
    stc("{G account put ХХХ [gold] - положить ХХХ золота на счет.{x\n\r",ch); 
    stc("{G account put ХХХ silver - положить ХХХ серебра на счет.{x\n\r",ch); 
    stc("{G account put ХХХ diamonds - положить ХХХ бриллиантов на счет.{x\n\r",ch); 
    stc("{G account put ХХХ crystals - положить ХХХ кристаллов на счет.{x\n\r",ch); 
    stc("{G account transfer XXX <имя получателя> - перевести ХХХ золота на счет <имя получателя>.{x\n\r",ch); 
    stc("{G ACCOUNT help - эта справка.{x\n\r",ch); 
    return; 
  } 

  if (!str_prefix(arg1,"put"))
  {
    int64 count;

    if (  EMPTY(argument) || !str_prefix(argument, "gold") )
    {
      if (amount<10 || amount>ch->gold)
      {
        stc("{RТы не можешь положить меньше 10 или больше чем у тебя есть.\n\r{x",ch);
        return;
      }
      ch->gold-=amount;
      if (amount%10!=0) 
         tmp=1;
      ptc(ch,"\n\r{CТы кладешь на счет {Y%u {Cзолота. Высчитано {Y%u {C(10%).{x\n\r",amount,amount/10+tmp);
      amount-=amount/10+tmp;
      ch->pcdata->account+=amount;
      ptc(ch,"На счету теперь {Y%u{C.{x\n\r", ch->pcdata->account);
      WILLSAVE(ch);
      return;
    }
    
    if (!str_prefix(argument, "silver"))
    {
      if (amount<110 || amount>ch->silver)
      {
        stc("{RТы не можешь положить меньше 110 или больше чем у тебя есть.\n\r{x",ch);
        return;
      }
      ch->silver-=amount;
      ptc(ch,"\n\r{CТы кладешь на счет {Y%u {Cсеребра. Высчитано {Y%u {C(10%).{x\n\r",amount,amount/10);
      amount-=amount/10;
      ch->pcdata->account+=amount/100;
      count=amount-amount/100*100;
      ptc(ch,"На счету теперь {Y%u{C.{x\n\rСдача %u серебра.\n\r", ch->pcdata->account,count);
      ch->silver+=count;
      WILLSAVE(ch);
      return;
    }
    
    if (!str_prefix(argument, "diamonds"))
    {
      count=sell_gem(ch,amount,OBJ_VNUM_DIAMOND);
      ptc(ch, "Ты кладешь бриллианты в количестве %u штук на свой счет. Это %u золота.\n\r", count, (int64)count*90);
      ch->pcdata->account+=count*90;
      ptc(ch,"На твоем счету теперь {Y%u{C.{x\n\r", ch->pcdata->account);
      WILLSAVE(ch);
      return;
    }
    
    if (!str_prefix(argument, "crystals"))
    {
      count=sell_gem(ch,amount,OBJ_VNUM_CRYSTAL);
      ptc(ch, "Ты кладешь кристаллы в количестве %u штук на свой счет. Это %u золота.\n\r", count, (int64)(count*9000));
      ch->pcdata->account+=count*9000;
      ptc(ch,"На твоем счету теперь {Y%u{C.{x\n\r",ch->pcdata->account);
      WILLSAVE(ch);
      return;
    }
  }
 
  if (!str_prefix(arg1,"get")) 
  { 
    if (amount<10 || amount>ch->pcdata->account) 
    { 
      stc("{RТы не можешь снять меньше 10 или больше, чем у тебя на счету.{x",ch); 
      return; 
    } 
    if (IS_IMMORTAL(ch))
    {
      if (!EMPTY(argument))
      {
        if ((victim = get_pchar_world(ch, argument)) == NULL)
        {
          stc("Тут таких нет.\n\r", ch);
          return;
        }
        else
        {
          victim->pcdata->account-=amount; 
          ptc(ch,"\n\r{CТы снимаешь со счета {R%s{x {Y%u {Cзолота. Осталось {Y%u.{x\n\r",victim->name,amount,victim->pcdata->account); 
          WILLSAVE(victim);
          return;
        }
      }
    }
    ch->pcdata->account-=amount; 
    ch->gold+=amount; 
    ptc(ch,"\n\r{CТы снимаешь со счета {Y%u {Cзолота. Осталось {Y%u.{x\n\r",amount,ch->pcdata->account); 
    WILLSAVE(ch);
      return;
  } 

/* Transfers money from one char to another. (c) Wagner */
  if (!str_prefix(arg1,"transfer")) 
  { 
    if ((victim = get_pchar_world(ch, argument)) == NULL)
    {
      stc("Тут таких нет.\n\r", ch);
      return;
    }

    if (amount<5 || (21*amount/20+1)>ch->pcdata->account) 
    { 
      stc("{RТы не можешь перевести меньше 5 или больше, чем у тебя на счету.{x",ch); 
      return; 
    } 
    if (amount%20!=0) tmp=1;

    ptc(ch,"\n\r{CТы переводишь {Y%u {Cзолота на счет {G%s{C.\n\r",amount,victim->name);
    ptc(ch,"Плата за перевод: {Y%u {C(5%).{x\n\r",amount/20+tmp);
    ch->pcdata->account-=21*amount/20+tmp;
    victim->pcdata->account+=amount;
    ptc(ch,"На счету теперь {Y%u{C.{x\n\r", ch->pcdata->account);
    WILLSAVE(ch);
    return;
  } 
} 
 
int64 toggle_flag(CHAR_DATA *ch,int64 flag,int64 bit, char *text, bool invert) 
{ 
  if(IS_SET(flag,bit)) 
  { 
    ptc(ch,"%s {G%s{x.\n\r",text,(!invert) ? "включ":"выключ" ); 
    REM_BIT(flag,bit); 
    return flag; 
  } 
  SET_BIT(flag,bit); 
  ptc(ch,"%s {R%s{x.\n\r",text,(!invert) ? "выключ":"включ"); 
  return flag; 
} 
 
void fix_keepers(MOB_INDEX_DATA *mob) 
{ 
 mob->level=number_range(99,109); 
 mob->hitroll=mob->level*2; 
 SET_BIT(mob->imm_flags,IMM_SUMMON); 
 SET_BIT(mob->imm_flags,IMM_CHARM); 
 SET_BIT(mob->imm_flags,IMM_WEAPON); 
 SET_BIT(mob->imm_flags,IMM_MAGIC); 
 SET_BIT(mob->act,ACT_NOSTEAL);
 
 mob->damage[0]=mob->level/10; 
 mob->damage[1]=mob->level/5; 
 mob->damage[2]=number_range(mob->level/2,200); 
} 
 
void do_blacksmith (CHAR_DATA *ch, const char *argument) 
{ 
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH]; 
  char notetext[MAX_INPUT_LENGTH];

  if (ch->in_room == NULL)
  {
   bug("Char in NULL room",0);
   char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
  }

  argument=one_argument(argument, arg1);

  if (arg1[0]=='\0')
  {
    stc ("{CКузнец {xможет перековать любой предмет, если у тебя есть\n\r",ch);
    stc ("один из {Cмагических предметов{x.\n\r",ch);
    stc ("Их можно получить от {CБогов{x, или найти в мире..\n\r",ch);
    stc ("{YСинтаксис:{x help\n\r",ch);
    stc ("            {x blacksmith use <artefact> <item>\n\r",ch);
    stc ("{Y          {x blacksmith unequip <char> <item>\n\r",ch);
    return;
  }
 
  if (!str_prefix(arg1, "help"))
  {
    stc ("Основные моменты:\n\r",ch);
    stc ("1.При исключении из гильдии со Старейшины снимается флаг Старейшина.\n\r",ch);
    stc ("2.unequip работает только в специальной комнате (кузнице)\n\r",ch);
    stc ("3.При unequip предмет не теряет никаких флагов.\n\r",ch);
    stc ("4.При unequip предмет оказывается на полу комнаты.\n\r",ch);
    stc ("5.При перековке предмета уровень гнома должен быть равен или выше уровня\n\r  предмета.К уровню артефакта прибавляется 1/6 уровня гнома.\n\r",ch);
    stc ("6.При перековке артефакт толжен быть в инвентори гнома, а перековываемый предмет\n\r  сперва ищется в инвентори, затем на полу комнаты.\n\r",ch);
    stc ("7.Уровень гнома вне гильдии во всех опциях blacksmith делится на два.\n\r",ch);
    return;
  }

  if (!IS_SET(race_table[ch->race].spec,SPEC_BLACKSMITH) && !IS_IMMORTAL(ch))
  {
    stc("Ты не умеешь перековывать предметы\n\r",ch);
    return;
  }

  if (!str_prefix(arg1, "use"))
  {
    OBJ_DATA *objm,*objt;
    int award=0;

    argument=one_argument(argument, arg1);
    argument=one_argument(argument, arg2);

    // Looking for magic object and target object
    objm = get_obj_carry( ch, arg1, ch ); 
    if (objm==NULL) 
    { 
      stc( "У тебя нет магического камня.\n\r",ch); 
      return; 
    } 
    objt = get_obj_carry( ch, arg2, ch ); 
    if (objt==NULL) objt=get_obj_list(ch,arg2,ch->in_room->contents); 
   
    if (objt==NULL) 
    { 
      stc( "У тебя нет этой вещи.\n\r",ch); 
      return; 
    } 
   
    if (objm->item_type!=ITEM_ENCHANT)
    { 
      stc( "Ты не можешь использовать всякий мусор для переделки\n\r",ch); 
      return; 
    } 

    if (objt->item_type==ITEM_ENCHANT)
    { 
      stc( "Ну и нахрена тебе какое-нибуть 'пылающее сердце вампира'?!:)\n\r",ch);
      return; 
    } 

    if (!IS_SET(ch->in_room->room_flags,ROOM_BLACKSMITH))
    {
      stc("Это можно делать только в кузнице.\n\r", ch);
      return;
    }

    if (GUILD(ch, DWARVES_GUILD)) award=1;
    else award=2;

    if (!IS_IMMORTAL(ch) && objt->level > UMIN(ch->level/award + objm->level/6,objm->level + ch->level/(award*6)))
    {
      stc("Твоего умения не хватает, чтобы перековать этот предмет этим артефактом.\n\r",ch);
      return;
    }

    //Note about Blacksmith using. Improved for better Guild control. (c)LastSoul
    if (!IS_IMMORTAL(ch))
    {
      do_printf(notetext,"{GName: {Y%s{G. Level: {Y%d{G,{x\n\rCommand string: {Y%s %s{x\n\r",ch->name,ch->level,arg1,arg2);
      send_note("Белоснежка","elder","{WBlacksmith{x",notetext,0);
    }

    switch (objm->value[0]) 
    { 
      case ENCH_BURNPROOF: 
        SET_BIT(objt->extra_flags,ITEM_BURN_PROOF); 
        break; 
   
      case ENCH_BLESS: 
        {
          AFFECT_DATA af;

          if (IS_SET(objt->extra_flags,ITEM_BLESS))  
          { 
            stc( "Этот предмет уже благословлен.\n\r",ch);
            return;
          } 

          if (IS_SET(objt->extra_flags,ITEM_EVIL)) REM_BIT(objt->extra_flags,ITEM_EVIL);
          else
          {
           SET_BIT(objt->extra_flags,ITEM_BLESS); 
           af.where    = TO_OBJECT;
           af.type     = skill_lookup("bless");
           af.level    = objm->level;
           af.duration = -1;
           af.location = APPLY_SAVES;
           af.modifier = -1 - (objm->level>50?1:0);
           af.bitvector= 0;
           affect_to_obj(objt,&af);
          }
          break;
          
        }
   
      case ENCH_HUM: 
        SET_BIT(objt->extra_flags,ITEM_HUM); 
        break; 
   
      case ENCH_NODROP: 
        SET_BIT(objt->extra_flags,ITEM_NODROP); 
        break; 
   
      case ENCH_NOREMOVE: 
        SET_BIT(objt->extra_flags,ITEM_NOREMOVE); 
        break; 
   
      case ENCH_NOLOCATE: 
        SET_BIT(objt->extra_flags,ITEM_NOLOCATE); 
        break; 
   
      case ENCH_NOUNCURSE: 
        SET_BIT(objt->extra_flags,ITEM_NOUNCURSE); 
        break; 
   
      case ENCH_REMINVIS: 
        REM_BIT(objt->extra_flags,ITEM_INVIS); 
        break; 
      /*
      case ENCH_REMANTIGOOD: 
        REM_BIT(objt->extra_flags,ITEM_ANTI_GOOD); 
        break; 
   
      case ENCH_REMANTINEUTRAL: 
        REM_BIT(objt->extra_flags,ITEM_ANTI_NEUTRAL); 
        break; 
   
      case ENCH_REMANTIEVIL: 
        REM_BIT(objt->extra_flags,ITEM_ANTI_EVIL); 
        break; 
      */
      case ENCH_REMVAMP: 
        if (objt->item_type!=ITEM_WEAPON)  
        { 
          stc( "Ты можешь применить силу этого артефакта только на оружие...\n\r",ch); 
          return; 
        } 
        REM_BIT(objt->value[4],WEAPON_VAMPIRIC); 
        break; 
   
      case ENCH_ADDVAMP: 
        if (objt->item_type!=ITEM_WEAPON)  
        { 
          stc( "Ты можешь применить силу этого артефакта только на оружие...\n\r",ch); 
          return; 
        } 
        SET_BIT(objt->value[4],WEAPON_VAMPIRIC); 
        break; 
      case ENCH_SHARP: 
        if (objt->item_type!=ITEM_WEAPON)  
        { 
          stc( "Ты можешь применить силу этого артефакта только на оружие...\n\r",ch); 
          return; 
        } 
        SET_BIT(objt->value[4],WEAPON_SHARP); 
        break; 
   
      case ENCH_ADDFLAMING: 
        if (objt->item_type!=ITEM_WEAPON)  
        { 
          stc( "Ты можешь применить силу этого артефакта только на оружие.\n\r",ch); 
          return; 
        } 
        SET_BIT(objt->value[4],WEAPON_FLAMING); 
        break; 
   
      case ENCH_REMTWOHAND: 
        if (objt->item_type!=ITEM_WEAPON)  
        { 
          stc( "Ты можешь применить силу этого артефакта только на оружие.\n\r",ch); 
          return; 
        } 
        REM_BIT(objt->value[4],WEAPON_TWO_HANDS); 
        break; 
      case ENCH_SETWEIGHT: 
        objt->weight=0; 
        break; 
      
      case ENCH_NONE: 
      default: 
        stc( "Ты не можешь понять магическую силу артефакта...\n\r",ch); 
        return; 
    } 
    act("$n взмахивает рукой и $i4 на секунду обрамляется ярким светом.",ch,objt,NULL,TO_ROOM); 
    ptc(ch,"Ты вливаешь силу артефакта в %s. Яркий свет на секунду вспыхивает вокруг.\n\r",get_obj_desc(objt, '4')); 
    objt->level+=(int)objm->value[1]; 
    obj_from_char(objm); 
    extract_obj(objm); 
    return;
  }

  if (!str_prefix(arg1, "unequip"))
  {
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int reward=0;
    char buf[MAX_INPUT_LENGTH];

    argument=one_argument(argument, arg1);
    argument=one_argument(argument, arg2);

    if (!IS_SET(ch->in_room->room_flags,ROOM_BLACKSMITH))
    {
      stc("Это можно делать только в кузнице.\n\r", ch);
      return;
    }

    victim=get_char_room(ch, arg1);

    if (!victim)
    {
      stc("Тут нет таких\n\r", ch);
      return;
    }
    obj=get_obj_victim(ch, victim, arg2);
    if (!obj)
    {
      stc("Ты не находишь этого.\n\r", ch);
      return;
    }

    if (GUILD(ch, DWARVES_GUILD)) reward=1;
    else reward=2;

    if (!IS_IMMORTAL(ch) && obj->level> (ch->level/reward ))
    {
      ptc(ch,"Тебе не хватает %d уровней.\n\r",obj->level-(ch->level/reward));
      return;
    }

    if (obj->wear_loc!=WEAR_NONE) unequip_char(victim, obj);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    ptc(ch, "Ты снимаешь {C%s{x с {Y%s{x.\n\r", get_obj_desc(obj, '1'), get_char_desc(victim, '2'));
    ptc(victim, "{Y%s{x снимает {C%s{x с тебя.\n\r", get_char_desc(ch, '1'), get_obj_desc(obj, '4'));
    do_printf(buf, "{Y$c1{x снимает {C%s{x с {Y$C2{x.",get_obj_desc(obj, '4'));
    act(buf, ch, NULL, victim, TO_NOTVICT);
    if (!IS_IMMORTAL(ch)) WAIT_STATE(ch,3*PULSE_VIOLENCE);
    return;
  }
  stc("Набери blacksmith без параметров для справки.\n\r", ch);
} 
 
void do_suicide( CHAR_DATA *ch, const char *argument )
{ 
  char buf[MAX_STRING_LENGTH];

  if (IS_AFFECTED(ch, AFF_CHARM))
  {
     stc("Ты не можешь покинуть своего любимого хозяина.\n\r", ch);
     return;
  }
  if (IS_SET(ch->in_room->room_flags,ROOM_NOSUICIDE) && !IS_IMMORTAL(ch))
  {
    stc("Твой инстинкт самосохранения не дает тебе воткнуть себе в живот кинжал.\n\r",ch);
    WAIT_STATE(ch,PULSE_VIOLENCE);
    return;
  }

  if (!IS_AWAKE(ch))
  {
   stc("Ты же спишь!\n\rТы хочешь умереть от кошмарного сна?\n\r",ch);
   return;
  }
  
  do_printf( buf, "%s сделал себе харакири %s [room %u]",get_char_desc(ch,'1'), 
    ch->in_room ? ch->in_room->name:"NULL", ch->in_room?ch->in_room->vnum:0); 
  log_string(buf);
 
  // Dying penalty: 2/3 way back to previous level. 
  // 1/5 of exp_per_level, if exp<=exp_per_level*level
  if (!IS_NPC(ch) && !IS_SET(ch->act,PLR_ARMY)) 
  { 
    if (ch->exp > exp_per_level(ch,ch->pcdata->points) * ch->level) 
    gain_exp(ch,(2 * (exp_per_level(ch,ch->pcdata->points) * ch->level - ch->exp)/3)+50); 
  }      
 
  if (ch->morph_obj != NULL) 
  { 
    extract_obj(ch->morph_obj); 
  } 
  ch->pcdata->deathcounter+=3;
  raw_kill(ch);
  stc("{RТы окончил жизнь самоубийством. Это cкверно.{x\n\r",ch); 
} 
 
void do_online (CHAR_DATA *ch, const char *argument) 
{ 
 char *class; 
 DESCRIPTOR_DATA *d; 
 bool found = FALSE; 
 CHAR_DATA *wch;
 
 if (argument[0] == '\0') 
 { 
  stc("Введи имя или список имен (разделитель - пробел).\n\r",ch); 
  return; 
 } 
 
 if (!str_cmp(argument, "auto")) 
 { 
   if (ch->pcdata->auto_online) do_online(ch, ch->pcdata->auto_online); 
   else stc("Ваш список пуст.\n\r",ch);
   return; 
 } 
 
 for (d = descriptor_list; d != NULL; d = d->next) 
 { 
  if (d->connected != CON_PLAYING) continue; 
  wch =  d->character; 
  if (IS_NPC(wch) || !can_see(ch,wch,CHECK_LVL)) continue;
  if (!is_name(wch->name, argument) && (!wch->clan || str_cmp(wch->clan->name,argument))) continue;
 
  class = classname(wch);
  switch ( wch->level ) 
   { 
    default: break; 
     { 
      case MAX_LEVEL - 0 : class = "{DIMPLEMENT {x"; break; 
      case MAX_LEVEL - 1 : class = "{DCREATOR   {x"; break; 
      case MAX_LEVEL - 2 : class = "{CSUPERIOR  {x"; break; 
      case MAX_LEVEL - 3 : class = "{CDEITY     {x"; break; 
      case MAX_LEVEL - 4 : class = "{CGOD       {x"; break; 
      case MAX_LEVEL - 5 : class = "{rIMMORTAL  {x"; break; 
      case MAX_LEVEL - 6 : class = "{rDEMIGOD   {x"; break; 
      case MAX_LEVEL - 7 : class = "{rANGEL     {x"; break; 
      case MAX_LEVEL - 8 : class = "{rAVATAR    {x"; break; 
     } 
  } 
 
  ptc(ch,"{C[{Y%3d {x%5s %s{C]{x %s%s%s %s {Y%s {x%s{x\n\r", 
   wch->level, race_wname(wch),
   class, (wch->clan==NULL) ? "" :wch->clan->show_name, 
   IS_SET(wch->comm, COMM_AFK) ? "{c[A]{x" : "", 
   IS_SET(wch->act, PLR_WANTED) ? "{r(W){x" : "", 
   IS_SET(wch->act, PLR_RAPER)  ? "{R(Н){x"  : "", 
   wch->name, wch->pcdata->title); 
   found=TRUE; 
 } 
 if (!found) stc("Никого из перечисленных здесь нет.\n\r",ch); 
} 
 
int check_align(CHAR_DATA *ch) 
{ 
  register int chance=0; 
 
  if (ch->real_alignment==1) 
  { 
    if (ch->alignment>=350) chance = ch->alignment/100; 
    if (ch->alignment<350 && ch->alignment >-350) chance=-5; 
    if (ch->alignment<=-350) chance=ch->alignment/50; 
  } 
 
  if (ch->real_alignment==-1) 
  { 
    if (ch->alignment<=350) chance = ch->alignment/100; 
    if (ch->alignment>-350 && ch->alignment <350) chance=5; 
    if (ch->alignment>=-350) chance=ch->alignment/50; 
    chance=!chance; 
  } 
  return chance; 
} 
 
void do_room(CHAR_DATA *ch, const char *argument) 
{ 
  char arg[MAX_INPUT_LENGTH]; 
   
  argument=one_argument(argument, arg); 
 
  if (arg[0]=='\0') 
  { 
    stc("Usage: room reset    - reload resets in this room.\n\r",ch); 
    stc("       room raffect  - show roomaffects.\n\r",ch);
    stc("       room clear    - clear room affects.\n\r",ch);
    return; 
  } 
 
  if (!str_prefix(arg,"reset")) 
  { 
    reset_room(ch->in_room); 
    return; 
  } 

  if (!str_prefix(arg,"raffect"))
  {
    RAFFECT *ra;

    ptc(ch,"Room Affects in this room:\n\r{GLvl{x.{GTick{x Targ {MFlag  {WName                 {YOwner{x\n\r----------------------------------------------------------\n\r");
    if (ch->in_room->raffect)
    {
      for (ra=ch->in_room->raffect;ra;ra=ra->next_in_room)
        ptc(ch,"{G%3d{x.{G%4d {x%4d {M%5s {W%20s {Y%s{x\n\r",
          ra->level,ra->duration,ra->target,
          pflag64(ra->flag),raff_name(ra->bit),ra->owner?ra->owner:"{DNone");
    }
    return;
  }

  if (!str_prefix(arg,"clear"))
  {
    RAFFECT *ra,*ra_next;

    stc("Clearing raffects...\n\r",ch);
    if (ch->in_room->raffect)
    {
      for (ra=ch->in_room->raffect;ra;ra=ra_next)
      {
        ra_next=ra->next_in_room;
        ptc(ch,"Cleared: %s\n\r",raff_name(ra->bit));
        free_raffect(ra);
      }
    }
    return;
  }
 
  if (is_number(arg)) 
  { 
    reset_room(get_room_index(atoi64(arg))); 
    return; 
  } 
} 
 
void do_spec(CHAR_DATA *ch, const char *argument) 
{ 
 char arg[MAX_INPUT_LENGTH];
 
 if (argument[0]=='\0') 
 { 
   stc("Special commands for raceskills.\n\r",ch); 
   stc("Usage:\n\r",ch); 
   stc("      spec invis\n\r",ch); 
   stc("      spec restore\n\r",ch); 
   stc("      spec mist\n\r",ch); 
   stc("      spec vampiric\n\r",ch); 
   return; 
 } 
 
 argument=one_argument(argument,arg);
 if (!str_prefix(arg,"invis")) 
 { 
   if (!IS_SET(race_table[ch->race].spec,SPEC_INVIS))  
   { 
     stc("Невидимость не является твоей врожденной способностью.",ch); 
     return; 
   } 
   if ( IS_AFFECTED(ch, AFF_INVISIBLE) ) 
   { 
     stc("Ты уже невидим.\n\r",ch); 
     return; 
   } 
   SET_BIT(ch->affected_by,AFF_INVISIBLE); 
   stc( "Ты растворяешься в воздухе.\n\r", ch ); 
   act( "{Y$c2{x Растворяется в воздухе.", ch, NULL, NULL, TO_ROOM ); 
   return; 
 } 
 if (!str_prefix(arg,"restore")) 
 { 
   ch->affected_by=ch->affected_by|race_table[ch->race].aff; 
   stc( "Твои врожденные особенности обновлены.\n\r", ch ); 
   return; 
 } 

 if (!str_prefix(arg,"vampiric"))
 {
   OBJ_DATA *obj;
   AFFECT_DATA af;
   char buf[MAX_INPUT_LENGTH];

   if (!GUILD(ch,VAMPIRE_GUILD))
   {
     stc("Только Вампиры из гильдии вампиров могут это использовать.\n\r",ch);
     return;
   }
   if (!(obj=get_obj_here(ch,argument)))
   {
     stc ("Предмет не найден.\n\r",ch);
     return;
   }

   if (obj->item_type!=ITEM_WEAPON)
   {
     stc ("Это должно быть оружием.\n\r",ch);
     return;
   }

   if (IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC))
   {
     ptc (ch,"%s уже содержит ЗЛО.\n\r",get_obj_desc(obj,'1'));
     return;
   }

   af.where     = TO_WEAPON;
   af.type      = skill_lookup("enchant weapon");
   af.level     = ch->level;
   af.duration  = ELDER(ch,VAMPIRE_GUILD) ? ch->level*2:ch->level/2;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = WEAPON_VAMPIRIC;
   affect_to_obj(obj,&af);
   ptc(ch,"Ты вкладываешь в {C%s{x свою силу.\n\r",get_obj_desc(obj,'4'));
   do_printf(buf,"{Y$n{x внимательно смотрит на %s, его глаза вспыхивают красным",get_obj_desc(obj,'4'));
   act(buf,ch,NULL,NULL,TO_ROOM);
   return;
 }
} 
 
void do_referi(CHAR_DATA *ch, const char *argument) 
{ 
  CHAR_DATA *referi; 
  char buf[MAX_INPUT_LENGTH]; 
 
  if (argument[0]=='\0') 
  { 
    stc("{RSyntax:{x referi affects\n\r", ch); 
    return; 
  } 
 
  for (referi=ch->in_room->people;referi!=NULL;referi=referi->next_in_room) 
    if (IS_NPC(referi) && IS_SET(referi->act, ACT_REFERI)) break; 
 
  if (!referi || !IS_NPC(referi) || !IS_SET(referi->act, ACT_REFERI)) 
  { 
    stc("Тут нет рефери.\n\r", ch); 
    return; 
  } 
 
  if (!str_prefix(argument, "show")) 
  { 
    AFFECT_DATA *paf; 
    int paf_last=0; 
 
    if ( ch->affected != NULL ) 
    { 
      act("{Y$c1{x произносит '{G$C1 находится под влианием следующих заклинаний:{x'",referi,NULL,ch,TO_ALL_IN_ROOM); 
      for ( paf = ch->affected; paf != NULL; paf = paf->next ) 
      { 
        if (paf_last==paf->type) continue; 

        do_printf(buf, "{CЗаклинание : {W%20s {Cуровня {W%d{x",
        (paf->type < 0)?"{CSpellaffect{x":skill_table[paf->type].name, paf->level); 
        act(buf, ch, NULL, NULL, TO_ALL_IN_ROOM); 
        paf_last=paf->type; 
      } 
    } 
    else act("{Y$c1{x произносит '{G$C1 не находится под влиянием заклинаний{x'",referi,NULL,ch,TO_ALL_IN_ROOM); 
    return; 
  } 
 
  if (!str_prefix(argument, "cancel")) 
  { 
    return; 
  } 
 
  stc("Usage: refery show\n\r", ch); 
} 

#define ispath(x) ( x=='d' || x=='s' || x=='u' || x=='n' || x=='w' || x=='e' || x=='D' || x=='S' || x=='U' || x=='N' || x=='W' || x=='E' )

void do_run(CHAR_DATA *ch, const char *argument)
{
  int arg,maxmoves=0;
  static char runbuf[MAX_INPUT_LENGTH+2];
  char local[20];
  bool fPath=FALSE;

  if (EMPTY(argument))
  {
    stc("Синтаксис: run [...путь...]\n\r",ch);
    stc("Указывая путь, используйте буквы 's n w e u d' с ведущими цифрами.\n\r",ch);
    stc("Пробелы игнорируются, остальные символы будут вызывать ошибку.\n\r",ch);
    stc("Примеры: run 2e3n\n\r         run 4e 2s e e 3n2sww\n\r",ch);
    return;
  }
  do_printf(runbuf,"");

  if ( IS_AFFECTED(ch,AFF_HIDE) )
   REM_BIT(ch->affected_by,AFF_HIDE);

  for (arg=0;argument[arg];arg++)
  {
    if ( !ispath(argument[arg]) && !isspace(argument[arg]) && !isnum(argument[arg]))
    {
      ptc(ch,"Ошибочный символ '%c' в строке пути.\n\r",argument[arg]);
      return;
    }
    if (isspace(argument[arg])) continue;
    if (isnum(argument[arg]))
    {
      int64 num=0;

      while (isnum(argument[arg]))
      {
        if (num>0) num*=10;
        num+=argument[arg]-48;
        arg++;
      }

      while (num>0)
      {
       do_printf(local,"%c",argument[arg]);
       strcat(runbuf,local);
       num--;
       fPath=TRUE;
       if ((maxmoves++) >198)
       {
         stc("Слишком далеко бежать.\n\r",ch);
         return;
       }
      }
    }
    else
    {
      do_printf(local,"%c",argument[arg]);
      strcat(runbuf,local);
      fPath=TRUE;
      if ((maxmoves++) >198)
      {
        stc("Слишком далеко бежать.\n\r",ch);
        return;
      }
    }
  }
  if (!fPath)
  {
    stc ("Вы не указали куда нужно бежать.\n\r",ch);
    return;
  }
  free_string(ch->runbuf);
  ch->runbuf=str_dup(runbuf);
  stc("Ты начинаешь бежать.\n\r",ch);
//  ptc(ch,"%s",ch->runbuf);
}

void run(CHAR_DATA *ch)
{
  static char runbuf[300];
  int door=0;
  bool permanent=FALSE;

  if (ch->runbuf[0]=='\0' || ch->fighting!=NULL)
  {
    free_string(ch->runbuf);
    ch->runbuf=&str_empty[0];
    if (ch->fighting!=NULL) stc("Твой путь был прерван.\n\r",ch);
    return;
  }
       if (ch->runbuf[0]=='n') door=0;
  else if (ch->runbuf[0]=='e') door=1;
  else if (ch->runbuf[0]=='s') door=2;
  else if (ch->runbuf[0]=='w') door=3;
  else if (ch->runbuf[0]=='u') door=4;
  else if (ch->runbuf[0]=='d') door=5;
  else if (ch->runbuf[0]=='N') { door=0; permanent=TRUE; }
  else if (ch->runbuf[0]=='E') { door=1; permanent=TRUE; }
  else if (ch->runbuf[0]=='S') { door=2; permanent=TRUE; }
  else if (ch->runbuf[0]=='W') { door=3; permanent=TRUE; }
  else if (ch->runbuf[0]=='U') { door=4; permanent=TRUE; }
  else if (ch->runbuf[0]=='D') { door=5; permanent=TRUE; }

  if (permanent)
  {
    if ( ++ch->maxrun[1] > ch->maxrun[0] )
    {
      free_string(ch->runbuf);
      ch->runbuf=&str_empty[0];
      stc("Ты начинаешь задыхаться и останавливаешься.\n\r",ch);
      ch->maxrun[1]=0;
      return;
    }
    if (do_move_char( ch, door, TRUE, TRUE )==FALSE)
    {
      do_printf(runbuf,ch->runbuf);
      free_string(ch->runbuf);
      ch->runbuf=str_dup(runbuf+1);
      ch->maxrun[1]=0;
    }
    return;
  }

  if (do_move_char( ch, door, TRUE, TRUE )==FALSE)
  {
    free_string(ch->runbuf);
    ch->runbuf=&str_empty[0];
    stc("Твой путь был прерван.\n\r",ch);
    return;
  }
  do_printf(runbuf,ch->runbuf);
  free_string(ch->runbuf);
  ch->runbuf=str_dup(runbuf+1);
}

void astat( CHAR_DATA *ch, const char *arg )
{
  CHAR_DATA *wch;

  wch = get_char_world(ch,arg);
  if (!wch)
  {
    stc("Персонаж не найден.\n\r", ch);
    return;
  }

 stc("{g/=---------------------------{CAdvanced  Statistic{g---------------------------=/{x\n\r",ch);
 ptc( ch, "{g|{x [{Y%3d{x] %s {G(%s)  {YRoom: {x%u{x\n\r",
  wch->level,wch->name,race_table[wch->race].name,
  wch->in_room==NULL ? 0 : wch->in_room->vnum);
 ptc( ch, "{g| {YClan: {x%s  {YClanrank: {x%d \n\r", 
  wch->clan==NULL? "{D - none -{x ": wch->clan->show_name,
  wch->clanrank);
 stc("{g=---------------------------------------------------------------------------={x\n\r",ch);

 ptc(ch, "{g| {GShort desc:{x %s{x\n\r", get_char_desc(wch, '1'));
 ptc(ch, "{g| {GLong desc :{x %s{x",
  wch->long_descr[0] != '\0' ? wch->long_descr :"{D(none){x" );

 if ( IS_NPC(wch) && wch->spec_fun != 0 )
  ptc(ch,"{g| {MMobile has special procedure {x%s{x\n\r",spec_name(wch->spec_fun));

 if (!IS_NPC(wch))
 {
   ptc(ch, "{g| {GTrained {GHp: {x%5d  {CMana: {x%5d   {RDeathCounter: {x%d\n\r",
    wch->pcdata->hptrained,
    wch->pcdata->manatrained, 
    wch->pcdata->deathcounter);

   ptc(ch,"{g| {CClass:(%s %s %s %s{C) {Gremorts: {x%d \n\r", 
    class_table[wch->class[0]].name,
    wch->remort>0 ? class_table[wch->class[1]].name:"",
    wch->remort>1 ? class_table[wch->class[2]].name:"",
    wch->remort>2 ? class_table[wch->class[3]].name:"",
    wch->remort);
   ptc(ch,"{g| {CMag:%s Cle:%s War:%s Thi:%s\n\r",
    wch->classmag?"yes":"no ",wch->classcle?"yes":"no ",
    wch->classwar?"yes":"no ",wch->classthi?"yes":"no ");

   ptc(ch,"{g| {GReply to  {Y%s{x\n\r",(wch->reply==NULL)?"none":wch->reply->name);
   ptc(ch,"{g| {RCriminal: {x%d\n\r",ch->criminal);
   ptc(ch,"{g| {RПроклятие: {x%d\n\r",ch->godcurse);
   ptc (ch,"{g| {Y%s{x %sявляется %s Гильдии Гномов.\n\r",wch->name,    GUILD(wch, DWARVES_GUILD)?"":"не ",ELDER(wch,DWARVES_GUILD)?"старейшиной":"кузнецом");
   ptc (ch,"{g| {Y%s{x %sявляется %s Гильдии Ассасинов.\n\r",wch->name, GUILD(wch, ASSASIN_GUILD)?"":"не ",ELDER(wch,DWARVES_GUILD)?"мастером":"членом");
   ptc (ch,"{g| {Y%s{x %sявляется %s Гильдии Вампиров.\n\r",wch->name,  GUILD(wch, VAMPIRE_GUILD)?"":"не ",ELDER(wch,DWARVES_GUILD)?"высшим вампиром":"членом");
   ptc (ch,"{g| {Y%s{x %sявляется %s Гильдии Вампиров.\n\r",wch->name,  GUILD(wch, DRUIDS_GUILD)?"":"не ",ELDER(wch,DRUIDS_GUILD)?"старейшиной":"послушником");
   ptc (ch,"{g| {GСемейное положение:[{Y%s{G]\n\r",EMPTY(wch->pcdata->marry)?"none":wch->pcdata->marry);
   ptc (ch,"{g| {GПомолвленость: [{Y%s{G]\n\r",wch->pcdata->lovers);
   ptc (ch,"{g| {GПредложение: [{Y%s{G]\n\r",wch->pcdata->propose_marry);
 }
 stc("{g=---------------------------------------------------------------------------={x\n\r",ch);
}

int64 sell_gem(CHAR_DATA *ch, int64 amount, int64 vnum)
{
  OBJ_DATA *obj, *obj_next;
  OBJ_INDEX_DATA *gem;
  int64 count=0;

  gem=get_obj_index(vnum);
  if (!gem) return 0;
  
  for(obj=ch->carrying;obj!=NULL;obj=obj_next)
  {
    obj_next=obj->next_content;
    if (obj->pIndexData->vnum==vnum)
    {
      if (amount<=0) break;
      obj_from_char(obj);
      extract_obj(obj);
      amount--;
      count++;
    }
  }
  return count;
}

void remove_penalty(CHAR_DATA *ch, PENALTY_DATA *penalty)
{
  PENALTY_DATA *pnext;

  if (!ch->penalty)
  {
    log_string("BUG:Trying to remove NULL penalty");
    return;
  }

  if (penalty==ch->penalty) ch->penalty=penalty->next;
  else for (pnext=ch->penalty;pnext;pnext=pnext->next)
  {
    if (pnext->next==penalty)
    {
      pnext->next=penalty->next;
      break;
    }
  }

  // remove affect
  switch(penalty->affect)
  {
    case P_NOCHANNELS:
      REM_BIT(ch->comm, COMM_NOCHANNELS);
      stc("\n\r{YКаналы {Gвключены.{x\n\r",ch);
      break;
    case P_NOPOST:
      REM_BIT(ch->act, PLR_NOPOST);
      stc ("\n\r{YТы снова можешь писать сообщения.{x\n\r",ch);
      break;
    case P_NOMLOVE:
      REM_BIT(ch->act, PLR_NOMLOVE);
      stc ("\n\r{GРежим NoMlove выключен.{x\n\r",ch);
      break;
    case P_NOGSOCIAL:
      REM_BIT(ch->comm,COMM_NOGSOC);
      stc ("\n\r{YGSocials {Gвключены.{x\n\r",ch);
      break;
    case P_NOSTALGIA:
      stc ("\n\r{CВот тебя и потянуло на {WРодину{x\n\r", ch);
      break;
    case P_GODCURSE:
      stc ("\n\r{CБожественное проклятье покидает тебя.{x\n\r", ch);
      ch->godcurse=0;
      break;
    case P_FREEZE:
      stc ("\n\r{RТы {Gразморожен{R!{x\n\r", ch);
      REM_BIT(ch->act, PLR_FREEZE);
      break;
    case P_NOEMOTE:
      stc ("\n\r{YЭмоции {G включены.{x\n\r",ch);        
      REM_BIT(ch->comm, COMM_NOEMOTE);
      break;
    case P_TIPSY:
      REM_BIT(ch->act,PLR_TIPSY);
      stc("\n\r{GНаконец-то ты проспался. Ну и как оно теперь? Похмелье не мучит?{x\n\r",ch);
      break;
    case P_NOTELL:
      REM_BIT(ch->comm, COMM_NOTELL);
      stc ("\n\r{GДар речи вернулся к тебе!{x\n\r",ch);
      break;
    case P_NODELETE:
      REM_BIT(ch->pcdata->cfg,CFG_NODELETE);
      stc ("\n\r{RРежим nodelete снят!{x\n\r",ch);
      break;  
    default:
      break;
  }
  free_penalty(penalty);
}

void do_add_penalty(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH],arg1[MAX_INPUT_LENGTH];
  PENALTY_DATA *penalty,pnew;
  CHAR_DATA *victim;
  int64 type;

  if (EMPTY(argument) || !str_cmp(argument,"help"))
  {
    stc("Syntax:addpenalty list                  - show penalty types{x\n\r",ch); 
    stc("       addpenalty remove {Y<char>{x [<num>] - remove penalty from char{x\n\r",ch); 
    stc("       addpenalty show   {Y<char>{x         - show penalty on char{x\n\r",ch); 
    stc("       addpenalty {Y<char>{x <penalty> <ticks amount> [<value>]\n\r", ch);
    return;
  }

  if (!str_cmp(argument, "list")) 
  {
    stc("{Cfreeze    {G- заморозить персонажа{x\n\r", ch);
    stc("{Cgodcurse  {G- божественное проклятье <level>{x\n\r", ch);
    stc("{Cnochannel {G- выключить каналы{x\n\r", ch);
    stc("{Cnodelete  {G- запретить использовать команду delete{x\n\r", ch);
    stc("{Cnoemote   {G- запретить использовать эмоции{x\n\r", ch);
    stc("{Cnogsocial {G- запретить глобальные социалы{x\n\r", ch);
    stc("{Cnomlove   {G- запретить команду mlove{x\n\r", ch);
    stc("{Cnopost    {G- запретить писать сообщения{x\n\r", ch);
    stc("{Cnostalgia {G- злая-злая вещь..{x\n\r", ch);
    stc("{Cnotell    {G- запретить использовать tell{x\n\r", ch);
    stc("{Ctipsy     {G- повесить флаг Пьяного{x\n\r", ch);
    return;
  }

  argument=one_argument(argument, arg);
  argument=one_argument(argument, arg1);

  if (!str_cmp(arg,"show"))
  {
    int cnt=1;
    victim=get_pchar_world(ch,arg1);

    if (!victim)
    {
      stc("Персонаж не найден.\n\r",ch);
      return;
    }

    if (!victim->penalty)
    {
      ptc(ch,"У %s нет штрафов.\n\r",victim->name);
      return;
    }

    for(penalty=victim->penalty;penalty;penalty=penalty->next)
    {
      switch(penalty->type)
      {
        case PENALTY_TICK:
          ptc(ch,"%2d. {R%10s{x set for {G%u{x ticks by {Y%s{x\n\r",cnt++,
            flag_string(penalty_flags,penalty->affect),
            penalty->ticks,penalty->inquisitor?penalty->inquisitor:"{Dнекто");
          break;
        case PENALTY_PERMANENT:
          ptc(ch,"%2d. {R%10s{x set permanently by {Y%s{x\n\r",cnt++,
            flag_string(penalty_flags,penalty->affect),
            penalty->inquisitor?penalty->inquisitor:"{Dнекто");
          break;
        case PENALTY_DATE:
        default:
          stc("Unknown type\n\r",ch);
          break;
      }
    }
    return;
  }

  if (!str_cmp(arg,"remove"))
  {
    int cnt=1, number;
    victim=get_pchar_world(ch, arg1);

    if (EMPTY(argument) || !is_number(argument))
    {
      stc("Укажите номер пенальти для удаления.\n\r",ch);
      return;
    }
    number=atoi(argument);
    if (!victim)
    {
      ptc(ch,"Персонаж не найден.\n\r",victim->name);
      return;
    }

    if (!victim->penalty)
    {
      ptc(ch,"У %s нет штрафов.\n\r",victim->name);
      return;
    }
    for(penalty=victim->penalty;penalty;penalty=penalty->next)
    {
      if (cnt++==number)
      {
        ptc(ch,"%s удалена с %s.\n\r",flag_string(penalty_flags,penalty->affect),victim->name);
        remove_penalty(victim,penalty);
        return;
      }
    }
    stc("Укажите правильный номер пенальти для удаления.\n\r",ch);
    return;
  }

  victim=get_pchar_world(ch, arg);

  if (!victim)
  {
    stc("Персонаж не найден.\n\r", ch);
    return;
  }

  if( get_trust(victim) >= get_trust(ch) || victim->pcdata->protect > get_trust(ch) )
  {
    stc("Не вышло.\n\r", ch);
    return;
  }

  type=flag_lookup(arg1,penalty_flags);
  if (type==NO_FLAG)
  {
    do_add_penalty(ch, "");
    return;
  }

  argument=one_argument(argument, arg);
  if (!is_number(arg))
  {
    do_add_penalty(ch, "");
    return;
  }

  pnew.affect=type;
  pnew.ticks=atoi(arg);
  if (pnew.ticks==0) pnew.type=PENALTY_PERMANENT;
  else pnew.type=PENALTY_TICK;
  pnew.inquisitor=str_dup(ch->name);
  ptc(ch,"{Y%s{x добавлено {R%s{x на {G%u{x тиков.\n\r",victim->name,
    flag_string(penalty_flags,type),pnew.ticks);
  add_penalty(victim, &pnew, atoi(argument));
}

void add_penalty(CHAR_DATA *ch, PENALTY_DATA *penalty, int value)
{
  PENALTY_DATA *pnew;
  AFFECT_DATA af;
  bool found=FALSE;

  for(pnew=ch->penalty;pnew;pnew=pnew->next)
  {
    if (pnew->type==penalty->type && pnew->affect==penalty->affect)
    {
      found=TRUE;
      break;
    }
  }
  if (!found)
  {
    pnew=new_penalty();
    pnew->next=ch->penalty;
    ch->penalty=pnew;
    pnew->type=penalty->type;
    pnew->affect=penalty->affect;
  }
  pnew->ticks=penalty->ticks;
  free_string(pnew->inquisitor);
  pnew->inquisitor=str_dup(penalty->inquisitor);

  switch(penalty->affect)
  {
    case P_NOCHANNELS:
      SET_BIT(ch->comm, COMM_NOCHANNELS);
      ptc(ch, "\n\r{YКаналы {Rотключены на {W%d{R тиков.{x\n\r", penalty->ticks);
      break;
    case P_NOPOST:
      SET_BIT(ch->act, PLR_NOPOST);
      ptc (ch, "\n\r{YPost {Rотключен на {W%d {Rтиков.{x\n\r", penalty->ticks);
      break;
    case P_NOMLOVE:
      SET_BIT(ch->act, PLR_NOMLOVE);
      ptc (ch, "\n\r{Rвключен {Yрежим полового воздержания {Rна {W%d {Rтиков.{x\n\r",penalty->ticks);
      break;
    case P_NOGSOCIAL:
      SET_BIT(ch->comm,COMM_NOGSOC);
      ptc (ch, "\n\r{YGSocials {R отключены на {W%d {Rтиков.{x\n\r",penalty->ticks);
      break;
    case P_NOSTALGIA:
      if (found)
      {
        AFFECT_DATA *aff=affect_find(ch->affected,skill_lookup("nostalgia"));
        aff->duration=(int)penalty->ticks;
        aff->level=110;
        ptc (ch, "\n\r{RТы затосковал по Родине. \n\rЧерез {W%d {Rтиков{G %s тебе поможет{x\n\r", penalty->ticks,SabAdron);
        break;
      }
      af.where  = TO_AFFECTS;
      af.type   = skill_lookup("nostalgia");
      af.level  = 110;
      af.duration  = (int)penalty->ticks;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_NOSTALGIA;
      affect_to_char(ch,&af);
      ptc (ch, "\n\r{RТы затосковал по Родине. \n\rЧерез {W%d {Rтиков{G %s тебе поможет{x\n\r", penalty->ticks,SabAdron);
      break;
    case P_GODCURSE:
      ptc (ch,"\n\r{RБоги прокляли тебя на {G%d {Rтиков{x\n\r", penalty->ticks);
      ch->godcurse=value;
      break;
    case P_FREEZE:
      ptc (ch,"\n\r{RТы заморожен на {G%d{R тиков.{x\n\r", penalty->ticks);
      SET_BIT(ch->act, PLR_FREEZE);
      break;
    case P_NOEMOTE:
      ptc (ch, "\n\r{YЭмоции {R отключены на {W%d {Rтиков.{x\n\r",penalty->ticks);        
      SET_BIT(ch->comm, COMM_NOEMOTE);
      break;
    case P_TIPSY:
      SET_BIT(ch->act,PLR_TIPSY);
      tipsy(ch,"begin");
      ptc (ch, "{RИ быть тебе ещё таким {G%d {Rтиков{x\n\r",penalty->ticks);
      break;
    case P_NOTELL:
      SET_BIT(ch->comm, COMM_NOTELL);
      ptc (ch, "\n\r{RБоги отняли у тебя дар речи на {G%d {Rтиков{x\n\r",penalty->ticks);
      break;
    case P_NODELETE:
      SET_BIT(ch->pcdata->cfg,CFG_NODELETE);
      ptc (ch, "\n\r{RБоги волнуются за твою жизнь. Nodelete на {G%d {Rтиков{x\n\r",penalty->ticks);
      break;  
    default:
      break;
  }
}

void do_vote(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  VOTE_DATA *vote;
  int count, newcount;

  argument=one_argument(argument,arg);

  if(ch->level<21 && ((int)(ch->played + current_time - ch->logon) / 3600) < 50
    && ch->remort==0 && !IS_SET( global_cfg, CFG_GTFIX) )
  {
    stc("Ты недостаточно авторитетен, чтоб голосовать.\n\r",ch);
    return;
  }

  if (EMPTY(arg))
  {
    stc("Текущие темы:\n\r",ch);
    count=1;
    for(vote=vote_list;vote;vote=vote->next)
    {
      VOTE_CHAR *vch;
      int countyes=0,countno=0,counter=0;

      if (!IS_IMMORTAL(ch) && !EMPTY(vote->owner))
      {
        if(!ch->clan || str_cmp(vote->owner,ch->clan->name)) continue;
      }

      if (!EMPTY(vote->owner)) ptc(ch,"{g[{c%s{g]",vote->owner);
      ptc(ch,"{D[{Y%d{D]{C %s\n\r",count,vote->msg);
      for (vch=vote->chars;vch;vch=vch->next)
      {
        if (vch->voting==VOTE_YES) countyes++;
        if (vch->voting==VOTE_NO)  countno++;
        counter++;
      }
      ptc(ch,"{GЗа:[%3d] {RНет:[%3d] {DВозд:[%3d] {xВсего:[%3d]\n\r",countyes,countno,counter-countyes-countno,counter);
      ptc(ch,"Твое слово: [%s].\n\r\n\r",char_vote(ch,vote));
      count++;
    }
    if (count==1) stc("Не обнаружено.\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"help"))
  {
    stc("Синтаксис: vote help               - эта справка.\n\r",ch);
    stc("           vote                    - список тем.\n\r",ch);
    stc("           vote new                - список новых тем.\n\r",ch);
    stc("           vote <num> <yes|no|hz>  - голосовать за/против/воздержаться.\n\r",ch);
    if (IS_DEITY(ch)) 
       stc("           vote <num> list  - просмотреть список проголосовавших.\n\r",ch);
    if (IS_IMMORTAL(ch)) 
       stc("           vote <num> restring <newtext> - сменить тему голосования.\n\r",ch);
    if (IS_IMMORTAL(ch)) 
       stc("           vote add <text>    - добавить тему для голосования.\n\r",ch);
    if (IS_IMMORTAL(ch)) 
       stc("           vote add <clan_name> <text>    - добавить тему для голосования.\n\r",ch);
    if (IS_IMMORTAL(ch)) 
       stc("           vote remove <num>  - убрать тему из списка.\n\r",ch);
    stc(" Для участия в голосовании ты должен достичь совершеннолетия (21 level).\n\r",ch);
    return;
  }

  if (! str_prefix(arg, "new"))
  {
    stc("Новые темы для голосования:\n\r",ch);
    count=1;
    newcount = 0;
    for(vote=vote_list;vote;vote=vote->next)
    {
      VOTE_CHAR *vch;
      int countyes=0, countno=0, counter=0, bIsNewVote = 1;

      if (!IS_IMMORTAL(ch) && !EMPTY(vote->owner))
      {
        if(!ch->clan || str_cmp(vote->owner,ch->clan->name)) continue;
      }
      for (vch=vote->chars;vch;vch=vch->next)
      {
        if (! strcmp(vch->name, ch->name))
          bIsNewVote = 0;
        if (vch->voting==VOTE_YES) countyes++;
        if (vch->voting==VOTE_NO)  countno++;
        counter++;
      }
      if (bIsNewVote)
      {
        if (!EMPTY(vote->owner)) ptc(ch,"{g[{c%s{g]",vote->owner);
        ptc(ch,"{D[{Y%d{D]{C %s\n\r",count,vote->msg);
        ptc(ch,"{GЗа:[%3d] {RНет:[%3d] {DВозд:[%3d] {xВсего:[%3d]\n\r\n\r",countyes,countno,counter-countyes-countno,counter);
        newcount++;
      }
      count++;
    }
    if (! newcount) stc("Не обнаружено.\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"add") && (IS_IMMORTAL(ch) ||ch->clanrank>=4))
  {
    CLAN_DATA *clan;

    vote=new_vote();
    one_argument(argument,arg);
    if (IS_IMMORTAL(ch))
    {
      if ((clan=clan_lookup(arg))!=NULL)
      {
        vote->owner=str_dup(clan->name);
        argument=one_argument(argument,arg);
      }
    }
    else vote->owner=str_dup(ch->clan->name);
    vote->msg=str_dup(argument);
    save_vote();
    ptc(ch, "Новая тема для голосования добавлена: {C%s{x.\n\r", arg);
    return;
  }

  if ((IS_IMMORTAL(ch) ||ch->clanrank>=4) && !str_prefix(arg,"remove"))
  {
    argument = one_argument(argument,arg2);
    if (!is_number(arg2))
    {
      stc("Укажите номер.\n\r",ch);
      return;
    }
    count=atoi(arg2);
    for (vote=vote_list;vote;vote=vote->next)
    {
      if (count==1)
      {
        if (!IS_IMMORTAL(ch) && (EMPTY(vote->owner) 
         || str_cmp(vote->owner,ch->clan->name)))
        {
          stc ("Ты не имеешь прав, чтобы удалить этот пункт.\n\r",ch);
          return;
        }
        free_vote(vote);
        save_vote();
        stc("Ok.\n\r",ch);
        return;
      }
      if (!IS_IMMORTAL(ch) && !EMPTY(vote->owner))
      {
        if(!ch->clan || str_cmp(vote->owner,ch->clan->name)) continue;
      }
      count--;
    }
    stc("В списке нет столько пунктов.\n\r",ch);
    return;
  }

  if (is_number(arg))
  {
    argument = one_argument(argument,arg2);
    count=atoi(arg);
    for (vote=vote_list;vote;vote=vote->next)
    {
      if (!IS_IMMORTAL(ch) && !EMPTY(vote->owner))
      {
        if(!ch->clan || str_cmp(vote->owner,ch->clan->name)) continue;
      }
      if (count==1)
      {
        if (EMPTY(arg2)) remove_voting_char(ch,vote);
        else if (!str_prefix(arg2,"list") && (get_trust(ch) >= 107) )
        { 
          VOTE_CHAR *vch;
          int countyes=0,countno=0,counter=0;

          ptc(ch,"Голосование по теме %s:\n\r",vote->msg);
          for (vch=vote->chars;vch;vch=vch->next)
          {
            ptc(ch,"[%3d] %12s (%3dлет) --> [%s]\n\r",vch->level,vch->name,
              vch->age, vote_table[vch->voting]);
            if (vch->voting==VOTE_YES) countyes++;
            if (vch->voting==VOTE_NO)  countno++;
            counter++;
          }
          ptc(ch,"\n\r{GЗА [%3d]  {RПРОТИВ [%3d]  {DВоздержалось[%3d]{x\n\r",countyes,countno,counter-countyes-countno);
          ptc(ch,"Принимало участие в голосовании %d игроков.\n\r",counter);
          return;
        }
        else if( !str_cmp(arg2,"restring" ) && IS_IMMORTAL(ch) )
        {
          ptc( ch, "Текст по голосованию {w'{C%s{w'{x\n\rИзменен на : ", vote->msg );
          vote->msg = str_dup(argument);
          save_vote();
          ptc(ch, "{w'{C%s{w'{x\n\r", vote->msg );
          return;
        }
        else if (!str_prefix(arg2,"yes"))char_voting(ch,vote,VOTE_YES);
        else if (!str_prefix(arg2,"no")) char_voting(ch,vote,VOTE_NO);
        else if (!str_prefix(arg2,"hz")) char_voting(ch,vote,VOTE_HZ);
        else do_vote(ch,"help"); // remove_voting_char(ch,vote);
        save_vote();
        return;
      }
      count--;
    }
    stc("В списке нет столько пунктов.\n\r",ch);
    return;
  }
  do_vote(ch,"help");
}

char *char_vote(CHAR_DATA *ch,VOTE_DATA *vote)
{
  VOTE_CHAR *vch;

  for (vch=vote->chars;vch;vch=vch->next)
  {
    if (str_cmp(ch->name,vch->name)) continue;
    return vote_table[vch->voting];
  }
  return vote_table[0];
}

void char_voting(CHAR_DATA *ch,VOTE_DATA *vote,int type)
{
  VOTE_CHAR *vch;

  if (vote->chars)
   for (vch=vote->chars;vch;vch=vch->next)
   {
     if (!str_cmp(vch->name,ch->name))
     {
       vch->voting=type;
       ptc(ch,"Ты голосуешь %s.\n\r",vote_table[type]);
       return;
     }
   }
  vch=new_vote_char();
  vch->next=vote->chars;
  vote->chars=vch;
  vch->name=str_dup(ch->name);
  vch->level=ch->level;
  vch->age=get_age(ch);
  vch->voting=type;
  ptc(ch,"Ты проголосовал %s.\n\r",vote_table[type]);
}

void remove_voting_char(CHAR_DATA *ch,VOTE_DATA *vote)
{
  VOTE_CHAR *vch=NULL, *tmp;

  if (!vote->chars) return;

  for (tmp=vote->chars;tmp;tmp=tmp->next)
  {
    if (!str_cmp(tmp->name,ch->name))
    {
      if (vch) vch->next=tmp->next;
      if (vote->chars==tmp) vote->chars=tmp->next;
      free_vote_char(tmp);
      return;
    }
    vch=tmp;
  }
}

/*
 Note types: NOTE 0     IDEA    1  PENALTY    2
             NEWS 3     CHANGES 4  BUGREPORT  5 OFFTOPIC 6
*/
void send_note( const char *from, const char *to, const char *subject,
                const char *text, int type)
{
  NOTE_DATA *note;
  char *buf;

  note = new_note();
  note->next         = NULL;
  note->sender       = str_dup(from);
  note->host         = str_dup("{GSystem{x");
  buf                = ctime( &current_time );
  buf[strlen(buf)-1] = '\0';
  note->date         = str_dup( buf );
  note->date_stamp   = current_time;
  note->to_list      = str_dup(to);
  note->subject      = str_dup(subject);
  note->text         = str_dup(text);
  note->type         = type;
  append_note(note);
}

void do_flag(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH],arg3[MAX_INPUT_LENGTH];
  char word[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char type;

  int64 old=0, new=0, marked=0, pos, *flag=NULL;

  const struct flag_type *flag_table=NULL;

  argument = one_argument(argument,arg1);
  argument = one_argument(argument,arg2);
  argument = one_argument(argument,arg3);

  type = argument[0];

  if (type == '=' || type == '-' || type == '+')
      argument = one_argument(argument,word);

  if (arg1[0] == '\0')
  {
      stc("Syntax:\n\r",ch);
      stc("  flag mob  <name> <field> <flags>\n\r",ch);
      stc("  flag char <name> <field> <flags>\n\r",ch);
      stc("  mob  flags: act,off,imm,res,vuln,form\n\r",ch);
      stc("  char flags: plr,comm,aff,imm,res,vuln,\n\r",ch);
      stc("  +: add flag, -: remove flag, = set equal to\n\r",ch);
      stc("  otherwise flag toggles the flags listed.\n\r",ch);
      return;
  }

  if (arg2[0] == '\0')
  {
      stc("What do you wish to set flags on?\n\r",ch);
      return;
  }

  if (arg3[0] == '\0')
  {
      stc("You need to specify a flag to set.\n\r",ch);
      return;
  }

  if (argument[0] == '\0')
  {
      stc("Which flags do you wish to change?\n\r",ch);
      return;
  }

  if (!str_prefix(arg1,"mob") || !str_prefix(arg1,"char"))
  {
    victim = get_char_world(ch,arg2);
    if (victim == NULL)
    {
      stc("You can't find them.\n\r",ch);
      return;
    }

    /* select a flag to set */
    if (!str_prefix(arg3,"act"))
    {
      if (!IS_NPC(victim))
      {
          stc("Use plr for PCs.\n\r",ch);
          return;
      }

      flag = &victim->act;
      flag_table = act_flags;
    }

    else if (!str_prefix(arg3,"plr"))
    {
      if (IS_NPC(victim))
      {
          stc("Use act for NPCs.\n\r",ch);
          return;
      }

      flag = &victim->act;
      flag_table = plr_flags;
    }

    else if (!str_prefix(arg3,"aff"))
    {
      flag = &victim->affected_by;
      flag_table = affect_flags;
    }

    else if (!str_prefix(arg3,"immunity"))
    {
      flag = &victim->imm_flags;
      flag_table = imm_flags;
    }

    else if (!str_prefix(arg3,"resist"))
    {
      flag = &victim->res_flags;
      flag_table = imm_flags;
    }

    else if (!str_prefix(arg3,"vuln"))
    {
      flag = &victim->vuln_flags;
      flag_table = imm_flags;
    }

    else if (!str_prefix(arg3,"form"))
    {
      if (!IS_NPC(victim))
      {
        stc("Form can't be set on PCs.\n\r",ch);
        return;
      }
      flag = &victim->form;
      flag_table = form_flags;
    }

    else if (!str_prefix(arg3,"comm"))
    {
      if (IS_NPC(victim))
      {
        stc("Comm can't be set on NPCs.\n\r",ch);
        return;
      }
      flag = &victim->comm;
      flag_table = comm_flags;
    }

    else 
    {
      stc("That's not an acceptable flag.\n\r",ch);
      return;
    }

    old = *flag;
    victim->zone = NULL;
  
    if (type != '=') new = old;
  
    /* mark the words */
    for (; ;)
    {
      argument = one_argument(argument,word);
  
      if (word[0] == '\0') break;
  
      pos = flag_lookup(word,flag_table);
      if (pos == 0)
      {
        stc("That flag doesn't exist!\n\r",ch);
        return;
      }
      else
      {
       SET_BIT(marked,pos);
      }
    }
    for (pos = 0; flag_table[pos].name != NULL; pos++)
    {
      if (!flag_table[pos].settable && IS_SET(old,flag_table[pos].bit))
      {
          SET_BIT(new,flag_table[pos].bit);
          continue;
      }
  
      if (IS_SET(marked,flag_table[pos].bit))
      {
        switch(type)
        {
          case '=':
          case '+':
            SET_BIT(new,flag_table[pos].bit);
            break;
          case '-':
            REM_BIT(new,flag_table[pos].bit);
            break;
          default:
            stc("Flag toggled.\n\r",ch);
            if (IS_SET(old,flag_table[pos].bit)) REM_BIT(new,flag_table[pos].bit);
            else SET_BIT(new,flag_table[pos].bit);
        }
      }
    }
    *flag = new;
    return;
  }
}

CHAR_DATA *find_summoner ( CHAR_DATA *ch )
{
  CHAR_DATA * summoner;

  for ( summoner = ch->in_room->people; summoner != NULL; summoner = summoner->next_in_room )
  {
    if (!IS_NPC(summoner)) continue;
    if (summoner->spec_fun == spec_lookup( "spec_summoner" )) return summoner;
    if (summoner->spec_fun == spec_lookup( "spec_summonera" )) return summoner;
  }

  if (summoner == NULL ||
    ( summoner->spec_fun != spec_lookup("spec_summoner" ) 
   && summoner->spec_fun != spec_lookup("spec_summonera") ) )
  {
    stc("Ты не можешь этого сделать.\n\r",ch);
    return NULL;
  }

  if ( summoner->fighting != NULL)
  {
    stc("Подожди пока закончитсЯ битва.\n\r",ch);
    return NULL;
  }
  return NULL;
}

void do_travel(CHAR_DATA *ch, const char *argument)
{
  CHAR_DATA *summoner;
  char  buf[MAX_STRING_LENGTH];
  char  arg[MAX_STRING_LENGTH];

  if ( (summoner = find_summoner (ch))==NULL) return;

  argument = one_argument(argument, arg );

  if (arg[0] == '\0')
  {
    do_printf(buf, "Используй команды:\n\r\tTRAVEL list         Просмотреть список куда можно переместитсЯ.\n\r\tTRAVEL <name> Переместится в выбранную арию.");    act("$C1 произносит '$t'.", ch, buf, summoner, TO_CHAR);
    return;
  }

  if (!str_prefix(arg,"list"))
  {
    do_printf(buf, "\n\r{cТы можешь выбрать:{x\n\rOfcol            25 gold\n\rGalaxy          200 gold\n\rLich Tower       250 gold\n\rHell            500 gold\n\rMausoleum       350 gold\n\rCloudy Mountain  50 gold\n\r");
    act("$C1 произносит '$t'.", ch, buf, summoner, TO_CHAR);
    return;
  }

  if (summoner->spec_fun == spec_lookup("spec_summoner"))
  { if (!str_prefix(arg, "ofcol"))           travel(601,25,ch,summoner);
    else if (!str_prefix(arg, "hell"))       travel(10404,500,ch,summoner);
    else if (!str_prefix(arg, "mausoleum"))  travel(4602,350,ch,summoner);
    else if (!str_prefix(arg, "galaxy"))     travel(9301,200,ch,summoner);
    else if (!str_prefix(arg, "lich tower")) travel(29350,250,ch,summoner);
    else if (!str_prefix(arg, "cloudy mountain")) travel(13200,50,ch,summoner);
    else
    {      do_printf(buf, "Такого места нет в моем списке");
      act("$C1 произносит '$t'.", ch, buf, summoner, TO_CHAR);
      do_printf(buf, "Набери: 'travel list' или\n\r    'travel <путешествие>'");
      act("$C1 произносит '$t'.", ch, buf, summoner, TO_CHAR);
    }
  }
}

void travel(int room, int cost, CHAR_DATA *ch, CHAR_DATA *summoner)
{
  CHAR_DATA *pet;
  char  buf[MAX_STRING_LENGTH];

  if (ch->gold < cost)
  {
    do_printf(buf, "У тебЯ нет столько золота");
    act("$C1 призносит '{G$t{x'.", ch, buf, summoner, TO_CHAR);
    return;
  }

  pet = ch->pet;              
  if ( ch->pet != NULL && ch->pet->in_room == ch->in_room)
  {
    act( "$c1 исчезает!", pet, NULL, NULL, TO_ROOM );
    char_from_room( pet );
    char_to_room( pet, get_room_index(room) );
    act( "$c1 появляется в комнате!", pet, NULL, NULL, TO_ROOM );
  }

  act( "$c1 бормочет 'Abrakadabra'!", summoner, NULL, NULL,TO_ROOM );
  act( "$c1 исчезает!", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, get_room_index(room) );
  act( "$c1 внезапно появляется в комнате!", ch, NULL, NULL, TO_ROOM );
  ch->gold -= cost;
  summoner->gold += cost;
  do_look (ch, "");
}

char *race_wname(CHAR_DATA *ch)
{
  static char arg[32];

  arg[0]='\0';
  if( !IS_NPC(ch))
  {
         if (!str_cmp(ch->name,"Saboteur")) strcat(arg,"{CЧеловек{x ");
            else strcat(arg,race_table[ch->race].who_name);
  }
  else strcat(arg,race_table[ch->race].who_name);
  return arg;
}

char *classname( CHAR_DATA *ch)
{
  static char buf[32];
  int i;

  buf[0]='\0';
  if (!ch) strcat(buf,"BUG   ");
  else
  if (IS_NPC(ch)) strcat(buf,"mobile");
  else if (IS_SET(ch->act, PLR_LASTREMORT))
  {
    strcat(buf,"{CAvatar{x    ");
    return buf;
  }
  else if (IS_SET(ch->act, PLR_5REMORT))
  {
    strcat(buf,"{wH{WE{wR{WO{x      ");
    return buf;
  }
  else
  {
    for (i=0; i<=16; i++)
    {
      if (class_ntab[i].war == ch->classwar &&
        class_ntab[i].cle == ch->classcle &&
        class_ntab[i].thi == ch->classthi &&
        class_ntab[i].mag == ch->classmag) break;
    }
  strcat(buf,class_ntab[i].name);
  }
  return buf;
}

char *clasname( CHAR_DATA *ch)
{
  static char buf[32];
  int i;

  buf[0]='\0';
  if (IS_NPC(ch)) strcat(buf,"mobile");
  else if (IS_SET(ch->act, PLR_5REMORT))
  {
    strcat(buf,"{YПросветлен{x");
    strcat(buf,"{wH{WE{wR{WO{x");
    return buf;
  }
  else
  {
    for (i=0; i<=16; i++)
    {
      if (class_nntab[i].war == ch->classwar &&
        class_nntab[i].cle == ch->classcle &&
        class_nntab[i].thi == ch->classthi &&
        class_nntab[i].mag == ch->classmag) break;
    }
  strcat(buf,class_nntab[i].name);
  }
  return buf;
}

const struct class_name_type class_ntab[16] = 
{ 
 { "{xMage{x      ", 1,0,0,0}, 
 { "{xCleric{x    ", 0,1,0,0}, 
 { "{xThief{x     ", 0,0,1,0}, 
 { "{xWarrior{x   ", 0,0,0,1}, 
 { "{GWizard{x    ", 1,1,0,0}, //M+C
 { "{GNightBlade{x", 1,0,1,0}, //M+T
 { "{GBattleMage{x", 1,0,0,1}, //M+W
 { "{GHeretic{x   ", 0,1,1,0}, //C+T
 { "{GPaladin{x   ", 0,1,0,1}, //C+W
 { "{GNinja{x     ", 0,0,1,1}, //T+W
 { "{RBard{x      ", 1,1,1,0}, //M+C+T
 { "{RMonk{x      ", 1,1,0,1}, //M+C+W
 { "{RSpellSword{x", 1,0,1,1}, //M+T+W
 { "{RTemplar{x   ", 0,1,1,1}, //C+T+W
 { "{YBodhisatva{x", 1,1,1,1}, //M+C+T+W
 { "{rError{x     ", 1,1,1,1}  //If error
}; 

const struct class_name_type class_nntab[16] = 
{ 
 { "{xMage{x",1,0,0,0},
 { "{xCleric{x",0,1,0,0},
 { "{xThief{x",0,0,1,0},
 { "{xWarrior{x",0,0,0,1},
 { "{GWizard{x",1,1,0,0}, //M+C
 { "{GNightBlade{x",1,0,1,0}, //M+T
 { "{GBattleMage{x",1,0,0,1}, //M+W
 { "{GHeretic{x",0,1,1,0}, //C+T
 { "{GPaladin{x",0,1,0,1}, //C+W
 { "{GNinja{x",0,0,1,1}, //T+W
 { "{RBard{x",1,1,1,0}, //M+C+T
 { "{RMonk{x",1,1,0,1}, //M+C+W
 { "{RSpellSword{x",1,0,1,1}, //M+T+W
 { "{RTemplar{x",0,1,1,1}, //C+T+W
 { "{YBodhisatva{x",1,1,1,1}, //M+C+T+W
 { "{rError{x",1,1,1,1}  //If error
};

void do_inform(CHAR_DATA *ch, const char *argument)
{
  int gn,col=0;

  if (*argument && !str_prefix(argument,"quenia"))
  {
    stc("Твои познания {Cquenia{x:\n\r",ch);
    for (gn=0;quenia_table[gn].name!=Q_END;gn++) ptc(ch,"Word: [%10s] - %u\n\r",quenia_table[gn].descr,ch->pcdata->quenia[gn]);
    return;
  }

  stc("{CВы знаете следующие группы:\n\r{x",ch);
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
  ptc(ch,"У вас %d cp.\n\r",ch->pcdata->points);
  ptc(ch,"Ваш класс(ы): {C[{G%s %s %s %s{C]{x\n\r",
    class_table[ch->class[0]].name,
    ch->remort>0 ? class_table[ch->class[1]].name:"",
    ch->remort>1 ? class_table[ch->class[2]].name:"",
    ch->remort>2 ? class_table[ch->class[3]].name:"");

  if( IS_DEVOTED_ANY(ch) )
  {
    ptc(ch, "Ты являешься %s %s.\n\r",
  (ch->sex==2)?t_favour[favour_string(ch)].fav_afstr:t_favour[favour_string(ch)].fav_amstr,
  get_rdeity(deity_table[ch->pcdata->dn].russian,'2') );
  }
  stc("{cТвои пристрастия к группам {Cмагии{c:\n\r",ch);
  ptc(ch,"{CWater     :{x%3d    ",category_bonus(ch,WATER));
  ptc(ch,"{WAir       :{x%3d\n\r",category_bonus(ch,AIR));
  ptc(ch,"{DEarth     :{x%3d    ",category_bonus(ch,EARTH));
  ptc(ch,"{RFire      :{x%3d\n\r",category_bonus(ch,FIRE));
  ptc(ch,"{MSpirit    :{x%3d    ",category_bonus(ch,SPIRIT));
  ptc(ch,"{MMind      :{x%3d\n\r",category_bonus(ch,MIND));
  ptc(ch,"{WLight     :{x%3d    ",category_bonus(ch,LIGHT));
  ptc(ch,"{RDark      :{x%3d\n\r",category_bonus(ch,DARK));
  ptc(ch,"{GFortitude :{x%3d    ",category_bonus(ch,FORTITUDE));
  ptc(ch,"{CCurative  :{x%3d\n\r",category_bonus(ch,CURATIVE));
  ptc(ch,"{YPerception:{x%3d    ",category_bonus(ch,PERCEP));
  ptc(ch,"{GLearning  :{x%3d\n\r",category_bonus(ch,LEARN));
  ptc(ch,"{ROffence   :{x%3d    ",category_bonus(ch,OFFENCE));
  ptc(ch,"{WProtection:{x%3d\n\r",category_bonus(ch,PROTECT));
  ptc(ch,"{YMakeMaster:{x%3d\n\r",category_bonus(ch,MAKE));
}

void do_setclass( CHAR_DATA *ch, const char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  int  class[4],i;
  bool mag=FALSE;
  bool war=FALSE;
  bool thi=FALSE;
  bool cle=FALSE;
  CHAR_DATA *victim;

  class[1]=0;
  class[2]=1;
  class[3]=2;
  class[4]=3;

  if(EMPTY(argument))
  {
    stc("syntax : test <class1> ... <classX>\n\r\n\r",ch);

    stc("{gПример1: test mage war thi       - пример для Spellsword{x\n\r",ch);
    stc("{gПример2: test thi cle            - для Еретика{x\n\r",ch);
    stc("{gПример3: test war mage th cleric - для Бодисатвы{x\n\r",ch);
    return;
  }

  argument=one_argument(argument,arg);

  if ((victim=get_pchar_world(ch,arg))==NULL)
  {
    stc("Персонаж не найден.\n\r",ch);
    return;
  }

  if (victim->remort==0)
  {
    ptc(ch,"Он еще не перерождался.\n\r",victim->name);
    return;
  }

  for (i=1;i<5;i++)
  {
    argument=one_argument(argument,arg);
    if (EMPTY(arg)) break;
    class[i]=class_lookup(arg);
    if ( (class[i]==CLASS_WAR && war==TRUE)
      || (class[i]==CLASS_MAG && mag==TRUE)
      || (class[i]==CLASS_CLE && cle==TRUE)
      || (class[i]==CLASS_THI && thi==TRUE))
    {
      stc("Ошибка - два раза выбран одинаковый класс.\n\r",ch);
      return;
    }

    if (class[i]==CLASS_MAG) mag=TRUE;
    if (class[i]==CLASS_WAR) war=TRUE;
    if (class[i]==CLASS_THI) thi=TRUE;
    if (class[i]==CLASS_CLE) cle=TRUE;

    if (class[i]==-1)
    {
      stc("Синтаксическая ошибка - такого класса нет.\n\r",ch);
      return;
    }
    if ( (class[i]==CLASS_WAR && !victim->classwar) ||
         (class[i]==CLASS_MAG && !victim->classmag) ||
         (class[i]==CLASS_CLE && !victim->classcle) ||
         (class[i]==CLASS_THI && !victim->classthi))
    {
      stc ("Ошибка - ты не перерождался в такой класс.\n\r",ch);
      return;
    }
  }
  if (i <= victim->remort+1)
  {
    stc ("Ты не указал все классы его перерождений.\n\r",ch);
    return;
  }
  victim->class[0]=class[1];
  victim->class[1]=class[2];
  victim->class[2]=class[3];
  victim->class[3]=class[4];
  ptc(ch,"Перерождения для {Y%s{x: %s %s %s %s, всего [%d].\n\r", 
   victim->name,class_table[victim->class[0]].name,
   victim->remort>0 ? class_table[victim->class[1]].name:"",
   victim->remort>1 ? class_table[victim->class[2]].name:"",
   victim->remort>2 ? class_table[victim->class[3]].name:"",
   victim->remort);
}

const char *remove_word(const char *argument, const char *word)
{
  char buf[MAX_INPUT_LENGTH],arg[MAX_INPUT_LENGTH];
  bool found=FALSE;
  const char *str = argument;

  buf[0]='\0';
  for (;;)
  {
    str = one_argument(str, arg);
    if (EMPTY(arg)) break;
    if (!str_cmp(word,arg)) continue;
    if (found) strcat(buf, " ");
    strcat(buf, arg);
    found=TRUE;
  }
  
  /*XXX! (not sure)*/
  free_string(argument);

  if (found) 
          return str_dup(buf);
  else 
          return &str_empty[0];
}

const char *add_word(const char *argument, const char *word)
{
  char buf[MAX_INPUT_LENGTH];

  buf[0]=0;
  
  if(!EMPTY(argument)) {
          strcat(buf,argument);
          strcat(buf," ");
  }

  strcat(buf,word);

  /*XXX! (not sure)*/
  free_string(argument);

  return str_dup(buf);
}

void do_marry(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH],arg3[MAX_INPUT_LENGTH];
  CHAR_DATA *victim, *victim1;
  ROOM_INDEX_DATA *proom;

  argument=one_argument(argument,arg1);
  argument=one_argument(argument,arg2);
  argument=one_argument(argument,arg3);

  if (EMPTY(arg1))
  {
    stc("Синтаксис: marry <имя>                  - сделать предложение руки и сердца.\n\r",ch);
    if (IS_IMMORTAL(ch) || ch->classcle)
    stc("           marry <имя1> <имя2>          - обвенчать молодых.\n\r",ch);
    if (IS_IMMORTAL(ch))
    stc("           marry <имя1> <имя2> official - поженить молодых.\n\r",ch);
    if (IS_IMMORTAL(ch))
    stc("           marry room <name> <vnum>     - назначить комнату.\n\r",ch);
    return;
  }

  if (IS_IMMORTAL(ch) && !str_cmp(arg1,"room"))
  {
    if (!(victim=get_pchar_world(ch,arg2)))
    {
      stc ("Персонаж не найден.\n\r",ch);
      return;
    }
    
    if (!is_number(arg3) || (proom=get_room_index(atoi64(arg3)))==NULL
     || (!IS_ELDER(ch) && IS_SET( proom->room_flags, ROOM_ELDER)) )
    {
      stc("Такой комнаты не существует.\n\r",ch);
      return;
    }
    victim->pcdata->proom=atoi64(arg3);
    ptc(ch,"Ты поселяешь {Y%s{x в комнате %u.\n\r",victim->name,victim->pcdata->proom);
    return;
  }

  if (EMPTY(arg2))
  {
    victim=get_pchar_world(ch,arg1);

    if (!victim || ch==victim)
    {
      stc("Персонаж не найден.\n\r",ch);
      return;
    }
    if (IS_MARRY(ch,victim))
    {
      stc("Вы уже женаты.\n\r",ch);
      return;
    }

    if (is_exact_name(victim->name,ch->pcdata->propose_marry))
    {
      ch->pcdata->propose_marry=remove_word(ch->pcdata->propose_marry,victim->name);
      ptc(ch,"Ты забираешь предложение руки и сердца для {Y%s{x.\n\r",victim->name);
      ptc(victim,"%s забирает свои слова о предложение руки и сердца.\n\r",ch->name);
    }
    else
    {
      ch->pcdata->propose_marry=add_word(ch->pcdata->propose_marry,victim->name);
      ptc(ch,"Ты делаешь предложение руки и сердца {Y%s{x.\n\r",victim->name);
      ptc(victim,"%s делает тебе предложение руки и сердца.\n\r",ch->name);
    }
    return;
  }

  if (!ch->classcle && !IS_IMMORTAL(ch))
  {
    stc("Только священник может обвенчать молодых.\n\r",ch);
    return;
  }

  victim=get_pchar_world(ch,arg1);
  victim1=get_pchar_world(ch,arg2);

  if (!victim || !victim1)
  {
    stc("Молодые (или один из них) не найден.\n\r",ch);
    return;
  }
  if (victim==victim1 && !IS_IMMORTAL(ch))
  {
    stc("Ты не можешь обвенчать сам себя.\n\r",ch);
    return;
  }

  if (!IS_SET(ch->in_room->room_flags,ROOM_SAFE)
      && !IS_SET(ch->in_room->ra, RAFF_SAFE_PLC) )
  {
    stc("Ты должен быть в святом месте для проведения обряда.\n\r",ch);
    return;
  }

  if (ch->in_room!=victim->in_room || ch->in_room!=victim1->in_room)
  {
    stc("Молодые должны быть здесь для проведения обряда.\n\r",ch);
    return;
  }
  
  if (!is_exact_name(victim->name,victim1->pcdata->propose_marry)
   || !is_exact_name(victim1->name,victim->pcdata->propose_marry))
  {
    stc("Они не делали друг другу предложение руки и сердца.\n\r",ch);
    return;
  }

  if (IS_IMMORTAL(ch) && !EMPTY(arg3) && !str_prefix(arg3,"official"))
  {
    if (IS_MARRY(victim1,victim))
    {
      stc("Они уже женаты.\n\r",ch);
      return;
    }
    if (!EMPTY(victim->pcdata->marry) || !EMPTY(victim1->pcdata->marry))
    {
      stc("Один из супругов уже женат или замужем.\n\r",ch);
      return;
    }
    victim1->pcdata->propose_marry=remove_word(victim1->pcdata->propose_marry,victim->name);
    victim->pcdata->propose_marry=remove_word(victim->pcdata->propose_marry, victim1->name);
    victim1->pcdata->lovers=remove_word(victim1->pcdata->lovers,victim->name);
    victim->pcdata->lovers=remove_word(victim->pcdata->lovers,victim1->name);
    victim1->pcdata->marry=str_dup(victim->name);
    victim->pcdata->marry=str_dup(victim1->name);
    do_printf(arg1,"{Y%s{x оъявляет {Y%s{x и{Y %s{x мужем и женой.",ch->name, victim->name,victim1->name);
    act(arg1,ch,NULL,NULL,TO_ALL_IN_ROOM);
    do_printf(arg1,"%s и %s зарегистрировали свой брак.\n\r",victim->name,victim1->name);
    send_news(arg1,NEWS_MARRY);
    return;
  }

  if (IS_LOVER(victim1,victim))
  {
    stc("Вы уже помолвлены.\n\r",ch);
    return;
  }
  victim1->pcdata->propose_marry=remove_word(victim1->pcdata->propose_marry,victim->name);
  victim->pcdata->propose_marry=remove_word(victim->pcdata->propose_marry,victim1->name);
  victim1->pcdata->lovers=add_word(victim1->pcdata->lovers,victim->name);
  victim->pcdata->lovers=add_word(victim->pcdata->lovers,victim1->name);
  do_printf(arg1,"{Y%s{x объявляет {Y%s{x и{Y %s{x женихом и невестой.",ch->name, victim->name,victim1->name);
  act(arg1,ch,NULL,NULL,TO_ALL_IN_ROOM);
}

void do_divorce(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH],arg3[MAX_INPUT_LENGTH];
  CHAR_DATA *victim, *victim1;

  argument=one_argument(argument,arg1);
  argument=one_argument(argument,arg2);
  argument=one_argument(argument,arg3);

  if (EMPTY(arg1))
  {
    if (!IS_IMMORTAL(ch) && !ch->classcle)
    {
      stc("Только священник может разорвать узы брака.\n\r",ch);
      return;
    }
    if (IS_IMMORTAL(ch) || ch->classcle)
      stc("Синтаксис: divorce <имя1> <имя2>          - развести пару.\n\r",ch);
    if (IS_IMMORTAL(ch))
      stc("           divorce <имя1> <имя2> official - развести пару (официально).\n\r",ch);
    if (IS_IMMORTAL(ch)) {
      stc("           divorce <char> force           - насильно убрать статус married.\n\r",ch);
      stc("                                            (для клинических случаев)\n\r",ch);
     }       
    return;
  }

  victim=get_pchar_world(ch,arg1);
  if (!victim)
  {
   stc ("Персонаж не найден.\n\r",ch);
   return;
  }
  if (IS_IMMORTAL(ch) && !str_cmp(arg2,"force"))
  { 
  victim->pcdata->lovers=&str_empty[0];
  victim->pcdata->marry=&str_empty[0];
  stc ("Данные женатости/полюбовности очищены\n\r.",ch);
  ptc (victim, "Теперь ты свобод%s как ветер!\n\r",(victim->sex==SEX_FEMALE)?"на":"ен");
  return;
  }
   
  victim1=get_pchar_world(ch,arg2);
  if(!victim || !victim1 || victim==victim1 || (!IS_MARRY(victim,victim1) && !IS_LOVER(victim,victim1)))
  {
    stc ("Супругов или один из них не найдено.\n\r",ch);
    return;
  }
  if (ch->in_room!=victim->in_room || ch->in_room!=victim1->in_room || !IS_SET(ch->in_room->room_flags,ROOM_MARRY))
  {
    stc("Вы должны все находится в священном месте бракосочетаний.\n\r",ch);
    return;
  }
  victim->pcdata->marry=remove_word(victim->pcdata->marry,victim1->name);
  victim1->pcdata->marry=remove_word(victim1->pcdata->marry,victim->name);
  victim->pcdata->lovers=remove_word(victim->pcdata->lovers,victim1->name);
  victim1->pcdata->lovers=remove_word(victim1->pcdata->lovers,victim->name);
  ptc(ch,"Ты рассторгаешь священные узы брака между {Y%s{x и {Y%s{x.\n\r",victim->name,victim1->name);
  ptc(victim,"%s рассторгает священные узы брака между тобой и {Y%s{x.\n\r",ch->name,victim1->name);
  ptc(victim1,"%s рассторгает священные узы брака между тобой и {Y%s{x.\n\r",ch->name,victim->name);
}

void do_vislist(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument=one_argument(argument,arg);

  if (EMPTY(arg))
  {
    if (ch->incog_level>0 || ch->invis_level>0)
      ptc(ch,"Incognito: %d, Invisible: %d\n\r",ch->incog_level,ch->invis_level);
    ptc(ch,"Current visible list:\n\r[%s]\n\r",ch->pcdata->vislist);
    return;
  }

  if (!str_prefix(arg,"add"))
  {
    argument=one_argument(argument,arg);
    if (EMPTY(arg))
    {
      do_vislist(ch,"");
      return;
    }
    ch->pcdata->vislist=add_word(ch->pcdata->vislist,arg);
    ptc(ch,"%s добавлен в Visible List.\n\r",arg);
    return;
  }

  if (!str_prefix(arg,"remove"))
  {
    argument=one_argument(argument,arg);
    if (EMPTY(arg))
    {
      do_vislist(ch,"");
      return;
    }
    ch->pcdata->vislist=remove_word(ch->pcdata->vislist,arg);
    ptc(ch,"%s удален из Visible List.\n\r",arg);
    return;
  }
  stc("Syntax: vislist <add|remove> <char_name>\n\r",ch);
}

void do_ignorelist(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument=one_argument(argument,arg);

  if (EMPTY(arg))
  {
    if (ch->incog_level>0 || ch->invis_level>0)
      ptc(ch,"Incognito: %d, Invisible: %d\n\r",ch->incog_level,ch->invis_level);
    ptc(ch,"Current ignore list:\n\r[%s]\n\r",ch->pcdata->ignorelist);
    return;
  }

  if (!str_prefix(arg,"add"))
  {
    argument=one_argument(argument,arg);
    if (EMPTY(arg))
    {
      do_ignorelist(ch,"");
      return;
    }
    ch->pcdata->ignorelist=add_word(ch->pcdata->ignorelist,arg);
    ptc(ch,"%s добавлен в Ignore List.\n\r",arg);
    return;
  }

  if (!str_prefix(arg,"remove"))
  {
    argument=one_argument(argument,arg);
    if (EMPTY(arg))
    {
      do_ignorelist(ch,"");
      return;
    }
    ch->pcdata->ignorelist=remove_word(ch->pcdata->ignorelist,arg);
    ptc(ch,"%s удален из Ignore List.\n\r",arg);
    return;
  }
  stc("Syntax: ignorelist <add|remove> <char_name>\n\r",ch);
}


char *spell_group_name(int64 group)
{
  static char buf[512];

  buf[0] = '\0';
  if (group & FIRE      ) strcat(buf, " Fire");
  if (group & AIR       ) strcat(buf, " Air");
  if (group & WATER     ) strcat(buf, " Water");
  if (group & EARTH     ) strcat(buf, " Earth");
  if (group & SPIRIT    ) strcat(buf, " Spirit");
  if (group & MIND      ) strcat(buf, " Mind");
  if (group & PROTECT   ) strcat(buf, " Protection");
  if (group & FORTITUDE ) strcat(buf, " Fortitude");
  if (group & CURATIVE  ) strcat(buf, " Curative");
  if (group & LIGHT     ) strcat(buf, " Light");
  if (group & DARK      ) strcat(buf, " Dark");
  if (group & LEARN     ) strcat(buf, " Learning");
  if (group & PERCEP    ) strcat(buf, " Perception");
  if (group & MAKE      ) strcat(buf, " MakeMastery");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

int abs(int number)
{
 if (number<0) return -number;
 return number;
}

char *get_logchar(int flag)
{
  static char buf[5];
  buf[0]='\0';
  switch (flag)
  {
    case WIZ_NEWBIE   : strcat (buf,":N:");break;
    case WIZ_LOGINS   : strcat (buf,":L:");break;
    case WIZ_SITES    :
    case WIZ_ADDR     :
    case WIZ_LINKS    : strcat (buf,":C:");break;

    case WIZ_DEATHS   : strcat (buf,":D:");break;
    case WIZ_MOBDEATHS: strcat (buf,":d:");break;

    case WIZ_PENALT   : strcat (buf,":P:");break;
    case WIZ_RESETS   :
    case WIZ_SECURE   :
    case WIZ_SWITCH   :
    case WIZ_SNOOPS   :
    case WIZ_RESTORE  :
    case WIZ_LOAD     : strcat (buf,":W:");break;

    case WIZ_SACCING  :
    case WIZ_SPAM     : strcat (buf,":s:");break;

    case WIZ_NOTES    : strcat (buf,":n:");break;
    case WIZ_SPEAKS   : strcat (buf,":s:");break;
    case WIZ_GSPEAKS  : strcat (buf,":g:" );break;

    case WIZ_SKILLS   :
    case WIZ_CONFIG   : strcat (buf,":k:"  );break;

    case WIZ_PREFIX   :
    case WIZ_FLAGS    :
    case WIZ_ON       :
    case WIZ_LEVELS   :
    case WIZ_TICKS    :
    default           : strcat (buf,":"  );break;
  }
  return buf;
}

int get_loglevel(int flag)
{
  int level=110;
  switch (flag)
  {
    case WIZ_ON       : level=0  ;break;
    case WIZ_TICKS    : level=0  ;break;
    case WIZ_LOGINS   : level=0  ;break;
    case WIZ_SITES    : level=107;break;
    case WIZ_LINKS    : level=106;break;
    case WIZ_DEATHS   : level=0  ;break;
    case WIZ_RESETS   : level=0  ;break;
    case WIZ_MOBDEATHS: level=0  ;break;
    case WIZ_FLAGS    : level=106;break;
    case WIZ_PENALT   : level=106;break;
    case WIZ_SACCING  : level=0  ;break;
    case WIZ_LEVELS   : level=0  ;break;
    case WIZ_SECURE   : level=107;break;
    case WIZ_SWITCH   : level=0  ;break;
    case WIZ_SNOOPS   : level=108;break;
    case WIZ_RESTORE  : level=108;break;
    case WIZ_LOAD     : level=108;break;
    case WIZ_NEWBIE   : level=0  ;break;
    case WIZ_PREFIX   : level=0  ;break;
    case WIZ_SPAM     : level=0  ;break;
    case WIZ_ADDR     : level=108;break;
    case WIZ_SPEAKS   : level=109;break;
    case WIZ_SKILLS   : level=0  ;break;
    case WIZ_CONFIG   : level=0  ;break;
    case WIZ_NOTES    : level=109;break;
    case WIZ_GSPEAKS  : level=103;break;
  }
  return level;
}

int get_material_modifier(CHAR_DATA *ch, OBJ_DATA *obj)
{
  if (IS_SET(ch->res_flags,material_table[material_num(obj->material)].res))  return material_table[material_num(obj->material)].resnum;
  if (IS_SET(ch->vuln_flags,material_table[material_num(obj->material)].vul)) return material_table[material_num(obj->material)].vulnum;
  return 0;
} 

void do_fly( CHAR_DATA *ch, const char *argument )
{

  if ( !IS_SET(race_table[ch->race].spec,SPEC_FLY)
      && (IS_NPC(ch) || !IS_SET(ch->act,PLR_CAN_FLY)) )
  { 
    stc("Ты не умеешь летать",ch); 
    return; 
  } 

  if ( IS_AFFECTED(ch, AFF_FLYING) ) stc("Ты уже паришь в воздухе.\n\r",ch);
  else 
  {
    SET_BIT(ch->affected_by,AFF_FLYING); 
    stc( "Твои ноги отрываются от земли.\n\r", ch ); 
    act( "Ноги {Y$c2{x отрываются от земли.", ch, NULL, NULL, TO_ROOM ); 
  }
}

void do_walk( CHAR_DATA *ch, const char *argument )
{
  if ( !IS_AFFECTED(ch, AFF_FLYING) ) stc("Ты уже стоишь на земле.\n\r",ch);
  else
  { 
    affect_strip(ch,skill_lookup("fly")); 
    REM_BIT(ch->affected_by,AFF_FLYING); 
    stc("Ты приземляешься.\n\r",ch); 
    act( "$c1 плавно приземляется.", ch, NULL, NULL, TO_ROOM ); 
  } 
}

void send_news(char *text, int type)
{
  NEWS_DATA *news;
  char *buf;
  int counter=0;

  if (!newspaper_list)
  {
    newspaper_list=new_news();
    news=newspaper_list;
  }
  else
  {
    for (news=newspaper_list;news->next!=NULL;news=news->next) counter++;
    if (counter>450)
    {
      news->next=newspaper_list;
      news=newspaper_list;
      news->next=NULL;
      newspaper_list=newspaper_list->next;
      free_news(news);
    }
    else
    {
      news->next=new_news();
      news=news->next;
    }
  }
  buf                = ctime( &current_time );
  buf[strlen(buf)-1] = '\0';
  free_string(news->date);
  news->date         = str_dup( buf );
  news->date_stamp   = current_time;
  news->type         = type;
  free_string(news->text);
  news->text         = str_dup(text);
  save_newspaper();
}

int get_skill_bonus(CHAR_DATA *ch,int sn)
{
  int bonus=0;
  if (!ch || !ch->in_room) return 0;

  if (IS_SET(skill_table[sn].group, WATER))
  {
   if (weather_info.sky>=2) bonus+=2;
   if (weather_info.sky==0) bonus-=1;
  }

  if (IS_SET(skill_table[sn].group, FIRE))
  {
   if (weather_info.sky==0) bonus+=2;
   if (weather_info.sky>=2) bonus-=2;
  }

  if (IS_SET(skill_table[sn].group, LIGHT))
  {
    if (weather_info.sunlight==SUN_RISE) bonus+=3;
    if (weather_info.sunlight==SUN_DARK) bonus-=3;
    if (weather_info.sky>=2) bonus++;
    if (weather_info.sky==3) bonus++;
  }
  if (IS_SET(skill_table[sn].group, DARK))
  {
    if (weather_info.sunlight==SUN_DARK) bonus+=3;
    if (weather_info.sunlight==SUN_RISE) bonus-=3;
  }
  if (!ch) return bonus;
  switch(ch->in_room->sector_type)
  {
    case SECT_MOUNTAIN:
    case SECT_ROCK_MOUNTAIN:
    case SECT_SNOW_MOUNTAIN:
      if (IS_SET(skill_table[sn].group,EARTH)) bonus+=3;
      if (IS_SET(skill_table[sn].group,AIR)) bonus-=3;
      break;
    case SECT_AIR:
      if (IS_SET(skill_table[sn].group,EARTH)) bonus-=3;
      if (IS_SET(skill_table[sn].group,AIR)) bonus+=3;
      break;
    case SECT_DESERT:
      if (IS_SET(skill_table[sn].group,WATER)) bonus-=3;
      if (IS_SET(skill_table[sn].group,FIRE)) bonus+=3;
      break;
    case SECT_WATER_SWIM:
    case SECT_SWAMP:
    case SECT_WATER_NOSWIM:
      if (IS_SET(skill_table[sn].group,WATER)) bonus+=3;
      if (IS_SET(skill_table[sn].group,FIRE)) bonus-=3;
      break;
  }
  return bonus;
}

void do_smoke( CHAR_DATA *ch, const char *argument )
{ 
  OBJ_DATA *obj;
  bool found=FALSE;

  for (obj = ch->in_room->contents; obj; obj= obj->next_content)
  {
    if (!obj->pIndexData) return;
    switch(obj->pIndexData->vnum)
    {
      case 26510:
        switch( ch->version)
        {
          case 100:
          case 101:
          case 102:
          case 103:
           stc("Ты затягиваешься сигаретой, наслаждаясь ее ароматом.\n\r",ch);
           act("{Y$n{x затягивается сигаретой, наслаждаясь ее ароматом.",ch,NULL,NULL,TO_ROOM);
           ch->version+=1;
           break;
          case 104:
           stc("Ты делаешь последнюю затяжку сигаретой и тушишь окурок в пепельнице.\n\r",ch);
           act("{Y$n{x делает последнюю затяжку сигаретой и тушит окурок в пепельнице.\n\rПо комнате разносится тонкий аромат.",ch,NULL,NULL,TO_ROOM);
           ch->version=13;
           break;
         default:
           stc("Ты достаешь из {Mрозового ящичка {xсигарету и раскуриваешь ее.\n\rПо комнате разносится тонкий аромат.\n\r",ch);
           act("{Y$n{x достает из розового ящичка сигарету и раскуривает ее.\n\rПо комнате разносится тонкий аромат.",ch,NULL,NULL,TO_ROOM);
           ch->version=100;
           break;
        }
        found=TRUE;
        break;
      default:
        break;
    }
    if (found) break;
  }
  if (!found) stc("Ась?\n\r",ch);
}

const char *create_word()
{
  int length,i;
  char buf[20];

  length=number_range(4,8);
  for (i=0;i<length;i++) buf[i]=(char)number_range(97,122);
  buf[i]='\0';
  return str_dup(buf);
}

void do_newspaper(CHAR_DATA *ch, const char *argument)
{
  char arg[MAX_INPUT_LENGTH],buf[MAX_INPUT_LENGTH];
  BUFFER *output;

  argument=one_argument(argument,arg);
  if (EMPTY(arg))
  {
    stc(" magazine type             - показать наименования типов событий\n\r",ch);
    stc(" magazine list <{Ytype{x>      - вывести события указанного типа\n\r",ch);
    stc(" magazine list <{Gx{x>         - вывести с номера x\n\r",ch);
    stc(" magazine list <{G-x{x>        - вывести последние x событий\n\r",ch);
    stc(" magazine list <{Gx{x> <{Ytype{x>  - с номера x указанного типа\n\r",ch);
    stc(" magazine list <{G-x{x> <{Ytype{x> - последние x событий указанного типа\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"type"))
  {
    stc("Существуют следующие разделы: (вводить слово нужно целиком)\n\r",ch);
    stc("murder  - убийства\n\r",ch);
    stc("death   - смерти\n\r",ch);
    stc("victory - победы на арене\n\r",ch);
    stc("marry   - свадьбы\n\r",ch);
    stc("remort  - перерождения\n\r",ch);
    stc("gquest  - выполнение заданий\n\r",ch);
    stc("reward  - получения наград\n\r",ch);
    stc("all     - все события\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"list"))
  {
    NEWS_DATA *n;
    bool args[10];
    int matrix[MAX_NEWSPAPER], number, cnt;

    for (number=0;number<10;number++) args[number]=FALSE;
    number=0;

    one_argument(argument,arg);
    if (is_number(arg))
    {
      argument=one_argument(argument,arg);
      number=atoi(arg);
    }

    if (EMPTY(argument))             args[NEWS_ALL   ]=TRUE;
    else
    {
      if (is_name("murder", argument)) args[NEWS_MURDER] =TRUE;
      if (is_name("death",  argument)) args[NEWS_DEATH ] =TRUE;
      if (is_name("marry",  argument)) args[NEWS_MARRY ] =TRUE;
      if (is_name("remort", argument)) args[NEWS_REMORT] =TRUE;
      if (is_name("gquest", argument)) args[NEWS_GQUEST] =TRUE;
      if (is_name("victory",argument)) args[NEWS_VICTORY]=TRUE;
      if (is_name("all",    argument)) args[NEWS_ALL   ] =TRUE;
      if (is_name("reward", argument)) args[NEWS_REWARD] =TRUE;
    }

    stc("\n\r=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r",ch);

    // Creating matrix with filtered messaged numbers:
    cnt=0;
    for (n=newspaper_list;n;n=n->next)
    {
      if (args[NEWS_ALL] || args[n->type]) matrix[cnt]=1;
      else matrix[cnt]=2;
      cnt++;
    }
    matrix[cnt]=0;
    
    if (number<0)
    {
      for (;cnt>0;cnt--)
      {
        if (matrix[cnt]==2) continue;
        if (number++>=0) break;
      }
    }
    else if (number>0)
    {
      for (cnt=0;cnt<=500;cnt++)
      {
//        if (matrix[cnt]==2) continue; (turn on event filtered counting)
        if (matrix[cnt]==0 || --number<=0) break;
      }
      if (matrix[cnt]==0)
      {
        stc("Столько событий не зарегистрировано\n\r",ch);
        return;
      }
    }
    else cnt=0;
    for (number=0,n=newspaper_list;n;n=n->next,number++) if (number>=cnt) break;

    number=0;
    output = new_buf();

    for (;n;n=n->next)
    {
      if (matrix[cnt]==0) break;
      if (matrix[cnt++]==2) continue;
      do_printf(buf,"{C[{Y%3d{C] at %s : %s{x\n\r%s\n\r\n\r",cnt,n->date,newspaper_subject(n->type),n->text);
      add_buf(output,buf);
      if (++number>75)
      {
        add_buf(output,"Too many events...\n\r");
        break;
      }
    }
    page_to_char( buf_string(output), ch );
    free_buf(output);
    stc("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r",ch);
    return;
  }
}

char *newspaper_subject(int type)
{
  static char buf[128];

  buf[0] = '\0';

  if (type==NEWS_MURDER  ) strcat(buf, " {RУбийство!{x");
  if (type==NEWS_DEATH   ) strcat(buf, " {DНекрономикум{x");
  if (type==NEWS_VICTORY ) strcat(buf, " {YПоединок{x");
  if (type==NEWS_OTHER   ) strcat(buf, " {CСобытия{x");
  if (type==NEWS_REMORT  ) strcat(buf, " {YПерерождение{x");
  if (type==NEWS_MARRY   ) strcat(buf, " {YСвадьба{x");
  if (type==NEWS_GQUEST  ) strcat(buf, " {YПобеда{x");
  if (type==NEWS_REWARD  ) strcat(buf, " {WНаграда{x");
  
  return (buf[0]!= '\0')? buf : "unknown";
}

void do_russian( CHAR_DATA *ch, const char *argument )
{
  ch->comm=toggle_int64(ch->comm,COMM_RUSSIAN);
  ptc(ch,"Поддержка русского языка %s{x.\n\r",IS_SET(ch->comm,COMM_RUSSIAN)?"{Gвкл":"{Rвыкл");
}

int get_full_class(CHAR_DATA *ch)
{
  int bonus=0;

  switch(ch->class[0])
  {
    case CLASS_WAR:
     SET_BIT(bonus,C_WAR);
     break;
    case CLASS_MAG:
     SET_BIT(bonus,C_MAG);
     break;
    case CLASS_CLE:
     SET_BIT(bonus,C_CLE);
     break;
    case CLASS_THI:
     SET_BIT(bonus,C_THI);
     break;
  }
  if (ch->remort>0) switch(ch->class[0])
  {
    case CLASS_WAR:
     if (ch->class[1]==CLASS_MAG) SET_BIT(bonus,C_BATTLEMAGE);
     if (ch->class[1]==CLASS_CLE) SET_BIT(bonus,C_PALADIN);
     if (ch->class[1]==CLASS_THI) SET_BIT(bonus,C_NINJA);
     break;
    case CLASS_MAG:
     if (ch->class[1]==CLASS_CLE) SET_BIT(bonus,C_WIZARD);
     if (ch->class[1]==CLASS_THI) SET_BIT(bonus,C_NIGHTBLADE);
     if (ch->class[1]==CLASS_WAR) SET_BIT(bonus,C_BATTLEMAGE);
     break;
    case CLASS_CLE:
     if (ch->class[1]==CLASS_MAG) SET_BIT(bonus,C_WIZARD);
     if (ch->class[1]==CLASS_THI) SET_BIT(bonus,C_HERETIC);
     if (ch->class[1]==CLASS_WAR) SET_BIT(bonus,C_PALADIN);
     break;
    case CLASS_THI:
     if (ch->class[1]==CLASS_MAG) SET_BIT(bonus,C_NIGHTBLADE);
     if (ch->class[1]==CLASS_CLE) SET_BIT(bonus,C_HERETIC);
     if (ch->class[1]==CLASS_WAR) SET_BIT(bonus,C_NINJA);
     break;
  }
  if (ch->remort==2)
  {
    if (!ch->classmag) SET_BIT(bonus,C_TEMPLAR);
    if (!ch->classcle) SET_BIT(bonus,C_SPELLSWORD);
    if (!ch->classthi) SET_BIT(bonus,C_MONK);
    if (!ch->classwar) SET_BIT(bonus,C_BARD);
  }
  else if (ch->remort==3)
  {
    SET_BIT(bonus,C_BODHISATVA);
    if (ch->class[3]==CLASS_MAG) SET_BIT(bonus,C_TEMPLAR);
    if (ch->class[3]==CLASS_CLE) SET_BIT(bonus,C_SPELLSWORD);
    if (ch->class[3]==CLASS_THI) SET_BIT(bonus,C_MONK);
    if (ch->class[3]==CLASS_WAR) SET_BIT(bonus,C_BARD);
  }
  return bonus;
}

int category_bonus(CHAR_DATA *ch,int category)
{
 #define RBON  4
 #define RPEN  4
 int bonus=0;

 int class=get_full_class(ch);

 if (IS_SET(category,SPIRIT))
 {
   if (IS_SET(class,C_CLE)    )                   bonus+=2 ; // cleric
   if (IS_SET(class,C_HERETIC))                   bonus+=2 ; // heretic
   if (IS_SET(class,C_PALADIN))                   bonus+=2 ; // paladin
   if (IS_SET(class,C_MONK)   )                   bonus++  ; // monk
   if (IS_SET(class,C_TEMPLAR))                   bonus++  ; // templar
   if (IS_SET(race_table[ch->race].c_pen,SPIRIT)) bonus-=RPEN ; // race
   if (IS_SET(race_table[ch->race].c_bon,SPIRIT)) bonus+=RBON; // race
   bonus += dcp(ch,2,5);
//   counter++;
 }
 if (IS_SET(category,MIND))
 {
   if (IS_SET(class,C_MAG))                       bonus+=2; // mage
   if (IS_SET(class,C_NIGHTBLADE))                bonus+=2; // nightblade
   if (IS_SET(class,C_BARD))                      bonus++ ; // Bard
   if (IS_SET(race_table[ch->race].c_pen,MIND))   bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,MIND))   bonus+=RBON; // race
   if (ch->sex==SEX_FEMALE) bonus++;
   bonus += dcp(ch,2,6);
 }
 if (IS_SET(category,EARTH))
 {
   if (IS_SET(class,C_WIZARD))                    bonus+=2; // wizard
   if (IS_SET(race_table[ch->race].c_pen,EARTH))  bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,EARTH))  bonus+=RBON; // race
   bonus += dcp(ch,2,3);
 }
 if (IS_SET(category,AIR))
 {
   if (IS_SET(class,C_WIZARD))                    bonus+=2; // wizard
   if (IS_SET(race_table[ch->race].c_pen,AIR))    bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,AIR))    bonus+=RBON; // race
   bonus += dcp(ch,2,2);
 }
 if (IS_SET(category,WATER))
 {
   if (IS_SET(class,C_WIZARD))                    bonus+=2; // wizard
   if (IS_SET(race_table[ch->race].c_pen,WATER))  bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,WATER))  bonus+=RBON; // race
   bonus += dcp(ch,2,1);
 }
 if (IS_SET(category,FIRE))
 {
   if (IS_SET(class,C_HERETIC))                   bonus+=1; // heretic
   if (IS_SET(class,C_WIZARD))                    bonus+=2; // wizard
   if (IS_SET(race_table[ch->race].c_pen,FIRE))   bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,FIRE))   bonus+=RBON; // race
   bonus += dcp(ch,2,4);
 }
 if (IS_SET(category,FORTITUDE))
 {
   if (IS_SET(class,C_WAR))                       bonus+=2;  // warrior
   if (IS_SET(class,C_BATTLEMAGE))                bonus+=1; // battlemage
   if (IS_SET(class,C_NINJA))                     bonus+=1; // ninja
   if (IS_SET(class,C_MONK))                      bonus+=1; // monk
   if (IS_SET(race_table[ch->race].c_pen,FORTITUDE))   bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,FORTITUDE))   bonus+=RBON; // race
   if (ch->sex==SEX_MALE) bonus++;
   bonus += dcp(ch,2,9);
 }
 if (IS_SET(category,CURATIVE))
 {
   if (IS_SET(class,C_CLE)    )                   bonus+=2 ; // cleric
   if (IS_SET(class,C_PALADIN))                   bonus+=1 ; // paladin
   if (IS_SET(class,C_MONK)   )                   bonus+=1  ; // monk
   if (IS_SET(race_table[ch->race].c_pen,CURATIVE))   bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,CURATIVE))   bonus+=RBON; // race
   bonus += dcp(ch,2,10);
 }
 if (IS_SET(category,DARK))
 {
   if (IS_SET(race_table[ch->race].c_pen,DARK))   bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,DARK))   bonus+=RBON; // race
   if (ch->alignment>350)                         bonus--;  // good
   if (ch->alignment>750)                         bonus-=2; // very good
   if (ch->alignment==1000)                       bonus-=3; // angelic
   if (ch->alignment<-350)                        bonus++;  // evil
   if (ch->alignment<-750)                        bonus+=2; // very evil
   if (ch->alignment==-1000)                      bonus+=3; // demonic
   bonus += dcp(ch,2,8);
 }
 if (IS_SET(category,LIGHT))
 {
   if (IS_SET(race_table[ch->race].c_pen,LIGHT))  bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,LIGHT))  bonus+=RBON; // race
   if (ch->alignment>350)                         bonus++;  // good
   if (ch->alignment>750)                         bonus+=2; // very good
   if (ch->alignment==1000)                       bonus+=3; // angelic
   if (ch->alignment<-350)                        bonus--;  // evil
   if (ch->alignment<-750)                        bonus-=2; // very evil
   if (ch->alignment==-1000)                      bonus-=3; // demonic
   bonus += dcp(ch,2,7);
 }
 if (IS_SET(category,LEARN))
 {
   if (IS_SET(class,C_MAG))                       bonus+=2; // mage
   if (IS_SET(class,C_NIGHTBLADE))                bonus+=1; // NightBlade
   if (IS_SET(class,C_SPELLSWORD))                bonus++;  // Spellsword
   if (IS_SET(class,C_BATTLEMAGE))                bonus+=2; // battlemage
   if (IS_SET(class,C_BARD))                      bonus+=1; // Bard
   if (IS_SET(race_table[ch->race].c_pen,LEARN))  bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,LEARN))  bonus+=RBON; // race
   bonus += dcp(ch,2,12);
 }
 if (IS_SET(category,MAKE))
 {
   if (GUILD(ch,DWARVES_GUILD))                   bonus++; 
   if (IS_SET(class,C_BATTLEMAGE))                bonus++; // battlemage
   if (IS_SET(race_table[ch->race].c_pen,MAKE))   bonus-=RPEN; // race
   if (IS_SET(race_table[ch->race].c_bon,MAKE))   bonus+=RBON; // race
   bonus += dcp(ch,2,15);
 }
 if (IS_SET(category,PERCEP))
 {
   if (IS_SET(class,C_THI))                       bonus+=3;  // thief
   if (IS_SET(class,C_HERETIC))                   bonus+=2; // heretic
   if (IS_SET(class,C_NINJA))                     bonus+=2; // thief
   if (IS_SET(class,C_NIGHTBLADE))                bonus+=2; // NightBlade
   if (IS_SET(class,C_SPELLSWORD))                bonus+=1;  // Spellsword
   if (IS_SET(class,C_BARD))                      bonus+=1; // Bard
   if (IS_SET(class,C_TEMPLAR))                   bonus+=1; // templar
   if (IS_SET(race_table[ch->race].c_pen,PERCEP)) bonus-=RPEN; // race -3
   if (IS_SET(race_table[ch->race].c_bon,PERCEP)) bonus+=RBON; // race +3
   bonus += dcp(ch,2,11);
 }
 if (IS_SET(category,PROTECT))
 {
   if (IS_SET(class,C_PALADIN))                   bonus+=2; // paladin +2
   if (IS_SET(class,C_TEMPLAR))                   bonus+=1; // templar
   if (IS_SET(class,C_MONK))                      bonus+=1; // monk
   if (IS_SET(race_table[ch->race].c_bon,PROTECT))bonus+=RBON; // race +3
   if (IS_SET(race_table[ch->race].c_pen,PROTECT))bonus-=RPEN; // race -3
   bonus += dcp(ch,2,14);
 }
 if (IS_SET(category,OFFENCE))
 {
   if (IS_SET(class,C_WAR))                       bonus+=2; //
   if (IS_SET(class,C_THI))                       bonus++;  // thief 
   if (IS_SET(class,C_BATTLEMAGE))                bonus+=1; // battlemage
   if (IS_SET(class,C_NINJA))                     bonus+=2; // 
   if (IS_SET(class,C_SPELLSWORD))                bonus+=1;  // Spellsword
   if (IS_SET(race_table[ch->race].c_bon,OFFENCE))bonus+=RBON; // race +3
   if (IS_SET(race_table[ch->race].c_pen,OFFENCE))bonus-=RPEN; // race -3
   bonus += dcp(ch,2,13);
 }

 return bonus;
}


char *get_align(CHAR_DATA *ch)
{
  static char buf[512];

  buf[0] = '\0';
  if       (ch->alignment < -900) strcat(buf,"{Rдьявольский");
  else if  (ch->alignment < -700) strcat(buf,"{Rдемонический");
  else if  (ch->alignment < -350) strcat(buf,"{rзлой");
  else if  (ch->alignment < -100) strcat(buf,"{mподлый");
  else if  (ch->alignment < 100)  strcat(buf,"{Dнейтральный");
  else if  (ch->alignment < 350)  strcat(buf,"{gмягкий");
  else if  (ch->alignment < 700)  strcat(buf,"{Gдобрый");
  else if  (ch->alignment < 900)  strcat(buf,"{Wсвятой");
  else                            strcat(buf,"{Wангельский");
  return buf;
}


char *get_clan_rank(CHAR_DATA *ch)
{
  static char buf[512];

  buf[0] = '\0';
  if (!ch->clan)            strcat(buf,"{Dнет      ");
  else 
    switch ( ch->clanrank )
    {
      case 0:
             strcat(buf,"{xMember   ");break;
      case 1:
             strcat(buf,"{WJunior   ");break;
      case 2:
             strcat(buf,"{MSenior   ");break;
      case 3:
             strcat(buf,"{GDeputy   ");break;
      case 4:
             if (!str_cmp(ch->clan->name, "chaos"))
             {
               ch->sex==1 ? strcat(buf,"{CKnight   ") : strcat(buf,"{CLady     ") ;
             }
             else 
               strcat(buf,"{CSecond   ");
             break;
      case 5:
             strcat(buf,"{WLEADER   ");break;
     default:
             strcat(buf,"{Dнет      ");break;
    }
  return buf;
}

void bust_arg( CHAR_DATA *ch, const char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  const char *str;
  const char *i;
  char *point;
  char *pbuff;
  char buffer[ MAX_STRING_LENGTH*2 ];
  char doors[MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;
  bool found;
  const char *dir_name[] = {"N","E","S","W","U","D", "n", "e", "s", "w", "u", "d"};
  int door;
 
  point = buf;
  str = argument;
  if( !str || str[0] == '\0') return;

  if (IS_SET(ch->comm,COMM_AFK))
  {
    stc("<AFK> ",ch);
    return;
  }

  while( *str != '\0' )
  {
    if( *str != '%' )
    {
      *point++ = *str++;
      continue;
    }
    ++str;
    switch( *str )
    {
      default :
         i = " "; break;
      case 'e':
        found = FALSE;
        doors[0] = '\0';
        for (door = 0; door < 6; door++)
        {
          if ((pexit = ch->in_room->exit[door]) != NULL
            &&  pexit ->u1.to_room != NULL
            &&  (can_see_room(ch,pexit->u1.to_room)
            ||   (IS_AFFECTED(ch,AFF_INFRARED) 
            &&    !IS_AFFECTED(ch,AFF_BLIND))))
          {
            found = TRUE;
            strcat(doors,IS_SET(pexit->exit_info,EX_CLOSED) ?  dir_name[door+6]:dir_name[door]);
          }
        }
        if (!found) strcat(buf,"none");
        do_printf(buf2,"%s",doors);
        i = buf2; break;
      case 'c' :
        do_printf(buf2,"%s","\n\r");
        i = buf2; break;
      case 'C' :
        do_printf(buf2,"%s%d",(IS_SET(ch->act, PLR_WANTED)) ?"W":"", ch->criminal);
        i = buf2; break;
      case 'h' :
        do_printf( buf2, "%d", ch->hit );
        i = buf2; break;
      case 'H' :
        do_printf( buf2, "%d", ch->max_hit );
        i = buf2; break;
      case 'm' :
        do_printf( buf2, "%d", ch->mana );
        i = buf2; break;
      case 'M' :
        do_printf( buf2, "%d", ch->max_mana );
        i = buf2; break;
      case 'v' :
        do_printf( buf2, "%d", ch->move );
        i = buf2; break;
      case 'V' :
        do_printf( buf2, "%d", ch->max_move );
        i = buf2; break;
      case 'x' :
        do_printf( buf2, "%d", ch->exp );
        i = buf2; break;
      case 'X' :
        do_printf(buf2, "%d", IS_NPC(ch) ? 0 : (ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);
        i = buf2; break;
      case 'g' :
        do_printf( buf2, "%u", ch->gold);
        i = buf2; break;
      case 's' :
        do_printf( buf2, "%u", ch->silver);
        i = buf2; break;
      case 'a' :
        if( ch->level > 9 ) do_printf( buf2, "%d", ch->alignment );
        else do_printf( buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ? "evil" : "neutral" );
        i = buf2; break;
      case 'r' :
        if( ch->in_room != NULL ) do_printf( buf2, "%s", 
          ((!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT)) ||
          (!IS_AFFECTED(ch,AFF_BLIND) && !room_is_dark( ch->in_room )))
          ? ch->in_room->name : "darkness");
        else do_printf( buf2, " " );
        i = buf2; break;
      case 'R' :
        if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
             do_printf( buf2, "%u", ch->in_room->vnum );
        else do_printf( buf2, " " );
        i = buf2; break;
      case 'z' :
        if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
             do_printf( buf2, "%s", ch->in_room->area->name );
        else do_printf( buf2, " " );
        i = buf2; break;
      case 'Z' :
        if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
             do_printf( buf2, "%s", flag_string( sector_flags, ch->in_room->sector_type ));
        else do_printf( buf2, " " );
        i = buf2; break;
      case '%' :
       do_printf( buf2, "%%" );
       i = buf2; break;
     case 'o' :
       do_printf( buf2, "%s", olc_ed_name(ch) );
       i = buf2; break;
     case 'O' :
       do_printf( buf2, "%s", olc_ed_vnum(ch) );
       i = buf2; break;
     case 'Q' :
       do_printf(buf2,"%d",ch->questpoints);
       i = buf2; break;
     case 'q' :
       if(ch->countdown>0) do_printf(buf2,"-%d",ch->countdown);
       else do_printf(buf2,"+%d",ch->nextquest);
       i = buf2; break;
    }
    ++str;
    while( (*point = *i) != '\0' ) ++point, ++i;
   }
   *point       = '\0';
   pbuff        = buffer;
   colourconv( pbuff, buf, ch );
   write_to_buffer( ch->desc, buffer, 0 );

   if (ch->prefix[0] != '\0') write_to_buffer(ch->desc,ch->prefix,0);
}

void do_dig( CHAR_DATA *ch, const char *argument )
{
  if (ch->race!=RACE_SKELETON)
  {
    stc("Ты не умеешь закапываться!.\n\r",ch);
    return;
  }
  stc("Но у тебя нет лопаты!\n\r",ch);
}

void do_guards( CHAR_DATA *ch)
{
  CHAR_DATA *vch,*vch_next,*guard;
  char *message;
  bool found_crime = FALSE;

  if (!IS_SET(ch->in_room->room_flags,ROOM_LAW))
  {
   stc("{YЭта комната не находится под защитой {WЗАКОНА{x\n\r",ch);
   return;
  }

  // Spawn a guard foreach criminal
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
  {
   vch_next = vch->next_in_room;
   if (!IS_NPC(vch) && (vch->criminal > 60))
   {
    found_crime = TRUE;

    act("{WОхранник ЗАКОНА появляется в комнате",ch,NULL,NULL,TO_ALL_IN_ROOM);
    if (IS_SET(vch->act,PLR_WANTED))
    guard = create_mobile(get_mob_index (MOB_VNUM_BOUNTYHUNTER)); // spawn strong killer 
    else 
    guard = create_mobile(get_mob_index (MOB_VNUM_LAWGUARD)); // spawn usual guard 
    
    char_to_room( guard, vch->in_room);
    switch (number_range(0,6))
    {
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
      default:        message = NULL;         break;
    }
    if (message != NULL) act(message,ch,NULL,NULL,TO_ROOM);
    multi_hit(guard,vch);
   }
 }

 if (found_crime)
 {
 // spawn some support 
 create_mobile(get_mob_index (MOB_VNUM_LAWGUARD)); 
 create_mobile(get_mob_index (MOB_VNUM_LAWGUARD)); 

 act("{YСтраж {WЗАКОНА{Y кричит '{MВсем остальным разойтись!{x'",ch,NULL,NULL,TO_ROOM);
 }
 else //false call
 {
  act("{YСтраж {WЗАКОНА{Y появляется и осматривается по сторонам!{x'",ch,NULL,NULL,TO_ROOM);
  act("{YСтраж {WЗАКОНА{Y не находит преступников и бросает $c1 {Yв {Wтюрьму,{Y за ложный вызов{x'",ch,NULL,ch,TO_NOTVICT);
  act("{YСтраж {WЗАКОНА{Y говорит тебе 'В следующий раз будешь знать, как нарушать покой'",ch,NULL,NULL,TO_CHAR);
  char_from_room(ch);
  char_to_room(ch, get_room_index(ROOM_VNUM_PRISON));
  stc("Теперь жди, пока тебя кто-то освободит.\n\r",ch);
 }
}
