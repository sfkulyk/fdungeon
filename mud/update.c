// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"

// for web who.htm
const char *fname_online;
const char *fname_who;

void advance_level( CHAR_DATA *ch, bool hide )
{
  char buf[MAX_STRING_LENGTH];
  int add_hp;
  int add_mana;
  int add_move;
  int add_prac;
  register int chclass;

  ch->pcdata->last_level = ( ch->played + (int) (current_time - ch->logon) ) / 3600;
  do_printf( buf, " %s",title_table [ch->class[ch->remort]] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
  if (IS_CFG(ch,CFG_AUTOTITLE)) set_title( ch, buf );

  chclass=0;
  if (ch->classcle) chclass=1;
  if (ch->classthi) chclass=2;
  if (ch->classwar) chclass=3;

  add_hp  = con_app[get_curr_stat(ch,STAT_CON)].hitp + number_range(class_table[chclass].hp_min,class_table[chclass].hp_max);
  add_hp += ((ch->size-2)*2);

  if (ch->classmag)
  {
   add_mana=(get_curr_stat(ch, STAT_INT)*3+get_curr_stat(ch, STAT_WIS))/2;
   add_mana=number_range(add_mana, add_mana/5*6);
  }
  else if (ch->classcle)
  {
   add_mana=(get_curr_stat(ch, STAT_INT)+get_curr_stat(ch, STAT_WIS)*3)/2;
   add_mana=number_range(add_mana/5*4, add_mana);
  }
  else if (ch->classthi)
  {
   add_mana=(get_curr_stat(ch, STAT_INT)*2+get_curr_stat(ch, STAT_WIS)*2)/3;
   add_mana=number_range(add_mana/3, add_mana/5*3);
  }
  else
  {
   add_mana=(get_curr_stat(ch, STAT_INT)*2+get_curr_stat(ch, STAT_WIS))/3;
   add_mana=number_range(add_mana/4, add_mana/2);
  }

  add_move = (get_curr_stat(ch,STAT_CON) + get_curr_stat(ch,STAT_DEX)*3)/4;
  add_move = number_range(add_move/4, add_move/2);

  add_prac = wis_app[get_curr_stat(ch,STAT_WIS)].practice;

  add_hp   = UMAX( 2, add_hp   );
  add_mana = UMAX( 2, add_mana );
  add_move = UMAX( 6, add_move );

  ch->max_hit  += add_hp;
  ch->max_mana += add_mana;
  ch->max_move += add_move;
  ch->practice += add_prac;
  if (ch->level<=5 || ch->remort==0) ch->train++;
  else if (ch->level/5*5==ch->level) ch->train++;

  ch->pcdata->perm_hit  += add_hp;
  ch->pcdata->perm_mana += add_mana;
  ch->pcdata->perm_move += add_move;

  ch->qcomplete[1]=0;

  if (!ch->clan && !IS_SET(ch->act,PLR_LASTREMORT) && ch->level>=PK_LEVEL)
  {
    stc( "Теперь ты одинокий воин.\n\r", ch );
    ch->clan=clan_lookup("loner");
    ch->clanrank = 0;
    ch->clanpet=NULL;
  }

  if (!hide)
  {
    ptc(ch,"Ты получаешь %d здоровья, %d маны, %d движений, и %d практик.\n\r", add_hp,  add_mana, add_move, add_prac);
    stc("Тебе теперь доступны такие навыки:",ch);
    do_printf(buf, "%d %d",ch->level,ch->level);
    do_skills(ch,buf);
    stc("Тебе теперь доступны такие заклинания:",ch);
    do_printf(buf, "%d %d",ch->level,ch->level);    do_spells(ch,buf);
  }
}

void gain_exp( CHAR_DATA *ch, int gain )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  register int i;

  if ( IS_NPC(ch) || ch->level >= LEVEL_HERO ) return;

  if (gain<1 || !IS_CFG(ch,CFG_GETEXP)) ch->exp = UMAX( exp_per_level(ch,ch->pcdata->points), ch->exp + gain);
  while ( ch->level < LEVEL_HERO && ch->exp >= exp_per_level(ch,ch->pcdata->points) * (ch->level+1) )
  {
    stc( "Ты поднимаешься на следующий уровень!!  ", ch );
    ch->level += 1;
    log_printf("%s Gained %d level",ch->name,ch->level);
    do_printf(buf,"$C1 достигает %d уровня!",ch->level);
    info(ch,0,1,ch->name,"достигает следующей ступени!");
    wiznet(buf,ch,NULL,WIZ_LEVELS,0);
    advance_level(ch,FALSE);

    // The end of quite newbie's life
    if ((ch->level == 5) && (ch->remort == 0) )
    {
                 stc("\n\r/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\\",ch);
      do_printf( buf,"\n\r|   Теперь ты стал%1s старше и перед тобой открываются новые|__/",ch->sex==1?"":"a");
                 stc(buf,ch);
                 stc("\n\r| горизонты. Школа останется приятным воспоминанием,      |",ch);
                 stc("\n\r| которое ты пронесешь в своем сердце на протяжении долгих|",ch);
                 stc("\n\r| лет своей славной жизни.                                |",ch);
                 stc("\n\r|   У тебя появится много трудностей. Охранять тебя будет | ",ch);
                 stc("\n\r| больше некому и тебе придется научиться с достоинством  | ",ch);
                 stc("\n\r| смотреть в глаза противнику. От твоей руки прольется    | ",ch);
                 stc("\n\r| много {rкрови{x. Иногда ее придется проливать отстаивая свою| ",ch);
                 stc("\n\r| честь или жизнь.                                        | ",ch);
                 stc("\n\r|    Иди и пусть мужество и благородство не покинут тебя! | ",ch);
                 stc("\n\r\\_________________________________________________________\\__/\n\r",ch);
    }

    // character gains 17 level
    if (ch->level == 17)
    {
         stc("\n\r/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\\",ch);
         stc("\n\r|                       {RПОВЕСТКА{x                         | __/",ch);
         stc("\n\r|      {Yоб обязательной явке на призывной участок{x         |",ch);
         stc("\n\r|            {Yдля выполнения воинского долга{x              |",ch);
         stc("\n\r|________________________________________________________|",ch);
         stc("\n\r|                                                        |",ch);
      ptc(ch,"\n\r|  {YПризывни%2s  {W%-18s{Y, вы обязаны явиться на{x |",ch->sex==1?"к":"ца",ch->name);
         stc("\n\r|  {Yпризывной участок по достижении вами 18 уровня.{x       |",ch);
         stc("\n\r|  {YПри себе иметь : голый торс. Все что останется от{x     |",ch);
         stc("\n\r|  {Yголого торса, оставить дома. Примите во внимание,{x     |",ch);
         stc("\n\r|  {Yчто вещи, найденые у вас при поступлении на военную {x  |",ch);
         stc("\n\r|  {Yслужбу, будут конфискованы и уничтожены для{x           |",ch);
         stc("\n\r|  {Yвоспрепятствования размножению вредных насекомых{x      |",ch);
         stc("\n\r|                                                        |",ch);
         stc("\n\r|                                        {CМайор {rМолчанов{x  |",ch);
         stc("\n\r|                                                        | ",ch);
         stc("\n\r\\_________________________________________________________\\__/\n\r",ch);
    }

    // character gains 17 level and not army
    if ((ch->level>=18) && (!IS_SET(ch->act,PLR_ARMY)) && (ch->level<21))
    {
      SET_BIT(ch->act, PLR_ARMY);
      stc( "\n\r{rАрмия{g ждет тебя {Gс распростертыми обьятиями...{x\n\r", ch );
      info(ch,0,1,ch->name,"уходит служить в {RАрмию{x!");
      /* exclude char from Auction and sell his objects */
      if (auction->buyer==ch)
      {
        do_printf (buf, "Текущий покупатель уходит в армию, аукцион проводится заново.");
        talk_auction(buf);
        auction->going=0;
        auction->bet=0;
        auction->buyer->gold += auction->bet;
        stc ("Ваши деньги возвpащены.\n\r",auction->buyer);
      }
      if (auction->seller==ch)
      {
        do_printf (buf, "Аукционер уходит в армию, лот %s снЯт с аукциона.",get_obj_desc(auction->item,'1'));
        talk_auction(buf);
        act ("Аукционеp поЯвлЯется пеpед тобой и возвpащает тебе $i4.{/"
             "Толстый прапорщик незаметно подкрадывается, выхватывает $i4{/"
             "и со злобной ухмылкой распихивает ништяки по карманам.",
              auction->seller,auction->item,NULL,TO_CHAR);
        act ("Аукционеp поЯвлЯется пеpед $c5 и возвpащает $u $i4.{/"
             "Толстый прапорщик незаметно подкрадывается, выхватывает $i4{/"
             "и со злобной ухмылкой распихивает ништяки по карманам.",
             auction->seller,auction->item,NULL,TO_ROOM);
        extract_obj(auction->item);
        auction->item = NULL; /* clear auction */
      }
      if (( ch->gold > 0 )||(ch->silver > 0))
      {
        ch->gold=0;
        ch->silver=0;
      }
      if (ch->fighting!=NULL) stop_fighting(ch, TRUE);
      for ( obj = ch->carrying; obj != NULL; obj = obj_next )
      {
        obj_next = obj->next_content;
        extract_obj( obj );
      }
      while ( ch->affected )
      affect_remove( ch, ch->affected );
      ch->affected_by = race_table[ch->race].aff;
      for (i = 0; i < 4; i++) ch->armor[i]= 100;
      ch->position= POS_RESTING;
      char_from_room(ch);
      char_to_room(ch,get_room_index(12401));
      do_function (ch, &do_look, "");
      do_function (ch, &do_outfit,"");
    }
    // character gains 22 level (Army's end)
    if ( (ch->level>21) && IS_SET(ch->act,PLR_ARMY))
    {
      stc("\n\rВходит майор {rМолчанов{x.\n\r", ch);
      stc("Чем-то это лицо тебе знакомо... Аааа! Это тот тип, который тебя сюда определил!\n\r",ch);
      ptc(ch,"Майор {rМолчанов{x произосит: '{G%s! Ваш срок службы истек. Можете быть свободны.{x\n\r",ch->name);
      stc("Майор {rМолчанов{x уводит тебя.\n\r",ch);

      act("\n\rВходит майор {rМолчанов{x.", ch,NULL,NULL,TO_ROOM);
      act("Майор {rМолчанов{x произосит: '{G$n! Ваш срок службы истек. Можете быть свободны.", ch,NULL,NULL,TO_ROOM);
      act("Майор {rМолчанов{x уводит $n.", ch,NULL,NULL,TO_NOTVICT);

      WAIT_STATE(ch, 5*PULSE_VIOLENCE);

      ch->position= POS_RESTING;
      char_from_room(ch);
      char_to_room(ch,get_room_index(12482));
      do_function (ch, &do_look, "");
    }

    // character gains 101 level (Superhero)
    if (ch->level==101)
    {
      DESCRIPTOR_DATA *d;
      CHAR_DATA *victim;

      for (d = descriptor_list; d != NULL; d = d->next)
      {
        if (d->connected!=CON_PLAYING) continue;
        victim = d->character;
        if (victim == NULL || IS_NPC(victim)) continue;
        affect_strip(victim,gsn_plague);
        affect_strip(victim,gsn_poison);
        affect_strip(victim,gsn_blindness);
        affect_strip(victim,gsn_sleep);
        affect_strip(victim,gsn_curse);

        victim->hit  = victim->max_hit;
        victim->mana = victim->max_mana;
        victim->move = victim->max_move;
        update_pos( victim);
      }
      gecho("{/{DГром и вспышки {Cмолний{D возвещают о рождении {CSUPERHERO{D!{*{/{x");
    }

    if (ch->in_room == NULL)
    {
     bug("Char in NULL room",0);
     char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
    }
    save_char_obj(ch);
  }
}

