// $Id: olc_act.c,v 1.26 2003/10/29 08:40:52 wagner Exp $
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
#include "recycle.h"

char * mprog_type_to_name ( int64 type );
void  fix_local     args( ( void ) );

/* Return TRUE if area changed, FALSE if not. */
#define REDIT( fun )      bool fun( CHAR_DATA *ch, const char *argument )
#define OEDIT( fun )      bool fun( CHAR_DATA *ch, const char *argument )
#define MEDIT( fun )      bool fun( CHAR_DATA *ch, const char *argument )
#define AEDIT( fun )      bool fun( CHAR_DATA *ch, const char *argument )

struct olc_help_type
{
  const char *command;
  const void *structure;
  const char *desc;
};

bool show_version( CHAR_DATA *ch, const char *argument )
{
  stc( VERSION, ch );
  stc( "\n\r", ch );
  stc( AUTHOR, ch );
  stc( "\n\r", ch );
  stc( DATE, ch );
  stc( "\n\r", ch );
  stc( CREDITS, ch );
  stc( "\n\r", ch );
  return FALSE;
}  

// This table contains help commands and a brief description of each.
const struct olc_help_type help_table[] =
{
  {   "area",         area_flags,      "Аттрибуты арий."               },
  {   "room",         room_flags,      "Аттрибуты комнат."             },
  {   "sector",       sector_flags,    "Типы земель, секторов."        },
  {   "exit",         exit_flags,      "Типы выходов."                 },
  {   "type",         type_flags,      "Типы предметов."               },
  {   "extra",        extra_flags,     "Аттрибуты предметов."          },
  {   "wear",         wear_flags,      "Куда одевать предмет."         },
  {   "spec",         spec_table,      "Доступные спец. программы."    },
  {   "sex",          sex_flags,       "Пол."                          },
  {   "act",          act_flags,       "Аттрибуты монстров."           },
  {   "affect",       affect_flags,    "Влияния на монстрах."          },
  {   "wear-loc",     wear_loc_flags,  "Куда монстры одевают предметы."},
  {   "spells",       skill_table,     "Имена текущих заклинаний."     },
  {   "weapon",       weapon_flags,    "Типы оружия."                  },
  {   "container",    container_flags, "Статус контейнеров."           },

  {   "armor",        ac_type,         "Защита от разных атак."        },
  {   "apply",        apply_flags,     "Apply флаги"                   },
  {   "form",         form_flags,      "Формы тела монстров."          },
  {   "imm",          imm_flags,       "Иммунитеты монстров."          },
  {   "res",          res_flags,       "Сопротивляемость монстров."    },
  {   "vuln",         vuln_flags,      "Уязвимость монстров."          },
  {   "off",          off_flags,       "Типы поведения монстров."      },
  {   "size",         size_flags,      "Размеры монстров."             },
  {   "position",     position_flags,  "Позиции монстров."             },
//  {   "material",     material_type,   "Материалы."                    },
  {   "wclass",       weapon_class,    "Класс оружия."                 }, 
  {   "wtype",        weapon_type2,    "Специальные типы оружия."      },
  {   "portal",       portal_flags,    "Типы порталов."                },
  {   "furniture",    furniture_flags, "Типы диванов и лежаков."       },
  {   "mprog",        mprog_flags,     "Флаги MobProgram."             },
  {   NULL,           NULL,            NULL                            }
};

/*****************************************************************************
 Name:          show_flag_cmds
 Purpose:       Displays settable flags and stats.
 Called by:     show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
  char buf  [ MAX_STRING_LENGTH ];
  char buf1 [ MAX_STRING_LENGTH ];
  int flag;
  int  col;

  buf1[0] = '\0';
  col = 0;
  for (flag = 0; flag_table[flag].name != NULL; flag++)
  {
    if ( flag_table[flag].settable )
    {
      do_printf( buf, "%-19s", flag_table[flag].name );
      strcat( buf1, buf );
      if ( ++col % 4 == 0 )
        strcat( buf1, "\n\r" );
    }
  }
  if ( col % 4 != 0 ) strcat( buf1, "\n\r" );
  stc( buf1, ch );
}

/*****************************************************************************
 Name:          show_skill_cmds
 Purpose:       Displays all skill functions.
                Does remove those damn immortal commands from the list.
                Could be improved by:
                (1) Adding a check for a particular class.
                (2) Adding a check for a level range.
 Called by:     show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds( CHAR_DATA *ch, int tar )
{
  char buf  [ MAX_STRING_LENGTH ];
  char buf1 [ MAX_STRING_LENGTH*2 ];
  int  sn;
  int  col;

  buf1[0] = '\0';
  col = 0;
  for (sn = 0; sn < max_skill; sn++)
  {
    if ( !skill_table[sn].name ) break;

    if ( !str_cmp( skill_table[sn].name, "reserved" )
    || skill_table[sn].spell_fun == spell_null ) continue;

    if ( tar == -1 || skill_table[sn].target == tar )
    {
      do_printf( buf, "%-19s", skill_table[sn].name );
      strcat( buf1, buf );
      if ( ++col % 4 == 0 ) strcat( buf1, "\n\r" );
    }
  }
  if ( col % 4 != 0 ) strcat( buf1, "\n\r" );
  stc( buf1, ch );
}



/*****************************************************************************
 Name:          show_spec_cmds
 Purpose:       Displays settable special functions.
 Called by:     show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds( CHAR_DATA *ch )
{
  char buf  [ MAX_STRING_LENGTH ];
  char buf1 [ MAX_STRING_LENGTH ];
  int  spec;
  int  col;

  buf1[0] = '\0';
  col = 0;
  stc( "Все специальные функции начинаются с 'spec_'\n\r\n\r", ch );
  for (spec = 0; spec_table[spec].function != NULL; spec++)
  {
      do_printf( buf, "%-19s", &spec_table[spec].name[5] );
      strcat( buf1, buf );
      if ( ++col % 4 == 0 ) strcat( buf1, "\n\r" );
  }

  if ( col % 4 != 0 ) strcat( buf1, "\n\r" );

  stc( buf1, ch );
}

/*****************************************************************************
 Name:          show_help
 Purpose:       Displays help for many tables used in OLC.
 Called by:     olc interpreters.
 ****************************************************************************/
bool show_help( CHAR_DATA *ch, const char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char spell[MAX_INPUT_LENGTH];
  int cnt;

  argument = one_argument( argument, arg );
  one_argument( argument, spell );

  /* Display syntax. */
  if ( arg[0] == '\0' )
  {
      stc( "Синтаксис: ? [комманда]\n\r\n\r", ch );
      stc( "[комманда]  [описание]\n\r", ch );
      for (cnt = 0; help_table[cnt].command != NULL; cnt++)
      {
          do_printf( buf, "%-10s -%s\n\r",
              capitalize( help_table[cnt].command ),
              help_table[cnt].desc );
          stc( buf, ch );
      }
      return FALSE;
  }

  // Find the command, show changeable data.
  for (cnt = 0; help_table[cnt].command != NULL; cnt++)
  {
    if (  arg[0] == help_table[cnt].command[0]
      && !str_prefix( arg, help_table[cnt].command ) )
    {
      if ( help_table[cnt].structure == spec_table )
      {
        show_spec_cmds( ch );
        return FALSE;
      }
      else
      if ( help_table[cnt].structure == skill_table )
      {
        if ( spell[0] == '\0' )
        {
            stc( "Синтаксис:  ? spells "
                "[ignore/attack/defend/self/object/all]\n\r", ch );
            return FALSE;
        }

        if ( !str_prefix( spell, "all" ) )
             show_skill_cmds( ch, -1 );
        else if ( !str_prefix( spell, "ignore" ) )
             show_skill_cmds( ch, TAR_IGNORE );
        else if ( !str_prefix( spell, "attack" ) )
             show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
        else if ( !str_prefix( spell, "defend" ) )
             show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
        else if ( !str_prefix( spell, "self" ) )
             show_skill_cmds( ch, TAR_CHAR_SELF );
        else if ( !str_prefix( spell, "object" ) )
             show_skill_cmds( ch, TAR_OBJ_INV );
        else stc( "Syntax:  ? spell "
                "[ignore/attack/defend/self/object/all]\n\r", ch );
        return FALSE;
      }
      else
      {
          show_flag_cmds( ch, help_table[cnt].structure );
          return FALSE;
      }
    }
  }
  show_help( ch, "" );
  return FALSE;
}

REDIT( redit_rlist )
{
   ROOM_INDEX_DATA     *pRoomIndex;
   AREA_DATA           *pArea;
   char                buf  [ MAX_STRING_LENGTH   ];
   BUFFER              *buf1;
   char                arg  [ MAX_INPUT_LENGTH    ];
   bool found;
   int64 vnum;
   int  col = 0;

   one_argument( argument, arg );

   pArea = ch->in_room->area;
   buf1=new_buf();
   found   = FALSE;

   for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
   {
     if ( ( pRoomIndex = get_room_index( vnum ) ) )
     {
       found = TRUE;
       do_printf( buf, "[%7u] %-17s",vnum, capitalize( pRoomIndex->name ) );
       add_buf( buf1, buf );
       if ( ++col % 3 == 0 ) add_buf( buf1, "\n\r" );
     }
   }

   if ( !found )
   {
     stc( "Комната(комнаты) в этой арии не обнаружены.\n\r", ch);
     return FALSE;
   }

   if ( col % 3 != 0 ) add_buf( buf1, "\n\r" );

   page_to_char( buf_string(buf1), ch );
   free_buf(buf1);
   return FALSE;
}

REDIT( redit_mlist )
{
  MOB_INDEX_DATA      *pMobIndex;
  AREA_DATA           *pArea;
  char                buf  [ MAX_STRING_LENGTH   ];
  BUFFER              *buf1;
  char                arg  [ MAX_INPUT_LENGTH    ];
  bool fAll, found;
  int64 vnum;
  int  col = 0;

  one_argument( argument, arg );
  if ( arg[0] == '\0' )
  {
      stc( "Синтаксис:  mlist <all/имя>\n\r", ch );
      return FALSE;
  }

  buf1=new_buf();
  pArea = ch->in_room->area;
  fAll    = !str_cmp( arg, "all" );
  found   = FALSE;

  for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
  {
      if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
      {
          if ( fAll || is_name( arg, pMobIndex->player_name ) )
          {
              found = TRUE;
              do_printf( buf, "[%7u] ",pMobIndex->vnum);
              add_buf( buf1, buf );
              do_printf( buf, "%-17s",capitalize(get_mobindex_desc(pMobIndex,'1')));
              add_buf( buf1, buf );
              if ( ++col % 3 == 0 ) add_buf( buf1, "\n\r" );
          }
      }
  }

  if ( !found )
  {
      stc( "Монстр(монстры) в этой арии не найдены.\n\r", ch);
      return FALSE;
  }

  if ( col % 3 != 0 ) add_buf( buf1, "\n\r" );

  page_to_char( buf_string(buf1), ch );
  free_buf(buf1);
  return FALSE;
}

REDIT( redit_olist )
{
  OBJ_INDEX_DATA      *pObjIndex;
  AREA_DATA           *pArea;
  char                buf  [ MAX_STRING_LENGTH   ];
  BUFFER              *buf1;
  char                arg  [ MAX_INPUT_LENGTH    ];
  bool fAll, found;
  int64 vnum;
  int  col = 0;

  one_argument( argument, arg );
  if ( arg[0] == '\0' )
  {
      stc( "Синтаксис:  olist <all/имя/тип_предмета>\n\r", ch );
      return FALSE;
  }

  pArea = ch->in_room->area;
  buf1=new_buf();
  fAll    = !str_cmp( arg, "all" );
  found   = FALSE;

  for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
  {
      if ( ( pObjIndex = get_obj_index( vnum ) ) )
      {
          if ( fAll || is_name( arg, pObjIndex->name )
          || flag_value( type_flags, arg ) == pObjIndex->item_type )
          {
              found = TRUE;
              do_printf( buf, "[%7u] ",pObjIndex->vnum);
              add_buf( buf1, buf );
              do_printf( buf, "%-17s",capitalize( pObjIndex->short_descr ) );
              add_buf( buf1, buf );
              if ( ++col % 3 == 0 ) add_buf( buf1, "\n\r" );
          }
      }
  }

  if ( !found )
  {
      stc( "Предмет(предметы) в этой арии не найдены.\n\r", ch);
      return FALSE;
  }

  if ( col % 3 != 0 ) add_buf( buf1, "\n\r" );

  page_to_char( buf_string(buf1), ch );
  free_buf(buf1);
  return FALSE;
}

REDIT( redit_mshow )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] == '\0' )
  {
      stc( "Синтаксис:  mshow <vnum>\n\r", ch );
      return FALSE;
  }

  if ( !is_number( argument ) )
  {
     stc( "REdit: Аргумент должен быть числом.\n\r", ch);
     return FALSE;
  }

  if ( is_number( argument ) )
  {
      value = atoi64( argument );
      if ( !( pMob = get_mob_index( value ) ))
      {
          stc( "REdit:  Этот монстр не существует.\n\r", ch );
          return FALSE;
      }

      ch->desc->pEdit = (void *)pMob;
  }

  medit_show( ch, argument );
  ch->desc->pEdit = (void *)ch->in_room;
  return FALSE; 
}

REDIT( redit_oshow )
{
  OBJ_INDEX_DATA *pObj;
  int64 value;

  if ( argument[0] == '\0' )
  {
      stc( "Синтаксис:  oshow <vnum>\n\r", ch );
      return FALSE;
  }

  if ( !is_number( argument ) )
  {
     stc( "REdit: Аргумент должен быть числом.\n\r", ch);
     return FALSE;
  }

  if ( is_number( argument ) )
  {
      value = atoi64( argument );
      if ( !( pObj = get_obj_index( value ) ))
      {
          stc( "REdit:  Такого предмета не существует.\n\r", ch );
          return FALSE;
      }

      ch->desc->pEdit = (void *)pObj;
  }

  oedit_show( ch, argument );
  ch->desc->pEdit = (void *)ch->in_room;
  return FALSE; 
}

