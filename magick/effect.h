/*
  ImageMagick Image Effect Methods.
*/
#ifndef _MAGICK_EFFECT_H
#define _MAGICK_EFFECT_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *AdaptiveThresholdImage(const Image *,const unsigned long,const unsigned long,
     const double,ExceptionInfo *),
  *AddNoiseImage(const Image *,const NoiseType,ExceptionInfo *),
  *BlurImage(const Image *,const double,const double,ExceptionInfo *),
  *DespeckleImage(const Image *,ExceptionInfo *),
  *EdgeImage(const Image *,const double,ExceptionInfo *),
  *EmbossImage(const Image *,const double,const double,ExceptionInfo *),
  *EnhanceImage(const Image *,ExceptionInfo *),
  *GaussianBlurImage(const Image *,const double,const double,ExceptionInfo *),
  *MedianFilterImage(const Image *,const double,ExceptionInfo *),
  *MotionBlurImage(const Image *,const double,const double,const double,
     ExceptionInfo *),
  *ReduceNoiseImage(const Image *,const double,ExceptionInfo *),
  *ShadeImage(const Image *,const unsigned int,double,double,ExceptionInfo *),
  *SharpenImage(const Image *,const double,const double,ExceptionInfo *),
  *SpreadImage(const Image *,const unsigned int,ExceptionInfo *),
  *UnsharpMaskImage(const Image *,const double,const double,const double,
     const double,ExceptionInfo *);

extern MagickExport unsigned int
  ChannelThresholdImage(Image *,const char *),
  ThresholdImage(Image *,const double);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_EFFECT_H */
