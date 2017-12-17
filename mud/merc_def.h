// $Id: merc_def.h,v 1.79 2003/12/16 19:01:26 ghost Exp $
// Defines in FD.
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#ifndef __merc_def
#define __merc_def

// Files Section
#include "files.h"

// For Save Objects
#define MAX_NEST        100

// String and memory management parameters.
#define MAX_KEY_HASH       1024
#define MAX_STRING_LENGTH  4096
#define MAX_INPUT_LENGTH   1024 
#define PAGELEN              22

// Memory management.
// Increase MAX_STRING if you have too.
// Tune the others only if you understand what you're doing
#define  MAX_STRING      8300000
#define  MAX_PERM_BLOCK  131072
#define  MAX_MEM_LIST    11

#define REBOOTCOUNTER_STEP   6 // every reboot add 6 ticks
#define REBOOTCOUNTER_MAX   60 // if reboot_counter>60 - create shutdown file


#define FILE_NORMAL 0x00 // Normal file - No read/write restrictions
#define FILE_RO     0x01 // Read only file
#define FILE_HID    0x02 // Hidden file
#define FILE_SYS    0x04 // System file
#define FILE_DIR    0x10 // Subdirectory
#define SUB_DIR     0x10 // Subdirectory
#define FILE_ARH    0x20 // Archive file

#define ED_AREA 1
#define ED_ROOM 2
#define ED_OBJECT 3
#define ED_MOBILE 4
#define ED_MPCODE 5

#define TEMP_MPCODE 2300000

#define MASK_IGNORE     (1<<TAR_IGNORE)
#define MASK_OFFENSIVE  (1<<TAR_CHAR_OFFENSIVE)
#define MASK_DEFENSIVE  (1<<TAR_CHAR_DEFENSIVE)
#define MASK_SELF       (1<<TAR_CHAR_SELF)
#define MASK_INV        (1<<TAR_OBJ_INV)

// RT ASCII conversions -- used so we can have letters in this file
#define A  1                // 1
#define B  2                // 2
#define C  4                // 3
#define D  8                // 4
#define E  16               // 5
#define F  32               // 6
#define G  64               // 7
#define H  128              // 8

#define I  256              // 9
#define J  512              //10
#define K  1024             //11    
#define L  2048             //12
#define M  4096             //13
#define N  8192             //14
#define O  16384            //15
#define P  32768            //16

#define Q  65536            //17
#define R  131072           //18
#define S  262144           //19
#define T  524288           //20
#define U  1048576          //21
#define V  2097152          //22
#define W  4194304          //23
#define X  8388608          //24

#define Y  16777216         //25
#define Z  33554432         //26
#define aa 67108864         //27 doubled due to conflicts
#define bb 134217728        //28
#define cc 268435456        //29
#define dd 536870912        //30
#define ee 1073741824       //31
#define MAX_FLAG 1073741824 //31

#define ff 2147483648       //32

#define gg 4294967296       //33
#define hh 8589934592       //34
#define ii 17179869184      //35
#define jj 34359738368      //36
#define kk 68719476736      //37
#define ll 137438953472     //38
#define mm 274877906944     //39
#define nn 549755813888     //40

#define oo 1099511627776    //41
#define pp 2199023255552    //42
#define qq 4398046511104    //43
#define rr 8796093022208    //44
#undef ss  //DDD
#define _ss_ 17592186044416   //45
#define tt 35184372088832   //46
#define uu 70368744177664   //47
#define vv 140737488355328  //48

#define ww 281474976710656  //49
#define xx 562949953421312  //50
#define yy 1125899906842624 //51
#define zz 2251799813685248 //52

// Global Config defines
#define CFG_WIZLOCK     (A) // allow login only for immortals
#define CFG_NEWLOCK     (B) // disable creating new chars
#define CFG_BIRTHDAY    (C)
#define CFG_NEWYEAR     (D) // enable new year mode
#define CFG_LOCAL       (E) // enable local mode
#define CFG_RANDOM      (G) // enable random items
#define CFG_PUBLIC      (H)
#define CFG_GQUEST      (I)
#define CFG_ANTICRASH   (J) // Only if compiled 
#define CFG_IGNORECRASH (K) // with ANTICRASH option.
#define CFG_BUGTRACE    (L) // (unicorn)
#define CFG_DLOG        (M) // turn on debug log
#define CFG_NEWHIT      (N) // turn on new_one_hit function
#define CFG_DAMAGEOBJ   (O) // turn on object damaging
#define CFG_GTFIX       (P) // GlobalTemporaryFix option (used once per reboot)
#define CFG_NODR        (Q) // Turn off damroll & hit damage shaping

// Saving defines 
#define SAVE_NORMAL  0 // write to player dir
#define SAVE_BACKUP  1 // write to backup dir
#define SAVE_DELETED 2 // write to deleted dir
#define SAVE_BACKUP2 3 // write to backup2 dir

// Quenia words:
#define MAX_QUENIA     20   // reserved for pfiles and data array
#define Q_RESTORE      (B)
#define Q_GOODSPELL    (C)
#define Q_BADSPELL     (D)
#define Q_POWER        (E)
#define Q_BLESS        (F)
#define Q_FIREPROOF    (G)
#define Q_PEACE        (H)
#define Q_END          (ee)

// defines for items ITEM_BONUS
#define SCROLL_QUENIA    1
#define SCROLL_CLANSKILL 2

// number of mobiles you "remember"
#define MAX_VICT 10  

// Game parameters.
// Increase the max'es if you add more of something.
// Adjust the pulse numbers to suit yourself.
#define MAX_PK_RANGE  6
#define MAX_SKILL   219
#define PK_LEVEL      5
#define MAX_GROUP    41 
#define MAX_IN_GROUP 50
#define MAX_ALIAS    15
#define MAX_CLASS     4

#define CLASS_MAG     0
#define CLASS_CLE     1
#define CLASS_THI     2
#define CLASS_WAR     3

#define MAX_NEWSPAPER       500
#define MAX_DAMAGE_MESSAGE   45
#define MAX_LEVEL           110
#define LEVEL_HERO     (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL (MAX_LEVEL - 8)

#define IMPLEMENTOR MAX_LEVEL
#define CREATOR     (MAX_LEVEL - 1)
#define SUPREME     (MAX_LEVEL - 2)
#define DEITY       (MAX_LEVEL - 3)
#define GOD         (MAX_LEVEL - 4)
#define IMMORTAL    (MAX_LEVEL - 5)
#define DEMI        (MAX_LEVEL - 6)
#define ANGEL       (MAX_LEVEL - 7)
#define AVATAR      (MAX_LEVEL - 8)
#define HERO        LEVEL_HERO

#define CLAN_LONER      (A)
#define CLAN_ROM        (B) // for 100-th channel 
#define CLAN_CANTLEAVE  (C) // members can't petition to another clan 
#define CLAN_SEC_ACCEPT (D)
#define CLAN_SEC_REJECT (E)
#define CLAN_SEC_RAISE  (F)
#define CLAN_SEC_REMOVE (G)
#define CLAN_SEC_SHOW   (H)
#define CLAN_SEC_WAR    (I)
#define CLAN_SEC_ALLI   (J)
#define CLAN_DEP_ACCEPT (K)
#define CLAN_DEP_REJECT (L)
#define CLAN_DEP_RAISE  (M)
#define CLAN_DEP_REMOVE (N)
#define CLAN_DEP_SHOW   (O)
#define CLAN_DEP_WAR    (P)
#define CLAN_DEP_ALLI   (Q)
#define CLAN_MERCENARIE (R)
#define CLAN_CYBORG     (S)
#define CLAN_WARRIOR    (R)
#define CLAN_MAGE       (S)
#define CLAN_NIGHT      (T)
#define CLAN_LIGHT      (U)
#define CLAN_LIFE       (V)
#define CLAN_DEATH      (W)
#define CLAN_THIEF      (X)
#define CLAN_HUNTER     (Y)

// Channel list
#define AUCTION     1
#define GOSSIP      2
#define NEWBIECHAT  3
#define GRATS       4
#define QUOTE       5
#define ANSWER      6
#define MUSIC       7
#define CLAN        8
#define SHOUT       9
#define YELL       10
#define CHAT       11
#define ACLAN      12

// races
#define RACE_NULL     0
#define RACE_HUMAN    1
#define RACE_ELF      2
#define RACE_DWARF    3
#define RACE_GIANT    4
#define RACE_VAMPIRE  5
#define RACE_OGRE     6
#define RACE_HOBBIT   7
#define RACE_ORC      8
#define RACE_DROW     9
#define RACE_CENTAUR  10
#define RACE_LIZARD   11
#define RACE_ETHERIAL 12
#define RACE_SPRITE   13
#define RACE_HGRYPHON 14
#define RACE_DRUID    15
#define RACE_SKELETON 16
#define RACE_ZOMBIE   17
#define RACE_UNIQUE   42

