// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "db.h"

bool run_olc_editor( DESCRIPTOR_DATA *d )
{
  switch ( d->editor )
  {
    case ED_AREA:
      aedit( d->character, d->incomm );
      break;
    case ED_ROOM:
      redit( d->character, d->incomm );
      break;
    case ED_OBJECT:
      oedit( d->character, d->incomm );
      break;
    case ED_MOBILE:
      medit( d->character, d->incomm );
      break;
    case ED_MPCODE:
      mpedit( d->character, d->incomm );
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

char *olc_ed_name( CHAR_DATA *ch )
{
  static char buf[10];
    
  buf[0] = '\0';
  switch (ch->desc->editor)
  {
    case ED_AREA:
      do_printf( buf, "AEdit" );
      break;
    case ED_ROOM:
      do_printf( buf, "REdit" );
      break;
    case ED_OBJECT:
      do_printf( buf, "OEdit" );
      break;
    case ED_MOBILE:
      do_printf( buf, "MEdit" );
      break;
    case ED_MPCODE:
      do_printf( buf, "MPEdit" );
      break;
    default:
      do_printf( buf, " " );
      break;
  }
  return buf;
}

char *olc_ed_vnum( CHAR_DATA *ch )
{
  AREA_DATA *pArea;
  ROOM_INDEX_DATA *pRoom;
  OBJ_INDEX_DATA *pObj;
  MOB_INDEX_DATA *pMob;
  static char buf[10];
      
  buf[0] = '\0';
  switch ( ch->desc->editor )
  {
    case ED_AREA:
      pArea = (AREA_DATA *)ch->desc->pEdit;
      do_printf( buf, "%d", pArea ? pArea->vnum : 0 );
      break;
    case ED_ROOM:
      pRoom = ch->in_room;
      do_printf( buf, "%u", pRoom ? pRoom->vnum : 0 );
      break;
    case ED_OBJECT:
      pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
      do_printf( buf, "%u", pObj ? pObj->vnum : 0 );
      break;
    case ED_MOBILE:
      pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
      do_printf( buf, "%u", pMob ? pMob->vnum : 0 );
      break;
    default:
      do_printf( buf, " " );
      break;
  }
  return buf;
}

void show_olc_cmds( CHAR_DATA *ch, const struct olc_cmd_type *olc_table )
{
  char buf  [ MAX_STRING_LENGTH ];
  char buf1 [ MAX_STRING_LENGTH ];
  int  cmd, col;

  buf1[0] = '\0';
  col = 0;
  for (cmd = 0; olc_table[cmd].name != NULL; cmd++)
  {
    do_printf( buf, "%-15s", olc_table[cmd].name );
    strcat( buf1, buf );
    if ( ++col % 5 == 0 ) strcat( buf1, "\n\r" );
  }

  if ( col % 5 != 0 ) strcat( buf1, "\n\r" );

  stc( buf1, ch );
}

bool show_commands( CHAR_DATA *ch, const char *argument )
{
  switch (ch->desc->editor)
  {
    case ED_AREA:
      show_olc_cmds( ch, aedit_table );
      break;
    case ED_ROOM:
      show_olc_cmds( ch, redit_table );
      break;
    case ED_OBJECT:
      show_olc_cmds( ch, oedit_table );
      break;
    case ED_MOBILE:
      show_olc_cmds( ch, medit_table );
      break;
    case ED_MPCODE:
      show_olc_cmds( ch, mpedit_table );
      break;
  }
  return FALSE;
}

const struct olc_cmd_type aedit_table[] =
{
  {   "age",      aedit_age       },
  {   "builder",  aedit_builder   },
  {   "commands", show_commands   },
  {   "create",   aedit_create    },
  {   "filename", aedit_file      },
  {   "name",     aedit_name      },
  {   "clan",     aedit_clan      },
  {   "reset",    aedit_reset     },
  {   "security", aedit_security  },
  {   "show",     aedit_show      },
  {   "vnum",     aedit_vnum      },
  {   "lvnum",    aedit_lvnum     },
  {   "uvnum",    aedit_uvnum     },
  {   "credits",  aedit_credits   },
  {   "flag",     aedit_flags     },
  {   "?",        show_help       },
  {   "version",  show_version    },
  {   NULL,       0,              }
};

const struct olc_cmd_type redit_table[] =
{
  {   "commands", show_commands   },
  {   "create",   redit_create    },
  {   "delete",   redit_delete    },
  {   "undelete", redit_undelete  },
  {   "desc",     redit_desc      },
  {   "ed",       redit_ed        },
  {   "format",   redit_format    },
  {   "name",     redit_name      },
  {   "show",     redit_show      },
  {   "heal",     redit_heal      },
  {   "mana",     redit_mana      },
  {   "north",    redit_north     },
  {   "south",    redit_south     },
  {   "east",     redit_east      },
  {   "west",     redit_west      },
  {   "up",       redit_up        },
  {   "down",     redit_down      },

  {   "mreset",   redit_mreset    },
  {   "oreset",   redit_oreset    },
  {   "mlist",    redit_mlist     },
  {   "rlist",    redit_rlist     },
  {   "olist",    redit_olist     },
  {   "mshow",    redit_mshow     },
  {   "oshow",    redit_oshow     },

  {   "?",        show_help       },
  {   "version",  show_version    },

  {   NULL,       0,              }
};

const struct olc_cmd_type oedit_table[] =
{
  {   "addaffect", oedit_addaffect },
  {   "commands",  show_commands   },
  {   "cost",      oedit_cost      },
  {   "create",    oedit_create    },
  {   "delaffect", oedit_delaffect },
  {   "ed",        oedit_ed        },
  {   "long",      oedit_long      },
  {   "name",      oedit_name      },
  {   "short",     oedit_short     },
  {   "show",      oedit_show      },
  {   "v0",        oedit_value0    },
  {   "v1",        oedit_value1    },
  {   "v2",        oedit_value2    },
  {   "v3",        oedit_value3    },
  {   "v4",        oedit_value4    },
  {   "weight",    oedit_weight    },

  {   "extra",     oedit_extra     },
  {   "wear",      oedit_wear      },
  {   "type",      oedit_type      },
  {   "material",  oedit_material  },
  {   "level",     oedit_level     },
  {   "durability",oedit_durability},
  {   "condition", oedit_condition },
  {   "liqlist",   oedit_liqlist   },
  {   "?",         show_help       },
  {   "version",   show_version    },

  {   NULL,        0,              }
};

const struct olc_cmd_type mpedit_table[] =
{
 { "commands",show_commands }, 
 { "create",  mpedit_create },
 { "code",    mpedit_code   },
 { "clear",   mpedit_clear  },
 { "show",    mpedit_show   },
 { "?",       mpedit_help   },
 { NULL,      0             }
};

const struct olc_cmd_type medit_table[] =
{
    {   "alignment",    medit_align     },
    {   "commands",     show_commands   },
    {   "create",       medit_create    },
    {   "desc",         medit_desc      },
    {   "level",        medit_level     },
    {   "long",         medit_long      },
    {   "name",         medit_name      },
    {   "shop",         medit_shop      },
    {   "short",        medit_short     },
    {   "show",         medit_show      },
    {   "spec",         medit_spec      },

    {   "sex",          medit_sex       },
    {   "act",          medit_act       },
    {   "affect",       medit_affect    },
    {   "armor",        medit_ac        },
    {   "form",         medit_form      },
    {   "imm",          medit_imm       },
    {   "res",          medit_res       },
    {   "vuln",         medit_vuln      },
    {   "off",          medit_off       },
    {   "size",         medit_size      },
    {   "hitdice",      medit_hitdice   },
    {   "manadice",     medit_manadice  },
    {   "damdice",      medit_damdice   },
    {   "race",         medit_race      },
    {   "position",     medit_position  },
    {   "wealth",       medit_gold      },
    {   "hitroll",      medit_hitroll   },
    {   "damtype",      medit_damtype   },
    {   "addmprog",     medit_addmprog  },
    {   "delmprog",     medit_delmprog  },
    {   "?",            show_help       },
    {   "version",      show_version    },

    {   NULL,           0,              }
};

AREA_DATA *get_area_data( int vnum )
{
  AREA_DATA *pArea;

  for (pArea = area_first; pArea; pArea = pArea->next )
  {
    if (pArea->vnum == vnum) return pArea;
  }
  return 0;
}

bool edit_done( CHAR_DATA *ch )
{
  ch->desc->pEdit = NULL;
  ch->desc->editor = 0;
  return FALSE;
}

void aedit( CHAR_DATA *ch, const char *argument )
{
  AREA_DATA *pArea;
  char command[MAX_INPUT_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int  cmd;
  int64 value;

  EDIT_AREA(ch, pArea);
  strcpy( arg, argument );
  argument = one_argument( argument, command );

  if ( !IS_BUILDER( ch, pArea ) )
  {
    stc( "AEdit:  Insufficient security to modify area.\n\r", ch );
    edit_done( ch );
    return;
  }

  if ( !str_cmp(command, "done") )
  {
    edit_done( ch );
    return;
  }

  if ( !IS_BUILDER( ch, pArea ) )
  {
    interpret( ch, arg );
    return;
  }

  if ( command[0] == '\0' )
  {
    aedit_show( ch, argument );
    return;
  }

  if ( ( value = flag_value( area_flags, command ) ) != NO_FLAG )
  {
    TOGGLE_BIT(pArea->area_flags, value);

    stc( "Flag toggled.\n\r", ch );
    return;
  }

  for ( cmd = 0; aedit_table[cmd].name != NULL; cmd++ )
  {
    if ( !str_prefix( command, aedit_table[cmd].name ) )
    {
      if ( (*aedit_table[cmd].olc_fun) ( ch, argument ) )
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
      }
      else return;
    }
  }
  interpret( ch, arg );
}

void redit( CHAR_DATA *ch, const char *argument )
{
  ROOM_INDEX_DATA *pRoom;
  AREA_DATA *pArea;
  char arg[MAX_STRING_LENGTH];
  char command[MAX_INPUT_LENGTH];
  int  cmd;
  int64  value;

  EDIT_ROOM(ch, pRoom);
  pArea = pRoom->area;

  strcpy( arg, argument );
  argument = one_argument( argument, command );

  if ( !IS_BUILDER( ch, pArea ) )
  {
    stc( "REdit:  Insufficient security to modify room.\n\r", ch );
    edit_done( ch );
    return;
  }

  if ( !str_cmp(command, "done") )
  {
    edit_done( ch );
    return;
  }

  if ( !IS_BUILDER( ch, pArea ) )
  {
    interpret( ch, arg );
    return;
  }

  if ( command[0] == '\0' )
  {
    redit_show( ch, argument );
    return;
  }

  if ((value=flag_value(room_flags, command))!=NO_FLAG)
  {
    TOGGLE_BIT(pRoom->room_flags, value);

    SET_BIT( pArea->area_flags, AREA_CHANGED );
    stc( "Room flag toggled.\n\r", ch );
    return;
  }

  if ( ( value = flag_value( sector_flags, command ) ) != NO_FLAG )
  {
    pRoom->sector_type  = value;

    SET_BIT( pArea->area_flags, AREA_CHANGED );
    stc( "Sector type set.\n\r", ch );
    return;
  }

  for ( cmd = 0; redit_table[cmd].name != NULL; cmd++ )
  {
    if ( !str_prefix( command, redit_table[cmd].name ) )
    {
      if ( (*redit_table[cmd].olc_fun) ( ch, argument ) )
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
      }
      else return;
    }
  }
  interpret( ch, arg );
}

void oedit( CHAR_DATA *ch, const char *argument )
{
  AREA_DATA *pArea;
  OBJ_INDEX_DATA *pObj;
  char arg[MAX_STRING_LENGTH];
  char command[MAX_INPUT_LENGTH];
  int  cmd;

  strcpy( arg, argument );
  argument = one_argument( argument, command );

  EDIT_OBJ(ch, pObj);
  pArea = pObj->area;

  if ( !IS_BUILDER( ch, pArea ) )
  {
    stc( "OEdit: Insufficient security to modify area.\n\r", ch );
    edit_done( ch );
    return;
  }

  if ( !str_cmp(command, "done") )
  {
    edit_done( ch );
    return;
  }

  if ( !IS_BUILDER( ch, pArea ) )
  {
    interpret( ch, arg );
    return;
  }

  if ( command[0] == '\0' )
  {
    oedit_show( ch, argument );
    return;
  }

  for ( cmd = 0; oedit_table[cmd].name != NULL; cmd++ )
  {
    if ( !str_prefix( command, oedit_table[cmd].name ) )
    {
      if ( (*oedit_table[cmd].olc_fun) ( ch, argument ) )
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
      }
      else return;
    }
  }
  interpret( ch, arg );
}

