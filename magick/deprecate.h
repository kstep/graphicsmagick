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

extern MagickExport char
  *PostscriptGeometry(const char *);

extern MagickExport ExtendedSignedIntegralType
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
