/*
  ImageMagick Image Resize Methods.
*/
#ifndef _MAGICK_RESIZE_H
#define _MAGICK_RESIZE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *MagnifyImage(const Image *,ExceptionInfo *),
  *MinifyImage(const Image *,ExceptionInfo *),
  *ResizeImage(const Image *,const unsigned long,const unsigned long,
     const FilterTypes,const double,ExceptionInfo *),
  *SampleImage(const Image *,const unsigned long,const unsigned long,
   ExceptionInfo *),
  *ScaleImage(const Image *,const unsigned long,const unsigned long,
     ExceptionInfo *),
  *ThumbnailImage(const Image *,const unsigned long,const unsigned long,
   ExceptionInfo *),
  *ZoomImage(const Image *,const unsigned long,const unsigned long,
     ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_RESIZE_H */
