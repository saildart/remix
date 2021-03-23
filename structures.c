/* remix / structures.c
   ------------------------------------------------------------------------------------------------ 100
   -*- mode:C;coding:utf-8; -*-
*/
#ifndef PDP10WORD
#define PDP10WORD
typedef union {
  uint64 fw;
  struct {  int64  word:36,        :28; } full; //    signed
  struct {  int64 right:18,left:18,:28; } half; //    signed
  struct { uint64  word:36,        :28; } u36;  // Un-signed
  struct { uint64 right:18,left:18,:28; } u18;  // Un-signed
  struct { uint64 hi:12,mid:12,lo:12,:28; } third;
  struct { uint64 c6:6,c5:6,c4:6,c3:6,c2:6,c1:6,:28;}    sixbit; // sixbit+040
  struct { uint64 bit35:1,a5:7,a4:7,a3:7,a2:7,a1:7,:28;	} seven; // SAIL-ASCII
} pdp10_word;
#endif

struct Out {
  // ./OUTPUT/csv/ database tables
  FILE *blobattr;
  FILE *blobcon;
  FILE *nametag;
  FILE *ribs;
  FILE *tapedex;
  FILE *address;
  FILE *dates;
  FILE *class;
  FILE *type;
} O;

struct Global {  
  // Global option switch settings
  int verbose;
  int quiet;
  int fancy_flag;
  int histo_flag;
  int histo[5]; // Re-count the four DART record types: 6, -3, 0, -9 over the span.
  // histo[4] count PME Previous Media Errors taxon ERRBLOB
  int data8_flag;
  int octal_flag;
  int  text_flag;
  int   csv_flag;  
  // span specification
  int first, final;     // input span as tapes or reels INCLUSIVE.
  int final_plus_1;     // end-of-span ( limit, not included )
  int perm;    // start_at TAPE#1 or REEL#3000
  int rcrd;    // start at dart-record-number
  int count;   // default 1
  char *unit; // "tape" default, or "reel" specified by P# 3000:3228
} G;

struct Blob {
  // Blob aggregation
  pdp10_word Tape[390000]; // pdp10 words of 8 bytes each is almost 3 MB.
  pdp10_word Template[36]; // First 36. words of a -3 START_FILE dart-record for comparison
  int seen;
  int blob_word_count;
  int zcount;
  int newblob;
} B;

// SAIL-WAITS file name components : FILNAM . EXT [ PRJ , PRG ]
char prg[4], prj[4], filnam[8], ext[4]; // lower case cooked
char PRG[4], PRJ[4], FILNAM[8], EXT[4]; // upper case  raw

char dartname18wide[32];
char dartname18wide_[32]; // spaces replaced with underbar
char unixname[32];
char prg_prj[8];
char PRG_PRJ[8];

int mode, prot, hidate;
char write_program[16];
char write_ppn[16];
u64 ddloc, ddlng, dsatid; // rib values at offset 04, 05, 012

#define FIRST_TAPE    1
#define FINAL_TAPE 2984
// pythonic range  2985 bookend

#define FIRST_REEL 3000
#define FINAL_REEL 3228
// pythonic range  3229 bookend

off_t seekaddrtable[3229];  // seek address table of Perm# tape or reel
int dartrecntable[3229];  // dart record number of Perm# tape or reel, zero for lost tapes.

int dumpset_of_tape[3000]; // dump set # 1. to 858. of tape. Place holder values at #0 and #2984
int    reel_of_tape[3000]; // reel # 3000. to 3229. of tape.
int  fln_sequential[3000]; // increasing values of fln per each "tape slot" of time capsule
int     fln_of_tape[3000];
int    tape_in_slot[3000];

off_t seekaddr=0;       // byte offset seek position DART record

int dump_number=1;      //    1. to  854.
int tape_number=1;      //    1. to 2984.
int reel_number=3000;   // 3000. to 3228.

int name_count=0;       // DART name tags
int blob_count=0;       // number of unique blobs
int blob_word_count=0;
int serial_number=0;
int bad_text_bit_count=0;

