/*
  ImageMagick Image Transform Methods.
*/
#ifndef _MAGICK_TRANSFORM_H
#define _MAGICK_TRANSFORM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *ChopImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *CoalesceImages(const Image *,ExceptionInfo *),
  *CropImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *DeconstructImages(const Image *,ExceptionInfo *),
  *FlattenImages(const Image *,ExceptionInfo *),
  *FlipImage(const Image *,ExceptionInfo *),
  *FlopImage(const Image *,ExceptionInfo *),
  *MosaicImages(const Image *,ExceptionInfo *),
  *RollImage(const Image *,const long,const long,ExceptionInfo *),
  *ShaveImage(const Image *,const RectangleInfo *,ExceptionInfo *);

extern MagickExport unsigned int
  ProfileImage(Image *,const char *,const unsigned char *,const size_t,
    unsigned int);

extern MagickExport void
  TransformImage(Image **,const char *,const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_TRANSFORM_H */
