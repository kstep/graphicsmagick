/*
  ImageMagick Decorate Methods.
*/
#ifndef _MAGICK_DECORATE_H
#define _MAGICK_DECORATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *BorderImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *FrameImage(const Image *,const FrameInfo *,ExceptionInfo *);

MagickExport unsigned int
  RaiseImage(Image *,const RectangleInfo *,const int);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_DECORATE_H */
