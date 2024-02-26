// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#ifndef __merc_str
#define __merc_str

#include "merc.h"

struct guild_type
{
  int   guild;
  const char *name;
  int64 accounter;
  int64 gold;
};

struct  ban_data
{
  BAN_DATA *  next;
  const char *      host;
  int         ban_flags;
};

struct  cmd_type
{
  const char * name;
  const char * const rname;
  const char * const uname;
  DO_FUN *     do_fun;
  int          position;
  int          level;
  int          log;
  int64        flag;
};

struct  gq_data
{
  int status;
  int target[20];
};

struct gquest_data
{
  int status;
  int min_level;
  int max_level;
  int counter;
  int tmp_counter;
  int mobs;
  int64 target[20];
  int target_counter[20];
  int64 room[100];
};

struct quenia_words
{
  int64  name;
  char   word[20];
  char   descr[20];
  char   long_name[20];
  int    const start_counter;
  int    counter;
};

// mobprograms olc
struct keywords_type
{
    char *name;
    char *help;
};

// mobprograms
struct mob_cmd_type
{
  char   *name;
  DO_FUN *do_fun;
  char   *help;
};

extern struct mob_cmd_type mob_cmd_table[];

struct flag_type
{
  const char *name;
  int64 bit;
  bool settable;
};

struct buf_type
{
  BUFFER * next;
  bool     valid;
  int      state;  // error state of the buffer      
  int      size;   // size in k     
  char *   string; // buffer's string     
};

struct mprog_list
{
   int64       trig_type;
   const char *trig_phrase;
   int         vnum;
   const char *code;
   MPROG_LIST *next;
   bool        valid;
};

struct  time_info_data
{
  int hour;
  int day;
  int month;
  int year;
};

struct  weather_data
{
  int mmhg;
  int change;
  int sky;
  int sunlight;
};

// Descriptor (channel) structure.
struct  descriptor_data
{
  DESCRIPTOR_DATA * next;
  DESCRIPTOR_DATA * snoop_by;
  CHAR_DATA *       character;
  bool              valid;
  const char *      host;
  int               descriptor;
  int               connected;
  bool              fcommand;
  char              inbuf    [4 * MAX_STRING_LENGTH];
  char              incomm   [MAX_STRING_LENGTH];
  char              inlast   [MAX_STRING_LENGTH];
  int               repeat;
  int               codepage;
  char *            outbuf;
  int               outsize;
  int               outtop;
  char *            showstr_head;
  char *            showstr_point;
  void *            pEdit;
  const char **     pString;
  int               editor;
};

// Attribute bonus structures.
struct  str_app_type
{
  int tohit;
  int todam;
  int carry;
  int wield;
};

struct  int_app_type
{
  int learn;
};

struct  wis_app_type
{
  int practice;
};

struct  dex_app_type
{
  int defensive;
};

struct  con_app_type
{
  int hitp;
  int shock;
};

// Help table types.
struct  help_data
{
  HELP_DATA * next;
  int      level;
  const char *      keyword;
  const char *      text;
};

struct  shop_data
{
  SHOP_DATA * next;                 // Next shop in list              
  int64       keeper;               // Vnum of shop keeper mob
  int         buy_type [MAX_TRADE]; // Item types shop will buy       
  int         profit_buy;           // Cost multiplier for buying     
  int         profit_sell;          // Cost multiplier for selling
  int         open_hour;            // First opening hour           
  int         close_hour;           // First closing hour           
};

struct  class_type
{
  const char *  name;          // the full name of the class
  char    who_name[4];   // Three-letter name for 'who'
  int     attr_prime;    // Prime attribute
  int     weapon;        // First weapon
  int     skill_adept;   // Maximum skill level
  int     hp_min;        // Min hp gained on leveling
  int     hp_max;        // Max hp gained on leveling
  bool    fMana;         // Class gains mana on level
  const char *  base_group;    // base skills gained
  const char *  default_group; // default skills gained
};

