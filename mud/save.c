// $Id: save.c,v 1.86 2003/12/18 13:55:22 wagner Exp $
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
#include "fdweb.h"

#ifdef WITH_UNICORN
#include "unicorn.h"
#endif

extern  int     _filbuf         args( (FILE *) );

int64  flag_convert64 (char letter);
int64  fread_flag64   (FILE * fp);
char * pflag64        (int64 flag);
void save_clans    ();
//DDD int  rename (const char * oldfname, const char * newfname);
void save_guild (FILE *fp);

char * print_flags(int flag)
{
  int count, pos = 0;
  static char buf[52];

  for (count = 0; count < 32;  count++)
  {
    if (IS_SET(flag,1<<count))
    {
      if (count < 26)  buf[pos] = 'A' + count;
      else             buf[pos] = 'a' + (count - 26);
      pos++;
    }
  }
  if (pos == 0)
  {
    buf[pos] = '0';
    pos++;
  }
  buf[pos] = '\0';
  return buf;
}

// Array of containers read for proper re-nesting of objects.
static  OBJ_DATA *      rgObjNest       [MAX_NEST];

// Local functions.
void fwrite_char args( ( CHAR_DATA *ch, FILE *fp ) );
void fwrite_obj  args( ( CHAR_DATA *ch, OBJ_DATA *obj,FILE *fp,int iNest, bool type));
void fwrite_pet  args( ( CHAR_DATA *pet,FILE *fp ) );
void fread_char  args( ( CHAR_DATA *ch, FILE *fp ) );
void fread_pet   args( ( CHAR_DATA *ch, FILE *fp ) );
void fread_obj   args( ( CHAR_DATA *ch, FILE *fp, bool type ) );

void save_char_obj (CHAR_DATA * ch)
{
  CHAR_DATA *wch;
  int count = 1;

  save_one_char (ch, SAVE_NORMAL);
  for (wch=char_list;wch;wch=wch->next)
  {
    if (IS_NPC(wch)) continue;
    if (IS_SET (wch->comm, COMM_WILLSAVE))
    {
      save_one_char (wch,SAVE_NORMAL);
      count++;
    }
  }
//  if (count > 1) log_printf ("%d chars saved", count);
}

void fwrite_char (CHAR_DATA * ch, FILE * fp)
{
  AFFECT_DATA * paf;
  PENALTY_DATA * pena;
  int sn, gn, pos;

  if (IS_NPC(ch)) return;
  do_fprintf (fp, "#PLAYER\n");
  do_fprintf (fp, "Name %s~\n",ch->name);
  do_fprintf (fp, "Id   %l\n" ,ch->id);
  do_fprintf (fp, "LogO %l\n" ,current_time);
  do_fprintf (fp, "Vers %d\n" ,ch->version);
  do_fprintf (fp, "Carma %d\n" , ch->pcdata->carma);
  do_fprintf (fp, "Favour %d\n" , ch->pcdata->favour);
  do_fprintf (fp, "DeityCurse %d\n", ch->pcdata->dc);
  do_fprintf (fp, "Deitynumber %d\n", ch->pcdata->dn);
  do_fprintf (fp, "DeityRank %d\n", ch->pcdata->dr);
  do_fprintf (fp, "Ldefend %d\n", ch->pcdata->protect);

  if (ch->host != '\0') do_fprintf (fp, "Host %s~\n", ch->host);
  else                  do_fprintf (fp, "Host unknown~\n");

  do_fprintf (fp, "LastLogin %l\n", ch->lastlogin);

  if (!EMPTY(ch->pcdata->deny_addr))
    do_fprintf(fp,"PerBan %s~\n",ch->pcdata->deny_addr);
  if (!EMPTY(ch->pcdata->denied_by))
    do_fprintf(fp,"Denied_by %s~\n",ch->pcdata->denied_by);

// Quest info
  if (ch->questpoints != 0) do_fprintf (fp, "QuestPnts %d\n", ch->questpoints);
  if (ch->nextquest   != 0) do_fprintf (fp, "QuestNext %d\n", ch->nextquest) ;
  do_fprintf (fp, "Qstat %l %l %l\n",ch->qcounter, ch->qcomplete[0], ch->qcomplete[1]);
  do_fprintf (fp, "GQstat %d %d\n",ch->gqcounter, ch->gqcomplete);

// Kills statistic
  if (ch->vic_npc        != 0) do_fprintf (fp,"VicNPC %l\n",     ch->vic_npc);
  if (ch->death_npc      != 0) do_fprintf (fp,"DeathNPC %l\n",   ch->death_npc);
  if (ch->vic_pc_total   != 0) do_fprintf (fp,"VicPCTot %l\n",   ch->vic_pc_total);
  if (ch->death_pc_total != 0) do_fprintf (fp,"DeathPCTot %l\n", ch->death_pc_total);
  if (ch->vic_pc_arena   != 0) do_fprintf (fp,"VicArena %l\n",   ch->vic_pc_arena);
  if (ch->death_pc_arena != 0) do_fprintf (fp,"DeathArena %l\n", ch->death_pc_arena);

  if (ch->criminal > 0) do_fprintf (fp, "Criminal %d\n", ch->criminal);
  if (ch->godcurse > 0) do_fprintf (fp, "GodCurse %d\n", ch->godcurse);

  if (ch->short_descr[0] != '\0') do_fprintf (fp, "ShD  %s~\n", ch->short_descr);
  if (ch->long_descr[0]  != '\0') do_fprintf (fp, "LnD  %s~\n", ch->long_descr );
  if (ch->description[0] != '\0') do_fprintf (fp, "Desc %s~\n", ch->description);

  if (ch->prompt != NULL || !str_cmp (ch->prompt, "{w<{R%h{w/{y%Hhp {c%m{w/{y%Mm {g%vmv{w {G%e {B%X>{x  "))
      do_fprintf (fp, "Prom %s~\n", ch->prompt);

  if (ch->pcdata->tickstr) do_fprintf (fp, "Ticks %s~\n", ch->pcdata->tickstr);

  do_fprintf(fp, "Race %s~\n", race_table[ch->race].name);
  do_fprintf(fp, "Guild %s\n", pflag64(ch->pcdata->guild));
  do_fprintf(fp, "Elder %s\n", pflag64(ch->pcdata->elder));

  if (ch->clan)
  {
    do_fprintf (fp, "Clan %s~\n", ch->clan->name);
    do_fprintf (fp, "Rank %d\n",  ch->clanrank);
  }
  if (ch->clanpet) do_fprintf (fp, "Petit %s~\n", ch->clanpet->name);

  do_fprintf (fp, "Bou %u\n", ch->pcdata->bounty);
  do_fprintf (fp, "Sex %d\n", ch->sex);
  do_fprintf (fp, "Classdat %d %d %d %d\n", ch->class[0],ch->class[1],ch->class[2],ch->class[3]);
  do_fprintf (fp, "Class %d %d %d %d\n", ch->classmag, ch->classcle,
                                         ch->classthi, ch->classwar);
  do_fprintf (fp, "Remort %d\n",ch->remort);
  do_fprintf (fp, "Levl %d\n", ch->level);

  if (ch->trust != 0)  do_fprintf (fp, "Tru %d\n", ch->trust);

  do_fprintf (fp, "Sec %d\n", ch->pcdata->security);
  do_fprintf (fp, "Plyd %d\n", ch->played + (int) (current_time - ch->logon));
  do_fprintf (fp, "Not %l %l %l %l %l %l %l %l\n",
           ch->pcdata->last_note,    ch->pcdata->last_idea,
           ch->pcdata->last_penalty, ch->pcdata->last_news,
           ch->pcdata->last_changes, ch->pcdata->last_bugreport,
           ch->pcdata->last_offtopic,ch->pcdata->last_complain);

  do_fprintf (fp, "Scro %d\n",   ch->lines);
  if (!ch->in_room || ch->in_room==get_room_index(ROOM_VNUM_LIMBO))
  {
    if (ch->was_in_room) do_fprintf (fp, "Room %u\n", ch->was_in_room->vnum);
    else do_fprintf(fp, "Room %u", IS_SET(ch->act, PLR_ARMY) ? ROOM_VNUM_ARMY_BED : ROOM_VNUM_ALTAR );
  }
  else do_fprintf(fp, "Room %u\n", ch->in_room->vnum);
  do_fprintf (fp, "HMV %d %d %d %d %d %d\n",ch->hit, ch->max_hit,
              ch->mana, ch->max_mana, ch->move,ch->max_move );

  do_fprintf (fp, "Gold %u\n", (ch->gold > 0)   ? ch->gold   : 0);
  do_fprintf (fp, "Silv %u\n", (ch->silver > 0) ? ch->silver : 0);

  do_fprintf (fp, "Account %u\n", (ch->pcdata->account > 0) ? ch->pcdata->account : 0);
  do_fprintf (fp, "Exp %d\n", ch->exp);

  if (ch->act        != 0) do_fprintf (fp, "Act %s\n",    pflag64 (ch->act));
  if (ch->imm_flags  != 0) do_fprintf (fp, "Immun %s\n",  pflag64 (ch->imm_flags));
  if (ch->res_flags  != 0) do_fprintf (fp, "Resist %s\n", pflag64 (ch->res_flags));
  if (ch->vuln_flags != 0) do_fprintf (fp, "Vulner %s\n", pflag64 (ch->vuln_flags));

  if (ch->pcdata->auto_online)
     do_fprintf (fp, "Autoonline %s~\n", ch->pcdata->auto_online);
  do_fprintf (fp, "Autologout %d\n",  ch->settimer);

  if (ch->affected_by != 0) do_fprintf (fp,"AfBy %s\n", pflag64(ch->affected_by));

  do_fprintf (fp, "Talk %s\n", pflag64(ch->talk));
  do_fprintf (fp, "Comm %s\n", pflag64(ch->comm));
  do_fprintf (fp, "Coms %s\n", pflag64(ch->pcdata->comm_save));
  do_fprintf (fp, "Spam %s\n", pflag64(ch->spam));
  do_fprintf (fp, "Cfg  %s\n", pflag64(ch->pcdata->cfg));
  if (ch->maxrun[0]>0) do_fprintf (fp, "Maxrun %d\n", ch->maxrun[0]);

  if (ch->wiznet)      do_fprintf (fp,"Wizn %s\n", pflag64(ch->wiznet));
  if (ch->invis_level) do_fprintf (fp,"Invi %d\n", ch->invis_level);
  if (ch->incog_level) do_fprintf (fp,"Inco %d\n", ch->incog_level);

  do_fprintf (fp, "Pos %d\n", ch->position == POS_FIGHTING ? POS_STANDING : ch->position);

  if (ch->practice != 0) do_fprintf (fp, "Prac %d\n", ch->practice);
  if (ch->train    != 0) do_fprintf (fp, "Trai %d\n", ch->train   );
  do_fprintf(fp,"Trained %d %d\n",ch->pcdata->hptrained,ch->pcdata->manatrained);

  if (ch->saving_throw != 0) do_fprintf (fp, "Save %d\n", ch->saving_throw);

  do_fprintf (fp, "Alig %d\n",  ch->alignment);
  do_fprintf (fp, "Ralig %d\n", ch->real_alignment);
  do_fprintf (fp, "DeathCounter %d\n", ch->pcdata->deathcounter);

  if (ch->hitroll != 0) do_fprintf( fp, "Hit   %d\n",ch->hitroll);
  if (ch->damroll != 0) do_fprintf( fp, "Dam   %d\n",ch->damroll);

  do_fprintf (fp, "ACs %d %d %d %d\n",
    ch->armor[0], ch->armor[1], ch->armor[2], ch->armor[3]);

  if (ch->wimpy != 0) do_fprintf (fp, "Wimp %d\n", ch->wimpy);

  do_fprintf (fp, "Attr %d %d %d %d %d\n",
           ch->perm_stat[STAT_STR], ch->perm_stat[STAT_INT],
           ch->perm_stat[STAT_WIS], ch->perm_stat[STAT_DEX],
           ch->perm_stat[STAT_CON]);

  do_fprintf (fp, "AMod %d %d %d %d %d\n",
           ch->mod_stat[STAT_STR], ch->mod_stat[STAT_INT],
           ch->mod_stat[STAT_WIS], ch->mod_stat[STAT_DEX],
           ch->mod_stat[STAT_CON]);

  do_fprintf (fp, "Pass %s~\n", ch->pcdata->pwd);

  if (ch->pcdata->bamfin[0] != '\0') do_fprintf (fp, "Bin %s~\n", ch->pcdata->bamfin);

  if (ch->pcdata->bamfout[0] != '\0') do_fprintf (fp, "Bout %s~\n", ch->pcdata->bamfout);

  if (ch->pcdata->pseudoname) 
    do_fprintf (fp, "Pseudo %s~\n", ch->pcdata->pseudoname);

  do_fprintf (fp, "Titl %s~\n",      ch->pcdata->title     );
  do_fprintf (fp, "Pnts %d\n",       ch->pcdata->points    );
  do_fprintf (fp, "TSex %d\n",       ch->pcdata->true_sex  );
  do_fprintf (fp, "LLev %d\n",       ch->pcdata->last_level);
  do_fprintf (fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit,
       ch->pcdata->perm_mana,ch->pcdata->perm_move);

  do_fprintf (fp, "Cnd  %d %d %d %d\n",
       ch->pcdata->condition[0],ch->pcdata->condition[1],
       ch->pcdata->condition[2],ch->pcdata->condition[3]);

  do_fprintf(fp, "Marry %s~\n", ch->pcdata->marry);
  do_fprintf(fp, "Father %s~\n", ch->pcdata->father);
  do_fprintf(fp, "Mother %s~\n", ch->pcdata->mother);
  do_fprintf(fp, "Kins %s~\n", ch->pcdata->kins);
  do_fprintf(fp, "Mroom %u\n", ch->pcdata->proom);
  do_fprintf(fp, "Lovers %s~\n", ch->pcdata->lovers);
  do_fprintf(fp, "Vislist %s~\n", ch->pcdata->vislist);
  do_fprintf(fp, "IgnoreList %s~\n", ch->pcdata->ignorelist);
 
  {
    int a;

    do_fprintf(fp,"Quenia");
    for (a=0;a<MAX_QUENIA;a++) do_fprintf(fp," %u",ch->pcdata->quenia[a]);
    do_fprintf(fp,"\n");
  }
  for (pos = 0; pos < MAX_ALIAS; pos++)
  {
    if (ch->pcdata->alias[pos] == NULL
       || ch->pcdata->alias_sub[pos] == NULL) break;
      do_fprintf (fp,"Alias %s %s~\n", ch->pcdata->alias[pos], ch->pcdata->alias_sub[pos]);
  }

  for (sn = 0; sn < max_skill; sn++)
  {
    if (skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0)
      do_fprintf (fp, "Sk %d '%s'\n", ch->pcdata->learned[sn],skill_table[sn].name);
  }

  for (gn = 0; gn < MAX_GROUP; gn++)
  {
    if (group_table[gn].name != NULL && ch->pcdata->group_known[gn])
      do_fprintf (fp, "Gr '%s'\n",group_table[gn].name);
  }

  for (paf = ch->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type < 0 || paf->type >= max_skill) continue;
    do_fprintf (fp, "Affc '%s' %d %d %d %d %d %u\n",
      skill_table[paf->type].name, paf->where,
      paf->level, paf->duration, paf->modifier, paf->location,
      paf->bitvector);
  }
  for (pena = ch->penalty;pena; pena = pena->next)
  {
    do_fprintf (fp, "Pena %d %u %u %s~\n",
      pena->type, pena->affect, pena->ticks, pena->inquisitor);
  }   
  
  do_fprintf (fp, "End\n\n");
}