void medit( CHAR_DATA *ch, const char *argument )
{
  AREA_DATA *pArea;
  MOB_INDEX_DATA *pMob;
  char command[MAX_INPUT_LENGTH];
  char arg[MAX_STRING_LENGTH];
  int  cmd;

  strcpy( arg, argument );
  argument = one_argument( argument, command );

  EDIT_MOB(ch, pMob);
  pArea = pMob->area;

  if ( !IS_BUILDER( ch, pArea ) )
  {
    stc( "MEdit: Insufficient security to modify area.\n\r", ch );
    edit_done( ch );
    return;
  }

  if ( !str_cmp(command, "done") )
  {
    edit_done( ch );
    return;
  }

  if ( !IS_BUILDER( ch, pArea ) )
  {
    interpret( ch, arg );
    return;
  }

  if ( command[0] == '\0' )
  {
    medit_show( ch, argument );
    return;
  }

  for ( cmd = 0; medit_table[cmd].name != NULL; cmd++ )
  {
    if ( !str_prefix( command, medit_table[cmd].name ) )
    {
      if ( (*medit_table[cmd].olc_fun) ( ch, argument ) )
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
      }
      else return;
    }
  }
  interpret( ch, arg );
}

const struct editor_cmd_type editor_table[] =
{
    {   "area",         do_aedit        },
    {   "room",         do_redit        },
    {   "object",       do_oedit        },
    {   "mobile",       do_medit        },
    {   NULL,           0,              }
};