void sifilis_update(CHAR_DATA *ch)
{
  int64 vnum=0;

  switch(number_range(1,10))
  {
    case  2:
      if (IS_SET(race_table[ch->race].parts,PART_GUTS))
      {
        act("Кишки {y$c2{x pасплескиваются во все стоpоны.",ch,NULL,NULL,TO_ROOM);
        act("Кишки вываливаются из твоего прогнившего живота прямо тебе под ноги.",ch,NULL,NULL,TO_CHAR);
        vnum = OBJ_VNUM_GUTS;
      }
      break;
    case  6:
      if (IS_SET(race_table[ch->race].parts,PART_ARMS))
      {
        act("Рука {y$c2{x отваливается от $g гниющего тела.",ch,NULL,NULL,TO_ROOM);
        act("Твоя рука сгнивает и отваливается от тела.",ch,NULL,NULL,TO_CHAR);
        vnum = OBJ_VNUM_SLICED_ARM;
      }
      break;
    case  9:
      if (IS_SET(race_table[ch->race].parts,PART_LEGS))
      {
        act("Нога {y$c2{x отваливается от $g гниющего тела.",ch,NULL,NULL,TO_ROOM);
        act("Кусок твоей сгнившей ноги отваливается от тела.",ch,NULL,NULL,TO_CHAR);
        vnum = OBJ_VNUM_SLICED_LEG;
      }
    break;
  }

  if ( vnum != 0 ) {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    const char *name;

    name = IS_NPC(ch) ? get_char_desc(ch,'2'): ch->name;
    obj = create_object( get_obj_index( vnum ), 0 );
    obj->timer = number_range( 4, 7 );
    do_printf( buf, obj->short_descr, name );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );
    do_printf( buf, obj->description, name );
    free_string( obj->description );
    obj->description = str_dup( buf );
    if (obj->item_type == ITEM_FOOD) {
      if (IS_SET(ch->form,FORM_POISON)) obj->value[3] = 1;
      else if (!IS_SET(ch->form,FORM_EDIBLE)) obj->item_type = ITEM_TRASH;
    }
    obj_to_room( obj, ch->in_room );
  }
}

//----------
void drent_update (CHAR_DATA *ch)
{
  CHAR_DATA *vch,*vch_next;

  if (ch->pcdata->account<1)
  {
    if (ch->gold<1)
    {
      vch_next = ch->next_in_room;
      for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
        vch_next = vch->next_in_room;
        if(!IS_AWAKE(vch))
        {
          act("Ты просыпаешься от страшного рева",ch,NULL,NULL,TO_ALL_IN_ROOM);
          do_function(vch,&do_wake,"");
        }
      }
      act("{DПризрак Фрамина{x с криком '{MБарук Казад!{x' выбрасывает {Y$c1{x в окно спальни.{/Потом он разворачивается и говорит:'{GНет денег - спи в храме!'{x ",ch,NULL,NULL,TO_ROOM);
      act("В который раз вылетая из окна, ты снова убеждаешься в справедливости одной простой истины:{/{YДварфы любят за деньги...{x.",ch,NULL,ch,TO_CHAR);
      char_from_room( ch );
      char_to_room( ch, get_room_index(ROOM_VNUM_KOZEL));
      act("Из окна {yтаверны{x вылетает очередной заспанный клиент.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
      ch->gold--;
      guild_table[guild_lookup("dwarves guild")].gold++;
    }
  }
  else
  {
    guild_table[guild_lookup("dwarves guild")].gold++;
    ch->pcdata->account--;
  }
}

void sect_water_noswim_update (CHAR_DATA *ch)
{
 OBJ_DATA *obj;
 bool boat=FALSE;

 if (IS_IMMORTAL(ch)) return;

 for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
 {
  if ( obj->item_type == ITEM_BOAT )
  {
   boat = TRUE;
   break;
  }
 }
 if ( !boat )
 {
  stc("Ты тонешь!\n\r",ch);
  if (ch->hit>200)
     damage(ch,ch,UMIN(100,ch->hit-1),0,DAM_NONE,FALSE, FALSE, NULL);
  else
     damage(ch,ch,UMIN(15,ch->hit-1),0,DAM_NONE,FALSE, FALSE, NULL);
 }
}

void sect_uwater_update (CHAR_DATA *ch)
{
 OBJ_DATA *obj;
 bool scuba=FALSE;
 int dam;

 if (IS_IMMORTAL(ch)) return;

 for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
 {
  if ( obj->item_type == ITEM_SCUBA )
  {
   scuba = TRUE;
   break;
  }
 }
 if ( !scuba )
  {
   stc("Ты тонешь!\n\r",ch);
   dam = UMAX(ch->max_hit/4, ch->hit/4);
   damage( ch, ch, dam, gsn_backstab,DAM_NONE,FALSE, TRUE, NULL);
  }
}

