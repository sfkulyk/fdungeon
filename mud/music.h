// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#define MAX_SONGS       30
#define MAX_LINES       100 // this boils down to about 1k per song
#define MAX_GLOBAL      10  // max songs the global jukebox can hold

struct song_data
{
  const char *group;
  const char *name;
  const char *lyrics[MAX_LINES];
  int lines;
};

extern struct song_data song_table[MAX_SONGS];
void song_update args( (void) );
void load_songs args( (void) );
