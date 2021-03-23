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
  mkdir( "./OUTPUT/pumpkin",  0775 ); // lowercase  OK        pumpkin / prg.prj / filnam.ext / version#
  mkdir( "./OUTPUT/YEAR",     0775 ); // UPPERCASE            YEAR    / {1972..1990} /    PRG.PRJ / FILNAM.EXT
  //
  mkdir( "./OUTPUT/octal/reeltape", 0775 ); //                  octal / reeltape / p3000 / p0001 /  drn.type._PRJ_PRG_FILNAM_EXT._10240_words
  mkdir( "./OUTPUT/text/unixlike",  0775 ); // lowercase        unixlike / prg / prj / filnam.ext.~version#~
  {
    int year;
    char path[64];
    for ( year=1970; year<=1990; year++){
      sprintf(path,"./OUTPUT/YEAR/%d",year);
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
symlink_trailblazer(){
  // Place symlinks into directory hierarcy for study and presentation
  // so authentic FILE content (data8/sn) can be found.
  //    by year with
  //       somewhat authentic UPPERCASE tags            YEAR / year# / PRG.PRJ / FILNAM.EXT
  //    by unixlike lowercase pathnames w/version#      unixlike / prg / prj / filnam.ext.~#~
  //    provide all instances retrieved from tapes      pumpkin / prg.prj / filnam.ext / {'.' when binary} dump# {'.' when damaged}
  //    by HTML server (Apache) URL remapping
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
    sprintf( target_sn,"../../../data8/sn/%06d", serial_number );
    sprintf( sub_dir,  "./OUTPUT/YEAR/%4.4s/%s",         iso_mdatetime, PRG_PRJ );
    sprintf( new_path, "./OUTPUT/YEAR/%4.4s/%s/.%s%s%s%c", iso_mdatetime, PRG_PRJ, FILNAM, (strlen(EXT)?".":""), EXT, damdot );
    mkdir(   sub_dir, 0775 );
    unlink(  new_path );
    symlink( target_sn, new_path );

    //  pumpkin
    sprintf( sub_dir,  "./OUTPUT/pumpkin/%s",            prg_prj );
    sprintf( sub_sub,  "./OUTPUT/pumpkin/%s/%s%s%s",     prg_prj, filnam, strlen(ext) ? "." : "", ext );
    sprintf( new_plink,"./OUTPUT/pumpkin/%s/%s%s%s/.%d%c", prg_prj, filnam, strlen(ext) ? "." : "", ext, dump_number, damdot );
    mkdir(   sub_dir, 0775 );
    mkdir(   sub_sub, 0775 );
    unlink(  new_plink );
    symlink( target_sn, new_plink );
    break;
    
  case ETEXT:
  case OTEXT:
    
    //  YEAR    text basename is simply FILNAM.EXT
    sprintf( target_sn,"../../../text/sn/%06d", serial_number );
    sprintf( sub_dir,  "./OUTPUT/YEAR/%4.4s/%s",        iso_mdatetime, PRG_PRJ );
    sprintf( new_path, "./OUTPUT/YEAR/%4.4s/%s/%s%s%s%c", iso_mdatetime, PRG_PRJ, FILNAM, (strlen(EXT)?".":""), EXT, damdot );
    mkdir(   sub_dir, 0775 );
    unlink(  new_path );
    symlink( target_sn, new_path );

    //  pumpkin
    sprintf( sub_dir,  "./OUTPUT/pumpkin/%s",           prg_prj );
    sprintf( sub_sub,  "./OUTPUT/pumpkin/%s/%s%s%s",    prg_prj, filnam, strlen(ext) ? "." : "", ext );
    sprintf( new_plink,"./OUTPUT/pumpkin/%s/%s%s%s/%d%c", prg_prj, filnam, strlen(ext) ? "." : "", ext, dump_number, damdot );
    mkdir(   sub_dir, 0775 );
    mkdir(   sub_sub, 0775 );
    unlink(  new_plink );
    symlink( target_sn, new_plink );
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