void do_olc( CHAR_DATA *ch, const char *argument )
{
  char command[MAX_INPUT_LENGTH];
  int  cmd;

  argument = one_argument( argument, command );

  if ( command[0] == '\0' )
  {
    do_help( ch, "olc" );
    return;
  }

  for ( cmd = 0; editor_table[cmd].name != NULL; cmd++ )
  {
    if ( !str_prefix( command, editor_table[cmd].name ) )
    {
      (*editor_table[cmd].do_fun) ( ch, argument );
      return;
    }
  }
  do_help( ch, "olc" );
}

void do_aedit( CHAR_DATA *ch, const char *argument )
{
  AREA_DATA *pArea;
  int value;
  char value2[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];

  pArea = ch->in_room->area;

  argument = one_argument(argument,arg);
  if ( is_number( arg ) )
  {
    value = atoi( arg );
    if ( !( pArea = get_area_data( value ) ) )
    {
      stc( "That area vnum does not exist.\n\r", ch );
      return;
    }
  }
  else
  {
    if ( !str_cmp( arg, "create" ) )
    {
      if (!IS_NPC(ch) && (ch->pcdata->security < 9) )
      {
        stc("Insuficiente seguridad para crear areas.\n\r",ch);
        return;
      }
      argument            = one_argument(argument,value2);
      value = atoi (value2);
      if (get_area_data(value) != NULL)
      {
        stc("That area vnum does not exist.\n\r",ch);
        return;
      }
      pArea               = new_area();
      area_last->next     = pArea;
      area_last           = pArea;      /* Thanks, Walker. */
      SET_BIT( pArea->area_flags, AREA_ADDED );
      stc("Area created.\n\r",ch);
    }
  }

  if (!IS_BUILDER(ch,pArea))
  {
    stc("У тебя недостаточно прав для редактирования арий.\n\r",ch);
    return;
  }

  ch->desc->pEdit = (void *)pArea;
  ch->desc->editor = ED_AREA;
}

