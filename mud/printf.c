// $Id: printf.c,v 1.5 2004/07/29 06:39:14 mud Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

//  This file used by do_printf , do_fprintf,  printf_to_char functions
// begin of printf parsing
/**************************************************************************
printf function by Sergey Kulik aka Saboteur. Special for Forgotten Dungeon

 Formats:
   %d      - decimal
   %D      - unsigned decimal
   %l      - long decimal
   %u      - int64 decimal
   %c      - ASCII char
   %s      - ASCII string
**************************************************************************/
while (*fmt)
{
  if (*fmt == '%')
  {
    fmt++;
    left=FALSE;
    fMatch=FALSE;
    format=0;
    fmtenable=FALSE;

    if (*fmt == '-' )
    {
      left=TRUE;
      fmt++;
    }

    if (*fmt >= '0' && *fmt <= '9')
    {
      format=(*fmt-48);
      fmt++;
      if (*fmt >= '0' && *fmt <= '9')
      {
        format=10*format+(*fmt-48);
        fmt++;
      }
      fmtenable=TRUE;
    }

    if (*fmt == 'd')
    {
      int dec = va_arg(data,int);

      fMatch=TRUE;
      p = tmp;
      if (dec < 0)
      {
        *(buf++) = '-';
        dec = -dec;
        format--;
      }
      do
      {
        *(p++) = '0' + (dec%10);
        dec = dec/10;
        format--;
        if (fmtenable && format<=0) break;
      } while(dec);

      if (fmtenable && format>0 && !left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
      while ((--p) >= tmp)
      {
        *(buf++) = *p;
      }
      if (fmtenable && format>0 && left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
    }

    if (*fmt == 'D')
    {
      unsigned int dec = va_arg(data,unsigned int);

      fMatch=TRUE;
      p = tmp;
      do
      {
        *(p++) = '0' + (dec%10);
        dec = dec/10;
        format--;
        if (fmtenable && format<=0) break;
      } while(dec);
      if (fmtenable && format>0 && !left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
      while ((--p) >= tmp)
      {
        *(buf++) = *p;
      }
      if (fmtenable && format>0 && left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
    }
    if (*fmt == 'l')
    {
      long dec = va_arg(data,long);

      fMatch=TRUE;
      p = tmp;
      if (*(fmt+1) == 'd') fmt++;
      if (dec < 0)
      {
        *(buf++) = '-';
        dec = -dec;
        format--;
      }
      do
      {
        *(p++) = '0' + (dec%10);
        dec = dec/10;
        format--;
        if (fmtenable && format<=0) break;
      } while(dec);
      if (fmtenable && format>0 && !left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
      while ((--p) >= tmp)
      {
        *(buf++) = *p;
      }
      if (fmtenable && format>0 && left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
    }
    if (*fmt == 'u')
    {
      int64 dec64=va_arg(data,int64);

      fMatch=TRUE;
      p = tmp;
      if (dec64 < 0)
      {
         *(buf++) = '-';
         dec64 = -dec64;
         format--;
      }
      do
      {
        *(p++) = '0' + (char)(dec64%10);
        dec64 = dec64/10;
        format--;
        if (fmtenable && format<=0) break;
      } while(dec64);
      if (fmtenable && format>0 && !left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
      while ((--p) >= tmp)
      {
        *(buf++) = *p;
      }
      if (fmtenable && format>0 && left) while (format>0)
      {
        format--;
        *(buf++) = ' ';
      }
    }
    if (*fmt == '%')
    {
      *(buf++) = '%';
      fMatch=TRUE;
    }
    if (*fmt == 'c')
    {
      *(buf++) = va_arg(data,int);
      fMatch=TRUE;
    }
    if (*fmt == 's')
    {
      p = va_arg(data,char*);
      if (p!=NULL)
      {
        while (*p && *p!='\0')
        {
          if (*p=='{' || *p=='`')
          {
            *(buf++) = *p++;
            if (!*p || *p=='\0') break;
            if (*p=='{' || *p=='`') format--;
          }
          else format--;
          *(buf++) = *p++;
          if (fmtenable && format<=0) break;
        }
      }
      else
      {
        *(buf++)='(';
        *(buf++)='n';
        *(buf++)='u';
        *(buf++)='l';
        *(buf++)='l';
        *(buf++)=')';
        format-=6;
      }
      if (fmtenable && format>0) while(format>0)
      {
        *(buf++)=' ';
        format--;
      }
      fMatch=TRUE;
    }
    if (!fMatch)
    {
      *(buf++) = '%';
      fmt--;
    }
  }
  else *(buf++) = *fmt;
  fmt++;
}
// end of printf parsing