/*****************************************************************************
 Name:          check_range( lower vnum, upper vnum )
 Purpose:       Ensures the range spans only one area.
 Called by:     aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range( int64 lower, int64 upper )
{
  AREA_DATA *pArea;
  int cnt = 0;

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
      // lower < area < upper
      if ( ( lower <= pArea->min_vnum && pArea->min_vnum <= upper )
      ||   ( lower <= pArea->max_vnum && pArea->max_vnum <= upper ) )
          ++cnt;

      if ( cnt > 1 )
          return FALSE;
  }
  return TRUE;
}

AREA_DATA *get_vnum_area( int64 vnum )
{
  AREA_DATA *pArea;

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
      if ( vnum >= pArea->min_vnum
        && vnum <= pArea->max_vnum )
          return pArea;
  }
  return 0;
}

// Area Editor Functions.
AEDIT( aedit_show )
{
  AREA_DATA *pArea;
  char buf  [MAX_STRING_LENGTH];

  EDIT_AREA(ch, pArea);

  do_printf( buf, "Name:     [%5d] %s\n\r", pArea->vnum, pArea->name );
  stc( buf, ch );

  ptc(ch, "File:    [%s]\n\r", pArea->file_name );
  ptc(ch, "Vnums:   [%u-%u]\n\r", pArea->min_vnum, pArea->max_vnum );
  ptc(ch, "Age:     [%d]\n\r", pArea->age );
  ptc(ch, "Players: [%d]\n\r", pArea->nplayer );
  ptc(ch, "Security:[%d]\n\r", pArea->security );
  ptc(ch, "Builders:[%s]\n\r", pArea->builders );
  ptc(ch, "Credits :[%s]\n\r", pArea->credits );
  ptc(ch, "Clan    :[%s]\n\r", pArea->clan );
  ptc(ch, "Flags:   [%s]\n\r", flag_string(area_flags,pArea->area_flags));

  return FALSE;
}

AEDIT( aedit_reset )
{
  AREA_DATA *pArea;

  EDIT_AREA(ch, pArea);

  reset_area( pArea );
  stc( "Ария сброшена.\n\r", ch );

  return FALSE;
}

AEDIT( aedit_create )
{
  AREA_DATA *pArea;

  pArea               = new_area();
  area_last->next     = pArea;
  area_last           = pArea;      /* Thanks, Walker. */
  ch->desc->pEdit     = (void *)pArea;

  SET_BIT( pArea->area_flags, AREA_ADDED );
  stc( "Aрия создана.\n\r", ch );
  return FALSE;
}

AEDIT( aedit_name )
{
  AREA_DATA *pArea;

  EDIT_AREA(ch, pArea);

  if ( argument[0] == '\0' )
  {
      stc( "Синтаксис:   name [$имя]\n\r", ch );
      return FALSE;
  }

  free_string( pArea->name );
  pArea->name = str_dup( argument );

  stc( "Имя присвоено.\n\r", ch );
  return TRUE;
}

AEDIT( aedit_clan )
{
  AREA_DATA *pArea;
  CLAN_DATA *clan;

  EDIT_AREA(ch, pArea);

  if ( argument[0] == '\0' )
  {
      stc( "Синтаксис:   clan [<clan_name>]\n\r", ch );
      return FALSE;
  }

  free_string( pArea->clan );
  clan=clan_lookup(argument);
  if (!clan)
  {
   pArea->clan = str_dup( "none" );
   stc( "Ария теперь не принадлежит никакому клану.\n\r", ch );
  }
  else
  {
   pArea->clan = str_dup( clan->name);
   stc( "Имя присвоено.\n\r", ch );
  }
  return TRUE;
}

AEDIT( aedit_credits )
{
  AREA_DATA *pArea;

  EDIT_AREA(ch, pArea);

  if ( argument[0] == '\0' )
  {
      stc( "Синтаксис:   credits [$credits]\n\r", ch );
      return FALSE;
  }

  free_string( pArea->credits );
  pArea->credits = str_dup( argument );

  stc( "Копирайт установлен.\n\r", ch );
  return TRUE;
}

AEDIT( aedit_flags )
{
  AREA_DATA *pArea;

  EDIT_AREA(ch, pArea);

  if ( argument[0] == '\0' )
  {
      stc( "Синтаксис:   flag NOQUEST|NOREFORM|WIZLOCK|SAVELOCK\n\r", ch );
      return FALSE;
  }

  if (!str_prefix(argument, "noquest"))
  {
   if (IS_SET(pArea->area_flags, AREA_NOQUEST))
   {
    REM_BIT(pArea->area_flags, AREA_NOQUEST);
    stc("NOQUEST flag removed.\n\r",ch);
   }
   else
   {
    SET_BIT(pArea->area_flags, AREA_NOQUEST);
    stc("NOQUEST flag enabled.\n\r",ch);
   }
   return TRUE;
  }

  if (!str_prefix(argument, "noreform"))
  {
   if (IS_SET(pArea->area_flags, AREA_NOREFORM))
   {
    REM_BIT(pArea->area_flags, AREA_NOREFORM);
    stc("NOREFORM flag removed.\n\r",ch);
   }
   else
   {
    SET_BIT(pArea->area_flags, AREA_NOREFORM);
    stc("NOREFORM flag enabled.\n\r",ch);
   }
   return TRUE;
  }

  if (!str_prefix(argument, "wizlock"))
  {
   if (IS_SET(pArea->area_flags, AREA_WIZLOCK))
   {
    REM_BIT(pArea->area_flags, AREA_WIZLOCK);
    stc("WIZLOCK flag removed.\n\r",ch);
   }
   else
   {
    SET_BIT(pArea->area_flags, AREA_WIZLOCK);
    stc("WIZLOCK flag set.\n\r",ch);
   }
   return TRUE;
  }

  if (!str_prefix(argument, "savelock"))
  {
   if (IS_SET(pArea->area_flags, AREA_SAVELOCK))
   {
    REM_BIT(pArea->area_flags, AREA_SAVELOCK);
    stc("SAVELOCK flag removed.\n\r",ch);
   }
   else
   {
    SET_BIT(pArea->area_flags, AREA_SAVELOCK);
    stc("SAVELOCK flag set.\n\r",ch);
   }
   return TRUE;
  }

 stc( "Синтаксис:   flag <flag> или без параметра для списка\n\r", ch );
 return FALSE;
}

AEDIT( aedit_file )
{
    AREA_DATA *pArea;
    char file[MAX_STRING_LENGTH];
    int i, length;

    EDIT_AREA(ch, pArea);

    one_argument( argument, file );     /* Forces Lowercase */

    if ( argument[0] == '\0' )
    {
        stc( "Синтаксис:  filename [$имя_file]\n\r", ch );
        return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
    length = strlen( argument );
    if ( length > 8 )
    {
        stc( "No more than eight characters allowed.\n\r", ch );
        return FALSE;
    }
    
    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
        if ( !isalnum( file[i] ) )
        {
            stc( "Only letters and numbers are valid.\n\r", ch );
            return FALSE;
        }
    }    

    free_string( pArea->file_name );
    strcat( file, ".are" );
    pArea->file_name = str_dup( file );

    stc( "Filename set.\n\r", ch );
    return TRUE;
}



AEDIT( aedit_age )
{
    AREA_DATA *pArea;
    char age[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, age );

    if ( !is_number( age ) || age[0] == '\0' )
    {
        stc( "Syntax:  age [#xage]\n\r", ch );
        return FALSE;
    }

    pArea->age = atoi( age );

    stc( "Age set.\n\r", ch );
    return TRUE;
}


AEDIT( aedit_security )
{
  AREA_DATA *pArea;
  char sec[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  value;

  EDIT_AREA(ch, pArea);

  one_argument( argument, sec );

  if ( !is_number( sec ) || sec[0] == '\0' )
  {
      stc( "Syntax:  security [#xlevel]\n\r", ch );
      return FALSE;
  }

  value = atoi( sec );

  if ( value > ch->pcdata->security || value < 0 )
  {
      if ( ch->pcdata->security != 0 )
      {
          do_printf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
          stc( buf, ch );
      }
      else
          stc( "Security is 0 only.\n\r", ch );
      return FALSE;
  }

  pArea->security = value;

  stc( "Security set.\n\r", ch );
  return TRUE;
}

AEDIT( aedit_builder )
{
  AREA_DATA *pArea;

  EDIT_AREA(ch, pArea);

  if ( EMPTY(argument))
  {
    stc( "Syntax:  builder names\n\r", ch );
    stc( "Syntax:  builder none\n\r", ch );
    return FALSE;
  }
  free_string(pArea->builders);

  if (!str_cmp(argument,"none")) pArea->builders=str_dup("none");
  else                           pArea->builders=str_dup(argument);
  return TRUE;
}

AEDIT( aedit_vnum )
{
  AREA_DATA *pArea;
  char lower[MAX_STRING_LENGTH];
  char upper[MAX_STRING_LENGTH];
  int64  ilower;
  int64  iupper;

  EDIT_AREA(ch, pArea);

  argument = one_argument( argument, lower );
  one_argument( argument, upper );

  if ( !is_number( lower ) || lower[0] == '\0'
  || !is_number( upper ) || upper[0] == '\0' )
  {
      stc( "Syntax:  vnum [#xlower] [#xupper]\n\r", ch );
      return FALSE;
  }

  if ( ( ilower = atoi64( lower ) ) > ( iupper = atoi64( upper ) ) )
  {
      stc( "AEdit:  Upper must be larger then lower.\n\r", ch );
      return FALSE;
  }
  
  if ( !check_range( atoi64( lower ), atoi64( upper ) ) )
  {
      stc( "AEdit:  Range must include only this area.\n\r", ch );
      return FALSE;
  }

  if ( get_vnum_area( ilower )
  && get_vnum_area( ilower ) != pArea )
  {
      stc( "AEdit:  Lower vnum already assigned.\n\r", ch );
      return FALSE;
  }

  pArea->min_vnum = ilower;
  stc( "Lower vnum set.\n\r", ch );

  if ( get_vnum_area( iupper )
  && get_vnum_area( iupper ) != pArea )
  {
      stc( "AEdit:  Upper vnum already assigned.\n\r", ch );
      return TRUE;    /* The lower value has been set. */
  }

  pArea->max_vnum = iupper;
  stc( "Upper vnum set.\n\r", ch );

  return TRUE;
}

AEDIT( aedit_lvnum )
{
  AREA_DATA *pArea;
  char lower[MAX_STRING_LENGTH];
  int64  ilower;
  int64  iupper;

  EDIT_AREA(ch, pArea);

  one_argument( argument, lower );

  if ( !is_number( lower ) || lower[0] == '\0' )
  {
      stc( "Syntax:  min_vnum [#xlower]\n\r", ch );
      return FALSE;
  }

  if ( ( ilower = atoi64( lower ) ) > ( iupper = pArea->max_vnum ) )
  {
      stc( "AEdit:  Value must be less than the max_vnum.\n\r", ch );
      return FALSE;
  }
  
  if ( !check_range( ilower, iupper ) )
  {
      stc( "AEdit:  Range must include only this area.\n\r", ch );
      return FALSE;
  }

  if ( get_vnum_area( ilower )
  && get_vnum_area( ilower ) != pArea )
  {
      stc( "AEdit:  Lower vnum already assigned.\n\r", ch );
      return FALSE;
  }

  pArea->min_vnum = ilower;
  stc( "Lower vnum set.\n\r", ch );
  return TRUE;
}

AEDIT( aedit_uvnum )
{
  AREA_DATA *pArea;
  char upper[MAX_STRING_LENGTH];
  int64  ilower;
  int64  iupper;

  EDIT_AREA(ch, pArea);

  one_argument( argument, upper );

  if ( !is_number( upper ) || upper[0] == '\0' )
  {
      stc( "Syntax:  max_vnum [#xupper]\n\r", ch );
      return FALSE;
  }

  if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi64( upper ) ) )
  {
      stc( "AEdit:  Upper must be larger then lower.\n\r", ch );
      return FALSE;
  }
  
  if ( !check_range( ilower, iupper ) )
  {
      stc( "AEdit:  Range must include only this area.\n\r", ch );
      return FALSE;
  }

  if ( get_vnum_area( iupper )
  && get_vnum_area( iupper ) != pArea )
  {
      stc( "AEdit:  Upper vnum already assigned.\n\r", ch );
      return FALSE;
  }

  pArea->max_vnum = iupper;
  stc( "Upper vnum set.\n\r", ch );

  return TRUE;
}