void do_redit( CHAR_DATA *ch, const char *argument )
{
  ROOM_INDEX_DATA *pRoom, *pRoom2;
  char arg1[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );

  pRoom = ch->in_room;

  if ( !str_cmp( arg1, "reset" ) )
  {
    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
      stc( "У тебя недостаточно прав для редактирования комнат.\n\r" , ch );
      return;
    }
    reset_room( pRoom );
    stc( "Room reset.\n\r", ch );
    return;
  }
  else
  if ( !str_cmp( arg1, "create" ) )
  {
    if ( argument[0] == '\0' || atoi64( argument ) == 0 )
    {
      stc( "Syntax:  edit room create [vnum]\n\r", ch );
      return;
    }

    if ( redit_create( ch, argument ) )
    {
      char_from_room( ch );
      char_to_room( ch, ch->desc->pEdit );
      SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
      pRoom = ch->in_room;
    }
  }
  else
  {
    pRoom2 = get_room_index(atoi64(arg1));

    if ( (pRoom2 != NULL) && IS_BUILDER(ch,pRoom2->area) )
    {
      if( IS_SET( pRoom2->room_flags, ROOM_ELDER)
        && !IS_ELDER(ch) )
      {  
         stc("Ты не можешь редактировать эту комнату.\n\r", ch);
         return;
      }
      char_from_room( ch );
      char_to_room( ch, pRoom2 );
      pRoom = ch->in_room;
    }
    else if(atoi64(arg1) != 0)
    {
      stc("У тебя недостаточно прав для редактирования комнат, или комната не существует.\n\r",ch);
      return;
    }   
  }
  if ( !IS_BUILDER( ch, pRoom->area ) )
  {
    stc( "У тебя недостаточно прав для редактирования комнат.\n\r" , ch );
    return;
  }
  ch->desc->editor = ED_ROOM;
}