struct name_type
{
  char * name;
  char * name_who;
  bool   classmag;
  bool   classcle;
  bool   classthi;
  bool   classwar;
};

struct class_name_type
{
  char * name;
  bool   mag;
  bool   cle;
  bool   thi;
  bool   war;
};

struct item_type
{
  int    type;
  char * name;
};

struct weapon_type
{
  char * name;
  int64  vnum;
  int    type;
  int    *gsn;
};

struct wiznet_type
{
  char *  name;
  int64   flag;
  int     level;
  char *  logchar;
};

struct attack_type
{
  char * name;   // name
  char * noun;   // message
  int    damage; // damage class
};

struct race_type
{
  const char * name;              // MUST be in race_type
  const char * who_name;          // Race name for output
  bool  pc_race;                  // Can play this race
  int   points;                   // cost in points of the race
  int   class_mult[MAX_CLASS];    // exp multiplier for class, * 100
  int64 act;                     // act bits for the race
  int64 aff;                      // aff bits for the race
  int64 off;                      // off bits for the race
  int64 imm;                      // imm bits for the race
  int64 res;                      // res bits for the race
  int64 vuln;                     // vuln bits for the race
  int64 form;                     // default form flag for the race
  int64 parts;                    // default parts for the race
  const char * skills[5];         // bonus skills for the race
  int   stats[MAX_STATS];         // starting stats
  int   max_stats[MAX_STATS];     // maximum stats
  int   high_stats[MAX_STATS];    // high stats
  int   size;                     // size
  int64 spec;                     // speciality
  int   c_bon;                    // cast bonus for magic categories
  int   c_pen;                    // cast penalties for magic categories
  int   dambonus[DAM_MAX];        // immun-vuln table
  int   weapon_bonus[WEAPON_MAX]; // weapon bonus
  const char   *hand_dam;         // hand damage type
  int   temp;                     // temp for use in lists
  bool  wealth;                   // this race can use money
};

struct deity_choice
{
  const char *name;
  const char *russian;
  int min_align;
  int max_align;
  const char *descr;
  int worship;
  int d_apply[20];
};

struct deity_apply_type
{
  char *param;
  char *inform;
  char *resist;
  const char *res_flag;
//  char *skill;
//  char *spell;
};

struct spec_type
{
  char     *name;      /* special function name */
  SPEC_FUN *function;  /* the function */
};

struct  note_data
{
  NOTE_DATA * next;
  bool        valid;
  int         type;
  const char *sender;
  const char *date;
  const char *to_list;
  const char *subject;
  const char *text;
  time_t      date_stamp;
  const char *host;
};

struct  news_data
{
  NEWS_DATA  *next;
  bool        valid;
  const char *date;
  int         type;
  const char *text;
  time_t      date_stamp;
};

struct penalty_data
{
  PENALTY_DATA *next;       // next_penalty
  bool          valid;
  int           type;       // TICKS, DATE, PERMANENT
  int64         affect;     // Affect
  int64         ticks;      // ticks left
  const char   *inquisitor; // who sets penalty
};

struct vote_char
{
  VOTE_CHAR  *next;
  const char *name;
  int         age;
  int         level;
  int         voting;
};

struct vote_data
{
  const char *msg;
  VOTE_DATA  *next;
  VOTE_CHAR  *chars;
  const char *owner;
};

struct offer_data
{
  OFFER_DATA *next;
  const char *name;
  int64       gold;
  const char *customer;
  int         flag;
};

struct  raffect
{
  RAFFECT         *next;
  RAFFECT         *next_in_room;
  ROOM_INDEX_DATA *room;      // room of raffect
  int              level;     // level of room affect
  int              duration;  // duration of room affect. -1 = forever
  int64            bit;       // bit - sn
  int              target;    // target: DIR_WEST etc
  int64            flag;      // flag for using in sub-spell: IS_FLY etc
  const char      *owner;     // owner of room affect (if need). else NULL
};

