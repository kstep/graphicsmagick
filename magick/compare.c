/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               CCCC   OOO   M   M  PPPP    AAA   RRRR    EEEEE               %
%              C      O   O  MM MM  P   P  A   A  R   R   E                   %
%              C      O   O  M M M  PPPP   AAAAA  RRRR    EEE                 %
%              C      O   O  M   M  P      A   A  R R     E                   %
%               CCCC   OOO   M   M  P      A   A  R  R    EEEEE               %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Image Compare Methods                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/compare.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p a r e I m a g e C h a n n e l s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompareImageChannels() compares one or more image channels of an image
%  with those in a second image and returns a difference image.
%
%  The format of the CompareImageChannels method is:
%
%      Image *CompareImageChannels(const Image *reference_image,
%        const Image *compare_image,const ChannelType channel,
%        const MetricType metric,double *distortion,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o reference_image: the reference image.
%
%    o compare_image: the comparison image.
%
%    o channel: the channel(s) to compare.
%
%    o metric: the comparison metric (UndefinedMetric,
%        MeanAbsoluteErrorMetric, MeanSquaredErrorMetric,
%        PeakAbsoluteErrorMetric, PeakSignalToNoiseRatioMetric,
%        or RootMeanSquaredErrorMetric)
%
%    o distortion: the computed distortion between the images based on the
%        specified metric.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
typedef struct _CompareImageOptions
{
  ChannelType channel;
} CompareImageOptions;
static MagickPassFail
CompareImageChannelsPixels(void *user_data,                   /* User provided mutable data */
                           const Image *source1_image,         /* Source 1 image */
                           const PixelPacket *source1_pixels,  /* Pixel row in source 1 image */
                           const IndexPacket *source1_indexes, /* Pixel row indexes in source 1 image */
                           const Image *source2_image,         /* Source 2 image */
                           const PixelPacket *source2_pixels,  /* Pixel row in source 2 image */
                           const IndexPacket *source2_indexes, /* Pixel row indexes in source 2 image */
                           Image *update_image,                   /* Update image */
                           PixelPacket *update_pixels,            /* Pixel row in update image */
                           IndexPacket *update_indexes,           /* Pixel row indexes in update image */
                           const long npixels,                 /* Number of pixels in row */
                           ExceptionInfo *exception            /* Exception report */
                           )
{
  const CompareImageOptions
    *compare_options = (const CompareImageOptions *) user_data;

  register ChannelType
    channels = compare_options->channel;

  register long
    i;

  register MagickBool
    change;

  ARG_NOT_USED(source2_image);
  ARG_NOT_USED(update_image);
  ARG_NOT_USED(update_indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      change=MagickFalse;

      if (IsCMYKColorspace(source1_image->colorspace))
        {
          if (MagickChannelEnabled(channels,CyanChannel) &&
              (GetCyanSample(&source1_pixels[i]) != GetCyanSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,MagentaChannel) &&
              (GetMagentSample(&source1_pixels[i]) != GetMagentSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,YellowChannel) &&
              (GetYellowSample(&source1_pixels[i]) != GetYellowSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,BlackChannel) &&
              (GetBlackSample(&source1_pixels[i]) != GetBlackSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,OpacityChannel) &&
              (source1_indexes[i] != source2_indexes[i]))
            change=MagickTrue;
        }
      else
        {
          if (MagickChannelEnabled(channels,RedChannel) &&
              (GetRedSample(&source1_pixels[i]) != GetRedSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,GreenChannel) &&
              (GetGreenSample(&source1_pixels[i]) != GetGreenSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,BlueChannel) &&
              (GetBlueSample(&source1_pixels[i]) != GetBlueSample(&source2_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,OpacityChannel) &&
              (GetOpacitySample(&source1_pixels[i]) != GetOpacitySample(&source2_pixels[i])))
            change=MagickTrue;
        }
      /*
        Modify update image to reflect change.
      */
      if (change)
        update_pixels[i]=AlphaComposite(&source1_image->border_color,0.75*MaxRGBDouble,
                                        &update_pixels[i],update_pixels[i].opacity);
      else
        update_pixels[i]=AlphaComposite(&source1_image->background_color,0.75*MaxRGBDouble,
                                        &update_pixels[i],update_pixels[i].opacity);
    }

  return MagickPass;
}

MagickExport Image *
CompareImageChannels(const Image *reference_image,
                     const Image *compare_image,const ChannelType channel,
                     const MetricType metric,double *distortion,
                     ExceptionInfo *exception)
{
  CompareImageOptions
    compare_options;

  Image
    *difference_image;

  assert(reference_image != (const Image *) NULL);
  assert(reference_image->signature == MagickSignature);
  assert(compare_image != (const Image *) NULL);
  assert(compare_image->signature == MagickSignature);
  assert(distortion != (double *) NULL);

  compare_options.channel=channel;
  *distortion=0.0;
  difference_image=CloneImage(reference_image,reference_image->columns,
                              reference_image->rows,MagickTrue,exception);
  if (difference_image == (Image *) NULL)
    return ((Image *) NULL);

  /*
    Update "difference" image to mark changes.
  */
  (void) PixelIterateTripleModify(CompareImageChannelsPixels,
                                  "Compare image pixels ...",
                                  &compare_options,
                                  reference_image->columns,reference_image->rows,
                                  reference_image, compare_image,0, 0,
                                  difference_image, 0, 0,
                                  exception);
  /*
    Compute image error based on metric.
    FIXME: TBD
  */

  return difference_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p a r e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompareImages() compares an image to a second image and returns a
%  difference image.
%
%  The format of the CompareImages method is:
%
%      Image *CompareImages(const Image *reference_image,
%        const Image *compare_image,
%        const MetricType metric,double *distortion,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o reference_image: the reference image.
%
%    o compare_image: the comparison image.
%
%    o metric: the comparison metric (UndefinedMetric,
%        MeanAbsoluteErrorMetric, MeanSquaredErrorMetric,
%        PeakAbsoluteErrorMetric, PeakSignalToNoiseRatioMetric,
%        or RootMeanSquaredErrorMetric)
%
%    o distortion: the computed distortion between the images based on the
%        specified metric.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport Image *
CompareImages(const Image *reference_image,const Image *compare_image,
              const MetricType metric,double *distortion,ExceptionInfo *exception)
{
  return CompareImageChannels(reference_image,compare_image,AllChannels,
                              metric,distortion,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s I m a g e s E q u a l                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsImagesEqual() measures the difference between colors at each pixel
%  location of two images.  A value other than 0 means the colors match
%  exactly.  Otherwise an error measure is computed by summing over all
%  pixels in an image the distance squared in RGB space between each image
%  pixel and its corresponding pixel in the reference image.  The error
%  measure is assigned to these image members:
%
%    o mean_error_per_pixel:  The mean error for any single pixel in
%      the image.
%
%    o normalized_mean_error:  The normalized mean quantization error for
%      any single pixel in the image.  This distance measure is normalized to
%      a range between 0 and 1.  It is independent of the range of red, green,
%      and blue values in the image.
%
%    o normalized_maximum_error:  The normalized maximum quantization
%      error for any single pixel in the image.  This distance measure is
%      normalized to a range between 0 and 1.  It is independent of the range
%      of red, green, and blue values in your image.
%
%  A small normalized mean square error, accessed as
%  image->normalized_mean_error, suggests the images are very similiar in
%  spatial layout and color.
%
%  The format of the IsImagesEqual method is:
%
%      unsigned int IsImagesEqual(Image *image,const Image *reference)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o reference: The reference image.
%
*/
typedef struct _PixelErrorStats {
  double
    maximum_error_per_pixel,
    total_error;
} PixelErrorStats;

static MagickPassFail
ComputeRowPixelError(void *user_data,
                     const Image *first_image,
                     const PixelPacket *first_pixels,
                     const IndexPacket *first_indexes,
                     const Image *second_image,
                     const PixelPacket *second_pixels,
                     const IndexPacket *second_indexes,
                     const long npixels,
                     ExceptionInfo *exception)
{
  PixelErrorStats
    *stats = (PixelErrorStats *) user_data;

  double
    difference,
    distance,
    distance_squared;

  register long
    i;

  ARG_NOT_USED(first_indexes);
  ARG_NOT_USED(second_image);
  ARG_NOT_USED(second_indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      difference=(first_pixels[i].red-(double) second_pixels[i].red)/MaxRGB;
      distance_squared=(difference*difference);
      difference=(first_pixels[i].green-(double) second_pixels[i].green)/MaxRGB;
      distance_squared+=(difference*difference);
      difference=(first_pixels[i].blue-(double) second_pixels[i].blue)/MaxRGB;
      distance_squared+=(difference*difference);
      if (first_image->matte)
        {
          difference=(first_pixels[i].opacity-(double) second_pixels[i].opacity)/MaxRGB;
          distance_squared+=(difference*difference);
        }
      distance=sqrt(distance_squared);
      stats->total_error+=distance;
      if (distance > stats->maximum_error_per_pixel)
        stats->maximum_error_per_pixel=distance;
    }
  return (MagickPass);
}

MagickExport unsigned int
IsImagesEqual(Image *image,const Image *reference)
{
  PixelErrorStats
    stats;

  double
    mean_error_per_pixel,
    normalize,
    number_pixels;

  /*
    Initialize measurement.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(reference != (const Image *) NULL);
  assert(reference->signature == MagickSignature);
  (void) memset(&image->error,0,sizeof(ErrorInfo));
  if ((image->rows != reference->rows) ||
      (image->columns != reference->columns))
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageSizeDiffers);
  if ((image->colorspace != reference->colorspace) &&
      (!IsRGBColorspace(image->colorspace) || !IsRGBColorspace(reference->colorspace)))
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageColorspaceDiffers);
  if(image->matte != reference->matte)
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageOpacityDiffers);

  /*
    For each pixel, collect error statistics.
  */
  number_pixels=(double) image->columns*image->rows;
  stats.maximum_error_per_pixel=0.0;
  stats.total_error=0.0;

  (void) PixelIterateDualRead(ComputeRowPixelError,
                              "Compute pixel error ...",
                              (void *) &stats,
                              image->columns,image->rows,
                              image,0,0,
                              reference,0,0,
                              &image->exception);

  /*
    Compute final error statistics.
  */
  
  if (image->matte)
    normalize = sqrt(4.0); /* sqrt(1.0*1.0+1.0*1.0+1.0*1.0+1.0*1.0) */
  else
    normalize = sqrt(3.0); /* sqrt(1.0*1.0+1.0*1.0+1.0*1.0) */
  mean_error_per_pixel=stats.total_error/number_pixels;
  image->error.mean_error_per_pixel=mean_error_per_pixel*MaxRGB;
  image->error.normalized_mean_error=mean_error_per_pixel/normalize;
  image->error.normalized_maximum_error=stats.maximum_error_per_pixel/normalize;
  return(image->error.normalized_mean_error == 0.0);
}