REDIT( redit_show )
{
  ROOM_INDEX_DATA     *pRoom;
  char                buf  [MAX_STRING_LENGTH];
  char                buf1 [2*MAX_STRING_LENGTH];
  OBJ_DATA            *obj;
  CHAR_DATA           *rch;
  int                 door;
  bool                fcnt;
  
  EDIT_ROOM(ch, pRoom);

  buf1[0] = '\0';
  
  do_printf( buf, "Description:\n\r%s", pRoom->description );
  strcat( buf1, buf );

  do_printf( buf, "Name:       [%s]\n\rArea:       [%5d] %s\n\r",
          pRoom->name, pRoom->area->vnum, pRoom->area->name );
  strcat( buf1, buf );

  do_printf( buf, "Vnum:       [%u]\n\r",pRoom->vnum);
  strcat( buf1, buf );

  do_printf(buf,"Sector:     [%s]\n\r",
          flag_string( sector_flags, pRoom->sector_type ) );
  strcat( buf1, buf );

  do_printf( buf, "Room flags: [%s]\n\r",
          flag_string( room_flags, pRoom->room_flags ) );
  strcat( buf1, buf );

  do_printf( buf, "Health recovery:[%d]\n\rMana recovery  :[%d]\n\r",
          pRoom->heal_rate , pRoom->mana_rate );
  strcat( buf1, buf );
      
  if ( pRoom->extra_descr )
  {
      EXTRA_DESCR_DATA *ed;

      strcat( buf1, "Desc Kwds:  [" );
      for ( ed = pRoom->extra_descr; ed; ed = ed->next )
      {
          strcat( buf1, ed->keyword );
          if ( ed->next )
              strcat( buf1, " " );
      }
      strcat( buf1, "]\n\r" );
  }

  strcat( buf1, "Characters: [" );
  fcnt = FALSE;
  for ( rch = pRoom->people; rch; rch = rch->next_in_room )
  {
      one_argument( rch->name, buf );
      strcat( buf1, buf );
      strcat( buf1, " " );
      fcnt = TRUE;
  }

  if ( fcnt )
  {
      int end;

      end = strlen(buf1) - 1;
      buf1[end] = ']';
      strcat( buf1, "\n\r" );
  }
  else
      strcat( buf1, "none]\n\r" );

  strcat( buf1, "Objects:    [" );
  fcnt = FALSE;
  for ( obj = pRoom->contents; obj; obj = obj->next_content )
  {
      one_argument( obj->name, buf );
      strcat( buf1, buf );
      strcat( buf1, " " );
      fcnt = TRUE;
  }

  if ( fcnt )
  {
      int end;

      end = strlen(buf1) - 1;
      buf1[end] = ']';
      strcat( buf1, "\n\r" );
  }
  else
      strcat( buf1, "none]\n\r" );

  for ( door = 0; door < MAX_DIR; door++ )
  {
      EXIT_DATA *pexit;

      if ( ( pexit = pRoom->exit[door] ) )
      {
          char word[MAX_INPUT_LENGTH];
          char reset_state[MAX_STRING_LENGTH];
          const char *state;
          int i, length;

          do_printf( buf, "-%-5s to [%7u] ",
              capitalize(dir_name[door]),
              pexit->u1.to_room ? pexit->u1.to_room->vnum : 0);
          strcat( buf1, buf );
          do_printf( buf, "Key: [%7u] ",pexit->key );
          strcat( buf1, buf );

          /*
           * Format up the exit info.
           * Capitalize all flags that are not part of the reset info.
           */
          strcpy( reset_state, flag_string( exit_flags, pexit->rs_flags ) );
          state = flag_string( exit_flags, pexit->exit_info );
          strcat( buf1, " Exit flags: [" );
          for (; ;)
          {
              state = one_argument( state, word );

              if ( word[0] == '\0' )
              {
                  int end;

                  end = strlen(buf1) - 1;
                  buf1[end] = ']';
                  strcat( buf1, "\n\r" );
                  break;
              }

              if ( str_infix( word, reset_state ) )
              {
                  length = strlen(word);
                  for (i = 0; i < length; i++)
                      word[i] = UPPER(word[i]);
              }
              strcat( buf1, word );
              strcat( buf1, " " );
          }

          if ( pexit->keyword && pexit->keyword[0] != '\0' )
          {
              do_printf( buf, "Kwds: [%s]\n\r", pexit->keyword );
              strcat( buf1, buf );
          }
          if ( pexit->description && pexit->description[0] != '\0' )
          {
              do_printf( buf, "%s", pexit->description );
              strcat( buf1, buf );
          }
      }
  }

  stc( buf1, ch );
  return FALSE;
}

bool change_exit( CHAR_DATA *ch, const char *argument, int door )
{
  ROOM_INDEX_DATA *pRoom;
  char command[MAX_INPUT_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int64  value;

  EDIT_ROOM(ch, pRoom);

  // Set the exit flags, needs full argument.
  // ----------------------------------------
  if ( ( value = flag_value( exit_flags, argument ) ) != NO_FLAG )
  {
      ROOM_INDEX_DATA *pToRoom;
      int rev;

      if ( !pRoom->exit[door] )
      {
        stc("Здесь нет двери.\n\r",ch);
        return FALSE;
      }

      // This room.
      TOGGLE_BIT(pRoom->exit[door]->rs_flags,  value);
      // Don't toggle exit_info because it can be changed by players.
      pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

      // Connected room.
      pToRoom = pRoom->exit[door]->u1.to_room;
      rev = rev_dir[door];

      if (pToRoom->exit[rev] != NULL)
      {
         TOGGLE_BIT(pToRoom->exit[rev]->rs_flags,  value);
         TOGGLE_BIT(pToRoom->exit[rev]->exit_info, value);
      }

      stc( "Exit flag toggled.\n\r", ch );
      return TRUE;
  }

  // Now parse the arguments.
  argument = one_argument( argument, command );
  one_argument( argument, arg );

  if ( command[0] == '\0' && argument[0] == '\0' )    /* Move command. */
  {
    do_move_char( ch, door, TRUE, FALSE );
    return FALSE;
  }

  if ( command[0] == '?' )
  {
    do_help( ch, "EXIT" );
    return FALSE;
  }

  if ( !str_cmp( command, "delete" ) )
  {
      ROOM_INDEX_DATA *pToRoom;
      int rev; 
      
      if ( !pRoom->exit[door] )
      {
          stc( "REdit:  Cannot delete a null exit.\n\r", ch );
          return FALSE;
      }

      // Remove ToRoom Exit.
      rev = rev_dir[door];
      pToRoom = pRoom->exit[door]->u1.to_room;
      
      if ( pToRoom->exit[rev] )
      {
          free_exit( pToRoom->exit[rev] );
          pToRoom->exit[rev] = NULL;
      }

      // Remove this exit.
      free_exit( pRoom->exit[door] );
      pRoom->exit[door] = NULL;

      stc( "Exit unlinked.\n\r", ch );
      return TRUE;
  }

  if ( !str_cmp( command, "link" ) )
  {
    EXIT_DATA *pExit;

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
        stc( "Syntax:  [direction] link [vnum]\n\r", ch );
        return FALSE;
    }

    value = atoi64( arg );

    if ( !get_room_index( value ) )
    {
        stc( "REdit:  Cannot link to non-existant room.\n\r", ch );
        return FALSE;
    }

    if ( !IS_BUILDER( ch, get_room_index( value )->area ) )
    {
        stc( "REdit:  Cannot link to that area.\n\r", ch );
        return FALSE;
    }

    if ( get_room_index( value )->exit[rev_dir[door]] )
    {
        stc( "REdit:  Remote side's exit already exists.\n\r", ch );
        return FALSE;
    }

    if ( !pRoom->exit[door] )
    {
        pRoom->exit[door] = new_exit();
    }

    pRoom->exit[door]->u1.to_room = get_room_index( value );
    pRoom->exit[door]->orig_door = door;
    
    pRoom                   = get_room_index( value );
    door                    = rev_dir[door];
    pExit                   = new_exit();
    pExit->u1.to_room       = ch->in_room;
    pExit->orig_door        = door;
    pRoom->exit[door]       = pExit;

    stc( "Two-way link established.\n\r", ch );
    return TRUE;
  }
      
  if ( !str_cmp( command, "dig" ) )
  {
      char buf[MAX_STRING_LENGTH];
      
      if ( arg[0] == '\0' || !is_number( arg ) )
      {
          stc( "Syntax: [direction] dig <vnum>\n\r", ch );
          return FALSE;
      }
      
      redit_create( ch, arg );
      do_printf( buf, "link %s", arg );
      change_exit( ch, buf, door);
      return TRUE;
  }

  if ( !str_cmp( command, "room" ) )
  {
      if ( arg[0] == '\0' || !is_number( arg ) )
      {
          stc( "Syntax:  [direction] room [vnum]\n\r", ch );
          return FALSE;
      }

      if ( !pRoom->exit[door] )
      {
          pRoom->exit[door] = new_exit();
      }

      value = atoi64( arg );

      if ( !get_room_index( value ) )
      {
          stc( "REdit:  Cannot link to non-existant room.\n\r", ch );
          return FALSE;
      }

      pRoom->exit[door]->u1.to_room = get_room_index( value );
      pRoom->exit[door]->orig_door = door;

      stc( "One-way link established.\n\r", ch );
      return TRUE;
  }

  if ( !str_cmp( command, "key" ) )
  {
      if ( arg[0] == '\0' || !is_number( arg ) )
      {
          stc( "Syntax:  [direction] key [vnum]\n\r", ch );
          return FALSE;
      }

      if ( !pRoom->exit[door] )
      {
          pRoom->exit[door] = new_exit();
      }

      value = atoi64( arg );

      if ( !get_obj_index( value ) )
      {
          stc( "REdit:  Item doesn't exist.\n\r", ch );
          return FALSE;
      }

      if ( get_obj_index( atoi64( argument ) )->item_type != ITEM_KEY )
      {
          stc( "REdit:  Key doesn't exist.\n\r", ch );
          return FALSE;
      }

      pRoom->exit[door]->key = value;

      stc( "Exit key set.\n\r", ch );
      return TRUE;
  }

  if ( !str_cmp( command, "name" ) )
  {
      if ( arg[0] == '\0' )
      {
          stc( "Syntax:  [direction] name [string]\n\r", ch );
          return FALSE;
      }

      if ( !pRoom->exit[door] )
      {
          pRoom->exit[door] = new_exit();
      }

      free_string( pRoom->exit[door]->keyword );
      pRoom->exit[door]->keyword = str_dup( arg );

      stc( "Exit name set.\n\r", ch );
      return TRUE;
  }

  if ( !str_prefix( command, "description" ) )
  {
      if ( arg[0] == '\0' )
      {
          if ( !pRoom->exit[door] )
          {
              pRoom->exit[door] = new_exit();
          }
          string_append( ch, &pRoom->exit[door]->description );
          return TRUE;
      }

      stc( "Syntax:  [direction] desc\n\r", ch );
      return FALSE;
  }
  return FALSE;
}

REDIT( redit_north )
{
  if (change_exit(ch,argument,DIR_NORTH)) return TRUE;
  return FALSE;
}

REDIT( redit_south )
{
  if (change_exit(ch,argument,DIR_SOUTH)) return TRUE;
  return FALSE;
}

REDIT( redit_east )
{
  if (change_exit(ch,argument,DIR_EAST)) return TRUE;
  return FALSE;
}

REDIT( redit_west )
{
  if (change_exit(ch,argument,DIR_WEST)) return TRUE;
  return FALSE;
}

REDIT( redit_up )
{
  if (change_exit(ch,argument,DIR_UP)) return TRUE;
  return FALSE;
}

REDIT( redit_down )
{
  if (change_exit(ch,argument,DIR_DOWN)) return TRUE;
  return FALSE;
}

REDIT( redit_ed )
{
  ROOM_INDEX_DATA *pRoom;
  EXTRA_DESCR_DATA *ed;
  char command[MAX_INPUT_LENGTH];
  char keyword[MAX_INPUT_LENGTH];

  EDIT_ROOM(ch, pRoom);

  argument = one_argument( argument, command );
  one_argument( argument, keyword );

  if ( command[0] == '\0' || keyword[0] == '\0' )
  {
      stc( "Syntax:  ed add [keyword]\n\r", ch );
      stc( "         ed edit [keyword]\n\r", ch );
      stc( "         ed delete [keyword]\n\r", ch );
      stc( "         ed format [keyword]\n\r", ch );
      return FALSE;
  }

  if ( !str_cmp( command, "add" ) )
  {
      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed add [keyword]\n\r", ch );
          return FALSE;
      }

      ed                      = new_extra_descr();
      ed->keyword             = str_dup( keyword );
      ed->description         = str_dup( "" );
      ed->next                = pRoom->extra_descr;
      pRoom->extra_descr      = ed;

      string_append( ch, &ed->description );

      return TRUE;
  }


  if ( !str_cmp( command, "edit" ) )
  {
      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed edit [keyword]\n\r", ch );
          return FALSE;
      }

      for ( ed = pRoom->extra_descr; ed; ed = ed->next )
      {
          if ( is_name( keyword, ed->keyword ) )
              break;
      }

      if ( !ed )
      {
          stc( "REdit:  Extra description keyword not found.\n\r", ch );
          return FALSE;
      }

      string_append( ch, &ed->description );

      return TRUE;
  }


  if ( !str_cmp( command, "delete" ) )
  {
      EXTRA_DESCR_DATA *ped = NULL;

      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed delete [keyword]\n\r", ch );
          return FALSE;
      }

      for ( ed = pRoom->extra_descr; ed; ed = ed->next )
      {
          if ( is_name( keyword, ed->keyword ) )
              break;
          ped = ed;
      }

      if ( !ed )
      {
          stc( "REdit:  Extra description keyword not found.\n\r", ch );
          return FALSE;
      }

      if ( !ped )
          pRoom->extra_descr = ed->next;
      else
          ped->next = ed->next;

      free_extra_descr( ed );

      stc( "Extra description deleted.\n\r", ch );
      return TRUE;
  }


  if ( !str_cmp( command, "format" ) )
  {
      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed format [keyword]\n\r", ch );
          return FALSE;
      }

      for ( ed = pRoom->extra_descr; ed; ed = ed->next )
      {
          if ( is_name( keyword, ed->keyword ) )
              break;
      }

      if ( !ed )
      {
          stc( "REdit:  Extra description keyword not found.\n\r", ch );
          return FALSE;
      }

      ed->description = string_format( ed->description );

      stc( "Extra description formatted.\n\r", ch );
      return TRUE;
  }

  redit_ed( ch, "" );
  return FALSE;
}

REDIT( redit_create )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int64 value;
    int iHash;
    
    EDIT_ROOM(ch, pRoom);

    value = atoi64( argument );

    if ( argument[0] == '\0' || value <= 0 )
    {
        stc( "Syntax:  create [vnum > 0]\n\r", ch );
        return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
        stc( "REdit:  That vnum is not assigned an area.\n\r", ch );
        return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        stc( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
        return FALSE;
    }

    if ( get_room_index( value ) )
    {
        stc( "REdit:  Room vnum already exists.\n\r", ch );
        return FALSE;
    }

    pRoom                       = new_room_index();
    pRoom->area                 = pArea;
    pRoom->vnum                 = value;

    if ( value > top_vnum_room ) top_vnum_room = value;

    iHash                       = (int)value % MAX_KEY_HASH;
    pRoom->next                 = room_index_hash[iHash];
    room_index_hash[iHash]      = pRoom;
    ch->desc->pEdit             = (void *)pRoom;

    stc( "Room created.\n\r", ch );
    return TRUE;
}

