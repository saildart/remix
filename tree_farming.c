// remix / tree_farming.c -*- mode:C;coding:utf-8; -*-

void
treefarm_startup(){
  // Assure OUTPUT sub directories exist with respect to current working directory which is DOT.
  mkdir( "./OUTPUT/csv",   0775 );
  mkdir( "./OUTPUT/data8", 0775 );  mkdir( "./OUTPUT/data8/sn", 0775 );
  mkdir( "./OUTPUT/octal", 0775 );  mkdir( "./OUTPUT/octal/sn", 0775 );
  mkdir( "./OUTPUT/text",  0775 );  mkdir( "./OUTPUT/text/sn",  0775 );
  //
  //                                             Abandon      pumpkin / prg.prj / filnam.ext / reel#.dump#.tape#
  //                                                OK        pumpkin / prg.prj / filnam.ext.tape#
  mkdir( "./OUTPUT/PUMPKIN",  0775 ); // lowercase  OK        pumpkin / prg.prj / filnam.ext / version#
  mkdir( "./OUTPUT/YEAR",     0775 ); // UPPERCASE            YEAR    / {1972..1990} /    PRG.PRJ / FILNAM.EXT
  mkdir( "./OUTPUT/BOX",      0775 ); // UPPERCASE            BOX     / {1..858}     /    PRG.PRJ / FILNAM.EXT
  //
  mkdir( "./OUTPUT/octal/reeltape", 0775 ); //                  octal / reeltape / p3000 / p0001 /  drn.type._PRJ_PRG_FILNAM_EXT._10240_words
  mkdir( "./OUTPUT/text/unixlike",  0775 ); // lowercase        unixlike / prg / prj / filnam.ext.~version#~
  {
    int year,box;
    char path[64];
    for ( year=1970; year<=1990; year++){
      sprintf(path,"./OUTPUT/YEAR/%d",year);
      mkdir( path, 0775 );
    }
    // Define each "dump-set" as a "box-of-tapes"
    for ( box=1; box<=858; box++){
      sprintf(path,"./OUTPUT/BOX/%03d",box);
      mkdir( path, 0775 );
    }
  }
}
/* Historical note:

   The 2012 static web site ( www.SAILDART.org ) file system hierarchy is

   /Large2012/content/octal
   /Large2012/content/redacted
   /Large2012/content/sn
   /Large2012/content/text
   /Large2012/content/text-html
   /Large2012/content/text-utf8

   /Large2012/data13/sn
   /Large2012/data7/sn
   /Large2012/etc/apache2

   /Large2012/static/html / PRG / PRJ / FILNAM.EXT[PRJ,PRG]{version#}{,_octal,_blob}.html

   /Large2012/static/geomed-plt / sn#.svg
   /Large2012/static/iii / sn#
   /Large2012/static/ogg / sn#.ogg
   /Large2012/static/pdf
   /Large2012/static/plt / sn#.plt
   /Large2012/static/png / sn#.png
   /Large2012/static/svg / sn#.svg

   /Large2012/SYS0/track
   /Large2012/var/log
   /Large2012/var/www
*/
  
