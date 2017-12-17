// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

struct flag_stat_type
{
  const struct flag_type *structure;
  bool stat;
};

/*****************************************************************************
 Name:          flag_stat_table
 Purpose:       This table catagorizes the tables following the lookup
                functions below into stats and flags.  Flags can be toggled
                but stats can only be assigned.  Update this table when a
                new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] =
{
/*{ structure          stat  }, */
  { area_flags,        FALSE },
  { sex_flags,         TRUE  },
  { exit_flags,        FALSE },
  { door_resets,       TRUE  },
  { room_flags,        FALSE },
  { sector_flags,      TRUE  },
  { type_flags,        TRUE  },
  { act_flags,         FALSE },
  { extra_flags,       FALSE },
  { wear_flags,        FALSE },
  { affect_flags,      FALSE },
  { apply_flags,       TRUE  },
  { wear_loc_flags,    TRUE  },
  { wear_loc_strings,  TRUE  },
  { weapon_flags,      TRUE  },
  { container_flags,   FALSE },

// ROM specific flags:
//{ material_type,  TRUE  },
  { form_flags,     FALSE },
  { part_flags,     FALSE },
  { ac_type,        TRUE  },
  { size_flags,     TRUE  },
  { position_flags, TRUE  },
  { off_flags,      FALSE },
  { imm_flags,      FALSE },
  { res_flags,      FALSE },
  { vuln_flags,     FALSE },
  { weapon_class,   TRUE  },
  { weapon_type2,   FALSE },
  { penalty_flags,  TRUE  },
  { 0,              0     }
};

/*****************************************************************************
 Name:          is_stat( table )
 Purpose:       Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:     flag_value and flag_string.
 Note:          This function is local and used only in bit.c.
 ****************************************************************************/
bool is_stat( const struct flag_type *flag_table )
{
  int flag;

  for (flag = 0; flag_stat_table[flag].structure; flag++)
  {
    if( flag_stat_table[flag].structure == flag_table
      && flag_stat_table[flag].stat ) return TRUE;
  }
  return FALSE;
}

// Purpose:  Returns the value of the flags entered. Multi-flags accepted.
// Called by:olc.c and olc_act.c.
int64 flag_value( const struct flag_type *flag_table, const char *argument)
{
  char word[MAX_INPUT_LENGTH];
  int64 bit, marked = 0;
  bool found = FALSE;

  if (is_stat( flag_table ))
  {
    one_argument(argument, word);

    if ((bit=flag_lookup(word,flag_table))!=NO_FLAG) return bit;
    return NO_FLAG;
  }

  for (;;)
  {
    argument = one_argument(argument, word);

    if (word[0] == '\0') break;

    if ((bit = flag_lookup(word, flag_table))!= NO_FLAG)
    {
      SET_BIT(marked, bit);
      found = TRUE;
    }
  }
  if (found) return marked;
  else return NO_FLAG;
}

char *flag_string( const struct flag_type *flag_table, int64 bits )
{
  static char buf[512];
  int flag;

  buf[0] = '\0';

  for (flag = 0; flag_table[flag].name != NULL; flag++)
  {
    if ( !is_stat( flag_table ) && IS_SET(bits, flag_table[flag].bit) )
    {
      strcat( buf, " " );
      strcat( buf, flag_table[flag].name );
    }
    else if ( flag_table[flag].bit == bits )
    {
      strcat( buf, " " );
      strcat( buf, flag_table[flag].name );
      break;
    }
  }
  return (buf[0] != '\0') ? buf+1 : "нет";
}

