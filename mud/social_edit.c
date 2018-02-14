// $Id: social_edit.c,v 1.7 2002/10/11 10:22:27 saboteur Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

// Online Social Editting Module, 
//   (c) 1996,97 Erwin S. Andreasen <erwinpip.dknet.dk>
//   See the file "License" for important licensing information
//   Upgraded by Saboteur

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "db.h"

struct social_type *social_table;

void load_social (FILE *fp, struct social_type *social,bool new)
{
  strcpy(social->name, fread_string (fp));
  social->char_no_arg   = fread_string (fp);
  social->others_no_arg = fread_string (fp);
  social->char_found    = fread_string (fp);
  social->others_found  = fread_string (fp);
  social->vict_found    = fread_string (fp);
  social->char_auto     = fread_string (fp);
  social->others_auto   = fread_string (fp);
  social->flag          = fread_flag64 (fp);
  social->help          = fread_string (fp);
}

void load_social_table ()
{
  FILE *fp;
  int i;
  bool new=FALSE;
        
  if ((fp=fopen(SOCIAL_FILE,"r"))==NULL)
  {
    log_string("BUG:Could not open " SOCIAL_FILE " for reading.");
    exit(1);
  }
  maxsocial=fread_number(fp);

  // IMPORTANT to use malloc so we can realloc later on
  social_table = malloc (sizeof(struct social_type) * (maxsocial+1));
        
  for (i = 0; i < maxsocial; i++) load_social (fp,&social_table[i],new);

  log_printf("Loaded %d socials",i);
  // For backwards compatibility
  strcpy(social_table[maxsocial].name, str_dup(""));
  fclose (fp);
}

void save_social (const struct social_type *s, FILE *fp)
{
  // get rid of (null)
  do_fprintf (fp, "%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%s\n%s~\n\n",
    s->name          ? s->name          : "" , 
    s->char_no_arg   ? s->char_no_arg   : "" , 
    s->others_no_arg ? s->others_no_arg : "" ,
    s->char_found    ? s->char_found    : "" , 
    s->others_found  ? s->others_found  : "" , 
    s->vict_found    ? s->vict_found    : "" ,
    s->char_auto     ? s->char_auto     : "" , 
    s->others_auto   ? s->others_auto   : "" ,
    pflag64(s->flag),s->help);
}

void save_social_table()
{
  FILE *fp;
  int i;

  fp = fopen (SOCIAL_FILE, "w");
        
  if (!fp)
  {
    bug ("Could not open " SOCIAL_FILE " for writing.",0);
    return;
  }

  do_fprintf (fp, "Maxsocials %d\n", maxsocial);
        
  for ( i = 0 ; i < maxsocial ; i++) save_social (&social_table[i], fp);
  fclose (fp);
}

// Find a social based on name
int social_lookup (const char *name)
{
  int i;
        
  for (i = 0; i < maxsocial ; i++)
    if (!str_cmp(name, social_table[i].name)) return i;
                        
  return -1;
}