void fwrite_pet (CHAR_DATA * pet, FILE * fp)
{
  AFFECT_DATA * paf;
  OBJ_DATA    * obj;
  OBJ_DATA    * obj_next;

  for (obj = pet->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    if (obj->wear_loc == WEAR_NONE) continue;
    obj_from_char (obj);
    obj_to_room (obj, pet->in_room);
  }
  do_fprintf (fp, "#PET\n");
  do_fprintf (fp, "Vnum %u\n", pet->pIndexData->vnum);
  do_fprintf (fp, "Name %s~\n", pet->name);
  do_fprintf (fp, "LogO %l\n", current_time);

  if (pet->short_descr != pet->pIndexData->short_descr)
    do_fprintf (fp, "ShD %s~\n", pet->short_descr);

  if (pet->long_descr != pet->pIndexData->long_descr)
    do_fprintf (fp, "LnD %s~\n", pet->long_descr);

  if (pet->description != pet->pIndexData->description)
    do_fprintf (fp, "Desc %s~\n", pet->description);

  if (pet->race != pet->pIndexData->race)
    do_fprintf (fp,"Race %s~\n", race_table[pet->race].name);

  if (pet->clan) do_fprintf (fp, "Clan %s~\n", pet->clan->name);

  do_fprintf (fp, "Sex %d\n", pet->sex);

  if (pet->level != pet->pIndexData->level)
    do_fprintf (fp, "Levl %d\n", pet->level);

  do_fprintf (fp, "HMV  %d %d %d %d %d %d\n",
     pet->hit, pet->max_hit, pet->mana,
     pet->max_mana, pet->move, pet->max_move);

  if (pet->gold   > 0) do_fprintf (fp, "Gold %u\n", pet->gold);
  if (pet->silver > 0) do_fprintf (fp, "Silv %u\n", pet->silver);
  if (pet->exp    > 0) do_fprintf (fp, "Exp  %d\n",  pet->exp);

  if (pet->act != pet->pIndexData->act)
    do_fprintf (fp, "Act %s\n", pflag64(pet->act));

  if (pet->affected_by != pet->pIndexData->affected_by)
    do_fprintf(fp, "AfBy %s\n", pflag64(pet->affected_by));

  if (pet->comm != 0) do_fprintf (fp, "Comm %s\n", pflag64(pet->comm));
    do_fprintf (fp, "Pos %d\n", pet->position = POS_FIGHTING ? POS_STANDING : pet->position);

  if (pet->saving_throw != 0)
    do_fprintf (fp, "Save %d\n", pet->saving_throw);

  if (pet->alignment != pet->pIndexData->alignment)
    do_fprintf (fp, "Alig %d\n", pet->alignment);

  if (pet->hitroll != pet->pIndexData->hitroll)
    do_fprintf (fp, "Hit %d\n", pet->hitroll);

  if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
    do_fprintf (fp, "Dam %d\n", pet->damroll);

  do_fprintf (fp, "ACs %d %d %d %d\n", pet->armor[0], pet->armor[1],
                                       pet->armor[2], pet->armor[3]);

  do_fprintf (fp, "Attr %d %d %d %d %d\n", pet->perm_stat[STAT_STR],
    pet->perm_stat[STAT_INT], pet->perm_stat[STAT_WIS],
    pet->perm_stat[STAT_DEX], pet->perm_stat[STAT_CON]);

  do_fprintf (fp, "AMod %d %d %d %d %d\n", pet->mod_stat[STAT_STR],
    pet->mod_stat[STAT_INT], pet->mod_stat[STAT_WIS],
    pet->mod_stat[STAT_DEX], pet->mod_stat[STAT_CON]);
    
  for (paf = pet->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type < 0 || paf->type >= max_skill) continue;
            
    do_fprintf (fp, "Affc '%s' %d %d %d %d %d %u\n",
      skill_table[paf->type].name,
      paf->where, paf->level, paf->duration, paf->modifier,
      paf->location, paf->bitvector);
  }
  do_fprintf (fp,"End\n");
}
    
