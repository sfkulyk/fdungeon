// $Id: db.c,v 1.85 2004/02/17 13:02:13 wagner Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <stdio.h>

// comment out to turn "magic number" debugging of memory allocation/recycling off
#define MAGIC_CHECKING


#ifdef MAGIC_CHECKING

#define MAGIC(x) ((struct magic*)x)
#define MAGIC_COOKIE 0x1118

/* MCB. Sizeof is still == 4 */
struct magic {
        unsigned short cookie;  /* This is for identification. Should be MAGIC_COOKIE */
        unsigned short size;    /* Allocation size (without this struct)*/
        char data[0];           /* Beginning of a real data */
};
#endif


// #define OLD_RAND
// comment out to use the CRTL pseudorandom number generator

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#if defined(WIN32)
#include <sys/types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"

#ifdef WITH_UNICORN
#include "unicorn.h"
#ifdef WITH_ANTICRASH
#include <signal.h>
#include <unistd.h>
#endif
#endif

extern  int     _filbuf         args( (FILE *) );

#if !defined(OLD_RAND)
#ifdef  WIN32 
#define random() rand()
#define srandom( x ) srand( x )
#else
long random();
void srandom(unsigned int);
#endif
int getpid();
time_t time(time_t *tloc);
#endif

// externals for counting purposes
extern OBJ_DATA        *obj_free;
extern CHAR_DATA       *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern PC_DATA         *pcdata_free;
extern AFFECT_DATA     *affect_free;
extern GQUEST_DATA     *gquest_data;
void fix_keepers(MOB_INDEX_DATA *mIndex);
void load_config();
void load_races();
void load_vote();
void load_newspaper(void);

// Globals
HELP_DATA *       help_first;
HELP_DATA *       help_last;
SHOP_DATA *       shop_first;
SHOP_DATA *       shop_last;
NOTE_DATA *       note_free;
NEWS_DATA *       news_free;
VOTE_DATA *       vote_free;

// For debugging and counting memory leaks
int cnt_char;  
int cnt_pcdata;
int cnt_descr; 
int cnt_obj;   
int cnt_aff;   
int cnt_raff;  
int cnt_buff;  

char              bug_buf         [2*MAX_STRING_LENGTH];
CHAR_DATA *       char_list;
VOTE_DATA *       vote_list;
OFFER_DATA *      offer_list;
RAFFECT *         raffect_list=NULL;
const char *            help_greeting;
char              log_buf         [2*MAX_STRING_LENGTH];
KILL_DATA         kill_table      [MAX_LEVEL];
NEWS_DATA *       newspaper_list;
OBJ_DATA *        object_list;
TIME_INFO_DATA    time_info;
WEATHER_DATA      weather_info;
MPROG_CODE *      mprog_list;
GQUEST_DATA       gquest;

int    gsn_quenia;
int    gsn_backstab;
int    gsn_dodge;
int    gsn_blink;
int    gsn_envenom;
int    gsn_hide;
int    gsn_peek;
int    gsn_pick_lock;
int    gsn_sneak;
int    gsn_steal;
int    gsn_strangle;

int    gsn_cleave;
int    gsn_disarm;
int    gsn_enhanced_damage;
int    gsn_kick;
int    gsn_parry;
int    gsn_dual;
int    gsn_dual_sec_attack;
int    gsn_rescue;
int    gsn_second_attack;
int    gsn_third_attack;
int    gsn_shock_hit;

int    gsn_blindness;
int    gsn_charm_person;
int    gsn_curse;
int    gsn_invis;
int    gsn_mass_invis;
int    gsn_poison;
int    gsn_plague;
int    gsn_sleep;
int    gsn_gaseous_form;

int    gsn_axe;
int    gsn_dagger;
int    gsn_flail;
int    gsn_mace;
int    gsn_polearm;
int    gsn_shield_block;
int    gsn_spear;
int    gsn_staff;
int    gsn_sword;
int    gsn_whip;
int    gsn_missile;
int    gsn_bash;
int    gsn_crush;
int    gsn_frame;
int    gsn_berserk;
int    gsn_dirt;
int    gsn_hand_to_hand;
int    gsn_trip;
int    gsn_fast_healing;
int    gsn_haggle;
int    gsn_lore;
int    gsn_meditation;
int    gsn_scrolls;
int    gsn_staves;
int    gsn_wands;
int    gsn_recall;
int    gsn_pray;
int    gsn_lash;
int    gsn_charge;

MOB_INDEX_DATA *  mob_index_hash  [MAX_KEY_HASH];
OBJ_INDEX_DATA *  obj_index_hash  [MAX_KEY_HASH];
ROOM_INDEX_DATA * room_index_hash [MAX_KEY_HASH];
char *            string_hash     [MAX_KEY_HASH];

AREA_DATA * area_first;
AREA_DATA * area_last;
char *  string_space;
char *  top_string;
char    str_empty       [1];

int   top_affect;
int   top_area;
int   top_ed;
int   top_exit;
int   top_help;
int64   top_mob_index;
int64   top_obj_index;
int   top_reset;
int64   top_room;
int   top_shop;
int64   top_vnum_room;
int64   top_vnum_mob;
int64   top_vnum_obj;
int   mobile_count = 0;
int   newmobs = 0;
int   newobjs = 0;
int64   top_mprog_index;

void *     rgFreeList      [MAX_MEM_LIST];
const int  rgSizeList      [MAX_MEM_LIST]  =
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};

int nAllocString;
int sAllocString;
int nAllocPerm;
int sAllocPerm;

bool    fBootDb;
FILE *  fpArea;
char    strArea[MAX_STRING_LENGTH];

void  init_mm         args( ( void ) );
void  load_area       args( ( FILE *fp ) );
void  load_helps      args( ( FILE *fp ) );
void  load_mobiles    args( ( FILE *fp ) );
void  load_objects    args( ( FILE *fp ) );
void  load_resets     args( ( FILE *fp ) );
void  load_rooms      args( ( FILE *fp ) );
void  load_shops      args( ( FILE *fp ) );
void  load_specials   args( ( FILE *fp ) );
void  load_notes      args( ( void ) );
void  load_bans       args( ( void ) );
void  fix_exits       args( ( void ) );
void  fix_weapons     args( ( void ) );
void  reset_area      args( ( AREA_DATA * pArea ) );
void  load_mobprogs   args( ( FILE *fp ) );
void  fix_mobprogs    args( ( void ) );
void  mpedit          args( ( CHAR_DATA *ch, char *argument ) );
int   is_random_obj_in_room( ROOM_INDEX_DATA *pRoom );
void  reset_random_items( AREA_DATA *pArea, int nroom );
int   get_random_pierce_message();
int   get_random_slash_message ();
int   get_random_bash_message  ();
int   get_random_exotic_message();

random_armor random_armors[]=
{
 {"random armor", 3703 ,"Доспех "},
 {"random shield", 3704 ,"Щит "},
 {"random amulet", 3705 ,"Амулет "},
 {"random helm", 3706 ,"Шлем "},
 {"random leggins", 3707 ,"Поножи "},
 {"random boots", 3708 ,"Ботинки "},
 {"random gloves", 3709 ,"Перчатки "},
 {"random sleeves", 3710 ,"Рукава "},
 {"random cape", 3711 ,"Плащ "},
 {"random belt", 3712 ,"Ремень "},
 {"random bracer", 3713 ,"Браслет "},
 {"", 0 ,""}
};

void boot_db( void )
{
  {
    if ( ( string_space = calloc( 1, MAX_STRING ) ) == NULL )
    {
      bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
      exit( 1 );
    }
    top_string      = string_space;
    fBootDb         = TRUE;
  }

  // Init random number generator.
  {
    init_mm( );
  }

  log_printf("-= Loading Forgotten Dungeon =-");
  {
    long lhour, lday, lmonth;

    lhour           = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
    time_info.hour  = lhour  % 24;
    lday            = lhour  / 24;
    time_info.day   = lday   % 35;
    lmonth          = lday   / 35;
    time_info.month = lmonth % 17;
    time_info.year  = lmonth / 17;

         if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
    else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
    else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
    else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
    else                            weather_info.sunlight = SUN_DARK;

    weather_info.change     = 0;
    weather_info.mmhg       = 960;
    if ( time_info.month >= 7 && time_info.month <=12 )
         weather_info.mmhg += number_range( 1, 50 );
    else weather_info.mmhg += number_range( 1, 80 );

         if ( weather_info.mmhg <= 980 ) weather_info.sky = SKY_LIGHTNING;
    else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
    else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
    else                                  weather_info.sky = SKY_CLOUDLESS;
  }

  auction = (AUCTION_DATA *) malloc (sizeof(AUCTION_DATA));
  if (auction == NULL)
  {
    bug ("malloc'ing AUCTION_DATA didn't give %d bytes",sizeof(AUCTION_DATA));
    exit (1);
  }
                
  auction->item = NULL; /* nothing is being sold */

  //  prepare max_string variable
  {
    int i;
    for (i=0;skill_table[i].name;i++);
    max_skill=i;
  }

  // Assign gsn's for skills which have them.
  {
    int sn;

    for ( sn = 0; sn < max_skill; sn++ )
    {
      if ( skill_table[sn].pgsn != NULL ) *skill_table[sn].pgsn = sn;
    }
  }

  // Autologin feature
  autologin_code=0;
  autologin_pass=0;

  load_config();

  // shotdown mud if crashes very often. (C) Saboteur
  reboot_counter+=REBOOTCOUNTER_STEP;
  if (reboot_counter>=REBOOTCOUNTER_MAX)
  {
    ID_FILE="shutdown.txt";
    stf("Shutdown by crash overlock!!!",NULL);
    reboot_counter=0;
  }
  save_config();
  load_races();
  load_vote();
  load_newspaper();
// LOADING_DEITIES !!!
  load_deities();
  load_clans();
  // Loading areas
  {
    FILE *fpList;

    if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
    {
      perror( AREA_LIST );
      exit( 1 );
    }
    for (;;)
    {
      strcpy(strArea, fread_word( fpList ));
      if ( strArea[0] == '$' ) break;
      if ( strArea[0] == '-' ) fpArea = stdin;
      else
      {
        if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
        {
          log_printf("Failed to load %s", strArea);
          continue;
        }
        else log_printf("Loading area %s...", strArea);
      }
      for ( ; ; )
      {
        const char *word;

        if ( fread_letter( fpArea ) != '#' )
        {
          bug( "Boot_db: # not found.", 0 );
          exit( 1 );
        }

        word = fread_word( fpArea );
             if ( word[0] == '$'               )                 break;
        else if ( !str_cmp( word, "AREADATA" ) ) load_area    (fpArea);
        else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea);
        else if ( !str_cmp( word, "MOBILES"  ) ) load_mobiles (fpArea);
        else if ( !str_cmp( word, "OBJECTS"  ) ) load_objects (fpArea);
        else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
        else if ( !str_cmp( word, "ROOMS"    ) ) load_rooms   (fpArea);
        else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
        else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
        else if ( !str_cmp( word, "MOBPROGS" ) ) load_mobprogs(fpArea);
        else
        {
          log_printf("Boot_db: bad area_section name '%s'", word);
          exit( 1 );
        }
      }
      if ( fpArea != stdin ) fclose( fpArea );
      fpArea = NULL;
    }
    fclose( fpList );
  }
  {
    int i;
    log_string("Checks loaded data...");
    fix_exits( );
    log_string("Exits fixed");
    fix_mobprogs( );
    log_string("Mobprogs fixed");
    fBootDb = FALSE;
    fBootDb = TRUE;
    fix_weapons( );
    log_string("Weapons fixed");
    fBootDb = FALSE;
    convert_objects( );
    area_update( );
    log_string("Updating areas...");
    load_notes( );
    load_bans();
    load_social_table();
    load_songs();
    for (i=0;quenia_table[i].start_counter!=255;i++)
    {
      quenia_table[i].word[0]='\0';
      strcat(quenia_table[i].word,create_word());
      quenia_table[i].counter=quenia_table[i].start_counter;
    }
    log_string("Quenia language initialized");
  }
}

#define nelems(a) (sizeof (a)/sizeof (a)[0])

// Calculate a meaningful modifier and amount
void random_apply( OBJ_DATA *obj, CHAR_DATA *mob )
{
  static int attrib_types[] = { APPLY_STR, APPLY_DEX, APPLY_DEX, APPLY_INT,
      APPLY_INT, APPLY_WIS, APPLY_CON, APPLY_CON, APPLY_CON };

  static int power_types[] = { APPLY_MANA, APPLY_HIT, APPLY_MOVE, APPLY_AC };

  static int combat_types[] = { APPLY_HITROLL, APPLY_HITROLL, APPLY_DAMROLL,
      APPLY_SAVING_SPELL, APPLY_SAVING_SPELL, APPLY_DAMROLL };

  AFFECT_DATA *paf = alloc_perm( sizeof(*paf) );
  paf->type         = -1;
  paf->duration  = -1;
  paf->bitvector = 0;
  paf->next         = obj->affected;
  obj->affected  = paf;
  switch (number_bits(2))
  {
  case 0:
    paf->location  = attrib_types[number_range(0, nelems(attrib_types)-1)];
    paf->modifier  = 1;
    break;
  case 1:
    paf->location  = power_types[number_range(0, nelems(power_types)-1)];
    paf->modifier  = number_range(mob->level/2, mob->level);
    break;
  case 2:
  case 3:
    paf->location  = combat_types[number_range(0, nelems(combat_types)-1)];
    paf->modifier  = number_range(1, mob->level/6+1);
    break;
  }

  SET_BIT(obj->extra_flags, ITEM_MAGIC);
  // Is item cursed?
  if (number_percent() <= 5)
    {
      paf->modifier = -paf->modifier;
      SET_BIT(obj->extra_flags, ITEM_NODROP);
      if (number_percent() <= 45) SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
    }
}

// Jewelry stuff
char  const *cond1[] = 
  { "сверкающ|ий|его|ему|ий|им|ем|их|их",                       
    "древн|ий|его|ему|ий|им|ем|их|их", 
    "дымнчат|ый|ого|ому|ый|ым|ом|ых|ых",
    "темн|ый|ого|ому|ый|ым|ом|ых|ых",
    "переливающ|ийся|егося|емуся|ийся|имся|емся|ихся|ихся", 
    "пылающ|ий|его|ему|ий|им|ем|их|их", 
    "искрящ|ийся|егося|емуся|ийся|имся|емся|ихся|ихся", 
    "светящ|ийся|егося|емуся|ийся|имся|емся|ихся|ихся", 
    "светл|ый|ого|ому|ый|ым|ом|ых|ых", 
    "гудящ|ий|его|ему|ий|им|ем|их|их",
    "мерцающ|ий|его|ему|ий|им|ем|их|их" };

char  const *cond2[] =
  { "бурлящ|ий|его|ему|ий|им|ем|их|их", 
    "зловонн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "мутн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "кипящ|ий|его|ему|ий|им|ем|их|их",                     
    "пузырящ|ийся|егося|емуся|ийся|имся|емся|ихся|ихся", 
    "прозрачн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "кристально чист|ый|ого|ому|ый|ым|ом|ых|ых", 
    "густ|ой|ого|ому|ой|ым|ом|ых|ых", 
    "гудящ|ий|его|ему|ий|им|ем|их|их"};

char  const *cond3[] =
  { "древн|ий|его|ему|ий|им|ем|их|их",
    "таинственн|ый|ого|ому|ый|ым|ом|ых|ых",
    "загадочн|ый|ого|ому|ый|ым|ом|ых|ых",
    "неразборчив|ый|ого|ому|ый|ым|ом|ых|ых",
    "помят|ый|ого|ому|ый|ым|ом|ых|ых",
    "свернут|ый|ого|ому|ый|ым|ом|ых|ых в трубочку",
    "пульсирующ|ий|его|ему|ий|им|ем|их|их",
    "холодн|ый|ого|ому|ый|ым|ом|ых|ых наощупь",
    "горяч|ий|его|ему|ий|им|ем|их|их наощупь",
    "подпаленн|ый|ого|ому|ый|ым|ом|ых|ых",
    "непонятн|ый|ого|ому|ый|ым|ом|ых|ых"};

char  const *cond4[] =
  { "погнут|ый|ого|ому|ый|ым|ом|ых|ых",
    "покореженн|ый|ого|ому|ый|ым|ом|ых|ых",
    "обожженн|ый|ого|ому|ый|ым|ом|ых|ых",
    "перемолот|ый|ого|ому|ый|ым|ом|ых|ых",
    "полусгнивш|ий|ого|ому|ий|им|ем|их|их",
    "мягкий|ий|его|ему|ий|им|ем|их|их", 
    "тверд|ый|ого|ому|ый|ым|ом|ых|ых"};

static char *cond5[] =
  { "остр|ый|ого|ому|ый|ым|ом|ых|ых ", 
    "гладк|ый|ого|ому|ый|ым|ом|ых|ых ",  
    "стар|ый|ого|ому|ый|ым|ом|ых|ых ",
    "ржав|ый|ого|ому|ый|ым|ом|ых|ых ", 
    "нов|ый|ого|ому|ый|ым|ом|ых|ых ", 
    "крив|ой|ого|ому|ой|ым|ом|ых|ых ",
    "",""};