void light_update (CHAR_DATA *ch)
{
 if (!IS_NPC(ch))
 {
  OBJ_DATA *obj=get_eq_char(ch,WEAR_LIGHT);

  if ( obj && obj->item_type == ITEM_LIGHT)
  {
   if ((obj->value[2]==-1 || obj->value[2]>998)) obj->value[2]=300;
   if (obj->value[2] == 0)
   {
    bug("Light has value2 = 0",0);
    obj->value[2]=1;
   }
   if ( --obj->value[2] <= 0 && ch->in_room && ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
   {
    --ch->in_room->light;
    act( "$i1 мерцает.", ch, obj, NULL, TO_ROOM );
    act( "$i1 мерцает и затухает.", ch, obj, NULL, TO_CHAR );
    extract_obj( obj );
   }
   else
   if ( obj->value[2] <= 5 && ch->in_room != NULL)
    act("$i1 мерцает.",ch,obj,NULL,TO_CHAR);
  }
 }
}

//-----------

void nostalgia_update(CHAR_DATA *ch)
{
 switch(number_range(1,10))
 {
  case 1: do_function(ch,do_emote,"тихо плачет.");
          break;
  case 2: do_function(ch,do_say,"Мама! Забери меня отсюда! Я буду вести себя хорошо...");
          break;
  case 3: do_function(ch,do_emote,"заливается слезами.");
          break;
  case 4: do_function(ch,do_emote,"жалобно всхлипывает.");
          break;
  case 5: do_function(ch,do_emote,"затравленно оглядывается по сторонам.");
          break;
  case 6: do_function(ch,do_say,"За что мне такое наказание? Отпустите меня... я волшебное слово знаю...");
          break;
  case 7: do_function(ch,do_emote,"достает из кармана огромный клетчатый платок и шумно сморкается.");
          break;
  case 8: do_function(ch,do_emote,"тоскливо смотрит в никуда.");
          break;
  case 9: do_function(ch,do_emote,"неуверенно оглядывается по сторонам.");
          do_function(ch,do_say,"Какого хрена я здесь вообще делаю?");
          break;
  case 10:do_function(ch,do_say,"Уу, саммонеры чертовы! Оторвать бы им... руки...");
          break;
  default:do_function(ch,do_emote,"рыдает.");
          break;
 }
}

//--------
void plague_update (CHAR_DATA *ch)
{
 AFFECT_DATA *af, plague;
 CHAR_DATA *vch;
 int dam;

 if (IS_IMMORTAL(ch))  return;

 if (ch->in_room == NULL) return;

 act("$c1 вскрикивает от боли в страшных чумных язвах.",ch,NULL,NULL,TO_ROOM);
 stc("Ты бьешься в агонии от чумы.\n\r",ch);
 for ( af = ch->affected; af != NULL; af = af->next )
  if(af->type == gsn_plague) break;

 if (af == NULL)
 {
  REM_BIT(ch->affected_by,AFF_PLAGUE);
  return;
 }

 if (af->level == 1) return;

 plague.where    = TO_AFFECTS;
 plague.type     = gsn_plague;
 plague.level    = af->level - 1;
 plague.duration = number_range(1,2 * plague.level);
 plague.location = APPLY_STR;
 plague.modifier = -5;
 plague.bitvector = AFF_PLAGUE;

 for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
 {
  if (!saves_spell(plague.level - 2,vch,DAM_DISEASE)
  &&  !IS_IMMORTAL(vch)
// a esli y tebya immunitet - ne zarazishsya (c) Wagner
  &&  check_immune(vch,DAM_DISEASE)!=IS_IMMUNE
  &&  !IS_AFFECTED(vch,AFF_PLAGUE) && number_bits(4) == 0)
  {
   stc("Ты чувствуешь жар по всему телу.\n\r",vch);
   act("$c1 дрожит и выглядит очень больным.",vch,NULL,NULL,TO_ROOM);
   affect_join(vch,&plague);
  }
 }

 dam = UMAX(ch->level*2,af->level/5+1);
 if (IS_AFFECTED(ch,AFF_HASTE)) dam*=2;
 if (IS_AFFECTED(ch,AFF_SLOW)) dam/=2;
 if ( check_immune(ch,DAM_DISEASE)!=IS_IMMUNE)
 {
   ch->mana -= dam;
   ch->move -= dam;
 }
// Questmobs won't die (c) Wagner
 if (IS_NPC(ch) && ch->questmob!=NULL)
    dam=UMIN(abs(ch->hit-1),dam);

 damage( ch, ch, dam, gsn_plague,DAM_DISEASE,FALSE, FALSE, NULL);
}

//--------
void poison_update(CHAR_DATA *ch)
{
 AFFECT_DATA *poison;
 poison = affect_find(ch->affected,gsn_poison);

 if (IS_IMMORTAL(ch))  return;

 if (poison != NULL)
 {
  int tdam;

  act( "$c1 дрожит и стонет.", ch, NULL, NULL, TO_ROOM );
  stc( "Тебя бьет озноб и ты стонешь от боли.\n\r", ch );

  tdam=poison->level + 1;
  if (IS_AFFECTED(ch,AFF_HASTE)) tdam*=2;
  if (IS_AFFECTED(ch,AFF_SLOW)) tdam/=2;
// Questmobs won't die (c) Wagner
  if (IS_NPC(ch) && ch->questmob!=NULL)
     tdam=UMIN(abs(ch->hit-1),tdam);
   damage(ch,ch,tdam,gsn_poison,DAM_POISON,FALSE, FALSE, NULL);
 }
}

void hungry_damage(CHAR_DATA *ch, int Cond)
{
  int condition;

  if( is_affected( ch, gsn_sleep) ) return;
  condition=ch->pcdata->condition[Cond];
  if (Cond==COND_HUNGER) stc("У тебя кружится голова от голода.\n\r",ch);
  else stc("У тебя кружится голова от жажды.\n\r",ch);

  if (ch->level<5) condition/=2;
  if (Cond==COND_HUNGER) condition*=2;

  if (!ch->in_room) return;
  if (ch->in_room->vnum == 3 || ch->in_room->vnum==2) return;
  damage(ch,ch,abs(condition*(1+ch->level/10)),TYPE_UNDEFINED,DAM_NONE,FALSE, FALSE, NULL);
}

void gain_condition( CHAR_DATA *ch, int iCond, int64 value )
{
  int condition;
  if (value==0 || IS_NPC(ch)) return;

  condition = ch->pcdata->condition[iCond];
  ch->pcdata->condition[iCond] = URANGE(-10, condition + (int)value, 48 );
  if ( ch->pcdata->condition[iCond] <= 0 )
  {
    switch ( iCond )
    {
      case COND_HUNGER:
        if (IS_SET(race_table[ch->race].spec,SPEC_NOEAT) || ch->level >= LEVEL_IMMORTAL)
        {
          ch->pcdata->condition[iCond] = 20;
          return;
        }
        if (condition<=-3) hungry_damage(ch, iCond);
        else stc( "Ты хочешь есть.\n\r",ch );
        break;
      case COND_THIRST:
        if (IS_SET(race_table[ch->race].spec,SPEC_NODRINK) || ch->level >= LEVEL_IMMORTAL)
         {
          ch->pcdata->condition[iCond] = 20;
          REM_BIT(ch->act,PLR_MUSTDRINK);
          return;
         }
        if (condition<=-3) hungry_damage(ch, iCond);
        else stc( "Ты хочешь пить.\n\r", ch );
        break;
      case COND_DRUNK:
        if ( condition > 0 ) stc( "Ты протрезвел.\n\r", ch );
        ch->pcdata->condition[iCond] = 0;
        break;
      case COND_ADRENOLIN:
        if ( condition > 0 ) stc( "{GТы успокоился.{x\n\r", ch );
        ch->pcdata->condition[iCond] = 0;
        break;
    }
  }
}

// Mob autonomous action.
// This function takes 25% to 35% of ALL Merc cpu time. Furey
void mobile_update( void )
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *victim, *v_next;
  EXIT_DATA *pexit;
  register int door;
  const int stealer_update=0;


  // Examine all mobs.
  for ( ch = char_list; ch != NULL; ch = ch_next )
  {
   ch_next = ch->next;

   if ( !IS_NPC(ch))
   {
     ch->mprog_delay--;
     if (ch->mprog_delay<=0)
     {
       ch->mprog_delay=0;
       switch(ch->start_pos)
       {
        case CASTING_WEB:
          stc("У тебя мало сил для этого заклинания.\n\r",ch);
          break;
        default:
          break;
       }
       ch->start_pos=0;
     }
     continue;
   }
   if (!ch->in_room ||
    (ch->in_room->area->empty && !IS_SET(ch->act,ACT_UPDATE_ALWAYS))) continue;

   if (IS_AFFECTED(ch,AFF_NOSTALGIA)
      && (ch->fighting!=NULL
       || IS_AFFECTED(ch,AFF_CALM)
       || IS_AFFECTED(ch,AFF_CHARM)
       || IS_AFFECTED(ch,AFF_BLIND)
       || ch->position==POS_SLEEPING))
      {
        affect_strip(ch,skill_lookup("nostalgia"));
        REM_BIT(ch->affected_by,AFF_NOSTALGIA);
        act("$c1 воспрянул духом.",ch,NULL,NULL,TO_ROOM);
      }

   if (IS_AFFECTED(ch,AFF_CHARM)) continue;

   // Examine call for special procedure
   if ( ch->spec_fun != 0 ) if ( (*ch->spec_fun) ( ch ) ) continue;

   // Midgaard Healer kills mobiles in room
   if (ch->pIndexData->vnum==3012)
   {
     for (victim=ch->in_room->people;victim!=NULL;victim=v_next)
     {
       v_next = victim->next_in_room;
       if (victim!=ch && IS_NPC(victim) && !IS_AFFECTED(victim,AFF_CHARM)
        && victim->questmob==NULL )
       {
        act("$c1 достает из чемоданчика {RОГРОМНЫЙ СКАЛЬПЕЛЬ{x и отрезает голову {Y$C3{x.",ch,NULL,victim,TO_ROOM);
        raw_kill(victim);
        if (!IS_SET(victim->act,ACT_EXTRACT_CORPSE))
        {
         act("Тело $C2 мгновенно рассыпается в прах.",ch,NULL,victim,TO_ROOM);
         extract_obj(get_obj_list(ch,"corpse",ch->in_room->contents));
        }
       }
     }
   }

   if (race_table[ch->pIndexData->race].wealth || ch->pIndexData->pShop )
   {
     if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth)
     {
       ch->gold += ch->pIndexData->wealth * number_range(1,20)/5000000;
       ch->silver += ch->pIndexData->wealth * number_range(1,20)/50000;
     }
     stealer_update++;
     if (ch->stealer!=NULL && !EMPTY(ch->stealer) && stealer_update>3000)
     {
       stealer_update=0;
       remove_one_stealer(ch);
     }
   }

   // That's all for sleeping / busy monster, and empty zones
   if ( ch->position != POS_STANDING ) continue;

   // Scavenge
   if ( IS_SET(ch->act, ACT_SCAVENGER)
     &&   ch->in_room->contents != NULL
     &&   number_bits( 6 ) == 0 )
   {
     OBJ_DATA *obj;
     OBJ_DATA *obj_best;
     int64 max;

     max         = 1;
     obj_best    = 0;
     for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
     {
       if ( CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj)
         && obj->cost > max  && obj->cost > 0)
       {
         obj_best    = obj;
         max         = obj->cost;
       }
     }

     if ( obj_best )
     {
       obj_from_room( obj_best );
       obj_to_char( obj_best, ch );
       act( "$c1 подбирает $i4.", ch, obj_best, NULL, TO_ROOM );
     }
   }

   // Check triggers only if mobile still in default position
   if ( ch->position == ch->pIndexData->default_pos )
   {
     if ( HAS_TRIGGER( ch, TRIG_DELAY) && ch->mprog_delay > 0 )
     {
       if ( --ch->mprog_delay <= 0 )
       {
         mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_DELAY );
         continue;
       }
     }
     if ( HAS_TRIGGER( ch, TRIG_RANDOM) )
     {
       if( mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_RANDOM ) )
       continue;
     }
   }

   // Wander
   door=number_range(0,15);
   pexit=ch->in_room->exit[door];
   if (   !IS_SET(ch->act, ACT_SENTINEL)
       && door<=5
       && pexit && pexit->u1.to_room
       && !IS_SET(pexit->exit_info,EX_CLOSED)
       && !IS_SET(pexit->u1.to_room->room_flags,ROOM_NO_MOB)
       && (!IS_SET(ch->act,ACT_STAY_AREA)
          || ch->in_room->area==pexit->u1.to_room->area)
       && (!IS_SET(ch->act,ACT_STAY_PLAIN)
          || ch->in_room->sector_type==pexit->u1.to_room->sector_type)
       && ( !IS_SET(ch->act,ACT_STOPDOOR)
          || (  (IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)
              && IS_SET(ch->in_room->room_flags,ROOM_INDOORS))
          ||   (!IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)
              && !IS_SET(ch->in_room->room_flags,ROOM_INDOORS)) )))
   {
     dlog("Door: [%d]\n\r",door);
     do_move_char( ch, door, FALSE,FALSE );
   }

   if (IS_SET(global_cfg,CFG_BIRTHDAY) && ch->position==ch->pIndexData->default_pos &&
       number_range(1,50)>48)
     act("{Y$n{x $t",ch,bdmsg[number_range(0,9)],NULL,TO_ROOM);

   if (IS_SET(global_cfg, CFG_NEWYEAR)
    &&  ch->in_room->people!=NULL
    &&  ch->position!=POS_FIGHTING)
   {
     if (number_range(1,100)>95)
     {
        CHAR_DATA *nir;  //next_in_room;

        switch (number_range(1,6))
        {
          case 1: do_function(ch, &do_say,"С Новым Годом! С новым счастьем! С новым спиртным!");
                 do_function(ch, &do_emote,"хлещет водку.");
                 do_function(ch, &do_emote,"глупо улыбается и лезет к тебе целоваться.");
                 break;
          case 2: do_function(ch, &do_emote,"тянет куда-то здоровенную {Gелку{x.");
                 do_function(ch, &do_say,"Маленькой елочке холодно зимой.... Глотнуть что ли для сугреву?");
                 do_function(ch, &do_emote,"достает из кармана бутылку коньяка и жадно пьет из горлышка.");
                 do_function(ch, &do_say,"Эхх, хор-рош-шо!.. Из лесу елочку взяли мы домой...");
                 break;
          case 3: do_function(ch, &do_say,"И в этот здамедательный день... я хочу... ик!");
                 do_function(ch, &do_emote,"смотрит на тебя бессмысленным взглядом.");
                 do_function(ch, &do_say,"Пьяздьявьяю с Новым Годом, желаю счастья в личной жизни! Ик!");
                 do_function(ch, &do_emote,"достает из кармана бутылку пива '{yОболонь{x'.");
                 do_function(ch, &do_say,"Ваш-ше зда-а-ровье! Ик!");
                 break;
          case 4:
                 if (get_obj_carry(ch,"condom",ch)!=NULL) break;
                 do_function(ch, &do_emote,"радостно улыбается и спешит к тебе.");
                 do_function(ch, &do_say,"Поздравляю Вас!");
                 do_function(ch, &do_say,"Вы стали победителем в нашей еженовогодней лотерее '{MПопадись мне в темном переулке!{x'");
                 do_function(ch, &do_say,"Позвольте мне вручить Вам этот скромный новогодний {Rс{Cу{Yв{Gе{Mн{Wи{Bр{x.");
                 do_function(ch, &do_emote,"достает из кармана украшенную яркими лентами упаковку {Yп{Gр{Wе{Dз{Cе{Rр{Yв{Gа{Wт{Dи{Cв{Rо{Yв{x и вручает тебе.");
                 if (ch->in_room!=NULL)
                  for(nir=ch->in_room->people;nir!=NULL;nir=nir->next_in_room)
                    if (!IS_NPC(nir))
                      obj_to_char(create_object(get_obj_index(23014),0),nir);
                 break;
          case 5: do_function(ch, &do_say, "Слышь, друган! Новый Год, понимаешь... а денег нету...");
                 do_function(ch, &do_say, "Страсть как хочется похмелиться...");
                 do_function(ch, &do_emote,"наклоняется к тебе, странно ухмыляясь.");
                 do_function(ch, &do_say, "Одолжи на бутылку...");
                 break;
          case 6: do_function(ch, &do_say, "Какой чудесный день... какой чудесный я...");
                 do_function(ch, &do_emote,"напевает себе под нос, похлебывая пиво из {Yогромной {Wк{wр{Dу{wж{Wк{wи{x.");
                 do_function(ch, &do_say, "С Новым Годом! Хочешь пивка?");
                 do_function(ch, &do_emote,"достает {Rдв{rу{Rхл{rи{Rтр{rо{Rву{rю{x кружку, наполняет ее {yпивом{x из бочки и дает тебе.");
                 break;
          case 7: do_function(ch, &do_emote,"умиленно оглядывается по сторонам.");
                 do_function(ch, &do_say, "Праздник! Детишки радуются! Музыка играет! Народ похмеляется! Хорошо-то как!");
                 do_function(ch, &do_say,"Как я люблю Новый Год!");
                 do_function(ch, &do_emote,"достает из кармана спички и, радостно улыбаясь, поджигает большую {Mн{Yа{Cр{Rя{Bж{Wе{Yн{Cн{Mу{Bю {Gелку{x.");
                 do_function(ch, &do_emote,"счастливо улыбается.");
                 break;
          default: break;
        }
      }
    }
  }
}

