// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "merc.h"
#include "db.h"
#include "tables.h"
#include "recycle.h"
#include "fdweb.h"

CLAN_DATA *new_clan();
void load_one_news(FILE *fp);

void load_mobiles( FILE *fp )
{
  MOB_INDEX_DATA *pMobIndex;

  if ( !area_last )
  {
    bug( "Load_mobiles: no #AREA seen yet.", 0 );
    exit( 1 );
  }

  for (;;)
  {
    int vnum;
    char letter;
    int iHash;

    letter = fread_letter( fp );
    if ( letter != '#' )
    {
      bug( "Load_mobiles: # not found.", 0 );
      exit( 1 );
    }

    vnum = fread_number( fp );
    if ( vnum == 0 ) break;

    fBootDb = FALSE;
    if ( get_mob_index( vnum ) != NULL )
    {
      bug( "Load_mobiles: vnum %u duplicated.", vnum );
      exit( 1 );
    }
    fBootDb = TRUE;

    pMobIndex               = alloc_perm( sizeof(*pMobIndex) );
    pMobIndex->vnum         = vnum;
    pMobIndex->area         = area_last;
    newmobs++;
    pMobIndex->player_name  = fread_string( fp );
    pMobIndex->short_descr  = fread_string( fp );
    pMobIndex->long_descr   = fread_string( fp );
    pMobIndex->description  = fread_string( fp );
    pMobIndex->race         = race_lookup(fread_string( fp ));
    ((char *)pMobIndex->long_descr)[0]  = UPPER(pMobIndex->long_descr[0]);
    ((char *)pMobIndex->description)[0] = UPPER(pMobIndex->description[0]);

    pMobIndex->act            = fread_flag64( fp ) | ACT_IS_NPC
                              | race_table[pMobIndex->race].act;

    if (pMobIndex->area->version <3 )
       REM_BIT(pMobIndex->act,ACT_FORGER);        // remove old usage of flag

    pMobIndex->affected_by    = fread_flag64( fp )
                              | race_table[pMobIndex->race].aff;
    if (IS_SET(pMobIndex->affected_by,AFF_MIST) && pMobIndex->race!=RACE_VAMPIRE)
    REM_BIT(pMobIndex->affected_by,AFF_MIST);

    pMobIndex->pShop          = NULL;
    pMobIndex->alignment      = fread_number( fp );
    pMobIndex->group          = fread_number( fp );

    pMobIndex->level          = fread_number( fp );
    pMobIndex->hitroll        = fread_number( fp );

    /* read hit dice */
    pMobIndex->hit[DICE_NUMBER]     = fread_number( fp );  
    /* 'd'          */                fread_letter( fp ); 
    pMobIndex->hit[DICE_TYPE]       = fread_number( fp );
    /* '+'          */                fread_letter( fp );   
    pMobIndex->hit[DICE_BONUS]      = fread_number( fp ); 

    /* read mana dice */
    pMobIndex->mana[DICE_NUMBER]    = fread_number( fp );
                                      fread_letter( fp );
    pMobIndex->mana[DICE_TYPE]      = fread_number( fp );
                                      fread_letter( fp );
    pMobIndex->mana[DICE_BONUS]     = fread_number( fp );

    /* read damage dice */
    pMobIndex->damage[DICE_NUMBER]  = fread_number( fp );
                                      fread_letter( fp );
    pMobIndex->damage[DICE_TYPE]    = fread_number( fp );
                                      fread_letter( fp );
    pMobIndex->damage[DICE_BONUS]   = fread_number( fp );
    pMobIndex->dam_type             = attack_lookup(fread_word(fp));

    /* read armor class */
    pMobIndex->ac[AC_PIERCE]        = fread_number( fp ) * 10;
    pMobIndex->ac[AC_BASH]          = fread_number( fp ) * 10;
    pMobIndex->ac[AC_SLASH]         = fread_number( fp ) * 10;
    pMobIndex->ac[AC_EXOTIC]        = fread_number( fp ) * 10;

    // read flags and add in data from the race table
    pMobIndex->off_flags            = fread_flag64( fp ) 
                                    | race_table[pMobIndex->race].off;
    pMobIndex->imm_flags            = fread_flag64( fp )
                                    | race_table[pMobIndex->race].imm;
    pMobIndex->res_flags            = fread_flag64( fp )
                                    | race_table[pMobIndex->race].res;
    pMobIndex->vuln_flags           = fread_flag64( fp )
                                    | race_table[pMobIndex->race].vuln;

    /* vital statistics */
    pMobIndex->start_pos            = position_lookup(fread_word(fp));
    pMobIndex->default_pos          = position_lookup(fread_word(fp));
    pMobIndex->sex                  = sex_lookup(fread_word(fp));

    pMobIndex->wealth               = fread_number( fp );

    pMobIndex->form                 = fread_flag64( fp )
                                    | race_table[pMobIndex->race].form;
    if (pMobIndex->area->version <2 ) fread_flag64( fp ); // was mobile parts
    pMobIndex->size                 = size_lookup(fread_word(fp));
    if (pMobIndex->area->version <2 ) fread_word( fp ); // was mobile material

    for ( ; ; )
    {
      letter = fread_letter( fp );

      if (letter == 'F')
      {
        const char *word;
        int64 vector;

        word   = fread_word(fp);
        vector = fread_flag64(fp);

        if (!str_prefix(word,"act")) REM_BIT(pMobIndex->act,vector);
        else if (!str_prefix(word,"aff")) REM_BIT(pMobIndex->affected_by,vector);
        else if (!str_prefix(word,"off")) REM_BIT(pMobIndex->off_flags,vector);
        else if (!str_prefix(word,"imm")) REM_BIT(pMobIndex->imm_flags,vector);
        else if (!str_prefix(word,"res")) REM_BIT(pMobIndex->res_flags,vector);
        else if (!str_prefix(word,"vul")) REM_BIT(pMobIndex->vuln_flags,vector);
        else if (!str_prefix(word,"for")) REM_BIT(pMobIndex->form,vector);
        else
        {
          bug("Flag remove: flag not found.",0);
          exit(1);
        }
      }
      else if ( letter == 'M' )
      {
        MPROG_LIST *pMprog;
        const char *word;
        int64 trigger = 0;
          
        pMprog  = alloc_perm(sizeof(*pMprog));
        word    = fread_word( fp );
        if ( !(trigger = flag_lookup( word, mprog_flags )) )
        {
          bug("MOBprogs: invalid trigger.",0);
          exit(1);
        }
        SET_BIT( pMobIndex->mprog_flags, trigger );
        pMprog->trig_type   = trigger;
        pMprog->vnum        = fread_number( fp );
        pMprog->trig_phrase = fread_string( fp );
        pMprog->next        = pMobIndex->mprogs;
        pMobIndex->mprogs   = pMprog;
      }
      else
      {
        ungetc(letter,fp);
        break;
      }
    }
    iHash                 = vnum % MAX_KEY_HASH;
    pMobIndex->next       = mob_index_hash[iHash];
    mob_index_hash[iHash] = pMobIndex;
    top_mob_index++;
    top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
    assign_area_vnum( vnum );
    kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
  }
}

