// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'


// The version info.  Please use this info when reporting bugs.
// It is displayed in the game by typing 'version' while editing.
// Do not remove these from the code - by request of Jason Dinkel

#define VERSION "ILAB Online Creation [Beta 1.0, ROM 2.3 modified]\n\r" \
                "     Port a ROM 2.4 v1.00\n\r"
#define AUTHOR  "     By Jason(jdinkelmines.colorado.edu)\n\r" \
                "     Modified for use with ROM 2.3\n\r"        \
                "     By Hans Birkeland (hansbiifi.uio.no)\n\r" \
                "     Modificado para uso en ROM 2.4v4\n\r"     \
                "     Por Birdie (itoledoramses.centic.utem.cl)\n\r" \
                "     Modified for use with Forgotten Dungeon\n\r" \
                "     by Sergey Kulik aka Saboteur\n\r"
#define DATE    "     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r" \
                "     (Port a ROM 2.4 - Nov 2, 1996)\n\r"
#define CREDITS "     Original by Surreality(cxw197psu.edu) and Locke(lockelm.com)"

typedef bool OLC_FUN            args( ( CHAR_DATA *ch, const char *argument ) );
#define DECLARE_OLC_FUN( fun )  OLC_FUN    fun

DECLARE_DO_FUN2(    do_help    );
DECLARE_SPELL_FUN( spell_null );
DECLARE_OLC_FUN( redit_mpshow   );

void aedit  args( ( CHAR_DATA *ch, const char *argument ) );
void redit  args( ( CHAR_DATA *ch, const char *argument ) );
void medit  args( ( CHAR_DATA *ch, const char *argument ) );
void oedit  args( ( CHAR_DATA *ch, const char *argument ) );
void mpedit args( ( CHAR_DATA *ch, const char *argument ) );

#define MAX_MOB 1 /* Default maximum number for resetting mobs */

struct olc_cmd_type
{
  char * const        name;
  OLC_FUN *           olc_fun;
};

struct  editor_cmd_type
{
  char * const        name;
  DO_FUN *            do_fun;
};

AREA_DATA *get_vnum_area args((int64 vnum));
AREA_DATA *get_area_data args((int vnum));
int64 flag_value  args((const struct flag_type *flag_table, const char *argument));
void  add_reset   args((ROOM_INDEX_DATA *room,RESET_DATA *pReset,int index));

extern const struct olc_cmd_type aedit_table[];
extern const struct olc_cmd_type redit_table[];
extern const struct olc_cmd_type oedit_table[];
extern const struct olc_cmd_type medit_table[];
extern const struct olc_cmd_type mpedit_table[];

DECLARE_DO_FUN2( do_aedit  );
DECLARE_DO_FUN2( do_redit  );
DECLARE_DO_FUN2( do_oedit  );
DECLARE_DO_FUN2( do_medit  );
DECLARE_DO_FUN2( do_mpedit );

bool show_commands args ( ( CHAR_DATA *ch, const char *argument ) );
bool show_help     args ( ( CHAR_DATA *ch, const char *argument ) );
bool edit_done     args ( ( CHAR_DATA *ch ) );
bool show_version  args ( ( CHAR_DATA *ch, const char *argument ) );

DECLARE_OLC_FUN( aedit_show     );
DECLARE_OLC_FUN( aedit_create   );
DECLARE_OLC_FUN( aedit_name     );
DECLARE_OLC_FUN( aedit_clan     );
DECLARE_OLC_FUN( aedit_file     );
DECLARE_OLC_FUN( aedit_age      );
DECLARE_OLC_FUN( aedit_reset    );
DECLARE_OLC_FUN( aedit_security );
DECLARE_OLC_FUN( aedit_builder  );
DECLARE_OLC_FUN( aedit_vnum     );
DECLARE_OLC_FUN( aedit_lvnum    );
DECLARE_OLC_FUN( aedit_uvnum    );
DECLARE_OLC_FUN( aedit_credits  );
DECLARE_OLC_FUN( aedit_flags    );

// Room Editor Prototypes
DECLARE_OLC_FUN( redit_show     );
DECLARE_OLC_FUN( redit_create   );
DECLARE_OLC_FUN( redit_delete   );
DECLARE_OLC_FUN( redit_undelete );
DECLARE_OLC_FUN( redit_name     );
DECLARE_OLC_FUN( redit_desc     );
DECLARE_OLC_FUN( redit_ed       );
DECLARE_OLC_FUN( redit_format   );
DECLARE_OLC_FUN( redit_north    );
DECLARE_OLC_FUN( redit_south    );
DECLARE_OLC_FUN( redit_east     );
DECLARE_OLC_FUN( redit_west     );
DECLARE_OLC_FUN( redit_up       );
DECLARE_OLC_FUN( redit_down     );
DECLARE_OLC_FUN( redit_mreset   );
DECLARE_OLC_FUN( redit_oreset   );
DECLARE_OLC_FUN( redit_mlist    );
DECLARE_OLC_FUN( redit_rlist    );
DECLARE_OLC_FUN( redit_olist    );
DECLARE_OLC_FUN( redit_mshow    );
DECLARE_OLC_FUN( redit_oshow    );
DECLARE_OLC_FUN( redit_heal     );
DECLARE_OLC_FUN( redit_mana     );