REDIT( redit_name )
{
  ROOM_INDEX_DATA *pRoom;

  EDIT_ROOM(ch, pRoom);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  name [name]\n\r", ch );
      return FALSE;
  }

  free_string( pRoom->name );
  pRoom->name = str_dup( argument );

  stc( "Name set.\n\r", ch );
  return TRUE;
}

REDIT( redit_desc )
{
  ROOM_INDEX_DATA *pRoom;

  EDIT_ROOM(ch, pRoom);

  if ( argument[0] == '\0' )
  {
      string_append( ch, &pRoom->description );
      return TRUE;
  }

  stc( "Syntax:  desc\n\r", ch );
  return FALSE;
}

REDIT( redit_heal )
{
  ROOM_INDEX_DATA *pRoom;
  
  EDIT_ROOM(ch, pRoom);
  
  if (is_number(argument))
     {
        pRoom->heal_rate = atoi ( argument );
        stc ( "Heal rate set.\n\r", ch);
        return TRUE;
     }

  stc ( "Syntax : heal <#xnumber>\n\r", ch);
  return FALSE;
}       

REDIT( redit_mana )
{
  ROOM_INDEX_DATA *pRoom;
  
  EDIT_ROOM(ch, pRoom);
  
  if (is_number(argument))
     {
        pRoom->mana_rate = atoi ( argument );
        stc ( "Mana rate set.\n\r", ch);
        return TRUE;
     }

  stc ( "Syntax : mana <#xnumber>\n\r", ch);
  return FALSE;
}       

REDIT( redit_delete )
{
  ROOM_INDEX_DATA *pRoom;
  
  EDIT_ROOM(ch, pRoom);
  
  if (IS_SET(pRoom->room_flags,ROOM_DELETED))
  {
    stc("Комната уже помечена к удалению - ожидайте перезагрузки.\n\r",ch);
    return FALSE;
  }
  SET_BIT(pRoom->room_flags,ROOM_DELETED);
  stc("Комната помечена к удалению\n\r",ch);
  return TRUE;
}       

REDIT( redit_undelete )
{
  ROOM_INDEX_DATA *pRoom;
  
  EDIT_ROOM(ch, pRoom);
  
  if (!IS_SET(pRoom->room_flags,ROOM_DELETED))
  {
    stc("Комната не помечена к удалению - нечего восстанавливать.\n\r",ch);
    return FALSE;
  }
  REM_BIT(pRoom->room_flags,ROOM_DELETED);
  stc("Пометка к удалению убрана\n\r",ch);
  return TRUE;
}       

REDIT( redit_format )
{
  ROOM_INDEX_DATA *pRoom;

  EDIT_ROOM(ch, pRoom);

  pRoom->description = string_format( pRoom->description );

  stc( "String formatted.\n\r", ch );
  return TRUE;
}

REDIT( redit_mreset )
{
  ROOM_INDEX_DATA     *pRoom;
  MOB_INDEX_DATA      *pMobIndex;
  CHAR_DATA           *newmob;
  char                arg [ MAX_INPUT_LENGTH ];
  char                arg2 [ MAX_INPUT_LENGTH ];

  RESET_DATA          *pReset;
  char                output [ MAX_STRING_LENGTH ];

  EDIT_ROOM(ch, pRoom);

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );

  if ( arg[0] == '\0' || !is_number( arg ) )
  {
      stc ( "Syntax:  mreset <vnum> <max #x> <mix #x>\n\r", ch );
      return FALSE;
  }

  if ( !( pMobIndex = get_mob_index( atoi64( arg ) ) ) )
  {
      stc( "REdit: No mobile has that vnum.\n\r", ch );
      return FALSE;
  }

  if ( pMobIndex->area != pRoom->area )
  {
      stc( "REdit: No such mobile in this area.\n\r", ch );
      return FALSE;
  }

  pReset              = new_reset_data();
  pReset->command     = 'M';
  pReset->arg1        = pMobIndex->vnum;
  pReset->arg2        = is_number( arg2 ) ? atoi( arg2 ) : MAX_MOB;
  pReset->arg3        = pRoom->vnum;
  pReset->arg4        = is_number( argument ) ? atoi (argument) : 1;
  add_reset( pRoom, pReset, 0/* Last slot*/ );

  newmob = create_mobile( pMobIndex );
  char_to_room( newmob, pRoom );

  do_printf( output, "%s (%u) has been loaded and added to resets.\n\r"
      "There will be a maximum of %d loaded to this room.\n\r",
      capitalize( get_mobindex_desc(pMobIndex,'1')),
      pMobIndex->vnum,
      pReset->arg2 );
  stc( output, ch );
  act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
  return TRUE;
}

struct wear_type
{
  int wear_loc;
  int64 wear_bit;
};

const struct wear_type wear_table[] =
{
  {   WEAR_NONE,      ITEM_TAKE               },
  {   WEAR_LIGHT,     ITEM_LIGHT              },
  {   WEAR_FINGER_L,  ITEM_WEAR_FINGER        },
  {   WEAR_FINGER_R,  ITEM_WEAR_FINGER        },
  {   WEAR_NECK,      ITEM_WEAR_NECK          },
  {   WEAR_BODY,      ITEM_WEAR_BODY          },
  {   WEAR_HEAD,      ITEM_WEAR_HEAD          },
  {   WEAR_LEGS,      ITEM_WEAR_LEGS          },
  {   WEAR_FEET,      ITEM_WEAR_FEET          },
  {   WEAR_HANDS,     ITEM_WEAR_HANDS         },
  {   WEAR_ARMS,      ITEM_WEAR_ARMS          },
  {   WEAR_SHIELD,    ITEM_WEAR_SHIELD        },
  {   WEAR_ABOUT,     ITEM_WEAR_ABOUT         },
  {   WEAR_WAIST,     ITEM_WEAR_WAIST         },
  {   WEAR_WRIST_L,   ITEM_WEAR_WRIST         },
  {   WEAR_WRIST_R,   ITEM_WEAR_WRIST         },
  {   WEAR_RHAND,     ITEM_WIELD              },
  {   WEAR_HOLD,      ITEM_HOLD               },
  {   NO_FLAG,        NO_FLAG                 }
};

/*****************************************************************************
 Name:          wear_loc
 Purpose:       Returns the location of the bit that matches the count.
                1 = first match, 2 = second match etc.
 Called by:     oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc(int64 bits, int count)
{
  int flag;

  for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++)
  {
      if ( IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
          return wear_table[flag].wear_loc;
  }

  return NO_FLAG;
}

/*****************************************************************************
 Name:          wear_bit
 Purpose:       Converts a wear_loc into a bit.
 Called by:     redit_oreset(olc_act.c).
 ****************************************************************************/
int64 wear_bit(int64 loc)
{
  int64 flag;

  for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++)
  {
      if ( loc == wear_table[flag].wear_loc )
          return wear_table[flag].wear_bit;
  }

  return 0;
}

REDIT( redit_oreset )
{
  ROOM_INDEX_DATA     *pRoom;
  OBJ_INDEX_DATA      *pObjIndex;
  OBJ_DATA            *newobj;
  OBJ_DATA            *to_obj;
  CHAR_DATA           *to_mob;
  char                arg1 [ MAX_INPUT_LENGTH ];
  char                arg2 [ MAX_INPUT_LENGTH ];
  int                 olevel = 0;

  RESET_DATA          *pReset;
  char                output [ MAX_STRING_LENGTH ];

  EDIT_ROOM(ch, pRoom);

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\0' || !is_number( arg1 ) )
  {
      stc ( "Syntax:  oreset <vnum> <args>\n\r", ch );
      stc ( "        -no_args               = into room\n\r", ch );
      stc ( "        -<obj_name>            = into obj\n\r", ch );
      stc ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
      return FALSE;
  }

  if ( !( pObjIndex = get_obj_index( atoi64( arg1 ) ) ) )
  {
      stc( "REdit: No object has that vnum.\n\r", ch );
      return FALSE;
  }

  if ( pObjIndex->area != pRoom->area )
  {
      stc( "REdit: No such object in this area.\n\r", ch );
      return FALSE;
  }

  // Load into room.
  if ( arg2[0] == '\0' )
  {
      pReset          = new_reset_data();
      pReset->command = 'O';
      pReset->arg1    = pObjIndex->vnum;
      pReset->arg2    = 0;
      pReset->arg3    = pRoom->vnum;
      pReset->arg4    = 0;
      add_reset( pRoom, pReset, 0/* Last slot*/ );

      newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
      obj_to_room( newobj, pRoom );

      do_printf( output, "%s (%u) has been loaded and added to resets.\n\r",
          capitalize( pObjIndex->short_descr ),
          pObjIndex->vnum );
      stc( output, ch );
  }
  else // Load into object's inventory.
  if ( argument[0] == '\0'
  && ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL ) )
  {
      pReset          = new_reset_data();
      pReset->command = 'P';
      pReset->arg1    = pObjIndex->vnum;
      pReset->arg2    = 0;
      pReset->arg3    = to_obj->pIndexData->vnum;
      pReset->arg4    = 1;
      add_reset( pRoom, pReset, 0/* Last slot*/ );

      newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
      newobj->cost = 0;
      obj_to_obj( newobj, to_obj );

      do_printf( output, "%s (%u) has been loaded into "
          "%s (%u) and added to resets.\n\r",
          capitalize( newobj->short_descr ),
          newobj->pIndexData->vnum,
          to_obj->short_descr,
          to_obj->pIndexData->vnum );
      stc( output, ch );
  }
  else // Load into mobile's inventory.
  if ( ( to_mob = get_char_room( ch, arg2 ) ) != NULL )
  {
      int64 wear_loc;

      if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
      {
          stc( "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
          return FALSE;
      }

      // Disallow loading a sword(WEAR_RHAND) into WEAR_HEAD.
      if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
      {
          do_printf( output,
              "%s (%u) has wear flags: [%s]\n\r",
              capitalize( pObjIndex->short_descr ),
              pObjIndex->vnum,
              flag_string( wear_flags, pObjIndex->wear_flags ) );
          stc( output, ch );
          return FALSE;
      }

      // Can't load into same position.
      if ((int)wear_loc!=WEAR_NONE && get_eq_char( to_mob, (int)wear_loc ) )
      {
          stc( "REdit:  Object already equipped.\n\r", ch );
          return FALSE;
      }

      pReset          = new_reset_data();
      pReset->arg1    = pObjIndex->vnum;
      pReset->arg2    = (int)wear_loc;
      if ( pReset->arg2 == WEAR_NONE )
          pReset->command = 'G';
      else
          pReset->command = 'E';
      pReset->arg3    = wear_loc;

      add_reset( pRoom, pReset, 0/* Last slot*/ );

      olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
      newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

      if ( to_mob->pIndexData->pShop )        /* Shop-keeper? */
      {
          switch ( pObjIndex->item_type )
          {
          default:            olevel = 0;                             break;
          case ITEM_PILL:     olevel = number_range(  0, 10 );        break;
          case ITEM_POTION:   olevel = number_range(  0, 10 );        break;
          case ITEM_SCROLL:   olevel = number_range(  5, 15 );        break;
          case ITEM_WAND:     olevel = number_range( 10, 20 );        break;
          case ITEM_STAFF:    olevel = number_range( 15, 25 );        break;
          case ITEM_ARMOR:    olevel = number_range(  5, 15 );        break;
          case ITEM_WEAPON:   if ( pReset->command == 'G' )
                                  olevel = number_range( 5, 15 );
                              else
                                  olevel = number_fuzzy( olevel );
              break;
          }

          newobj = create_object( pObjIndex, olevel );
          if ( pReset->arg2 == WEAR_NONE )
              SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
      }
      else
          newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

      obj_to_char( newobj, to_mob );
      if ( pReset->command == 'E' )
          equip_char( to_mob, newobj, (int)pReset->arg3 );

      do_printf( output, "%s (%u) has been loaded "
          "%s of %s (%u) and added to resets.\n\r",
          capitalize( pObjIndex->short_descr ),
          pObjIndex->vnum,
          flag_string( wear_loc_strings, pReset->arg3 ),
          get_char_desc(to_mob,'1'),
          to_mob->pIndexData->vnum );
      stc( output, ch );
  }
  else
  {
    stc( "REdit:  That mobile isn't here.\n\r", ch );
    return FALSE;
  }

  act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
  return TRUE;
}