// Criminal 
#define C_RAPER    1
#define C_CRIMINAL 2
#define C_DEMBEL   3

// Colour stuff by Lope of Loping Through The MUD
#define CLEAR       "[0m"    // Resets Colour        
#define C_RED       "[0;31m" // Normal Colours       
#define C_GREEN     "[0;32m"
#define C_YELLOW    "[0;33m"
#define C_BLUE      "[0;34m"
#define C_MAGENTA   "[0;35m"
#define C_CYAN      "[0;36m"
#define C_WHITE     "[0;37m"
#define C_D_GREY    "[1;30m" // Light Colors         
#define C_B_RED     "[1;31m"
#define C_B_GREEN   "[1;32m"
#define C_B_YELLOW  "[1;33m"
#define C_B_BLUE    "[1;34m"
#define C_B_MAGENTA "[1;35m"
#define C_B_CYAN    "[1;36m"
#define C_B_WHITE   "[1;37m"
#define C_FLASHON   "[0;5m"  // Flash 
#define C_FLASHOFF  "[0;4m"

// Site ban structure. 
#define BAN_NEWBIE      (A)
#define BAN_ALL         (B)
#define BAN_PERMANENT   (C)
#define BAN_SUFFIX      (D)
#define BAN_PREFIX      (E)
#define BAN_IMM         (F)

#define MAX_VNUM  99999 

#define DAM_MAX     20    // Max number of DAM_TYPES
#define WEAPON_MAX  11    // Max number of WEAPON types
#define RANDOM_VNUM 25014

#define SHOW    (A) // command shown by 'command' command
#define MORPH   (B) // command is avaible, when you are morphed
#define HIDE    (C) // command will not remove hide affect
#define FULL    (D) // command must be typed in full form
#define NOMOB   (E) // mobs cant use this command
#define SPAM    (F) // command is protected by ANTISPAM system
#define NOARMY  (G) // command is can't be used in army
#define NOADR   (H) // command is can't be used under adrenalin
#define NOZRIT  (I) // command is can't be used if you are zritel
#define DENY    (J) // command is can't be used
#define NOALL   (K) // do not use with order all
#define NOORDER (L) // do not use with order
#define OLC     (M) // OLC LOG
#define FREEZE  (O) // not work on freezed chars
#define SAVE    (P) // char will mark with willsave flag
#define NOPUB   (Q) // command will not allowed in restricted version
#define NOLOG   (R) // command will not allowed in restricted version
#define NOFORCE (S) // not allowed with order or any type of force

// Time and weather
#define SUN_DARK   0
#define SUN_RISE   1
#define SUN_LIGHT  2
#define SUN_SET    3

#define SKY_CLOUDLESS  0
#define SKY_CLOUDY     1
#define SKY_RAINING    2
#define SKY_LIGHTNING  3

// Connected state for a channel.
#define CON_PLAYING                 0
#define CON_GET_NAME                1
#define CON_GET_OLD_PASSWORD        2
#define CON_CONFIRM_NEW_NAME        3
#define CON_GET_NEW_PASSWORD        4
#define CON_CONFIRM_NEW_PASSWORD    5
#define CON_GET_NEW_RACE            6
#define CON_GET_NEW_SEX             7
#define CON_GET_NEW_CLASS           8
#define CON_GET_ALIGNMENT           9
#define CON_DEFAULT_CHOICE         10 
#define CON_GEN_GROUPS             11 
#define CON_PICK_WEAPON            12
#define CON_READ_MOTD              14
#define CON_BREAK_CONNECT          15
#define CON_GET_CODEPAGE           16
#define CON_REMORT                 17
#define CON_DROP_CLASS             18

// TO types for act.
#define TO_ROOM         0
#define TO_NOTVICT      1
#define TO_VICT         2
#define TO_CHAR         3
#define TO_ALL          4
#define TO_ALL_IN_ROOM  5
#define TO_NOTCHARVICT  6

// do_act types of spam
#define SPAM_MISS     (A)
#define SPAM_OTHERF   (B)
#define SPAM_SKMISS   (C)
#define SPAM_WEAPAF   (D)

// Shop types.
#define MAX_TRADE  5

// Per-class stuff.
#define MAX_STATS   5
#define STAT_STR    0
#define STAT_INT    1
#define STAT_WIS    2
#define STAT_DEX    3
#define STAT_CON    4
#define STAT_HP     5
#define STAT_MANA   6

// Data structure for notes.
#define NOTE_NOTE       0
#define NOTE_IDEA       1
#define NOTE_PENALTY    2
#define NOTE_NEWS       3
#define NOTE_CHANGES    4
#define NOTE_BUGREPORT  5
#define NOTE_OFFTOPIC   6
#define NOTE_COMPLAIN   7

// Clan ranks
#define MEMBER 0
#define JUNIOR 1
#define SENIOR 2
#define DEPUTY 3
#define SECOND 4
#define LEADER 5

#define VOTE_UNKNOWN  0
#define VOTE_YES      1
#define VOTE_NO       2
#define VOTE_HZ       3

// where definitions
#define TO_AFFECTS  0
#define TO_OBJECT   1
#define TO_IMMUNE   2
#define TO_RESIST   3
#define TO_VULN     4
#define TO_WEAPON   5

// Well known mob virtual numbers. Defined in #MOBILES.
#define MOB_VNUM_FIDO       3090
#define MOB_VNUM_CITYGUARD  3060
#define MOB_VNUM_VAMPIRE    3404
#define MOB_VNUM_BOUNTYHUNTER 98
#define MOB_VNUM_LAWGUARD     99

#define MOB_VNUM_PATROLMAN  2106
#define GROUP_VNUM_TROLLS   2100
#define GROUP_VNUM_OGRES    2101
#define ITEM_EAR              24

// ACT bits for mobs. Used in #MOBILES.
#define ACT_IS_NPC            (A) // Auto set for mobs
#define ACT_SENTINEL          (B) // Stays in one room
#define ACT_SCAVENGER         (C) // Picks up objects
#define ACT_IS_KEEPER         (D) // Shop
#define ACT_ASSASIN_MASTER    (E) // ASSASING GUILD MASTER
#define ACT_AGGRESSIVE        (F) // Attacks PC's
#define ACT_STAY_AREA         (G) // Won't leave area
#define ACT_WIMPY             (H) // Mob will flee
#define ACT_PET               (I) // Auto set for pets
#define ACT_TRAIN             (J) // Can train PC's
#define ACT_PRACTICE          (K) // Can practice PC's
#define ACT_ACCOUNTER         (L) // can give account (C) Sab
#define ACT_NOQUEST           (M) // can't be quest target (C) Sab
#define ACT_NOSTEAL           (N)
#define ACT_UNDEAD            (O)
#define ACT_REFERI            (P) // is referi (C) Sab
#define ACT_CLERIC            (Q)
#define ACT_MAGE              (R)
#define ACT_THIEF             (S)
#define ACT_WARRIOR           (T)
#define ACT_NOALIGN           (U)
#define ACT_NOPURGE           (V)
#define ACT_STOPDOOR          (W) // Do not wander thru doors
#define ACT_CLANENCHANTER     (X) // Clan enchants (C) Jasana
#define ACT_FORGER            (Y) // can use forge skills (C) Ast
#define ACT_STAY_PLAIN        (Z) // Mob moves only within one sector type
#define ACT_IS_HEALER         (aa)
#define ACT_GAIN              (bb)
#define ACT_UPDATE_ALWAYS     (cc)
#define ACT_IS_CHANGER        (dd)
#define ACT_EXTRACT_CORPSE    (ee)

// damage classes
#define DAM_NONE       0
#define DAM_BASH       1
#define DAM_PIERCE     2
#define DAM_SLASH      3
#define DAM_FIRE       4
#define DAM_COLD       5
#define DAM_LIGHTNING  6
#define DAM_ACID       7
#define DAM_POISON     8
#define DAM_NEGATIVE   9
#define DAM_HOLY       10
#define DAM_ENERGY     11
#define DAM_MENTAL     12
#define DAM_DISEASE    13
#define DAM_DROWNING   14
#define DAM_LIGHT      15
#define DAM_OTHER      16
#define DAM_HARM       17
#define DAM_CHARM      18
#define DAM_SOUND      19