// Social editting command
void do_sedit (CHAR_DATA *ch, const char *argument)
{
  char cmd[MAX_INPUT_LENGTH], social[MAX_INPUT_LENGTH];
  int iSocial;
        
  argument = one_argument (argument,cmd);
  argument = one_argument (argument,social);
        
  if (!cmd[0])
  {
    stc ("Ась? Набери {WHELP SEDIT{x чтобы посмотреть синтаксис.\n\r",ch);
    return;
  }
                
  if (!social[0])
  {
    stc ("С каким social вы хотите работать?\n\r",ch);
    return;
  }
        
  iSocial = social_lookup (social);
        
  if (str_cmp(cmd,"new") && (iSocial == -1))
  {
    stc ("Такого social нет.\n\r",ch);
    return;
  }

  if (!str_cmp(cmd, "delete"))
  {
    int i,j;
    struct social_type *new_table = malloc (sizeof(struct social_type) * maxsocial);
                
    if (!new_table)
    {
      stc ("Ошибка разделения памяти. Щас будет торба...\n\r",ch);
      return;
    }
                
    for (i = 0, j = 0; i < maxsocial+1; i++)
      if (i != iSocial)
      {
        new_table[j] = social_table[i];
        j++;
      }
    free (social_table);
    social_table = new_table;
                
    maxsocial--;
    stc ("Social удален.\n\r",ch);
  }
        
  else if (!str_cmp(cmd, "new"))
  {
    struct social_type *new_table;
                
    if (iSocial != -1)
    {
      stc ("Social с таким именем уже существует.\n\r",ch);
      return;
    }
                
    // reallocate the table
    // Note that the table contains maxsocial socials PLUS one empty spot!
    maxsocial++;
    new_table = realloc (social_table, sizeof(struct social_type) * (maxsocial + 1));
                
    if (!new_table)
    {
      stc ("Ошибка разделения памяти. Щас будет торба...\n\r",ch);
      return;
    }
                
    social_table = new_table;
                
    strcpy(social_table[maxsocial-1].name, str_dup (social));
    social_table[maxsocial-1].char_no_arg = str_dup ("");
    social_table[maxsocial-1].others_no_arg = str_dup ("");
    social_table[maxsocial-1].char_found = str_dup ("");
    social_table[maxsocial-1].others_found = str_dup ("");
    social_table[maxsocial-1].vict_found = str_dup ("");
    social_table[maxsocial-1].char_auto = str_dup ("");
    social_table[maxsocial-1].others_auto = str_dup ("");
    social_table[maxsocial-1].help = str_dup ("");
                
    strcpy(social_table[maxsocial].name, str_dup (""));
    // 'terminating' empty string
    stc ("Новый social добавлен.\n\r",ch);
  }
  else if (!str_cmp(cmd, "show")) /* Show a certain social */
  {
    ptc(ch, "Social: %s\n\r"
       "(cnoarg) Аргументов нет, {wигрок{x видит:\n\r"
        "%s\n\r\n\r"
       "(onoarg) Аргументов нет, {rдругие{x видят:\n\r"
        "%s\n\r\n\r"
       "(cfound) Цель найдена, {wигрок{x видит:\n\r"
        "%s\n\r\n\r"
       "(ofound) Цель найдена, {rдругие{x видят:\n\r"
        "%s\n\r\n\r"
       "(vfound) Цель найдена, {Gжертва{x видит:\n\r"
        "%s\n\r\n\r"
       "(cself) Цель - {wигрок{x:\n\r"
        "%s\n\r\n\r"
       "(oself) Цель - игрок, {rдругие{x видят:\n\r"
        "%s\n\r"
       "(help)  Аргументов нет. Справка:\n\r"
        "%s\n\r\n\r",
          social_table[iSocial].name,
          social_table[iSocial].char_no_arg,
          social_table[iSocial].others_no_arg,
          social_table[iSocial].char_found,
          social_table[iSocial].others_found,
          social_table[iSocial].vict_found,
          social_table[iSocial].char_auto,
          social_table[iSocial].others_auto,
          social_table[iSocial].help);
    return;
  }
  else if (!str_cmp(cmd, "cnoarg"))
  {
    free_string (social_table[iSocial].char_no_arg);
    social_table[iSocial].char_no_arg = str_dup(argument);          

    if (!argument[0]) stc ("Теперь игрок не будет видеть ничего, если не укажет параметров.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "onoarg"))
  {
    free_string (social_table[iSocial].others_no_arg);
    social_table[iSocial].others_no_arg = str_dup(argument);                

    if (!argument[0]) stc ("Теперь окружающие не будут видеть ничего, если не укажет параметров.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "cfound"))
  {
    free_string (social_table[iSocial].char_found);
    social_table[iSocial].char_found = str_dup(argument);           

    if (!argument[0]) stc ("Теперь игрок не будет видеть ничего, если цель найдена.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "ofound"))
  {
    free_string (social_table[iSocial].others_found);
    social_table[iSocial].others_found = str_dup(argument);         

    if (!argument[0]) stc ("Теперь окружающие не будут видеть ничего, если цель найдена.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "vfound"))
  {
    free_string (social_table[iSocial].vict_found);
    social_table[iSocial].vict_found = str_dup(argument);           
                                              
    if (!argument[0]) stc ("Теперь жертва не будет видеть ничего, если цель найдена.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "cself"))
  {
    free_string (social_table[iSocial].char_auto);
    social_table[iSocial].char_auto = str_dup(argument);            

    if (!argument[0]) stc ("Теперь игрок не будет видеть ничего, когда цель - он сам.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "oself"))
  {
    free_string (social_table[iSocial].others_auto);
    social_table[iSocial].others_auto = str_dup(argument);          

    if (!argument[0]) stc ("Теперь окружающие не будут видеть ничего, когда цель - он сам.\n\r",ch);
    else              ptc(ch,"Новое сообщение:\n\r%s\n\r", argument);
  }
  else if (!str_cmp(cmd, "help"))
  {
    free_string (social_table[iSocial].help);
    social_table[iSocial].help = str_dup(argument);

    if (!argument[0]) stc ("Справка по этому social теперь отсутствует.\n\r",ch);
    else              ptc(ch,"Справка по этому social:\n\r%s\n\r", argument);
  }
  else
  {
    stc ("Ась? Попробуй {WHELP SEDIT{x.\n\r",ch);
    return;
  }
  save_social_table();
}

char *soc_group_name(int64 group)
{
  static char buf[512];

  buf[0] = '\0';
  if (group & SOC_NOSPAM      ) strcat(buf, " Nospam");
  if (group & SOC_NOGLOBAL    ) strcat(buf, " NoGlobal");
  if (group & SOC_COMMON      ) strcat(buf, " Common");
  if (group & SOC_CULTURAL    ) strcat(buf, " Cultural");
  if (group & SOC_BAD         ) strcat(buf, " Bad");
  if (group & SOC_GOOD        ) strcat(buf, " Good");
  if (group & SOC_NOMOB       ) strcat(buf, " Nomob");
  if (group & SOC_LOVING      ) strcat(buf, " Loving");

  return ( buf[0] != '\0' ) ? buf+1 : "none";
}
