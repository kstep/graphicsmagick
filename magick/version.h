/*
  ImageMagick version and copyright.
*/
#ifndef _MAGICK_VERSION_H
#define _MAGICK_VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define MagickCopyright  "Copyright (C) 2001 ImageMagick Studio"
#define MagickLibVersion  0x0540
#define MagickLibVersionText  "5.4.0"
#define MagickLibVersionNumber  5.4.0,0
#if QuantumDepth == 8
#define MagickVersion \
  "@(#)ImageMagick 5.4.0 11/01/01 Q:8 http://www.imagemagick.org"
#elif QuantumDepth == 16
#define MagickVersion \
  "@(#)ImageMagick 5.4.0 11/01/01 Q:16 http://www.imagemagick.org"
#else
#define MagickVersion "@(#)ImageMagick 5.4.0 11/01/01 http://www.imagemagick.org"
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