static char *material1[] = 
  { "золот|ой|ого|ому|ой|ым|ом|ых|ых",  
    "топазов|ый|ого|ому|ый|ым|ом|ых|ых", 
    "рубинов|ый|ого|ому|ый|ым|ом|ых|ых", 
    "платинов|ый|ого|ому|ый|ым|ом|ых|ых", 
    "ониксов|ый|ого|ому|ый|ым|ом|ых|ых", 
    "стеклянн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "мифрилов|ый|ого|ому|ый|ым|ом|ых|ых", 
    "серебрянн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "гранитн|ый|ого|ому|ый|ым|ом|ых|ых",
    "бронзов|ый|ого|ому|ый|ым|ом|ых|ых",
    "медн|ый|ого|ому|ый|ым|ом|ых|ых",
    "бриллиантов|ый|ого|ому|ый|ым|ом|ых|ых",
    "изумрудн|ый|ого|ому|ый|ым|ом|ых|ых",
    "опалов|ый|ого|ому|ый|ым|ом|ых|ых"};

static char *material2[] =
  { "стальн|ой|ого|ому|ой|ым|ом|ых|ых",  
    "бронзов|ый|ого|ому|ый|ым|ом|ых|ых", 
    "медн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "железн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "деревянн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "гранитн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "металлическ|ий|его|ему|ий|им|ем|их|их", 
    "серебрян|ый|ого|ому|ый|ым|ом|ых|ых",
    "золот|ой|ого|ому|ой|ым|ом|ых|ых"};


static char *material3[] =
  { "кожанн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "матерчат|ый|ого|ому|ый|ым|ом|ых|ых", 
    "войлочн|ый|ого|ому|ый|ым|ом|ых|ых",
    "суконн|ый|ого|ому|ый|ым|ом|ых|ых",
    "мехов|ой|ого|ому|ой|ым|ом|ых|ых",
    "подбит|ый|ого|ому|ый|ым|ом|ых|ых мехом"};

static char *material4[] =
  { "пергаментн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "кожанн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "папирусн|ый|ого|ому|ый|ым|ом|ых|ых",
    "атласн|ый|ого|ому|ый|ым|ом|ых|ых",
    "бумажн|ый|ого|ому|ый|ым|ом|ых|ых"};

static char *material5[] =
  { "красн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "коричнев|ый|ого|ому|ый|ым|ом|ых|ых", 
    "зелен|ый|ого|ому|ый|ым|ом|ых|ых",
    "оранжев|ый|ого|ому|ый|ым|ом|ых|ых",
    "фиолетов|ый|ого|ому|ый|ым|ом|ых|ых",
    "бесцветн|ый|ого|ому|ый|ым|ом|ых|ых",
    "бел|ый|ого|ому|ый|ым|ом|ых|ых",
    "черн|ый|ого|ому|ый|ым|ом|ых|ых"};


static char *material6[] =
  { "кожанн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "укрепленн|ый|ого|ому|ый|ым|ом|ых|ых кожанн|ый|ого|ому|ый|ым|ом|ых|ых",
    "бронзов|ый|ого|ому|ый|ым|ом|ых|ых",
    "кольчужн|ый|ого|ому|ый|ым|ом|ых|ых",
    "стальн|ой|ого|ому|ой|ым|ом|ых|ых", 
    "золот|ой|ого|ому|ой|ым|ом|ых|ых", 
    "серебрянн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "кованн|ый|ого|ому|ый|ым|ом|ых|ых", 
    "лит|ой|ого|ому|ой|ым|ом|ых|ых"};

static char *potions[] =
  { "сосуд||а|у||ом|е|а|ов", 
    "эликсир||а|у||ом|е|а|ов", 
    "отвар||а|у||ом|е|а|ов", 
    "насто|й|я|ю|й|ем|е|я|ев" };

static char *pills[] =
  { "кор|ень|ня|ню|ень|нем|не|ня|ней", 
    "лист||а|у||ом|е|а|ьев", 
    "кореш|ок|ка|ку|ок|ком|ке|ка|ков", 
    "кусоч|ек|ка|ку|ек|ком|ке|ка|ков тины", 
    "облом|ок|ка|ку|ок|ком|ке|ка|ков ветки",
    "обруб|ок|ка|ку|ок|ком|ке|ка|ков дерева",
    "обрыв|ок|ка|ку|ок|ком|ке|ка|ков паутины"};

static char *bags[] = 
  { "рюкзак||а|у||ом|е|а|ов", 
    "кошел|ь|я|ю|ь|ем|е|я|ей", 
    "короб||а|у||ом|е|а|ов", 
    "чемодан||а|у||ом|е|а|ов"};


static char *weapon_types[] = 
  { "меч||а|у||ом|е|а|ей", 
    "серп||а|у||ом|е|а|ов", 
    "плуг||а|у||ом|е|а|ов", 
    "топор||а|у||ом|е|а|ов", 
    "кинжал||а|у||ом|е|а|ов", 
    "хлыст||а|у||ом|е|а|ов", 
    "клев|ец|ца|цу|ец|цом|це|ца|цов", 
    "са|й|я|ю|й|ем|е|я|ев", 
    "посох||а|у||ом|е|а|ов", 
    "кнут||а|у||ом|е|а|ов", 
    "кистен|ь|я|ю|ь|ем|е|я|ей", 
    "эспадон||а|у||ом|е|а|ов", 
    "фламберг||а|у||ом|е|а|ов", 
    "секач||а|у||ом|е|а|ей", 
    "гладиус||а|у||ом|е|а|ов", 
    "стилет||а|у||ом|е|а|ов", 
    "эсток||а|у||ом|е|а|ов", 
    "шест||а|у||ом|е|а|ов", 
    "трезуб|ец|ца|цу|ец|цом|це|ца|цев"};


static char *weapon_names[] =
  { "sword random weapon",
    "sickle random weapon", 
    "plow random weapon", 
    "axe random weapon", 
    "dagger random weapon", 
    "lash random weapon", 
    "axe random weapon", 
    "sai random weapon", 
    "staff random weapon", 
    "whip random weapon", 
    "slung random weapon", 
    "espadon random weapon", 
    "flamberg random weapon", 
    "axe random weapon", 
    "gladius random weapon", 
    "stiletto random weapon", 
    "estok random weapon", 
    "pole random weapon", 
    "trident random weapon"};


static int weapon_classes[] =
  { WEAPON_SWORD, WEAPON_EXOTIC, WEAPON_EXOTIC, WEAPON_AXE, 
    WEAPON_DAGGER, WEAPON_DAGGER, WEAPON_AXE, WEAPON_DAGGER, 
    WEAPON_SPEAR, WEAPON_WHIP, WEAPON_MACE, WEAPON_SWORD, 
    WEAPON_SWORD, WEAPON_AXE, WEAPON_SWORD, WEAPON_DAGGER, 
    WEAPON_SWORD, WEAPON_SPEAR, WEAPON_POLEARM};


static char *noun[] = { "жезл", "божок", "идол", "череп" };


// Returns a clerical or magical spell of the appropriate (masked) type
int random_spell( int level, int mask, int *type )
{
  for ( ;; )
  {
    int skill_no = number_range(0, max_skill-1);

    if ((skill_table[skill_no].skill_level[CLASS_MAG] <= level
         || skill_table[skill_no].skill_level[CLASS_CLE] <= level)
      && skill_table[skill_no].spell_fun // an actual spell?
      && mask & (1<<skill_table[skill_no].target)  // appropriate?
      && !IS_SET(skill_table[skill_no].flag, S_CLAN))
    {
      if (IS_SET(skill_table[skill_no].group,SPELL))
      {
        *type = skill_table[skill_no].target;
        return skill_no;
      }
    }
  }
}