struct  affect_data
{
  AFFECT_DATA *next;
  bool         valid;
  int          where;
  int          type;
  int          level;
  int          duration;
  int          location;
  int          modifier;
  int64        bitvector;
};

// A kill structure (indexed by level)
struct  kill_data
{
  int number;
  int killed;
};

struct  clan_data
{
  CLAN_DATA   *next;       // pointer to next clan
  bool         valid;      // validation
  const char  *name;       // clan name
  const char  *show_name;  // clan show name
  const char  *recalmsg1;  // recal message
  const char  *recalmsg2;  // recal message 2
  const char  *war;        // enemy clans
  const char  *alli;       // alli clan
  const char  *acceptalli; // wait for accept alliance
  const char  *short_desc; // short_description of clanobj
  const char  *long_desc;  // long_description of clanobj
  AFFECT_DATA *mod;        // clanobject modificator
  int64        clandeath;  // clandeath room
  int64        clanrecal;  // clanrecal room
  int64        flag;       // clan flags
  int64        gold;       // clanbank gold
  int64        wear_loc;   // clanobj wear location
  int          qp;         // clanbank qp
  int          clansn[21]; // clanskills
  int64        clansnt[21];// clanskills timer
};

// Prototype for a mob. This is the in-memory version of #MOBILES.
struct picture_type
{
 const char * picture[MAX_WEAR];
};

struct  mob_index_data
{
  MOB_INDEX_DATA * next;
  SPEC_FUN       * spec_fun;
  SHOP_DATA      * pShop;
  MPROG_LIST     * mprogs;
  AREA_DATA      * area;
  const char * player_name;
  const char * short_descr;
  const char * long_descr;
  const char * description;
  const char * secure;
  long    wealth;
  int64   vnum;
  int64   act;
  int64   affected_by;
  int64   off_flags;
  int64   imm_flags;
  int64   res_flags;
  int64   vuln_flags;
  int64   mprog_flags;
  int64   form;
  int     group;
  int     count;
  int     killed;
  int     alignment;
  int     level;
  int     hitroll;
  int     hit[3];
  int     mana[3];
  int     damage[3];
  int     ac[4];
  int     dam_type;
  int     start_pos;
  int     default_pos;
  int     sex;
  int     race;
  int     size;
};

/* memory for mobs */
struct mem_data
{
  MEM_DATA    *next;
  bool        valid;
  int         id;     
  int         reaction;
  time_t      when;
};

