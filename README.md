# /data/DART.remix/,
#       comma , README
# ----------------------------------------------------------------- 70
#              R       E        M       I       X
# ---------------------------------------------------------------------------------------------- 100
# Self assembly bash command 'source remix.c'
#       a simple 'makefile' exists,
#       so the bash command 'make' is shorter.

INPUT files GIVEN
        3adbff17fd7f9f6eb9107755594ae0b9  large/flat_DART_data8
        b9663bbd331646607452485b6e7d8f52  large/sn-hash8-accession-by-sn
    equivalent
  
PROCESSING the five D's
        de-frag      concatenate DART-7track-record data-payloads into SAIL-blobs
        de-dup       hash digest SAIL-blob-data8 to get serial numbered unique blob content
        de-damage    Mark files with Previous-Media-Error or defective headings
        de-flate     omit excessive record padding, redundancy, and in-band DART tables. (aka de-pad)
        de-tox       omit ephemera, legal hazards, dipolmatic faux pas [foh pahz] and Phở Soup.
        
OUTPUT files
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
# ---------------------------------
# The 'C' program for 2020

        remix.c
        
# broken up into 11 pieces ( in the hope of re-factoring towards improved understanding )

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

# ---------------------------------
# 'C' programs started before 2020.
        md5.c        md5.h
        dart-flatten-2016.c
        dart-remix.c
        old-undart-2017.c
        old-undart-2018.c
        remix-combo.c
        undart-2018.c
        mmap-dart.c
# ----------------------------
# 'SQL' programs
#       t1;./_main_batch.bash;t2
_main_batch.bash # executes the following five
        step10_blobs_and_tags.sql       # create REMIX tables : nametag and blobsnhash
        step20_MFD_tables.sql           # curated MFD  tables : mx  my  mz
        step30_misc_tables.sql          # DCPower  thesis  lesterspin
        step40_WhoWhen1.sql             # first level Who When tables :
        step50_WhoWhen2.sql             #  more       Who When curation :
# -----------------------------------------------
# process re-write 2020 and after, based on using mmap and python3

        mmap-dart.c
        simply-read-flat-dart.c

        hex2base64.py2  // binascii module
        hex2base64.py   // struct and base64 modules easier to deploy now (RFC_3548_urlsafe)

# --------------------------------------------------------------------------------
#       R       E       L       I       N       K
# symbolic-link convenient pathnames to subsets of the /data8/ binary files
#
t1;awk '{printf("ln -s /data8/sn/%s /data8/nickname/%s\n",$1,$2)}' \
        data/Disassemble./sn_fn|bash;t2

# -------------------
# initialize some SD chips ( say 128 GB )
# Assume for example that three SD usb chip appead at device locations /dev/sde /dev/sdf /dev/sdg
# Then
        sudo bash
        mkdir -p /e1 /f1 /g1            # make some mount points
for x in e f g;do echo;
        sgdisk -Z /dev/sd$x;
        sgdisk -N0 /dev/sd$x;
        mkfs.ext4 -F /dev/sd${x}1;
        mount /dev/sd${x}1 /${x}1;
done
        chown bgb:bgb /e1 /f1 /g1       # user access for copy commands
# --------------------------------------------------------------------------------
# For excessive purity of the SAILDART archive byte vector
#       Avoid PARTITION table
#       Avoid FILE SYSTEM 
# copy flat_DART_data8 to the SD WITHOUT file system or partition
#
sudo bash
time dd if=/d/large/flat_DART_data8 of=/dev/sdd oflag=direct status=progress bs=64M
90314177512 bytes (90 GB, 84 GiB) copied, 5963 s, 15.1 MB/s
1345+1 records in
1345+1 records out
90314177512 bytes (90 GB, 84 GiB) copied, 5966.4 s, 15.1 MB/s

real	99m26.408s