// OFF bits for mobiles
#define OFF_AREA_ATTACK (A)
#define OFF_BACKSTAB    (B)
#define OFF_BASH        (C)
#define OFF_BERSERK     (D)
#define OFF_DISARM      (E)
#define OFF_DODGE       (F)
#define OFF_FADE        (G)
#define OFF_FAST        (H)
#define OFF_KICK        (I)
#define OFF_KICK_DIRT   (J)
#define OFF_PARRY       (K)
#define OFF_RESCUE      (L)
#define OFF_TAIL        (M)
#define OFF_TRIP        (N)
#define OFF_CRUSH       (O)
#define ASSIST_ALL      (P)
#define ASSIST_ALIGN    (Q)
#define ASSIST_RACE     (R)
#define ASSIST_PLAYERS  (S)
#define ASSIST_GUARD    (T)
#define ASSIST_VNUM     (U)
#define OFF_BLINK       (W)   // mob can use blink (c) Apc
#define OFF_DWARVES_ACCOUNTER (Y) // dwarven guild accounter
#define OFF_ASSASIN_ACCOUNTER (Z) // assasin guild accounter

// return values for check_imm
#define IS_NORMAL       0
#define IS_IMMUNE       1
#define IS_RESISTANT    2
#define IS_VULNERABLE   3

// IMM bits for mobs
#define IMM_SUMMON     (A)
#define IMM_CHARM      (B)
#define IMM_MAGIC      (C)
#define IMM_WEAPON     (D)
#define IMM_BASH       (E)
#define IMM_PIERCE     (F)
#define IMM_SLASH      (G)
#define IMM_FIRE       (H)
#define IMM_COLD       (I)
#define IMM_LIGHTNING  (J)
#define IMM_ACID       (K)
#define IMM_POISON     (L)
#define IMM_NEGATIVE   (M)
#define IMM_HOLY       (N)
#define IMM_ENERGY     (O)
#define IMM_MENTAL     (P)
#define IMM_DISEASE    (Q)
#define IMM_DROWNING   (R)
#define IMM_LIGHT      (S)
#define IMM_SOUND      (T)
#define IMM_HARM       (U)
#define IMM_WOOD       (X)
#define IMM_SILVER     (Y)
#define IMM_IRON       (Z)
#define IMM_ALL        (aa)
 
// RES bits for mobs
#define RES_SUMMON     (A)
#define RES_CHARM      (B)
#define RES_MAGIC      (C)
#define RES_WEAPON     (D)
#define RES_BASH       (E)
#define RES_PIERCE     (F)
#define RES_SLASH      (G)
#define RES_FIRE       (H)
#define RES_COLD       (I)
#define RES_LIGHTNING  (J)
#define RES_ACID       (K)
#define RES_POISON     (L)
#define RES_NEGATIVE   (M)
#define RES_HOLY       (N)
#define RES_ENERGY     (O)
#define RES_MENTAL     (P)
#define RES_DISEASE    (Q)
#define RES_DROWNING   (R)
#define RES_LIGHT      (S)
#define RES_SOUND      (T)
#define RES_WOOD       (X)
#define RES_SILVER     (Y)
#define RES_IRON       (Z)
 
// VULN bits for mobs
#define VULN_SUMMON    (A)
#define VULN_CHARM     (B)
#define VULN_MAGIC     (C)
#define VULN_WEAPON    (D)
#define VULN_BASH      (E)
#define VULN_PIERCE    (F)
#define VULN_SLASH     (G)
#define VULN_FIRE      (H)
#define VULN_COLD      (I)
#define VULN_LIGHTNING (J)
#define VULN_ACID      (K)
#define VULN_POISON    (L)
#define VULN_NEGATIVE  (M)
#define VULN_HOLY      (N)
#define VULN_ENERGY    (O)
#define VULN_MENTAL    (P)
#define VULN_DISEASE   (Q)
#define VULN_DROWNING  (R)
#define VULN_LIGHT     (S)
#define VULN_SOUND     (T)
#define VULN_WOOD      (X)
#define VULN_SILVER    (Y)
#define VULN_IRON      (Z)
 
// body form
#define FORM_EDIBLE        (A)
#define FORM_POISON        (B)
#define FORM_MAGICAL       (C)
#define FORM_INSTANT_DECAY (D)
#define FORM_OTHER         (E)  // defined by material bit
 
// actual form
#define FORM_ANIMAL      (G)
#define FORM_SENTIENT    (H)
#define FORM_UNDEAD      (I)
#define FORM_CONSTRUCT   (J)
#define FORM_MIST        (K)
#define FORM_INTANGIBLE  (L)
#define FORM_BIPED       (M)
#define FORM_CENTAUR     (N)
#define FORM_INSECT      (O)
#define FORM_SPIDER      (P)
#define FORM_CRUSTACEAN  (Q)
#define FORM_WORM        (R)
#define FORM_BLOB        (S)
 
#define FORM_MAMMAL      (V)
#define FORM_BIRD        (W)
#define FORM_REPTILE     (X)
#define FORM_SNAKE       (Y)
#define FORM_DRAGON      (Z)
#define FORM_AMPHIBIAN   (aa)
#define FORM_FISH        (bb)
#define FORM_COLD_BLOOD  (cc)    
 
// body parts
#define PART_HEAD        (A)
#define PART_ARMS        (B)
#define PART_LEGS        (C)
#define PART_HEART       (D)
#define PART_BRAINS      (E)
#define PART_GUTS        (F)
#define PART_HANDS       (G)
#define PART_FEET        (H)
#define PART_FINGERS     (I)
#define PART_EAR         (J)
#define PART_EYE         (K)
#define PART_LONG_TONGUE (L)
#define PART_EYESTALKS   (M)
#define PART_TENTACLES   (N)
#define PART_FINS        (O)
#define PART_WINGS       (P)
#define PART_TAIL        (Q)

// for combat
#define PART_CLAWS   (U)
#define PART_FANGS   (V)
#define PART_HORNS   (W)
#define PART_SCALES  (X)
#define PART_TUSKS   (Y)

// Bits for 'affected_by'. Used in #MOBILES.
#define AFF_BLIND          (A)
#define AFF_INVISIBLE      (B)
#define AFF_DETECT_EVIL    (C)
#define AFF_DETECT_INVIS   (D)
#define AFF_DETECT_MAGIC   (E)
#define AFF_DETECT_HIDDEN  (F)
#define AFF_DETECT_GOOD    (G)
#define AFF_SANCTUARY      (H)
#define AFF_FAERIE_FIRE    (I)
#define AFF_INFRARED       (J)
#define AFF_CURSE          (K)
#define AFF_SWIM           (L)
#define AFF_POISON         (M)
#define AFF_PROTECT_EVIL   (N)
#define AFF_PROTECT_GOOD   (O)
#define AFF_SNEAK          (P)
#define AFF_HIDE           (Q)
#define AFF_NOSTALGIA      (R)
#define AFF_CHARM          (S)
#define AFF_FLYING         (T)
#define AFF_PASS_DOOR      (U)
#define AFF_HASTE          (V)
#define AFF_CALM           (W)
#define AFF_PLAGUE         (X)
#define AFF_SHIELD         (Y)
#define AFF_DARK_VISION    (Z)
#define AFF_BERSERK        (aa)
#define AFF_FIRESHIELD     (bb)
#define AFF_REGENERATION   (cc)
#define AFF_SLOW           (dd)
#define AFF_MIST           (ee)
#define AFF_EVIL_PR        (ff)
#define AFF_LIFE_STR       (gg)
#define AFF_GASEOUS_FORM   (hh)
#define AFF_FORCEFIELD     (ii)
#define AFF_SAFE_PLACE     (jj)
#define AFF_REJUVINATE     (kk)
#define AFF_PEACE          (ll)
#define AFF_VIOLENCE       (mm)
#define AFF_CANT_FEAR      (nn)
#define AFF_MADNESS        (oo)
#define AFF_VISION         (pp)
#define AFF_MAGIC_BAR      (qq)//Last


// Defines for race specials
#define SPEC_FLY           (A)  // Can fly
#define SPEC_SNEAK         (B)  // Permanent Sneak
#define SPEC_MANAREGEN     (C)  // Mana regeneration double
#define SPEC_DKICK         (D)  // Double Kick
#define SPEC_CRUSH         (E)  // Crush skill
#define SPEC_TAIL          (F)  // Tail skill
#define SPEC_VAMPIRE       (G)  // Vampires feature:
                                // Do not changes align with vampiric
                                // can't wear silver
                                // drinking blood adds hp
                                // stats depending on sol
                                // hand damage sometimes energy
