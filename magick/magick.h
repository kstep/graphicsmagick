/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_MAGICK_H
#define _MAGICK_MAGICK_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _MagickInfo
{
  const char
    *name,
    *description,
    *version,
    *module;

  ImageInfo
    *image_info;

  Image
    *(*decoder)(const ImageInfo *,ExceptionInfo *);

  unsigned int
    (*encoder)(const ImageInfo *,Image *),
    (*magick)(const unsigned char *,const size_t);

  void
    *client_data;

  unsigned int
    adjoin,
    raw,
    stealth,
    blob_support,
    thread_support;

  unsigned long
    signature;

  struct _MagickInfo
    *previous,
    *next;
} MagickInfo;

/*
  Magick method declaractions.
*/
extern MagickExport char
  *MagickToMime(const char *);

extern const MagickExport MagickInfo
  *GetMagickInfo(const char *,ExceptionInfo *exception);

extern MagickExport MagickInfo
  *RegisterMagickInfo(MagickInfo *),
  *SetMagickInfo(const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
