/*
  ImageMagick Image Composite Methods.
*/
#ifndef _MAGICK_COMPOSITE_H
#define _MAGICK_COMPOSITE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

extern MagickExport unsigned int
  CompositeImage(Image *,const CompositeOperator,const Image *,const long,
    const long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COMPOSITE_H */
