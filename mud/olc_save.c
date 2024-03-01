// $Id: olc_save.c,v 1.19 2002/10/11 10:22:27 saboteur Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "tables.h"

// Verbose writes reset data in plain english into the comments
// section of the resets.  It makes areas considerably larger but
// may aid in debugging.
#define VERBOSE

// Returns a string without \r and ~.
char *fix_string( const char *str )
{
  static char strfix[MAX_STRING_LENGTH];
  int i, o;

  if ( !str ) return '\0';
  for ( o = i = 0; str[i+o] != '\0'; i++ )
  {
    if (str[i+o] == '\r' || str[i+o] == '~') o++;
    strfix[i] = str[i+o];
  }
  strfix[i] = '\0';
  return strfix;
}

// Saves the listing of files to be loaded at startup.
// Called by:     do_asave(olc_save.c).
void save_area_list()
{
  FILE *fp;
  AREA_DATA *pArea;

  fclose(fpReserve);
  if ( ( fp = fopen( "area.lst", "w" ) ) == NULL )
  {
    bug( "Save_area_list: fopen", 0 );
    perror( "area.lst" );
  }
  else
  {
    // Add any help files to this section
    do_fprintf( fp, "help.are\n"    );
    do_fprintf( fp, "mobprog.prg\n" );
    do_fprintf( fp, "romhelp.are\n" );
    do_fprintf( fp, "group.are\n"   );
    do_fprintf( fp, "olc.hlp\n"     );
    do_fprintf( fp, "imms.are\n"    ); // by Dinger
    do_fprintf( fp, "clanhelp.are\n"); // by Dinger
    do_fprintf( fp, "spells.are\n"  ); // by Dinger

    for( pArea = area_first; pArea; pArea = pArea->next )
      do_fprintf( fp, "%s\n", pArea->file_name );

    do_fprintf( fp, "$\n" );
    fclose( fp );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

// Save one mobile to file, new format -- Hugin
// Called by:     save_mobiles (below).
void save_mobile( FILE *fp, MOB_INDEX_DATA *pMobIndex )
{
  MPROG_LIST  *mprg;
  int race = pMobIndex->race;
  int i;

  do_fprintf( fp, "#%u\n",  pMobIndex->vnum );
  do_fprintf( fp, "%s~\n",  pMobIndex->player_name );
  do_fprintf( fp, "%s~\n",  pMobIndex->short_descr );
  do_fprintf( fp, "%s~\n",  fix_string( pMobIndex->long_descr ) );
  do_fprintf( fp, "%s~\n",  fix_string( pMobIndex->description) );
  do_fprintf( fp, "%s~\n",  race_table[race].name );
  do_fprintf( fp, "%s ",    pflag64(pMobIndex->act));
  do_fprintf( fp, "%s ",    pflag64(pMobIndex->affected_by));
  do_fprintf( fp, "%d %d\n",pMobIndex->alignment , pMobIndex->group);
  do_fprintf( fp, "%d ",    pMobIndex->level );
  do_fprintf( fp, "%d ",    pMobIndex->hitroll );
  do_fprintf( fp, "%dd%d+%d ", pMobIndex->hit[DICE_NUMBER], 
     pMobIndex->hit[DICE_TYPE], 
     pMobIndex->hit[DICE_BONUS] );
  do_fprintf( fp, "%dd%d+%d ", pMobIndex->mana[DICE_NUMBER], 
     pMobIndex->mana[DICE_TYPE], 
     pMobIndex->mana[DICE_BONUS] );
  do_fprintf( fp, "%dd%d+%d ", pMobIndex->damage[DICE_NUMBER], 
     pMobIndex->damage[DICE_TYPE], 
     pMobIndex->damage[DICE_BONUS] );
  do_fprintf( fp, "%s\n",      attack_table[pMobIndex->dam_type].name );
  do_fprintf( fp, "%d %d %d %d\n", pMobIndex->ac[AC_PIERCE] / 10, 
     pMobIndex->ac[AC_BASH]   / 10, 
     pMobIndex->ac[AC_SLASH]  / 10, 
     pMobIndex->ac[AC_EXOTIC] / 10 );
  do_fprintf( fp, "%s ",  pflag64(pMobIndex->off_flags));
  do_fprintf( fp, "%s ",  pflag64( pMobIndex->imm_flags));
  do_fprintf( fp, "%s ",  pflag64( pMobIndex->res_flags));
  do_fprintf( fp, "%s\n", pflag64( pMobIndex->vuln_flags));
  do_fprintf( fp, "%s %s %s %d\n",
     position_table[pMobIndex->start_pos].short_name,
     position_table[pMobIndex->default_pos].short_name,
     pMobIndex->sex == 0 ? "none" :
     pMobIndex->sex == 1 ? "male" :
     pMobIndex->sex == 2 ? "female" : "either",
     pMobIndex->wealth );
  do_fprintf( fp, "%s ",  pflag64( pMobIndex->form));
//  do_fprintf( fp, "parts " );

  do_fprintf( fp, "%s\n",  size_table[pMobIndex->size].name );
//  do_fprintf( fp, "%s\n", material_lookup(pMobIndex->material));

  if ( pMobIndex->mprog_flags )
    for ( i = 0, mprg = pMobIndex->mprogs; mprg ; mprg = mprg->next )
    {
      i++;
      do_fprintf( fp, "M %s %d %s~\n", mprog_type_to_name( mprg->trig_type ),
        mprg->vnum,mprg->trig_phrase );
    }    
}

// Saves all mobiles in area
void save_mobiles( FILE *fp, AREA_DATA *pArea )
{
  int64 i;
  MOB_INDEX_DATA *pMob;

  do_fprintf( fp, "#MOBILES\n" );

  for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
  {
    if ( (pMob = get_mob_index( i )) ) save_mobile( fp, pMob );
  }
  do_fprintf( fp, "#0\n\n" );
}

// Purpose:   Save one object to file.
// Called by: save_objects (below).
void save_object( FILE *fp, OBJ_INDEX_DATA *pObjIndex )
{
  char letter;
  AFFECT_DATA *pAf;
  EXTRA_DESCR_DATA *pEd;

  do_fprintf( fp, "#%u\n",    pObjIndex->vnum );
  do_fprintf( fp, "%s~\n",    pObjIndex->name );
  do_fprintf( fp, "%s~\n",    pObjIndex->short_descr );
  do_fprintf( fp, "%s~\n",    fix_string( pObjIndex->description ) );
  do_fprintf( fp, "%s~\n",    material_lookup(pObjIndex->material));
  do_fprintf( fp, "%s ",      item_name(pObjIndex->item_type));

  REM_BIT(pObjIndex->extra_flags, ITEM_CLANENCHANT);
  do_fprintf( fp, "%s ",      pflag64( pObjIndex->extra_flags) );
  do_fprintf( fp, "%s\n",     pflag64( pObjIndex->wear_flags) );

  switch ( pObjIndex->item_type )
  {
    default:
      do_fprintf( fp, "%u ",  pObjIndex->value[0]);
      do_fprintf( fp, "%u ",  pObjIndex->value[1]);
      do_fprintf( fp, "%u ",  pObjIndex->value[2]);
      do_fprintf( fp, "%u ",  pObjIndex->value[3]);
      do_fprintf( fp, "%u\n", pObjIndex->value[4]);
      break;

    case ITEM_LIGHT:
      do_fprintf( fp, "0 0 %u 0 0\n",
         pObjIndex->value[2] < 1 ? 998 : pObjIndex->value[2] );
      break;

    case ITEM_MONEY:
      do_fprintf( fp, "%u %u 0 0 0\n",pObjIndex->value[0],
                                      pObjIndex->value[1]);
      break;
            
    case ITEM_DRINK_CON:
      do_fprintf( fp, "%u %u '%s' %u 0\n",pObjIndex->value[0],
                                          pObjIndex->value[1],
                                          liq_table[pObjIndex->value[2]].liq_name,
                                          pObjIndex->value[3]);
      break;
                    
    case ITEM_FOUNTAIN:
      do_fprintf( fp, "%u %u '%s' 0 0 \n",pObjIndex->value[0],
                                          pObjIndex->value[1],
                                          liq_table[pObjIndex->value[2]].liq_name);
      break;
            
    case ITEM_CONTAINER:
      do_fprintf( fp, "%u %s %u %u %u\n",pObjIndex->value[0],
                                         pflag64( pObjIndex->value[1]),
                                         pObjIndex->value[2],
                                         pObjIndex->value[3],
                                         pObjIndex->value[4]);
      break;
            
    case ITEM_FOOD:
      do_fprintf( fp, "%u %u 0 %s 0\n",pObjIndex->value[0], 
                                       pObjIndex->value[1], 
                                       pflag64(pObjIndex->value[3]));
      break;
            
    case ITEM_PORTAL:
      do_fprintf( fp, "%u %s %s %u 0\n",pObjIndex->value[0], 
                                        pflag64(pObjIndex->value[1]),
                                        pflag64(pObjIndex->value[2]),
                                        pObjIndex->value[3]);
      break;
            
    case ITEM_FURNITURE:
      do_fprintf( fp, "%u ",pObjIndex->value[0]);
      do_fprintf( fp, "%u ",pObjIndex->value[1]);
      do_fprintf( fp, "%s %u ",pflag64(pObjIndex->value[2]),
                               pObjIndex->value[3]);
      do_fprintf( fp, "%u\n",pObjIndex->value[4]);
      break;
            
    case ITEM_WEAPON:
      do_fprintf( fp, "%s %u %u %s %s\n",
             weapon_name((int)pObjIndex->value[0]),
             pObjIndex->value[1],
             pObjIndex->value[2],
             attack_table[pObjIndex->value[3]].name,
             pflag64(pObjIndex->value[4]));
      break;
            
    case ITEM_ARMOR:
      do_fprintf( fp, "%u ",pObjIndex->value[0]);
      do_fprintf( fp, "%u ",pObjIndex->value[1]);
      do_fprintf( fp, "%u ",pObjIndex->value[2]);
      do_fprintf( fp, "%u ",pObjIndex->value[3]);
      do_fprintf( fp, "%u\n",pObjIndex->value[4]);
      break;
            
    case ITEM_PILL:
    case ITEM_POTION:
    case ITEM_SCROLL:
      do_fprintf( fp, "%u ",pObjIndex->value[0] > 0 ?
                            pObjIndex->value[0] : 0);
      do_fprintf( fp, "'%s' '%s' '%s' '%s'\n",
           pObjIndex->value[1] != -1 ? skill_table[pObjIndex->value[1]].name: 0,
           pObjIndex->value[2] != -1 ? skill_table[pObjIndex->value[2]].name: 0,
           pObjIndex->value[3] != -1 ? skill_table[pObjIndex->value[3]].name: 0,
           pObjIndex->value[4] != -1 ? skill_table[pObjIndex->value[4]].name: 0);
      break;

    case ITEM_STAFF:
    case ITEM_WAND:
      do_fprintf( fp, "%u ", pObjIndex->value[0] );
      do_fprintf( fp, "%u ", pObjIndex->value[1] );
      do_fprintf( fp, "%u ",pObjIndex->value[2]);
      do_fprintf( fp, "'%s' 0\n",pObjIndex->value[3] != -1 ? skill_table[pObjIndex->value[3]].name : 0 );
      break;
  }

  do_fprintf( fp, "%d ", pObjIndex->level );
  do_fprintf( fp, "%d ", pObjIndex->weight );
  do_fprintf( fp, "%u ", pObjIndex->cost );

  if( pObjIndex )
  {
    if( pObjIndex->durability )
    {
      if( pObjIndex->durability != -1 )
        if( pObjIndex->durability == 0 || pObjIndex->durability == 200 
         || pObjIndex->durability < -1 || pObjIndex->durability > 1000
         || pObjIndex->durability != material_table[material_num(pObjIndex->material)].d_dam ) 
              pObjIndex->durability = material_table[material_num(pObjIndex->material)].d_dam;
    }
    else pObjIndex->durability = material_table[material_num(pObjIndex->material)].d_dam;
  }
  else 
  {
    bug("NULL obj is sent as an argument to save_obj()!", 0);
    return;
  }

    if( pObjIndex->condition > pObjIndex->durability) pObjIndex->condition = pObjIndex->durability;

  do_fprintf( fp, "%d ", pObjIndex->durability );
{
       if ( pObjIndex->condition >  900 ) letter = 'P';
  else if ( pObjIndex->condition >  750 ) letter = 'G';
  else if ( pObjIndex->condition >  500 ) letter = 'A';
  else if ( pObjIndex->condition >  250 ) letter = 'W';
  else if ( pObjIndex->condition >  100 ) letter = 'D';
  else if ( pObjIndex->condition >    0 ) letter = 'B';
  else                                    letter = 'R';

  do_fprintf( fp, "%c\n", letter );
}

  for( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
    do_fprintf( fp, "A\n%d %d\n",  pAf->location, pAf->modifier );

  for( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
  {
    do_fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
      fix_string( pEd->description ) );
  }
  if (pObjIndex->ttime!=0 || pObjIndex!=0)
  {
    do_fprintf(fp,"T %s %d\n",pflag64(pObjIndex->ttype),pObjIndex->ttime);
  }
}
 
// Saves all objects in area
void save_objects( FILE *fp, AREA_DATA *pArea )
{
  int64 i;
  OBJ_INDEX_DATA *pObj;

  do_fprintf( fp, "#OBJECTS\n" );

  for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
  {
    pObj=get_obj_index(i);
    if (pObj && !IS_SET(pObj->extra_flags,ITEM_DELETED)) 
    {
      save_object(fp,pObj);
    }
  }
  do_fprintf( fp, "#0\n\n" );
}

// Saves all rooms in area
void save_rooms( FILE *fp, AREA_DATA *pArea )
{
  ROOM_INDEX_DATA *pRoomIndex;
  EXTRA_DESCR_DATA *pEd;
  EXIT_DATA *pExit;
  int iHash;
  int door;

  do_fprintf( fp, "#ROOMS\n" );
  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
    {
      if (IS_SET(pRoomIndex->room_flags,ROOM_DELETED))
      {
        log_printf("Asave: Deleting room %u",pRoomIndex->vnum);
        continue;
      }
      if (pRoomIndex->area!=pArea) continue;

      do_fprintf( fp, "#%u\n", pRoomIndex->vnum );
      do_fprintf( fp, "%s~\n", pRoomIndex->name );
      do_fprintf( fp, "%s~\n", fix_string( pRoomIndex->description ) );
      do_fprintf( fp, "0 " );
      do_fprintf( fp, "%u ",   pRoomIndex->room_flags );
      do_fprintf( fp, "%u\n",  pRoomIndex->sector_type );

      for ( pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next )
      {
        do_fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
                         fix_string( pEd->description ) );
      }
      for( door = 0; door < MAX_DIR; door++ )
      {
        if ( ( pExit = pRoomIndex->exit[door] ) && pExit->u1.to_room )
        {
          int locks = 0;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR ) 
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) ) 
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )  ) locks = 1;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )  ) locks = 2;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) ) )  locks = 3;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) ) )  locks = 4;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_NOPASS ) ) )   locks = 5;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_NOPASS ) ) )   locks = 6;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_NOPASS ) ) )   locks = 7;
          if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
            && ( IS_SET( pExit->rs_flags, EX_NOPASS ) ) )   locks = 8;
                      
          do_fprintf( fp, "D%d\n",      pExit->orig_door );
          do_fprintf( fp, "%s~\n",      fix_string( pExit->description ) );
          do_fprintf( fp, "%s~\n",      pExit->keyword );
          do_fprintf( fp, "%d %u %u\n", locks, pExit->key, pExit->u1.to_room->vnum );
        }
      }
      if (pRoomIndex->mana_rate != 100 || pRoomIndex->heal_rate != 100)
        do_fprintf ( fp, "M %d H %d\n",pRoomIndex->mana_rate,
                                       pRoomIndex->heal_rate);
      do_fprintf( fp, "S\n" );
    }
  }
  do_fprintf( fp, "#0\n\n\n\n" );
}

