/*
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
%                   ImageMagick Image Decoration Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

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

  /*
    Check frame geometry.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(frame_info != (FrameInfo *) NULL);
  if ((frame_info->outer_bevel < 0) || (frame_info->inner_bevel < 0))
    ThrowImageException(OptionWarning,"Unable to frame image",
      "bevel width is negative");
  bevel_width=frame_info->outer_bevel+frame_info->inner_bevel;
  width=(long) (frame_info->width-frame_info->x-bevel_width);
  height=(long) (frame_info->height-frame_info->y-bevel_width);
  if ((width < (long) image->columns) || (height < (long) image->rows))
    ThrowImageException(OptionWarning,"Unable to frame image",
      "frame is less than image size");
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
  accentuate.red=(Quantum) ((unsigned long) (matte.red*(MaxRGB-
    AccentuateModulate)+MaxRGB*AccentuateModulate)/MaxRGB);
  accentuate.green=(Quantum) ((unsigned long) (matte.green*(MaxRGB-
    AccentuateModulate)+MaxRGB*AccentuateModulate)/MaxRGB);
  accentuate.blue=(Quantum) ((unsigned long) (matte.blue*(MaxRGB-
    AccentuateModulate)+MaxRGB*AccentuateModulate)/MaxRGB);
  accentuate.opacity=(Quantum) ((unsigned long) (matte.opacity*(MaxRGB-
    AccentuateModulate)+MaxRGB*AccentuateModulate)/MaxRGB);
  highlight.red=(Quantum) ((unsigned long) (matte.red*(MaxRGB-
    HighlightModulate)+MaxRGB*HighlightModulate)/MaxRGB);
  highlight.green=(Quantum) ((unsigned long) (matte.green*(MaxRGB-
    HighlightModulate)+MaxRGB*HighlightModulate)/MaxRGB);
  highlight.blue=(Quantum) ((unsigned long) (matte.blue*(MaxRGB-
    HighlightModulate)+MaxRGB*HighlightModulate)/MaxRGB);
  highlight.opacity=(Quantum) ((unsigned long) (matte.opacity*(MaxRGB-
    HighlightModulate)+MaxRGB*HighlightModulate)/MaxRGB);
  shadow.red=(Quantum) ((unsigned long) (matte.red*ShadowModulate)/MaxRGB);
  shadow.green=(Quantum) ((unsigned long) (matte.green*ShadowModulate)/MaxRGB);
  shadow.blue=(Quantum) ((unsigned long) (matte.blue*ShadowModulate)/MaxRGB);
  shadow.opacity=(Quantum)
    ((unsigned long) (matte.opacity*ShadowModulate)/MaxRGB);
  trough.red=(Quantum) ((unsigned long) (matte.red*TroughModulate)/MaxRGB);
  trough.green=(Quantum) ((unsigned long) (matte.green*TroughModulate)/MaxRGB);
  trough.blue=(Quantum) ((unsigned long) (matte.blue*TroughModulate)/MaxRGB);
  trough.opacity=(Quantum)
    ((unsigned long) (matte.opacity*TroughModulate)/MaxRGB);
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
        for (x=0; x < (long) (frame_image->columns-2*frame_info->outer_bevel); x++)
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
      MagickMonitor(FrameImageText,y,image->rows);
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
%        const int raised)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o raise_info: Define the width and height of the raised area.
%
%    o raised: A value other than zero creates a 3-D raised effect,
%      otherwise it has a lowered effect.
%
%
*/
MagickExport unsigned int RaiseImage(Image *image,
  const RectangleInfo *raise_info,const int raised)
{
#define AccentuateFactor  Upscale(135)
#define HighlightFactor  Upscale(190)
#define ShadowFactor  Upscale(190)
#define RaiseImageText  "  Raise image...  "
#define TroughFactor  Upscale(135)

  long
    y;

  Quantum
    foreground,
    background;

  register long
    i,
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(raise_info != (RectangleInfo *) NULL);
  if ((image->columns <= (raise_info->width << 1)) ||
      (image->rows <= (raise_info->height << 1)))
    ThrowBinaryException(OptionWarning,"Unable to raise image",
      "image size must exceed bevel width");
  foreground=MaxRGB;
  background=0;
  if (!raised)
    {
      foreground=0;
      background=MaxRGB;
    }
  i=0;
  SetImageType(image,TrueColorType);
  for (y=0; y < (long) raise_info->height; y++)
  {
    q=GetImagePixels(image,0,i++,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < y; x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q++;
    }
    for (x=0; x < (long) (image->columns-2*y); x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*AccentuateFactor+foreground*
        (MaxRGB-AccentuateFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green* AccentuateFactor+
        foreground*(MaxRGB-AccentuateFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*AccentuateFactor+foreground*
        (MaxRGB-AccentuateFactor))/MaxRGB);
      q++;
    }
    for (x=0; x < y; x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(i,image->rows))
      MagickMonitor(RaiseImageText,i,image->rows);
  }
  for (y=0; y < (long) (image->rows-2*raise_info->height); y++)
  {
    q=GetImagePixels(image,0,i++,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) raise_info->width; x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q++;
    }
    for (x=0; x < (long) (image->columns-2*raise_info->width); x++)
      q++;
    for (x=0; x < (long) raise_info->width; x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(i,image->rows))
      MagickMonitor(RaiseImageText,i,image->rows);
  }
  for (y=0; y < (long) raise_info->height; y++)
  {
    q=GetImagePixels(image,0,i++,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) (raise_info->width-y); x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*HighlightFactor+foreground*
        (MaxRGB-HighlightFactor))/MaxRGB);
      q++;
    }
    for (x=0; x < (long) (image->columns-2*(raise_info->width-y)); x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*TroughFactor+background*
        (MaxRGB-TroughFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*TroughFactor+background*
        (MaxRGB-TroughFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*TroughFactor+background*
        (MaxRGB-TroughFactor))/MaxRGB);
      q++;
    }
    for (x=0; x < (long) (raise_info->width-y); x++)
    {
      q->red=(Quantum) ((unsigned long) (q->red*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q->green=(Quantum) ((unsigned long) (q->green*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q->blue=(Quantum) ((unsigned long) (q->blue*ShadowFactor+background*
        (MaxRGB-ShadowFactor))/MaxRGB);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(i,image->rows))
      MagickMonitor(RaiseImageText,i,image->rows);
  }
  return(True);
}
