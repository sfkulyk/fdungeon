// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

// Uncomment this line for compile public restricted version of Fdungeon.
// Saboteur.
// #define PUBLIC
#ifndef __merc
#define __merc

#define FreeBSD
#if defined(TRADITIONAL)
#define const
#define DECLARE_SPEC_FUN( fun )   bool fun( )
#define DECLARE_SPELL_FUN( fun )  void fun( )
#else
#define const
#define DECLARE_SPEC_FUN( fun )   SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )  SPELL_FUN fun
#endif

#define args( list ) list
#define DECLARE_DO_FUN2( fun )   void fun( CHAR_DATA* ch, const char* argument )
#define DECLARE_DO_FUN1( fun )  void fun( CHAR_DATA* ch)

// Assume BSD compatible GCC - By Shiva
#if defined (WIN32)
typedef __int64    int64;
typedef unsigned __int64 uint64;

#else
#include <stdlib.h>
// typedef int64_t int64; // For OLD FreeBSD
typedef  __int64_t  int64;
typedef  __uint64_t  uint64; // unsigned int64
#endif 

// system calls
int unlink();
int system();

#undef isspace
#define isspace(x) ( x==' ' || x=='\t' || (x > 9 && x < 14 ))

#undef isnum
#define isnum(x) ( x>='0' && x<='9')

// Short scalar types.Diavolo reports AIX compiler has bugs with short types.
#if     !defined(FALSE)
#define FALSE    0
#endif

#if     !defined(TRUE)
#define TRUE     1
#endif

#if     defined(_AIX)
#if     !defined(const)
#define const
#endif
// typedef int sh_int;
typedef int bool;
#define unix
#else
typedef unsigned char   bool;
#endif


// Structure types.
typedef struct  gq_data          GQ_DATA;
typedef struct  affect_data      AFFECT_DATA;
typedef struct  vote_data        VOTE_DATA;
typedef struct  offer_data       OFFER_DATA;
typedef struct  vote_char        VOTE_CHAR;
typedef struct  penalty_data     PENALTY_DATA;
typedef struct  area_data        AREA_DATA;
typedef struct  ban_data         BAN_DATA;
typedef struct  buf_type         BUFFER;
typedef struct  char_data        CHAR_DATA;
typedef struct  descriptor_data  DESCRIPTOR_DATA;
typedef struct  exit_data        EXIT_DATA;
typedef struct  extra_descr_data EXTRA_DESCR_DATA;
typedef struct  help_data        HELP_DATA;
typedef struct  kill_data        KILL_DATA;
typedef struct  mem_data         MEM_DATA;
typedef struct  mob_index_data   MOB_INDEX_DATA;
typedef struct  note_data        NOTE_DATA;
typedef struct  news_data        NEWS_DATA;
typedef struct  obj_data         OBJ_DATA;
typedef struct  obj_index_data   OBJ_INDEX_DATA;
typedef struct  victim_data      VICTIM_DATA;
typedef struct  pc_data          PC_DATA;
typedef struct  gen_data         GEN_DATA;
typedef struct  reset_data       RESET_DATA;
typedef struct  room_index_data  ROOM_INDEX_DATA;
typedef struct  shop_data        SHOP_DATA;
typedef struct  time_info_data   TIME_INFO_DATA;
typedef struct  weather_data     WEATHER_DATA;
typedef struct  mprog_list       MPROG_LIST;
typedef struct  mprog_code       MPROG_CODE;
typedef struct  clan_data        CLAN_DATA;
typedef struct  tournament_data  TOURNAMENT_DATA;
typedef struct  bet_data         BET_DATA;
typedef struct  raffect          RAFFECT;
typedef struct  gquest_data      GQUEST_DATA;
typedef struct  auction_data     AUCTION_DATA;

// Our function prototypes. this is every function in Merc
//DDD #define CD  CHAR_DATA
typedef struct char_data CD;
#define MID MOB_INDEX_DATA
#define OD  OBJ_DATA
#define OID OBJ_INDEX_DATA
#define RID ROOM_INDEX_DATA
#define SF  SPEC_FUN
#define AD  AFFECT_DATA
#define DSD DESCRIPTOR_DATA

// Function types.
typedef void DO_FUN    args( ( CD *ch, const char *argument ) );
typedef bool SF        args( ( CD *ch ) );
typedef void SPELL_FUN args( ( int sn, int level, CD *ch, void *vo, int target ));

// including definitions and structure types
#include "merc_def.h" // All defines
#include "merc_str.h" // All structures