// Save #SPECIALS section of area file.
// Called by:     save_area(olc_save.c).
void save_specials( FILE *fp, AREA_DATA *pArea )
{
  int iHash;
  MOB_INDEX_DATA *pMobIndex;
    
  do_fprintf( fp, "#SPECIALS\n" );

  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
    {
      if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun )
      {
        do_fprintf( fp, "M %u ", pMobIndex->vnum);
        do_fprintf( fp, "%s Load to: %s\n",spec_name( pMobIndex->spec_fun ),
                                           get_mobindex_desc(pMobIndex,1) );
      }
    }
  }
  do_fprintf( fp, "S\n\n" );
}

// This function is obsolete.  It it not needed but has been left here
// for historical reasons.  It is used currently for the same reason.
// I don't think it's obsolete in ROM -- Hugin.
void save_door_resets( FILE *fp, AREA_DATA *pArea )
{
  int iHash;
  ROOM_INDEX_DATA *pRoomIndex;
  EXIT_DATA *pExit;
  int door;

  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
    {
      if ( pRoomIndex->area == pArea )
      {
        for( door = 0; door < MAX_DIR; door++ )
        {
          if ( ( pExit = pRoomIndex->exit[door] )
            && pExit->u1.to_room 
            && ( IS_SET( pExit->rs_flags, EX_CLOSED )
            || IS_SET( pExit->rs_flags, EX_LOCKED ) ) )
          {
#if defined( VERBOSE )
            do_fprintf( fp, "D 0 %u ", pRoomIndex->vnum);
            do_fprintf( fp, "%d %d The %s door of %s is %s\n",
               pExit->orig_door,
               IS_SET( pExit->rs_flags, EX_LOCKED) ? 2 : 1,
               dir_name[ pExit->orig_door ],
               pRoomIndex->name,
               IS_SET( pExit->rs_flags, EX_LOCKED) ? "closed and locked": "closed" );
#endif
#if !defined( VERBOSE )
            do_fprintf( fp, "D 0 %u ", pRoomIndex->vnum);
            do_fprintf( fp, "%d %d\n",pExit->orig_door,
               IS_SET( pExit->rs_flags, EX_LOCKED) ? 2 : 1 );
#endif
          }
        }
      }
    }
  }
}