// Snarf an obj section. new style
void load_objects( FILE *fp )
{
  OBJ_INDEX_DATA *pObjIndex;
  const char *pOIT;

  if ( !area_last )
  {
    bug( "Load_objects: no #AREA seen yet.", 0 );
    exit( 1 );
  }

  for ( ; ; )
  {
    int64 vnum;
    char letter;
    int64 iHash;

    letter = fread_letter( fp );
    if ( letter != '#' )
    {
      bug( "Load_objects: # not found.", 0 );
      exit( 1 );
    }

    vnum = fread_number64( fp );
    if ( vnum == 0 ) break;

    fBootDb = FALSE;
    if ( get_obj_index( vnum ) != NULL )
    {
      bug( "Load_objects: vnum %u duplicated.", vnum );
      exit( 1 );
    }
    fBootDb = TRUE;

    pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
    pObjIndex->vnum                 = vnum;
    pObjIndex->area                 = area_last;
    pObjIndex->new_format           = TRUE;
    pObjIndex->reset_num            = 0;
    newobjs++;
    pObjIndex->name                 = fread_string( fp );
    pObjIndex->short_descr          = fread_string( fp );
    pObjIndex->description          = fread_string( fp );

    pObjIndex->material=fread_string(fp);
    if( !pObjIndex->material ) 
        pObjIndex->material = material_table[MATERIAL_NONE].name;

    pObjIndex->item_type            = item_lookup(fread_word( fp ));
    pObjIndex->extra_flags          = fread_flag64( fp );
    pObjIndex->wear_flags           = fread_flag64( fp );
    switch(pObjIndex->item_type)
    {
    case ITEM_WEAPON:
        pObjIndex->value[0]         = weapon_type(fread_word(fp));
        pObjIndex->value[1]         = fread_number64(fp);
        pObjIndex->value[2]         = fread_number64(fp);
        pObjIndex->value[3]         = attack_lookup(fread_word(fp));
        pObjIndex->value[4]         = fread_flag64(fp);
        break;
    case ITEM_CONTAINER:
        pObjIndex->value[0]         = fread_number64(fp);
        pObjIndex->value[1]         = fread_flag64(fp);
        pObjIndex->value[2]         = fread_number64(fp);
        pObjIndex->value[3]         = fread_number64(fp);
        pObjIndex->value[4]         = fread_number64(fp);
        if (IS_SET(pObjIndex->value[1],CONT_CLOSED)) SET_BIT(pObjIndex->value[1],CONT_CLOSEABLE);
        break;
    case ITEM_DRINK_CON:
    case ITEM_FOUNTAIN:
        pObjIndex->value[0]         = fread_number64(fp);
        pObjIndex->value[1]         = fread_number64(fp);
        pObjIndex->value[2]         = liq_lookup(fread_word(fp));
        pObjIndex->value[3]         = fread_number64(fp);
        pObjIndex->value[4]         = fread_number64(fp);
        break;
    case ITEM_WAND:
    case ITEM_STAFF:
        pObjIndex->value[0]         = fread_number64(fp);
        pObjIndex->value[1]         = fread_number64(fp);
        pObjIndex->value[2]         = fread_number64(fp);
        pObjIndex->value[3]         = skill_lookup(fread_word(fp));
        pObjIndex->value[4]         = fread_number64(fp);
        break;
    case ITEM_POTION:
    case ITEM_PILL:
    case ITEM_SCROLL:
        pObjIndex->value[0]         = fread_number64(fp);
        pObjIndex->value[1]         = skill_lookup(fread_word(fp));
        pObjIndex->value[2]         = skill_lookup(fread_word(fp));
        pObjIndex->value[3]         = skill_lookup(fread_word(fp));
        pObjIndex->value[4]         = skill_lookup(fread_word(fp));
        break;
    case ITEM_LIGHT:
        pObjIndex->value[0]         = fread_number64(fp);
        pObjIndex->value[1]         = fread_number64(fp);
        pObjIndex->value[2]         = fread_number64(fp);
        pObjIndex->value[3]         = fread_number64(fp);
        pObjIndex->value[4]         = fread_number64(fp);
        pObjIndex->value[0]=0;
        pObjIndex->value[1]=0;
        pObjIndex->value[3]=0;
        pObjIndex->value[4]=0;
        break;
    
    default:
        pObjIndex->value[0]             = fread_flag64( fp );
        pObjIndex->value[1]             = fread_flag64( fp );
        pObjIndex->value[2]             = fread_flag64( fp );
        pObjIndex->value[3]             = fread_flag64( fp );
        pObjIndex->value[4]             = fread_flag64( fp );
        break;
    }
    pObjIndex->level                = fread_number( fp );
    pObjIndex->weight               = fread_number( fp );
    pObjIndex->cost                 = fread_number64( fp ); 

    pOIT=fread_word( fp );

    if( is_number(pOIT) ) 
    {
      pObjIndex->durability = atoi(pOIT);
      letter = fread_letter( fp );
    }
    else if( (atoi(pOIT) < -1) || atoi(pOIT) == 0 );
    else
    {
      pObjIndex->durability = material_table[material_num(pObjIndex->material)].d_dam;
      letter = pOIT[0];
    }

      switch (letter)
      {
        case ('P') : pObjIndex->condition = 1000; break;
        case ('G') : pObjIndex->condition = 900; break;
        case ('A') : pObjIndex->condition = 750; break;
        case ('W') : pObjIndex->condition = 500; break;
        case ('D') : pObjIndex->condition = 250; break;
        case ('B') : pObjIndex->condition = 100; break;
        case ('R') : pObjIndex->condition = 0; break;
        default:     pObjIndex->condition = 200; break;
      }

    if( (pObjIndex->condition > pObjIndex->durability) && pObjIndex->durability != 0)
       pObjIndex->condition = pObjIndex->durability;

    for ( ; ; )
    {
      char letter;

      letter = fread_letter( fp );

      if ( letter == 'A' )
      {
        AFFECT_DATA *paf;

        paf                     = alloc_perm( sizeof(*paf) );
        paf->where              = TO_OBJECT;
        paf->type               = -1;
        paf->level              = pObjIndex->level;
        paf->duration           = -1;
        paf->location           = fread_number( fp );
        if (paf->location == APPLY_SPELL_AFFECT) paf->type = skill_lookup("spellaffect");
        paf->modifier           = fread_number( fp );
        paf->bitvector          = 0;
        paf->next               = pObjIndex->affected;
        pObjIndex->affected     = paf;
        top_affect++;
      }

      else if (letter == 'F')
      {
        AFFECT_DATA *paf;

        paf                     = alloc_perm( sizeof(*paf) );
        letter                  = fread_letter(fp);
        switch (letter)
        {
        case 'A':
            paf->where          = TO_AFFECTS;
            break;
        case 'I':
            paf->where          = TO_IMMUNE;
            break;
        case 'R':
            paf->where          = TO_RESIST;
            break;
        case 'V':
            paf->where          = TO_VULN;
            break;
        default:
            bug( "Load_objects: Bad where on flag set.", 0 );
           exit( 1 );
        }
        paf->type               = -1;
        paf->level              = pObjIndex->level;
        paf->duration           = -1;
        paf->location           = fread_number(fp);
        paf->modifier           = fread_number(fp);
        paf->bitvector          = fread_flag64(fp);
        paf->next               = pObjIndex->affected;
        pObjIndex->affected     = paf;
        top_affect++;
      }

      else if ( letter == 'E' )
      {
        EXTRA_DESCR_DATA *ed;

        ed                      = alloc_perm( sizeof(*ed) );
        ed->keyword             = fread_string( fp );
        ed->description         = fread_string( fp );
        ed->next                = pObjIndex->extra_descr;
        pObjIndex->extra_descr  = ed;
        top_ed++;
      }

      else if ( letter == 'T' )
      {
        pObjIndex->ttype = fread_flag64(fp);
        pObjIndex->ttime = fread_number(fp);
      }

      else
      {
          ungetc( letter, fp );
          break;
      }
    }

    iHash                   = vnum % MAX_KEY_HASH;
    pObjIndex->next         = obj_index_hash[iHash];
    obj_index_hash[iHash]   = pObjIndex;
    top_obj_index++;
    top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;   /* OLC */
    assign_area_vnum( vnum );                                   /* OLC */
  }
}

