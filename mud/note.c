// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <sys/types.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

// globals from db.c for load_notes
extern FILE * fpArea;
extern char   strArea[MAX_INPUT_LENGTH];

NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *penalty_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;
NOTE_DATA *bugreport_list;
NOTE_DATA *offtopic_list;
NOTE_DATA *complain_list;

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
  if (pnote->type==NOTE_COMPLAIN && IS_IMMORTAL(ch) ) return TRUE;
  if (pnote->type==NOTE_BUGREPORT && (IS_SET(ch->comm,COMM_CODER) || IS_IMMORTAL(ch)) ) return TRUE; // Now immortals will receive bugreports (c) Wagner.
  if ( !str_cmp( ch->name, pnote->sender ) ) return TRUE;
  if ( is_exact_name( "all", pnote->to_list ) ) return TRUE;
  if (IS_IMMORTAL(ch) && is_exact_name("imm",pnote->to_list)) return TRUE;
  if ((IS_SET(ch->comm,COMM_CODER) || IS_IMMORTAL(ch)) && is_exact_name("coder",pnote->to_list)) return TRUE; // (c) Wagner
  if (IS_IMMORTAL(ch) && is_exact_name("immortal",pnote->to_list)) return TRUE;
  if (is_exact_name(ch->name,"Saboteur Magica Astellar Dragon") && is_exact_name("elder",pnote->to_list)) return TRUE;
  if (ch->clan!=NULL && is_exact_name("clan",pnote->to_list)) return TRUE;
  if (ch->clanrank==LEADER && is_exact_name("leader",pnote->to_list)) return TRUE;
  if (ch->clan==NULL && is_exact_name("noclan",pnote->to_list)) return TRUE;
  if (ch->clan && is_exact_name(ch->clan->name,pnote->to_list)) return TRUE;
  if (is_exact_name(ch->name, pnote->to_list ) ) return TRUE;
  if (is_exact_name(race_table[ch->race].name,pnote->to_list)) return TRUE;
  return FALSE;
}

bool hide_note (CHAR_DATA *ch, NOTE_DATA *pnote)
{
  time_t last_read;

  if (IS_NPC(ch)) return TRUE;

  switch (pnote->type)
  {
    default:
      return TRUE;
    case NOTE_NOTE:
      last_read = ch->pcdata->last_note;
      break;
    case NOTE_OFFTOPIC:
      last_read = ch->pcdata->last_offtopic;
      break;
    case NOTE_COMPLAIN:
      last_read = ch->pcdata->last_complain;
      break;
    case NOTE_IDEA:
      last_read = ch->pcdata->last_idea;
      break;
    case NOTE_PENALTY:
      last_read = ch->pcdata->last_penalty;
      break;
    case NOTE_BUGREPORT:
      last_read = ch->pcdata->last_bugreport;
      break;
    case NOTE_NEWS:
      last_read = ch->pcdata->last_news;
      break;
    case NOTE_CHANGES:
      last_read = ch->pcdata->last_changes;
      break;
  }
  if (pnote->date_stamp <= last_read) return TRUE;
  if (!str_cmp(ch->name,pnote->sender)) return TRUE;
  if (!is_note_to(ch,pnote)) return TRUE;
  return FALSE;
}

int count_spool(CHAR_DATA *ch, NOTE_DATA *spool)
{
  int count = 0;
  NOTE_DATA *pnote;

  for (pnote = spool; pnote != NULL; pnote = pnote->next)
      if (!hide_note(ch,pnote)) count++;

  return count;
}