void fwrite_obj (CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest, bool type)
{
  EXTRA_DESCR_DATA * ed;
  AFFECT_DATA * paf;

  /* Slick recursion to write lists backwards,
    so loading them will load in forwards order */
  if (type && obj->next_content != NULL) fwrite_obj (ch, obj->next_content, fp, iNest, TRUE);

  /* Castrate storage characters (NOT IMMS (c)Apc :))) ). */
  if (!IS_IMMORTAL(ch) && ch->level < obj->level-5 &&
      obj->item_type != ITEM_CONTAINER &&
      obj->item_type != ITEM_ENCHANT) return;

  if (obj->item_type==ITEM_KEY   || obj->pIndexData->vnum==26  ||
      obj->pIndexData->vnum==120 || obj->pIndexData->vnum==121 ||
      obj->pIndexData->vnum==122 || obj->pIndexData->vnum==123) return;

  if (type) do_fprintf (fp, "#O\n");
  else  do_fprintf (fp, "#F\n");
  do_fprintf (fp, "Vnum %u\n", obj->pIndexData->vnum);

  if (!obj->pIndexData->new_format) do_fprintf (fp, "Oldstyle\n");

  if (obj->enchanted) do_fprintf (fp, "Enchanted\n");
  do_fprintf (fp, "Nest %d\n", iNest);

  /* These data are only used if they do not match the defaults */
  if (obj->name != obj->pIndexData->name)
    do_fprintf (fp, "Name %s~\n", obj->name);

  if (obj->owner)
    do_fprintf (fp, "Owner %s~\n", obj->owner);

  if (obj->loaded_by!=NULL)
    do_fprintf (fp, "Loaded_by %s~\n", obj->loaded_by);

  if (obj->short_descr != obj->pIndexData->short_descr)
    do_fprintf (fp, "ShD %s~\n", obj->short_descr);

  if (obj->description != obj->pIndexData->description)
    do_fprintf (fp, "Desc %s~\n", obj->description);

  if (obj->extra_flags != obj->pIndexData->extra_flags)
    do_fprintf (fp, "ExtF %s\n", pflag64(obj->extra_flags));

  if (obj->wear_flags != obj->pIndexData->wear_flags)
    do_fprintf (fp, "WeaF %s\n",pflag64(obj->wear_flags));

  if (obj->item_type != obj->pIndexData->item_type)
    do_fprintf (fp, "Ityp %d\n", obj->item_type);

  if (obj->weight != obj->pIndexData->weight)
    do_fprintf (fp, "Wt %d\n", obj->weight);

  if (obj->material != obj->pIndexData->material)
    do_fprintf (fp, "Mater %s~\n", obj->material);

  if (obj->condition != obj->pIndexData->condition)
    do_fprintf (fp, "Cond %d\n", obj->condition);

  /*-- Variable data --*/
  do_fprintf (fp, "Wear %d\n", obj->wear_loc);

  if (obj->level != obj->pIndexData->level)
    do_fprintf (fp, "Lev %d\n", obj->level);

  if (obj->timer != 0) do_fprintf (fp, "Time %d\n",obj->timer);

  do_fprintf (fp, "Cost %u\n", obj->cost);

  if (obj->durability != obj->pIndexData->durability )
    do_fprintf (fp, "Dura %d\n", obj->durability);

  if (obj->value[0] != obj->pIndexData->value[0] ||
      obj->value[1] != obj->pIndexData->value[1] ||
      obj->value[2] != obj->pIndexData->value[2] ||
      obj->value[3] != obj->pIndexData->value[3] ||
      obj->value[4] != obj->pIndexData->value[4]) 
    do_fprintf (fp, "Val  %u %u %u %u %u\n",
      obj->value[0], obj->value[1], obj->value[2],
      obj->value[3], obj->value[4]);

  switch (obj->item_type)
  {
    case ITEM_POTION:
    case ITEM_SCROLL:
    case ITEM_PILL:
      if (obj->value[1] > 0 && obj->value[1] < max_skill)
        do_fprintf (fp, "Spell 1 '%s'\n", skill_table[obj->value[1]].name);
      if (obj->value[2] > 0 && obj->value[2] < max_skill)
        do_fprintf (fp, "Spell 2 '%s'\n", skill_table[obj->value[2]].name);
      if (obj->value[3] > 0 && obj->value[3] < max_skill)
        do_fprintf (fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name);
      break;
    case ITEM_STAFF:
    case ITEM_WAND:
      if (obj->value[3] > 0 && obj->value[3] < max_skill)
        do_fprintf (fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name);
      break;
  }

  for (paf = obj->affected; paf != NULL; paf = paf->next )
  {
    if (paf->type < 0 || paf->type >= max_skill || paf->location == APPLY_SPELL_AFFECT) continue;
      do_fprintf (fp, "Affc '%s' %d %d %d %d %d %u\n",
        skill_table[paf->type].name, paf->where,
        paf->level, paf->duration, paf->modifier,
        paf->location, paf->bitvector);
  }

  for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
    do_fprintf (fp, "ExDe %s~ %s~\n",ed->keyword, ed->description);

  do_fprintf (fp, "End\n\n");

  if (type && obj->contains != NULL) fwrite_obj (ch, obj->contains, fp, iNest + 1, TRUE);
}

bool load_char_obj (DESCRIPTOR_DATA * d, const char * name, int status)
{
  char strsave [MAX_INPUT_LENGTH] ;
  char buf     [MAX_STRING_LENGTH];
  CHAR_DATA * ch;

  FILE * fp ;
  bool found;
  int  stat;

  #if defined (WIN32)
      do_printf (buf, " ");
  #endif

  ch = new_char ();
  ch->pcdata = new_pcdata ();

  d->character               = ch;
  ch->desc                   = d;
  ch->name                   = str_dup(name);
  ch->id                     = get_pc_id ();
  ch->race                   = race_lookup ("human");
  ch->act                    = PLR_NOSUMMON;
  ch->comm                   = COMM_PROMPT ;
  ch->prompt                 = str_dup("{w<{R%h{w/{y%Hhp {c%m{w/{y%Mm {g%vmv{w {G%e {B%X>{x  ");
  ch->pcdata->confirm_delete = FALSE;
  ch->pcdata->pwd            = str_dup ("");
  ch->pcdata->bamfin         = str_dup (""); 
  ch->pcdata->bamfout        = str_dup ("");
  ch->pcdata->pseudoname     = str_dup (""); 
  ch->pcdata->title          = str_dup ("");
  ch->pcdata->deny_addr      = str_dup ("");
  ch->pcdata->denied_by      = str_dup ("");
  ch->host                   = NULL;

  for (stat = 0; stat < MAX_STATS; stat++) ch->perm_stat[stat] = 13;
  ch->pcdata->condition[COND_THIRST]  = 48  ; 
  ch->pcdata->condition[COND_FULL]    = 48  ;
  ch->pcdata->condition[COND_HUNGER]  = 48  ;
  ch->pcdata->security                = 0   ;
  ch->morph_obj                       = NULL;
  ch->pcdata->tournament              = 0   ;
  ch->pcdata->bet                     = 0   ;
  ch->pcdata->charged_spell           = 0   ;
  ch->pcdata->charged_num             = 0   ;

  found = FALSE;
  fclose (fpReserve);
  if (status==SAVE_NORMAL)
    do_printf (strsave, "%s%s", PLAYER_DIR, capitalize (name));
  if (status==SAVE_BACKUP)
    do_printf (strsave, "%s%s", PLAYER_DIR2, capitalize (name));
  if (status==SAVE_BACKUP2)
    do_printf (strsave, "%s%s", PLAYER_DIR3, capitalize (name));
  if ((fp = fopen (strsave, "r")) == NULL)
  {
  #if defined(unix)
    /* decompress if player.tgz file exists */
    do_printf (strsave, "%splayer.tgz", status==SAVE_NORMAL?PLAYER_DIR:PLAYER_DIR2);
    if ((fp = fopen (strsave, "r")) != NULL)
    {
      fclose  (fp);
      do_printf (buf, "tar -xvfz %s %s %s", strsave,name,status==SAVE_NORMAL?PLAYER_DIR:PLAYER_DIR2);
      system  (buf);
    }
  #else
    // decompress if players.rar exist
    do_printf (strsave, "%splayer.rar", status==SAVE_NORMAL?PLAYER_DIR:PLAYER_DIR2);
    if ((fp = fopen (strsave, "r")) != NULL)
    {
      fclose  (fp);
      do_printf (buf, "rar e %s %s %s", strsave,name,status==SAVE_NORMAL?PLAYER_DIR:PLAYER_DIR2);
      system  (buf);
    }
  #endif
  if (status==SAVE_NORMAL)
    do_printf (strsave, "%s%s", PLAYER_DIR, capitalize (name));
  if (status==SAVE_BACKUP)
    do_printf (strsave, "%s%s", PLAYER_DIR2, capitalize (name));
  if (status==SAVE_BACKUP2)
    do_printf (strsave, "%s%s", PLAYER_DIR3, capitalize (name));
  fp = fopen (strsave, "r");
  }
  if (fp)
  {
    int iNest;

    for (iNest = 0; iNest < MAX_NEST; iNest++)
          rgObjNest [iNest] = NULL;

    found = TRUE;

    for (;!feof(fp);)
    {
      char letter;
      const char * word;

      letter = fread_letter (fp);
      if (letter == '*')
      {
        fread_to_eol (fp);
        continue;
      }

      if (letter != '#')
      {
        bug ("Load_char_obj: # not found.", 0);
        break;
      }

      word = fread_word (fp);

      if ( !str_cmp (word, "PLAYER") )
      {
        fread_char (ch, fp);
        if ( ch == NULL )
        {
          bug("Loading character's objects failed: NULL character.", 0);
          break;
        }
      }
      else if (!str_cmp (word, "O"  )) fread_obj  (ch, fp, TRUE);
      else if (!str_cmp (word, "F"  )) fread_obj  (ch, fp, FALSE);
      else if (!str_cmp (word, "PET")) fread_pet  (ch, fp);
      else if (!str_cmp (word, "END")) break;
      else
      {
        bug ("Load_char_obj: bad section.", 0);
        break;
      }

    }
    fclose (fp);
  }

  fpReserve = fopen (NULL_FILE, "r");
  if (found)
  {
    int i;

    if (ch->race == 0) ch->race = RACE_HUMAN;

    ch->size = race_table[ch->race].size;
    ch->dam_type = 17; /* punch */

    for (i = 0; i < 5; i++)
    {
      if (race_table[ch->race].skills[i] == NULL) break;
      group_add (ch,race_table[ch->race].skills[i],FALSE);
    }
    if (ch->clan) group_add(ch,"clan",FALSE);

    ch->affected_by = ch->affected_by | race_table[ch->race].aff;
    ch->imm_flags   = ch->imm_flags   | race_table[ch->race].imm;
    ch->res_flags   = ch->res_flags   | race_table[ch->race].res;
    ch->vuln_flags  = ch->vuln_flags  | race_table[ch->race].vuln;

    ch->form        = race_table[ch->race].form ;
  }
  // Checks on correct character data
  if (ch->level==0) found=FALSE;
  else if (!ch->name) found=FALSE;
  else if (!ch->pcdata || !ch->pcdata->pwd) found=FALSE;
  return found;
}

