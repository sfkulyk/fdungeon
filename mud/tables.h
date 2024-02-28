// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include "merc.h"
 
struct clan_rank_type
{
    char *name;
};

struct position_type
{
    char *name;
    char *short_name;
};

struct sex_type
{
    char *name;
};

struct size_type
{
    char *name;
};

struct mitem_type
{       
    char *name;
    char *desc;
    int64 bit;
};

struct wear_ntype
{       
    char *name;
    int wear_num;
};

struct rspec_type
{
  char *info;
  int64 bit;
};

struct material_type
{
  char *name;       // lookup name
  char *real_name;  // output name
  int   d_dam;      // durability dependant damage_obj table
  int   hard;       // hardness
  bool  metal;      // is metal?
  int64 res;        // resist affects
  int64 vul;        // vulnerable affects
  int   resnum;     // vulnerable 
  int   vulnum;     // vulnerable 
};

/* game tables */
extern  const   struct  clan_rank_type  clan_ranks[7];
extern  const   struct  position_type   position_table[];
extern  const   struct  sex_type        sex_table[];
extern  const   struct  size_type       size_table[];
extern  const   struct  mitem_type      mitem[];
extern  const   struct  wear_ntype      wear_l[];
extern  const   struct  rspec_type      rspec_table[];
extern  const   struct  material_type   material_table[];

/* flag tables */
extern  const   struct  flag_type act_flags[];
extern  const   struct  flag_type plr_flags[];
extern  const   struct  flag_type affect_flags[];
extern  const   struct  flag_type off_flags[];
extern  const   struct  flag_type imm_flags[];
extern  const   struct  flag_type form_flags[];
extern  const   struct  flag_type part_flags[];
extern  const   struct  flag_type comm_flags[];
extern  const   struct  flag_type extra_flags[];
extern  const   struct  flag_type wear_flags[];
extern  const   struct  flag_type weapon_flags[];
extern  const   struct  flag_type container_flags[];
extern  const   struct  flag_type portal_flags[];
extern  const   struct  flag_type room_flags[];
extern  const   struct  flag_type exit_flags[];
extern  const   struct  flag_type mprog_flags[];
extern  const   struct  flag_type raff_flags[];