#define SPEC_NOEAT         (H)  // Do not become hunger
#define SPEC_NODRINK       (I)  // Do not become thirst
#define SPEC_TWOHAND       (J)  // Can use two-hand weapon in one hand
#define SPEC_REGENSP       (K)  // Spell Regeneration casting improved
#define SPEC_BLACKSMITH    (L)  // Blacksmith skill
#define SPEC_REGENERATION  (M)  // Regeneration Great improved
#define SPEC_ENERGY        (N)  // Energy Drain casting improved
#define SPEC_DODGE         (O)  // Advanced Dodge using
#define SPEC_INVIS         (P)  // Invisible spec
#define SPEC_HIDE          (P)  // Hide spec
#define SPEC_PSY           (Q)  // Psionic
#define SPEC_PASSFLEE      (R)  // Can flee thru closed doors
#define SPEC_MIST          (S)  // Can use mist command (vampires)
#define SPEC_HOWL          (T)  // Can use howl command (ethereals)
#define SPEC_TRAIN         (U)  // Can train +1 stat (humans)
#define SPEC_RESBASH       (V)  // Resist to Bash
#define SPEC_UWATER        (W)  // Like a fish
#define SPEC_NOLOSTEXP     (X)  // Not lost expirience with death
#define SPEC_WATERWALK     (Y)  // +3 dex on Water
#define SPEC_RDEATH        (Z)  // full restore after death
#define SPEC_BACKSTAB      (aa) // +7% backstab damage
#define SPEC_IGNOREALIGN   (bb) // ignore align for equipment

// Sex. Used in #MOBILES.
#define SEX_NEUTRAL 0
#define SEX_NONE    0
#define SEX_MALE    1
#define SEX_FEMALE  2
#define SEX_EITHER  3

// AC types
#define AC_PIERCE  0
#define AC_BASH    1
#define AC_SLASH   2
#define AC_EXOTIC  3

// dice
#define DICE_NUMBER 0
#define DICE_TYPE   1
#define DICE_BONUS  2

// size
#define SIZE_TINY   0
#define SIZE_SMALL  1
#define SIZE_MEDIUM 2
#define SIZE_LARGE  3
#define SIZE_HUGE   4
#define SIZE_GIANT  5

// Well known object virtual numbers. Defined in #OBJECTS.
#define OBJ_VNUM_SILVER_ONE        1
#define OBJ_VNUM_GOLD_ONE          2
#define OBJ_VNUM_GOLD_SOME         3
#define OBJ_VNUM_SILVER_SOME       4
#define OBJ_VNUM_COINS             5
#define OBJ_VNUM_DIAMOND        3377
#define OBJ_VNUM_CRYSTAL          32
#define MAX_OBJS_VALUE         32767  
#define OBJ_VNUM_MUMMY            20
                                 
#define OBJ_VNUM_CORPSE_NPC       10
#define OBJ_VNUM_CORPSE_PC        11
#define OBJ_VNUM_SEVERED_HEAD     12
#define OBJ_VNUM_TORN_HEART       13
#define OBJ_VNUM_SLICED_ARM       14
#define OBJ_VNUM_SLICED_LEG       15
#define OBJ_VNUM_GUTS             16
#define OBJ_VNUM_BRAINS           17

#define OBJ_VNUM_MUSHROOM         20
#define OBJ_VNUM_LIGHT_BALL       21
#define OBJ_VNUM_SPRING           22
#define OBJ_VNUM_DISC             23
#define OBJ_VNUM_PORTAL           25

#define OBJ_VNUM_ROSE           1001
#define OBJ_VNUM_PIT            3010

#define OBJ_VNUM_SCHOOL_MACE    3700
#define OBJ_VNUM_SCHOOL_DAGGER  3701
#define OBJ_VNUM_SCHOOL_SWORD   3702
#define OBJ_VNUM_SCHOOL_SPEAR   3717
#define OBJ_VNUM_SCHOOL_STAFF   3718
#define OBJ_VNUM_SCHOOL_AXE     3719
#define OBJ_VNUM_SCHOOL_FLAIL   3720
#define OBJ_VNUM_SCHOOL_WHIP    3721
#define OBJ_VNUM_SCHOOL_POLEARM 3722

#define OBJ_VNUM_SCHOOL_VEST    3703
#define OBJ_VNUM_SCHOOL_SHIELD  3704
#define OBJ_VNUM_SCHOOL_BANNER  3716
#define OBJ_VNUM_MAP            3162

#define OBJ_VNUM_ARMY_MACE     12411
#define OBJ_VNUM_ARMY_DAGGER   12409
#define OBJ_VNUM_ARMY_SWORD    12406
#define OBJ_VNUM_ARMY_SPEAR    12414
#define OBJ_VNUM_ARMY_STAFF    12414
#define OBJ_VNUM_ARMY_AXE      12410
#define OBJ_VNUM_ARMY_FLAIL    12412
#define OBJ_VNUM_ARMY_WHIP     12415
#define OBJ_VNUM_ARMY_POLEARM  12413

#define OBJ_VNUM_ARMY_VEST     12433
#define OBJ_VNUM_ARMY_SHIELD   12434
#define OBJ_VNUM_ARMY_BOOTS    12403
#define OBJ_VNUM_ARMY_CAP      12401
#define OBJ_VNUM_WHISTLE        2116
#define OBJ_VNUM_ARMYARTEFACT     70
#define OBJ_VNUM_VAMPIREARTEFACT  71

// Item types. Used in #OBJECTS.
#define ITEM_LIGHT       1
#define ITEM_SCROLL      2
#define ITEM_WAND        3
#define ITEM_STAFF       4
#define ITEM_WEAPON      5
#define ITEM_TREASURE    8
#define ITEM_ARMOR       9
#define ITEM_POTION     10
#define ITEM_CLOTHING   11
#define ITEM_FURNITURE  12
#define ITEM_TRASH      13
#define ITEM_CONTAINER  15
#define ITEM_DRINK_CON  17
#define ITEM_KEY        18
#define ITEM_FOOD       19
#define ITEM_MONEY      20
#define ITEM_BOAT       22
#define ITEM_CORPSE_NPC 23
#define ITEM_CORPSE_PC  24
#define ITEM_FOUNTAIN   25
#define ITEM_PILL       26
#define ITEM_PROTECT    27
#define ITEM_MAP        28
#define ITEM_PORTAL     29
#define ITEM_WARP_STONE 30
#define ITEM_ROOM_KEY   31
#define ITEM_GEM        32
#define ITEM_JEWELRY    33
#define ITEM_JUKEBOX    34
#define ITEM_ENCHANT    35
#define ITEM_SCUBA      36
#define ITEM_BONUS      37

// Extra flags. Used in #OBJECTS.
#define ITEM_GLOW          (A)
#define ITEM_HUM           (B)
#define ITEM_DARK          (C)
#define ITEM_LOCK          (D)
#define ITEM_EVIL          (E)
#define ITEM_INVIS         (F)
#define ITEM_MAGIC         (G)
#define ITEM_NODROP        (H)
#define ITEM_BLESS         (I)
#define ITEM_ANTI_GOOD     (J)
#define ITEM_ANTI_EVIL     (K)
#define ITEM_ANTI_NEUTRAL  (L)
#define ITEM_NOREMOVE      (M)
#define ITEM_INVENTORY     (N)
#define ITEM_NOPURGE       (O)
#define ITEM_ROT_DEATH     (P)
#define ITEM_VIS_DEATH     (Q)
#define ITEM_CLANENCHANT   (R)
#define ITEM_NONMETAL      (S)
#define ITEM_NOLOCATE      (T)
#define ITEM_MELT_DROP     (U)
#define ITEM_HAD_TIMER     (V)
#define ITEM_SELL_EXTRACT  (W)
#define ITEM_NO_IDENT      (X)
#define ITEM_BURN_PROOF    (Y)
#define ITEM_NOUNCURSE     (Z)
#define ITEM_WILLSAVE     (aa)
#define ITEM_DELETED      (dd)

// Wear flags.Used in #OBJECTS.
#define ITEM_TAKE        (A)
#define ITEM_WEAR_FINGER (B)
#define ITEM_WEAR_NECK   (C)
#define ITEM_WEAR_BODY   (D)
#define ITEM_WEAR_HEAD   (E)
#define ITEM_WEAR_LEGS   (F)
#define ITEM_WEAR_FEET   (G)
#define ITEM_WEAR_HANDS  (H)
#define ITEM_WEAR_ARMS   (I)
#define ITEM_WEAR_SHIELD (J)
#define ITEM_WEAR_ABOUT  (K)
#define ITEM_WEAR_WAIST  (L)
#define ITEM_WEAR_WRIST  (M)
#define ITEM_WIELD       (N)
#define ITEM_HOLD        (O)
#define ITEM_NO_SAC      (P)
#define ITEM_WEAR_FLOAT  (Q)

