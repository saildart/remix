// remix / load_feedback.c -*- mode:C;coding:utf-8; -*-
// --------------------------------------------------------------------------------------------- 100

// associative array using hsearch from the 'C' Library.
#define MAXSN 999999 // The early SAILDART collection was at 886465
uchar BIT[]={1,2,4,8,16,32,64,128}; // boolean bit-vector
uchar snseen[1+(MAXSN/8)]={}; // boolean array, used by de-dup to detect blob reappearance.
uchar sntext[1+(MAXSN/8)]={}; // boolean array, used to enable writing text files.
long snmax=1; // next sn ( there is no serial#0 )
long snmax_data=1; // next sn ( there is no serial#0 )
long snmax_xot=980000; //    16. xots and up to 3228. bots and 3228. eots.
long snmax_err=990000; // 8,318. error blobs
long snmax_gap=999900; //    61. gaps

long
check_sn(char *hash){
  ENTRY e,*f;                   // see the 'hsearch' man 3 page
  long value;
  e.key = hash;                 // NAME-key is the digest-hash
  f= hsearch(e,FIND);
  value = ( f ? (long)(f->data) : 0 );
  // if(value==0)fprintf(stderr,"check_sn HASH8/%s/ NOT found !\n",hash);
  return value;
}

long
fetch_sn(char *hash){
  ENTRY e,*f;                   // see the 'hsearch' man 3 page
  e.key = hash;                 // NAME-key is the digest-hash
  if(!(f= hsearch(e,FIND))){    // is this blob's hash key in the table ?
    e.key = strdup(hash);       // place copy of hash-key into malloc space
    switch(taxon){
    case ERRBLOB: e.data = (void *)snmax_err++; break;
    case GAP: e.data = (void *)snmax_gap++; break;
    case XOT: e.data = (void *)snmax_xot++; break;
    default: e.data = (void *)snmax_data++; break;   // coin new VALUE next serial number
    }
    f = hsearch(e,ENTER);
  }
  assert(f);
  return((long)(f->data));
}

void
load_old_snhash8(){
  // The snhash8 have become the "Accession-Numbers" for this "Digital-Archive".
  ENTRY e,*f;
  FILE *old;
  char *old_snhash8="./FEEDBACK/sn-hash8-accession-by-sn";
  long sn,cnt=0;
  // char altkey[24];
  errno = 0;
  old = fopen(old_snhash8,"r");
  if (!(old)){
        fprintf(stderr,"Open failed path old_snhash8='%s'\n", old_snhash8);
        exit(1);
  }
  while(EOF != fscanf( old, "%ld,%32ms\n", &sn, &e.key )){
    e.data= (void *)sn;
    //    fprintf(stderr,"%ld,/%s/\n",sn,e.key);
    f = hsearch(e,ENTER);
    assert((f=&e));
    cnt++;
    if(sn>snmax)snmax=sn;
    if(sn>snmax_data && sn<980000)snmax_data=sn;
  }
  snmax_data++; // next available sn
  fprintf(stderr,
          "Loaded %s \t       cnt=%ld blob serial numbers,"
          " next available sn=%ld\n",
          old_snhash8, cnt, snmax_data );
}

void
load_dumpset(){
  char *path="./FEEDBACK/tape-dumpset-reel.csv";
  FILE *i;
  int tape, dump, reel;
  i = fopen(path,"r");
  if (!i) handle_error(path);
  while(EOF != fscanf( i, "%d,%d,%d,%*[0-9]\n", &tape, &dump, &reel)){
    dumpset_of_tape[tape]=dump;
    reel_of_tape[tape]=reel;
    //fprintf(stderr,"tape#%4d dump#%4d reel#%4d\n",tape,dump,reel);
  }
  fprintf(stderr,"Loaded %s\n",path);
}

void
load_seekaddress(){
  // seek address table initialization
  char *path="./FEEDBACK/perm-darn-byte";
  FILE *table;
  int n,p,perm,darn;
  off_t byte;
  table = fopen(path,"r");
  if(!table) handle_error("perm-darn-byte");
  for (p=0;p<=3229;p++){
    fscanf(table,"%d %d %ld\n",&perm,&darn,&byte);
    // fprintf(stderr,"%4d %12d %12d %12ld\n",p,perm,darn,byte);
    assert(p==perm);
    dartrecntable[p] = darn;
    seekaddrtable[p] = byte;
  }
  fclose(table);
  fprintf(stderr,"Loaded %s\n",path);
}

/*
  This FEEDBACK table is needed to gracefully be on the correct tape number and dump set
        tape    fln
  where the out-of-order tape #1024 shall always appear on reel#3074 not where it belongs on reel#3059
  tape 1024 sits between tapes 1286 and 1287 in the flat DART data8 time capsule
  ---
               slot  fln
tape-fln-order-1020,698004
tape-fln-order-1021,698972
tape-fln-order-1022,699816
tape-fln-order:1023,700064
tape-fln-order-1025,700811
tape-fln-order-1026,702026
tape-fln-order-1027,703123
--
tape-fln-order-1283,860789
tape-fln-order-1284,861882
tape-fln-order-1285,862269
tape-fln-order:1286,862787
tape-fln-order:1024,863040
tape-fln-order:1287,863959
tape-fln-order-1288,864772
tape-fln-order-1289,865635
tape-fln-order-1290,866914
--
tape-dumpset-reel.csv-1020,294,3059,
tape-dumpset-reel.csv-1021,294,3059,
tape-dumpset-reel.csv-1022,295,3059,
tape-dumpset-reel.csv:1023,295,3059,
tape-dumpset-reel.csv:1024,295,3074,
tape-dumpset-reel.csv-1025,296,3059,
tape-dumpset-reel.csv-1026,296,3059,
tape-dumpset-reel.csv-1027,296,3059,
--
tape-dumpset-reel.csv-1283,353,3074,
tape-dumpset-reel.csv-1284,353,3074,
tape-dumpset-reel.csv-1285,353,3074,
tape-dumpset-reel.csv:1286,354,3074,
tape-dumpset-reel.csv:1287,355,3074,
tape-dumpset-reel.csv-1288,355,3074,
tape-dumpset-reel.csv-1289,355,3074,
tape-dumpset-reel.csv-1290,356,3074,
 */
void
load_tape_fln(){
  // tape fln table initialization
  char *path="./FEEDBACK/tape-fln-order";
  FILE *table;
  int n,slot,tape,fln;
  off_t byte;
  table = fopen(path,"r");
  if(!table) handle_error("tape-fln-order");
  for ( slot=1; slot<=2985; slot++ ){
    fscanf(table,"%d,%d\n", &tape, &fln );
    fln_sequential[ slot ] = fln;
    fln_of_tape[ tape ] = fln;
    tape_in_slot[slot ] = tape;
  }
  fclose(table);
  fprintf(stderr,"Loaded %s\n",path);
}
