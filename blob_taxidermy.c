/* remix / blob_taxidermy.c -*- mode:C;coding:utf-8; -*-
   --------------------------------------------------------------------------------------------- 100
   taxix: Greek 'arrangement'
   taxonomy: classification and naming
   taxidermy: art of mounting dead skins to resemble the living form.
*/
void
dart_rib_csv( int dartrn, int fln, int type, int length ){  
  char damstr[6]="";
  char PTdumptape[8]="     , ";
  if(!( length>61 && (type==-3 || type==0) )) return;
  if(damaged) sprintf(damstr,"%03o",damaged);
  if( Vaddr->TP || Vaddr->dumptape || Vaddr->P_count )      
    sprintf( PTdumptape, "%1s%04d,%1d", (Vaddr->TP ? "T" : "P"), Vaddr->dumptape, Vaddr->P_count );
  sprintf( mdate6, "%2.2s%2.2s%2.2s", iso_mdatetime+2, iso_mdatetime+5, iso_mdatetime+8 );
  fprintf( O.ribs,
           "%7d,"          // fln
           "%-18.18s,"     // dartname
           "%10.10s,"      // d    #3
           "%7d,"
           "%3.3s,"
           "%1d,"               // #6
           "%3.3s.%3.3s,"
           "%6.6s, "
           "%16.16s, "          // #9
           "%16.16s, "
           "%10.10s, "
           "%10.10s, "
           "%s,"                // fields #13 and #14
           "%12llu,"
           "%12lld,"
           "%12llo\n",          // #17
           fln,
           dartname18wide,
           mdate6, // iso_mdatetime,
           dartrn,
           damstr,
           abs(type),
           write_ppn+3, write_ppn, write_program,
           iso_mdatetime,
           iso_adatetime,
           iso_cdatetime,
           iso_ddatetime,
           PTdumptape,
           ddloc,     // disk location of file, track number
           ddlng,     // disk length in PDP10 words
           dsatid );  // ID of public file system SYS:
}
void
output_csv( int drn, int fln, int tape ){
  char damstr[6]="";
  char damdot=' ',bindot=' ';
  char tldate[4];
  iso_date( iso_botape, botape_date, botape_time, botape_plusec );
  iso_date( iso_boreel, boreel_date, boreel_time, boreel_plusec );
  botape_plusec++; // advance estimated time one unit
  boreel_plusec++;
  damage_accumulated |= damaged;
  if ( damage_accumulated ) sprintf(damstr,"%03o",damage_accumulated);
  damdot = (damage_accumulated) ? '.' : ' ';
  bindot = (taxon==ETEXT || taxon==OTEXT) ? ' ' : '.';
  name_count++; // expect 1892023 final count.
  sprintf( mdate6, "%2.2s%2.2s%2.2s", iso_mdatetime+2, iso_mdatetime+5, iso_mdatetime+8 );
  strcpy( tldate, iso_mdatetime+28 ); // Three-Letter-Date code
  fprintf( O.nametag,
           "%7d,"          // fln
           "%-18.18s,"     // dartname
           "%10.10s,"     // d                date_only                      
           "%06d," // sn
           "%7d," // word count
           "%8d," // byte count
           "%3.3s.%3.3s,%6s,"  // write PPN Program
           "%-16.16s," // ISO m date time
           "%1c%1s%03o_%02o%1c,"                // bindot { _ | x } prot_mode damdot
           "%3.3s, "  // dam damage
           "%s\n",    // unix name of file
           fln,
           dartname18wide, mdate6, // iso_mdatetime,
           serial_number,
           B.seen,
           ConCount.bytes,
           write_ppn+3, write_ppn, write_program,
           iso_mdatetime,
           bindot, (prot&022 ? "x":"_"), // "x" prefix when NO read permitted. Read Protected from local and world.
           prot, mode, damdot,
           damstr,
           unixname );
  if(B.newblob)
    fprintf( O.blobattr,
             "%7d,"          // fln
             "%-18.18s,"     // dartname
             "%10.10s,"      // d                date_only
             "%06d,"         // sn
             "%s,"           // hash
             "%-7s,"         // taxon
             "%7d,%7d,"             // wrdcnt    zcnt
             "%7d,%7d,"             // notnull  u8cnt
             "%7d,%7d,%7d,"     // space alnum other
             "%8d,%7d,%7d,"     // bits  bytes null
             "%7d,%7d,%7d,"     // badtxt   ff   nl     formfeed newline counters
             "%7d,%7d,%7d\n",   // partial  rubout  downarrow
             fln,
             dartname18wide, mdate6, // iso_mdatetime,
             serial_number,             // sn
             hashhex,                   // hash8
             taxon_name[taxon],         // taxon             
             B.seen,                    // seen IS wrdcnt
             // B.blob_word_count,      // wrdcnt             
             B.zcount,                  // zcount               words of zeroes trailing             
             ConCount.not_null,         // notnull             
             ConCount.u8length,     // UTF8 strlen character count
             // ConCount.sailcount, // UTF8 strlen character count check             
             ConCount.space,            // cnt_space
             ConCount.alnum,            // cnt_alnum
             ConCount.other,            // cnt_other             
             ConCount.bits,             // cnt_bits             number of bits value ONE
             ConCount.bytes,            // cnt_bytes
             ConCount.null,             // cnt_nl             
             bad_text_bit_count,        // cnt_badtxtbit             
             ConCount.ff,               // cnt_ff
             ConCount.newline,          // cnt_nl
             ConCount.pd,               // cnt_pd       ∂ tokens seen, email date prefix
             ConCount.rubout,           // cnt_rubout
             ConCount.downarrow );      // down_arrow   XGP DVI POX as BINARY rather than TEXT.            
  fprintf( O.tapedex,
           "%7d,"          // fln          time-capsule file sequential serial number
           "%-18.18s,"     // dartname     'PRG.PRJ FILNAM.EXT' underbar for space in [2,2]
           " %3.3s,"       // tld
           "%10.10s,"      // d            date_only
           "%4d,"          // tape
           "%7d,"          // wrdcnt       word count seen
           "%-6s,"         // program      write program
           " %3d_%04d"   // dumptape
           "\n",
           fln,
           dartname18wide,
           tldate,
           mdate6,
           (taxon == EOT7) ? tape_number-1 : tape_number,
           B.seen,
           write_program,
           dump_number, (taxon == EOT7) ? tape_number-1 : tape_number
           );       
  fprintf( O.address,
           "%7d,"          // fln
           "%-18.18s,"     // dartname
           "%10.10s,"      // d                date_only six-digit
           "%12ld,"        // seekaddr         byte seek address based on flat_DART_data8
           "%7d,"          // drn              dart record number from undart and flatten.
           "%4d,"          // tape             from manually improved BOT & EOT tape markers
           "%3d,"          // dump                 feed back from TOTALS.LST [ MC, DMP ] report
           "%04d"          // reel                 solid inference based on 229 reel BOT markers
           "\n",
           fln,
           dartname18wide,
           mdate6,                
           seekaddr,
           drn,                
           tape_number,
           dump_number,
           reel_number );       
  fprintf( O.dates,
           "%7d,"       // fln
           "%-18.18s,"  // dartname
           "%10.10s, "  // d               date_only six-digit
           "%-19.19s, " // dttape          estimated date time tape 7-Track record written
           "%-19.19s, " // dtreel          estimated date time reel 9-Track record written
           "%16.16s, "  // dt              date time
           "%16.16s, "  // access          date time
           "%10.10s, "  // birth
           "%10.10s"    // ddump           dump date
           "\n",
           fln, dartname18wide, mdate6,
           iso_botape,
           iso_boreel,
           iso_mdatetime,
           iso_adatetime,
           iso_cdatetime,
           iso_ddatetime );
}