void
link_trailblazer(){
  /*
    Hard link additional hierarcies of filenames for study and presentation
    so the authentic Sail DART file content the (data8/sn) can be found
    by year with
    somewhat authentic UPPERCASE tags            YEAR / year# / PRG.PRJ / FILNAM.EXT
    by unixlike lowercase pathnames w/version#      unixlike / prg / prj / filnam.ext.~#~
    provide all instances retrieved from tapes      pumpkin / prg.prj / filnam.ext / {'.' when binary} dump# {'.' when damaged}
    by HTML server (Apache) URL remapping
  */
  char sub_dir[64];
  char sub_sub[64];
  char new_path[64];
  char new_plink[64];
  char target_sn[64];
  char damdot = damage_accumulated ? '.' : 0;
  //
  switch(taxon){
  case BOT: case BOT7: case BOT9:
  case EOT: case EOT7: case EOT9:
  case XOT: case GAP: case ERRBLOB:
  case BINARY:
    
    //  YEAR
    sprintf( target_sn,"./OUTPUT/data8/sn/%06d", serial_number );
    sprintf( sub_dir,  "./OUTPUT/YEAR/%4.4s/%s",           iso_mdatetime, PRG_PRJ );
    sprintf( new_path, "./OUTPUT/YEAR/%4.4s/%s/.%s%s%s%c", iso_mdatetime, PRG_PRJ, FILNAM, (strlen(EXT)?".":""), EXT, damdot );
    mkdir(   sub_dir, 0775 );
    unlink(  new_path );
    link( target_sn, new_path );

    //  BOX
    sprintf( target_sn,"./OUTPUT/data8/sn/%06d", serial_number );
    sprintf( sub_dir,  "./OUTPUT/BOX/%03d/%s",           dump_number, PRG_PRJ );
    sprintf( new_path, "./OUTPUT/BOX/%03d/%s/.%s%s%s%c", dump_number, PRG_PRJ, FILNAM, (strlen(EXT)?".":""), EXT, damdot );
    mkdir(   sub_dir, 0775 );
    unlink(  new_path );
    link( target_sn, new_path );
    if(!strcmp(FILNAM,"ALLDIR") && !strcmp(EXT,"DAT")){
      char box_dir[64];
      sprintf( box_dir,  "./OUTPUT/BOX/%03d", dump_number );
      touch_file_modtime( box_dir, iso_mdatetime );
    }

    //  PUMPKIN
    sprintf( sub_dir,  "./OUTPUT/PUMPKIN/%s",              PRG_PRJ );
    sprintf( sub_sub,  "./OUTPUT/PUMPKIN/%s/%s%s%s",       PRG_PRJ, FILNAM, strlen(EXT) ? "." : "", EXT );
    sprintf( new_plink,"./OUTPUT/PUMPKIN/%s/%s%s%s/.%d%c", PRG_PRJ, FILNAM, strlen(EXT) ? "." : "", EXT, dump_number, damdot );
    mkdir(   sub_dir, 0775 );
    mkdir(   sub_sub, 0775 );
    unlink(  new_plink );
    link( target_sn, new_plink );
    break;
    
  case ETEXT:
  case OTEXT:
    
    //  YEAR    text basename is simply FILNAM.EXT
    sprintf( target_sn,"./OUTPUT/text/sn/%06d", serial_number );
    sprintf( sub_dir,  "./OUTPUT/YEAR/%4.4s/%s",          iso_mdatetime, PRG_PRJ );
    sprintf( new_path, "./OUTPUT/YEAR/%4.4s/%s/%s%s%s%c", iso_mdatetime, PRG_PRJ, FILNAM, (strlen(EXT)?".":""), EXT, damdot );
    mkdir(   sub_dir, 0775 );
    unlink(  new_path );
    link( target_sn, new_path );

    //  BOX
    sprintf( target_sn,"./OUTPUT/text/sn/%06d", serial_number );
    sprintf( sub_dir,  "./OUTPUT/BOX/%03d/%s",          dump_number, PRG_PRJ );
    sprintf( new_path, "./OUTPUT/BOX/%03d/%s/%s%s%s%c", dump_number, PRG_PRJ, FILNAM, (strlen(EXT)?".":""), EXT, damdot );
    mkdir(   sub_dir, 0775 );
    unlink(  new_path );
    link( target_sn, new_path );

    //  PUMPKIN
    sprintf( sub_dir,  "./OUTPUT/PUMPKIN/%s",             PRG_PRJ );
    sprintf( sub_sub,  "./OUTPUT/PUMPKIN/%s/%s%s%s",      PRG_PRJ, FILNAM, strlen(EXT) ? "." : "", EXT );
    sprintf( new_plink,"./OUTPUT/PUMPKIN/%s/%s%s%s/%d%c", PRG_PRJ, FILNAM, strlen(EXT) ? "." : "", EXT, dump_number, damdot );
    mkdir(   sub_dir, 0775 );
    mkdir(   sub_sub, 0775 );
    unlink(  new_plink );
    link( target_sn, new_plink );
    break;
  }
}
#if 0
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
#endif
