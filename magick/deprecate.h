/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Log methods.
*/
#ifndef _MAGICK_DEPRECATE_H
#define _MAGICK_DEPRECATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Legacy names for (possibly) large integral types
*/

#if !defined(ExtendedSignedIntegralType)
#  define ExtendedSignedIntegralType magick_int64_t
#endif
#if !defined(ExtendedUnsignedIntegralType)
#  define ExtendedUnsignedIntegralType magick_uint64_t
#endif

/*
  Compatibility definitions to handle the renaming of
  ExtendedSignedIntegralType and ExtendedUnsignedIntegralType to
  MagickSignedType and MagickUnsignedType which occured in ImageMagick
  5.5.8.  ImageMagick 5.5.8 also introduced MagickRationalType.
*/
#if !defined(MagickSignedType)
#  define MagickSignedType magick_int64_t
#endif
#if !defined(MagickUnsignedType)
#  define MagickUnsignedType magick_uint64_t
#endif
#if !defined(MagickRationalType)
#  if defined(HAVE_LONG_DOUBLE)
#    define MagickRationalType long double
#  else
#    define MagickRationalType double
#  endif
#endif

extern MagickExport char
  *PostscriptGeometry(const char *);

extern MagickExport magick_off_t
  SizeBlob(const Image *image);

extern MagickExport Image
  *GetImageList(const Image *,const long,ExceptionInfo *),
  *GetNextImage(const Image *),
  *GetPreviousImage(const Image *),
  *PopImageList(Image **),
  *ShiftImageList(Image **),
  *SpliceImageList(Image *,const long,const unsigned long,const Image *,
    ExceptionInfo *);

extern MagickExport long
  GetImageListIndex(const Image *);

extern MagickExport IndexPacket
  ValidateColormapIndex(Image *,const unsigned long);

extern MagickExport int
  ParseImageGeometry(const char *,long *,long *,unsigned long *,
    unsigned long *);

extern MagickExport unsigned int
  DeleteImageList(Image *,const long offset),
  GetNumberScenes(const Image *),
  PopImagePixels(const Image *,const QuantumType,unsigned char *),
  PushImagePixels(Image *,const QuantumType,const unsigned char *),
  PushImageList(Image **,const Image *,ExceptionInfo *),
  SetImageList(Image **,const Image *,const long,ExceptionInfo *),
  UnshiftImageList(Image **,const Image *,ExceptionInfo *);

extern MagickExport unsigned long
  GetImageListSize(const Image *);

extern MagickExport void
  DestroyImages(Image *),
  SetCacheThreshold(const unsigned long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