// Name:          convert_objects
//   Purpose:       Converts all old format objects to new format
//   Called by:     boot_db (db.c).
//   Note:          Loops over all resets to find the level of the mob
//                  loaded before the object to determine the level of
//                  the object.
//                  It might be better to update the levels in load_resets().
//                  This function is not pretty.. Sorry about that :)
//   Author:        Hugin
void convert_objects( void )
{
  int64 vnum;
  AREA_DATA  *pArea;
  RESET_DATA *pReset;
  MOB_INDEX_DATA *pMob = NULL;
  OBJ_INDEX_DATA *pObj;
  ROOM_INDEX_DATA *pRoom;

  if ( newobjs == top_obj_index ) return;

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
      if ( !( pRoom = get_room_index( vnum ) ) ) continue;

      for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
      {
        switch ( pReset->command )
        {
          case 'M':
            if ( !( pMob = get_mob_index( pReset->arg1 ) ) )
              bug( "Convert_objects: 'M': bad vnum %u.", pReset->arg1 );
            break;
          case 'O':
            if ( !( pObj = get_obj_index( pReset->arg1 ) ) )
            {
              bug( "Convert_objects: 'O': bad vnum %u.", pReset->arg1 );
              break;
            }
            if ( pObj->new_format ) continue;
            if ( !pMob )
            {
              bug( "Convert_objects: 'O': No mob reset yet.", 0 );
              break;
            }
            pObj->level = pObj->level < 1 ? pMob->level - 2
              : UMIN(pObj->level, pMob->level - 2);
            break;
          case 'P':
            {
              OBJ_INDEX_DATA *pObj, *pObjTo;

              if ( !( pObj = get_obj_index( pReset->arg1 ) ) )
              {
                bug( "Convert_objects: 'P': bad vnum %u.", pReset->arg1 );
                break;
              }
              if ( pObj->new_format ) continue;
              if ( !( pObjTo = get_obj_index( pReset->arg3 ) ) )
              {
                bug( "Convert_objects: 'P': bad vnum %u.", pReset->arg3 );
                break;
              }
              pObj->level = pObj->level < 1 ? pObjTo->level
                : UMIN(pObj->level, pObjTo->level);
            }
            break;
          case 'G':
          case 'E':
            if ( !( pObj = get_obj_index( pReset->arg1 ) ) )
            {
              bug( "Convert_objects: 'E' or 'G': bad vnum %u.", pReset->arg1 );
              break;
            }
            if ( !pMob )
            {
              bug( "Convert_objects: 'E' or 'G': null mob for vnum %u.",pReset->arg1 );
              break;
            }
            if ( pObj->new_format ) continue;
            if ( pMob->pShop )
            {
              switch ( pObj->item_type )
              {
                default:
                  pObj->level = UMAX(0, pObj->level);
                  break;
                case ITEM_PILL:
                case ITEM_POTION:
                  pObj->level = UMAX(5, pObj->level);
                  break;
                case ITEM_SCROLL:
                case ITEM_ARMOR:
                case ITEM_WEAPON:
                  pObj->level = UMAX(10, pObj->level);
                  break;
                case ITEM_WAND:
                case ITEM_TREASURE:
                  pObj->level = UMAX(15, pObj->level);
                  break;
                case ITEM_STAFF:
                  pObj->level = UMAX(20, pObj->level);
                  break;
              }
            }
            else pObj->level = pObj->level < 1 ? pMob->level
                  : UMIN( pObj->level, pMob->level );
            break;
          } /* switch ( pReset->command ) */
        }
      }
    }
    for ( pArea = area_first; pArea ; pArea = pArea->next )
      for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        if ( (pObj = get_obj_index( vnum )) )
          if ( !pObj->new_format ) convert_object( pObj );
    return;
}

