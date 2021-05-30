// remix/main/usage.c -*- mode:C;coding:utf-8; -*-
void
usage(){
  fprintf(stderr,"\n\
NAME \n\
        remix - read SAILDART time-capsule                  \n\
              - write time-capsule data into several formats \n\
SYNOPSIS \n\
        remix extracts stuff \n\
        from the SAILDART archive Time-Capsule file named 'flat_DART_data8',   \n\
        which should have the md5 hash value 3adbff17fd7f9f6eb9107755594ae0b9.  \n\
        remix writes a Smörgåsbord of 21st century formats for historical study. \n\
DESCRIPTION \n\
        -v --verbose enable progress display\n\
        -q --quiet   \n\
        -f --fancy   \n\
                add annotation to octal dumps \n\
                add more columns to csv files \n\
                add more csv files \n\
  input span \n\
        -p   --perm=#      Tape    #1 to #2984. \n\
                        or Reel #3000 to #3227. \n\
        -n   --number=# of Reels 1:228. or Tapes 1:2984.\n\
  output families \n\
        -o --octal tree of files on path ./OUTPUT/octal/reeltape/P#reel/P#tape/RECORDNAMES \n\
                there may be many DART records to form one SAIL file \n\
        -8 --data8 binary files on path ./OUTPUT/data8/sn/###### \n\
                data8 is 8 bytes per PDP-10 word \n\
                blob content of SAIL RalF files   \n\
                by SAIL Archive accession serial number, sn. \n\
        -t --text files on path ./OUTPUT/text/sn/###### \n\
        -d --database the csv files are /OUTPUT/csv/{attributes,coordinates,dates,ribs,tags,tapedex} \n\
        -f --fancy csv files are      \n\
                    who_when.csv        \n\
                    tags_ontape_toc.csv     from   DART.DAT and   DART.ARC \n\
                    tags_ondisk_toc.csv     from ALLDIR.DAT and ALLDIR.OLD \n\
                    session.csv            \n\
  global \n\
        -h --histogram verification of dart record type counts and length of input span \n\
                outputs TOC feedback file named csv/byte-record-dartname.csv \n\
                outputs Virgin blob hash sn accession numbers. NOT exact match for the Canonical sn numbers. \n\
                enables feedback mechanism for random access to input spans using lseek \n\
        -a --all regular ( write data8 and text files by sn and relink as SAIL-WAITS unixlike file hierarchies ) \n\
        -e --everything  ( write ### GBytes, output all formats, create index tree structures in file system ) \n\
INPUT path \n\
        ./INPUT/flat_DART_data8                 sine-qua-non the MUST-HAVE input file \n\
        ./INPUT/sn-hash8-accession-by-sn        optional (Baumgart 2020 accession numbering) \n\
OUTPUT paths \n\
        ./OUTPUT/csv/        # 10 GB    database friendly \n\
        ./OUTPUT/data8/      # 54 GB    bitwise-exact, for PDP10 simulator / reënactment \n\
        ./OUTPUT/log/        #          remix development / maintenance \n\
        ./OUTPUT/octal/      #          bitwise-exact, low level, human legible \n\
        ./OUTPUT/pumpkin/    #          tree of symlinks by ppn / filename.extension / bindot dumpset damdot\n\
                                        pre and post fixed dots mark binary and damage file paths\n\
        ./OUTPUT/static/     #          html version of SAIL WAITS files \n\
        ./OUTPUT/text/       # 19 GB    UTF8 version of SAIL WAITS text files\n\
OUTPUT sub-directory examples \n\
        octal/reeltape/P#reel/P#tape/record-names \n\
        octal/sn/###### \n\
        data8/sn/###### \n\
        text/sn/######  \n\
        text/unixlike/prg/prj/filnam.ext.~#~        # with version numbering \n\
        static/html/PRG/PRJ/* \n\
        static/{iii,ogg,pdf,plt,png,svg} \n\
FEEDBACK files \n\
        ./FEEDBACK/perm-darn-byte                   # tape/reel seek addresses \n\
        ./FEEDBACK/csv/seekaddr-dartrecd.csv \n\
        ./FEEDBACK/csv/sn-hash8.csv                 # sn accession numbering \n\
EXAMPLES \n\
        remix           # usage message \n\
        remix -n0       # do nothing after initialization and path sniffing \n\
        remix -p1       # one tape \n\
        remix -p3000    # one reel \n\
\n\
t1;time remix -a -d -p3000 -n229;t2\n\
\n\
");
  exit(0);
}