// These are skill_lookup return values for common skills and spells
extern int gsn_blink;
extern int gsn_backstab;
extern int gsn_dodge;
extern int gsn_envenom;
extern int gsn_hide;
extern int gsn_peek;
extern int gsn_pick_lock;
extern int gsn_sneak;
extern int gsn_steal;
extern int gsn_cleave;
extern int gsn_disarm;
extern int gsn_enhanced_damage;
extern int gsn_kick;
extern int gsn_parry;
extern int gsn_dual;
extern int gsn_dual_sec_attack;
extern int gsn_rescue;
extern int gsn_second_attack;
extern int gsn_third_attack;
extern int gsn_blindness;
extern int gsn_charm_person;
extern int gsn_curse;
extern int gsn_invis;
extern int gsn_mass_invis;
extern int gsn_plague;
extern int gsn_poison;
extern int gsn_sleep;
extern int gsn_gaseous_form;
extern int gsn_quenia;
extern int gsn_shock_hit;
extern int gsn_strangle;

extern int gsn_axe;
extern int gsn_dagger;
extern int gsn_flail;
extern int gsn_mace;
extern int gsn_polearm;
extern int gsn_shield_block;
extern int gsn_spear;
extern int gsn_staff;
extern int gsn_sword;
extern int gsn_whip;
extern int gsn_missile;
 
extern int gsn_bash;
extern int gsn_crush;
extern int gsn_berserk;
extern int gsn_dirt;
extern int gsn_hand_to_hand;
extern int gsn_trip;
extern int gsn_frame;
 
extern int gsn_fast_healing;
extern int gsn_haggle;
extern int gsn_lore;
extern int gsn_meditation;
 
extern int gsn_scrolls;
extern int gsn_staves;
extern int gsn_wands;
extern int gsn_recall;
extern int gsn_pray;
extern int gsn_lash;
extern int gsn_charge;

// Global constants.
extern const  struct  str_app_type    str_app         [37];
extern const  struct  int_app_type    int_app         [37];
extern const  struct  wis_app_type    wis_app         [37];
extern const  struct  dex_app_type    dex_app         [37];
extern const  struct  con_app_type    con_app         [37];

extern        struct  cmd_type        cmd_table       [];
extern const  struct  class_type      class_table     [MAX_CLASS];
extern        struct  quenia_words    quenia_table[];
extern const  struct  class_name_type class_ntab [16];
extern const  struct  class_name_type class_nntab [16];
extern const  struct  picture_type    where_name_male   [];
extern const  struct  picture_type    where_name_female [];
extern const  struct  weapon_type     weapon_table      [];
extern const  struct  weapon_type     weapon_army_table       [];
extern const  struct  item_type       item_table      [];
extern const  struct  wiznet_type     wiznet_table    [];
extern const  struct  attack_type     attack_table    [];

struct dam_msg_type
{
  int  from; // damage min
  int  to;   // damage max
  const char *vs; // message other damage
  const char *vp; // message self damage
};

extern const struct dam_msg_type dam_msg_table[];

extern        struct  race_type       race_table      [];
extern        struct  deity_choice    deity_table     [MAX_DEITIES];
extern        struct  guild_type      guild_table     [];
extern const  struct  spec_type       spec_table      [];
extern const  struct  liq_type        liq_table       [];
extern const  struct  fix_liquid      fix_liq_table   [];
extern const  struct  item_cond_type  item_cond_table [];
extern const  struct  item_durability_status item_durability_table[];
extern const  struct  favour_output   t_favour[];
extern const  struct  deity_apply_type deity_apply_table[];
extern        struct  skill_type      skill_table     [];
extern const  struct  group_type      group_table     [MAX_GROUP];
extern        struct  social_type     *social_table;
extern char * const   title_table     [MAX_CLASS] [MAX_LEVEL+1] [2];
extern const  char *  bonus_dam_types [DAM_MAX+1];
extern const  char *  weapon_bonus_types[WEAPON_MAX+1];
extern const  int     pk_range_table  [4][4];
extern const  unsigned char char_table[256];
extern char           *GreyEq         [MAX_WEAR_L+1];
extern char           *GreyRank       []; 

extern void change_favour args( (CHAR_DATA *ch, int cvalue) );

// Global variables.
extern HELP_DATA  * help_first;
extern SHOP_DATA  * shop_first;

extern CD              * char_list;
extern DSD             * descriptor_list;
extern OD              * object_list;
extern CLAN_DATA       * clan_list;
extern VOTE_DATA       * vote_list;
extern OFFER_DATA      * offer_list;
extern RAFFECT         * raffect_list;
extern NEWS_DATA       * newspaper_list;
extern TOURNAMENT_DATA * tournament;
extern GQUEST_DATA       gquest;