// Object Editor Functions.
void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
  char buf[MAX_STRING_LENGTH];

  switch( obj->item_type )
  {
      default:
          break;
          
      case ITEM_LIGHT:
          if ( obj->value[2] == 998 )
              do_printf( buf, "[v2] Light:  Infinite[998]\n\r" );
          else
              do_printf( buf, "[v2] Light:  [%u]\n\r", obj->value[2] );
          stc( buf, ch );
          break;

      case ITEM_WAND:
      case ITEM_STAFF:
          do_printf( buf,
              "[v0] Level:          [%u]\n\r"
              "[v1] Charges Total:  [%u]\n\r"
              "[v2] Charges Left:   [%u]\n\r"
              "[v3] Spell:          %s\n\r",
              obj->value[0],
              obj->value[1],
              obj->value[2],
              obj->value[3] != -1 ? skill_table[obj->value[3]].name
                                  : "none" );
          stc( buf, ch );
          break;

      case ITEM_PORTAL:
          do_printf( buf,
              "[v0] Charges:        [%u]\n\r"
              "[v1] Exit Flags:     %s\n\r"
              "[v2] Portal Flags:   %s\n\r"
              "[v3] Goes to (vnum): [%u]\n\r",
              obj->value[0],
              flag_string( exit_flags, obj->value[1]),
              flag_string( portal_flags , obj->value[2]),
              obj->value[3] );
          stc( buf, ch);
          break;
          
      case ITEM_FURNITURE:          
          do_printf( buf,
              "[v0] Max people:      [%u]\n\r"
              "[v1] Max weight:      [%u]\n\r"
              "[v2] Furniture Flags: %s\n\r"
              "[v3] Heal bonus:      [%u]\n\r"
              "[v4] Mana bonus:      [%u]\n\r",
              obj->value[0],
              obj->value[1],
              flag_string( furniture_flags, obj->value[2]),
              obj->value[3],
              obj->value[4] );
          stc( buf, ch );
          break;

      case ITEM_SCROLL:
      case ITEM_POTION:
      case ITEM_PILL:
          do_printf( buf,
              "[v0] Level:  [%u]\n\r"
              "[v1] Spell:  %s\n\r"
              "[v2] Spell:  %s\n\r"
              "[v3] Spell:  %s\n\r"
              "[v4] Spell:  %s\n\r",
              obj->value[0],
              obj->value[1] != -1 ? skill_table[obj->value[1]].name
                                  : "none",
              obj->value[2] != -1 ? skill_table[obj->value[2]].name
                                  : "none",
              obj->value[3] != -1 ? skill_table[obj->value[3]].name
                                  : "none",
              obj->value[4] != -1 ? skill_table[obj->value[4]].name
                                  : "none" );
          stc( buf, ch );
          break;

      case ITEM_ARMOR:
          do_printf( buf,
              "[v0] Ac pierce       [%u]\n\r"
              "[v1] Ac bash         [%u]\n\r"
              "[v2] Ac slash        [%u]\n\r"
              "[v3] Ac exotic       [%u]\n\r",
              obj->value[0],
              obj->value[1],
              obj->value[2],
              obj->value[3] );
          stc( buf, ch );
          break;

      case ITEM_WEAPON:
          ptc(ch,"[v0] Weapon class:   %s\n\r",
                   flag_string( weapon_class, obj->value[0] ) );
          ptc(ch,"[v1] Number of dice: [%u]\n\r", obj->value[1] );
          ptc(ch,"[v2] Type of dice:   [%u]\n\r", obj->value[2] );
          ptc(ch,"[v3] Type:           %s\n\r",
                  flag_string( weapon_flags, obj->value[3] ) );
          ptc(ch,"[v4] Special type:   %s\n\r",
                   flag_string( weapon_type2,  obj->value[4] ) );
          break;



      case ITEM_CONTAINER:
          ptc(ch,
              "[v0] Weight:     [%u kg]\n\r"
              "[v1] Flags:      [%s]\n\r"
              "[v2] Key:     %s [%u]\n\r"
              "[v3] Capacity    [%u]\n\r"
              "[v4] Weight Mult [%u]\n\r",
              obj->value[0],
              flag_string( container_flags, obj->value[1] ),
              get_obj_index(obj->value[2])
                  ? get_obj_index(obj->value[2])->short_descr
                  : "none",
              obj->value[2],
              obj->value[3],
              obj->value[4] );
          break;

      case ITEM_DRINK_CON:
          ptc(ch,
              "[v0] Liquid Total: [%u]\n\r"
              "[v1] Liquid Left:  [%u]\n\r"
              "[v2] Liquid:       %s (%s)\n\r"
              "[v3] Poisoned:     %s\n\r",
              obj->value[0],
              obj->value[1],
              liq_table[obj->value[2]].liq_name,
              liq_table[obj->value[2]].liq_showname,
              obj->value[3] != 0 ? "Yes" : "No" );
          break;

      case ITEM_FOUNTAIN:
          ptc(ch,"[v0] Liquid Total: [%u]\n\r"
                 "[v1] Liquid Left:  [%u]\n\r"
                 "[v2] Liquid:       %s (%s)\n\r",
              obj->value[0],
              obj->value[1],
              liq_table[obj->value[2]].liq_name,
              liq_table[obj->value[2]].liq_showname );
          break;
              
      case ITEM_FOOD:
          ptc(ch,
              "[v0] Food hours: [%u]\n\r"
              "[v1] Full hours: [%u]\n\r"
              "[v3] Poisoned:   %s\n\r",
              obj->value[0],
              obj->value[1],
              obj->value[3] != 0 ? "Yes" : "No" );
          break;

      case ITEM_MONEY:
          ptc(ch,"[v0] Gold:   [%u]\n\r", obj->value[0] );
          break;

      case ITEM_ENCHANT:
          ptc (ch,"Эффект: %s (%u)\n\rИзменяет уровень на %u\r\n",mitem[obj->value[0]].desc,obj->value[0],obj->value[1] );
          break;

      case ITEM_BONUS:
        if (obj->value[0]==SCROLL_QUENIA)
         ptc (ch,"[v0] Type:    [%2u]\n\r[v1] Word:    [%s]\n\r[v2] quantity:[%u]\n\r",
           obj->value[0],quenia_table[obj->value[1]].descr,obj->value[2]); 
        else if (obj->value[0]==SCROLL_CLANSKILL)
         ptc (ch,"[v0] Type:    [%2u]\n\r[v1] Skill:   [%s]\n\r[v2] ticks   :[%u]\n\r",
           obj->value[0],skill_table[obj->value[1]].name,obj->value[2]);
        break;
  }
}

bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, const char *argument)
{
  switch( pObj->item_type )
  {
    default:
      break;
        
    case ITEM_LIGHT:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_LIGHT" );
          return FALSE;
        case 2:
          stc( "HOURS OF LIGHT SET.\n\r\n\r", ch );
          pObj->value[2] = atoi( argument );
          break;
      }
      break;
    case ITEM_WAND:
    case ITEM_STAFF:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_STAFF_WAND" );
          return FALSE;
        case 0:
          stc( "SPELL LEVEL SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
        case 2:
          stc( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
          pObj->value[2] = atoi( argument );
          break;
        case 3:
          stc( "SPELL TYPE SET.\n\r", ch );
          pObj->value[3] = skill_lookup( argument );
          break;
      }
      break;
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_SCROLL_POTION_PILL" );
          return FALSE;
        case 0:
          stc( "SPELL LEVEL SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "SPELL TYPE 1 SET.\n\r\n\r", ch );
          pObj->value[1] = skill_lookup( argument );
          break;
        case 2:
          stc( "SPELL TYPE 2 SET.\n\r\n\r", ch );
          pObj->value[2] = skill_lookup( argument );
          break;
        case 3:
          stc( "SPELL TYPE 3 SET.\n\r\n\r", ch );
          pObj->value[3] = skill_lookup( argument );
          break;
        case 4:
          stc( "SPELL TYPE 4 SET.\n\r\n\r", ch );
          pObj->value[4] = skill_lookup( argument );
          break;
      }
      break;
    case ITEM_ARMOR:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_ARMOR" );
          return FALSE;
        case 0:
          stc( "AC PIERCE SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "AC BASH SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
        case 2:
          stc( "AC SLASH SET.\n\r\n\r", ch );
          pObj->value[2] = atoi( argument );
          break;
        case 3:
          stc( "AC EXOTIC SET.\n\r\n\r", ch );
          pObj->value[3] = atoi( argument );
          break;
      }
      break;
    case ITEM_WEAPON:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_WEAPON" );
          return FALSE;
        case 0:
          stc( "WEAPON CLASS SET.\n\r\n\r", ch );
          pObj->value[0] = flag_value( weapon_class, argument );
          break;
        case 1:
          stc( "NUMBER OF DICE SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
        case 2:
          stc( "TYPE OF DICE SET.\n\r\n\r", ch );
          pObj->value[2] = atoi( argument );
          break;
        case 3:
          stc( "WEAPON TYPE SET.\n\r\n\r", ch );
          pObj->value[3] = flag_value( weapon_flags, argument );
          break;
        case 4:
          stc( "SPECIAL WEAPON TYPE SET.\n\r\n\r", ch );
          pObj->value[4] = flag_value( weapon_type2, argument );
          break;
      }
      break;
    case ITEM_PORTAL:
      switch ( value_num )
      {
        default:
          do_help(ch, "ITEM_PORTAL" );
          return FALSE;
        case 0:
          stc( "CHARGES SET.\n\r\n\r", ch);
          pObj->value[0] = atoi ( argument );
          break;
        case 1:
          stc( "EXIT FLAGS SET.\n\r\n\r", ch);
          pObj->value[1] = flag_value( exit_flags, argument );
          break;
        case 2:
          stc( "PORTAL FLAGS SET.\n\r\n\r", ch);
          pObj->value[2] = flag_value( portal_flags, argument );
          break;
        case 3:
          stc( "EXIT VNUM SET.\n\r\n\r", ch);
          pObj->value[3] = atoi ( argument );
          break;
      }
      break;
    case ITEM_FURNITURE:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_FURNITURE" );
          return FALSE;
        case 0:
          stc( "NUMBER OF PEOPLE SET.\n\r\n\r", ch);
          pObj->value[0] = atoi ( argument );
          break;
        case 1:
          stc( "MAX WEIGHT SET.\n\r\n\r", ch);
          pObj->value[1] = atoi ( argument );
          break;
        case 2:
          stc( "FURNITURE FLAGS SET.\n\r\n\r", ch);
          pObj->value[2] = flag_value( furniture_flags, argument );
          break;
        case 3:
          stc( "HEAL BONUS SET.\n\r\n\r", ch);
          pObj->value[3] = atoi ( argument );
          break;
        case 4:
          stc( "MANA BONUS SET.\n\r\n\r", ch);
          pObj->value[4] = atoi ( argument );
          break;
      }
      break;
    case ITEM_CONTAINER:
      switch ( value_num )
      {
        int64 value;
            
        default:
          do_help( ch, "ITEM_CONTAINER" );
          return FALSE;
        case 0:
          stc( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          if ( ( value = flag_value( container_flags, argument ) )
            != NO_FLAG ) TOGGLE_BIT(pObj->value[1], value);
          else
          {
            do_help ( ch, "ITEM_CONTAINER" );
            return FALSE;
          }
          stc( "CONTAINER TYPE SET.\n\r\n\r", ch );
          break;
        case 2:
          if ( atoi64(argument) != 0 )
          {
            if ( !get_obj_index( atoi64( argument ) ) )
            {
              stc( "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
              return FALSE;
            }
            if ( get_obj_index( atoi64( argument ) )->item_type != ITEM_KEY )
            {
              stc( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
              return FALSE;
            }
          }
          stc( "CONTAINER KEY SET.\n\r\n\r", ch );
          pObj->value[2] = atoi( argument );
          break;
        case 3:
          stc( "CONTAINER MAX WEIGHT SET.\n\r", ch);
          pObj->value[3] = atoi( argument );
          break;
        case 4:
          stc( "WEIGHT MULTIPLIER SET.\n\r\n\r", ch );
          pObj->value[4] = atoi ( argument );
          break;
      }
      break;
    case ITEM_DRINK_CON:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_DRINK" );
          return FALSE;
        case 0:
          stc( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
        case 2:
          stc( "LIQUID TYPE SET.\n\r\n\r", ch );
          pObj->value[2] = liq_lookup(argument);
          break;
        case 3:
          stc( "POISON VALUE TOGGLED.\n\r\n\r", ch );
          pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
          break;
      }
      break;
    case ITEM_FOUNTAIN:
      switch (value_num)
      {
        default:
          do_help( ch, "ITEM_FOUNTAIN" );
          return FALSE;
        case 0:
          stc( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
        case 2:
          stc( "LIQUID TYPE SET.\n\r\n\r", ch );
          pObj->value[2] = liq_lookup( argument );
          break;
      }
      break;
    case ITEM_FOOD:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_FOOD" );
          return FALSE;
        case 0:
          stc( "HOURS OF FOOD SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "HOURS OF FULL SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
        case 3:
          stc( "POISON VALUE TOGGLED.\n\r\n\r", ch );
          pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
          break;
      }
      break;
    case ITEM_MONEY:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_MONEY" );
          return FALSE;
        case 0:
          stc( "GOLD AMOUNT SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          stc( "SILVER AMOUNT SET.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
      }
      break;
    case ITEM_ENCHANT:
      switch ( value_num )
      {
        default:
          do_help( ch, "ITEM_ENCHANT" );
          return FALSE;
        case 0:
          stc( "Affect set.\n\r", ch );
          pObj->value[0] = atoi( argument );
          if (pObj->value[0] > MAX_ENCHANTS) pObj->value[0]=0;
          break;
        case 1:
          stc( "Level Counter set.\n\r\n\r", ch );
          pObj->value[1] = atoi( argument );
          break;
      }
      break;
    case ITEM_BONUS:
      switch ( value_num )
      {
        default: return FALSE;
        case 0:
          stc( "Type Set.\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          if (pObj->value[0]==SCROLL_QUENIA)
          {
            int word;
            bool qword=FALSE;
            for (word=0;quenia_table[word].name!=Q_END;word++)
            {
              if (!str_cmp(argument,quenia_table[word].descr))
              {
                qword=TRUE;
                break;
              }
            }
            if (qword)
            {
              pObj->value[1] = word;
              stc( "Word Set.\n\r", ch );
            }
            else
            {
              stc( "Word not found.\n\r", ch );
            }
          }
          else if (pObj->value[0]==SCROLL_CLANSKILL)
          {
            int sn;
            sn=skill_lookup(argument);
            if (sn==-1)
            {
              stc( "Skill not found.\n\r",ch );
            }
            else
            {
              pObj->value[1]=sn;
              stc( "Skill Set.\n\r", ch );
            }
          }
          break;
        case 2:
          stc( "Quantity Set.\n\r", ch );
          pObj->value[2] = atoi( argument );
          break;
      }
      break;
  }
  show_obj_values( ch, pObj );
  return TRUE;
}

OEDIT( oedit_show )
{
  OBJ_INDEX_DATA *pObj;
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;
  int cnt;

  EDIT_OBJ(ch, pObj);

  do_printf( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r",
      pObj->name,
      !pObj->area ? -1        : pObj->area->vnum,
      !pObj->area ? "No Area" : pObj->area->name );
  stc( buf, ch );


  do_printf( buf, "Vnum:        [%7d]\n\r",pObj->vnum);
  stc( buf, ch );

  do_printf( buf, "Type:        [%s]\n\r",
      flag_string( type_flags, pObj->item_type ) );
  stc( buf, ch );

  do_printf( buf, "Level:       [%5d]\n\r", pObj->level );
  stc( buf, ch );

  do_printf( buf, "Wear flags:  [%s]\n\r",
      flag_string( wear_flags, pObj->wear_flags ) );
  stc( buf, ch );

  do_printf( buf, "Extra flags: [%s]\n\r",
      flag_string( extra_flags, pObj->extra_flags ) );
  stc( buf, ch );

  ptc(ch, "Material:    [%s][%s]\n\r", pObj->material, material_table[material_num(pObj->material)].real_name);

  ptc(ch, "Durability:  [%5d]\n\r", pObj->durability);

  ptc(ch, "Condition:   [%5d]\n\r", pObj->condition );

  do_printf( buf, "Weight:      [%5d]\n\rCost:        [%5d]\n\r",
      pObj->weight, pObj->cost );
  stc( buf, ch );

  if ( pObj->extra_descr )
  {
      EXTRA_DESCR_DATA *ed;

      stc( "Ex desc kwd: ", ch );

      for ( ed = pObj->extra_descr; ed; ed = ed->next )
      {
          stc( "[", ch );
          stc( ed->keyword, ch );
          stc( "]", ch );
      }

      stc( "\n\r", ch );
  }

  do_printf( buf, "Short desc:  %s\n\rLong desc:\n\r     %s\n\r",
      pObj->short_descr, pObj->description );
  stc( buf, ch );

  for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
  {
      if ( cnt == 0 )
      {
          stc( "Number Modifier Affects\n\r", ch );
          stc( "------ -------- -------\n\r", ch );
      }
      do_printf( buf, "[%4d] %-8d %s\n\r", cnt,
          paf->modifier,
          flag_string( apply_flags, paf->location ) );
      stc( buf, ch );
      cnt++;
  }

  show_obj_values( ch, pObj );

  return FALSE;
}

// Need to issue warning if flag isn't valid. -- does so now -- Hugin.
OEDIT( oedit_addaffect )
{
  int64 value;
  OBJ_INDEX_DATA *pObj;
  AFFECT_DATA *pAf;
  char loc[MAX_STRING_LENGTH];
  char mod[MAX_STRING_LENGTH];

  EDIT_OBJ(ch, pObj);

  argument = one_argument( argument, loc );
  one_argument( argument, mod );

  if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
  {
      stc( "Syntax:  addaffect [location] [#mod]\n\r", ch );
      return FALSE;
  }

  if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG ) /* Hugin */
  {
      stc( "Valid affects are:\n\r", ch );
      show_help( ch, "? affect" );
      return FALSE;
  }

  pAf             = new_affect();
  pAf->location   = (int)value;
  pAf->modifier   = atoi( mod );
  pAf->type       = -1;
  pAf->duration   = -1;
  pAf->bitvector  = 0;
  pAf->next       = pObj->affected;
  pObj->affected  = pAf;

  stc( "Affect added.\n\r", ch);
  return TRUE;
}

// My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
// for really teaching me how to manipulate pointers.
OEDIT( oedit_delaffect )
{
  OBJ_INDEX_DATA *pObj;
  AFFECT_DATA *pAf;
  AFFECT_DATA *pAf_next;
  char affect[MAX_STRING_LENGTH];
  int  value;
  int  cnt = 0;

  EDIT_OBJ(ch, pObj);

  one_argument( argument, affect );

  if ( !is_number( affect ) || affect[0] == '\0' )
  {
      stc( "Syntax:  delaffect [#xaffect]\n\r", ch );
      return FALSE;
  }

  value = atoi( affect );

  if ( value < 0 )
  {
      stc( "Only non-negative affect-numbers allowed.\n\r", ch );
      return FALSE;
  }

  if ( !( pAf = pObj->affected ) )
  {
      stc( "OEdit:  Non-existant affect.\n\r", ch );
      return FALSE;
  }

  if( value == 0 )    /* First case: Remove first affect */
  {
      pAf = pObj->affected;
      pObj->affected = pAf->next;
      free_affect( pAf );
  }
  else                /* Affect to remove is not the first */
  {
      while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
           pAf = pAf_next;

      if( pAf_next )          /* See if it's the next affect */
      {
          pAf->next = pAf_next->next;
          free_affect( pAf_next );
      }
      else                                 /* Doesn't exist */
      {
           stc( "No such affect.\n\r", ch );
           return FALSE;
      }
  }

  stc( "Affect removed.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_name )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  name [string]\n\r", ch );
      return FALSE;
  }

  free_string( pObj->name );
  pObj->name = str_dup( argument );

  stc( "Name set.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_short )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  short [string]\n\r", ch );
      return FALSE;
  }

  free_string( pObj->short_descr );
  pObj->short_descr = str_dup( argument );
  ((char*)pObj->short_descr)[0] = LOWER( pObj->short_descr[0] );

  stc( "Short description set.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_long )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  long [string]\n\r", ch );
      return FALSE;
  }
      
  free_string( pObj->description );
  pObj->description = str_dup( argument );
  ((char*)pObj->description)[0] = UPPER( pObj->description[0] );

  stc( "Long description set.\n\r", ch);
  return TRUE;
}