// magic item's affects
#define ENCH_NONE            0
#define ENCH_BLESS           1
#define ENCH_REMVAMP         2
#define ENCH_NODROP          3
#define ENCH_NOREMOVE        4
#define ENCH_BURNPROOF       5
#define ENCH_REMINVIS        6
#define ENCH_NOLOCATE        7
#define ENCH_NOUNCURSE       8
#define ENCH_REMANTIGOOD     9
#define ENCH_REMANTIEVIL    10
#define ENCH_REMANTINEUTRAL 11
#define ENCH_ADDVAMP        12
#define ENCH_ADDFLAMING     13
#define ENCH_REMTWOHAND     14
#define ENCH_SETWEIGHT      15
#define ENCH_HUM            16
#define ENCH_SHARP          17
#define MAX_ENCHANTS        17

// weapon class
#define WEAPON_EXOTIC   0
#define WEAPON_SWORD    1
#define WEAPON_DAGGER   2
#define WEAPON_SPEAR    3
#define WEAPON_MACE     4
#define WEAPON_AXE      5
#define WEAPON_FLAIL    6
#define WEAPON_WHIP     7       
#define WEAPON_POLEARM  8
#define WEAPON_STAFF    9

// weapon types
#define WEAPON_FLAMING   (A)
#define WEAPON_FROST     (B)
#define WEAPON_VAMPIRIC  (C)
#define WEAPON_SHARP     (D)
#define WEAPON_VORPAL    (E)
#define WEAPON_TWO_HANDS (F)
#define WEAPON_SHOCKING  (G)
#define WEAPON_POISON    (H)
#define WEAPON_RETURN    (I)
#define WEAPON_ROUND     (J)
#define WEAPON_MISSILE   (K)
#define WEAPON_VAMP_MANA (L)

// gate flags
#define GATE_NORMAL_EXIT (A)
#define GATE_NOCURSE     (B)
#define GATE_GOWITH      (C)
#define GATE_BUGGY       (D)
#define GATE_RANDOM      (E)

// furniture flags
#define STAND_AT      (A)
#define STAND_ON      (B)
#define STAND_IN      (C)
#define SIT_AT        (D)
#define SIT_ON        (E)
#define SIT_IN        (F)
#define REST_AT       (G)
#define REST_ON       (H)
#define REST_IN       (I)
#define SLEEP_AT      (J)
#define SLEEP_ON      (K)
#define SLEEP_IN      (L)
#define PUT_AT        (M)
#define PUT_ON        (N)
#define PUT_IN        (O)
#define PUT_INSIDE    (P)

// Apply types (for affects). Used in #OBJECTS.
#define APPLY_NONE            0
#define APPLY_STR             1
#define APPLY_DEX             2
#define APPLY_INT             3
#define APPLY_WIS             4
#define APPLY_CON             5
#define APPLY_SEX             6
#define APPLY_CLASS           7
#define APPLY_LEVEL           8
#define APPLY_AGE             9
#define APPLY_HEIGHT         10
#define APPLY_WEIGHT         11
#define APPLY_MANA           12
#define APPLY_HIT            13
#define APPLY_MOVE           14
#define APPLY_GOLD           15
#define APPLY_EXP            16
#define APPLY_AC             17
#define APPLY_HITROLL        18
#define APPLY_DAMROLL        19
#define APPLY_SAVES          20
#define APPLY_SAVING_PARA    20
#define APPLY_SAVING_ROD     21
#define APPLY_SAVING_PETRI   22
#define APPLY_SAVING_BREATH  23
#define APPLY_SAVING_SPELL   24
#define APPLY_SPELL_AFFECT   25

// Values for containers (value[1]). Used in #OBJECTS.
#define CONT_CLOSEABLE    1
#define CONT_PICKPROOF    2
#define CONT_CLOSED       4
#define CONT_LOCKED       8
#define CONT_PUT_ON      16

// Well known room virtual numbers.Defined in #ROOMS.
#define ROOM_VNUM_LIMBO           2
#define ROOM_VNUM_PRISON          3
#define ROOM_VNUM_CHAT         1200
#define ROOM_VNUM_TEMPLE       3001
#define ROOM_VNUM_ALTAR        3054
#define ROOM_VNUM_SCHOOL       3700
#define ROOM_VNUM_BALANCE      4500
#define ROOM_VNUM_CIRCLE       4400
#define ROOM_VNUM_DEMISE       4201
#define ROOM_VNUM_HONOR        4300
#define ROOM_VNUM_ARENA        9800
#define ROOM_VNUM_ARMY_BED    12554
#define ROOM_VNUM_ARMY_MORGUE 12559
#define ROOM_VNUM_BLACKSMITH   3298
#define ROOM_VNUM_KOZEL        3005

// Room flags.Used in #ROOMS.
#define ROOM_EVIL(ch)      (IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL) || IS_SET( ch->in_room->ra, RAFF_EVIL_PR))
#define ROOM_DARK          (A) // char must have light
#define ROOM_NO_MAGIC      (B) // spells aren't work there
#define ROOM_NO_MOB        (C) // mobs can't enter room
#define ROOM_INDOORS       (D) // no weather
#define ROOM_HARD          (E) // great waitstate for crossing room
#define ROOM_NOMORPH       (F) // morph did not work there
#define ROOM_ALL_VIS       (G) // Everyone (not items) is visible in this room
#define ROOM_PRIVATE       (J) // only 2 chars allowed
#define ROOM_RFORGE        (H) // forging allowed in here
#define ROOM_DELETED       (I) // room will be deleted
#define ROOM_SAFE          (K) // no combat there
#define ROOM_SOLITARY      (L) // only 1 persone allowed
#define ROOM_PET_SHOP      (M) // pet shop
#define ROOM_NO_RECALL     (N) // recall did not work there
#define ROOM_GODS_ONLY     (P) // only gods allowed
#define ROOM_NOSUICIDE     (O) // suicide not allowed
#define ROOM_HEROES_ONLY   (Q) // only heroes allowed
#define ROOM_NEWBIES_ONLY  (R) // only newbies allowed
#define ROOM_LAW           (S) // law room (nocharm, ..)
#define ROOM_NOWHERE       (T) // hidden from where command
#define ROOM_ARENA         (U) // arena
#define ROOM_NOFLEE        (V) // flee did not work there
#define ROOM_ELDER         (W) // room is protected by elders
#define ROOM_BLACKSMITH    (X) // blacksmith
#define ROOM_DWARVES_GUILD (Y) // only dwarves guild member allowed
#define ROOM_DWARVES_RENT  (Z) // Dwarves money
#define ROOM_MAG_ONLY     (aa) // only mages allowed
#define ROOM_WAR_ONLY     (bb) // only warriors allowed
#define ROOM_THI_ONLY     (cc) // only thieves allowed
#define ROOM_CLE_ONLY     (dd) // only clerics allowed
#define ROOM_MARRY        (ee) // divorce and marry command allowed there

// Directions. Used in #ROOMS.
#define MAX_DIR 6       // Up Down North East South West

#define DIR_NORTH   0
#define DIR_EAST    1
#define DIR_SOUTH   2
#define DIR_WEST    3
#define DIR_UP      4
#define DIR_DOWN    5

// Exit flags.Used in #ROOMS.
#define EX_ISDOOR       (A)
#define EX_CLOSED       (B)
#define EX_LOCKED       (C)
#define EX_PICKPROOF    (F)
#define EX_NOPASS       (G)
#define EX_EASY         (H)
#define EX_HARD         (I)
#define EX_INFURIATING  (J)
#define EX_NOCLOSE      (K)
#define EX_NOLOCK       (L)
#define EX_BASHPROOF    (M)
#define EX_DWARVESGUILD (N)

// Sector types.Used in #ROOMS.
#define SECT_INSIDE         0
#define SECT_CITY           1
#define SECT_FIELD          2
#define SECT_FOREST         3
#define SECT_HILLS          4
#define SECT_MOUNTAIN       5
#define SECT_WATER_SWIM     6
#define SECT_WATER_NOSWIM   7
#define SECT_UNUSED         8
#define SECT_AIR            9
#define SECT_DESERT        10
#define SECT_ROCK_MOUNTAIN 12
#define SECT_SNOW_MOUNTAIN 13
#define SECT_ENTER         14
#define SECT_ROAD          15
#define SECT_SWAMP         16
#define SECT_JUNGLE        17
#define SECT_RUINS         18
#define SECT_UWATER        19
#define SECT_MAX           20