void
tally(int c);

void
convert_blob_into_utf8();

void
output_blob_sn();

void
de_frag( int drn,int fln, int type, int length ){
  // Concatenate the File-Continuation blobs
  // Append payload data words to file content blob
  // Ignore blob_word_count, size of payload determined by record length.
  switch(type){
  case -3:      // file start
  case 0:       // file continue
    memcpy( blob+B.seen, B.Tape+36, (length-61)*sizeof(pdp10_word) );
    B.seen += ( length-61 );
    break;
  case -9:      // gap
    memcpy( blob+B.seen, B.Tape+2, (length-2)*sizeof(pdp10_word) );      
    B.seen += ( length-2 );
    break;
  case 6:       // tape marker
    assert( length==12 );
    memcpy( blob+B.seen, B.Tape, length*sizeof(pdp10_word));
    B.blob_word_count = length;
    B.seen = length;
    break;
  }
}

void
de_dup( int drn, int fln, int type, int length ){
  int i; char *q;
  unsigned char *p;

  // Blob may be longer or shorter than expected ( log damage )
  if( (B.seen>0) && (B.seen != B.blob_word_count) ){
    if (B.seen < B.blob_word_count ){
      logdam( 4 ); // SHORTER than expected word count.
    }else{
      logdam( 8 ); // LONGER than expected word count.
    }
  }
  
  // Count trailing zero words
  for( B.zcount=0;
       B.zcount < B.seen  &&  blob[ B.seen - B.zcount-1 ].fw == 0;
       B.zcount++ ){
  }
  
  // Compute MD5 digest hash and lookup (or assign) to it a serial number.
  bzero(digest,sizeof(digest));
  md5_init_ctx( &ctx );
  md5_process_bytes( blob, B.seen*8, &ctx );
  md5_finish_ctx( &ctx, digest );
  
  // Hexadecimal 32-digit character string of numerals 0 to 9 and letters a to f.
  p = digest;
  q = hashhex;
  bzero(hashhex,sizeof(hashhex));
  if(1){
    for (i=0; i<16; i++) // 8-bits each loop cycle
      {
        *q++ = "0123456789abcdef"[(*p & 0xF0 ) >> 4 ];
        *q++ = "0123456789abcdef"[(*p++) & 0xF ];
      }
    hashhex[32]=0;
  }
  convert_blob_into_utf8(); // extra work - need TAXON for re-link destination
  B.newblob=0; // don't know yet
  serial_number = check_sn( hashhex );
  if(!( snseen[serial_number/8] & BIT[serial_number%8] )){
    // convert_blob_into_utf8();
    B.newblob=1;
  }
  if(serial_number==0){
    serial_number = fetch_sn( hashhex ); // coin new serial number when needed
    assert( serial_number <= MAXSN );
    fprintf(stderr,"Gak!_new_SN_HASH8  %06d,%s\n", serial_number, hashhex );
  }
  
  // Set bit-marking that this blob's serial number has been SEEN.
  snseen[serial_number/8] |= BIT[serial_number%8];
  // count the characters in the utf8 buffer, compare to the souce byte counters
  ConCount.not_null = ConCount.bytes - ConCount.null;
  ConCount.u8length = u8_strlen( utf8 );
  assert( ConCount.u8length == ConCount.sailcount );
  
  // Write the blob content once only
  if(B.newblob){
    output_blob_sn();
    blob_count++; // expect 900642 final count.
  }
}

