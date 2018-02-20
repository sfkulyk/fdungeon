// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

// -----------------------------------------------------------------------
// Name:    string_edit
// Purpose: Clears string and puts player into editing mode.
// -----------------------------------------------------------------------
void string_edit (CHAR_DATA * ch, const char ** pString)
{
  stc ("-=======- Режим редактирования текста -=======-\n\r", ch) ;
  stc (" Введите :h или .h для вызова справки          \n\r", ch) ;
  stc ("-=============================================-\n\r", ch) ;

  // new empty string
  if (*pString != NULL) free_string (*pString) ;
  *pString = str_empty ;

  ch->desc->pString = pString ;
}

// -----------------------------------------------------------------------
// Name:     string_append
// Purpose:  Puts player into append mode for given string.
// Called:   By (many) olc_act.c
// -----------------------------------------------------------------------
void string_append (CHAR_DATA * ch, const char ** pString)
{
  stc ("-=======- Режим редактирования текста -=======-\n\r", ch) ;
  stc (" Введите :h или .h для вызова справки          \n\r", ch) ;
  stc ("-=============================================-\n\r", ch) ;
  
  // check for empty string
  if (*pString == NULL) *pString = str_empty ;
  string_show (ch, *pString) ;

  // final newline
  if (*(*pString + strlen(*pString) - 1) != '\r') stc ("\n\r", ch) ;

  ch->desc->pString = pString ;
}

// -----------------------------------------------------------------------
// Name:     string_add
// Purpose:  Interpreter for string editing.
// Called:   game_loop_xxxx (comm.c).
// -----------------------------------------------------------------------
void string_add (CHAR_DATA * ch, const char * argument)
{
  char buf[MAX_STRING_LENGTH];

  // process command
  if (*argument == '.' || *argument == ':' || *argument == '#')
  {
    char arg1 [MAX_INPUT_LENGTH] ;
    char arg2 [MAX_INPUT_LENGTH] ;

    argument = one_argument (argument, arg1) ;

    // arg1+1 contains start of command name

    // if command is a number - insert string before the numbered string
    // I'll keep that for backward compatibility
    // the new verision is :i <num> <str>
    if (is_number (arg1 + 1) || arg1[1] == 'i')
    {
      const char * temp ;
      int number ;

      // the second arg must be a number
      if (arg1[1] == 'i')
      {
        argument = one_argument (argument, arg2) ;

        if (EMPTY (argument) || EMPTY (arg2) || !is_number (arg2))
        {
          stc ("Синтаксис: :i <num> <string>\n\r", ch) ;
          return;
        }

        number = atoi (arg2) ;
      }
      else 
      {
        if (EMPTY (argument) || !is_number (arg1 + 1))
        {
          stc ("Синтаксис: :<num> <string>\n\r", ch) ;
          return ;
        }
        number = atoi (arg1 + 1) ;
      }

      // check if the string entered is too long to be stored
      if (strlen (*ch->desc->pString) + strlen (argument) >=
          MAX_STRING_LENGTH - 6)
      {
        stc ("Слишком длинное сообщение.\n\r", ch) ;
        return ;
      }

      // lookup the string to insert and do the insertion
      // return new text or NULL if no string is found
      temp = string_insert (*ch->desc->pString, argument, number) ;

      if (temp == NULL)
      {
        stc ("Укажите правильный номер строки.\n\r", ch) ;
        return ;
      }

      *ch->desc->pString = temp ;
      stc ("Строка добавлена.\n\r", ch) ;
      return ;
    }

    // clear the whole text
    if (arg1[1] == 'c')
    {
      stc ("Текст очищен.\n\r", ch) ;
      free_string (*ch->desc->pString) ;
      *ch->desc->pString = str_empty ;
      return ;
    }

    // show the text
    if (arg1[1] == 's')
    {
      stc ("Просмотр текста:\n\r", ch) ;
      string_show (ch, *ch->desc->pString) ;
      return ;
    }

    // format the text
    if (arg1[1] == 'f')
    {
      *ch->desc->pString = string_format (*ch->desc->pString) ;
      stc ("Текст отформатирован.\n\r", ch) ;
      return ;
    }

    // help
    if (arg1[1] == 'h')
    {
      stc (" Справка по редактированию строк:          \n\r", ch) ;
      stc (" Вместо ':' можно использовать '.' или '#')\n\r", ch) ;
      stc (" :h             - вызвать эту справку      \n\r", ch) ;
      stc (" :s             - просмотр всего текста    \n\r", ch) ;
      stc (" :f             - отформатировать текст    \n\r", ch) ;
      stc (" :c             - очистить весь тескт      \n\r", ch) ;
      stc (" :d <num>       - удалить одну строку      \n\r", ch) ;
      stc (" :i <num> <str> - вставить строку          \n\r", ch) ;
      stc (" :r <num> <str> - заменить строку          \n\r", ch) ;
      stc (" @              - закончить редактирование \n\r", ch) ;
      return ;
    }

    // other commands have the second argument to be a number
    argument = one_argument (argument, arg2) ;

    // replace one string with the another
    if (arg1[1] == 'r')
    {
      const char * temp ;

      if (EMPTY (argument) || EMPTY (arg2) || !is_number (arg2))
      {
        stc ("Синтаксис: :r <num> <string>\n\r", ch) ;
        return;
      }

      // check if the string entered is too long to be stored
      // do this check even including the string being replaced to
      // ease the life
      if (strlen (*ch->desc->pString) + strlen (argument) >=
          MAX_STRING_LENGTH - 6)
      {
        stc ("Слишком длинное сообщение.\n\r", ch) ;
        return ;
      }

      // lookup the string to insert and do the insertion
      // return new text or NULL if no string is found
      temp = string_replace (*ch->desc->pString, argument, atoi (arg2)) ;

      if (temp == NULL)
      {
        stc ("Укажите правильный номер строки.\n\r", ch) ;
        return ;
      }

      *ch->desc->pString = temp ;
      stc ("Строка заменена.\n\r", ch) ;
      return ;
    }

    // delete string at a position
    if (arg1[1] == 'd')
    {
      if (EMPTY (arg2) || !is_number (arg2))
      {
        stc ("Синтаксис: :r <num>\n\r", ch) ;
        return ;
      }

      *ch->desc->pString = string_remove (*ch->desc->pString, atoi (arg2)) ;
      ptc (ch, "Строка %d удалена.\n\r", atoi(arg2)) ;
      return ;
    }
      
    stc ("Нет такой команды редактирования.\n\r", ch) ;
    return ;
  }

  // end editing
  if (*argument == '#' || *argument == '@')
  {
    ch->desc->pString = NULL ;
    return ;
  }

  // check for too long strings
  if (strlen (*ch->desc->pString) + strlen (argument) >=
      MAX_STRING_LENGTH - 6)
  {
    stc ("Слишком длинное сообщение.\n\r", ch) ;
    return ;
  }

  // add string to the buffer
  strcpy (buf, *ch->desc->pString) ;
  strcat (buf, argument) ;
  strcat (buf, "\n\r")   ;

  // reallocate
  free_string (*ch->desc->pString) ;
  *ch->desc->pString = str_dup (buf) ;
}

