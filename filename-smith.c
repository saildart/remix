typedef unsigned long long uint64;
typedef          long long  int64;
typedef unsigned long long    u64;
typedef          long long    i64;
typedef unsigned char uchar;

#ifndef PDP10WORD
#define PDP10WORD
typedef union
{
  uint64 fw;
  struct { uint64 c6:6,c5:6,c4:6,c3:6,c2:6,c1:6, pad:28; } sixbit; // ASCII sixbit + 040
  struct {  int64 right:18,  left:18,            pad:28; } half;   // half word left,,right
} pdp10_word;
#endif

#define MAXCNT 3020800
pdp10_word m[ 3020800 ];

/* 
   PDP-10 sixbit encoding: octal 00 space     to    octal 77 underbar
   ASCII space                     atsign              ASCII underbar
   040                             0100                          0137
   ↓                               ↓                              ↓
  " !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_"

  special convert SIXBIT tables

  _fname replaces double quote, comma, dot, back-slash, forward-slash with underbar.
  _safe  replaces the five punctuation
        characters ▶ ",.\/ ◀ with ▶ {~}|| ◀ for easier CSV format and BASH shelling.
  _ppn Project Programmer safe, assumes that any punctuation is a media error,
        and so replaces all punctuation with underbar.
*/
char *sixbit_fname = " !_"  "#$%&'()*+_-__0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ" "[_]^_";
char *sixbit_safe  = " !{"  "#$%&'()*+~-}|0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ" "[|]^_";
char *sixbit_ppn   = " __"  "_____________0123456789_______ABCDEFGHIJKLMNOPQRSTUVWXYZ" "_____";

/* ascii SIXBIT decoding functions
 * -------------------------------------------------------------------- *
 * sixbit     word into ASCII string 
 * sixbit halfword into ASCII string
 */
char *
sixbit_u64_into_ascii( char *p0, u64 u, char *tbl )
{
  char *p=p0;
  pdp10_word w;
  w.fw = u;
  // decode SIXBIT into ASCII by arithmetic would simply be + 040
  *p++ = tbl[w.sixbit.c1];
  *p++ = tbl[w.sixbit.c2];
  *p++ = tbl[w.sixbit.c3];
  *p++ = tbl[w.sixbit.c4];
  *p++ = tbl[w.sixbit.c5];
  *p++ = tbl[w.sixbit.c6];
  *p++ = 0;
  return p0;
}
char *
sixbit_word_into_ascii( char *p0, pdp10_word w, char *tbl )
{
  char *p=p0;
  // decode SIXBIT into ASCII by arithmetic would simply be + 040
  *p++ = tbl[w.sixbit.c1];
  *p++ = tbl[w.sixbit.c2];
  *p++ = tbl[w.sixbit.c3];
  *p++ = tbl[w.sixbit.c4];
  *p++ = tbl[w.sixbit.c5];
  *p++ = tbl[w.sixbit.c6];
  *p++ = 0;
  return p0;
}
char *
sixbit_halfword_into_ascii( char *p0, int halfword, char *tbl )
{
  char *p=p0;
  *p++ = tbl[(halfword >> 12) & 077];
  *p++ = tbl[(halfword >>  6) & 077];
  *p++ = tbl[ halfword        & 077];
  *p++ = 0;
  return p0;
}
void
omit_spaces(char *q)
{
  char *t,*p;
  for (p= t= q; *p; p++)
    if (*p != ' ')
      *t++ = *p;
  *t++ = 0;
}

void
force_to_lowercase(char *p)
{
  for (;*p;p++)    
    if ('A'<=*p && *p<='Z')
      *p |= 040;
}
void
force_to_uppercase(char *p)
{
  for (;*p;p++)    
    if ('a'<=*p && *p<='z')
      *p &= ~040;
}
void
space_to_underbar(char *p)
{
  for (;*p;p++)
    if (*p==' ')
      *p = '_';
}
void
leading_space_to_underbar(char *p)
{
  for (;*p;p++)
    if (*p==' ')
      *p = '_';
    else
      break;
}

void
dartname_(char* flat, char* filnam, char* ext, char* prj, char* prg)
{
  sprintf( flat, "%3.3s.%3.3s %-6.6s %-3.3s", prg, prj, filnam, ext );
  if( flat[15] != ' ' ) flat[14]='.'; // dot when extension
}
void
dartname_u64(char* flat, u64 u_ppn, u64 u_fil, u64 u_ext)
{
  char ppn[8],fil[8],ext[8];
  sixbit_u64_into_ascii( ppn, u_ppn, sixbit_ppn  );
  sixbit_u64_into_ascii( fil, u_fil, sixbit_safe );
  sixbit_u64_into_ascii( ext, u_ext, sixbit_safe );
  if ( u_ppn == 000022000022LL ){ // space preservation for [2,2] mail box file names
    space_to_underbar(fil);
  }
  sprintf( flat, "%3.3s.%3.3s %-6.6s %-3.3s", ppn+3, ppn, fil, ext );
  if( flat[15] != ' ' ) flat[14]='.'; // dot when extension
}
void
unixname_(char* flat, char* filnam, char* ext, char* prj, char* prg)
{
  if(!strcmp(prg,"  2") && !strcmp(prj,"  2")){
    space_to_underbar(filnam);
  }  
  omit_spaces(ext);
  sprintf( flat, "%.3s.%.3s/%.6s%s%.3s",
           prg, prj, filnam,
           *ext ? "." : "",
           ext );
  omit_spaces(flat);
  force_to_lowercase(flat);
}

void
touch_file_modtime(char *path,char *iso_dt){
  struct tm tm;
  struct utimbuf utm;
  //    
  bzero(&tm,sizeof(tm));
  strptime(iso_dt,"%F %R",&tm);
  utm.actime = utm.modtime = mktime( &tm );
  utime(path,&utm);
}
