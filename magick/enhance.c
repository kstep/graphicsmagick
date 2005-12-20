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
%              EEEEE  N   N  H   H   AAA   N   N   CCCC  EEEEE                %
%              E      NN  N  H   H  A   A  NN  N  C      E                    %
%              EEE    N N N  HHHHH  AAAAA  N N N  C      EEE                  %
%              E      N  NN  H   H  A   A  N  NN  C      E                    %
%              EEEEE  N   N  H   H  A   A  N   N   CCCC  EEEEE                %
%                                                                             %
%                                                                             %
%                  GraphicsMagick Image Enhancement Methods                   %
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
#include "magick/enhance.h"
#include "magick/gem.h"
#include "magick/pixel_cache.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n t r a s t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ContrastImage() enhances the intensity differences between the lighter and
%  darker elements of the image.  Set sharpen to a value other than 0 to
%  increase the image contrast otherwise the contrast is reduced.
%
%  The format of the ContrastImage method is:
%
%      unsigned int ContrastImage(Image *image,const unsigned int sharpen)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o sharpen: Increase or decrease image contrast.
%
%
*/
MagickExport MagickPassFail ContrastImage(Image *image,const unsigned int sharpen)
{
#define DullContrastImageText  "  Dulling image contrast...  "
#define SharpenContrastImageText  "  Sharpening image contrast...  "

  int
    sign;

  long
    y;

  register long
    i,
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  sign=sharpen ? 1 : -1;
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Contrast enhance DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=(long) image->columns; x > 0; x--)
        {
          Contrast(sign,&q->red,&q->green,&q->blue);
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          {
            unsigned int
              monitor_status;

            if (sharpen)
              monitor_status=MagickMonitor(SharpenContrastImageText,y,image->rows,
                &image->exception);
            else
              monitor_status=MagickMonitor(DullContrastImageText,y,image->rows,
                &image->exception);
            if (monitor_status == False)
              {
                status=MagickFail;
                break;
              }
          }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Contrast enhance PseudoClass image.
      */
      assert(image->colormap != (PixelPacket *) NULL);
      q=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
        {
          Contrast(sign,&(q->red),&(q->green),&(q->blue));
          q++;
        }
      status &= SyncImage(image);
      break;
    }
  }
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E q u a l i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EqualizeImage() applies a histogram equalization to the image.
%
%  The format of the EqualizeImage method is:
%
%      unsigned int EqualizeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
*/
MagickExport MagickPassFail EqualizeImage(Image *image)
{
#define EqualizeImageText  "  Equalizing image...  "

  DoublePixelPacket
    high,
    *histogram,
    intensity,
    low,
    *map;

  long
    y;

  PixelPacket
    *equalize_map;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate and initialize histogram arrays.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  histogram=MagickAllocateMemory(DoublePixelPacket *,
    (MaxMap+1)*sizeof(DoublePixelPacket));
  map=MagickAllocateMemory(DoublePixelPacket *,(MaxMap+1)*sizeof(DoublePixelPacket));
  equalize_map=MagickAllocateMemory(PixelPacket *,(MaxMap+1)*sizeof(PixelPacket));
  if ((histogram == (DoublePixelPacket *) NULL) ||
      (map == (DoublePixelPacket *) NULL) ||
      (equalize_map == (PixelPacket *) NULL))
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      MagickMsg(OptionError,UnableToEqualizeImage));
  /*
    Form histogram.
  */
  (void) memset(histogram,0,(MaxMap+1)*sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    if (image->matte)
      for (x=(long) image->columns; x > 0; x--)
        {
          histogram[ScaleQuantumToMap(p->red)].red++;
          histogram[ScaleQuantumToMap(p->green)].green++;
          histogram[ScaleQuantumToMap(p->blue)].blue++;
          histogram[ScaleQuantumToMap(p->opacity)].opacity++;
          p++;
        }
    else
      for (x=(long) image->columns; x > 0; x--)
        {
          histogram[ScaleQuantumToMap(p->red)].red++;
          histogram[ScaleQuantumToMap(p->green)].green++;
          histogram[ScaleQuantumToMap(p->blue)].blue++;
          p++;
        }
  }
  /*
    Integrate the histogram to get the equalization map.
  */
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));  
  if (image->matte)
    for (i=0; i <= (long) MaxMap; i++)
      {
        intensity.red+=histogram[i].red;
        intensity.green+=histogram[i].green;
        intensity.blue+=histogram[i].blue;
        intensity.opacity+=histogram[i].opacity;
        map[i]=intensity;
      }
  else
    for (i=0; i <= (long) MaxMap; i++)
      {
        intensity.red+=histogram[i].red;
        intensity.green+=histogram[i].green;
        intensity.blue+=histogram[i].blue;
        map[i]=intensity;
      }
  low=map[0];
  high=map[MaxMap];
  (void) memset(equalize_map,0,(MaxMap+1)*sizeof(PixelPacket));
  for (i=0; i <= (long) MaxMap; i++)
  {
    if (high.red != low.red)
      equalize_map[i].red=ScaleMapToQuantum(
        (MaxMap*(map[i].red-low.red))/(high.red-low.red));
    if (high.green != low.green)
      equalize_map[i].green=ScaleMapToQuantum(
        (MaxMap*(map[i].green-low.green))/(high.green-low.green));
    if (high.blue != low.blue)
      equalize_map[i].blue=ScaleMapToQuantum(
        (MaxMap*(map[i].blue-low.blue))/(high.blue-low.blue));
    if (image->matte)
      if (high.opacity != low.opacity)
        equalize_map[i].opacity=ScaleMapToQuantum(
         (MaxMap*(map[i].opacity-low.opacity))/(high.opacity-low.opacity));
  }
  MagickFreeMemory(histogram);
  MagickFreeMemory(map);
  /*
    Stretch the histogram.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Equalize DirectClass packets.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        if (image->matte)
          for (x=(long) image->columns; x > 0; x--)
            {
              if (low.red != high.red)
                q->red=equalize_map[ScaleQuantumToMap(q->red)].red;
              if (low.green != high.green)
                q->green=equalize_map[ScaleQuantumToMap(q->green)].green;
              if (low.blue != high.blue)
                q->blue=equalize_map[ScaleQuantumToMap(q->blue)].blue;
              if (low.opacity != high.opacity)
                q->opacity=equalize_map[ScaleQuantumToMap(q->opacity)].opacity;
              q++;
            }
        else
          for (x=(long) image->columns; x > 0; x--)
            {
              if (low.red != high.red)
                q->red=equalize_map[ScaleQuantumToMap(q->red)].red;
              if (low.green != high.green)
                q->green=equalize_map[ScaleQuantumToMap(q->green)].green;
              if (low.blue != high.blue)
                q->blue=equalize_map[ScaleQuantumToMap(q->blue)].blue;
              q++;
            }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(EqualizeImageText,y,image->rows,&image->exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Equalize PseudoClass packets.
      */
      assert(image->colormap != (PixelPacket *) NULL);
      for (i=0; i < (long) image->colors; i++)
      {
        if (low.red != high.red)
          image->colormap[i].red=
            equalize_map[ScaleQuantumToMap(image->colormap[i].red)].red;
        if (low.green != high.green)
          image->colormap[i].green=
            equalize_map[ScaleQuantumToMap(image->colormap[i].green)].green;
        if (low.blue != high.blue)
          image->colormap[i].blue=
            equalize_map[ScaleQuantumToMap(image->colormap[i].blue)].blue;
      }
      status &= SyncImage(image);
      break;
    }
  }
  MagickFreeMemory(equalize_map);
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a m m a I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use GammaImage() to gamma-correct an image.  The same image viewed on
%  different devices will have perceptual differences in the way the
%  image's intensities are represented on the screen.  Specify individual
%  gamma levels for the red, green, and blue channels, or adjust all three
%  with the gamma parameter.  Values typically range from 0.8 to 2.3.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the GammaImage method is:
%
%      unsigned int GammaImage(Image *image,const char *level)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o level: Define the level of gamma correction.
%
%
*/
MagickExport MagickPassFail GammaImage(Image *image,const char *level)
{
#define GammaImageText  "  Gamma correcting the image...  "

  DoublePixelPacket
    gamma;

  long
    count,
    y;

  PixelPacket
    *gamma_map;

  register long
    i,
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (level == (char *) NULL)
    return(MagickFail);
  gamma.red=1.0;
  gamma.green=1.0;
  gamma.blue=1.0;
  count=sscanf(level,"%lf%*[,/]%lf%*[,/]%lf",&gamma.red,&gamma.green,
    &gamma.blue);
  if (count == 1)
    {
      if (gamma.red == 1.0)
        return(MagickFail);
      gamma.green=gamma.red;
      gamma.blue=gamma.red;
    }
  is_grayscale=(image->is_grayscale && (gamma.red == gamma.green) &&
                (gamma.green == gamma.blue));
  /*
    Allocate and initialize gamma maps.
  */
  gamma_map=MagickAllocateMemory(PixelPacket *,(MaxMap+1)*sizeof(PixelPacket));
  if (gamma_map == (PixelPacket *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToGammaCorrectImage);
  (void) memset(gamma_map,0,(MaxMap+1)*sizeof(PixelPacket));
  for (i=0; i <= (long) MaxMap; i++)
  {
    if (gamma.red != 0.0)
      gamma_map[i].red=
        ScaleMapToQuantum(MaxMap*pow((double) i/MaxMap,1.0/gamma.red));
    if (gamma.green != 0.0)
      gamma_map[i].green=
       ScaleMapToQuantum(MaxMap*pow((double) i/MaxMap,1.0/gamma.green));
    if (gamma.blue != 0.0)
      gamma_map[i].blue=
        ScaleMapToQuantum(MaxMap*pow((double) i/MaxMap,1.0/gamma.blue));
  }
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Gamma-correct DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=(long) image->columns; x > 0; x--)
        {
          q->red=gamma_map[ScaleQuantumToMap(q->red)].red;
          q->green=gamma_map[ScaleQuantumToMap(q->green)].green;
          q->blue=gamma_map[ScaleQuantumToMap(q->blue)].blue;
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(GammaImageText,y,image->rows,&image->exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Gamma-correct PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].red=
          gamma_map[ScaleQuantumToMap(image->colormap[i].red)].red;
        image->colormap[i].green=
          gamma_map[ScaleQuantumToMap(image->colormap[i].green)].green;
        image->colormap[i].blue=
          gamma_map[ScaleQuantumToMap(image->colormap[i].blue)].blue;
      }
      status &= SyncImage(image);
      break;
    }
  }
  if (image->gamma != 0.0)
    image->gamma*=(gamma.red+gamma.green+gamma.blue)/3.0;
  MagickFreeMemory(gamma_map);
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L e v e l I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LevelImage() adjusts the levels of an image by scaling the colors falling
%  between specified white and black points to the full available quantum
%  range. The parameters provided represent the black, mid (gamma), and white
%  points. The black point specifies the darkest color in the image. Colors
%  darker than the black point are set to zero. Mid point specifies a gamma
%  correction to apply to the image.  White point specifies the lightest color
%  in the image. Colors brighter than the white point are set to the maximum
%  quantum value.
%
%  The format of the LevelImage method is:
%
%      unsigned int LevelImage(Image *image,const char *level)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o levels: Specify the levels as a string of the form "black/mid/white"
%      (e.g. "10,1.0,65000) where black and white have the range of 0-MaxRGB,
%      and mid has the range 0-10.
%
%
*/
MagickExport MagickPassFail LevelImage(Image *image,const char *levels)
{
#define LevelImageText  "  Leveling the image...  "

  const char
    *lp;

  char
    *cp,
    buffer[MaxTextExtent];

  MagickBool
    percent = MagickFalse;

  double
    black_point,
    *levels_map,
    mid_point,
    white_point;

  int
    count;

  unsigned int
    is_grayscale;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;


  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (levels == (char *) NULL)
    return(MagickFail);
  is_grayscale=image->is_grayscale;
  black_point=0.0;
  mid_point=1.0;
  white_point=MaxRGB;
  for (cp=buffer, lp=levels; (cp < (buffer+sizeof(buffer)-1)) && (*lp != 0); lp++)
    {
      if (*lp == '%')
        percent = MagickTrue;
      else
        *cp++=*lp;
    }
  *cp=0;
  count=sscanf(buffer,"%lf%*[,/]%lf%*[,/]%lf",&black_point,&mid_point,
    &white_point);
  if (percent)
    {
      if (count > 0)
        black_point*=MaxRGB/100.0;
      if (count > 2)
        white_point*=MaxRGB/100.0;
    }
  black_point=ConstrainToQuantum(black_point);
  black_point=ScaleQuantumToMap(black_point);
  white_point=ConstrainToQuantum(white_point);
  white_point=ScaleQuantumToMap(white_point);
  if (count == 1)
    white_point=MaxMap-black_point;
  levels_map=MagickAllocateMemory(double *,(MaxMap+1)*sizeof(double));
  if (levels_map == (double *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToLevelImage);
  for (i=0; i <= (long) MaxMap; i++)
  {
    if (i < black_point)
      {
        levels_map[i]=0;
        continue;
      }
    if (i > white_point)
      {
        levels_map[i]=MaxMap;
        continue;
      }
    levels_map[i]=MaxMap*(pow(((double) i-black_point)/
      (white_point-black_point),1.0/mid_point));
  }
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Level DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=(long) image->columns; x > 0; x--)
        {
          q->red=ScaleMapToQuantum(levels_map[ScaleQuantumToMap(q->red)]);
          q->green=ScaleMapToQuantum(levels_map[ScaleQuantumToMap(q->green)]);
          q->blue=ScaleMapToQuantum(levels_map[ScaleQuantumToMap(q->blue)]);
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(LevelImageText,y,image->rows,&image->exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Level PseudoClass image.
      */
      assert(image->colormap != (PixelPacket *) NULL);
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].red=ScaleMapToQuantum(
          levels_map[ScaleQuantumToMap(image->colormap[i].red)]);
        image->colormap[i].green=ScaleMapToQuantum(
          levels_map[ScaleQuantumToMap(image->colormap[i].green)]);
        image->colormap[i].blue=ScaleMapToQuantum(
          levels_map[ScaleQuantumToMap(image->colormap[i].blue)]);
      }
      status &= SyncImage(image);
      break;
    }
  }
  MagickFreeMemory(levels_map);
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L e v e l I m a g e C h a n n e l                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LevelImageChannel() adjusts the levels of a particular image channel by
%  scaling the colors falling between specified white and black points to
%  the full available quantum range. The parameters provided represent the
%  black, mid (gamma), and white points.  The black point specifies the
%  darkest color in the image. Colors darker than the black point are set to
%  zero. Mid point specifies a gamma correction to apply to the image.
%  White point specifies the lightest color in the image.  Colors brighter
%  than the white point are set to the maximum quantum value.
%
%  The format of the LevelImage method is:
%
%      unsigned int LevelImageChannel(Image *image,const char *level,
%        const ChannelType channel,const double black_point,
%        const double mid_point,const double white_point)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Identify which channel to level: Red, Cyan, Green, Magenta,
%      Blue, Yellow, or Opacity.
%
%    o black_point, mid_point, white_point: Specify the levels where the black
%      and white points have the range of 0-MaxRGB, and mid has the range 0-10.
%
%
*/
MagickExport MagickPassFail LevelImageChannel(Image *image,
  const ChannelType channel,const double black_point,const double mid_point,
  const double white_point)
{
  double
    black,
    *levels_map,
    white;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  levels_map=MagickAllocateMemory(double *,(MaxMap+1)*sizeof(double));
  if (levels_map == (double *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToLevelImage);
  black=ScaleQuantumToMap(black_point);
  white=ScaleQuantumToMap(white_point);
  for (i=0; i <= (long) MaxMap; i++)
    {
      if (i < black)
        {
          levels_map[i]=0;
          continue;
        }
      if (i > white)
        {
          levels_map[i]=MaxMap;
          continue;
        }
      levels_map[i]=MaxMap*(pow(((double) i-black)/
        (white-black),1.0/mid_point));
    }
  switch (image->storage_class)
    {
    case DirectClass:
    default:
      {
        /*
          Level DirectClass image.
        */
        for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              {
                status=MagickFail;
                break;
              }
            switch (channel)
              {
              case RedChannel:
              case CyanChannel:
                {
                  for (x=(long) image->columns; x > 0; x--)
                    {
                      q->red=ScaleMapToQuantum(
                        levels_map[ScaleQuantumToMap(q->red)]);
                      q++;
                    }
                  break;
                }
              case GreenChannel:
              case MagentaChannel:
                {
                  for (x=(long) image->columns; x > 0; x--)
                    {
                      q->green=ScaleMapToQuantum(
                        levels_map[ScaleQuantumToMap(q->green)]);
                      q++;
                    }
                  break;
                }
              case BlueChannel:
              case YellowChannel:
                {
                  for (x=(long) image->columns; x > 0; x--)
                    {
                      q->blue=ScaleMapToQuantum(
                        levels_map[ScaleQuantumToMap(q->blue)]);
                      q++;
                    }
                  break;
                }
              case OpacityChannel:
              case BlackChannel:
                {
                  for (x=(long) image->columns; x > 0; x--)
                    {
                      q->opacity=ScaleMapToQuantum(
                        levels_map[ScaleQuantumToMap(q->opacity)]);
                      q++;
                    }
                  break;
                }
              default:
                break;
              }          
            if (!SyncImagePixels(image))
              {
                status=MagickFail;
                break;
              }
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LevelImageText,y,image->rows,
                &image->exception))
                {
                  status=MagickFail;
                  break;
                }
          }
        break;
      }
    case PseudoClass:
      {
        /*
          Level PseudoClass image.
        */
        assert(image->colormap != (PixelPacket *) NULL);
        for (i=0; i < (long) image->colors; i++)
          {
            switch (channel)
              {
              case RedChannel:
              case CyanChannel:
                {
                  image->colormap[i].red=ScaleMapToQuantum(
                    levels_map[ScaleQuantumToMap(image->colormap[i].red)]);
                  break;
                }
              case GreenChannel:
              case MagentaChannel:
                {
                  image->colormap[i].green=ScaleMapToQuantum(
                    levels_map[ScaleQuantumToMap(image->colormap[i].green)]);
                  break;
                }
              case BlueChannel:
              case YellowChannel:
                {
                  image->colormap[i].blue=ScaleMapToQuantum(
                    levels_map[ScaleQuantumToMap(image->colormap[i].blue)]);
                  break;
                }
              default:
                break;
              }
          }
        status &= SyncImage(image);
        break;
      }
    }
  MagickFreeMemory(levels_map);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o d u l a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ModulateImage() lets you control the brightness, saturation, and hue
