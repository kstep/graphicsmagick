/*
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
%                    ImageMagick Image Enhancement Methods                    %
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
#include "studio.h"
#include "gem.h"
#include "monitor.h"
#include "utility.h"

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
MagickExport unsigned int ContrastImage(Image *image,const unsigned int sharpen)
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

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  sign=sharpen ? 1 : -1;
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      unsigned int
        status;

      /*
        Contrast enhance DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          Contrast(sign,&q->red,&q->green,&q->blue);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          {
            if (sharpen)
              status=MagickMonitor(SharpenContrastImageText,y,image->rows,
                &image->exception);
            else
              status=MagickMonitor(DullContrastImageText,y,image->rows,
                &image->exception);
            if (status == False)
              break;
          }
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Contrast enhance PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
        Contrast(sign,&image->colormap[i].red,&image->colormap[i].green,
          &image->colormap[i].blue);
      SyncImage(image);
      break;
    }
  }
  return(False);
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
MagickExport unsigned int EqualizeImage(Image *image)
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

  register PixelPacket
    *q;

  /*
    Allocate and initialize histogram arrays.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  histogram=(DoublePixelPacket *)
    AcquireMemory(65536L*sizeof(DoublePixelPacket));
  map=(DoublePixelPacket *) AcquireMemory(65536L*sizeof(DoublePixelPacket));
  equalize_map=(PixelPacket *) AcquireMemory(65536L*sizeof(PixelPacket));
  if ((histogram == (DoublePixelPacket *) NULL) ||
      (map == (DoublePixelPacket *) NULL) ||
      (equalize_map == (PixelPacket *) NULL))
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToEqualizeImage");
  /*
    Form histogram.
  */
  memset(histogram,0,65536L*sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      histogram[ScaleQuantumToShort(p->red)].red++;
      histogram[ScaleQuantumToShort(p->green)].green++;
      histogram[ScaleQuantumToShort(p->blue)].blue++;
      histogram[ScaleQuantumToShort(p->opacity)].opacity++;
      p++;
    }
  }
  /*
    Integrate the histogram to get the equalization map.
  */
  memset(&intensity,0,sizeof(DoublePixelPacket));
  for (i=0; i <= 65535L; i++)
  {
    intensity.red+=histogram[i].red;
    intensity.green+=histogram[i].green;
    intensity.blue+=histogram[i].blue;
    intensity.opacity+=histogram[i].opacity;
    map[i]=intensity;
  }
  low=map[0];
  high=map[65535L];
  memset(equalize_map,0,65536L*sizeof(PixelPacket));
  for (i=0; i <= 65535L; i++)
  {
    if (high.red != low.red)
      equalize_map[i].red=ScaleShortToQuantum(
        (65535L*(map[i].red-low.red))/(high.red-low.red)+0.5);
    if (high.green != low.green)
      equalize_map[i].green=ScaleShortToQuantum(
        (65535L*(map[i].green-low.green))/(high.green-low.green)+0.5);
    if (high.blue != low.blue)
      equalize_map[i].blue=ScaleShortToQuantum(
        (65535L*(map[i].blue-low.blue))/(high.blue-low.blue)+0.5);
    if (high.opacity != low.opacity)
      equalize_map[i].opacity=ScaleShortToQuantum(
        (65535L*(map[i].opacity-low.opacity))/(high.opacity-low.opacity)+0.5);
  }
  LiberateMemory((void **) &histogram);
  LiberateMemory((void **) &map);
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
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (low.red != high.red)
            q->red=equalize_map[ScaleQuantumToShort(q->red)].red;
          if (low.green != high.green)
            q->green=equalize_map[ScaleQuantumToShort(q->green)].green;
          if (low.blue != high.blue)
            q->blue=equalize_map[ScaleQuantumToShort(q->blue)].blue;
          if (low.opacity != high.opacity)
            q->opacity=equalize_map[ScaleQuantumToShort(q->opacity)].opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(EqualizeImageText,y,image->rows,&image->exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Equalize PseudoClass packets.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        if (low.red != high.red)
          image->colormap[i].red=
            equalize_map[ScaleQuantumToShort(image->colormap[i].red)].red;
        if (low.green != high.green)
          image->colormap[i].green=
            equalize_map[ScaleQuantumToShort(image->colormap[i].green)].green;
        if (low.blue != high.blue)
          image->colormap[i].blue=
            equalize_map[ScaleQuantumToShort(image->colormap[i].blue)].blue;
      }
      SyncImage(image);
      break;
    }
  }
  LiberateMemory((void **) &equalize_map);
  return(True);
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
MagickExport unsigned int GammaImage(Image *image,const char *level)
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

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (level == (char *) NULL)
    return(False);
  gamma.red=1.0;
  gamma.green=1.0;
  gamma.blue=1.0;
  count=sscanf(level,"%lf%*[,/]%lf%*[,/]%lf",&gamma.red,&gamma.green,
    &gamma.blue);
  if (count == 1)
    {
      if (gamma.red == 1.0)
        return(False);
      gamma.green=gamma.red;
      gamma.blue=gamma.red;
    }
  /*
    Allocate and initialize gamma maps.
  */
  gamma_map=(PixelPacket *) AcquireMemory(65536L*sizeof(PixelPacket));
  if (gamma_map == (PixelPacket *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToGammaCorrectImage");
  (void) memset(gamma_map,0,65536L*sizeof(PixelPacket));
  for (i=0; i <= 65535L; i++)
  {
    if (gamma.red != 0.0)
      gamma_map[i].red=
        ScaleShortToQuantum((65535L*pow((double) i/65535L,1.0/gamma.red))+0.5);
    if (gamma.green != 0.0)
      gamma_map[i].green=ScaleShortToQuantum(
       (65535L*pow((double) i/65535L,1.0/gamma.green))+0.5);
    if (gamma.blue != 0.0)
      gamma_map[i].blue=
        ScaleShortToQuantum((65535L*pow((double) i/65535L,1.0/gamma.blue))+0.5);
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
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=gamma_map[ScaleQuantumToShort(q->red)].red;
          q->green=gamma_map[ScaleQuantumToShort(q->green)].green;
          q->blue=gamma_map[ScaleQuantumToShort(q->blue)].blue;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(GammaImageText,y,image->rows,&image->exception))
            break;
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
          gamma_map[ScaleQuantumToShort(image->colormap[i].red)].red;
        image->colormap[i].green=
          gamma_map[ScaleQuantumToShort(image->colormap[i].green)].green;
        image->colormap[i].blue=
          gamma_map[ScaleQuantumToShort(image->colormap[i].blue)].blue;
      }
      SyncImage(image);
      break;
    }
  }
  if (image->gamma != 0.0)
    image->gamma*=(gamma.red+gamma.green+gamma.blue)/3.0;
  LiberateMemory((void **) &gamma_map);
  return(True);
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
%  LevelImage() adjusts the levels of an image given these points:  black,
%  mid, and white. Black specifies the darkest color in the image. Colors
%  darker than the Black value are set to the Black value. Mid specifies a
%  gamma correction to apply to the image.  White specifies the lightest color
%  in the image. Colors brighter than the White value are set to the White
%  value.
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
%              (e.g. "10,1.0,65000) where black and white have the range of
%              0-MaxRGB, and mid has the range 0-10.
%
*/
MagickExport unsigned int LevelImage(Image *image,const char *levels)
{
#define LevelImageText  "  Leveling the image...  "

  double
    black_point,
    mid_point,
    white_point;

  int
    count;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned short
    *levels_map;

  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (levels == (char *) NULL)
    return(False);
  black_point=0.0;
  mid_point=1.0;
  white_point=65535L;
  count=sscanf(levels,"%lf%*[,/]%lf%*[,/]%lf",&black_point,&mid_point,
    &white_point);
  if (count == 1)
    white_point=65535L-black_point;
  levels_map=(unsigned short *) AcquireMemory(65536L*sizeof(unsigned short));
  if (levels_map == (unsigned short *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToLevelTheImage");
  for (i=0; i <= 65535L; i++)
  {
    if (i < black_point)
      {
        levels_map[i]=0;
        continue;
      }
    if (i > white_point)
      {
        levels_map[i]=65535L;
        continue;
      }
    levels_map[i]=(unsigned short) (65535L*(pow(((double) i-black_point)/
      (white_point-black_point),1.0/mid_point)));
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
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=ScaleShortToQuantum(
            levels_map[ScaleQuantumToShort(q->red)]);
          q->green=ScaleShortToQuantum(
            levels_map[ScaleQuantumToShort(q->green)]);
          q->blue=ScaleShortToQuantum(
            levels_map[ScaleQuantumToShort(q->blue)]);
          q->opacity=ScaleShortToQuantum(
            levels_map[ScaleQuantumToShort(q->opacity)]);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(GammaImageText,y,image->rows,&image->exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Level PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].red=ScaleShortToQuantum(
          levels_map[ScaleQuantumToShort(image->colormap[i].red)]);
        image->colormap[i].green=ScaleShortToQuantum(
          levels_map[ScaleQuantumToShort(image->colormap[i].green)]);
        image->colormap[i].blue=ScaleShortToQuantum(
          levels_map[ScaleQuantumToShort(image->colormap[i].blue)]);
      }
      SyncImage(image);
      break;
    }
  }
  LiberateMemory((void **) &levels_map);
  return(True);
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
MagickExport unsigned int ModulateImage(Image *image,const char *modulate)
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

  register PixelPacket
    *q;

  /*
    Initialize gamma table.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (modulate == (char *) NULL)
    return(False);
  percent_brightness=100.0;
  percent_saturation=100.0;
  percent_hue=100.0;
  (void) sscanf(modulate,"%lf%*[,/]%lf%*[,/]%lf",&percent_brightness,
    &percent_saturation,&percent_hue);
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
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          Modulate(percent_hue,percent_saturation,percent_brightness,
            &q->red,&q->green,&q->blue);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(ModulateImageText,y,image->rows,&image->exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Modulate the color for a PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
        Modulate(percent_hue,percent_saturation,percent_brightness,
          &image->colormap[i].red,&image->colormap[i].green,
          &image->colormap[i].blue);
      SyncImage(image);
      break;
    }
  }
  return(True);
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
MagickExport unsigned int NegateImage(Image *image,const unsigned int grayscale)
{
#define NegateImageText  "  Negating the image colors...  "

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
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
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (grayscale)
            if ((q->red != q->green) || (q->green != q->blue))
              {
                q++;
                continue;
              }
          q->red=(~q->red);
          q->green=(~q->green);
          q->blue=(~q->blue);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(NegateImageText,y,image->rows,&image->exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Negate PseudoClass packets.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        if (grayscale)
          if ((image->colormap[i].red != image->colormap[i].green) ||
              (image->colormap[i].green != image->colormap[i].blue))
            continue;
        image->colormap[i].red=(~image->colormap[i].red);
        image->colormap[i].green=(~image->colormap[i].green);
        image->colormap[i].blue=(~image->colormap[i].blue);
      }
      SyncImage(image);
      break;
    }
  }
  return(True);
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
MagickExport unsigned int NormalizeImage(Image *image)
{
#define MaxRange(color)  ScaleQuantumToShort(color)
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

  unsigned long
    threshold_intensity;

  /*
    Allocate histogram and normalize map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  histogram=(DoublePixelPacket *)
    AcquireMemory(65536L*sizeof(DoublePixelPacket));
  normalize_map=(PixelPacket *) AcquireMemory(65536L*sizeof(PixelPacket));
  if ((histogram == (DoublePixelPacket *) NULL) ||
      (normalize_map == (PixelPacket *) NULL))
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToNormalizeImage");
  /*
    Form histogram.
  */
  memset(histogram,0,65536L*sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      histogram[ScaleQuantumToShort(p->red)].red++;
      histogram[ScaleQuantumToShort(p->green)].green++;
      histogram[ScaleQuantumToShort(p->blue)].blue++;
      histogram[ScaleQuantumToShort(p->opacity)].opacity++;
      p++;
    }
  }
  /*
    Find the histogram boundaries by locating the 0.1 percent levels.
  */
  threshold_intensity=(long) (image->columns*image->rows)/1000;
  memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.red=0; low.red < 65535L; low.red++)
  {
    intensity.red+=histogram[(long) low.red].red;
    if (intensity.red > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.red=65535L; high.red != 0; high.red--)
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
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.red=0; low.red < MaxRange(MaxRGB); low.red++)
      {
        intensity.red+=histogram[(long) low.red].red;
        if (intensity.red > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.red=MaxRange(MaxRGB); high.red != 0; high.red--)
      {
        intensity.red+=histogram[(long) high.red].red;
        if (intensity.red > threshold_intensity)
          break;
      }
    }
  memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.green=0; low.green < MaxRange(MaxRGB); low.green++)
  {
    intensity.green+=histogram[(long) low.green].green;
    if (intensity.green > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(DoublePixelPacket));
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
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.green=0; low.green < MaxRange(MaxRGB); low.green++)
      {
        intensity.green+=histogram[(long) low.green].green;
        if (intensity.green > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.green=MaxRange(MaxRGB); high.green != 0; high.green--)
      {
        intensity.green+=histogram[(long) high.green].green;
        if (intensity.green > threshold_intensity)
          break;
      }
    }
  memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.blue=0; low.blue < MaxRange(MaxRGB); low.blue++)
  {
    intensity.blue+=histogram[(long) low.blue].blue;
    if (intensity.blue > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(DoublePixelPacket));
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
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.blue=0; low.blue < MaxRange(MaxRGB); low.blue++)
      {
        intensity.blue+=histogram[(long) low.blue].blue;
        if (intensity.blue > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.blue=MaxRange(MaxRGB); high.blue != 0; high.blue--)
      {
        intensity.blue+=histogram[(long) high.blue].blue;
        if (intensity.blue > threshold_intensity)
          break;
      }
    }
  memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.opacity=0; low.opacity < MaxRange(MaxRGB); low.opacity++)
  {
    intensity.opacity+=histogram[(long) low.opacity].opacity;
    if (intensity.opacity > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(DoublePixelPacket));
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
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.opacity=0; low.opacity < MaxRange(MaxRGB); low.opacity++)
      {
        intensity.opacity+=histogram[(long) low.opacity].opacity;
        if (intensity.opacity > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.opacity=MaxRange(MaxRGB); high.opacity != 0; high.opacity--)
      {
        intensity.opacity+=histogram[(long) high.opacity].opacity;
        if (intensity.opacity > threshold_intensity)
          break;
      }
    }
  LiberateMemory((void **) &histogram);
  /*
    Stretch the histogram to create the normalized image mapping.
  */
  memset(normalize_map,0,65536L*sizeof(PixelPacket));
  for (i=0; i <= 65535L; i++)
  {
    if (i < (long) low.red)
      normalize_map[i].red=0;
    else
      if (i > (long) high.red)
        normalize_map[i].red=MaxRGB;
      else
        if (low.red != high.red)
          normalize_map[i].red=
            ScaleShortToQuantum((65535L*(i-low.red))/(high.red-low.red)+0.5);
    if (i < (long) low.green)
      normalize_map[i].green=0;
    else
      if (i > (long) high.green)
        normalize_map[i].green=MaxRGB;
      else
        if (low.green != high.green)
          normalize_map[i].green=ScaleShortToQuantum((65535L*(i-low.green))/
            (high.green-low.green)+0.5);
    if (i < (long) low.blue)
      normalize_map[i].blue=0;
    else
      if (i > (long) high.blue)
        normalize_map[i].blue=MaxRGB;
      else
        if (low.blue != high.blue)
          normalize_map[i].blue=
            ScaleShortToQuantum((65535L*(i-low.blue))/(high.blue-low.blue)+0.5);
    if (i < (long) low.opacity)
      normalize_map[i].opacity=0;
    else
      if (i > (long) high.opacity)
        normalize_map[i].opacity=(Quantum) 65535L;
      else
        if (low.opacity != high.opacity)
          normalize_map[i].opacity=ScaleShortToQuantum(
            (65535L*(i-low.opacity))/(high.opacity-low.opacity)+0.5);
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
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (low.red != high.red)
            q->red=normalize_map[ScaleQuantumToShort(q->red)].red;
          if (low.green != high.green)
            q->green=normalize_map[ScaleQuantumToShort(q->green)].green;
          if (low.blue != high.blue)
            q->blue=normalize_map[ScaleQuantumToShort(q->blue)].blue;
          if (low.opacity != high.opacity)
            q->opacity=normalize_map[ScaleQuantumToShort(q->opacity)].opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(NormalizeImageText,y,image->rows,exception))
            break;
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
            normalize_map[ScaleQuantumToShort(image->colormap[i].red)].red;
        if (low.green != high.green)
          image->colormap[i].green=
            normalize_map[ScaleQuantumToShort(image->colormap[i].green)].green;
        if (low.blue != high.blue)
          image->colormap[i].blue=
            normalize_map[ScaleQuantumToShort(image->colormap[i].blue)].blue;
      }
      SyncImage(image);
      break;
    }
  }
  LiberateMemory((void **) &normalize_map);
  return(True);
}
