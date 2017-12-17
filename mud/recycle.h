// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

// externs
extern char str_empty[1];
extern int mobile_count;

// stuff for providing a crash-proof buffer
#define MAX_BUF         16384
#define MAX_BUF_LIST    10
#define BASE_BUF        1024

// valid states
#define BUFFER_SAFE     0
#define BUFFER_OVERFLOW 1
#define BUFFER_FREED    2

// note recycling
#define ND NOTE_DATA
ND      *new_note args( (void) );
void    free_note args( (NOTE_DATA *note) );
#undef ND

// ban data recycling
BAN_DATA *new_ban args( (void) );
void    free_ban args( (BAN_DATA *ban) );

// descriptor recycling
#define DD DESCRIPTOR_DATA
DD      *new_descriptor args( (void) );
void    free_descriptor args( (DESCRIPTOR_DATA *d) );
#undef DD

// char gen data recycling
#define GD GEN_DATA
GD      *new_gen_data args( (void) );
void    free_gen_data args( (GEN_DATA * gen) );
#undef GD

// extra descr recycling
#define ED EXTRA_DESCR_DATA
ED      *new_extra_descr args( (void) );
void    free_extra_descr args( (EXTRA_DESCR_DATA *ed) );
#undef ED

// affect recycling
#define AD AFFECT_DATA
AD      *new_affect args( (void) );
void    free_affect args( (AFFECT_DATA *af) );
#undef AD

// object recycling
#define OD OBJ_DATA
OD      *new_obj args( (void) );
void    free_obj args( (OBJ_DATA *obj) );
#undef OD

// character recyling
#define CD CHAR_DATA
#define PD PC_DATA
CD      *new_char args( (void) );
void    free_char args( (CHAR_DATA *ch) );
PD      *new_pcdata args( (void) );
void    free_pcdata args( (PC_DATA *pcdata) );
#undef PD
#undef CD

// mob id and memory procedures
#define MD MEM_DATA
long    get_pc_id args( (void) );
long    get_mob_id args( (void) );
MD      *new_mem_data args( (void) );
void    free_mem_data args( ( MEM_DATA *memory) );
MD      *find_memory args( (MEM_DATA *memory, long id) );
#undef MD

// buffer procedures
BUFFER  *new_buf args( (void) );
//BUFFER  *new_buf_size args( (int size) );
void    free_buf args( (BUFFER *buffer) );
bool    add_buf args( (BUFFER *buffer, const char *string) );
void    clear_buf args( (BUFFER *buffer) );
char    *buf_string args( (BUFFER *buffer) );

PENALTY_DATA *new_penalty(void);
void free_penalty(PENALTY_DATA *penalty);

VOTE_DATA *new_vote(void);
void free_vote(VOTE_DATA *vote);

VOTE_CHAR *new_vote_char(void);
void free_vote_char(VOTE_CHAR *vote_char);

OFFER_DATA *new_offer(void);
void free_offer(OFFER_DATA *offer);

NEWS_DATA  *new_news args( (void) );
void    free_news args( (NEWS_DATA *news) );
