/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_H
#define _MAGICK_H

#if defined(sun)
#define __EXTENSIONS__  1
#endif

#if defined(__hpux)
#define _HPUX_SOURCE  1
#endif

#if defined(vms)
#define _POSIX_C_SOURCE  1
#endif

/*
  System include declarations.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined(_VISUALC_)
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#if !defined(__OPENNT)
#include <time.h>
#else
#include <sys/time.h>
#endif
#if !defined(__MWERKS__)
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <SIOUX.h>
#include <console.h>
#include <unix.h>
#include <types.h>
#include <stat.h>
#endif

/*
  ImageMagick include declarations.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(__cplusplus) || defined(c_plusplus)
#define class  c_class
#endif

#if !defined(WIN32)
#define Export
#else
#define Export  __declspec(dllexport)
#pragma warning( disable : 4018 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4142 )
#endif

#undef index
#undef tainted

#define MaxTextExtent  1664
#if defined(macintosh)
#define HasJPEG
#define HasLZW
#define HasPNG
#define HasTIFF
#define HasTTF
#define HasZLIB
#endif

#if defined(WIN32)
#define HasJBIG
#define HasJPEG
#define HasLZW
#define HasPNG
#define HasTIFF
#define HasTTF
#define HasZLIB
#endif

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if defined(HAVE_CONFIG_H)
#include <magick/config.h>
#endif
#include <magick/classify.h>
#include <magick/image.h>
#include <magick/quantize.h>
#include <magick/gems.h>
#include <magick/compress.h>
#include <magick/utility.h>
#include <magick/monitor.h>
#include <magick/error.h>
#include <magick/X.h>
#include <magick/widget.h>
#include <magick/PreRvIcccm.h>
#include <magick/memory.h>
#include <magick/delegates.h>
#include <magick/version.h>
#else
#include "classify.h"
#include "image.h"
#include "quantize.h"
#include "gems.h"
#include "compress.h"
#include "utility.h"
#include "monitor.h"
#include "error.h"
#if defined(_VISUALC_)
#define INT32  X11_INT32
#endif
#include "X.h"
#include "widget.h"
#include "PreRvIcccm.h"
#include "memory.h"
#include "delegates.h"
#include "version.h"
#endif

/*
  Typedef declarations.
*/
typedef struct _MagickInfo
{
  char
    *magick;

  Image
    *(*decoder)(const ImageInfo *);

  unsigned int
    (*encoder)(const ImageInfo *,Image *),
    adjoin;

  const char
    *description;
} MagickInfo;

/*
  Magick utilities methods.
*/
extern Export const MagickInfo
  *GetMagickInfo(const char *);

extern Export void
  ListMagickInfo(FILE *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