extern char            bug_buf[];
extern time_t          current_time;
extern bool            fLogAll;
extern FILE          * fpReserve;
extern FILE          * logReserve;
extern KILL_DATA       kill_table[];
extern char            log_buf[];
extern TIME_INFO_DATA  time_info;
extern WEATHER_DATA    weather_info;
extern MPROG_CODE    * mprog_list;
extern int64           top_mprog_index;
extern bool            MOBtrigger;

// New Globals Variables
extern int cnt_char;
extern int cnt_pcdata;
extern int cnt_descr;
extern int cnt_obj;
extern int cnt_aff;
extern int cnt_raff;
extern int cnt_buff;

int64          global_cfg;  // Configuration
int            max_skill;  // was max_skill definition. Now variable
char         * assasin_targets;
int            rebootcount;       // Time-counter to reboot
int            PULSE_PER_SECOND;
int            PULSE_UPDCHAR;
int            PULSE_VIOLENCE;
int            PULSE_MOBILE;
int            PULSE_MUSIC;
int            PULSE_TICK;
int            PULSE_AREA;
const char   * bdmsg[10];
AUCTION_DATA * auction;
char         * ID_FILE;
int            autologin_code;
int            autologin_pass;

// OS-dependent declarations.
// These are all very standard library functions,
// but some systems have incomplete or non-ansi header files.
#if     defined(linux)
char *  crypt          args( ( const char *key, const char *salt ) );
#endif

#if     defined(WIN32)
#define NOCRYPT
#if     defined(unix)
#undef  unix
#endif
#endif

#if     defined(sequent)
char *  crypt          args( ( const char *key, const char *salt ) );
int     fclose         args( ( FILE *stream ) );
int     fprintf        args( ( FILE *stream, const char *format, ... ) );
int     fread          args( ( void *ptr, int size, int n, FILE *stream ) );
int     fseek          args( ( FILE *stream, long offset, int ptrname ) );
void    perror         args( ( const char *s ) );
int     ungetc         args( ( int c, FILE *stream ) );
#endif

// The crypt(3) function is not available on some operating systems.
// In particular, the U.S. Government prohibits its export from the
//   United States to foreign countries.
// Turn on NOCRYPT to keep passwords in plain text.
#if     defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#endif

// interp.c
bool  is_set        args( ( int64 f1, int64 f2) );
void  set_bit       args( ( int64 flag, int64 bit) );
void  rem_bit       args( ( int64 flag, int64 bit) );
bool check_social( CHAR_DATA *ch, char *command, const char *argument );

// act_comm.c
void  wear_obj      args( ( CD *ch, OD *obj, bool fReplace, bool w_left) );
void  check_sex     args( ( CD *ch) );
void  add_follower  args( ( CD *ch, CD *master ) );
void  stop_follower args( ( CD *ch ) );
void  nuke_pets     args( ( CD *ch ) );
void  die_follower  args( ( CD *ch ) );
bool  is_same_group args( ( CD *ach, CD *bch ) );
int   toggle_int        ( int flag,int bit);
int64 toggle_int64      ( int64 flag,int64 bit);
void  info              (CD * ch, int level, int mes, const char *name, const char *fraze);
void  talk_auction      (char *argument);
bool  check_parse_name  (const char * name, bool new);

// act_info.c
void set_title  args( ( CD *ch, const char *title ) );
int  getnumname       ( CD *ch );
void whois_info( CHAR_DATA* ch, CHAR_DATA *victim);
void mstat_info( CHAR_DATA *ch, CHAR_DATA *victim);

// act_move.c
bool do_move_char         ( CD *ch, int door, bool follow, bool run); //(C) Saboteur
RID *get_random_room args ((CD *ch) );

// act_obj.c
bool can_loot      args( (CD *ch, OD *obj) );
int  local_get_obj args( (CD *ch, OD *obj, OD *container, bool IsOne) );
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace );

// act_wiz.c
void wiznet  args( (char *string, CD *ch, OD *obj,int64 flag,int min_level ) );
int  tipsy   args( (CD *ch, const char *argument) );          // tipsy by Dinger
void tipsy2  args( (CD *ch, const char *arg1, char *arg2) );  // tipsy by Dinger
void stf         ( const char *str, CD *ch); // send_to_file - fprintf to ID_FILE
void gecho       (const char *argument ); // system gecho

// alias.c
void  substitute_alias args( (DSD *d, const char *input) );

// clan.c
bool clan_cfg   (CLAN_DATA *clan, int64 flag);
void do_ear     (CHAR_DATA *victim,CHAR_DATA *ch);
bool add_clanskill(CLAN_DATA *clan, int sn, int64 time);
bool rem_clanskill(CLAN_DATA *clan,int sn);

