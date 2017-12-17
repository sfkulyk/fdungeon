// $Id: recycle.c,v 1.32 2003/10/29 08:40:53 wagner Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

// stuff for recyling notes
NOTE_DATA *note_free;

NOTE_DATA *new_note()
{
  NOTE_DATA *note;

  if (note_free == NULL) note = alloc_perm(sizeof(*note));
  else
  { 
    note = note_free;
    note_free = note_free->next;
  }
  VALIDATE(note);
  return note;
}

void free_note(NOTE_DATA *note)
{
  if (!IS_VALID(note)) return;

  free_string( note->text    );
  free_string( note->host    );
  free_string( note->subject );
  free_string( note->to_list );
  free_string( note->date    );
  free_string( note->sender  );
  INVALIDATE(note);
  note->next = note_free;
  note_free  = note;
}

    
// stuff for recycling ban structures
BAN_DATA *ban_free;

BAN_DATA *new_ban(void)
{
  static BAN_DATA ban_zero;
  BAN_DATA *ban;

  if (ban_free == NULL) ban = alloc_perm(sizeof(*ban));
  else
  {
     ban = ban_free;
     ban_free = ban_free->next;
  }
  *ban = ban_zero;
  ban->host = &str_empty[0];
  return ban;
}

void free_ban(BAN_DATA *ban)
{
  free_string(ban->host);
  ban->next = ban_free;
  ban_free = ban;
}

// stuff for recycling descriptors
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *new_descriptor(void)
{
  static DESCRIPTOR_DATA d_zero;
  DESCRIPTOR_DATA *d;

  if (!descriptor_free ) d = alloc_perm(sizeof(*d));
  else
  {
    d = descriptor_free;
    descriptor_free = descriptor_free->next;
  }
  *d = d_zero;
  d->snoop_by=NULL;
  d->character=NULL;
  d->connected=0;
  VALIDATE(d);
  cnt_descr++;
  return d;
}

void free_descriptor(DESCRIPTOR_DATA *d)
{
  if (!IS_VALID(d)) return;

  free_string( d->host );
  free_string(d->showstr_head);
  free_string(d->showstr_point);
  free_mem( d->outbuf, d->outsize );

  if (d==descriptor_list) descriptor_list=d->next;
  else
  {
    DESCRIPTOR_DATA *dprev;
    bool found=FALSE;

    for ( dprev = descriptor_list; dprev; dprev = dprev->next )
    {
      if ( dprev->next == d )
      {
         dprev->next = d->next;
         found=TRUE;
         break;
      }
    }
    if (!found) bug( "Free_descriptor: d in descriptor_list not found.", 0 );
  }
  d->next = descriptor_free;
  descriptor_free = d;
  INVALIDATE(d);
  cnt_descr--;
}

// stuff for recycling gen_data
GEN_DATA *gen_data_free;

GEN_DATA *new_gen_data(void)
{
  static GEN_DATA gen_zero;
  GEN_DATA *gen;

  if (gen_data_free == NULL) gen = alloc_perm(sizeof(*gen));
  else
  {
    gen = gen_data_free;
    gen_data_free = gen_data_free->next;
  }
  *gen = gen_zero;
  VALIDATE(gen);
  return gen;
}

void free_gen_data(GEN_DATA *gen)
{
  if (!IS_VALID(gen)) return;
  INVALIDATE(gen);
  gen->next = gen_data_free;
  gen_data_free = gen;
} 

/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free;

EXTRA_DESCR_DATA *new_extra_descr(void)
{
  EXTRA_DESCR_DATA *ed;

  if (extra_descr_free == NULL) ed = alloc_perm(sizeof(*ed));
  else
  {
    ed = extra_descr_free;
    extra_descr_free = extra_descr_free->next;
  }
  ed->keyword = &str_empty[0];
  ed->description = &str_empty[0];
  VALIDATE(ed);
  return ed;
}

void free_extra_descr(EXTRA_DESCR_DATA *ed)
{
  if (!IS_VALID(ed)) return;

  free_string(ed->keyword);
  free_string(ed->description);
  INVALIDATE(ed);
  ed->next = extra_descr_free;
  extra_descr_free = ed;
}

// stuff for recycling affects
AFFECT_DATA *affect_free;

AFFECT_DATA *new_affect(void)
{
  static AFFECT_DATA af_zero;
  AFFECT_DATA *af;

  if (affect_free == NULL) af = alloc_perm(sizeof(*af));
  else
  {
    af = affect_free;
    affect_free = affect_free->next;
  }
  *af = af_zero;
  VALIDATE(af);
  cnt_aff++;
  return af;
}

