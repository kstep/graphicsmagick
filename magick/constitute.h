/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Constitute Methods.
*/
#ifndef _MAGICK_CONSTITUTE_H
#define _MAGICK_CONSTITUTE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *ConstituteImage(const unsigned long,const unsigned long,const char *,
   const StorageType,const void *,ExceptionInfo *),
  *PingImage(const ImageInfo *,ExceptionInfo *),
  *ReadImage(const ImageInfo *,ExceptionInfo *),
  *ReadInlineImage(const ImageInfo *,const char *,ExceptionInfo *);

MagickExport unsigned int
  DispatchImage(const Image *,const long,const long,const unsigned long,
    const unsigned long,const char *,const StorageType,void *,ExceptionInfo *),
  PopImagePixels(const Image *,const QuantumType,unsigned char *);

extern MagickExport unsigned int
  PushImagePixels(Image *,const QuantumType,const unsigned char *),
  WriteImage(const ImageInfo *,Image *),
  WriteImages(ImageInfo *,Image *,const char *,ExceptionInfo *);

extern MagickExport void
  DestroyConstitute(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CONSTITUTE_H */
