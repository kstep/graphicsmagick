/*
  ImageMagick version and copyright.
*/
#ifndef _MAGICK_VERSION_H
#define _MAGICK_VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define MagickCopyright  "Copyright (C) 2002 ImageMagick Studio LLC"
#define MagickLibVersion  0x0549
#define MagickLibVersionText  "5.4.9"
#define MagickLibVersionNumber  5,4,9,0
#if (QuantumDepth == 8)
#define MagickQuantumDepth  "Q8"
#elif (QuantumDepth == 16)
#define MagickQuantumDepth  "Q16"
#elif (QuantumDepth == 32)
#define MagickQuantumDepth  "Q32"
#else
#define MagickQuantumDepth  "Q?"
#endif
#define MagickReleaseDate  "09/01/02"
#define MagickVersion "ImageMagick " MagickLibVersionText " " \
  MagickReleaseDate " " MagickQuantumDepth " " MagickWebSite
#define MagickWebSite  "http://www.imagemagick.org"

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