// Name:          convert_object
// Purpose:       Converts an old_format obj to new_format
// Called by:     convert_objects (db2.c).
// Note:          Dug out of create_obj (db.c)
// Author:        Hugin
void convert_object( OBJ_INDEX_DATA *pObjIndex )
{
  int level;
  int number, type;  /* for dice-conversion */

  if ( !pObjIndex || pObjIndex->new_format ) return;

  level = pObjIndex->level;

  pObjIndex->level    = UMAX( 0, pObjIndex->level ); /* just to be sure */
  pObjIndex->cost     = 10*level;

  switch ( pObjIndex->item_type )
  {
      default:
          bug( "Obj_convert: vnum %d bad type.", pObjIndex->item_type );
          break;

      case ITEM_LIGHT:
      case ITEM_TREASURE:
      case ITEM_FURNITURE:
      case ITEM_TRASH:
      case ITEM_CONTAINER:
      case ITEM_DRINK_CON:
      case ITEM_KEY:
      case ITEM_FOOD:
      case ITEM_BOAT:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
      case ITEM_FOUNTAIN:
      case ITEM_MAP:
      case ITEM_CLOTHING:
      case ITEM_SCROLL:
          break;

      case ITEM_WAND:
      case ITEM_STAFF:
          pObjIndex->value[2] = pObjIndex->value[1];
          break;

      case ITEM_WEAPON:
          /*
           * The conversion below is based on the values generated
           * in one_hit() (fight.c).  Since I don't want a lvl 50 
           * weapon to do 15d3 damage, the min value will be below
           * the one in one_hit, and to make up for it, I've made 
           * the max value higher.
           * (I don't want 15d2 because this will hardly ever roll
           * 15 or 30, it will only roll damage close to 23.
           * I can't do 4d8+11, because one_hit there is no dice-
           * bounus value to set...)
           *
           * The conversion below gives:

           level:   dice      min      max      mean
             1:     1d8      1( 2)    8( 7)     5( 5)
             2:     2d5      2( 3)   10( 8)     6( 6)
             3:     2d5      2( 3)   10( 8)     6( 6)
             5:     2d6      2( 3)   12(10)     7( 7)
            10:     4d5      4( 5)   20(14)    12(10)
            20:     5d5      5( 7)   25(21)    15(14)
            30:     5d7      5(10)   35(29)    20(20)
            50:     5d11     5(15)   55(44)    30(30)

           */

          number = UMIN(level/4 + 1, 5);
          type   = (level + 7)/number;

          pObjIndex->value[1] = number;
          pObjIndex->value[2] = type;
          break;

      case ITEM_ARMOR:
          pObjIndex->value[0] = level / 5 + 3;
          pObjIndex->value[1] = pObjIndex->value[0];
          pObjIndex->value[2] = pObjIndex->value[0];
          break;

      case ITEM_POTION:
      case ITEM_PILL:
          break;

      case ITEM_MONEY:
          pObjIndex->value[0] = pObjIndex->cost;
          break;
  }

  pObjIndex->new_format = TRUE;
  ++newobjs;
}

// Read in a char.
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )  \
   if (!str_cmp(word,literal))        \
   {                                  \
     field  = value;                  \
     fMatch = TRUE;                   \
     break;                           \
   }

// provided to free strings
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value ) \
   if (!str_cmp(word,literal))        \
   {                                  \
     free_string(field);              \
     field  = value;                  \
     fMatch = TRUE;                   \
     break;                           \
   }


