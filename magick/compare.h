/*
  Copyright (C) 2008 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Image Comparison Methods.
*/
#ifndef _MAGICK_COMPARE_H
#define _MAGICK_COMPARE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* defined(__cplusplus) || defined(c_plusplus) */

typedef enum
{
  UndefinedDifferenceAlgorithm,
  AnnotateDifferenceAlgorithm,
  SubtractDifferenceAlgorithm,
  OpacityDifferenceAlgorithm
} DifferenceImageAlgorithm;

typedef struct _DifferenceImageOptions
{
  DifferenceImageAlgorithm algorithm;
  ChannelType              channel;
} DifferenceImageOptions;

extern MagickExport void
  DifferenceImageOptionsDefaults(DifferenceImageOptions *options);

extern MagickExport Image
  *DifferenceImage(const Image *reference_image,const Image *compare_image,
                   const DifferenceImageOptions *difference_options,
                   ExceptionInfo *exception);

extern MagickExport MagickBool
  IsImagesEqual(Image *,const Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COMPARE_H */