// -----------------------------------------------------------------------
// Name:     string_remove
// Purpose:  remove one string.
// -----------------------------------------------------------------------
const char * string_remove (const char * strch, int number)
{
  char buf [MAX_STRING_LENGTH] ;
  int  i = 0, j = 0 ;

  // ensure we do not go over the static buffer
  while (strch[i] != '\0' && j < MAX_STRING_LENGTH - 1)
  {
    if (number != 1) buf[j++] = strch[i] ;
    if (strch[i++] == '\r') number-- ;
  }

  // must be terminated for proper use  
  buf[j] = '\0' ;

  // reallocate
  free_string (strch) ;
  return str_dup (buf) ;
}

// -----------------------------------------------------------------------
// Name:     string_insert
// Purpose:  insert one string at specified place.
// -----------------------------------------------------------------------
const char * string_insert (const char * orig, const char * new, int number)
{
  char buf [MAX_STRING_LENGTH] ;
  int  i = 0, j = 0 ;

  // ensure we do not go over the static buffer
  while (orig[i] != '\0' && i <= MAX_STRING_LENGTH - 6 &&
                            j <= MAX_STRING_LENGTH - 1)
  {
    if (number == 1)
    {
      // must be terminated here!
      buf[j] = '\0' ;

      strcat (buf, new) ;
      strcat (buf, "\n\r") ;
      strcat (buf, &orig[i]) ;

      // reallocate
      free_string   (orig) ;
      return str_dup (buf) ;
    }

    buf[j++] = orig[i] ;
    if (orig[i++] == '\r') number-- ;
  }

  return NULL ;
}

