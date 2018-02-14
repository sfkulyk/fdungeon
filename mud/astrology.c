// $Id: astrology.c,v 1.6 2002/01/02 13:06:31 saboteur Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

Итак что нужно:
 Ввести периодичность дат:
 года (вроде 24, я нарисую таблицу)
 месяцы (12, по зодиаку)
 дни (высмотреть как там в каждом месяце, сделать учитывание високосного года)
 фазы луны (я цикл напишу)

Все циклы должны соответсвовать реальности. То есть если високосный год 199x то и в муде
должен быть именно он

структура date
 int day (1-28..31)
 int month (12)_
 int year (xx)
 int moonphase (28 phases)

char: ->pcdata->bd
день рождения чара.
 int day
 int month
 int year
 int hour

 int element (air,earth,fire,water)
Знаки зодиака делятся на 4 элемента (по три знака каждого)
существует пятый элемент - эфир, но он куда-то девался.
По слухам раньше в солнечной системе была еще одна планета, и иногда проявлялся еще один
знак зодиака, которого сейчас нет.

struct zodiac_data
{
  const char * name;
  const char * ename;
  int bday;
  int bmonth;
  int eday;
  int emonth;
  int element;
  int64 bonus;
};

struct zodiac_data zodiac_table[] =
{
//  name       ename         bday bmonth eday emonth element         bonus
  { "овен",    "aries",      21,  03,    20,   04,   ELEMENT_FIRE,   BONUS_STR},
  { "телец",   "taurus",     21,  04,    20,   05,   ELEMENT_EARTH,  BONUS_STR},
  { "близнецы","gemini",     21,  05,    21,   06,   ELEMENT_AIR,    BONUS_STR},
  { "рак",     "cancer",     22,  06,    22,   07,   ELEMENT_WATER,  BONUS_STR},
  { "лев",     "leo",        23,  07,    23,   08,   ELEMENT_,       BONUS_STR},
  { "дева",    "virgo",      24,  08,    23,   09,   ELEMENT_,       BONUS_STR},
  { "весы",    "libra",      24,  09,    23,   10,   ELEMENT_AIR,    BONUS_STR},
  { "скорпион","scorpio",    24,  10,    22,   11,   ELEMENT_WATER,  BONUS_STR},
  { "стрелец", "saggitarius",23,  11,    21,   12,   ELEMENT_,       BONUS_STR},
  { "козерог", "capricorn",  22,  12,    20,   01,   ELEMENT_,       BONUS_STR},
  { "водолей", "aquarius",   21,  01,    20,   02,   ELEMENT_WATER,  BONUS_STR},
  { "рыбы",    "pisces",     21,  02,    20,   03,   ELEMENT_AIR,    BONUS_STR},
  { "end",     "end",         1,   1,     1,    1,   0,              0}
};

struct date_data
{
  int month;
  int mdays;
  char *name;
  char *ename;
}

struct date_data month_table[]=
{
//  month mdays name        ename
  {  1,   31,   "Январь",   "January"   },
  {  2,   29,   "Февраль",  "February"  },
  {  3,   31,   "Март",     "March"     },
  {  4,   30,   "Апрель",   "April"     },
  {  5,   31,   "Май",      "May"       },
  {  6,   31,   "Июнь",     "June"      },
  {  7,   30,   "Июль",     "July"      },
  {  8,   31,   "Август",   "August"    },
  {  9,   30,   "Сентябрь", "September" },
  { 10,   31,   "Октябрь",  "October"   },
  { 11,   30,   "Ноябрь",   "November"  },
  { 12,   31,   "Декабрь",  "December"  }
}

struct year_data
{
  int year;
  char *name;
  char *rname1;
  char *rname2;
}

struct year_data year_table[]=
{

  { 1920, "monkey",  "Обезьяна", "Обезьяны" },
  { 1921, "Cock",    "Петух",    "Петуха"   },
  { 1922, "dog",     "Собака",   "Собаки"   },
  { 1923, "boar",    "Кабан",    "Кабана"   },
  { 1924, "rat",     "Крыса",    "Крысы"    },
  { 1925, "bullock", "Вол",      "Вола"     },
  { 1926, "tiger",   "Тигр",     "Тигра"    },
  { 1927, "rabbit",  "Кролик",   "Кролика"  },
  { 1928, "dragon",  "Дракон",   "Дракона"  },
  { 1929, "snake",   "Змея",     "Змеи"     },
  { 1930, "horse",   "Лошадь",   "Лошади"   },
  { 1931, "sheep",   "Овца",     "Овцы"     }
}
