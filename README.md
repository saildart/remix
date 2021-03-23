# saildart/remix/README.md
#       R       E        M       I       X
To compile, just execute the source file __remix.c__
to make a binary at path **/usr/local/bin/remix**
or type the bash command 'source remix.c'.
A simple 'makefile' exists, so just type 'make'.

# INPUT files
Place a symbolic link named **./INPUT** pointing at the directory
containing the two large time capsule files named **flat_DART_data8**
and **sn-hash8-accession-by-sn**
md5 hash                         | path
-------------------------------- | ------------------------------
3adbff17fd7f9f6eb9107755594ae0b9 | large/flat_DART_data8
b9663bbd331646607452485b6e7d8f52 | large/sn-hash8-accession-by-sn
  
# PROCESS the five D's
D          | description  
---------- | --------------------------------------------------------------------------------
de-frag    | concatenate DART-7track-record data-payloads into SAIL-blobs
de-dup     | hash digest SAIL-blob-data8 to get serial numbered unique blob content
de-damage  | Mark files with Previous-Media-Error or defective headings
de-flate   | omit excessive record padding, redundancy, and in-band DART tables. (aka de-pad)
de-tox     | omit ephemera, legal hazards, dipolmatic faux pas [foh pahz] and Phở Soup.
        
# OUTPUT files
Plase a symbolic link named **./OUTPUT** pointing at 100GB of empty space.
For development I build loop mounted containers so the delete is rapid.

        reset-OUTPUT-container.bash

        data base tables into csv formated files /OUTPUT/csv/
                address
                attr
                dates
                ribs
                tags
                tapedex
                
        content blobs into file system as
                DATA8 binary raw
                UTF-8 text   raw
                UTF-8 text cooked
                
# The unified 'C' program for after 2021 will be one large file named
        remix.c
        
# It is now broken up into 12 pieces
## in the hope of re-factoring towards improved understanding
        remix.c      
        main_bigtop.c
        structures.c  
        filename_smith.c
        datetime_smith.c
  
        dart_decoding.c
        load_feedback.c
        tree_farming.c
  
        md5.c
        blob_taxidermy.c  
        main_command_options.c
        main_usage.c
        
# auxillary Read Only
        md5.c        md5.h
        utf8.c       utf8.h
        
# 'SQL' programs
The SQL scripts have been moved out of remix.
The saildart data base processing will require several new repositories.

#       R       E       L       I       N       K
## symbolic-link convenient pathnames to subsets of the /data8/ binary files

```bash
t1;awk '{printf("ln -s /data8/sn/%s /data8/nickname/%s\n",$1,$2)}' \
        data/Disassemble./sn_fn|bash;t2
```

# Initialize some SD chips ( say 128 GB )
## for offsite, cold-copy, time-capsule distribution

For example, I plugged in three USD SD chips which then appeared
at device locations /dev/sde /dev/sdf /dev/sdg
so

```bash
        sudo bash
        mkdir -p /e1 /f1 /g1            \# make some mount points
for x in e f g;do echo;
        sgdisk -Z /dev/sd$x;
        sgdisk -N0 /dev/sd$x;
        mkfs.ext4 -F /dev/sd${x}1;
        mount /dev/sd${x}1 /${x}1;
done
        chown bgb:bgb /e1 /f1 /g1       \# user access for copy commands
```

# For excessive purity of the SAILDART archive byte vector
       Avoid PARTITION table
       Avoid FILE SYSTEM       
       Copy **flat_DART_data8** to SD without file system or partition table
```bash
sudo bash
time dd if=/d/large/flat_DART_data8 of=/dev/sdd oflag=direct status=progress bs=64M
```
90314177512 bytes (90 GB, 84 GiB) copied, 5963 s, 15.1 MB/s
1345+1 records in
1345+1 records out
90314177512 bytes (90 GB, 84 GiB) copied, 5966.4 s, 15.1 MB/s
\#real	99m26.408s