// Purpose:       Saves the #RESETS section of an area file.
// Called by:     save_area(olc_save.c)
void save_resets( FILE *fp, AREA_DATA *pArea )
{
  RESET_DATA *pReset;
  MOB_INDEX_DATA *pLastMob = NULL;
  OBJ_INDEX_DATA *pLastObj;
  ROOM_INDEX_DATA *pRoom;
  char buf[MAX_STRING_LENGTH];
  int iHash;

  do_fprintf( fp, "#RESETS\n" );

  save_door_resets( fp, pArea );

  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
    {
      if ( pRoom->area == pArea )
      {
        for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
        {
          switch ( pReset->command )
          {
            default:
              bug( "Save_resets: bad command %c.", pReset->command );
              break;

#if defined( VERBOSE )
            case 'M':
              pLastMob = get_mob_index( pReset->arg1 );
              do_fprintf( fp, "M 1 %u %d %u %d Load %s\n",
                   pReset->arg1, pReset->arg2,
                   pReset->arg3, pReset->arg4,
                   get_mobindex_desc(pLastMob,1) );
              break;

            case 'O':
              pLastObj = get_obj_index( pReset->arg1 );
              pRoom = get_room_index( pReset->arg3 );
              do_fprintf( fp, "O 0 %u 0 %u %s loaded to %s\n",
                    pReset->arg1, pReset->arg3,
                    capitalize(pLastObj->short_descr),
                    pRoom->name );
              break;

            case 'P':
              pLastObj = get_obj_index( pReset->arg1 );
              do_fprintf( fp, "P 0 %u %d %u %d %s put inside %s\n", 
                  pReset->arg1, pReset->arg2,
                  pReset->arg3, pReset->arg4,
                  capitalize(get_obj_index( pReset->arg1 )->short_descr),
                  pLastObj->short_descr );
              break;
              
            case 'G':
              do_fprintf( fp, "G 0 %u 0 %s is given to %s\n",
                   pReset->arg1,
                   capitalize(get_obj_index( pReset->arg1 )->short_descr),
                   pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
              if ( !pLastMob )
              {
                  do_printf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                  bug( buf, 0 );
              }
              break;

            case 'E':
              do_fprintf( fp, "E 0 %u 0 %u %s is loaded %s of %s\n",
                pReset->arg1, pReset->arg3,
//                capitalize(get_obj_index( pReset->arg1 )->short_descr),
		get_objindex_desc(get_obj_index(pReset->arg1),1),
                flag_string( wear_loc_strings, pReset->arg3 ),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
              if ( !pLastMob )
              {
                do_printf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
              }
              break;

            case 'D':
              break;

            case 'R':
              pRoom = get_room_index( pReset->arg1 );
              do_fprintf( fp, "R 0 %u %d Randomize %s\n", 
                pReset->arg1, pReset->arg2, pRoom->name );
              break;
          }
#endif
#if !defined( VERBOSE )
          case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
            do_fprintf( fp, "M 1 %u %d %u %d \n", 
                pReset->arg1, pReset->arg2,
                pReset->arg3, pReset->arg4 );
            break;

          case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
            do_fprintf( fp, "O 0 %u 0 %u\n", pReset->arg1,pReset->arg3);
            break;

          case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
            do_fprintf( fp, "P 0 %u %d %u %d\n",pReset->arg1, pReset->arg2,
                pReset->arg3, pReset->arg4 );
            break;

          case 'G':
            do_fprintf( fp, "G 0 %u 0\n", pReset->arg1 );
            if ( !pLastMob )
            {
              do_printf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
              bug( buf, 0 );
            }
            break;

          case 'E':
            do_fprintf( fp, "E 0 %u 0 %u\n", pReset->arg1, pReset->arg3 );
            if ( !pLastMob )
            {
              do_printf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
              bug( buf, 0 );
            }
            break;

          case 'D':
            break;

          case 'R':
            pRoom = get_room_index( pReset->arg1 );
            do_fprintf( fp, "R 0 %u %d\n", pReset->arg1, pReset->arg2 );
            break;
            }
#endif
        }
      }   // End if correct area
    }     // End for pRoom
  }       // End for iHash
  do_fprintf( fp, "S\n\n\n\n" );
}