/*-- Read in a char --*/
#if defined(KEY)
#undef KEY
#endif

#define KEY(literal, field, value)           \
        if (!str_cmp (word, literal))        \
        {                                    \
            field  = value;                 \
            fMatch = TRUE;                  \
            break;                          \
        }

/*-- provided to free strings --*/
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS(literal, field, value)          \
        if (!str_cmp (word, literal))        \
        {                                    \
            free_string (field);            \
            field  = value;                 \
            fMatch = TRUE;                  \
            break;                          \
        }

void fread_char (CHAR_DATA * ch, FILE * fp)
{
  const char * word;
  const char * tmp ;
  bool fMatch;
  int count = 0;
  int lastlogoff = current_time;
  int percent;
  int i;
//  int hp_per_lvl, diff;

  tmp = str_dup ("");

  log_printf("Loading %s.", ch->name);

  ch->vic_npc = ch->death_npc = ch->vic_pc_total =
  ch->death_pc_total = ch->vic_pc_arena = ch->death_pc_arena = 0;

  ch->pcdata->auto_online = '\0';
  ch->pcdata->deathcounter = 0;
  ch->host = NULL;
  ch->deity = NULL;
  ch->pcdata->dn = 25;
  ch->qcounter = ch->qcomplete[0] = ch->qcomplete[1] = 0;
  for (i=0;i<MAX_VICT;i++)
  {
   ch->pcdata->victims[i].victim = NULL;
   ch->pcdata->victims[i].dampool = 0;
  }

  for (;;)
  {
    word   = feof (fp) ? "End" : fread_word (fp);
    fMatch = FALSE;

    switch (UPPER(word[0]))
    {
     case '*':
      fMatch = TRUE;
      fread_to_eol (fp);
      break;

     case 'A':
      KEY("Account",     ch->pcdata->account,     fread_number64(fp));
      KEY("Act",         ch->act,                 fread_flag64(fp));
      if (IS_SET(ch->act, PLR_QUESTOR)) REM_BIT(ch->act, PLR_QUESTOR);
      KEY("AfBy",        ch->affected_by,         fread_flag64(fp));
      KEY("Alig",        ch->alignment,           fread_number(fp));
      KEY("Autoonline",  ch->pcdata->auto_online, fread_string(fp));
      KEY("Autologout",  ch->settimer,            fread_number(fp));

      if (!str_cmp (word, "Alias"))
      {
        if (count >= MAX_ALIAS)
        {
          fread_to_eol (fp);
          fMatch = TRUE;
          break;
        }

        ch->pcdata->alias[count]     = str_dup(fread_word(fp));
        ch->pcdata->alias_sub[count] = fread_string(fp);
        count++;
        fMatch = TRUE;
        break;
      }

      if (!str_cmp (word,"ACs"))
      {
        int i;

        for (i = 0; i < 4; i++) ch->armor[i] = fread_number(fp);
        fMatch = TRUE;
        break;
      }

      if (!str_cmp (word, "Affc"))
      {
        AFFECT_DATA * paf;
        int sn;
        paf = new_affect ();
        paf->type=0;
        sn = skill_lookup(fread_word(fp));

        if (sn < 0) bug ("Fread_char: unknown skill.", 0);
        else paf->type = sn;

        paf->where      = fread_number (fp);
        paf->level      = fread_number (fp);
        paf->duration   = fread_number (fp);
        paf->modifier   = fread_number (fp);
        paf->location   = fread_number (fp);
        paf->bitvector  = fread_number64 (fp);
        paf->next       = ch->affected;
        ch->affected    = paf;

        fMatch = TRUE;
        break;
      }

      if (!str_cmp (word, "AttrMod") || !str_cmp(word,"AMod"))
      {
          int stat;

          for (stat = 0; stat < MAX_STATS; stat ++)
              ch->mod_stat[stat] = fread_number(fp);

          fMatch = TRUE;
          break;
      }

      if (!str_cmp (word, "AttrPerm") || !str_cmp(word,"Attr"))
      {
          int stat;

          for (stat = 0; stat < MAX_STATS; stat++)
              ch->perm_stat[stat] = fread_number(fp);

          fMatch = TRUE;
          break;
      }
      break;

     case 'B':
      KEY("Bin",  ch->pcdata->bamfin,  fread_string (fp));
      KEY("Bout", ch->pcdata->bamfout, fread_string (fp));
      KEY("Bou",  ch->pcdata->bounty,  fread_number64 (fp));
      break;

     case 'C':
      KEY("Carma",    ch->pcdata->carma, fread_number(fp));
      KEY("Cla",      ch->class[0],      fread_number(fp));
      KEY("Clan",     ch->clan,          clan_lookup(fread_string(fp)));
      if ( ch->clan == NULL && ch->level > 10)
       {    
        ch->clan = clan_lookup("loner");
        ch->clanrank = 0;
       }        
      KEY("Criminal", ch->criminal,   fread_number(fp));
      KEY("Comm",     ch->comm,       fread_flag64(fp)); 
      KEY("Coms",     ch->pcdata->comm_save, fread_flag64(fp));
      KEY("Cfg",      ch->pcdata->cfg,fread_flag64(fp)); 

      if (!str_cmp (word, "Class"))
      {
        ch->classmag = fread_number (fp);
        ch->classcle = fread_number (fp);
        ch->classthi = fread_number (fp);
        ch->classwar = fread_number (fp);
        fMatch = TRUE;
        break;
      }

      if (!str_cmp (word, "Classdat"))
      {
        int tmp;
        tmp=fread_number(fp);
        ch->class[0] = URANGE(0,tmp,3);
        tmp=fread_number(fp);
        ch->class[1] = URANGE(0,tmp,3);
        tmp=fread_number(fp);
        ch->class[2] = URANGE(0,tmp,3);
        tmp=fread_number(fp);
        ch->class[3] = URANGE(0,tmp,3);
        fMatch = TRUE;
        break;
      }

      if (!str_cmp(word, "Cnd"))
      {
        ch->pcdata->condition[0] = fread_number(fp);
        ch->pcdata->condition[1] = fread_number(fp);
        ch->pcdata->condition[2] = fread_number(fp);
        ch->pcdata->condition[3] = fread_number(fp);
        fMatch = TRUE;
        break;
      }
      break;

     case 'D':
      KEY("Dam",         ch->damroll,           fread_number (fp));
      KEY("DeathCounter",ch->pcdata->deathcounter, fread_number (fp));
      KEY("DeathNPC",    ch->death_npc,         fread_number (fp));
      KEY("DeathPCTot",  ch->death_pc_total,    fread_number (fp));
      KEY("DeathArena",  ch->death_pc_arena,    fread_number (fp));
      KEY("DeityCurse",  ch->pcdata->dc,        fread_number (fp));
      KEY("Deitynumber", ch->pcdata->dn,        fread_number (fp));
      KEY("DeityRank",   ch->pcdata->dr,        fread_number (fp));
      KEY("Denied_by",   ch->pcdata->denied_by, fread_string (fp));
      KEY("Description", ch->description,       fread_string (fp));
      KEY("Desc",        ch->description,       fread_string (fp));
      break;
     case 'E':
      KEY("Exp", ch->exp,   fread_number (fp));
      KEY("Elder", ch->pcdata->elder, fread_flag(fp));

      if (!str_cmp (word, "End"))
      {
      // adjust hp mana move up  -- here for speed's sake 
        percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);
        percent = UMIN(percent, 100);


        if (ch->exp > exp_per_level(ch,ch->pcdata->points) * (ch->level+1)
        || ch->exp < exp_per_level(ch,ch->pcdata->points) * ch->level)
           ch->exp=exp_per_level(ch,ch->pcdata->points) * ch->level;
        if (ch->settimer<3) ch->settimer = 5;
        if (ch->remort>3) ch->remort=3;
        if (ch->exp < exp_per_level(ch,ch->pcdata->points) * (ch->level))
        ch->exp=exp_per_level(ch,ch->pcdata->points)*(ch->level);

        if (percent > 0 && !IS_AFFECTED(ch, AFF_POISON) &&
          !IS_AFFECTED (ch, AFF_PLAGUE))
        {
          ch->hit  += (ch->max_hit - ch->hit)   * percent / 100;
          ch->mana += (ch->max_mana - ch->mana) * percent / 100;
          ch->move += (ch->max_move - ch->move) * percent / 100;
        }
//        if (ch->version<15) ch->comm=COMM_PROMPT;
        REM_BIT(ch->pcdata->guild,ASSASIN_GUILD);
        REM_BIT(ch->pcdata->elder,ASSASIN_GUILD);

// Hero & BH removal
/*
        REM_BIT(ch->act, PLR_LASTREMORT);
        REM_BIT(ch->act, PLR_5REMORT);
        if (ch->remort > 2) 
         {
          ch->remort = 2;
               if (ch->class[3]==0)     ch->classmag=FALSE;
          else if (ch->class[3]==1)     ch->classcle=FALSE;
          else if (ch->class[3]==2)     ch->classthi=FALSE;
          else if (ch->class[3]==3)     ch->classwar=FALSE;
         }
*/         
        return;
         }
      break;

     case 'F':
      KEY("Father",    ch->pcdata->father,    fread_string(fp) );
      KEY("Favour",    ch->pcdata->favour,    fread_number(fp) );
      break;

     case 'G':
      KEY("GodCurse", ch->godcurse,   fread_number(fp));
      KEY("Gold",  ch->gold, fread_number64 (fp));
      KEY("Guild", ch->pcdata->guild, fread_flag(fp));
      if (!str_cmp (word, "Group") || !str_cmp (word,"Gr"))
      {
          int gn;
          const char * temp;

          temp = fread_word (fp);
          gn = group_lookup (temp);

          if (gn < 0)
          {
            do_fprintf (stderr, "%s", temp);
            bug ("Fread_char: unknown group. ", 0);
          }
          else gn_add (ch, gn);
          fMatch = TRUE;
      }
      if (!str_cmp(word, "GQstat"))
      {
        ch->gqcounter  = fread_number (fp);
        ch->gqcomplete = fread_number (fp);
        fMatch = TRUE;
        break;
      }
      break;

     case 'H':
      KEY("Hit", ch->hitroll, fread_number (fp));

      if (!str_cmp (word, "Host"))
      { 
          KEY("Host", ch->host, fread_string (fp));
          fMatch = TRUE;
          break;
      }

      if (!str_cmp (word, "HpManaMove") || !str_cmp(word,"HMV"))
      {
          ch->hit      = fread_number (fp);
          ch->max_hit  = fread_number (fp);
          ch->mana     = fread_number (fp);
          ch->max_mana = fread_number (fp);
          ch->move     = fread_number (fp);
          ch->max_move = fread_number (fp);
          fMatch = TRUE;
          break;
      }

      if (!str_cmp (word, "HpManaMovePerm") || !str_cmp(word,"HMVP"))
      {
          ch->pcdata->perm_hit  = fread_number (fp);
          ch->pcdata->perm_mana = fread_number (fp);
          ch->pcdata->perm_move = fread_number (fp);
          fMatch = TRUE;
          break;
      }
      break;

     case 'I':
      KEY("Id",         ch->id,                 fread_number (fp));
      KEY("IgnoreList", ch->pcdata->ignorelist, fread_string (fp));
      KEY("InvisLevel", ch->invis_level,        fread_number (fp));
      KEY("Inco",       ch->incog_level,        fread_number (fp));
      KEY("Invi",       ch->invis_level,        fread_number (fp));
      KEY("Immun",      ch->imm_flags,          fread_flag64 (fp));
      break;

     case 'K':
      KEY("Kins",    ch->pcdata->kins,    fread_string (fp));
      break;

     case 'L':
      KEY("LastLevel", ch->pcdata->last_level, fread_number (fp));
      KEY("LastLogin", ch->lastlogin,          fread_number (fp));
      KEY("Ldefend",   ch->pcdata->protect,    fread_number (fp));
      KEY("LLev",      ch->pcdata->last_level, fread_number (fp));
      KEY("Lovers",    ch->pcdata->lovers,     fread_string (fp));
      KEY("Levl",      ch->level,              fread_number (fp));
      KEY("LogO",      lastlogoff,             fread_number (fp));
      KEY("LnD",       ch->long_descr,         fread_string (fp));
      break;

     case 'M':
      KEY("Marry",     ch->pcdata->marry,     fread_string (fp));
      KEY("Mother",    ch->pcdata->mother,    fread_string (fp));
      KEY("Mroom",     ch->pcdata->proom,     fread_number(fp));
      KEY("Maxrun",    ch->maxrun[0],         fread_number(fp));
      break;

     case 'N':
      KEYS("Name", ch->name, fread_string (fp));

      if (!str_cmp(word,"Not"))
      {
        ch->pcdata->last_note    = fread_number (fp);
        ch->pcdata->last_idea    = fread_number (fp);
        ch->pcdata->last_penalty = fread_number (fp);
        ch->pcdata->last_news    = fread_number (fp);
        ch->pcdata->last_changes = fread_number (fp);
        ch->pcdata->last_bugreport = fread_number(fp);
        ch->pcdata->last_offtopic = fread_number (fp);
        ch->pcdata->last_complain = fread_number (fp);
        fMatch = TRUE;
        break;
      }
      break;

     case 'P':
      KEY("Pass",  ch->pcdata->pwd,        fread_string (fp));
      KEY("Plyd",  ch->played,             fread_number (fp));
      KEY("Pnts",  ch->pcdata->points,     fread_number (fp));
      KEY("Pos",   ch->position,           fread_number (fp));
      KEY("Prac",  ch->practice,           fread_number (fp));
      KEY("Prom",  ch->prompt,             fread_string (fp));
      KEY("PerBan",ch->pcdata->deny_addr,  fread_string (fp));
      KEY("Petit", ch->clanpet,clan_lookup(fread_string (fp)));
      KEY("Pseudo",ch->pcdata->pseudoname, fread_string (fp));
  
      if (!str_cmp (word, "Pena"))
      {
        PENALTY_DATA *pena, *pnext;
        pena=new_penalty();
        pena->type      = fread_number(fp);
        pena->affect    = fread_number64(fp);
        pena->ticks     = fread_number64(fp);
//        if (ch->version<22) fread_number(fp);
        pena->inquisitor= fread_string(fp);
        pnext=ch->penalty;        
        ch->penalty=pena;
        ch->penalty->next=pnext;
        fMatch = TRUE;
        break;
      }
      break;

     case 'Q':
      if (!str_cmp(word,"Quenia"))
      {
        int a;
        for (a=0;a<MAX_QUENIA;a++) ch->pcdata->quenia[a]=fread_number64(fp);
        fMatch=TRUE;
        break;
      }
      KEY ("QuestPnts", ch->questpoints,    fread_number (fp));
      KEY ("QuestNext", ch->nextquest,      fread_number (fp));
      if (!str_cmp(word, "Qstat"))
      {
        ch->qcounter     = fread_number (fp);
        ch->qcomplete[0] = fread_number (fp);
        ch->qcomplete[1] = fread_number (fp);
        fMatch = TRUE;
        break;
      }
      break;
     case 'R':
      KEY("Rank",   ch->clanrank,       fread_number (fp));
      KEY("Ralig",  ch->real_alignment, fread_number (fp));
      KEY("Remort", ch->remort,         fread_number (fp));
      KEY("Resist", ch->res_flags,      fread_flag64 (fp));
      KEY("Race",   ch->race,           race_lookup(fread_string(fp)));

      if (!str_cmp(word, "Room"))
      {
        ch->in_room = get_room_index (fread_number64 (fp));
        if (!ch->in_room) ch->in_room = get_room_index (IS_SET(ch->act, PLR_ARMY)?ROOM_VNUM_ARMY_BED:ROOM_VNUM_ALTAR);
        fMatch = TRUE;
        break;
      }

      break;

     case 'S':
      KEY("Save", ch->saving_throw,     fread_number (fp));
      KEY("Scro", ch->lines,            fread_number (fp));
      KEY("Sex",  ch->sex,              fread_number (fp));
      KEY("ShD",  ch->short_descr,      fread_string (fp));
      KEY("Sec",  ch->pcdata->security, fread_number (fp));   /* OLC */
      KEY("Silv", ch->silver,           fread_number64 (fp));
      KEY("Spam", ch->spam,             fread_flag(fp)); 

      if (!str_cmp(word, "Skill") || !str_cmp(word, "Sk"))
      {
        int    sn;
        int    value;
        const char * temp;

        value = fread_number (fp);
        temp  = fread_word   (fp);

        sn    = skill_lookup (temp);
        if (sn<0 && !str_cmp(temp,"know alignment")) sn=skill_lookup("know person");

        if (sn < 0)
        {
          do_fprintf (stderr,"%s", temp);
          bug ("Fread_char: unknown skill. ", 0);
        }
        else ch->pcdata->learned[sn] = UMIN(value,100);

        fMatch = TRUE;
      }

      break;

     case 'T':
      KEY("Talk", ch->talk,             fread_flag64(fp));
      KEY("TSex", ch->pcdata->true_sex, fread_number (fp));
      KEY("Trai", ch->train,            fread_number (fp));
      KEY("Tru",  ch->trust,            fread_number (fp));

      if (!str_cmp (word, "Trained"))
      {
        ch->pcdata->hptrained=fread_number(fp);
        ch->pcdata->manatrained=fread_number(fp);
        fMatch = TRUE;
        break;
      }

      KEY("Ticks", ch->pcdata->tickstr, fread_string (fp));
      KEY("Title",ch->pcdata->title, fread_string (fp));
      KEY("Titl",ch->pcdata->title, fread_string (fp));
      break;

     case 'V':
      KEY("Vers",     ch->version,        fread_number (fp));
      KEY("VicNPC",   ch->vic_npc,        fread_number (fp));
      KEY("VicPCTot", ch->vic_pc_total,   fread_number (fp));
      KEY("VicArena", ch->vic_pc_arena,   fread_number (fp));
      KEY("Vislist",  ch->pcdata->vislist,fread_string (fp));
      KEY("Vulner",   ch->vuln_flags,     fread_flag64 (fp));

      if (!str_cmp (word, "Vnum"))
      {
        ch->pIndexData = get_mob_index (fread_number64 (fp));
        fMatch = TRUE;
        break;
      }
      break;

     case 'W':
      KEY("Wimp", ch->wimpy,  fread_number (fp));
      KEY("Wizn", ch->wiznet, fread_flag   (fp));

      break;
    }
    
    if (!fMatch)
    {
      log_printf("Fread_char: no match '%s'", word);
      fread_to_eol (fp);
    }
  }
      if ( ch->clan == NULL && ( ch->level > 10 || ch->remort > 0) )
       {    
        ch->clan = clan_lookup("loner");
        ch->clanrank = 0;
       }

  if (ch->pcdata && ch->pcdata->hptrained)
  {
   ch->train   += ch->pcdata->hptrained;
   ch->max_hit -= ch->pcdata->hptrained*10;
  }
  return;
}