// Update the weather
void weather_update( void )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  register int diff;

  buf[0] = '\0';
/*  Update by Wagner. */

    switch ( ++time_info.hour )
    {
    case  5:
        weather_info.sunlight = SUN_LIGHT;
        if (weather_info.sky > SKY_CLOUDY)
            strcat( buf, "Наступает утро.\n\r" );
        else
            strcat( buf, "{WСветает{x.\n\r" );
        break;

    case  6:
        weather_info.sunlight = SUN_RISE;
        if (weather_info.sky >= SKY_CLOUDY)
            strcat( buf, "Где-то за {Dпеленой туч{x, из-за горизонта появляется {yсолнце{x.\n\r");
        else
            strcat( buf, "На горизонте появляется {Yсолнце{x.\n\r" );
        break;

    case 19:
        weather_info.sunlight = SUN_SET;
        if (weather_info.sky < SKY_CLOUDY)
            strcat( buf, "{YСолнце{x прячется за горизонтом.\n\r" );
        else
            strcat( buf, "{wСтановится темнее{x.\n\r");
        break;

    case 20:
        weather_info.sunlight = SUN_DARK;
        if (weather_info.sky > SKY_CLOUDY)
            strcat( buf, "На небе появляются {Wз{Dв{Wе{Dз{Wд{Dы{x.\n\r" );
        else
            strcat( buf, "{wНаступила {DНОЧЬ{x.\n\r" );
        break;

    case 24:
        time_info.hour = 0;
        time_info.day++;
        break;
    }

  if ( time_info.day   >= 35 )
  {
    time_info.day = 0;
    time_info.month++;
  }

  if ( time_info.month >= 17 )
  {
    time_info.month = 0;
    time_info.year++;
  }

  // Weather change
  if ( time_info.month >= 9 && time_info.month <= 16 )
    diff = weather_info.mmhg >  985 ? -2 : 2;
  else
    diff = weather_info.mmhg > 1015 ? -2 : 2;

  weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
  weather_info.change    = UMAX(weather_info.change, -12);
  weather_info.change    = UMIN(weather_info.change,  12);

  weather_info.mmhg += weather_info.change;
  weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
  weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

  switch ( weather_info.sky )
  {
    default:
      bug( "Weather_update: bad sky %d.", weather_info.sky );
      weather_info.sky = SKY_CLOUDLESS;
      break;

    case SKY_CLOUDLESS:
        if ( weather_info.mmhg <  990
        || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
        {
            strcat( buf, "{DГрозовые{x тучи затягивают небо.\n\r" );
            weather_info.sky = SKY_CLOUDY;
        }
        break;

    case SKY_CLOUDY:
        if ( weather_info.mmhg <  970
        || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
        {
            strcat( buf, "Начинается дождь.\n\r" );
            weather_info.sky = SKY_RAINING;
        }

        if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
        {
            strcat( buf, "Облака рассеиваются.\n\r" );
            weather_info.sky = SKY_CLOUDLESS;
        }
        break;

    case SKY_RAINING:
        if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
        {
            strcat( buf, "{*Гремит гром и сверкают молнии.\n\r" );
            weather_info.sky = SKY_LIGHTNING;
        }

        if ( weather_info.mmhg > 1030
        || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
        {
            strcat( buf, "Дождь прекращается.\n\r" );
            weather_info.sky = SKY_CLOUDY;
        }
        break;

    case SKY_LIGHTNING:
        if ( weather_info.mmhg > 1010
        || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
        {
            strcat( buf, "Буря утихает.\n\r" );
            weather_info.sky = SKY_RAINING;
            break;
        }
        break;
    }

  if ( buf[0] != '\0' )
  {
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
      if ( d->connected != CON_PLAYING) continue;
      if (IS_SET(race_table[d->character->race].spec,SPEC_VAMPIRE) && time_info.hour==20)
         stc("Ты чувствуешь прилив сил! Ты жаждешь {RКРОВИ!{x\n\r", d->character);
      if (IS_SET(race_table[d->character->race].spec,SPEC_VAMPIRE) && time_info.hour==6)
         stc("Ты чувствуешь что силы оставляют тебя.\n\r", d->character);
      if (IS_OUTSIDE(d->character) && IS_AWAKE(d->character) )
         stc( buf, d->character );
    }
  }
}

// Update all chars, including mobs
void char_update( void )
{
  CHAR_DATA *ch,*ch_next;
  int const save_number = 0;

  save_number++; // update save counter

  if (save_number > 29) save_number = 0;

  for ( ch = char_list; ch != NULL; ch = ch_next )
  {
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    ch_next=ch->next;
    if (!IS_NPC(ch))
    {
      if (ch->penalty) penalty_update(ch);
      if (++ch->timer>30 && !IS_IMMORTAL(ch))
      {
        save_char_obj( ch );
        stc("Autologout in progress.\n\r",ch);
        if (ch->desc) close_socket(ch->desc);
        extract_char(ch, TRUE);
        continue;
      }
      // Char in Limbo will not update (affects/status..)
      if (ch->in_room == get_room_index(ROOM_VNUM_LIMBO)) continue;
      if (ch->pcdata->dc > 0 ) ch->pcdata->dc --;
      if (ch->pcdata->dc == 0 && IS_SET(ch->act, PLR_DISAVOWED) )
         REM_BIT( ch->act, PLR_DISAVOWED);
      if (ch->desc && ch->desc->connected!=CON_PLAYING) continue;
      if (ch->in_room && IS_SET(ch->in_room->room_flags,ROOM_DWARVES_RENT))
         drent_update(ch);

      if (IS_DEVOTED_ANY(ch) && number_percent()==77)
      {
          change_favour(ch, -1);
          stc("Твое божество потихоньку забывает о тебе.\n\r", ch);
      }

      // do not put in limbo characters that are in creation
      if (ch->timer>=ch->settimer && ch->desc && !IS_IMMORTAL(ch))
      {
        if (ch->fighting && ch->pcdata->condition[COND_ADRENOLIN]==0)
           stop_fighting(ch, TRUE);
        act("$n растворяется в пустоте.", ch, NULL, NULL, TO_ROOM);
        stc("Ты растворяешься в пустоте.\n\r", ch);
        if ( ch->level > 1 ) save_char_obj(ch);
        ch->was_in_room=ch->in_room;
        char_from_room(ch);
        char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
        continue;
      }
      if (IS_CFG(ch,CFG_TICK)) stc(ch->pcdata->tickstr,ch);

      if (IS_SET(ch->act, PLR_SIFILIS) && number_percent() < 50)
         sifilis_update(ch);

      if ( ch->in_room !=NULL && ch->in_room->sector_type == SECT_WATER_NOSWIM
        && (!IS_NPC(ch)) && (!IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_SWIM)))
      sect_water_noswim_update(ch);

      if ( ch->in_room !=NULL && ch->in_room->sector_type == SECT_UWATER
        && (!IS_NPC(ch)) && (!is_affected(ch,skill_lookup("wbreath"))
        && !IS_SET(race_table[ch->race].spec,SPEC_UWATER)))
      sect_uwater_update(ch);

      if ( ch->position >= POS_STUNNED )
      {
       // check to see if we need to go home
       if (IS_NPC(ch) && ch->zone && ch->zone != ch->in_room->area
         && ch->desc &&  ch->fighting == NULL
         && !IS_AFFECTED(ch,AFF_CHARM) && number_percent() < 5)
       {
         act("$c1 возвращается в сознание.",ch,NULL,NULL,TO_ROOM);
         extract_char(ch,TRUE);
         continue;
       }

       if ( ch->position == POS_STUNNED )  update_pos( ch );
      }

      if ( (ch->criminal > 0 || IS_SET(ch->act,PLR_WANTED)) && --(ch->criminal)<=0)
      {
       REM_BIT(ch->act, PLR_WANTED);
       stc("{YТы больше не в розыске.{x\n\r",ch);
       ch->criminal=0;
      }

      light_update(ch);

      gain_condition( ch, COND_DRUNK,  -1 );
      gain_condition( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
      gain_condition( ch, COND_THIRST, -1 );
      gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
      if (ch->fighting==NULL) gain_condition( ch, COND_ADRENOLIN,  -1 );
    }
    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
      paf_next        = paf->next;
      if ( paf->duration > 0 )
      {
        paf->duration--;
        if (number_range(0,4) == 0 && paf->level > 0)
          paf->level--;  // spell strength fades with time
      }
      else if ( paf->duration < 0 ) ;
      else
      {
        if ( paf_next == NULL
          || paf_next->type != paf->type
          || paf_next->duration > 0 )
        {
          if (paf->bitvector==AFF_NOSTALGIA)
          {
            switch (number_range(1,3))
            {
              case 1 : do_function(ch,do_say,"Как тут тоскливо и противно!");
                       break;
              case 2 : do_function(ch,do_say,"Какого ... меня сюда притянули???");
                       break;
              default: do_function(ch,do_say,"Кто куда, а я - домой...");
                       break;
            }
            do_function(ch,do_emote,"бормочет 'gate'.");
            do_function(ch,do_emote,"ступает в портал и исчезает.");
            extract_char(ch,TRUE);
            return;
          }

          if (paf->bitvector==AFF_CHARM) stop_follower(ch);
          if ( paf->type > 0 && skill_table[paf->type].msg_off )
          {
            ptc(ch,"%s\n\r",skill_table[paf->type].msg_off);
          }
        }
        affect_remove( ch, paf );
      }
    }

    if (IS_AFFECTED(ch,AFF_NOSTALGIA)) nostalgia_update(ch);

    // Careful with the damages here,
    //  MUST NOT refer to ch after damage taken,
    //  as it may be lethal damage (on NPC).

    if (is_affected(ch, gsn_plague)) plague_update(ch);

    if (IS_AFFECTED(ch, AFF_POISON)) poison_update(ch);
    else if ( ch->position == POS_INCAP && number_range(0,1) == 0)
    {
      damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,FALSE, FALSE, NULL);
    }
    else if ( ch->position == POS_MORTAL )
    {
      damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,FALSE, FALSE, NULL);
    }

//    if (ch!=NULL)
//      if (IS_NPC(ch))
    if (IS_STATUE(ch) && time_info.hour==12)
          statue_moving(ch);

  }

  for ( ch = char_list; ch != NULL; ch = ch->next)
  {
    if (ch->desc && ch->desc->descriptor % 30 == save_number)
      save_char_obj(ch);
  }
}