// comm.c
bool  check_ban        args( ( const char *site, int type) );
void  show_string      args( ( struct descriptor_data *d, char *input) );
void  close_socket     args( ( DSD *dclose ) );
void  write_to_buffer  args( ( DSD *d, const char *txt, int length ) );
void  stc              args( ( const char *txt, CD *ch ) );
void  page_to_char     args( ( const char *txt, CD *ch ) );
void  act              args( ( const char *format, CD *ch,
                               const void *arg1, const void *arg2, int type ) );
char  * act_parse_name args( ( char *buf, CD *ch, CD *vch, char needcase));
char  * act_parse_obj  args( ( char *buf, CD *ch, OD *obj, char needcase ));
const char * act_ending args(( char *buf, int sex, const char *str));
void  gram_newformat         ( char *buf, const char *string, char gram_case );

void  act_new  args( ( const char *format, CD *ch, const void *arg1,
                       const void *arg2, int type, int min_pos) );
void  do_act   args( ( const char *format, CD *ch, const void *arg1,
                       const void *arg2, int type, int min_pos, int spam ) );
void  bust_a_prompt args( ( CD *ch ) ); // moved from comm.c by Dinger

// Colour stuff by Lope of Loping Through The MUD
int   colour          args( ( char type, CD *ch, char *string ) );
void  colourconv      args( ( char *buffer, const char *txt, CD *ch ) );
void  send_to_char_bw args( ( const char *txt, CD *ch ) );
void  page_to_char_bw args( ( const char *txt, CD *ch ) );

// db.c
void  smash_flash   ( char *str );
void  smash_beep    ( char *str, bool total );
void  smash_percent ( char *str );
void  smash_tilde   ( char *str );
void  smash_newline ( char *str );
long  flag_convert  ( char letter );
char * pflag64      ( int64 flag );
char * print_flags     args( ( int flag ));
char * pflag64         args( ( int64 flag));     // Sab: 64bit Flag output
void   boot_db         ();
void   area_update     ();
CD *   create_mobile   args( ( MID *pMobIndex ) );
void   clone_mobile    args( ( CD *parent, CD *clone) );
OD *   create_object   args( ( OID *pObjIndex, int level ) );
void   clone_object    args( ( OD *parent, OD *clone ) );
void   clear_char      args( ( CD *ch ) );
MID *  get_mob_index   args( ( int64 vnum ) );
OID *  get_obj_index   args( ( int64 vnum ) );
RID *  get_room_index  args( ( int64 vnum ) );
char   fread_letter    args( ( FILE *fp ) );
int    fread_number    args( ( FILE *fp ) );
int64  fread_number64  args( ( FILE *fp ) );
long   fread_flag      args( ( FILE *fp ) );
void   fread_to_eol    args( ( FILE *fp ) );
int64  flag_convert64  args( ( char letter) );
void * alloc_mem       args( ( int sMem ) );
void * alloc_perm      args( ( int sMem ) );
void   free_mem        args( ( void *pMem, int sMem ) );
void   load_deities(void);

// Sure, it have to be `const' (; (unicorn)
const   char * str_dup  args( ( const char *str ) ); 
void    free_string     args( ( const char *pstr ) );
int     number_fuzzy    args( ( int number ) );
int     number_range    args( ( int from, int to ) );
int     number_percent  args( ( void ) );
int     number_door     args( ( void ) );
int     number_bits     args( ( int width ) );
long    number_mm       args( ( void ) );
int     dice            args( ( int number, int size ) );
int     interpolate     args( ( int level, int value_00, int value_32 ) );
void    load_races      ();
void    smash_tilde     args( ( char *str ) );
void    smash_dollar    args( ( char *str ) );
bool    str_cmp         args( ( const char *astr, const char *bstr ) );
bool    str_prefix      args( ( const char *astr, const char *bstr ) );
bool    str_infix       args( ( const char *astr, const char *bstr ) );
bool    str_suffix      args( ( const char *astr, const char *bstr ) );
char *  capitalize      args( ( const char *str ) );
void    append_file     args( ( CD *ch, char *file, char *str ) );
void    bug             args( ( const char *str, int64 param ) );
void    log_string      args( ( const char *str ) );
void    tail_chain      args( ( void ) );
const char * fread_word args( ( FILE *fp ) );
OD *create_random_item      ( int lvl );
OD *create_random_armor     ( int lvl );

const char * get_extra_descr args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MPROG_CODE * get_mprog_index args( ( int vnum ) );
const char * fread_string    args( ( FILE *fp ) );
const char * fread_string_eol args(( FILE *fp ) );


// astellar.c
void damage_obj       args( ( CHAR_DATA *victim, OBJ_DATA *obj, int dam_v, bool inf_char ) );
int  deity_char_power ( CHAR_DATA *ch, int type, int subtype);
int  favour_string    ( CHAR_DATA *ch);
void dec_worship      ( CHAR_DATA *ch);