void fread_pet (CHAR_DATA * ch, FILE * fp)
{
  const char * word;
  CHAR_DATA * pet;
  bool fMatch;
  int lastlogoff = current_time;
  int percent;

  /*-- First entry had BETTER be the vnum or we barf --*/
  word = feof(fp) ? "END" : fread_word(fp);

  if (!str_cmp(word, "Vnum"))
  {
    int64 vnum;
    
    vnum = fread_number64 (fp);

    if (get_mob_index(vnum) == NULL)
    {
      bug ("Fread_pet: bad vnum %u.", vnum);
      pet = create_mobile (get_mob_index (MOB_VNUM_FIDO));
    }
    else pet = create_mobile (get_mob_index(vnum));
  }
  else
  {
    bug ("Fread_pet: no vnum in file.", 0);
    pet = create_mobile (get_mob_index (MOB_VNUM_FIDO));
  }
  
  for (;;)
  {
    word   = feof(fp) ? "END" : fread_word (fp);
    fMatch = FALSE;
    
    switch (UPPER(word[0]))
    {
      case '*':
      fMatch = TRUE;
      fread_to_eol (fp);
      break;
          
      case 'A':
      KEY("Act",  pet->act,         fread_flag64 (fp));
      KEY("AfBy", pet->affected_by, fread_flag64 (fp));
      KEY("Alig", pet->alignment,   fread_number (fp));
      
      if (!str_cmp(word,"ACs"))
      {
        int i;
        
        for (i = 0; i < 4; i++) pet->armor[i] = fread_number(fp);

        fMatch = TRUE;
        break;
      }
      
      if (!str_cmp (word,"Affc"))
      {
        AFFECT_DATA *paf;
        int sn;

        paf = new_affect();

        sn = skill_lookup(fread_word(fp));
        if (sn < 0) bug("Fread_char: unknown skill.", 0);
        else        paf->type = sn;

        paf->where     = fread_number(fp);
        paf->level     = fread_number(fp);
        paf->duration  = fread_number(fp);
        paf->modifier  = fread_number(fp);
        paf->location  = fread_number(fp);
        paf->bitvector = fread_number64(fp);
        paf->next      = pet->affected;
        pet->affected  = paf;
        fMatch         = TRUE;
        break;
      }
       
      if (!str_cmp (word, "AMod"))
      {
        int stat;
        
        for (stat = 0; stat < MAX_STATS; stat++)
            pet->mod_stat[stat] = fread_number (fp);
        fMatch = TRUE;
        break;
      }
       
      if (!str_cmp(word,"Attr"))
      {
         int stat;
         
         for (stat = 0; stat < MAX_STATS; stat++)
             pet->perm_stat[stat] = fread_number (fp);

         fMatch = TRUE;
         break;
      }
      break;
       
      case 'C':
      KEY("Clan", pet->clan, clan_lookup(fread_string (fp)));
      KEY("Comm", pet->comm, fread_flag64 (fp)); 
      break;
       
      case 'D':
      KEY("Dam", pet->damroll,     fread_number (fp));
      KEY("Desc",pet->description, fread_string (fp));
      break;
       
      case 'E':
      if (!str_cmp(word,"End"))
      {
        pet->leader = ch ;
        pet->master = ch ;
        ch->pet     = pet;

        /* adjust hp mana move up  -- here for speed's sake */
        percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

        if (percent > 0 && !IS_AFFECTED (ch, AFF_POISON) &&
            !IS_AFFECTED (ch, AFF_PLAGUE))
        {
          percent    = UMIN(percent,100);
          pet->hit  += (pet->max_hit - pet->hit)   * percent / 100;
          pet->mana += (pet->max_mana - pet->mana) * percent / 100;
          pet->move += (pet->max_move - pet->move) * percent / 100;
        }
        return;
      }
      KEY("Exp", pet->exp,  fread_number(fp));
      break;
       
      case 'G':
      KEY("Gold", pet->gold, fread_number64(fp));
      break;
       
      case 'H':
      KEY("Hit", pet->hitroll, fread_number(fp));
       
      if (!str_cmp(word,"HMV"))
      {
        pet->hit      = fread_number (fp);
        pet->max_hit  = fread_number (fp);
        pet->mana     = fread_number (fp);
        pet->max_mana = fread_number (fp);
        pet->move     = fread_number (fp);
        pet->max_move = fread_number (fp);
        fMatch = TRUE;
        break;
      }
      break;
       
      case 'L':
      KEY("Levl", pet->level,      fread_number (fp));
      KEY("LnD",  pet->long_descr, fread_string (fp));
      KEY("LogO", lastlogoff,      fread_number (fp));
      break;
       
      case 'N':
      KEY("Name", pet->name,       fread_string (fp));
      break;
       
      case 'P':
      KEY("Pos",  pet->position,   fread_number (fp));
      break;
       
      case 'R':
      KEY("Race",  pet->race,       race_lookup(fread_string (fp)));
      break;
      
      case 'S' :
      KEY("Save",  pet->saving_throw, fread_number (fp));
      KEY("Sex",   pet->sex,          fread_number (fp));
      KEY("ShD",   pet->short_descr,  fread_string (fp));
      KEY("Silv",  pet->silver,       fread_number64 (fp));
      break;
      
      if (!fMatch)
      {
        bug ("Fread_pet: no match.", 0);
        fread_to_eol (fp);
      }
    }
  }
}

