/*
  ImageMagick version and copyright.
*/
#ifndef _VERSION_H
#define _VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(HasFPX)
#define MagickCopyright  "Copyright (C) 2000 ImageMagick Studio"
#else
#define MagickCopyright  "Copyright (C) 2000 ImageMagick Studio" \
  "\n           portions copyright Eastman Kodak Company, 1996"
#endif
#define MagickLibVersion  0x0510
#define MagickVersion  \
  "@(#)ImageMagick 5.1.0 00/01/01 cristy@mystic.es.dupont.com"

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