void do_oedit( CHAR_DATA *ch, const char *argument )
{
  OBJ_INDEX_DATA *pObj;
  AREA_DATA *pArea;
  char arg1[MAX_STRING_LENGTH];
  int64 value;

  argument = one_argument( argument, arg1 );

  if ( is_number( arg1 ) )
  {
    value = atoi64( arg1 );
    if ( !( pObj = get_obj_index( value ) ) )
    {
      stc( "OEdit:  That vnum does not exist.\n\r", ch );
      return;
    }

    if ( !IS_BUILDER( ch, pObj->area ) )
    {
      stc( "У тебя недостаточно прав для редактирования предметов.\n\r" , ch );
      return;
    }

    ch->desc->pEdit = (void *)pObj;
    ch->desc->editor = ED_OBJECT;
    return;
  }
  else
  {
    if ( !str_cmp( arg1, "create" ) )
    {
      value = atoi64( argument );
      if ( argument[0] == '\0' || value == 0 )
      {
        stc( "Syntax:  edit object create [vnum]\n\r", ch );
        return;
      }

      pArea = get_vnum_area( value );

      if ( !pArea )
      {
        stc( "OEdit:  That vnum is not assigned an area.\n\r", ch );
        return;
      }

      if ( !IS_BUILDER( ch, pArea ) )
      {
        stc( "У тебя недостаточно прав для редактирования предметов.\n\r" , ch );
        return;
      }

      if ( oedit_create( ch, argument ) )
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        ch->desc->editor = ED_OBJECT;
      }
      return;
    }
    if ( !str_cmp( arg1, "delete" ) )
    {
      OBJ_INDEX_DATA *obji=NULL;

      value = atoi64( argument );
      if ( argument[0] == '\0' || value == 0 )
      {
        stc( "Syntax:  edit object delete [vnum]\n\r", ch );
        return;
      }

      pArea = get_vnum_area( value );

      if ( !pArea )
      {
        stc( "OEdit:  That vnum is not assigned an area.\n\r", ch );
        return;
      }

      if ( !IS_BUILDER( ch, pArea ) )
      {
        stc( "У тебя недостаточно прав для редактирования предметов.\n\r" , ch );
        return;
      }

      obji=get_obj_index(value);

      if (obji)
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        SET_BIT( obji->extra_flags, ITEM_DELETED);
        ptc(ch,"[%u] (%s) marked as deleted.\n\r",obji->vnum,obji->name);
      }
      else stc("Item is not exist.\n\r",ch);
      return;
    }
  }
  stc( "OEdit:  There is no default object to edit.\n\r", ch );
}

void do_medit( CHAR_DATA *ch, const char *argument )
{
  MOB_INDEX_DATA *pMob;
  AREA_DATA *pArea;
  int64 value;
  char arg1[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );

  if ( is_number( arg1 ) )
  {
    value = atoi64( arg1 );
    if ( !( pMob = get_mob_index( value ) ))
    {
      stc( "MEdit:  That vnum does not exist.\n\r", ch );
      return;
    }

    if ( !IS_BUILDER( ch, pMob->area ) )
    {
      stc( "У тебя недостаточно прав для редактирования монстров.\n\r" , ch );
      return;
    }

    ch->desc->pEdit = (void *)pMob;
    ch->desc->editor = ED_MOBILE;
    return;
  }
  else
  {
    if ( !str_cmp( arg1, "create" ) )
    {
      value = atoi64( argument );
      if ( arg1[0] == '\0' || value == 0 )
      {
        stc( "Syntax:  edit mobile create [vnum]\n\r", ch );
        return;
      }

      pArea = get_vnum_area( value );

      if ( !pArea )
      {
        stc( "OEdit:  That vnum is not assigned an area.\n\r", ch );
        return;
      }

      if ( !IS_BUILDER( ch, pArea ) )
      {
        stc( "У тебя недостаточно прав для редактирования монстров.\n\r" , ch );
        return;
      }

      if ( medit_create( ch, argument ) )
      {
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        ch->desc->editor = ED_MOBILE;
      }
      return;
    }
  }
  stc( "MEdit:  There is no default mobile to edit.\n\r", ch );
}