bool set_value( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, const char *argument, int value )
{
  if ( argument[0] == '\0' )
  {
      set_obj_values( ch, pObj, -1, "" );     /* '\0' changed to "" -- Hugin */
      return FALSE;
  }

  if ( set_obj_values( ch, pObj, value, argument ) ) return TRUE;
  return FALSE;
}

/*****************************************************************************
 Name:          oedit_values
 Purpose:       Finds the object and sets its value.
 Called by:     The four valueX functions below. (now five -- Hugin )
 ****************************************************************************/
bool oedit_values( CHAR_DATA *ch, const char *argument, int value )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( set_value( ch, pObj, argument, value ) ) return TRUE;
  return FALSE;
}


OEDIT( oedit_value0 )
{
  if ( oedit_values( ch, argument, 0 ) ) return TRUE;
  return FALSE;
}

OEDIT( oedit_value1 )
{
  if ( oedit_values( ch, argument, 1 ) ) return TRUE;
  return FALSE;
}

OEDIT( oedit_value2 )
{
  if ( oedit_values( ch, argument, 2 ) ) return TRUE;
  return FALSE;
}

OEDIT( oedit_value3 )
{
  if ( oedit_values( ch, argument, 3 ) ) return TRUE;
  return FALSE;
}

OEDIT( oedit_value4 )
{
  if ( oedit_values( ch, argument, 4 ) ) return TRUE;
  return FALSE;
}

OEDIT( oedit_weight )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
      stc( "Syntax:  weight [number]\n\r", ch );
      return FALSE;
  }

  pObj->weight = atoi( argument );

  stc( "Weight set.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_cost )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
      stc( "Syntax:  cost [number]\n\r", ch );
      return FALSE;
  }

  pObj->cost = atoi( argument );

  stc( "Cost set.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_create )
{
  OBJ_INDEX_DATA *pObj;
  AREA_DATA *pArea;
  int64  value;
  int  iHash;

  value = atoi( argument );
  if ( argument[0] == '\0' || value == 0 )
  {
      stc( "Syntax:  oedit create [vnum]\n\r", ch );
      return FALSE;
  }

  pArea = get_vnum_area( value );
  if ( !pArea )
  {
      stc( "OEdit:  That vnum is not assigned an area.\n\r", ch );
      return FALSE;
  }

  if ( !IS_BUILDER( ch, pArea ) )
  {
      stc( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
      return FALSE;
  }

  if ( get_obj_index( value ) )
  {
      stc( "OEdit:  Object vnum already exists.\n\r", ch );
      return FALSE;
  }
      
  pObj                        = new_obj_index();
  pObj->vnum                  = value;
  pObj->area                  = pArea;
      
  if ( value > top_vnum_obj )
      top_vnum_obj = value;

  iHash                       = (int)value % MAX_KEY_HASH;
  pObj->next                  = obj_index_hash[iHash];
  obj_index_hash[iHash]       = pObj;
  ch->desc->pEdit             = (void *)pObj;

  stc( "Object Created.\n\r", ch );
  return TRUE;
}

OEDIT( oedit_ed )
{
  OBJ_INDEX_DATA *pObj;
  EXTRA_DESCR_DATA *ed;
  char command[MAX_INPUT_LENGTH];
  char keyword[MAX_INPUT_LENGTH];

  EDIT_OBJ(ch, pObj);

  argument = one_argument( argument, command );
  one_argument( argument, keyword );

  if ( command[0] == '\0' )
  {
      stc( "Syntax:  ed add [keyword]\n\r", ch );
      stc( "         ed delete [keyword]\n\r", ch );
      stc( "         ed edit [keyword]\n\r", ch );
      stc( "         ed format [keyword]\n\r", ch );
      return FALSE;
  }

  if ( !str_cmp( command, "add" ) )
  {
      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed add [keyword]\n\r", ch );
          return FALSE;
      }

      ed                  = new_extra_descr();
      ed->keyword         = str_dup( keyword );
      ed->next            = pObj->extra_descr;
      pObj->extra_descr   = ed;

      string_append( ch, &ed->description );

      return TRUE;
  }

  if ( !str_cmp( command, "edit" ) )
  {
      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed edit [keyword]\n\r", ch );
          return FALSE;
      }

      for ( ed = pObj->extra_descr; ed; ed = ed->next )
      {
          if ( is_name( keyword, ed->keyword ) )
              break;
      }

      if ( !ed )
      {
          stc( "OEdit:  Extra description keyword not found.\n\r", ch );
          return FALSE;
      }

      string_append( ch, &ed->description );

      return TRUE;
  }

  if ( !str_cmp( command, "delete" ) )
  {
      EXTRA_DESCR_DATA *ped = NULL;

      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed delete [keyword]\n\r", ch );
          return FALSE;
      }

      for ( ed = pObj->extra_descr; ed; ed = ed->next )
      {
          if ( is_name( keyword, ed->keyword ) )
              break;
          ped = ed;
      }

      if ( !ed )
      {
          stc( "OEdit:  Extra description keyword not found.\n\r", ch );
          return FALSE;
      }

      if ( !ped )
          pObj->extra_descr = ed->next;
      else
          ped->next = ed->next;

      free_extra_descr( ed );

      stc( "Extra description deleted.\n\r", ch );
      return TRUE;
  }


  if ( !str_cmp( command, "format" ) )
  {
      if ( keyword[0] == '\0' )
      {
          stc( "Syntax:  ed format [keyword]\n\r", ch );
          return FALSE;
      }

      for ( ed = pObj->extra_descr; ed; ed = ed->next )
      {
          if ( is_name( keyword, ed->keyword ) )
              break;
      }

      if ( !ed )
      {
              stc( "OEdit:  Extra description keyword not found.\n\r", ch );
              return FALSE;
      }

      ed->description = string_format( ed->description );

      stc( "Extra description formatted.\n\r", ch );
      return TRUE;
  }

  oedit_ed( ch, "" );
  return FALSE;
}

OEDIT( oedit_extra )
{
  OBJ_INDEX_DATA *pObj;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_OBJ(ch, pObj);

      if ( ( value = flag_value( extra_flags, argument ) ) != NO_FLAG )
      {
          TOGGLE_BIT(pObj->extra_flags, value);

          stc( "Extra flag toggled.\n\r", ch);
          return TRUE;
      }
  }

  stc( "Syntax:  extra [flag]\n\r"
                "Type '? extra' for a list of flags.\n\r", ch );
  return FALSE;
}


OEDIT( oedit_wear )
{
  OBJ_INDEX_DATA *pObj;
  int64 value;

   if ( argument[0] != '\0' )
  {
      EDIT_OBJ(ch, pObj);

      if ( ( value = flag_value( wear_flags, argument ) ) != NO_FLAG )
      {
          TOGGLE_BIT(pObj->wear_flags, value);

          stc( "Wear flag toggled.\n\r", ch);
          return TRUE;
      }
  }

  stc( "Syntax:  wear [flag]\n\r"
                "Type '? wear' for a list of flags.\n\r", ch );
  return FALSE;
}

OEDIT( oedit_type )
{
  OBJ_INDEX_DATA *pObj;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_OBJ(ch, pObj);

      if ( ( value = flag_value( type_flags, argument ) ) != NO_FLAG )
      {
          pObj->item_type = (int)value;

          stc( "Type set.\n\r", ch);

          /*
           * Clear the values.
           */
          pObj->value[0] = 0;
          pObj->value[1] = 0;
          pObj->value[2] = 0;
          pObj->value[3] = 0;
          pObj->value[4] = 0;

          return TRUE;
      }
  }

  stc( "Syntax:  type [flag]\n\r"
                "Type '? type' for a list of flags.\n\r", ch );
  return FALSE;
}

OEDIT( oedit_durability )
{
  OBJ_INDEX_DATA *pObj;
  int value;

  if ( argument[0] != '\0' && ( value = atoi (argument) ) != 0 
  && value >= -1 && value <= 1000 )
  {
      EDIT_OBJ( ch, pObj );

      pObj->durability = value;
      pObj->condition  = value;
      stc( "Durability set.\n\r", ch );

      return TRUE;
  }
  stc( "Syntax:  durability [number]\n\r"
                "Where number can range from 1 (decoration) to 1000 (artefact).\n\r", ch );

  return FALSE;
}

OEDIT( oedit_condition )
{
  OBJ_INDEX_DATA *pObj;
  int value;

  if ( argument[0] != '\0' 
  && ( value = atoi (argument ) ) >= 0 
  && ( value <= 1000 ) )
  {
      EDIT_OBJ( ch, pObj );

      pObj->condition = value;

      stc( "Condition set.\n\r", ch );

      return TRUE;
  }
  stc( "Syntax:  condition [number]\n\r"
                "Where number can range from 0 (ruined) to 1000 (perfect).\n\r", ch );
  return FALSE;
}

OEDIT( oedit_material )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' )
  {
    stc( "Syntax:  material [string]\n\r", ch );
    return FALSE;
  }

  free_string( pObj->material );

  if( (pObj->material = str_dup( material_lookup(argument)) ) == NULL )
   pObj->material = material_table[MATERIAL_NONE].name;

  if( str_cmp( get_obj_cond( (OBJ_DATA *)pObj, 0), SHOW_COND_ETERNAL) 
   || pObj->durability != -1 );
  pObj->condition = pObj->durability = material_table[material_num(pObj->material)].d_dam;

  if( pObj->condition > pObj->durability ) pObj->condition = pObj->durability;

  ptc( ch,"Material [{G%s{x] set.\n\r", pObj->material);
  return TRUE;
}

