// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
#include <stdio.h>
#include <string.h>

void  log_printf ( const char *fmt,...); //(C)Sab printf to logfile
#define CHAR_CONTROL   1
#define CHAR_SYMBOL    2
#define CHAR_DIGIT     3
#define CHAR_LETTER    4

//  KOI8-R
static unsigned char koi8rEncodeArr[] =
{
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
  0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
  0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
  0x9a,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
  0xb3,0xbf,0xaa,0xab,0xac,0xad,0xae,0xaf,
  0x9c,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0x9e,
  0xa3,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
  0xe1,0xe2,0xf7,0xe7,0xe4,0xe5,0xf6,0xfa,
  0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,
  0xf2,0xf3,0xf4,0xf5,0xe6,0xe8,0xe3,0xfe,
  0xfb,0xfd,0xff,0xf9,0xf8,0xfc,0xe0,0xf1,
  0xc1,0xc2,0xd7,0xc7,0xc4,0xc5,0xd6,0xda,
  0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,
  0xd2,0xd3,0xd4,0xd5,0xc6,0xc8,0xc3,0xde,
  0xdb,0xdd,0xdf,0xd9,0xd8,0xdc,0xc0,0xf1
};

unsigned char koi8rDecodeArr[] =
{
  128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 147, 148, 149, 150, 151,
  152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 168, 186, 165, 179, 191,
  168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 168, 170, 181, 178, 175,
  184, 185, 186, 187, 188, 189, 190, 191,
  254, 224, 225, 246, 228, 229, 244, 227,
  245, 232, 233, 234, 235, 236, 237, 238,
  239, 223, 240, 241, 242, 243, 230, 226,
  252, 251, 231, 248, 253, 249, 247, 250,
  222, 192, 193, 214, 196, 197, 212, 195,
  213, 200, 201, 202, 203, 204, 205, 206,
  207, 223, 208, 209, 210, 211, 198, 194,
  220, 219, 199, 216, 221, 217, 215, 218
};


// CP866
static unsigned char dosEncodeArr[] =
{
  128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 147, 148, 149, 150, 151,
  152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175,
  224, 225, 226, 227, 228, 229, 230, 231,
  232, 233, 234, 235, 236, 237, 238, 239,
};

unsigned char dosDecodeArr[] =
{
  0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7, // À-Ç
  0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf, // È-Ï
  0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7, // Ð-×
  0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf, // Ø-ß
  0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7, // à-ç
  0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef, // è-ï
  0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7, //
  0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf, //
  0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7, //
  0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf, //
  0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7, //
  0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf, //
  0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7, // ð-÷
  0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0x9f, // ø-ÿ
  0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7, //
  0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xcf  //
};

// Transliteration
static unsigned char transliterationEncodeArr[] =
{
  'A', 'B', 'V', 'G', 'D', 'E', 'Z', 'Z',
  'I', 'Y', 'K', 'L', 'M', 'N', 'O', 'P',
  'R', 'S', 'T', 'U', 'F', 'H', 'C', 'C',
  'S', 'S','\'', 'Y','\'', 'E', 'Y', 'Y',
  'a', 'b', 'v', 'g', 'd', 'e', 'z', 'z',
  'i', 'y', 'k', 'l', 'm', 'n', 'o', 'p',
  'r', 's', 't', 'u', 'f', 'h', 'c', 'c',
  's', 's','\'', 'y','\'', 'e', 'y', 'y'
};

// Encoding
// - Convert windows-1251 data to koi8-r                                    -
// - Args:                                                                  -
// -  pWindows     [IN]     the binary data (or NULL to close the encoder)  -
// -  pKoi8r       [IN]     pointer to buffer for the quoted-printable data -
// -                                                                        -
// - Returns: The length of the koi8-r data                                 -
long EncodeKOI8R (char * pWindows, char * pKoi8r)
{
  const char * pKoi8rStart = pKoi8r; /* Remember the start pos */
  char * pEnd;
  unsigned char ch;
  long nLen = strlen (pWindows);

  // Check if this call is to close the encoding
  if ((pWindows == NULL) || (nLen < 1)) return (0);

  // Loop through the binary data
  for (pEnd = pWindows + nLen; pWindows < pEnd; pWindows++)
  {
    ch = *pWindows;

    if (ch > 127) *pKoi8r++ = koi8rEncodeArr [ch - 128];
    else          *pKoi8r++ = ch;
  }
  return (pKoi8r - pKoi8rStart);
}