extern OBJ_DATA * obj_free;

void fread_obj (CHAR_DATA * ch, FILE * fp, bool type)
{
  OBJ_DATA * obj;
  const char * word;
  int  iNest ;
  bool fMatch;
  bool fNest ;
  bool fVnum ;
  bool first ;
  bool new_format;  // to prevent errors
  bool make_new  ;  // update object
  
  fVnum      = FALSE;
  obj        = NULL ;
  first      = TRUE ;  // used to counter fp offset
  new_format = FALSE;
  make_new   = FALSE;

  word = feof (fp) ? "End" : fread_word (fp);

  if (!str_cmp (word, "Vnum"))
  {
      int64 vnum;
      first = FALSE; // fp will be in right place

      vnum = fread_number64 (fp);
      if (get_obj_index (vnum) == NULL)
      {
          bug ("Fread_obj: bad vnum %u.", vnum);
      }
      else
      {
          obj = create_object (get_obj_index(vnum), -1);
          new_format = TRUE;
      }
          
  }

  if (obj == NULL) // either not found or old style
  {
      obj = new_obj ();
      obj->name        = str_dup ("");
      obj->short_descr = str_dup ("");
      obj->description = str_dup ("");
  }

  fNest = FALSE;
  fVnum = TRUE ;
  iNest = 0;

  for (;;)
  {
    if (first) first = FALSE;
    else       word  = feof (fp) ? "END" : fread_word (fp);

    fMatch = FALSE;

    switch (UPPER(word[0]))
    {
      case '*':
      fMatch = TRUE;
      fread_to_eol (fp);
      break;

      case 'A':
      if (!str_cmp (word, "Affc"))
      {
        AFFECT_DATA * paf;
        int sn;

        paf = new_affect ();
        sn  = skill_lookup(fread_word (fp));

        if (sn < 0) bug ("Fread_obj: unknown skill.", 0);
        else paf->type = sn;

        paf->where     = fread_number (fp);
        paf->level     = fread_number (fp);
        paf->duration  = fread_number (fp);
        paf->modifier  = fread_number (fp);
        paf->location  = fread_number (fp);
        paf->bitvector = fread_number64 (fp);
        paf->next      = obj->affected;
        obj->affected  = paf ;
        fMatch         = TRUE;
        break;
      }
      break;

      case 'C':
      KEY("Cond", obj->condition, fread_number (fp));
      KEY("Cost", obj->cost,      fread_number64 (fp));
      break;

      case 'D':
      KEY("Desc", obj->description, fread_string (fp));
      KEY("Dura", obj->durability, fread_number(fp));
      break;

      case 'E':
      KEY("ExtF", obj->extra_flags, fread_flag64 (fp));

      if (!str_cmp (word, "Enchanted"))
      {
        obj->enchanted = TRUE;
        fMatch         = TRUE;
        break;
      }

      if (!str_cmp (word, "ExtraDescr") || !str_cmp (word, "ExDe"))
      {
        EXTRA_DESCR_DATA * ed;

        ed = new_extra_descr ();

        ed->keyword      = fread_string (fp);
        ed->description  = fread_string (fp);
        ed->next         = obj->extra_descr ;
        obj->extra_descr = ed;
        fMatch           = TRUE;
      }

      if (!str_cmp (word, "End"))
      {
        if (!fNest)
        {
          bug ("Fread_obj: incomplete object by Nest.", 0);
          free_string (obj->name       );
          free_string (obj->description);
          free_string (obj->short_descr);
          obj->next = obj_free;
          obj_free  = obj;
          return;
        }
        else if (fVnum && obj->pIndexData == NULL)
        {
          bug ("Fread_obj: incomplete object by pIndexData.", 0);
          free_string (obj->name       );
          free_string (obj->description);
          free_string (obj->short_descr);
          obj->next = obj_free;
          obj_free  = obj;
          return;
        }
        else
        {
            if (!fVnum)
            {
              free_string (obj->name       );
              free_string (obj->description);
              free_string (obj->short_descr);
              obj->next = obj_free;
              obj_free  = obj;
              obj = create_object (get_obj_index (OBJ_VNUM_DUMMY), 0);
            }

            if (!new_format)
            {
              obj->next   = object_list;
              object_list = obj;
              obj->pIndexData->count++;
            }

            if (!obj->pIndexData->new_format &&
              obj->item_type == ITEM_ARMOR &&
              obj->value[1]  == 0)
            {
              obj->value[1] = obj->value[0];
              obj->value[2] = obj->value[0];
            }

            if (make_new)
            {
              int wear;
              
              wear = obj->wear_loc;
              extract_obj (obj);
              obj = create_object (obj->pIndexData, 0);
              obj->wear_loc = wear;
            }

            if (!type)
            {
              obj_to_room(obj, ch->in_room);
              ch->on=obj;
            }
            else if (iNest == 0 || rgObjNest[iNest] == NULL)
                obj_to_char (obj, ch);
            else
                obj_to_obj (obj, rgObjNest[iNest-1]);
            return;
        }
      }

      break;

      case 'I':
      KEY("Ityp", obj->item_type, fread_number (fp));
      break;

      case 'L':
      KEY("Lev",       obj->level,     fread_number (fp));
      KEY("Loaded_by", obj->loaded_by, fread_string (fp));
      break;

      case 'M':
      KEY("Mater",     obj->material,  fread_string(fp));

      case 'N':
      KEY("Name", obj->name,      fread_string (fp));

      if (!str_cmp (word, "Nest"))
      {
        iNest = fread_number( fp );
        if (iNest < 0 || iNest >= MAX_NEST)
            bug ("Fread_obj: bad nest %d.", iNest);
        else
        {
          rgObjNest[iNest] = obj;
          fNest = TRUE;
        }
        fMatch = TRUE;
      }

      break;

      case 'O':
      if (!str_cmp  (word, "Oldstyle"))
      {                       
        if (obj->pIndexData != NULL && obj->pIndexData->new_format)
              make_new = TRUE;
        fMatch = TRUE;
      }
      KEY("Owner", obj->owner, fread_string (fp));
      break;


      case 'S':
      KEY("ShD",  obj->short_descr, fread_string (fp));

      if (!str_cmp (word, "Spell"))
      {
        int iValue;
        int sn;

        iValue = fread_number (fp);
        sn     = skill_lookup (fread_word (fp));

        if (iValue < 0 || iValue > 3)
            bug ("Fread_obj: bad iValue %d.", iValue);
        else if (sn < 0)
            bug ("Fread_obj: unknown skill.", 0);
        else obj->value[iValue] = sn;

        fMatch = TRUE ;
        break ;
      }

      break;

      case 'T':
      KEY("Time", obj->timer, fread_number (fp));

      break;

      case 'V':
      if (!str_cmp (word, "Val"))
      {
        obj->value[0] = fread_number64 (fp);
        obj->value[1] = fread_number64 (fp);
        obj->value[2] = fread_number64 (fp);
        obj->value[3] = fread_number64 (fp);
        obj->value[4] = fread_number64 (fp);
        if (obj->pIndexData!=NULL)
        { 
         if (obj->pIndexData->item_type==ITEM_LIGHT)
         {
          obj->value[0] = 0;
          obj->value[1] = 0;
          obj->value[3] = 0;
          obj->value[4] = 0;
         }
         if (obj->pIndexData->vnum==28)
         {
           obj->value[1]=-1;
           if (obj->value[2]!=0) obj->value[0]=-1;
         }
        }

        fMatch = TRUE;
        break;
      }

      if (!str_cmp (word, "Vnum"))
      {
        int64 vnum;

        vnum = fread_number64 (fp);

        if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
            bug ("Fread_obj: bad vnum %u.", vnum);
        else fVnum = TRUE;
        fMatch = TRUE;
        break;
      }
      break;

      case 'W':
      KEY("WeaF", obj->wear_flags, fread_flag64 (fp));
      KEY("Wear", obj->wear_loc,   fread_number (fp));
      KEY("Wt",   obj->weight,     fread_number (fp));

      break;
    }

    if (!fMatch)
    {
      bug ("Fread_obj: no match.", 0);
      fread_to_eol (fp);
    }
  }
  if (obj->item_type == ITEM_WEAPON)
  {
    if (obj->value[3] != obj->pIndexData->value[3])
    {
       bug("Fread_obj: weapon dam_type is non-equal to vnum dam_type, fixed.", 0);
       obj->value[3] = obj->pIndexData->value[3];
    };
  }
}

