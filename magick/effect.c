/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   EEEEE  FFFFF  FFFFF  EEEEE  CCCC  TTTTT                   %
%                   E      F      F      E     C        T                     %
%                   EEE    FFF    FFF    EEE   C        T                     %
%                   E      F      F      E     C        T                     %
%                   EEEEE  F      F      EEEEE  CCCC    T                     %
%                                                                             %
%                                                                             %
%                      ImageMagick Image Effects Methods                      %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 October 1996                                %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,            %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
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
#include "cache.h"
#include "effect.h"
#include "enhance.h"
#include "fx.h"
#include "gem.h"
#include "monitor.h"
#include "render.h"
#include "utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d a p t i v e T h r e s h o l d I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AdaptiveThresholdImage() selects an individual threshold for each pixel
%  based on the range of intensity values in its local neighborhood.  This
%  allows for thresholding of an image whose global intensity histogram
%  doesn't contain distinctive peaks.
%
%  The format of the AdaptiveThresholdImage method is:
%
%      Image *AdaptiveThresholdImage(Image *image,const unsigned long width,
%        const unsigned long height,const unsigned long unsigned long,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o width: The width of the local neighborhood.
%
%    o height: The height of the local neighborhood.
%
%    o offset: The mean offset.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AdaptiveThresholdImage(const Image *image,
  const unsigned long width,const unsigned long height,
  const double offset,ExceptionInfo *exception)
{
#define ThresholdImageText  "  Threshold the image...  "

  DoublePixelPacket
    aggregate,
    mean,
    zero;

  Image
    *threshold_image;

  long
    y;

  register const PixelPacket
    *p,
    *r;

  register long
    x,
    u,
    v;

  register PixelPacket
    *q;

  /*
    Initialize thresholded image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < width) || (image->rows < height))
    ThrowImageException(OptionError,"UnableToThresholdImage",
      "ImageSmallerThanRadius");
  threshold_image=CloneImage(image,0,0,True,exception);
  if (threshold_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(threshold_image,TrueColorType);
  /*
    Threshold each row of the image.
  */
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,-(long) width/2,y-height/2,image->columns+width,
      height,exception);
    q=SetImagePixels(threshold_image,0,y,threshold_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      r=p;
      aggregate=zero;
      for (v=0; v < (long) height; v++)
      {
        for (u=0; u < (long) width; u++)
        {
          aggregate.red+=r[u].red;
          aggregate.green+=r[u].green;
          aggregate.blue+=r[u].blue;
          aggregate.opacity+=r[u].opacity;
        }
        r+=image->columns+width;
      }
      mean.red=aggregate.red/(width*height)+offset;
      mean.green=aggregate.green/(width*height)+offset;
      mean.blue=aggregate.blue/(width*height)+offset;
      mean.opacity=aggregate.opacity/(width*height)+offset;
      q->red=q->red <= mean.red ? 0 : MaxRGB;
      q->green=q->green <= mean.green ? 0 : MaxRGB;
      q->blue=q->blue <= mean.blue ? 0 : MaxRGB;
      q->opacity=q->opacity <= mean.opacity ? 0 : MaxRGB;
      p++;
      q++;
    }
    if (!SyncImagePixels(threshold_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ThresholdImageText,y,image->rows,exception))
        break;
  }
  return(threshold_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d d N o i s e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddNoiseImage() adds random noise to the image.
%
%  The format of the AddNoiseImage method is:
%
%      Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o noise_type:  The type of noise: Uniform, Gaussian, Multiplicative,
%      Impulse, Laplacian, or Poisson.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
  ExceptionInfo *exception)
{
#define AddNoiseImageText  "  Add noise to the image...  "

  Image
    *noise_image;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize noise image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  noise_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  noise_image->storage_class=DirectClass;
  /*
    Add noise in each row.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=SetImagePixels(noise_image,0,y,noise_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=GenerateNoise(p->red,noise_type);
      q->green=GenerateNoise(p->green,noise_type);
      q->blue=GenerateNoise(p->blue,noise_type);
      p++;
      q++;
    }
    if (!SyncImagePixels(noise_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(AddNoiseImageText,y,image->rows,exception))
        break;
  }
  return(noise_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l u r I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlurImage() blurs an image.  We convolve the image with a Gaussian
%  operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and BlurImage() selects a suitable radius for you.
%
%  The format of the BlurImage method is:
%
%      Image *BlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static void BlurScanline(const double *kernel,const unsigned long width,
  const PixelPacket *source,PixelPacket *destination,
  const unsigned long columns)
{
  double
    scale;

  DoublePixelPacket
    aggregate,
    zero;

  register const double
    *p;

  register const PixelPacket
    *q;

  register long
    i,
    x;

  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  if (width > columns)
    {
      for (x=0; x < (long) columns; x++)
      {
        aggregate=zero;
        scale=0.0;
        p=kernel;
        q=source;
        for (i=0; i < (long) columns; i++)
        {
          if ((i >= (x-(long) width/2)) && (i <= (x+(long) width/2)))
            {
              aggregate.red+=(*p)*q->red;
              aggregate.green+=(*p)*q->green;
              aggregate.blue+=(*p)*q->blue;
              aggregate.opacity+=(*p)*q->opacity;
            }
          if (((i+width/2-x) >= 0) && ((i+width/2-x) < width))
            scale+=kernel[i+width/2-x];
          p++;
          q++;
        }
        scale=1.0/scale;
        destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
        destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
        destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
        destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
      }
      return;
    }
  /*
    Blur scanline.
  */
  for (x=0; x < (long) (width/2); x++)
  {
    aggregate=zero;
    scale=0.0;
    p=kernel+width/2-x;
    q=source;
    for (i=width/2-x; i < (long) width; i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      aggregate.opacity+=(*p)*q->opacity;
      scale+=(*p);
      p++;
      q++;
    }
    scale=1.0/scale;
    destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
    destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
    destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
    destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
  }
  for ( ; x < (long) (columns-width/2); x++)
  {
    aggregate=zero;
    p=kernel;
    q=source+(x-width/2);
    for (i=0; i < (long) width; i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      aggregate.opacity+=(*p)*q->opacity;
      p++;
      q++;
    }
    destination[x].red=(Quantum) (aggregate.red+0.5);
    destination[x].green=(Quantum) (aggregate.green+0.5);
    destination[x].blue=(Quantum) (aggregate.blue+0.5);
    destination[x].opacity=(Quantum) (aggregate.opacity+0.5);
  }
  for ( ; x < (long) columns; x++)
  {
    aggregate=zero;
    scale=0;
    p=kernel;
    q=source+(x-width/2);
    for (i=0; i < (long) (columns-x+width/2); i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      aggregate.opacity+=(*p)*q->opacity;
      scale+=(*p);
      p++;
      q++;
    }
    scale=1.0/scale;
    destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
    destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
    destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
    destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
  }
}

