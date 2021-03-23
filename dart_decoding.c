/* dart-decoding.c
 * --------------------------------------------------------------------------------------------- 100
 *      DART records 6,,000013 for BOT and EOT tape events
 *
 * The HEADER AND TRAILER BLOCKS:
 *  Word  0: version,,length
 *          "version" is the positive version number of the DART that created this tape (VERSION);
 *           "length" is always octal 013, decimal 11. the length of the data following,
 *              includes the rotated checksum word below.
 * Word  1: 'DART  ' sixbit DART
 * Word  2: '*HEAD*' or '*TAIL*' sixbit, indicates Header or Trailer block,
 * Word  3: time,date	in file system format
 *               Bits 13:23 are time in mins, bits 24:35 are date.
 *               Bits 4:12 are unused (considered high minutes).
 *         DART Version 5: Bits 0-2 are high date.
 *         DART Version 6: Bit 3 means this is time from previous media
 * Word  4: ppn 		the name of the person running Dart.
 * Word  5: class,,tapno	Tape number of this tape
 *            Dump class of this dump is Permanent == 2 class tapes
              ( us future people never get to see any Temporary == 1 class tapes )
 *            The top bits of tapno indicate which sequence of tapes this nbr is in.
 * Word  6: relative dump,,absolute dump	(relative is within this reel)
 * Word  7: tape position in feet from load point
 * Word  8:	 0		reserved for future use
 * Word  9:	-1		all bits on
 * Word 10:	 0		all bits off
 * Word 11.	rotated checksum of words 1 through 012 above.
 */
#define HEAD 0125045414412LL // SIXBIT/*HEAD*/
#define TAIL 0126441515412LL // SIXBIT/*TAIL*/

void
touch_file_modtime(char *path,char *iso_dt);

void
octal_reel_tape( int dartrn, int fln, int type, int length ){
  /*
    Write octal for each DART record into a wide path name
    ./OUTPUT/octal/reeltape/P#reel/P#tape/#darn.type.dartname18wide.length

    Put some annotation within the octal, like a disassembly listing, as a Fancy option.

    RATIONALE:
    * octal is the classic PDP-10 native bit-exact format.
    * Us few humans from the PDP10 era can see
    patterns in octal wall paper from across the room.

    It is possible to convert this bulk data into hex, base64, database-binary, data8 or data9;
    each of which may be checksummed/hashed, compressed and/or encrypted.

    `2020-10-02 bgbaumgart@mac.com'
  */
  char dart_record_name[128];
  char directory_path_reel[128];
  char directory_path_tape[128];
  FILE *fdo;
  int i;
  if(!G.octal_flag)return; // when disabled ? exit immediately.
  sprintf( directory_path_reel,"./OUTPUT/octal/reeltape/p%04d",       reel_number );
  sprintf( directory_path_tape,"./OUTPUT/octal/reeltape/p%04d/p%04d", reel_number, tape_number);
  mkdir( directory_path_reel, 0775 );
  mkdir( directory_path_tape, 0775 );
  sprintf( dart_record_name, "./OUTPUT/octal/reeltape/p%04d/p%04d/%07d.%1d.%s.%6u words",
           reel_number,
           tape_number,
           dartrn,
           abs(Vaddr->type),
           dartname18wide,
           length );
  space_to_underbar( dart_record_name );
  if(G.verbose)fprintf(stderr,"%s\n",dart_record_name);
  fdo = fopen(dart_record_name,"w");    
  if(fdo){
    if( length<=61 ){ // 61. or fewer words
      // simple column octal only
      for(i=0;i<length;i++){
        fprintf(fdo,"%012llo\n",B.Tape[i].fw);
      }
    }else{
      //
      //        Header 36. words
      //
      for(i=0;i<27;i++){
        fprintf(fdo,"%012llo ",B.Tape[i].fw);
      }
      for(i=27;i<35;i++){
        fprintf(fdo, (i==28 ? "%012llo " : "%llo "),B.Tape[i].fw);
      }
      fprintf(fdo,"%llo\n",B.Tape[35].fw);
      //
      //        Payload (length-61.) words
      //                        61.  words is wrapper size
      //
      for(i=36;i<length-25;i++){
        fprintf(fdo,"%012llo\n",B.Tape[i].fw);
      }
      //
      //        Trailer 25 words
      //
      for(i=length-25;i<length-2;i++){
        fprintf(fdo,"%llo ",B.Tape[i].fw);
      }
      fprintf(fdo,"%012llo ", B.Tape[length-2].fw);
      fprintf(fdo,"%012llo\n",B.Tape[length-1].fw);
    }    
    fclose(fdo);
    touch_file_modtime( dart_record_name, iso_mdatetime );
  }
}


