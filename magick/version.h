/*
  ImageMagick version and copyright.
*/
#ifndef _MAGICK_VERSION_H
#define _MAGICK_VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define MagickCopyright  "Copyright (C) 2001 ImageMagick Studio"
#define MagickLibVersion  0x0539
#define MagickLibVersionText  "5.3.9"
#define MagickLibVersionNumber  5,3,9,0
#if defined(QuantumLeap)
#define MagickVersion \
  "@(#)ImageMagick 5.3.9 10/01/01 Q:16 http://www.imagemagick.org"
#else
#define MagickVersion \
  "@(#)ImageMagick 5.3.9 10/01/01 Q:8 http://www.imagemagick.org"
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