void
tally(int c){
  // Character Counting and Bit Counting, for BLOB in-page attributes.
  // content counters // struct {int  bits, bytes, nul, newline, ff, space, alnum, other;} ConCount;
  // initialize table for counting bits
  //                                0 1 2 3  4 5 6 7  8 9 A B  C D E F
  static int number_of_bits[4096]={ 0,1,1,2, 1,2,2,3, 1,2,2,3, 2,3,3,4, };
  static int once_only=1;
  if(once_only){
    int n;
    once_only=0;
    for(n=16;n<4095;n++){
      // number_of_bits[n] = number_of_bits[n/256]+number_of_bits[(n/16)%16]+number_of_bits[n%16];
      number_of_bits[n] = number_of_bits[n>>8] + number_of_bits[(n>>4)&15] + number_of_bits[n&15]; // thirds
    }
    // verify bit counting table for seven-bit SAIL characters.
    for(n=0;n<127;n++){
      assert(number_of_bits[n] == ((n&1?1:0)+(n&2?1:0)+(n&4?1:0)+(n&8?1:0)+(n&16?1:0)+(n&32?1:0)+(n&64?1:0)));
    }
  }
  ConCount.bytes++;
  ConCount.bits += number_of_bits[c];
  switch(c){
  case 0:  case 013:  case 015:  case 0175:  case 0177: break;  // omit these SAIL code points
  default: ConCount.sailcount++; break; // check count for UTF-8 text u8_strlen
  }
  switch(c){
  case 0:               ConCount.null++;        break;
  case 1:               ConCount.downarrow++;   break; // escape character for XGP, DVI, IRF xerographic printers
  case '\n':            ConCount.newline++;     break; // line count
  case '\f':            ConCount.ff++;          break; // page count
  case '\017':          ConCount.pd++;          break; // ∂ partial derivative as SAIL-ASCII character 017.
  case ' ':             ConCount.space++;       break;
  case 'a'...'z':
  case 'A'...'Z':       histogram[c]++;
  case '0'...'9':       ConCount.alnum++;       break; // alphanumeric
  case 0177:            ConCount.rubout++;      break;
  default:              ConCount.other++;       break; // punctuation and symbols
  }
}