// Update all objs.
// This function is performance sensitive.
void obj_update( void )
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  AFFECT_DATA *paf, *paf_next;

  for ( obj = object_list; obj != NULL; obj = obj_next )
  {
    CHAR_DATA *rch=NULL;
    char *message;

    obj_next = obj->next;

    if( obj->carried_by) // Durability & condition updating
    {
      if((rch=obj->carried_by) == NULL)
      {
        bug("NULL char is detected in recieved data!", 0);
        return;
      }
    }

    if( obj )
    {
      if( obj->durability != -1 )
        if( obj->durability == 0 || obj->durability == 200
         || obj->durability < -1 || obj->durability > 1000
         || obj->durability != material_table[material_num(obj->material)].d_dam )
              obj->durability = material_table[material_num(obj->material)].d_dam;
    }
    else
    {
      bug("NULL obj is sent as an argument to obj_update()!", 0);
      return;
    }

    if( obj->condition > obj->durability || obj->condition < 0 )
        obj->condition = obj->durability;

    for ( paf = obj->affected; paf != NULL; paf = paf_next )
    {
      paf_next    = paf->next;
      if ( paf->duration > 0 )
      {
        paf->duration--;
        if (number_range(0,4) == 0 && paf->level > 0) paf->level--;
      }
      else if ( paf->duration < 0 ) ;
      else
      {
        if ( paf_next == NULL
          ||   paf_next->type != paf->type
          ||   paf_next->duration > 0 )
        {
          if ( paf->type > 0 && skill_table[paf->type].msg_obj )
          {
            if (obj->carried_by != NULL)
            {
//            rch = obj->carried_by;
              act(skill_table[paf->type].msg_obj,rch,obj,NULL,TO_CHAR);
            }
            if (obj->in_room != NULL && obj->in_room->people != NULL)
            {
              rch = obj->in_room->people;
              act(skill_table[paf->type].msg_obj,
              rch,obj,NULL,TO_ALL_IN_ROOM);
            }
          }
        }
        affect_remove_obj( obj, paf );
      }
    }

    if ( obj->timer <= 0 || --obj->timer > 0 )continue;

    switch ( obj->item_type )
    {
      default:              message = "$i1 рассыпается в прах.";  break;
      case ITEM_FOUNTAIN:   message = "$i1 высыхает.";         break;
      case ITEM_CORPSE_NPC: message = "$i1 разлагается в грязь."; break;
      case ITEM_CORPSE_PC:  message = "$i1 разлагается в грязь."; break;
      case ITEM_FOOD:       message = "$i1 разлагается.";        break;
      case ITEM_POTION:     message = "$i1 испаряется от долгого неупотребления.";break;
      case ITEM_PORTAL:     message = "$i1 пропадает из виду."; break;
      case ITEM_CONTAINER:
        if (CAN_WEAR(obj,ITEM_WEAR_FLOAT))
         if (obj->contains) message = "$i1 вспыхивает и пропадает, вываливая содержимое на землю.";
         else message = "$i1 вспыхивает и пропадает.";
        else message = "$i1 рассыпается в пыль.";
        break;
    }

    if ( obj->carried_by != NULL )
    {
      if (IS_NPC(obj->carried_by)
        &&  obj->carried_by->pIndexData->pShop != NULL)
        obj->carried_by->silver += obj->cost/5;
      else
      {
        act( message, obj->carried_by, obj, NULL, TO_CHAR );
        if ( obj->wear_loc == WEAR_FLOAT)
          act(message,obj->carried_by,obj,NULL,TO_ROOM);
      }
    }
    else if ( obj->in_room != NULL && ( rch = obj->in_room->people ) != NULL )
    {
      if (! (obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
                   && !CAN_WEAR(obj->in_obj,ITEM_TAKE)))
      {
        act( message, rch, obj, NULL, TO_ROOM );
        act( message, rch, obj, NULL, TO_CHAR );
      }
    }

    if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
      &&  obj->contains)
    {   // save the contents
      OBJ_DATA *t_obj, *next_obj;

      for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
      {
        next_obj = t_obj->next_content;
        obj_from_obj(t_obj);

        if (obj->in_obj) obj_to_obj(t_obj,obj->in_obj);

        else if (obj->carried_by) if (obj->wear_loc == WEAR_FLOAT)
          if (obj->carried_by->in_room == NULL) extract_obj(t_obj);
          else obj_to_room(t_obj,obj->carried_by->in_room);
           else obj_to_char(t_obj,obj->carried_by);
            else if (obj->in_room == NULL) extract_obj(t_obj);
             else obj_to_room(t_obj,obj->in_room);
      }
    }
    extract_obj( obj );
  }
}