// Equpiment wear locations.Used in #RESETS.
#define WEAR_NONE      -1
#define WEAR_LIGHT      0
#define WEAR_FINGER_L   1
#define WEAR_FINGER_R   2
#define WEAR_NECK       3
#define WEAR_BODY       5
#define WEAR_HEAD       6
#define WEAR_LEGS       7
#define WEAR_FEET       8
#define WEAR_HANDS      9
#define WEAR_ARMS      10
#define WEAR_SHIELD    11
#define WEAR_ABOUT     12
#define WEAR_WAIST     13
#define WEAR_WRIST_L   14
#define WEAR_WRIST_R   15
#define WEAR_RHAND     16
#define WEAR_HOLD      17
#define WEAR_FLOAT     18
#define WEAR_LHAND     19
#define MAX_WEAR       20
#define MAX_WEAR_L     18

// Conditions.
#define COND_DRUNK      0
#define COND_FULL       1
#define COND_THIRST     2
#define COND_HUNGER     3
#define COND_ADRENOLIN  4

// Positions.
#define POS_DEAD      0
#define POS_MORTAL    1
#define POS_INCAP     2
#define POS_STUNNED   3
#define POS_SLEEPING  4
#define POS_RESTING   5
#define POS_SITTING   6
#define POS_FIGHTING  7
#define POS_STANDING  8

// ACT bits for players.
#define PLR_IS_NPC     (A) // Don't EVER set.
#define PLR_TIPSY      (B)
#define PLR_AUTOASSIST (C) // Autoassist in battle
#define PLR_NOPOST     (D) // deny note, idea, etc posting
#define PLR_AUTOLOOT   (E)
#define PLR_AUTOSAC    (F)
#define PLR_AUTOGOLD   (G)
#define PLR_LASTREMORT (H)
#define PLR_BLINK      (I) // mage's blink
#define PLR_HIGHPRIEST (J)
#define PLR_QUESTOR    (M)
#define PLR_HOLYLIGHT  (N) // RT personal flags
#define PLR_NOSUMMON   (Q)
#define PLR_NOFOLLOW   (R) // 1 bit reserved, S
#define PLR_NOMLOVE    (S) // penalty
#define PLR_COLOUR     (T) // Colour Flag By Lope
#define PLR_PERMIT     (U) // penalty flags
#define PLR_5REMORT    (V)
#define PLR_LOG        (W)
#define PLR_DENY       (X)
#define PLR_FREEZE     (Y)
#define PLR_WANTED     (Z)
#define PLR_MUSTDRINK  (aa)
#define PLR_RAPER      (bb)
#define PLR_NOCANCEL   (cc)
#define PLR_NOSEND     (dd)
#define PLR_ARMY       (ee)
#define PLR_AUTOSPIT   (gg)
#define PLR_SIFILIS    (hh)
#define PLR_CAN_FLY    (ii)
#define PLR_DISAVOWED  (jj)

// RT comm flags -- may be used on both mobs and chars
#define COMM_QUIET      (A)
#define COMM_DEAF       (B)
#define COMM_NOEMOTE    (C)
#define COMM_NOCHANNELS (D)
#define COMM_NOGSOC     (E)
#define COMM_NODELETE   (F)
#define COMM_NOTELL     (G)
#define COMM_RUSSIAN    (H)
#define COMM_BRIEF      (I)
#define COMM_PROMPT     (J)
#define COMM_TELNET_GA  (K)
#define COMM_AFK        (L)
#define COMM_WILLSAVE   (M)
#define COMM_FLUSH      (N)
#define COMM_COMSYS     (gg)
#define COMM_CODER      (hh)

// Personal Configuration flags
#define CFG_ZRITEL     (A)
#define CFG_NODELETE   (B)
#define CFG_NOSNOOP    (C)
#define CFG_SHOWKILLS  (E)
#define CFG_AUTODAM    (F)
#define CFG_AUTOEXIT   (G)
#define CFG_AUTOTITLE  (H)
#define CFG_SHORTFLAG  (I)
#define CFG_GETEXP     (J)
#define CFG_GETALL     (K)
#define CFG_AUTORECAL  (L)
#define CFG_AUTOSPLIT  (M)
#define CFG_RUNSHOW    (N)
#define CFG_NOIMMS     (O)
#define CFG_AFFSHORT   (P)
#define CFG_TICK       (Q)
#define CFG_AUTOLOOK   (R)
#define CFG_AUTOVOTE   (S)
#define CFG_AUTONOTE   (T)
#define CFG_AUTOPLR    (U)
#define CFG_AUTOPEEK   (V)

//Defines for interpret.c
#define ML MAX_LEVEL       // implementor
#define L1 MAX_LEVEL - 1   // creator
#define L2 MAX_LEVEL - 2   // supreme being
#define L3 MAX_LEVEL - 3   // deity
#define L4 MAX_LEVEL - 4   // god
#define L5 MAX_LEVEL - 5   // immortal
#define L6 MAX_LEVEL - 6   // demigod
#define L7 MAX_LEVEL - 7   // angel
#define L8 MAX_LEVEL - 8   // avatar
#define IM LEVEL_IMMORTAL  // avatar
#define HE LEVEL_HERO      // hero

#define LOG_NEVER  0
#define COM_INGORE 1

// WIZnet flags
#define WIZ_ON        (A)
#define WIZ_TICKS     (B)
#define WIZ_LOGINS    (C)
#define WIZ_SITES     (D)
#define WIZ_LINKS     (E)
#define WIZ_DEATHS    (F)
#define WIZ_RESETS    (G)
#define WIZ_MOBDEATHS (H)
#define WIZ_FLAGS     (I)
#define WIZ_PENALT    (J)
#define WIZ_SACCING   (K)
#define WIZ_LEVELS    (L)
#define WIZ_SECURE    (M)
#define WIZ_SWITCH    (N)
#define WIZ_SNOOPS    (O)
#define WIZ_RESTORE   (P)
#define WIZ_LOAD      (Q)
#define WIZ_NEWBIE    (R)
#define WIZ_PREFIX    (S)
#define WIZ_SPAM      (T)
#define WIZ_ADDR      (U)
#define WIZ_SPEAKS    (V)
#define WIZ_SKILLS    (W)
#define WIZ_CONFIG    (X)
#define WIZ_NOTES     (Y)
#define WIZ_GSPEAKS   (Z)

// SPELLS and SKILLS FLAGS
#define C_NODUAL  (A) // Spell will not cast with dual wield
#define S_CLAN    (B) // Clanskill
#define S_DEITY   (C) // Spell\Skill will be allowed to devoted char

// Memory of mobiles
#define MEM_CUSTOMER (A)
#define MEM_SELLER   (B)
#define MEM_HOSTILE  (C)
#define MEM_AFRAID   (D)

// Liquids.
#define LIQ_WATER  1

// Types of attacks.
// Must be non-overlapping with spell/skill types,
// but may be arbitrary beyond that.
#define TYPE_UNDEFINED -1
#define TYPE_BACKSTAB  100
#define TYPE_MISSILE   101
#define TYPE_HIT       1000

#define TAR_IGNORE             0
#define TAR_CHAR_OFFENSIVE     1
#define TAR_CHAR_DEFENSIVE     2
#define TAR_CHAR_SELF          3
#define TAR_OBJ_INV            4
#define TAR_OBJ_CHAR_DEF       5
#define TAR_OBJ_CHAR_OFF       6
#define TAR_OBJ_HERE_CHAR_OFF  7
#define TAR_OBJ_HERE_CHAR_DEF  8
#define TAR_OBJ_HERE           9
#define TAR_IGNORE_OFFENSIVE  10
#define TAR_CHAR_ROOM         11
#define TAR_ARG               12

// Target who
#define TARGET_CHAR 0
#define TARGET_OBJ  1
#define TARGET_ROOM 2
#define TARGET_NONE 3
#define TARGET_ARG  4

// MOBprog definitions
#define TRIG_ACT    (A)
#define TRIG_BRIBE  (B)
#define TRIG_DEATH  (C)
#define TRIG_ENTRY  (D)
#define TRIG_FIGHT  (E)
#define TRIG_GIVE   (F)
#define TRIG_GREET  (G)
#define TRIG_GRALL  (H)
#define TRIG_KILL   (I)
#define TRIG_HPCNT  (J)
#define TRIG_RANDOM (K)
#define TRIG_SPEECH (L)
#define TRIG_EXIT   (M)
#define TRIG_EXALL  (N)
#define TRIG_DELAY  (O)
#define TRIG_SURR   (P)

// Object defined in limbo.are Used in save.c to load objects that don't exist.
#define OBJ_VNUM_DUMMY  30

