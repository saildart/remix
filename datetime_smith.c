// remix/datetime_smith.c  -*- mode:C;coding:utf-8; -*-
//
//      Date Time stamps ( iso_mdatetime will become the most relevant )
//      created, modified, accessed, dumped
//
// These are NOT unix dates.
// These are SAIL WAITS file system date-time stamps.
int cdate, mdate, adate, ddate;
int        mtime, atime;
//
// Remix Prescience
//
// 1972-11-05 11:59     Tape #0001 BOT is at Noon 5 November 1972, nearly 18 years later
// 1990-08-17 17:08     Tape #3228 the final file ALLDIR.DAT[DMP,SYS] is written.
//                      so the DART records span ten million minutes, since
//                      timestampdiff( minute, '1972-11-05 11:59',' 1990-08-17 17:08' )
//                      is 9,351,669.
//
// Remix (actualy curation processing after the remix) may wish to enforce FILE date
// consistency using the tape and reel BOT date time stamps
// for FILE mdates which are impossibly out of range.
//
// The BOT date for the track7 tapes, should be the maximum (latest) possible date for the files on that tape.
// However, the important DART log file named ALLDIR.DAT is NOT closed and written to its tape until the final moment
// which is on average 15 minutes after BOT time, but can be as long as 3 hours after BOT time,
// due to the heavy time sharing load, system crash interrupts, operator Chinese dinner break, or whatever.
//
char iso_mcopy_bot_datetime[32]; // BOT9 date from MCOPY track9 BOT marker
char iso_bdatetime[32]; // BOT7 date on prior BOT7 marker of track7 tape

char iso_cdatetime[32]; // file system RIB alleged "creation" date
char iso_mdatetime[32]; // file "modification" / "write" date from dart recorder HEADER.
char     mdate6[8];     // six digit date example 721105 - confined to the SAILDART epoch 1972 to 1990, used as file discriminator.
char iso_adatetime[32]; // "access" date from RIB
char iso_ddatetime[32]; // "dump" date   from RIB