void do_unread(CHAR_DATA *ch,char *argument)
{
  int count;
  bool found = FALSE;

  if ( !EMPTY(argument) && !str_prefix(argument,"catchup"))
  {
     ch->pcdata->last_note = current_time;
     ch->pcdata->last_offtopic = current_time;
     ch->pcdata->last_complain = current_time;
     ch->pcdata->last_idea = current_time;
     ch->pcdata->last_penalty = current_time;
     ch->pcdata->last_bugreport = current_time;
     ch->pcdata->last_news = current_time;
     ch->pcdata->last_changes = current_time;
     return;
  }

    if ((count = count_spool(ch,news_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"Непрочтен%s %d групп%s новостей.(news)\n\r",
         count > 1 ? "ы" : "а",count, count > 1 ? "" : "а");
    }

    if ((count = count_spool(ch,changes_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"Непрочитанных изменений(changes) : %d .\n\r",count);
    }

    if ((count = count_spool(ch,note_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"Непрочитанных сообщениий(notes) : %d.\n\r",count);
    }

    if ((count = count_spool(ch,offtopic_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"Непрочитанных неигровых сообщений(offtopics) : %d.\n\r",count);
    }

    if ((count = count_spool(ch,idea_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"Непрочитанных идей(ideas) : %d.\n\r",count);
    }

    if ((IS_SET(ch->comm,COMM_CODER) || IS_IMMORTAL(ch)) && (count = count_spool(ch,bugreport_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"Непрочитанных багрепортов(bugreport) : %d.\n\r",count);
    }

    if (IS_TRUSTED(ch,ANGEL) && (count = count_spool(ch,penalty_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"%d %s.\n\r",count, "Непрочитанных штрафных санкций (penalty)");
    }

    if (IS_TRUSTED(ch,ANGEL) && (count = count_spool(ch,complain_list)) > 0)
    {
      found = TRUE;
      ptc(ch,"%d %s.\n\r",count, "Непрочитанных жалоб (complain)");
    }

    if (!found) stc("У тебя нет непрочитанных сообщений.\n\r",ch);


}

void update_read(CHAR_DATA *ch, NOTE_DATA *pnote)
{
  time_t stamp;

  if (IS_NPC(ch)) return;

  stamp = pnote->date_stamp;

  switch (pnote->type)
  {
    default:
      return;
    case NOTE_NOTE:
      ch->pcdata->last_note = UMAX(ch->pcdata->last_note,stamp);
      break;
    case NOTE_OFFTOPIC:
      ch->pcdata->last_offtopic = UMAX(ch->pcdata->last_offtopic,stamp);
      break;
    case NOTE_COMPLAIN:
      ch->pcdata->last_complain = UMAX(ch->pcdata->last_complain,stamp);
      break;
    case NOTE_IDEA:
      ch->pcdata->last_idea = UMAX(ch->pcdata->last_idea,stamp);
      break;
    case NOTE_PENALTY:
      ch->pcdata->last_penalty = UMAX(ch->pcdata->last_penalty,stamp);
      break;
    case NOTE_BUGREPORT:
      ch->pcdata->last_bugreport = UMAX(ch->pcdata->last_bugreport,stamp);
      break;
    case NOTE_NEWS:
      ch->pcdata->last_news = UMAX(ch->pcdata->last_news,stamp);
      break;
    case NOTE_CHANGES:
      ch->pcdata->last_changes = UMAX(ch->pcdata->last_changes,stamp);
      break;
  }
}

void save_notes(int type)
{
  FILE *fp;
  char *name;
  NOTE_DATA *pnote;

  switch (type)
  {
    default:
      return;
    case NOTE_NOTE:
      name = NOTE_FILE;
      pnote = note_list;
      break;
    case NOTE_OFFTOPIC:
      name = OFFTOPIC_FILE;
      pnote = offtopic_list;
      break;
    case NOTE_IDEA:
      name = IDEA_FILE;
      pnote = idea_list;
      break;
    case NOTE_BUGREPORT:
      name = BUGREPORT_FILE;
      pnote = bugreport_list;
      break;
    case NOTE_PENALTY:
      name = PENALTY_FILE;
      pnote = penalty_list;
      break;
    case NOTE_COMPLAIN:
      name = COMPLAIN_FILE;
      pnote = complain_list;
      break;
    case NOTE_NEWS:
      name = NEWS_FILE;
      pnote = news_list;
      break;
    case NOTE_CHANGES:
      name = CHANGES_FILE;
      pnote = changes_list;
      break;
  }

  fclose( fpReserve );
  if ( ( fp = fopen( name, "w" ) ) == NULL )
  {
      perror( name );
  }
  else
  {
    for ( ; pnote != NULL; pnote = pnote->next )
    {
      do_fprintf( fp, "Sender  %s~\n", pnote->sender);
      do_fprintf( fp, "Host    %s~\n", pnote->host);
      do_fprintf( fp, "Date    %s~\n", pnote->date);
      do_fprintf( fp, "Stamp   %l\n",  pnote->date_stamp);
      do_fprintf( fp, "To      %s~\n", pnote->to_list);
      do_fprintf( fp, "Subject %s~\n", pnote->subject);
      do_fprintf( fp, "Text\n%s~\n",   pnote->text);
    }
    fclose( fp );
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, bool delete)
{
  char to_new[MAX_INPUT_LENGTH];
  char to_one[MAX_INPUT_LENGTH];
  NOTE_DATA *prev;
  NOTE_DATA **list;
  const char *to_list;

  if (!delete)
  {
    // make a new list
    to_new[0]       = '\0';
    to_list = pnote->to_list;
    while ( *to_list != '\0' )
    {
      to_list     = one_argument( to_list, to_one );
      if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
      {
        strcat( to_new, " " );
        strcat( to_new, to_one );
      }
    }
   // Just a simple recipient removal?
   if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
   {
     free_string( pnote->to_list );
     pnote->to_list = str_dup( to_new + 1 );
     return;
   }
  }
  /* nuke the whole note */
  switch(pnote->type)
  {
    default:
      return;
    case NOTE_NOTE:
      list = &note_list;
      break;
    case NOTE_OFFTOPIC:
      list = &offtopic_list;
      break;
    case NOTE_COMPLAIN:
      list = &complain_list;
      break;
    case NOTE_IDEA:
      list = &idea_list;
      break;
    case NOTE_PENALTY:
      list = &penalty_list;
      break;
    case NOTE_BUGREPORT:
      list = &bugreport_list;
      break;
    case NOTE_NEWS:
      list = &news_list;
      break;
    case NOTE_CHANGES:
      list = &changes_list;
      break;
  }
  /* Remove note from linked list. */
  if ( pnote == *list ) *list = pnote->next;
  else
  {
    for ( prev = *list; prev != NULL; prev = prev->next )
    {
      if ( prev->next == pnote ) break;
    }

    if ( prev == NULL )
    {
      bug( "Note_remove: pnote not found.", 0 );
      return;
    }
    prev->next = pnote->next;
  }
  save_notes(pnote->type);
  free_note(pnote);
}

void note_attach( CHAR_DATA *ch, int type )
{
  NOTE_DATA *pnote;

  if ( ch->pnote != NULL ) return;
  pnote = new_note();

  pnote->next         = NULL;
  pnote->sender       = str_dup(ch->name);
  /* fixed APAR for `order lostlink_player note to all' (unicorn) */
  pnote->host         = (IS_SET(ch->act,PLR_AUTOSPIT) || !ch->desc) ?
          str_dup(ch->host) : str_dup(ch->desc->host);
  pnote->date         = str_dup("");
  pnote->to_list      = str_dup("");
  pnote->subject      = str_dup("");
  pnote->text         = str_dup("");
  pnote->type         = type;
  ch->pnote           = pnote;
}

void append_note(NOTE_DATA *pnote)
{
  FILE *fp;
  char *name;
  NOTE_DATA **list;
  NOTE_DATA *last;

  switch(pnote->type)
  {
    default:
      return;
    case NOTE_NOTE:
      name = NOTE_FILE;
      list = &note_list;
      break;
    case NOTE_OFFTOPIC:
      name = OFFTOPIC_FILE;
      list = &offtopic_list;
      break;
    case NOTE_COMPLAIN:
      name = COMPLAIN_FILE;
      list = &complain_list;
      break;
    case NOTE_IDEA:
      name = IDEA_FILE;
      list = &idea_list;
      break;
    case NOTE_BUGREPORT:
      name = BUGREPORT_FILE;
      list = &bugreport_list;
      break;
    case NOTE_PENALTY:
      name = PENALTY_FILE;
      list = &penalty_list;
      break;
    case NOTE_NEWS:
      name = NEWS_FILE;
      list = &news_list;
      break;
    case NOTE_CHANGES:
      name = CHANGES_FILE;
      list = &changes_list;
      break;
  }

  if (*list == NULL) *list = pnote;
  else
  {
    for ( last = *list; last->next != NULL; last = last->next);
    last->next = pnote;
  }

  fclose(fpReserve);
  if ( ( fp = fopen(name, "a" ) ) == NULL )
  {
    perror(name);
  }
  else
  {
    do_fprintf( fp, "Sender  %s~\n", pnote->sender);
    do_fprintf( fp, "Host    %s~\n", pnote->host);
    do_fprintf( fp, "Date    %s~\n", pnote->date);
    do_fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
    do_fprintf( fp, "To      %s~\n", pnote->to_list);
    do_fprintf( fp, "Subject %s~\n", pnote->subject);
    do_fprintf( fp, "Text\n%s~\n", pnote->text);
    fclose( fp );
  }
  // send to tg
  if ( (fp=fopen("send_note.txt","w") ) == NULL ) perror(name);
  else  {
    int exitcode;
    do_fprintf( fp, "From:%s\nTo: %s\nSubject:[%d] %s\n\n%s", pnote->sender,pnote->to_list,pnote->type,pnote->subject,pnote->text);
    fclose(fp);
    exitcode=system("./send_note.sh");
    log_printf ("sent to TG (%d)", exitcode);
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

void parse_note( CHAR_DATA *ch, const char *argument, int type )
{
  BUFFER *buffer;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char time_buf[25];
  NOTE_DATA *pnote;
  NOTE_DATA **list;
  char *list_name;
  int vnum;
  int anum;

  if (IS_SET(ch->act,PLR_TIPSY) && tipsy(ch,"note")) return;// tipsy by Dinger
  switch(type)
  {
    default:
      return;
    case NOTE_NOTE:
      list = &note_list;
      list_name = "сообщений(notes)";
      break;
    case NOTE_OFFTOPIC:
      list = &offtopic_list;
      list_name = "неигровых сообщений(offtopics)";
      break;
    case NOTE_COMPLAIN:
      list = &complain_list;
      list_name = "жалоб(complains)";
      break;
    case NOTE_IDEA:
      list = &idea_list;
      list_name = "идей(ideas)";
      break;
    case NOTE_PENALTY:
      list = &penalty_list;
      list_name = "штрафных санкций(penalties)";
      break;
    case NOTE_BUGREPORT:
      list = &bugreport_list;
      list_name = "багрепортов(bugreports)";
      break;
    case NOTE_NEWS:
      list = &news_list;
      list_name = "новостей(news)";
      break;
    case NOTE_CHANGES:
      list = &changes_list;
      list_name = "изменений(changes)";
      break;
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\0' || !str_prefix( arg, "read" ) )
  {
    bool fAll;

    if ( !str_cmp( argument, "all" ) )
    {
      fAll = TRUE;
      anum = 0;
    }

    else if ( argument[0] == '\0' || !str_prefix(argument, "next"))
    /* read next unread note */
    {
      vnum = 0;
      for ( pnote = *list; pnote != NULL; pnote = pnote->next)
      {
        if (!hide_note(ch,pnote))
        {
        ptc( ch, "{G[{x%3d{G] {Y%s:{x %s\n\r{C%s   {g%s\n\r{GTo:{x %s\n\r",
         vnum,
         pnote->sender,
         pnote->subject,
         pnote->date,!IS_IMMORTAL(ch) ? "": EMPTY(pnote->host) ? "[undefined]":pnote->host,
         pnote->to_list);
         page_to_char( pnote->text, ch );
         update_read(ch,pnote);
         return;
        }
        else if (is_note_to(ch,pnote)) vnum++;
      }
      ptc(ch,"У тебя нет непрочитанных %s.\n\r",list_name);
      return;
    }

    else if ( is_number( argument ) )
    {
      fAll = FALSE;
      anum = atoi( argument );
      if (anum < 0)
      {
       for (pnote=*list; pnote; pnote=pnote->next)
       if (is_note_to( ch, pnote ))    anum++;
      }
    }
    else
    {
      stc ( "Прочитать какой номер?\n\r", ch );
      return;
    }

    vnum = 0;
    for ( pnote = *list; pnote != NULL; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
      {
        ptc( ch, "{G[{x%3d{G] {Y%s:{x %s\n\r{C%s {c%s\n\r{GTo:{x %s\n\r",
         vnum - 1,
         pnote->sender,
         pnote->subject,
         pnote->date,!IS_IMMORTAL(ch) ? "":EMPTY(pnote->host) ?"undefined":pnote->host,
         pnote->to_list);
        page_to_char( pnote->text, ch );
        return;
      }
    }
    ptc(ch,"Тут нет столько %s.\n\r",list_name);
    return;
  }

  if ( !str_prefix( arg, "list" ) )
  {
    BUFFER *output;
    int listfrom=atoi (argument);
    int counter=0;

    vnum = 0;
    output = new_buf(); // Alloc space for output lines

    stc ("\n\rListing:\n\r",ch);

    if (listfrom < 0)
    {
      for (pnote=*list; pnote; pnote=pnote->next)
      if (is_note_to( ch, pnote ))
        listfrom++;
    }

    for ( pnote = *list; pnote != NULL; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) )
      {
        if (vnum >= listfrom)
        {
          char noteto[64];

          one_argument(pnote->to_list,noteto);

          do_printf( buf, "{w[%3d%s] {WFrom:%10s To:%10s Subj:%s{x\n\r",
             vnum, hide_note(ch,pnote) ? " " : "N",
             pnote->sender,noteto, pnote->subject );
          add_buf(output,buf);
          counter++;
        }
        vnum++;
      }
      if (counter>200)
      {
        do_printf(buf,"Слишком большой список");
        add_buf(output,buf);
        break;
      }
    }
    page_to_char(buf_string(output),ch);
    free_buf(output);
    if (!vnum)
    {
      switch(type)
      {
        default:
        case NOTE_NOTE:
          stc("Нет непрочитанных сообщений.\n\r",ch);
          break;
        case NOTE_OFFTOPIC:
          stc("Нет непрочитанных неигровых сообщений.\n\r",ch);
          break;
        case NOTE_IDEA:
          stc("Нет непрочитанных идей.\n\r",ch);
          break;
        case NOTE_PENALTY:
          stc("Нет непрочитанных штрафных санкций.\n\r",ch);
          break;
        case NOTE_NEWS:
          stc("Нет непрочитанных новостей.\n\r",ch);
          break;
        case NOTE_CHANGES:
          stc("Нет непрочитанных изменений.\n\r",ch);
          break;
      }
    }
    return;
  }

  if ( !str_prefix( arg, "remove" ) )
  {
    if ( !is_number( argument ) )
    {
      stc( "Удалить сообщение с каким номером?\n\r", ch );
      return;
    }

    anum = atoi( argument );
    vnum = 0;
    for ( pnote = *list; pnote != NULL; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && vnum++ == anum )
      {
        note_remove( ch, pnote, FALSE );
        stc( "Ok.\n\r", ch );
        return;
      }
    }
    ptc(ch,"Тут нет так много %s.",list_name);
    return;
  }

  if ( !str_prefix( arg, "delete" ) && IS_DEITY(ch))
  {
    if ( !is_number( argument ) )
    {
      stc( "Удалить сообщение с каким номером?\n\r", ch );
      return;
    }

    anum = atoi( argument );
    vnum = 0;
    for ( pnote = *list; pnote != NULL; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && vnum++ == anum )
      {
        note_remove( ch, pnote,TRUE );
        stc( "Ok.\n\r", ch );
        return;
      }
    }
    ptc(ch,"Тут нет так много %s.",list_name);
    return;
  }

  if (!str_prefix(arg,"help"))
  {
    stc("{wСправка по почтовой системе нашего Мира{x.\n\r",ch);
    stc("{CСинтаксис: {x\n\r",ch);
    stc("{G<тип сообщения> <параметры>: {x\n\r",ch);
    stc("{G   где <тип сообщения>: note, idea, news, changes, offtopic, bugreport, complain{x",ch);
    if (IS_IMMORTAL(ch))
       stc("{G, penalty{x",ch);
    stc(".\n\r{G   где <параметры>: один(несколько) из допустимых параметров: {x\n\r",ch);

    stc("\n\r{gЧтение сообщений: {x\n\r",ch);
    stc("{Gcatchup    :{x пометить все сообщения данного типа как \"прочитанные\".\n\r",ch);
    stc("{Glist       :{x вывести полный список сообщений.\n\r",ch);
    stc("{Glist -ХХ   :{x вывести список последних {WХХ{x сообщений.\n\r",ch);
    stc("{Gread       :{x чтение следующего непрочитанного сообщения.\n\r",ch);
    stc("{Gread ХХ    :{x чтение сообщения под номером {WХХ{x в списке сообщений.\n\r",ch);

    stc("{g\n\rРедактирование сообщений: {x\n\r",ch);
    stc("{G+ <строка> :{x добавить <строка> в редактируемое сообщение.\n\r",ch);
    stc("{G-          :{x удалить последнюю строку в редактируемом сообщении.\n\r",ch);
    stc("{Gclear      :{x очистить редактируемое сообщение.\n\r",ch);
    if (IS_DEITY(ch))
       stc("{Gdelete XX  :{R удалить сообщение под номером {WXX{x.\n\r",ch);

    stc("{Gedit       :{x запустить встроенный редактор соообщений.\n\r",ch);
    stc("{R             ВНИМАНИЕ! В редакторе не действуют комманды отличные от комманд редактора!\n\r{x",ch);
    stc("{Gpost       :{x отправить сообщение (аналогично параметру send).\n\r",ch);
    stc("{Gremove ХХ  :{x удалить написанное ВАМИ соообщение под номером {WХХ{x.\n\r",ch);
    stc("{Gto         :{x указать получателя. ({Rобязательное поле{x)\n\r",ch);
    stc("{Gsend       :{x отправить сообщение (аналогично параметру post).\n\r",ch);
    stc("{Gshow       :{x показать текущее редактируемое сообщение.\n\r",ch);
    stc("{Gsubject    :{x указать тему сообщения. ({Rобязательное поле{x)\n\r",ch);

    return;

  }

  if (!str_prefix(arg,"catchup"))
  {
    switch(type)
    {
      case NOTE_NOTE:
        ch->pcdata->last_note = current_time;
        break;
      case NOTE_OFFTOPIC:
        ch->pcdata->last_offtopic = current_time;
        break;
      case NOTE_COMPLAIN:
        ch->pcdata->last_complain = current_time;
        break;
      case NOTE_IDEA:
        ch->pcdata->last_idea = current_time;
        break;
      case NOTE_PENALTY:
        ch->pcdata->last_penalty = current_time;
        break;
      case NOTE_BUGREPORT:
        ch->pcdata->last_bugreport = current_time;
        break;
      case NOTE_NEWS:
        ch->pcdata->last_news = current_time;
        break;
      case NOTE_CHANGES:
        ch->pcdata->last_changes = current_time;
        break;
    }
    return;
  }

  // below this point only certain people can edit notes
  if (
      (!IS_SET(ch->comm,COMM_CODER)) &&
      (
       (type == NOTE_NEWS && !IS_TRUSTED(ch,ANGEL)) ||
       (type == NOTE_CHANGES && !IS_TRUSTED(ch,ANGEL))
      )
     )
  {
    ptc(ch,"Ты еще недостаточного уровня для написания %s.",list_name);
    return;
  }

  if ( !str_cmp( arg, "+" ) )
  {
    note_attach( ch,type );
    if (ch->pnote->type != type)
    {
      stc("Ты уже работаешь над другим типом сообщения.\n\r",ch);
      return;
    }

    if (strlen(ch->pnote->text)+strlen(argument) >= MAX_STRING_LENGTH-2)
    {
      stc( "Сообщение слишком длинное.\n\r", ch );
      return;
    }

    buffer = new_buf();

    add_buf(buffer,ch->pnote->text);
    add_buf(buffer,argument);
    add_buf(buffer,"\n\r");
    free_string( ch->pnote->text );
    ch->pnote->text = str_dup( buf_string(buffer) );
    free_buf(buffer);
    stc( "Ok.\n\r", ch );
    return;
  }

  if (!str_cmp(arg,"edit"))
  {
    note_attach( ch,type );
    if (ch->pnote->type != type)
    {
      stc("Ты уже работаешь над другим типом сообщения.\n\r",ch);
      return;
    }
    string_append(ch, &ch->pnote->text);
    return;
  }

  if (!str_cmp(arg,"-"))
  {
    int len;
    bool found = FALSE;

    note_attach(ch,type);
    if (ch->pnote->type != type)
    {
        stc("Ты уже работаешь над другим типом сообщения.\n\r",ch);
        return;
    }

    if (ch->pnote->text == NULL || ch->pnote->text[0] == '\0')
    {
        stc("Ты уже все удалил.\n\r",ch);
        return;
    }

    strcpy(buf,ch->pnote->text);

    for (len = strlen(buf); len > 0; len--)
    {
      if (buf[len] == '\r')
      {
        if (!found)
        {
          if (len > 0) len--;
          found = TRUE;
        }
        else // found the second one
        {
          buf[len + 1] = '\0';
          free_string(ch->pnote->text);
          ch->pnote->text = str_dup(buf);
          return;
        }
      }
    }
    buf[0] = '\0';
    free_string(ch->pnote->text);
    ch->pnote->text = str_dup(buf);
    return;
  }

  if ( !str_prefix( arg, "subject" ) )
  {
    note_attach( ch,type );
    if (ch->pnote->type != type)
    {
      stc("Ты уже работаешь над другим типом сообщения.\n\r",ch);
      return;
    }
    free_string( ch->pnote->subject );
    ch->pnote->subject = str_dup( argument );
    stc( "Ok.\n\r", ch );
    return;
  }

  if ( !str_prefix( arg, "to" ) )
  {
    note_attach( ch,type );
    if (ch->pnote->type != type)
    {
      stc("Ты уже работаешь над другим типом сообщения.\n\r",ch);
      return;
    }
    free_string( ch->pnote->to_list );
    ch->pnote->to_list = str_dup( argument );
    stc( "Ok.\n\r", ch );
    return;
  }

  if ( !str_prefix( arg, "clear" ) )
  {
    if ( ch->pnote != NULL )
    {
      free_note(ch->pnote);
      ch->pnote = NULL;
    }
    stc( "Ok.\n\r", ch );
    return;
  }

  if ( !str_prefix( arg, "show" ) )
  {
    if ( ch->pnote == NULL )
    {
      stc( "Ты не работаешь над сообщением.\n\r", ch );
      return;
    }

    if (ch->pnote->type != type)
    {
      stc("Ты не работаешь над этиим типом сообщений.\n\r",ch);
      return;
    }

    ptc( ch, "%s: %s\n\rTo: %s\n\r",
      ch->pnote->sender,
      ch->pnote->subject,
      ch->pnote->to_list);
     stc( ch->pnote->text, ch );
     return;
  }

  if ( !str_prefix( arg, "post" ) || !str_prefix(arg, "send"))
  {
    if ( ch->pnote == NULL )
    {
      stc( "Ты не работаешь над сообщением.\n\r", ch );
      return;
    }

    if (ch->pnote->type != type)
    {
      stc("Ты не работаешь над этим тиипом сообщений.\n\r",ch);
      return;
    }

    if (!str_cmp(ch->pnote->to_list,""))
    {
      stc("Ты должен указать получателя. Можно указать:\n\r{Call, <name>, <clanname>, <race>, imm, immortal, clan, noclan, leader.{x\n\r",ch);
      return;
    }

    if (!str_cmp(ch->pnote->subject,""))
    {
      stc("Ты должен указать тему (subject).\n\r",ch);
      return;
    }

    if (IS_SET(ch->act, PLR_NOPOST))
    {
      int i=number_range(1,11);
      switch (i)
      {
        case 1: stc("Слишком длинная строка.\n\r",ch);break;
        case 2: stc("Ась?\n\r",ch);break;
        case 3: stc("Набери help note.\n\r",ch);break;
        case 4: stc("Неверно указан адресат.\n\r",ch);break;
        case 5: stc("Для отправления сделайте quit затем зайдите заново.\n\r",ch);break;
        case 6: stc("Сообщение не может быть отправлено, \n\rтак как сотрудника КГБ читающего вашу почту \n\rвременно нет на месте.\n\r",ch);break;
        case 7: stc("Почтальона нет на месте.\n\r",ch);break;
        case 8: stc("Боги запрещают тебе отправлять сообщения.\n\r",ch);break;
        case 9: stc("Ты не работаешь над сообщением.\n\r",ch);break;
        case 10: stc("Вы не указали отправителя.\n\r",ch);break;
        case 11: stc("Сообщение не прошло цензуру.\n\r",ch);break;
      }
      return;
    }
    ch->pnote->next                 = NULL;
    strftime(time_buf,25,"%y%m%d %a %H:%M:%S:",localtime(&current_time));
    ch->pnote->date                 = str_dup( time_buf );
    ch->pnote->date_stamp           = current_time;
    append_note(ch->pnote);
    ch->pnote = NULL;
    stc("Сообщение послано.\n\r",ch);
    unread_update();
    return;
  }
  stc( "Ты не можешь этого сделать.\n\r", ch );
}

void do_note(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_NOTE);
}
void do_idea(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_IDEA);
}
void do_bugreport(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_BUGREPORT);
}
void do_penalty(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_PENALTY);
}
void do_news(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_NEWS);
}
void do_changes(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_CHANGES);
}
void do_offtopic(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_OFFTOPIC);
}
void do_complain(CHAR_DATA *ch,char *argument)
{
  parse_note(ch,argument,NOTE_COMPLAIN);
}

