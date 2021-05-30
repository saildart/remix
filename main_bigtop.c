// remix/main_bigtop.c  -*- mode:C;coding:utf-8; -*->

typedef unsigned long long uint64;
typedef          long long  int64;
typedef unsigned long long    u64;
typedef          long long    i64;
typedef unsigned char uchar;

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <locale.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <wchar.h>

#include "utf8.c"