// set ISO date string, given SAIL-WAITS system_date and system_time
//      ISO date string      return     "1999-12-31T11:59:59"  or  "_000000_000000_"
//      SAIL-WAITS      system_date     (year - 1964)*372 + (month-1)*31 + (day-1)
//      SAIL-WAITS      system_time     Minutes since midnight 0. to 1439. inclusize
// offset the datetime stamp by plus_seconds ( correctly handling the roll over for day, month and year;
// including Julian calendar rules, Nixon oil-crisis daylight savings dates,
// but ignoring the 17 leap seconds that occurred during this Epoch ).
void
iso_date( char *dest, int sysdate, int systime, int plus_seconds ){
  int day,month,year,hour,minute,second;
  int plus_day=0;
  int plus_minutes=0;
  int dpm[]={0,31,28,31, 30,31,30, 31,31,30, 31,30,31}; // days per month
  // 123456789.123456
  // 0000-00-00 00:00
  // 000000----000000
  // sprintf(dest,"%-20s","null");
  // sprintf(dest,"_%06d_%06d_",sysdate,systime);
  dest[0]=0; // empty string
  /*
    boundary checking for possible tape media errors

    if(sysdate>0 && ((sysdate < (1968-1964)*372) ||  (sysdate > (1991-1964)*372) || (systime > 1440 ))){
    fprintf(stderr,"%7d / sysdate=%d systime=%d ",record,sysdate,systime);
    fprintf(stderr,"iso_date defect year %d month %d day %d hour %d minute %d\n",year,month,day,hour,minute);
    }  
  */
  if(sysdate < (1968-1964)*372) return; // below  floor  
  if(sysdate > (1991-1964)*372) return; // above ceiling
  if(systime > 1440 ) return;           // over the wall
  /*
    The plus_second offset mechanism
    assigns estimated date_time stamps
    for when DART records were
    first written to 7 track tapes AND then
    second copied to 9 track reels
    based on the BOT records for tapes and reels.
  */
  second =              plus_seconds % 60;
  plus_minutes =        plus_seconds / 60;
  systime +=            plus_minutes;
  if( systime >= 1440){
    // reel#3031 has 18,689 file-start records
    //     there are 86,400 seconds per day
    plus_day = systime / 1440;
    assert( plus_day == 1 );
    systime = systime % 1440;
  }  
  minute= systime % 60;
  hour  = systime / 60;
  if( hour >= 24 ){
    fprintf(stderr,"Sheesh! BAD hour=%d minute=%d sysdate=%d systime=%d plusec=%d\n",
            hour, minute, sysdate, systime, plus_seconds );
    return;
  }
  // Note 31 times 12 is 372.
  //    sysdate = (year - 1964)*372 + (month-1)*31 + (day-1)
  //    systime = hour*60 + minute
  day   =  sysdate % 31 + 1;
  month = (sysdate / 31) % 12 + 1;  
  year  = (sysdate / 31) / 12 + 1964;
  day += plus_day;
  // Day of month too big ?
  if( day > ((month!=2) ? dpm[month] : (year%4 == 0) ? 29 : 28)){
    day = 1;
    month++;
    if(month>12){
      month=1;
      year++;
    }
  }
  
  // Check for the 23-hour Day Light Savings Spring Forward dates
  // increment one hour for times between 02:00 AM and 02:59 AM inclusive
  switch(sysdate){
  case   920: //  24-Apr-1966
  case  1310: //  30-Apr-1967
  case  1692: //  28-Apr-1968
  case  2075: //  27-Apr-1969
  case  2458: //  26-Apr-1970
  case  2841: //  25-Apr-1971
  case  3230: //  30-Apr-1972
  case  3613: //  29-Apr-1973
  case  3878: //   6-Jan-1974 Nixon oil crisis Daylight-Savings-Time
  case  4311: //  23-Feb-1975 ditto (but Ford was president in 1975)
  case  4761: //  25-Apr-1976
  case  5144: //  24-Apr-1977
  case  5534: //  30-Apr-1978
  case  5917: //  29-Apr-1979
  case  6299: //  27-Apr-1980
  case  6682: //  26-Apr-1981
  case  7065: //  25-Apr-1982
  case  7448: //  24-Apr-1983
  case  7837: //  29-Apr-1984
  case  8220: //  28-Apr-1985
  case  8603: //  27-Apr-1986
  case  8965: //   5-Apr-1987
  case  9347: //   3-Apr-1988
  case  9730: //   2-Apr-1989
  case 10113: //   1-Apr-1990
    if(hour==2){ // 2 AM clocks SPRING forward ... for GNU/linux that hour is INVALID.
      hour++; // work around for the impossible datetime stamp values
    }
    break;
  default: break;
  }
  // No adjustment for leap seconds.
  // ================================================================================
  // From 1972 to 1991 there were seventeen positive LEAP SECONDS
  // which occured at GMT 00:00:00 on 1 January 1972 to 1980, 1988, 1990, 1991.
  //           and at GMT 00:00:00 on 1 July 1972, 1982, 1983 and 1985.
  // There were no LEAP SECOND in 1984, 1986, 1987 and 1989.
  //    On 1 January 1972 the difference (TAI-UTC) was 10 seconds, and
  //    on 1 January 1991 the difference (TAI-UTC) was 26 seconds.
  // Although NTP Network-Time-Protocol overlaps the SAILDART epoch,
  // the computer date time was normally set from somebody's wristwatch
  // after a building power failure. The PDP10 clock interrupt was driven
  // at 60 hertz by powerline cycles, the Petit electronic clock likely
  // had a crystal oscillator but was set by software based on a wristwatch reading.
  // Wristwatches in the 1970s were set by AM radio time bleep at the top of the hour,
  // or by dialing POPCORN on the telephone. In the 20th century there was an analog
  // circuit switching telephone network with an audio time of day recorded message
  // at local number POP-CORN. See https://www.wikiwand.com/en/Speaking_clock
  // =================================================================================
  // No adjustment for leap seconds.
  // sprintf( dest,"%4d-%02d-%02dT%02d:%02d:%02d",year,month,day,hour,minute,second );
  if(systime<0){
    sprintf( dest,"%4d-%02d-%02d",year,month,day );
  }else{
    sprintf( dest,"%4d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second );
  }
  // dirty TLD hack - place a Three-Letter-Date code at the end of the destination 32 byte string.
  if (1965<=year && year<=1990)
    sprintf(dest+28,"%c%c%c",
          (year%100),  // 1965 'A' to 1990 'Z' for years within the SAILDART epoch
          (month+64), // January 'A' to December 'L'
          "@ABCDEFGHIJKLMNOPQRSTUVWXYZuvxyz"[day] ); // day-of-month 1 to 31
          
  // 0        10        20        30
  // .123456789.123456789.123456789.1
  // 1999-12-31 11:59:59         ymd
}