OEDIT( oedit_level )
{
  OBJ_INDEX_DATA *pObj;

  EDIT_OBJ(ch, pObj);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
    stc( "Syntax:  level [number]\n\r", ch );
    return FALSE;
  }

  pObj->level = atoi( argument );

  stc( "Level set.\n\r", ch);
  return TRUE;
}

// Mobile Editor Functions.
MEDIT( medit_show )
{
  MOB_INDEX_DATA *pMob;
  MPROG_LIST *list;

  EDIT_MOB(ch, pMob);

  ptc(ch, "{GName: [{x%s{G]\n\r",pMob->player_name);

  ptc(ch,"{GShort desc: {x%s\n\r{GLong descr:{x\n\r%s",
      get_mobindex_desc(pMob,'1'),pMob->long_descr );

  ptc(ch,"{CLevel {Y%3d  {CVnum: [{Y%u{C]  Area: [{Y%5d{C] {G%s{x\n\r",
   pMob->level,pMob->vnum,
   !pMob->area ? -1:pMob->area->vnum,
   !pMob->area ? "No Area" : pMob->area->name);

  ptc(ch, "Race [{G%s{x]   Sex: [{G%s{x]\n\r",
      race_table[pMob->race].name, 
      pMob->sex == SEX_MALE    ? "male"   :
      pMob->sex == SEX_FEMALE  ? "female" : 
      pMob->sex == 3           ? "random" : "neutral" );

  ptc(ch,"{GHit dice:{x    [%6dd%-5d+%6d]\n\r",
   pMob->hit[DICE_NUMBER],
   pMob->hit[DICE_TYPE],
   pMob->hit[DICE_BONUS] );

  ptc(ch,"{RDamage dice:{x [%6dd%-5d+%6d]\n\r",
   pMob->damage[DICE_NUMBER],
   pMob->damage[DICE_TYPE],
   pMob->damage[DICE_BONUS] );

  ptc(ch,"{CMana dice:{x   [%6dd%-5d+%6d]\n\r",
   pMob->mana[DICE_NUMBER],
   pMob->mana[DICE_TYPE],
   pMob->mana[DICE_BONUS] );

  ptc(ch,"Wealth:[%10l]   Align:  [%5d]\n\r",
      pMob->wealth,pMob->alignment );

  ptc(ch,"Damage:[%10s]   Hitroll:[%5d]\n\r",
   attack_table[pMob->dam_type].name ,pMob->hitroll );

  ptc(ch,"Armor: [pierce: %d  bash: %d  slash: %d  magic: %d]\n\r",
   pMob->ac[AC_PIERCE], pMob->ac[AC_BASH],
   pMob->ac[AC_SLASH], pMob->ac[AC_EXOTIC] );

  ptc(ch, "Act: [{R%s{x]\n\r",flag_string( act_flags, pMob->act ) );

  ptc(ch, "Aff: [{C%s{x]\n\r",flag_string( affect_flags, pMob->affected_by ) );

  ptc(ch,"Pos   : starting [{Y%s{x]  default [{Y%s{x]\n\r",
      flag_string( position_flags, pMob->start_pos ),
      flag_string( position_flags, pMob->default_pos) );

  ptc(ch,"Imm:  [{W%s{x]\n\r",flag_string( imm_flags, pMob->imm_flags ) );
  ptc(ch,"Res:  [{Y%s{x]\n\r",flag_string( res_flags, pMob->res_flags ) );
  ptc(ch,"Vuln: [{y%s{x]\n\r",flag_string( vuln_flags, pMob->vuln_flags ) );
  ptc(ch,"Off:  [{M%s{x]\n\r",flag_string( off_flags,  pMob->off_flags ) );
  ptc(ch,"Size: [{G%s{x]\n\r",flag_string( size_flags, pMob->size ) );

  ptc(ch,"Form:     [%s]\n\r",flag_string( form_flags, pMob->form ) );

  if ( pMob->spec_fun )
   ptc(ch,"Spec fun: [%s]\n\r",spec_name( pMob->spec_fun ) );

  ptc(ch,"Description:\n\r%s", pMob->description );

  if ( pMob->pShop )
  {
    SHOP_DATA *pShop;
    int iTrade;

    pShop = pMob->pShop;

    ptc(ch,"Shop data for [%7d]:\n\r"
           "  Markup for purchaser: %d%%\n\r"
           "  Markdown for seller:  %d%%\n\r",
      pShop->keeper, pShop->profit_buy, pShop->profit_sell );
      ptc(ch,"  Hours: %d to %d.\n\r",
      pShop->open_hour, pShop->close_hour );

    for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
    {
      if ( pShop->buy_type[iTrade] != 0 )
      {
        if ( iTrade == 0 )
        {
          stc( "  Number Trades Type\n\r", ch );
          stc( "  ------ -----------\n\r", ch );
        }
        ptc(ch," [%5d] %s\n\r",iTrade,flag_string( type_flags, pShop->buy_type[iTrade] ) );
      }
    }
  }
  if ( pMob->mprogs )
  {
    int cnt;
    ptc(ch,"\n\rMOBPrograms for [%7d]:\n\r", pMob->vnum);

    for (cnt=0, list=pMob->mprogs; list; list=list->next)
    {
      if (cnt ==0)
      {
        stc ( " Number Vnum Trigger Phrase\n\r", ch );
        stc ( " ------ ---- ------- ------\n\r", ch );
      }

      ptc(ch,"[%6d] %5d %8s %s\n\r", cnt,
        list->vnum,mprog_type_to_name(list->trig_type),
        list->trig_phrase);
      cnt++;
    }
  }
  return FALSE;
}

MEDIT( medit_create )
{
  MOB_INDEX_DATA *pMob;
  AREA_DATA *pArea;
  int64  value;
  int  iHash;

  value = atoi( argument );
  if ( argument[0] == '\0' || value == 0 )
  {
      stc( "Syntax:  medit create [vnum]\n\r", ch );
      return FALSE;
  }

  pArea = get_vnum_area( value );

  if ( !pArea )
  {
      stc( "MEdit:  That vnum is not assigned an area.\n\r", ch );
      return FALSE;
  }

  if ( !IS_BUILDER( ch, pArea ) )
  {
      stc( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
      return FALSE;
  }

  if ( get_mob_index( value ) )
  {
      stc( "MEdit:  Mobile vnum already exists.\n\r", ch );
      return FALSE;
  }

  pMob                        = new_mob_index();
  pMob->vnum                  = value;
  pMob->area                  = pArea;
      
  if ( value > top_vnum_mob )
      top_vnum_mob = value;        

  pMob->act                   = ACT_IS_NPC;
  iHash                       = (int)value % MAX_KEY_HASH;
  pMob->next                  = mob_index_hash[iHash];
  mob_index_hash[iHash]       = pMob;
  ch->desc->pEdit             = (void *)pMob;

  stc( "Mobile Created.\n\r", ch );
  return TRUE;
}

MEDIT( medit_spec )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  spec [special function]\n\r", ch );
      return FALSE;
  }


  if ( !str_cmp( argument, "none" ) )
  {
      pMob->spec_fun = NULL;

      stc( "Spec removed.\n\r", ch);
      return TRUE;
  }

  if ( spec_lookup( argument ) )
  {
      pMob->spec_fun = spec_lookup( argument );
      stc( "Spec set.\n\r", ch);
      return TRUE;
  }

  stc( "MEdit: No such special function.\n\r", ch );
  return FALSE;
}

MEDIT( medit_damtype )
{
  MOB_INDEX_DATA *pMob;
  int64 value = NO_FLAG;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  damtype [damage message]\n\r", ch );
      stc( "Para ver una lista de tipos de mensajes, pon '? weapon'.\n\r", ch );
      return FALSE;
  }

  if ( ( value = flag_value( weapon_flags, argument ) ) != NO_FLAG )
  {
      pMob->dam_type = (int)value;
      stc( "Damage type set.\n\r", ch);
      return TRUE;
  }

  stc( "MEdit: No such damage type.\n\r", ch );
  return FALSE;
}


MEDIT( medit_align )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
      stc( "Syntax:  alignment [number]\n\r", ch );
      return FALSE;
  }

  pMob->alignment = atoi( argument );

  stc( "Alignment set.\n\r", ch);
  return TRUE;
}

MEDIT( medit_level )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
      stc( "Syntax:  level [number]\n\r", ch );
      return FALSE;
  }

  pMob->level = atoi( argument );

  stc( "Level set.\n\r", ch);
  return TRUE;
}

MEDIT( medit_desc )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' )
  {
      string_append( ch, &pMob->description );
      return TRUE;
  }

  stc( "Syntax:  desc    - line edit\n\r", ch );
  return FALSE;
}

MEDIT( medit_long )
{
  char buf[MAX_STRING_LENGTH];
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  long [string]\n\r", ch );
      return FALSE;
  }

  strcpy( buf, argument);
  strcat( buf, "\n\r" );

  free_string( pMob->long_descr );
  pMob->long_descr = str_dup( buf );
  ((char*)pMob->long_descr)[0] = UPPER( pMob->long_descr[0]  );

  stc( "Long description set.\n\r", ch);
  return TRUE;
}

MEDIT( medit_short )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  short [string]\n\r", ch );
      return FALSE;
  }

  free_string( pMob->short_descr );
  pMob->short_descr = str_dup( argument );

  stc( "Short description set.\n\r", ch);
  return TRUE;
}

MEDIT( medit_name )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' )
  {
      stc( "Syntax:  name [string]\n\r", ch );
      return FALSE;
  }

  free_string( pMob->player_name );
  pMob->player_name = str_dup( argument );

  stc( "Name set.\n\r", ch);
  return TRUE;
}

MEDIT( medit_shop )
{
  MOB_INDEX_DATA *pMob;
  char command[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];

  argument = one_argument( argument, command );
  argument = one_argument( argument, arg1 );

  EDIT_MOB(ch, pMob);

  if ( command[0] == '\0' )
  {
    stc( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
    stc( "         shop profit [#xbuying%%] [#xselling%]\n\r", ch );
    stc( "         shop type [#x0-4] [item type]\n\r", ch );
    stc( "         shop delete [#x0-4]\n\r", ch );
    return FALSE;
  }

  if ( !str_cmp( command, "hours" ) )
  {
    if ( arg1[0] == '\0' || !is_number( arg1 )
    || argument[0] == '\0' || !is_number( argument ) )
    {
      stc( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
      return FALSE;
    }

    if ( !pMob->pShop )
    {
      pMob->pShop         = new_shop();
      pMob->pShop->keeper = pMob->vnum;
      shop_last->next     = pMob->pShop;
    }

    pMob->pShop->open_hour = atoi( arg1 );
    pMob->pShop->close_hour = atoi( argument );

    stc( "Shop hours set.\n\r", ch);
    return TRUE;
  }


  if ( !str_cmp( command, "profit" ) )
  {
    if ( arg1[0] == '\0' || !is_number( arg1 )
    || argument[0] == '\0' || !is_number( argument ) )
    {
      stc( "Syntax:  shop profit [#xbuying%%] [#xselling%%]\n\r", ch );
      return FALSE;
    }

    if ( !pMob->pShop )
    {
      pMob->pShop         = new_shop();
      pMob->pShop->keeper = pMob->vnum;
      shop_last->next     = pMob->pShop;
    }

    pMob->pShop->profit_buy     = atoi( arg1 );
    pMob->pShop->profit_sell    = atoi( argument );

    stc( "Shop profit set.\n\r", ch);
    return TRUE;
  }


  if ( !str_cmp( command, "type" ) )
  {
    int64 value;

    if ( arg1[0] == '\0' || !is_number( arg1 )
    || argument[0] == '\0' )
    {
      stc( "Syntax:  shop type [#x0-4] [item type]\n\r", ch );
      return FALSE;
    }

    if ( atoi( arg1 ) >= MAX_TRADE )
    {
      ptc(ch,"REdit:  May sell %d items max.\n\r", MAX_TRADE );
      return FALSE;
    }

    if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
    {
      stc( "REdit:  That type of item is not known.\n\r", ch );
      return FALSE;
    }

    if ( !pMob->pShop )
    {
      pMob->pShop         = new_shop();
      pMob->pShop->keeper = pMob->vnum;
      shop_last->next     = pMob->pShop;
    }

    pMob->pShop->buy_type[atoi( arg1 )] = (int)value;

    stc( "Shop type set.\n\r", ch);
    return TRUE;
  }


  if ( !str_cmp( command, "delete" ) )
  {
      SHOP_DATA *pShop;
      SHOP_DATA *pShop_next;
      int value;
      int cnt = 0;
      
      if ( arg1[0] == '\0' || !is_number( arg1 ) )
      {
          stc( "Syntax:  shop delete [#x0-4]\n\r", ch );
          return FALSE;
      }

      value = atoi( argument );
      
      if ( !pMob->pShop )
      {
          stc( "REdit:  Non-existant shop.\n\r", ch );
          return FALSE;
      }

      if ( value == 0 )
      {
          pShop = pMob->pShop;
          pMob->pShop = pMob->pShop->next;
          free_shop( pShop );
      }
      else
      for ( pShop = pMob->pShop, cnt = 0; pShop; pShop = pShop_next, cnt++ )
      {
          pShop_next = pShop->next;
          if ( cnt+1 == value )
          {
              pShop->next = pShop_next->next;
              free_shop( pShop_next );
              break;
          }
      }

      stc( "Shop deleted.\n\r", ch);
      return TRUE;
  }

  medit_shop( ch, "" );
  return FALSE;
}

MEDIT( medit_sex )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( sex_flags, argument ) ) != NO_FLAG )
    {
      pMob->sex = (int)value;

      stc( "Sex set.\n\r", ch);
      return TRUE;
    }
  }

  stc( "Syntax: sex [sex]\n\r"
                "Type '? sex' for a list of flags.\n\r", ch );
  return FALSE;
}