// io.c
int64 fread_flag64   args( ( FILE *fp ) );
char *number_string      ( int64 number);
char *do_printf          ( char *buf, const char *fmt,...);//(C)Saboteur printf
void  do_fprintf         ( FILE *fp, const char *fmt,...); //(C)Sab printf>file
void  log_printf         ( const char *fmt,...); //(C)Sab printf to logfile
void  dlog               (const char *fmt,...);  //(C)Sab log_printf w/debuglog
void  add_stealer        (CHAR_DATA *ch, CHAR_DATA *stealer);
void  add_pkiller        (CHAR_DATA *ch, CHAR_DATA *killer);
void  remove_pkiller     (CHAR_DATA *ch, char *name);
void  remove_one_stealer (CHAR_DATA *ch);



// saboteur.c
bool  is_offered          ( CD *ch );
char  *class_remort_names ( CD *ch );
char  *classname          ( CD *ch );
char  *get_align          ( CD *ch );
char  *get_clan_rank      ( CD *ch );
char  *get_logchar        ( int flag );
char  *race_wname         ( CD *ch );
char  *spell_group_name   ( int64 group );
const char *add_word      ( const char *argument, const char *word );
const char *create_word   ();
const char *remove_word   ( const char *argument, const char *word );
int   abs                 ( int a );
int   category_bonus      ( CD *ch,int category );
int   get_full_class      ( CD *ch );
int   get_loglevel        ( int flag );
int   get_material_modifier(CD *ch,OD *obj);
int   get_skill_bonus     ( CD *ch,int sn );
int64 sell_gem            ( CD *ch, int64 amount, int64 vnum );
int64 toggle_flag         (CHAR_DATA *ch,int64 flag,int64 bit, char *text, bool invert);
void  assasin_echo        ( const char *argument );
void  astat               ( CD *ch, const char *arg );
void  bust_arg            ( CD *ch, const char *argument );
void  run                 ( CD *ch );
void  send_news           ( char *text, int type );
void  send_note           ( const char *from, const char *to,const char *subject, const char *text,int type);
void  write_skill_table   ( CHAR_DATA *ch);
void remove_penalty(CHAR_DATA *ch, PENALTY_DATA *penalty);

// guild.c
int  guild_lookup    (const char *name);
void load_one_guild  (FILE *fp);
void save_guild      (FILE *fp);

// effect.c
void acid_effect   args( (void *vo, int level, int dam, int target) );
void cold_effect   args( (void *vo, int level, int dam, int target) );
void fire_effect   args( (void *vo, int level, int dam, int target) );
void poison_effect args( (void *vo, int level, int dam, int target) );
void shock_effect  args( (void *vo, int level, int dam, int target) );

// fight.c
bool PK_RANGE            (CD *ch, CD *victim);
bool is_safe         args( (CD *ch, CD *victim ) );
void cant_mes        args( (CD *c ) );
bool is_safe_spell   args( (CD *ch, CD *victim, bool area ) );
void violence_update ();
void multi_hit       args( ( CD *ch, CD *victim) );
bool damage          args( ( CD *ch, CD *victim, int dam, int dt, int class, bool show, bool breath, OD *obj ) );
bool damage_old      args( ( CD *ch, CD *victim, int dam,int dt, int class, bool show ) );
void update_pos      args( ( CD *victim ) );
void stop_fighting   args( ( CD *ch, bool fBoth ) );
void check_criminal  args( ( CD *ch, CD *victim, int level) );
bool can_attack      args( ( CD *ch, int type) );
void raw_kill        args( ( CHAR_DATA *victim ) ); 
bool check_skill     args( ( CHAR_DATA *ch, int gsn_skill) ); 