int otnum; // word#24 in Darth ( possibly "other tape number" or T# Temp vs Perm tape )

#define MAX_BYTE_COUNT 65000000LL // Larger then any blob in the flat_DATA8_file
#define MAX_WORD_COUNT ((MAX_BYTE_COUNT+4)/5)

// pdp10_word blob[ MAX_WORD_COUNT ]; // 64-bit words.
// char utf8[ MAX_BYTE_COUNT ];
size_t sizeof_blob = MAX_BYTE_COUNT;
size_t sizeof_utf8 = MAX_BYTE_COUNT;
pdp10_word *blob;
char *utf8;
//
//      MD5 digest values of a file data BLOB
//
struct md5_ctx ctx;
unsigned char digest[32];
char hashhex[32+1]; // hexadecimal string

int64 histogram[128]={}; // SAIL-ASCII code-point counters

// Four Kingdoms (Linnaean classification) for data blob content : BINARY TEXT LABEL NOISE

const char* taxon_name[]={
                          "BINARY","ETEXT","OTEXT","ERRBLOB","GAP","XOT","BOT","EOT","BOT7","EOT7","BOT9","EOT9"};
enum taxon_value{
                 BINARY,   ETEXT,  OTEXT, ERRBLOB,  GAP,  XOT,  BOT,  EOT,  BOT7,  EOT7,  BOT9,  EOT9};
enum taxon_value taxon;

// content counters
struct {int bits, bytes,
        null,
    not_null,
    u8length, sailcount,
    downarrow,
    newline,
    ff,
    pd,
    space,
    alnum,
    rubout,
    other    
    ;
} ConCount;

// damage code bits, octal notation             decimal
// --------------------------------             -------
// 001 xorbad, bad blob data payload             1.
// 002 bad landmark(s) in DART header            2.
// 004 blob shorter than expected.               4.
// 010 blob longer  than expected.               8.
// 020 bad zip. DART zero cells NOT zero.       16. previous media error
// 040 mismatched file continuation.            32. bad fragment
// 100 bad SAT id value in RIB dsatid           64.
// 200          unused                         128.
// 400          unused                         256.
uint damaged=0;
uint damage_accumulated=0;
#define logdam(X) damaged|=X

// Estimated date-time that record was written
// first  7-Track DART  to tape,
// second 9-track MCOPY to reel.
int botape_date, botape_time, botape_plusec; char iso_botape[32];
int boreel_date, boreel_time, boreel_plusec; char iso_boreel[32];

struct Xot {
  int64 leng:18,type:18,:28;    // word#0.               000006000013
  uint64 DART__:36,:28;         // word#1. 'DART␣␣'      444162640000
  uint64 headtail:36,:28;       // word#2. '*HEAD*' 0125045414412
                                //      or '*TAIL*' 0126441515412
  uint64 date_lo:12,            // word#3 date time
    time: 11, :9,
    dateflag:1,
    date_hi:3,              :28;  // word#3  date time
  uint64 prg:18,     prj:18,:28;  // word#4. PPN
  uint64 tapeno:18,class:18,:28;  // word#5.
  uint64 word6:36,:28;            // word#6.                      000002  000002
  uint64 feet:36,:28;             // word#7.
  uint64 zip8;                    // word#8 is always zero.       000000  000000
  int64 neg1:36,:28;              // word#9 is always -1 == octal 777777,,777777
  uint64 zip10;                   // word#10 is always zero.      000000  000000
  uint64 word11:36,:28; // Rotated checksum of words #1. through #10. above.
} *TapeMarker;
/*
 *      DART record -3,,leng           Start File
 *      DART record  0,,leng           Continue File
 *
 * DART tape file record header is named 'darth'.
 * In data8 format,  the sizeof(darth) is 288. bytes which contains 36. PDP10 words.
 * Naturally, the Virtual Address pointer is named Vaddr.
 *
 */