// Wands/Staves/Potions/Pills and bags
void wield_random_magic( CHAR_DATA *mob )
{
  int item_type = number_range(59, 64);  /* Get template obj from LIMBO.ARE */
  OBJ_INDEX_DATA *pObjIndex = get_obj_index( item_type );
  OBJ_DATA *obj = create_object( pObjIndex, 0 );
  int type;

  char buffer[256];
  char buf[MAX_STRING_LENGTH];

  if(!obj) return;
  obj->level = number_fuzzy( mob->level );
  switch (item_type)
  {
    case 59: 
     do_printf(buffer, "%s %s свит|ок|ка|ку|ок|ком|ке|ка|ков", 
      cond3[(number_range(0, nelems(cond3)-1))],
      material4[number_range(0, nelems(material4)-1)]);

     obj->value[0] = number_range(obj->level - 5, obj->level +25); 
     obj->value[0] = URANGE(1, obj->value[0], 110);
     obj->value[1] = random_spell((int)obj->value[0],
      MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
     if (number_percent() < 50)
      obj->value[2] = random_spell((int)obj->value[0], 1<<type, &type);
     if (number_percent() < 50)
      obj->value[3] = random_spell((int)obj->value[0], 1<<type, &type);
     if (number_percent() < 50)
      obj->value[4] = random_spell((int)obj->value[0], 1<<type, &type);

     break;
   case 60: /* wand */
     do_printf(buffer, "%s %s жезл||а|у||ом|е|а|ов", 
      cond1[number_range(0, (nelems(cond1)-1))],
      material1[number_range(0, (nelems(material1)-1))]);

     obj->value[0] = number_range(obj->level -5, obj->level +25);  /* level */
     obj->value[0] = URANGE(1, obj->value[0], 110);
     obj->value[1] = number_fuzzy((int)obj->level / 2 + 3); /* max charges */
     obj->value[2] = number_range(1, (int)obj->value[1]);   /* charges left */
     obj->value[3] = random_spell((int)obj->value[0], 
      MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
     break;

   case 61: /* staff */
     do_printf(buffer, "%s %s посох||а|у||ом|е|а|ов",
      cond1[number_range(0, (nelems(cond1)-1))],
      material2[number_range(0, (nelems(material2)-1))]);
     obj->value[0] = number_range(obj->level - 10, obj->level +25);  /* level */
     obj->value[0] = URANGE(1, obj->value[0], 110);
     obj->value[1] = number_fuzzy(obj->level / 2 + 3); /* max charges */
     obj->value[2] = number_range(1, (int)obj->value[1]);   /* charges left */
     obj->value[3] = random_spell((int)obj->value[0],
      MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
     break;
   case 62: /* potion */
     do_printf(buffer, "%s %s %s", 
      cond2[number_range(0, (nelems(cond2)-1))],
      material5[number_range(0, (nelems(material5)-1))],
      potions[number_range(0, nelems(potions)-1)]);
     obj->value[0] = number_range(obj->level / - 5, obj->level + 25); /* level */
     obj->value[0] = URANGE(1, obj->value[0], 110);
     obj->value[1] = random_spell((int)obj->value[0], MASK_DEFENSIVE, &type);
     if (number_percent() < 50)
      obj->value[2] = random_spell((int)obj->value[0], MASK_DEFENSIVE, &type);
     if (number_percent() < 40)
      obj->value[3] = random_spell((int)obj->value[0], MASK_DEFENSIVE, &type);
     if (number_percent() < 30)
      obj->value[4] = random_spell((int)obj->value[0], MASK_DEFENSIVE, &type);
     break;

   case 63: /* pill */
     do_printf(buffer, "%s %s", 
      cond4[number_range(0, (nelems(cond4)-1))],
      pills[number_range(0, nelems(pills)-1)]);
     obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
     obj->value[0] = URANGE(1, obj->value[0], 36);
     obj->value[1] = random_spell((int)obj->value[0], MASK_DEFENSIVE, &type);
     if (number_percent() < 50)
      obj->value[2] = random_spell((int)obj->value[0], MASK_DEFENSIVE, &type);
     break;

   case 64: /* bag */
     do_printf(buffer, "%s %s",
      material3[number_range(0, (nelems(material3)-1))],
      bags[number_range(0, nelems(bags)-1)]);
     obj->value[0] = number_range(1, mob->level); /* weight */
     obj->value[1] = number_range(0, 1);
     obj->value[2] = -1;
     obj->value[3] = number_range(20,200);
     obj->value[4] = number_range(30,90);
     break;
  };

  // Generate the description strings

  free_string( obj->short_descr );
  obj->short_descr = str_dup( buffer );

  free_string( obj->description );
  do_printf( buf, "Тут лежит %s.", get_obj_desc(obj,'1'));
  obj->description = str_dup( buf );

  // Put the item in the mob's inventory
  obj_to_char( obj, mob );
}

// Anything wearable, and trinkets
void wield_random_armor( CHAR_DATA *mob )
{
  int item_type = number_range(0, MAX_WEAR - 1); /* template from LIMBO.ARE */
  OBJ_INDEX_DATA *pObjIndex = get_obj_index( item_type + 40 );
  OBJ_DATA *obj = create_object( pObjIndex, 0 );


  static int armor_mul[] = { 1, 3, 2, 5, 6, 6, 10, 10, 12};
  static int armor_div[] = { 1, 2, 1, 1, 1, 3, 2,  1,  2 };

  char buffer[64];

  char buf[MAX_STRING_LENGTH];
  
  if (!obj) return;
  obj->level = number_fuzzy( mob->level );
  if (obj->item_type == ITEM_ARMOR)
  {
    int ac_type = number_range(0,  nelems(material6)-1);
    do_printf( buffer, obj->short_descr, material6[ac_type]);
    obj->weight *= armor_mul[ac_type];
    obj->weight /= armor_div[ac_type];
    obj->value[0] = number_range(mob->level/5,mob->level);
    obj->value[1] = number_range(mob->level/5,mob->level);
    obj->value[2] = number_range(mob->level/5,mob->level);
    obj->value[3] = number_range(mob->level/5,mob->level);
    obj->cost = number_range(0,mob->level*1000);
    if (number_percent() < 100) random_apply(obj, mob);
  }

  else if (obj->item_type == ITEM_WEAPON)
  {
    int wea_type = number_range(0, nelems(weapon_types)-1);
    do_printf(buffer,"%s%s %s", cond5[number_range(0, nelems(cond5)-1)],
     material2[number_range(0, nelems(material2)-1)],weapon_types[wea_type]);
    obj->value[0] = weapon_classes[wea_type];
    obj->value[1] = number_range(obj->level/10+1, obj->level/4+3);
    obj->value[2] = number_range(obj->level/10+1, obj->level/2+5);
    obj->value[3] = number_range(2, MAX_DAMAGE_MESSAGE-2);
    obj->name=weapon_names[wea_type];
    random_apply(obj, mob);
  }

  else if (obj->item_type == ITEM_TREASURE)
  {
    random_apply(obj, mob);
    do_printf(buffer, "%s %s %s", cond1[number_range(0, nelems(cond1)-1)],
     material1[number_range(0, nelems(material1)-1)],
     noun[number_range(0, nelems(noun)-1)]);
  }

  free_string( obj->short_descr );
  obj->short_descr = str_dup( buffer );
  free_string( obj->description );
  do_printf( buf, "Тут лежит %s.", get_obj_desc(obj,'1'));
  obj->description = str_dup( buf );

  obj_to_char( obj, mob );
  equip_char( mob, obj, item_type );
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
                                }


/* OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * End
 */
void load_area( FILE *fp )
{
  AREA_DATA *pArea;
  const char      *word;
  bool      fMatch;

  pArea               = alloc_perm( sizeof(*pArea) );
  pArea->age          = 15;
  pArea->nplayer      = 0;
  pArea->file_name    = str_dup( strArea );
  pArea->vnum         = top_area;
  pArea->name         = str_dup( "New Area" );
  pArea->builders     = str_dup( "" );
  pArea->clan         = str_dup( "none" );
  pArea->security     = 9;                    /* 9 -- Hugin */
  pArea->min_vnum     = 0;
  pArea->max_vnum     = 0;
  pArea->area_flags   = 0;
  pArea->version      = 0;

  for ( ; ; )
  {
    word   = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
      case 'N':
       SKEY( "Name", pArea->name );
       break;
      case 'S':
       KEY( "Security", pArea->security, fread_number( fp ) );
       break;
      case 'V':
       if ( !str_cmp( word, "VNUMs" ) )
       {
         pArea->min_vnum = fread_number64( fp );
         pArea->max_vnum = fread_number64( fp );
       }
       if ( !str_cmp(word,"Version")) pArea->version=fread_number(fp);
       break;
      case 'E':
       if ( !str_cmp( word, "End" ) )
       {
         fMatch = TRUE;
         if ( area_first == NULL ) area_first = pArea;
         if ( area_last  != NULL ) area_last->next = pArea;
         area_last   = pArea;
         pArea->next = NULL;
         top_area++;
         return;
       }
       break;
      case 'B':
       SKEY( "Builders", pArea->builders );
       break;
      case 'C':
       SKEY( "Credits", pArea->credits );
       SKEY( "Clan", pArea->clan );
       break;
      case 'F':
       KEY( "Flags", pArea->area_flags, fread_number64( fp ) );
       break;
    }
  }
}

/* Sets vnum range for area using OLC protection features. */
void assign_area_vnum( int64 vnum )
{
  if ( area_last->min_vnum == 0 || area_last->max_vnum == 0 )
   area_last->min_vnum = area_last->max_vnum = vnum;
  if ( vnum != URANGE( area_last->min_vnum, vnum, area_last->max_vnum ) )
  {
    if ( vnum < area_last->min_vnum ) area_last->min_vnum = vnum;
        else                          area_last->max_vnum = vnum;
  }
}

/* Snarf a help section. */
void load_helps( FILE *fp )
{
  HELP_DATA *pHelp;

  for ( ; ; )
  {
    pHelp           = alloc_perm( sizeof(*pHelp) );
    pHelp->level    = fread_number( fp );
    pHelp->keyword  = fread_string( fp );
    if ( pHelp->keyword[0] == '$' ) break;
    pHelp->text     = fread_string( fp );

    if ( !str_cmp( pHelp->keyword, "greeting" ) ) help_greeting = pHelp->text;

    if ( help_first == NULL ) help_first = pHelp;
    if ( help_last  != NULL ) help_last->next = pHelp;

    help_last       = pHelp;
    pHelp->next     = NULL;
    top_help++;
  }
}

void new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
  RESET_DATA *pr;

  if ( !pR ) return;

  pr = pR->reset_last;

  if ( !pr )
  {
    pR->reset_first = pReset;
    pR->reset_last  = pReset;
  }
  else
  {
    pR->reset_last->next = pReset;
    pR->reset_last       = pReset;
    pR->reset_last->next = NULL;
  }
  top_reset++;
}

void load_resets( FILE *fp )
{
  RESET_DATA *pReset;
  int64       iLastRoom = 0;
  int64       iLastObj  = 0;

  if ( !area_last )
  {
    bug( "Load_resets: no #AREA seen yet.", 0 );
    exit( 1 );
  }

  for ( ; ; )
  {
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pexit;
    char letter;

    if ( ( letter = fread_letter( fp ) ) == 'S' ) break;

    if ( letter == '*' )
    {
      fread_to_eol( fp );
      continue;
    }

    pReset          = alloc_perm( sizeof(*pReset) );
    pReset->command = letter;
    /* if_flag */     fread_number  (fp);
    pReset->arg1    = fread_number64(fp);
    pReset->arg2    = fread_number  (fp);

    // This is temporary bugfix. Saboteur
    if (letter=='O' && pReset->arg2 !=0)
    {
      pReset->arg3=pReset->arg2;
      pReset->arg2=0;
    }
    else
    pReset->arg3    = (letter == 'G' || letter == 'R')
                            ? 0 : fread_number64( fp );
    pReset->arg4    = (letter == 'P' || letter == 'M')
                            ? fread_number(fp) : 0;
/*  pReset->arg4    = (letter == 'P' || letter == 'M')
                            ? ((temp == 0)? 1 : fread_number(fp)) : 0; */
    fread_to_eol( fp );

    /* Validate parameters.
     * We're calling the index functions for the side effect.*/
    switch ( letter )
    {
      default:
        bug( "Load_resets: bad command '%c'.", letter );
        exit( 1 );
        break;
      case 'M':
        get_mob_index  ( pReset->arg1 );
        if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
        {
          new_reset( pRoomIndex, pReset );
          iLastRoom = pReset->arg3;
        }
        break;
      case 'O':
        get_obj_index ( pReset->arg1 );
        if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
        {
          new_reset( pRoomIndex, pReset );
          iLastObj = pReset->arg3;
        }
        break;
      case 'P':
        get_obj_index  ( pReset->arg1 );
        if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
        {
          new_reset( pRoomIndex, pReset );
        }
        break;
      case 'G':
      case 'E':
        get_obj_index (pReset->arg1);
        if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
        {
          new_reset( pRoomIndex, pReset );
          iLastObj = iLastRoom;
        }
        break;
      case 'D':
        pRoomIndex = get_room_index( pReset->arg1 );
        if ( pReset->arg2 < 0
         ||   pReset->arg2 > 5
         || !pRoomIndex
         || !( pexit = pRoomIndex->exit[pReset->arg2] )
         || !IS_SET( pexit->rs_flags, EX_ISDOOR ) )
        {
          bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
          exit( 1 );
        }
        switch ( pReset->arg3 )
        {
          default:
            bug( "Load_resets: 'D': bad 'locks': %u." , pReset->arg3);
          case 0: break;
          case 1: SET_BIT( pexit->rs_flags, EX_CLOSED );
                  SET_BIT( pexit->exit_info, EX_CLOSED ); break;
          case 2: SET_BIT( pexit->rs_flags, EX_CLOSED | EX_LOCKED );
                  SET_BIT( pexit->exit_info, EX_CLOSED | EX_LOCKED ); break;
        }
        break;
      case 'R':
        pRoomIndex          = get_room_index( pReset->arg1 );
        if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
        {
          bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
          exit( 1 );
        }

        if ( pRoomIndex ) new_reset( pRoomIndex, pReset );
        break;
    }
  }
}

void load_rooms( FILE *fp )
{
  ROOM_INDEX_DATA *pRoomIndex;

  if ( area_last == NULL )
  {
    bug( "Load_resets: no #AREA seen yet.", 0 );
    exit( 1 );
  }

  for ( ; ; )
  {
    int64 vnum;
    char letter;
    int door;
    int iHash;

    letter = fread_letter( fp );
    if ( letter != '#' )
    {
      bug( "Load_rooms: # not found.", 0 );
      exit( 1 );
    }

    vnum   = fread_number64( fp );
    if ( vnum == 0 ) break;

    fBootDb = FALSE;
    if ( get_room_index( vnum ) != NULL )
    {
      bug( "Load_rooms: vnum %u duplicated.", vnum );
      exit( 1 );
    }
    fBootDb = TRUE;

    pRoomIndex              = alloc_perm( sizeof(*pRoomIndex) );
    pRoomIndex->owner       = str_dup("");
    pRoomIndex->people      = NULL;
    pRoomIndex->contents    = NULL;
    pRoomIndex->extra_descr = NULL;
    pRoomIndex->ra          = 0;
    pRoomIndex->raffect     = NULL;
    pRoomIndex->area        = area_last;
    pRoomIndex->vnum        = vnum;
    pRoomIndex->name        = fread_string( fp );
    pRoomIndex->description = fread_string( fp );
                              fread_number( fp ); // Area Number
    pRoomIndex->room_flags  = fread_flag64( fp );
    if (IS_SET(pRoomIndex->room_flags,ROOM_DELETED))
    {
      log_printf("Room %u corrected from deleted flag",pRoomIndex->vnum);
      REM_BIT(pRoomIndex->room_flags,ROOM_DELETED);
    }
    pRoomIndex->sector_type = fread_number( fp );
    pRoomIndex->light       = 0;
    for ( door = 0; door <= 5; door++ ) pRoomIndex->exit[door] = NULL;

    // Defaults
    pRoomIndex->heal_rate = 100;
    pRoomIndex->mana_rate = 100;

    for ( ; ; )
    {
      letter = fread_letter( fp );

      if ( letter == 'S' ) break;

      if ( letter == 'H') pRoomIndex->heal_rate = fread_number(fp);
        
      else if ( letter == 'M') pRoomIndex->mana_rate = fread_number(fp);

      else if ( letter == 'C') /* clan */
      {
        fread_string(fp); /* was clan rooms */
      }
        
      else if ( letter == 'D' )
      {
        EXIT_DATA *pexit;
        int locks;

        door = fread_number( fp );
        if ( door < 0 || door > 5 )
        {
          bug( "Fread_rooms: vnum %u has bad door number.", vnum );
          exit( 1 );
        }

        pexit              = alloc_perm( sizeof(*pexit) );
        pexit->description = fread_string( fp );
        pexit->keyword     = fread_string( fp );
        pexit->exit_info   = 0;
        pexit->rs_flags    = 0;                    /* OLC */
        locks              = fread_number( fp );
        pexit->key         = fread_number64( fp );
        pexit->u1.vnum     = fread_number64( fp );
        pexit->orig_door   = door;                 /* OLC */

        switch ( locks )
        {
 case 1: pexit->exit_info = EX_ISDOOR;               
         pexit->rs_flags  = EX_ISDOOR;
         break;
 case 2: pexit->exit_info = EX_ISDOOR|EX_PICKPROOF;
         pexit->rs_flags  = pexit->exit_info;
         break;
 case 3: pexit->exit_info = EX_ISDOOR|EX_NOPASS;
         pexit->rs_flags  = pexit->exit_info;
         break;
 case 4: pexit->exit_info = EX_ISDOOR|EX_PICKPROOF|EX_BASHPROOF;
         pexit->rs_flags  = pexit->exit_info;
         break;
 case 5: pexit->exit_info = EX_ISDOOR|EX_NOPASS;
         pexit->rs_flags  = pexit->exit_info;
         break;
 case 6: pexit->exit_info = EX_ISDOOR|EX_PICKPROOF|EX_NOPASS;
         pexit->rs_flags  = pexit->exit_info;
         break;
 case 7: pexit->exit_info = EX_ISDOOR|EX_BASHPROOF|EX_NOPASS;
         pexit->rs_flags  = pexit->exit_info;
         break;
 case 8: pexit->exit_info = EX_ISDOOR|EX_PICKPROOF|EX_BASHPROOF|EX_NOPASS;
         pexit->rs_flags  = pexit->exit_info;
         break;
        }

         pRoomIndex->exit[door]  = pexit;
         pRoomIndex->old_exit[door] = pexit;
         top_exit++;
      }
      else if ( letter == 'E' )
      {
        EXTRA_DESCR_DATA *ed;

        ed                      = alloc_perm( sizeof(*ed) );
        ed->keyword             = fread_string( fp );
        ed->description         = fread_string( fp );
        ed->next                = pRoomIndex->extra_descr;
        pRoomIndex->extra_descr = ed;
        top_ed++;
      }
      else if (letter=='K') {fread_to_eol(fp);}
      else if (letter == 'O')
      {
        if (pRoomIndex->owner[0] != '\0')
        {
          bug("Load_rooms: duplicate owner.",0);
          exit(1);
        }
        pRoomIndex->owner = fread_string(fp);
      }
      else
      {
        bug( "Load_rooms: vnum %u has flag not 'DES'.", vnum );
        exit( 1 );
      }
    }

    iHash                   = (int)vnum % MAX_KEY_HASH;
    pRoomIndex->next        = room_index_hash[iHash];
    room_index_hash[iHash]  = pRoomIndex;
    top_room++;
    top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; /* OLC */
    assign_area_vnum( vnum );                                    /* OLC */

  }
}

/* Snarf a shop section. */
void load_shops( FILE *fp )
{
  SHOP_DATA *pShop;

  for ( ; ; )
  {
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;

    pShop            = alloc_perm( sizeof(*pShop) );
    pShop->keeper    = fread_number64( fp );
    if ( pShop->keeper == 0 ) break;
    for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
     pShop->buy_type[iTrade]     = fread_number( fp );
    pShop->profit_buy  = fread_number( fp );
    pShop->profit_sell = fread_number( fp );
    pShop->open_hour   = fread_number( fp );
    pShop->close_hour  = fread_number( fp );
    fread_to_eol( fp );
    pMobIndex          = get_mob_index( pShop->keeper );
    pMobIndex->pShop   = pShop;
    fix_keepers(pMobIndex);
    if ( shop_first == NULL ) shop_first = pShop;
    if ( shop_last  != NULL ) shop_last->next = pShop;

    shop_last       = pShop;
    pShop->next     = NULL;
    top_shop++;
  }
}

/* Snarf spec proc declarations. */
void load_specials( FILE *fp )
{
  for ( ; ; )
  {
    MOB_INDEX_DATA *pMobIndex;
    char letter;

    switch ( letter = fread_letter( fp ) )
    {
      default:
        bug( "Load_specials: letter '%c' not *MS.", letter );
        exit( 1 );
      case 'S':
        return;
      case '*':
        break;

      case 'M':
        pMobIndex           = get_mob_index ( fread_number64 ( fp ) );
        pMobIndex->spec_fun = spec_lookup   ( fread_word   ( fp ) );
        if ( pMobIndex->spec_fun == 0 )
        {
          bug( "Load_specials: 'M': vnum %u.", pMobIndex->vnum );
          exit( 1 );
        }
        break;
    }
    fread_to_eol( fp );
  }
}

/* Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits. */
void fix_exits( void )
{
//  extern const int rev_dir [];
//  char buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *pRoomIndex;
//  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit;
//  EXIT_DATA *pexit_rev;
  int iHash;
  int door;

  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for ( pRoomIndex  = room_index_hash[iHash];
          pRoomIndex != NULL;
          pRoomIndex  = pRoomIndex->next )
    {
      bool fexit;

      fexit = FALSE;
      for ( door = 0; door <= 5; door++ )
      {
        if ( ( pexit = pRoomIndex->exit[door] ) != NULL )
        {
          if ( pexit->u1.vnum <= 0 || get_room_index(pexit->u1.vnum) == NULL)
           pexit->u1.to_room = NULL;
          else
          {
            fexit = TRUE; 
            pexit->u1.to_room = get_room_index( pexit->u1.vnum );
          }
        }
      }
      if (!fexit) SET_BIT(pRoomIndex->room_flags,ROOM_NO_MOB);
    }
  }

/*
  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for ( pRoomIndex  = room_index_hash[iHash];
          pRoomIndex != NULL;
          pRoomIndex  = pRoomIndex->next )
    {
      for ( door = 0; door <= 5; door++ )
      {
        if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
         &&   ( to_room   = pexit->u1.to_room            ) != NULL
         &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
         &&   pexit_rev->u1.to_room != pRoomIndex 
         &&   (pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299)
         &&   (pRoomIndex->vnum < 10000 || pRoomIndex->vnum > 10200))
        {
            do_printf( buf, "Fix_exits: %u:%d -> %u:%d -> %u.",
           pRoomIndex->vnum, door,to_room->vnum, rev_dir[door],
           (pexit_rev->u1.to_room == NULL) ? 0 : pexit_rev->u1.to_room->vnum );
          bug( buf, 0 );
  
        }
      }
    }
  }
*/
}

/* Repopulate areas periodically.*/
void area_update( void )
{
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];

// Updating gquest mobiles
  if (gquest.status==GQ_STARTED && area_first->age>=3)
  {
    int i;
    ROOM_INDEX_DATA *room;

    for (i=0;i<100;i++)
    {
      room=get_room_index(gquest.room[i]);
      if (room) reset_room(room);
    }
  }
// Updating areas
  for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
  {
    if ( ++pArea->age < 3 ) continue;

    /* Check age and reset.
     * Note: Mud School resets every 3 minutes (not 15).*/
    if ( (!pArea->empty && (pArea->nplayer == 0 || pArea->age >= 15))
     ||    pArea->age >= 31)
    {
      ROOM_INDEX_DATA *pRoomIndex;

      reset_area( pArea );
      do_printf(buf,"%s has just been reset.",pArea->name);
      wiznet(buf,NULL,NULL,WIZ_RESETS,0);
        
      pArea->age = number_range( 0, 3 );
      pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
      if ( pRoomIndex != NULL && pArea == pRoomIndex->area ) pArea->age = 15 - 2;
      else if (pArea->nplayer == 0) pArea->empty = TRUE;
     }
  }
}