// Aggress. for each mortal PC for each mob in room
// aggress on some random PC
// This function takes 25% to 35% of ALL Merc cpu time.
// Unfortunately, checking on each PC move is too tricky,
// because we don't want the mob to just attack the first PC
// who leads the party into the room.
// Furey
void aggr_update( void )
{
  char buf[100];
  CHAR_DATA *wch;
  CHAR_DATA *ch,  *ch_next;
  CHAR_DATA *vch, *vch_next;
  CHAR_DATA static *wch_next;
  CHAR_DATA *victim;
  AFFECT_DATA af;

  for ( wch = char_list; wch; wch = wch_next )
  {
    wch_next = wch->next;
    if ( IS_NPC(wch) || wch->level >= LEVEL_IMMORTAL
     || !wch->in_room || wch->in_room->area->empty) continue;

    for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
    {
      register int count;
      ch_next = ch->next_in_room;

      if ( !IS_NPC(ch)
        || ch->in_room==NULL
        || IS_SET(ch->in_room->room_flags,ROOM_SAFE)
        || IS_SET(ch->in_room->ra,RAFF_SAFE_PLC)
        || IS_SET(ch->in_room->area->area_flags,AREA_LAW)
        || IS_SET(ch->act, ACT_PET)
        || IS_AFFECTED(ch,AFF_CALM)
        || ch->fighting != NULL
        || IS_AFFECTED(ch, AFF_CHARM)
        || !IS_AWAKE(ch)
        || ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
        || !can_see( ch, wch,CHECK_LVL )
        || number_bits(1) == 0)
      continue;

      if (ch->pIndexData!=NULL && ch->pIndexData->pShop!=NULL)
      {
       if (ch->stealer==NULL || !is_exact_name(wch->name,ch->stealer))
       continue;

       do_printf(buf,"{Y%s{m грязный {RВОР{m! ДЕРЖИТЕ ВОРА!{x\n\r",wch->name);
       do_yell(ch, buf);
       multi_hit( ch, wch);
       continue;
      }

      if (!IS_SET(ch->act, ACT_AGGRESSIVE)) continue;
      if (IS_AFFECTED(ch,AFF_CHARM)) continue;

      // Ok we have a 'wch' player character and a 'ch' npc aggressor.
      // Now make the aggressor fight a RANDOM pc victim in the room,
      // giving each 'vch' an equal chance of selection
      count        = 0;
      victim        = NULL;

     // Now the agressor feels 'nostalgia' if it is not home
      if (!IS_AFFECTED(ch,AFF_NOSTALGIA)
        && ch->in_room->area!=ch->pIndexData->area
        && ch->position > POS_SLEEPING
        && ch->fighting==NULL
        && ch->pIndexData->vnum != 1 // zombie special
        && can_attack(ch,1))
      {
        af.where  = TO_AFFECTS;
        af.type   = skill_lookup("nostalgia");
        af.level  = ch->level;
        af.duration  = number_range(2,6);
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_NOSTALGIA;
        affect_to_char(ch,&af);
        act("$c1 чувствует себя неуютно и тоскует по родине.",ch,NULL,NULL,TO_ROOM);
      }
      for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
      {
        vch_next = vch->next_in_room;

        if ( !IS_NPC(vch)
         &&   vch->level < LEVEL_IMMORTAL
         &&   ch->level +5 >= vch->level + vch->criminal/50
         &&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
         &&   can_see( ch, vch,CHECK_LVL ) )
        {
            if ( number_range( 0, count ) == 0 ) victim = vch;
            count++;
        }
      }

      if ( victim == NULL ) continue;

      if (IS_SET(ch->off_flags,OFF_BACKSTAB)) do_function(ch, &do_backstab, victim->name);
      else multi_hit( ch, victim);
    }
  }
}

