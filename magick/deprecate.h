/*
  Log methods.
*/
#ifndef _MAGICK_DEPRECATE_H
#define _MAGICK_DEPRECATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport char
  *PostscriptGeometry(const char *);

extern MagickExport int
  ParseImageGeometry(const char *,long *,long *,unsigned long *,
    unsigned long *);

extern MagickExport void
  SetCacheThreshold(const unsigned long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
