/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_MAGICK_H
#define _MAGICK_MAGICK_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef Image
  *(*DecoderHandler)(const ImageInfo *,ExceptionInfo *);

typedef unsigned int
  (*EncoderHandler)(const ImageInfo *,Image *),
  (*MagickHandler)(const unsigned char *,const size_t);

typedef struct _MagickInfo
{
  char
    *name,              /* format ID ("magick") */
    *description,       /* format description */
    *note,              /* usage note for user */
    *version,           /* support library version */
    *module;            /* name of loadable module */

  ImageInfo
    *image_info;        /* huh??? */

  DecoderHandler
    decoder;            /* function vector to decoding routine */

  EncoderHandler
    encoder;            /* function vector to encoding routine */

  MagickHandler
    magick;             /* function vector to format test routine */

  void
    *client_data;       /* arbitrary user supplied data */

  unsigned int
    adjoin,             /* coder may read/write multiple frames (default True) */
    raw,                /* coder requires that size be set (default False) */
    stealth,            /* coder should not appear in formats listing (default False) */
    seekable_stream,    /* coder uses BLOB "seek" APIs (default False) */
    blob_support,	/* coder uses BLOB APIs (default True) */
    thread_support;     /* coder is thread safe (default True) */

  unsigned long
    signature;          /* private, structure validator */

  struct _MagickInfo
    *previous,          /* private, previous member in list */
    *next;              /* private, next member in list */
} MagickInfo;

/*
  Magick method declaractions.
*/
extern MagickExport char
  *MagickToMime(const char *);

extern MagickExport const char
  *GetImageMagick(const unsigned char *,const size_t);

extern MagickExport unsigned int
  IsMagickConflict(const char *),
  ListModuleMap(FILE *,ExceptionInfo *),
  ListMagickInfo(FILE *,ExceptionInfo *),
  UnregisterMagickInfo(const char *);

extern MagickExport void
  DestroyMagick(void),
  DestroyMagickInfo(void),
  InitializeMagick(const char *);

extern MagickExport const MagickInfo
  *GetMagickInfo(const char *,ExceptionInfo *exception);

extern MagickExport const MagickInfo
  **GetMagickInfoArray(ExceptionInfo *exception);

extern MagickExport MagickInfo
  *RegisterMagickInfo(MagickInfo *),
  *SetMagickInfo(const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