void display_resets( CHAR_DATA *ch )
{
  ROOM_INDEX_DATA     *pRoom;
  RESET_DATA          *pReset;
  MOB_INDEX_DATA      *pMob = NULL;
  char                buf   [ MAX_STRING_LENGTH ];
  char                final [ MAX_STRING_LENGTH ];
  int                 iReset = 0;

  EDIT_ROOM(ch, pRoom);
  final[0]  = '\0';
  
  stc ( 
"#N   Vnum      Описание ресета     Куда  грузится    Vnum   Mx Mn Описание\n\r"
"== ======== ===================== ================ ======== ===== ===========\n\r", ch );

  for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
  {
    OBJ_INDEX_DATA  *pObj;
    MOB_INDEX_DATA  *pMobIndex;
    OBJ_INDEX_DATA  *pObjIndex;
    OBJ_INDEX_DATA  *pObjToIndex;
    ROOM_INDEX_DATA *pRoomIndex;

    final[0] = '\0';
    do_printf( final, "{C%2d{x.", ++iReset );

    switch ( pReset->command )
    {
      default:
        do_printf( buf, "{RReset: неизвестный ресет: %c.{x", pReset->command );
        strcat( final, buf );
        break;
      case 'M':
        if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
        {
          do_printf( buf, "{RReset mob2room - не найден mobile %u{x\n\r", pReset->arg1 );
          strcat( final, buf );
          continue;
        }
        if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
        {
          do_printf( buf, "{RReset mob2room - не найдена комната %u{x\n\r", pReset->arg3 );
          strcat( final, buf );
          continue;
        }

        pMob = pMobIndex;
        do_printf( buf, "{GM[%5u]{x %-21s {Gв комнату {x       {CR[%5u]{x %2d-%2d %s\n\r",
          pReset->arg1, get_mobindex_desc(pMob,'1'), pReset->arg3,
          pReset->arg2, pReset->arg4, pRoomIndex->name );
        strcat( final, buf );
        {
          ROOM_INDEX_DATA *pRoomIndexPrev;

          pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
          if ( pRoomIndexPrev
           && IS_SET(pRoomIndexPrev->room_flags,ROOM_PET_SHOP)) final[5]='P';
        }
        break;

      case 'O':
        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
        {
          do_printf( buf, "{RReset obj2room - предмет %u не найден{x\n\r",pReset->arg1 );
          strcat( final, buf );
          continue;
        }
        pObj       = pObjIndex;
        if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
        {
          do_printf( buf, "{RReset obj2room - комната %u не найдена{x\n\r", pReset->arg3 );
          strcat( final, buf );
          continue;
        }
        do_printf( buf, "{MO[%5u]{x %-21s {Gв комнату{x        {CR[%5u]{x       %s\n\r",
          pReset->arg1,get_objindex_desc(pObj,'1'),pReset->arg3, pRoomIndex->name);
        strcat( final, buf );
        break;

      case 'P':
        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
        {
          do_printf( buf, "{RReset obj2obj - предмет %u не найден.{x\n\r",pReset->arg1 );
          strcat( final, buf );
          continue;
        }

        pObj = pObjIndex;

        if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
        {
          do_printf( buf, "{RReset obj2obj - не найден предмет %u для помещения внутрь{x\n\r",pReset->arg3 );
          strcat( final, buf );
          continue;
        }

        do_printf( buf,
              "{MO[%5u]{x %-21s {Gвнутрь{x           {MO[%5u]{x %2d-%2d %s\n\r",
              pReset->arg1,get_objindex_desc(pObj,'1'),pReset->arg3,
              pReset->arg2,pReset->arg4,pObjToIndex->short_descr );
        strcat( final, buf );
        break;

      case 'G':
      case 'E':
        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
        {
          do_printf( buf, "{RReset obj2mob - объект %u не найден{x\n\r",pReset->arg1 );
          strcat( final, buf );
          continue;
        }

        pObj       = pObjIndex;

        if ( !pMob )
        {
          do_printf( buf, "{RReset obj2mob - не найден mobile для экипировки.{x\n\r" );
          strcat( final, buf );
          break;
        }

        if ( pMob->pShop )
        {
          do_printf( buf,
              "{MO[%5u]{x %-21s {Gв инвентори{x      {ПS[%5u]{x       %s\n\r",
              pReset->arg1,get_objindex_desc(pObj,'1'),                           
              pMob->vnum,get_mobindex_desc(pMob,'2'));
        }
        else
        do_printf( buf,
              "{MO[%5u]{x %-21s {G%-16s{x {GM[%5u]{x       %s\n\r",
              pReset->arg1,get_objindex_desc(pObj,'1'),
              (pReset->command == 'G') ?
                  flag_string( wear_loc_strings, WEAR_NONE )
                : flag_string( wear_loc_strings, pReset->arg3 ),
                pMob->vnum,
                get_mobindex_desc(pMob,'2'));
        strcat( final, buf );
        break;

      case 'D':
        pRoomIndex = get_room_index( pReset->arg1 );
        do_printf( buf, "{CR[%5u]{x %s дверь %-16s выход в %s\n\r",
              pReset->arg1,
              capitalize( dir_name[ pReset->arg2 ] ),
              pRoomIndex->name,
              flag_string( door_resets, pReset->arg3 ) );
        strcat( final, buf );

        break;
      case 'R':
        if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
        {
          do_printf( buf, "{RReset random_room - неправильная комната %u{x\n\r",pReset->arg1 );
          strcat( final, buf );
          continue;
        }

        do_printf( buf, "{CR[%5u]{x выход случайным образом в %s\n\r",
              pReset->arg1, pRoomIndex->name );
        strcat( final, buf );
        break;
    }
    stc( final, ch );
  }
}

