// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

struct guild_type guild_table[] =
{
 { 0,             "None",          0,                     0 },
 { DWARVES_GUILD, "Dwarves Guild", OFF_DWARVES_ACCOUNTER, 0 },
 { ASSASIN_GUILD, "Assasin Guild", OFF_ASSASIN_ACCOUNTER, 0 },
 { VAMPIRE_GUILD, "Vampire Guild", 0,                     0 },
 { DRUIDS_GUILD,  "Druides Guild", 0,                     0 },
 { 255,           NULL,            0,                     0 }
};

DECLARE_DO_FUN2( do_say  );

void save_guild(FILE *fp)
{
  int count;

  for (count=1;guild_table[count].guild!=255;count++)
  {
    do_fprintf( fp, "GUILD\n");
    do_fprintf( fp, "%s~\n", guild_table[count].name);
    do_fprintf( fp, "%u\n", guild_table[count].gold);
  }
}

void load_one_guild(FILE *fp)
{
  int guild;
  const char *name;

  name=fread_string(fp);
  guild=guild_lookup(name);
  if (guild!=0) guild_table[guild].gold=fread_number64(fp);
  else log_printf( "Load guild data: guild [%s] not found.",name);
  fread_to_eol(fp);
}

int guild_lookup(const char *name)
{
  int guild;

  for (guild=0;guild_table[guild].guild!=255;guild++)
  {
    if (!str_prefix(name,guild_table[guild].name)) return guild;
  }
  return 0;
}

void do_gaccount( CHAR_DATA *ch, const char *argument )
{
  CHAR_DATA *keeper;
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  int64 amount;
  int guild=0,tmp;

  if (IS_SET(ch->act,PLR_TIPSY)) if (tipsy(ch,"account")) return;

  if (ch->pcdata->guild==0)
  {
    stc("Ась?\n\r", ch);
    return;
  }

  if (EMPTY(argument))
  {
    ptc(ch,"{CНа счету в гильдии {G'%s'{x: {Y%u {Cзолота{x\n\r", guild_table[ch->pcdata->guild].name,guild_table[ch->pcdata->guild].gold);
    return;
  }

  for ( keeper=ch->in_room->people; keeper; keeper=keeper->next_in_room )
  {
    if (!IS_NPC(keeper)) continue;
    for (tmp=1;guild_table[tmp].guild!=255;tmp++)
    {
      if (guild_table[tmp].accounter!=0
       && GUILD(ch,guild_table[tmp].guild)
       && IS_SET(keeper->off_flags,guild_table[tmp].accounter))
      {
        guild=tmp;
        break;
      }
    }
    if (guild!=0) break;
  }

  if (!keeper)
  {
    stc("{RСначала зайди в банк.\n\r{x",ch);
    return;
  }

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (is_number(arg2)) amount=atoi(arg2);
  else
  {
    stc("синтаксис: GACCOUNT <get|put> <число> [gold]\n\r",ch);
    stc("           GACCOUNT <put> <число> diamonds\n\r",ch);
    stc("           GACCOUNT <put> <чиcло> crystals\n\r",ch);
    return;
  }

  if (!str_prefix(arg1,"put"))
  {
    int64 count=0;

    if (EMPTY(argument) || !str_prefix(argument, "gold"))
    {
      register int tmp=0;
      if (amount<10 || amount>ch->gold)
      {
        stc("{RТы не можешь положить меньше 10 или больше чем у тебя есть.\n\r{x",ch);
        return;
      }
      ch->gold-=amount;
      if (amount%10!=0) tmp=1;
      ptc(ch,"\n\r{CТы кладешь на счет {Y%u {Cзолота. Высчитано {Y%u {C(10%).{x\n\r",amount,amount/10+tmp);
      amount-=amount/10+tmp;
      guild_table[guild].gold+=amount;
      ptc(ch,"На счету Гильдии теперь {Y%u{C.{x\n\r", guild_table[guild].gold);
    }
    else if (!str_prefix(argument, "diamonds"))
    {
      count=sell_gem(ch,amount,OBJ_VNUM_DIAMOND);
      ptc(ch,"Ты кладешь бриллианты в количестве %u штук на счет Гильдии. Это %u золота.\n\r", count, (int64)count*90);
      guild_table[guild].gold+=count*90;
      ptc(ch,"На счету Гильдии теперь {Y%u{C.{x\n\r",guild_table[guild].gold);
    }
    else if (!str_prefix(argument, "crystals"))
    {
      count=sell_gem(ch,amount,OBJ_VNUM_CRYSTAL);
      ptc(ch,"Ты кладешь кристаллы в количестве %u штук на счет Гильдии. Это %u золота.\n\r", count, (int64)(count*9000));
      guild_table[guild].gold+=count*9000;
      ptc(ch,"На счету Гильдии теперь {Y%u{C.{x\n\r",guild_table[guild].gold);
    }
    else
    {
      do_gaccount(ch, "");
      return;
    }
    save_config ();
  }

  if (!str_prefix(arg1,"get"))
  {
    if (amount<10 || amount>guild_table[guild].gold)
    {
      stc("{RТы не можешь снять меньше 10 или больше, чем есть на счету.{x",ch);
      return;
    }
    guild_table[guild].gold-=amount;
    ch->gold+=amount;
    ptc(ch,"\n\r{CТы снимаешь со счета {Y%u {Cзолота. Осталось {Y%u.{x\n\r",amount,guild_table[guild].gold);
    save_config ();
  }
}