char *SAIL_ASCII[]=
  {
   // Decode SAIL seven bit codepoints into UTF8 strings.
   // Omit five codes 000 null, 013 vt, 015 cr, 0175 Stanford-ALT and 0177 rubout
   // 00      01      02      03          04      05      06      07
   "",       "↓",    "α",    "β",         "∧",    "¬",    "ε",    "π",  //  000
   "λ",     "\t",   "\n",     "",        "\f",   "",      "∞",    "∂",  //  010
   "⊂",      "⊃",    "∩",    "∪",         "∀",    "∃",    "⊗",    "↔",  //  020
   "_",      "→",    "~",    "≠",         "≤",    "≥",    "≡",    "∨",  //  030
   " ",      "!",   "\"",    "#",         "$",    "%",    "&",    "'",  //  040
   "(",      ")",    "*",    "+",         ",",    "-",    ".",    "/",  //  050
   "0",      "1",    "2",    "3",         "4",    "5",    "6",    "7",  //  060
   "8",      "9",    ":",    ";",         "<",    "=",    ">",    "?",  //  070
   "@",      "A",    "B",    "C",         "D",    "E",    "F",    "G",  // 0100
   "H",      "I",    "J",    "K",         "L",    "M",    "N",    "O",  // 0110
   "P",      "Q",    "R",    "S",         "T",    "U",    "V",    "W",  // 0120
   "X",      "Y",    "Z",    "[",         "\\",   "]",    "↑",    "←",  // 0130
   "'",      "a",    "b",    "c",         "d",    "e",    "f",    "g",  // 0140
   "h",      "i",    "j",    "k",         "l",    "m",    "n",    "o",  // 0150
   "p",      "q",    "r",    "s",         "t",    "u",    "v",    "w",  // 0160
   "x",      "y",    "z",    "{",         "|",    "",     "}",    ""    // 0170
  };