struct Darth {
  int64 leng:18,type:18,:28;            // word  0. Type,,Leng
  uint64 DSK___:36,:28;                 // word  1. 'DSK␣␣␣' marker
  // RIB = Retrieval-Information-Block for the SAIL Ralph File System
  pdp10_word filnam;                    // word  2. FILNAM      DDNAM
  uint64 c_date:18,ext:18,:28;          // word  3. EXT         DDEXT
  uint64 date_lo:12,time:11,mode:4,prot:9,:28; // word  4.      DDPRO
  uint64 prg:18,prj:18,:28;             // word  5.             DDPPN
  uint64 track:36,:28;                  // word  6.             DDLOC
  uint64 count:22,:42;                  // word  7.             DDLNG    file size word count
  uint64 refdate:12, reftime:11, :41;   // word  8.             DREFTM   access date time
  uint64 dumpdate:15,
    dumptape:12, :2,                    // word  9.           DDMPTM   dump  date time
    never:1,           // dart bookkeeping
    reap:1,
    P_invalid:1,
    P_count:3,
    TP:1,              // Temporary or Permanent dump tape#
    :28;
  uint64 word10:36,:28; // DGRP1R
  uint64 word11:36,:28; // DNXTGP
  uint64 word12:36,:28; // DSATID
  // DQINFO five words
  uint64 word13:36,:28;
  uint64 word14:36,:28;
  pdp10_word write_program;  // word15. WPROGRM              wrtool
  pdp10_word write_ppn;      // word16. WPPN                 wrtppn
  uint64 word17:36,:28;
  // Thin AIR: six words from master copy tape-to-tape, MCOPY Leader.
  uint64 DART__:36,:28; // word 18. 'DART  ' marker
  uint64 _FILE_:36,:28; // word 19. '*FILE*' or 'CON  #' marker
  uint64 word20:36,:28;
  uint64 word21:36,:28;
  uint64 word22:36,:28; // MCOPY class==2,,reel#3000 to #3228 inclusive
  uint64 word23:36,:28;
  // Landmark AIR: five words of (sometimes unreliable / unnecessary) redundant information
  uint64 otnum:36,:28; // o_tape number 0 to 3526 seen at word24
  uint64 word25:36,:28; //  0
  uint64 word26:36,:28; // -1
  uint64 word27:36,:28; //  0
  uint64 word28:36,:28; // payload words remaining in file
  // Clear AIR: seven word block of zeroes.
  uint64 word29:36,:28; // 0
  uint64 word30:36,:28; // 0
  uint64 word31:36,:28; // 0
  uint64 word32:36,:28; // 0
  uint64 word33:36,:28; // 0
  uint64 word34:36,:28; // 0
  uint64 word35:36,:28; // 0
} *Vaddr;
/*
 *      DART trailer for -3
 *      DART trailer for  0
 *
 *      25. PDP10 words
 *
 *
 *
 *
 */

//
// DART record validation
//
uint64 xor_check_tally, xor_check_word;  char *xor_check_result;
uint64 zip_check,kword;
//
// DART record internal markers
//
char word_dsk[8];   // expect sixbit'DSK   ' 44 63 53 00 00 00
char word18[8];     // expect sixbit'DART  ' 44 41 62 64 00 00
char word19[8];     // expect sixbut'*FILE*' 12 46 51 54 45 12 or 'CON  #' 43 57 56 00 00 03
char word_pend[8];  // expect sixbit'$PEND$' 04 60 45 56 44 04 at Tape[m+length=2]
char word_mcsys[8]; // expect sixbit' MCSYS' 00 55 43 63 71 63

void
reset_globals(){
  B.newblob=0;
  hashhex[0]=0;
  B.seen= 0;
  serial_number= 0;
  B.blob_word_count= 0;
  B.zcount = 0;
  prot= mode= 0;
  iso_mdatetime[0]= 0;
  dartname18wide[0]= 0;
  dartname18wide_[0]= 0;  
  unixname[0]= 0;
  write_ppn[0]= write_program[0]= 0;
  ddloc = ddlng = dsatid = 0;
  damaged = damage_accumulated = 0; // redundant reset
  taxon = BINARY; // default
  bad_text_bit_count=0;
  otnum=0;
  memset(histogram,0,sizeof(histogram)); // ASCII value blob character frequency
}