void save_clans ()
{
  FILE * fp;
  CLAN_DATA * clan;
  AFFECT_DATA *paf;
  int count = 0, cnt;

  fclose (fpReserve);
  if ((fp = fopen (CLAN_FILE, "w")) == NULL)
  {
      bug ("Could not open " CLAN_FILE " for writing.", 0);
      return;
  }

  for (clan = clan_list; clan != NULL; clan=clan->next)
  {
      count++;
      do_fprintf (fp, "#CLAN\n");
      if (!EMPTY(clan->recalmsg1)) do_fprintf (fp, "Name %s~\n",clan->name);
      if (!EMPTY(clan->recalmsg1)) do_fprintf (fp, "Showname %s~\n",  clan->show_name);
      if (!EMPTY(clan->recalmsg1)) do_fprintf (fp, "Recalmsg1 %s~\n", clan->recalmsg1);
      if (!EMPTY(clan->recalmsg2)) do_fprintf (fp, "Recalmsg2 %s~\n", clan->recalmsg2);
      do_fprintf (fp, "Clanqp %d\n",     clan->qp);
      do_fprintf (fp, "Clangold %u\n",   clan->gold);
      do_fprintf (fp, "Clanrecal %u\n",  clan->clanrecal);
      do_fprintf (fp, "Clandeath %u\n",  clan->clandeath);
      do_fprintf (fp, "Flag %s\n", pflag64(clan->flag));
      if (!EMPTY(clan->war))  do_fprintf (fp, "War %s~\n",  clan->war);
      if (!EMPTY(clan->alli)) do_fprintf (fp, "Alli %s~\n", clan->alli);
      if (!EMPTY(clan->acceptalli)) do_fprintf (fp, "AcceptAlli %s~\n", clan->acceptalli);

      if (clan->wear_loc!=-1)
      {
        do_fprintf (fp, "Enchant %u\nShort %s~\nLong %s~\n",
         clan->wear_loc,clan->short_desc,clan->long_desc);
        for(paf=clan->mod;paf!=NULL;paf=paf->next)
        {
         if (paf->type < 0 || paf->type >= max_skill) continue;
         do_fprintf (fp, "Affc '%s' %d %d %d %d %d %u\n",
                    skill_table[paf->type].name, paf->where,
                    paf->level, paf->duration, paf->modifier,
                    paf->location, paf->bitvector);
        }
      }
      for (cnt=0;cnt<20;cnt++)
      {
        if (clan->clansn[cnt]==0) break;
        do_fprintf(fp,"ClanSkill %d %u\n",clan->clansn[cnt],clan->clansnt[cnt]);
      }
      do_fprintf (fp, "\n");
  }
  do_fprintf (fp, "#END\n");
  do_fprintf (fp, "; Currently %d clans in Forgotten Dungeon\n", count);

  fclose  (fp);
  fpReserve = fopen (NULL_FILE, "r");
}