// handler.c
RAFFECT *get_raffect        ( ROOM_INDEX_DATA *room, int bit);
int     raffect_level       ( ROOM_INDEX_DATA *room, int ra_bit);
AD      *affect_find    args( (AD *paf, int sn));
void    affect_check    args( (CD *ch, int where, int64 vector) );
int     count_users     args( (OD *obj) );
void    deduct_cost     args( (CD *ch, int64 cost) );
void    affect_enchant  args( (OD *obj) );
int     check_immune    args( (CD *ch, int dt) );
int64   flag_lookup         ( const char *name, const struct flag_type *flag_table);
int     position_lookup args( ( const char *name) );
int     sex_lookup      args( ( const char *name) );
int     size_lookup     args( ( const char *name) );
int     liq_lookup      args( ( const char *name) );
char   *material_lookup args( ( const char *name) );
char   *materialrus_lookup args( ( const char *name) );
int     material_num          ( const char *name);
int     materialrus_num       ( const char *name);
int     item_cond_num         ( OBJ_DATA *obj );
const char *get_obj_cond      ( OBJ_DATA *obj, int rtype );
int     weapon_lookup   args( ( const char *name) );
int     weapon_type     args( ( const char *name) );
CLAN_DATA *clan_lookup  args( ( const char *name) );
const char *weapon_name args( ( int weapon_Type) );
const char *item_name   args( ( int item_type) ); 
int     item_lookup     args( ( const char *name) );
int     attack_lookup   args( ( const char *name) );
int     race_lookup     args( ( const char *name) );
int     deity_lookup    args( ( const char *name) );
int64   wiznet_lookup   args( ( const char *name) );
int     class_lookup    args( ( const char *name) );
bool    is_same_clan    args( ( CD *ch, CD *victim));
int     get_skill       args( ( CD *ch, int sn ) );
int     get_weapon_sn   args( ( CD *ch, bool rhand ) );
int     get_weapon_skill args(( CD *ch, int sn ) );
int     get_age         args( ( CD *ch ) );
void    reset_char      args( ( CD *ch )  );
int     get_trust       args( ( CD *ch ) );
int     get_curr_stat   args( ( CD *ch, int stat ) );
int     get_max_train   args( ( CD *ch, int stat ) );
int     can_carry_n     args( ( CD *ch ) );
int     can_carry_w     args( ( CD *ch ) );
bool    is_name         args( ( const char *str, const char *namelist ) );
bool    is_exact_name   args( ( const char *str, const char *namelist ) );
void    affect_to_char  args( ( CD *ch, AD *paf ) );
void    affect_to_obj   args( ( OD *obj, AD *paf ) );
void    affect_remove   args( ( CD *ch, AD *paf ) );
void    affect_remove_obj args( (OD *obj, AD *paf ) );
void    affect_strip    args( ( CD *ch, int sn ) );
bool    is_affected     args( ( CD *ch, int sn ) );
void    affect_join     args( ( CD *ch, AD *paf ) );
void    char_from_room  args( ( CD *ch ) );
void    char_to_room    args( ( CD *ch, RID *pRoomIndex ) );
void    obj_to_char     args( ( OD *obj, CD *ch ) );
void    obj_from_char   args( ( OD *obj ) );
void    raffect_to_room args( (RAFFECT *ra,RID *room) );
void    raffect_from_room args((RAFFECT *ra));
int     apply_ac        args( ( OD *obj, int iWear, int type ) );
OD *    get_eq_char     args( ( CD *ch, int iWear ) );
void    equip_char      args( ( CD *ch, OD *obj, int iWear ) );
void    unequip_char    args( ( CD *ch, OD *obj ) );
int     count_obj_list  args( ( OID *obj, OD *list ) );
void    obj_from_room   args( ( OD *obj ) );
void    obj_to_room     args( ( OD *obj, RID *pRoomIndex ) );
void    obj_to_obj      args( ( OD *obj, OD *obj_to ) );
void    obj_from_obj    args( ( OD *obj ) );
void    extract_obj     args( ( OD *obj ) );
void    extract_char    args( ( CD *ch, bool fPull ) );
CD *    get_char_room   args( ( CD *ch, const char *argument ) );
CD *    get_char_world  args( ( CD *ch, const char *argument ) );
CD *    get_pchar_world args( ( CD *ch, const char *argument ) );
CD *    get_online_char args( ( CD *ch, const char *argument ) );
CD *    get_char_area   args( ( CD *ch, const char *argument ) );
OD *    get_obj_type    args( ( OID *pObjIndexData ) );
OD *    get_obj_list    args( ( CD *ch, const char *argument,OD *list ) );
OD *    get_obj_carry   args( ( CD *ch, const char *argument,CD *viewer ) );
OD *    get_obj_wear    args( ( CD *ch, const char *argument ) );
OD *    get_obj_here    args( ( CD *ch, const char *argument ) );
OD *    get_obj_world   args( ( CD *ch, const char *argument ) );
OD *    get_obj_victim  args( ( CD *ch, CD *victim, const char *argument ) );
OD *    create_money    args( ( int64 gold, int64 silver ) );
int     get_obj_number  args( ( OD *obj ) );
int     get_obj_weight  args( ( OD *obj ) );
int     get_true_weight args( ( OD *obj ) );
bool    room_is_dark    args( ( RID *pRoomIndex ) );
bool    is_room_owner   args( ( CD *ch, RID *room) );
bool    room_is_private args( ( RID *pRoomIndex ) );
bool    can_see         args( ( CD *ch, CD *victim, int check_level ) );
bool    can_see_obj     args( ( CD *ch, OD *obj ) );
bool    can_see_sneak   args( (CD *ch, CD *victim, int check_level));
bool    can_see_room    args( ( CD *ch, RID *pRoomIndex) );
bool    can_drop_obj    args( ( CD *ch, OD *obj ) );
char *  affect_loc_name args( ( int location ) );
char *  spec_bit_name   args( ( int64 vector ) );
char *  skill_flag_name args( ( int64 vector ) );
char *  affect_bit_name args( ( int64 vector ) );
char *  extra_bit_name  args( ( int64 extra_flags ) );
char *  wear_bit_name   args( ( int64 wear_flags ) );
char *  act_bit_name    args( ( int64 act_flags ) );
char *  off_bit_name    args( ( int64 off_flags ) );
char *  imm_bit_name    args( ( int64 imm_flags ) );
char *  form_bit_name   args( ( int64 form_flags ) );
char *  part_bit_name   args( ( int64 part_flags ) );
char *  weapon_bit_name args( ( int64 weapon_flags ) );
char *  comm_bit_name   args( ( int64 comm_flags ) );
char *  cont_bit_name   args( ( int64 cont_flags) );
int     ch_skill_nok    args( ( CD *ch , int skill ) );
char *  raff_name       args( ( int64 vector ) );
char    num_char        args( ( int n ));
char    num_char64      args( ( int64 n ));
int64   atoi64          args( (const char *arg)); //Jasana
int     str_len         args( (const char *arg));
int     ch_skill_nok_nomessage  args( ( CD *ch , int skill ) );
const char *  get_char_desc     args( ( CD *ch, char needcase ));
const char *  get_mobindex_desc args( ( MID *pIndex, char needcase ));
const char *  get_obj_desc      args( ( OD *obj, char needcase ));
const char *  get_rdeity        args( ( const char *rdeity, char needcase ));
const char *  get_objindex_desc args( ( OID *pIndex, char needcase ));
const char *  str_cut           args( (char *arg,int len)); 
bool  check_clanspell(int sn, CLAN_DATA *clan);