// - Convert windows-1251 data to cp866                                     -
// -                                                                        -
// - Args:                                                                  -
// -  pWindows     [IN]     the binary data (or NULL to close the encoder)  -
// -  pDos         [IN]     pointer to buffer for the quoted-printable data -
// -                                                                        -
// - Returns: The length of the cp866 data                                  -
long EncodeDOS (char * pWindows, char * pDos)
{
  const char * pDosStart = pDos; // Remember the start pos
  char * pEnd;
  unsigned char ch;
  long nLen = strlen (pWindows);

  // Check if this call is to close the encoding
  if ((pWindows == NULL) || (nLen < 1)) return (0);

  // Loop through the binary data
  for (pEnd = pWindows + nLen; pWindows < pEnd; pWindows++)
  {
      ch = *pWindows;

      if (ch >= 191) *pDos++ = dosEncodeArr [ch - 192];
      else           *pDos++ = ch;
  }
  return (pDos - pDosStart);
}

// - Convert windows-1251 data to translit                                  -
// -                                                                        -
// - Args:                                                                  -
// -  pWindows     [IN]     the binary data (or NULL to close the encoder)  -
// -  pTransliter  [IN]     pointer to buffer for the quoted-printable data -
// -                                                                        -
// - Returns: The length of the transliterated data                         -
long EncodeTRANSLITERATION (char * pWindows, char * pTransliteration)
{
  // Remember the start pos
  const char * pTransliterationStart = pTransliteration;
  char * pEnd;
  unsigned char ch;
  long nLen = strlen (pWindows);

  // Check if this call is to close the encoding
  if ((pWindows == NULL) || (nLen < 1)) return (0);

  // Loop through the binary data
  for (pEnd = pWindows + nLen; pWindows < pEnd; pWindows++)
  {
    ch = *pWindows;

    if (ch >= 192)  // Beginning of cyrillic letters in CP1251
    {
      *pTransliteration++ = transliterationEncodeArr [ch - 192];
      switch ( (unsigned int) ch )
      {
        case 198: // 'Æ'
        case 215: // '×'
        case 216: // 'Ø'
        case 230: // 'æ'
        case 247: // '÷'
        case 248: // 'ø'
          *pTransliteration++ = 'h';
          break;

        case 217: // 'Ù'
        case 249: // 'ù'
          *pTransliteration++ = 'c';
          *pTransliteration++ = 'h';
          break;

        case 222: // 'Þ'
        case 254: // 'þ'
          *pTransliteration++ = 'u';
          break;

        case 223: // 'ß'
        case 255: // 'ÿ'
          *pTransliteration++ = 'a';
          break;
      }
    }
    else *pTransliteration++ = ch;
  }
  return (pTransliteration - pTransliterationStart);
}

void TelnetCopy (char * dest, const char * source)
{
  if (!dest)
  {
    log_printf("TelnetCopy - destination not found for [%s].", source);
    return;
  }
  while (*source && *source != '\0')
  {
    if (*source == 'ÿ') *dest++ = 'ß';
    else                *dest++ = *source;
    source++;
  }
  *dest = '\0';
}

void TelnetCopy1 (char * dest, const char * source)
{
  while (*source != '\0') *dest++ = *source++;
  *dest = '\0';
}

void TelnetCopy2 (char * dest, const char * source)
{
  while (*source != '\0')
  {
    if (*source == 'ÿ')
    {
      *dest++ = 'ÿ';
      *dest++ = 'ÿ';
    }
    else *dest++ = *source;
    source++;
  }
  *dest = '\0';
}

char convert_in(int codepage, unsigned char source)
{
  if (codepage==2 && (unsigned char) source==255) return (unsigned char)'ß';

  if ((unsigned char)source > 127)
  {
    if (codepage==1) return (unsigned char)(koi8rDecodeArr[(unsigned char)source-128]);
    if (codepage==3) return (unsigned char)(dosDecodeArr[(unsigned char)source-128]);
  }
  return (unsigned char)source;
}