// OLC. Reset one room.  Called by reset_area and olc.
void reset_room( ROOM_INDEX_DATA *pRoom )
{
  RESET_DATA  *pReset;
  CHAR_DATA   *pMob;
  CHAR_DATA   *mob;
  OBJ_DATA    *pObj;
  CHAR_DATA   *LastMob = NULL;
  OBJ_DATA    *LastObj = NULL;
  int iExit;
  int level = 0;
  bool last;

  if ( !pRoom ) return;

  pMob        = NULL;
  last        = FALSE;
    
  for ( iExit = 0;  iExit < MAX_DIR;  iExit++ )
  {
    EXIT_DATA *pExit;
    if ( ( pExit = pRoom->exit[iExit] )
          /*  && !IS_SET( pExit->exit_info, EX_BASHED )   ROM OLC */ )  
    {
      pExit->exit_info = pExit->rs_flags;
      if ( ( pExit->u1.to_room != NULL )
       && ( ( pExit = pExit->u1.to_room->exit[rev_dir[iExit]] ) ) )
      {
        pExit->exit_info = pExit->rs_flags;
      }
    }
  }

  for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
  {
    MOB_INDEX_DATA  *pMobIndex;
    OBJ_INDEX_DATA  *pObjIndex;
    OBJ_INDEX_DATA  *pObjToIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[MAX_STRING_LENGTH];
    int count,limit=0;
    //OBJ_DATA *rnd_obj;

    switch ( pReset->command )
    {
      default:
        bug( "Reset_room: bad command %c.", pReset->command );
        break;
      case 'M':
        if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
        {
          bug( "Reset_room: 'M': bad vnum %u.", pReset->arg1 );
          continue;
        }
        if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
        {
          bug( "Reset_area: 'R': bad vnum %u.", pReset->arg3 );
          continue;
        }
        if ( pMobIndex->count >= pReset->arg2 )
        {
          last = FALSE;
          break;
        }
        count = 0;
        for (mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room)
          if (mob->pIndexData == pMobIndex)
          {
            count++;
            if (count >= pReset->arg4)
            {
              last = FALSE;
              break;
            }
          }

        if (count >= pReset->arg4) break;

        if((pMob = create_mobile( pMobIndex ))==NULL)
        {
          bug("Unable to create mob in reset! Room vnum: %d, Mob: %d",pRoom->vnum);
          continue;
        }
        if ( room_is_dark( pRoom ) ) SET_BIT(pMob->affected_by, AFF_INFRARED);

        /* Pet shop mobiles get ACT_PET set. */
        {
          ROOM_INDEX_DATA *pRoomIndexPrev;

          pRoomIndexPrev = get_room_index( pRoom->vnum - 1 );
          if ( pRoomIndexPrev && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
           SET_BIT( pMob->act, ACT_PET);
        }

        char_to_room( pMob, pRoom );

        LastMob = pMob;
        level  = URANGE( 0, pMob->level - 2, LEVEL_HERO - 1 ); /* -1 ROM */
        last = TRUE;
        //rnd_obj=create_random_item(number_range(20,100));
        //obj_to_char(rnd_obj,LastMob);

        break;

      case 'O':
        if (!(pObjIndex=get_obj_index(pReset->arg1)))
        {
          bug( "Reset_room: 'O' 1 : bad vnum %u", pReset->arg1 );
          do_printf(buf,"%u %d %u %d",pReset->arg1,pReset->arg2,pReset->arg3,pReset->arg4);
          bug(buf,1);
          continue;
        }

        if (!(pRoomIndex=get_room_index(pReset->arg3)))
        {
          bug( "Reset_room: 'O' 2 : bad vnum %u.",pReset->arg3 );
          do_printf (buf,"%u %d %u %d",pReset->arg1,pReset->arg2,pReset->arg3,pReset->arg4 );
          bug(buf,1);
          continue;
        }

        if (pRoom->area->nplayer>0||count_obj_list(pObjIndex,pRoom->contents)>0)
        {
          last = FALSE;
          break;
        }

        pObj = create_object( pObjIndex, UMIN(number_fuzzy( level ), LEVEL_HERO -1) );
        pObj->cost = 0;
        obj_to_room( pObj, pRoom );
        last = TRUE;
        break;
      case 'P':
        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
        {
          bug( "Reset_room: 'P': bad vnum %u.", pReset->arg1 );
          continue;
        }

        if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
        {
          bug( "Reset_room: 'P': bad vnum %u.", pReset->arg3 );
          continue;
        }

        if (pReset->arg2 > 50) limit = 6;
        else if (pReset->arg2 == -1) limit = 999;
        else limit = pReset->arg2;

        if ( pRoom->area->nplayer > 0
         || ( LastObj = get_obj_type( pObjToIndex ) ) == NULL
         || ( LastObj->in_room == NULL && !last)
         || ( pObjIndex->count >= limit /* && number_range(0,4) != 0 */ )
         || ( count = count_obj_list( pObjIndex, LastObj->contains ) ) > pReset->arg4  )
        {
          last = FALSE;
          break;
        }
        while (count < pReset->arg4)
        {
          pObj = create_object( pObjIndex, number_fuzzy( LastObj->level ) );
          obj_to_obj( pObj, LastObj );
          count++;
          if (pObjIndex->count >= limit)
          break;
        }

        /* fix object lock state! */
        LastObj->value[1] = LastObj->pIndexData->value[1];
        last = TRUE;
        break;
      case 'G':
      case 'E':
        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
        {
          bug( "Reset_room: 'E' or 'G': bad vnum %u.", pReset->arg1 );
          continue;
        }

        if ( !last ) break;

        if ( !LastMob )
        {
          bug( "Reset_room: 'E' or 'G': null mob for vnum %u.", pReset->arg1 );
          last = FALSE;
          break;
        }

        if ( LastMob->pIndexData->pShop )   /* Shop-keeper? */
        {
          int olevel=0,i,j;
          if (!pObjIndex->new_format) switch ( pObjIndex->item_type )
          {
            default:                olevel = 0;                      break;
            case ITEM_PILL:
            case ITEM_POTION:
            case ITEM_SCROLL:
              olevel = 53;
              for (i = 1; i < 5; i++)
              {
                if (pObjIndex->value[i] > 0)
                {
                  for (j = 0; j < MAX_CLASS; j++)
                  {
                    olevel = UMIN(olevel,
                    skill_table[pObjIndex->value[i]].skill_level[j]);
                  }
                }
              }
                   
              olevel = UMAX(0,(olevel * 3 / 4) - 2);
              break;
            case ITEM_WAND:         olevel = number_range( 10, 20 ); break;
            case ITEM_STAFF:        olevel = number_range( 15, 25 ); break;
            case ITEM_ARMOR:        olevel = number_range(  5, 15 ); break;
            case ITEM_WEAPON:       olevel = number_range(  5, 15 ); break;
            case ITEM_TREASURE:     olevel = number_range( 10, 20 ); break;

            #if 0 /* envy version */
            case ITEM_WEAPON:  if ( pReset->command == 'G' ) olevel = number_range( 5, 15 );
                               else olevel = number_fuzzy( level );
            #endif /* envy version */
                               break;
          }

          pObj = create_object( pObjIndex, olevel );
          SET_BIT( pObj->extra_flags, ITEM_INVENTORY );  /* ROM OLC */

          #if 0 /* envy version */
          if ( pReset->command == 'G' ) SET_BIT( pObj->extra_flags, ITEM_INVENTORY );
          #endif /* envy version */

        }
        else   /* ROM OLC else version */
        {
          int limit;
          if (pReset->arg2 > 50 ) limit = 6;
          else if ( pReset->arg2 == -1 || pReset->arg2 == 0 ) limit = 999;
          else limit = pReset->arg2;
          if (pObjIndex->count < limit || number_range(0,4)==0)
          {
            pObj = create_object(pObjIndex,
            UMIN( number_fuzzy(level),LEVEL_HERO-1) );
          }
          else break;
        }
                                                                         
        #if 0 /* envy else version */
      else
      {
        pObj = create_object( pObjIndex, number_fuzzy( level ) );
      }
      #endif /* envy else version */

      obj_to_char( pObj, LastMob );
      if ( pReset->command == 'E' ) equip_char( LastMob, pObj, (int)pReset->arg3 );
      last = TRUE;
      break;

    case 'D':
      break;

    case 'R':
      if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
      {
        bug( "Reset_room: 'R': bad vnum %u.", pReset->arg1 );
        continue;
      }

      {
        EXIT_DATA *pExit;
        int d0;
        int d1;

        for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
        {
          d1                   = number_range( d0, pReset->arg2-1 );
          pExit                = pRoomIndex->exit[d0];
          pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
          pRoomIndex->exit[d1] = pExit;
        }
      }
      break;
    }
  }
}

// OLC. Reset one area.
void reset_area( AREA_DATA *pArea )
{
  ROOM_INDEX_DATA *pRoom;
  int64  vnum;
  int nroom;

  pArea->nrandom=0; nroom=0;
  for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
  {
    if ( ( pRoom = get_room_index(vnum) ) )
    {
        reset_room(pRoom);
        pArea->nrandom+=is_random_obj_in_room(pRoom);
        nroom++;
    }
  }
  
  if ( pArea->nrandom < 3 )
  reset_random_items(pArea,nroom);
}

//  Translate mobprog vnums pointers to real code
void fix_mobprogs( void )
{
  MOB_INDEX_DATA *pMobIndex;
  MPROG_LIST        *list;
  MPROG_CODE        *prog;
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
          bug( "Fix_mobprogs: code vnum %u not found.", list->vnum );
          exit( 1 );
        }
      }
    }
  }
}

/* Create an instance of a mobile.*/
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
  CHAR_DATA *mob;
  int i;
  AFFECT_DATA af;

  mobile_count++;

  if ( pMobIndex == NULL )
  {
    bug( "Create_mobile: NULL pMobIndex.", 0 );
    exit(1);//return NULL;
  }

  mob = new_char();

  mob->pIndexData     = pMobIndex;

  mob->name           = str_dup( pMobIndex->player_name );
  mob->short_descr    = str_dup( pMobIndex->short_descr );
  mob->long_descr     = str_dup( pMobIndex->long_descr );
  mob->description    = str_dup( pMobIndex->description );
  mob->stealer        = NULL;
  mob->id             = get_mob_id();
  mob->spec_fun       = pMobIndex->spec_fun;
  mob->prompt         = NULL;
  mob->mprog_target   = NULL;

// if (pMobIndex->wealth == 0)
  if (race_table[pMobIndex->race].wealth || pMobIndex->pShop )
  {
    long wealth;

    wealth = number_range(pMobIndex->wealth/2, pMobIndex->wealth);
    mob->gold = number_range(wealth/200,wealth/100);
    mob->silver = wealth - (mob->gold * 100);
  } 
  else mob->silver=mob->gold=0;

  mob->group       = pMobIndex->group;
  mob->act         = pMobIndex->act;
  mob->affected_by = pMobIndex->affected_by;
  mob->alignment   = pMobIndex->alignment;
  mob->real_alignment   = pMobIndex->alignment;
  mob->level       = pMobIndex->level;
  mob->hitroll     = pMobIndex->hitroll;
  mob->damroll     = pMobIndex->damage[DICE_BONUS];
  mob->max_hit     = dice(pMobIndex->hit[DICE_NUMBER],
                          pMobIndex->hit[DICE_TYPE])
                        + pMobIndex->hit[DICE_BONUS];
  mob->hit         = mob->max_hit;
  mob->max_mana    = dice(pMobIndex->mana[DICE_NUMBER],
                          pMobIndex->mana[DICE_TYPE])
                        + pMobIndex->mana[DICE_BONUS];
  mob->mana        = mob->max_mana;
  mob->damage[DICE_NUMBER]= pMobIndex->damage[DICE_NUMBER];
  mob->damage[DICE_TYPE]  = pMobIndex->damage[DICE_TYPE];
  mob->dam_type           = pMobIndex->dam_type;
  if (mob->dam_type == 0)
    switch(number_range(1,3))
    {
      case (1): mob->dam_type = 3;  break;  /* slash */
      case (2): mob->dam_type = 7;  break;  /* pound */
      case (3): mob->dam_type = 11; break;  /* pierce */
    }
    for (i = 0; i < 4; i++)
     mob->armor[i]   = pMobIndex->ac[i]; 
    mob->off_flags   = pMobIndex->off_flags;
    mob->imm_flags   = pMobIndex->imm_flags;
    mob->res_flags   = pMobIndex->res_flags;
    mob->vuln_flags  = pMobIndex->vuln_flags;
    mob->start_pos   = pMobIndex->start_pos;
    mob->default_pos = pMobIndex->default_pos;
    mob->sex         = pMobIndex->sex;
    if (mob->sex == 3) mob->sex = number_range(1,2);
    mob->race        = pMobIndex->race;
    mob->form        = pMobIndex->form;
    mob->size        = pMobIndex->size;

    // computed on the spot
    for (i = 0; i < MAX_STATS; i ++)
          mob->perm_stat[i] = UMIN(25,11 + mob->level/4);
          
    if (IS_SET(mob->act,ACT_WARRIOR))
    {
      mob->perm_stat[STAT_STR] += 3;
      mob->perm_stat[STAT_INT] -= 1;
      mob->perm_stat[STAT_CON] += 2;
    }
      
    if (IS_SET(mob->act,ACT_THIEF))
    {
      mob->perm_stat[STAT_DEX] += 3;
      mob->perm_stat[STAT_INT] += 1;
      mob->perm_stat[STAT_WIS] -= 1;
    }
      
    if (IS_SET(mob->act,ACT_CLERIC))
    {
      mob->perm_stat[STAT_WIS] += 3;
      mob->perm_stat[STAT_DEX] -= 1;
      mob->perm_stat[STAT_STR] += 1;
    }
      
    if (IS_SET(mob->act,ACT_MAGE))
    {
      mob->perm_stat[STAT_INT] += 3;
      mob->perm_stat[STAT_STR] -= 1;
      mob->perm_stat[STAT_DEX] += 1;
    }
      
    if (IS_SET(mob->off_flags,OFF_FAST))
          mob->perm_stat[STAT_DEX] += 2;
          
    mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
    mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;
/*
    mob->perm_stat[STAT_STR] = UMAX(mob->perm_stat[STAT_STR] + 3, 31);
    mob->perm_stat[STAT_DEX] = UMAX(mob->perm_stat[STAT_DEX] + 3, 31);
    mob->perm_stat[STAT_INT] = UMAX(mob->perm_stat[STAT_INT] + 3, 31);
    mob->perm_stat[STAT_WIS] = UMAX(mob->perm_stat[STAT_WIS] + 3, 31);
    mob->perm_stat[STAT_CON] = UMAX(mob->perm_stat[STAT_CON] + 3, 31);
*/
    /* let's get some spell action */
    if (IS_AFFECTED(mob,AFF_SANCTUARY))
    {
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("sanctuary");
      af.level     = mob->level;
      af.duration  = -1;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_SANCTUARY;
      affect_to_char( mob, &af );
    }

    if (IS_AFFECTED(mob,AFF_HASTE))
    {
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("haste");
      af.level     = mob->level;
      af.duration  = -1;
      af.location  = APPLY_DEX;
      af.modifier  = 1 + (mob->level >= 18) + (mob->level >= 25) + 
                         (mob->level >= 32);
      af.bitvector = AFF_HASTE;
      affect_to_char( mob, &af );
    }

    if (IS_AFFECTED(mob,AFF_PROTECT_EVIL))
    {
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("protection evil");
      af.level     = mob->level;
      af.duration  = -1;
      af.location  = APPLY_SAVES;
      af.modifier  = -1;
      af.bitvector = AFF_PROTECT_EVIL;
      affect_to_char(mob,&af);
    }

    if (IS_AFFECTED(mob,AFF_PROTECT_GOOD))
    {
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("protection good");
      af.level     = mob->level;
      af.duration  = -1;
      af.location  = APPLY_SAVES;
      af.modifier  = -1;
      af.bitvector = AFF_PROTECT_GOOD;
      affect_to_char(mob,&af);
    }
  mob->position = mob->start_pos;
  pMobIndex->count++;
  if (number_percent() <= 3) wield_random_magic( mob );
  mob->next = char_list;
  char_list = mob;
  return mob;
}

// duplicate a mobile exactly -- except inventory
void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone)
{
  int i;
  AFFECT_DATA *paf;

  if ( parent == NULL || clone == NULL || !IS_NPC(parent)) return;
    
  /* start fixing values */ 
  clone->name         = str_dup(parent->name);
  clone->version      = parent->version;
  clone->short_descr  = str_dup(parent->short_descr);
  clone->long_descr   = str_dup(parent->long_descr);
  clone->description  = str_dup(parent->description);
  clone->group        = parent->group;
  clone->sex          = parent->sex;
  clone->class[0]     = parent->class[0];
  clone->class[1]     = parent->class[1];
  clone->class[2]     = parent->class[2];
  clone->class[3]     = parent->class[3];
  clone->race         = parent->race;
  clone->level        = parent->level;
  clone->trust        = 0;
  clone->timer        = parent->timer;
  clone->wait         = parent->wait;
  clone->hit          = parent->hit;
  clone->max_hit      = parent->max_hit;
  clone->mana         = parent->mana;
  clone->max_mana     = parent->max_mana;
  clone->move         = parent->move;
  clone->max_move     = parent->max_move;
  clone->gold         = parent->gold;
  clone->silver       = parent->silver;
  clone->exp          = parent->exp;
  clone->act          = parent->act;
  clone->comm         = parent->comm;
  clone->imm_flags    = parent->imm_flags;
  clone->res_flags    = parent->res_flags;
  clone->vuln_flags   = parent->vuln_flags;
  clone->invis_level  = parent->invis_level;
  clone->affected_by  = parent->affected_by;
  clone->position     = parent->position;
  clone->practice     = parent->practice;
  clone->train        = parent->train;
  clone->saving_throw = parent->saving_throw;
  clone->alignment    = parent->alignment;
  clone->hitroll      = parent->hitroll;
  clone->damroll      = parent->damroll;
  clone->wimpy        = parent->wimpy;
  clone->form         = parent->form;
  clone->size         = parent->size;
  clone->off_flags    = parent->off_flags;
  clone->dam_type     = parent->dam_type;
  clone->start_pos    = parent->start_pos;
  clone->default_pos  = parent->default_pos;
  clone->spec_fun     = parent->spec_fun;
  
  for (i = 0; i < 4; i++) clone->armor[i] = parent->armor[i];

  for (i = 0; i < MAX_STATS; i++)
  {
    clone->perm_stat[i]     = parent->perm_stat[i];
    clone->mod_stat[i]      = parent->mod_stat[i];
  }

  for (i = 0; i < 3; i++) clone->damage[i] = parent->damage[i];

  /* now add the affects */
  for (paf = parent->affected; paf != NULL; paf = paf->next)
   affect_to_char(clone,paf);
}

/* Create an instance of an object.*/
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
  AFFECT_DATA *paf;
  OBJ_DATA *obj;
  int i;

  if ( pObjIndex == NULL )
  {
    bug( "Create_object: NULL pObjIndex.", 0 );
    exit(1);//return NULL;
  }

  obj = new_obj();

  obj->pIndexData     = pObjIndex;
  obj->in_room        = NULL;
  obj->enchanted      = FALSE;

  if (pObjIndex->new_format) obj->level = pObjIndex->level;
  else obj->level  = UMAX(0,level);
  obj->wear_loc       = -1;

  obj->name           = str_dup( pObjIndex->name );
  obj->short_descr    = str_dup( pObjIndex->short_descr );
  obj->description    = str_dup( pObjIndex->description );
  obj->material       = str_dup(pObjIndex->material);
  obj->item_type      = pObjIndex->item_type;
  obj->extra_flags    = pObjIndex->extra_flags;
  obj->wear_flags     = pObjIndex->wear_flags;
  obj->value[0]       = pObjIndex->value[0];
  obj->value[1]       = pObjIndex->value[1];
  obj->value[2]       = pObjIndex->value[2];
  obj->value[3]       = pObjIndex->value[3];
  obj->value[4]       = pObjIndex->value[4];
  obj->value[5]       = pObjIndex->value[5];
  obj->weight         = pObjIndex->weight;
  obj->durability     = pObjIndex->durability;
  obj->condition      = pObjIndex->condition;