void
decode( int drn, int fln, int type, int length ){
  
  if(type != 0 ){ // This record is NOT a file continuation record, so clear off sticky state.
    bzero( write_program, sizeof(write_program));
    bzero( write_ppn,     sizeof(write_ppn));
    bzero( iso_cdatetime, sizeof(iso_cdatetime)); // birth
    bzero( iso_mdatetime, sizeof(iso_mdatetime)); // modified
    bzero( iso_adatetime, sizeof(iso_adatetime)); // accessed
    bzero( iso_ddatetime, sizeof(iso_ddatetime)); // dump
  }
  
  switch(type){    
  case 6:
    taxon =
      TapeMarker->tapeno < 3000 ? (
                                   TapeMarker->headtail == HEAD ? BOT7 :
                                   TapeMarker->headtail == TAIL ? EOT7 : XOT
                                   ) : (
                                        TapeMarker->headtail == HEAD ? BOT9 :
                                        TapeMarker->headtail == TAIL ? EOT9 : XOT
                                        );
    if( TapeMarker->class != 2 ){
      taxon = XOT; // only once, a bad second P00028.BOT out on tape 1290 1978-11-22
    }
    
    // 5486. tape BOT and EOT type=6 records. class==2.
    sprintf( FILNAM, "P%05d", TapeMarker->tapeno );
    strcpy( PRJ, "DMP");
    mdate = TapeMarker->date_lo | (TapeMarker->date_hi << 12);
    mtime = TapeMarker->time;
    iso_date( iso_mdatetime, mdate, mtime, 0);
    iso_mdatetime[10]=' ';
    iso_mdatetime[16]=0;
#define Offset_3hours 10800
    switch(taxon){
      
    case BOT7:
      strcpy( EXT, "BOT" );
      tape_number = TapeMarker->tapeno;
      dump_number = dumpset_of_tape[ tape_number ];
      strcpy( iso_bdatetime, iso_mdatetime ); // BOT date of pre-MCOPY tape
      botape_date = mdate;
      botape_time = mtime;
      botape_plusec = Offset_3hours;
      iso_date( iso_botape, botape_date, botape_time, botape_plusec );
      break;
      
    case EOT7:
      strcpy( EXT, "EOT" );
      tape_number = TapeMarker->tapeno + 1; // next tape#
      dump_number = dumpset_of_tape[ tape_number ];
      strcpy( iso_bdatetime, iso_mdatetime ); // BOT date of pre-MCOPY tape
      botape_date = mdate;
      botape_time = mtime;
      botape_plusec = Offset_3hours;
      iso_date( iso_botape, botape_date, botape_time, botape_plusec );
      break;
      
    case BOT9:
      strcpy( EXT, "BOT" );
      reel_number = TapeMarker->tapeno;
      boreel_date = mdate;
      boreel_time = mtime;
      boreel_plusec = Offset_3hours;
      iso_date( iso_boreel, boreel_date, boreel_time, boreel_plusec );
      break;
      
    case EOT9:
      strcpy( EXT, "EOT" );
      reel_number = TapeMarker->tapeno + 1; // next reel#
      boreel_date = mdate;
      boreel_time = mtime;
      boreel_plusec = Offset_3hours;
      iso_date( iso_boreel, boreel_date, boreel_time, boreel_plusec );
      break;
      
    case XOT:
      strcpy( EXT, "XOT" );
      break;
    default: break;
    }
    strcpy( PRG, EXT ); // Over write "SYS.DMP" to become  "BOT.DMP" or "EOT.DMP" or "XOT.DMP"
    break;
    
  case -9: // 61. gap records type=-9
    sprintf(FILNAM,"GAP_%02d",G.histo[3]);
    strcpy( EXT, "ERR");
    strcpy( PRJ, "GAP");
    strcpy( PRG, "GAP");
    strcpy( iso_mdatetime, iso_bdatetime ); // use BOT date
    taxon = GAP;
    break;
    
  case 0: // continue file
    damage_accumulated |= damaged;
    damaged = 0;
    break;
    
  case -3: // start file
    damage_accumulated = 0;
    damaged = 0;
    B.blob_word_count = Vaddr->count; // File size word count ( sometimes wrong )
    B.seen = 0;
    
    // copy DART File-Start record header into a Template
    // to validate the File-Continuation Records
    memcpy( B.Template, B.Tape, 8*35 );

    sixbit_halfword_into_ascii( PRG, Vaddr->prg,    sixbit_ppn   );
    sixbit_halfword_into_ascii( PRJ, Vaddr->prj,    sixbit_ppn   );
    sixbit_word_into_ascii(  FILNAM, Vaddr->filnam, sixbit_fname );
    sixbit_halfword_into_ascii( EXT, Vaddr->ext,    sixbit_fname );
    
    sixbit_word_into_ascii( write_program,  Vaddr->write_program ,      sixbit_fname );
    sixbit_word_into_ascii( write_ppn,      Vaddr->write_ppn,           sixbit_ppn   );
    
    //  8,318. File Start    *ERROR.ERR
    // 10,052. File Continue *ERROR.ERR
    if( !strcmp(FILNAM,"*ERROR") &&
        !strcmp(EXT,"ERR") &&
        !strcmp(PRJ,"ERR") &&
        !strcmp(PRG,"OR ")){
      taxon = ERRBLOB;
      G.histo[4]++;
      // sprintf( FILNAM,"%06d",sn); # would be nice, but it's too soon
      // sprintf( FILNAM,"%06d", fln % 1000000 ); // 20,000 in two million. 1% roulette.
      sprintf( FILNAM,"_%04d_", G.histo[4] );  //  8,318 ERRBLOB files
      strcpy( EXT, "ERR");
      strcpy( PRJ, "PRM");
      strcpy( PRG, "ERR");
      strcpy( write_program,"-REMIX" );
      strcpy( write_ppn,    "ERRPRM" );
      strcpy( iso_mdatetime, iso_bdatetime ); // use BOT date
    }
    mode = Vaddr->mode;
    prot = Vaddr->prot;
    cdate = Vaddr->c_date;                     // 18-bits
    mdate = Vaddr->date_lo | (cdate & 070000); // hi order 3-bits from cdate, lo order 12-bits
    adate = Vaddr->refdate;
    ddate = Vaddr->dumpdate;
    atime = Vaddr->reftime;
    mtime = Vaddr->time;
    iso_date(iso_cdatetime,cdate,   -1,0); // birth
    iso_date(iso_mdatetime,mdate,mtime,0); // modified date-time
    iso_date(iso_adatetime,adate,atime,0); // accessed date-time
    iso_date(iso_ddatetime,ddate,   -1,0); // dump
    ddloc = Vaddr->track;
    ddlng = Vaddr->count;
    dsatid = Vaddr->word12;
  } // switch( type )  
}