const unsigned char char_table[256]=
{
  CHAR_CONTROL, //   0
  CHAR_CONTROL, //   1
  CHAR_CONTROL, //   2
  CHAR_CONTROL, //   3
  CHAR_CONTROL, //   4
  CHAR_CONTROL, //   5
  CHAR_CONTROL, //   6
  CHAR_CONTROL, //   7
  CHAR_CONTROL, //   8
  CHAR_CONTROL, //   9
  CHAR_CONTROL, //  10
  CHAR_CONTROL, //  11
  CHAR_CONTROL, //  12
  CHAR_CONTROL, //  13
  CHAR_CONTROL, //  14
  CHAR_CONTROL, //  15
  CHAR_CONTROL, //  16
  CHAR_CONTROL, //  17
  CHAR_CONTROL, //  18
  CHAR_CONTROL, //  19
  CHAR_CONTROL, //  20
  CHAR_CONTROL, //  21
  CHAR_CONTROL, //  22
  CHAR_CONTROL, //  23
  CHAR_CONTROL, //  24
  CHAR_CONTROL, //  25
  CHAR_CONTROL, //  26
  CHAR_CONTROL, //  27
  CHAR_CONTROL, //  28
  CHAR_CONTROL, //  29
  CHAR_CONTROL, //  30
  CHAR_CONTROL, //  31
  CHAR_CONTROL, //  32 " "
  CHAR_SYMBOL,  //  33 "!"
  CHAR_SYMBOL,  //  34 "\""
  CHAR_SYMBOL,  //  35 "#"
  CHAR_SYMBOL,  //  36 "$"
  CHAR_SYMBOL,  //  37 "%"
  CHAR_SYMBOL,  //  38 "&"
  CHAR_SYMBOL,  //  39 "'"
  CHAR_SYMBOL,  //  40 "("
  CHAR_SYMBOL,  //  41 ")"
  CHAR_SYMBOL,  //  42 "*"
  CHAR_SYMBOL,  //  43 "+"
  CHAR_SYMBOL,  //  44 ","
  CHAR_SYMBOL,  //  45 "-"
  CHAR_SYMBOL,  //  46 "."
  CHAR_SYMBOL,  //  47 "/"
  CHAR_DIGIT,   //  48 "0"
  CHAR_DIGIT,   //  49 "1"
  CHAR_DIGIT,   //  50 "2"
  CHAR_DIGIT,   //  51 "3"
  CHAR_DIGIT,   //  52 "4"
  CHAR_DIGIT,   //  53 "5"
  CHAR_DIGIT,   //  54 "6"
  CHAR_DIGIT,   //  55 "7"
  CHAR_DIGIT,   //  56 "8"
  CHAR_DIGIT,   //  57 "9"
  CHAR_CONTROL, //  58 ":"
  CHAR_CONTROL, //  59 ";"
  CHAR_CONTROL, //  60 "<"
  CHAR_CONTROL, //  61 "="
  CHAR_CONTROL, //  62 ">"
  CHAR_CONTROL, //  63 "?"
  CHAR_CONTROL, //  64 "@"
  CHAR_LETTER,  //  65 "A"
  CHAR_LETTER,  //  66 "B"
  CHAR_LETTER,  //  67 "C"
  CHAR_LETTER,  //  68 "D"
  CHAR_LETTER,  //  69 "E"
  CHAR_LETTER,  //  70 "F"
  CHAR_LETTER,  //  71 "G"
  CHAR_LETTER,  //  72 "H"
  CHAR_LETTER,  //  73 "I"
  CHAR_LETTER,  //  74 "J"
  CHAR_LETTER,  //  75 "K"
  CHAR_LETTER,  //  76 "L"
  CHAR_LETTER,  //  77 "M"
  CHAR_LETTER,  //  78 "N"
  CHAR_LETTER,  //  79 "O"
  CHAR_LETTER,  //  80 "P"
  CHAR_LETTER,  //  81 "Q"
  CHAR_LETTER,  //  82 "R"
  CHAR_LETTER,  //  83 "S"
  CHAR_LETTER,  //  84 "T"
  CHAR_LETTER,  //  85 "U"
  CHAR_LETTER,  //  86 "V"
  CHAR_LETTER,  //  87 "W"
  CHAR_LETTER,  //  88 "X"
  CHAR_LETTER,  //  89 "Y"
  CHAR_LETTER,  //  90 "Z"
  CHAR_SYMBOL,  //  91 "["
  CHAR_SYMBOL,  //  92 "\"
  CHAR_SYMBOL,  //  93 "]"
  CHAR_SYMBOL,  //  94 "^"
  CHAR_SYMBOL,  //  95 "_"
  CHAR_SYMBOL,  //  96 "`"
  CHAR_LETTER,  //  97 "a"
  CHAR_LETTER,  //  98 "b"
  CHAR_LETTER,  //  99 "c"
  CHAR_LETTER,  // 100 "d"
  CHAR_LETTER,  // 101 "e"
  CHAR_LETTER,  // 102 "f"
  CHAR_LETTER,  // 103 "g"
  CHAR_LETTER,  // 104 "h"
  CHAR_LETTER,  // 105 "i"
  CHAR_LETTER,  // 106 "j"
  CHAR_LETTER,  // 107 "k"
  CHAR_LETTER,  // 108 "l"
  CHAR_LETTER,  // 109 "m"
  CHAR_LETTER,  // 110 "n"
  CHAR_LETTER,  // 111 "o"
  CHAR_LETTER,  // 112 "p"
  CHAR_LETTER,  // 113 "q"
  CHAR_LETTER,  // 114 "r"
  CHAR_LETTER,  // 115 "s"
  CHAR_LETTER,  // 116 "t"
  CHAR_LETTER,  // 117 "u"
  CHAR_LETTER,  // 118 "v"
  CHAR_LETTER,  // 119 "w"
  CHAR_LETTER,  // 120 "x"
  CHAR_LETTER,  // 121 "y"
  CHAR_LETTER,  // 122 "z"
  CHAR_SYMBOL,  // 123 "{"
  CHAR_SYMBOL,  // 124 "|"
  CHAR_SYMBOL,  // 125 "}"
  CHAR_SYMBOL,  // 126 "~"
  CHAR_CONTROL, // 127 " "
  CHAR_CONTROL, // 128 " "
  CHAR_CONTROL, // 129 " "
  CHAR_CONTROL, // 130 " "
  CHAR_CONTROL, // 131 " "
  CHAR_CONTROL, // 132 " "
  CHAR_CONTROL, // 133 " "
  CHAR_CONTROL, // 134 " "
  CHAR_CONTROL, // 135 " "
  CHAR_CONTROL, // 136 " "
  CHAR_CONTROL, // 137 " "
  CHAR_CONTROL, // 138 " "
  CHAR_CONTROL, // 139 " "
  CHAR_CONTROL, // 140 " "
  CHAR_CONTROL, // 141 " "
  CHAR_CONTROL, // 142 " "
  CHAR_CONTROL, // 143 " "
  CHAR_CONTROL, // 144 " "
  CHAR_CONTROL, // 145 " "
  CHAR_CONTROL, // 146 " "
  CHAR_CONTROL, // 147 " "
  CHAR_CONTROL, // 148 " "
  CHAR_CONTROL, // 149 " "
  CHAR_CONTROL, // 150 " "
  CHAR_CONTROL, // 151 " "
  CHAR_CONTROL, // 152 " "
  CHAR_CONTROL, // 153 " "
  CHAR_CONTROL, // 154 " "
  CHAR_CONTROL, // 155 " "
  CHAR_CONTROL, // 156 " "
  CHAR_CONTROL, // 157 " "
  CHAR_CONTROL, // 158 " "
  CHAR_CONTROL, // 159 " "
  CHAR_CONTROL, // 160 " "
  CHAR_CONTROL, // 161 " "
  CHAR_CONTROL, // 162 " "
  CHAR_CONTROL, // 163 " "
  CHAR_CONTROL, // 164 " "
  CHAR_CONTROL, // 165 " "
  CHAR_CONTROL, // 166 " "
  CHAR_CONTROL, // 167 " "
  CHAR_CONTROL, // 168 " "
  CHAR_CONTROL, // 169 " "
  CHAR_CONTROL, // 170 " "
  CHAR_CONTROL, // 171 " "
  CHAR_CONTROL, // 172 " "
  CHAR_CONTROL, // 173 " "
  CHAR_CONTROL, // 174 " "
  CHAR_CONTROL, // 175 " "
  CHAR_CONTROL, // 176 " "
  CHAR_CONTROL, // 177 " "
  CHAR_CONTROL, // 178 " "
  CHAR_CONTROL, // 179 " "
  CHAR_CONTROL, // 180 " "
  CHAR_CONTROL, // 181 " "
  CHAR_CONTROL, // 182 " "
  CHAR_CONTROL, // 183 " "
  CHAR_CONTROL, // 184 " "
  CHAR_CONTROL, // 185 " "
  CHAR_CONTROL, // 186 " "
  CHAR_CONTROL, // 187 " "
  CHAR_CONTROL, // 188 " "
  CHAR_CONTROL, // 189 " "
  CHAR_CONTROL, // 190 " "
  CHAR_CONTROL, // 191 " "
  CHAR_LETTER,  // 192 "À"
  CHAR_LETTER,  // 193 "Á"
  CHAR_LETTER,  // 194 "Â"
  CHAR_LETTER,  // 195 "Ã"
  CHAR_LETTER,  // 196 "Ä"
  CHAR_LETTER,  // 197 "Å"
  CHAR_LETTER,  // 198 "Æ"
  CHAR_LETTER,  // 199 "Ç"
  CHAR_LETTER,  // 200 "È"
  CHAR_LETTER,  // 201 "É"
  CHAR_LETTER,  // 202 "Ê"
  CHAR_LETTER,  // 203 "Ë"
  CHAR_LETTER,  // 204 "Ì"
  CHAR_LETTER,  // 205 "Í"
  CHAR_LETTER,  // 206 "Î"
  CHAR_LETTER,  // 207 "Ï"
  CHAR_LETTER,  // 208 "Ð"
  CHAR_LETTER,  // 209 "Ñ"
  CHAR_LETTER,  // 210 "Ò"
  CHAR_LETTER,  // 211 "Ó"
  CHAR_LETTER,  // 212 "Ô"
  CHAR_LETTER,  // 213 "Õ"
  CHAR_LETTER,  // 214 "Ö"
  CHAR_LETTER,  // 215 "×"
  CHAR_LETTER,  // 216 "Ø"
  CHAR_LETTER,  // 217 "Ù"
  CHAR_LETTER,  // 218 "Ú"
  CHAR_LETTER,  // 219 "Û"
  CHAR_LETTER,  // 220 "Ü"
  CHAR_LETTER,  // 221 "Ý"
  CHAR_LETTER,  // 222 "Þ"
  CHAR_LETTER,  // 223 "ß"
  CHAR_LETTER,  // 224 "à"
  CHAR_LETTER,  // 225 "á"
  CHAR_LETTER,  // 226 "â"
  CHAR_LETTER,  // 227 "ã"
  CHAR_LETTER,  // 228 "ä"
  CHAR_LETTER,  // 229 "å"
  CHAR_LETTER,  // 230 "æ"
  CHAR_LETTER,  // 231 "ç"
  CHAR_LETTER,  // 232 "è"
  CHAR_LETTER,  // 233 "é"
  CHAR_LETTER,  // 234 "ê"
  CHAR_LETTER,  // 235 "ë"
  CHAR_LETTER,  // 236 "ì"
  CHAR_LETTER,  // 237 "í"
  CHAR_LETTER,  // 238 "î"
  CHAR_LETTER,  // 239 "ï"
  CHAR_LETTER,  // 240 "ð"
  CHAR_LETTER,  // 241 "ñ"
  CHAR_LETTER,  // 242 "ò"
  CHAR_LETTER,  // 243 "ó"
  CHAR_LETTER,  // 244 "ô"
  CHAR_LETTER,  // 245 "õ"
  CHAR_LETTER,  // 246 "ö"
  CHAR_LETTER,  // 247 "÷"
  CHAR_LETTER,  // 248 "ø"
  CHAR_LETTER,  // 249 "ù"
  CHAR_LETTER,  // 250 "ú"
  CHAR_LETTER,  // 251 "û"
  CHAR_LETTER,  // 252 "ü"
  CHAR_LETTER,  // 253 "ý"
  CHAR_LETTER,  // 254 "þ"
  CHAR_LETTER   // 255 "ÿ"
};