void free_affect(AFFECT_DATA *af)
{
  if (!IS_VALID(af)) return;
  af->next = affect_free;
  affect_free = af;
  INVALIDATE(af);
  cnt_aff--;
}

// stuff for recycling objects
OBJ_DATA *obj_free;

OBJ_DATA *new_obj(void)
{
  static OBJ_DATA obj_zero;
  OBJ_DATA *obj;

  if (obj_free == NULL) obj = alloc_perm(sizeof(*obj));
  else
  {
    obj = obj_free;
    obj_free = obj_free->next;
  }
  *obj = obj_zero;
  VALIDATE(obj);
  cnt_obj++;
  return obj;
}

void free_obj(OBJ_DATA *obj)
{
  AFFECT_DATA *paf, *paf_next;
  EXTRA_DESCR_DATA *ed, *ed_next;

  if (!IS_VALID(obj))   return;
  for (paf = obj->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    free_affect(paf);
  }
  obj->affected = NULL;

  for (ed = obj->extra_descr; ed != NULL; ed = ed_next )
  {
    ed_next = ed->next;
    free_extra_descr(ed);
  }
  obj->extra_descr = NULL;

  free_string( obj->name);
  free_string( obj->description );
  free_string( obj->short_descr );
  free_string( obj->material );
  free_string( obj->owner);
  obj->next   = obj_free;
  obj_free    = obj; 
  INVALIDATE(obj);
  cnt_obj--;
}

// stuff for recyling characters
CHAR_DATA *char_free;

CHAR_DATA *new_char (void)
{
  static CHAR_DATA ch_zero;
  CHAR_DATA *ch;
  int i;

  if (char_free == NULL) ch = alloc_perm(sizeof(*ch));
  else
  {
    ch = char_free;
    char_free = char_free->next;
  }
  *ch = ch_zero;
  VALIDATE(ch);
  ch->name = &str_empty[0];
  ch->desc=NULL;
  ch->short_descr = &str_empty[0];
  ch->long_descr = &str_empty[0];
  ch->description = &str_empty[0];
  ch->prompt = &str_empty[0];
  ch->prefix = &str_empty[0];
  ch->runbuf = &str_empty[0];
  ch->logon = current_time;
  ch->host = &str_empty[0];
  ch->lines = PAGELEN;
  for (i = 0; i < 4; i++) ch->armor[i] = 100;
  ch->position = POS_STANDING;
  ch->hit = 20;
  ch->max_hit = 20;
  ch->maxrun[0]=100;
  ch->maxrun[1]=0;
  ch->mana = 100;
  ch->max_mana = 100;
  ch->move = 100;
  ch->max_move = 100;
  ch->master=NULL;
  ch->leader=NULL;
  ch->fighting=NULL;
  ch->questmob=NULL;
  ch->questroom=NULL;
  ch->affected=NULL;
  ch->pnote=NULL;
  ch->master=NULL;
  ch->leader=NULL;
  ch->reply=NULL;
  ch->pet=NULL;
  ch->mprog_target=NULL;

  for (i = 0; i < MAX_STATS; i ++)
  {
    ch->perm_stat[i] = 13;
    ch->mod_stat[i] = 0;
  }
  cnt_char++;
  return ch;
}

void free_char (CHAR_DATA *ch)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  if (!IS_VALID(ch)) return;
  if (IS_NPC(ch)) mobile_count--;
  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    extract_obj(obj);
  }
  for (paf = ch->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    affect_remove(ch,paf);
  }
  free_string(ch->name);
  free_string(ch->short_descr);
  free_string(ch->long_descr);
  free_string(ch->description);
  free_string(ch->prompt);
  free_string(ch->prefix);
  free_string(ch->runbuf);
  free_string(ch->host);
  free_note(ch->pnote);
  free_pcdata(ch->pcdata);

  ch->next = char_free;
  char_free  = ch;
  INVALIDATE(ch);
  cnt_char--;
  return;
}

PC_DATA *pcdata_free;