void
sailwaits_filenames(){
  /*
    GIVEN:      PRG PRJ FILNAM EXT

    RESULT:    
    dartname18wide        PRG.PRJ FILNAM.EXT
    dartname18wide_       PRG.PRJ_FILNAM.EXT
    unixname              prg.prj/filnam.ext
  */
  
  // For [2,2] file names, spaces become underbars.
  if( Vaddr->prg == 022 &
      Vaddr->prj == 022 ){    
    space_to_underbar( FILNAM );
  }else{
    omit_spaces( FILNAM );
  }
  dartname_( dartname18wide, FILNAM, EXT, PRJ, PRG );
  unixname_( unixname,       FILNAM, EXT, PRJ, PRG );
  
  strcpy(filnam,FILNAM);force_to_lowercase( filnam );
  strcpy(ext,EXT);force_to_lowercase(ext);
  strcpy(prj,PRJ);force_to_lowercase(prj);
  strcpy(prg,PRG);force_to_lowercase(prg);
  
  strcpy( dartname18wide_, dartname18wide );
  space_to_underbar( dartname18wide_ ); // wunderbar_! used for OCTAL reeltape files
  
  sprintf( PRG_PRJ, "%.3s.%.3s", PRG, PRJ ); omit_spaces( PRG_PRJ );
  sprintf( prg_prj, "%.3s.%.3s", PRG, PRJ ); force_to_lowercase(prg_prj ); omit_spaces(prg_prj);
  
  omit_spaces(filnam);
  omit_spaces(ext);
  omit_spaces(EXT);
}