void
convert_blob_into_utf8(){
  char *p = utf8;               // bottom
  char *q = utf8+sizeof_utf8;   // top
  int n,ma;
  int a1,a2,a3,a4,a5;
  int page_count;
  memset( utf8, 0, sizeof_utf8 );               // destination
  memset( &ConCount, 0, sizeof(ConCount) );     // stats
  for( ma=0; ma<B.seen; ma++ )
    if( blob[ma].fw ){
      a1 = blob[ma].seven.a1;tally(a1);
      a2 = blob[ma].seven.a2;tally(a2);
      a3 = blob[ma].seven.a3;tally(a3);
      a4 = blob[ma].seven.a4;tally(a4);
      a5 = blob[ma].seven.a5;tally(a5);
      n = sprintf(p,"%s%s%s%s%s",
                  SAIL_ASCII[ a1 ],
                  SAIL_ASCII[ a2 ],
                  SAIL_ASCII[ a3 ],
                  SAIL_ASCII[ a4 ],
                  SAIL_ASCII[ a5 ]);
      // advance
      p += n;
      assert(p<q);
      // statistics      
      if( blob[ma].seven.bit35 ){
        ConCount.bits++;
        if(!(('0'<=a5 && a5<='9')||a5==' '))    // When final character non-numeric non-space,
          bad_text_bit_count++;                 // not-text file
      }
    }  
  switch(taxon){
  case BOT7:
  case EOT7:
  case BOT9:
  case EOT9:
  case XOT:
  case GAP:
  case ERRBLOB:
    return; // not TEXT
  }
  assert(taxon==BINARY); // still at default ? only file-start or file-continue records get to here.
  // Judgement: Is this blob clean good-looking seven-bit text ?
  n = sscanf(utf8,"COMMENT ⊗   VALID %05d PAGES\n"
             "C REC  PAGE   DESCRIPTION\n"
             "C00001 00001",&page_count);
  if(n==1){
    taxon = ETEXT; // 'E' text table of contents found.
    if( page_count != 1+ConCount.ff ){
      if(0)fprintf( stderr, "sn=%06d "
                    "defective COMMENT ⊗ VALID %05d PAGES vs %05d FormFeeds+1 '%s' %s\n",
                    serial_number, page_count, 1+ConCount.ff, write_program, dartname18wide );
    }
  }else if(!bad_text_bit_count){
    taxon = OTEXT; // other 'O' text
  }
}

void
output_blob_sn(){
  int n,bytcnt,sno;  // Serial-Numbered Output.
  char sn_path_data8[64];
  char sn_path_text[64];
  // DATA8
  // Write blob to Serial Numbered path with eight bytes per PDP10 word.
  errno=0;
  sprintf(sn_path_data8,"./OUTPUT/data8/sn/%06d",serial_number);
  if( G.data8_flag )
    if(access(sn_path_data8,F_OK)==-1){// Test for Non-Existence
      sno = open( sn_path_data8, O_CREAT | O_WRONLY | O_TRUNC, 0400 );
      if (!(sno>0)){perror("WTF:");fprintf(stderr,"Open failed filename sn_path='%s'\n", sn_path_data8);exit(1);}
      bytcnt = B.seen * 8;
      n = write( sno, blob, bytcnt );
      assert( n == bytcnt );
      assert(!close( sno ));
      touch_file_modtime( sn_path_data8, iso_mdatetime );
    }
  // TEXT UTF8
  // Write blob to Serial Numbered path as UTF8 text.
  errno=0;
  if( G.text_flag )
    if( taxon==ETEXT || taxon==OTEXT ){
      sprintf(sn_path_text,"./OUTPUT/text/sn/%06d",serial_number);
      if(access(sn_path_text,F_OK)==-1){// Test for Non-Existence
        sno = open( sn_path_text, O_CREAT | O_WRONLY | O_TRUNC, 0444 );
        if (!(sno>0)){perror("WTF:");fprintf(stderr,"Open failed filename sn_path_text='%s'\n", sn_path_text);exit(1);}
        bytcnt = strlen(utf8);
        n = write( sno, utf8, bytcnt );
        assert( n == bytcnt );
        assert(!close( sno ));
        touch_file_modtime( sn_path_text, iso_mdatetime );
      }
    }
}
