/*
  ImageMagick version and copyright.
*/
#ifndef _VERSION_H
#define _VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(HasFPX)
#define MagickCopyright  "Copyright 1999 E. I. du Pont de Nemours and Company"
#else
#define MagickCopyright  "Copyright 1999 E. I. du Pont de Nemours and Company" \
  "\n           portions copyright Eastman Kodak Company, 1996"
#endif
#define MagickLibVersion  0x0421
#define MagickVersion  \
  "@(#)ImageMagick 4.2.2 99/05/01 cristy@mystic.es.dupont.com"

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
