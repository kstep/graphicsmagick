/*
  GraphicsMagick version and copyright.
*/
#ifndef _MAGICK_VERSION_H
#define _MAGICK_VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.
*/
#define MagickPackageName "GraphicsMagick"
#define MagickCopyright  "Copyright (C) 2002 " MagickPackageName " Group"
#define MagickLibVersion  0x0100
#define MagickLibVersionText  "1.0.0"
#define MagickLibVersionNumber  1,0,0,0
#define MagickReleaseDate  "04/01/03"


#if (QuantumDepth == 8)
#define MagickQuantumDepth  "Q8"
#elif (QuantumDepth == 16)
#define MagickQuantumDepth  "Q16"
#elif (QuantumDepth == 32)
#define MagickQuantumDepth  "Q32"
#else
# error Unsupported quantum depth.
#endif

#define MagickVersion MagickPackageName " " MagickLibVersionText " " \
  MagickReleaseDate " " MagickQuantumDepth " " MagickWebSite
#define MagickWebSite  "http://www." MagickPackageName ".org/"

/*
  Method declarations.
*/
extern MagickExport const char
  *GetMagickCopyright(void),
  *GetMagickVersion(unsigned long *),
  *GetMagickWebSite(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