void add_reset( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index )
{
  RESET_DATA *reset;
  int iReset = 0;

  if ( !room->reset_first )
  {
    room->reset_first = pReset;
    room->reset_last  = pReset;
    pReset->next      = NULL;
    return;
  }
  index--;

  if ( index == 0 )   /* First slot (1) selected. */
  {
    pReset->next = room->reset_first;
    room->reset_first = pReset;
    return;
  }

  // If negative slot( <= 0 selected) then this will find the last.
  for ( reset = room->reset_first; reset->next; reset = reset->next )
  {
    if ( ++iReset == index ) break;
  }

  pReset->next        = reset->next;
  reset->next         = pReset;
  if ( !pReset->next ) room->reset_last = pReset;
}

void do_resets( CHAR_DATA *ch, const char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char arg4[MAX_INPUT_LENGTH];
  char arg5[MAX_INPUT_LENGTH];
  char arg6[MAX_INPUT_LENGTH];
  char arg7[MAX_INPUT_LENGTH];
  RESET_DATA *pReset = NULL;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  argument = one_argument( argument, arg4 );
  argument = one_argument( argument, arg5 );
  argument = one_argument( argument, arg6 );
  argument = one_argument( argument, arg7 );

  if ( !IS_BUILDER( ch, ch->in_room->area ) )
  {
    stc( "Resets: Invalid security for editing this area.\n\r",ch );
    return;
  }

  if ( arg1[0] == '\0' )
  {
    if ( ch->in_room->reset_first )
    {
      stc("Обозначения: {MM{xobile, {MR{xoom, {MO{xbject, {MP{xet, {MS{xhopkeeper\n\r", ch );
      display_resets( ch );
    }
    else stc( "No resets in this room.\n\r", ch );
  }

  if ( is_number( arg1 ) )
  {
    ROOM_INDEX_DATA *pRoom = ch->in_room;

    if ( !str_cmp( arg2, "delete" ) )
    {
      int insert_loc = atoi( arg1 );

      if ( !ch->in_room->reset_first )
      {
        stc( "No resets in this area.\n\r", ch );
        return;
      }
      if ( insert_loc-1 <= 0 )
      {
        pReset = pRoom->reset_first;
        pRoom->reset_first = pRoom->reset_first->next;
        if ( !pRoom->reset_first ) pRoom->reset_last = NULL;
      }
      else
      {
        int iReset = 0;
        RESET_DATA *prev = NULL;

        for ( pReset = pRoom->reset_first;pReset;pReset = pReset->next )
        {
          if ( ++iReset == insert_loc ) break;
          prev = pReset;
        }
        if ( !pReset )
        {
          stc( "Reset not found.\n\r", ch );
          return;
        }
        if ( prev ) prev->next = prev->next->next;
        else        pRoom->reset_first = pRoom->reset_first->next;

        for ( pRoom->reset_last = pRoom->reset_first;
                  pRoom->reset_last->next;
                  pRoom->reset_last = pRoom->reset_last->next );
      }
      free_reset_data( pReset );
      stc( "Reset deleted.\n\r", ch );
    }
    else if ( (!str_cmp( arg2, "mob" ) && is_number( arg3 ))
          || (!str_cmp( arg2, "obj" ) && is_number( arg3 )) )
    {
      if ( !str_cmp( arg2, "mob" ) )
      {
        pReset = new_reset_data();
        pReset->command = 'M';
        if (get_mob_index( is_number(arg3) ? atoi64( arg3 ) : 1 ) == NULL)
        {
          stc("Монстр не существует.\n\r",ch);
          return;
        }
        pReset->arg1    = atoi64( arg3 );
        pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 1; /* Max # */
        pReset->arg3    = ch->in_room->vnum;
        pReset->arg4    = is_number( arg5 ) ? atoi( arg5 ) : 1; /* Min # */
      }
      else if ( !str_cmp( arg2, "obj" ) )
      {
        pReset = new_reset_data();
        pReset->arg1    = atoi64( arg3 );
        if ( !str_prefix( arg4, "inside" ) )
        {
          pReset->command = 'P';
          pReset->arg2    = 0;
          if ((get_obj_index(is_number(arg5)? atoi64(arg5) : 1 ))->item_type != ITEM_CONTAINER)
          {
            stc( "Предмет не является контейнером.\n\r", ch);
            return;
          }
          pReset->arg2    = is_number( arg6 ) ? atoi( arg6 ) : 1;
          pReset->arg3    = is_number( arg5 ) ? atoi64( arg5 ) : 1;
          pReset->arg4    = is_number( arg7 ) ? atoi( arg7 ) : 1;
        }
        else if ( !str_cmp( arg4, "room" ) )
        {
          pReset           = new_reset_data();
          pReset->command  = 'O';
          if (get_obj_index(atoi64(arg3)) == NULL)
          {
            stc( "Предметов с таким номером не существует.\n\r",ch);
            return;
          }
          pReset->arg1     = atoi64( arg3 );
          pReset->arg2     = 0;
          pReset->arg3     = ch->in_room->vnum;
          pReset->arg4     = 0;
        }
        else
        {
          if ( flag_value( wear_loc_flags, arg4 ) == NO_FLAG )
          {
            stc( "Resets: '? wear-loc'\n\r", ch );
            return;
          }
          pReset = new_reset_data();
          if (get_obj_index(atoi64(arg3)) == NULL)
          {
            stc( "Предметов с таким номером не существует.\n\r",ch);
            return;
          }
          pReset->arg1 = atoi64(arg3);
          pReset->arg3 = flag_value( wear_loc_flags, arg4 );
          if ( pReset->arg3 == WEAR_NONE ) pReset->command = 'G';
          else pReset->command = 'E';
        }
      }
      add_reset( ch->in_room, pReset, atoi( arg1 ) );
      SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
      stc( "Reset added.\n\r", ch );
    }
    else
    {
      stc( "Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch );
      stc( "        RESET <number> OBJ <vnum> inside <vnum> [limit] [count]\n\r", ch );
      stc( "        RESET <number> OBJ <vnum> room\n\r", ch );
      stc( "        RESET <number> MOB <vnum> [max # area] [max # room]\n\r", ch );
      stc( "        RESET <number> DELETE\n\r", ch );
    }
  }
}

void do_alist( CHAR_DATA *ch, const char *argument )
{
  AREA_DATA *pArea;
  int a_first=1, a_last=200;
  char arg[MAX_INPUT_LENGTH];

  if (!EMPTY(argument))
  {
    argument=one_argument(argument,arg);

    if (is_number(arg))
    {
      a_first=atoi(arg)+1;
      one_argument(argument,arg);
      if (is_number(arg)) a_last=atoi(arg)+2;
    }
    else
    {
      if (!str_cmp(arg,"help"))
      {
        stc("alist [<first area num>|<filename>|help]\n\r",ch);
        return;
      }
      else a_first=0;
    }
  }
  ptc(ch,"\n\r%3s Sec [%27s] (%5s-%5s) %15s [%-10s] [%s]\n\r-------------------------------------------------------------------------------------\n\r",
    "Num", "Area Name", "lvnum", "uvnum", "Filename", "Builders", "Clan" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    if (--a_last<1) break;
    if (a_first>1)
    {
      a_first--;
      continue;
    }
    if (a_first==0 && str_prefix(arg,pArea->file_name)) continue;

    ptc(ch,"%3d [%d] {W%-29s {x(%5u-%5u) %15s [%10s] [%s]\n\r",
    pArea->vnum,pArea->security,pArea->name,
    pArea->min_vnum,pArea->max_vnum,
    pArea->file_name,pArea->builders,pArea->clan);
  }
}