// Area flags.
#define AREA_NONE      0
#define AREA_CHANGED  (A)  // has been modified
#define AREA_ADDED    (B)  // has been added to
#define AREA_LOADING  (C)  // Used for counting in db.c
#define AREA_NOQUEST  (D)  // clan,army & other noquest areas
#define AREA_NOREFORM (E)  // can't reform in area
#define AREA_WIZLOCK  (F)  // area wizlocked
#define AREA_LAW      (G)  // area LAW (aggres will not attack)
#define AREA_SAVELOCK (H)  // area will not saved by asave command

#define NO_FLAG -5 // Must not be used in flags or stats.

// GUILDS of FD
#define DWARVES_GUILD (A)
#define ASSASIN_GUILD (B)
#define VAMPIRE_GUILD (C)
#define DRUIDS_GUILD  (D)

// PENALTY DATA
#define PENALTY_PERMANENT 0
#define PENALTY_TICK      1
#define PENALTY_DATE      2

// Penalty types
#define P_NOCHANNELS 1
#define P_NOMLOVE    3
#define P_NOPOST     4
#define P_GODCURSE   5
#define P_NOGSOCIAL  6
#define P_FREEZE     8
#define P_NOEMOTE    9
#define P_NOTELL    10
#define P_TIPSY     11
#define P_NODELETE  12
#define P_NOSTALGIA 13

// Materials defines
#define MATERIAL_NONE 0

// Objects defines
#define OBJECT_BROKEN 0
#define OBJECT_ETERNAL 9
#define SHOW_COND_ETERNAL item_cond_table[OBJECT_ETERNAL].showcon
#define DAM_NORMAL 0
#define DAM_CLEAVE 1
#define DAM_BREATH 2

// ROOM AFFECTS
#define RAFF_BLIND    (A) // all chars in room can't see
#define RAFF_NOWHERE  (B) // where command do not show name of this room
#define RAFF_HIDE     (C) // mortals can't see this room
#define RAFF_WEB      (D) // mortals can't leave this room by land
#define RAFF_ALL_VIS  (E) // faerie fog
#define RAFF_OASIS    (F) // increase heal_rate
#define RAFF_MIND_CH  (G) // increate mana_rate
#define RAFF_EVIL_PR  (H) // disables magic transportation from room
#define RAFF_LIFE_STR (I) // removes evil_presence from room or allows recall
#define RAFF_SAFE_PLC (J) // makes room safe
#define RAFF_VIOLENCE (K) // removes safe from room

// ROOM AFFECTS FLAGS
#define RAF_STUN        (A) // stuns character for some time (waitstate)
#define RAF_ALARM       (B) // just alerts trap owner
#define RAF_REMFLY      (C) // removes FLY affect from char
#define RAF_REMINVIS    (D) // removes INVIS affect from char
#define RAF_FEAR        (E) // adds FEAR affect
#define RAF_CURSE       (F) // adds CURSE affect
#define RAF_BLIND       (G) // blinds target
#define RAF_TARGET_ANY  (H) // trap affects any char
#define RAF_TARGET_NPC  (I) // trap affects NPCs
#define RAF_TARGET_PC   (J) // trap affects PCs
#define RAF_TARGET_FLY  (K) // trap affects flying targets
#define RAF_TARGET_WALK (L) // trap affects walking(nonflying) targets

// ch->start_pos for PC
// HighMagic spells
#define CASTING_WEB (A)

// BITs defines for magic groups
#define SPELL     (A)
#define SKILL     (B)
#define FIRE      (C)
#define AIR       (D)
#define WATER     (E)
#define EARTH     (F)
#define SPIRIT    (G)
#define MIND      (H)
#define FORTITUDE (I)
#define CURATIVE  (J)
#define LIGHT     (K)
#define DARK      (L)
#define LEARN     (M)
#define PERCEP    (N)
#define MAKE      (O)
#define PROTECT   (P)
#define OFFENCE   (Q)

// Socials flags:
#define SOC_NOSPAM   (A) // do not check for spam
#define SOC_NOGLOBAL (B) // can't used in global mode
#define SOC_COMMON   (C)
#define SOC_CULTURAL (D) // get's movements
#define SOC_BAD      (E)
#define SOC_LOVING   (F)
#define SOC_GOOD     (G)
#define SOC_NOMOB    (H) // mobs can't use this socials

// Newspaper data
#define NEWS_MURDER  1
#define NEWS_DEATH   2
#define NEWS_VICTORY 3
#define NEWS_OTHER   4
#define NEWS_REMORT  5
#define NEWS_MARRY   6
#define NEWS_ALL     7
#define NEWS_FACTS   8
#define NEWS_GQUEST  9
#define NEWS_REWARD  0

// Chars - for char table
#define CHAR_CONTROL 1
#define CHAR_SYMBOL  2
#define CHAR_DIGIT   3
#define CHAR_LETTER  4

// Channels Bit Flags
#define CBIT_YELL     (A)
#define CBIT_AUCTION  (B)
#define CBIT_GOSSIP   (C)
#define CBIT_SHOUT    (D)
#define CBIT_QUESTION (E)
#define CBIT_ANSWER   (F)
#define CBIT_MUSIC    (G)
#define CBIT_CLAN     (H)
#define CBIT_QUOTE    (I)
#define CBIT_CENSORED (J)
#define CBIT_GRAT     (K)
#define CBIT_CHAT     (L)
#define CBIT_NEWBIE   (M)
#define CBIT_ALLI     (N)
#define CBIT_KAZAD    (O)
#define CBIT_PTALK    (P)
#define CBIT_AVENGE   (Q)
#define CBIT_GTELL    (R)
#define CBIT_IMMTALK  (S)
#define CBIT_INFO     (T)
#define CBIT_GSOCIAL  (U)
#define CBIT_EMOTE    (V)
#define CBIT_FD       (W)
                         
// Channels numbers
#define CHAN_YELL     0
#define CHAN_AUCTION  1
#define CHAN_GOSSIP   2
#define CHAN_SHOUT    3
#define CHAN_QUESTION 4
#define CHAN_ANSWER   5
#define CHAN_MUSIC    6
#define CHAN_CLAN     7
#define CHAN_QUOTE    8
#define CHAN_CENSORED 9 
#define CHAN_GRAT     10
#define CHAN_CHAT     11
#define CHAN_NEWBIE   12
#define CHAN_ALLI     13
#define CHAN_KAZAD    14
#define CHAN_PTALK    15
#define CHAN_AVENGE   16
#define CHAN_GTELL    17
#define CHAN_IMMTALK  18
#define CHAN_INFO     19
#define CHAN_GSOCIAL  20
#define CHAN_EMOTE    21
#define CHAN_FD       22

// Full Class
#define C_MAG         (A)  // M
#define C_CLE         (B)  // C
#define C_THI         (C)  // T
#define C_WAR         (D)  // W
#define C_WIZARD      (E)  // MC
#define C_NIGHTBLADE  (F)  // MT
#define C_BATTLEMAGE  (G)  // MW
#define C_HERETIC     (H)  // CT
#define C_PALADIN     (I)  // WC
#define C_NINJA       (J)  // WT
#define C_BARD        (K)  // MCT
#define C_MONK        (L)  // MCW
#define C_SPELLSWORD  (M)  // MTW
#define C_TEMPLAR     (N)  // CTW
#define C_BODHISATVA  (O)  // MCTW

// Qguest defines
#define GQ_JOINED     1
#define GQ_STARTED    1
#define GQ_STARTING   2

// defines for can_see function
#define CHECK_LVL    1
#define NOCHECK_LVL  0

// Quest types
#define Q_KILL_MOB   (A)
#define Q_FIND_ITEM  (B)
#define Q_MOB_KILLED (C)
#define Q_BRING_ITEM (D)

// Quest Objects
#define QUEST_OBJQUEST1 26
#define QUEST_OBJQUEST2 120
#define QUEST_OBJQUEST3 121
#define QUEST_OBJQUEST4 122
#define QUEST_OBJQUEST5 123

// Macros Section
#define SabAdron         number_range(1,10)>5?"Adron":"Saboteur"

// Managment macros
#define IS_VALID(data)   ((data) != NULL && (data)->valid)
#define VALIDATE(data)   ((data)->valid = TRUE)
#define INVALIDATE(data) ((data)->valid = FALSE)
#define ptc printf_to_char
#define NOPUBLIC  IS_SET(global_cfg,CFG_PUBLIC)
#define PULSE_AUCTION    (20 * PULSE_PER_SECOND) /* 10 seconds */

// System macros
#define IS_LETTER(c) (char_table[(unsigned char)(c)]==CHAR_LETTER)
#define IS_SYMBOL(c) (char_table[(unsigned char)(c)]==CHAR_SYMBOL)
#define IS_DIGIT(c)  (char_table[(unsigned char)(c)]==CHAR_DIGIT)