MEDIT( medit_act ) // Moved out of medit() due to naming conflicts -- Hugin
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( act_flags, argument ) ) != 0 )
      {
          pMob->act ^= value;
          SET_BIT( pMob->act, ACT_IS_NPC );

          stc( "Act flag toggled.\n\r", ch);
          return TRUE;
      }
  }

  stc( "Syntax: act [flag]\n\r"
                "Type '? act' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_affect )      /* Moved out of medit() due to naming conflicts -- Hugin */
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( affect_flags, argument ) ) != NO_FLAG )
      {
          pMob->affected_by ^= value;

          stc( "Affect flag toggled.\n\r", ch);
          return TRUE;
      }
  }

  stc( "Syntax: affect [flag]\n\r"
                "Type '? affect' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_ac )
{
  MOB_INDEX_DATA *pMob;
  char arg[MAX_INPUT_LENGTH];
  int pierce, bash, slash, exotic;

  do   /* So that I can use break and send the syntax in one place */
  {
      if ( argument[0] == '\0' )  break;

      EDIT_MOB(ch, pMob);
      argument = one_argument( argument, arg );

      if ( !is_number( arg ) )  break;
      pierce = atoi( arg );
      argument = one_argument( argument, arg );

      if ( arg[0] != '\0' )
      {
          if ( !is_number( arg ) )  break;
          bash = atoi( arg );
          argument = one_argument( argument, arg );
      }
      else
          bash = pMob->ac[AC_BASH];

      if ( arg[0] != '\0' )
      {
          if ( !is_number( arg ) )  break;
          slash = atoi( arg );
          argument = one_argument( argument, arg );
      }
      else
          slash = pMob->ac[AC_SLASH];

      if ( arg[0] != '\0' )
      {
          if ( !is_number( arg ) )  break;
          exotic = atoi( arg );
      }
      else
          exotic = pMob->ac[AC_EXOTIC];

      pMob->ac[AC_PIERCE] = pierce;
      pMob->ac[AC_BASH]   = bash;
      pMob->ac[AC_SLASH]  = slash;
      pMob->ac[AC_EXOTIC] = exotic;
      
      stc( "Ac set.\n\r", ch );
      return TRUE;
  } while ( FALSE );    /* Just do it once.. */

  stc( "Syntax:  ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
                "help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
  return FALSE;
}

MEDIT( medit_form )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( form_flags, argument ) ) != NO_FLAG )
      {
          pMob->form ^= value;
          stc( "Form toggled.\n\r", ch );
          return TRUE;
      }
  }

  stc( "Syntax: form [flags]\n\r"
                "Type '? form' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_imm )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( imm_flags, argument ) ) != NO_FLAG )
      {
          pMob->imm_flags ^= value;
          stc( "Immunity toggled.\n\r", ch );
          return TRUE;
      }
  }

  stc( "Syntax: imm [flags]\n\r"
                "Type '? imm' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_res )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( res_flags, argument ) ) != NO_FLAG )
      {
          pMob->res_flags ^= value;
          stc( "Resistance toggled.\n\r", ch );
          return TRUE;
      }
  }

  stc( "Syntax: res [flags]\n\r"
                "Type '? res' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_vuln )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( vuln_flags, argument ) ) != NO_FLAG )
      {
          pMob->vuln_flags ^= value;
          stc( "Vulnerability toggled.\n\r", ch );
          return TRUE;
      }
  }

  stc( "Syntax: vuln [flags]\n\r"
                "Type '? vuln' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_off )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
      EDIT_MOB( ch, pMob );

      if ( ( value = flag_value( off_flags, argument ) ) != NO_FLAG )
      {
          pMob->off_flags ^= value;
          stc( "Offensive behaviour toggled.\n\r", ch );
          return TRUE;
      }
  }

  stc( "Syntax: off [flags]\n\r"
                "Type '? off' for a list of flags.\n\r", ch );
  return FALSE;
}

MEDIT( medit_size )
{
  MOB_INDEX_DATA *pMob;
  int64 value;

  if ( argument[0] != '\0' )
  {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( size_flags, argument ) ) != NO_FLAG )
    {
      pMob->size = (int)value;
      stc( "Size set.\n\r", ch );
      return TRUE;
    }
  }

  stc( "Syntax: size [size]\n\rType '? size' for a list of sizes.\n\r", ch );
  return FALSE;
}

MEDIT( medit_hitdice )
{
  static char syntax[] = "Syntax:  hitdice <number> d <type> + <bonus>\n\r";
  char buf[MAX_STRING_LENGTH], *num, *type, *bonus, *cp;
  MOB_INDEX_DATA *pMob;

  EDIT_MOB( ch, pMob );

  if ( argument[0] == '\0' )
  {
      stc( syntax, ch );
      return FALSE;
  }

  strcpy(buf, argument);
  num = cp = buf;

  while ( isdigit( *cp ) ) ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

  type = cp;

  while ( isdigit( *cp ) ) ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

  bonus = cp;

  while ( isdigit( *cp ) ) ++cp;
  if ( *cp != '\0' ) *cp = '\0';

  if ( ( !is_number( num   ) || atoi( num   ) < 1 )
  ||   ( !is_number( type  ) || atoi( type  ) < 1 ) 
  ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
  {
      stc( syntax, ch );
      return FALSE;
  }

  pMob->hit[DICE_NUMBER] = atoi( num   );
  pMob->hit[DICE_TYPE]   = atoi( type  );
  pMob->hit[DICE_BONUS]  = atoi( bonus );

  stc( "Hitdice set.\n\r", ch );
  return TRUE;
}

MEDIT( medit_manadice )
{
  static char syntax[] = "Syntax:  manadice <number> d <type> + <bonus>\n\r";
  char buf[MAX_STRING_LENGTH], *num, *type, *bonus, *cp;
  MOB_INDEX_DATA *pMob;

  EDIT_MOB( ch, pMob );

  if ( argument[0] == '\0' )
  {
      stc( syntax, ch );
      return FALSE;
  }

  strcpy(buf, argument);
  num = cp = buf;

  while ( isdigit( *cp ) ) ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

  type = cp;

  while ( isdigit( *cp ) ) ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

  bonus = cp;

  while ( isdigit( *cp ) ) ++cp;
  if ( *cp != '\0' ) *cp = '\0';

  if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
  {
      stc( syntax, ch );
      return FALSE;
  }

  if ( ( !is_number( num   ) || atoi( num   ) < 1 )
  ||   ( !is_number( type  ) || atoi( type  ) < 1 ) 
  ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
  {
      stc( syntax, ch );
      return FALSE;
  }

  pMob->mana[DICE_NUMBER] = atoi( num   );
  pMob->mana[DICE_TYPE]   = atoi( type  );
  pMob->mana[DICE_BONUS]  = atoi( bonus );

  stc( "Manadice set.\n\r", ch );
  return TRUE;
}

MEDIT( medit_damdice )
{
  static char syntax[] = "Syntax:  damdice <number> d <type> + <bonus>\n\r";
  char buf[MAX_STRING_LENGTH], *num, *type, *bonus, *cp;
  MOB_INDEX_DATA *pMob;

  EDIT_MOB( ch, pMob );

  if ( argument[0] == '\0' )
  {
      stc( syntax, ch );
      return FALSE;
  }

  strcpy(buf, argument);
  num = cp = buf;

  while ( isdigit( *cp ) ) ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

  type = cp;

  while ( isdigit( *cp ) ) ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

  bonus = cp;

  while ( isdigit( *cp ) ) ++cp;
  if ( *cp != '\0' ) *cp = '\0';

  if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
  {
      stc( syntax, ch );
      return FALSE;
  }

  if ( ( !is_number( num   ) || atoi( num   ) < 1 )
  ||   ( !is_number( type  ) || atoi( type  ) < 1 ) 
  ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
  {
      stc( syntax, ch );
      return FALSE;
  }

  pMob->damage[DICE_NUMBER] = atoi( num   );
  pMob->damage[DICE_TYPE]   = atoi( type  );
  pMob->damage[DICE_BONUS]  = atoi( bonus );

  stc( "Damdice set.\n\r", ch );
  return TRUE;
}

MEDIT( medit_race )
{
  MOB_INDEX_DATA *pMob;
  int race;

  if ( argument[0] != '\0'
  && ( race = race_lookup( argument ) ) != 0 )
  {
    EDIT_MOB( ch, pMob );

    pMob->race = race;
    pMob->off_flags   |= race_table[race].off;
    pMob->imm_flags   |= race_table[race].imm;
    pMob->res_flags   |= race_table[race].res;
    pMob->vuln_flags  |= race_table[race].vuln;
    pMob->form        |= race_table[race].form;
    stc( "Race set.\n\r", ch );
    return TRUE;
  }

  if ( argument[0] == '?' )
  {
    stc( "Available races are:", ch );

    for ( race = 0; race_table[race].name != NULL; race++ )
    {
      if ( ( race % 3 ) == 0 ) stc( "\n\r", ch );
      ptc(ch," %-15s", race_table[race].name );
    }
    stc( "\n\r", ch );
    return FALSE;
  }

  stc( "Syntax:  race [race]\n\r"
       "Type 'race ?' for a list of races.\n\r", ch );
  return FALSE;
}

MEDIT( medit_position )
{
  MOB_INDEX_DATA *pMob;
  char arg[MAX_INPUT_LENGTH];
  int64 value;

  argument = one_argument( argument, arg );

  switch ( arg[0] )
  {
  default:
      break;

  case 'S':
  case 's':
      if ( str_prefix( arg, "start" ) )
          break;

      if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
          break;

      EDIT_MOB( ch, pMob );

      pMob->start_pos = (int)value;
      stc( "Start position set.\n\r", ch );
      return TRUE;

  case 'D':
  case 'd':
      if ( str_prefix( arg, "default" ) )
          break;

      if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
          break;

      EDIT_MOB( ch, pMob );

      pMob->default_pos = (int)value;
      stc( "Default position set.\n\r", ch );
      return TRUE;
  }

  stc( "Syntax:  position [start/default] [position]\n\r"
                "Type '? position' for a list of positions.\n\r", ch );
  return FALSE;
}

MEDIT( medit_gold )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
      stc( "Syntax:  wealth [number]\n\r", ch );
      return FALSE;
  }

  pMob->wealth = atoi( argument );

  stc( "Wealth set.\n\r", ch);
  return TRUE;
}

MEDIT ( medit_addmprog )
{
  int64 value;
  MOB_INDEX_DATA *pMob;
  MPROG_LIST *list;
  MPROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_MOB(ch, pMob);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        stc("Syntax:   addmprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value(mprog_flags, trigger) ) == NO_FLAG)
  {
        stc("Valid flags are:\n\r",ch);
        show_help( ch, "mprog");
        return FALSE;
  }

  if ( ( code =get_mprog_index (atoi(num) ) ) == NULL)
  {
        stc("No such MOBProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_mprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pMob->mprog_flags,value);
  list->next            = pMob->mprogs;
  pMob->mprogs          = list;

  stc( "Mprog Added.\n\r",ch);
  return TRUE;
}

MEDIT ( medit_delmprog )
{
  MOB_INDEX_DATA *pMob;
  MPROG_LIST *list;
  MPROG_LIST *list_next;
  char mprog[MAX_STRING_LENGTH];
  int64 value;
  int cnt = 0;

  EDIT_MOB(ch, pMob);

  one_argument( argument, mprog );
  if (!is_number( mprog ) || mprog[0] == '\0' )
  {
     stc("Syntax:  delmprog [#mprog]\n\r",ch);
     return FALSE;
  }

  value = atol ( mprog );

  if ( value < 0 )
  {
      stc("Only non-negative mprog-numbers allowed.\n\r",ch);
      return FALSE;
  }

  if ( !(list= pMob->mprogs) )
  {
      stc("MEdit:  Non existant mprog.\n\r",ch);
      return FALSE;
  }

  if ( value == 0 )
  {
      REM_BIT(pMob->mprog_flags, pMob->mprogs->trig_type);
      list = pMob->mprogs;
      pMob->mprogs = list->next;
      free_mprog( list );
  }
  else
  {
      while ( (list_next = list->next) && (++cnt < value ) )
              list = list_next;

      if ( list_next )
      {
              REM_BIT(pMob->mprog_flags, list_next->trig_type);
              list->next = list_next->next;
              free_mprog(list_next);
      }
      else
      {
              stc("No such mprog.\n\r",ch);
              return FALSE;
      }
  }

  stc("Mprog removed.\n\r", ch);
  return TRUE;
}

MEDIT( medit_hitroll )
{
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
    stc( "Syntax:  hitroll [number]\n\r", ch );
    return FALSE;
  }
  pMob->hitroll = atoi( argument );
  stc( "Hitroll set.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_liqlist )
{
  int liq;
  BUFFER *buffer;
  char buf[MAX_STRING_LENGTH];
  
  buffer = new_buf();
  do_printf(buf,"Name       ShowName          Color          Proof Full Thirst Food Ssize\n\r");
  
  for ( liq = 0; liq_table[liq].liq_name; liq++)
  {
    do_printf(buf, "%-20s %-35s %-20s %5d %4d %6d %4d %5d\n\r",
       liq_table[liq].liq_name,
       liq_table[liq].liq_showname,
       liq_table[liq].liq_color,
       liq_table[liq].liq_affect[0],liq_table[liq].liq_affect[1],
       liq_table[liq].liq_affect[2],liq_table[liq].liq_affect[3],
       liq_table[liq].liq_affect[4] );
    add_buf(buffer,buf);
  }

  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);

  return TRUE;
}

void fix_local( void )
{
   MOB_INDEX_DATA *pMobIndex;
   MPROG_LIST     *list;
   MPROG_CODE     *prog;
   int iHash;

   for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
   {
     for ( pMobIndex   = mob_index_hash[iHash];
           pMobIndex   != NULL;
           pMobIndex   = pMobIndex->next )
     {
       for( list = pMobIndex->mprogs; list != NULL; list = list->next )
       {
         if ( ( prog = get_mprog_index( list->vnum ) ) != NULL ) list->code = prog->code;
         else
         {
           bug( "Fix_local: code vnum %u not found.", list->vnum );
           exit( 1 );
         }
       }
     }
   }
}
