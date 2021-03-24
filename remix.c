#if 0 // remix.c -*- mode:C; coding:utf-8; -*-
NAME="remix"
SRC="/data/Remix."
DST="/usr/local/bin"
#           -Wall
gcc -g -m64       -Werror -o $DST/$NAME  $SRC/$NAME.c && echo OK || echo FAILED
echo $DST/$NAME
return 2>/dev/null || exit 0
#endif

#include "main_bigtop.c"  
#include "datetime_smith.c"
#include "structures.c"
#include "filename-smith.c"
  
#include "dart_decoding.c"
#include "load_feedback.c"
#include "tree_farming.c"
  
#include "md5.c"
#include "blob_taxidermy.c"  
#include "main_command_options.c"
#include "main_usage.c"
                                                                     
// ------------------------------------------------------------------------------------------- 100
// ----------------------------------------------------------------------------------------------------- 110

  int
  main(int ac, char *av[])
  {
   int fd;
   struct stat sb;
   size_t filesize;
   int dartfln=0;       //  file number
   int dartrn=1;        // record number
                        // slot          1..1023, 1024..1285, 1286, 1287..2984
   int slot=0;          // tape sequence 1..1023, 1025..1286, 1024, 1287..2984
   int tape=0;
   int type=0;
   size_t length=0;
   char *infile="./INPUT/TimeCapsule/flat_DART_data8";  // input from the Time Capsule
   int m,n;
   char *progress_format;
   
   if(ac==1)usage();
   fprintf(stderr,"\nBEGIN remix\n\n");
   command_line_options(ac,av);
   treefarm_startup(); // make ./OUTPUT/ sub directories and sub-sub directories.
   // Input flat_DART_data8 from the TimeCapsule directory
   fd= open( infile, O_RDONLY );
   if (fd == -1){
     fprintf(stderr,"\n\nPlease arrange symbolic links to access a path to read\n\n%50s\n\n",infile);
     handle_error("open");
   }
   if (fstat(fd, &sb) == -1) handle_error("fstat");
   filesize = sb.st_size;
   fprintf(stderr,"Opened %s\t       Time Capsule filesize is %ld bytes = %6.3f GBytes\n",
           infile, filesize, (float)filesize / (1024. * 1024. * 1024. ) );
   Vaddr =    (struct Darth *)B.Tape;
   TapeMarker = (struct Xot *)B.Tape;
  
   // Initialize an hsearch hash table
   // for looking up previously assigned ACCESSION NUMBERS, serial number, sn Values;
   // given the 32-character hexadecimal Key string for the MD5 of the data8 blob content.
   hcreate( 2*MAXSN );
   load_old_snhash8();
   load_dumpset();
   load_seekaddress();
   load_tape_fln();
  
   // clear off the sticky global date BOT values
   iso_date(iso_mcopy_bot_datetime,0,0,0);
   iso_date(iso_bdatetime,0,0,0);
   iso_date(iso_cdatetime,0,0,0);
   iso_date(iso_mdatetime,0,0,0);
   iso_date(iso_adatetime,0,0,0);
   iso_date(iso_ddatetime,0,0,0);

   // Allocate the large buffers
   blob   = (pdp10_word *)malloc(sizeof_blob);
   utf8   =       (char *)malloc(sizeof_utf8);
  
   // open output for Database-Tables in Comma-String-Value, CVS, format.
   O.blobattr =fopen("./OUTPUT/csv/attributes","w");assert(errno==0);
   O.address  =fopen("./OUTPUT/csv/coordinates","w");assert(errno==0);
   O.dates    =fopen("./OUTPUT/csv/dates","w");assert(errno==0);
   O.nametag  =fopen("./OUTPUT/csv/tags","w");assert(errno==0);
   O.ribs     =fopen("./OUTPUT/csv/ribs","w");assert(errno==0);
   O.tapedex  =fopen("./OUTPUT/csv/tapedex","w");assert(errno==0);
   // not needed yet
   // O.blobcon  =fopen("./OUTPUT/csv/blob","w");assert(errno==0);
   // O.class    =fopen("./OUTPUT/csv/class","w");assert(errno==0);
   // O.type     =fopen("./OUTPUT/csv/type","w");assert(errno==0);

   // row 1 for the column names
   fprintf( O.address,  "      0,          dartname,         d,    seekaddr,    drn,tape,dmp,reel\n");
   fprintf( O.blobattr, "      0,          dartname,         d,    sn,                           hash8,  taxon,   seen,   zcnt, notnul,  u8cnt,  space,  alnum,   other,   bits,  bytes,    nul, badtxt,     ff,     nl,     pd,    rub,   daro\n");
   fprintf( O.dates,    "      0,          dartname,         d,              dttape,              dtreel,              mdt,           access,      birth,     ddump\n");
   fprintf( O.nametag,  "      0,          dartname,         d,    sn,  words,  bytes,  wrtppn,  tool,             mdt, protdots,dam,           unixname\n");
   fprintf( O.ribs,     "      0,          dartname,         d,    drn,dam,ty,wrtppn,wrtool,              mdt,              adt,       born,       dump, PTnum,cnt,     ddloc,       ddlng,       satid\n");
   fprintf( O.tapedex,  "      0,          dartname, tld,         d,tape, wrdcnt,  tool, dumptape\n");

   // do nothing, except the initialization up to here.
   if( G.count==0 ) goto Finish; // ← this is considered harmful.
   // ( The most famous criticism of GOTO is a 1968 letter
   //   by Edsger Dijkstra titled 'Go To Statement Considered Harmful'. )
   span_validation();

   // Seek to the first tape-or-reel of the span.
   seekaddr = seekaddrtable[G.first];
   if( lseek( fd, seekaddr, SEEK_SET ) < 0 ) handle_error("lseek");
   n = read( fd, &B.Tape[0], 8); // READ first word of DART record.
   if(n!=8)handle_error("read 1st word of span");
  
   // Process span of DART records.
   for( dartrn = dartrecntable[G.first];
        dartrn < dartrecntable[G.final_plus_1];
        dartrn++ ){

     // Compute length of DART record.
     n = read( fd, &B.Tape[1], 8);               // READ second word of DART record.
     if(n!=8)handle_error("read 1 word");
     type = Vaddr->type;
     assert( type==0 || type==-3 || type==6 || type==-9 );
     if (dartrn==  22161) type=-3; // Fixup headless file AFTER reel P3000.EOT
     if (dartrn==2404328) type=-3; // Fixup headless file AFTER reel P3169.EOT
     G.histo[ abs(type)/3 ]++;     // histogram of dart record types
     length = (type == -9) ? B.Tape[1].full.word + 2 : B.Tape[0].half.right + ((type==6) ? 1 : 2);
    
     // READ the remaining words of this record;
     // PLUS one look ahead word of NEXT record.
     m = 8*(length - 1);    
     n = read(fd,&B.Tape[2],m);
     if( n != m ){
       if((m-n)==8){ // Expected short fall of 1 word, 8 bytes at End-of-File.
         B.Tape[length].half.left = 999; // make End-of-File. Any non-zero value will do.
       }else{
         handle_error("read DART record unexpected length");
       }
     }
    
     // Process one DART record.
     if(type!=0) dartfln++; // file number when NOT continuation. So tape markers and gaps count as files.
     decode( dartrn, dartfln, type, length ); // Decode external DART-nametag and DART-record-DATE.
     sailwaits_filenames();
     if( dartfln >= fln_sequential[slot+1] ){
       slot++;
       tape = tape_in_slot[slot];
       tape_number = tape_in_slot[slot];
       dump_number = dumpset_of_tape[ tape_number ];
     }

     if(1){
       damage( dartrn, dartfln, type, length ); // Check for Current-Damage as well as Previous-Media-Errors.
       de_frag(dartrn, dartfln, type, length ); // Concatenate DART record payload into file content blob.
     }
     //if(G.octal_flag)
     if(0)octal_reel_tape( dartrn, dartfln, type, length ); // write each DART record into octal/reeltape/
     //if(G.csv_flag)
     if(1)dart_rib_csv( dartrn, dartfln, type, length ); // append RIB line to its CSV file

     // Progress REPORT, about 3000 times for tape markers.     
     if(!G.quiet && type==6){ // tape markers ( once only for sequential markers )
       static int dartrn_previous=-10;
       if((dartrn - dartrn_previous)>9){
         progress_format =
           "    Progress REPORT :"
           " %6.2f %% : "
           " read %6.3f GBytes  %7d records %7d files    reel P%04d    tape# %4d   dump# %3d\n";
         fprintf(stderr, progress_format,
                 100.0 * (float)seekaddr / (float)filesize,
                 (float)seekaddr/(1024.*1024.*1024.),
                 dartrn, dartfln,
               reel_number, tape_number, dump_number );
       }
       dartrn_previous = dartrn;
     }
     // Detect whether this record is the "final" piece of a file.
     if ( type== 6 ||  // tape MARKER, is complete in one record.
          type==-9 ||  // gap,         is complete in one record.
          B.Tape[length].half.left ) // Next record is NOT a CONTINUE; SAIL File is now complete.
       {
         // Apply hash MD5 to data blob.
         // Output blob to /data8/sn/# and /text/sn/#
         de_dup( dartrn, dartfln, type, length );
         output_csv( dartrn, dartfln, tape );
         // symbolic links back to serial numbered DART file content blobs.
         if(G.fancy_flag)       symlink_trailblazer();
         // prepare for building the next file
         reset_globals();
       }
    
     // advance dart record
     seekaddr += length*8; // seekaddr might not be useful downstream, but is included in csv/address rows
     B.Tape[0] = B.Tape[length]; // move the look ahead word from last to first slot    
   }
   
   fprintf(stderr,progress_format,
           100.0 * (float)seekaddr / (float)filesize,
           (float)seekaddr/(1024.*1024.*1024.),
           dartrn, dartfln,
           reel_number, tape_number, dump_number );

  Finish:
  
   fprintf(stderr,"\n"); // Advance past the progress report text.
   fprintf(stderr,"%8d blobs\n",blob_count);
   fprintf(stderr,"%8d names\n",name_count);
   fprintf(stderr,"%8d dartfln file numbers\n",dartfln ); // same as name count.
   fprintf(stderr,"%8d dartrn record numbers\n",dartrn );
   fprintf(stderr,"\n");  // Advance past the progress report text.
   
   if(G.histo_flag){
     printf("G.histogram of DART record types\n");
     printf("type  6:  %8d BOT and EOT\n",G.histo[2]);
     printf("type -3:  %8d file start \n",G.histo[1]);
     printf("type  0:  %8d file continuation\n",G.histo[0]);
     printf("type -9:  %8d gaps \n",G.histo[3]);
     printf("  Total:  %8d records\n",G.histo[0]+G.histo[1]+G.histo[2]+G.histo[3]);
   }
   // final /OUTPUT/csv/ file closure
   fclose( O.blobattr );
   // fclose( O.blobcon );
   fclose( O.nametag );
   fclose( O.ribs );
   fclose( O.tapedex );
   fclose( O.address );
   fclose( O.dates );
   // fclose( O.type );
   // fclose( O.class );
   fprintf(stderr,"remix DONE\n\n");
  }