void do_offer(CHAR_DATA *ch, const char *argument)
{
  OFFER_DATA *offer;
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *master;

  argument=one_argument(argument,arg);

  if (!str_prefix(arg,"help"))
  {
    stc("Syntax:offer request  <char_name>    - сделать заказ\n\r",ch);
    stc("       offer cancel  [<char_name>]   - отменить заказ\n\r",ch);
    stc("       offer list                    - просмотреть список заказов\n\r",ch);
    if (GUILD(ch,ASSASIN_GUILD))
      stc("       offer complete <char_name>    - выполнить заказ (сдать ухо)\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"list"))
  {
    bool found=FALSE;
    stc("Список заказов:\n\r",ch);
    for(offer=offer_list;offer;offer=offer->next)
    {
      if (!strcmp(ch->name,offer->customer) || GUILD(ch,ASSASIN_GUILD))
      {
        ptc(ch,"За голову {Y%s{x %s {Y%u {xзолота{x.\n\r",
        offer->name,(!strcmp(ch->name,offer->customer)) ? "ты даешь":"некто дает",offer->gold);
        found=TRUE;
      }
    }
    if (!found) stc("Никого нет в списке.\n\r",ch);
    return;
  }

  for (master=ch->in_room->people;master;master=master->next_in_room)
    if(IS_NPC(master) && IS_SET(master->act,ACT_ASSASIN_MASTER)) break;

  if (!master)
  {
    stc("Тут нет Мастера Ассасинов.\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"request"))
  {
    int64 reward;
    CHAR_DATA *vch;
    char buf[MAX_STRING_LENGTH];

    argument=one_argument(argument,arg);

    vch=get_pchar_world(ch,arg);

    if (!vch)
    {
      stc("Персонаж не найден.\n\r",ch);
      return;
    }

    if ( vch->level < 21 && !vch->remort )
    {
      stc("Ты не можешь заказать убийство ребенка!\n\r",ch);
      return;
    }

    if ( ch->level < 21 && !ch->remort )
    {
      stc("Ты еще слишком мал для таких вещей!\n\r",ch);
      return;
    }

    if ( GUILD(ch,ASSASIN_GUILD) )
    {
      stc("Ты не можешь нарушить устав гильдии!\n\r",ch);
      return;
    }

    if ( ch == vch )
    {
      stc("Приключений захотелось? Напади на Хассана!\n\r",ch);
      return;
    }

    reward=vch->level*(vch->remort+1)*100;

    if (ch->gold<reward || reward<1)
    {
      stc("У вас нет столько золота.",ch);
      return;
    }


    for(offer=offer_list;offer;offer=offer->next)
    {
      if (!strcmp(ch->name,offer->customer) && !str_cmp(offer->name, vch->name))
      {
        /*
        if (offer->gold>0)
        {
          offer->gold+=reward;
          ptc(ch,"Вы добавляете {Y%u{x золота - теперь за голову {Y%s{x назначено {Y%u{x.\n\r",reward,offer->name,offer->gold);
          do_printf(buf,"Увеличено вознаграждение за голову {Y%s{R, теперь оно составляет {Y%u{R золота.",offer->name,offer->gold);
          assasin_echo(buf);
          ch->gold-=reward;
          save_config();
        }
        else stc("Вы можете только добавить золото в награду.\n\r",ch);
        */
        ptc(ch,"Вы уже заказали убийство этого персонажа.\n\r");
        return;
      }
    }


    offer=new_offer();
    offer->customer=str_dup(ch->name);
    offer->name=str_dup(vch->name);
    offer->gold=reward;
    ptc(ch,"{Y%s{x включен в список.\n\r",offer->name);
    do_printf(buf,"Получен новый заказ на {Y%s{R, цена за голову - {Y%u{R золота.",offer->name,reward);
    assasin_echo(buf);
    ch->gold-=reward;
    save_config();
    return;
  }

  if (!str_prefix(arg,"cancel"))
  {
    char buf[MAX_STRING_LENGTH];

    argument=one_argument(argument,arg);

    for (offer=offer_list;offer;offer=offer->next)
    {
      if (!strcmp(ch->name,offer->customer) && !str_prefix(arg,offer->name))
      {
        ptc(ch,"Ты отменяешь заказ на {Y%s{x.\n\r",offer->name);
        do_printf(buf,"Заказ на {Y%s{R отменен. {Y%u{R золота получено(10%).",offer->name,offer->gold/10);
        guild_table[guild_lookup("assasin_guild")].gold+=offer->gold/10;
        ch->gold+=offer->gold*9/10;
        assasin_echo(buf);
        free_offer(offer);
        save_config();
        return;
      }
    }
    stc("В списке нет этого имени.\n\r",ch);
    return;
  }

  if (!str_prefix(arg,"complete") && GUILD(ch,ASSASIN_GUILD))
  {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OFFER_DATA *tmp;
    bool found=FALSE;

    argument=one_argument(argument,arg);

    if (EMPTY(arg))
    {
      stc("Укажите имя.\n\r",ch);
      return;
    }
    for(offer=offer_list;offer;offer=offer->next)
      if(!str_prefix(arg,offer->name)) break;

    if (!offer)
    {
      stc("Имя в списке не найдено.\n\r",ch);
      return;
    }

    for(obj=ch->carrying;obj;obj=obj->next_content)
    {
      if (obj->pIndexData->vnum==ITEM_EAR && is_exact_name(offer->name,obj->short_descr))
      {
        while(offer)
        {
          if (is_exact_name(offer->name,obj->short_descr))
          {
            guild_table[guild_lookup("assasin guild")].gold+=offer->gold;
            do_printf(buf,"За голову {Y%s{R получено {Y%u{x золота. Браво, {Y%s{R!",offer->name,offer->gold,ch->name);
            assasin_echo(buf);
            do_printf(buf,"{RКонтракт [N%d] выполнен успешно.\n\rУхо %s было предъявлено {DМастеру Гильдии {Rдля доказательства.\n\r",number_range(1,20000),offer->name);
            send_note("Assasin Guild",offer->customer,"{DContract complete{x",buf,3);
            tmp=offer;
            offer=offer->next;
            free_offer(tmp);
            found=TRUE;
            save_config();
          }
          else offer=offer->next;
        }
      }
    }
    if (!found) stc("Ты должен доставить ухо жертвы.\n\r",ch);
    return;
  }
  do_offer(ch,"");
}

void assasin_echo(const char *argument )
{
 DESCRIPTOR_DATA *d;

 if ( argument[0] == '\0' ) return;
 for ( d = descriptor_list; d; d = d->next )
   if ( d->connected == CON_PLAYING && GUILD(d->character,ASSASIN_GUILD))
     ptc(d->character,"{D[Assasin]:{R%s{x\n\r",argument);
}

bool is_offered( CHAR_DATA *ch )
{
  OFFER_DATA *off;

  for ( off=offer_list; off; off=off->next )
    if (!strcmp(ch->name,off->name)) return TRUE;
  return FALSE;
}

void do_guild( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CLAN_DATA *clan;
  int gn_old;
  int gn_new;

  if (!IS_IMMORTAL(ch) && ch->pcdata->elder==0)
  {
    stc("Ась?\n\r",ch);
    return;
  }
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (!str_prefix(arg1,"help"))
  {
     stc ("guild <char> <guild name>\n\r",ch);
    if (!IS_IMMORTAL(ch)) return;
     stc ("guild <char> elder\n\r",ch);
     stc ("guild <char> none\n\r",ch);
     stc ("guild list\n\r",ch);
     stc ("guild show\n\r",ch);
    return;
  }

  if (IS_IMMORTAL(ch) && !str_prefix(arg1,"list"))
  {
    int c;

    stc("{CСписок кланов:{x\n\r",ch);
     for (clan=clan_list;clan!=NULL;clan=clan->next)
      ptc(ch, "%s %s\n\r", clan->show_name, clan->name);
    stc("\n\r{CСписок Гильдий:{x\n\r",ch);
    for (c=0;guild_table[c].guild!=255;c++)
      ptc(ch, "%d. '%s'\n\r", c+1, guild_table[c].name);
    return;
  }

  if (IS_IMMORTAL(ch) && !str_prefix(arg1,"show"))
  {
    int c;

    for (c=1;guild_table[c].guild!=255;c++)
      ptc(ch, "На счету у {G%s{x: {Y%u{x.\n\r", guild_table[c].name,guild_table[c].gold);
    return;
  }

  if (EMPTY(arg2))
  {
    do_guild(ch,"help");
    return;
  }

  if ( ( victim = get_pchar_world( ch, arg1 ) ) == NULL )
  {
    stc( "Тут таких нет.\n\r", ch );
    return;
  }

  if (IS_IMMORTAL(ch) && !str_prefix(arg2,"none"))
  {
    stc("Этот персонаж теперь вне клана.\n\r",ch);
    stc("Ты теперь вне клана!\n\r",victim);
   // if ( (victim->clan->name != "loner") && (victim->clan != NULL) )
    if (victim->clan && str_cmp(victim->clan->name,"loner"))
    {
      gn_old=group_lookup(victim->clan->name);
      if ( gn_old > -1) gn_remove(victim,gn_old);
     }
    victim->clan = NULL;
    return;
  }

  if ((IS_IMMORTAL(ch) || ELDER(ch,DWARVES_GUILD)) && !str_prefix(arg2, "dwarves"))
  {
    argument=one_argument(argument, arg2);
    if (victim->race!=RACE_DWARF)
    {
      stc("Только Гномы могут состоять в гильдии!\n\r",ch);
      return;
    }

    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,DWARVES_GUILD);
      if (ELDER(victim, DWARVES_GUILD))
      {
        ptc(ch, "%s теперь старейшина в Гильдии Гномов.\n\r",victim->name);
        stc ("Ты теперь старейшина в гильдии Гномов.\n\r", victim);
        SET_BIT(victim->pcdata->guild,DWARVES_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из старейшин Гильдии Гномов.\n\r",victim->name);
        stc("Тебя исключили из старейшин гильдию Гномов.\n\r", victim);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,DWARVES_GUILD);
    if (GUILD(victim, DWARVES_GUILD))
    {
      ptc(ch,"%s теперь состоит в Гильдии Гномов.\n\r",victim->name);
//      if (!IS_IMMORTAL(ch))
      gecho("{YЯркие вспышки прочертили небо, и земля содрогнулась от тяжкого стона.{x");
//      if (!IS_IMMORTAL(ch))
      gecho("{DГильдия гномов{x приняла еще одного {CКУЗНЕЦА{x.");
      stc ("Тебя приняли в гильдию Гномов.\n\r", victim);
    }
    else
    {
      ptc(ch,"%s исключен из Гильдии Гномов.\n\r",victim->name);
      stc("Тебя исключили из гильдию Гномов.\n\r", victim);
      REM_BIT(victim->pcdata->elder,DWARVES_GUILD);
    }
    return;
  }

  if ((IS_IMMORTAL(ch) || ELDER(ch,DRUIDS_GUILD)) && !str_prefix(arg2, "druids"))
  {
    argument=one_argument(argument, arg2);
    if (victim->race!=RACE_DRUID)
    {
      stc("Только Друиды могут состоять в гильдии!\n\r",ch);
      return;
    }

    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,DRUIDS_GUILD);
      if (ELDER(victim, DRUIDS_GUILD))
      {
        ptc(ch, "%s теперь старейшина в Гильдии Друидов.\n\r",victim->name);
        stc ("Ты теперь старейшина в гильдии Друидов.\n\r", victim);
        SET_BIT(victim->pcdata->guild,DRUIDS_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из старейшин Гильдии Друидов.\n\r",victim->name);
        stc("Тебя исключили из старейшин гильдию Друидов.\n\r", victim);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,DRUIDS_GUILD);
    if (GUILD(victim, DRUIDS_GUILD))
    {
      ptc(ch,"%s теперь послушник в Гильдии Друидов.\n\r",victim->name);
//      if (!IS_IMMORTAL(ch))
      gecho("{YЯркие вспышки прочертили небо, травы и деревья радостно прошелестели в приветствии.{x");
//      if (!IS_IMMORTAL(ch))
      gecho("{GГильдия Друидов{x приняла еще одного {GПОСЛУШНИКА{x.");
      stc ("Тебя приняли в гильдию Друидов.\n\r", victim);
    }
    else
    {
      ptc(ch,"%s исключен из Гильдии Друидов.\n\r",victim->name);
      stc("Тебя исключили из гильдию Друидов.\n\r", victim);
      REM_BIT(victim->pcdata->elder,DRUIDS_GUILD);
    }
    return;
  }

  if ( (IS_IMMORTAL(ch) || ELDER(ch,ASSASIN_GUILD)) && !str_prefix(arg2, "assasin"))
  {
    argument=one_argument(argument, arg2);
    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,ASSASIN_GUILD);
      if (ELDER(victim, ASSASIN_GUILD))
      {
        ptc(ch, "%s теперь Мастер в Гильдии Ассасинов.\n\r",victim->name);
        stc ("Ты теперь Мастер в гильдии Ассасинов.\n\r", victim);
        SET_BIT(victim->pcdata->guild,ASSASIN_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из Мастеров Гильдии Ассасинов.\n\r",victim->name);
        stc("Тебя исключили из Мастеров гильдию Ассасинов.\n\r", victim);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,ASSASIN_GUILD);
    if (GUILD(victim,ASSASIN_GUILD))
         ptc(ch, "%s теперь асассин.\n\r",victim->name);
    else ptc(ch, "%s исключен из асассинов.\n\r",victim->name);
    if (!GUILD(victim,ASSASIN_GUILD)) REM_BIT(victim->pcdata->elder,ASSASIN_GUILD);
    return;
  }
  if ( (IS_IMMORTAL(ch) || ELDER(ch,VAMPIRE_GUILD)) && !str_prefix(arg2, "vampire"))
  {
    argument=one_argument(argument, arg2);
    if (victim->race!=RACE_VAMPIRE)
    {
      stc("Только Вампиры могут состоять в гильдии!\n\r",ch);
      return;
    }
    if (IS_IMMORTAL(ch) && !str_cmp(arg2,"elder"))
    {
      victim->pcdata->elder=toggle_int(victim->pcdata->elder,VAMPIRE_GUILD);
      if (ELDER(victim, VAMPIRE_GUILD))
      {
        ptc(ch, "%s теперь Высший Вампир.\n\r",victim->name);
        stc ("Ты теперь Высший Вампир.\n\r", victim);
        SET_BIT(victim->pcdata->guild,VAMPIRE_GUILD);
      }
      else
      {
        ptc(ch,"%s исключен из Высших Вампиров.\n\r",victim->name);
        stc("Тебя исключили из Высших Вампиров.\n\r", victim);
      }
      return;
    }
    victim->pcdata->guild=toggle_int(victim->pcdata->guild,VAMPIRE_GUILD);
    if (GUILD(victim,VAMPIRE_GUILD))
    {
      OBJ_DATA *obj,*obj_next=NULL;
      // checks for silvery equipment
      for (obj=victim->carrying;obj;obj=obj_next)
      {
        obj_next=obj->next_content;
        if (obj->wear_loc!=WEAR_NONE && !strcmp(material_lookup(obj->material),"silver"))
        {
          obj_from_char(obj);
          obj_to_room(obj,victim->in_room);
          act("$i1 спадает с {Y$c1{x.",victim,obj,NULL, TO_ROOM);
          act("$i1 обжигает тебя и падает на пол.",victim,obj,NULL, TO_CHAR);
        }
      }
      ptc(ch, "%s теперь член гильдии Вампиров.\n\r",victim->name);
    }
    else ptc(ch, "%s исключен из гильдии Вампиров.\n\r",victim->name);
    if (!GUILD(victim,VAMPIRE_GUILD)) REM_BIT(victim->pcdata->elder,VAMPIRE_GUILD);
    return;
  }

  if (!IS_IMMORTAL(ch)) return;

  if ((clan = clan_lookup(arg2)) == NULL)
  {
    stc("Такого клана нет.\n\r",ch);
    return;
  }

  if (IS_SET(clan->flag, CLAN_LONER))
  {
    ptc(ch,"Этот персонаж теперь %s.\n\r",clan->name);
    ptc(victim,"Ты теперь %s.\n\r",clan->name);
  }
  else
  {
    ptc(ch,"Этот персонаж теперь член клана %s.\n\r",capitalize(clan->name));
    ptc(victim,"Ты теперь член клана %s.\n\r",capitalize(clan->name));
  }

  if ( (victim->clan == NULL) && !strcmp(clan->name,"loner") )
  {
    gn_new=group_lookup(clan->name);
    if ( gn_new > -1) gn_add(victim,gn_new);
  }
  else if ( strcmp(victim->clan->name,"loner") && !strcmp(clan->name,"loner") )
  {
    gn_new=group_lookup(clan->name);
    if ( gn_new > -1) gn_add(victim,gn_new);
  }
  else if ( !strcmp(victim->clan->name,"loner") && strcmp(clan->name,"loner") )
  {
    gn_old=group_lookup(victim->clan->name);
    if (gn_old > -1) gn_remove(victim,gn_old);
    gn_new=group_lookup(clan->name);
    if (gn_new > -1) gn_add(victim,gn_new);
  }
  else if ( !strcmp(victim->clan->name,"loner") && strcmp(clan->name,"loner") )
  {
    gn_old=group_lookup(victim->clan->name);
    if (gn_old > -1) gn_remove(victim,gn_old);
  }

  victim->clan = clan;
  victim->clanrank = 0;
  victim->clanpet=NULL;
}

void do_resurrect( CHAR_DATA *ch, const char *argument )
{
  char buff[MAX_STRING_LENGTH];
  CHAR_DATA *mob;
  int i;

  if (!GUILD(ch,DRUIDS_GUILD))
  {
     stc("ась?\n",ch);
     return;
  }

  if (ch->in_room->sector_type!=SECT_FOREST)
  {
      stc("Тебе некого здесь призывать.\n\r",ch);
      return;
  }

  if( ch->pet != NULL )
  {
      stc( "Деревья лишь тихо зашелестели в ответ на твой призыв.\n\r", ch );
      return;
  }

  // add dendroid
  if((mob = create_mobile(get_mob_index(2)))==NULL) // VNUM 2 - dendroid (LIMBO)
  {
   stc("{RBUG! Unable to create mob! Report to Imms NOW!{x\n\r",ch);
   return;
  }

  mob->level = ch->level;

  if (ch->level<90) mob->level=ch->level+(category_bonus(ch,LIGHT)-category_bonus(ch,DARK))/((110-ch->level)/8);

  if (ELDER(ch,DRUIDS_GUILD))
  {
    do_printf(buff,"Дендроид, сплетение корней, преданно ждет приказов %s.",ch->name);
    free_string(mob->long_descr);
    mob->long_descr = str_dup(buff);
  }

  if (mob->level < 30) mob->max_hit =mob->level * 30 + number_range( mob->level * mob->level/4, mob->level * mob->level);
  else if (mob->level < 60) mob->max_hit =mob->level * 60 + number_range( mob->level * mob->level/2, mob->level * mob->level);
  else if (mob->level < 90) mob->max_hit =mob->level * 90 + number_range( mob->level * mob->level*2/3, mob->level * mob->level*3/2);
  else mob->max_hit =mob->level * 100 + number_range( mob->level * mob->level, mob->level * mob->level*2);
  mob->hitroll = mob->level;
  mob->damroll = (short) (mob->level*0.7);
  mob->max_hit = UMIN((int)(mob->max_hit*.9),29000);
  mob->hit                    = mob->max_hit;
  mob->max_mana               = 100 + dice(mob->level,10);
  mob->mana                   = mob->max_mana;
  for (i = 0;i < 3;i++)
  mob->armor[i]               = interpolate(mob->level,100,-100);
  mob->armor[3]               = interpolate(mob->level,100,0);
  mob->alignment=0;

  for (i = 0;i < MAX_STATS;i++) mob->perm_stat[i] = 11 + mob->level/4;

  char_to_room( mob, ch->in_room );
  act( "Корни разрывают землю вокруг, выпуская в мир Дендроида!", ch, NULL, NULL, TO_ALL_IN_ROOM );

  SET_BIT(mob->affected_by, AFF_CHARM);
  SET_BIT(mob->act, ACT_PET);
  SET_BIT(mob->act, ACT_EXTRACT_CORPSE);

  if (ch->clan!=NULL)
  {
    mob->clan=ch->clan;
    mob->clanrank=0;
  }
  add_follower( mob, ch );
  mob->leader = ch;
  ch->pet = mob;
  do_say( mob, "Чем я могу служить, хозяин?" );
  return;
}