// Gain hp,mana,moves
void gain_stats(CHAR_DATA *ch )
{
  int hp, mana, moves, number,dam;
  AFFECT_DATA *af;

  hp = UMAX(3,get_curr_stat(ch,STAT_CON) + ch->level/2);
  if (ch->classthi) hp+=hp/15;
  if (ch->classwar) hp+=hp/20;
  number = number_percent();

  if (number < get_skill(ch,gsn_fast_healing)+3*category_bonus(ch,FORTITUDE))
  {
    hp+= number * hp/100;
    if (ch->hit < ch->max_hit) check_improve(ch,gsn_fast_healing,TRUE,8);
  }
  if (IS_SET(race_table[ch->race].spec,SPEC_REGENERATION))
  {
    hp*=4;
    if (ch->position==POS_SLEEPING) hp+=hp/2;
    if (ch->position==POS_FIGHTING) hp/=2;
    hp=UMIN(hp,ch->max_hit/3*2);
  }
  else switch ( ch->position )
  {
    default: hp/= 4;break;
    case POS_SLEEPING: break;
    case POS_RESTING : hp/= 2;break;
    case POS_FIGHTING: hp/= 6;break;
  }

  if ( IS_NPC(ch) )
  {
    mana= 5 + ch->level;
    switch (ch->position)
    {
      default:mana/= 2;break;
      case POS_SLEEPING:mana= 3 * mana/2;break;
      case POS_RESTING:break;
      case POS_FIGHTING:mana/= 3;break;
    }
  }
  else
  {
    mana= (get_curr_stat(ch,STAT_WIS) + get_curr_stat(ch,STAT_INT) + ch->level) / 2;
    number = number_percent();
    if (number < get_skill(ch,gsn_meditation) + category_bonus(ch,SPIRIT)*3)
    {
      mana+= number * mana/ 100;
      if (ch->mana < ch->max_mana) check_improve(ch,gsn_meditation,TRUE,8);
    }
    if (!(ch->classcle || ch->classmag)) mana /= 2;
    if (IS_SET(race_table[ch->race].spec,SPEC_MANAREGEN)) mana*=2;
    else switch ( ch->position )
    {
      default:mana/= 4;break;
      case POS_SLEEPING:break;
      case POS_RESTING:mana /= 2;break;
      case POS_FIGHTING:mana /= 6;break;
    }
  }

  moves = UMAX( 15, ch->level );
  switch ( ch->position )
  {
   case POS_SLEEPING: moves += get_curr_stat(ch,STAT_DEX);break;
   case POS_RESTING:  moves += get_curr_stat(ch,STAT_DEX) / 2;break;
  }

  hp = hp * (ch->in_room->heal_rate+raffect_level(ch->in_room,RAFF_OASIS)) / 100;
  if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    hp= hp* (int)ch->on->value[3] / 100;

  mana = mana * (ch->in_room->mana_rate +raffect_level(ch->in_room,RAFF_MIND_CH))/ 100;
  if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    mana= mana* (int)ch->on->value[4] / 100;

  moves = moves * ch->in_room->heal_rate/100;
  if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    moves = moves * (int)ch->on->value[3] / 100;

  if (!IS_NPC(ch))
  {
    if ( ch->pcdata->condition[COND_HUNGER] <= 0 )
    {
      moves /= 2;
      hp/=2;
      mana/=2;
    }
    if ( ch->pcdata->condition[COND_THIRST] <= 0 )
    {
      moves /= 2;
      hp/=2;
      mana/=2;
    }
  }

  if (IS_AFFECTED(ch,AFF_POISON) && !IS_IMMORTAL(ch))
  {
    int tdam;

    hp/= 4;
    mana/=4;
    moves/=4;

    af = affect_find(ch->affected,gsn_poison);

    if (af) tdam=af->level+1;
    else tdam=ch->level+1;
    if (IS_AFFECTED(ch,AFF_HASTE)) tdam*=2;
    if (IS_AFFECTED(ch,AFF_SLOW)) tdam/=2;

// Questmobs won't die (c) Wagner
    if (IS_NPC(ch) && ch->questmob!=NULL)
        tdam=UMIN(abs(ch->hit-1),tdam);

    act( "$c1 дрожит и стонет.", ch, NULL, NULL, TO_ROOM );
    stc("{GЯд в твоих венах даёт о себе знать..\n\r{x",ch);
       damage(ch,ch,tdam,gsn_poison,DAM_POISON,FALSE, FALSE, NULL);

  }

  if (IS_AFFECTED(ch,AFF_PLAGUE) && !IS_IMMORTAL(ch))
  {
    hp/= 8;
    mana/=8;
    moves/=8;

    af = affect_find(ch->affected,gsn_plague);
    if (af) dam=UMAX(ch->level*2,af->level/5+1);
    else dam = UMAX(ch->level*2,ch->level/5+1);
    if (IS_AFFECTED(ch,AFF_HASTE)) dam*=2;
    if (IS_AFFECTED(ch,AFF_SLOW)) dam/=2;

    ch->mana -= dam;
    ch->move -= dam;
// Questmobs won't die (c) Wagner
    if (IS_NPC(ch) && ch->questmob!=NULL)
        dam=UMIN(abs(ch->hit-1),dam);

    act("$c1 вскрикивает от боли в страшных чумных язвах.",ch,NULL,NULL,TO_ROOM);
    stc("Ты бьёшья в агонии от чумы..\n\r",ch);
    damage( ch, ch, dam, gsn_plague,DAM_DISEASE,FALSE, FALSE, NULL);
  }

  if (IS_AFFECTED(ch,AFF_HASTE))
  {
    hp/=2;
    mana/=2;
    moves/=2;
  }
  if (IS_AFFECTED(ch,AFF_SLOW))
  {
    hp*=2;
    mana*=2;
    moves*=2;
  }
  if (is_affected(ch,skill_lookup("ensnare"))) moves/=2;
  if (IS_AFFECTED(ch,AFF_REGENERATION)) hp*= 2;

  hp/=4;
  mana/=4;
  moves/=4;
  if (IS_NPC(ch) && ch->questmob!=NULL)
      { }
    else
  {
     if (ch->hit<ch->max_hit)
        ch->hit +=UMIN(hp,   ch->max_hit  - ch->hit);
     if (ch->mana<ch->max_mana)
        ch->mana+=UMIN(mana, ch->max_mana - ch->mana);
     if (ch->move<ch->max_move)
        ch->move+=UMIN(moves,ch->max_move - ch->move);
     if (number_range(1,50)>45)
     {
        ch->hit=UMIN(ch->hit,ch->max_hit);
        ch->mana=UMIN(ch->mana,ch->max_mana);
        ch->move=UMIN(ch->move,ch->max_move);
     }
  }
}

void gain_update()
{
  CHAR_DATA *ch;

  for ( ch = char_list; ch != NULL; ch = ch->next )
  {
    if (!ch->in_room) continue;
    if (ch->in_room == get_room_index(ROOM_VNUM_LIMBO)) continue;
    if (is_affected(ch,skill_lookup("mummy"))) continue;
    gain_stats(ch);
  }
}

void remort_update(void)
{
  OBJ_DATA *        obj;
  OBJ_DATA *        obj_next;
  int               i,sn;
  const char *      chname;
  const char *      chdeity;
  int               chcarma;
  int               chfavour;
  CHAR_DATA *       ch=NULL;
  DESCRIPTOR_DATA * d;

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    if ( d->character && d->connected==CON_PLAYING && !IS_NPC(d->character) &&
         d->character->pcdata->confirm_remort==2) {
      ch=d->character;
      break;
    }
  }
  if (ch == NULL) return;
  stc( "Начинаем процесс перерождения!\n\r", ch);
  log_printf("Remort process is started for %s",(ch->name) ? ch->name : "BUG: unknown name");
  d->connected=CON_REMORT;

/*buggy*/
//  cancel_quest(ch,TRUE,20,30);
  if (ch->questmob!=NULL)
  {
    cancel_quest(ch,TRUE,20,30);
  }
  ch->nextquest=number_range(10,20);

  stop_fighting( ch, TRUE );
  save_one_char( ch, SAVE_BACKUP );
  save_char_obj( ch );

  // After extract_char the ch is no longer valid!
  chname=str_dup(ch->name);
  chdeity=str_dup(ch->deity);
  chcarma=ch->pcdata->carma;
  chfavour=ch->pcdata->favour;
  extract_char( ch, TRUE );
  load_char_obj( d, chname, SAVE_NORMAL );
  ch->desc=d;
  ch=d->character;
  ch->level=0;
  ch->gold=ch->silver=0;
  ch->sex=ch->pcdata->true_sex;
  ch->reply=NULL;
  ch->version=2;
  ch->pet=NULL;
  ch->group=0;
  ch->clanpet=NULL;
  ch->remort++;
  ch->exp=0;
  ch->practice=UMIN(ch->practice,20); // prac - maximum 20.
  ch->train=UMIN(ch->train,20);       // train - maximum 20.
  ch->deity=str_dup(chdeity);
  ch->pcdata->carma=chcarma;
  ch->pcdata->favour=chfavour;
  ch->pcdata->confirm_delete=FALSE;
  ch->pcdata->confirm_remort=0;
  ch->pcdata->hptrained=0;
  ch->pcdata->manatrained=0;

  // reassign race parameters
  for (i = 0; i < MAX_STATS; i++)
  {
    ch->perm_stat[i] = race_table[ch->race].stats[i];
    ch->mod_stat[i] = 0;
  }

  ch->affected_by = ch->affected_by|race_table[ch->race].aff;
  ch->imm_flags = ch->imm_flags|race_table[ch->race].imm;
  ch->res_flags = ch->res_flags|race_table[ch->race].res;
  ch->vuln_flags = ch->vuln_flags|race_table[ch->race].vuln;
  ch->form  = race_table[ch->race].form;

  // add race skills
  for (i = 0; i < 5; i++)
  {
    if (race_table[ch->race].skills[i] == NULL) break;
    group_add(ch,race_table[ch->race].skills[i],FALSE);
  }

  ch->pcdata->points = race_table[ch->race].points;

  //All learned skills set to 1%
  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name == NULL ) break;
    if ( ch->pcdata->learned[sn] > 0) ch->pcdata->learned[sn] = 1;
  }

  ch->max_mana=100;
  ch->max_move=100;
  ch->max_hit=20;
  ch->pcdata->perm_hit=20;
  ch->pcdata->perm_mana=100;
  ch->pcdata->perm_move=100;
  ch->hit=ch->max_hit;
  ch->mana=ch->max_mana;
  ch->move=ch->max_move;

  // removes all obj from char
  for ( obj = ch->carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    obj_from_char( obj );
    extract_obj(obj);
  }

  // drop extra classes
  REM_BIT(ch->act,PLR_5REMORT);
  REM_BIT(ch->act,PLR_LASTREMORT);
  if (ch->remort > 3)
  {
    stc ("Выбери какой класс выкинуть: \n\r",ch);
    if (ch->classmag) stc("mage ",ch);
    if (ch->classwar) stc("warrior ",ch);
    if (ch->classcle) stc("cleric ",ch);
    if (ch->classthi) stc("thief ",ch);
    stc("\n\r",ch);

    d->connected=CON_DROP_CLASS;
  }
  else
  {
  // end of Remort process, and go to select new class
    if (!ch->classmag) stc("mage ",ch);
    if (!ch->classwar) stc("warrior ",ch);
    if (!ch->classcle) stc("cleric ",ch);
    if (!ch->classthi) stc("thief ",ch);
    stc("\n\r",ch);

    d->connected=CON_GET_NEW_CLASS;
  }
}