static int GetBlurKernel(unsigned long width,const double sigma,double **kernel)
{
#define KernelRank 3

  double
    alpha,
    normalize;

  int
    bias;

  register long
    i;

  /*
    Generate a 1-D convolution matrix.  Calculate the kernel at higher
    resolution than needed and average the results as a form of numerical
    integration to get the best accuracy.
  */
  if (width == 0)
    width=3;
  *kernel=(double *) AcquireMemory(width*sizeof(double));
  if (*kernel == (double *) NULL)
    return(0);
  for (i=0; i < (long) width; i++)
    (*kernel)[i]=0.0;
  bias=KernelRank*width/2;
  for (i=(-bias); i <= bias; i++)
  {
    alpha=exp(-((double) i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
    (*kernel)[(i+bias)/KernelRank]+=alpha/(MagickSQ2PI*sigma);
  }
  normalize=0;
  for (i=0; i < (long) width; i++)
    normalize+=(*kernel)[i];
  for (i=0; i < (long) width; i++)
    (*kernel)[i]/=normalize;
  return(width);
}

MagickExport Image *BlurImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
#define BlurImageText  "  Blur image...  "

  double
    *kernel;

  Image
    *blur_image;

  int
    width;

  long
    y;

  PixelPacket
    *scanline;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  /*
    Get convolution matrix for the specified standard-deviation.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  kernel=(double *) NULL;
  if (radius > 0)
    width=GetBlurKernel((int) (2*ceil(radius)+1),sigma,&kernel);
  else
    {
      double
        *last_kernel;

      last_kernel=(double *) NULL;
      width=GetBlurKernel(3,sigma,&kernel);
      while ((long) (MaxRGB*kernel[0]) > 0)
      {
        if (last_kernel != (double *)NULL)
          LiberateMemory((void **) &last_kernel);
        last_kernel=kernel;
        kernel=(double *) NULL;
        width=GetBlurKernel(width+2,sigma,&kernel);
      }
      if (last_kernel != (double *) NULL)
        {
          LiberateMemory((void **) &kernel);
          width-=2;
          kernel=last_kernel;
        }
    }
  if (width < 3)
    ThrowImageException(OptionError,"UnableToBlurImage",
      "KernelRadiusIsTooSmall");
  /*
    Allocate blur image.
  */
  blur_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (blur_image == (Image *) NULL)
    {
      LiberateMemory((void **) &kernel);
      return((Image *) NULL);
    }
  blur_image->storage_class=DirectClass;
  scanline=(PixelPacket *) AcquireMemory(image->rows*sizeof(PixelPacket));
  if (scanline == (PixelPacket *) NULL)
    {
      DestroyImage(blur_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToBlurImage")
    }
  /*
    Blur the image rows.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=SetImagePixels(blur_image,0,y,image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    BlurScanline(kernel,width,p,q,image->columns);
    if (!SyncImagePixels(blur_image))
      break;
    if (QuantumTick(y,blur_image->rows+blur_image->columns))
      {
        status=MagickMonitor(BlurImageText,y,blur_image->rows+
          blur_image->columns,exception);
        if (status == False)
          break;
      }
  }
  /*
    Blur the image columns.
  */
  for (x=0; x < (long) image->columns; x++)
  {
    q=GetImagePixels(blur_image,x,0,1,image->rows);
    if (q == (PixelPacket *) NULL)
      break;
    (void) memcpy(scanline,q,image->rows*sizeof(PixelPacket));
    BlurScanline(kernel,width,scanline,q,image->rows);
    if (!SyncImagePixels(blur_image))
      break;
    if (QuantumTick(blur_image->rows+x,blur_image->rows+blur_image->columns))
      {
        status=MagickMonitor(BlurImageText,blur_image->rows+x,blur_image->rows+
          blur_image->columns,exception);
        if (status == False)
          break;
      }
  }
  LiberateMemory((void **) &scanline);
  LiberateMemory((void **) &kernel);
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C h a n n e l T h r e s h o l d I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ChannelThresholdImage() changes the value of individual pixels based on
%  the intensity of each pixel channel.  The result is a high-contrast image.
%
%  The format of the ChannelThresholdImage method is:
%
%      unsigned int ChannelThresholdImage(Image *image,const char *threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold: define the threshold values.
%
%
*/
MagickExport unsigned int ChannelThresholdImage(Image *image,
  const char *threshold)
{
#define ThresholdImageText  "  Threshold the image...  "

  DoublePixelPacket
    pixel;

  IndexPacket
    index;

  long
    count,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (threshold == (const char *) NULL)
    return(True);
  SetImageType(image,TrueColorType);
  pixel.red=MaxRGB;
  pixel.green=MaxRGB;
  pixel.blue=MaxRGB;
  pixel.opacity=MaxRGB;
  count=sscanf(threshold,"%lf%*[/,%%]%lf%*[/,%%]%lf%*[/,%%]%lf",
    &pixel.red,&pixel.green,&pixel.blue,&pixel.opacity);
  if (count == 1)
    if (!AllocateImageColormap(image,2))
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToThresholdImage");
  if (strchr(threshold,'%') != (char *) NULL)
    {
      pixel.red*=MaxRGB/100.0;
      pixel.green*=MaxRGB/100.0;
      pixel.blue*=MaxRGB/100.0;
      pixel.opacity*=MaxRGB/100.0;
    }
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    if (count == 1)
      for (x=0; x < (long) image->columns; x++)
      {
        index=PixelIntensityToQuantum(q) <= pixel.red ? 0 : 1;
        indexes[x]=index;
        q->red=image->colormap[index].red;
        q->green=image->colormap[index].green;
        q->blue=image->colormap[index].blue;
        q++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=q->red <= pixel.red ? 0 : MaxRGB;
        q->green=q->green <= pixel.green ? 0 : MaxRGB;
        q->blue=q->blue <= pixel.blue ? 0 : MaxRGB;
        q->opacity=q->opacity <= pixel.opacity ? 0 : MaxRGB;
        q++;
      }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ThresholdImageText,y,image->rows,&image->exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D e s p e c k l e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Despeckle() reduces the speckle noise in an image while perserving the
%  edges of the original image.
%
%  The format of the DespeckleImage method is:
%
%      Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
{
#define DespeckleImageText  "  Despeckle image...  "

  Image
    *despeckle_image;

  int
    layer;

  long
    j,
    y;

  Quantum
    *buffer,
    *pixels;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    length;

  static const int
    X[4]= {0, 1, 1,-1},
    Y[4]= {1, 0, 1, 1};

  /*
    Allocate despeckled image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  despeckle_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (despeckle_image == (Image *) NULL)
    return((Image *) NULL);
  despeckle_image->storage_class=DirectClass;
  /*
    Allocate image buffers.
  */
  length=(image->columns+2)*(image->rows+2)*sizeof(Quantum);
  pixels=(Quantum *) AcquireMemory(length);
  buffer=(Quantum *) AcquireMemory(length);
  if ((buffer == (Quantum *) NULL) || (pixels == (Quantum *) NULL))
    {
      DestroyImage(despeckle_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToDespeckleImage")
    }
  /*
    Reduce speckle in the image.
  */
  for (layer=0; layer <= 3; layer++)
  {
    (void) memset(pixels,0,length);
    j=(long) image->columns+2;
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      j++;

      switch (layer)
        {
        case 0:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->red;
          break;
        case 1:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->green;
          break;
        case 2:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->blue;
          break;
        case 3:
          for (x=(long) image->columns; x > 0; x--)
            pixels[j++]=p++->opacity;
          break;
        default: break;
        }

      j++;
    }
    (void) memset(buffer,0,length);
    for (i=0; i < 4; i++)
    {
      if (!MagickMonitor(DespeckleImageText,4*layer+i,15,exception))
        break;
      Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,1);
      Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,1);
      Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,-1);
      Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,-1);
    }
    j=(long) image->columns+2;
    for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(despeckle_image,0,y,despeckle_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      j++;

      switch (layer)
        {
        case 0:
          for (x=(long) image->columns; x > 0; x--)
            q++->red=pixels[j++];
          break;
        case 1:
          for (x=(long) image->columns; x > 0; x--)
            q++->green=pixels[j++];
          break;
        case 2:
          for (x=(long) image->columns; x > 0; x--)
            q++->blue=pixels[j++];
          break;
        case 3:
          for (x=(long) image->columns; x > 0; x--)
            q++->opacity=pixels[j++];
          break;
        default: break;
        }

      if (!SyncImagePixels(despeckle_image))
        break;
      j++;
    }
  }
  /*
    Free memory.
  */
  LiberateMemory((void **) &buffer);
  LiberateMemory((void **) &pixels);
  return(despeckle_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E d g e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EdgeImage() finds edges in an image.  Radius defines the radius of the
%  convolution filter.  Use a radius of 0 and Edge() selects a suitable
%  radius for you.
%
%  The format of the EdgeImage method is:
%
%      Image *EdgeImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EdgeImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *edge_image;

  int
    width;

  register long
    i;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionError,"UnableToEdgeImage",
      "ImageIsSmallerThanRadius");
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToEdgeImage");
  for (i=0; i < (width*width); i++)
    kernel[i]=(-1.0);
  kernel[i/2]=width*width-1.0;
  edge_image=ConvolveImage(image,width,kernel,exception);
  LiberateMemory((void **) &kernel);
  return(edge_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E m b o s s I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EmbossImage() returns a grayscale image with a three-dimensional effect.
%  We convolve the image with a Gaussian operator of the given radius and
%  standard deviation (sigma).  For reasonable results, radius should be
%  larger than sigma.  Use a radius of 0 and Emboss() selects a suitable
%  radius for you.
%
%  The format of the EmbossImage method is:
%
%      Image *EmbossImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EmbossImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel;

  Image
    *emboss_image;

  int
    j,
    width;

  register long
    i,
    u,
    v;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToEmbossImage");
  i=0;
  j=width/2;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=((u < 0) || (v < 0) ? -8.0 : 8.0)*
        alpha/(2.0*MagickPI*sigma*sigma);
      if (u == j)
        kernel[i]=v == j ? 1.0 : 0.0;
      i++;
    }
    j--;
  }
  emboss_image=ConvolveImage(image,width,kernel,exception);
  if (emboss_image != (Image *) NULL)
    (void) EqualizeImage(emboss_image);
  LiberateMemory((void **) &kernel);
  return(emboss_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E n h a n c e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EnhanceImage() applies a digital filter that improves the quality of a
%  noisy image.
%
%  The format of the EnhanceImage method is:
%
%      Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
{
#define Enhance(weight) \
  mean=((double) r->red+pixel.red)/2; \
  distance=r->red-(double) pixel.red; \
  distance_squared=(2.0*((double) MaxRGB+1.0)+mean)*distance*distance/MaxRGB; \
  mean=((double) r->green+pixel.green)/2; \
  distance=r->green-(double) pixel.green; \
  distance_squared+=4.0*distance*distance; \
  mean=((double) r->blue+pixel.blue)/2; \
  distance=r->blue-(double) pixel.blue; \
  distance_squared+= \
    (3.0*((double) MaxRGB+1.0)-1.0-mean)*distance*distance/MaxRGB; \
  mean=((double) r->opacity+pixel.opacity)/2; \
  distance=r->opacity-(double) pixel.opacity; \
  distance_squared+= \
    (3.0*((double) MaxRGB+1.0)-1.0-mean)*distance*distance/MaxRGB; \
  if (distance_squared < ((double) MaxRGB*MaxRGB/25.0)) \
    { \
      aggregate.red+=(weight)*r->red; \
      aggregate.green+=(weight)*r->green; \
      aggregate.blue+=(weight)*r->blue; \
      aggregate.opacity+=(weight)*r->opacity; \
      total_weight+=(weight); \
    } \
  r++;
#define EnhanceImageText  "  Enhance image...  "

  DoublePixelPacket
    aggregate,
    zero;

  double
    distance,
    distance_squared,
    mean,
    total_weight;

  Image
    *enhance_image;

  long
    y;

  PixelPacket
    pixel;

  register const PixelPacket
    *p,
    *r;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize enhanced image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 5) || (image->rows < 5))
    return((Image *) NULL);
  enhance_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (enhance_image == (Image *) NULL)
    return((Image *) NULL);
  enhance_image->storage_class=DirectClass;
  /*
    Enhance image.
  */
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    /*
      Read another scan line.
    */
    p=AcquireImagePixels(image,0,y-2,image->columns,5,exception);
    q=SetImagePixels(enhance_image,0,y,enhance_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Transfer first 2 pixels of the scanline.
    */
    *q++=(*(p+2*image->columns));
    *q++=(*(p+2*image->columns+1));
    for (x=2; x < (long) (image->columns-2); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      aggregate=zero;
      total_weight=0.0;
      r=p+2*image->columns+2;
      pixel=(*r);
      r=p;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      r=p+image->columns;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      r=p+2*image->columns;
      Enhance(10); Enhance(40); Enhance(80); Enhance(40); Enhance(10);
      r=p+3*image->columns;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      r=p+4*image->columns;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      q->red=(Quantum) ((aggregate.red+(total_weight/2)-1)/total_weight);
      q->green=(Quantum) ((aggregate.green+(total_weight/2)-1)/total_weight);
      q->blue=(Quantum) ((aggregate.blue+(total_weight/2)-1)/total_weight);
      q->opacity=(Quantum)
        ((aggregate.opacity+(total_weight/2)-1)/total_weight);
      p++;
      q++;
    }
    p++;
    *q++=(*p++);
    *q++=(*p++);
    if (!SyncImagePixels(enhance_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(EnhanceImageText,y,image->rows-2,exception))
        break;
  }
  return(enhance_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a u s s i a n B l u r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GaussianBlurImage() blurs an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and GaussianBlurImage() selects a suitable radius for you
%
%  The format of the BlurImage method is:
%
%      Image *GaussianBlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method GaussianBlurImage returns a pointer to the image
%      after it is blur.  A null image is returned if there is a memory
%      shortage.
%
%    o radius: the radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: the standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *GaussianBlurImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel;

  Image
    *blur_image;

  int
    width;

  register long
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth2D(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionError,"UnableToBlurImage",
      "ImageIsSmallerThanRadius");
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToBlurImage");
  i=0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=alpha/(2.0*MagickPI*sigma*sigma);
      i++;
    }
  }
  blur_image=ConvolveImage(image,width,kernel,exception);
  LiberateMemory((void **) &kernel);
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M e d i a n F i l t e r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MedianFilterImage() applies a digital filter that improves the quality
%  of a noisy image.  Each pixel is replaced by the median in a set of
%  neighboring pixels as defined by radius.
%
%  The algorithm was contributed by Mike Edmonds and implements an insertion
%  sort for selecting median color-channel values.  For more on this algorithm
%  see "Skip Lists: A probabilistic Alternative to Balanced Trees" by William
%  Pugh in the June 1990 of Communications of the ACM.
%
%  The format of the MedianFilterImage method is:
%
%      Image *MedianFilterImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

typedef struct _MedianListNode
{
  unsigned long
    next[9],
    count,
    signature;
} MedianListNode;

typedef struct _MedianSkipList
{
  int
    level;

  MedianListNode
    nodes[65537];
} MedianSkipList;

typedef struct _MedianPixelList
{
  unsigned long
    center,
    seed,
    signature;

  MedianSkipList
    lists[4];
} MedianPixelList;

static void AddNodeMedianList(MedianPixelList *pixel_list,int channel,
  unsigned long color)
{
  register long
    level;

  register MedianSkipList
    *list;

  unsigned long
    search,
    update[9];

  /*
    Initialize the node.
  */
  list=pixel_list->lists+channel;
  list->nodes[color].signature=pixel_list->signature;
  list->nodes[color].count=1;
  /*
    Determine where it belongs in the list.
  */
  search=65536L;
  for (level=list->level; level >= 0; level--)
  {
    while (list->nodes[search].next[level] < color)
      search=list->nodes[search].next[level];
    update[level]=search;
  }
  /*
    Generate a pseudo-random level for this node.
  */
  for (level=0; ; level++)
  {
    pixel_list->seed=(pixel_list->seed*42893621L)+1L;
    if ((pixel_list->seed & 0x300) != 0x300)
      break;
  }
  if (level > 8)
    level=8;
  if (level > (list->level+2))
    level=list->level+2;
  /*
    If we're raising the list's level, link back to the root node.
  */
  while (level > list->level)
  {
    list->level++;
    update[list->level]=65536L;
  }
  /*
    Link the node into the skip-list.
  */
  do
  {
    list->nodes[color].next[level]=list->nodes[update[level]].next[level];
    list->nodes[update[level]].next[level]=color;
  }
  while (level-- > 0);
}

static PixelPacket GetMedianList(MedianPixelList *pixel_list)
{
  PixelPacket
    pixel;

  register long
    channel;

  register MedianSkipList
    *list;

  unsigned long
    center,
    channels[4],
    color,
    count;

  /*
    Find the median value for each of the color.
  */
  center=pixel_list->center;
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    count=0;
    do
    {
      color=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    }
    while (count <= center);
    channels[channel]=color;
  }
  pixel.red=ScaleShortToQuantum(channels[0]);
  pixel.green=ScaleShortToQuantum(channels[1]);
  pixel.blue=ScaleShortToQuantum(channels[2]);
  pixel.opacity=ScaleShortToQuantum(channels[3]);
  return(pixel);
}