void save_config ()
{
  FILE * fp;
  int cmd;
  OFFER_DATA *offer;

  fclose (fpReserve);

  if ((fp = fopen (CONFIG_FILE, "w")) == NULL)
  {
      bug ("Could not open " CONFIG_FILE " for writing.", 0);
      return;
  }
  // Saving configuration
  do_fprintf(fp,"; Configuration file\n");
  do_fprintf(fp,"Rebootcounter %d\n", reboot_counter);

  if (IS_SET(global_cfg,CFG_LOCAL))
  {
    REM_BIT(global_cfg,CFG_LOCAL);
    do_fprintf(fp,"Global_config %s\n",pflag64(global_cfg));
    SET_BIT(global_cfg,CFG_LOCAL);
    do_fprintf(fp,"LocalMode\n");
  }
  else do_fprintf(fp,"Global_config %s\n",pflag64(global_cfg));

  do_fprintf(fp,"Autologin %d %d\n", autologin_code, autologin_pass);
  do_fprintf(fp,"FILE_WHO %s~\n",fname_who);
  do_fprintf(fp,"FILE_ONLINE %s~\n",fname_online);

#ifdef WITH_DSO
  {
        struct module *mod;
//      store modules at the beginning
        do_fprintf(fp, "; Load following modules at startup time.\n");
        MODS_FOREACH(mod)
                if(mod->md_fname)
                        do_fprintf(fp, "Module %s~\n", mod->md_fname);
  }
#endif

  do_fprintf(fp,"; Birthday phrazes (emote)\n");
  for (cmd=0;cmd<10;cmd++) do_fprintf(fp,"BD%d %s~\n",cmd,bdmsg[cmd]);

  do_fprintf(fp,"; Disabled commands:\n");
  // Saving disabled commands
#ifdef WITH_DSO
          /*
           * So, how do I disable commands, which not loaded (yet)?
           */
#endif
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
    if (IS_SET(cmd_table[cmd].flag, DENY))
      do_fprintf(fp, "Deny %s\n", cmd_table[cmd].name);
  }

  // Saving guilds
  do_fprintf(fp,"; Guilds data:\n");
  save_guild(fp);
  // Saving offers
  do_fprintf(fp,"; Offers data:\n");
  for (offer=offer_list;offer;offer=offer->next)
  {
    do_fprintf(fp,"Offer\n%s~\n%s~\n%u\n\n",offer->name,offer->customer,offer->gold);
  }
  do_fprintf (fp, "#END\n");

  fclose (fp);
  fpReserve = fopen (NULL_FILE, "r");
}

void save_one_char( CHAR_DATA *ch ,int action)
{
  char strsave [MAX_INPUT_LENGTH];
  char buf [200];
  FILE * fp;

  if (!ch || IS_NPC (ch)) return;

  switch (action)
  {
    case SAVE_BACKUP:
      do_printf (strsave, "%s%s", PLAYER_DIR2, capitalize (ch->name));
      break;
    case SAVE_BACKUP2:
      do_printf (strsave, "%s%s", PLAYER_DIR3, capitalize (ch->name));
      break;
    case SAVE_DELETED:
      do_printf (strsave, "%s%s", DELETED_DIR, capitalize (ch->name));
      break;
    default:
      do_printf (strsave, "%s%s", PLAYER_DIR, capitalize (ch->name));
      break;
  }

  fclose (fpReserve);

  if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
  {
    bug( "Save_one_char: fopen", 0 );
    perror( strsave );
  }
  else
  {
    REM_BIT (ch->comm, COMM_WILLSAVE);
    do_printf (buf, "Saving: %s,", ch->name);
    fwrite_char (ch, fp);

    if (ch->carrying != NULL )
    {
      fwrite_obj (ch, ch->carrying, fp, 0, TRUE );
      strcat (buf, " Objects: yes");
    }

    if (ch->on!=NULL)
    {
     CHAR_DATA *victim;
     OBJ_DATA *obj_on;

     obj_on=ch->on;

     for (victim=ch->in_room->people;victim!=NULL;victim=victim->next_in_room)
       if(!IS_NPC(victim) && victim!=ch && victim->on==obj_on) obj_on=NULL;
      
     if (obj_on && CAN_WEAR(ch->on,ITEM_TAKE)) fwrite_obj (ch, ch->on, fp, 0, FALSE );
    }

    if (ch->pet != NULL && ch->pet->in_room == ch->in_room)
    {
      fwrite_pet (ch->pet, fp);
      strcat (buf," Pet: yes");
    }
    do_fprintf (fp, "#END\n");

//    log_string (buf);
#if defined (WIN32)
    if (unlink(strsave)== -1) log_printf ("BUG:Can't rename TEMP to %s",strsave);
#endif
    fclose  (fp);
    fpReserve = fopen (NULL_FILE, "r");
    rename(TEMP_FILE,strsave);
  }
}

void save_vote()
{
  FILE * fp;
  VOTE_DATA *vote;
  VOTE_CHAR *vch;

  fclose (fpReserve);

  if ((fp = fopen (VOTE_FILE, "w")) == NULL)
  {
      bug ("Could not open " VOTE_FILE " for writing.", 0);
      return;
  }

  do_fprintf (fp, "; Vote file\n");

  for (vote=vote_list;vote;vote=vote->next)
  {
    do_fprintf(fp,"#VOTE\nMsg %s~\n",vote->msg);
    if (vote->owner[0]!=str_empty[0]) do_fprintf(fp,"Owner %s~\n",vote->owner);

    for (vch=vote->chars;vch;vch=vch->next)
    {
      if (vch->voting==0) continue;
      do_fprintf(fp,"CharData %s~\n%d %d %d\n",vch->name,vch->age,vch->level,vch->voting);
    }

    do_fprintf(fp,"END\n\n");
  }
  do_fprintf(fp,"#END\n");
  fclose  (fp);
  fpReserve = fopen (NULL_FILE, "r");
}

void save_races()
{
  FILE * fp;
  int counter=0,i,race;

  fclose (fpReserve);
  if ((fp = fopen (RACE_FILE, "w")) == NULL)
  {
    bug ("Could not open " RACE_FILE " for writing.", 0);
    return;
  }
 
  do_fprintf (fp, "; Races data version 2\n");
  for (race=0;;race++)
  {
    if (!race_table[race].name) break;
    do_fprintf(fp,"#Race\n");
    do_fprintf(fp,"Name %s~\n",race_table[race].name);
    do_fprintf(fp,"WName %s~\n",race_table[race].who_name);
    do_fprintf(fp,"Pc_race %s~\n",race_table[race].pc_race ? "TRUE":"FALSE");
    do_fprintf(fp,"Points %d\n",race_table[race].points);
    do_fprintf(fp,"Class %d %d %d %d\n",race_table[race].class_mult[0],
       race_table[race].class_mult[1],race_table[race].class_mult[2],race_table[race].class_mult[3]);
    do_fprintf(fp,"Act %s\n",pflag64(race_table[race].act));
    do_fprintf(fp,"Aff %s\n",pflag64(race_table[race].aff));
    do_fprintf(fp,"Off %s\n",pflag64(race_table[race].off));
    do_fprintf(fp,"Imm %s\n",pflag64(race_table[race].imm));
    do_fprintf(fp,"Res %s\n",pflag64(race_table[race].res));
    do_fprintf(fp,"Vuln %s\n",pflag64(race_table[race].vuln));
    do_fprintf(fp,"Form %s\n",pflag64(race_table[race].form));
    do_fprintf(fp,"Spec %s\n",pflag64(race_table[race].spec));
    do_fprintf(fp,"Size %d\n",race_table[race].size);
    do_fprintf(fp,"Cbonus %s\n",pflag64(race_table[race].c_bon));
    do_fprintf(fp,"Cpenalty %s\n",pflag64(race_table[race].c_pen));
    do_fprintf(fp,"Stats %d %d %d %d %d\n",
           race_table[race].stats[0],race_table[race].stats[1],race_table[race].stats[2],
           race_table[race].stats[3],race_table[race].stats[4]);
    do_fprintf(fp,"Mstats %d %d %d %d %d\n",
           race_table[race].max_stats[0],race_table[race].max_stats[1],race_table[race].max_stats[2],
           race_table[race].max_stats[3],race_table[race].max_stats[4]);
    do_fprintf(fp,"Hstats %d %d %d %d %d\n",
           race_table[race].high_stats[0],race_table[race].high_stats[1],race_table[race].high_stats[2],
           race_table[race].high_stats[3],race_table[race].high_stats[4]);
    do_fprintf(fp,"Sk1 '%s'\n",race_table[race].skills[0]);
    do_fprintf(fp,"Sk2 '%s'\n",race_table[race].skills[1]);
    do_fprintf(fp,"Sk3 '%s'\n",race_table[race].skills[2]);
    do_fprintf(fp,"Sk4 '%s'\n",race_table[race].skills[3]);
    do_fprintf(fp,"Sk5 '%s'\n",race_table[race].skills[4]);
    do_fprintf(fp,"Dambonus");
    for (i=0;i<DAM_MAX;i++) do_fprintf(fp," %d",race_table[race].dambonus[i]);
    do_fprintf(fp,"\n");
    do_fprintf(fp,"Wbonus");
    for (i=0;i<WEAPON_MAX;i++) do_fprintf(fp," %d",race_table[race].weapon_bonus[i]);
    do_fprintf(fp,"\n");
    do_fprintf(fp,"Hand %s\n",race_table[race].hand_dam);
    do_fprintf(fp,"Wealth %d\n",race_table[race].wealth);
    do_fprintf(fp,"END\n");
    counter++;
  }
  do_fprintf(fp,"#END\n");
  do_fprintf(fp,"; Saved %d races\n",counter);
  fclose  (fp);
  fpReserve = fopen (NULL_FILE, "r");
}

void save_newspaper()
{
  FILE *fp;
  NEWS_DATA *news;
  int counter=0;

  fclose( fpReserve );
  if ( ( fp = fopen( NEWSPAPER_FILE, "w" ) ) == NULL ) perror( NEWSPAPER_FILE );
  else
  {
    for (news=newspaper_list ; news != NULL; news = news->next )
    {
      counter++;
      do_fprintf( fp, "Type %d\n",  news->type);
      do_fprintf( fp, "Date %s~\n", news->date);
      do_fprintf( fp, "Stamp %l\n",  news->date_stamp);
      do_fprintf( fp, "Text\n%s~\n",   news->text);
      if (counter>500) break;
    }
    do_fprintf( fp, "#END\n");
    do_fprintf( fp, "; Currently %d news\n",counter);
    fclose( fp );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