// One character (PC or NPC)
struct  char_data
{
  CHAR_DATA * next;           // list of all chars
  CHAR_DATA * next_in_room;   // list of chars in room
  CHAR_DATA * master;         // master
  CHAR_DATA * leader;         // group leader
  CHAR_DATA * fighting;       // fightning who
  CHAR_DATA * reply;          // reply to
  CHAR_DATA * pet;            // pointer to pet
  CHAR_DATA * mprog_target;
  CHAR_DATA * questmob;       // Quest quest target mob
  PC_DATA   * pcdata;         // only PC have this
  const char * deity;         // char's deity. Used online
  ROOM_INDEX_DATA * questroom;   // Quest quest room for quest info
  ROOM_INDEX_DATA * in_room;     // in room
  ROOM_INDEX_DATA * was_in_room;
  MOB_INDEX_DATA  * pIndexData;  // pointer to mob index
  DESCRIPTOR_DATA * desc;        // pointer to descriptor
  PENALTY_DATA * penalty;
  MEM_DATA     * memory;
  SPEC_FUN     * spec_fun;
  AFFECT_DATA  * affected; // affects on char
  NOTE_DATA * pnote;       // note writing
  OBJ_DATA  * carrying;    // carrying items
  OBJ_DATA  * on;
  OBJ_DATA  * morph_obj;   // pointer to object
  AREA_DATA * zone;
  GEN_DATA  * gen_data;
  CLAN_DATA * clan;        // character's clan
  CLAN_DATA * clanpet;     // petition to clan
  const char * name;       // name
  const char * short_descr;
  const char * long_descr;
  const char * description;
  const char * prompt;
  const char * prefix;
  const char * stealer;
  const char * host;
  const char * runbuf;
  bool   valid;          // validation
  bool   classmag;
  bool   classwar;
  bool   classthi;
  bool   classcle;
  int64  gold;           // gold
  int64  silver;         // silver
  int64  act;
  int64  comm;
  int64  talk;
  int64  wiznet;
  int64  imm_flags;
  int64  res_flags;
  int64  vuln_flags;
  int64  affected_by;    // affect bits
  int64  form;
  int64  off_flags;
  int64  questgiver;     // Quest master
  int64  q_stat;         // Quest progress & type
  int64  questobj;       // Quest quest target obj
  long   qcounter;       // Quest counter statistic
  long   qcomplete[2];   // Quest complete statistic
  long   id;
  long   vic_npc;        // npc killed
  long   death_npc;      // deaths from npc
  long   vic_pc_total;   // pc killed
  long   death_pc_total; // death from pc
  long   vic_pc_arena;   // victories on arena
  long   death_pc_arena; // deaths on arena
  int    criminal;
  int    godcurse;
  int    spam;
  int    maxrun[2];
  int    damage[3];
  int    dam_type;
  int    start_pos;
  int    default_pos;
  int    size;       // size of char
  int    invis_level;
  int    incog_level;
  int    position;
  int    practice;
  int    train;
  int    carry_weight;
  int    carry_number;
  int    saving_throw;
  int    alignment;
  int    real_alignment; // alignment which selected when char created
  int    hitroll;
  int    damroll;
  int    armor[4];
  int    wimpy;
  int    perm_stat[MAX_STATS];
  int    mod_stat[MAX_STATS];
  int    exp;        // expiricence
  int    questpoints;// Quest quest points
  int    nextquest;  // Quest time to next quest
  int    countdown;  // Quest time for quest
  int    gqcounter;  // GQuest counter statistic
  int    gqcomplete; // GQuest complete statistic
  int    version;    // version of char
  int    group;
  int    clanrank;   // clan rank
  int    sex;
  int    class[MAX_CLASS];
  int    remort;     // number of remorts
  int    race;       // race
  int    level;      // level
  int    trust;      // level-trust
  int    played;
  int    lines;      // lines on screen
  int    timer;
  int    settimer;
  int    wait;
  int    daze;
  int    hit;        // current hp
  int    max_hit;    // max hp
  int    mana;       // current mana
  int    max_mana;   // max mana
  int    move;       // current move
  int    max_move;   // max move
  int    mprog_delay;
  time_t lastlogin;
  time_t lastlogout;
  time_t logon;
};

// data for exp modifying
struct victim_data
{
  CHAR_DATA * victim;
  int         dampool;
};

// Data which only PC's have
struct  pc_data
{
  PC_DATA *    next;
  BUFFER *     buffer;
  bool         valid;
  bool         group_known[MAX_GROUP];
  bool         confirm_delete;
  const char * pseudoname;
  const char * run;
  const char * vislist;
  const char * ignorelist;
  const char * pwd;
  const char * bamfin;
  const char * bamfout;
  const char * title;
  const char * auto_online;
  const char * pkillers;
  const char * marry;
  const char * lovers;
  const char * propose_marry;
  const char * mother;
  const char * father;
  const char * kins;
  const char * deny_addr;
  const char * denied_by;
  const char * alias[MAX_ALIAS];
  const char * alias_sub[MAX_ALIAS];
  const char * runbuf;
  const char * tickstr;
  time_t       last_note;
  time_t       last_idea;
  time_t       last_penalty;
  time_t       last_news;
  time_t       last_bugreport;
  time_t       last_changes;
  time_t       last_offtopic;
  time_t       last_complain;
  int          protect;
  int          charged_spell; // charged spell sn
  int          charged_num;   // number of charges
  int          guild;
  int          elder;
  int          hptrained;
  int          manatrained;
  int          deathcounter;
  int          perm_hit;
  int          perm_mana;
  int          perm_move;
  int          true_sex;
  int          last_level;
  int          condition[5];
  int          learned[MAX_SKILL];
  int          mastery[MAX_SKILL];
  int          points;
  int          confirm_remort;
  int          security;
  int          carma;         // char's carma value
  int          favour;        // char's deityfavour value
  int          dn;            // Only this is saved in player's file
  int          dr;            // Characters' deityrank
  int          dc;            // Deity cursecounter
  int64        proom;
  int64        bounty;
  int64        account;
  int64        comm_save;     // for backup channels in disconnect
  int64        quenia[MAX_QUENIA];
  int64        cfg;
  VICTIM_DATA  victims[MAX_VICT];
  TOURNAMENT_DATA *tournament;
  BET_DATA     *bet;
  GQ_DATA      gquest;
};