void load_clans()
{
  FILE *fp;
  const char *word;
  CLAN_DATA *clan=NULL;
  bool fMatch;
  int clans=0, clanskill=0;

  clan_list = NULL;

  fp=fopen(CLAN_FILE,"r");
  if (!fp)
  {
    log_string ("BUG: CLANFILE not found!");
    exit (1);
  }

  for ( ; ; )
  {
    fMatch = FALSE;
    if ( feof( fp ) )
    {
      fclose (fp);
      return;
    }

    word = fread_word(fp);

    switch ( word[0] )
    {
      case '*':
      case ';':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case '#':
        if (!strcmp (word, "#END"))
        {
          fclose (fp);
          log_printf("%d clans loaded",clans);
          return;
        }

        if (strcmp (word, "#CLAN")) break;

        clan = new_clan ();
        clan->next=clan_list;
        clan_list = clan;
        fread_to_eol(fp);
        clanskill=0;
        fMatch = TRUE;
        clans++;
        break;
  
      case 'A':
        KEYS( "AcceptAlli", clan->acceptalli,fread_string(fp));
        if (!str_cmp (word, "Affc"))
        {
         AFFECT_DATA * paf ;
         int sn;
         paf = new_affect () ;
         sn  = skill_lookup(fread_word (fp)) ;
         if (sn < 0) bug ("Fread_obj: unknown skill.", 0) ;
         else paf->type = sn ;
         paf->where      = fread_number (fp);
         paf->level      = fread_number (fp);
         paf->duration   = fread_number (fp);
         paf->modifier   = fread_number (fp);
         paf->location   = fread_number (fp);
         paf->bitvector  = fread_number64 (fp);
         paf->next       = clan->mod;
         clan->mod = paf;
         fMatch = TRUE;
         break;
        }
        KEYS( "Alli", clan->alli,fread_string(fp));
        break;

      case 'C':
        KEY( "Clanrecal", clan->clanrecal,fread_number64(fp));
        KEY( "Clandeath", clan->clandeath,fread_number64(fp));
        KEY( "Clanqp",    clan->qp,fread_number(fp));
        KEY( "Clangold",  clan->gold,fread_number64(fp));

        if (!str_cmp (word, "ClanSkill"))
        {
          clan->clansn[clanskill]=fread_number(fp);
          clan->clansnt[clanskill]=fread_number64(fp);
          clanskill++;
          fMatch = TRUE;
        }
        break;

      case 'E':
        KEY( "Enchant",clan->wear_loc,fread_number64(fp));
        break;

      case 'F':
        KEY("Flag", clan->flag, fread_flag64( fp ) ); 
        break;

      case 'I':
        if (!str_cmp(word, "Independent"))
        {
          fread_to_eol(fp);
          SET_BIT(clan->flag,CLAN_LONER); 
          fMatch=TRUE;
        }
        break;

      case 'L':
        KEYS( "Long", clan->long_desc,fread_string(fp));
        break;

      case 'N':
        KEY( "Name", clan->name,fread_string(fp));
        break;

      case 'R':
        KEYS( "Recalmsg1", clan->recalmsg1,fread_string(fp));
        KEYS( "Recalmsg2", clan->recalmsg2,fread_string(fp));
        break;

      case 'S':
         KEYS( "Short", clan->short_desc,fread_string(fp));
         KEYS( "Showname", clan->show_name,fread_string(fp));
        break;

      case 'W':
        KEYS( "War", clan->war,fread_string(fp));
        break;
    }

    if ( !fMatch )
    {
      log_string( "BUG: read_clans: unknown string.");
      fread_to_eol( fp );
    }
  }
}
 
void load_config()
{
  FILE *fp;
  const char *word;
  bool fMatch;
  int cmd,offers=0;

  fp=fopen(CONFIG_FILE,"r");
  if (!fp)
  {
    log_string ("BUG: Configuration file not found!");
    return;
  }

  for ( ; ; )
  {
    fMatch = FALSE;
    if ( feof( fp ) )
    {
      fclose (fp);
      return;
    }

    word = fread_word(fp);

    switch ( word[0] )
    {
      case '*':
      case ';':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case '#':
        if (!strcmp (word, "#END"))
        {
          log_printf("%d offers loaded",offers);
          log_string("Configuration loaded");
          fclose (fp);
          REM_BIT(global_cfg, CFG_PUBLIC);
#if defined (PUBLIC)
          SET_BIT(global_cfg, CFG_PUBLIC);
#endif
          return;
        }

      case 'A':
        if (!strcmp (word, "Autologin"))
        {
          fMatch = TRUE;

          autologin_code=fread_number(fp);
          autologin_pass=fread_number(fp);
          fread_to_eol(fp);
        }
        break;
        
      case 'B':
        KEYS( "BD0", bdmsg[0],fread_string(fp));
        KEYS( "BD1", bdmsg[1],fread_string(fp));
        KEYS( "BD2", bdmsg[2],fread_string(fp));
        KEYS( "BD3", bdmsg[3],fread_string(fp));
        KEYS( "BD4", bdmsg[4],fread_string(fp));
        KEYS( "BD5", bdmsg[5],fread_string(fp));
        KEYS( "BD6", bdmsg[6],fread_string(fp));
        KEYS( "BD7", bdmsg[7],fread_string(fp));
        KEYS( "BD8", bdmsg[8],fread_string(fp));
        KEYS( "BD9", bdmsg[9],fread_string(fp));
        break;
      case 'D':
        if (!strcmp (word, "Dlog"))
        {
          fMatch = TRUE;
          SET_BIT(global_cfg, CFG_DLOG);
          fread_to_eol(fp);
        }

        if (!strcmp (word, "Deny"))
        {
          fMatch = TRUE;
          word = fread_word(fp);
#ifdef WITH_DSO
          /*
           * So, how do I disable commands, which is not loaded (yet)?
           */
#endif
          for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
          {
            if (!str_cmp(cmd_table[cmd].name, word)) SET_BIT(cmd_table[cmd].flag, DENY);
          }
          fread_to_eol(fp);
        }
        break;

      case 'F':
        KEYS( "FILE_ONLINE", fname_online,fread_string(fp));
        KEYS( "FILE_WHO", fname_who,fread_string(fp));
        break;
      case 'G':
        KEY( "Global_config", global_cfg,fread_flag64(fp));
        if (!strcmp(word,"GUILD"))
        {
          load_one_guild(fp);
          fMatch=TRUE;
        }
        break;

      case 'L':
        if (!strcmp (word, "LocalMode"))
        {
          fMatch = TRUE;
          SET_BIT(global_cfg, CFG_LOCAL);
          fread_to_eol(fp);
        }
        break;

      case 'O':
        if (!strcmp(word,"Offer"))
        {
          OFFER_DATA *offer;

          offer=new_offer();
          offer->name=fread_string(fp);
          offer->customer=fread_string(fp);
          offer->gold=fread_number64(fp);
          //offer->flag=fread_number(fp);
          offer->flag=0;
          fread_to_eol(fp);
          fMatch=TRUE;
          offers++;
        }
        break;
#ifdef WITH_DSO
      case 'M':
        if (!strcmp(word, "Module")) {
                const char *dname = fread_string(fp);
                dl_load(dname, NULL);
                fMatch = TRUE;
        }
        break;
#endif
    }

    if ( !fMatch )
    {
      log_printf( "read_config: unknown string [%s].",word);
      fread_to_eol( fp );
    }
  }
}