%  of an image.  Modulate represents the brightness, saturation, and hue
%  as one parameter (e.g. 90,150,100).
%
%  The format of the ModulateImage method is:
%
%      unsigned int ModulateImage(Image *image,const char *modulate)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o modulate: Define the percent change in brightness, saturation, and
%      hue.
%
%
*/
MagickExport MagickPassFail ModulateImage(Image *image,const char *modulate)
{
#define ModulateImageText  "  Modulating image...  "

  double
    percent_brightness,
    percent_hue,
    percent_saturation;

  long
    y;

  register long
    i,
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (modulate == (char *) NULL)
    return(MagickFail);
  is_grayscale=image->is_grayscale;
  percent_brightness=100.0;
  percent_saturation=100.0;
  percent_hue=100.0;
  (void) sscanf(modulate,"%lf%*[,/]%lf%*[,/]%lf",&percent_brightness,
    &percent_saturation,&percent_hue);
  /*
    Ensure that adjustment values are positive so they don't need to
    be checked in Modulate.
  */
  percent_brightness=AbsoluteValue(percent_brightness);
  percent_saturation=AbsoluteValue(percent_saturation);
  percent_hue=AbsoluteValue(percent_hue);
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Modulate the color for a DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=(long) image->columns; x > 0; x--)
        {
          Modulate(percent_hue,percent_saturation,percent_brightness,
            &q->red,&q->green,&q->blue);
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(ModulateImageText,y,image->rows,&image->exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Modulate the color for a PseudoClass image.
      */
      PixelPacket
        *colormap;

      assert(image->colormap != (PixelPacket *) NULL);
      colormap=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
        {
          Modulate(percent_hue,percent_saturation,percent_brightness,
                   &(colormap->red),&(colormap->green),&(colormap->blue));
                   colormap++;
        }
      status &= SyncImage(image);
      break;
    }
  }
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N e g a t e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NegateImage negates the colors in the reference image.  The
%  Grayscale option means that only grayscale values within the image are
%  negated.
%
%  The format of the NegateImage method is:
%
%      unsigned int NegateImage(Image *image,const unsigned int grayscale)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport MagickPassFail NegateImage(Image *image,const unsigned int grayscale)
{
#define NegateImageText  "  Negating the image colors...  "

  long
    y;

  register long
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  if (image->clip_mask)
    image->storage_class=DirectClass;
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Negate DirectClass packets.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }

        if (grayscale)
        {
          /* Process only the non-gray pixels */
          for (x=(long) image->columns; x > 0; x--)
            {
              if ((q->red != q->green) || (q->green != q->blue))
              {
                q++;
                continue;
              }
              q->red=(~q->red);
              q->green=(~q->green);
              q->blue=(~q->blue);
              if (image->colorspace == CMYKColorspace)
                q->opacity=(~q->opacity);
              q++;
            }
        }
        else
        {
          /* Process all pixels */
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=(~q->red);
              q->green=(~q->green);
              q->blue=(~q->blue);
              if (image->colorspace == CMYKColorspace)
                q->opacity=(~q->opacity);
              q++;
            }
        }

        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(NegateImageText,y,image->rows,&image->exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Negate PseudoClass packets.
      */
      assert(image->colormap != (PixelPacket *) NULL);
      q=image->colormap;
      if (grayscale)
      {
        /* Process only the non-gray pixels */
        for (x=(long) image->colors; x > 0; x--)
          {
            if ((q->red != q->green) ||
                (q->green != q->blue))
              continue;
            q->red=(~q->red);
            q->green=(~q->green);
            q->blue=(~q->blue);
            q++;
          }
      }
      else
      {
        /* Process all pixels */
        for (x=(long) image->colors; x > 0; x--)
          {
            q->red=(~q->red);
            q->green=(~q->green);
            q->blue=(~q->blue);
            q++;
          }
      }
      status &= SyncImage(image);
      break;
    }
  }
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o r m a l i z e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The NormalizeImage() method enhances the contrast of a color image by
%  adjusting the pixels color to span the entire range of colors available.
%
%  The format of the NormalizeImage method is:
%
%      unsigned int NormalizeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport MagickPassFail NormalizeImage(Image *image)
{
#define MaxRange(color)  ScaleQuantumToMap(color)
#define NormalizeImageText  "  Normalizing image...  "

  DoublePixelPacket
    high,
    *histogram,
    intensity,
    low;

  long
    y;

  PixelPacket
    *normalize_map;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  unsigned int
    is_grayscale;

  unsigned long
    threshold_intensity;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate histogram and normalize map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  histogram=MagickAllocateMemory(DoublePixelPacket *,
    (MaxMap+1)*sizeof(DoublePixelPacket));
  normalize_map=MagickAllocateMemory(PixelPacket *,(MaxMap+1)*sizeof(PixelPacket));
  if ((histogram == (DoublePixelPacket *) NULL) ||
      (normalize_map == (PixelPacket *) NULL))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToNormalizeImage);
  /*
    Form histogram.
  */
  (void) memset(histogram,0,(MaxMap+1)*sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    if (image->matte)
      for (x=(long) image->columns; x > 0; x--)
        {
          histogram[ScaleQuantumToMap(p->red)].red++;
          histogram[ScaleQuantumToMap(p->green)].green++;
          histogram[ScaleQuantumToMap(p->blue)].blue++;
          histogram[ScaleQuantumToMap(p->opacity)].opacity++;
          p++;
        }
    else
      for (x=(long) image->columns; x > 0; x--)
        {
          histogram[ScaleQuantumToMap(p->red)].red++;
          histogram[ScaleQuantumToMap(p->green)].green++;
          histogram[ScaleQuantumToMap(p->blue)].blue++;
          p++;
        }
  }
  /*
    Find the histogram boundaries by locating the 0.1 percent levels.
  */
  threshold_intensity=(long) (image->columns*image->rows)/1000;
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.red=0; low.red < MaxMap; low.red++)
  {
    intensity.red+=histogram[(long) low.red].red;
    if (intensity.red > threshold_intensity)
      break;
  }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.red=MaxMap; high.red != 0; high.red--)
  {
    intensity.red+=histogram[(long) high.red].red;
    if (intensity.red > threshold_intensity)
      break;
  }
  if (low.red == high.red)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.red=0; low.red < MaxRange(MaxRGB); low.red++)
      {
        intensity.red+=histogram[(long) low.red].red;
        if (intensity.red > threshold_intensity)
          break;
      }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.red=MaxRange(MaxRGB); high.red != 0; high.red--)
      {
        intensity.red+=histogram[(long) high.red].red;
        if (intensity.red > threshold_intensity)
          break;
      }
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.green=0; low.green < MaxRange(MaxRGB); low.green++)
  {
    intensity.green+=histogram[(long) low.green].green;
    if (intensity.green > threshold_intensity)
      break;
  }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.green=MaxRange(MaxRGB); high.green != 0; high.green--)
  {
    intensity.green+=histogram[(long) high.green].green;
    if (intensity.green > threshold_intensity)
      break;
  }
  if (low.green == high.green)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.green=0; low.green < MaxRange(MaxRGB); low.green++)
      {
        intensity.green+=histogram[(long) low.green].green;
        if (intensity.green > threshold_intensity)
          break;
      }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.green=MaxRange(MaxRGB); high.green != 0; high.green--)
      {
        intensity.green+=histogram[(long) high.green].green;
        if (intensity.green > threshold_intensity)
          break;
      }
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.blue=0; low.blue < MaxRange(MaxRGB); low.blue++)
  {
    intensity.blue+=histogram[(long) low.blue].blue;
    if (intensity.blue > threshold_intensity)
      break;
  }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.blue=MaxRange(MaxRGB); high.blue != 0; high.blue--)
  {
    intensity.blue+=histogram[(long) high.blue].blue;
    if (intensity.blue > threshold_intensity)
      break;
  }
  if (low.blue == high.blue)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.blue=0; low.blue < MaxRange(MaxRGB); low.blue++)
      {
        intensity.blue+=histogram[(long) low.blue].blue;
        if (intensity.blue > threshold_intensity)
          break;
      }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.blue=MaxRange(MaxRGB); high.blue != 0; high.blue--)
      {
        intensity.blue+=histogram[(long) high.blue].blue;
        if (intensity.blue > threshold_intensity)
          break;
      }
    }
  high.opacity=0;
  low.opacity=0;
  if (image->matte)
    {
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.opacity=0; low.opacity < MaxRange(MaxRGB); low.opacity++)
        {
          intensity.opacity+=histogram[(long) low.opacity].opacity;
          if (intensity.opacity > threshold_intensity)
            break;
        }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.opacity=MaxRange(MaxRGB); high.opacity != 0; high.opacity--)
        {
          intensity.opacity+=histogram[(long) high.opacity].opacity;
          if (intensity.opacity > threshold_intensity)
            break;
        }
      if (low.opacity == high.opacity)
        {
          /*
            Unreasonable contrast;  use zero threshold to determine boundaries.
          */
          threshold_intensity=0;
          (void) memset(&intensity,0,sizeof(DoublePixelPacket));
          for (low.opacity=0; low.opacity < MaxRange(MaxRGB); low.opacity++)
            {
              intensity.opacity+=histogram[(long) low.opacity].opacity;
              if (intensity.opacity > threshold_intensity)
                break;
            }
          (void) memset(&intensity,0,sizeof(DoublePixelPacket));
          for (high.opacity=MaxRange(MaxRGB); high.opacity != 0; high.opacity--)
            {
              intensity.opacity+=histogram[(long) high.opacity].opacity;
              if (intensity.opacity > threshold_intensity)
                break;
            }
        }
    }
  MagickFreeMemory(histogram);
  /*
    Stretch the histogram to create the normalized image mapping.
  */
  (void) memset(normalize_map,0,(MaxMap+1)*sizeof(PixelPacket));
  for (i=0; i <= (long) MaxMap; i++)
  {
    if (i < (long) low.red)
      normalize_map[i].red=0;
    else
      if (i > (long) high.red)
        normalize_map[i].red=MaxRGB;
      else
        if (low.red != high.red)
          normalize_map[i].red=
            ScaleMapToQuantum((MaxMap*(i-low.red))/(high.red-low.red));
    if (i < (long) low.green)
      normalize_map[i].green=0;
    else
      if (i > (long) high.green)
        normalize_map[i].green=MaxRGB;
      else
        if (low.green != high.green)
          normalize_map[i].green=ScaleMapToQuantum((MaxMap*(i-low.green))/
            (high.green-low.green));
    if (i < (long) low.blue)
      normalize_map[i].blue=0;
    else
      if (i > (long) high.blue)
        normalize_map[i].blue=MaxRGB;
      else
        if (low.blue != high.blue)
          normalize_map[i].blue=
            ScaleMapToQuantum((MaxMap*(i-low.blue))/(high.blue-low.blue));
    if (image->matte)
      {
        if (i < (long) low.opacity)
          normalize_map[i].opacity=0;
        else
          if (i > (long) high.opacity)
            normalize_map[i].opacity=MaxRGB;
          else
            if (low.opacity != high.opacity)
              normalize_map[i].opacity=ScaleMapToQuantum(
                (MaxMap*(i-low.opacity))/(high.opacity-low.opacity));
      }
  }
  /*
    Normalize the image.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      /*
        Normalize DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        if (image->matte)
          for (x=(long) image->columns; x > 0; x--)
            {
              if (low.red != high.red)
                q->red=normalize_map[ScaleQuantumToMap(q->red)].red;
              if (low.green != high.green)
                q->green=normalize_map[ScaleQuantumToMap(q->green)].green;
              if (low.blue != high.blue)
                q->blue=normalize_map[ScaleQuantumToMap(q->blue)].blue;
              if (low.opacity != high.opacity)
                q->opacity=normalize_map[ScaleQuantumToMap(q->opacity)].opacity;
              q++;
            }
        else
          for (x=(long) image->columns; x > 0; x--)
            {
              if (low.red != high.red)
                q->red=normalize_map[ScaleQuantumToMap(q->red)].red;
              if (low.green != high.green)
                q->green=normalize_map[ScaleQuantumToMap(q->green)].green;
              if (low.blue != high.blue)
                q->blue=normalize_map[ScaleQuantumToMap(q->blue)].blue;
              q++;
            }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(NormalizeImageText,y,image->rows,exception))
            {
              status=MagickFail;
              break;
            }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Normalize PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        if (low.red != high.red)
          image->colormap[i].red=
            normalize_map[ScaleQuantumToMap(image->colormap[i].red)].red;
        if (low.green != high.green)
          image->colormap[i].green=
            normalize_map[ScaleQuantumToMap(image->colormap[i].green)].green;
        if (low.blue != high.blue)
          image->colormap[i].blue=
            normalize_map[ScaleQuantumToMap(image->colormap[i].blue)].blue;
      }
      status &= SyncImage(image);
      break;
    }
  }
  MagickFreeMemory(normalize_map);
  image->is_grayscale=is_grayscale;
  return(status);
}