static void InitializeMedianList(MedianPixelList *pixel_list,long width)
{
  pixel_list->center=width*width/2;
  pixel_list->signature=MagickSignature;
  (void) memset((void *) pixel_list->lists,0,4*sizeof(MedianSkipList));
}

static inline void InsertMedianList(MedianPixelList *pixel_list,
  const PixelPacket *pixel)
{
  unsigned long
    signature;

  unsigned int
    red_index,
    green_index,
    blue_index,
    opacity_index;

  red_index=ScaleQuantumToShort(pixel->red);
  signature=pixel_list->lists[0].nodes[red_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,0,red_index);
  else
    pixel_list->lists[0].nodes[red_index].count++;

  green_index=ScaleQuantumToShort(pixel->green);
  signature=pixel_list->lists[1].nodes[green_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,1,green_index);
  else
    pixel_list->lists[1].nodes[green_index].count++;

  blue_index=ScaleQuantumToShort(pixel->blue);
  signature=pixel_list->lists[2].nodes[blue_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,2,blue_index);
  else
    pixel_list->lists[2].nodes[blue_index].count++;

  opacity_index=ScaleQuantumToShort(pixel->opacity);
  signature=pixel_list->lists[3].nodes[opacity_index].signature;
  if (signature != pixel_list->signature)
    AddNodeMedianList(pixel_list,3,opacity_index);
  else
    pixel_list->lists[3].nodes[opacity_index].count++;
}