void web_update()
{
  DESCRIPTOR_DATA * d ;
  FILE *fp;
  static char web_class_names[10][36] = {
    "<font color=blue>Hero  </font>\0",
    "<font color=blue>Avatar  </font>\0",
    "<font color=blue>Angel   </font>\0",
    "<font color=blue>ArhAngel</font>\0",
    "<font color=cyan>Immortal</font>\0",
    "<font color=green>DemiGod </font>\0",
    "<font color=green>GOD     </font>\0",
    "<font color=magenta>SUPERIOR</font>\0",
    "<font color=red>CREATOR </font>\0",
    "<font color=red>IMPLEMENTOR </font>\0"};

  if (fname_who)
  {
    if ( ( fp = fopen( fname_who, "w" ) ) == NULL )
    {
      perror( fname_who );
    }
    do_fprintf(fp,"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><title>Кто онлайн</title></head><body bgcolor=BLACK text=WHITE vlink=BLUE link=BLUE><hr><pre>");

    for (d = descriptor_list ; d != NULL ; d = d->next)
    {
      CHAR_DATA * wch   ;
      char      * class ;

      if (d->connected != CON_PLAYING) continue ; // skip disconnected

      // don't use trust as that exposes trusted mortals
      if (!d->character) continue ;
      wch = d->character ;

      if (wch->level < 102) class=classname(wch);
      else class = web_class_names[wch->level-101];

      do_fprintf (fp, "%3d %12s %s</font>] %s",
               wch->level, race_wname (wch), class,
               wch->clan ? wch->clan->show_name : "                ");

      do_fprintf (fp, "%s%s%s %s %s</font>\n\r",
               IS_SET(wch->act, PLR_WANTED) ? "<font color=red>(W)</font>" : "",
               IS_SET(wch->act, PLR_RAPER)  ? "<font color=red>(Ќ)</font>" : "",
               wch->godcurse                ? "<font color=grey>(C)</font>" : "",
               wch->name, IS_NPC(wch) ? "" : wch->pcdata->title) ;
    }
    do_fprintf(fp,"</pre><hr>");
    fclose (fp);
  }
}

// Handle all kinds of updates. Called once per pulse from game loop.
// Random times to defeat tick-timing clients and players.
void update_handler( void )
{
  static  int     pulse_area;
  static  int     pulse_mobile;
  static  int     pulse_violence;
  static  int     pulse_point;
  static  int     pulse_music;
  static  int     pulse_updchar;

  if ( --pulse_area     <= 0 )
  {
    pulse_area        = PULSE_AREA;
//    dlog("area_update");
    area_update        ( );
//    dlog("area_update finished");
  }

  if ( --pulse_updchar <= 0 )
  {
    pulse_updchar  = PULSE_UPDCHAR;
    gain_update();
//    dlog("gain_update finished");
  }

  if ( --pulse_music          <= 0 )
  {
    pulse_music        = PULSE_MUSIC;
    song_update();
//    dlog("song_update finished");
  }

  if ( --pulse_mobile   <= 0 )
  {
    pulse_mobile        = PULSE_MOBILE;
    mobile_update        ( );
//    dlog("mobile_update finished");
  }

  if ( --pulse_violence <= 0 )
  {
    pulse_violence        = PULSE_VIOLENCE;
    violence_update        ( );
//    dlog("violence_update finished");
  }

  if (--auction->pulse <=0)
  {
    auction->pulse = PULSE_AUCTION;
    auction_update();
//    dlog("auction_update finished");
  }

  if ( --pulse_point    <= 0 )
  {
    pulse_point     = PULSE_TICK; // Updating tick
    weather_update();             // Weather
    char_update   ();             // characters, affects
    obj_update    ();             // objects
    quest_update  ();             // quests
    gquest_update ();             // Global Quests
    unread_update ();             // auto unread
    raffect_update();             // room affects
    remort_update ();             // remort
    clan_update   ();             // clans update
    web_update    ();             // update web pages

    if (rebootcount>0)
    {
      DESCRIPTOR_DATA *d;

      rebootcount--;
      if (rebootcount==0) do_reboot(NULL,"now") ;
      if (rebootcount<6 || rebootcount==10 || rebootcount ==15 ||rebootcount==20)
      for ( d = descriptor_list; d; d = d->next )
      {
        if ( d->connected != CON_PLAYING ) continue;
        if (rebootcount>0) ptc (d->character,"{RSystem: REBOOT in %d ticks!{x\n\r",rebootcount);
        if (rebootcount<6) stc("{*",d->character);
      }
    }
  }
//  dlog("update.c - Updating Aggressive mobiles");
  aggr_update();
//  dlog("update.c - Updating complete");
  tail_chain();
}

void auction_update (void)
{
  char buf[MAX_STRING_LENGTH];

  if (!auction->item) return;

  switch (++auction->going)
  {
    case 1 :// going once
    case 2 :// going twice
      if (auction->bet > 0)
        if (auction->buyer)
          do_printf (buf, "%s: %s (текущая ставка %u).",get_obj_desc(auction->item,'1'),((auction->going == 1) ? "pаз" : "два"), auction->bet);
        else do_printf (buf, "%s: %s (начальная ставка %u).",get_obj_desc(auction->item,'1'),((auction->going == 1) ? "pаз" : "два"), auction->bet);
      else do_printf (buf, "%s: %s (ставок еще не было).",get_obj_desc(auction->item,'1'),((auction->going == 1) ? "pаз" : "два"));
      talk_auction (buf);
      break;
    case 3 :// SOLD!
      if (auction->bet > 0 && auction->buyer)
      {
        do_printf (buf, "Лот %s пpодан %s за %u золота.",get_obj_desc(auction->item,'1'),          IS_NPC(auction->buyer) ? get_char_desc(auction->buyer,'2') : auction->buyer->name,
          auction->bet);
        talk_auction(buf);
        obj_to_char (auction->item,auction->buyer);
        act ("Аукционеp появляется пеpед тобой и вpучает тебе $i4.",
          auction->buyer,auction->item,NULL,TO_CHAR);
        act ("Аукционеp появляется пеpед $c5, и вpучает $u $i4",
          auction->buyer,auction->item,NULL,TO_ROOM);

        auction->seller->gold += auction->bet; // give him the money
        auction->item = NULL; // reset item
        auction->seller = NULL;
        auction->buyer  = NULL;
      }
      else // not sold
      {
        do_printf (buf, "Ставок не получено - лот %s снЯт с аукциона.",get_obj_desc(auction->item,'1'));
        talk_auction(buf);
        act ("Аукционеp поЯвлЯется пеpед тобой и возвpащает тебе $i4.",
                 auction->seller,auction->item,NULL,TO_CHAR);
        act ("Аукционеp поЯвлЯется пеpед $c5 и возвpащает $u $i4.",
                 auction->seller,auction->item,NULL,TO_ROOM);
        obj_to_char (auction->item,auction->seller);
        auction->bet = 0;
        auction->item = NULL; /* clear auction */
      }
   default:
     if (auction->going>3) auction->going=3;
     else auction->going=2;
  }
}

void penalty_update(CHAR_DATA *ch)
{
  PENALTY_DATA *penalty,*penalty_next;

  if (!ch->penalty) return;

  for(penalty=ch->penalty;penalty!=NULL;penalty=penalty_next)
  {
    penalty_next=penalty->next;

    switch (penalty->type)
    {
      case PENALTY_TICK:
        penalty->ticks--;
        if (penalty->ticks<=0) remove_penalty(ch, penalty);
        break;
      case PENALTY_DATE:
        break;
      case PENALTY_PERMANENT:
      default:
        break;
    }
  }
}

void raffect_update(void)
{
  RAFFECT *ra, *ra_next;

  for (ra=raffect_list;ra;ra=ra_next)
  {
    ra_next=ra->next;
    if (ra->duration==-1) continue;
    ra->duration--;
    if (ra->duration<0) free_raffect(ra);
    if (ra->level>1 && number_percent()>60) ra->level--;
  }
}

void clan_update()
{
  CLAN_DATA *clan;
  int i;

  for (clan=clan_list;clan;clan=clan->next)
  {
    for (i=0;i<20;i++)
    {
      if (clan->clansn[i]==0) break;
      if (clan->clansnt[i]>0)
      {
        clan->clansnt[i]--;
        if (clan->clansnt[i]==0)
        {
          DESCRIPTOR_DATA *d;
          for (d=descriptor_list;d;d=d->next)
          {
            if (d->character && d->connected==CON_PLAYING && d->character->clan==clan)
              ptc(d->character,"Ваш клан теряет способность к %s.\n\r",skill_table[clan->clansn[i]].name);
          }
          rem_clanskill(clan,clan->clansn[i]);
        }
      }
    }
  }
}

void statue_moving(CHAR_DATA *ch)
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
     && !IS_CLAN_ROOM(room)) break;
  }
  do_function(ch,do_emote,"растворяется в {Dнеизвестности{x.");
  char_from_room(ch);
  char_to_room(ch, room);
  do_function(ch,do_emote,"появляется из {Dнеизвестности{x.");
}