// interp.c
void    interpret       args( ( CD *ch, const char *argument ));
bool    is_number       args( ( const char *arg ) );
int     number_argument args( ( char *argument, char *arg ) );
int     mult_argument   args( ( char *argument, char *arg) );
const char *one_argument args(( const char *argument, char *arg_first ) );

// magic.c
int     find_spell      args( ( CD *ch, const char *name) );
int     mana_cost       (CD *ch, int sn);
int     skill_lookup    args( ( const char *name ) );
int     calc_saves      args( (CHAR_DATA *victim ) );
bool    saves_spell     args( ( int level, CD *victim, int dam_type ) );
void    obj_cast_spell  args( ( int sn, int level, CD *ch, CD *victim, OD *obj ) );
RAFFECT *new_raffect();
void    free_raffect          (RAFFECT *raffect);
int     get_int_modifier args(( CD *ch, CD *victim, int sn, int dt ) );
bool check_dispel( int dis_level, CHAR_DATA *victim, int sn) ;

// note.c
void unread_update(void);

// quest.c
int  cancel_quest(CD *ch, bool reward, int from,int to);
bool is_gqmob    (int64 vnum);

// save.c
void save_char_obj   args( ( CD *ch ) );
bool load_char_obj   args( ( DSD *d, const char *name, int status ) );
void save_one_char       (CD *ch, int action) ;
void save_config   ();
void save_races    ();
void save_newspaper();
void save_vote     ();
void save_clans    ();

// skills.c
bool parse_gen_groups args(( CD *ch, const char *argument ) );
void list_group_costs args(( CD *ch ) );
void list_group_known args(( CD *ch ) );
int  exp_per_level    args(( CD *ch, int points ) );
void check_improve    args(( CD *ch, int sn, bool success,int multiplier ) );
int  group_lookup     args((const char *name) );
void gn_add           args(( CD *ch, int gn) );
void gn_remove        args(( CD *ch, int gn) );
void group_add        args(( CD *ch, const char *name, bool deduct) );
void group_remove     args(( CD *ch, const char *name) );
int  min_level            (CHAR_DATA *ch,int sn);
int  group_cost           (CHAR_DATA *ch,int gn);

// special.c
SF   * spec_lookup  args( ( const char *name ) );
char * spec_name    args( ( SF *function ) );

// teleport.c
RID  * room_by_name args( ( char *target, int level, bool error) );

// mob_prog.c
void program_flow   args( ( int64 vnum, const char *source, CD *mob, CD *ch,
                               const void *arg1, const void *arg2 ) );
void mp_act_trigger args( ( char *argument, CD *mob, CD *ch,
                               const void *arg1, const void *arg2, int type ) );

