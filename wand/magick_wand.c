/* Copyright (C) 2003-2015 GraphicsMagick Group */
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
%  Copyright (C) 1999-2004 ImageMagick Studio LLC, a non-profit organization  %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  This software and documentation is provided "as is," and the copyright     %
%  holders and contributing author(s) make no representations or warranties,  %
%  express or implied, including but not limited to, warranties of            %
%  merchantability or fitness for any particular purpose or that the use of   %
%  the software or documentation will not infringe any third party patents,   %
%  copyrights, trademarks or other rights.                                    %
%                                                                             %
%  The copyright holders and contributing author(s) will not be held liable   %
%  for any direct, indirect, special or consequential damages arising out of  %
%  any use of the software or documentation, even if advised of the           %
%  possibility of such damage.                                                %
%                                                                             %
%  Permission is hereby granted to use, copy, modify, and distribute this     %
%  source code, or portions hereof, documentation and executables, for any    %
%  purpose, without fee, subject to the following restrictions:               %
%                                                                             %
%    1. The origin of this source code must not be misrepresented.            %
%    2. Altered versions must be plainly marked as such and must not be       %
%       misrepresented as being the original source.                          %
%    3. This Copyright notice may not be removed or altered from any source   %
%       or altered source distribution.                                       %
%                                                                             %
%  The copyright holders and contributing author(s) specifically permit,      %
%  without fee, and encourage the use of this source code as a component for  %
%  supporting image processing in commercial products.  If you use this       %
%  source code in a product, acknowledgment is not required but would be      %
%  appreciated.                                                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
 * This version has been modified by Bob Friesenhahn for use with
 * GraphicsMagick.
 *
 * The following functions are not implemented at this time and always
 * return False:
 *
 *   MagickFxImage
 *   MagickFxImageChannel
 *   MagickGetConfigureInfo
 *   MagickPreviewImages
 *   MagickRadialBlurImage
 *   MagickTintImage
 *
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
#include "wand/wand_private.h"

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
  char
    id[MaxTextExtent];

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;

  Image
    *image,             /* Current working image */
    *images;            /* Whole image list */

  unsigned int
    iterator;

  unsigned long
    signature;
};