PC_DATA *new_pcdata(void)
{
  int a;

  static PC_DATA pcdata_zero;
  PC_DATA *pcdata;
  if (pcdata_free == NULL) pcdata = alloc_perm(sizeof(*pcdata));
  else
  {
    pcdata = pcdata_free;
    pcdata_free = pcdata_free->next;
  }
  *pcdata = pcdata_zero;

  for (a = 0; a < MAX_QUENIA; a++) pcdata->quenia[a]=0;

  for (a = 0; a < MAX_ALIAS; a++)
  {
    pcdata->alias[a] = NULL;
    pcdata->alias_sub[a] = NULL;
  }
  pcdata->buffer = new_buf();
  pcdata->marry=&str_empty[0];
  pcdata->mother=&str_empty[0];
  pcdata->kins=&str_empty[0];
  pcdata->father=&str_empty[0];
  pcdata->vislist=&str_empty[0];
  pcdata->ignorelist=&str_empty[0];
  pcdata->lovers=&str_empty[0];
  pcdata->propose_marry=&str_empty[0];
  pcdata->tickstr=&str_empty[0];
  
  VALIDATE(pcdata);
  cnt_pcdata++;
  return pcdata;
}
        
void free_pcdata(PC_DATA *pcdata)
{
  int alias;

  if (!IS_VALID(pcdata)) return;
  free_string(pcdata->pwd);
  free_string(pcdata->bamfin);
  free_string(pcdata->bamfout);
//  free_string(deity);
  free_string(pcdata->pseudoname);
  free_string(pcdata->auto_online);
  free_string(pcdata->pkillers);
  free_string(pcdata->run);
  free_string(pcdata->runbuf);
  free_string(pcdata->vislist);
  free_string(pcdata->ignorelist);
  free_string(pcdata->marry);
  free_string(pcdata->mother);
  free_string(pcdata->father);
  free_string(pcdata->kins);
  free_string(pcdata->lovers);
  free_string(pcdata->propose_marry);
  free_string(pcdata->title);
  free_buf(pcdata->buffer);
  for (alias = 0; alias < MAX_ALIAS; alias++)
  {
    free_string(pcdata->alias[alias]);
    free_string(pcdata->alias_sub[alias]);
  }
  INVALIDATE(pcdata);
  pcdata->next = pcdata_free;
  pcdata_free = pcdata;
  cnt_pcdata--;
}

// stuff for setting ids
long    last_pc_id;
long    last_mob_id;

long get_pc_id(void)
{
  int val;
  val = (current_time <= last_pc_id) ? last_pc_id + 1 : current_time;
  last_pc_id = val;
  return val;
}

long get_mob_id(void)
{
  last_mob_id++;
  return last_mob_id;
}

MEM_DATA *mem_data_free;

// procedures and constants needed for buffering
BUFFER *buf_free;

MEM_DATA *new_mem_data(void)
{
  MEM_DATA *memory;
  
  if (mem_data_free == NULL) memory = alloc_mem(sizeof(*memory));
  else
  {
    memory = mem_data_free;
    mem_data_free = mem_data_free->next;
  }
  memory->next = NULL;
  memory->id = 0;
  memory->reaction = 0;
  memory->when = 0;
  VALIDATE(memory);
  return memory;
}

void free_mem_data(MEM_DATA *memory)
{
  if (!IS_VALID(memory)) return;
  memory->next = mem_data_free;
  mem_data_free = memory;
  INVALIDATE(memory);
}

// buffer sizes
const int buf_size[MAX_BUF_LIST] =
{
  16,32,64,128,256,1024,2048,4096,8192,16384
};

// local procedure for finding the next acceptable size 
// -1 indicates out-of-boundary error
int get_size (int val)
{
  int i;

  for (i = 0; i < MAX_BUF_LIST; i++)
  if (buf_size[i] >= val) return buf_size[i];
  return -1;
}

BUFFER *new_buf()
{
  BUFFER *buffer;

  if (buf_free == NULL) buffer = alloc_perm(sizeof(*buffer));
  else
  {
    buffer = buf_free;
    buf_free = buf_free->next;
  }
  buffer->next  = NULL;
  buffer->state = BUFFER_SAFE;
  buffer->size  = get_size(BASE_BUF);

  buffer->string = alloc_mem(buffer->size);
  buffer->string[0] = '\0';
  VALIDATE(buffer);
  cnt_buff++;
  return buffer;
}

/* remmed, 'cause never used
BUFFER *new_buf_size(int size)
{
  BUFFER *buffer;

  if (buf_free == NULL) buffer = alloc_perm(sizeof(*buffer));
  else
  {
    buffer = buf_free;
    buf_free = buf_free->next;
  }

  buffer->next = NULL;
  buffer->state= BUFFER_SAFE;
  buffer->size = get_size(size);
  if (buffer->size == -1)
  {
    bug("new_buf: buffer size %d too large.",size);
    exit(1);
  }
  buffer->string = alloc_mem(buffer->size);
  buffer->string[0] = '\0';
  VALIDATE(buffer);
  return buffer;
}
*/