void load_vote()
{
  FILE *fp;
  const char *word;
  bool fMatch;
  VOTE_DATA *vote=NULL;
  VOTE_CHAR *vch;
  int votes=0, voices=0;


  fp=fopen(VOTE_FILE,"r");
  if (!fp)
  {
   log_string ("BUG: Configuration vote not found!");
   return;
  }

  for ( ; ; )
  {
    fMatch = FALSE;
    if ( feof( fp ) )
    {
      fclose (fp);
      return;
    }

    word = fread_word(fp);

    switch ( word[0] )
    {
      case '*':
      case ';':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case '#':
        if (!strcmp (word, "#END"))
        {
          fclose (fp);
          log_printf("Loading votes: %d votes, %d voices complete", votes, voices);
          return;
        }

        if (strcmp (word, "#VOTE")) break;
        vote = new_vote ();
        fMatch = TRUE;
        votes++;
        break;
      case 'E':
        if (strcmp(word,"End"));
        fMatch=TRUE;
        break;
      case 'M':
        KEYS( "Msg", vote->msg,fread_string(fp));
        break;
      case 'C':
        if (strcmp(word,"CharData")) break;
        vch=new_vote_char();
        fMatch=TRUE;
        vch->name=fread_string(fp);
        vch->age=fread_number(fp);
        vch->level=fread_number(fp);
        vch->voting=fread_number(fp);
        vch->next=vote->chars;
        vote->chars=vch;
        fread_to_eol(fp);
        voices++;
        break;
      case 'O':
        KEYS( "Owner", vote->owner,fread_string(fp));
        break;
    }
    if ( !fMatch )
    {
      log_string( "BUG: read_config: unknown string.");
      fread_to_eol( fp );
    }
  }
}

BAN_DATA *ban_list;

void save_bans(void)
{
  BAN_DATA *pban;
  FILE *fp;
  bool found = FALSE;

  fclose( fpReserve ); 
  if ( ( fp = fopen( BAN_FILE, "w" ) ) == NULL )
  {
    perror( BAN_FILE );
  }

  for (pban = ban_list; pban != NULL; pban = pban->next)
  {
    if (IS_SET(pban->ban_flags,BAN_PERMANENT))
    {
      found = TRUE;
      do_fprintf(fp,"%s %d\n",pban->host,pban->ban_flags);
    }
  }
  fclose(fp);
  fpReserve = fopen( NULL_FILE, "r" );
  if (!found) unlink(BAN_FILE);
}

void load_bans(void)
{
  FILE *fp;
  BAN_DATA *ban_last;
  int bans=0;
 
  if ( ( fp = fopen( BAN_FILE, "r" ) ) == NULL ) return;
 
  ban_last = NULL;

  fseek(fp,0,SEEK_END);
  if (!ftell(fp)) return;
  rewind(fp);
  for ( ; ; )
  {
    BAN_DATA *pban;

    if ( feof(fp) )
    {
      log_printf("Loaded %d bans",bans);
      fclose( fp );
      return;
    }
 
    pban = new_ban();
    bans++;
 
    pban->host = str_dup(fread_word(fp));
    pban->ban_flags = fread_number(fp);
    SET_BIT(pban->ban_flags, BAN_PERMANENT);
    fread_to_eol(fp);

    if (ban_list == NULL) ban_list = pban;
    else ban_last->next = pban;
    ban_last = pban;
  }
}

bool check_ban(const char *site,int type)
{
  BAN_DATA *pban;
  char host[MAX_STRING_LENGTH];

  strcpy(host,site);

  for ( pban = ban_list; pban != NULL; pban = pban->next ) 
  {
    if (!IS_SET(pban->ban_flags,type)) continue;
    if (!str_cmp(pban->host,host)) return TRUE;
    if (IS_SET(pban->ban_flags,BAN_PREFIX) && !str_prefix(pban->host,host)) return TRUE;
    if (IS_SET(pban->ban_flags,BAN_SUFFIX) && !str_suffix(pban->host,host)) return TRUE;
  }
  return FALSE;
}