void
damage( int drn, int fln, int type, int length ){
  int j;
  if( type==-9 || type==6 ) return;
  //
  // Verify DART Header record constants. Type 0 and -3.
  //
  sixbit_word_into_ascii(word_dsk,B.Tape[1],sixbit_ppn);  // sixbit'DSK   ' 044635 300000
  sixbit_word_into_ascii(word18,B.Tape[18], sixbit_ppn);  // sixbit'DART  ' 444162 640000
  sixbit_word_into_ascii(word19,B.Tape[19], sixbit_fname);// sixbit'*FILE*' 124651 544512 -3 records
  sixbit_word_into_ascii(word19,B.Tape[19], sixbit_fname);// sixbit'CON  #' 435756 000003  0 records
  sixbit_word_into_ascii(word_pend,B.Tape[length-2], sixbit_fname); // sixbit'$PEND$' 046045 564404
  sixbit_word_into_ascii(word_mcsys,B.Tape[21],      sixbit_ppn  );     // sixbit' MCSYS' 005543 637163
  //
  // DART header landmarks, damage #2 ?                                                       002 landmark damage
  //
  if(strcmp( word_dsk,"DSK   "))          logdam(2);
  if(strcmp(   word18,"DART  "))          logdam(2);
  if(type==-3)if(strcmp(word19,"*FILE*")) logdam(2);
  if(type== 0)if(strcmp(word19,"CON  #")) logdam(2);
  if(strcmp(word_pend,  "$PEND$"))        logdam(2);
  if(strcmp(word_mcsys," MCSYS"))         logdam(2);
  if( B.Tape[17].fw != 1 )                logdam(2); // 2321 records with this defect.
  //
  // Three valid SATID values
  //
  if(!( B.Tape[12].fw==03164236LL ||
        B.Tape[12].fw==0646353000000LL ||
        B.Tape[12].fw==0626353000000LL )) logdam(2);
  if(!( B.Tape[11].fw == 0 )) logdam(2); // 2,618,679. records valid
  if(!( B.Tape[10].fw == 1 )) logdam(2); // 2,912,964. records valid
  //
  // Check the XOR checksum, includes blob data payload, damage #1 ?                          001 bad XOR check
  //
  xor_check_tally = 0;
  xor_check_word = B.Tape[length-1].fw;
  for( j=1; j<=(length-2); j++){
    xor_check_tally ^= B.Tape[j].fw;
  }
  xor_check_result = (xor_check_tally == xor_check_word) ? "" : "xorBAD";
  if (xor_check_tally != xor_check_word){
    logdam(1); // xorBAD                                                                      001 xor BAD
  }
  //
  // check whether "PRMERR" (previous tape media) errors exist, damage =16. 020
  //
  zip_check = 0;
  for(j=3;j<=25;j++){
    zip_check |= B.Tape[length-j].fw;
  }
  zip_check |= B.Tape[25].fw;
  zip_check |= B.Tape[27].fw;
  for(j=29;j<=35;j++){
    zip_check |= B.Tape[j].fw;
  }
  if (zip_check){
    logdam(16);                         // zip BAD indicates Previous Media Errors 020        020 zip BAD previous media error
  }    
  // Verify File-Continuation Match
  // Detect Mismatch continuation damage =32. 020
  if(type==0){
    //
    // compare File-Continue
    //  with   File-Start Template
    // except for words 0, 19, 20, 24 and 28 which are allowed to differ.
    //
    for(j=0;j<=35;j++){
      switch(j){
      case 0:
      case 19:
      case 20:
      case 24:
      case 28: break; // OK for these slots to be different
      default:
        if( B.Tape[j].fw != B.Template[j].fw ){
          logdam(32); // file-continuation Mismatch 040                                        040 continuation mismatch
        }
      }
    }
  }
}