// Data for generating characters -- only used during generation
struct gen_data
{
  GEN_DATA *next;
  bool      valid;
  bool      skill_chosen[MAX_SKILL];
  bool      group_chosen[MAX_GROUP];
  int       points_chosen;
};

struct  liq_type
{
  char * liq_name;
  char * liq_showname;
  char * liq_color;
  int    liq_affect[5];
};

struct fix_liquid
{
  char * name;
  char * newname;
};

// Extra description data for a room or object.
struct  extra_descr_data
{
  EXTRA_DESCR_DATA *next; // Next in list
  bool valid;
  const char *keyword;          // Keyword in look/examine
  const char *description;      // What to see
};

struct mprog_code
{
  int64        vnum;
  const char * code;
  MPROG_CODE * next;
};

// Object condition table
struct item_cond_type
{
  int         oIndex;
  int         calccon;
  const char *showcon;
  const char *gshowcon;
  const char *showshortcon;
};

// Material durability info table
struct item_durability_status
{
 int from;              // cond_list lower 
 int to;                // cond_list upper
 const char *d_message; // durability fit message
};

// Follower message table
struct favour_output
{
  int from;
  int to;
  int amp;
  const char *fav_nmstr;
  const char *fav_nfstr;
  const char *fav_amstr;
  const char *fav_afstr;
};

// Prototype for an object
struct  obj_index_data
{
  OBJ_INDEX_DATA   *next;
  EXTRA_DESCR_DATA *extra_descr;
  AFFECT_DATA      *affected;
  AREA_DATA        *area;
  bool        new_format;
  const char *name;
  const char *short_descr;
  const char *description;
  const char *material;
  const char *secure;
  int         level;
  int         reset_num;
  int         item_type;
  int         durability;
  int         condition;
  int         count;
  int         weight;
  int         ttime;
  int64       vnum;
  int64       extra_flags;
  int64       wear_flags;
  int64       cost;
  int64       ttype;
  int64       value[6];
};

// One object
struct  obj_data
{
  OBJ_DATA *        next;
  OBJ_DATA *        next_content;
  OBJ_DATA *        contains;
  OBJ_DATA *        in_obj;
  OBJ_DATA *        on;
  CHAR_DATA *       carried_by;
  EXTRA_DESCR_DATA *extra_descr;
  AFFECT_DATA *     affected;
  OBJ_INDEX_DATA *  pIndexData;
  ROOM_INDEX_DATA * in_room;
  CHAR_DATA *       morph_name;
  bool              is_morphed;
  bool              valid;
  bool              enchanted;
  const char *      owner;
  const char *      name;
  const char *      short_descr;
  const char *      description;
  const char *      material;
  const char *      loaded_by;
  int               level;
  int               item_type;
  int               wear_loc;
  int               weight;
  int               durability;
  int               condition;
  int               timer;
  int               ttime;
  int64             cost;
  int64             extra_flags;
  int64             wear_flags;
  int64             ttype;
  int64             value   [6];
};