// Saves the #SHOPS section of an area file.
// Called by:     save_area(olc_save.c)
void save_shops( FILE *fp, AREA_DATA *pArea )
{
  SHOP_DATA *pShopIndex;
  MOB_INDEX_DATA *pMobIndex;
  int iTrade;
  int iHash;
    
  do_fprintf( fp, "#SHOPS\n" );

  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
    {
      if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop )
      {
        pShopIndex = pMobIndex->pShop;

        do_fprintf( fp, "%u ", pShopIndex->keeper );
        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShopIndex->buy_type[iTrade] != 0 )
          {
            do_fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
          }
          else do_fprintf( fp, "0 ");
        }
        do_fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
        do_fprintf( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
      }
    }
  }
  do_fprintf( fp, "0\n\n\n\n" );
}

// Save an area, note that this format is new.
// Called by:     do_asave(olc_save.c).
void save_area( AREA_DATA *pArea )
{
  FILE *fp;
  char buf[MAX_STRING_LENGTH];
  int64 vnum;
  int minlevel = 120,maxlevel = 0; 
  MOB_INDEX_DATA *pMob;

  fclose( fpReserve );
  if ( ( fp = fopen( pArea->file_name, "w" ) )==NULL )
  {
    bug( "Open_area: fopen", 0 );
    perror( pArea->file_name );
  }
  else
  {
    do_fprintf( fp, "#AREADATA\n" );
    do_fprintf( fp, "Name %s~\n",     pArea->name );
    do_fprintf( fp, "Builders %s~\n", fix_string( pArea->builders ) );
    do_fprintf( fp, "VNUMs %u %u\n",  pArea->min_vnum,pArea->max_vnum );

    for (vnum=pArea->min_vnum;vnum<=pArea->max_vnum;vnum++)
      if ((pMob=get_mob_index(vnum)))
      {
       //these are not victims
       if (IS_SET(pMob->act, ACT_IS_HEALER)
         || IS_SET(pMob->act, ACT_TRAIN)
         || pMob->pShop != NULL
         || IS_SET(pMob->act, ACT_PET)) continue; 
       if (pMob->level < minlevel) minlevel = pMob->level;
       if (pMob->level > maxlevel) maxlevel = pMob->level;
      }
    // fix the numbers if there's no mob in this area
    if (maxlevel==0)   maxlevel = 110;
    if (minlevel==120) minlevel = 0;
    
    do_printf(buf, "<%3d %3d> %s", minlevel, maxlevel, pArea->name);  
    free_string( pArea->credits );
    pArea->credits = str_dup(buf);
    
    do_fprintf( fp, "Credits %s~\n",pArea->credits );
    do_fprintf( fp, "Security %d\n",pArea->security );
    do_fprintf( fp, "Flags %u\n",pArea->area_flags );
    do_fprintf( fp, "Clan %s~\n",pArea->clan );
    do_fprintf( fp, "Version 3\n");
// version 2
// version 3 - remove outdoors and indoors flag from mobiles. now stopdoor
//
//

    do_fprintf( fp, "End\n\n\n\n" );

    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );
    save_specials( fp, pArea );
    save_resets( fp, pArea );
    save_shops( fp, pArea );

    do_fprintf( fp, "#$\n" );
    fclose( fp );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

void do_mpsave( CHAR_DATA *ch, char *argument )
{
  int i;
  FILE *fp;
  MPROG_CODE *pMprog;

  fclose(fpReserve);
  if (!(fp = fopen( "mobprog.prg", "w")))
  {
    bug( "Open_help: fopen", 0);
    perror( "mobprog.prg");
  }
  else 
  {
    do_fprintf(fp, "#MOBPROGS\n");
    for (i=0; i< MAX_VNUM; i++)
    {
      if ( (pMprog = get_mprog_index(i)) )
        do_fprintf(fp, "#%d\n%s~\n",i, fix_string(pMprog->code));
    }
    do_fprintf(fp,"#0\n\n");
    do_fprintf(fp, "#$\n" );
    fclose( fp );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

// Name:          do_asave
// Purpose:       Entry point for saving area data.
// Called by:     interpreter(interp.c) 
void do_asave( CHAR_DATA *ch, char *argument )
{
  char arg1 [MAX_INPUT_LENGTH];
  AREA_DATA *pArea;
  int value;

  if ( !ch )
  {
    save_area_list();
    for( pArea = area_first; pArea; pArea = pArea->next )
    {
      if (IS_SET(pArea->area_flags,AREA_SAVELOCK))
      {
        ptc(ch,"Area %s was not saved! (Locked from saving)\n\r",ch);
        continue;
      }
      REM_BIT( pArea->area_flags, AREA_CHANGED );
      save_area( pArea );
    }
    return;
  }
  strcpy( arg1, argument );
  if ( arg1[0] == '\0' )
  {
    stc( "Syntax:\n\r", ch );
    stc( "  asave <vnum>   - saves a particular area\n\r",      ch );
    stc( "  asave list     - saves the area.lst file\n\r",      ch );
    stc( "  asave area     - saves the area being edited\n\r",  ch );
    stc( "  asave changed  - saves all changed zones\n\r",      ch );
    stc( "  asave world    - saves the world! (db dump)\n\r",   ch );
    stc( "\n\r", ch );
    return;
  }

  // Snarf the value (which need not be numeric)
  value = atoi( arg1 );
  if ( !( pArea = get_area_data( value ) ) && is_number( arg1 ) )
  {
    stc( "That area does not exist.\n\r", ch );
    return;
  }

  // Save area of given vnum.
  if ( is_number( arg1 ) )
  {
    if ( !IS_BUILDER( ch, pArea ) )
    {
      stc( "You are not a builder for this area.\n\r", ch );
      return;
    }
    if (IS_SET(pArea->area_flags,AREA_SAVELOCK))
    {
      ptc(ch,"Area %s was not saved! (Locked from saving)\n\r",ch);
      return;
    }
    save_area_list();
    save_area( pArea );
    return;
  }

  // Save the world, only authorized areas
  if ( !str_cmp( "world", arg1 ) )
  {
    save_area_list();
    for( pArea = area_first; pArea; pArea = pArea->next )
    {
      if (IS_SET(pArea->area_flags,AREA_SAVELOCK))
      {
        ptc(ch,"Area %s was not saved! (Locked from saving)\n\r",ch);
        continue;
      }
      if ( !IS_BUILDER( ch, pArea ) ) continue;     

      REM_BIT( pArea->area_flags, AREA_CHANGED );
      log_printf("Saving: %s", pArea->file_name);
      save_area( pArea );
    }
    stc( "Все арии сохранены.\n\r", ch );
    return;
  }

  // Save changed areas, only authorized areas
  if ( !str_cmp( "changed", arg1 ) )
  {
    char buf[MAX_INPUT_LENGTH];

    save_area_list();

    stc( "Saved zones:\n\r", ch );
    do_printf( buf, "None.\n\r" );

    for( pArea = area_first; pArea; pArea = pArea->next )
    {
      /* Builder must be assigned this area. */
      if ( !IS_BUILDER( ch, pArea ) ) continue;

      if (IS_SET(pArea->area_flags,AREA_SAVELOCK))
      {
        ptc(ch,"Area %s was not saved! (Locked from saving)\n\r",ch);
        continue;
      }
      /* Save changed areas. */
      if ( IS_SET(pArea->area_flags, AREA_CHANGED) )
      {
        REM_BIT( pArea->area_flags, AREA_CHANGED );
        save_area( pArea );
        ptc(ch, "%24s - '%s'\n\r", pArea->name, pArea->file_name );
      }
    }
    if ( !str_cmp( buf, "None.\n\r" ) ) stc( buf, ch );
    return;
  }

  // Save the area.lst file
  if ( !str_cmp( arg1, "list" ) )
  {
    save_area_list();
    return;
  }

  // Save area being edited, if authorized
  if ( !str_cmp( arg1, "area" ) )
  {
    // Is character currently editing
    if ( ch->desc->editor == 0 ) pArea=ch->in_room->area;
    else switch (ch->desc->editor)
    {
      case ED_AREA:
        pArea = (AREA_DATA *)ch->desc->pEdit;
        break;
      case ED_ROOM:
        pArea = ch->in_room->area;
        break;
      case ED_OBJECT:
        pArea = ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area;
        break;
      case ED_MOBILE:
        pArea = ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area;
        break;
      default:
        pArea = ch->in_room->area;
        break;
    }

    if (IS_SET(pArea->area_flags,AREA_SAVELOCK))
    {
      ptc(ch,"Area %s was not saved! (Locked from saving)\n\r",ch);
      return;
    }
    if (!pArea || !IS_BUILDER( ch, pArea ) )
    {
      stc( "У вас нет прав изменять эту арию.\n\r", ch );
      return;
    }

    save_area_list();
    REM_BIT( pArea->area_flags, AREA_CHANGED );
    save_area( pArea );
    stc( "Area saved.\n\r", ch );
    return;
  }
  do_asave( ch, "" );
}