//  obj->owner          = str_dup("(public)");

  if (level == -1 || pObjIndex->new_format) obj->cost = pObjIndex->cost;
  else obj->cost = number_fuzzy( 10 ) * number_fuzzy( level ) * number_fuzzy( level );

  // Mess with object properties.
  switch ( obj->item_type )
  {
    default:
      bug( "Read_object: vnum %u bad type.", pObjIndex->vnum );
      break;
    case ITEM_LIGHT:
      if (obj->value[2] >= 999) obj->value[2] = -1;
      break;
    case ITEM_ENCHANT:
    case ITEM_SCUBA:
    case ITEM_BONUS:
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
    case ITEM_PORTAL:
      if (!pObjIndex->new_format) obj->cost /= 5;
      break;
    case ITEM_TREASURE:
    case ITEM_WARP_STONE:
    case ITEM_ROOM_KEY:
    case ITEM_GEM:
    case ITEM_JEWELRY:
      break;
    case ITEM_JUKEBOX:
      for (i = 0; i < 5; i++) obj->value[i] = -1;
      break;
    case ITEM_SCROLL:
      if (level != -1 && !pObjIndex->new_format)
         obj->value[0] = number_fuzzy( (int)obj->value[0] );
      break;
    case ITEM_WAND:
    case ITEM_STAFF:
      if (level != -1 && !pObjIndex->new_format)
      {
        obj->value[0] = number_fuzzy( (int)obj->value[0] );
        obj->value[1] = number_fuzzy( (int)obj->value[1] );
        obj->value[2] = obj->value[1];
      }
      if (!pObjIndex->new_format) obj->cost *= 2;
      break;
    case ITEM_WEAPON:
      if (level != -1 && !pObjIndex->new_format)
      {
        obj->value[1] = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
        obj->value[2] = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
      }
      break;
    case ITEM_ARMOR:
      if (level != -1 && !pObjIndex->new_format)
      {
        obj->value[0]       = number_fuzzy( level / 5 + 3 );
        obj->value[1]       = number_fuzzy( level / 5 + 3 );
        obj->value[2]       = number_fuzzy( level / 5 + 3 );
      }
      break;
    case ITEM_POTION:
    case ITEM_PILL:
      if (level != -1 && !pObjIndex->new_format)
        obj->value[0] = number_fuzzy( number_fuzzy( (int)obj->value[0] ) );
      break;

    case ITEM_MONEY:
      if (!pObjIndex->new_format) obj->value[0] = obj->cost;
      break;
  }
  
  for (paf = pObjIndex->affected; paf != NULL; paf = paf->next) 
    if ( paf->location == APPLY_SPELL_AFFECT ) affect_to_obj(obj,paf);
  
  obj->next           = object_list;
  object_list         = obj;
  pObjIndex->count++;

  return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
  int i;
  AFFECT_DATA *paf;
  EXTRA_DESCR_DATA *ed,*ed_new;

  if (parent == NULL || clone == NULL) return;

  /* start fixing the object */
  clone->name         = str_dup(parent->name);
  clone->short_descr  = str_dup(parent->short_descr);
  clone->description  = str_dup(parent->description);
  clone->item_type    = parent->item_type;
  clone->extra_flags  = parent->extra_flags;
  clone->wear_flags   = parent->wear_flags;
  clone->weight       = parent->weight;
  clone->cost         = parent->cost;
  clone->level        = parent->level;
  clone->durability   = parent->durability;
  clone->condition    = parent->condition;
  clone->material     = str_dup(parent->material);
  clone->timer        = parent->timer;

  for (i = 0;  i < 5; i ++) clone->value[i] = parent->value[i];

  /* affects */
  clone->enchanted    = parent->enchanted;
  
  for (paf = parent->affected; paf != NULL; paf = paf->next) 
    affect_to_obj(clone,paf);

  /* extended desc */
  for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
  {
    ed_new                  = new_extra_descr();
    ed_new->keyword         = str_dup( ed->keyword);
    ed_new->description     = str_dup( ed->description );
    ed_new->next            = clone->extra_descr;
    clone->extra_descr      = ed_new;
  }
}

/* Load mobprogs section */
void load_mobprogs( FILE *fp )
{
  MPROG_CODE *pMprog;
 
  for ( ; ; )
  {
    int vnum;
    char letter;
 
    letter = fread_letter( fp );
    if ( letter != '#' )
    {
      bug( "Load_mobprogs: # not found.", 0 );
      exit( 1 );
    }
 
    vnum = fread_number( fp );
    if ( vnum == 0 ) break;
 
    fBootDb = FALSE;
    if ( get_mprog_index( vnum ) != NULL )
    {
      bug( "Load_mobprogs: vnum %u duplicated.", vnum );
      exit( 1 );
    }
    fBootDb = TRUE;
 
    pMprog          = alloc_perm( sizeof(*pMprog) );
    pMprog->vnum    = vnum;
    pMprog->code    = fread_string( fp );
    if ( mprog_list == NULL ) mprog_list = pMprog;
    else
    {
      pMprog->next = mprog_list;
      mprog_list  = pMprog;
    }
    top_mprog_index++;
  }
}

MPROG_CODE *get_mprog_index( int vnum )
{
  MPROG_CODE *prg;
  for( prg = mprog_list; prg; prg = prg->next )
  {
    if ( prg->vnum == vnum ) return( prg );
  }
  return NULL;
}    

/* Clear a new character. */
void clear_char( CHAR_DATA *ch )
{
  static CHAR_DATA ch_zero;
  int i;

  *ch              = ch_zero;
  ch->name         = &str_empty[0];
  ch->short_descr  = &str_empty[0];
  ch->long_descr   = &str_empty[0];
  ch->description  = &str_empty[0];
  ch->prompt       = &str_empty[0];
  ch->logon        = current_time;
  ch->lines        = PAGELEN;
  for (i = 0; i < 4; i++) ch->armor[i] = 100;
  ch->position     = POS_STANDING;
  ch->hit          = 20;
  ch->max_hit      = 20;
  ch->mana         = 100;
  ch->max_mana     = 100;
  ch->move         = 100;
  ch->max_move     = 100;
  ch->on           = NULL;
  for (i = 0; i < MAX_STATS; i ++)
  {
    ch->perm_stat[i] = 13; 
    ch->mod_stat[i] = 0;
  }
}

/* Get an extra description from a list. */
const char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
  for ( ; ed != NULL; ed = ed->next )
  {
    if ( is_name( name, ed->keyword ) ) return ed->description;
  }
  return NULL;
}

/* Translates mob virtual number to its mob index struct.
 * Hash table lookup. */
MOB_INDEX_DATA *get_mob_index( int64 vnum )
{
  MOB_INDEX_DATA *pMobIndex;

  for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
        pMobIndex != NULL;
        pMobIndex  = pMobIndex->next )
  {
    if ( pMobIndex->vnum == vnum ) return pMobIndex;
  }

  if ( fBootDb )
  {
    bug( "Get_mob_index: bad vnum %u.", vnum );
    exit( 1 );
  }
  return NULL;
}

/* Translates mob virtual number to its obj index struct.
 * Hash table lookup. */
OBJ_INDEX_DATA *get_obj_index( int64 vnum )
{
  OBJ_INDEX_DATA *pObjIndex;

  for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
        pObjIndex != NULL;
        pObjIndex  = pObjIndex->next )
  {
    if ( pObjIndex!=NULL && pObjIndex->vnum == vnum ) return pObjIndex;
  }

  if ( fBootDb )
  {
    bug( "Get_obj_index: bad vnum %u.", vnum );
    exit( 1 );
  }
  return NULL;
}

/* Translates mob virtual number to its room index struct.
 * Hash table lookup. */
ROOM_INDEX_DATA *get_room_index( int64 vnum )
{
  ROOM_INDEX_DATA *pRoomIndex;

  for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
        pRoomIndex != NULL;
        pRoomIndex  = pRoomIndex->next )
  {
    if ( pRoomIndex && pRoomIndex->vnum == vnum ) return pRoomIndex;
// Dirty check - only temporary. Sab
  }

  if ( fBootDb )
  {
    bug( "Get_room_index: bad vnum %u.", vnum );
    return get_room_index(ROOM_VNUM_TEMPLE);
  }
  return NULL;
}

/* Read a letter from a file. */
char fread_letter( FILE *fp )
{
  char c;

  do
  {
    c = getc( fp );
  }
  while ( isspace(c) );

  return c;
}

// Read a number from a file.
int fread_number( FILE *fp )
{
  int number;
  bool sign;
  char c;
  int pos;

  do
  {
    c = getc( fp );
  }
  while ( isspace(c) );
  pos=ftell ( fp );

  number = 0;

  sign   = FALSE;
  if ( c == '+' )
  {
    c = getc( fp );
  }
  else if ( c == '-' )
  {
    sign = TRUE;
    c = getc( fp );
  }

  if ( !isdigit(c) )
  {
    log_printf( "Fread_number: bad format '%c' at offset %d",c,pos);
    do
    {
      c = getc( fp );
    }
    while ( isspace(c) );
//    exit (1);//return 0;
  }

  while ( isdigit(c) )
  {
    number = number * 10 + c - '0';
    c      = getc( fp );
  }

  if ( sign ) number = 0 - number;

  if ( c == '|' ) number += fread_number( fp );
  else if ( c != ' ' ) ungetc( c, fp );

  return number;
}

int64 fread_number64( FILE *fp )
{
  int64 number;
  bool sign;
  char c;
  int pos;

  do
  {
    c = getc( fp );
  }
  while ( isspace(c) );
  pos = ftell ( fp );

  number = 0;

  sign   = FALSE;
  if ( c == '+' )
  {
    c = getc( fp );
  }
  else if ( c == '-' )
  {
    sign = TRUE;
    c = getc( fp );
  }

  if ( !isdigit(c) )
  {
    log_printf( "Fread_number64: bad format '%c' at offset %d",c,pos);
    exit (1);//return 0;
  }

  while ( isdigit(c) )
  {
    number = number * 10 + c - '0';
    c      = getc( fp );
  }

  if ( sign ) number = 0 - number;

  if ( c == '|' ) number += fread_number64( fp );
  else if ( c != ' ' ) ungetc( c, fp );
  return number;
}

long fread_flag( FILE *fp)
{
  int number;
  char c;
  bool negative = FALSE;

  do
  {
    c = getc(fp);
  }
  while ( isspace(c));

  if (c == '-')
  {
    negative = TRUE;
    c = getc(fp);
  }

  number = 0;

  if (!isdigit(c))
  {
    while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
    {
      number+=flag_convert(c);
      c = getc(fp);
    }
  }

  while (isdigit(c))
  {
    number = number * 10 + c - '0';
    c = getc(fp);
  }

  if (c == '|') number += fread_flag(fp);
  else if  ( c != ' ') ungetc(c,fp);

  if (negative) return -1 * number;
  return number;
}

long flag_convert(char letter )
{
  long bitsum = 0;
  char i;

  if ('A' <= letter && letter <= 'Z') 
  {
    bitsum = 1;
    for (i = letter; i > 'A'; i--) bitsum *= 2;
  }
  else if ('a' <= letter && letter <= 'z')
  {
    bitsum = 67108864; /* 2^26 */
    for (i = letter; i > 'a'; i --) bitsum *= 2;
  }
  return bitsum;
}

int64 flag_convert64(char letter )
{
  int64 bitsum = 0;
  char i;

  if ('A' <= letter && letter <= 'Z')
  {
    bitsum = 1;
    for (i = letter; i > 'A'; i--) bitsum *= 2;
  }
  else if ('a' <= letter && letter <= 'z')
  {
    bitsum = 67108864; /* 2^26 */
    for (i = letter; i > 'a'; i --) bitsum *= 2;
  }
  return bitsum;
}

