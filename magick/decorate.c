/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
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
%          DDDD   EEEEE   CCCC   OOO   RRRR    AAA   TTTTT  EEEEE             %
%          D   D  E      C      O   O  R   R  A   A    T    E                 %
%          D   D  EEE    C      O   O  RRRR   AAAAA    T    EEE               %
%          D   D  E      C      O   O  R R    A   A    T    E                 %
%          DDDD   EEEEE   CCCC   OOO   R  R   A   A    T    EEEEE             %
%                                                                             %
%                                                                             %
%                   GraphicsMagick Image Decoration Methods                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
#include "magick/cache.h"
#include "magick/decorate.h"
#include "magick/monitor.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B o r d e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BorderImage() surrounds the image with a border of the color defined by
%  the bordercolor member of the image structure.  The width and height
%  of the border are defined by the corresponding members of the border_info
%  structure.
%
%  The format of the BorderImage method is:
%
%      Image *BorderImage(const Image *image,const RectangleInfo *border_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o border_info:  Define the width and height of the border.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *BorderImage(const Image *image,
  const RectangleInfo *border_info,ExceptionInfo *exception)
{
  Image
    *border_image,
    *clone_image;

  FrameInfo
    frame_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(border_info != (RectangleInfo *) NULL);
  frame_info.width=image->columns+(border_info->width << 1);
  frame_info.height=image->rows+(border_info->height << 1);
  frame_info.x=(long) border_info->width;
  frame_info.y=(long) border_info->height;
  frame_info.inner_bevel=0;
  frame_info.outer_bevel=0;
  clone_image=CloneImage(image,0,0,True,exception);
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  clone_image->matte_color=image->border_color;
  border_image=FrameImage(clone_image,&frame_info,exception);
  DestroyImage(clone_image);
  if (border_image != (Image *) NULL)
    border_image->matte_color=image->matte_color;
  return(border_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F r a m e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FrameImage() adds a simulated three-dimensional border around the image.
%  The color of the border is defined by the matte_color member of image.
%  Members width and height of frame_info specify the border width of the
%  vertical and horizontal sides of the frame.  Members inner and outer
%  indicate the width of the inner and outer shadows of the frame.
%
%  The format of the FrameImage method is:
%
%      Image *FrameImage(const Image *image,const FrameInfo *frame_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o frame_info: Define the width and height of the frame and its bevels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *FrameImage(const Image *image,const FrameInfo *frame_info,
  ExceptionInfo *exception)
{
#define FrameImageText  "  Add frame to image...  "

  Image
    *frame_image;

  long
    height,
    width,
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  PixelPacket
    accentuate,
    highlight,
    matte,
    shadow,
    trough;

  unsigned long
    bevel_width;

  unsigned int
    is_grayscale;

  /*
    Check frame geometry.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(frame_info != (FrameInfo *) NULL);
  is_grayscale=image->is_grayscale;
  if ((frame_info->outer_bevel < 0) || (frame_info->inner_bevel < 0))
    ThrowImageException3(OptionError,UnableToFrameImage,
      BevelWidthIsNegative);
  bevel_width=frame_info->outer_bevel+frame_info->inner_bevel;
  width=(long) (frame_info->width-frame_info->x-bevel_width);
  height=(long) (frame_info->height-frame_info->y-bevel_width);
  if ((width < (long) image->columns) || (height < (long) image->rows))
    ThrowImageException3(OptionError,UnableToFrameImage,
      FrameIsLessThanImageSize);
  /*
    Initialize framed image attributes.
  */
  frame_image=
    CloneImage(image,frame_info->width,frame_info->height,True,exception);
  if (frame_image == (Image *) NULL)
    return(False);
  SetImageType(frame_image,frame_image->matte_color.opacity !=
    OpaqueOpacity ? TrueColorMatteType : TrueColorType);
  /*
    Initialize 3D effects color.
  */
  matte=image->matte_color;
  if (!IsGray(image->matte_color))
    is_grayscale=False;
  accentuate.red=(Quantum) ((((double) MaxRGB-AccentuateModulate)*matte.red+
    ((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  accentuate.green=(Quantum) ((((double) MaxRGB-AccentuateModulate)*matte.green+
    ((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  accentuate.blue=(Quantum) ((((double) MaxRGB-AccentuateModulate)*matte.blue+
    ((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  accentuate.opacity=(Quantum) ((((double) MaxRGB-AccentuateModulate)*
    matte.opacity+((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  highlight.red=(Quantum) ((((double) MaxRGB-HighlightModulate)*matte.red+
    ((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  highlight.green=(Quantum) ((((double) MaxRGB-HighlightModulate)*matte.green+
    ((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  highlight.blue=(Quantum) ((((double) MaxRGB-HighlightModulate)*matte.blue+
    ((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  highlight.opacity=(Quantum) ((((double) MaxRGB-HighlightModulate)*
    matte.opacity+((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  shadow.red=(Quantum) (((double) matte.red*ShadowModulate)/MaxRGB+0.5);
  shadow.green=(Quantum) (((double) matte.green*ShadowModulate)/MaxRGB+0.5);
  shadow.blue=(Quantum) (((double) matte.blue*ShadowModulate)/MaxRGB+0.5);
  shadow.opacity=(Quantum) (((double) matte.opacity*ShadowModulate)/MaxRGB+0.5);
  trough.red=(Quantum) (((double) matte.red*TroughModulate)/MaxRGB+0.5);
  trough.green=(Quantum) (((double) matte.green*TroughModulate)/MaxRGB+0.5);
  trough.blue=(Quantum) (((double) matte.blue*TroughModulate)/MaxRGB+0.5);
  trough.opacity=(Quantum) (((double) matte.opacity*TroughModulate)/MaxRGB+0.5);
  /*
    Draw top of ornamental border.
  */
  height=(long) (frame_info->outer_bevel+(frame_info->y-bevel_width)+
    frame_info->inner_bevel);
  q=SetImagePixels(frame_image,0,0,frame_image->columns,height);
  if (q != (PixelPacket *) NULL)
    {
      for (y=0; y < frame_info->outer_bevel; y++)
      {
        for (x=0; x < (long) (frame_image->columns-y); x++)
          if (x < y)
            *q++=highlight;
          else
            *q++=accentuate;
        for ( ; x < (long) frame_image->columns; x++)
          *q++=shadow;
      }
      for (y=0; y < (long) (frame_info->y-bevel_width); y++)
      {
        for (x=0; x < frame_info->outer_bevel; x++)
          *q++=highlight;
        width=(long) (frame_image->columns-2*frame_info->outer_bevel);
        for (x=0; x < (long) width; x++)
          *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
          *q++=shadow;
      }
      for (y=0; y < frame_info->inner_bevel; y++)
      {
        for (x=0; x < frame_info->outer_bevel; x++)
          *q++=highlight;
        for (x=0; x < (long) (frame_info->x-bevel_width); x++)
          *q++=matte;
        width=(long) (image->columns+(frame_info->inner_bevel << 1)-y);
        for (x=0; x < width; x++)
          if (x < y)
            *q++=shadow;
          else
            *q++=trough;
        for ( ; x < (long) (image->columns+(frame_info->inner_bevel << 1)); x++)
          *q++=highlight;
        width=(long)
          (frame_info->width-frame_info->x-image->columns-bevel_width);
        for (x=0; x < width; x++)
          *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
          *q++=shadow;
      }
      (void) SyncImagePixels(frame_image);
    }
  /*
    Draw sides of ornamental border.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    /*
      Initialize scanline with border color.
    */
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=SetImagePixels(frame_image,0,frame_info->y+y,frame_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (long) (frame_info->x-bevel_width); x++)
      *q++=matte;
    for (x=0; x < frame_info->inner_bevel; x++)
      *q++=shadow;
    /*
      Transfer scanline.
    */
    (void) memcpy(q,p,image->columns*sizeof(PixelPacket));
    q+=image->columns;
    for (x=0; x < frame_info->inner_bevel; x++)
      *q++=highlight;
    width=(long) (frame_info->width-frame_info->x-image->columns-bevel_width);
    for (x=0; x < width; x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
    if (!SyncImagePixels(frame_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(FrameImageText,y,image->rows,exception))
        break;
  }
  /*
    Draw bottom of ornamental border.
  */
  height=(long) (frame_info->inner_bevel+frame_info->height-frame_info->y-
    image->rows-bevel_width+frame_info->outer_bevel);
  q=SetImagePixels(frame_image,0,(long) (frame_image->rows-height),
    frame_image->columns,height);
  if (q == (PixelPacket *) NULL)
    return(frame_image);
  for (y=frame_info->inner_bevel-1; y >= 0; y--)
  {
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (long) (frame_info->x-bevel_width); x++)
      *q++=matte;
    for (x=0; x < y; x++)
      *q++=shadow;
    for ( ; x < (long) (image->columns+(frame_info->inner_bevel << 1)); x++)
      if (x >= (long) (image->columns+(frame_info->inner_bevel << 1)-y))
        *q++=highlight;
      else
        *q++=accentuate;
    width=(long) (frame_info->width-frame_info->x-image->columns-bevel_width);
    for (x=0; x < (long) width; x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
  }
  height=(long) (frame_info->height-frame_info->y-image->rows-bevel_width);
  for (y=0; y < height; y++)
  {
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=highlight;
    for (x=0; x < (long) (frame_image->columns-2*frame_info->outer_bevel); x++)
      *q++=matte;
    for (x=0; x < frame_info->outer_bevel; x++)
      *q++=shadow;
  }
  for (y=frame_info->outer_bevel-1; y >= 0; y--)
  {
    for (x=0; x < y; x++)
      *q++=highlight;
    for ( ; x < (long) frame_image->columns; x++)
      if (x >= (long) (frame_image->columns-y))
        *q++=shadow;
      else
        *q++=trough;
  }
  (void) SyncImagePixels(frame_image);
  frame_image->is_grayscale=is_grayscale;
  return(frame_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R a i s e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RaiseImage() creates a simulated three-dimensional button-like effect
%  by lightening and darkening the edges of the image.  Members width and
%  height of raise_info define the width of the vertical and horizontal
%  edge of the effect.
%
%  The format of the RaiseImage method is:
%
%      unsigned int RaiseImage(Image *image,const RectangleInfo *raise_info,
%        const int raise)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o raise_info: Define the width and height of the raise area.
%
%    o raise: A value other than zero creates a 3-D raise effect,
%      otherwise it has a lowered effect.
%
%
*/
MagickExport unsigned int RaiseImage(Image *image,
  const RectangleInfo *raise_info,const int raise)
{
#define AccentuateFactor  ScaleCharToQuantum(135)
#define HighlightFactor  ScaleCharToQuantum(190)
#define ShadowFactor  ScaleCharToQuantum(190)
#define RaiseImageText  "  Raise image...  "
#define TroughFactor  ScaleCharToQuantum(135)

  Quantum
    foreground,
    background;

  long
    y;

  register long
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(raise_info != (RectangleInfo *) NULL);
  is_grayscale=image->is_grayscale;
  if ((image->columns <= (raise_info->width << 1)) ||
      (image->rows <= (raise_info->height << 1)))
    ThrowBinaryException3(OptionError,UnableToRaiseImage,
      ImageSizeMustExceedBevelWidth);
  foreground=MaxRGB;
  background=0;
  if (!raise)
    {
      foreground=0;
      background=MaxRGB;
    }
  SetImageType(image,TrueColorType);
  for (y=0; y < (long) raise_info->height; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < y; x++)
    {
      q->red=(Quantum) (((double) q->red*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB);
      q->green=(Quantum) (((double) q->green*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB);
      q->blue=(Quantum) (((double) q->blue*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB);
      q++;
    }
    for ( ; x < (long) (image->columns-y); x++)
    {
      q->red=(Quantum) (((double) q->red*AccentuateFactor+
        (double) foreground*(MaxRGB-AccentuateFactor))/MaxRGB);
      q->green=(Quantum) (((double) q->green*AccentuateFactor+
        (double) foreground*(MaxRGB-AccentuateFactor))/MaxRGB);
      q->blue=(Quantum) (((double) q->blue*AccentuateFactor+
        (double) foreground*(MaxRGB-AccentuateFactor))/MaxRGB);
      q++;
    }
    for ( ; x < (long) image->columns; x++)
    {
      q->red=(Quantum) (((double) q->red*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB);
      q->green=(Quantum) (((double) q->green*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB);
      q->blue=(Quantum) (((double) q->blue*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(RaiseImageText,y,image->rows,&image->exception))
        break;
  }
  for ( ; y < (long) (image->rows-raise_info->height); y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) raise_info->width; x++)
    {
      q->red=(Quantum) (((double) q->red*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB);
      q->green=(Quantum) (((double) q->green*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB);
      q->blue=(Quantum) (((double) q->blue*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB);
      q++;
    }
    for ( ; x < (long) (image->columns-raise_info->width); x++)
      q++;
    for ( ; x < (long) image->columns; x++)
    {
      q->red=(Quantum) (((double) q->red*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB);
      q->green=(Quantum) (((double) q->green*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB);
      q->blue=(Quantum) (((double) q->blue*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(RaiseImageText,y,image->rows,&image->exception))
        break;
  }
  for ( ; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) (image->rows-y); x++)
    {
      q->red=(Quantum) (((double) q->red*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB+0.5);
      q->green=(Quantum) (((double) q->green*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB+0.5);
      q->blue=(Quantum) (((double) q->blue*HighlightFactor+
        (double) foreground*(MaxRGB-HighlightFactor))/MaxRGB+0.5);
      q++;
    }
    for ( ; x < (long) (image->columns-(image->rows-y)); x++)
    {
      q->red=(Quantum) (((double) q->red*TroughFactor+
        (double) background*(MaxRGB-TroughFactor))/MaxRGB+0.5);
      q->green=(Quantum) (((double) q->green*TroughFactor+
        (double) background*(MaxRGB-TroughFactor))/MaxRGB+0.5);
      q->blue=(Quantum) (((double) q->blue*TroughFactor+
        (double) background*(MaxRGB-TroughFactor))/MaxRGB+0.5);
      q++;
    }
    for ( ; x < (long) image->columns; x++)
    {
      q->red=(Quantum) (((double) q->red*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB+0.5);
      q->green=(Quantum) (((double) q->green*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB+0.5);
      q->blue=(Quantum) (((double) q->blue*ShadowFactor+
        (double) background*(MaxRGB-ShadowFactor))/MaxRGB+0.5);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(RaiseImageText,y,image->rows,&image->exception))
        break;
  }
  image->is_grayscale=is_grayscale;
  return(True);
}
