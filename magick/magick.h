/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_H
#define _MAGICK_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  System include declarations.
*/
#define _XOPEN_SOURCE  500
#define _POSIX_C_SOURCE  199506L

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if !defined(_VISUALC_)
#  include <unistd.h>
#else
#  include <direct.h>
#  define HAVE_STRERROR
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define fseek  fseeko
#define ftell  ftello
#endif

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#if !defined(vms) && !defined(macintosh)
#  include <sys/types.h>
#  include <sys/stat.h>
#if !defined(WIN32)
#  include <sys/time.h>
#  include <sys/times.h>
#endif
#  include "magick/api.h"
#else
#  include <types.h>
#  include <stat.h>
#if defined(macintosh)
#  include <SIOUX.h>
#  include <console.h>
#  include <unix.h>
#endif
#  include "api.h"
#endif

#if defined(HAVE_MMAP)
#  include <sys/mman.h>
#endif
#if defined(HasPTHREADS)
#  include <pthread.h>
#endif
#if defined(HAVE_POLL)
#  include <sys/poll.h>
#endif

#undef index
#undef pipe

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