static void ResetMedianList(MedianPixelList *pixel_list)
{
  int
    level;

  register long
    channel;

  register MedianListNode
    *root;

  register MedianSkipList
    *list;

  /*
    Reset the skip-list.
  */
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    root=list->nodes+65536L;
    list->level=0;
    for (level=0; level < 9; level++)
      root->next[level]=65536L;
  }
  pixel_list->seed=pixel_list->signature++;
}

MagickExport Image *MedianFilterImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define MedianFilterImageText  "  Filter image with neighborhood ranking...  "

  Image
    *median_image;

  long
    width,
    x,
    y;

  MedianPixelList
    *skiplist;

  register const PixelPacket
    *p,
    *r;

  register long
    u,
    v;

  register PixelPacket
    *q;

  /*
    Initialize median image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionError,"UnableToFilterImage",
      "ImageSmallerThanKernelRadius");
  median_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (median_image == (Image *) NULL)
    return((Image *) NULL);
  median_image->storage_class=DirectClass;
  /*
    Allocate skip-lists.
  */
  skiplist=(MedianPixelList *) AcquireMemory(sizeof(MedianPixelList));
  if (skiplist == (MedianPixelList *) NULL)
    {
      DestroyImage(median_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToMedianFilterImage")
    }
  /*
    Median filter each image row.
  */
  InitializeMedianList(skiplist,width);
  for (y=0; y < (long) median_image->rows; y++)
  {
    p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
      exception);
    q=SetImagePixels(median_image,0,y,median_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) median_image->columns; x++)
    {
      r=p;
      ResetMedianList(skiplist);
      for (v=0; v < width; v++)
      {
        for (u=0; u < width; u++)
          InsertMedianList(skiplist,r+u);
        r+=image->columns+width;
      }
      *q++=GetMedianList(skiplist);
      p++;
    }
    if (!SyncImagePixels(median_image))
      break;
    if (QuantumTick(y,median_image->rows))
      if (!MagickMonitor(MedianFilterImageText,y,median_image->rows,exception))
        break;
  }
  LiberateMemory((void **) &skiplist);
  return(median_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o t i o n B l u r I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MotionBlurImage() simulates motion blur.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and MotionBlurImage() selects a suitable radius for you.
%  Angle gives the angle of the blurring motion.
%
%  Andrew Protano contributed this effect.
%
%  The format of the MotionBlurImage method is:
%
%    Image *MotionBlurImage(const Image *image,const double radius,
%      const double sigma,const double angle,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting
%     the center pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o angle: Apply the effect along this angle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static int GetMotionBlurKernel(int width,const double sigma,double **kernel)
{
#define KernelRank 3

  double
    alpha,
    normalize;

  int
    bias;

  register long
    i;

  /*
    Generate a 1-D convolution matrix.  Calculate the kernel at higher
    resolution than needed and average the results as a form of numerical
    integration to get the best accuracy.
  */
  if (width <= 0)
    width=3;
  *kernel=(double *) AcquireMemory(width*sizeof(double));
  if (*kernel == (double *) NULL)
    return(0);
  for (i=0; i < width; i++)
    (*kernel)[i]=0.0;
  bias=KernelRank*width;
  for (i=0; i < bias; i++)
  {
    alpha=exp(-((double) i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
    (*kernel)[i/KernelRank]+=alpha/(MagickSQ2PI*sigma);
  }
  normalize=0;
  for (i=0; i < width; i++)
    normalize+=(*kernel)[i];
  for (i=0; i < width; i++)
    (*kernel)[i]/=normalize;
  return(width);
}

MagickExport Image *MotionBlurImage(const Image *image,const double radius,
  const double sigma,const double angle,ExceptionInfo *exception)
{
  double
    *kernel;

  DoublePixelPacket
    aggregate,
    zero;

  Image
    *blur_image;

  int
    width;

  long
    y;

  PixelPacket
    pixel;

  PointInfo
    *offsets;

  register long
    i,
    x,
    u,
    v;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  kernel=(double *) NULL;
  if (radius > 0)
    width=GetMotionBlurKernel((int) (2.0*ceil(radius)+1.0),sigma,&kernel);
  else
    {
      double
        *last_kernel;

      last_kernel=(double *) NULL;
      width=GetMotionBlurKernel(3,sigma,&kernel);
      while ((MaxRGB*kernel[width-1]) > 0.0)
      {
        if (last_kernel != (double *)NULL)
          LiberateMemory((void **) &last_kernel);
        last_kernel=kernel;
        kernel=(double *) NULL;
        width=GetMotionBlurKernel(width+2,sigma,&kernel);
      }
      if (last_kernel != (double *) NULL)
        {
          LiberateMemory((void **) &kernel);
          width-=2;
          kernel=last_kernel;
        }
    }
  if (width < 3)
    ThrowImageException(OptionError,"UnableToBlurimage",
      "KernelRadiusIsTooSmall");
  offsets=(PointInfo *) AcquireMemory(width*sizeof(PointInfo));
  if (offsets == (PointInfo *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToMotionBlurImage");
  /*
    Allocate blur image.
  */
  blur_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (blur_image == (Image *) NULL)
    {
      LiberateMemory((void **) &kernel);
      LiberateMemory((void **) &offsets);
      return((Image *) NULL);
    }
  blur_image->storage_class=DirectClass;
  x=(long) (width*sin(DegreesToRadians(angle)));
  y=(long) (width*cos(DegreesToRadians(angle)));
  for (i=0; i < width; i++)
  {
    offsets[i].x=i*x/sqrt(x*x+y*y);
    offsets[i].y=i*y/sqrt(x*x+y*y);
  }
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(blur_image,0,y,blur_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      aggregate=zero;
      for (i=0; i < width; i++)
      {
        u=x+(long) offsets[i].x;
        v=y+(long) offsets[i].y;
        if ((u < 0) || (u >= (long) image->columns) ||
            (v < 0) || (v >= (long) image->rows))
          continue;
        pixel=AcquireOnePixel(image,u,v,exception);
        aggregate.red+=kernel[i]*pixel.red;
        aggregate.green+=kernel[i]*pixel.green;
        aggregate.blue+=kernel[i]*pixel.blue;
        aggregate.opacity+=kernel[i]*pixel.opacity;
      }
      q->red=(Quantum) aggregate.red;
      q->green=(Quantum) aggregate.green;
      q->blue=(Quantum) aggregate.blue;
      q->opacity=(Quantum) aggregate.opacity;
      q++;
    }
    if (!SyncImagePixels(blur_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(BlurImageText,y,image->rows,exception))
        break;
  }
  LiberateMemory((void **) &kernel);
  LiberateMemory((void **) &offsets);
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R e d u c e N o i s e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReduceNoiseImage() smooths the contours of an image while still preserving
%  edge information.  The algorithm works by replacing each pixel with its
%  neighbor closest in value.  A neighbor is defined by radius.  Use a radius
%  of 0 and ReduceNoise() selects a suitable radius for you.
%
%  The format of the ReduceNoiseImage method is:
%
%      Image *ReduceNoiseImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static PixelPacket GetNonpeakMedianList(MedianPixelList *pixel_list)
{
  PixelPacket
    pixel;

  register MedianSkipList
    *list;

  register long
    channel;

  unsigned long
    channels[4],
    center,
    color,
    count,
    previous,
    next;

  /*
    Finds the median value for each of the color.
  */
  center=pixel_list->center;
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    next=list->nodes[color].next[0];
    count=0;
    do
    {
      previous=color;
      color=next;
      next=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    }
    while (count <= center);
    if ((previous == 65536L) && (next != 65536L))
      color=next;
    else
      if ((previous != 65536L) && (next == 65536L))
        color=previous;
    channels[channel]=color;
  }
  pixel.red=ScaleShortToQuantum(channels[0]);
  pixel.green=ScaleShortToQuantum(channels[1]);
  pixel.blue=ScaleShortToQuantum(channels[2]);
  pixel.opacity=ScaleShortToQuantum(channels[3]);
  return(pixel);
}

MagickExport Image *ReduceNoiseImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define ReduceNoiseImageText  "  Reduce the image noise...  "

  Image
    *noise_image;

  long
    width,
    x,
    y;

  MedianPixelList
    *skiplist;

  register const PixelPacket
    *p,
    *r;

  register long
    u,
    v;

  register PixelPacket
    *q;

  /*
    Initialize noised image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionError,"UnableToFilterImage",
      "ImageSmallerThanKernelRadius");
  noise_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  noise_image->storage_class=DirectClass;
  /*
    Allocate skip-lists.
  */
  skiplist=(MedianPixelList *) AcquireMemory(sizeof(MedianPixelList));
  if (skiplist == (MedianPixelList *) NULL)
    {
      DestroyImage(noise_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToNoiseFilterImage")
    }
  /*
    Median filter each image row.
  */
  InitializeMedianList(skiplist,width);
  for (y=0; y < (long) noise_image->rows; y++)
  {
    p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
      exception);
    q=SetImagePixels(noise_image,0,y,noise_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) noise_image->columns; x++)
    {
      r=p;
      ResetMedianList(skiplist);
      for (v=width; v > 0; v--)
      {
        register const PixelPacket
          *ru=r;
        for (u=width; u > 0; u--)
          InsertMedianList(skiplist,ru++);
        r+=image->columns+width;
      }
      *q++=GetNonpeakMedianList(skiplist);
      p++;
    }
    if (!SyncImagePixels(noise_image))
      break;
    if (QuantumTick(y,noise_image->rows))
      if (!MagickMonitor(ReduceNoiseImageText,y,noise_image->rows,exception))
        break;
  }
  LiberateMemory((void **) &skiplist);
  return(noise_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ShadeImage() shines a distant light on an image to create a
%  three-dimensional effect. You control the positioning of the light with
%  azimuth and elevation; azimuth is measured in degrees off the x axis
%  and elevation is measured in pixels above the Z axis.
%
%  The format of the ShadeImage method is:
%
%      Image *ShadeImage(const Image *image,const unsigned int gray,
%        double azimuth,double elevation,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o gray: A value other than zero shades the intensity of each pixel.
%
%    o azimuth, elevation:  Define the light source direction.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ShadeImage(const Image *image,const unsigned int gray,
  double azimuth,double elevation,ExceptionInfo *exception)
{
#define ShadeImageText  "  Shade image...  "

  double
    distance,
    normal_distance,
    shade;

  Image
    *shade_image;

  long
    y;

  PrimaryInfo
    light,
    normal;

  register const PixelPacket
    *p,
    *s0,
    *s1,
    *s2;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize shaded image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  shade_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (shade_image == (Image *) NULL)
    return((Image *) NULL);
  shade_image->storage_class=DirectClass;
  /*
    Compute the light vector.
  */
  azimuth=DegreesToRadians(azimuth);
  elevation=DegreesToRadians(elevation);
  light.x=(double) MaxRGB*cos(azimuth)*cos(elevation);
  light.y=(double) MaxRGB*sin(azimuth)*cos(elevation);
  light.z=(double) MaxRGB*sin(elevation);
  normal.z=2.0*MaxRGB;  /* constant Z of surface normal */
  /*
    Shade image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,-1,y-1,image->columns+2,3,exception);
    q=SetImagePixels(shade_image,0,y,shade_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Shade this row of pixels.
    */
    s0=p+1;
    s1=s0+image->columns+2;
    s2=s1+image->columns+2;
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Determine the surface normal and compute shading.
      */
      normal.x=(double) PixelIntensity(s0-1)+(double) PixelIntensity(s1-1)+
        (double) PixelIntensity(s2-1)-(double) PixelIntensity(s0+1)-
        (double) PixelIntensity(s1+1)-(double) PixelIntensity(s2+1);
      normal.y=(double) PixelIntensity(s2-1)+(double) PixelIntensity(s2)+
        (double) PixelIntensity(s2+1)-(double) PixelIntensity(s0-1)-
        (double) PixelIntensity(s0)-(double) PixelIntensity(s0+1);
      if ((normal.x == 0.0) && (normal.y == 0.0))
        shade=light.z;
      else
        {
          shade=0.0;
          distance=normal.x*light.x+normal.y*light.y+normal.z*light.z;
          if (distance > MagickEpsilon)
            {
              normal_distance=
                normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
              if (normal_distance > (MagickEpsilon*MagickEpsilon))
                shade=distance/sqrt(normal_distance);
            }
        }
      if (gray)
        {
          q->red=(Quantum) shade;
          q->green=(Quantum) shade;
          q->blue=(Quantum) shade;
        }
      else
        {
          q->red=(Quantum) ((shade*s1->red)/MaxRGB+0.5);
          q->green=(Quantum) ((shade*s1->green)/MaxRGB+0.5);
          q->blue=(Quantum) ((shade*s1->blue)/MaxRGB+0.5);
        }
      q->opacity=s1->opacity;
      s0++;
      s1++;
      s2++;
      q++;
    }
    if (!SyncImagePixels(shade_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ShadeImageText,y,image->rows,exception))
        break;
  }
  return(shade_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a r p e n I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SharpenImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and SharpenImage() selects a suitable radius for you.
%
%  The format of the SharpenImage method is:
%
%    Image *SharpenImage(const Image *image,const double radius,
%      const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Laplacian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SharpenImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel,
    normalize;

  Image
    *sharp_image;

  long
    width;

  register long
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionError,"UnableToSharpenImage",
      "ImageIsSmallerThanRadius");
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToSharpenImage");
  i=0;
  normalize=0.0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=alpha/(2.0*MagickPI*sigma*sigma);
      normalize+=kernel[i];
      i++;
    }
  }
  kernel[i/2]=(-2.0)*normalize;
  sharp_image=ConvolveImage(image,width,kernel,exception);
  LiberateMemory((void **) &kernel);
  return(sharp_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S p r e a d I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpreadImage() is a special effects method that randomly displaces each
%  pixel in a block defined by the radius parameter.
%
%  The format of the SpreadImage method is:
%
%      Image *SpreadImage(const Image *image,const unsigned int radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius:  Choose a random pixel in a neighborhood of this extent.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SpreadImage(const Image *image,const unsigned int radius,
  ExceptionInfo *exception)
{
#define SpreadImageText  "  Spread image...  "
#define OFFSETS_ENTRIES 5000

  Image
    *spread_image;

  int
    offsets_index;

  long
    quantum,
    x_distance,
    y,
    y_distance;

  register long
    x;

  long
    *offsets;

  register PixelPacket
    *q;

  const PixelPacket
    *neighbors;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 3) || (image->rows < 3))
    return((Image *) NULL);
  /*
    Initialize spread image attributes.
  */
  spread_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (spread_image == (Image *) NULL)
    return((Image *) NULL);
  spread_image->storage_class=DirectClass;
  offsets_index=0;

  /*
    Initialize random offsets cache
  */
  offsets=(long *)AcquireMemory(OFFSETS_ENTRIES*sizeof(long));
  if (offsets == (long *) NULL)
    {
      ThrowException(exception,ResourceLimitError,
                  "MemoryAllocationFailed",NULL);
      return (Image *) NULL;
    }
  for (x=0; x < (OFFSETS_ENTRIES-1); x++)
    offsets[x]=((((2*(double) radius+1)*rand())/RAND_MAX)-((long)radius));

  /*
    Convolve each row.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    long
      y_min,
      y_max;

    q=SetImagePixels(spread_image,0,y,spread_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;

    if (radius > y)
      y_min=0;
    else
      y_min=y-radius;
    
    if ((y+radius) >= image->rows)
      y_max=image->rows-1;
    else
      y_max=y+radius;

    neighbors=AcquireImagePixels(image,0,y_min,image->columns,y_max-y_min,exception);
    if (neighbors == (PixelPacket *) NULL)
      break;

    for (x=0; x < (long) image->columns; x++)
    {
      do
      {
        x_distance=offsets[offsets_index++];
        if (offsets_index==OFFSETS_ENTRIES)
          offsets_index=0;
      } while (((x+x_distance) < 0) ||
               ((x+x_distance) >= (long) image->columns));
      do
      {
        y_distance=offsets[offsets_index++];
        if (offsets_index==OFFSETS_ENTRIES)
          offsets_index=0;
      } while (((y+y_distance) < 0) ||
               ((y+y_distance) >= (long) image->rows));
      *q=*(neighbors+(x+x_distance)+((y+y_distance-y_min)*image->columns));
      q++;
    }
    if (!SyncImagePixels(spread_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(SpreadImageText,y,image->rows,exception))
        break;
  }
  LiberateMemory((void **)&offsets);
  return(spread_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T h r e s h o l d I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ThresholdImage() changes the value of individual pixels based on
%  the intensity of each pixel compared to threshold.  The result is a
%  high-contrast, two color image.
%
%  The format of the ThresholdImage method is:
%
%      unsigned int ThresholdImage(Image *image,const double threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold: Define the threshold value
%
%
*/
MagickExport unsigned int ThresholdImage(Image *image,const double threshold)
{
#define ThresholdImageText  "  Threshold the image...  "

  register IndexPacket
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!AllocateImageColormap(image,2))
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      "UnableToThresholdImage");
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=(long) image->columns; x > 0; x--)
    {
      index=PixelIntensityToQuantum(q) <= threshold ? 0 : 1;
      *indexes++=index;
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ThresholdImageText,y,image->rows,&image->exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     U n s h a r p M a s k I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnsharpMaskImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a radius
%  of 0 and UnsharpMaskImage() selects a suitable radius for you.
%
%  The format of the UnsharpMaskImage method is:
%
%    Image *UnsharpMaskImage(const Image *image,const double radius,
%      const double sigma,const double amount,const double threshold,
%      ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
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
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *UnsharpMaskImage(const Image *image,const double radius,
  const double sigma,const double amount,const double threshold,
  ExceptionInfo *exception)
{
#define SharpenImageText  "  Sharpen image...  "

  DoublePixelPacket
    pixel;

  Image
    *sharp_image;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  sharp_image=GaussianBlurImage(image,radius,sigma,exception);
  if (sharp_image == (Image *) NULL)
    return((Image *) NULL);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=GetImagePixels(sharp_image,0,y,sharp_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      pixel.red=p->red-(double) q->red;
      if (AbsoluteValue(2.0*pixel.red) < (MaxRGB*threshold))
        pixel.red=p->red;
      else
        pixel.red=p->red+(pixel.red*amount);
      pixel.green=p->green-(double) q->green;
      if (AbsoluteValue(2.0*pixel.green) < (MaxRGB*threshold))
        pixel.green=p->green;
      else
        pixel.green=p->green+(pixel.green*amount);
      pixel.blue=p->blue-(double) q->blue;
      if (AbsoluteValue(2.0*pixel.blue) < (MaxRGB*threshold))
        pixel.blue=p->blue;
      else
        pixel.blue=p->blue+(pixel.blue*amount);
      pixel.opacity=p->opacity-(double) q->opacity;
      if (AbsoluteValue(2.0*pixel.opacity) < (MaxRGB*threshold))
        pixel.opacity=p->opacity;
      else
        pixel.opacity=p->opacity+(pixel.opacity*amount);
      q->red=(Quantum) ((pixel.red < 0) ? 0 :
        (pixel.red > MaxRGB) ? MaxRGB : pixel.red+0.5);
      q->green=(Quantum) ((pixel.green < 0) ? 0 :
        (pixel.green > MaxRGB) ? MaxRGB : pixel.green+0.5);
      q->blue=(Quantum) ((pixel.blue < 0) ? 0 :
        (pixel.blue > MaxRGB) ? MaxRGB : pixel.blue+0.5);
      q->opacity=(Quantum) ((pixel.opacity < 0) ? 0 :
        (pixel.opacity > MaxRGB) ? MaxRGB : pixel.opacity+0.5);
      p++;
      q++;
    }
    if (!SyncImagePixels(sharp_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(SharpenImageText,y,image->rows,exception))
        break;
  }
  return(sharp_image);
}
