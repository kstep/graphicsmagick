/*
  ImageMagick Image Montage Methods.
*/
#ifndef _MAGICK_MONTAGE_H
#define _MAGICK_MONTAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport Image
  *MontageImages(const Image *,const MontageInfo *,ExceptionInfo *);

extern MagickExport MontageInfo
  *CloneMontageInfo(const ImageInfo *,const MontageInfo *);

extern MagickExport void
  DestroyMontageInfo(MontageInfo *),
  GetMontageInfo(const ImageInfo *,MontageInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_MONTAGE_H */
