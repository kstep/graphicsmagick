/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                         W   W   AAA   N   N  DDDD                           %
%                         W   W  A   A  NN  N  D   D                          %
%                         W W W  AAAAA  N N N  D   D                          %
%                         WW WW  A   A  N  NN  D   D                          %
%                         W   W  A   A  N   N  DDDD                           %
%                                                                             %
%                                                                             %
%                 ImageMagick MagickWand Programming Interface                %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 August 2003                                 %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  This is the tentative public API for ImageMagick.  Use it with caution
%  because it is subject to change until it is finished somewhere around
%  September of 2003.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/error.h"
#include "magick/image.h"
#include "magick/list.h"
#include "magick/paint.h"
#include "magick/quantize.h"
#include "magick/resize.h"
#include "magick/utility.h"
#include "wand/wand_api.h"
#include "wand/magick_compat.h"

/*
  Typedef declarations.
*/
struct _MagickWand
{
  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;

  Image
    *image,
    *images;

  unsigned long
    signature;
};

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k W a n d                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickWand() deallocates memory associated with an MagickWand.
%
%  The format of the DestroyMagickWand method is:
%
%      unsigned int DestroyMagickWand(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%
*/
WandExport void DestroyMagickWand(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  DestroyQuantizeInfo(wand->quantize_info);
  DestroyImageInfo(wand->image_info);
  DestroyExceptionInfo(&wand->exception);
  if (wand->images != (Image *) NULL)
    DestroyImageList(wand->images);
  RelinquishMagickMemory(wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A d a p t i v e T h r e s o l d I m a g e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAdaptiveThresholdImage() selects an individual threshold for each pixel
%  based on the range of intensity values in its local neighborhood.  This
%  allows for thresholding of an image whose global intensity histogram
%  doesn't contain distinctive peaks.
%
%  The format of the AdaptiveThresholdImage method is:
%
%      unsigned int MagickAdaptiveThresholdImage(MagickWand *wand,
%        const unsigned long width,const unsigned long height,const long offset)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width: The width of the local neighborhood.
%
%    o height: The height of the local neighborhood.
%
%    o offset: The mean offset.
%
*/
WandExport unsigned int MagickAdaptiveThresholdImage(MagickWand *wand,
  const unsigned long width,const unsigned long height,const long offset)
{
  Image
    *threshold_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  threshold_image=AdaptiveThresholdImage(wand->image,width,height,offset,
    &wand->exception);
  if (threshold_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,threshold_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M a g i c k A d d N o i s e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAddNoiseImage() adds random noise to the image.
%
%  The format of the MagickAddNoiseImage method is:
%
%      unsigned int MagickAddNoiseImage(MagickWand *wand,
%        const NoiseType noise_type)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o noise_type:  The type of noise: Uniform, Gaussian, Multiplicative,
%      Impulse, Laplacian, or Poisson.
%
*/
WandExport unsigned int MagickAddNoiseImage(MagickWand *wand,
  const NoiseType noise_type)
{
  Image
    *noise_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  noise_image=AddNoiseImage(wand->image,noise_type,&wand->exception);
  if (noise_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,noise_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A f f i n e T r a n s f o r m I m a g e                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAffineTransformImage() transforms an image as dictated by the affine
%  matrix of the drawing wand.
%
%  The format of the MagickAffineTransformImage method is:
%
%      unsigned int MagickAffineTransformImage(MagickWand *wand,
%        const DrawingWand *draw_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o draw_wand: The draw wand.
%
*/
WandExport unsigned int MagickAffineTransformImage(MagickWand *wand,
  const DrawingWand *draw_wand)
{
  const DrawInfo
    *draw_info;

  Image
    *affine_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  draw_info=DrawPeekGraphicContext((const DrawContext)draw_wand);
  if (draw_info == (DrawInfo *) NULL)
    return(False);
  affine_image=AffineTransformImage(wand->image,&draw_info->affine,
    &wand->exception);
  if (affine_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,affine_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A n n o t a t e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAnnotateImage() annotates an image with text.
%
%  The format of the MagickAnnotateImage method is:
%
%      unsigned int MagickAnnotateImage(MagickWand *wand,
%        const DrawingWand *draw_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o draw_wand: The draw wand.
%
*/
WandExport unsigned int MagickAnnotateImage(MagickWand *wand,
  const DrawingWand *draw_wand)
{
  const DrawInfo
    *draw_info;

  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  draw_info=DrawPeekGraphicContext((const DrawContext)draw_wand);
  if (draw_info == (const DrawInfo *) NULL)
    return(False);
  status=AnnotateImage(wand->image,draw_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A n i m a t e I m a g e s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAnimateImages() animates an image or image sequence.
%
%  The format of the MagickAnimateImages method is:
%
%      unsigned int MagickAnimateImages(MagickWand *wand,
%        const char *server_name)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o server_name: The X server name.
%
%
*/
WandExport unsigned int MagickAnimateImages(MagickWand *wand,
  const char *server_name)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->server_name=(char *) AcquireString(server_name);
  status=AnimateImages(wand->image_info,wand->images);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k B l a c k T h r e s o l d I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickBlackThresholdImage() is like MagickThresholdImage() but  forces all
%  pixels below the threshold into black while leaving all pixels above the
%  threshold unchanged.
%
%  The format of the MagickBlackThresholdImage method is:
%
%      unsigned int MagickBlackThresholdImage(MagickWand *wand,
%        const PixelWand *threshold)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o threshold: The pixel wand.
%
*/
#if FIXME
WandExport unsigned int MagickBlackThresholdImage(MagickWand *wand,
  const PixelWand *threshold)
{
  char
    thresholds[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  FormatString(thresholds,"%u,%u,%u,%u",PixelGetRedQuantum(threshold),
    PixelGetGreenQuantum(threshold),PixelGetBlueQuantum(threshold),
    PixelGetOpacityQuantum(threshold));
  status=BlackThresholdImage(wand->image,thresholds);
  return(status);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k B l u r I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickBlurImage() blurs an image.  We convolve the image with a Gaussian
%  operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and BlurImage() selects a suitable radius for you.
%
%  The format of the MagickBlurImage method is:
%
%      unsigned int MagickBlurImage(MagickWand *wand,const double radius,
%        const double sigma)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
*/
WandExport unsigned int MagickBlurImage(MagickWand *wand,const double radius,
  const double sigma)
{
  Image
    *blur_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  blur_image=BlurImage(wand->image,radius,sigma,&wand->exception);
  if (blur_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,blur_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k B o r d e r I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickBorderImage() surrounds the image with a border of the color defined
%  by the bordercolor pixel wand.
%
%  The format of the MagickBorderImage method is:
%
%      unsigned int MagickBorderImage(MagickWand *wand,
%        const PixelWand *bordercolor,const unsigned long width,
%        const unsigned long height)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o bordercolor: The border color pixel wand.
%
%    o width: The border width.
%
%    o height: The border height.
%
*/
WandExport unsigned int MagickBorderImage(MagickWand *wand,
  const PixelWand *bordercolor,const unsigned long width,
  const unsigned long height)
{
  Image
    *border_image;

  RectangleInfo
    border;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  (void) memset(&border,0,sizeof(RectangleInfo));
  border.width=width;
  border.height=height;
  PixelGetQuantumColor(bordercolor,&wand->image->border_color);
  border_image=BorderImage(wand->image,&border,&wand->exception);
  if (border_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,border_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C h a n n e l I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickChannelImage() extracts a channel from the image.  A channel is a
%  particular color component of each pixel in the image.
%
%  The format of the MagickChannelImage method is:
%
%      unsigned int MagickChannelImage(MagickWand *wand,
%        const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      or BlackChannel.
%
*/
WandExport unsigned int MagickChannelImage(MagickWand *wand,
  const ChannelType channel)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  status=ChannelImage(wand->image,channel);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C h a r c o a l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCharcoalImage() simulates a charcoal drawing.
%
%  The format of the MagickCharcoalImage method is:
%
%      unsigned int MagickCharcoalImage(MagickWand *wand,const double radius,
%        const double sigma)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
*/
WandExport unsigned int MagickCharcoalImage(MagickWand *wand,
  const double radius,const double sigma)
{
  Image
    *charcoal_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  charcoal_image=CharcoalImage(wand->image,radius,sigma,&wand->exception);
  if (charcoal_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,charcoal_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C h o p I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickChopImage() removes a region of an image and collapses the image to
%  occupy the removed portion
%
%  The format of the MagickChopImage method is:
%
%      unsigned int MagickChopImage(MagickWand *wand,const unsigned long width,
%        const unsigned long height,const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width: The region width.
%
%    o height: The region height.
%
%    o x: The region x offset.
%
%    o y: The region y offset.
%
%
*/
WandExport unsigned int MagickChopImage(MagickWand *wand,
  const unsigned long width,const unsigned long height,const long x,
  const long y)
{
  Image
    *chop_image;

  RectangleInfo
    chop;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  chop.width=width;
  chop.height=height;
  chop.x=x;
  chop.y=y;
  chop_image=ChopImage(wand->image,&chop,&wand->exception);
  if (chop_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,chop_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C l o n e W a n d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCloneWand() deallocates memory associated with an MagickWand.
%
%  The format of the MagickCloneWand method is:
%
%      unsigned int MagickCloneWand(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%
*/
WandExport MagickWand *MagickCloneWand(const MagickWand *wand)
{
  Image
    *image;

  MagickWand
    *clone_wand;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  clone_wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateImage");
  (void) memset(clone_wand,0,sizeof(MagickWand));
  GetExceptionInfo(&clone_wand->exception);
  ThrowException(&clone_wand->exception,wand->exception.severity,
    wand->exception.reason,wand->exception.description);
  clone_wand->image_info=CloneImageInfo(wand->image_info);
  clone_wand->quantize_info=CloneQuantizeInfo(wand->quantize_info);
  clone_wand->images=CloneImageList(wand->images,&clone_wand->exception);
  clone_wand->image=clone_wand->images;
  for (image=wand->images; image != wand->image; image=image->next)
    clone_wand->image=clone_wand->image->next;
  clone_wand->signature=MagickSignature;
  return(clone_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C l i p I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickClipImage() ets the image clip mask based any clipping path
%  information if it exists.
%
%  The format of the MagickClipImage method is:
%
%      unsigned int MagickClipImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickClipImage(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return(ClipImage(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o e l e s c e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCoalesceImages() composites a set of images while respecting any page
%  offsets and disposal methods.  GIF, MIFF, and MNG animation sequences
%  typically start with an image background and each subsequent image
%  varies in size and offset.  MagickCoalesceImages() returns a new sequence
%  where each image in the sequence is the same size as the first and
%  composited with the next image in the sequence.
%
%  The format of the MagickCoalesceImages method is:
%
%      MagickWand *MagickCoalesceImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickWand *MagickCoalesceImages(MagickWand *wand)
{
  Image
    *coalesce_image;

  MagickWand
    *clone_wand;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  coalesce_image=CoalesceImages(wand->images,&wand->exception);
  if (coalesce_image == (Image *) NULL)
    return((MagickWand *) NULL);
  clone_wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    {
      DestroyImage(coalesce_image);
      MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
        "UnableToAllocateImage");
    }
  (void) memset(clone_wand,0,sizeof(MagickWand));
  clone_wand->images=coalesce_image;
  clone_wand->image=clone_wand->images;
  ThrowException(&clone_wand->exception,wand->exception.severity,
    wand->exception.reason,wand->exception.description);
  clone_wand->image_info=CloneImageInfo(wand->image_info);
  clone_wand->quantize_info=CloneQuantizeInfo(wand->quantize_info);
  clone_wand->images=CloneImageList(wand->images,&clone_wand->exception);
  clone_wand->signature=MagickSignature;
  return(clone_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o l o r F l o o d f i l l I m a g e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickColorFloodfillImage() changes the color value of any pixel that matches
%  target and is an immediate neighbor.  If the method FillToBorderMethod is
%  specified, the color value is changed for any neighbor pixel that does not
%  match the bordercolor member of image.
%
%  By default target must match a particular pixel color exactly.
%  However, in many cases two colors may differ by a small amount.  The
%  fuzz member of image defines how much tolerance is acceptable to
%  consider two colors as the same.  For example, set fuzz to 10 and the
%  color red at intensities of 100 and 102 respectively are now
%  interpreted as the same color for the purposes of the floodfill.
%
%  The format of the MagickColorFloodfillImage method is:
%
%      unsigned int MagickColorFloodfillImage(MagickWand *wand,PixelWand *fill,
%        PixelWand *bordercolor,const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fill: The floodfill color pixel wand.
%
%    o bordercolor: The border color pixel wand.
%
%    o x,y: The starting location of the operation.
%
*/
WandExport unsigned int MagickColorFloodfillImage(MagickWand *wand,
  PixelWand *fill,PixelWand *bordercolor,const long x,const long y)
{
  DrawInfo
    *draw_info;

  PixelPacket
    target;

  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  draw_info=CloneDrawInfo(wand->image_info,(DrawInfo *) NULL);
  PixelGetQuantumColor(fill,&draw_info->fill);
  target=AcquireOnePixel(wand->image,x % wand->image->columns,
    y % wand->image->rows,&wand->exception);
  if (bordercolor != (PixelWand *) NULL)
    PixelGetQuantumColor(bordercolor,&target);
  status=ColorFloodfillImage(wand->image,draw_info,target,x,y,
    bordercolor != (PixelWand *) NULL ? FillToBorderMethod : FloodfillMethod);
  DestroyDrawInfo(draw_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o l o r i z e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickColorizeImage() blends the fill color with each pixel in the image.
%
%  The format of the MagickColorizeImage method is:
%
%      unsigned int MagickColorizeImage(MagickWand *wand,const PixelWand *fill,
%        const PixelWand *opacity)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fill: The fill pixel wand.
%
%    o opacity: The opacity pixel wand.
%
*/
WandExport unsigned int MagickColorizeImage(MagickWand *wand,
  const PixelWand *fill,const PixelWand *opacity)
{
  char
    percent_opaque[MaxTextExtent];

  Image
    *colorize_image;

  PixelPacket
    target;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  FormatString(percent_opaque,"%g,%g,%g,%g",
    100.0*PixelGetRedQuantum(fill)/MaxRGB,
    100.0*PixelGetGreenQuantum(fill)/MaxRGB,
    100.0*PixelGetBlueQuantum(fill)/MaxRGB,
    100.0*PixelGetOpacityQuantum(fill)/MaxRGB);
  PixelGetQuantumColor(fill,&target);
  colorize_image=ColorizeImage(wand->image,percent_opaque,target,
    &wand->exception);
  if (colorize_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,colorize_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o m m e n t I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCommentImage() adds a comment to your image.
%
%  The format of the MagickCommentImage method is:
%
%      unsigned int MagickCommentImage(MagickWand *wand,const char *comment)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o comment: The image comment
%
*/
WandExport unsigned int MagickCommentImage(MagickWand *wand,
  const char *comment)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  (void) SetImageAttribute(wand->image,"comment",(char *) NULL);
  SetImageAttribute(wand->image,"comment",comment);
  return(SetImageAttribute(wand->image,"comment",comment));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o m p a r e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCompareImage() measures the difference between colors at each pixel
%  location of two images.  A value other than 0 means the colors match
%  exactly.  Otherwise an error measure is computed by summing over all
%  pixels in an image the distance squared in RGB space between each image
%  pixel and its corresponding pixel in the reference image.  A small
%  normalized mean square error, suggests the images are very similiar in
%  spatial layout and color.
%
%  The format of the MagickCompareImage method is:
%
%      unsigned int MagickCompareImage(MagickWand *wand,
%        const MagickWand *reference,double *mean_error_per_pixel,
%        double *normalized_mean_error,double *normalized_maximum_error)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o reference: The reference wand.
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
*/
WandExport unsigned int MagickCompareImage(MagickWand *wand,
  const MagickWand *reference,double *mean_error_per_pixel,
  double *normalized_mean_error,double *normalized_maximum_error)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  if (reference->image == (Image *) NULL)
    return(False);
  status=IsImagesEqual(wand->image,reference->image);
  *mean_error_per_pixel=wand->image->error.mean_error_per_pixel;
  *normalized_mean_error=wand->image->error.normalized_mean_error;
  *normalized_maximum_error=wand->image->error.normalized_maximum_error;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o m p o s i t e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCompositeImage() composite one image onto another at the specified
%  offset.
%
%  The format of the MagickCompositeImage method is:
%
%      unsigned int MagickCompositeImage(MagickWand *wand,
%        const CompositeOperator compose,const MagickWand *composite_wand,
%        const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o compose: This operator affects how the composite is applied to the
%      image.  The default is Over.  Choose from these operators:
%
%        OverCompositeOp       InCompositeOp         OutCompositeOP
%        AtopCompositeOP       XorCompositeOP        PlusCompositeOP
%        MinusCompositeOP      AddCompositeOP        SubtractCompositeOP
%        DifferenceCompositeOP BumpmapCompositeOP    CopyCompositeOP
%        DisplaceCompositeOP
%
%    o composite_image: The composite image.
%
%    o x_offset: The column offset of the composited image.
%
%    o y_offset: The row offset of the composited image.
%
%
*/
WandExport unsigned int MagickCompositeImage(MagickWand *wand,
  const CompositeOperator compose,const MagickWand *composite_wand,
  const long x,const long y)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (composite_wand->image == (Image *) NULL))
    return(False);
  status=CompositeImage(wand->image,compose,composite_wand->image,x,y);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o n t r a s t I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickContrastImage() enhances the intensity differences between the lighter
%  and darker elements of the image.  Set sharpen to a value other than 0 to
%  increase the image contrast otherwise the contrast is reduced.
%
%  The format of the MagickContrastImage method is:
%
%      unsigned int MagickContrastImage(MagickWand *wand,
%        const unsigned int sharpen)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o sharpen: Increase or decrease image contrast.
%
%
*/
WandExport unsigned int MagickContrastImage(MagickWand *wand,
  const unsigned int sharpen)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return(ContrastImage(wand->image,sharpen));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o n v o l v e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickConvolveImage() applies a custom convolution kernel to the image.
%
%  The format of the MagickConvolveImage method is:
%
%      unsigned int MagickConvolveImage(MagickWand *wand,
%        const unsigned int order,const double *kernel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o order: The number of columns and rows in the filter kernel.
%
%    o kernel: An array of double representing the convolution kernel.
%
%
*/
WandExport unsigned int MagickConvolveImage(MagickWand *wand,
  const unsigned int order,const double *kernel)
{
  Image
    *convolve_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (kernel == (const double *) NULL)
    return(False);
  if (wand->image == (Image *) NULL)
    return(False);
  convolve_image=ConvolveImage(wand->image,order,kernel,&wand->exception);
  if (convolve_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,convolve_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C r o p I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCropImage() extracts a region of the image.
%
%  The format of the MagickCropImage method is:
%
%      unsigned int MagickCropImage(MagickWand *wand,const unsigned long width,
%        const unsigned long height,const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width: The region width.
%
%    o height: The region height.
%
%    o x: The region x offset.
%
%    o y: The region y offset.
%
%
*/
WandExport unsigned int MagickCropImage(MagickWand *wand,
  const unsigned long width,const unsigned long height,const long x,
  const long y)
{
  Image
    *crop_image;

  RectangleInfo
    crop;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  crop.width=width;
  crop.height=height;
  crop.x=x;
  crop.y=y;
  crop_image=CropImage(wand->image,&crop,&wand->exception);
  if (crop_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,crop_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C y c l e C o l o r m a p I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCycleColormapImage() displaces an image's colormap by a given number
%  of positions.  If you cycle the colormap a number of times you can produce
%  a psychodelic effect.
%
%  The format of the MagickCycleColormapImage method is:
%
%      unsigned int MagickCycleColormapImage(MagickWand *wand,
%        const long displace)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o pixel_wand: The pixel wand.
%
*/
WandExport unsigned int MagickCycleColormapImage(MagickWand *wand,
  const long displace)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  status=CycleColormapImage(wand->image,displace);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o e l e s c e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDeconstructImages() compares each image with the next in a sequence
%  and returns the maximum bounding region of any pixel differences it
%  discovers.
%
%  The format of the MagickDeconstructImages method is:
%
%      MagickWand *MagickDeconstructImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickWand *MagickDeconstructImages(MagickWand *wand)
{
  Image
    *deconstruct_image;

  MagickWand
    *clone_wand;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  deconstruct_image=DeconstructImages(wand->images,&wand->exception);
  if (deconstruct_image == (Image *) NULL)
    return((MagickWand *) NULL);
  clone_wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    {
      DestroyImage(deconstruct_image);
      MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
        "UnableToAllocateImage");
    }
  (void) memset(clone_wand,0,sizeof(MagickWand));
  clone_wand->images=deconstruct_image;
  clone_wand->image=clone_wand->images;
  ThrowException(&clone_wand->exception,wand->exception.severity,
    wand->exception.reason,wand->exception.description);
  clone_wand->image_info=CloneImageInfo(wand->image_info);
  clone_wand->quantize_info=CloneQuantizeInfo(wand->quantize_info);
  clone_wand->images=CloneImageList(wand->images,&clone_wand->exception);
  clone_wand->signature=MagickSignature;
  return(clone_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W r i t e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDescribeImage()  describes an image by printing its attributes to the
%  file.  Attributes include the image width, height, size, and others.
%
%  The format of the MagickDescribeImage method is:
%
%      unsigned int MagickDescribeImage(MagickWand *wand,FILE *file)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o The file, typically stdout.
%
%
*/
WandExport unsigned int MagickDescribeImage(MagickWand *wand,FILE *file)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return(DescribeImage(wand->image,file,True));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M a g i c k D e s p e c k l e I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDespeckleImage() reduces the speckle noise in an image while
%  perserving the edges of the original image.
%
%  The format of the MagickDespeckleImage method is:
%
%      unsigned int MagickDespeckleImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickDespeckleImage(MagickWand *wand)
{
  Image
    *despeckle_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  despeckle_image=DespeckleImage(wand->image,&wand->exception);
  if (despeckle_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,despeckle_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D i s p l a y I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDisplayImage() displays an image.
%
%  The format of the MagickDisplayImage method is:
%
%      unsigned int MagickDisplayImage(MagickWand *wand,const char *server_name)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o server_name: The X server name.
%
*/
WandExport unsigned int MagickDisplayImage(MagickWand *wand,
  const char *server_name)
{
  Image
    *image;

  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (image == (Image *) NULL)
    return(False);
  wand->image_info->server_name=(char *) AcquireString(server_name);
  status=DisplayImages(wand->image_info,image);
  DestroyImage(image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D i s p l a y I m a g e s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDisplayImages() displays an image or image sequence.
%
%  The format of the MagickDisplayImages method is:
%
%      unsigned int MagickDisplayImages(MagickWand *wand,
%        const char *server_name)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o server_name: The X server name.
%
%
*/
WandExport unsigned int MagickDisplayImages(MagickWand *wand,
  const char *server_name)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  CloneString(&wand->image_info->server_name,server_name);
  status=DisplayImages(wand->image_info,wand->images);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D r a w I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDrawImage() draw an image the specified number of degrees.
%
%  The format of the MagickDrawImage method is:
%
%      unsigned int MagickDrawImage(MagickWand *wand,
%        const DrawingWand *draw_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o draw_wand: The draw wand.
%
*/
/* FIXME: This interface is brain-damaged */
WandExport unsigned int MagickDrawImage(MagickWand *wand,
  const DrawingWand *draw_wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return DrawRender((const DrawContext)draw_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E d g e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickEdgeImage() enhance edges within the image with a convolution filter
%  of the given radius.  Use a radius of 0 and Edge() selects a suitable
%  radius for you.
%
%  The format of the MagickEdgeImage method is:
%
%      unsigned int MagickEdgeImage(MagickWand *wand,const double radius)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: the radius of the pixel neighborhood.
%
*/
WandExport unsigned int MagickEdgeImage(MagickWand *wand,
  const double radius)
{
  Image
    *edge_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  edge_image=EdgeImage(wand->image,radius,&wand->exception);
  if (edge_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,edge_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E m b o s s I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickEmbossImage() returns a grayscale image with a three-dimensional
%  effect.  We convolve the image with a Gaussian operator of the given radius
%  and standard deviation (sigma).  For reasonable results, radius should be
%  larger than sigma.  Use a radius of 0 and Emboss() selects a suitable
%  radius for you.
%
%  The format of the MagickEmbossImage method is:
%
%      unsigned int MagickEmbossImage(MagickWand *wand,const double radius,
%        const double sigma)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
*/
WandExport unsigned int MagickEmbossImage(MagickWand *wand,
  const double radius,const double sigma)
{
  Image
    *emboss_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  emboss_image=EmbossImage(wand->image,radius,sigma,&wand->exception);
  if (emboss_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,emboss_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E n h a n c e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickEnhanceImage() applies a digital filter that improves the quality of a
%  noisy image.
%
%  The format of the MagickEnhanceImage method is:
%
%      unsigned int MagickEnhanceImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickEnhanceImage(MagickWand *wand)
{
  Image
    *enhance_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  enhance_image=EnhanceImage(wand->image,&wand->exception);
  if (enhance_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,enhance_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E q u a l i z e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickEqualizeImage() equalizes the image histogram.
%
%  The format of the MagickEqualizeImage method is:
%
%      unsigned int MagickEqualizeImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickEqualizeImage(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return(EqualizeImage(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k F l a t t e n I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFlattenImages() merges a sequence of images.  This is useful for
%  combining Photoshop layers into a single image.
%
%  The format of the MagickFlattenImages method is:
%
%      MagickWand *MagickFlattenImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickWand *MagickFlattenImages(MagickWand *wand)
{
  Image
    *flatten_image;

  MagickWand
    *clone_wand;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  flatten_image=FlattenImages(wand->images,&wand->exception);
  if (flatten_image == (Image *) NULL)
    return((MagickWand *) NULL);
  clone_wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    {
      DestroyImage(flatten_image);
      MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
        "UnableToAllocateImage");
    }
  (void) memset(clone_wand,0,sizeof(MagickWand));
  clone_wand->images=flatten_image;
  clone_wand->image=clone_wand->images;
  ThrowException(&clone_wand->exception,wand->exception.severity,
    wand->exception.reason,wand->exception.description);
  clone_wand->image_info=CloneImageInfo(wand->image_info);
  clone_wand->quantize_info=CloneQuantizeInfo(wand->quantize_info);
  clone_wand->images=CloneImageList(wand->images,&clone_wand->exception);
  clone_wand->signature=MagickSignature;
  return(clone_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k F l i p I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFlipImage() creates a vertical mirror image by reflecting the pixels
%  around the central x-axis.
%
%  The format of the MagickFlipImage method is:
%
%      unsigned int MagickFlipImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickFlipImage(MagickWand *wand)
{
  Image
    *flip_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  flip_image=FlipImage(wand->image,&wand->exception);
  if (flip_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,flip_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k F l o p I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFlopImage() creates a horizontal mirror image by reflecting the pixels
%  around the central y-axis.
%
%  The format of the MagickFlopImage method is:
%
%      unsigned int MagickFlopImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickFlopImage(MagickWand *wand)
{
  Image
    *flop_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  flop_image=FlopImage(wand->image,&wand->exception);
  if (flop_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,flop_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k F r a m e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFrameImage() adds a simulated three-dimensional border around the
%  image. The color of the border is defined by the matte_color member of
%  image.  Members width and height of frame_info specify the border width of
%  the vertical and horizontal sides of the frame.  Members inner and outer
%  indicate the width of the inner and outer shadows of the frame.
%
%  The format of the MagickFrameImage method is:
%
%      unsigned int MagickFrameImage(MagickWand *wand,
%        const PixelWand *framecolor,const unsigned long width,
%        const unsigned long height,const long inner_bevel,
%        const long outer_bevel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o framecolor: The frame color pixel wand.
%
%    o width: The border width.
%
%    o height: The border height.
%
%    o inner_bevel: The inner bevel width.
%
%    o outer_bevel: The outer bevel width.
%
*/
WandExport unsigned int MagickFrameImage(MagickWand *wand,
  const PixelWand *framecolor,const unsigned long width,
  const unsigned long height,const long inner_bevel,const long outer_bevel)
{
  Image
    *frame_image;

  FrameInfo
    frame_info;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  (void) memset(&frame_info,0,sizeof(FrameInfo));
  frame_info.width=wand->image->columns+2*width;
  frame_info.height=wand->image->rows+2*height;
  frame_info.x=(long) width;
  frame_info.y=(long) height;
  frame_info.inner_bevel=inner_bevel;
  frame_info.outer_bevel=outer_bevel;
  frame_image=FrameImage(wand->image,&frame_info,&wand->exception);
  if (frame_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,frame_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k B l a c k T h r e s o l d I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use MagickGammaImage() to gamma-correct an image.  The same image viewed on
%  different devices will have perceptual differences in the way the
%  image's intensities are represented on the screen.  Specify individual
%  gamma levels for the red, green, and blue channels, or adjust all three
%  with the gamma parameter.  Values typically range from 0.8 to 2.3.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the MagickGammaImage method is:
%
%      unsigned int MagickGammaImage(MagickWand *wand,const PixelWand *level)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o level: Define the level of gamma correction.
%
*/
WandExport unsigned int MagickGammaImage(MagickWand *wand,
  const PixelWand *level)
{
  char
    levels[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  FormatString(levels,"%u,%u,%u,%u",PixelGetRedQuantum(level),
    PixelGetGreenQuantum(level),PixelGetBlueQuantum(level),
    PixelGetOpacityQuantum(level));
  status=GammaImage(wand->image,levels);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t E x c e p t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetException() returns the severity, reason, and description of any
%  error that occurs when using other methods in this API.
%
%  The format of the MagickGetException method is:
%
%      unsigned int MagickGetException(MagickWand *wand,char **description)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o description: A description of the error.
%
*/

static const char *GetErrorMessageString(const int error_number)
{
  const char
    *message;

  message=strerror(error_number);
  if (message == (const char *) NULL)
    return("Error number is out of range");
  return(message);
}

WandExport unsigned int MagickGetException(MagickWand *wand,
  char **description)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  assert(description != (char **) NULL);
  *description=(char *) malloc(2*MaxTextExtent);
  if (*description == (char *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateString");
  **description='\0';
  if (wand->exception.reason != (char *) NULL)
    (void) strncpy(*description,GetLocaleExceptionMessage(
      wand->exception.severity,wand->exception.reason),MaxTextExtent-1);
  if (wand->exception.description != (char *) NULL)
    {
      (void) strcat(*description," (");
      (void) strncat(*description,GetLocaleExceptionMessage(
        wand->exception.severity,wand->exception.description),
        MaxTextExtent-strlen(*description)-1);
      (void) strcat(*description,")");
    }
  if ((wand->exception.severity != OptionError) && errno)
    {
      (void) strcat(*description," [");
      (void) strncat(*description,GetErrorMessageString(errno),
        MaxTextExtent-strlen(*description)-1);
      (void) strcat(*description,"]");
    }
  return(wand->exception.severity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t F i l e n a m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetFilename() returns the filename associated with an image sequence.
%
%  The format of the MagickGetFilename method is:
%
%      const char *MagickGetFilename(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%
*/
WandExport const char *MagickGetFilename(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return(AcquireString(wand->image_info->filename));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e F i l e n a m e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageFilename() returns the filename of a particular image in a
%  sequence.
%
%  The format of the MagickGetImageFilename method is:
%
%      const char MagickGetImageFilename(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport const char *MagickGetImageFilename(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return(AcquireString(wand->image->filename));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e P i x e l s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImagePixels() extracts pixel data from an image and returns it to
%  you.  The method returns False on success otherwise True if an error is
%  encountered.  The data is returned as char, short int, int, long, float,
%  or double in the order specified by map.
%
%  Suppose you want to extract the first scanline of a 640x480 image as
%  character data in red-green-blue order:
%
%      MagickGetImagePixels(wand,0,0,0,640,1,"RGB",CharPixel,pixels);
%
%  The format of the MagickGetImagePixels method is:
%
%      unsigned int MagickGetImagePixels(MagickWand *wand,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType storage,
%        void *pixels)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_offset, y_offset, columns, rows:  These values define the perimeter
%      of a region of pixels you want to extract.
%
%    o map:  This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha, C = cyan, Y = yellow, M = magenta, K = black, or
%      I = intensity (for grayscale).
%
%    o storage: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%
*/
WandExport unsigned int MagickGetImagePixels(MagickWand *wand,
  const long x_offset,const long y_offset,const unsigned long columns,
  const unsigned long rows,const char *map,const StorageType storage,
  void *pixels)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  status=ExportImagePixels(wand->image,x_offset,y_offset,columns,rows,map,
    storage,pixels,&wand->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t N u m b e r O f I m a g e s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetNumberOfImages() returns the number of images associated with a
%  magick wand.
%
%  The format of the MagickGetNumberOfImages method is:
%
%      unsigned long MagickGetNumberOfImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetNumberOfImages(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  return(GetImageListLength(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t S i z e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetSize() returns the size associated with the magick wand.
%
%  The format of the MagickGetSize method is:
%
%      void MagickGetSize(MagickWand *wand,unsigned long *columns,
%        unsigned long *rows)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o columns: The width in pixels.
%
%    o height: The height in pixels.
%
*/
WandExport void MagickGetSize(MagickWand *wand,unsigned long *columns,
  unsigned long *rows)
{
  RectangleInfo
    geometry;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  (void) memset(&geometry,0,sizeof(RectangleInfo));
  (void) ParseAbsoluteGeometry(wand->image_info->size,&geometry);
  *columns=geometry.width;
  *rows=geometry.height;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k N e x t I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickNextImage() selects the next image associated with a magick wand.
%
%  The format of the MagickNextImage method is:
%
%      const char MagickNextImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickNextImage(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) || (wand->image->next == (Image *) NULL))
    return(False);
  wand->image=wand->image->next;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k P r e v i o u s I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickPreviousImage() selects the previous image associated with a magick
%  wand.
%
%  The format of the MagickPreviousImage method is:
%
%      const char MagickPreviousImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickPreviousImage(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (wand->image->previous == (Image *) NULL))
    return(False);
  wand->image=wand->image->previous;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e a d I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReadImage() reads an image or image sequence.
%
%  The format of the MagickReadImage method is:
%
%      unsigned int MagickReadImage(MagickWand *wand,const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
%
*/
WandExport unsigned int MagickReadImage(MagickWand *wand,
  const char *filename)
{
  Image
    *images;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  (void) strncpy(wand->image_info->filename,filename,MaxTextExtent-1);
  images=ReadImage(wand->image_info,&wand->exception);
  if (images == (Image *) NULL)
    return(False);
  AppendImageToList(&wand->images,images);
  if (wand->image == (Image *) NULL)
    wand->image=wand->images;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e m o v e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRemoveImage() removes an image from the image list.
%
%  The format of the MagickRemoveImage method is:
%
%      unsigned int MagickRemoveImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickRemoveImage(MagickWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  DeleteImageFromList(&wand->image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e s i z e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickResizeImage() scales an image to the desired dimensions with one of
%  these filters:
%
%    Bessel   Blackman   Box
%    Catrom   Cubic      Gaussian
%    Hanning  Hermite    Lanczos
%    Mitchell Point      Quandratic
%    Sinc     Triangle
%
%  Most of the filters are FIR (finite impulse response), however, Bessel,
%  Gaussian, and Sinc are IIR (infinite impulse response).  Bessel and Sinc
%  are windowed (brought down to zero) with the Blackman filter.
%
%  The format of the MagickResizeImage method is:
%
%      unsigned int MagickResizeImage(MagickWand *wand,
%        const unsigned long columns,const unsigned long rows,
%        const FilterTypes filter,const double blur)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%    o filter: Image filter to use.
%
%    o blur: The blur factor where > 1 is blurry, < 1 is sharp.
%
%
*/
WandExport unsigned int MagickResizeImage(MagickWand *wand,
  const unsigned long columns,const unsigned long rows,const FilterTypes filter,
  const double blur)
{
  Image
    *resize_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  resize_image=ResizeImage(wand->image,columns,rows,filter,blur,
    &wand->exception);
  if (resize_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,resize_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R o t a t e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRotateImage() rotates an image the specified number of degrees.
%
%  The format of the MagickRotateImage method is:
%
%      unsigned int MagickRotateImage(MagickWand *wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o degrees: The number of degrees to rotate the image.
%
%
*/
WandExport unsigned int MagickRotateImage(MagickWand *wand,
  const double degrees)
{
  Image
    *rotate_image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  rotate_image=RotateImage(wand->image,degrees,&wand->exception);
  if (rotate_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,rotate_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t F u z z y C o l o r D i s t a n c e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetFuzzyColorDistance() colors within the specified RGB distance-
%  squared are treated by certain ImageMagick algorithms as the same color.
%
%  The format of the MagickSetFuzzyColorDistance method is:
%
%      void MagickSetFuzzyColorDistance(MagickWand *wand,const double distance)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o distance: RGB distance-squared.
%
*/
WandExport void MagickSetFuzzyColorDistance(MagickWand *wand,
  const double distance)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->fuzz=distance;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t F i l e n a m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetFilename() sets the image filename before you read or write an
%  image file.
%
%  The format of the MagickSetFilename method is:
%
%      void MagickSetFilename(MagickWand *wand,const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
*/
WandExport void MagickSetFilename(MagickWand *wand,const char *filename)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  (void) strncpy(wand->image_info->filename,filename,MaxTextExtent-1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImage() selects the image associated with a magick wand.
%
%  The format of the MagickSetImage method is:
%
%      const char MagickSetImage(MagickWand *wand,const unsigned long scene)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o scene: The scene number.
%
*/
WandExport unsigned int MagickSetImage(MagickWand *wand,
  const unsigned long scene)
{
  Image
    *image;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  image=GetImageFromList(wand->images,scene);
  if (image == (Image *) NULL)
    return(False);
  wand->image=image;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e F i l e n a m e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageFilename() sets the filename of a particular image in a
%  sequence.
%
%  The format of the MagickSetImageFilename method is:
%
%      unsigned int MagickSetImageFilename(MagickWand *wand,
%        const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
*/
WandExport unsigned int MagickSetImageFilename(MagickWand *wand,
  const char *filename)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  (void) strncpy(wand->image->filename,filename,MaxTextExtent-1);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e P i x e l s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImagePixels() accepts pixel data and stores it in the image at the
%  location you specify.  The method returns False on success otherwise True
%  if an error is encountered.  The pixel data can be either char, short int,
%  int, long, float, or double in the order specified by map.
%
%  Suppose your want want to upload the first scanline of a 640x480 image from
%  character data in red-green-blue order:
%
%      MagickSetImagePixels(wand,0,0,0,640,1,"RGB",CharPixel,pixels);
%
%  The format of the MagickSetImagePixels method is:
%
%      unsigned int MagickSetImagePixels(MagickWand *wand,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType storage,
%        void *pixels)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_offset, y_offset, columns, rows:  These values define the perimeter
%      of a region of pixels you want to define.
%
%    o map:  This string reflects the pixel data type and expected ordering of
%      the pixel array.  The first character of the map must be one of: "C" for
%      character, "S" for short, "I" for integer, "L" for long, "F" for float,
%      or "D" for double.  Note, Float and double types are normalized to
%      [0..1] otherwise [0..MaxRGB].  The remaining characters of the map
%      reflect the ordering of the pixel array.  It can be any combination or
%      order of R = red, G = green, B = blue, A = alpha, C = cyan, Y = yellow,
%      M = magenta, K = black, or I = intensity (for grayscale).
%
%    o storage: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%
*/
WandExport unsigned int MagickSetImagePixels(MagickWand *wand,
  const long x_offset,const long y_offset,const unsigned long columns,
  const unsigned long rows,const char *map,const StorageType storage,
  void *pixels)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  status=ImportImagePixels(wand->image,x_offset,y_offset,columns,rows,map,
    storage,pixels);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t S i z e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetSize() sets the size of the magick wand.  Set it before you
%  read a raw image format such as RGB, GRAY, or CMYK.
%
%  The format of the MagickSetSize method is:
%
%      void MagickSetSize(MagickWand *wand,const unsigned long columns,
%        const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o columns: The width in pixels.
%
%    o height: The height in pixels.
%
%
*/
WandExport void MagickSetSize(MagickWand *wand,const unsigned long columns,
  const unsigned long rows)
{
  char
    geometry[MaxTextExtent];

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  FormatString(geometry,"%lux%lu",columns,rows);
  (void) CloneString(&wand->image_info->size,geometry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W r i t e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWriteImage() writes an image.
%
%  The format of the MagickWriteImage method is:
%
%      unsigned int MagickWriteImage(MagickWand *wand,const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
%
*/
WandExport unsigned int MagickWriteImage(MagickWand *wand,
  const char *filename)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    return(False);
  (void) strncpy(wand->image->filename,filename,MaxTextExtent-1);
  return(WriteImage(wand->image_info,wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W r i t e I m a g e s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWriteImages() writes an image or image sequence.
%
%  The format of the MagickWriteImages method is:
%
%      unsigned int MagickWriteImages(MagickWand *wand,
%        const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
%
*/
WandExport unsigned int MagickWriteImages(MagickWand *wand,
  const char *filename)
{
  unsigned int
    status;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  (void) strncpy(wand->image_info->filename,filename,MaxTextExtent-1);
  status=WriteImages(wand->image_info,wand->images,
    wand->image_info->filename,&wand->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w M a g i c k W a n d                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewMagickWand() returns a wand required for all other methods in the API.
%
%  The format of the NewMagickWand method is:
%
%      MagickWand NewMagickWand(void)
%
%
*/
WandExport MagickWand *NewMagickWand(void)
{
  MagickWand
    *wand;

  wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (wand == (MagickWand *) NULL)
    return((MagickWand *) NULL);
  (void) memset(wand,0,sizeof(MagickWand));
  GetExceptionInfo(&wand->exception);
  wand->image_info=CloneImageInfo((ImageInfo *) NULL);
  wand->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  wand->images=NewImageList();
  wand->signature=MagickSignature;
  return(wand);
}