void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time)
{
  FILE *fp;
  NOTE_DATA *pnotelast;
  const char *buf;

  if ( ( fp = fopen( name, "r" ) ) == NULL ) return;

  pnotelast = NULL;
  for ( ; ; )
  {
    NOTE_DATA *pnote;
    char letter;

    do
    {
      letter = getc( fp );
      if ( feof(fp) )
      {
        fclose( fp );
        return;
      }
    }
    while ( isspace(letter) );
    ungetc( letter, fp );

    pnote = alloc_perm( sizeof(*pnote) );

    if (str_cmp(fread_word(fp),"sender")) break;
    pnote->sender = fread_string(fp);

    buf = fread_word(fp);
    if (str_cmp(buf,"host")) pnote->host = str_dup("");
    else
    {
      pnote->host = fread_string(fp);
      buf = fread_word(fp);
    }

    if (str_cmp(buf,"date")) break;
    pnote->date = fread_string(fp);

    if (str_cmp(fread_word(fp),"stamp")) break;
    pnote->date_stamp = fread_number(fp);

    if (str_cmp(fread_word(fp),"to")) break;
    pnote->to_list = fread_string(fp);

    if (str_cmp(fread_word(fp),"subject")) break;
    pnote->subject = fread_string(fp);

    if (str_cmp(fread_word(fp),"text")) break;
    pnote->text = fread_string(fp);

    if (free_time && pnote->date_stamp < current_time - free_time)
    {
      free_note(pnote);
      continue;
    }
    pnote->type = type;

    if (*list == NULL) *list = pnote;
    else pnotelast->next     = pnote;
    pnotelast       = pnote;
  }

  strcpy( strArea, NOTE_FILE );
  fpArea = fp;
  bug( "Load_notes: bad key word.", 0 );
  exit( 1 );
}

