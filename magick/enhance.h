/*
  ImageMagick Image Enhancement Methods.
*/
#ifndef _MAGICK_ENHANCE_H
#define _MAGICK_ENHANCE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif  /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport unsigned int
  ContrastImage(Image *,const unsigned int), /* enhance.c */
  EqualizeImage(Image *), /* enhance.c */
  GammaImage(Image *,const char *), /* enhance.c */
  LevelImage(Image *,const char *), /* enhance.c */
  LevelImageChannel(Image *,const ChannelType,const double,const double,
    const double), /* enhance.c */
  ModulateImage(Image *,const char *), /* enhance.c */
  NegateImage(Image *,const unsigned int), /* enhance.c */
  NormalizeImage(Image *); /* enhance.c */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_ENHANCE_H */