// Arifmethic and string macros
#define EMPTY(arg)        ((arg)[0]=='\0')
#define UMIN(a, b)        ((a) < (b) ? (a) : (b))
#define UMAX(a, b)        ((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)   ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)          ((c)>='A' && (c)<='Z'? (c)+32 : (unsigned)(c)>='А' && (unsigned)(c)<='Я' ? (c)+32 : (c))
#define UPPER(c)          ((c)>='a' && (c)<='z'? (c)-32 : (unsigned)(c)>='а' && (unsigned)(c)<='я' ? (c)-32 : (c))
#define IS_SET(flag, bit) ((flag) & (bit))
#define SET_BIT(var, bit) ((var) |= (bit))
#define REM_BIT(var, bit) ((var) &= ~(bit))

// Deity macros
#define MAX_DEITY_APP 20
#define MAX_DEITIES 25
#define dcp  deity_char_power
#define gvd  get_vacant_deity
#define IS_DEVOTED_ANY(ch)       (!IS_NPC(ch) && (ch->pcdata->dn < gvd() ) )
#define IS_DEVOTED( ch, ndeity)  (ch->pcdata->dn < NULL && !str_cmp(ch->deity, deity_table[ndeity].name) )
#define MIDDLE_FAVOUR 5
#define IS_EVIL_DEITY(ch)        (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==2 || ch->pcdata->dn==3 || ch->pcdata->dn==5  || ch->pcdata->dn==14))
#define IS_GOOD_DEITY(ch)        (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==0 || ch->pcdata->dn==4 || ch->pcdata->dn==10))
#define IS_NEUTRAL_DEITY(ch)     (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==1 || ch->pcdata->dn==7 || ch->pcdata->dn==11 || ch->pcdata->dn==13))
#define IS_MAGIC_DEITY(ch)       (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==0 || ch->pcdata->dn==1))
#define IS_DARKMAGIC_DEITY(ch)   (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==2 || ch->pcdata->dn==12 || ch->pcdata->dn==14))
#define IS_LIGHTMAGIC_DEITY(ch)  (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==0 || ch->pcdata->dn==10))
#define IS_MIGHT_DEITY(ch)       (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==3 || ch->pcdata->dn==7 || ch->pcdata->dn==8 || ch->pcdata->dn==11 || ch->pcdata->dn==12 ))
#define IS_PROTECT_DEITY(ch)     (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==1 || ch->pcdata->dn==7 || ch->pcdata->dn==11))
#define IS_ENCHANT_DEITY(ch)     (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==4 || ch->pcdata->dn==11))
#define IS_NATURE_DEITY(ch)      (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==4 || ch->pcdata->dn==10))
#define IS_AIR_DEITY(ch)         (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==15))
#define IS_WATER_DEITY(ch)       (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==16))
#define IS_FIRE_DEITY(ch)        (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==6 || ch->pcdata->dn==12))
#define IS_EARTH_DEITY(ch)       (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==5 || ch->pcdata->dn==11))
#define IS_STEALTH_DEITY(ch)     (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==1 || ch->pcdata->dn==5 || ch->pcdata->dn==13))
#define IS_MIGHTMAGIC_DEITY(ch)  (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==2 || ch->pcdata->dn==5 || ch->pcdata->dn==12))
#define IS_INTELLIGENT_DEITY(ch) (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==0 || ch->pcdata->dn==4 || ch->pcdata->dn==9 ))
#define IS_FANATIC_DEITY(ch)     (IS_DEVOTED_ANY(ch) && (ch->pcdata->dn==3 || ch->pcdata->dn==6  || ch->pcdata->dn==8 || ch->pcdata->dn==12))
#define DEITY_SPELL(sn)          IS_SET(skill_table[sn].flag,S_DEITY)

// Character macros
#define WILLSAVE(ch)         SET_BIT((ch)->comm, COMM_WILLSAVE);
#define IS_NPC(ch)           (IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)      (get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_ELDER(ch)         (!IS_NPC(ch) && is_exact_name(ch->name,"Saboteur Magica Astellar Dragon"))
#define IS_CREATOR(ch)       (!IS_NPC(ch) && get_trust(ch) >= 109 )
#define IS_DEITY(ch)         (!IS_NPC(ch) && get_trust(ch) >= 107)
#define IS_HERO(ch)          (get_trust(ch) >= LEVEL_HERO)
#define IS_CODER(ch)         (!IS_NPC(ch) && IS_SET((ch)->comm,COMM_CODER))
#define IS_TRUSTED(ch,level) (get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)  (IS_SET((ch)->affected_by, (sn)))
#define IS_CFG(ch,bit)       (!IS_NPC(ch) && IS_SET((ch)->pcdata->cfg,bit))
#define GUILD(ch,bit)        (!IS_NPC(ch) && IS_SET((ch)->pcdata->guild,bit))
#define ELDER(ch,bit)        (!IS_NPC(ch) && IS_SET((ch)->pcdata->elder,bit))
#define IS_MARRY(ch,victim)  (!IS_NPC(ch) && !IS_NPC(victim) && !EMPTY((ch)->pcdata->marry) && !str_cmp((ch)->pcdata->marry,victim->name))
#define IS_LOVER(ch,victim)  (!IS_NPC(ch) && !IS_NPC(victim) && is_exact_name(victim->name,(ch)->pcdata->lovers))
#define IS_CLAN_ROOM(room)   ((room) && str_cmp((room)->area->clan,"none"))
#define IS_AWAKE(ch)         (ch->position > POS_SLEEPING)
#define WAIT_STATE(ch, npulse)  ((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define QSTAT(ch, type) IS_SET((ch)->q_stat,(type))

// Get some info macros
#define GET_AGE(ch)     ((int) (17 + ((ch)->played + current_time - (ch)->logon )/72000))
#define IS_QUESTOR(ch)  (IS_SET((ch)->act, PLR_QUESTOR))
#define GET_AC(ch,type) ((ch)->armor[type] + ( IS_AWAKE(ch) ? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))  
#define GET_HITROLL(ch) ((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) ((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)
#define get_carry_weight(ch) ((ch)->carry_weight + (ch)->silver/10 + (ch)->gold * 2 / 5)

#define IS_GOOD(ch)     (ch->alignment >= 350)
#define IS_EVIL(ch)     (ch->alignment <= -350)
#define IS_NEUTRAL(ch)  (!IS_GOOD(ch) && !IS_EVIL(ch))
#define IS_OUTSIDE(ch)  (!IS_SET((ch)->in_room->room_flags,ROOM_INDOORS))
#define IS_STATUE(ch)   (IS_NPC(ch) && ch->pIndexData->vnum>23079 && ch->pIndexData->vnum<23098)

#define CAN_WEAR(obj, part)     (IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)  (IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define IS_EAR(obj)      ((obj)->pIndexData == get_obj_index (ITEM_EAR))
#define WEIGHT_MULT(obj) ((obj)->item_type == ITEM_CONTAINER ? (obj)->value[4] : 100)

// Skill macros
#define CLEVEL_OK(ch,skill)  (ch->level >= skill_table[skill].skill_level[ch->class[ch->remort]] )

#define act(format,ch,arg1,arg2,type) act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING)
#define doact(format,ch,arg1,arg2,type,spam) do_act((format),(ch),(arg1),(arg2),(type),POS_RESTING,(spam))
#define HAS_TRIGGER(ch,trig) (IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define PERS(ch, looker)     (can_see( looker, (ch),CHECK_LVL ) ? get_char_desc (ch, '1') : "некто" )

// Clan and Guilds macros
#define ISORDEN(char)                                        \
   if ((char)->clan && !str_cmp((char)->clan->name,"orden")) \
   {                                             \
     stc("Это запрещено {WОРДЕНОМ{x!\n\r",char); \
     return;                                     \
   }

#define IS_ORDEN(ch) ((ch)->clan && !str_cmp((ch)->clan->name,"orden"))
#define BAD_ORDEN(ch) ((ch)->clan && !str_cmp((ch)->clan->name,"orden") && (ch)->alignment <750)
#define BAD_VAMP(ch) (GUILD((ch),VAMPIRE_GUILD) && (ch)->alignment > -750)
#define CLANSPELL(sn) IS_SET(skill_table[sn].flag,S_CLAN)
#define IS_GUARD(ch) ((ch)->clan && !str_cmp((ch)->clan->name,"guards"))

// Mobprog Code defines
#define MPEDIT( fun )           bool fun(CHAR_DATA *ch, const char*argument) 
#define EDIT_MPCODE(Ch, Code)   ( Code = (MPROG_CODE*)Ch->desc->pEdit )

#endif  // LEFT FROM THE TOP !