// Read and allocate space for a string from a file.
// These strings are read-only and shared.
// Strings are hashed:
//   each string prepended with hash pointer to prev string,
//   hash code is simply the string length.
//   this function takes 40% to 50% of boot-up time.
const char *fread_string( FILE *fp )
{
  char *plast;
  char c;
  int chy;

  plast = top_string + sizeof(char *);
  if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
  {
    bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
    exit (1);//return NULL;
  }
  /* Skip blanks. Read first char.*/
  do
  {
    c = getc( fp );
  }
  while ( isspace(c) );

  if ( ( *plast++ = c ) == '~' ) return &str_empty[0];

  for ( ;; )
  { 
    /* Back off the char type lookup, it was too dirty for portability.
     *   -- Furey */
    *plast=chy=getc(fp);
    switch(chy)
    {
      default:
        plast++;
        break;
      case EOF:
        bug( "Fread_string: EOF", 0 );
        return NULL;
        break;
      case '\n':
        plast++;
        *plast++ = '\r';
        break;
      case '\r':
        break;
      case '~':
        plast++;
        {
          union
          {
            char *      pc;
            char        rgc[sizeof(char *)];
          } u1;
          int ic;
          int iHash;
          char *pHash;
          char *pHashPrev;
          char *pString;

          plast[-1] = '\0';
          iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
          for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
          {
            for ( ic = 0; ic < sizeof(char *); ic++ ) u1.rgc[ic] = pHash[ic];
            pHashPrev = u1.pc;
            pHash    += sizeof(char *);

            if ( top_string[sizeof(char *)] == pHash[0]
             &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
             return pHash;
          }

          if ( fBootDb )
          {
            pString             = top_string;
            top_string          = plast;
            u1.pc               = string_hash[iHash];
            for ( ic = 0; ic < sizeof(char *); ic++ ) pString[ic] = u1.rgc[ic];
            string_hash[iHash]  = pString;

            nAllocString += 1;
            sAllocString += top_string - pString;
            return pString + sizeof(char *);
          }
          else
          {
            return str_dup( top_string + sizeof(char *) );
          }
        }
    }
  }
}

const char *fread_string_eol( FILE *fp )
{
  static bool char_special[256-EOF];
  char *plast;
  char c;
 
  if ( char_special[EOF-EOF] != TRUE )
  {
    char_special[EOF -  EOF] = TRUE;
    char_special['\n' - EOF] = TRUE;
    char_special['\r' - EOF] = TRUE;
  }
 
  plast = top_string + sizeof(char *);
  if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
  {
    bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
    exit (1);//return NULL;
  }
 
  /* Skip blanks.
   * Read first char. */
  do
  {
    c = getc( fp );
  }
  while ( isspace(c) );
 
  if ( ( *plast++ = c ) == '\n') return &str_empty[0];
 
  for ( ;; )
  {
    if ( !char_special[ ( *plast++ = getc( fp ) ) - EOF ] ) continue;
 
    switch ( plast[-1] )
    {
      default:
        break;
      case EOF:
        bug( "Fread_string_eol  EOF", 0 );
        exit( 1 );
        break;
      case '\n':  case '\r':
        {
          union
          {
            char *      pc;
            char        rgc[sizeof(char *)];
          } u1;
          int ic;
          int iHash;
          char *pHash;
          char *pHashPrev;
          char *pString;
 
          plast[-1] = '\0';
          iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
          for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
          {
            for ( ic = 0; ic < sizeof(char *); ic++ )
             u1.rgc[ic] = pHash[ic];
            pHashPrev = u1.pc;
            pHash    += sizeof(char *);
 
            if ( top_string[sizeof(char *)] == pHash[0]
             &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
             return pHash;
          }
 
          if ( fBootDb )
          {
            pString             = top_string;
            top_string          = plast;
            u1.pc               = string_hash[iHash];
            for ( ic = 0; ic < sizeof(char *); ic++ ) pString[ic] = u1.rgc[ic];
            string_hash[iHash]  = pString;
 
            nAllocString += 1;
            sAllocString += top_string - pString;
            return pString + sizeof(char *);
          }
          else
          {
            return str_dup( top_string + sizeof(char *) );
          }
        }
      }
    }
}

/* Read to end of line (for comments). */
void fread_to_eol( FILE *fp )
{
  char c;

  do
  {
    c = getc( fp );
  }
  while ( c != '\n' && c != '\r' );

  do
  {
    c = getc( fp );
  }
  while ( c == '\n' || c == '\r' );

  ungetc( c, fp );
  return;
}

/* Read one word (into static buffer).*/
const char *fread_word( FILE *fp )
{
  static char word[MAX_INPUT_LENGTH];
  char *pword;
  char cEnd;
  int pos;

  do { cEnd = getc( fp );}
  while ( isspace( cEnd ) );
  
  pos = ftell ( fp );

  if ( cEnd == '\'' || cEnd == '"' )
  {
      pword   = word;
  }
  else
  {
      word[0] = cEnd;
      pword   = word+1;
      cEnd    = ' ';
  }

  for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
  {
      *pword = getc( fp );

      if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
      {
          if ( cEnd == ' ' ) ungetc( *pword, fp );
          *pword = '\0';
          return word;
      }
  }

  log_printf("Fread word error at offset %d",pos);
  bug( "Fread_word: word too long.", 0 );
//  exit (1);
  return NULL;
}

int allocated = 0;

/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    void *pMem;
    int iList;

#ifdef MAGIC_CHECKING
    sMem += sizeof(struct magic);
#endif

    for (iList = 0; iList < MAX_MEM_LIST; iList++)
    {
        if (sMem <= rgSizeList[iList])
            break;
    }

    if (iList == MAX_MEM_LIST)
    {
        bug("Alloc_mem: size %d too large.", sMem);
        exit(1);
    }

    if (! rgFreeList[iList])
    {
        pMem = alloc_perm( rgSizeList[iList] );
    }
    else
    {
        pMem = rgFreeList[iList];
        rgFreeList[iList] = * ((void **) rgFreeList[iList]);
    }

#ifdef MAGIC_CHECKING
    MAGIC(pMem)->cookie = MAGIC_COOKIE;
    MAGIC(pMem)->size = sMem - sizeof(struct magic);
    pMem += sizeof(struct magic);  //MAGIC(pMem)++;
#endif

    allocated+=sMem;
    return pMem;
}



/* Free some memory.
 * Recycle it back onto the free list for blocks of that size. */
void free_mem( void *pMem, int sMem )
{
  int iList;

#ifdef MAGIC_CHECKING
  pMem -= sizeof(struct magic);  //MAGIC (pMem)--;
  
  if (MAGIC (pMem)->cookie != MAGIC_COOKIE)
    {
      char tmp[MAX_STRING_LENGTH];
#ifdef unix
      snprintf (tmp, sizeof(tmp), "Attempt to recycle invalid memory: %s",
#else
      printf (tmp, sizeof(tmp), "Attempt to recycle invalid memory: %s",
#endif
                      MAGIC (pMem)->data);
      return;
    }
  else if (MAGIC (pMem)->size != sMem)
    {
      char tmp[MAX_STRING_LENGTH];
#ifdef unix
      snprintf (tmp, sizeof(tmp), "Attempt to free memory with illegal size: "
#else
      printf (tmp, sizeof(tmp), "Attempt to free memory with illegal size: "
#endif
                "%d != %d (this can be fatal): %s",
                MAGIC (pMem)->size, sMem, MAGIC (pMem)->data);
      bug (tmp, 0);
      sMem = MAGIC (pMem)->size;
    }
 
  MAGIC (pMem)->cookie = 0;
  sMem += sizeof (struct magic);
#endif
  for (iList = 0; iList < MAX_MEM_LIST; iList++)
  {
    if (sMem <= rgSizeList[iList]) break;
  }

  if (iList == MAX_MEM_LIST)
  {
    bug("Free_mem: size %d too large.", sMem);
    exit(1);
  }

  * ((void **) pMem) = rgFreeList[iList];
  rgFreeList[iList]  = pMem;
    
  allocated-=sMem;
}

/* Allocate some permanent memory.Permanent memory is never freed,
 *   pointers into it may be copied safely. */
void *alloc_perm( int sMem )
{
  static char *pMemPerm;
  static int iMemPerm;
  void *pMem;

  while ( sMem % sizeof(long) != 0 ) sMem++;
  if ( sMem > MAX_PERM_BLOCK )
  {
    bug( "Alloc_perm: %d too large.", sMem );
    exit( 1 );
  }

  if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
  {
    iMemPerm = 0;
    if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
    {
      perror( "Alloc_perm" );
      exit( 1 );
    }
  }
  pMem        = pMemPerm + iMemPerm;
  iMemPerm   += sMem;
  nAllocPerm += 1;
  sAllocPerm += sMem;
  return pMem;
}

// Duplicate a string into dynamic memory.
// Fread_strings are read-only and shared.
const char * str_dup(const char *str)
{
  char *str_new;

  if (!str || str[0] == '\0') return (str_empty);

  str_new = alloc_mem(strlen(str) + 1);
  strcpy (str_new, str);
  return (str_new);
}

// Free a string.
// Null is legal here to simplify callers.
// Read-only shared strings are not touched.
void free_string( const char *pstr )
{
  if ( pstr == NULL || pstr == &str_empty[0]
    || ( pstr >= string_space && pstr < top_string ) ) return;

  free_mem( (void *)pstr, strlen(pstr) + 1 );
}

void do_areas( CHAR_DATA *ch, char *argument )
{
  AREA_DATA *pArea1;
  AREA_DATA *pArea2;
  int iArea;
  int iAreaHalf;

  if (argument[0] != '\0')
  {
    stc("No argument is used with this command.\n\r",ch);
    return;
  }

  iAreaHalf = (top_area + 1) / 2;
  pArea1    = area_first;
  pArea2    = area_first;
  for ( iArea = 0; iArea < iAreaHalf; iArea++ ) pArea2 = pArea2->next;

  for ( iArea = 0; iArea < iAreaHalf; iArea++ )
  {
    ptc(ch, "%-39s%-39s\n\r",
      pArea1->credits, (pArea2 != NULL) ? pArea2->credits : "" );
      pArea1 = pArea1->next;
      if ( pArea2 != NULL ) pArea2 = pArea2->next;
  }
}

void do_memory( CHAR_DATA *ch, char *argument )
{
  int count, used, inroom;
  CHAR_DATA *wch;
  DESCRIPTOR_DATA *d;

  if (EMPTY(argument))
  {
    ptc(ch,"+===========================================================+\n\r");
    ptc(ch,"| Areas : {Y%3d{x | t_room: {Y%5u{x | Exits : {Y%4d{x | Shops  : {Y%3d{x |\n\r", top_area, top_room, top_exit, top_shop);
    ptc(ch,"| Helps : {Y%3d{x | ExDesk: {Y%5d{x | Resets: {Y%4d{x | Socials: {Y%3d{x |\n\r", top_help, top_ed, top_reset, maxsocial);
    ptc(ch,"+===========================================================+\n\r");
    ptc(ch,"| Strings: {Y%8d bytes {xin {Y%8d strings{x. {GMax %8d{x |\n\r", sAllocString, nAllocString, MAX_STRING);
    ptc(ch,"| Perms  : {Y%8d bytes {xin {Y%8d blocks{x.               |\n\r", sAllocPerm, nAllocPerm);
    ptc(ch,"| Mobiles: {Gindexes: {Y%6u {Gnew format: {Y%6d {Gin use {Y%6d{x |\n\r", top_mob_index, newmobs, mobile_count);
    ptc(ch,"| Objects: {Gindexes: {Y%6u {Gnew format: {Y%6d {Gin use {Y%6d{x |\n\r", top_obj_index,newobjs,0);
    ptc(ch,"| Affects {Y%6d{x                                            |\n\r", top_affect    );
    ptc(ch,"+===========================================================+\n\r");

    count=used=0;
    for(d=descriptor_list;d;d=d->next)
    {
      count++;
      if (d->character) used++;
    }
    ptc(ch,"| Descriptors: {Y%5d{x In use : {Y%5d{x                         |\n\r", count, used);

    count=used=inroom=0;
    for(wch=char_list;wch;wch=wch->next)
    {
      count++;
      if (!IS_NPC(wch)) used++;
      if (wch->in_room) inroom++;
    }
    ptc(ch,"| Characters : {Y%5d{x Players: {Y%5d{x In rooms: {Y%5d{x         |\n\r", count, used, inroom);
    ptc(ch,"+===========================================================+\n\r");
    ptc(ch,"|  {Ccnt_char: {Y%5d   {Ccnt_pcdata: {Y%5d   {Ccnt_descr: {Y%5d{x   |\n\r", cnt_char, cnt_pcdata, cnt_descr);
    ptc(ch,"|  {Ccnt_obj : {Y%5d   {Ccnt_aff   : {Y%5d   {Ccnt_raff : {Y%5d{x   |\n\r", cnt_obj, cnt_aff, cnt_raff);
    ptc(ch,"|  {Ccnt_buff: {Y%5d                                          {x|\n\r", cnt_buff);
    ptc(ch,"+===========================================================+\n\r");
    return;
  }
  if (!str_prefix(argument,"used"))
  {
     int pc;
     ROOM_INDEX_DATA *room;
     int64 mem;
     OBJ_DATA *obj;

     ptc(ch,"Данные далеко не полные, только примерные.Sab.\n\r");
     ptc(ch,"Sizes of structures:\n\r");
     ptc(ch,"GQ_DATA          %6d     OBJ_DATA         %d\n\r",sizeof(GQ_DATA),sizeof(OBJ_DATA));          
     ptc(ch,"AFFECT_DATA      %6d     OBJ_INDEX_DATA   %d\n\r",sizeof(AFFECT_DATA),sizeof(OBJ_INDEX_DATA));    
     ptc(ch,"VOTE_DATA        %6d     VICTIM_DATA      %d\n\r",sizeof(VOTE_DATA),sizeof(VICTIM_DATA));       
     ptc(ch,"OFFER_DATA       %6d     PC_DATA          %d\n\r",sizeof(OFFER_DATA),sizeof(PC_DATA));           
     ptc(ch,"VOTE_CHAR        %6d     GEN_DATA         %d\n\r",sizeof(VOTE_CHAR),sizeof(GEN_DATA));          
     ptc(ch,"PENALTY_DATA     %6d     RESET_DATA       %d\n\r",sizeof(PENALTY_DATA),sizeof(RESET_DATA));        
     ptc(ch,"AREA_DATA        %6d     ROOM_INDEX_DATA  %d\n\r",sizeof(AREA_DATA),sizeof(ROOM_INDEX_DATA));   
     ptc(ch,"BAN_DATA         %6d     SHOP_DATA        %d\n\r",sizeof(BAN_DATA),sizeof(SHOP_DATA));         
     ptc(ch,"BUFFER           %6d     TIME_INFO_DATA   %d\n\r",sizeof(BUFFER),sizeof(TIME_INFO_DATA));    
     ptc(ch,"CHAR_DATA        %6d     WEATHER_DATA     %d\n\r",sizeof(CHAR_DATA),sizeof(WEATHER_DATA));      
     ptc(ch,"DESCRIPTOR_DATA  %6d     MPROG_LIST       %d\n\r",sizeof(DESCRIPTOR_DATA),sizeof(MPROG_LIST));        
     ptc(ch,"EXIT_DATA        %6d     MPROG_CODE       %d\n\r",sizeof(EXIT_DATA),sizeof(MPROG_CODE));        
     ptc(ch,"EXTRA_DESCR_DATA %6d     CLAN_DATA        %d\n\r",sizeof(EXTRA_DESCR_DATA),sizeof(CLAN_DATA));         
     ptc(ch,"HELP_DATA        %6d     TOURNAMENT_DATA  %d\n\r",sizeof(HELP_DATA),sizeof(TOURNAMENT_DATA));   
     ptc(ch,"KILL_DATA        %6d     BET_DATA         %d\n\r",sizeof(KILL_DATA),sizeof(BET_DATA));          
     ptc(ch,"MEM_DATA         %6d     RAFFECT          %d\n\r",sizeof(MEM_DATA),sizeof(RAFFECT));           
     ptc(ch,"MOB_INDEX_DATA   %6d     GQUEST_DATA      %d\n\r",sizeof(MOB_INDEX_DATA),sizeof(GQUEST_DATA));       
     ptc(ch,"NOTE_DATA        %6d     AUCTION_DATA     %d\n\r",sizeof(NOTE_DATA),sizeof(AUCTION_DATA));      
     ptc(ch,"NEWS_DATA        %6d\n\r\n\r",sizeof(NEWS_DATA));

     count=used=0;
     for(d=descriptor_list;d;d=d->next)
     {
       count++;
       if (d->character) used++;
     }
     ptc(ch,"Descriptor_data:   %5d (%d bytes), used %d\n\r", count, count*sizeof(DESCRIPTOR_DATA), used);


     count=used=0;
     mem=0;
     for (count=0;count<top_room;count++)
     {
       room=get_room_index(count);
       if (!room) continue;
       used++;
       mem+=sizeof(ROOM_INDEX_DATA);
       mem+=sizeof(room->name);
       mem+=sizeof(room->description);
       mem+=sizeof(room->owner);
     }
     ptc(ch,"Room_Index_Data:   %5d (%d bytes), mem used: %u bytes\n\r", used,used*sizeof(ROOM_INDEX_DATA),mem);

     count=used=0;
     for (count=0;count<top_mob_index;count++) if (get_mob_index(count)) used++;
     ptc(ch,"MobIndexData:      %5d (%d bytes)\n\r", used,used*sizeof(MOB_INDEX_DATA));

     count=pc=0;
     mem=0;
     for(wch=char_list;wch;wch=wch->next)
     {
       count++;
       mem+=sizeof(CHAR_DATA);
       mem+=sizeof(wch->name);
       mem+=sizeof(wch->short_descr);
       mem+=sizeof(wch->long_descr);
       mem+=sizeof(wch->description);
       mem+=sizeof(wch->prompt);
       mem+=sizeof(wch->prefix);
       mem+=sizeof(wch->stealer);
       mem+=sizeof(wch->host);
       mem+=sizeof(wch->runbuf);
       if (!IS_NPC(wch))
       {
         pc++;
         mem+=sizeof(PC_DATA);
         mem+=sizeof(wch->pcdata->pseudoname);
         mem+=sizeof(wch->pcdata->run);
         mem+=sizeof(wch->pcdata->vislist);
         mem+=sizeof(wch->pcdata->ignorelist);
         mem+=sizeof(wch->pcdata->pwd);
         mem+=sizeof(wch->pcdata->bamfin);
         mem+=sizeof(wch->pcdata->bamfout);
         mem+=sizeof(wch->pcdata->title);
         mem+=sizeof(wch->pcdata->auto_online);
         mem+=sizeof(wch->pcdata->pkillers);
         mem+=sizeof(wch->pcdata->marry);
         mem+=sizeof(wch->pcdata->lovers);
         mem+=sizeof(wch->pcdata->propose_marry);
         mem+=sizeof(wch->pcdata->mother);
         mem+=sizeof(wch->pcdata->father);
         mem+=sizeof(wch->pcdata->kins);
         mem+=sizeof(wch->pcdata->deny_addr);
         mem+=sizeof(wch->pcdata->denied_by);
         mem+=sizeof(wch->pcdata->runbuf);
         mem+=sizeof(wch->pcdata->tickstr);
       }
     }
     ptc(ch,"Char_data:         %5d (%d bytes), pcdata: %d (%d bytes), mem used %u\n\r", count, count*sizeof(CHAR_DATA), pc,pc*sizeof(PC_DATA),mem);

     count=used=0;
     for (count=0;count<top_obj_index;count++) if (get_obj_index(count)) used++;
     ptc(ch,"ObjIndexData:      %5d (%d bytes)\n\r", used,used*sizeof(OBJ_INDEX_DATA));

     count=0;
     mem=0;
     for (obj=object_list;obj->next;obj=obj->next)
     {
       count++;
       mem+=sizeof(OBJ_DATA);
       mem+=sizeof(obj->owner);
       mem+=sizeof(obj->name);
       mem+=sizeof(obj->short_descr);
       mem+=sizeof(obj->description);
       mem+=sizeof(obj->material);
       mem+=sizeof(obj->loaded_by);
     }
     ptc(ch,"Obj_data:          %5d (%d bytes), mem used %u\n\r", count,count*sizeof(OBJ_INDEX_DATA),mem);
  }
}

void do_dump( CHAR_DATA *ch, char *argument )
{
  int64 count,count2,num_pcs,aff_count;
  CHAR_DATA *fch;
  MOB_INDEX_DATA *pMobIndex;
  PC_DATA *pc;
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObjIndex;
  ROOM_INDEX_DATA *room=NULL;
  EXIT_DATA *exit=NULL;
  DESCRIPTOR_DATA *d;
  AFFECT_DATA *af;
  FILE *fp;
  int64 vnum,nMatch = 0;
  int64 summary;

  /* open file */
  fclose(fpReserve);
  fp = fopen("mem.dmp","w");

  /* report use of data structures */
  num_pcs = 0;
  aff_count = 0;

  /* mobile prototypes */
  do_fprintf(fp,"MobProt %5u (%9u bytes)\n", top_mob_index, top_mob_index * (sizeof(*pMobIndex)));
  summary=top_mob_index * (sizeof(*pMobIndex));

  /* mobs */
  count = 0;  count2 = 0;
  for (fch = char_list; fch != NULL; fch = fch->next)
  {
    count++;
    if (fch->pcdata != NULL) num_pcs++;
    for (af = fch->affected; af != NULL; af = af->next) aff_count++;
  }
  for (fch = char_free; fch != NULL; fch = fch->next) count2++;
  do_fprintf(fp,"Mobs    %5d (%9d bytes), %3d free (%d bytes)\n",
    count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)));
  summary+=count * (sizeof(*fch));
  summary+= count2 * (sizeof(*fch));

  /* pcdata */
  count = 0;
  for (pc = pcdata_free; pc != NULL; pc = pc->next) count++; 

  do_fprintf(fp,"Pcdata  %5u (%9d bytes), %3u free (%d bytes)\n",
    num_pcs, num_pcs * (sizeof(*pc)), count, count * (sizeof(*pc)));
  summary+= count * (sizeof(*pc));

  /* descriptors */
  count = 0; count2 = 0;
  for (d = descriptor_list; d != NULL; d = d->next) count++;
  for (d= descriptor_free; d != NULL; d = d->next) count2++;

  do_fprintf(fp, "Descs  %5d (%9d bytes), %3d free (%d bytes)\n",
    count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));
  summary+=count * (sizeof(*d));
  summary+=count2 * (sizeof(*d));

  /* object prototypes */
  for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
    {
      for (af = pObjIndex->affected; af != NULL; af = af->next) aff_count++;
      nMatch++;
    }
    do_fprintf(fp,"ObjProt %5u (%9u bytes)\n",
        top_obj_index, top_obj_index * (sizeof(*pObjIndex)));
    summary+=top_obj_index * (sizeof(*pObjIndex));

    /* objects */
    count = 0;  count2 = 0;
    for (obj = object_list; obj != NULL; obj = obj->next)
    {
      count++;
      for (af = obj->affected; af != NULL; af = af->next) aff_count++;
    }
    for (obj = obj_free; obj != NULL; obj = obj->next) count2++;

    do_fprintf(fp,"Objs    %5d (%9d bytes), %3d free (%d bytes)\n",
        count, count * (sizeof(*obj)), count2, count2 * (sizeof(*obj)));
    summary+=count * (sizeof(*obj));
    summary+=count2 * (sizeof(*obj));

    /* affects */
    count = 0;
    for (af = affect_free; af != NULL; af = af->next) count++;

    do_fprintf(fp,"Affects %5d (%9d bytes), %3d free (%d bytes)\n",
        aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)));
    summary+=aff_count * (sizeof(*af));
    summary+=count * (sizeof(*af));

    /* rooms */
    do_fprintf(fp,"Rooms   %5u (%9d bytes)\n",top_room,top_room *(sizeof(*room)));
    summary+=top_room *(sizeof(*room));

     /* exits */
    do_fprintf(fp,"Exits   %5d (%9d bytes)\n",top_exit,top_exit *(sizeof(*exit)));
    summary+=top_exit *(sizeof(*exit));
    do_fprintf(fp, "--------------------------------\n");
    do_fprintf(fp, "Summary: %u bytes.\n", summary);
    fclose(fp);

    /* start printing out mobile data */
    fp = fopen("mob.dmp","w");

    do_fprintf(fp,"\nMobile Analysis\n");
    do_fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_mob_index; vnum++)
      if ((pMobIndex = get_mob_index(vnum)) != NULL)
      {
        nMatch++;
        do_fprintf(fp,"#%-5u %3d active %3d killed     %s\n",
         pMobIndex->vnum,pMobIndex->count,
         pMobIndex->killed,pMobIndex->short_descr);
      }
    fclose(fp);

    /* start printing out object data */
    fp = fopen("obj.dmp","w");

    do_fprintf(fp,"\nObject Analysis\n");
    do_fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_obj_index; vnum++)
      if ((pObjIndex = get_obj_index(vnum)) != NULL)
      {
        nMatch++;
        do_fprintf(fp,"#%-5u %3d active %5d reset      %s\n",
          pObjIndex->vnum,pObjIndex->count,
          pObjIndex->reset_num,pObjIndex->short_descr);
      }

    /* close file */
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
}


