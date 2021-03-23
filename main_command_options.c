// remix/main/command-line-options.c -*- mode:C;coding:utf-8; -*-

void
command_line_options(int ac,char *av[]){
  int c, digit_optind=0;  
  G.perm = G.rcrd = G.count = 1;
  G.unit = "tape";
  while(1){
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"perm",   required_argument, &G.perm,    1 }, // input span
      {"number", required_argument, &G.count,   1 }, 
      {"verbose",   no_argument, &G.verbose,    1 }, // global
      {"quiet",     no_argument, &G.quiet,      1 },
      {"fancy",     no_argument, &G.fancy_flag, 1 },
      {"histogram", no_argument, &G.histo_flag, 1 },
      {"octal",     no_argument, &G.octal_flag, 1 }, // output modes
      {"data8",     no_argument, &G.data8_flag, 1 },
      {"text",      no_argument, &G.text_flag,  1 },
      {"database",  no_argument, &G.csv_flag,   1 },
      { 0, 0, 0, 0 }
    };
    c = getopt_long( ac, av, "abdefghN:n:oP:p:rtqv8", long_options, &option_index );
    if (c == -1) break; // done. No more options.
    switch (c) {
    case 0:
      printf("option %s option_index=%d",
             long_options[option_index].name,
             option_index
             );
      if (optarg)
        printf(" with arg %s", optarg);
      printf("\n");
      break;
    case '8':      printf("option 8\n"); G.data8_flag=1; break;
    case 'a':      printf("option a\n");
      G.data8_flag = 1;
      G.text_flag  = 1;
      G.octal_flag = 1;
      G.fancy_flag = 1;
      G.histo_flag = 1;
      G.csv_flag   = 1;
      break;
    case 't':      printf("option t\n"); G.text_flag=1;  break;
    case 'o':      printf("option o\n");                        G.octal_flag=1; break;
    case 'f':      printf("option f fancy details\n");          G.fancy_flag=1; break;
    case 'h':      printf("option h histogram verification\n"); G.histo_flag=1; break;
    case 'd':      printf("option d database \n");              G.csv_flag=1;   break;
    case 'p':
    case 'P':
      G.perm = atoi(optarg);
      G.unit = G.perm<3000 ? "tape" : "reel";
      printf("option p perm first %s set to P%04d\n",  G.unit, atoi(optarg));
      break;
    case 'n':
    case 'N':
      G.count = atoi(optarg); 
      G.unit = G.perm<3000 ? "tape" : "reel";
      printf("option n count set to %d %s%s\n", G.count, G.unit, (G.count==1 ? "" : "s"));
      break;
    case 'q':      printf("option q quiet\n");   G.quiet=1;   break;
    case 'v':      printf("option v verbose\n"); G.verbose=1; break;
    case '?':      break;
    default: printf("?? getopt returned character code 0%o ??\n", c);
    }
  }
  printf("\n");
}

// Classic omission in 'C' Library and Language. Usually implemented as a CPP definition.
int min(int x, int y){return (x<y) ? x : y;} 

void
span_validation(){
  // span specification; validate, fix up or reject.
  if((G.perm!=0) &&
     (   1<=G.perm && G.perm<=2984) ||
     (3000<=G.perm && G.perm<=3228)){
    G.unit = G.perm<3000 ? "tape" : "reel";
    G.first =  G.perm;
    while( dartrecntable[G.first]==0 ){
      fprintf(stderr,"skip LOST tape # P%04d\n",G.first);
      G.first++; // skip LOST tapes
    }
    G.final = (G.first<3000) ? min(2984,G.first+G.count-1) : min(3228,G.first+G.count-1);
    while( dartrecntable[G.final]==0 ){
      fprintf(stderr,"skip LOST tape # P%04d\n",G.final);
      G.final++; // skip LOST tapes
    }
    G.final_plus_1 = G.final+1;
    while( dartrecntable[G.final_plus_1]==0 ){
      fprintf(stderr,"skip LOST tape # P%04d\n",G.final_plus_1);
      G.final_plus_1++; // skip LOST tapes
    }
    fprintf(stderr,"First   %s # P%04d DART record %7d \n",
            G.unit,
            G.first,
            dartrecntable[G.first] );
    fprintf(stderr,"Final   %s # P%04d DART record %7d\n",
            G.unit,
            G.final,
            dartrecntable[G.final_plus_1] ); // slight cheat
  }else{
    fprintf(stderr,"\n\nSorry. First B.Tape# Reel# perm = P%d is out-of-range. \n\
\n\
Specify input span \n\
        -p   --perm=#      B.Tape    #1 to #2984. \n\
                        or Reel #3000 to #3227. \n\
        -n   --number=# of Reels 1:228. or B.Tapes 1:2984.\n\
\n",G.perm);
    exit(1); // lose
  }
}