void free_buf(BUFFER *buffer)
{
  if (!IS_VALID(buffer)) return;

  free_mem(buffer->string,buffer->size);
  buffer->string = NULL;
  buffer->size   = 0;
  buffer->state  = BUFFER_FREED;
  INVALIDATE(buffer);
  buffer->next  = buf_free;
  buf_free      = buffer;
  cnt_buff--;
}


bool add_buf(BUFFER *buffer, const char *string)
{
  int len;
  char *oldstr;
  int oldsize;

  oldstr = buffer->string;
  oldsize = buffer->size;

  if (buffer->state == BUFFER_OVERFLOW) return FALSE;
  len = strlen(buffer->string) + strlen(string) + 4;
  while (len >= buffer->size) // increase the buffer size
  {
    buffer->size = get_size(buffer->size + 1);
    {
      if (buffer->size == -1) // overflow
      {
        buffer->size = oldsize;
        buffer->state = BUFFER_OVERFLOW;
        bug("buffer overflow past size %d",buffer->size);
        return FALSE;
      }
    }
  }
  if (buffer->size != oldsize)
  {
    buffer->string = alloc_mem(buffer->size);
    strcpy(buffer->string,oldstr);
    free_mem(oldstr,oldsize);
  }
  strcat(buffer->string,string);
  return TRUE;
}

void clear_buf(BUFFER *buffer)
{
  buffer->string[0] = '\0';
  buffer->state     = BUFFER_SAFE;
}

char *buf_string(BUFFER *buffer)
{
  return buffer->string;
}

// stuff for recycling mobprograms
MPROG_LIST *mprog_free;
 
MPROG_LIST *new_mprog(void)
{
  static MPROG_LIST mp_zero;
  MPROG_LIST *mp;

  if (mprog_free == NULL) mp = alloc_perm(sizeof(*mp));
  else
  {
    mp = mprog_free;
    mprog_free=mprog_free->next;
  }
  *mp = mp_zero;
  mp->vnum     = 0;
  mp->trig_type= 0;
  mp->code     = str_dup("");
  VALIDATE(mp);
  return mp;
}

void free_mprog(MPROG_LIST *mp)
{
  if (!IS_VALID(mp)) return;
  INVALIDATE(mp);
  mp->next = mprog_free;
  mprog_free = mp;
}
    
CLAN_DATA *clan_free;

CLAN_DATA *new_clan()
{
  CLAN_DATA *clan;

  if (clan_free == NULL) clan = alloc_perm(sizeof(*clan));
  else
  { 
    clan = clan_free;
    clan_free = clan_free->next;
  }
  VALIDATE(clan);
  clan->name      =&str_empty[0];
  clan->show_name =&str_empty[0];
  clan->recalmsg1 =&str_empty[0];
  clan->recalmsg2 =&str_empty[0];
  clan->war       =&str_empty[0];
  clan->alli      =&str_empty[0];
  clan->acceptalli=&str_empty[0];
  clan->short_desc=&str_empty[0];
  clan->long_desc =&str_empty[0];
  clan->wear_loc  =-1;
  clan->flag      =0;
  clan->mod       =NULL;
  return clan;
}

void free_clan(CLAN_DATA *clan)
{
  AFFECT_DATA *af,*af_next;
  int i;

  if (!IS_VALID(clan)) return;

  free_string( clan->short_desc);
  free_string( clan->long_desc );
  free_string( clan->name      );
  free_string( clan->show_name );
  free_string( clan->recalmsg1 );
  free_string( clan->recalmsg2 );
  free_string( clan->war       );
  free_string( clan->alli      );
  free_string( clan->acceptalli);
  for (i=0;i<20;i++) clan->clansn[i]=0;
  for (af=clan->mod;af;af=af_next)
  {
    af_next=af->next;
    free_affect(af);
  }
  free_affect( clan->mod );
  INVALIDATE(clan);
  clan->next = clan_free;
  clan_free   = clan;
}

PENALTY_DATA *penalty_free;

PENALTY_DATA *new_penalty(void)
{
  PENALTY_DATA *penalty;

  if (!penalty_free) penalty = alloc_perm(sizeof(*penalty));
  else
  {
    penalty = penalty_free;
    penalty_free = penalty_free->next;
  }
  VALIDATE(penalty);
  return penalty;
}

void free_penalty(PENALTY_DATA *penalty)
{
  if (!IS_VALID(penalty)) return;
  free_string(penalty->inquisitor);
  penalty->type=0;
  penalty->ticks=0;
  penalty->affect=0;
  INVALIDATE(penalty);
  penalty->next = penalty_free;
  penalty_free = penalty;
}