/* Stick a little fuzz on a number. */
int number_fuzzy( int number )
{
  switch ( number_bits( 2 ) )
  {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
  }

  return UMAX( 1, number );
}

/* Generate a random number. */
int number_range( int from, int to )
{
/*
  int power;
  int number;

  if (from == 0 && to == 0) return 0;

*/
  if ( ( to = to - from + 1 ) <= 1 ) return from;
/*
  for ( power = 2; power < to; power <<= 1 ) ;

  while ( ( number = number_mm() & (power -1 ) ) >= to ) ;

  return from + number;
*/
  if (to==0) return 0;
  return (number_mm() % to) + from;
}

/* Generate a percentile roll. */
int number_percent( void )
{
/*
  int percent;

  while ( (percent = number_mm() & (128-1) ) > 99 ) ;

  return 1 + percent;
*/
  return number_mm() % 100;
}


 /* Generate a random door. */
int number_door( void )
{
  int door;

  while ( ( door = number_mm() & (8-1) ) > 5) ;

  return door;
}

int number_bits( int width )
{
  return number_mm( ) & ( ( 1 << width ) - 1 );
}

/* I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey */

/* I noticed streaking with this random number generator, so I switched
   back to the system srandom call.  If this doesn't work for you, 
   define OLD_RAND to use the old system -- Alander */

#if defined (OLD_RAND)
static  int     rgiState[2+55];
#endif
 
void init_mm( )
{
#if defined (OLD_RAND)
  int *piState;
  int iState;
 
  piState     = &rgiState[2];
 
  piState[-2] = 55 - 55;
  piState[-1] = 55 - 24;
 
  piState[0]  = ((int) current_time) & ((1 << 30) - 1);
  piState[1]  = 1;
  for ( iState = 2; iState < 55; iState++ )
  {
    piState[iState] = (piState[iState-1] + piState[iState-2])
      & ((1 << 30) - 1);
  }
#else
  srandom(time(NULL)^getpid());
#endif
  return;
}
  
long number_mm( void )
{
#if defined (OLD_RAND)
  int *piState;
  int iState1;
  int iState2;
  int iRand;
 
  piState             = &rgiState[2];
  iState1             = piState[-2];
  iState2             = piState[-1];
  iRand               = (piState[iState1] + piState[iState2])
                        & ((1 << 30) - 1);
  piState[iState1]    = iRand;
  if ( ++iState1 == 55 ) iState1 = 0;
  if ( ++iState2 == 55 ) iState2 = 0;
  piState[-2]         = iState1;
  piState[-1]         = iState2;
  return iRand >> 6;
#else
  return random() >> 6;
#endif
}


/* Roll some dice.*/
int dice( int number, int size )
{
  int idice;
  int sum;

  switch ( size )
  {
    case 0: return 0;
    case 1: return number;
  }

  for ( idice = 0, sum = 0; idice < number; idice++ ) sum += number_range( 1, size );
  return sum;
}

/* Simple linear interpolation.*/
int interpolate( int level, int value_00, int value_32 )
{
  return value_00 + level * (value_32 - value_00) / 32;
}


// Removes the tildes from a string.
// Used for player-entered strings that go into disk files.
void smash_tilde( char *str )
{
  for ( ; *str != '\0'; str++ )
  {
    if ( *str == '~' ) *str = '-';
  }
}

void smash_percent( char *str )
{
  for ( ; *str != '\0'; str++ )
  {
    if ( *str == '%' ) *str= 'x';
  }
}

void smash_beep( char *str, bool total )
{
  register int counter=0;

  for ( ; *str != '\0'; str++ )
  {
    if ( *str == '{' && *(str+1)=='*' )
    {
      counter++;
      if (total || counter>3) *str='(';
    }
  }
}

void smash_newline( char *str )
{
  for ( ; *str != '\0'; str++ )
   if ( *str == '{' && *(str+1)=='/' ) 
    *str='(';
}

// Compare strings, case insensitive.
// Return TRUE if different
//   (compatibility with historical functions).
bool str_cmp( const char *astr, const char *bstr )
{
  if ( astr == NULL )
  {
    bug( "Str_cmp: null astr.", 0 );
    return TRUE;
  }

  if ( bstr == NULL )
  {
    bug( "Str_cmp: null bstr.", 0 );
    return TRUE;
  }

  for ( ; *astr || *bstr; astr++, bstr++ )
  {
    if ( LOWER(*astr) != LOWER(*bstr) ) return TRUE;
  }
  return FALSE;
}

/* Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
  if ( astr == NULL )
  {
    bug( "Strn_cmp: null astr.", 0 );
    return TRUE;
  }

  if ( bstr == NULL )
  {
    bug( "Strn_cmp: null bstr.", 0 );
    return TRUE;
  }

  for ( ; *astr; astr++, bstr++ )
  {
    if ( LOWER(*astr) != LOWER(*bstr) ) return TRUE;
  }

  return FALSE;
}

/* Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
  int sstr1;
  int sstr2;
  int ichar;
  char c0;

  if ( ( c0 = LOWER(astr[0]) ) == '\0' ) return FALSE;

  sstr1 = strlen(astr);
  sstr2 = strlen(bstr);

  for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
  {
    if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) ) return FALSE;
  }

  return TRUE;
}

/* Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
  int sstr1;
  int sstr2;

  sstr1 = strlen(astr);
  sstr2 = strlen(bstr);
  if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) ) return FALSE;
  else return TRUE;
}

/* Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
  static char strcap[MAX_STRING_LENGTH];
  int i;

  for ( i = 0; str[i] != '\0'; i++ ) strcap[i] = LOWER(str[i]);
  strcap[i] = '\0';
  strcap[0] = UPPER(strcap[0]);
  return strcap;
}

/* Append a string to a file. */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
  FILE *fp;

  if ( IS_NPC(ch) || str[0] == '\0' ) return;

  fclose( fpReserve );
  if ( ( fp = fopen( file, "a" ) ) == NULL )
  {
    perror( file );
    stc( "Could not open the file!\n\r", ch );
  }
  else
  {
    do_fprintf( fp, "[%5d] %s: %s\n",
      ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
    fclose( fp );
  }

  fpReserve = fopen( NULL_FILE, "r" );
  return;
}

// Reports a bug.
void bug( const char *str, int64 param )
{
  if ( str == NULL )
  {
    bug("NULL parameter for BUG!",0);
    return;
  }

  if ( fpArea != NULL )
  {
    int iLine;
    int iChar;

    if ( fpArea == stdin ) iLine = 0;
    else
    {
      iChar = ftell( fpArea );
      fseek( fpArea, 0, 0 );
      for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
      {
        while ( getc( fpArea ) != '\n' ) ;
      }
      fseek( fpArea, iChar, 0 );
    }
    log_printf( "[***] FILE: %s LINE: %d", strArea, iLine);
  }
#ifdef WITH_ANTICRASH
  else if(is_set(global_cfg, CFG_BUGTRACE)) {
          log_printf(gdb_ipc("gdbcmd 'bt'"));
  }
#endif
  log_printf( "[*****] BUG: %s %u", str, param);
}

// Writes a string to the log
void log_string( const char *str )
{
  FILE *fp;
  char tmp[MAX_STRING_LENGTH];

  strftime(tmp,19,"%d%m %a %H:%M:%S:",localtime(&current_time));
  strcat(tmp,str);
  strcat(tmp,"\n");
#if defined(WIN32)
  do_fprintf(stdout,tmp);
#else
  do_fprintf(stderr,tmp);
#endif
  fclose (logReserve);
  fp = fopen ("full.log", "a+b");
  if (fp != NULL)
  {
    do_fprintf(fp,tmp);
    fclose(fp);
  }
  logReserve = fopen (LOGR_FILE, "r");
}

// This function is here to aid in debugging.
// If the last expression in a function is another function call,
//   gcc likes to generate a JMP instead of a CALL.
// This is called "tail chaining."
// It hoses the debugger call stack for that call.
// So I make this the last call in certain critical functions,
//   where I really need the call stack to be right for debugging!
//
// If you don't understand this, then LEAVE IT ALONE.
// Don't remove any calls to tail_chain anywhere.
// -- Furey
void tail_chain( void )
{
    return;
}


// Removes the tildes from a string.
// Used for player-entered strings that go into disk files
void smash_flash( char *str )
{
  char *str1;

  for ( ; *str != '\0';)
  {
    str1=str;
    str++;
    if ( *str1 == '{' && *str=='+' )
    {
      *str = 'x';
    }
  }
}

// Removes the $-sign from a string. Used for wiznet and others acts()
void smash_dollar( char *str )
{
  for ( ; *str != '\0';str++) if ( *str == '$' ) *str = 'S';
}

void fix_weapons(void)
{
 OBJ_INDEX_DATA *obj;
 int i;
 for(i=0; i<MAX_KEY_HASH; i++)
  for (obj=obj_index_hash[i]; obj!=NULL; obj=obj->next)
 {
  if (obj->item_type==ITEM_WEAPON && obj->value[3]==0)
   switch (obj->value[1])
   {
    case WEAPON_DAGGER: 
    case WEAPON_WHIP:
    case WEAPON_SPEAR:
         obj->value[3] = 3;   /* slash*/
         break;
    case WEAPON_MACE:
    case WEAPON_FLAIL:  
    case WEAPON_POLEARM:
         obj->value[3] = 7;    /* pound*/
         break;
    case WEAPON_AXE:
    case WEAPON_SWORD:
         obj->value[3] = 3;    /* slash*/
    case WEAPON_EXOTIC:
    default:
         obj->value[3] = number_range(1,15);
         break;
   }
 }
}

random_weapon_name rnd_weapon_names[]=
{
{ {"клинок","посох","оружие", ""}, {1, 1, 3, 0} },
{ {"меч", "клинок", "палаш", "сабля", "ятаган", ""}, {1, 1, 1, 2, 1, 0} },
{ {"кинжал", "стилет", ""}, {1, 1, 0} },
{ {"копье", ""}, {3, 0} },
{ {"булава", "молот", "дубина", "палица", ""}, {2, 1, 2, 2, 0} },
{ {"топор", "секира", ""}, {1, 2, 0} },
{ {"цепь", ""}, {2, 0} },
{ {"плеть", "хлыст", ""}, {2, 1, 0} },
{ {"алебарда", ""}, {2, 0} },
{ {"посох", ""}, {1, 0} },
{ {""}, {0} }
};

random_flag_name rnd_flag_names[]=
{
 {{"{Bострый{x ", "{Bзаточенный{x ", ""}, WEAPON_SHARP, 1},
 {{"{Bострая{x ", "{Bзаточенная{x ", ""}, WEAPON_SHARP, 2},
 {{"{Bострое{x ", "{Bзаточенное{x ", ""}, WEAPON_SHARP, 3},
 
 {{"{Mпылающий{x ", "{Mогненный{x ", ""}, WEAPON_FLAMING, 1},
 {{"{Mпылающая{x ", "{Mогненная{x ", ""}, WEAPON_FLAMING, 2},
 {{"{Mпылающее{x ", "{Mогненное{x ", ""}, WEAPON_FLAMING, 3},

 {{"{Rкровавый{x ", ""}, WEAPON_VAMPIRIC, 1},
 {{"{Rкровавая{x ", ""}, WEAPON_VAMPIRIC, 2},
 {{"{Rкровавое{x ", ""}, WEAPON_VAMPIRIC, 3},

 {{"{wдвуручный{x ", ""}, WEAPON_TWO_HANDS, 1},
 {{"{wдвуручная{x ", ""}, WEAPON_TWO_HANDS, 2},
 {{"{wдвуручное{x ", ""}, WEAPON_TWO_HANDS, 3},

 {{"{wхолодный{x ", ""}, WEAPON_FROST, 1},
 {{"{wхолодная{x ", ""}, WEAPON_FROST, 2},
 {{"{wхолодное{x ", ""}, WEAPON_FROST, 3},

 {{"{Wблестящий{x ", "{Wсверкающий{x ", ""}, WEAPON_SHOCKING, 1},
 {{"{Wблестящая{x ", "{Wсверкающая{x ", ""}, WEAPON_SHOCKING, 2},
 {{"{Wблестящее{x ", "{Wсверкающее{x ", ""}, WEAPON_SHOCKING, 3},

 {{"{Gотравленный{x ", ""}, WEAPON_POISON, 1},
 {{"{Gотравленная{x ", ""}, WEAPON_POISON, 2},
 {{"{Gотравленное{x ", ""}, WEAPON_POISON, 3},
 {{""},                    0,             0}
};

random_suffix_name rnd_suffix_names[]=
{
 {" меткости", 1, 0, 0, 0},
 {" поражения", 16, 0, 0, 0},
 {" мастерства", 41, 0, 0, 0},

 {" повреждения", 0, 1, 0, 0},
 {" разрушения", 0, 11, 0, 0},
 {" смерти", 0, 36, 0, 0},

 {" силы", 0, 0, 1, 0},
 {" титанов", 0, 0, 5, 0},

 {" ловкости", 0, 0, 0, 1},
 {" скорости", 0, 0, 0, 5},
 {""          , 0, 0, 0, 0}
};

random_asuffix_name rnd_asuffix_names[]=
{
 {"{R Уничтожения", 125, 135},
 {"{R Испепеления", 136, 140},
 {"{R Армагеддона", 141, 155},
 {"{R Мастера", 156, 170},
 {"{M Преобладания", 171, 185},
 {"{M Власти", 186, 200},
 {"{W Богов", 201, 1000},
 {""         , 0  , 0   }
};

int get_random_pierce_message()
{
    int i;
    i=number_percent();
    if ( i > 66 )
        return 1;
    else if ( i > 20 )
        return 10;
    return 33;
}

int get_random_slash_message()
{
    int i;
    i=number_percent();
    if ( i > 75 )
        return 2;
    else if ( i > 50 )
        return 17;
    else if ( i > 25 )
        return 20;
    return 24;
}

int get_random_bash_message()
{
    int i;
    i=number_percent();
    if ( i > 75 )
        return 5;
    else if ( i > 50 )
        return 6;
    else if ( i > 25 )
        return 7;
    return 26;
}

int get_random_exotic_message()
{
    int i;
    i=number_percent();
    if ( i > 75 )
        return 4;
    else if ( i > 50 )
        return 8;
    else if ( i > 25 )
        return 15;
    return 36;
}

int   is_random_obj_in_room(ROOM_INDEX_DATA *pRoom)
{       
    int v;
    CHAR_DATA *ch;
    OBJ_DATA *o;
    
    v=0;
    for ( ch = pRoom->people; ch; ch = ch->next_in_room )
    {
        if (IS_NPC(ch))
        {
            for ( o = ch->carrying; o; o = o->next_content )
                if ( o->pIndexData->vnum == RANDOM_VNUM ) v++;
        }
    }
    
    return v;
}

void  reset_random_items( AREA_DATA *pA, int nroom )
{
  ROOM_INDEX_DATA *pRoom;
  OBJ_DATA *o;
  CHAR_DATA *ch;
  int n1=0,cntr=0;
  int64 vnum;

  if (!IS_SET(global_cfg,CFG_RANDOM)) return;
  n1=number_range(0,URANGE(1,nroom-2,1000));
  cntr=0;
    
  if ( pA->clan && !pA->clan[0] ) return;
    
  for ( vnum = pA->min_vnum; vnum < pA->max_vnum; vnum++ )
  {
    pRoom = get_room_index(vnum);
    if ( pRoom )
    {
      if ( cntr>=n1 )
      {
        if ( pRoom->people )
        {
          ch=pRoom->people;
          if (!IS_NPC(ch)) continue;
                    
          if((o=create_random_item(number_range( URANGE(0,ch->pIndexData->level-5,101), URANGE(0,ch->pIndexData->level+5,100)+1 ))))
           obj_to_char(o,ch);
          //return;
          if((o=create_random_armor(number_range( URANGE(0,ch->pIndexData->level-5,101), URANGE(0,ch->pIndexData->level+5,100)+1 ))))
           obj_to_char(o,ch);
          return;
        }
      }
      cntr++;
    }
  }
}