bool mp_percent_trigger args( ( CD *mob, CD *ch,                               
                               const void *arg1, const void *arg2, int type ) );
void mp_bribe_trigger   args( ( CD *mob, CD *ch, int64 amount ) );
bool mp_exit_trigger    args( ( CD *ch, int dir ) );
void mp_give_trigger    args( ( CD *mob, CD *ch, OD *obj ) );
void mp_greet_trigger   args( ( CD *ch ) );
void mp_hprct_trigger   args( ( CD *mob, CD *ch ) );

// mob_cmds.c
void  mob_interpret  args( ( CD *ch, const char *argument ) );

// update.c
void  advance_level  args( ( CD *ch, bool hide ) );
void  gain_exp       args( ( CD *ch, int gain ) );
void  gain_condition args( ( CD *ch, int iCond, int64 value ) );
void  update_handler ();
void  statue_moving (CHAR_DATA *ch);
void  penalty_update(CHAR_DATA *ch);
void  auction_update();
void  mobile_update ();
void  song_update ();
void  char_update ();
void  obj_update ();
void  quest_update ();
void  clan_update ();
void  area_update ();
void  gquest_update ();
void  weather_update();
void raffect_update();

// Global Constants
extern  char *  const   dir_name  [];
extern  char *  const   dir_name2 [];
extern  const   int     rev_dir   [];          /* int - ROM OLC */
extern  const   struct  spec_type spec_table [];

// Global variables
extern AREA_DATA *       area_first;
extern AREA_DATA *       area_last;
extern SHOP_DATA *       shop_last;

extern int    top_affect;
extern int    top_area;
extern int    top_ed;
extern int    top_exit;
extern int    top_help;
extern int64  top_mob_index;
extern int64  top_obj_index;
extern int    top_reset;
extern int64  top_room;
extern int    top_shop;

extern int64  top_vnum_mob;
extern int64  top_vnum_obj;
extern int64  top_vnum_room;

extern char   str_empty[1];

extern MID * mob_index_hash  [MAX_KEY_HASH];
extern OID * obj_index_hash  [MAX_KEY_HASH];
extern RID * room_index_hash [MAX_KEY_HASH];

// db.c
void reset_area args( ( AREA_DATA * pArea ) );
void reset_room args( ( RID *pRoom ) );

// string.c
const char * string_remove (const char *, int) ;
const char * string_insert (const char *, const char *, int) ;
const char * string_replace(const char *, const char *, int) ;
void         string_edit   (CD *, const char **) ;
void         string_append (CD *, const char **) ;
void         string_add    (CD *, const char * ) ;
void         string_show   (CD *, const char *)  ;
const char * string_format (const char *) ;

// olc.c
bool  run_olc_editor args( ( DSD *d ) );
char  *olc_ed_name   args( ( CD *ch ) );
char  *olc_ed_vnum   args( ( CD *ch ) );

// olc_mpcode.c
char *mprog_type_to_name( int64 type );

// bit.c
extern const struct flag_type area_flags[];
extern const struct flag_type sex_flags[];
extern const struct flag_type exit_flags[];
extern const struct flag_type door_resets[];
extern const struct flag_type room_flags[];
extern const struct flag_type sector_flags[];
extern const struct flag_type type_flags[];
extern const struct flag_type extra_flags[];
extern const struct flag_type wear_flags[];
extern const struct flag_type act_flags[];
extern const struct flag_type affect_flags[];
extern const struct flag_type apply_flags[];
extern const struct flag_type wear_loc_strings[];
extern const struct flag_type wear_loc_flags[];
extern const struct flag_type weapon_flags[];
extern const struct flag_type container_flags[];
extern const struct flag_type penalty_flags[];
char *flag_string( const struct flag_type *flag_table, int64 bits );

// social edit
void load_social_table();
void save_social_table();
char *soc_group_name(int64 group);

// ROM OLC
extern const struct flag_type material_type[];
extern const struct flag_type form_flags[];
extern const struct flag_type part_flags[];
extern const struct flag_type ac_type[];
extern const struct flag_type size_flags[];
extern const struct flag_type off_flags[];
extern const struct flag_type imm_flags[];
extern const struct flag_type res_flags[];
extern const struct flag_type vuln_flags[];
extern const struct flag_type position_flags[];
extern const struct flag_type weapon_class[];
extern const struct flag_type weapon_type2[];
extern const struct flag_type furniture_flags[];
extern const struct flag_type mprog_flags[];
void printf_to_char args( ( CD *ch, char *fmt, ...) );

#undef CD
#undef MID
#undef OD
#undef OID
#undef RID
#undef SF
#undef AD
#undef DSD

#endif