// -----------------------------------------------------------------------
// Name:     string_replace
// Purpose:  replace one string with another at specified place.
// -----------------------------------------------------------------------
const char * string_replace (const char * orig, const char * new, int number)
{
  char buf [MAX_STRING_LENGTH] ;
  int  i = 0, j = 0 ;
  int  replaced = 0 ;

  // ensure we do not go over the static buffer
  while (orig[i] != '\0' && i <= MAX_STRING_LENGTH - 6 &&
                            j <= MAX_STRING_LENGTH - 1)
  {
    if (!replaced && number == 1)
    {
      // must be terminated here!
      buf[j] = '\0' ;

      strcat (buf, new) ;
      strcat (buf, "\n\r") ;

      // indicate we've done the work
      replaced = 1 ;
    }
    else
    if (number != 1) buf[j++] = orig[i] ;
    if (orig[i++] == '\r') number-- ;
  }

  if (!replaced) return NULL ;

  // reallocate
  free_string (orig) ;
  return str_dup (buf) ;
}

// -----------------------------------------------------------------------
// Name:     string_show
// Purpose:  shows text with the string numbers.
// -----------------------------------------------------------------------
void string_show (CHAR_DATA * ch, const char * strch)
{
  char buf [MAX_STRING_LENGTH]  ;
  int  i = 0, j = 0, number = 1 ;

  for ( ;; i++)
  {
    // ensure we do not go over the static buffer
    if (strch[i] == '\0' || j == MAX_STRING_LENGTH) break ;

    buf[j++] = strch[i] ;

    if (strch[i] == '\r')
    {
      buf[j] = '\0' ;
      ptc (ch, "%2d. %s", number, buf) ;
      j = 0 ;
      number++ ;
    }
  }
}

// -----------------------------------------------------------------------
// Name:     string_format
// Purpose:  special string formating and word-wrapping.
//
// THIS FUNCTION IS NOT CHECKED - I'LL REWRITE IT LATER. (c) Invader
// -----------------------------------------------------------------------
const char * string_format (const char * oldstring)
{
  char xbuf[MAX_STRING_LENGTH];
  char xbuf2[MAX_STRING_LENGTH], *p;
  const char *rdesc;
  int i=0;
  bool cap=TRUE;
  
  xbuf[0]=xbuf2[0]=0;
  i=0;
  for (rdesc = oldstring; *rdesc; rdesc++)
  {
    if (*rdesc=='\n')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc=='\r') ;
    else if (*rdesc==' ')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc==')')
    {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!'))
      {
        xbuf[i-2]=*rdesc;
        xbuf[i-1]=' ';
        xbuf[i]=' ';
        i++;
      }
      else
      {
        xbuf[i]=*rdesc;
        i++;
      }
    }
    else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!') {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
        xbuf[i-2]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i-1]=' ';
          xbuf[i]=' ';
          i++;
        }
        else
        {
          xbuf[i-1]='\"';
          xbuf[i]=' ';
          xbuf[i+1]=' ';
          i+=2;
          rdesc++;
        }
      }
      else
      {
        xbuf[i]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i+1]=' ';
          xbuf[i+2]=' ';
          i += 3;
        }
        else
        {
          xbuf[i+1]='\"';
          xbuf[i+2]=' ';
          xbuf[i+3]=' ';
          i += 4;
          rdesc++;
        }
      }
      cap = TRUE;
    }
    else
    {
      xbuf[i]=*rdesc;
      if ( cap )
        {
          cap = FALSE;
          xbuf[i] = (char)UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i]=0;
  strcpy(xbuf2,xbuf);
  
  p=xbuf2;
  xbuf[0]=0;
  for ( ; ; )
  {
    for (i=0; i<77; i++)
    {
      if (!*(p+i)) break;
    }
    if (i<77)
    {
      break;
    }
    for (i=(xbuf[0]?76:73) ; i ; i--)
    {
      if (*(p+i)==' ') break;
    }
    if (i)
    {
      *(p+i)=0;
      strcat(xbuf,p);
      strcat(xbuf,"\n\r");
      p += i+1;
      while (*p == ' ') p++;
    }
    else
    {
      bug ("No spaces", 0);
      *(p+75)=0;
      strcat(xbuf,p);
      strcat(xbuf,"-\n\r");
      p += 76;
    }
  }

  while (
          *(p+i) && 
          ( *(p+i)==' '|| *(p+i)=='\n'|| *(p+i)=='\r')
    ) i--;
  
  *(p+i+1)=0;
  
  strcat(xbuf,p);
  
  if (xbuf[strlen(xbuf)-2] != '\n')
      strcat(xbuf,"\n\r");
  
  free_string(oldstring);
  return str_dup(xbuf);
}
