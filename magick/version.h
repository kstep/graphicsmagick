/*
  ImageMagick version and copyright.
*/
#ifndef _VERSION_H
#define _VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define MagickCopyright  "Copyright (C) 2001 ImageMagick Studio"
#define MagickLibVersion  0x0534
#if defined(QuantumLeap)
#define MagickVersion \
  "@(#)ImageMagick 5.3.4 06/01/01 Q:16 http://www.imagemagick.org"
#else
#define MagickVersion \
  "@(#)ImageMagick 5.3.4 06/01/01 Q:8 http://www.imagemagick.org"
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
