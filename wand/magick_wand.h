/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2003 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick MagickWand interface.
*/

#ifndef _MAGICK_WAND_H
#define _MAGICK_WAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "wand/drawing_wand.h"
#include "wand/pixel_wand.h"

typedef struct _MagickWand
  MagickWand;

extern MagickExport const char
  *MagickGetFilename(MagickWand *),
  *MagickGetImageFilename(MagickWand *);

extern MagickExport MagickWand
  *MagickCloneWand(const MagickWand *),
  *MagickCoalesceImages(MagickWand *),
  *MagickDeconstructImages(MagickWand *),
  *MagickFlattenImages(MagickWand *),
  *NewMagickWand(void);

extern MagickExport unsigned int
  MagickAdaptiveThresholdImage(MagickWand *,const unsigned long,
    const unsigned long,const long),
  MagickAddNoiseImage(MagickWand *,const NoiseType),
  MagickAffineTransformImage(MagickWand *,const DrawingWand *),
  MagickAnnotateImage(MagickWand *,const DrawingWand *),
  MagickAnimateImages(MagickWand *,const char *),
  MagickBlackThresholdImage(MagickWand *,const PixelWand *),
  MagickBlurImage(MagickWand *,const double,const double),
  MagickBorderImage(MagickWand *,const PixelWand *,const unsigned long,
    const unsigned long),
  MagickChannelImage(MagickWand *,const ChannelType),
  MagickCharcoalImage(MagickWand *,const double,const double),
  MagickChopImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickClipImage(MagickWand *),
  MagickColorFloodfillImage(MagickWand *,PixelWand *,PixelWand *,
    const long,const long),
  MagickColorizeImage(MagickWand *,const PixelWand *,const PixelWand *),
  MagickCommentImage(MagickWand *,const char *),
  MagickCompositeImage(MagickWand *,const CompositeOperator,const MagickWand *,
    const long,const long),
  MagickCompareImage(MagickWand *,const MagickWand *,double *,double *,
    double *),
  MagickContrastImage(MagickWand *,const unsigned int),
  MagickConvolveImage(MagickWand *,const unsigned int,const double *),
  MagickCropImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickCycleColormapImage(MagickWand *,const long),
  MagickDescribeImage(MagickWand *,FILE *),
  MagickDespeckleImage(MagickWand *),
  MagickDisplayImage(MagickWand *,const char *),
  MagickDisplayImages(MagickWand *,const char *),
  MagickDrawImage(MagickWand *,const DrawingWand *),
  MagickEdgeImage(MagickWand *,const double),
  MagickEmbossImage(MagickWand *,const double,const double),
  MagickEnhanceImage(MagickWand *),
  MagickEqualizeImage(MagickWand *),
  MagickFlipImage(MagickWand *),
  MagickFlopImage(MagickWand *),
  MagickFrameImage(MagickWand *,const PixelWand *,const unsigned long,
    const unsigned long,const long,const long),
  MagickGammaImage(MagickWand *,const PixelWand *),
  MagickGetException(MagickWand *,char **),
  MagickGetImagePixels(MagickWand *,const long,const long,const unsigned long,
    const unsigned long,const char *,const StorageType,void *),
  MagickNextImage(MagickWand *),
  MagickPreviousImage(MagickWand *),
  MagickReadImage(MagickWand *,const char *),
  MagickResizeImage(MagickWand *,const unsigned long,const unsigned long,
    const FilterTypes,const double),
  MagickRemoveImage(MagickWand *),
  MagickRotateImage(MagickWand *,const double),
  MagickSetImage(MagickWand *,const unsigned long),
  MagickSetImageFilename(MagickWand *,const char *),
  MagickSetImagePixels(MagickWand *,const long,const long,const unsigned long,
    const unsigned long,const char *,const StorageType,void *),
  MagickWriteImage(MagickWand *,const char *),
  MagickWriteImages(MagickWand *,const char *);

extern MagickExport unsigned long
  MagickGetNumberOfImages(MagickWand *);

extern MagickExport void
  DestroyMagickWand(MagickWand *),
  MagickGetSize(MagickWand *,unsigned long *,unsigned long *),
  MagickSetFuzzyColorDistance(MagickWand *,const double),
  MagickSetFilename(MagickWand *,const char *),
  MagickSetSize(MagickWand *,const unsigned long,const unsigned long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
