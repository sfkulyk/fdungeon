// $Id: db.h,v 1.6 2002/08/26 20:01:32 black Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

// vals from db.c
extern bool fBootDb;
extern int            newmobs;
extern int            newobjs;
extern MOB_INDEX_DATA * mob_index_hash  [MAX_KEY_HASH];
extern OBJ_INDEX_DATA * obj_index_hash  [MAX_KEY_HASH];
extern int64          top_mob_index;
extern int64          top_obj_index;
extern int            top_affect;
extern int            top_ed; 
extern AREA_DATA      * area_first;

// conversion from db.h
void convert_mob(MOB_INDEX_DATA *mob);
void convert_obj(OBJ_INDEX_DATA *obj);
void load_deities();
void load_clans();

typedef struct
{
    char *names[10];
    int   s[10];     // rod imeni 1=male, 2=female, 3=it
} random_weapon_name;

typedef struct
{
    char *names[10];
    int flag;
    int s;
} random_flag_name;

typedef struct
{
    char *name;
    int min_hr;
    int min_dr;
    int min_str;
    int min_dex;
} random_suffix_name;

typedef struct
{
    char *name;
    int min_value;
    int max_value;
} random_asuffix_name;

// macro for flag swapping
#define GET_UNSET(flag1,flag2)  (~(flag1)&((flag1)|(flag2)))

// func from db.c
extern void assign_area_vnum( int64 vnum );

// from db2.c
void convert_mobile( MOB_INDEX_DATA *pMobIndex );
void convert_objects( void );
void convert_object( OBJ_INDEX_DATA *pObjIndex );
int  maxsocial;

// deities and races from db2.c
void load_deities(void);
void load_one_deity( FILE *fp, int deity);
void load_races(void);
void load_one_race( FILE *fp, int race);

typedef struct
{
    char *name;
    int  vnum;
    char *rushian;
} random_armor;
