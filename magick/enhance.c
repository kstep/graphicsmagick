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
              MagickMonitor(SharpenContrastImageText,y,image->rows);
            else
              MagickMonitor(DullContrastImageText,y,image->rows);
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

  typedef struct _HistogramPacket
  {
    unsigned long
      red,
      green,
      blue,
      opacity;
  } HistogramPacket;

  HistogramPacket
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
  histogram=(HistogramPacket *)
    AcquireMemory((MaxRGB+1)*sizeof(HistogramPacket));
  map=(HistogramPacket *) AcquireMemory((MaxRGB+1)*sizeof(HistogramPacket));
  equalize_map=(PixelPacket *) AcquireMemory((MaxRGB+1)*sizeof(PixelPacket));
  if ((histogram == (HistogramPacket *) NULL) ||
      (map == (HistogramPacket *) NULL) ||
      (equalize_map == (PixelPacket *) NULL))
    ThrowBinaryException(ResourceLimitError,"Unable to equalize image",
      "Memory allocation failed");
  /*
    Form histogram.
  */
  memset(histogram,0,(MaxRGB+1)*sizeof(HistogramPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      histogram[p->red].red++;
      histogram[p->green].green++;
      histogram[p->blue].blue++;
      histogram[p->opacity].opacity++;
      p++;
    }
  }
  /*
    Integrate the histogram to get the equalization map.
  */
  memset(&intensity,0,sizeof(HistogramPacket));
  for (i=0; i <= MaxRGB; i++)
  {
    intensity.red+=histogram[i].red;
    intensity.green+=histogram[i].green;
    intensity.blue+=histogram[i].blue;
    intensity.opacity+=histogram[i].opacity;
    map[i]=intensity;
  }
  low=map[0];
  high=map[MaxRGB];
  memset(equalize_map,0,(MaxRGB+1)*sizeof(PixelPacket));
  for (i=0; i <= MaxRGB; i++)
  {
    if (high.red != low.red)
      equalize_map[i].red=(Quantum) ((((double)
        (map[i].red-low.red))*MaxRGB)/(high.red-low.red));
    if (high.green != low.green)
      equalize_map[i].green=(Quantum) ((((double)
        (map[i].green-low.green))*MaxRGB)/(high.green-low.green));
    if (high.blue != low.blue)
      equalize_map[i].blue=(Quantum) ((((double)
        (map[i].blue-low.blue))*MaxRGB)/(high.blue-low.blue));
    if (high.opacity != low.opacity)
      equalize_map[i].opacity=(Quantum) ((((double)
        (map[i].opacity-low.opacity))*MaxRGB)/(high.opacity-low.opacity));
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
            q->red=equalize_map[q->red].red;
          if (low.green != high.green)
            q->green=equalize_map[q->green].green;
          if (low.blue != high.blue)
            q->blue=equalize_map[q->blue].blue;
          if (low.opacity != high.opacity)
            q->opacity=equalize_map[q->opacity].opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(EqualizeImageText,y,image->rows);
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
          image->colormap[i].red=equalize_map[image->colormap[i].red].red;
        if (low.green != high.green)
          image->colormap[i].green=equalize_map[image->colormap[i].green].green;
        if (low.blue != high.blue)
          image->colormap[i].blue=equalize_map[image->colormap[i].blue].blue;
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
%      unsigned int GammaImage(Image *image,const char *gamma)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o gamma: Define the level of gamma correction.
%
%
*/
MagickExport unsigned int GammaImage(Image *image,const char *gamma)
{
#define GammaImageText  "  Gamma correcting the image...  "

  typedef struct _GammaPacket
  {
    unsigned short 
      red,
      green,
      blue,
      opacity;
  } GammaPacket;

  double
    blue_gamma,
    green_gamma,
    red_gamma;

  GammaPacket
    *gamma_map;

  int
    count;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (gamma == (char *) NULL)
    return(False);
  red_gamma=1.0;
  green_gamma=1.0;
  blue_gamma=1.0;
  count=sscanf(gamma,"%lf%*[,/]%lf%*[,/]%lf",&red_gamma,&green_gamma,
    &blue_gamma);
  if (count == 1)
    {
      if (red_gamma == 1.0)
        return(False);
      green_gamma=red_gamma;
      blue_gamma=red_gamma;
    }
  /*
    Allocate and initialize gamma maps.
  */
  length=(ScaleQuantumToShort(MaxRGB)+1)*sizeof(GammaPacket);
  gamma_map=(GammaPacket *) AcquireMemory(length);
  if (gamma_map == (GammaPacket *) NULL)
    ThrowBinaryException(ResourceLimitError,"Unable to gamma correct image",
      "Memory allocation failed");
  (void) memset(gamma_map,0,length);
  for (i=0; i <= ScaleQuantumToShort(MaxRGB); i++)
  {
    if (red_gamma != 0.0)
      gamma_map[i].red=(unsigned long) ((pow((double) i/
        ScaleQuantumToShort(MaxRGB),1.0/red_gamma)*
        ScaleQuantumToShort(MaxRGB))+0.5);
    if (green_gamma != 0.0)
      gamma_map[i].green=(unsigned long) ((pow((double) i/
        ScaleQuantumToShort(MaxRGB),1.0/green_gamma)*
        ScaleQuantumToShort(MaxRGB))+0.5);
    if (blue_gamma != 0.0)
      gamma_map[i].blue=(unsigned long) ((pow((double) i/
        ScaleQuantumToShort(MaxRGB),1.0/blue_gamma)*
        ScaleQuantumToShort(MaxRGB))+0.5);
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
          q->red=ScaleShortToQuantum(gamma_map[q->red].red);
          q->green=ScaleShortToQuantum(gamma_map[q->green].green);
          q->blue=ScaleShortToQuantum(gamma_map[q->blue].blue);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(GammaImageText,y,image->rows);
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
          ScaleShortToQuantum(gamma_map[image->colormap[i].red].red);
        image->colormap[i].green=
          ScaleShortToQuantum(gamma_map[image->colormap[i].green].green);
        image->colormap[i].blue=
          ScaleShortToQuantum(gamma_map[image->colormap[i].blue].blue);
      }
      SyncImage(image);
      break;
    }
  }
  if (image->gamma != 0.0)
    image->gamma*=(red_gamma+green_gamma+blue_gamma)/3.0;
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
%  mid, and white.
%
%  The format of the LevelImage method is:
%
%      unsigned int LevelImage(Image *image,const char *level)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o levels: Define the levels.
%
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

  Quantum
    *levels_map;

  register long
    i,
    x;

  register PixelPacket
    *q;

  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (levels == (char *) NULL)
    return(False);
  black_point=0.0;
  mid_point=1.0;
  white_point=MaxRGB;
  count=sscanf(levels,"%lf%*[,/]%lf%*[,/]%lf",&black_point,&mid_point,
    &white_point);
  if (count == 1)
    white_point=MaxRGB-black_point;
  levels_map=(Quantum *) AcquireMemory((MaxRGB+1)*sizeof(Quantum));
  if (levels_map == (Quantum *) NULL)
    ThrowBinaryException(ResourceLimitError,"Unable to level the image",
      "Memory allocation failed");
  for (i=0; i <= MaxRGB; i++)
  {
    if (i < black_point)
      {
        levels_map[i]=0;
        continue;
      }
    if (i > white_point)
      {
        levels_map[i]=MaxRGB;
        continue;
      }
    levels_map[i]=(Quantum) ((pow((double) (i-black_point)/
      (white_point-black_point),1.0/mid_point)*MaxRGB)+0.5);
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
          q->red=levels_map[q->red];
          q->green=levels_map[q->green];
          q->blue=levels_map[q->blue];
          q->opacity=levels_map[q->opacity];
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(GammaImageText,y,image->rows);
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
        image->colormap[i].red=levels_map[image->colormap[i].red];
        image->colormap[i].green=levels_map[image->colormap[i].green];
        image->colormap[i].blue=levels_map[image->colormap[i].blue];
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
          MagickMonitor(ModulateImageText,y,image->rows);
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
          MagickMonitor(NegateImageText,y,image->rows);
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
#define NormalizeImageText  "  Normalizing image...  "

  typedef struct _HistogramPacket
  {
    unsigned long
      red,
      green,
      blue,
      opacity;
  } HistogramPacket;

  HistogramPacket
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
  histogram=(HistogramPacket *)
    AcquireMemory((MaxRGB+1)*sizeof(HistogramPacket));
  normalize_map=(PixelPacket *) AcquireMemory((MaxRGB+1)*sizeof(PixelPacket));
  if ((histogram == (HistogramPacket *) NULL) ||
      (normalize_map == (PixelPacket *) NULL))
    ThrowBinaryException(ResourceLimitError,"Unable to normalize image",
      "Memory allocation failed");
  /*
    Form histogram.
  */
  memset(histogram,0,(MaxRGB+1)*sizeof(HistogramPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      histogram[p->red].red++;
      histogram[p->green].green++;
      histogram[p->blue].blue++;
      histogram[p->opacity].opacity++;
      p++;
    }
  }
  /*
    Find the histogram boundaries by locating the 1 percent levels.
  */
  threshold_intensity=(long) (image->columns*image->rows)/100;
  memset(&intensity,0,sizeof(HistogramPacket));
  for (low.red=0; low.red < MaxRGB; low.red++)
  {
    intensity.red+=histogram[low.red].red;
    if (intensity.red > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (high.red=MaxRGB; high.red != 0; high.red--)
  {
    intensity.red+=histogram[high.red].red;
    if (intensity.red > threshold_intensity)
      break;
  }
  if (low.red == high.red)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      memset(&intensity,0,sizeof(HistogramPacket));
      for (low.red=0; low.red < MaxRGB; low.red++)
      {
        intensity.red+=histogram[low.red].red;
        if (intensity.red > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(HistogramPacket));
      for (high.red=MaxRGB; high.red != 0; high.red--)
      {
        intensity.red+=histogram[high.red].red;
        if (intensity.red > threshold_intensity)
          break;
      }
    }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (low.green=0; low.green < MaxRGB; low.green++)
  {
    intensity.green+=histogram[low.green].green;
    if (intensity.green > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (high.green=MaxRGB; high.green != 0; high.green--)
  {
    intensity.green+=histogram[high.green].green;
    if (intensity.green > threshold_intensity)
      break;
  }
  if (low.green == high.green)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      memset(&intensity,0,sizeof(HistogramPacket));
      for (low.green=0; low.green < MaxRGB; low.green++)
      {
        intensity.green+=histogram[low.green].green;
        if (intensity.green > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(HistogramPacket));
      for (high.green=MaxRGB; high.green != 0; high.green--)
      {
        intensity.green+=histogram[high.green].green;
        if (intensity.green > threshold_intensity)
          break;
      }
    }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (low.blue=0; low.blue < MaxRGB; low.blue++)
  {
    intensity.blue+=histogram[low.blue].blue;
    if (intensity.blue > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (high.blue=MaxRGB; high.blue != 0; high.blue--)
  {
    intensity.blue+=histogram[high.blue].blue;
    if (intensity.blue > threshold_intensity)
      break;
  }
  if (low.blue == high.blue)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      memset(&intensity,0,sizeof(HistogramPacket));
      for (low.blue=0; low.blue < MaxRGB; low.blue++)
      {
        intensity.blue+=histogram[low.blue].blue;
        if (intensity.blue > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(HistogramPacket));
      for (high.blue=MaxRGB; high.blue != 0; high.blue--)
      {
        intensity.blue+=histogram[high.blue].blue;
        if (intensity.blue > threshold_intensity)
          break;
      }
    }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (low.opacity=0; low.opacity < MaxRGB; low.opacity++)
  {
    intensity.opacity+=histogram[low.opacity].opacity;
    if (intensity.opacity > threshold_intensity)
      break;
  }
  memset(&intensity,0,sizeof(HistogramPacket));
  for (high.opacity=MaxRGB; high.opacity != 0; high.opacity--)
  {
    intensity.opacity+=histogram[high.opacity].opacity;
    if (intensity.opacity > threshold_intensity)
      break;
  }
  if (low.opacity == high.opacity)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      memset(&intensity,0,sizeof(HistogramPacket));
      for (low.opacity=0; low.opacity < MaxRGB; low.opacity++)
      {
        intensity.opacity+=histogram[low.opacity].opacity;
        if (intensity.opacity > threshold_intensity)
          break;
      }
      memset(&intensity,0,sizeof(HistogramPacket));
      for (high.opacity=MaxRGB; high.opacity != 0; high.opacity--)
      {
        intensity.opacity+=histogram[high.opacity].opacity;
        if (intensity.opacity > threshold_intensity)
          break;
      }
    }
  LiberateMemory((void **) &histogram);
  /*
    Stretch the histogram to create the normalized image mapping.
  */
  memset(normalize_map,0,(MaxRGB+1)*sizeof(PixelPacket));
  for (i=0; i <= MaxRGB; i++)
  {
    if (i < (long) low.red)
      normalize_map[i].red=0;
    else
      if (i > (long) high.red)
        normalize_map[i].red=MaxRGB;
      else
        if (low.red != high.red)
          normalize_map[i].red=(Quantum) ((MaxRGB*(double) (i-low.red))/
            (double) (high.red-low.red));
    if (i < (long) low.green)
      normalize_map[i].green=0;
    else
      if (i > (long) high.green)
        normalize_map[i].green=MaxRGB;
      else
        if (low.green != high.green)
          normalize_map[i].green=(Quantum) ((MaxRGB*(double) (i-low.green))/
            (double) (high.green-low.green));
    if (i < (long) low.blue)
      normalize_map[i].blue=0;
    else
      if (i > (long) high.blue)
        normalize_map[i].blue=MaxRGB;
      else
        if (low.blue != high.blue)
          normalize_map[i].blue=(Quantum) ((MaxRGB*(double) (i-low.blue))/
            (double) (high.blue-low.blue));
    if (i < (long) low.opacity)
      normalize_map[i].opacity=0;
    else
      if (i > (long) high.opacity)
        normalize_map[i].opacity=MaxRGB;
      else
        if (low.opacity != high.opacity)
          normalize_map[i].opacity=(Quantum) ((MaxRGB*(double) (i-low.opacity))/
            (double) (high.opacity-low.opacity));
  }
  /*
    Normalize the image.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Normalize DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (low.red != high.red)
            q->red=normalize_map[q->red].red;
          if (low.green != high.green)
            q->green=normalize_map[q->green].green;
          if (low.blue != high.blue)
            q->blue=normalize_map[q->blue].blue;
          if (low.opacity != high.opacity)
            q->opacity=normalize_map[q->opacity].opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(NormalizeImageText,y,image->rows);
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
          image->colormap[i].red=normalize_map[image->colormap[i].red].red;
        if (low.green != high.green)
          image->colormap[i].green=
            normalize_map[image->colormap[i].green].green;
        if (low.blue != high.blue)
          image->colormap[i].blue=normalize_map[image->colormap[i].blue].blue;
      }
      SyncImage(image);
      break;
    }
  }
  LiberateMemory((void **) &normalize_map);
  return(True);
}