void do_ban(CHAR_DATA *ch, const char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  BAN_DATA *pban, *prev;
  int type=255;

  argument = one_argument(argument,arg1);

  if (EMPTY(arg1))
  {
    if (ban_list == NULL)
    {
      stc("В списке нет ни одного адреса.\n\r",ch);
      return;
    }

    ptc(ch, "Type   Status Adress\n\r");
    ptc(ch, "------+------+------\n\r");
    for (pban = ban_list;pban != NULL;pban = pban->next)
    {
      ptc(ch, "%s [%s%s] %s | %s\n\r",
      IS_SET(pban->ban_flags,BAN_NEWBIE) ? "New" : IS_SET(pban->ban_flags,BAN_IMM)?"Imm":"All",
      IS_SET(pban->ban_flags,BAN_PREFIX)?"P":" ",
      IS_SET(pban->ban_flags,BAN_SUFFIX)?"S":" ",
      IS_SET(pban->ban_flags,BAN_PERMANENT) ? "Perm" : "Temp",
      pban->host);
    }
    return;
  }

  if (!str_prefix(arg1, "help"))
  {
    stc("{RSyntax:{x ban <all/newbie> <none|prefix|suffix> host\n\r",ch);
    stc("{RSyntax:{x ban save\n\r",ch);
    return;
  }

  if (!str_prefix(arg1, "save"))
  {
    for ( pban = ban_list; pban != NULL; prev = pban, pban = pban->next )
    {
      SET_BIT(pban->ban_flags, BAN_PERMANENT);
    }
    save_bans();
    return;
  }

  if (!str_prefix(arg1, "all"))         type=BAN_ALL;
  else if (!str_prefix(arg1, "newbie")) type=BAN_NEWBIE;
  else if (!str_prefix(arg1, "imm"))    type=BAN_IMM;
  else
  {
    do_ban(ch,"help");
    return;
  }

  argument = one_argument(argument,arg1);
  if (EMPTY(arg1))
  {
     do_ban(ch,"help");
     return;
  }

  if (!str_prefix(arg1, "prefix")) type+=BAN_PREFIX;
  else if (!str_prefix(arg1, "suffix")) type+=BAN_SUFFIX;
  else if (str_prefix(arg1, "none"))
  {
    do_ban(ch,"help");
    return;
  }

  prev = NULL;
  for ( pban = ban_list; pban != NULL; prev = pban, pban = pban->next )
  {
    if (!str_prefix(argument,pban->host))
    {
      if (prev == NULL) ban_list = pban->next;
      else prev->next = pban->next;
      free_ban(pban);
    }
  }

  pban = new_ban();
  pban->host = str_dup(argument);
  pban->ban_flags = type;
  pban->next  = ban_list;
  ban_list    = pban;
  ptc(ch,"Адрес %s закрыт для %s.\n\r",pban->host, IS_SET(pban->ban_flags, BAN_ALL) ? "всех":"новичков");
}

void do_allow( CHAR_DATA *ch, char *argument )                        
{
  char arg[MAX_INPUT_LENGTH];
  BAN_DATA *prev;
  BAN_DATA *curr;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    stc( "С какого адреса снять запрет?\n\r", ch );
    return;
  }

  prev = NULL;
  for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
  {
    if ( !str_prefix( arg, curr->host ) )
    {
      ptc(ch,"Запрет с адреса %s снят.\n\r",curr->host);
      if ( prev == NULL ) ban_list   = ban_list->next;
      else prev->next = curr->next;
      free_ban(curr);
      return;
    }
  }
  stc("Такого адреса нет в списке.\n\r", ch );
}

void load_races(void)
{
  FILE *fp;
  const char *word;
  int counter=0;

  if ((fp=fopen( RACE_FILE,"r"))==NULL)
  {
    log_string("BUG: Loading races: "RACE_FILE" not found!");
    exit(1);
  }
  for (;;)
  {
    if ( feof( fp ) )
    {
      fclose (fp);
      log_string("BUG: Load race data: "RACE_FILE" is corrupt! Check race table!");
      return;
    }
    word = fread_word(fp);
    if (word[0]==';' || word[0]=='*') fread_to_eol(fp);
    else if (!strcmp(word,"#END")) break;
    else if (!strcmp(word,"#Race"))
    {
      load_one_race(fp,counter);
      counter++;
    }
  }
  log_printf("Loaded %d races",counter);
}

#define rtab race_table[race]
void load_one_race(FILE *fp, int race)
{
  const char *word;
  bool fMatch;
  int i;

  for ( ; ; )
  {
    fMatch = FALSE;

    word = fread_word(fp);
    switch ( word[0] )
    {
      case '*':
      case ';':
      case '#':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'A':
        KEY("Act",rtab.act,fread_flag64(fp));
        KEY("Aff",rtab.aff,fread_flag64(fp));
        break;
      case 'C':
        KEY( "Cbonus",   rtab.c_bon,fread_flag(fp));
        KEY( "Cpenalty", rtab.c_pen,fread_flag(fp));
        if (!strcmp(word,"Class"))
        {
          for (i=0;i<MAX_CLASS;i++) rtab.class_mult[i]=fread_number(fp);
          fMatch=TRUE;
        }
        break;
      case 'D':
        if (!strcmp(word,"Dambonus"))
        {
          for (i=0;i<DAM_MAX;i++) rtab.dambonus[i]=fread_number(fp);
          fMatch=TRUE;
        }
        break;
      case 'E':
        if (!strcmp(word,"END")) return;
        break;
      case 'F':
        KEY( "Form", rtab.form,fread_flag64(fp));
        break;
      case 'H':
        KEY("Hand", rtab.hand_dam,str_dup(fread_word(fp)));
        if (!strcmp(word, "Hstats"))
        {
          for(i=0;i<MAX_STATS;i++) rtab.high_stats[i]=fread_number(fp);
          fMatch=TRUE;
        }
        break;
      case 'I':
        KEY("Imm", rtab.imm, fread_flag64( fp ) );
        break;
      case 'M':
        if (!strcmp(word, "Mstats"))
        {
          for (i=0;i<MAX_STATS;i++) rtab.max_stats[i]=fread_number(fp);
          fMatch=TRUE;
        }
        break;
      case 'N':
        KEY("Name", rtab.name,fread_string(fp));
        break;
      case 'O':
        KEY("Off", rtab.off, fread_flag64( fp ) );
        break;
      case 'P':
        KEY("Parts",  rtab.parts, fread_flag64( fp ) );
        KEY("Points", rtab.points, fread_number( fp ) );
        if (!strcmp(word,"Pc_race"))
        {
          if (!str_cmp(fread_string(fp),"TRUE")) rtab.pc_race=TRUE;
          else rtab.pc_race=FALSE;
          fMatch=TRUE;
        }
        break;
      case 'R':
        KEY("Res", rtab.res, fread_flag64( fp ) );
        break;
      case 'S':
        KEY("Size", rtab.size, fread_number( fp ) );
        KEY("Spec", rtab.spec, fread_flag64( fp ) );
        if (!strcmp(word, "Stats"))
        {
          for (i=0;i<MAX_STATS;i++) rtab.stats[i]=fread_number(fp);
          fMatch=TRUE;
        }
        if (word[1]=='k')
        {
              if (word[2]=='1') rtab.skills[0]=str_dup(fread_word(fp));
         else if (word[2]=='2') rtab.skills[1]=str_dup(fread_word(fp));
         else if (word[2]=='3') rtab.skills[2]=str_dup(fread_word(fp));
         else if (word[2]=='4') rtab.skills[3]=str_dup(fread_word(fp));
         else if (word[2]=='5') rtab.skills[4]=str_dup(fread_word(fp));
         fMatch=TRUE;
        }
        break;
      case 'V':
        KEY("Vuln", rtab.vuln, fread_flag64( fp ) );
        break;
      case 'W':
        KEY("Wealth", rtab.wealth,fread_number(fp));
        KEY("Wname", rtab.who_name,fread_string(fp));
        if (!strcmp(word,"Wbonus"))
        {
          for (i=0;i<WEAPON_MAX;i++) rtab.weapon_bonus[i]=fread_number(fp);
          fMatch=TRUE;
        }
        break;
    }
    if ( !fMatch )
    {
      log_printf( "load race: unknown string '%s'",word);
      fread_to_eol( fp );
    }
  }
}
#undef rtab

