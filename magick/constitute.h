/*
  Copyright (C) 2003, 2004 GraphicsMagick Group
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

typedef enum
{
  IndexQuantum,
  GrayQuantum,
  IndexAlphaQuantum,
  GrayAlphaQuantum,
  RedQuantum,
  CyanQuantum,
  GreenQuantum,
  YellowQuantum,
  BlueQuantum,
  MagentaQuantum,
  AlphaQuantum,
  BlackQuantum,
  RGBQuantum,
  RGBAQuantum,
  CMYKQuantum,
  CMYKAQuantum
} QuantumType;

typedef enum
{
  CharPixel,
  ShortPixel,
  IntegerPixel,
  LongPixel,
  FloatPixel,
  DoublePixel
} StorageType;

extern MagickExport Image
  *ConstituteImage(const unsigned long width,const unsigned long height,
     const char *map,const StorageType type,const void *pixels,
     ExceptionInfo *exception),
  *PingImage(const ImageInfo *image_info,ExceptionInfo *exception),
  *ReadImage(const ImageInfo *image_info,ExceptionInfo *exception),
  *ReadInlineImage(const ImageInfo *image_info,const char *content,
     ExceptionInfo *exception);

MagickExport MagickPassFail
  DispatchImage(const Image *image,const long x_offset,const long y_offset,
    const unsigned long columns,const unsigned long rows,const char *map,
    const StorageType type,void *pixels,ExceptionInfo *exception),
  ExportImagePixelArea(const Image *image,const QuantumType quantum_type,
    const unsigned int quantum_size,unsigned char *destination);

extern MagickExport MagickPassFail
  ImportImagePixelArea(Image *image,const QuantumType quantum_type,
    const unsigned int quantum_size,const unsigned char *source),
  WriteImage(const ImageInfo *image_info,Image *image),
  WriteImages(ImageInfo *image_info,Image *image,const char *filename,
    ExceptionInfo *exception);

extern MagickExport void
  DestroyConstitute(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CONSTITUTE_H */