void load_notes(void)
{
  load_thread(NOTE_FILE,&note_list, NOTE_NOTE, 14*24*60*60);
  load_thread(OFFTOPIC_FILE,&offtopic_list, NOTE_OFFTOPIC, 14*24*60*60);
  load_thread(COMPLAIN_FILE,&complain_list, NOTE_COMPLAIN, 14*24*60*60);
  load_thread(IDEA_FILE,&idea_list, NOTE_IDEA, 28*24*60*60);
  load_thread(BUGREPORT_FILE,&bugreport_list, NOTE_BUGREPORT, 28*24*60*60);
  load_thread(PENALTY_FILE,&penalty_list, NOTE_PENALTY, 0);
  load_thread(NEWS_FILE,&news_list, NOTE_NEWS, 0);
  load_thread(CHANGES_FILE,&changes_list,NOTE_CHANGES, 0);
  log_printf("Note data loaded");
}

void unread_update(void)
{
 CHAR_DATA *ch;
 DESCRIPTOR_DATA *d;
 int count;

 for ( d = descriptor_list; d != NULL; d = d->next )
 {
   if (d->connected!=CON_PLAYING || !d->character
    || IS_NPC(d->character)) continue;
   ch=d->character;

   if ((count = count_spool(ch,news_list)) > 0)
     ptc(ch,"Непрочтен%s %d групп%s новостей.\n\r",
         count > 1 ? "ы" : "а",count, count > 1 ? "" : "а");

   if ((count = count_spool(ch,changes_list)) > 0)
    ptc(ch,"Непрочитанных изменений(changes) : %d .\n\r",count);

   if ((count = count_spool(ch,note_list)) > 0)
    ptc(ch,"Непрочитанных сообщениий(notes) : %d.\n\r",count);

   if ((count = count_spool(ch,offtopic_list)) > 0)
    ptc(ch,"Непрочитанных неигровых сообщениий(offtopics) : %d.\n\r",count);

   if ((count = count_spool(ch,complain_list)) > 0)
    ptc(ch,"Непрочитанных жалоб(complains) : %d.\n\r",count);

   if ((count = count_spool(ch,idea_list)) > 0)
    ptc(ch,"Непрочитанных идей(ideas) : %d.\n\r",count);

   if (IS_SET(ch->comm,COMM_CODER) && (count = count_spool(ch,bugreport_list)) > 0)
    ptc(ch,"Непрочитанных багрепортов(bugreport) : %d.\n\r",count);

   if (IS_TRUSTED(ch,ANGEL) && (count = count_spool(ch,penalty_list)) > 0)
    ptc(ch,"%d %s.\n\r",
        count, count > 1 ? "Были добавлены новые штрафные санкции(penalties)" :
                           "Была добавлена новая штрафная санкция(penalty)");
  }
}

/* Saboteur:
 Note types: NOTE 0     IDEA    1  PENALTY    2
             NEWS 3     CHANGES 4  BUGREPORT  5 OFFTOPIC 6
*/
void send_note( const char *from, const char *to, const char *subject, const char *text, int type)
{
  NOTE_DATA *note;
  char *buf;

  note = new_note();
  note->next         = NULL;
  note->sender       = str_dup(from);
  note->host         = str_dup("{GSystem{x");
  buf                = ctime( &current_time );
  buf[strlen(buf)-1] = '\0';
  note->date         = str_dup( buf );
  note->date_stamp   = current_time;
  note->to_list      = str_dup(to);
  note->subject      = str_dup(subject);
  note->text         = str_dup(text);
  note->type         = type;
  append_note(note);
}