/*
  Forward declarations.
*/
static unsigned long
  GetMagickWandId(void);

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
%  CloneMagickWand() makes an exact copy of the specified wand.
%
%  The format of the CloneMagickWand method is:
%
%      MagickWand *CloneMagickWand(const MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand to clone.
%
*/
WandExport MagickWand *CloneMagickWand(const MagickWand *wand)
{
  MagickWand
    *clone_wand;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  clone_wand=MagickAllocateMemory(MagickWand *,sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImage);
  (void) memset(clone_wand,0,sizeof(MagickWand));
  (void) MagickFormatString(clone_wand->id,MaxTextExtent,"MagickWand-%lu",
    GetMagickWandId());
  GetExceptionInfo(&clone_wand->exception);
  CopyException(&clone_wand->exception,&wand->exception);
  clone_wand->image_info=CloneImageInfo(wand->image_info);
  clone_wand->quantize_info=CloneQuantizeInfo(wand->quantize_info);
  clone_wand->images=CloneImageList(wand->images,&clone_wand->exception);
  clone_wand->image=clone_wand->images;
  clone_wand->signature=MagickSignature;
  return(clone_wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o n e M a g i c k W a n d W i t h I m a g e s                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneMagickWandWithImages() clones the magick wand and inserts a new image
%  list.
%
%  The format of the CloneMagickWandWithImages method is:
%
%      MagickWand *CloneMagickWandWithImages(const MagickWand *wand,
%                                            Image *images)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o images: replace the image list with these image(s).
%
*/
static MagickWand *CloneMagickWandWithImages(const MagickWand *wand,
  Image *images)
{
  MagickWand
    *clone_wand;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  clone_wand=MagickAllocateMemory(MagickWand *,sizeof(MagickWand));
  if (clone_wand == (MagickWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
		      UnableToAllocateWand);
  (void) memset(clone_wand,0,sizeof(MagickWand));
  (void) MagickFormatString(clone_wand->id,MaxTextExtent,"MagickWand-%lu",
    GetMagickWandId());
  GetExceptionInfo(&clone_wand->exception);
  CopyException(&clone_wand->exception,&wand->exception);
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
  DestroyImageList(wand->images);
  MagickFreeMemory(wand);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A d a p t i v e T h r e s h o l d I m a g e                   %
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
%  The format of the MagickAdaptiveThresholdImage method is:
%
%      unsigned int MagickAdaptiveThresholdImage(MagickWand *wand,
%                     const unsigned long width,const unsigned long height,
%                     const long offset)
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%                                                                             %
%   M a g i c k A d d I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAddImage() adds the specified images at the current image location.
%
%  The format of the MagickAddImage method is:
%
%      unsigned int MagickAddImage(MagickWand *wand,
%        const MagickWand *add_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o insert: The splice wand.
%
*/
WandExport unsigned int MagickAddImage(MagickWand *wand,
  const MagickWand *add_wand)
{
  Image
    *images;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(add_wand != (MagickWand *) NULL);
  assert(add_wand->signature == MagickSignature);
  if (add_wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,add_wand->id);
  images=CloneImageList(add_wand->images,&wand->exception);
  if (images == (Image *) NULL)
    return(False);
  if ((wand->iterator != False) &&
      (GetPreviousImageInList(wand->image) == (Image *) NULL))
    PrependImageToList(&wand->image,images);
  else
    if ((wand->iterator != False) &&
        (GetNextImageInList(wand->image) == (Image *) NULL))
      AppendImageToList(&wand->image,images);
    else
      InsertImageInList(&wand->image,images);
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
%      Impulse, Laplacian, Poisson, or Random.
%
*/
WandExport unsigned int MagickAddNoiseImage(MagickWand *wand,
  const NoiseType noise_type)
{
  Image
    *noise_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%        const double angle,const char *text)
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
%    o angle: rotate text relative to this angle.
%
%    o text: text to draw
%
*/
WandExport unsigned int MagickAnnotateImage(MagickWand *wand,
  const DrawingWand *drawing_wand,const double x,const double y,
  const double angle,const char *text)
{
  char
    geometry[MaxTextExtent];

  DrawInfo
    *draw_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if (draw_info == (DrawInfo *) NULL)
    return(False);
  (void) CloneString(&draw_info->text,text);
  (void) MagickFormatString(geometry,MaxTextExtent,"%+f%+f",x,y);
  draw_info->affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
  draw_info->affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
  draw_info->affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
  draw_info->affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
  (void) CloneString(&draw_info->geometry,geometry);
  status=AnnotateImage(wand->image,draw_info);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
    CopyException(&wand->exception,&wand->image->exception);
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
  return(CloneMagickWandWithImages(wand,append_image));
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
  return(CloneMagickWandWithImages(wand,average_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k B l a c k T h r e s h o l d I m a g e                         %
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
  char
    thresholds[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) MagickFormatString(thresholds,MaxTextExtent,"%u,%u,%u,%u",
    PixelGetRedQuantum(threshold),PixelGetGreenQuantum(threshold),
    PixelGetBlueQuantum(threshold),PixelGetOpacityQuantum(threshold));
  status=BlackThresholdImage(wand->image,thresholds);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%     M a g i c k C d l I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The MagickCdlImage() method applies ("bakes in") the ASC-CDL which is a
%  format for the exchange of basic primary color grading information between
%  equipment and software from different manufacturers. The format defines
%  the math for three functions: slope, offset and power. Each function uses
%  a number for the red, green, and blue color channels for a total of nine
%  numbers comprising a single color decision. A tenth number for chrominance
%  (saturation) has been proposed but is not yet standardized.
%
%  The cdl argument string is comma delimited and is in the form (but
%  without invervening spaces or line breaks):
%
%    redslope, redoffset, redpower :
%    greenslope, greenoffset, greenpower :
%    blueslope, blueoffset, bluepower :
%    saturation
%
%  with the unity (no change) specification being:
%
%    "1.0,0.0,1.0:1.0,0.0,1.0:1.0,0.0,1.0:0.0"
%
%  See http://en.wikipedia.org/wiki/ASC_CDL for more information.
%
%  The format of the MagickCdlImage method is:
%
%      MagickPassFail MagickCdlImage(MagickWand *wand,const char *cdl)
%
%  A description of each parameter follows:
%
%    o wand: The image wand.
%
%    o cdl: Define the coefficients for slope offset and power in the
%      red green and blue channels, plus saturation.
%
*/
WandExport MagickPassFail MagickCdlImage(MagickWand *wand,const char *cdl)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(cdl != (const char *) NULL);

  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  return CdlImage(wand->image,cdl);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=ClipImage(wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=ClipPathImage(wand->image,pathname,inside);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o a l e s c e I m a g e s                                   %
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
  return(CloneMagickWandWithImages(wand,coalesce_image));
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  draw_info=CloneDrawInfo(wand->image_info,(DrawInfo *) NULL);
  PixelGetQuantumColor(fill,&draw_info->fill);
  (void) AcquireOnePixelByReference(wand->image,&target,x % wand->image->columns,
    y % wand->image->rows,&wand->exception);
  if (bordercolor != (PixelWand *) NULL)
    PixelGetQuantumColor(bordercolor,&target);
  wand->image->fuzz=fuzz;
  status=ColorFloodfillImage(wand->image,draw_info,target,x,y,
    bordercolor != (PixelWand *) NULL ? FillToBorderMethod : FloodfillMethod);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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

  ARG_NOT_USED(opacity);
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) MagickFormatString(percent_opaque,MaxTextExtent,"%g,%g,%g,%g",
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) SetImageAttribute(wand->image,"comment",(char *) NULL);
  status=SetImageAttribute(wand->image,"comment",comment);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o m p a r e I m a g e C h a n n e l s                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCompareImageChannels() compares one or more image channels and
%  returns the specified distortion metric.
%
%  The format of the MagickCompareImageChannels method is:
%
%      MagickWand *MagickCompareImageChannels(MagickWand *wand,
%        const MagickWand *reference,const ChannelType channel,
%        const MetricType metric,double *distortion)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o reference: The reference wand.
%
%    o channel: The channel.
%
%    o metric: The metric.
%
%    o distortion: The computed distortion between the images.
%
*/
WandExport MagickWand *
MagickCompareImageChannels(MagickWand *wand,
                           const MagickWand *reference,
                           const ChannelType channel,
                           const MetricType metric,
                           double *distortion)
{
  DifferenceImageOptions
    difference_options;

  Image
    *compare_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if ((wand->images == (Image *) NULL) || (reference->images == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if (distortion != (double *) NULL)
    *distortion=0.0;
  InitializeDifferenceImageOptions(&difference_options,
                                   &wand->image->exception);
  difference_options.channel=channel;
  compare_image=DifferenceImage(wand->image,reference->image,
                                &difference_options,
                                &wand->image->exception);
  if (compare_image == (Image *) NULL)
    return((MagickWand *) NULL);

  /*
    Perform statistical comparison of images using a metric.
  */
  if (distortion != (double *) NULL)
    {
      GetImageChannelDistortion(wand->image,reference->image,
                                channel,metric,distortion,
                                &wand->image->exception);
    }

  return(CloneMagickWandWithImages(wand,compare_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o m p a r e I m a g e s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCompareImage() compares one or more images and returns the specified
%  distortion metric.
%
%  The format of the MagickCompareImages method is:
%
%      MagickWand *MagickCompareImages(MagickWand *wand,
%        const MagickWand *reference,const MetricType metric,
%        double *distortion)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o reference: The reference wand.
%
%    o metric: The metric.
%
%    o distortion: The computed distortion between the images.
%
*/
WandExport MagickWand *MagickCompareImages(MagickWand *wand,
                                           const MagickWand *reference,
                                           const MetricType metric,
                                           double *distortion)
{
  return MagickCompareImageChannels(wand,reference,AllChannels,
                                    metric,distortion);
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
  if ((wand->images == (Image *) NULL) ||
      (composite_wand->images == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=CompositeImage(wand->image,compose,composite_wand->image,x,y);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=ContrastImage(wand->image,sharpen);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
%        const unsigned long order,const double *kernel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o order: The number of columns and rows in the filter kernel.
%
%    o kernel: An array of doubles representing the convolution kernel.
%
%
*/
WandExport unsigned int MagickConvolveImage(MagickWand *wand,
  const unsigned long order,const double *kernel)
{
  Image
    *convolve_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (kernel == (const double *) NULL)
    return(False);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=CycleColormapImage(wand->image,displace);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D e c o n s t r u c t I m a g e s                             %
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
  return(CloneMagickWandWithImages(wand,deconstruct_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k D e s c r i b e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickDescribeImage()  describes an image by formatting its attributes
%  to an allocated string which must be freed by the user.  Attributes
%  include the image width, height, size, and others.  The string is
%  similar to the output of 'identify -verbose'.
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

  size_t
    length;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  description=(char *) NULL;
  filename[0]='\0';
  length=0;
  if ((file = AcquireTemporaryFileStream(filename, TextFileIOMode)) == (FILE *) NULL)
    {
      ThrowException(&wand->exception,FileOpenError,
		     UnableToCreateTemporaryFile,filename);
    }
  else
    {
      (void) DescribeImage(wand->image,file,MagickTrue);
      (void) fclose(file);
      description=(char *) FileToBlob(filename,&length,&wand->exception);
      (void) LiberateTemporaryFile(filename);
    }
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (image == (Image *) NULL)
    return(False);
  wand->image_info->server_name=(char *) AcquireString(server_name);
  status=DisplayImages(wand->image_info,image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  (void) CloneString(&wand->image_info->server_name,server_name);
  status=DisplayImages(wand->image_info,wand->images);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
%  MagickDrawImage() draws vectors on the image as described by DrawingWand.
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if ((draw_info == (DrawInfo *) NULL) ||
      (draw_info->primitive == (char *) NULL))
    {
      if (draw_info != (DrawInfo *) NULL)
        DestroyDrawInfo(draw_info);
      return(False);
    }
  status=DrawImage(wand->image,draw_info);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=EqualizeImage(wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E x t e n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use MagickExtentImage() to change the image dimensions as specified by
%  geometry width and height.  The existing image content is composited at
%  the position specified by geometry x and y using the image compose method.
%  Existing image content which falls outside the bounds of the new image
%  dimensions is discarded.
%
%  The format of the MagickExtentImage method is:
%
%      unsigned int MagickExtentImage(MagickWand *wand,const size_t width,
%                                     const size_t height, const ssize_t x,
%                                     const ssize_t y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width: New image width
%
%    o height: New image height
%
%    o x, y: Top left composition coordinate to place existing image content
%            on the new image.
%
*/
WandExport unsigned int 
MagickExtentImage(MagickWand *wand,const size_t width,const size_t height,
		  const ssize_t x,const ssize_t y)
{
  Image
    *extent_image;

  RectangleInfo
    geometry;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  geometry.width=(unsigned long) width;
  geometry.height=(unsigned long) height;
  geometry.x=x;
  geometry.y=y;
  extent_image=ExtentImage(wand->image,&geometry,&wand->exception);
  if (extent_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,extent_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
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
  return(CloneMagickWandWithImages(wand,flatten_image));
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%   M a g i c k F x I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFxImage() evaluate expression for each pixel in the image.
%
%  The format of the MagickFxImage method is:
%
%      MagickWand *MagickFxImage(MagickWand *wand,const char *expression)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o expression: The expression.
%
*/
WandExport MagickWand *MagickFxImage(MagickWand *wand,const char *expression)
{
#if defined(NOT_IMPLEMENTED)
  Image
    *fx_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  fx_image=FxImage(wand->images,expression,&wand->exception);
  if (fx_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWandWithImages(wand,fx_image));
#else
  ARG_NOT_USED(expression);
  ThrowWandException(WandError,WandAPINotImplemented,"MagickFxImage");
#endif
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
%  MagickFxImageChannel() evaluate expression for each pixel in the specified
%  channel.
%
%  The format of the MagickFxImageChannel method is:
%
%      MagickWand *MagickFxImageChannel(MagickWand *wand,
%        const ChannelType channel,const char *expression)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to level: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel.
%
%    o expression: The expression.
%
*/
WandExport MagickWand * MagickFxImageChannel(MagickWand *wand,
  const ChannelType channel,const char *expression)
{
#if defined(NOT_IMPLEMENTED)
  Image
    *fx_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  fx_image=FxImageChannel(wand->images,channel,expression,&wand->exception);
  if (fx_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWandWithImages(wand,fx_image));
#else
  ARG_NOT_USED(channel);
  ARG_NOT_USED(expression);
  ThrowWandException(WandError,WandAPINotImplemented,"MagickFxImageChannel");
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
  char
    level[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  FormatString(level,"%g",gamma);
  status=GammaImage(wand->image,level);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
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
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return QuantumOperatorImage(wand->image,channel,GammaQuantumOp,
                              gamma,&wand->exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t C o n f i g u r e I n f o                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetConfigureInfo() returns ImageMagick configure attributes such as
%  NAME, VERSION, LIB_VERSION, COPYRIGHT, etc.
%
%  The format of the MagickGetConfigureInfo() method is:
%
%      char *MagickGetConfigureInfo(MagickWand *wand,const char *name)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o name: Return the attribute associated with this name.
%
*/
WandExport char *MagickGetConfigureInfo(MagickWand *wand,const char *name)
{
#if NOT_SUPPORTED
  const ConfigureInfo
    *configure_info;

  configure_info=GetConfigureInfo(name,&wand->exception);
  if (configure_info == (const ConfigureInfo *) NULL)
    return((char *) NULL);
  return(AcquireString(configure_info->value));
#else
  ARG_NOT_USED(name);
  ARG_NOT_USED(wand);
  return 0;
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t C o p y r i g h t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetCopyright() returns the ImageMagick API copyright as a string.
%
%  The format of the MagickGetCopyright method is:
%
%      const char *MagickGetCopyright(void)
%
*/
WandExport const char *MagickGetCopyright(void)
{
  return(GetMagickCopyright());
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
WandExport char *MagickGetException(const MagickWand *wand,
  ExceptionType *severity)
{
  char
    *description;

  assert(wand != (const MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(severity != (ExceptionType *) NULL);
  *severity=wand->exception.severity;
  description=MagickAllocateMemory(char *,2*MaxTextExtent);
  if (description == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateString);
  *description='\0';
  if (wand->exception.reason != (char *) NULL)
    (void) strlcpy(description,GetLocaleExceptionMessage(
      wand->exception.severity,wand->exception.reason),MaxTextExtent);
  if (wand->exception.description != (char *) NULL)
    {
      (void) strlcat(description," (",MaxTextExtent);
      (void) strlcat(description,GetLocaleExceptionMessage(
        wand->exception.severity,wand->exception.description),MaxTextExtent);
      (void) strlcat(description,")",MaxTextExtent);
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
%   M a g i c k G e t H o m e U R L                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetHomeURL() returns the ImageMagick home URL.
%
%  The format of the MagickGetHomeURL method is:
%
%      const char *MagickGetHomeURL(void)
%
*/
WandExport const char *MagickGetHomeURL(void)
{
  return(GetMagickWebSite());
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImage() clones the image at the current image index.
%
%  The format of the MagickGetImage method is:
%
%      MagickWand *MagickGetImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickWand *MagickGetImage(MagickWand *wand)
{
  Image
    *image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (image == (Image *) NULL)
    return(False);
  return(CloneMagickWandWithImages(wand,image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e A t t r i b u t e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageAttribute returns an image attribute as a string
%
%  The format of the MagickGetImageAttribute method is:
%
%      char *MagickGetImageAttribute(MagickWand *wand, const char *name)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o name: The name of the attribute
%
*/
WandExport char *MagickGetImageAttribute(MagickWand *wand, const char *name)
{
  const ImageAttribute
	  *attribute;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  attribute=GetImageAttribute(wand->image,name);
  if (attribute != (ImageAttribute *) NULL)
    return(AcquireString(attribute->value));
  CopyException(&wand->exception,&wand->image->exception);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e B a c k g r o u n d C o l o r                 %
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  PixelSetQuantumColor(border_color,&wand->image->border_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e B o u n d i n g B o x                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageBoundingBox() obtains the crop bounding box required to
%  remove a solid-color border from the image.  Color quantums which differ
%  less than the fuzz setting are considered to be the same.  If a border is
%  not detected, then the the original image dimensions are returned.  The
%  crop bounding box estimation uses the same algorithm as MagickTrimImage().
%
%  The format of the MagickGetImageBoundingBox method is:
%
%      unsigned int MagickGetImageBoundingBox(MagickWand *wand,
%                           const double fuzz,
%                           unsigned long *width,unsigned long *height,
%                           long *x, long *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fuzz: Color comparison fuzz factor.  Use 0.0 for exact match.
%
%    o width: The crop width
%
%    o height: The crop height
%
%    o x: The crop x offset
%
%    o y: The crop y offset
%
*/
WandExport unsigned int
MagickGetImageBoundingBox(MagickWand *wand,const double fuzz,
			  unsigned long *width, unsigned long *height,
			  long *x, long *y)
{
  RectangleInfo
    rectangle;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(width != (unsigned long *) NULL);
  assert(height != (unsigned long *) NULL);
  assert(x != (long *) NULL);
  assert(y != (long *) NULL);

  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  wand->image->fuzz=fuzz;
  rectangle=GetImageBoundingBox(wand->image,&wand->exception);
  *width=rectangle.width;
  *height=rectangle.height;
  *x=rectangle.x;
  *y=rectangle.y;

  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C h a n n e l D e p t h                       %
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
%      BlackChannel.
%
*/
WandExport unsigned long MagickGetImageChannelDepth(MagickWand *wand,
  const ChannelType channel)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(GetImageChannelDepth(wand->image,channel,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C h a n n e l E x t r e m a                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageChannelExtrema() gets the extrema for one or more image
%  channels.
%
%  The format of the MagickGetImageChannelExtrema method is:
%
%      unsigned int MagickGetImageChannelExtrema(MagickWand *wand,
%        const ChannelType channel,unsigned long *minima,unsigned long *maxima)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      or BlackChannel.
%
%    o minima:  The minimum pixel value for the specified channel(s).
%
%    o maxima:  The maximum pixel value for the specified channel(s).
%
*/
WandExport unsigned int
MagickGetImageChannelExtrema(MagickWand *wand,
                             const ChannelType channel,
                             unsigned long *minima,
                             unsigned long *maxima)
{
  ImageStatistics
    statistics;

  double
    minimum,
    maximum;

  MagickPassFail
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  status = GetImageStatistics(wand->image,&statistics,&wand->exception);
  minimum = 1.0;
  if (MagickChannelEnabled(channel,RedChannel) ||
      MagickChannelEnabled(channel,CyanChannel))
    minimum = Min(minimum,statistics.red.minimum);
  if (MagickChannelEnabled(channel,GreenChannel) ||
      MagickChannelEnabled(channel,MagentaChannel))
    minimum = Min(minimum,statistics.green.minimum);
  if (MagickChannelEnabled(channel,BlueChannel) ||
      MagickChannelEnabled(channel,YellowChannel))
    minimum = Min(minimum,statistics.blue.minimum);
  /* CMYK opacity not handled correctly here */
  if (MagickChannelEnabled(channel,OpacityChannel) ||
      MagickChannelEnabled(channel,BlackChannel))
    minimum = Min(minimum,statistics.opacity.minimum);
  minimum *= MaxRGBDouble;
  *minima = RoundDoubleToQuantum(minimum);

  maximum = 0.0;
  if (MagickChannelEnabled(channel,RedChannel) ||
      MagickChannelEnabled(channel,CyanChannel))
    maximum = Max(maximum,statistics.red.maximum);
  if (MagickChannelEnabled(channel,GreenChannel) ||
      MagickChannelEnabled(channel,MagentaChannel))
    maximum = Max(maximum,statistics.green.maximum);
  if (MagickChannelEnabled(channel,BlueChannel) ||
      MagickChannelEnabled(channel,YellowChannel))
    maximum = Max(maximum,statistics.blue.maximum);
  /* CMYK opacity not handled correctly here */
  if (MagickChannelEnabled(channel,OpacityChannel) ||
      MagickChannelEnabled(channel,BlackChannel))
    maximum = Max(maximum,statistics.opacity.maximum);
  maximum *= MaxRGBDouble;
  *maxima = RoundDoubleToQuantum(maximum);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C h a n n e l M e a n                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageChannelMean() gets the mean and standard deviation of one or
%  more image channels.
%
%  The format of the MagickGetImageChannelMean method is:
%
%      unsigned int MagickGetImageChannelMean(MagickWand *wand,
%        const ChannelType channel,double *mean,double *standard_deviation)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      or BlackChannel.
%
%    o mean:  The mean pixel value for the specified channel(s).
%
%    o standard_deviation:  The standard deviation for the specified channel(s).
%
*/
WandExport unsigned int MagickGetImageChannelMean(MagickWand *wand,
  const ChannelType channel,double *mean,double *standard_deviation)
{
  ImageStatistics
    statistics;

  unsigned int
    count;

  double
    deviation,
    meanf;

  MagickPassFail
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  status = GetImageStatistics(wand->image,&statistics,&wand->exception);
  deviation = 0.0;
  meanf =0.0;
  count = 0;
  if (MagickChannelEnabled(channel,RedChannel) ||
      MagickChannelEnabled(channel,CyanChannel))
    {
      deviation += statistics.red.standard_deviation;
      meanf += statistics.red.mean;
      count++;
    }
  if (MagickChannelEnabled(channel,GreenChannel) ||
      MagickChannelEnabled(channel,MagentaChannel))
    {
      deviation += statistics.green.standard_deviation;
      meanf += statistics.green.mean;
      count++;
    }
  if (MagickChannelEnabled(channel,BlueChannel) ||
      MagickChannelEnabled(channel,YellowChannel))
    {
      deviation += statistics.blue.standard_deviation;
      meanf += statistics.blue.mean;
      count++;
    }
  /* CMYK opacity not handled correctly here */
  if (MagickChannelEnabled(channel,OpacityChannel) ||
      MagickChannelEnabled(channel,BlackChannel))
    {
      deviation += statistics.opacity.standard_deviation;
      meanf += statistics.opacity.mean;
      count++;
    }
  if (count > 1)
    {
      /*
        If multiple channels are selected then deviation is not
        technically correct but an average of the deviation is better
        than nothing.
      */
      deviation /= (double) count;
      meanf /= (double) count;
    }
  deviation *= MaxRGBDouble;
  *standard_deviation = RoundDoubleToQuantum(deviation);
  
  meanf *= MaxRGBDouble;
  *mean = RoundDoubleToQuantum(meanf);

  return status;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if ((wand->image->colormap == (PixelPacket *) NULL) ||
      (index >= wand->image->colors))
    ThrowWandException(WandError,InvalidColormapIndex,(char *) NULL);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(GetNumberColors(wand->image,(FILE *) NULL,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e C o l o r s p a c e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageColorspace() gets the image colorspace.
%
%  The format of the MagickGetImageColorspace method is:
%
%      ColorspaceType MagickGetImageColorspace(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport ColorspaceType MagickGetImageColorspace(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedColorspace);
    }
  return(wand->image->colorspace);
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
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages
                            ,wand->id);
      return(UndefinedCompositeOp);
    }
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(GetImageDepth(wand->image,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e E x t r e m a                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageExtrema() gets the extrema for the image.
%
%  The format of the MagickGetImageExtrema method is:
%
%      unsigned int MagickGetImageExtrema(MagickWand *wand,
%        unsigned long *min,unsigned long *max)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o min:  The minimum pixel value for the specified channel(s).
%
%    o max:  The maximum pixel value for the specified channel(s).
%
*/
WandExport unsigned int MagickGetImageExtrema(MagickWand *wand,
  unsigned long *min,unsigned long *max)
{
  ImageStatistics
    statistics;

  double
    minimum,
    maximum;

  MagickPassFail
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  status = GetImageStatistics(wand->image,&statistics,&wand->exception);
  minimum = 1.0;
  minimum = Min(minimum,statistics.red.minimum);
  minimum = Min(minimum,statistics.green.minimum);
  minimum = Min(minimum,statistics.blue.minimum);
  minimum *= MaxRGBDouble;
  *min = RoundDoubleToQuantum(minimum);

  maximum = 0.0;
  maximum = Max(maximum,statistics.red.maximum);
  maximum = Max(maximum,statistics.green.maximum);
  maximum = Max(maximum,statistics.blue.maximum);
  maximum *= MaxRGBDouble;
  *max = RoundDoubleToQuantum(maximum);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e D i s p o s e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageDispose() gets the image disposal method.
%
%  The format of the MagickGetImageDispose method is:
%
%      DisposeType MagickGetImageDispose(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport DisposeType MagickGetImageDispose(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedDispose);
    }
  return((DisposeType) wand->image->dispose);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(AcquireString(wand->image->magick));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e F u z z                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageFuzz() returns the color comparison fuzz factor. Colors
%  closer than the fuzz factor are considered to be the same when comparing
%  colors.  Note that some other functions such as MagickColorFloodfillImage()
%  implicitly set this value.
%
%  The format of the MagickGetImageFuzz method is:
%
%      double MagickGetImageFuzz(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport double MagickGetImageFuzz(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return wand->image->fuzz;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->gamma);
}
/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %   M a g i c k G e t I m a g e G e o m e t r y                               %
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %
 %  MagickGetImageGeometry() gets the image geometry string.  NULL is
 %  returned if the image does not contain a geometry string.
 %
 %  The format of the MagickGetImageGeometry method is:
 %
 %      const char *MagickGetImageGeometry(MagickWand *wand)
 %
 %  A description of each parameter follows:
 %
 %    o wand: The magick wand.
 %
 */
WandExport const char *MagickGetImageGeometry(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->geometry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e G r a v i t y                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageGravity() gets the image gravity.
%
%  The format of the MagickGetImageGravity method is:
%
%      GravityType MagickGetImageGravity(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport GravityType MagickGetImageGravity(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->gravity);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  *x=wand->image->chromaticity.green_primary.x;
  *y=wand->image->chromaticity.green_primary.y;
  return(True);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->rows);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e H i s t o g r a m                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageHistogram() returns the image histogram as an array of
%  PixelWand wands.
%
%  The format of the MagickGetImageHistogram method is:
%
%      PixelWand *MagickGetImageHistogram(MagickWand *wand,
%        unsigned long *number_colors)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o number_colors: The number of unique colors in the image and the number
%      of pixel wands returned.
%
%
*/
WandExport PixelWand **MagickGetImageHistogram(MagickWand *wand,
  unsigned long *number_colors)
{
  HistogramColorPacket
    *histogram;

  PixelWand
    **pixel_wands;

  register long
    i;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  histogram=GetColorHistogram(wand->image,number_colors,&wand->exception);
  if (histogram == (HistogramColorPacket *) NULL)
    return((PixelWand **) NULL);
  pixel_wands=NewPixelWands(*number_colors);
  for (i=0; i < (long) *number_colors; i++)
  {
    PixelSetQuantumColor(pixel_wands[i],&histogram[i].pixel);
    PixelSetColorCount(pixel_wands[i],histogram[i].count);
  }
  MagickFreeMemory(histogram);
  return(pixel_wands);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e I n d e x                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageIndex() returns the index of the current image.
%
%  The format of the MagickGetImageIndex method is:
%
%      unsigned int MagickGetImageIndex(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport long MagickGetImageIndex(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImageIndexs,wand->id);
  return(GetImageIndexInList(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e I n t e r l a c e S c h e m e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageInterlaceScheme() gets the image interlace scheme.
%
%  The format of the MagickGetImageInterlaceScheme method is:
%
%      InterlaceType MagickGetImageInterlaceScheme(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport InterlaceType MagickGetImageInterlaceScheme(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedInterlace);
    }
  return(wand->image->interlace);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e I t e r a t i o n s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageIterations() gets the image iterations.
%
%  The format of the MagickGetImageIterations method is:
%
%      unsigned long MagickGetImageIterations(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetImageIterations(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->iterations);
}
/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %   M a g i c k G e t I m a g e M a t t e                                     %
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %
 %  MagickGetImageMatte() gets the image matte flag.  The flag is MagickTrue
 %  if the image supports an opacity (inverse of transparency) channel.
 %
 %  The format of the MagickGetImageMatte method is:
 %
 %      MagickBool MagickGetImageMatte(MagickWand *wand)
 %
 %  A description of each parameter follows:
 %
 %    o wand: The magick wand.
 %
 */
WandExport MagickBool MagickGetImageMatte(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->matte);
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
%      unsigned int MagickGetImageMatteColor(MagickWand *wand,
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  PixelSetQuantumColor(matte_color,&wand->image->matte_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e P a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImagePage() retrieves the image page size and offset used when
%  placing (e.g. compositing) the image.
%
%  The format of the MagickGetImagePage method is:
%
%      MagickGetImagePage(MagickWand *wand,
%                         unsigned long *width,
%                         unsigned long *height,
%                         long *x,
%                         long *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width, height: The region size.
%
%    o x, y: Offset (from top left) on base canvas image on
%      which to composite image data.
%
*/
WandExport unsigned int MagickGetImagePage(MagickWand *wand,
                                           unsigned long *width,
                                           unsigned long *height,
                                           long *x,
                                           long *y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  *width=wand->image->page.width;
  *height=wand->image->page.height;
  *x=wand->image->page.x;
  *y=wand->image->page.y;
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
%      MagickGetImagePixels(wand,0,0,640,1,"RGB",CharPixel,pixels);
%
%  The format of the MagickGetImagePixels method is:
%
%      unsigned int MagickGetImagePixels(MagickWand *wand,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType storage,
%        unsigned char *pixels)
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
  unsigned char *pixels)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=DispatchImage(wand->image,x_offset,y_offset,columns,rows,map,
    storage,pixels,&wand->exception);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e P r o f i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageProfile() returns the named image profile.
%
%  The format of the MagickGetImageProfile method is:
%
%      unsigned char *MagickGetImageProfile(MagickWand *wand,const char *name,
%        unsigned long *length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o name: Name of profile to return: ICC, IPTC, or generic profile.
%
%    o length: The length of the profile.
%
*/
WandExport unsigned char *MagickGetImageProfile(MagickWand *wand,
  const char *name,unsigned long *length)
{
  size_t
    profile_length=0;

  const unsigned char
    *profile=0;
  
  unsigned char
    *result=0;
  
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  *length=0;
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  profile=GetImageProfile(wand->image,name,&profile_length);
  if (profile)
    {
      result=MagickAllocateMemory(unsigned char *,profile_length);
      if (result)
        (void) memcpy(result,profile,profile_length);
    }
  *length=(unsigned long) profile_length;
  return (result);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedIntent);
    }
  return((RenderingIntent) wand->image->rendering_intent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e R e s o l u t i o n                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   MagickGetImageResolution() gets the image X & Y resolution.
%
%  The format of the  MagickGetImageResolution method is:
%
%      unsigned int  MagickGetImageResolution(MagickWand *wand,double *x,
%        double *y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x: The image x-resolution.
%
%    o y: The image y-resolution.
%
*/
WandExport unsigned int MagickGetImageResolution(MagickWand *wand,double *x,
  double *y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  *x=wand->image->x_resolution;
  *y=wand->image->y_resolution;
  return(True);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=SignatureImage(wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  attribute=GetImageAttribute(wand->image,"signature");
  if (attribute != (ImageAttribute *) NULL)
    return(AcquireString(attribute->value));
  CopyException(&wand->exception,&wand->image->exception);
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
%      MagickSizeType MagickGetImageSize(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport MagickSizeType MagickGetImageSize(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(GetBlobSize(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e T y p e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageType() gets the image type.
%
%  The format of the MagickGetImageType method is:
%
%      ImageType MagickGetImageType(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport ImageType MagickGetImageType(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedType);
    }
  return(GetImageType(wand->image,&wand->exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e S a v e d T y p e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageSavedType() gets the image type that will be used when the
%  image is saved. This may be different to the current image type, returned
%  by MagickGetImageType().
%
%  The format of the MagickGetImageSavedType method is:
%
%      ImageType MagickGetImageSavedType(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport ImageType MagickGetImageSavedType(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->image_info->type);
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
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedResolution);
    }
  return(wand->image->units);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t I m a g e V i r t u a l P i x e l M e t h o d           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetImageVirtualPixelMethod() returns the virtual pixel method for the
%  sepcified image.
%
%  The format of the MagickGetImageVirtualPixelMethod method is:
%
%      VirtualPixelMethod MagickGetImageVirtualPixelMethod(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport VirtualPixelMethod MagickGetImageVirtualPixelMethod(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    {
      (void) ThrowException(&wand->exception,WandError,WandContainsNoImages,
                            wand->id);
      return(UndefinedVirtualPixelMethod);
    }
  return(GetImageVirtualPixelMethod(wand->image));
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(wand->image->columns);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t N u m b e r I m a g e s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetNumberOfImages() returns the number of images associated with a
%  magick wand.
%
%  The format of the MagickGetNumberImages method is:
%
%      unsigned long MagickGetNumberImages(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned long MagickGetNumberImages(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(GetImageListLength(wand->image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t P a c k a g e N a m e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetPackageName() returns the ImageMagick package name.
%
%  The format of the MagickGetPackageName method is:
%
%      const char *MagickGetPackageName(void)
%
%
*/
WandExport const char *MagickGetPackageName(void)
{
  return(MagickPackageName);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t Q u a n t u m D e p t h                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetQuantumDepth() returns the ImageMagick quantum depth.
%
%  The format of the MagickGetQuantumDepth method is:
%
%      const char *MagickGetQuantumDepth(unsigned long *depth)
%
%  A description of each parameter follows:
%
%    o depth: The quantum depth is returned as a number.
%
%
*/
WandExport const char *MagickGetQuantumDepth(unsigned long *depth)
{
  *depth = (unsigned long) QuantumDepth;
  return(MagickQuantumDepth);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t R e l e a s e D a t e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetReleaseDate() returns the ImageMagick release date.
%
%  The format of the MagickGetReleaseDate method is:
%
%      const char *MagickGetReleaseDate(void)
%
*/
WandExport const char *MagickGetReleaseDate(void)
{
  return(MagickReleaseDate);
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
%   M a g i c k G e t S a m p l i n g F a c t o r s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetSamplingFactors() gets the horizontal and vertical sampling factor.
%
%  The format of the MagickGetSamplingFactors method is:
%
%      double *MagickGetSamplingFactors(MagickWand *wand,
%        unsigned long *number_factors)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o number_factors: The number of factors in the returned array.
%
*/
WandExport double *MagickGetSamplingFactors(MagickWand *wand,
  unsigned long *number_factors)
{
  double
    *sampling_factors;

  register const char
    *p;

  register long
    i;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  *number_factors=0;
  sampling_factors=(double *) NULL;
  if (wand->image_info->sampling_factor == (char *) NULL)
    return(sampling_factors);
  i=0;
  for (p=wand->image_info->sampling_factor; p != (char *) NULL; p=strchr(p,','))
  {
    while (((int) *p != 0) && ((isspace((int) ((unsigned char) *p)) != 0) ||
           (*p == ',')))
      p++;
    i++;
  }
  sampling_factors=MagickAllocateMemory(double *,(size_t) i*sizeof(double));
  if (sampling_factors == (double *) NULL)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      wand->image_info->filename);
  i=0;
  for (p=wand->image_info->sampling_factor; p != (char *) NULL; p=strchr(p,','))
  {
    while (((int) *p != 0) && ((isspace((int) ((unsigned char) *p)) != 0) ||
           (*p == ',')))
      p++;
    sampling_factors[i]=atof(p);
    i++;
  }
  *number_factors=(unsigned long) i;
  return(sampling_factors);
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
                                      unsigned long *columns,
                                      unsigned long *rows)
{
  RectangleInfo
    geometry;

  assert(wand != (const MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) memset(&geometry,0,sizeof(RectangleInfo));
  (void) GetGeometry(wand->image_info->size,&geometry.x,&geometry.y,
                     &geometry.width,&geometry.height);
  *columns=geometry.width;
  *rows=geometry.height;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t V e r s i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetVersion() returns the ImageMagick API version as a string and
%  as a number.
%
%  The format of the MagickGetVersion method is:
%
%      const char *MagickGetVersion(unsigned long *version)
%
%  A description of each parameter follows:
%
%    o version: The ImageMagick version is returned as a number.
%
*/
WandExport const char *MagickGetVersion(unsigned long *version)
{
  return(GetMagickVersion(version));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     M a g i c k H a l d C l u t I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The MagickHaldClutImage() method apply a color lookup table (Hald CLUT) to
%  the image.  The fundamental principle of the Hald CLUT algorithm is that
%  application of an identity CLUT causes no change to the input image,
%  but an identity CLUT image which has had its colors transformed in
%  some way (e.g. in Adobe Photoshop) may be used to implement an identical
%  transform on any other image.
%
%  The minimum CLUT level is 2, and the maximum depends on available memory
%  (largest successfully tested is 24).  A CLUT image is required to have equal
%  width and height. A CLUT of level 8 is an image of dimension 512x512, a CLUT
%  of level 16 is an image of dimension 4096x4096.  Interpolation is used so
%  extremely large CLUT images are not required.
%
%  GraphicsMagick provides an 'identity' coder which may be used to generate
%  identity HLUTs.  For example, reading from "identity:8" creates an identity
%  CLUT of order 8.
%
%  The Hald CLUT algorithm has been developed by Eskil Steenberg as described
%  at http://www.quelsolaar.com/technology/clut.html, and was adapted for
%  GraphicsMagick by Clément Follet with support from Cédric Lejeune of
%  Workflowers.
%
%  The format of the HaldClutImage method is:
%
%      MagickPassFail MagickHaldClutImage(MagickWand *wand,const MagickWand *clut_wand)
%
%  A description of each parameter follows:
%
%    o wand: The image wand.
%
%    o clut_wand: The color lookup table image wand
%
%
*/
WandExport MagickPassFail MagickHaldClutImage(MagickWand *wand,const MagickWand *clut_wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(clut_wand != (MagickWand *) NULL);
  assert(clut_wand->signature == MagickSignature);

  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  if (clut_wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  return HaldClutImage(wand->image,clut_wand->image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k H a s N e x t I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickHasNextImage() returns True if the wand has more images when
%  traversing the list in the forward direction
%
%  The format of the MagickHasNextImage method is:
%
%      unsigned int MagickHasNextImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickHasNextImage(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if (GetNextImageInList(wand->image) == (Image *) NULL)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k H a s P r e v i o u s I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickHasPreviousImage() returns True if the wand has more images when
%  traversing the list in the reverse direction
%
%  The format of the MagickHasPreviousImage method is:
%
%      unsigned int MagickHasPreviousImage(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport unsigned int MagickHasPreviousImage(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if (GetPreviousImageInList(wand->image) == (Image *) NULL)
    return(False);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) SetImageAttribute(wand->image,"label",(char *) NULL);
  status=SetImageAttribute(wand->image,"label",label);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k L e v e l I m a g e                                           %
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) MagickFormatString(levels,MaxTextExtent,"%g,%g,%g",black_point,
    white_point,gamma);
  status=LevelImage(wand->image,levels);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k L e v e l I m a g e C h a n n e l                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickLevelImageChannel() adjusts the levels of the specified channel of
%  the reference image by scaling the colors falling between specified white
%  and black points to the full available quantum range. The parameters
%  provided represent the black, mid, and white points. The black point
%  specifies the darkest color in the image. Colors darker than the black
%  point are set to zero.  Mid point specifies a gamma correction to apply
%  to the image.  White point specifies the lightest color in the image.
%  Colors brighter than the white point are set to the maximum quantum value.
%
%  The format of the MagickLevelImageChannel method is:
%
%      unsigned int MagickLevelImageChannel(MagickWand *wand,
%        const ChannelType channel,const double black_point,
%        const double gamma,const double white_point)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to level: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel.
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=LevelImageChannel(wand->image,channel,black_point,white_point,gamma);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if ((wand->images == (Image *) NULL) || (map_wand->images == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=MapImage(wand->image,map_wand->image,dither);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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

  ARG_NOT_USED(fuzz);
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  draw_info=CloneDrawInfo(wand->image_info,(DrawInfo *) NULL);
  (void) AcquireOnePixelByReference(wand->image,&target,x % wand->image->columns,
    y % wand->image->rows,&wand->exception);
  if (bordercolor != (PixelWand *) NULL)
    PixelGetQuantumColor(bordercolor,&target);
  status=MatteFloodfillImage(wand->image,target,opacity,x,y,
    bordercolor != (PixelWand *) NULL ? FillToBorderMethod : FloodfillMethod);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) MagickFormatString(modulate,MaxTextExtent,"%g,%g,%g",brightness,
    saturation,hue);
  status=ModulateImage(wand->image,modulate);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
    {
      (void) CloneString(&montage_info->font,font);
      MagickFreeMemory(font);
    }
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
  return(CloneMagickWandWithImages(wand,montage_image));
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
  return(CloneMagickWandWithImages(wand,morph_image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o s a i c I m a g e s                                       %
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
  return(CloneMagickWandWithImages(wand,mosaic_image));
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=NegateImage(wand->image,gray);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
%  MagickNegateImageChannel() negates the colors in the specified channel of the
%  reference image.  The Grayscale option means that only grayscale values
%  within the image are negated.  Note that the Grayscale option has no
%  effect for GraphicsMagick.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the MagickNegateImageChannel method is:
%
%      unsigned int MagickNegateImageChannel(MagickWand *wand,
%        const ChannelType channel,const unsigned int gray)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel.
%
%    o gray: If True, only negate grayscale pixels within the image.
%
*/
WandExport unsigned int MagickNegateImageChannel(MagickWand *wand,
  const ChannelType channel,const unsigned int gray)
{
  ARG_NOT_USED(gray);
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return QuantumOperatorImage(wand->image,channel,NegateQuantumOp,
                              0,&wand->exception);
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
%  MagickNextImage() associates the next image in the image list with a magick
%  wand.  True is returned if the Wand iterated to a next image, or False is
%  returned if the wand did not iterate to a next image.
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if (GetNextImageInList(wand->image) == (Image *) NULL)
    {
      wand->iterator=True;
      return(False);
    }
  if (wand->iterator != False)
    {
      wand->iterator=False;
      return(True);
    }
  wand->image=GetNextImageInList(wand->image);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=NormalizeImage(wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  PixelGetQuantumColor(target,&target_pixel);
  PixelGetQuantumColor(fill,&fill_pixel);
  wand->image->fuzz=fuzz;
  status=OpaqueImage(wand->image,target_pixel,fill_pixel);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %   M a g i c k O p e r a t o r I m a g e C h a n n e l                       %
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %
 %  MagickOperatorImageChannel() performs the requested arithmetic,
 %  bitwise-logical, or value operation on the selected channels of
 %  the entire image.  The AllChannels channel option operates on all
 %  color channels whereas the GrayChannel channel option treats the
 %  color channels as a grayscale intensity.
 %
 %  These operations are on the DirectClass pixels of the image and do not
 %  update pixel indexes or colormap.
 %
 %  The format of the MagickOperatorImageChannel method is:
 %
 %      MagickPassFail MagickOperatorImageChannel(MagickWand *wand,
 %        const ChannelType channel,const QuantumOperator quantum_operator,
 %        const double rvalue)
 %
 %  A description of each parameter follows:
 %
 %    o wand: The magick wand.
 %
 %    o channel: Channel to operate on (RedChannel, CyanChannel,
 %        GreenChannel, MagentaChannel, BlueChannel, YellowChannel,
 %        OpacityChannel, BlackChannel, MatteChannel, AllChannels,
 %        GrayChannel).  The AllChannels type only updates color
 %        channels.  The GrayChannel type treats the color channels
 %        as if they represent an intensity.
 %
 %    o quantum_operator: Operator to use (AddQuantumOp,AndQuantumOp,
 %        AssignQuantumOp, DepthQuantumOp, DivideQuantumOp, GammaQuantumOp,
 %        LShiftQuantumOp, MultiplyQuantumOp,  NegateQuantumOp,
 %        NoiseGaussianQuantumOp, NoiseImpulseQuantumOp,
 %        NoiseLaplacianQuantumOp, NoiseMultiplicativeQuantumOp,
 %        NoisePoissonQuantumOp, NoiseRandomQuantumOp, NoiseUniformQuantumOp,
 %        OrQuantumOp, RShiftQuantumOp, SubtractQuantumOp,
 %        ThresholdBlackQuantumOp, ThresholdQuantumOp, ThresholdWhiteQuantumOp,
 %        ThresholdBlackNegateQuantumOp, ThresholdWhiteNegateQuantumOp,
 %        XorQuantumOp).
 %
 %    o rvalue: Operator argument.
 %
 */
WandExport unsigned int
MagickOperatorImageChannel(MagickWand *wand,
                           const ChannelType channel,
                           const QuantumOperator quantum_operator,
                           const double rvalue)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return QuantumOperatorImage(wand->image,channel,quantum_operator,
                              rvalue,&wand->exception);
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
  (void) strlcpy(ping_info->filename,filename,MaxTextExtent);
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
%   M a g i c k P r e v i e w I m a g e s                                     %
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
  ARG_NOT_USED(preview);
  ThrowWandException(WandError,WandAPINotImplemented,"MagickPreviewImages");
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
  if ((wand->images == (Image *) NULL) ||
      (wand->image->previous == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if (GetPreviousImageInList(wand->image) == (Image *) NULL)
    {
      wand->iterator=True;
      return(False);
    }
  if (wand->iterator != False)
    {
      wand->iterator=False;
      return(True);
    }
  wand->image=GetPreviousImageInList(wand->image);
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
  const unsigned char *profile,const unsigned long length)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=ProfileImage(wand->image,name,(unsigned char *)profile,length,True);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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

  ARG_NOT_USED(dither);
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  quantize_info->number_colors=number_colors;
  quantize_info->tree_depth=treedepth;
  quantize_info->colorspace=colorspace;
  quantize_info->measure_error=measure_error;
  status=QuantizeImage(quantize_info,wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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

  ARG_NOT_USED(dither);
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  quantize_info->number_colors=number_colors;
  quantize_info->tree_depth=treedepth;
  quantize_info->colorspace=colorspace;
  quantize_info->measure_error=measure_error;
  status=QuantizeImages(quantize_info,wand->images);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  font_metrics=MagickAllocateMemory(double *,7*sizeof(double));
  if (font_metrics == (double *) NULL)
    return((double *) NULL);
  draw_info=DrawPeekGraphicContext(drawing_wand);
  if (draw_info == (DrawInfo *) NULL)
    {
      MagickFreeMemory(font_metrics);
      return((double *) NULL);
    }
  (void) CloneString(&draw_info->text,text);
  status=GetTypeMetrics(wand->image,draw_info,&metrics);
  DestroyDrawInfo(draw_info);
  if (status == False)
    {
      CopyException(&wand->exception,&wand->image->exception);
      MagickFreeMemory(font_metrics);
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
%   M a g i c k Q u e r y F o n t s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickQueryFonts() returns any font that match the specified pattern.
%
%  The format of the MagickQueryFonts function is:
%
%      char **MagickQueryFonts(const char *pattern,unsigned long *number_fonts)
%
%  A description of each parameter follows:
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_fonts:  This integer returns the number of fonts in the list.
%
%
*/
WandExport char **MagickQueryFonts(const char *pattern,
  unsigned long *number_fonts)
{
  return(GetTypeList(pattern,number_fonts));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k Q u e r y F o r m a t s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickQueryFormats() returns any image formats that match the specified
%  pattern.
%
%  The format of the MagickQueryFormats function is:
%
%      char **MagickQueryFormats(const char *pattern,
%        unsigned long *number_formats)
%
%  A description of each parameter follows:
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_formats:  This integer returns the number of image formats in the
%      list.
%
%
*/
WandExport char **MagickQueryFormats(const char *pattern,
  unsigned long *number_formats)
{
  ExceptionInfo
    exception;

  char
    **format_array;

  int
    entries,
    i;

  MagickInfo **
    magick_info;

  ARG_NOT_USED(pattern);
  *number_formats=0;
  GetExceptionInfo(&exception);
  magick_info=GetMagickInfoArray(&exception);
  DestroyExceptionInfo(&exception);
  if (!magick_info)
    return 0;

  for (entries = 0; magick_info[entries] != 0; entries++);
  if (!entries)
    {
      MagickFreeMemory(magick_info);
      return 0;
    }

  format_array=MagickAllocateMemory(char **,(entries+1)*sizeof(char *));
  if (!format_array)
    {
      MagickFreeMemory(magick_info);
      return 0;
    }

  for (i=0 ; i < entries ; i++)
    format_array[i]=AcquireString(magick_info[i]->name);
  format_array[i]=0;

  MagickFreeMemory(magick_info);
  *number_formats=entries;
  return (format_array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R a d i a l B l u r I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRadialBlurImage() radial blurs an image.
%
%  The format of the MagickRadialBlurImage method is:
%
%      unsigned int MagickRadialBlurImage(MagickWand *wand,const double angle)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o angle: The angle of the blur in degrees.
%
*/
WandExport unsigned int MagickRadialBlurImage(MagickWand *wand,
  const double angle)
{
#if defined(NOT_IMPLEMENTED)
  Image
    *blur_image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  blur_image=RadialBlurImage(wand->image,angle,&wand->exception);
  if (blur_image == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,blur_image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
#else
  ARG_NOT_USED(angle);
  ThrowWandException(WandError,WandAPINotImplemented,"MagickRadialBlurImage");
#endif
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
%        const unsigned int raise_flag)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width,height,x,y:  Define the dimensions of the area to raise.
%
%    o raise_flag: A value other than zero creates a 3-D raise effect,
%      otherwise it has a lowered effect.
%
*/
WandExport unsigned int MagickRaiseImage(MagickWand *wand,
  const unsigned long width,const unsigned long height,const long x,
  const long y,const unsigned int raise_flag)
{
  RectangleInfo
    raise_info;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  raise_info.width=width;
  raise_info.height=height;
  raise_info.x=x;
  raise_info.y=y;
  status=RaiseImage(wand->image,&raise_info,raise_flag);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  (void) strlcpy(read_info->filename,filename,MaxTextExtent);
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
%      unsigned int MagickReadImageBlob(MagickWand *wand,
%        const unsigned char *blob,const size_t length)
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
WandExport unsigned int MagickReadImageBlob(MagickWand *wand,
  const unsigned char *blob,const size_t length)
{
  Image
    *images;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  images=BlobToImage(wand->image_info,blob,length,&wand->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  MagickFreeMemory(memory);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  DeleteImageFromList(&wand->image);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e m o v e I m a g e P r o f i l e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRemoveImageProfile() removes the named image profile and returns it.
%
%  The format of the MagickRemoveImageProfile method is:
%
%      unsigned char *MagickRemoveImageProfile(MagickWand *wand,const char *name,
%        unsigned long *length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o name: Name of profile to return: ICC, IPTC, or generic profile.
%
%    o length: The length of the profile.
%
*/
WandExport unsigned char *MagickRemoveImageProfile(MagickWand *wand,
  const char *name,unsigned long *length)
{
  const unsigned char
    *profile;

  unsigned char
    *cloned_profile;

  size_t
    profile_length;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  *length=0;
  /*
    Obtain pointer to existing image profile
  */
  profile=GetImageProfile(wand->image,name,&profile_length);
  if (!profile || !profile_length)
    return 0;
  /*
    Clone profile
  */
  *length=(unsigned long) profile_length;
  cloned_profile=MagickAllocateMemory(unsigned char *,profile_length);
  if (!cloned_profile)
    return 0;
  (void) memcpy(cloned_profile,profile,profile_length);
  profile=0;
  /*
    Remove image profile
  */
  (void) DeleteImageProfile(wand->image,name);
  return cloned_profile;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e s e t I t e r a t o r                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReset() resets the wand iterator.  Use it in conjunction
%  with MagickNextImage() to iterate over all the images in a wand
%  container.
%
%  The format of the MagickReset method is:
%
%      void MagickResetIterator(MagickWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
*/
WandExport void MagickResetIterator(MagickWand *wand)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->iterator=True;
  wand->image=wand->images;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%   M a g i c k S e p a r a t e I m a g e C h a n n e l                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickChannelImage() separates a channel from the image and returns a
%  grayscale image.  A channel is a particular color component of each pixel
%  in the image.
%
%  The format of the MagickChannelImage method is:
%
%      unsigned int MagickSeparateImageChannel(MagickWand *wand,
%        const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: Identify which channel to extract: RedChannel, GreenChannel,
%      BlueChannel, OpacityChannel, CyanChannel, MagentaChannel, YellowChannel,
%      BlackChannel.
%
*/
WandExport unsigned int MagickSeparateImageChannel(MagickWand *wand,
  const ChannelType channel)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=ChannelImage(wand->image,channel);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t C o m p r e s s i o n Q u a l i t y                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetCompressionQuality() sets the image quality factor, which
%  determines compression options when saving the file.
%
%  For the JPEG and MPEG image formats, quality is 0 (lowest image
%  quality and highest compression) to 100 (best quality but least
%  effective compression).  The default quality is 75.  Use the
%  -sampling-factor option to specify the factors for chroma
%  downsampling.  To use the same quality value as that found by the
%  JPEG decoder, use the -define jpeg:preserve-settings flag.
% 
%  For the MIFF image format, and the TIFF format while using ZIP
%  compression, quality/10 is the zlib compres- sion level, which is 0
%  (worst but fastest compression) to 9 (best but slowest). It has no
%  effect on the image appearance, since the compression is always
%  lossless.
% 
%  For the JPEG-2000 image format, quality is mapped using a non-linear
%  equation to the compression ratio required by the Jasper library.
%  This non-linear equation is intended to loosely approximate the
%  quality provided by the JPEG v1 format.  The default quality value 75
%  results in a request for 16:1 compression. The quality value 100
%  results in a request for non-lossy compres- sion.
% 
%  For the MNG and PNG image formats, the quality value sets the zlib
%  compression level (quality / 10) and filter-type (quality % 10).
%  Compression levels range from 0 (fastest compression) to 100 (best
%  but slowest).  For compression level 0, the Huffman-only strategy is
%  used, which is fastest but not necessarily the worst compression.  If
%  filter-type is 4 or less, the specified filter-type is used for all
%  scanlines:
% 
%       0) none
%       1) sub
%       2) up
%       3) average
%       4) Paeth
% 
%  If filter-type is 5, adaptive filtering is used when quality is
%  greater than 50 and the image does not have a color map, otherwise no
%  filtering is used.
% 
%  If filter-type is 6, adaptive filtering with minimum-
%  sum-of-absolute-values is used.
% 
%  Only if the output is MNG, if filter-type is 7, the LOCO color
%  transformation and adaptive filtering with
%  minimum-sum-of-absolute-values are used.
% 
%  The default is quality is 75, which means nearly the best compression
%  with adaptive filtering.  The quality setting has no effect on the
%  appearance of PNG and MNG images, since the compression is always
%  lossless.
% 
%  For further information, see the PNG specification.
% 
%  When writing a JNG image with transparency, two quality values are
%  required, one for the main image and one for the grayscale image that
%  conveys the opacity channel.  These are written as a single integer
%  equal to the main image quality plus 1000 times the opacity quality.
%  For example, if you want to use quality 75 for the main image and
%  quality 90 to compress the opacity data, use -quality 90075.
% 
%  For the PNM family of formats (PNM, PGM, and PPM) specify a quality
%  factor of zero in order to obtain the ASCII variant of the
%  format. Note that -compress none used to be used to trigger ASCII
%  output but provided the opposite result of what was expected as
%  compared with other formats.
%
%  The format of the MagickSetCompressionQuality method is:
%
%      unsigned int MagickSetCompressionQuality(MagickWand *wand,
%        const unsigned long quality)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o delay: The image quality.
%
*/
WandExport unsigned int MagickSetCompressionQuality(MagickWand *wand,
  const unsigned long quality)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->quality=quality;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t D e p t h                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetDepth() sets the sample depth to be used when reading from a
%  raw image or a format which requires that the depth be specified in
%  advance by the user.
%
%  The format of the MagickSetDepth method is:
%
%      unsigned int MagickSetDepth(MagickWand *wand,const size_t depth)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o depth: The sample depth.
%
*/
WandExport unsigned int MagickSetDepth(MagickWand *wand,const size_t depth)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->depth = (unsigned long) depth;
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
  (void) strlcpy(wand->image_info->filename,filename,MaxTextExtent);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t F o r m a t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetFormat() sets the file or blob format (e.g. "BMP") to be used
%  when a file or blob is read.  Usually this is not necessary because
%  GraphicsMagick is able to auto-detect the format based on the file
%  header (or the file extension), but some formats do not use a unique
%  header or the selection may be ambigious. Use MagickSetImageFormat()
%  to set the format to be used when a file or blob is to be written.
%
%  The format of the MagickSetFormat method is:
%
%      unsigned int MagickSetFormat(MagickWand *wand,const char *format)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o filename: The file or blob format.
%
*/
WandExport unsigned int MagickSetFormat(MagickWand *wand,const char *format)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) strlcpy(wand->image_info->magick,format,MaxTextExtent);
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
%  MagickSetImage() replaces the last image returned by MagickSetImageIndex(),
%  MagickNextImage(), MagickPreviousImage() with the images from the specified
%  wand.
%
%  The format of the MagickSetImage method is:
%
%      unsigned int MagickSetImage(MagickWand *wand,const MagickWand *set_wand)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o set_wand: The set_wand wand.
%
*/
WandExport unsigned int MagickSetImage(MagickWand *wand,
  const MagickWand *set_wand)
{
  Image
    *images;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(set_wand != (MagickWand *) NULL);
  assert(set_wand->signature == MagickSignature);
  if (set_wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  images=CloneImageList(set_wand->images,&wand->exception);
  if (images == (Image *) NULL)
    return(False);
  ReplaceImageInList(&wand->image,images);
  wand->images=GetFirstImageInList(wand->image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e A t t r i b u t e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageAttribute sets an image attribute
%
%  The format of the MagickSetImageAttribute method is:
%
%      unsigned int MagickSetImageAttribute(MagickWand *wand, const char *name, const char *value)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o name: The name of the attribute
%
%    o value: The value of the attribute
%
*/
WandExport unsigned int MagickSetImageAttribute(MagickWand *wand, const char *name, 
  const char *value)
{
  unsigned int status;
  
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status = SetImageAttribute(wand->image,name,value);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  PixelGetQuantumColor(border,&wand->image->border_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e C o l o r m a p C o l o r                     %
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if ((wand->image->colormap == (PixelPacket *) NULL) ||
      (index >= wand->image->colors))
    ThrowWandException(WandError,InvalidColormapIndex,(char *) NULL);
  PixelGetQuantumColor(color,wand->image->colormap+index);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e C o l o r s p a c e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageColorspace() sets the image colorspace.
%
%  The format of the MagickSetImageColorspace method is:
%
%      unsigned int MagickSetImageColorspace(MagickWand *wand,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o colorspace: The image colorspace:   UndefinedColorspace, RGBColorspace,
%      GRAYColorspace, TransparentColorspace, OHTAColorspace, XYZColorspace,
%      YCbCrColorspace, YCCColorspace, YIQColorspace, YPbPrColorspace,
%      YPbPrColorspace, YUVColorspace, CMYKColorspace, sRGBColorspace,
%      HSLColorspace, or HWBColorspace.
%
*/
WandExport unsigned int MagickSetImageColorspace(MagickWand *wand,
  const ColorspaceType colorspace)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(TransformColorspace(wand->image,colorspace));
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->delay=delay;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e C h a n n e l D e p t h                       %
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
%      BlackChannel.
%
%    o depth: The image depth in bits.
%
*/
WandExport unsigned int MagickSetImageChannelDepth(MagickWand *wand,
  const ChannelType channel,const unsigned long depth)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) SetImageChannelDepth(wand->image,channel,depth);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) SetImageDepth(wand->image,depth);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e D i s p o s e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageDispose() sets the image disposal method.
%
%  The format of the MagickSetImageDispose method is:
%
%      unsigned int MagickSetImageDispose(MagickWand *wand,
%        const DisposeType dispose)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o dispose: The image disposeal type.
%
*/
WandExport unsigned int MagickSetImageDispose(MagickWand *wand,
  const DisposeType dispose)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->dispose=dispose;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) strlcpy(wand->image->filename,filename,MaxTextExtent);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e F o r m a t                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageFormat() sets the format of a particular image in a
%  sequence.  The format is designated by a magick string (e.g. "GIF").
%
%  The format of the MagickSetImageFormat method is:
%
%      unsigned int MagickSetImageFormat(MagickWand *wand,
%        const char *format)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o magick: The image format.
%
*/
WandExport unsigned int MagickSetImageFormat(MagickWand *wand,
  const char *format)
{
  const char
    *magick = "";

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  if (format != (const char *) NULL)
    magick=format;
  (void) strlcpy(wand->image->magick,magick,
                          sizeof(wand->image->magick));
  return MagickTrue;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e F u z z                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageFuzz() sets the color comparison fuzz factor.  Colors
%  closer than the fuzz factor are considered to be the same when comparing
%  colors.  Note that some other functions such as MagickColorFloodfillImage()
%  implicitly set this value.
%
%  The format of the MagickSetImageFuzz method is:
%
%      unsigned int MagickSetImageFuzz(MagickWand *wand,const double fuzz)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o fuzz: The color comparison fuzz factor
%
*/
WandExport unsigned int
MagickSetImageFuzz(MagickWand *wand,const double fuzz)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->fuzz=fuzz;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->gamma=gamma;
  return(True);
}

/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %   M a g i c k S e t I m a g e G e o m e t r y                               %
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %
 %  MagickSetImageGeometry() sets the image geometry string. 
 %
 %  The format of the MagickSetImageGeometry method is:
 %
 %      unsigned int MagickSetImageGeometry(MagickWand *wand,
 %        const char *geometry)
 %
 %  A description of each parameter follows:
 %
 %    o wand: The magick wand.
 %
 %    o geometry: The image geometry.
 %
 */
WandExport unsigned int MagickSetImageGeometry(MagickWand *wand,
                                               const char *geometry)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) CloneString(&wand->image->geometry,geometry);

  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e G r a v i t y                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageGravity() sets the image gravity.  This is used
%  when evaluating regions defined by a geometry and the image
%  dimensions.  It may be used in conjunction with operations which
%  use a geometry parameter to adjust the x, y parameters of the
%  final operation. Gravity is used in composition to determine where
%  the image should be placed within the defined geometry region.
%  It may be used with montage to effect placement of the image within
%  the tile.
%
%  The format of the MagickSetImageGravity method is:
%
%      unsigned int MagickSetImageGravity(MagickWand *wand,const GravityType)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o gravity: The image gravity.  Available values are ForgetGravity,
%        NorthWestGravity, NorthGravity, NorthEastGravity, WestGravity,
%        CenterGravity, EastGravity, SouthWestGravity, SouthGravity,
%        SouthEastGravity, and StaticGravity
%
*/
WandExport unsigned int MagickSetImageGravity(MagickWand *wand,
                                              const GravityType gravity)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->gravity=gravity;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->chromaticity.green_primary.x=x;
  wand->image->chromaticity.green_primary.y=y;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e I n d e x                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageIndex() set the current image to the position of the list
%  specified with the index parameter.
%
%  The format of the MagickSetImageIndex method is:
%
%      unsigned int MagickSetImageIndex(MagickWand *wand,const long index)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o index: The scene number.
%
*/
WandExport unsigned int MagickSetImageIndex(MagickWand *wand,const long index)
{
  Image
    *image;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    return(False);
  image=GetImageFromList(wand->images,index);
  if (image == (Image *) NULL)
    return(False);
  wand->image=image;
  wand->iterator=False;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e I n t e r l a c e S c h e m e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageInterlaceScheme() sets the image interlace scheme.  Please
%  use SetInterlaceScheme() instead to change the interlace scheme used when
%  writing the image.
%
%  The format of the MagickSetImageInterlaceScheme method is:
%
%      unsigned int MagickSetImageInterlaceScheme(MagickWand *wand,
%        const InterlaceType interlace_scheme)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o interlace_scheme: The image interlace scheme: NoInterlace, LineInterlace,
%      PlaneInterlace, PartitionInterlace.
%
*/
WandExport unsigned int MagickSetImageInterlaceScheme(MagickWand *wand,
  const InterlaceType interlace_scheme)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->interlace=interlace_scheme;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e I t e r a t i o n s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageIterations() sets the image iterations.
%
%  The format of the MagickSetImageIterations method is:
%
%      unsigned int MagickSetImageIterations(MagickWand *wand,
%        const unsigned long iterations)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o delay: The image delay in 1/100th of a second.
%
*/
WandExport unsigned int MagickSetImageIterations(MagickWand *wand,
  const unsigned long iterations)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->iterations=iterations;
  return(True);
}

/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %   M a g i c k S e t I m a g e M a t t e                                     %
 %                                                                             %
 %                                                                             %
 %                                                                             %
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %
 %  MagickSetImageMatte() sets the image matte flag.  The image opacity
 %  (inverse of transparency) channel is enabled if the matte flag is
 %  True.
 %
 %  The format of the MagickSetImageMatte method is:
 %
 %      unsigned int MagickSetImageMatte(MagickWand *wand,
 %        const unsigned int matte)
 %
 %  A description of each parameter follows:
 %
 %    o wand: The magick wand.
 %
 %    o matte: The image matte.
 %
 */
WandExport unsigned int MagickSetImageMatte(MagickWand *wand,
                                            const unsigned int matte)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->images->matte = matte;

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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  PixelGetQuantumColor(matte,&wand->image->matte_color);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e O p t i o n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageOption() associates one or options with a particular image
%  format (.e.g MagickSetImageOption(wand,"jpeg","preserve-settings","true").
%
%  The format of the MagickSetImageOption method is:
%
%      unsigned int MagickSetImageOption(MagickWand *wand,const char *format,
%        const char *key,const char *value)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o format: The image format.
%
%    o key:  The key.
%
%    o value:  The value.
%
*/
WandExport unsigned int MagickSetImageOption(MagickWand *wand,
  const char *format,const char *key,const char *value)
{
  char
    option[MaxTextExtent];

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) MagickFormatString(option,MaxTextExtent,"%.1024s:%.1024s=%.1024s",
    format,key,value);
  (void) AddDefinitions(wand->image_info,option,&wand->exception);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e P a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImagePage() sets the image page size and offset used when
%  placing (e.g. compositing) the image.  Pass all zeros for the
%  default placement.
%
%  The format of the MagickSetImagePage method is:
%
%       unsigned int MagickSetImagePage(MagickWand *wand,
%                                       const unsigned long width,
%                                       const unsigned long height,
%                                       const long x,
%                                       const long y)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o width, height: The region size.
%
%    o x, y: Offset (from top left) on base canvas image on
%      which to composite image data.
%
*/
WandExport unsigned int MagickSetImagePage(MagickWand *wand,
                                           const unsigned long width,
                                           const unsigned long height,
                                           const long x,
                                           const long y)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->page.width=width;
  wand->image->page.height=height;
  wand->image->page.x=x;
  wand->image->page.y=y;
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
%       unsigned int MagickSetImagePixels(MagickWand *wand,
%          const long x_offset,const long y_offset,const unsigned long columns,
%          const unsigned long rows,const char *map,const StorageType storage,
%          unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_offset, y_offset: Offset (from top left) on base canvas image on
%      which to composite image data.
%
%    o columns, rows: Dimensions of image.
%
%    o map:  This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, or I = intensity
%      (for grayscale). Specify "P" = pad, to skip over a quantum which is
%      intentionally ignored. Creation of an alpha channel for CMYK images
%      is currently not supported.
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
  unsigned char *pixels)
{
  Image
    *constitute_image,
    *image;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);

  image=wand->image;
  constitute_image=
    ConstituteImage(columns,rows,map,storage,pixels,&image->exception);
  if (constitute_image)
    {
      (void) CompositeImage(image,CopyCompositeOp,constitute_image,x_offset,
                            y_offset);
      DestroyImage(constitute_image);
      status = (image->exception.severity == UndefinedException);
    }
  else
    {
      status = False;
    }

  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e P r o f i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageProfile() adds a named profile to the magick wand.  If a
%  profile with the same name already exists, it is replaced.  This method
%  differs from the MagickProfileImage() method in that it does not apply any
%  CMS color profiles.
%
%  The format of the MagickSetImageProfile method is:
%
%      unsigned int MagickSetImageProfile(MagickWand *wand,const char *name,
%        const unsigned char *profile,const unsigned long length)
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
WandExport unsigned int MagickSetImageProfile(MagickWand *wand,const char *name,
  const unsigned char *profile,const unsigned long length)
{
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=SetImageProfile(wand->image,name,profile, length);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->rendering_intent=rendering_intent;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->x_resolution=x_resolution;
  wand->image->y_resolution=y_resolution;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e S c e n e                                     %
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->scene=scene;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e T y p e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageType() sets the image type.
%
%  The format of the MagickSetImageType method is:
%
%      unsigned int MagickSetImageType(MagickWand *wand,
%        const ImageType image_type)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o image_type: The image type:   UndefinedType, BilevelType, GrayscaleType,
%      GrayscaleMatteType, PaletteType, PaletteMatteType, TrueColorType,
%      TrueColorMatteType, ColorSeparationType, ColorSeparationMatteType,
%      or OptimizeType.
%
*/
WandExport unsigned int MagickSetImageType(MagickWand *wand,
  const ImageType image_type)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image_info->type=image_type;
  return(SetImageType(wand->image,image_type));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e S a v e d T y p e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageSavedType() sets the image type that will be used when the
%  image is saved.
%
%  The format of the MagickSetImageSavedType method is:
%
%      unsigned int MagickSetImageSavedType(MagickWand *wand,
%        const ImageType image_type)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o image_type: The image type:   UndefinedType, BilevelType, GrayscaleType,
%      GrayscaleMatteType, PaletteType, PaletteMatteType, TrueColorType,
%      TrueColorMatteType, ColorSeparationType, ColorSeparationMatteType,
%      or OptimizeType.
%
*/
WandExport unsigned int MagickSetImageSavedType(MagickWand *wand,
  const ImageType image_type)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->type = image_type;
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->units=units;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I m a g e V i r t u a l P i x e l M e t h o d           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetImageVirtualPixelMethod() sets the image virtual pixel method.
%
%  The format of the MagickSetImageVirtualPixelMethod method is:
%
%      unsigned int MagickSetImageVirtualPixelMethod(MagickWand *wand,
%        const VirtualPixelMethod method)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o method: The image virtual pixel method : UndefinedVirtualPixelMethod,
%      ConstantVirtualPixelMethod,  EdgeVirtualPixelMethod,
%      MirrorVirtualPixelMethod, or TileVirtualPixelMethod.
%
*/
WandExport unsigned int MagickSetImageVirtualPixelMethod(MagickWand *wand,
  const VirtualPixelMethod method)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return(SetImageVirtualPixelMethod(wand->image,method));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t I n t e r l a c e S c h e m e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetInterlaceScheme() sets the interlace scheme used when writing
%  the image.
%
%  The format of the MagickSetInterlaceScheme method is:
%
%      unsigned int MagickSetInterlaceScheme(MagickWand *wand,
%        const InterlaceType interlace_scheme)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o interlace_scheme: The image interlace scheme: NoInterlace, LineInterlace,
%      PlaneInterlace, PartitionInterlace.
%
*/
WandExport unsigned int MagickSetInterlaceScheme(MagickWand *wand,
  const InterlaceType interlace_scheme)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->interlace=interlace_scheme;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t R e s o l u t i o n                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetResolution() sets the resolution (density) of the magick wand.
%  Set it before you read an EPS, PDF, or Postscript file in order to
%  influence the size of the returned image, or after an image has already
%  been created to influence the rendered image size when used with
%  typesetting software.
%
%  Also see MagickSetResolutionUnits() which specifies the units to use for
%  the image resolution.
%
%  The format of the MagickSetResolution method is:
%
%      unsigned int MagickSetResolution(MagickWand *wand,
%              const double x_resolution,const double y_resolution)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o x_resolution: The horizontal resolution
%
%    o y_resolution: The vertical reesolution
%
%
*/
WandExport unsigned int
MagickSetResolution(MagickWand *wand,
		    const double x_resolution,const double y_resolution)
{
  char
    geometry[MaxTextExtent];

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) MagickFormatString(geometry,MaxTextExtent,"%gx%g",x_resolution,y_resolution);
  (void) CloneString(&wand->image_info->density,geometry);
  if (wand->image != (Image *) NULL)
    {
      wand->image->x_resolution=x_resolution;
      wand->image->y_resolution=y_resolution;
    }
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t R e s o l u t i o n U n i t s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetResolutionUnits() sets the resolution units of the magick wand.
%  It should be used in conjunction with MagickSetResolution().
%  This method works both before and after an image has been read.
%
%  Also see MagickSetImageUnits() which specifies the units which apply to
%  the image resolution setting after an image has been read.
%
%  The format of the MagickSetResolutionUnits method is:
%
%      unsigned int MagickSetResolutionUnits(MagickWand *wand,
%                                            const ResolutionType units)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o units: The image units of resolution : Undefinedresolution,
%      PixelsPerInchResolution, or PixelsPerCentimeterResolution.
%
*/
WandExport unsigned int
MagickSetResolutionUnits(MagickWand *wand,const ResolutionType units)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->image_info->units=units;
  if (wand->image != (Image *) NULL)
    wand->image->units=units;
  return(MagickPass);
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
%    o type: The type of resource: DiskResource, FileResource, MapResource,
%            MemoryResource, PixelsResource, ThreadsResource, WidthResource,
%            HeightResource.
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
%   M a g i c k S e t S a m p l i n g F a c t o r s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetSamplingFactors() sets the image sampling factors.
%
%  The format of the MagickSetSamplingFactors method is:
%
%      unsigned int MagickSetSamplingFactors(MagickWand *wand,
%        const unsigned long number_factors,const double *sampling_factors)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o number_factoes: The number of factors.
%
%    o sampling_factors: An array of doubles representing the sampling factor
%      for each color component (in RGB order).
%
*/
WandExport unsigned int MagickSetSamplingFactors(MagickWand *wand,
  const unsigned long number_factors,const double *sampling_factors)
{
  char
    sampling_factor[MaxTextExtent];

  register long
    i;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  MagickFreeMemory(wand->image_info->sampling_factor);
  if (number_factors == 0)
    return(True);
  for (i=0; i < (long) (number_factors-1); i++)
  {
    (void) MagickFormatString(sampling_factor,MaxTextExtent,"%g,",
      sampling_factors[i]);
    (void) ConcatenateString(&wand->image_info->sampling_factor,
      sampling_factor);
  }
  (void) MagickFormatString(sampling_factor,MaxTextExtent,"%g",
    sampling_factors[i]);
  (void) ConcatenateString(&wand->image_info->sampling_factor,sampling_factor);
  return(True);
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
  (void) MagickFormatString(geometry,MaxTextExtent,"%lux%lu",columns,rows);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  (void) CloneString(&wand->image_info->authenticate,passphrase);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%   M a g i c k S h e a r I m a g e                                         %
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=SolarizeImage(wand->image,threshold);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if ((wand->images == (Image *) NULL) ||
      (watermark_wand->images == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  wand->image->offset=offset;
  stegano_image=SteganoImage(wand->image,watermark_wand->image,
    &wand->exception);
  if (stegano_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWandWithImages(wand,stegano_image));
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
  if ((wand->images == (Image *) NULL) || (offset_wand->images == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  stereo_image=StereoImage(wand->image,offset_wand->image,&wand->exception);
  if (stereo_image == (Image *) NULL)
    return((MagickWand *) NULL);
  return(CloneMagickWandWithImages(wand,stereo_image));
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
%  MagickStripImage() removes all profiles and text attributes from the image.
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
  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=StripImage(wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%   M a g i c k T e x t u r e I m a g e                                       %
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
  if ((wand->images == (Image *) NULL) ||
      (texture_wand->images == (Image *) NULL))
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  texture_image=CloneImage(wand->image,0,0,True,&wand->exception);
  if (texture_image == (Image *) NULL)
    return((MagickWand *) NULL);
  status=TextureImage(texture_image,texture_wand->image);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(CloneMagickWandWithImages(wand,texture_image));
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  status=ThresholdImage(wand->image,threshold);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T h r e s h o l d I m a g e C h a n n e l                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickThresholdImageChannel() changes the value of individual pixel
%  component based on the intensity of each pixel compared to threshold.  The
%  result is a high-contrast, two color image.
%
%  The format of the MagickThresholdImage method is:
%
%      unsigned int MagickThresholdImageChannel(MagickWand *wand,
%        const ChannelType channel,const double threshold)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o channel: The channel.
%
%    o threshold: Define the threshold value.
%
*/
WandExport unsigned int MagickThresholdImageChannel(MagickWand *wand,
  const ChannelType channel,const double threshold)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  return QuantumOperatorImage(wand->image,channel,ThresholdQuantumOp,
                              threshold,&wand->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) MagickFormatString(percent_opaque,MaxTextExtent,"%g,%g,%g,%g",
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
  ARG_NOT_USED(tint);
  ARG_NOT_USED(opacity);
  ThrowWandException(WandError,WandAPINotImplemented,"MagickTintImage");
#endif /* NOT_IMPLEMENTED */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k T r a n s f o r m I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickTransformImage() is a convenience method that behaves like
%  MagickResizeImage() or MagickCropImage() but accepts scaling and/or cropping
%  information as a region geometry specification.  If the operation fails, the
%  original image handle is returned.
%
%  The format of the MagickTransformImage method is:
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
  return(CloneMagickWandWithImages(wand,transform_image));
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
  const PixelWand *target,const Quantum opacity,const double fuzz)
{
  PixelPacket
    target_pixel;

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  PixelGetQuantumColor(target,&target_pixel);
  wand->image->fuzz=fuzz;
  status=TransparentImage(wand->image,target_pixel,opacity);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%   M a g i c k U n s h a r p M a s k I m a g e                               %
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%   M a g i c k W h i t e T h r e s h o l d I m a g e                         %
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
  char
    thresholds[MaxTextExtent];

  unsigned int
    status;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) MagickFormatString(thresholds,MaxTextExtent,"%u,%u,%u,%u",
    PixelGetRedQuantum(threshold),PixelGetGreenQuantum(threshold),
    PixelGetBlueQuantum(threshold),PixelGetOpacityQuantum(threshold));
  status=WhiteThresholdImage(wand->image,thresholds);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  (void) strlcpy(wand->image->filename,filename,MaxTextExtent);
  write_info=CloneImageInfo(wand->image_info);
  write_info->adjoin=False;
  status=WriteImage(write_info,wand->image);
  DestroyImageInfo(write_info);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W r i t e I m a g e s F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickWriteImagesFile() writes an image or image sequence to a stdio
%  FILE handle.  This may be used to append an encoded image to an already
%  existing appended image sequence if the file seek position is at the end
%  of an existing file.
%
%  The format of the MagickWriteImages method is:
%
%      unsigned int MagickWriteImagesFile(MagickWand *wand,
%        FILE * file,const unsigned int adjoin)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o file: The open (and positioned) file handle.
%
%    o adjoin: join images into a single multi-image file.
%
*/
WandExport unsigned int MagickWriteImagesFile(MagickWand *wand,FILE * file,
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
  write_info->file=file;
  status=WriteImagesFile(write_info,wand->images,file,&wand->exception);
  DestroyImageInfo(write_info);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
%  returns the image as a blob (a formatted "file" in memory) and its
%  length, starting from the current position in the image sequence.
%  Use MagickSetImageFormat() to set the format to write to the blob
%  (GIF, JPEG,  PNG, etc.).
%
%  Use MagickResetIterator() on the wand if it is desired to write
%  a sequence from the beginning and the iterator is not currently
%  at the beginning.
%
%  The format of the MagickWriteImageBlob method is:
%
%      unsigned char *MagickWriteImageBlob(MagickWand *wand,size_t *length)
%
%  A description of each parameter follows:
%
%    o wand: The magick wand.
%
%    o length: The length of the blob.
%
*/
WandExport unsigned char *MagickWriteImageBlob(MagickWand *wand,size_t *length)
{
  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
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
%  MagickWriteImageFile() writes an image to an open file descriptor.
%
%  The format of the MagickWandToFile method is:
%
%      unsigned int MagickWriteImageFile(MagickWand *wand,FILE *file)
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
  if (wand->images == (Image *) NULL)
    ThrowWandException(WandError,WandContainsNoImages,wand->id);
  write_info=CloneImageInfo(wand->image_info);
  write_info->file=file;
  status=WriteImage(write_info,wand->image);
  DestroyImageInfo(write_info);
  if (status == False)
    CopyException(&wand->exception,&wand->image->exception);
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
%  MagickWriteImages() writes an image or image sequence.  If the wand
%  represents an image sequence, then it is written starting at the first
%  frame in the sequence.
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
    CopyException(&wand->exception,&wand->image->exception);
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
static unsigned long GetMagickWandId(void)
{
  static SemaphoreInfo
    *wand_semaphore = (SemaphoreInfo *) NULL;

  static unsigned long
    id = 0;

  if (wand_semaphore == (SemaphoreInfo *) NULL)
    wand_semaphore=AllocateSemaphoreInfo();
  LockSemaphoreInfo(wand_semaphore);
  id++;
  UnlockSemaphoreInfo(wand_semaphore);
  return(id);
}

WandExport MagickWand *NewMagickWand(void)
{
  MagickWand
    *wand;

  /*
    Initialize GraphicsMagick in case it is not already initialized.
  */
  InitializeMagick(NULL);

  wand=MagickAllocateMemory(MagickWand *,sizeof(MagickWand));
  if (wand == (MagickWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
		      UnableToAllocateWand);
  (void) memset(wand,0,sizeof(MagickWand));
  (void) MagickFormatString(wand->id,MaxTextExtent,"MagickWand-%lu",
    GetMagickWandId());
  GetExceptionInfo(&wand->exception);
  wand->image_info=CloneImageInfo((ImageInfo *) NULL);
  wand->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  wand->images=NewImageList();
  wand->signature=MagickSignature;
  return(wand);
}