VOTE_DATA *vote_free;

VOTE_DATA *new_vote(void)
{
  VOTE_DATA *vote, *oldvote;

  if (!vote_free) vote = alloc_perm(sizeof(*vote));
  else
  {
    vote=vote_free;
    vote_free=vote->next;
  }
  vote->msg=&str_empty[0];
  vote->owner=&str_empty[0];
  vote->chars=NULL;
  vote->next=NULL;
  if (!vote_list) vote_list=vote;
  else
  {
    for (oldvote=vote_list;oldvote->next;oldvote=oldvote->next);
    oldvote->next=vote;
  }
  return vote;
}

void free_vote(VOTE_DATA *vote)
{
  VOTE_DATA *v;
  VOTE_CHAR *vch;

  if (!vote) return;

  if (vote==vote_list) vote_list=vote->next;
  else for (v=vote_list;v;v=v->next)
  {
    if (v->next==vote)
    {
      v->next=vote->next;
      break;
    }
  }
  while (vote->chars)
  {
    vch=vote->chars;
    vote->chars=vch->next;
    free_vote_char(vch);
  }
  free_string(vote->msg);
  free_string(vote->owner);
  vote->next=vote_free;
  vote_free=vote;
}

VOTE_CHAR *vote_char_free;

VOTE_CHAR *new_vote_char(void)
{
  VOTE_CHAR *vote_char;

  if (!vote_char_free) vote_char = alloc_perm(sizeof(*vote_char));
  else
  {
    vote_char=vote_char_free;
    vote_char_free=vote_char->next;
  }
  vote_char->name=&str_empty[0];
  return vote_char;
}

void free_vote_char(VOTE_CHAR *vote_char)
{
  free_string(vote_char->name);
  vote_char->age=0;
  vote_char->level=0;
  vote_char->voting=0;
  vote_char->next=vote_char_free;
  vote_char_free=vote_char;
}

OFFER_DATA *new_offer(void)
{
  OFFER_DATA *offer,*off;

  offer = alloc_perm(sizeof(*offer));
  offer->name=&str_empty[0];
  offer->customer=&str_empty[0];
  offer->next=0;
  offer->flag=0;
  if (!offer_list)
  { offer_list=offer; return offer; }
  for (off=offer_list; off->next; off=off->next);
  off->next=offer;
  return offer;
}

void free_offer(OFFER_DATA *offer)
{
  OFFER_DATA *off;

  if (offer==offer_list) offer_list=offer->next;
  else for (off=offer_list;off;off=off->next)
  {
    if (off->next==offer)
    {
      off->next=offer->next;
      break;
    }
  }
  free_string(offer->name);
  free_string(offer->customer);
  offer->gold=0;
  offer->next=NULL;
}

RAFFECT *raffect_free;

RAFFECT *new_raffect()
{
  RAFFECT *ra;

  if (!raffect_free) ra = alloc_perm(sizeof(*ra));
  else
  {
    ra=raffect_free;
    raffect_free=ra->next;
  }
  ra->next=raffect_list;
  raffect_list=ra;
  free_string(ra->owner);
  ra->bit=ra->target=0;
  ra->level=0;
  ra->duration=0;
  ra->room=NULL;
  cnt_raff++;
  return ra;
}

void free_raffect(RAFFECT *raffect)
{
  RAFFECT *ra;

  if (!raffect) return;

  if (raffect->room) raffect_from_room(raffect);

  if (raffect==raffect_list) raffect_list=raffect->next;
  else for (ra=raffect_list;ra;ra=ra->next)
  {
    if (ra->next==raffect)
    {
      ra->next=raffect->next;
      break;
    }
  }
  raffect->bit=0;
  raffect->flag=0;
  free_string(raffect->owner);
  raffect->target=0;
  raffect->level=0;
  raffect->duration=0;
  raffect->next=raffect_free;
  raffect_free=raffect;
  cnt_raff--;
}

NEWS_DATA *news_free;

NEWS_DATA *new_news()
{
  NEWS_DATA *news;

  if (news_free == NULL) news = alloc_perm(sizeof(*news));
  else
  { 
    news = news_free;
    news_free = NULL;
  }
  VALIDATE(news);
  return news;
}

void free_news(NEWS_DATA *news)
{
  if (!IS_VALID(news)) return;

  free_string( news->text    );
  free_string( news->date    );
  INVALIDATE(news);
  if (!news_free) news_free = news;
}
