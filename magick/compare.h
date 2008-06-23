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
  UndefinedMetric,
  MeanAbsoluteErrorMetric,
  MeanSquaredErrorMetric,
  PeakAbsoluteErrorMetric,
  PeakSignalToNoiseRatioMetric,
  RootMeanSquaredErrorMetric
} MetricType;


extern MagickExport Image
  *CompareImageChannels(const Image *image,const Image *reconstruct_image,
                        const ChannelType channel,const MetricType metric,
                        double *distortion,ExceptionInfo *exception),
  *CompareImages(const Image *image,const Image *reconstruct_image,
                 const MetricType metric,double *distortion,
                 ExceptionInfo *exception);

extern MagickExport MagickBool
  IsImagesEqual(Image *,const Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_COMPARE_H */
