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
#include "magick/blob.h"
#include "magick/error.h"
#include "magick/image.h"
#include "magick/list.h"
#include "magick/paint.h"
#include "magick/quantize.h"
#include "magick/resize.h"
#include "magick/resource.h"
#include "wand/magick_wand.h"
#include "wand/magick_compat.h"

/*
  Define declarations.
*/
#define ThrowWandException(code,reason,description) \
{ \
  ThrowException(&wand->exception,code,reason,description); \
  return(False); \
}


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
%   C l o n e M a g i c k W a n d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneMagickWand() makes a copy of the wand excepting the image list.
%
%  The format of the CloneMagickWand method is:
%
%      unsigned int CloneMagickWand(MagickWand *wand,Image *images)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o images: replace the image list with these image(s).
%
*/
static MagickWand *CloneMagickWand(const MagickWand *wand,Image *images)
{
  MagickWand
    *clone_wand;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  clone_wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImage);
  (void) memset(clone_wand,0,sizeof(MagickWand));
  GetExceptionInfo(&clone_wand->exception);
  InheritException(&clone_wand->exception,&wand->exception);
  clone_wand->image_info=CloneImageInfo(wand->image_info);
  clone_wand->quantize_info=CloneQuantizeInfo(wand->quantize_info);
  clone_wand->images=images;
  clone_wand->image=clone_wand->images;
  clone_wand->signature=MagickSignature;
  return(clone_wand);
}

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
%      void DestroyMagickWand(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%
*/
WandExport unsigned int DestroyMagickWand(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  DestroyQuantizeInfo(wand->quantize_info);
  DestroyImageInfo(wand->image_info);
  DestroyExceptionInfo(&wand->exception);
  if (wand->images != (Image *) NULL)
    DestroyImageList(wand->images);
  wand=(MagickWand *) RelinquishMagickMemory(wand);
  return(True);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
%        const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o drawing_wand: The draw wand.
%
*/
WandExport unsigned int MagickAffineTransformImage(MagickWand *wand,
  const DrawingWand *drawing_wand)
{
  DrawInfo
    *draw_info;

  Image
    *affine_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if (draw_info == (DrawInfo *) NULL)
    return(False);
  affine_image=AffineTransformImage(wand->image,&draw_info->affine,
    &wand->exception);
  DestroyDrawInfo(draw_info);
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
%        const DrawingWand *drawing_wand,const double x,const double y,
%        const char *text)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o drawing_wand: The draw wand.
%
%    o x: x ordinate to left of text
%
%    o y: y ordinate to text baseline
%
%    o text: text to draw
%
*/
WandExport unsigned int MagickAnnotateImage(MagickWand *wand,
  const DrawingWand *drawing_wand,const double x,const double y,
  const char *text)
{
  char
    geometry[MaxTextExtent];

  DrawInfo
    *draw_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if (draw_info == (DrawInfo *) NULL)
    return(False);
  CloneString(&draw_info->text,text);
  (void) FormatMagickString(geometry,MaxTextExtent,"%+g%+g",x,y);
  CloneString(&draw_info->geometry,geometry);
  status=AnnotateImage(wand->image,draw_info);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  DestroyDrawInfo(draw_info);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->server_name=(char *) AcquireString(server_name);
  status=AnimateImages(wand->image_info,wand->images);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A p p e n d I m a g e s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAppendImages() append a set of images.
%
%  The format of the MagickAppendImages method is:
%
%      MagickWand *MagickAppendImages(MagickWand *wand,
%        const unsigned int stack)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o stack: By default, images are stacked left-to-right. Set stack to True
%      to stack them top-to-bottom.
%
*/
WandExport MagickWand *MagickAppendImages(MagickWand *wand,
  const unsigned int stack)
{
  Image
    *append_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  append_image=AppendImages(wand->images,stack,&wand->exception);
  if (append_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,append_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A v e r a g e I m a g e s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAverageImages() average a set of images.
%
%  The format of the MagickAverageImages method is:
%
%      MagickWand *MagickAverageImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickWand *MagickAverageImages(MagickWand *wand)
{
  Image
    *average_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  average_image=AverageImages(wand->images,&wand->exception);
  if (average_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,average_image));
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
WandExport unsigned int MagickBlackThresholdImage(MagickWand *wand,
  const PixelWand *threshold)
{
#if defined(NOT_IMPLEMENTED)
  char
    thresholds[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) FormatMagickString(thresholds,MaxTextExtent,"%u,%u,%u,%u",
    PixelGetRedQuantum(threshold),PixelGetGreenQuantum(threshold),
    PixelGetBlueQuantum(threshold),PixelGetOpacityQuantum(threshold));
  status=BlackThresholdImage(wand->image,thresholds);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
}

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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
    border_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  border_info.width=width;
  border_info.height=height;
  border_info.x=0;
  border_info.y=0;
  PixelGetQuantumColor(bordercolor,&wand->image->border_color);
  border_image=BorderImage(wand->image,&border_info,&wand->exception);
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
%      BlackChannel, or IndexChannel.
%
*/
WandExport unsigned int MagickChannelImage(MagickWand *wand,
  const ChannelType channel)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ChannelImage(wand->image,channel);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
%  MagickCloneWand() makes an exact copy of the specified wand.
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  clone_wand=(MagickWand *) AcquireMagickMemory(sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImage);
  (void) memset(clone_wand,0,sizeof(MagickWand));
  GetExceptionInfo(&clone_wand->exception);
  InheritException(&clone_wand->exception,&wand->exception);
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
%  MagickClipImage() clips along the first path from the 8BIM profile, if
%  present.
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
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ClipImage(wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C l i p P a t h I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickClipPathImage() clips along the named paths from the 8BIM profile, if
%  present. Later operations take effect inside the path.  Id may be a number
%  if preceded with #, to work on a numbered path, e.g., "#1" to use the first
%  path.
%
%  The format of the MagickClipPathImage method is:
%
%      unsigned int MagickClipPathImage(MagickWand *wand,const char *pathname,
%        const unsigned int inside)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o pathname: name of clipping path resource. If name is preceded by #, use
%      clipping path numbered by name.
%
%    o inside: if non-zero, later operations take effect inside clipping path.
%      Otherwise later operations take effect outside clipping path.
%
*/
WandExport unsigned int MagickClipPathImage(MagickWand *wand,
  const char *pathname,const unsigned int inside)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ClipPathImage(wand->image,pathname,inside);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  coalesce_image=CoalesceImages(wand->images,&wand->exception);
  if (coalesce_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,coalesce_image));
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
%  The format of the MagickColorFloodfillImage method is:
%
%      unsigned int MagickColorFloodfillImage(MagickWand *wand,
%        const PixelWand *fill,const double fuzz,const PixelWand *bordercolor,
%        const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fill: The floodfill color pixel wand.
%
%    o fuzz: By default target must match a particular pixel color
%      exactly.  However, in many cases two colors may differ by a small amount.
%      The fuzz member of image defines how much tolerance is acceptable to
%      consider two colors as the same.  For example, set fuzz to 10 and the
%      color red at intensities of 100 and 102 respectively are now interpreted
%      as the same color for the purposes of the floodfill.
%
%    o bordercolor: The border color pixel wand.
%
%    o x,y: The starting location of the operation.
%
*/
WandExport unsigned int MagickColorFloodfillImage(MagickWand *wand,
  const PixelWand *fill,const double fuzz,const PixelWand *bordercolor,
  const long x,const long y)
{
  DrawInfo
    *draw_info;

  PixelPacket
    target;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  draw_info=CloneDrawInfo(wand->image_info,(DrawInfo *) NULL);
  PixelGetQuantumColor(fill,&draw_info->fill);
  target=AcquireOnePixel(wand->image,x % wand->image->columns,
    y % wand->image->rows,&wand->exception);
  if (bordercolor != (PixelWand *) NULL)
    PixelGetQuantumColor(bordercolor,&target);
  wand->image->fuzz=fuzz;
  status=ColorFloodfillImage(wand->image,draw_info,target,x,y,
    bordercolor != (PixelWand *) NULL ? FillToBorderMethod : FloodfillMethod);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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
%      unsigned int MagickColorizeImage(MagickWand *wand,
%        const PixelWand *colorize,const PixelWand *opacity)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o colorize: The colorize pixel wand.
%
%    o opacity: The opacity pixel wand.
%
*/
WandExport unsigned int MagickColorizeImage(MagickWand *wand,
  const PixelWand *colorize,const PixelWand *opacity)
{
  char
    percent_opaque[MaxTextExtent];

  Image
    *colorize_image;

  PixelPacket
    target;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) FormatMagickString(percent_opaque,MaxTextExtent,"%g,%g,%g,%g",
    100.0*PixelGetRedQuantum(colorize)/MaxRGB,
    100.0*PixelGetGreenQuantum(colorize)/MaxRGB,
    100.0*PixelGetBlueQuantum(colorize)/MaxRGB,
    100.0*PixelGetOpacityQuantum(colorize)/MaxRGB);
  PixelGetQuantumColor(colorize,&target);
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
%    o comment: The image comment.
%
*/
WandExport unsigned int MagickCommentImage(MagickWand *wand,const char *comment)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) SetImageAttribute(wand->image,"comment",(char *) NULL);
  status=SetImageAttribute(wand->image,"comment",comment);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) || (reference->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=IsImagesEqual(wand->image,reference->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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
%        const MagickWand *composite_wand,const CompositeOperator compose,
%        const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o composite_image: The composite image.
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
%    o x_offset: The column offset of the composited image.
%
%    o y_offset: The row offset of the composited image.
%
%
*/
WandExport unsigned int MagickCompositeImage(MagickWand *wand,
  const MagickWand *composite_wand,const CompositeOperator compose,const long x,
  const long y)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (composite_wand->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=CompositeImage(wand->image,compose,composite_wand->image,x,y);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ContrastImage(wand->image,sharpen);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (kernel == (const double *) NULL)
    return(False);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=CycleColormapImage(wand->image,displace);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D e c o n s t r u c t I m a g e                               %
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  deconstruct_image=DeconstructImages(wand->images,&wand->exception);
  if (deconstruct_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,deconstruct_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D e s c r b e I m a g e                                       %
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
%      const char *MagickDescribeImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport char *MagickDescribeImage(MagickWand *wand)
{
  char
    *description,
    filename[MaxTextExtent];

  FILE
    *file;

  int
    unique_file;

  size_t
    length;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  description=(char *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  file=(FILE *) NULL;
  if (unique_file != -1)
    file=fdopen(unique_file,"wb");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      ThrowMagickException(&wand->exception,FileOpenError,
                           UnableToCreateTemporaryFile,filename);
    }
  else
    {
      (void) DescribeImage(wand->image,file,True);
      (void) fclose(file);
      description=(char *) FileToBlob(filename,&length,&wand->exception);
    }
  (void) RelinquishUniqueFileResource(filename);
  return(description);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (image == (Image *) NULL)
    return(False);
  wand->image_info->server_name=(char *) AcquireString(server_name);
  status=DisplayImages(wand->image_info,image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  CloneString(&wand->image_info->server_name,server_name);
  status=DisplayImages(wand->image_info,wand->images);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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
%        const DrawingWand *drawing_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o drawing_wand: The draw wand.
%
*/
WandExport unsigned int MagickDrawImage(MagickWand *wand,
  const DrawingWand *drawing_wand)
{
  DrawInfo
    *draw_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if ((draw_info == (DrawInfo *) NULL) ||
      (draw_info->primitive == (char *) NULL))
    return(False);
  status=DrawImage(wand->image,draw_info);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  DestroyDrawInfo(draw_info);
  return(status);
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
WandExport unsigned int MagickEdgeImage(MagickWand *wand,const double radius)
{
  Image
    *edge_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
WandExport unsigned int MagickEmbossImage(MagickWand *wand,const double radius,
  const double sigma)
{
  Image
    *emboss_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=EqualizeImage(wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k F l a t t e n I m a g e s                                     %
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  flatten_image=FlattenImages(wand->images,&wand->exception);
  if (flatten_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,flatten_image));
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
%  image.  The width and height specify the border width of the vertical and
%  horizontal sides of the frame.  The inner and outer bevels indicate the
%  width of the inner and outer shadows of the frame.
%
%  The format of the MagickFrameImage method is:
%
%      unsigned int MagickFrameImage(MagickWand *wand,
%        const PixelWand *matte_color,const unsigned long width,
%        const unsigned long height,const long inner_bevel,
%        const long outer_bevel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o matte_color: The frame color pixel wand.
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
  const PixelWand *matte_color,const unsigned long width,
  const unsigned long height,const long inner_bevel,const long outer_bevel)
{
  Image
    *frame_image;

  FrameInfo
    frame_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) memset(&frame_info,0,sizeof(FrameInfo));
  frame_info.width=wand->image->columns+2*width;
  frame_info.height=wand->image->rows+2*height;
  frame_info.x=(long) width;
  frame_info.y=(long) height;
  frame_info.inner_bevel=inner_bevel;
  frame_info.outer_bevel=outer_bevel;
  PixelGetQuantumColor(matte_color,&wand->image->matte_color);
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
%   M a g i c k F x I m a g e C h a n n e l                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFxImage() evaluate expression for each pixel in the specified channel.
%
%  The format of the MagickFxImage method is:
%
%      unsigned int MagickFxImage(MagickWand *wand,const MagickWand fx_wand,
%        const ChannelType channel,const char *expression)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fx_wand: The F(X) wand.
%
%    o channel: Identify which channel to level: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel, or IndexChannel.
%
%    o expression: The expression.
%
*/
WandExport unsigned int MagickFxImageChannel(MagickWand *wand,
  const MagickWand *fx_wand,const ChannelType channel,const char *expression)
{
#if defined(NOT_IMPLEMENTED)
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (fx_wand->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=FxImageChannel(wand->image,fx_wand->image,channel,expression);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G a m m a I m a g e                                           %
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
%      unsigned int MagickGammaImage(MagickWand *wand,const double gamma)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o gamme: Define the level of gamma correction.
%
*/
WandExport unsigned int MagickGammaImage(MagickWand *wand,const double gamma)
{
#if defined(NOT_IMPLEMENTED)
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=GammaImageChannel(wand->image,AllChannels,gamma);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G a m m a I m a g e C h a n n e l                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use MagickGammaImageChannel() to gamma-correct a particular image channel.
%  The same image viewed on different devices will have perceptual differences
%  in the way the image's intensities are represented on the screen.  Specify
%  individual gamma levels for the red, green, and blue channels, or adjust all
%  three with the gamma parameter.  Values typically range from 0.8 to 2.3.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the MagickGammaImageChannel method is:
%
%      unsigned int MagickGammaImageChannel(MagickWand *wand,
%        const ChannelType channel,const double gamma)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: The channel.
%
%    o level: Define the level of gamma correction.
%
*/
WandExport unsigned int MagickGammaImageChannel(MagickWand *wand,
  const ChannelType channel,const double gamma)
{
#if defined(NOT_IMPLEMENTED)
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=GammaImageChannel(wand->image,channel,gamma);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
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
%      char *MagickGetException(const MagickWand *wand,ExceptionType *severity)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o severity: The severity of the error is returned here.
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

WandExport char *MagickGetException(const MagickWand *wand,
  ExceptionType *severity)
{
  char
    *description;

  assert(wand != (const MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(severity != (ExceptionType *) NULL);
  *severity=wand->exception.severity;
  description=(char *) AcquireMagickMemory(2*MaxTextExtent);
  if (description == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateString);
  *description='\0';
  if (wand->exception.reason != (char *) NULL)
    (void) CopyMagickString(description,GetLocaleExceptionMessage(
      wand->exception.severity,wand->exception.reason),MaxTextExtent);
  if (wand->exception.description != (char *) NULL)
    {
      (void) ConcatenateMagickString(description," (",MaxTextExtent);
      (void) ConcatenateMagickString(description,GetLocaleExceptionMessage(
        wand->exception.severity,wand->exception.description),MaxTextExtent);
      (void) ConcatenateMagickString(description,")",MaxTextExtent);
    }
  if ((wand->exception.severity != OptionError) && errno)
    {
      (void) ConcatenateMagickString(description," [",MaxTextExtent);
      (void) ConcatenateMagickString(description,GetErrorMessageString(errno),
        MaxTextExtent);
      (void) ConcatenateMagickString(description,"]",MaxTextExtent);
    }
  return(description);
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
%      const char *MagickGetFilename(const MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%
*/
WandExport char *MagickGetFilename(const MagickWand *wand)
{
  assert(wand != (const MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(AcquireString(wand->image_info->filename));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e B a c k g o u n d C o l o r                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageBackgroundColor() returns the image background color.
%
%  The format of the MagickGetImageBackgroundColor method is:
%
%      unsigned int MagickGetImageBackgroundColor(MagickWand *wand,
%        PixelWand *background_color)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o background_color: Return the background color.
%
*/
WandExport unsigned int MagickGetImageBackgroundColor(MagickWand *wand,
  PixelWand *background_color)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelSetQuantumColor(background_color,&wand->image->background_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e B l u e P r i m a r y                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageBluePrimary() returns the chromaticy blue primary point for the
%  image.
%
%  The format of the MagickGetImageBluePrimary method is:
%
%      unsigned int MagickGetImageBluePrimary(MagickWand *wand,double *x,
%        double *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The chromaticity blue primary x-point.
%
%    o y: The chromaticity blue primary y-point.
%
*/
WandExport unsigned int MagickGetImageBluePrimary(MagickWand *wand,double *x,
  double *y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  *x=wand->image->chromaticity.blue_primary.x;
  *y=wand->image->chromaticity.blue_primary.y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e B o r d e r C o l o r                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageBorderColor() returns the image border color.
%
%  The format of the MagickGetImageBorderColor method is:
%
%      unsigned int MagickGetImageBorderColor(MagickWand *wand,
%        PixelWand *border_color)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o border_color: Return the border color.
%
*/
WandExport unsigned int MagickGetImageBorderColor(MagickWand *wand,
  PixelWand *border_color)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelSetQuantumColor(border_color,&wand->image->border_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C o l o r m a p C o l o r                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageColormapColor() returns the color of the specified colormap
%  index.
%
%  The format of the MagickGetImageColormapColor method is:
%
%      unsigned int MagickGetImageColormapColor(MagickWand *wand,
%        const unsigned long index,PixelWand *color)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o index: The offset into the image colormap.
%
%    o color: Return the colormap color in this wand.
%
*/
WandExport unsigned int MagickGetImageColormapColor(MagickWand *wand,
  const unsigned long index,PixelWand *color)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  if ((wand->image->colormap == (PixelPacket *) NULL) ||
      (index >= wand->image->colors))
    ThrowWandException(ImageError,InvalidColormapIndex,(char *) NULL);
  PixelSetQuantumColor(color,wand->image->colormap+index);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C o l o r s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageColors() gets the number of unique colors in the image.
%
%  The format of the MagickGetImageColors method is:
%
%      unsigned long MagickGetImageColors(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageColors(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(GetNumberColors(wand->image,(FILE *) NULL,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C o m p o s e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageCompose() returns the composite operator associated with the
%  image.
%
%  The format of the MagickGetImageCompose method is:
%
%      CompositeOperator MagickGetImageCompose(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport CompositeOperator MagickGetImageCompose(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->compose);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C o m p r e s s i o n                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageCompression() gets the image compression.
%
%  The format of the MagickGetImageCompression method is:
%
%      CompressionType MagickGetImageCompression(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport CompressionType MagickGetImageCompression(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->compression);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e D e l a y                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageDelay() gets the image delay.
%
%  The format of the MagickGetImageDelay method is:
%
%      unsigned long MagickGetImageDelay(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageDelay(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->delay);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e D e p t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageChannelDepth() gets the depth for a particular image channel.
%
%  The format of the MagickGetImageChannelDepth method is:
%
%      unsigned long MagickGetImageChannelDepth(MagickWand *wand,
%        const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel, or IndexChannel.
%
*/
WandExport unsigned long MagickGetImageChannelDepth(MagickWand *wand,
  const ChannelType channel)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(GetImageChannelDepth(wand->image,channel,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e D e p t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageDepth() gets the image depth.
%
%  The format of the MagickGetImageDepth method is:
%
%      unsigned long MagickGetImageDepth(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageDepth(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(GetImageDepth(wand->image,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e H e i g h t                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageHeight() returns the image height.
%
%  The format of the MagickGetImageHeight method is:
%
%      unsigned long MagickGetImageHeight(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageHeight(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(wand->image->rows);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e G a m m a                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageGamma() gets the image gamma.
%
%  The format of the MagickGetImageGamma method is:
%
%      double MagickGetImageGamma(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport double MagickGetImageGamma(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->gamma);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e G r e e n P r i m a r y                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageGreenPrimary() returns the chromaticy green primary point.
%
%  The format of the MagickGetImageGreenPrimary method is:
%
%      unsigned int MagickGetImageGreenPrimary(MagickWand *wand,double *x,
%        double *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The chromaticity green primary x-point.
%
%    o y: The chromaticity green primary y-point.
%
*/
WandExport unsigned int MagickGetImageGreenPrimary(MagickWand *wand,double *x,
  double *y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  *x=wand->image->chromaticity.green_primary.x;
  *y=wand->image->chromaticity.green_primary.y;
  return(True);
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
WandExport char *MagickGetImageFilename(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(AcquireString(wand->image->filename));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e F o r m a t                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageFormat() returns the format of a particular image in a
%  sequence.
%
%  The format of the MagickGetImageFormat method is:
%
%      const char MagickGetImageFormat(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport char *MagickGetImageFormat(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(AcquireString(wand->image->magick));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e M a t t e C o l o r                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageMatteColor() returns the image matte color.
%
%  The format of the MagickGetImageMatteColor method is:
%
%      unsigned int MagickGetImagematteColor(MagickWand *wand,
%        PixelWand *matte_color)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o matte_color: Return the matte color.
%
*/
WandExport unsigned int MagickGetImageMatteColor(MagickWand *wand,
  PixelWand *matte_color)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelSetQuantumColor(matte_color,&wand->image->matte_color);
  return(True);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ExportImagePixels(wand->image,x_offset,y_offset,columns,rows,map,
    storage,pixels,&wand->exception);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e R e d P r i m a r y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageRedPrimary() returns the chromaticy red primary point.
%
%  The format of the MagickGetImageRedPrimary method is:
%
%      unsigned int MagickGetImageRedPrimary(MagickWand *wand,double *x,
%        double *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The chromaticity red primary x-point.
%
%    o y: The chromaticity red primary y-point.
%
*/
WandExport unsigned int MagickGetImageRedPrimary(MagickWand *wand,double *x,
  double *y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  *x=wand->image->chromaticity.red_primary.x;
  *y=wand->image->chromaticity.red_primary.y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e R e n d e r i n g I n t e n t                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageRenderingIntent() gets the image rendering intent.
%
%  The format of the MagickGetImageRenderingIntent method is:
%
%      RenderingIntent MagickGetImageRenderingIntent(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport RenderingIntent MagickGetImageRenderingIntent(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->rendering_intent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e S c e n e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageScene() gets the image scene.
%
%  The format of the MagickGetImageScene method is:
%
%      unsigned long MagickGetImageScene(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageScene(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->scene);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e S i g n a t u r e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageSignature() generates an SHA-256 message digest for the image
%  pixel stream.
%
%  The format of the MagickGetImageSignature method is:
%
%      const char MagickGetImageSignature(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport char *MagickGetImageSignature(MagickWand *wand)
{
  const ImageAttribute
    *attribute;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=SignatureImage(wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  attribute=GetImageAttribute(wand->image,"signature");
  if (attribute != (ImageAttribute *) NULL)
    return(AcquireString(attribute->value));
  InheritException(&wand->exception,&wand->image->exception);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e S i z e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageSize() returns the image size.
%
%  The format of the MagickGetImageSize method is:
%
%      ExtendedSignedIntegralType MagickGetImageSize(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport ExtendedSignedIntegralType MagickGetImageSize(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(GetBlobSize(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e U n i t s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageUnits() gets the image units of resolution.
%
%  The format of the MagickGetImageUnits method is:
%
%      ResolutionType MagickGetImageUnits(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport ResolutionType MagickGetImageUnits(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->units);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e W h i t e P o i n t                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageWhitePoint() returns the chromaticy white point.
%
%  The format of the MagickGetImageWhitePoint method is:
%
%      unsigned int MagickGetImageWhitePoint(MagickWand *wand,double *x,
%        double *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The chromaticity white x-point.
%
%    o y: The chromaticity white y-point.
%
*/
WandExport unsigned int MagickGetImageWhitePoint(MagickWand *wand,double *x,
  double *y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  *x=wand->image->chromaticity.white_point.x;
  *y=wand->image->chromaticity.white_point.y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e W i d t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageWidth() returns the image width.
%
%  The format of the MagickGetImageWidth method is:
%
%      unsigned long MagickGetImageWidth(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageWidth(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(wand->image->columns);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e X R e s o l u t i o n                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageXResolution() gets the image X resolution.
%
%  The format of the MagickGetImageXResolution method is:
%
%      double MagickGetImageXResolution(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport double MagickGetImageXResolution(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->x_resolution);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e Y R e s o l u t i o n                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageYResolution() gets the image Y resolution.
%
%  The format of the MagickGetImageYResolution method is:
%
%      double MagickGetImageYResolution(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport double MagickGetImageYResolution(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowMagickException(&wand->exception,ImageError,NoImagesWereFound,
      (char *) NULL);
  return(wand->image->y_resolution);
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
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(GetImageListLength(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t R e s o u r c e L i m i t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetResourceLimit() returns the the specified resource in megabytes.
%
%  The format of the MagickGetResourceLimit method is:
%
%      unsigned long MagickGetResourceLimit(const ResourceType type)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetResourceLimit(const ResourceType type)
{
  return(GetMagickResource(type));
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
%      unsigned int MagickGetSize(const MagickWand *wand,unsigned long *columns,
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
WandExport unsigned int MagickGetSize(const MagickWand *wand,
  unsigned long *columns,unsigned long *rows)
{
  RectangleInfo
    geometry;

  assert(wand != (const MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) memset(&geometry,0,sizeof(RectangleInfo));
  (void) ParseAbsoluteGeometry(wand->image_info->size,&geometry);
  *columns=geometry.width;
  *rows=geometry.height;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k I m p l o d e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickImplodeImage() creates a new image that is a copy of an existing
%  one with the image pixels "implode" by the specified percentage.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the MagickImplodeImage method is:
%
%      unsigned int MagickImplodeImage(MagickWand *wand,const double radius)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o amount: Define the extent of the implosion.
%
*/
WandExport unsigned int MagickImplodeImage(MagickWand *wand,const double amount)
{
  Image
    *implode_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  implode_image=ImplodeImage(wand->image,amount,&wand->exception);
  if (implode_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,implode_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k L a b e l I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickLabelImage() adds a label to your image.
%
%  The format of the MagickLabelImage method is:
%
%      unsigned int MagickLabelImage(MagickWand *wand,const char *label)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o label: The image label.
%
*/
WandExport unsigned int MagickLabelImage(MagickWand *wand,const char *label)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) SetImageAttribute(wand->image,"label",(char *) NULL);
  status=SetImageAttribute(wand->image,"label",label);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k L e v e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickLevelImage() adjusts the levels of an image by scaling the colors
%  falling between specified white and black points to the full available
%  quantum range. The parameters provided represent the black, mid, and white
%  points. The black point specifies the darkest color in the image. Colors
%  darker than the black point are set to zero. Mid point specifies a gamma
%  correction to apply to the image.  White point specifies the lightest color
%  in the image. Colors brighter than the white point are set to the maximum
%  quantum value.
%
%  The format of the MagickLevelImage method is:
%
%      unsigned int MagickLevelImage(MagickWand *wand,const double black_point,
%        const double gamma,const double white_point)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o black_point: The black point.
%
%    o gamma: The gamma.
%
%    o white_point: The white point.
%
*/
WandExport unsigned int MagickLevelImage(MagickWand *wand,
  const double black_point,const double gamma,const double white_point)
{
  char
    levels[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) FormatMagickString(levels,MaxTextExtent,"%g,%g,%g",black_point,
    white_point,gamma);
  status=LevelImage(wand->image,levels);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k L e v e I m a g e C h a n n e l                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickLevelImage() adjusts the levels of the specified channel of the
%  reference image by scaling the colors falling between specified white and
%  black points to the full available quantum range. The parameters provided
%  represent the black, mid, and white points. The black point specifies the
%  darkest color in the image. Colors darker than the black point are set to
%  zero.  Mid point specifies a gamma correction to apply to the image.  White
%  point specifies the lightest color in the image. Colors brighter than the
%  white point are set to the maximum quantum value.
%
%  The format of the MagickLevelImage method is:
%
%      unsigned int MagickLevelImage(MagickWand *wand,const ChannelType channel,
%        const double black_point,const double gamma,const double white_point)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to level: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel, or IndexChannel.
%
%    o black_point: The black point.
%
%    o gamma: The gamma.
%
%    o white_point: The white point.
%
*/
WandExport unsigned int MagickLevelImageChannel(MagickWand *wand,
  const ChannelType channel,const double black_point,const double gamma,
  const double white_point)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=LevelImageChannel(wand->image,channel,black_point,white_point,gamma);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M a g n i f y I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMagnifyImage() is a convenience method that scales an image
%  proportionally to twice its original size.
%
%  The format of the MagickMagnifyImage method is:
%
%      unsigned int MagickMagnifyImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickMagnifyImage(MagickWand *wand)
{
  Image
    *magnify_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  magnify_image=MagnifyImage(wand->image,&wand->exception);
  if (magnify_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,magnify_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M a p I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapImage() replaces the colors of an image with the closest color
%  from a reference image.
%
%  The format of the MagickMapImage method is:
%
%      unsigned int MagickMapImage(MagickWand *wand,const MagickWand *map_wand,
%        const unsigned int dither)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o map: The map wand.
%
%    o dither: Set this integer value to something other than zero to dither
%      the mapped image.
%
*/
WandExport unsigned int MagickMapImage(MagickWand *wand,
  const MagickWand *map_wand,const unsigned int dither)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) || (map_wand->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=MapImage(wand->image,map_wand->image,dither);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M a t t e F l o o d f i l l I m a g e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMatteFloodfillImage() changes the transparency value of any pixel that
%  matches target and is an immediate neighbor.  If the method
%  FillToBorderMethod is specified, the transparency value is changed for any
%  neighbor pixel that does not match the bordercolor member of image.
%
%  The format of the MagickMatteFloodfillImage method is:
%
%      unsigned int MagickMatteFloodfillImage(MagickWand *wand,
%        const Quantum opacity,const double fuzz,const PixelWand *bordercolor,
%        const long x,const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o opacity: The opacity.
%
%    o fuzz: By default target must match a particular pixel color
%      exactly.  However, in many cases two colors may differ by a small amount.
%      The fuzz member of image defines how much tolerance is acceptable to
%      consider two colors as the same.  For example, set fuzz to 10 and the
%      color red at intensities of 100 and 102 respectively are now interpreted
%      as the same color for the purposes of the floodfill.
%
%    o bordercolor: The border color pixel wand.
%
%    o x,y: The starting location of the operation.
%
*/
WandExport unsigned int MagickMatteFloodfillImage(MagickWand *wand,
  const Quantum opacity,const double fuzz,const PixelWand *bordercolor,
  const long x,const long y)
{
  DrawInfo
    *draw_info;

  PixelPacket
    target;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  draw_info=CloneDrawInfo(wand->image_info,(DrawInfo *) NULL);
  target=AcquireOnePixel(wand->image,x % wand->image->columns,
    y % wand->image->rows,&wand->exception);
  if (bordercolor != (PixelWand *) NULL)
    PixelGetQuantumColor(bordercolor,&target);
  status=MatteFloodfillImage(wand->image,target,opacity,x,y,
    bordercolor != (PixelWand *) NULL ? FillToBorderMethod : FloodfillMethod);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  DestroyDrawInfo(draw_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M e d i a n F i l t e r I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMedianFilterImage() applies a digital filter that improves the quality
%  of a noisy image.  Each pixel is replaced by the median in a set of
%  neighboring pixels as defined by radius.
%
%  The format of the MagickMedianFilterImage method is:
%
%      unsigned int MagickMedianFilterImage(MagickWand *wand,
%        const double radius)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the pixel neighborhood.
%
*/
WandExport unsigned int MagickMedianFilterImage(MagickWand *wand,
  const double radius)
{
  Image
    *median_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  median_image=MedianFilterImage(wand->image,radius,&wand->exception);
  if (median_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,median_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M i n i f y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMinifyImage() is a convenience method that scales an image
%  proportionally to one-half its original size
%
%  The format of the MagickMinifyImage method is:
%
%      unsigned int MagickMinifyImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickMinifyImage(MagickWand *wand)
{
  Image
    *minify_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  minify_image=MinifyImage(wand->image,&wand->exception);
  if (minify_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,minify_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o d u l a t e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickModulateImage() lets you control the brightness, saturation, and hue
%  of an image.
%
%  The format of the MagickModulateImage method is:
%
%      unsigned int MagickModulateImage(MagickWand *wand,
%        const double brightness,const double saturation,const double hue)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o brightness: The percent change in brighness (-100 thru +100).
%
%    o saturation: The percent change in saturation (-100 thru +100)
%
%    o hue: The percent change in hue (-100 thru +100)
%
*/
WandExport unsigned int MagickModulateImage(MagickWand *wand,
  const double brightness,const double saturation,const double hue)
{
  char
    modulate[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) FormatMagickString(modulate,MaxTextExtent,"%g,%g,%g",brightness,
    saturation,hue);
  status=ModulateImage(wand->image,modulate);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o n t a g e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use MagickMontageImage() to create a composite image by combining several
%  separate images. The images are tiled on the composite image with the name
%  of the image optionally appearing just below the individual tile.
%
%  The format of the MagickMontageImage method is:
%
%      MagickWand MagickMontageImage(MagickWand *wand,
%        const DrawingWand drawing_wand,const char *tile_geometry,
%        const char *thumbnail_geometry,const MontageMode mode,
%        const char *frame)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o drawing_wand: The drawing wand.  The font name, size, and color are
%      obtained from this wand.
%
%    o tile_geometry: the number of tiles per row and page (e.g. 6x4+0+0).
%
%    o thumbnail_geometry: Preferred image size and border size of each
%      thumbnail (e.g. 120x120+4+3>).
%
%    o mode: Thumbnail framing mode: Frame, Unframe, or Concatenate.
%
%    o frame: Surround the image with an ornamental border (e.g. 15x15+3+3).
%      The frame color is that of the thumbnail's matte color.
%
*/
WandExport MagickWand *MagickMontageImage(MagickWand *wand,
  const DrawingWand *drawing_wand,const char *tile_geometry,
  const char *thumbnail_geometry,const MontageMode mode,const char *frame)
{
  char
    *font;

  Image
    *montage_image;

  MontageInfo
    *montage_info;

  PixelWand
    *pixel_wand;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  montage_info=CloneMontageInfo(wand->image_info,(MontageInfo *) NULL);
  switch (mode)
  {
    case FrameMode:
    {
      (void) CloneString(&montage_info->frame,"15x15+3+3");
      montage_info->shadow=True;
      break;
    }
    case UnframeMode:
    {
      montage_info->frame=(char *) NULL;
      montage_info->shadow=False;
      montage_info->border_width=0;
      break;
    }
    case ConcatenateMode:
    {
      montage_info->frame=(char *) NULL;
      montage_info->shadow=False;
      (void) CloneString(&montage_info->geometry,"+0+0");
      montage_info->border_width=0;
      break;
    }
    default:
      break;
  }
  font=DrawGetFont(drawing_wand);
  if (font != (char *) NULL)
    (void) CloneString(&montage_info->font,font);
  if (frame != (char *) NULL)
    (void) CloneString(&montage_info->frame,frame);
  montage_info->pointsize=DrawGetFontSize(drawing_wand);
  pixel_wand=NewPixelWand();
  DrawGetFillColor(drawing_wand,pixel_wand);
  PixelGetQuantumColor(pixel_wand,&montage_info->fill);
  DrawGetStrokeColor(drawing_wand,pixel_wand);
  PixelGetQuantumColor(pixel_wand,&montage_info->stroke);
  DestroyPixelWand(pixel_wand);
  if (thumbnail_geometry != (char *) NULL)
    (void) CloneString(&montage_info->geometry,thumbnail_geometry);
  if (tile_geometry != (char *) NULL)
    (void) CloneString(&montage_info->tile,tile_geometry);
  montage_image=MontageImages(wand->images,montage_info,&wand->exception);
  DestroyMontageInfo(montage_info);
  if (montage_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,montage_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o r p h I m a g e s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMorphImages() method morphs a set of images.  Both the image pixels
%  and size are linearly interpolated to give the appearance of a
%  meta-morphosis from one image to the next.
%
%  The format of the MagickMorphImages method is:
%
%      MagickWand *MagickMorphImages(MagickWand *wand,
%        const unsigned long number_frames)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o number_frames: The number of in-between images to generate.
%
*/
WandExport MagickWand *MagickMorphImages(MagickWand *wand,
  const unsigned long number_frames)
{
  Image
    *morph_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  morph_image=MorphImages(wand->images,number_frames,&wand->exception);
  if (morph_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,morph_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o r p h I m a g e s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMosaicImages() inlays an image sequence to form a single coherent
%  picture.  It returns a wand with each image in the sequence composited at
%  the location defined by the page offset of the image.
%
%  The format of the MagickMosaicImages method is:
%
%      MagickWand *MagickMosaicImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickWand *MagickMosaicImages(MagickWand *wand)
{
  Image
    *mosaic_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  mosaic_image=MosaicImages(wand->images,&wand->exception);
  if (mosaic_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,mosaic_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o t i o n B l u r I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMotionBlurImage() simulates motion blur.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and MotionBlurImage() selects a suitable radius for you.
%  Angle gives the angle of the blurring motion.
%
%  The format of the MagickMotionBlurImage method is:
%
%      unsigned int MagickMotionBlurImage(MagickWand *wand,
%        const double radius,const double sigma,const double angle)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the Gaussian, in pixels, not counting
%      the center pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o angle: Apply the effect along this angle.
%
*/
WandExport unsigned int MagickMotionBlurImage(MagickWand *wand,
  const double radius,const double sigma,const double angle)
{
  Image
    *blur_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  blur_image=MotionBlurImage(wand->image,radius,sigma,angle,&wand->exception);
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
%   M a g i c k N e g a t e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickNegateImage() negates the colors in the reference image.  The
%  Grayscale option means that only grayscale values within the image are
%  negated.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the MagickNegateImage method is:
%
%      unsigned int MagickNegateImage(MagickWand *wand,const unsigned int gray)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o gray: If True, only negate grayscale pixels within the image.
%
*/
WandExport unsigned int MagickNegateImage(MagickWand *wand,
  const unsigned int gray)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=NegateImage(wand->image,gray);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k N e g a t e I m a g e C h a n n e l                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickNegateImage() negates the colors in the specified channel of the
%  reference image.  The Grayscale option means that only grayscale values
%  within the image are negated.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the MagickNegateImage method is:
%
%      unsigned int MagickNegateImage(MagickWand *wand,
%        const ChannelType channel,const unsigned int gray)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel, or IndexChannel.
%
%    o gray: If True, only negate grayscale pixels within the image.
%
*/
WandExport unsigned int MagickNegateImageChannel(MagickWand *wand,
  const ChannelType channel,const unsigned int gray)
{
#if defined(NOT_IMPLEMENTED)
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=NegateImageChannel(wand->image,channel,gray);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
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
%      unsigned int MagickNextImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickNextImage(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) || (wand->image->next == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image=wand->image->next;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k N o r m a l i z e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickNormalizeImage() enhances the contrast of a color image by adjusting
%  the pixels color to span the entire range of colors available
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the MagickNormalizeImage method is:
%
%      unsigned int MagickNormalizeImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickNormalizeImage(MagickWand *wand)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=NormalizeImage(wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k O i l P a i n t I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickOilPaintImage() applies a special effect filter that simulates an oil
%  painting.  Each pixel is replaced by the most frequent color occurring
%  in a circular region defined by radius.
%
%  The format of the MagickOilPaintImage method is:
%
%      unsigned int MagickOilPaintImage(MagickWand *wand,const double radius)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the circular neighborhood.
%
*/
WandExport unsigned int MagickOilPaintImage(MagickWand *wand,
  const double radius)
{
  Image
    *paint_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  paint_image=OilPaintImage(wand->image,radius,&wand->exception);
  if (paint_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,paint_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k O p a q u e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickOpaqueImage() changes any pixel that matches color with the color
%  defined by fill.
%
%  The format of the MagickOpaqueImage method is:
%
%      unsigned int MagickOpaqueImage(MagickWand *wand,const PixelWand *target,
%        const PixelWand *fill,const double fuzz)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o target: Change this target color to the fill color within the image.
%
%    o fill: The fill pixel wand.
%
%    o fuzz: By default target must match a particular pixel color
%      exactly.  However, in many cases two colors may differ by a small amount.
%      The fuzz member of image defines how much tolerance is acceptable to
%      consider two colors as the same.  For example, set fuzz to 10 and the
%      color red at intensities of 100 and 102 respectively are now interpreted
%      as the same color for the purposes of the floodfill.
%
*/
WandExport unsigned int MagickOpaqueImage(MagickWand *wand,
  const PixelWand *target,const PixelWand *fill,const double fuzz)
{
  PixelPacket
    fill_pixel,
    target_pixel;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(target,&target_pixel);
  PixelGetQuantumColor(fill,&fill_pixel);
  wand->image->fuzz=fuzz;
  status=OpaqueImage(wand->image,target_pixel,fill_pixel);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k P i n g I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickPingImage() is like MagickReadImage() except the only valid
%  information returned is the image width, height, size, and format.  It
%  is designed to efficiently obtain this information from a file without
%  reading the entire image sequence into memory.
%
%  The format of the MagickPingImage method is:
%
%      unsigned int MagickPingImage(MagickWand *wand,const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
%
*/
WandExport unsigned int MagickPingImage(MagickWand *wand,const char *filename)
{
  Image
    *images;

  ImageInfo
    *ping_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  ping_info=CloneImageInfo(wand->image_info);
  (void) CopyMagickString(ping_info->filename,filename,MaxTextExtent);
  images=PingImage(ping_info,&wand->exception);
  DestroyImageInfo(ping_info);
  if (images == (Image *) NULL)
    return(False);
  AppendImageToList(&wand->images,images);
  wand->image=GetLastImageInList(wand->images);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k P r e v i e w I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickPreviewImages() tiles 9 thumbnails of the specified image with an
%  image processing operation applied at varying strengths.  This is helpful
%  to quickly pin-point an appropriate parameter for an image processing
%  operation.
%
%  The format of the MagickPreviewImages method is:
%
%      MagickWand *MagickPreviewImages(MagickWand *wand,
%        const PreviewType preview)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o preview: The preview type.
%
*/
WandExport MagickWand *MagickPreviewImages(MagickWand *wand,
  const PreviewType preview)
{
#if defined(NOT_IMPLEMENTED)
  Image
    *preview_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  preview_image=PreviewImage(wand->images,preview,&wand->exception);
  if (preview_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,preview_image));
#else
  return False;
#endif
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
%      unsigned int MagickPreviousImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickPreviousImage(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (wand->image->previous == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image=wand->image->previous;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k P r o f i l e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use MagickProfileImage() to add or remove a ICC, IPTC, or generic profile
%  from an image.  If the profile is NULL, it is removed from the image
%  otherwise added.  Use a name of '*' and a profile of NULL to remove all
%  profiles from the image.
%
%  The format of the MagickProfileImage method is:
%
%      unsigned int MagickProfileImage(MagickWand *wand,const char *name,
%        const unsigned char *profile,const size_t length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o name: Name of profile to add or remove: ICC, IPTC, or generic profile.
%
%    o profile: The profile.
%
%    o length: The length of the profile.
%
*/
WandExport unsigned int MagickProfileImage(MagickWand *wand,const char *name,
  const unsigned char *profile,const size_t length)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ProfileImage(wand->image,name,profile,length,True);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k Q u a n t i z e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickQuantizeImage() analyzes the colors within a reference image and
%  chooses a fixed number of colors to represent the image.  The goal of the
%  algorithm is to minimize the color difference between the input and output
%  image while minimizing the processing time.
%
%  The format of the MagickQuantizeImage method is:
%
%      unsigned int MagickQuantizeImage(MagickWand *wand,
%        const unsigned long number_colors,const ColorspaceType colorspace,
%        const unsigned long treedepth,const unsigned int dither,
%        const unsigned int measure_error)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o number_colors: The number of colors.
%
%    o colorspace: Perform color reduction in this colorspace, typically
%      RGBColorspace.
%
%    o treedepth: Normally, this integer value is zero or one.  A zero or
%      one tells Quantize to choose a optimal tree depth of Log4(number_colors).%      A tree of this depth generally allows the best representation of the
%      reference image with the least amount of memory and the fastest
%      computational speed.  In some cases, such as an image with low color
%      dispersion (a few number of colors), a value other than
%      Log4(number_colors) is required.  To expand the color tree completely,
%      use a value of 8.
%
%    o dither: A value other than zero distributes the difference between an
%      original image and the corresponding color reduced algorithm to
%      neighboring pixels along a Hilbert curve.
%
%    o measure_error: A value other than zero measures the difference between
%      the original and quantized images.  This difference is the total
%      quantization error.  The error is computed by summing over all pixels
%      in an image the distance squared in RGB space between each reference
%      pixel value and its quantized value.
%
*/
WandExport unsigned int MagickQuantizeImage(MagickWand *wand,
  const unsigned long number_colors,const ColorspaceType colorspace,
  const unsigned long treedepth,const unsigned int dither,
  const unsigned int measure_error)
{
  QuantizeInfo
    *quantize_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  quantize_info->number_colors=number_colors;
  quantize_info->tree_depth=treedepth;
  quantize_info->colorspace=colorspace;
  quantize_info->measure_error=measure_error;
  status=QuantizeImage(quantize_info,wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  DestroyQuantizeInfo(quantize_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k Q u a n t i z e I m a g e s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickQuantizeImage() analyzes the colors within a sequence of images and
%  chooses a fixed number of colors to represent the image.  The goal of the
%  algorithm is to minimize the color difference between the input and output
%  image while minimizing the processing time.
%
%  The format of the MagickQuantizeImages method is:
%
%      unsigned int MagickQuantizeImages(MagickWand *wand,
%        const unsigned long number_colors,const ColorspaceType colorspace,
%        const unsigned long treedepth,const unsigned int dither,
%        const unsigned int measure_error)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o number_colors: The number of colors.
%
%    o colorspace: Perform color reduction in this colorspace, typically
%      RGBColorspace.
%
%    o treedepth: Normally, this integer value is zero or one.  A zero or
%      one tells Quantize to choose a optimal tree depth of Log4(number_colors).%      A tree of this depth generally allows the best representation of the
%      reference image with the least amount of memory and the fastest
%      computational speed.  In some cases, such as an image with low color
%      dispersion (a few number of colors), a value other than
%      Log4(number_colors) is required.  To expand the color tree completely,
%      use a value of 8.
%
%    o dither: A value other than zero distributes the difference between an
%      original image and the corresponding color reduced algorithm to
%      neighboring pixels along a Hilbert curve.
%
%    o measure_error: A value other than zero measures the difference between
%      the original and quantized images.  This difference is the total
%      quantization error.  The error is computed by summing over all pixels
%      in an image the distance squared in RGB space between each reference
%      pixel value and its quantized value.
%
*/
WandExport unsigned int MagickQuantizeImages(MagickWand *wand,
  const unsigned long number_colors,const ColorspaceType colorspace,
  const unsigned long treedepth,const unsigned int dither,
  const unsigned int measure_error)
{
  QuantizeInfo
    *quantize_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  quantize_info->number_colors=number_colors;
  quantize_info->tree_depth=treedepth;
  quantize_info->colorspace=colorspace;
  quantize_info->measure_error=measure_error;
  status=QuantizeImages(quantize_info,wand->images);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  DestroyQuantizeInfo(quantize_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k Q u e r y F o n t M e t r i c s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickQueryFontMetrics() returns a 7 element array representing the
%  following font metrics:
%
%    Element Description
%    -------------------------------------------------
%          0 character width
%          1 character height
%          2 ascender
%          3 descender
%          4 text width
%          5 text height
%          6 maximum horizontal advance
%
%  The format of the MagickQueryFontMetrics method is:
%
%      double *MagickQueryFontMetrics(MagickWand *wand,
%        const DrawingWand *drawing_wand,const char *text)
%
%  A description of each parameter follows:
%
%    o wand: The Magick wand.
%
%    o drawing_wand: The drawing wand.
%
%    o text: The text.
%
% */
WandExport double *MagickQueryFontMetrics(MagickWand *wand,
  const DrawingWand *drawing_wand,const char *text)
{
  double
    *font_metrics;

  DrawInfo
    *draw_info;

  TypeMetric
    metrics;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(drawing_wand != (const DrawingWand *) NULL);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  font_metrics=(double *) AcquireMagickMemory(7*sizeof(double));
  if (font_metrics == (double *) NULL)
    return((double *) NULL);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if (draw_info == (DrawInfo *) NULL)
    {
      font_metrics=(double *) RelinquishMagickMemory(font_metrics);
      return((double *) NULL);
    }
  CloneString(&draw_info->text,text);
  status=GetTypeMetrics(wand->image,draw_info,&metrics);
  DestroyDrawInfo(draw_info);
  if (status == False)
    {
      InheritException(&wand->exception,&wand->image->exception);
      font_metrics=(double *) RelinquishMagickMemory(font_metrics);
      return((double *) NULL);
    }
  font_metrics[0]=metrics.pixels_per_em.x;
  font_metrics[1]=metrics.pixels_per_em.y;
  font_metrics[2]=metrics.ascent;
  font_metrics[3]=metrics.descent;
  font_metrics[4]=metrics.width;
  font_metrics[5]=metrics.height;
  font_metrics[6]=metrics.max_advance;
  return(font_metrics);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R a i s e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRaiseImage() creates a simulated three-dimensional button-like effect
%  by lightening and darkening the edges of the image.  Members width and
%  height of raise_info define the width of the vertical and horizontal
%  edge of the effect.
%
%  The format of the MagickRaiseImage method is:
%
%      unsigned int MagickRaiseImage(MagickWand *wand,const unsigned long width,
%        const unsigned long height,const long x,const long y,
%        const unsigned int raise)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width,height,x,y:  Define the dimensions of the area to raise.
%
%    o raise: A value other than zero creates a 3-D raise effect,
%      otherwise it has a lowered effect.
%
*/
WandExport unsigned int MagickRaiseImage(MagickWand *wand,
  const unsigned long width,const unsigned long height,const long x,
  const long y,const unsigned int raise)
{
  RectangleInfo
    raise_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  raise_info.width=width;
  raise_info.height=height;
  raise_info.x=x;
  raise_info.y=y;
  status=RaiseImage(wand->image,&raise_info,raise);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
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
*/
WandExport unsigned int MagickReadImage(MagickWand *wand,const char *filename)
{
  Image
    *images;

  ImageInfo
    *read_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  read_info=CloneImageInfo(wand->image_info);
  (void) CopyMagickString(read_info->filename,filename,MaxTextExtent);
  images=ReadImage(read_info,&wand->exception);
  DestroyImageInfo(read_info);
  if (images == (Image *) NULL)
    return(False);
  AppendImageToList(&wand->images,images);
  wand->image=GetLastImageInList(wand->images);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e a d I m a g e B l o b                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReadImageBlob() reads an image or image sequence from a blob.
%
%  The format of the MagickReadImageBlob method is:
%
%      unsigned int MagickReadImageBlob(MagickWand *wand,const void *blob,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o blob: The blob.
%
%    o length: The blob length.
%
*/
WandExport unsigned int MagickReadImageBlob(MagickWand *wand,const void *blob,
  const size_t length)
{
  Image
    *images;

  ImageInfo
    *read_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  read_info=CloneImageInfo(wand->image_info);
  read_info->blob=(void *) blob;
  read_info->length=length;
  images=ReadImage(read_info,&wand->exception);
  DestroyImageInfo(read_info);
  if (images == (Image *) NULL)
    return(False);
  AppendImageToList(&wand->images,images);
  wand->image=GetLastImageInList(wand->images);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e a d I m a g e F i l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReadImageFile() reads an image or image sequence from an open file 
%  descriptor.
%
%  The format of the MagickReadImageFile method is:
%
%      unsigned int MagickReadImageFile(MagickWand *wand,FILE *file)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o file: The file descriptor.
%
*/
WandExport unsigned int MagickReadImageFile(MagickWand *wand,FILE *file)
{
  Image
    *images;

  ImageInfo
    *read_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  read_info=CloneImageInfo(wand->image_info);
  read_info->file=file;
  images=ReadImage(read_info,&wand->exception);
  DestroyImageInfo(read_info);
  if (images == (Image *) NULL)
    return(False);
  AppendImageToList(&wand->images,images);
  wand->image=GetLastImageInList(wand->images);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M a g i c k R e d u c e N o i s e I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReduceNoiseImage() smooths the contours of an image while still
%  preserving edge information.  The algorithm works by replacing each pixel
%  with its neighbor closest in value.  A neighbor is defined by radius.  Use
%  a radius of 0 and ReduceNoise() selects a suitable radius for you.
%
%  The format of the MagickReduceNoiseImage method is:
%
%      unsigned int MagickReduceNoiseImage(MagickWand *wand,const double radius)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius: The radius of the pixel neighborhood.
%
*/
WandExport unsigned int MagickReduceNoiseImage(MagickWand *wand,
  const double radius)
{
  Image
    *noise_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  noise_image=ReduceNoiseImage(wand->image,radius,&wand->exception);
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
%   M a g i c k R e l i n q u i s h M e m o r y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRelinquishMemory() relinquishes memory resources returned by such
%  methods as MagickDescribeImage(), MagickGetException(), etc.
%
%  The format of the MagickRelinquishMemory method is:
%
%      unsigned int MagickRelinquishMemory(void *resource)
%
%  A description of each parameter follows:
%
%    o resource: Relinquish the memory associated with this resource.
%
%
*/
WandExport unsigned int MagickRelinquishMemory(void *memory)
{
  assert(memory != (void *) NULL);
  memory=(void *) RelinquishMagickMemory(memory);
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
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  DeleteImageFromList(&wand->image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e s a m p l e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickResampleImage() resample image to desired resolution.
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
%  The format of the MagickResampleImage method is:
%
%      unsigned int MagickResampleImage(MagickWand *wand,
%        const double x_resolution,const double y_resolution,
%        const FilterTypes filter,const double blur)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_resolution: The new image x resolution.
%
%    o y_resolution: The new image y resolution.
%
%    o filter: Image filter to use.
%
%    o blur: The blur factor where > 1 is blurry, < 1 is sharp.
%
%
*/
WandExport unsigned int MagickResampleImage(MagickWand *wand,
  const double x_resolution,const double y_resolution,const FilterTypes filter,
  const double blur)
{
  Image
    *resample_image;

  unsigned long
    height,
    width;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  width=(unsigned long) (x_resolution*wand->image->columns/
    (wand->image->x_resolution == 0.0 ? 72.0 : wand->image->x_resolution)+0.5);
  height=(unsigned long) (y_resolution*wand->image->rows/
    (wand->image->y_resolution == 0.0 ? 72.0 : wand->image->y_resolution)+0.5);
  resample_image=ResizeImage(wand->image,width,height,filter,blur,
    &wand->exception);
  if (resample_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,resample_image);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
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
%   M a g i c k R o l l I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRollImage() offsets an image as defined by x_offset and y_offset.
%
%  The format of the MagickRollImage method is:
%
%      unsigned int MagickRollImage(MagickWand *wand,const long x_offset,
%        const unsigned long y_offset)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_offset: The x offset.
%
%    o y_offset: The y offset.
%
%
*/
WandExport unsigned int MagickRollImage(MagickWand *wand,const long x_offset,
  const long y_offset)
{
  Image
    *roll_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  roll_image=RollImage(wand->image,x_offset,y_offset,&wand->exception);
  if (roll_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,roll_image);
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
%  MagickRotateImage() rotates an image the specified number of degrees. Empty
%  triangles left over from rotating the image are filled with the
%  background color.
%
%  The format of the MagickRotateImage method is:
%
%      unsigned int MagickRotateImage(MagickWand *wand,
%        const PixelWand *background,const double degrees)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o background: The background pixel wand.
%
%    o degrees: The number of degrees to rotate the image.
%
%
*/
WandExport unsigned int MagickRotateImage(MagickWand *wand,
  const PixelWand *background,const double degrees)
{
  Image
    *rotate_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(background,&wand->image->background_color);
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
%   M a g i c k S a m p l e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSampleImage() scales an image to the desired dimensions with pixel
%  sampling.  Unlike other scaling methods, this method does not introduce
%  any additional color into the scaled image.
%
%  The format of the MagickSampleImage method is:
%
%      unsigned int MagickSampleImage(MagickWand *wand,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%
*/
WandExport unsigned int MagickSampleImage(MagickWand *wand,
  const unsigned long columns,const unsigned long rows)
{
  Image
    *sample_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  sample_image=SampleImage(wand->image,columns,rows,&wand->exception);
  if (sample_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,sample_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S c a l e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickScaleImage() scales the size of an image to the given dimensions.
%
%  The format of the MagickScaleImage method is:
%
%      unsigned int MagickScaleImage(MagickWand *wand,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%
*/
WandExport unsigned int MagickScaleImage(MagickWand *wand,
  const unsigned long columns,const unsigned long rows)
{
  Image
    *scale_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  scale_image=ScaleImage(wand->image,columns,rows,&wand->exception);
  if (scale_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,scale_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
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
%  MagickSetFilename() sets the filename before you read or write an image file.
%
%  The format of the MagickSetFilename method is:
%
%      unsigned int MagickSetFilename(MagickWand *wand,const char *filename)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
*/
WandExport unsigned int MagickSetFilename(MagickWand *wand,const char *filename)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) CopyMagickString(wand->image_info->filename,filename,MaxTextExtent);
  return(True);
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
%      unsigned int MagickSetImage(MagickWand *wand,const unsigned long scene)
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

  assert(wand != (MagickWand *) NULL);
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
%   M a g i c k S e t I m a g e B a c k g r o u n d C o l o r                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageBackgroundColor() sets the image background color.
%
%  The format of the MagickSetImageBackgroundColor method is:
%
%      unsigned int MagickSetImageBackgroundColor(MagickWand *wand,
%        const PixelWand *background)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o background: The background pixel wand.
%
*/
WandExport unsigned int MagickSetImageBackgroundColor(MagickWand *wand,
  const PixelWand *background)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(background,&wand->image->background_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e B l u e P r i m a r y                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageBluePrimary() sets the image chromaticity blue primary point.
%
%  The format of the MagickSetImageBluePrimary method is:
%
%      unsigned int MagickSetImageBluePrimary(MagickWand *wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The blue primary x-point.
%
%    o y: The blue primary y-point.
%
%
*/
WandExport unsigned int MagickSetImageBluePrimary(MagickWand *wand,
  const double x,const double y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->chromaticity.blue_primary.x=x;
  wand->image->chromaticity.blue_primary.y=y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e B o r d e r C o l o r                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageBorderColor() sets the image border color.
%
%  The format of the MagickSetImageBorderColor method is:
%
%      unsigned int MagickSetImageBorderColor(MagickWand *wand,
%        const PixelWand *border)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o border: The border pixel wand.
%
*/
WandExport unsigned int MagickSetImageBorderColor(MagickWand *wand,
  const PixelWand *border)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(border,&wand->image->border_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e C o l o r m a p I m a g e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageColormapColor() sets the color of the specified colormap
%  index.
%
%  The format of the MagickSetImageColormapColor method is:
%
%      unsigned int MagickSetImageColormapColor(MagickWand *wand,
%        const unsigned long index,const PixelWand *color)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o index: The offset into the image colormap.
%
%    o color: Return the colormap color in this wand.
%
*/
WandExport unsigned int MagickSetImageColormapColor(MagickWand *wand,
  const unsigned long index,const PixelWand *color)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  if ((wand->image->colormap == (PixelPacket *) NULL) ||
      (index >= wand->image->colors))
    ThrowWandException(ImageError,InvalidColormapIndex,(char *) NULL);
  PixelGetQuantumColor(color,wand->image->colormap+index);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e C o m p o s e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageCompose() sets the image composite operator, useful for
%  specifying how to composite the image thumbnail when using the
%  MagickMontageImage() method.
%
%  The format of the MagickSetImageCompose method is:
%
%      unsigned int MagickSetImageCompose(MagickWand *wand,
%        const CompositeOperator compose)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o compose: The image composite operator.
%
*/
WandExport unsigned int MagickSetImageCompose(MagickWand *wand,
  const CompositeOperator compose)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->compose=compose;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e C o m p r e s s i o n                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageCompression() sets the image compression.
%
%  The format of the MagickSetImageCompression method is:
%
%      unsigned int MagickSetImageCompression(MagickWand *wand,
%        const CompressionType compression)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o compression: The image compression type.
%
*/
WandExport unsigned int MagickSetImageCompression(MagickWand *wand,
  const CompressionType compression)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->compression=compression;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e D e l a y                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageDelay() sets the image delay.
%
%  The format of the MagickSetImageDelay method is:
%
%      unsigned int MagickSetImageDelay(MagickWand *wand,
%        const unsigned long delay)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o delay: The image delay in 1/100th of a second.
%
*/
WandExport unsigned int MagickSetImageDelay(MagickWand *wand,
  const unsigned long delay)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->delay=delay;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e D e p t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageChannelDepth() sets the depth of a particular image channel.
%
%  The format of the MagickSetImageChannelDepth method is:
%
%      unsigned int MagickSetImageChannelDepth(MagickWand *wand,
%        const ChannelType channel,const unsigned long depth)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel, or IndexChannel.
%
%    o depth: The image depth in bits.
%
*/
WandExport unsigned int MagickSetImageChannelDepth(MagickWand *wand,
  const ChannelType channel,const unsigned long depth)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  SetImageChannelDepth(wand->image,channel,depth);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e D e p t h                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageDepth() sets the image depth.
%
%  The format of the MagickSetImageDepth method is:
%
%      unsigned int MagickSetImageDepth(MagickWand *wand,
%        const unsigned long depth)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o depth: The image depth in bits: 8, 16, or 32.
%
*/
WandExport unsigned int MagickSetImageDepth(MagickWand *wand,
  const unsigned long depth)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  SetImageDepth(wand->image,depth);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e G a m m a                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageGamma() sets the image gamma.
%
%  The format of the MagickSetImageGamma method is:
%
%      unsigned int MagickSetImageGamma(MagickWand *wand,const double gamma)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o gamma: The image gamma.
%
*/
WandExport unsigned int MagickSetImageGamma(MagickWand *wand,const double gamma)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->gamma=gamma;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e G r e e n P r i m a r y                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageGreenPrimary() sets the image chromaticity green primary
%  point.
%
%  The format of the MagickSetImageGreenPrimary method is:
%
%      unsigned int MagickSetImageGreenPrimary(MagickWand *wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The green primary x-point.
%
%    o y: The green primary y-point.
%
%
*/
WandExport unsigned int MagickSetImageGreenPrimary(MagickWand *wand,
  const double x,const double y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->chromaticity.green_primary.x=x;
  wand->image->chromaticity.green_primary.y=y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e M a t t e C o l o r                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageMatteColor() sets the image matte color.
%
%  The format of the MagickSetImageMatteColor method is:
%
%      unsigned int MagickSetImageMatteColor(MagickWand *wand,
%        const PixelWand *matte)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o matte: The matte pixel wand.
%
*/
WandExport unsigned int MagickSetImageMatteColor(MagickWand *wand,
  const PixelWand *matte)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(matte,&wand->image->matte_color);
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
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) CopyMagickString(wand->image->filename,filename,MaxTextExtent);
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

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ImportImagePixels(wand->image,x_offset,y_offset,columns,rows,map,
    storage,pixels);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e R e d P r i m a r y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageRedPrimary() sets the image chromaticity red primary point.
%
%  The format of the MagickSetImageRedPrimary method is:
%
%      unsigned int MagickSetImageRedPrimary(MagickWand *wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The red primary x-point.
%
%    o y: The red primary y-point.
%
*/
WandExport unsigned int MagickSetImageRedPrimary(MagickWand *wand,
  const double x,const double y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->chromaticity.red_primary.x=x;
  wand->image->chromaticity.red_primary.y=y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e R e n d e r i n g I n t e n t                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageRenderingIntent() sets the image rendering intent.
%
%  The format of the MagickSetImageRenderingIntent method is:
%
%      unsigned int MagickSetImageRenderingIntent(MagickWand *wand,
%        const RenderingIntent rendering_intent)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o rendering_intent: The image rendering intent: UndefinedIntent,
%      SaturationIntent, PerceptualIntent, AbsoluteIntent, or RelativeIntent.
%
*/
WandExport unsigned int MagickSetImageRenderingIntent(MagickWand *wand,
  const RenderingIntent rendering_intent)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->rendering_intent=rendering_intent;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e D e l a y                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageScene() sets the image scene.
%
%  The format of the MagickSetImageScene method is:
%
%      unsigned int MagickSetImageScene(MagickWand *wand,
%        const unsigned long scene)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o delay: The image scene number.
%
*/
WandExport unsigned int MagickSetImageScene(MagickWand *wand,
  const unsigned long scene)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->scene=scene;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e U n i t s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageUnits() sets the image units of resolution.
%
%  The format of the MagickSetImageUnits method is:
%
%      unsigned int MagickSetImageUnits(MagickWand *wand,
%        const ResolutionType units)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o units: The image units of resolution : Undefinedresolution,
%      PixelsPerInchResolution, or PixelsPerCentimeterResolution.
%
*/
WandExport unsigned int MagickSetImageUnits(MagickWand *wand,
  const ResolutionType units)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->units=units;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e R e s o l u t i o n                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageResolution() sets the image resolution.
%
%  The format of the MagickSetImageResolution method is:
%
%      unsigned int MagickSetImageResolution(MagickWand *wand,
%        const double x_resolution,const doubtl y_resolution)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_resolution: The image x resolution.
%
%    o y_resolution: The image y resolution.
%
*/
WandExport unsigned int MagickSetImageResolution(MagickWand *wand,
  const double x_resolution,const double y_resolution)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->x_resolution=x_resolution;
  wand->image->y_resolution=y_resolution;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t R e s o u r c e L i m i t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetResourceLimit() sets the limit for a particular resource in
%  megabytes.
%
%  The format of the MagickSetResourceLimit method is:
%
%      unsigned int MagickSetResourceLimit(const ResourceType type,
%        const unsigned long *limit)
%
%  A description of each parameter follows:
%
%    o type: The type of resource: AreaResource, MemoryResource, MapResource,
%      DiskResource, FileResource.
%
%    o The maximum limit for the resource.
%
*/
WandExport unsigned int MagickSetResourceLimit(const ResourceType type,
  const unsigned long limit)
{
  return(SetMagickResourceLimit(type,limit));
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
%      unsigned int MagickSetSize(MagickWand *wand,const unsigned long columns,
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
WandExport unsigned int MagickSetSize(MagickWand *wand,
  const unsigned long columns,const unsigned long rows)
{
  char
    geometry[MaxTextExtent];

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) FormatMagickString(geometry,MaxTextExtent,"%lux%lu",columns,rows);
  (void) CloneString(&wand->image_info->size,geometry);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e W h i t e P o i n t                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageWhitePoint() sets the image chromaticity white point.
%
%  The format of the MagickSetImageWhitePoint method is:
%
%      unsigned int MagickSetImageWhitePoint(MagickWand *wand,const double x,
%        const double y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The white x-point.
%
%    o y: The white y-point.
%
%
*/
WandExport unsigned int MagickSetImageWhitePoint(MagickWand *wand,
  const double x,const double y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->chromaticity.white_point.x=x;
  wand->image->chromaticity.white_point.y=y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t P a s s p h r a s e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetPassphrase() sets the passphrase.
%
%  The format of the MagickSetPassphrase method is:
%
%      unsigned int MagickSetPassphrase(MagickWand *wand,const char *passphrase)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o passphrase: The passphrase.
%
*/
WandExport unsigned int MagickSetPassphrase(MagickWand *wand,
  const char *passphrase)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) CopyMagickString(wand->image_info->authenticate,passphrase,
    MaxTextExtent);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S h a r p e n I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSharpenImage() sharpens an image.  We convolve the image with a Gaussian
%  operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and SharpenImage() selects a suitable radius for you.
%
%  The format of the MagickSharpenImage method is:
%
%      unsigned int MagickSharpenImage(MagickWand *wand,const double radius,
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
WandExport unsigned int MagickSharpenImage(MagickWand *wand,const double radius,
  const double sigma)
{
  Image
    *sharpen_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  sharpen_image=SharpenImage(wand->image,radius,sigma,&wand->exception);
  if (sharpen_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,sharpen_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S h a v e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickShaveImage() shaves pixels from the image edges.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the MagickShaveImage method is:
%
%      unsigned int MagickShaveImage(MagickWand *wand,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%
*/
WandExport unsigned int MagickShaveImage(MagickWand *wand,
  const unsigned long columns,const unsigned long rows)
{
  Image
    *shave_image;

  RectangleInfo
    shave_info;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  shave_info.width=columns;
  shave_info.height=rows;
  shave_info.x=0;
  shave_info.y=0;
  shave_image=ShaveImage(wand->image,&shave_info,&wand->exception);
  if (shave_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,shave_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S h e a r e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickShearImage() slides one edge of an image along the X or Y axis,
%  creating a parallelogram.  An X direction shear slides an edge along the X
%  axis, while a Y direction shear slides an edge along the Y axis.  The amount
%  of the shear is controlled by a shear angle.  For X direction shears, x_shear
%  is measured relative to the Y axis, and similarly, for Y direction shears
%  y_shear is measured relative to the X axis.  Empty triangles left over from
%  shearing the image are filled with the background color.
%
%  The format of the MagickShearImage method is:
%
%      unsigned int MagickShearImage(MagickWand *wand,
%        const PixelWand *background,const double x_shear,onst double y_shear)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o background: The background pixel wand.
%
%    o x_shear: The number of degrees to shear the image.
%
%    o y_shear: The number of degrees to shear the image.
%
%
*/
WandExport unsigned int MagickShearImage(MagickWand *wand,
  const PixelWand *background,const double x_shear,const double y_shear)
{
  Image
    *shear_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(background,&wand->image->background_color);
  shear_image=ShearImage(wand->image,x_shear,y_shear,&wand->exception);
  if (shear_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,shear_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M a g i c k S o l a r i z e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSolarizeImage() applies a special effect to the image, similar to the
%  effect achieved in a photo darkroom by selectively exposing areas of photo
%  sensitive paper to light.  Threshold ranges from 0 to MaxRGB and is a
%  measure of the extent of the solarization.
%
%  The format of the MagickSolarizeImage method is:
%
%      unsigned int MagickSolarizeImage(MagickWand *wand,const double threshold)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o threshold:  Define the extent of the solarization.
%
*/
WandExport unsigned int MagickSolarizeImage(MagickWand *wand,
  const double threshold)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=SolarizeImage(wand->image,threshold);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S p r e a d I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSpreadImage() is a special effects method that randomly displaces each
%  pixel in a block defined by the radius parameter.
%
%  The format of the MagickSpreadImage method is:
%
%      unsigned int MagickSpreadImage(MagickWand *wand,const double radius)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o radius:  Choose a random pixel in a neighborhood of this extent.
%
*/
WandExport unsigned int MagickSpreadImage(MagickWand *wand,const double radius)
{
  Image
    *spread_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  spread_image=SpreadImage(wand->image,radius,&wand->exception);
  if (spread_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,spread_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S t e g a n o I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use MagickSteganoImage() to hide a digital watermark within the image.
%  Recover the hidden watermark later to prove that the authenticity of
%  an image.  Offset defines the start position within the image to hide
%  the watermark.
%
%  The format of the MagickSteganoImage method is:
%
%      MagickWand *MagickSteganoImage(MagickWand *wand,
%        const MagickWand *watermark_wand,const long offset)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o watermark_wand: The watermark wand.
%
%    o offset: Start hiding at this offset into the image.
%
*/
WandExport MagickWand *MagickSteganoImage(MagickWand *wand,
  const MagickWand *watermark_wand,const long offset)
{
  Image
    *stegano_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (watermark_wand->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wand->image->offset=offset;
  stegano_image=SteganoImage(wand->image,watermark_wand->image,
    &wand->exception);
  if (stegano_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,stegano_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S t e r e o I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickStereoImage() composites two images and produces a single image that
%  is the composite of a left and right image of a stereo pair
%
%  The format of the MagickStereoImage method is:
%
%      MagickWand *MagickStereoImage(MagickWand *wand,
%        const MagickWand *offset_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o offset_wand: Another image wand.
%
*/
WandExport MagickWand *MagickStereoImage(MagickWand *wand,
  const MagickWand *offset_wand)
{
  Image
    *stereo_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) || (offset_wand->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  stereo_image=StereoImage(wand->image,offset_wand->image,&wand->exception);
  if (stereo_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,stereo_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S t r i p I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickStripImage() strips an image of all profiles and comments.
%
%  The format of the MagickStripImage method is:
%
%      unsigned int MagickStripImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickStripImage(MagickWand *wand)
{
#if defined(NOT_IMPLEMENTED)
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=StripImage(wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S w i r l I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSwirlImage() swirls the pixels about the center of the image, where
%  degrees indicates the sweep of the arc through which each pixel is moved.
%  You get a more dramatic effect as the degrees move from 1 to 360.
%
%  The format of the MagickSwirlImage method is:
%
%      unsigned int MagickSwirlImage(MagickWand *wand,const double degrees)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o degrees: Define the tightness of the swirling effect.
%
*/
WandExport unsigned int MagickSwirlImage(MagickWand *wand,const double degrees)
{
  Image
    *swirl_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  swirl_image=SwirlImage(wand->image,degrees,&wand->exception);
  if (swirl_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,swirl_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T e x u r e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickTextureImage() repeatedly tiles the texture image across and down the
%  image canvas.
%
%  The format of the MagickTextureImage method is:
%
%      MagickWand *MagickTextureImage(MagickWand *wand,
%        const MagickWand *texture_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o texture_wand: The texture wand
%
*/
WandExport MagickWand *MagickTextureImage(MagickWand *wand,
  const MagickWand *texture_wand)
{
  Image
    *texture_image;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->image == (Image *) NULL) ||
      (texture_wand->image == (Image *) NULL))
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  texture_image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (texture_image == (Image *) NULL)
    return((MagickWand *) NULL);
  status=TextureImage(texture_image,texture_wand->image);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(CloneMagickWand(wand,texture_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T h r e s h o l d I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickThresholdImage() changes the value of individual pixels based on
%  the intensity of each pixel compared to threshold.  The result is a
%  high-contrast, two color image.
%
%  The format of the MagickThresholdImage method is:
%
%      unsigned int MagickThresholdImage(MagickWand *wand,
%        const double threshold)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o threshold: Define the threshold value.
%
*/
WandExport unsigned int MagickThresholdImage(MagickWand *wand,
  const double threshold)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  status=ThresholdImage(wand->image,threshold);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T i n t I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickTintImage() applies a color vector to each pixel in the image.  The
%  length of the vector is 0 for black and white and at its maximum for the
%  midtones.  The vector weighting function is
%  f(x)=(1-(4.0*((x-0.5)*(x-0.5)))).
%
%  The format of the MagickTintImage method is:
%
%      unsigned int MagickTintImage(MagickWand *wand,
%        const PixelWand *tint,const PixelWand *opacity)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o tint: The tint pixel wand.
%
%    o opacity: The opacity pixel wand.
%
*/
WandExport unsigned int MagickTintImage(MagickWand *wand,
  const PixelWand *tint,const PixelWand *opacity)
{
#if defined(NOT_IMPLEMENTED)
  char
    percent_opaque[MaxTextExtent];

  Image
    *tint_image;

  PixelPacket
    target;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) FormatMagickString(percent_opaque,MaxTextExtent,"%g,%g,%g,%g",
    100.0*PixelGetRedQuantum(tint)/MaxRGB,
    100.0*PixelGetGreenQuantum(tint)/MaxRGB,
    100.0*PixelGetBlueQuantum(tint)/MaxRGB,
    100.0*PixelGetOpacityQuantum(tint)/MaxRGB);
  PixelGetQuantumColor(tint,&target);
  tint_image=TintImage(wand->image,percent_opaque,target,&wand->exception);
  if (tint_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,tint_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
#else
  return False;
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T r a n s f o r m I m a g e s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickTransformImages() is a convenience method that behaves like
%  MagickResizeImage() or MagickCropImage() but accepts scaling and/or cropping
%  information as a region geometry specification.  If the operation fails, the
%  original image handle is returned.
%
%  The format of the MagickTransformImages method is:
%
%      MagickWand *MagickTransformImage(MagickWand *wand,const char *crop,
%        const char *geometry)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o crop: A crop geometry string.  This geometry defines a subregion of the
%      image to crop.
%
%    o geometry: An image geometry string.  This geometry defines the final
%      size of the image.
%
*/
WandExport MagickWand *MagickTransformImage(MagickWand *wand,
  const char *crop,const char *geometry)
{
  Image
    *transform_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  transform_image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (transform_image == (Image *) NULL)
    return(False);
  TransformImage(&transform_image,crop,geometry);
  if (transform_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWand(wand,transform_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T r a n s p a r e n t I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickTransparentImage() changes any pixel that matches color with the color
%  defined by fill.
%
%  The format of the MagickTransparentImage method is:
%
%      unsigned int MagickTransparentImage(MagickWand *wand,
%        const PixelWand *target,const unsigned int opacity,const double fuzz)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o target: Change this target color to specified opacity value within
%      the image.
%
%    o opacity: The replacement opacity value.
%
%    o fuzz: By default target must match a particular pixel color
%      exactly.  However, in many cases two colors may differ by a small amount.
%      The fuzz member of image defines how much tolerance is acceptable to
%      consider two colors as the same.  For example, set fuzz to 10 and the
%      color red at intensities of 100 and 102 respectively are now interpreted
%      as the same color for the purposes of the floodfill.
%
*/
WandExport unsigned int MagickTransparentImage(MagickWand *wand,
  const PixelWand *target,const unsigned int opacity,const double fuzz)
{
  PixelPacket
    target_pixel;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  PixelGetQuantumColor(target,&target_pixel);
  wand->image->fuzz=fuzz;
  status=TransparentImage(wand->image,target_pixel,opacity);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T r i m I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickTrimImage() remove edges that are the background color from the image.
%
%  The format of the MagickTrimImage method is:
%
%      unsigned int MagickTrimImage(MagickWand *wand,const double fuzz)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fuzz: By default target must match a particular pixel color
%      exactly.  However, in many cases two colors may differ by a small amount.
%      The fuzz member of image defines how much tolerance is acceptable to
%      consider two colors as the same.  For example, set fuzz to 10 and the
%      color red at intensities of 100 and 102 respectively are now interpreted
%      as the same color for the purposes of the floodfill.
%
*/
WandExport unsigned int MagickTrimImage(MagickWand *wand,const double fuzz)
{
  Image
    *trim_image;

  RectangleInfo
    trim;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  trim.width=0;
  trim.height=0;
  trim.x=0;
  trim.y=0;
  wand->image->fuzz=fuzz;
  trim_image=CropImage(wand->image,&trim,&wand->exception);
  if (trim_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,trim_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S h a r p e n I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickUnsharpMaskImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a radius
%  of 0 and UnsharpMaskImage() selects a suitable radius for you.
%
%  The format of the MagickUnsharpMaskImage method is:
%
%      unsigned int MagickUnsharpMaskImage(MagickWand *wand,const double radius,
%        const double sigma,const double amount,const double threshold)
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
%    o amount: The percentage of the difference between the original and the
%      blur image that is added back into the original.
%
%    o threshold: The threshold in pixels needed to apply the diffence amount.
%
*/
WandExport unsigned int MagickUnsharpMaskImage(MagickWand *wand,
  const double radius,const double sigma,const double amount,
  const double threshold)
{
  Image
    *unsharp_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  unsharp_image=UnsharpMaskImage(wand->image,radius,sigma,amount,threshold,
    &wand->exception);
  if (unsharp_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,unsharp_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W a v e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWaveImage()  creates a "ripple" effect in the image by shifting
%  the pixels vertically along a sine wave whose amplitude and wavelength
%  is specified by the given parameters.
%
%  The format of the MagickWaveImage method is:
%
%      unsigned int MagickWaveImage(MagickWand *wand,const double amplitude,
%        const double wave_length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o amplitude, wave_length:  Define the amplitude and wave length of the
%      sine wave.
%
*/
WandExport unsigned int MagickWaveImage(MagickWand *wand,const double amplitude,
  const double wave_length)
{
  Image
    *wave_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  wave_image=WaveImage(wand->image,amplitude,wave_length,&wand->exception);
  if (wave_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,wave_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W h i t e T h r e s o l d I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWhiteThresholdImage() is like ThresholdImage() but  forces all pixels
%  above the threshold into white while leaving all pixels below the threshold
%  unchanged.
%
%  The format of the MagickWhiteThresholdImage method is:
%
%      unsigned int MagickWhiteThresholdImage(MagickWand *wand,
%        const PixelWand *threshold)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o threshold: The pixel wand.
%
*/
WandExport unsigned int MagickWhiteThresholdImage(MagickWand *wand,
  const PixelWand *threshold)
{
#if defined(NOT_IMPLEMENTED)
  char
    thresholds[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) FormatMagickString(thresholds,MaxTextExtent,"%u,%u,%u,%u",
    PixelGetRedQuantum(threshold),PixelGetGreenQuantum(threshold),
    PixelGetBlueQuantum(threshold),PixelGetOpacityQuantum(threshold));
  status=WhiteThresholdImage(wand->image,thresholds);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
#else
  return False;
#endif
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
WandExport unsigned int MagickWriteImage(MagickWand *wand,const char *filename)
{
  ImageInfo
    *write_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  (void) CopyMagickString(wand->image->filename,filename,MaxTextExtent);
  write_info=CloneImageInfo(wand->image_info);
  write_info->adjoin=False;
  status=WriteImage(write_info,wand->image);
  DestroyImageInfo(write_info);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W r i t e I m a g e B l o b                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWriteImageBlob() implements direct to memory image formats.  It
%  returns the image as a blob and its length.  The magick member of the Image
%  structure determines the format of the returned blob (GIF, JPEG,  PNG, etc.)
%
%  The format of the MagickWriteImageBlob method is:
%
%      void *MagickWriteImageBlob(MagickWand *wand,size_t *length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o length: The length of the blob.
%
*/
WandExport void *MagickWriteImageBlob(MagickWand *wand,size_t *length)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  return(ImageToBlob(wand->image_info,wand->image,length,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W r i t e I m a g e F i l e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWandToFile() writes an image to an open file descriptor.
%
%  The format of the MagickWandToFile method is:
%
%      unsigned int MagickWandToFile(MagickWand *wand,FILE *file)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o file: The file descriptor.
%
%
*/
WandExport unsigned int MagickWriteImageFile(MagickWand *wand,FILE *file)
{
  ImageInfo
    *write_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->image == (Image *) NULL)
    ThrowWandException(ImageError,NoImagesWereFound,(char *) NULL);
  write_info=CloneImageInfo(wand->image_info);
  write_info->file=file;
  status=WriteImage(write_info,wand->image);
  DestroyImageInfo(write_info);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
  return(status);
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
%        const char *filename,const unsigned int adjoin)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The image filename.
%
%    o adjoin: join images into a single multi-image file.
%
*/
WandExport unsigned int MagickWriteImages(MagickWand *wand,const char *filename,
  const unsigned int adjoin)
{
  ImageInfo
    *write_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  write_info=CloneImageInfo(wand->image_info);
  write_info->adjoin=adjoin;
  status=WriteImages(write_info,wand->images,filename,&wand->exception);
  DestroyImageInfo(write_info);
  if (status == False)
    InheritException(&wand->exception,&wand->image->exception);
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
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImage);
  (void) memset(wand,0,sizeof(MagickWand));
  GetExceptionInfo(&wand->exception);
  wand->image_info=CloneImageInfo((ImageInfo *) NULL);
  wand->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  wand->images=NewImageList();
  wand->signature=MagickSignature;
  return(wand);
}