// Object Editor Prototypes
DECLARE_OLC_FUN( oedit_show      );
DECLARE_OLC_FUN( oedit_create    );
DECLARE_OLC_FUN( oedit_name      );
DECLARE_OLC_FUN( oedit_short     );
DECLARE_OLC_FUN( oedit_long      );
DECLARE_OLC_FUN( oedit_addaffect );
DECLARE_OLC_FUN( oedit_delaffect );
DECLARE_OLC_FUN( oedit_value0    );
DECLARE_OLC_FUN( oedit_value1    );
DECLARE_OLC_FUN( oedit_value2    );
DECLARE_OLC_FUN( oedit_value3    );
DECLARE_OLC_FUN( oedit_value4    );
DECLARE_OLC_FUN( oedit_weight    );
DECLARE_OLC_FUN( oedit_cost      );
DECLARE_OLC_FUN( oedit_ed        );

DECLARE_OLC_FUN( oedit_extra     );
DECLARE_OLC_FUN( oedit_wear      );
DECLARE_OLC_FUN( oedit_type      );
DECLARE_OLC_FUN( oedit_affect    );
DECLARE_OLC_FUN( oedit_material  );
DECLARE_OLC_FUN( oedit_level     );
DECLARE_OLC_FUN( oedit_durability);
DECLARE_OLC_FUN( oedit_condition );
DECLARE_OLC_FUN( oedit_liqlist   );

// Mobile Editor Prototypes
DECLARE_OLC_FUN( medit_show     );
DECLARE_OLC_FUN( medit_create   );
DECLARE_OLC_FUN( medit_name     );
DECLARE_OLC_FUN( medit_short    );
DECLARE_OLC_FUN( medit_long     );
DECLARE_OLC_FUN( medit_shop     );
DECLARE_OLC_FUN( medit_desc     );
DECLARE_OLC_FUN( medit_level    );
DECLARE_OLC_FUN( medit_align    );
DECLARE_OLC_FUN( medit_spec     );
DECLARE_OLC_FUN( medit_sex      );
DECLARE_OLC_FUN( medit_act      );
DECLARE_OLC_FUN( medit_affect   );
DECLARE_OLC_FUN( medit_ac       );
DECLARE_OLC_FUN( medit_form     );
DECLARE_OLC_FUN( medit_imm      );
DECLARE_OLC_FUN( medit_res      );
DECLARE_OLC_FUN( medit_vuln     );
DECLARE_OLC_FUN( medit_off      );
DECLARE_OLC_FUN( medit_size     );
DECLARE_OLC_FUN( medit_hitdice  );
DECLARE_OLC_FUN( medit_manadice );
DECLARE_OLC_FUN( medit_damdice  );
DECLARE_OLC_FUN( medit_race     );
DECLARE_OLC_FUN( medit_position );
DECLARE_OLC_FUN( medit_gold     );
DECLARE_OLC_FUN( medit_tarifa   );
DECLARE_OLC_FUN( medit_hitroll  );
DECLARE_OLC_FUN( medit_camino   );
DECLARE_OLC_FUN( medit_damtype  );
DECLARE_OLC_FUN( medit_addmprog );
DECLARE_OLC_FUN( medit_delmprog );
DECLARE_OLC_FUN( mpedit_create  );
DECLARE_OLC_FUN( mpedit_code    );
DECLARE_OLC_FUN( mpedit_help    );
DECLARE_OLC_FUN( mpedit_show    );
DECLARE_OLC_FUN( mpedit_clear   );

// Macros
#define IS_BUILDER(ch, Area)    ( ( ch->pcdata->security >= Area->security  \
                                || strstr( Area->builders, ch->name )     \
                                || strstr( Area->builders, "All" ) ) )
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

// Return pointers to what is being edited.
#define EDIT_MOB(Ch, Mob)   ( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)   ( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room) ( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area) ( Area = (AREA_DATA *)Ch->desc->pEdit )

// Prototypes
// mem.c - memory prototypes.
#define ED      EXTRA_DESCR_DATA
RESET_DATA      *new_reset_data  args( ( void ) );
void            free_reset_data  args( ( RESET_DATA *pReset ) );
AREA_DATA       *new_area        args( ( void ) );
void            free_area        args( ( AREA_DATA *pArea ) );
EXIT_DATA       *new_exit        args( ( void ) );
void            free_exit        args( ( EXIT_DATA *pExit ) );
ED              *new_extra_descr args( ( void ) );
void            free_extra_descr args( ( ED *pExtra ) );
ROOM_INDEX_DATA *new_room_index  args( ( void ) );
void            free_room_index  args( ( ROOM_INDEX_DATA *pRoom ) );
AFFECT_DATA     *new_affect      args( ( void ) );
void            free_affect      args( ( AFFECT_DATA* pAf ) );
SHOP_DATA       *new_shop        args( ( void ) );
void            free_shop        args( ( SHOP_DATA *pShop ) );
OBJ_INDEX_DATA  *new_obj_index   args( ( void ) );
void            free_obj_index   args( ( OBJ_INDEX_DATA *pObj ) );
MOB_INDEX_DATA  *new_mob_index   args( ( void ) );
void            free_mob_index   args( ( MOB_INDEX_DATA *pMob ) );
MPROG_LIST      *new_mprog       args( ( void ) );
void            free_mprog       args( ( MPROG_LIST *mp ) );
MPROG_CODE      *new_mpcode      args( (void) );
void            free_mpcode      (MPROG_CODE *pMcode);
#undef  ED
