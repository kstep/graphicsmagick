/*
  ImageMagick version and copyright.
*/
#ifndef _MAGICK_VERSION_H
#define _MAGICK_VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Method declarations.
*/
extern MagickExport const char
  *GetMagickCopyright(void),
  *GetMagickVersion(unsigned long *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