void load_newspaper(void)
{
  FILE *fp;
  const char *word;
  int counter=0;
  bool fMatch;
  NEWS_DATA *news=0;

  if ((fp=fopen( NEWSPAPER_FILE,"r"))==NULL)
  {
    log_string("BUG: Loading newapaper: "NEWSPAPER_FILE" not found!");
    return;
  }
  for (;;)
  {
    if ( feof( fp ) )
    {
      fclose (fp);
      log_string("BUG:Loading newspaper: "NEWSPAPER_FILE" is corrupt!");
      return;
    }
    word = fread_word(fp);
    fMatch=FALSE;
    switch(word[0])
    {
      case '*':
      case ';':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case '#':
        if (!strcmp(word,"#END"))
        {
          log_printf("Loading newspaper: Complete, loaded %d news",counter);
          return;
        }
        break;
      case 'D':
        KEY("Date",news->date,fread_string(fp));
        break;
      case 'S':
        KEY("Stamp",news->date_stamp,fread_number(fp));
        break;
      case 'T':
        if (!strcmp(word, "Type"))
        {
           if (!newspaper_list)
           {
             newspaper_list=new_news();
             news=newspaper_list;
           }
           else
           {
             news->next=new_news();
             news=news->next;
           }
           news->type=fread_number(fp);
           counter++;
           fMatch=TRUE;
        }
        KEY("Text",news->text,fread_string(fp));
        break;
    }
    if ( !fMatch )
    {
      log_printf( "load newspaper: unknown string '%s'",word);
      fread_to_eol( fp );
    }
  }
}

void load_deities(void)
{
  FILE *fp;
  const char *word;
  int counter=0;

  if ( (fp=fopen( DEITY_FILE,"r"))==NULL )
  {
    log_string("BUG: "DEITY_FILE" not found!");
    exit(1);
  }
  for (;;)
  {
    if ( feof( fp ) )
    {
      fclose (fp);
      log_string("BUG: "DEITY_FILE" is corrupted! Check deity table!");
      return;
    }
    word = fread_word(fp);

    switch( word[0] )
    {
      case ';':
      case '*':
        fread_to_eol(fp);
        break;
      case '#':
        if( !strcmp (word, "#END") )
        {
          fclose (fp);
          log_printf("Loaded %d deities",counter);
          return;
        }
        if( !strcmp(word,"#Deity") )
        {
          load_one_deity( fp, counter);
          counter++;
          break;
        }
        break;
    }
  }
}

#define dtab deity_table[deity]
void load_one_deity(FILE *fp, int deity)
{
  const char *word;
  bool fMatch;
  int i;

  for ( ; ; )
  {
    fMatch = FALSE;

    word = fread_word(fp);
    switch ( word[0] )
    {
      case '*':
      case ';':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'A':
        if( !str_cmp(word, "App") )
        {
          for( i=0; i < MAX_DEITY_APP; i++)
            dtab.d_apply[i] = fread_number(fp);
          fread_to_eol( fp );
          fMatch = TRUE;
          break;
        }
        break;
      case 'E':
        if ( !strcmp(word,"END") )
        {
          log_printf("Loading deity: %s...",dtab.name);
          fread_to_eol(fp);
          return;
        }
        break;
      case 'M':
        KEY("MinAlign",dtab.min_align,fread_number(fp));
        KEY("MaxAlign",dtab.max_align,fread_number(fp));
        break;
      case 'N':
        KEY("Name",dtab.name, fread_string(fp));
        break;
      case 'R':
        KEY("Russian",dtab.russian, fread_string(fp));
        break;
      case 'D':
        KEY("Descr",dtab.descr, fread_string(fp));
        break;
      case 'W':
        KEY("Worship",dtab.worship, fread_number(fp));
        break;
    }
    if ( !fMatch )
    {
      log_printf( "Loading deities: unknown string '%s'", word);
      fread_to_eol( fp );
    }
  }
}
#undef dtab