// Exit data.
struct  exit_data
{
  union
  {
    ROOM_INDEX_DATA * to_room;
    int64             vnum;
  } u1;
  int   exit_info;
  int64 key;
  const char * keyword;
  const char * description;
  EXIT_DATA  * next;
  int   rs_flags;
  int   orig_door;
};

// Reset commands:
// '*': comment
// 'M': read a mobile 
// 'O': read an object
// 'P': put object in object
// 'G': give object to mobile
// 'E': equip object to mobile
// 'D': set state of door
// 'R': randomize room exits
// 'S': stop (end of list)

// Area-reset definition.
struct  reset_data
{
  RESET_DATA * next;
  char         command;
  int64        arg1;
  int          arg2;
  int64        arg3;
  int          arg4;
};

// Area definition.
struct  area_data
{
  AREA_DATA *  next;
  RESET_DATA * reset_first;
  RESET_DATA * reset_last;
  const char * file_name;
  const char * name;
  const char * credits;
  const char * clan;
  const char * builders;
  int          age;
  int          nplayer;
  int          low_range;
  int          high_range;
  int          vnum;
  int          security;
  int          nrandom;
  int          version;
  int64        area_flags;
  int64        min_vnum;
  int64        max_vnum;
  bool         empty;
};

// Room type.
struct  room_index_data
{
  ROOM_INDEX_DATA  *next;
  CHAR_DATA        *people;
  OBJ_DATA         *contents;
  EXTRA_DESCR_DATA * extra_descr;
  AREA_DATA  *area;
  EXIT_DATA  *exit[6];
  EXIT_DATA  *old_exit[6];
  RESET_DATA *reset_first;
  RESET_DATA *reset_last;
  const char *name;
  const char *description;
  const char *owner;
  int64    vnum;
  int64    ra;
  int64    room_flags;
  int64    sector_type;
  int      heal_rate;
  int      light;
  int      mana_rate;
  RAFFECT *raffect;
};

struct tournament_data
{
  CHAR_DATA *ch1,*ch2;
  int prize;
  int vnum;
  int bank;
  int isbet;
  int isfight;
  BET_DATA *bet;
  TOURNAMENT_DATA *next;
};

struct bet_data
{
  char chname[64], victimname[64];
  int prize;
  BET_DATA *next;
};

// Skills include spells as a particular case.
struct  skill_type
{
  char      * name;                   // Name of skill
  int         skill_level[MAX_CLASS]; // Level needed by class
  int         rating[MAX_CLASS];      // How hard it is to learn
  SPELL_FUN * spell_fun;              // Spell pointer (for spells)
  int         target;                 // Legal targets
  int         minimum_position;       // Position for caster / user
  int       * pgsn;                   // Pointer to associated gsn
  int         group;                  // category
  int         min_mana;               // Minimum mana used
  int         beats;                  // Waiting time after use
  char      * noun_damage;            // Damage message
  char      * msg_off;                // Wear off message
  char      * msg_obj;                // Wear off message for objects
  int64       flag;                   // Spell and Skill FLAG
  char      * help;                   // Brief help message
};

struct  group_type
{
  char * name;
  int    rating[MAX_CLASS];
  char * spells[MAX_IN_GROUP];
};

// Structure for a social in the socials table.
struct  social_type
{
  char         name[20];
  const char * char_no_arg;
  const char * others_no_arg;
  const char * char_found;
  const char * others_found;
  const char * vict_found;
  const char * char_not_found;
  const char * char_auto;
  const char * others_auto;
  const char * help;
  int64        flag;
};

struct  auction_data
{
  OBJ_DATA  * item;   // a pointer to the item
  CHAR_DATA * seller; // a pointer to the seller
  CHAR_DATA * buyer;  // a pointer to the buyer
  int64       bet;    // last bet - or 0 if noone has bet anything
  int         going;  // 1,2, sold
  int         pulse;  // how many pulses (.25 sec) until another call-out ?
};


#endif // left from the tOp !!!!