OBJ_DATA *create_random_item( int lvl )
{
  OBJ_DATA *o;
  AFFECT_DATA af;
  int i,j,delta,dep,dam_mes=0;
  int value;
  int is_exotic=0,is_sharp=0,is_flaming=0,is_vampiric=0;
  int is_twohanded=0,is_shocking=0,is_frost=0,is_poison=0;
  int nodrop=0, noremove=0, nouncurse=0;
  int hr=0,dr=0,str=0,dex=0;
  int avg,d1,d2;
  char s[256]="",wname[64]="",suffix[64]="",prefix[64]="";
  int64 flg;
  int sss;

  if ( lvl < 5 ) lvl=5;
  if ( lvl > 101 ) lvl=101;
    
  // Item - tolkienists sword :)
  if(!(o=create_object(get_obj_index(RANDOM_VNUM),lvl)))return NULL;
    
  o->enchanted=TRUE;
  o->owner = str_dup("(public)");
  //o->item_type=ITEM_WEAPON;
  //o->wear_loc=ITEM_TAKE | ITEM_WIELD;
  o->level=lvl;
    
  avg=number_range(lvl*2/3,lvl);
  if ( number_percent() < 20 ) avg+=number_range(0,lvl/2);
  if ( lvl > 95 )              avg=URANGE(0,avg,114*lvl/100);
  else                         avg=URANGE(0,avg,lvl);
  dep=delta=avg;d1=1;d2=1;
    
  // Get dice by average
  for ( i=1; i < avg; i++ )
  {
    for ( j=1; j < avg; j++ )
    {
      if ( abs(i*(j+1)/2-avg) < delta )
      {
        d1=i; d2=j; delta=abs(i*(j+1)/2-avg); dep=abs(i-j);
      }
      else if ( ( abs(i*(j+1)/2-avg) == delta ) && ( abs(i-j) < dep ) )
      {
        d1=i; d2=j; dep=abs(i-j);
      }
    }
  }
    
  if ( number_percent() > 55 ) hr=number_range(0,lvl/3);
  if ( number_percent() > 55 ) dr=number_range(0,lvl/3);
  if ( number_percent() > 80 ) hr=number_range(0,lvl/3);
  if ( number_percent() > 85 ) dr=number_range(0,lvl/3);
  
  if ( lvl < 30 )
  {
    hr=URANGE(0,hr,lvl/4);
    dr=URANGE(0,dr,lvl/4);
  }
  else if ( lvl < 55 )
  {
    hr=URANGE(0,hr,2*lvl/5);
    dr=URANGE(0,dr,3*lvl/10);
  }
  else if ( lvl < 85 )
  {
    hr=URANGE(0,hr,4*lvl/10);
    dr=URANGE(0,dr,3*lvl/10);
  }
  else if ( lvl < 95 )
  {
    hr=URANGE(0,hr,lvl/2);
    dr=URANGE(0,dr,lvl/2);
  }
  else
  {
    hr=URANGE(0,hr+15,lvl/2);
    dr=URANGE(0,dr+15,lvl/2);
  }
    
  is_sharp    = (avg>4*lvl/5) ? 4 : 7;
  is_flaming  = ((avg>4*lvl/5) ? 3 : 6);
  is_exotic   = ((avg>4*lvl/5) ? 3 : 5);
  is_vampiric = 8;
  is_shocking = 8;
  is_frost    = 8;
  is_poison   = 6;
  is_twohanded= 10;
  
  if ( lvl > 93 )
  {
    is_sharp*=3;
    is_flaming*=2;
    is_vampiric*=2;
  }
  else if ( lvl > 80 )
  {
    is_sharp = is_sharp*3/2;
    is_flaming = is_flaming*3/2;
  }
    
  if ( number_percent() <= is_sharp ) is_sharp=1; else is_sharp=0;
  if ( number_percent() <= is_flaming ) is_flaming=1; else is_flaming=0;
  if ( number_percent() <= is_exotic ) is_exotic=1; else is_exotic=0;
  if ( number_percent() <= is_vampiric ) is_vampiric=1; else is_vampiric=0;
  if ( number_percent() <= is_shocking ) is_shocking=1; else is_shocking=0;
  if ( number_percent() <= is_frost ) is_frost=1; else is_frost=0;
  if ( number_percent() <= is_poison ) is_poison=1; else is_poison=0;
  if ( number_percent() <= is_twohanded ) is_twohanded=1; else is_twohanded=0;

  /*if ( o->value[0]==WEAPON_MACE || o->value[0]==WEAPON_WHIP ||
    o->value[0]==WEAPON_FLAIL ) is_sharp=0;  */
  o->value[0]=is_exotic ? 0 : number_range(1,9);
  
  if ( o->value[0]==WEAPON_DAGGER || o->value[0]==WEAPON_WHIP ) is_twohanded=0;
    
  if ( number_percent() > 50 )
  {
    if ( is_sharp ) is_flaming=0;
  }
  else
  {
    if ( is_flaming ) is_sharp=0;
  }
  
  if ( is_flaming ) { is_frost=0; is_shocking=0; }
    
  value = avg*100/lvl + 8*is_exotic + 20*is_flaming + 20*is_sharp +
    12*is_vampiric + 7*(is_shocking+is_poison+is_frost) +
    hr*50/lvl + dr*100/lvl - 20*is_twohanded;
            
  o->weight=value/50;
  o->cost  =value*100;
    

  if ( value > 250 ) { nodrop=1; noremove=1; nouncurse=1; }
  else if ( value > 190 )
  {
    if ( number_percent() > 30 ) nodrop=1;
    if ( number_percent() > 30 ) noremove=1;
    if ( number_percent() > 30 ) nouncurse=1;
  }
  else if ( value > 125 )
  {
    if ( number_percent() > 75 ) nodrop=1;
    if ( number_percent() > 75 ) noremove=1;
    if ( number_percent() > 75 ) nouncurse=1;
  }
    
  if ( o->name ) free_string( o->name );
  if ( o->description ) free_string( o->description );
  if ( o->short_descr ) free_string( o->short_descr );
  
  o->name=0;
  o->description=0;
  o->short_descr=0;
  
  o->value[1]=d1;
  o->value[2]=d2;
  o->value[4]= ((is_sharp)     ? WEAPON_SHARP     : 0) | 
               ((is_flaming)   ? WEAPON_FLAMING   : 0) |
               ((is_vampiric)  ? WEAPON_VAMPIRIC  : 0) |
               ((is_shocking)  ? WEAPON_SHOCKING  : 0) |
               ((is_frost)     ? WEAPON_FROST     : 0) |
               ((is_poison)    ? WEAPON_POISON    : 0) |
               ((is_twohanded) ? WEAPON_TWO_HANDS : 0) ;
                 
  af.where = TO_AFFECTS;
  af.type  = 0;
  af.level = lvl;
  af.duration = -1;
  af.location = APPLY_HITROLL;
  af.modifier = hr;
  af.bitvector= 0;
  if (hr) affect_to_obj(o,&af);
    
  af.location = APPLY_DAMROLL;
  af.modifier = dr;
  if (dr) affect_to_obj(o,&af);
    
  o->extra_flags =  ((nodrop)     ? ITEM_NODROP    : 0) |
                    ((noremove)   ? ITEM_NOREMOVE  : 0) |
                    ((nouncurse)  ? ITEM_NOUNCURSE : 0) |
                    ((value>170)  ? ITEM_HUM       : 0) |
                    ((value>140)  ? ITEM_MAGIC     : 0) |
                    ((value>150)  ? ITEM_BLESS     : 0) | ITEM_NOLOCATE;
    
  for ( i=0; rnd_weapon_names[o->value[0]].names[i][0]; i++);
  i=number_range(0,URANGE(0,i-1,1000)); sss=rnd_weapon_names[o->value[0]].s[i];
  strcpy(wname,rnd_weapon_names[o->value[0]].names[i]);
    
  flg=0;
  if (is_twohanded) flg=WEAPON_TWO_HANDS;
  if (is_frost)     flg=WEAPON_FROST;
  if (is_shocking)  flg=WEAPON_SHOCKING;
  if (is_poison)    flg=WEAPON_POISON;
  if (is_vampiric)  flg=WEAPON_VAMPIRIC;
  if (is_sharp)     flg=WEAPON_SHARP;
  if (is_flaming)   flg=WEAPON_FLAMING;
    
  for ( i=0; rnd_flag_names[i].s && (rnd_flag_names[i].flag!=flg || 
       rnd_flag_names[i].s != sss); i++ );
  if ( rnd_flag_names[i].s && flg )
  {
    for ( j=0; rnd_flag_names[i].names[j][0]; j++);
    j=number_range(0,URANGE(0,j-1,1000));
    strcpy(prefix,rnd_flag_names[i].names[j]);
  }
    
  dr=dr*100/lvl; dr=URANGE(0,dr,100);
  hr=hr*100/lvl; hr=URANGE(0,hr,100);
  if ( value < 125 )
  {
    if ( dr > 0 && ( dr>=hr || dr > 30 ) )
    {
    
      for ( i=0; rnd_suffix_names[i].name[0] &&
           (  rnd_suffix_names[i+1].min_dr <= dr &&
           !( !rnd_suffix_names[i+1].min_dr && rnd_suffix_names[i].min_dr )); i++ );
             strcpy(suffix,rnd_suffix_names[i].name);
    }
    else
    {
      if ( hr )
      {
        for ( i=0; rnd_suffix_names[i].name[0] &&
            (  rnd_suffix_names[i+1].min_hr <= hr &&
            !( !rnd_suffix_names[i+1].min_hr && rnd_suffix_names[i].min_hr )); i++ );
        strcpy(suffix,rnd_suffix_names[i].name);
      }
      else if ( str )
      {
        for ( i=0; rnd_suffix_names[i].min_str <= str &&
            (  rnd_suffix_names[i+1].min_str <= str ||
               !rnd_suffix_names[i+1].min_str        ); i++ );
        strcpy(suffix,rnd_suffix_names[i].name);
      }
      else if ( dex )
      {
        for ( i=0; rnd_suffix_names[i].min_dex <= dex &&
            (  rnd_suffix_names[i+1].min_dex <= dex ||
               !rnd_suffix_names[i+1].min_dex        ); i++ );
        strcpy(suffix,rnd_suffix_names[i].name);
      }
      else strcpy(suffix,"");
    }
  }
  else
  {
    for ( i=0; rnd_asuffix_names[i].name[0]; i++)
      if ( value <= rnd_asuffix_names[i].max_value &&
           value >= rnd_asuffix_names[i].min_value ) break;
    strcpy(suffix,rnd_asuffix_names[i].name);
  }
    
  do_printf(s,"%s%s{w%s{x",prefix,wname,suffix);
  o->short_descr=str_dup(s);
  suffix[0]=0;
  switch ( o->value[0] )
  {
    case WEAPON_EXOTIC:
     strcpy(suffix,"exotic");
     dam_mes=get_random_exotic_message();
     break;
    case WEAPON_DAGGER:
     strcpy(suffix,"dagger");
     dam_mes=get_random_pierce_message();
     break;
    case WEAPON_SWORD:
     strcpy(suffix,"sword");
     dam_mes=get_random_slash_message();
     break;
    case WEAPON_AXE:
     strcpy(suffix,"axe");
     dam_mes=get_random_bash_message();
     break;
    case WEAPON_MACE:
     strcpy(suffix,"mace");
     dam_mes=get_random_bash_message();
     break;
    case WEAPON_POLEARM:
     strcpy(suffix,"polearm");
     dam_mes=get_random_slash_message();
     break;
    case WEAPON_SPEAR:
     strcpy(suffix,"spear");
     dam_mes=get_random_slash_message();
     break;
    case WEAPON_WHIP:
     strcpy(suffix,"whip");
     dam_mes=3;
     break;
    case WEAPON_FLAIL:
     strcpy(suffix,"flail");
     dam_mes=get_random_bash_message();     
     break;
    case WEAPON_STAFF:
     strcpy(suffix,"staff");
     dam_mes=get_random_slash_message();
     break;
  }
  do_printf(s,"%s random weapon",suffix);
  o->name=str_dup(s);
  o->description=str_dup("Неизвестное оружие притягивает твой взгляд.");
  o->value[3]=dam_mes+1;
  
  return o;
}

OBJ_DATA *create_random_armor( int lvl )
{                                                

  OBJ_DATA *o;
  AFFECT_DATA af;
  int value=0;
  int nodrop=0, noremove=0, nouncurse=0;
  int hr=0,dr=0,str=0,dex=0, ac=0, svs=0;
  int intt=0, wis=0, con=0;

    
if(!(o=create_object(get_obj_index(OBJ_VNUM_ARMY_SHIELD), 0))) return NULL;
    
  o->enchanted=TRUE;
  o->owner = str_dup("(public)");
  o->item_type=ITEM_ARMOR;
//  o->wear_loc=ITEM_TAKE | ITEM_WEAR_SHIELD;
  o->level=lvl;
  o->description=str_dup("Неизвестный щит лежит перед тобой.");
  if ( lvl < 5 ) lvl=12;
  if ( lvl > 101 ) lvl=101;

  if ( number_percent() > 55 ) hr=number_range(0,lvl/3);
  if ( number_percent() > 55 ) dr=number_range(0,lvl/3);
  if ( number_percent() > 80 ) hr=number_range(0,lvl/3);
  if ( number_percent() > 85 ) dr=number_range(0,lvl/3);
  
  if ( lvl < 30 )
  {
    hr=URANGE(0,hr,lvl/4);
    dr=URANGE(0,dr,lvl/4);
  }
  else if ( lvl < 55 )
  {
    hr=URANGE(0,hr,2*lvl/5);
    dr=URANGE(0,dr,3*lvl/10);
  }
  else if ( lvl < 85 )
  {
    hr=URANGE(0,hr,4*lvl/10);
    dr=URANGE(0,dr,3*lvl/10);
  }
  else if ( lvl < 95 )
  {
    hr=URANGE(0,hr,lvl/2);
    dr=URANGE(0,dr,lvl/2);
  }
  else
  {
    hr=URANGE(0,hr+15,lvl/2);
    dr=URANGE(0,dr+15,lvl/2);
  }

  //playing vs ac.
  if (( number_percent() > (100-lvl) ) && ((hr+dr*3/2)) < (lvl/2)) 
  ac=(-1)*number_range(lvl/2,lvl*3);

  //playing vs svs.
  if (( number_percent() > (100-(lvl/2)) ) && ((hr+dr*3/2-ac/10)) < (lvl/2)) 
  svs=(-1)*number_range(0,lvl/5);

  value = hr*50/lvl + dr*100/lvl - ac*10/lvl - svs*100/lvl;

  if (value < (2+lvl/2) )
  { if ( number_percent() > 80) str=lvl/12;
    else if ( number_percent() > 60) dex=lvl/12;
    else if ( number_percent() > 40) intt=lvl/12;
    else if ( number_percent() > 20) wis=lvl/12;
    else con=lvl/12;
  };

  af.where = TO_AFFECTS;
  af.type  = 0;
  af.level = lvl;
  af.duration = -1;
  af.location = APPLY_HITROLL;
  af.modifier = hr;
  af.bitvector= 0;
  if (hr) affect_to_obj(o,&af);
    
  af.location = APPLY_DAMROLL;
  af.modifier = dr;
  if (dr) affect_to_obj(o,&af);

  af.location = APPLY_AC;
  af.modifier = ac;
  if (ac) affect_to_obj(o,&af);

  af.location = APPLY_STR;
  af.modifier = str;
  if (str) affect_to_obj(o,&af);

  af.location = APPLY_DEX;
  af.modifier = dex;
  if (dex) affect_to_obj(o,&af);

  af.location = APPLY_INT;
  af.modifier = intt;
  if (intt) affect_to_obj(o,&af);

  af.location = APPLY_WIS;
  af.modifier = wis;
  if (wis) affect_to_obj(o,&af);

  af.location = APPLY_CON;
  af.modifier = con;
  if (con) affect_to_obj(o,&af);

  af.location = APPLY_SAVES;
  af.modifier = svs;
  if (svs) affect_to_obj(o,&af);
 
  if ( value > 200 ) { nodrop=1; noremove=1; nouncurse=1; }
  else if ( value > 120 )
  {
    if ( number_percent() > 30 ) nodrop=1;
    if ( number_percent() > 30 ) noremove=1;
    if ( number_percent() > 75 ) nouncurse=1;
  }
  else if ( value > 80 )
  {
    if ( number_percent() > 75 ) nodrop=1;
    if ( number_percent() > 75 ) noremove=1;
    if ( number_percent() > 95 ) nouncurse=1;
  }


  o->extra_flags =  ((nodrop)     ? ITEM_NODROP    : 0) |
                    ((noremove)   ? ITEM_NOREMOVE  : 0) |
                    ((nouncurse)  ? ITEM_NOUNCURSE : 0) |
                    ((value>30)  ? ITEM_HUM       : 0) |
                    ((value>40)  ? ITEM_MAGIC     : 0) |
                    ((value>50)  ? ITEM_BLESS     : 0) | ITEM_NOLOCATE;
    
    
  if (value < 20 )
    {
      if (str!=0) 
       { 
        o->short_descr="Щит {BСилы{x";
        o->name="shield random strength щит силы";
       }
      else 
        if (wis!=0) 
          {
            o->short_descr="Щит {CМудрости{x";
            o->name="shield random wisdom щит мудрости";
          }
        else 
         if (intt!=0) 
           {
              o->short_descr="Щит {RЗнания{x";
              o->name="shield random intelligence щит знания";
           }
         else 
          if (con!=0) 
            {
               o->short_descr="Щит {cЭнергии{x";
               o->name="shield random constitution щит энергии";
            }
          else 
           if (dex!=0) 
             {
                o->short_descr="Щит {GЛовкости{x";
                o->name="shield random dexterity щит ловкости";
             }
            else 
              {
                 o->short_descr="Обыкновенный щит";
                 o->name="shield random standart обыкновенный щит";
              }
    }
  else 
    if ( ac < ((-1)*lvl)) 
      {
        o->short_descr="Щит {WЗащиты{x";
        o->name="shield random defence щит защиты";
      }
    else 
      if ( hr > (lvl/10)) 
        {
           o->short_descr="Щит {rПовреждения{x";
           o->name="shield random hitroll щит повреждения";
        }
       else 
         if ( dr > (lvl/10)) 
           {
              o->short_descr="Щит {YОГРА{x";
              o->name="shield random damroll щит огра";
           }
          else 
            {
              o->short_descr="Щит {gиз драконовой кожи{x";
              o->name="shield random dragon scale щит драконовой кожи";
            }
  
  return o;
}
