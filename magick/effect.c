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
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Typedef declarations.
*/
typedef struct _AggregatePacket
{
  double
    red,
    green,
    blue,
    opacity;
} AggregatePacket;

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
%  Method AddNoiseImage creates a new image that is a copy of an existing
%  one with noise added.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the AddNoiseImage method is:
%
%      Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o noise_image: Method AddNoiseImage returns a pointer to the image after
%      the noise is minified.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o noise_type:  The type of noise: Gaussian, multiplicative Gaussian,
%      impulse, laplacian, or Poisson.
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
  noise_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(noise_image,TrueColorType);
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
      MagickMonitor(AddNoiseImageText,y,image->rows);
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
%  Method BlurImage creates a blurred copy of the input image.  We
%  convolve the image with a Gaussian operator of the given radius and
%  standard deviation (sigma).
%
%  Each output pixel is set to a value that is the weighted average of the
%  input pixels in an area enclosing the pixel.  The width parameter
%  determines how large the area is.  Each pixel in the area is weighted
%  in the average according to its distance from the center, and the
%  standard deviation, sigma. The actual weight is calculated according to
%  the Gaussian distribution (also called normal distribution), which
%  looks like a Bell Curve centered on a pixel.  The standard deviation
%  controls how 'pointy' the curve is.  The pixels near the center of the
%  curve (closer to the center of the area we are averaging) contribute
%  more than the distant pixels.
%
%  In general, the width should be wide enough to include most of the
%  total weight under the Gaussian for the standard deviation you choose.
%  As a guideline about 90% of the weight lies within two standard
%  deviations, and 98% of the weight within 3 standard deviations. Since
%  the width parameter to the function specifies the radius of the
%  Gaussian convolution mask in pixels, not counting the centre pixel, the
%  width parameter should be chosen larger than the standard deviation,
%  perhaps about twice as large to three times as large. A width of 1 will
%  give a (standard) 3x3 convolution mask, a width of 2 gives a 5 by 5
%  convolution mask. Using non-integral widths will result in some pixels
%  being considered 'partial' pixels, in which case their weight will be
%  reduced proportionally.
%
%  Pixels for which the convolution mask does not completely fit on the
%  image (e.g. pixels without a full set of neighbours) are averaged with
%  those neighbours they do have. Thus pixels at the edge of images are
%  typically less blur.
%
%  Since a 2d Gaussian is seperable, we perform the Gaussian blur by
%  convolving with two 1d Gaussians, first in the x, then in the y
%  direction. For an n by n image and Gaussian width w this requires 2wn^2
%  multiplications, while convolving with a 2d Gaussian requres w^2n^2
%  mults.
%
%  We blur the image into a copy, and the original is left untouched.
%  We must process the image in two passes, in each pass we change the
%  pixel based on its neighbors, but we need the pixel's original value
%  for the next pixel's calculation. For the first pass we could use the
%  original image but that's no good for the second pass, and it would
%  imply that the original image have to stay around in ram. Instead we
%  use a small (size=width) buffer to store the pixels we have
%  overwritten.
%
%  This method was contributed by runger@cs.mcgill.ca.
%
%  The format of the BlurImage method is:
%
%      Image *BlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method BlurImage returns a pointer to the image
%      after it is blur.  A null image is returned if there is a memory
%      shortage.
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

static void BlurScanline(const double *kernel,const int width,
  const PixelPacket *source,PixelPacket *destination,
  const unsigned long columns)
{
  AggregatePacket
    aggregate;

  double
    scale;

  register const double
    *p;

  register const PixelPacket
    *q;

  register long
    i,
    x;

  if ((unsigned long) width > columns)
    {
      for (x=0; x < (long) columns; x++)
      {
        (void) memset(&aggregate,0,sizeof(AggregatePacket));
        scale=0.0;
        p=kernel;
        q=source;
        for (i=0; i < (long) columns; i++)
        {
          if ((i >= (x-width/2)) && (i <= (x+width/2)))
            {
              aggregate.red+=(*p)*q->red;
              aggregate.green+=(*p)*q->green;
              aggregate.blue+=(*p)*q->blue;
              aggregate.opacity+=(*p)*q->opacity;
            }
          if (((i+width/2-x) >= 0) && ((i+width/2-x) < (long) width))
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
    (void) memset(&aggregate,0,sizeof(AggregatePacket));
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
    (void) memset(&aggregate,0,sizeof(AggregatePacket));
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
    (void) memset(&aggregate,0,sizeof(AggregatePacket));
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

static int GetBlurKernel(int width,const double sigma,double **kernel)
{
#define KernelRank 3

  double
    normalize;

  int
    bias;

  register int
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
  bias=KernelRank*width/2;
  for (i=(-bias); i <= bias; i++)
    (*kernel)[(i+bias)/KernelRank]+=
      exp((double) (-i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
  normalize=0;
  for (i=0; i < width; i++)
    normalize+=(*kernel)[i];
  for (i=0; i < width; i++)
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
      while ((int) (MaxRGB*kernel[0]) > 0)
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
    ThrowImageException(OptionWarning,"Unable to blur image",
      "kernel radius is too small");
  /*
    Allocate blur image.
  */
  blur_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (blur_image == (Image *) NULL)
    {
      LiberateMemory((void **) &kernel);
      return((Image *) NULL);
    }
  SetImageType(blur_image,TrueColorType);
  scanline=(PixelPacket *) AcquireMemory(image->rows*sizeof(PixelPacket));
  if (scanline == (PixelPacket *) NULL)
    {
      DestroyImage(blur_image);
      ThrowImageException(ResourceLimitWarning,"Unable to blur image",
        "Memory allocation failed")
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
      MagickMonitor(BlurImageText,y,blur_image->rows+blur_image->columns);
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
      MagickMonitor(BlurImageText,blur_image->rows+x,
        blur_image->rows+blur_image->columns);
  }
  LiberateMemory((void **) &scanline);
  LiberateMemory((void **) &kernel);
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C h a r c o a l I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CharcoalImage creates a new image that is a copy of an existing
%  one with the edge highlighted.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the CharcoalImage method is:
%
%      Image *CharcoalImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o charcoal_image: Method CharcoalImage returns a pointer to the image
%      after it is embossed.  A null image is returned if there is a memory
%      shortage.
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
MagickExport Image *CharcoalImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  Image
    *blur_image,
    *charcoal_image,
    *edge_image;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  charcoal_image=CloneImage(image,0,0,False,exception);
  if (charcoal_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(charcoal_image,GrayscaleType);
  edge_image=EdgeImage(charcoal_image,radius,exception);
  if (edge_image == (Image *) NULL)
    return((Image *) NULL);
  DestroyImage(charcoal_image);
  blur_image=BlurImage(edge_image,radius,sigma,exception);
  if (blur_image == (Image *) NULL)
    return((Image *) NULL);
  DestroyImage(edge_image);
  (void) NormalizeImage(blur_image);
  (void) NegateImage(blur_image,False);
  SetImageType(blur_image,GrayscaleType);
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o l o r i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ColorizeImage creates a new image that is a copy of an existing
%  one with the image pixels colorized.  The colorization is controlled
%  with the pen color and the opacity levels.
%
%  The format of the ColorizeImage method is:
%
%      Image *ColorizeImage(const Image *image,const char *opacity,
%        const PixelPacket target,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: the image.
%
%    o opacity:  A character string indicating the level of opacity as a
%      percentage (0-100).
%
%    o target: A color value.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ColorizeImage(const Image *image,const char *opacity,
  const PixelPacket target,ExceptionInfo *exception)
{
#define ColorizeImageText  "  Colorize the image...  "

  double
    blue,
    green,
    red;

  Image
    *colorize_image;

  long
    count,
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Allocate colorized image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  colorize_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (colorize_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(colorize_image,TrueColorType);
  /*
    Determine RGB values of the pen color.
  */
  red=100.0;
  green=100.0;
  blue=100.0;
  count=sscanf(opacity,"%lf/%lf/%lf",&red,&green,&blue);
  if (count == 1)
    {
      if (red == 0.0)
        return(colorize_image);
      green=red;
      blue=red;
    }
  /*
    Colorize DirectClass image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=SetImagePixels(colorize_image,0,y,colorize_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=(Quantum) ((p->red*(100.0-red)+target.red*red)/100.0);
      q->green=(Quantum) ((p->green*(100.0-green)+target.green*green)/100.0);
      q->blue=(Quantum) ((p->blue*(100.0-blue)+target.blue*blue)/100.0);
      q->opacity=p->opacity;
      p++;
      q++;
    }
    if (!SyncImagePixels(colorize_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(ColorizeImageText,y,image->rows);
  }
  return(colorize_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n v o l v e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvolveImage applies a general image convolution kernel to an
%  image returns the results.  ConvolveImage allocates the memory necessary for
%  the new Image structure and returns a pointer to the new image.
%
%  The format of the ConvolveImage method is:
%
%      Image *ConvolveImage(const Image *image,const unsigned int order,
%        const double *kernel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o convolve_image: Method ConvolveImage returns a pointer to the image
%      after it is convolved.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o order:  The number of columns and rows in the filter kernel.
%
%    o kernel:  An array of double representing the convolution kernel.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ConvolveImage(const Image *image,const unsigned int order,
  const double *kernel,ExceptionInfo *exception)
{
#define ConvolveImageText  "  Convolving image...  "
#define Cx(x) (x) < 0 ? (x)+(long) image->columns : \
  (x) >= (long) image->columns ? (x)-image->columns : x
#define Cy(y) (y) < 0 ? (y)+(long) image->rows : \
  (y) >= (long) image->rows ? (y)-image->rows : y

  AggregatePacket
    aggregate;

  double
    normalize;

  Image
    *convolve_image;

  long
    width,
    y;

  PixelPacket
    pixel;

  register const double
    *k;

  register const PixelPacket
    *p,
    *r;

  register long
    i,
    u,
    v,
    x;

  register PixelPacket
    *q;

  /*
    Initialize convolved image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=order;
  if ((width % 2) == 0)
    ThrowImageException(OptionWarning,"Unable to convolve image",
      "kernel width must be an odd number");
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionWarning,"Unable to convolve image",
      "image smaller than kernel width");
  convolve_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (convolve_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(convolve_image,TrueColorType);
  /*
    Convolve image.
  */
  normalize=0.0;
  for (i=0; i < (width*width); i++)
    normalize+=kernel[i];
  for (y=0; y < (long) convolve_image->rows; y++)
  {
    p=(const PixelPacket *) NULL;
    q=SetImagePixels(convolve_image,0,y,convolve_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) convolve_image->columns; x++)
    {
      (void) memset(&aggregate,0,sizeof(AggregatePacket));
      k=kernel;
      if ((x < (width/2)) || (x >= (long) (image->columns-width/2)) ||
          (y < (width/2)) || (y >= (long) (image->rows-width/2)))
        {
          for (v=(-width/2); v <= (width/2); v++)
          {
            for (u=(-width/2); u <= (width/2); u++)
            {
              pixel=AcquireOnePixel(image,Cx(x+u),Cy(y+v),exception);
              aggregate.red+=(*k)*pixel.red;
              aggregate.green+=(*k)*pixel.green;
              aggregate.blue+=(*k)*pixel.blue;
              aggregate.opacity+=(*k)*pixel.opacity;
              k++;
            }
          }
        }
      else
        {
          if (p == (const PixelPacket *) NULL)
            {
              p=AcquireImagePixels(image,0,y-width/2,image->columns,width,
                exception);
              if (p == (const PixelPacket *) NULL)
                break;
            }
          r=p+x;
          for (v=(-width/2); v <= (width/2); v++)
          {
            for (u=(-width/2); u <= (width/2); u++)
            {
              aggregate.red+=(*k)*r[u].red;
              aggregate.green+=(*k)*r[u].green;
              aggregate.blue+=(*k)*r[u].blue;
              aggregate.opacity+=(*k)*r[u].opacity;
              k++;
            }
            r+=image->columns;
          }
        }
      if ((normalize != 0.0) && (normalize != 1.0))
        {
          aggregate.red/=normalize;
          aggregate.green/=normalize;
          aggregate.blue/=normalize;
          aggregate.opacity/=normalize;
        }
      q->red=(Quantum) ((aggregate.red < 0) ? 0 :
        (aggregate.red > MaxRGB) ? MaxRGB : aggregate.red+0.5);
      q->green=(Quantum) ((aggregate.green < 0) ? 0 :
        (aggregate.green > MaxRGB) ? MaxRGB : aggregate.green+0.5);
      q->blue=(Quantum) ((aggregate.blue < 0) ? 0 :
        (aggregate.blue > MaxRGB) ? MaxRGB : aggregate.blue+0.5);
      q->opacity=(Quantum) ((aggregate.opacity < 0) ? 0 :
        (aggregate.opacity > MaxRGB) ? MaxRGB : aggregate.opacity+0.5);
      q++;
    }
    if (!SyncImagePixels(convolve_image))
      break;
    if (QuantumTick(y,convolve_image->rows))
      MagickMonitor(ConvolveImageText,y,convolve_image->rows);
  }
  return(convolve_image);
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
%  Method DespeckleImage creates a new image that is a copy of an existing
%  one with the speckle noise minified.  It uses the eight hull algorithm
%  described in Applied Optics, Vol. 24, No. 10, 15 May 1985, "Geometric filter
%  for Speckle Reduction", by Thomas R Crimmins.  Each pixel in the image is
%  replaced by one of its eight of its surrounding pixels using a polarity and
%  negative hull function.  DespeckleImage allocates the memory necessary for
%  the new Image structure and returns a pointer to the new image.
%
%  The format of the DespeckleImage method is:
%
%      Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o despeckle_image: Method DespeckleImage returns a pointer to the image
%      after it is despeckled.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
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
  despeckle_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (despeckle_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(despeckle_image,TrueColorType);
  /*
    Allocate image buffers.
  */
  length=(image->columns+2)*(image->rows+2)*sizeof(Quantum);
  pixels=(Quantum *) AcquireMemory(length);
  buffer=(Quantum *) AcquireMemory(length);
  if ((buffer == (Quantum *) NULL) || (pixels == (Quantum *) NULL))
    {
      DestroyImage(despeckle_image);
      ThrowImageException(ResourceLimitWarning,"Unable to despeckle image",
        "Memory allocation failed")
    }
  /*
    Reduce speckle in the image.
  */
  for (layer=0; layer <= 3; layer++)
  {
    (void) memset(buffer,0,length);
    (void) memset(pixels,0,length);
    j=image->columns+2;
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      j++;
      for (x=0; x < (long) image->columns; x++)
      {
        switch (layer)
        {
          case 0: pixels[j]=p->red; break;
          case 1: pixels[j]=p->green; break;
          case 2: pixels[j]=p->blue; break;
          case 3: pixels[j]=p->opacity; break;
          default: break;
        }
        p++;
        j++;
      }
      j++;
    }
    for (i=0; i < 4; i++)
    {
      MagickMonitor(DespeckleImageText,4*layer+i,15);
      Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,1);
      Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,1);
      Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,-1);
      Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,-1);
    }
    j=image->columns+2;
    for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(despeckle_image,0,y,despeckle_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      j++;
      for (x=0; x < (long) image->columns; x++)
      {
        switch (layer)
        {
          case 0: q->red=pixels[j]; break;
          case 1: q->green=pixels[j]; break;
          case 2: q->blue=pixels[j]; break;
          case 3: q->opacity=pixels[j]; break;
          default: break;
        }
        q++;
        j++;
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
%  Method EdgeImage creates a new image that is a copy of an existing
%  one with the edges enhanced.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the EdgeImage method is:
%
%      Image *EdgeImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o edge_image: Method EdgeImage returns a pointer to the image
%      after it is edge.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the address of a structure of type Image;  returned from
%      ReadImage.
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

  register int
    i;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionWarning,"Unable to edge image",
      "image is smaller than radius");
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to edge image",
      "Memory allocation failed");
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
%  Method EmbossImage creates a new image that is a copy of an existing
%  one with the edge highlighted.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the EmbossImage method is:
%
%      Image *EmbossImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o emboss_image: Method EmbossImage returns a pointer to the image
%      after it is embossed.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
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
    *kernel;

  Image
    *emboss_image;

  int
    j,
    width;

  register int
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
    ThrowImageException(ResourceLimitWarning,"Unable to emboss image",
      "Memory allocation failed");
  i=0;
  j=width/2;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      kernel[i]=(u < 0 || v < 0 ? -8.0 : 8.0)*
        exp(-(double) (u*u+v*v)/(sigma*sigma));
      if (u == j)
        kernel[i]=0.0;
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
%  Method EnhanceImage creates a new image that is a copy of an existing
%  one with the noise minified.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  EnhanceImage does a weighted average of pixels in a 5x5 cell around each
%  target pixel.  Only pixels in the 5x5 cell that are within a RGB distance
%  threshold of the target pixel are averaged.
%
%  Weights assume that the importance of neighboring pixels is negately
%  proportional to the square of their distance from the target pixel.
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the EnhanceImage method is:
%
%      Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o enhance_image: Method EnhanceImage returns a pointer to the image
%      after it is enhanced.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
{
#define Enhance(weight) \
  mean=(int) (r->red+pixel.red)/2; \
  distance=r->red-(int) pixel.red; \
  distance_squared= \
    (double) (2.0*(MaxRGB+1)+mean)*distance*distance/MaxRGB; \
  mean=(int) (r->green+pixel.green)/2; \
  distance=r->green-(int) pixel.green; \
  distance_squared+=4.0*distance*distance; \
  mean=(int) (r->blue+pixel.blue)/2; \
  distance=r->blue-(int) pixel.blue; \
  distance_squared+= \
    (double) (3.0*(MaxRGB+1)-1.0-mean)*distance*distance/MaxRGB; \
  mean=(int) (r->opacity+pixel.opacity)/2; \
  distance=r->opacity-(int) pixel.opacity; \
  distance_squared+= \
    (double) (3.0*(MaxRGB+1)-1.0-mean)*distance*distance/MaxRGB; \
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

  AggregatePacket
    aggregate;

  double
    distance_squared,
    mean,
    total_weight;

  Image
    *enhance_image;

  long
    distance,
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
  enhance_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (enhance_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(enhance_image,TrueColorType);
  /*
    Enhance image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    /*
      Read another scan line.
    */
    p=AcquireImagePixels(image,0,Min(Max(y-2,0),(long) image->rows-5),
      image->columns,5,exception);
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
      (void) memset(&aggregate,0,sizeof(AggregatePacket));
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
      MagickMonitor(EnhanceImageText,y,image->rows-2);
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
%  Method GaussianBlurImage creates a new image that is a copy of an existing
%  one with the pixels blur.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
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
    *kernel;

  Image
    *blur_image;

  int
    width;

  register int
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth2D(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionWarning,"Unable to Gaussian blur image",
      "image is smaller than radius");
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to Gaussian blur image",
      "Memory allocation failed");
  i=0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      kernel[i]=exp(-(double) (u*u+v*v)/(sigma*sigma));
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
%     I m p l o d e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImplodeImage creates a new image that is a copy of an existing
%  one with the image pixels "implode" by the specified percentage.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ImplodeImage method is:
%
%      Image *ImplodeImage(const Image *image,const double amount,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o implode_image: Method ImplodeImage returns a pointer to the image
%      after it is implode.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o amount:  A double value that defines the extent of the implosion.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ImplodeImage(const Image *image,const double amount,
  ExceptionInfo *exception)
{
#define ImplodeImageText  "  Implode image...  "

  double
    distance,
    radius,
    x_center,
    x_distance,
    x_scale,
    y_center,
    y_distance,
    y_scale;

  Image
    *implode_image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize implode image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  implode_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (implode_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(implode_image,TrueColorType);
  /*
    Compute scaling factor.
  */
  x_scale=1.0;
  y_scale=1.0;
  x_center=0.5*image->columns;
  y_center=0.5*image->rows;
  radius=x_center;
  if (image->columns > image->rows)
    y_scale=(double) image->columns/image->rows;
  else
    if (image->columns < image->rows)
      {
        x_scale=(double) image->rows/image->columns;
        radius=y_center;
      }
  /*
    Implode each row.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(implode_image,0,y,implode_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    y_distance=y_scale*(y-y_center);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Determine if the pixel is within an ellipse.
      */
      x_distance=x_scale*(x-x_center);
      distance=x_distance*x_distance+y_distance*y_distance;
      if (distance >= (radius*radius))
        *q=AcquireOnePixel(image,x,y,exception);
      else
        {
          double
            factor;

          /*
            Implode the pixel.
          */
          factor=1.0;
          if (distance > 0.0)
            factor=pow(sin(0.5*MagickPI*sqrt(distance)/radius),-amount);
          *q=InterpolateColor(image,factor*x_distance/x_scale+x_center,
            factor*y_distance/y_scale+y_center,exception);
        }
      q++;
    }
    if (!SyncImagePixels(implode_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(ImplodeImageText,y,image->rows);
  }
  return(implode_image);
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
%  Method MedianFilterImage creates a new image that is a copy of an existing
%  one with each pixel component replaced with the median color in a pixel
%  neighborhood.
%
%  The format of the MedianFilterImage method is:
%
%      Image *MedianFilterImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o median_image: Method MedianFilterImage returns a pointer to the image
%      after it is `filtered'.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the address of a structure of type Image;  returned from
%      ReadImage.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int BlueCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) (color_1->blue-color_2->blue));
}

static int GreenCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) (color_1->green-color_2->green));
}

static int OpacityCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) (color_1->opacity-color_2->opacity));
}

static int RedCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) (color_1->red-color_2->red));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport Image *MedianFilterImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define MedianFilterImageText  "  Filter image with neighborhood ranking...  "

  Image
    *median_image;

  long
    center,
    width,
    y;

  PixelPacket
    *window,
    *w;

  register const PixelPacket
    *p,
    *r;

  register long
    u,
    v,
    x;

  register PixelPacket
    *q;

  /*
    Initialize mediand image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionWarning,"Unable to median filter image",
      "image smaller than kernel radius");
  median_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (median_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(median_image,TrueColorType);
  /*
    Allocate window.
  */
  window=(PixelPacket *) AcquireMemory(width*width*sizeof(PixelPacket));
  if (window == (PixelPacket *) NULL)
    {
      DestroyImage(median_image);
      ThrowImageException(ResourceLimitWarning,"Unable to median filter image",
        "Memory allocation failed")
    }
  /*
    Median filter each image row.
  */
  center=width*width/2;
  for (y=0; y < (long) median_image->rows; y++)
  {
    v=Min(Max(y-width/2,0),(long) image->rows-width);
    p=AcquireImagePixels(image,0,v,image->columns,width,exception);
    q=SetImagePixels(median_image,0,y,median_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) median_image->columns; x++)
    {
      w=window;
      r=p+Min(Max(x-width/2,0),(long) image->columns-width);
      for (v=0; v < width; v++)
      {
        for (u=0; u < width; u++)
          *w++=r[u];
        r+=image->columns;
      }
      qsort((void *) window,width*width,sizeof(PixelPacket),RedCompare);
      q->red=window[center].red;
      qsort((void *) window,width*width,sizeof(PixelPacket),GreenCompare);
      q->green=window[center].green;
      qsort((void *) window,width*width,sizeof(PixelPacket),BlueCompare);
      q->blue=window[center].blue;
      qsort((void *) window,width*width,sizeof(PixelPacket),OpacityCompare);
      q->opacity=window[center].opacity;
      q++;
    }
    if (!SyncImagePixels(median_image))
      break;
    if (QuantumTick(y,median_image->rows))
      MagickMonitor(MedianFilterImageText,y,median_image->rows);
  }
  LiberateMemory((void **) &window);
  return(median_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o r p h I m a g e s                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MorphImages morphs a sequence of images.  Both the next pixels and
%  size are linearly interpolated to give the appearance of a meta-morphosis
%  from one next to the next.
%
%  The format of the MorphImage method is:
%
%      Image *MorphImages(const Image *image,const unsigned long number_frames,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o morph_images: Method MorphImages returns an next sequence that
%      has linearly interpolated pixels and size between two input image.
%
%    o image: The image.
%
%    o number_frames:  This unsigned integer reflects the number of in-between
%      image to generate.  The more in-between frames, the smoother
%      the morph.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MorphImages(const Image *image,
  const unsigned long number_frames,ExceptionInfo *exception)
{
#define MorphImageText  "  Morph image sequence...  "

  double
    alpha,
    beta;

  Image
    *clone_image,
    *morph_image,
    *morph_images;

  long
    y;

  MonitorHandler
    handler;

  register const Image
    *next;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    scene;

  /*
    Clone first frame in sequence.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  morph_images=CloneImage(image,0,0,True,exception);
  if (morph_images == (Image *) NULL)
    return((Image *) NULL);
  if (image->next == (Image *) NULL)
    {
      /*
        Morph single image.
      */
      for (i=1; i < (long) number_frames; i++)
      {
        morph_images->next=CloneImage(image,0,0,True,exception);
        if (morph_images->next == (Image *) NULL)
          {
            DestroyImages(morph_images);
            return((Image *) NULL);
          }
        morph_images->next->previous=morph_images;
        morph_images=morph_images->next;
        MagickMonitor(MorphImageText,i,number_frames);
      }
      while (morph_images->previous != (Image *) NULL)
        morph_images=morph_images->previous;
      return(morph_images);
    }
  /*
    Morph image sequence.
  */
  scene=0;
  for (next=image; next->next != (Image *) NULL; next=next->next)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    for (i=0; i < (long) number_frames; i++)
    {
      beta=(double) (i+1.0)/(number_frames+1.0);
      alpha=1.0-beta;
      clone_image=CloneImage(next,0,0,True,exception);
      if (clone_image == (Image *) NULL)
        break;
      morph_images->next=ZoomImage(clone_image,
        (unsigned long) (alpha*next->columns+beta*next->next->columns+0.5),
        (unsigned long) (alpha*next->rows+beta*next->next->rows+0.5),exception);
      DestroyImage(clone_image);
      if (morph_images->next == (Image *) NULL)
        break;
      morph_images->next->previous=morph_images;
      morph_images=morph_images->next;
      clone_image=CloneImage(next->next,0,0,True,exception);
      if (clone_image == (Image *) NULL)
        break;
      morph_image=ZoomImage(clone_image,morph_images->columns,
        morph_images->rows,exception);
      DestroyImage(clone_image);
      if (morph_image == (Image *) NULL)
        break;
      SetImageType(morph_images,TrueColorType);
      for (y=0; y < (long) morph_images->rows; y++)
      {
        p=AcquireImagePixels(morph_image,0,y,morph_image->columns,1,exception);
        q=GetImagePixels(morph_images,0,y,morph_images->columns,1);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          break;
        for (x=0; x < (long) morph_images->columns; x++)
        {
          q->red=(Quantum) (alpha*q->red+beta*p->red+0.5);
          q->green=(Quantum) (alpha*q->green+beta*p->green+0.5);
          q->blue=(Quantum) (alpha*q->blue+beta*p->blue+0.5);
          q->opacity=(Quantum) (alpha*q->opacity+beta*p->opacity+0.5);
          p++;
          q++;
        }
        if (!SyncImagePixels(morph_images))
          break;
      }
      DestroyImage(morph_image);
    }
    if (i < (long) number_frames)
      break;
    /*
      Clone last frame in sequence.
    */
    morph_images->next=CloneImage(next->next,0,0,True,exception);
    if (morph_images->next == (Image *) NULL)
      break;
    morph_images->next->previous=morph_images;
    morph_images=morph_images->next;
    (void) SetMonitorHandler(handler);
    MagickMonitor(MorphImageText,scene,GetNumberScenes(image));
    scene++;
  }
  while (morph_images->previous != (Image *) NULL)
    morph_images=morph_images->previous;
  if (next->next != (Image *) NULL)
    {
      DestroyImages(morph_images);
      return((Image *) NULL);
    }
  return(morph_images);
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
%  Method MotionBlurImage creates a new image that has a "motion blur"
%  effect applied to it.
%
%  Andrew Protano contributed this effect.
%
%  The format of the MotionBlurImage method is:
%
%    Image *MotionBlurImage(const Image *image,const double radius,
%      const double sigma,const double amount,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method MotionBlurImage returns a pointer to the image
%      after it is blur.  A null image is returned if there is a memory
%      shortage.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
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
    normalize;

  int
    bias;

  register int
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
    (*kernel)[i/KernelRank]+=
      exp((double) (-i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
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
  AggregatePacket
    aggregate;

  double
    *kernel;

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

  register int
    i;

  register long
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
      while ((int) (MaxRGB*kernel[width-1]) > 0)
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
    ThrowImageException(OptionWarning,"Unable to motion blur image",
      "kernel radius is too small");
  offsets=(PointInfo *) AcquireMemory(width*sizeof(PointInfo));
  if (offsets == (PointInfo *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to motion blur image",
      "Memory allocation failed");
  /*
    Allocate blur image.
  */
  blur_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (blur_image == (Image *) NULL)
    {
      LiberateMemory((void **) &kernel);
      LiberateMemory((void **) &offsets);
      return((Image *) NULL);
    }
  SetImageType(blur_image,TrueColorType);
  x=(int) (width*sin(DegreesToRadians(angle)));
  y=(int) (width*cos(DegreesToRadians(angle)));
  for (i=0; i < width; i++)
  {
    offsets[i].x=i*x/sqrt(x*x+y*y);
    offsets[i].y=i*y/sqrt(x*x+y*y);
  }
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(blur_image,0,y,blur_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      (void) memset(&aggregate,0,sizeof(AggregatePacket));
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
      MagickMonitor(BlurImageText,y,image->rows);
  }
  LiberateMemory((void **) &kernel);
  LiberateMemory((void **) &offsets);
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O i l P a i n t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OilPaintImage creates a new image that is a copy of an existing
%  one with each pixel component replaced with the color of greatest frequency
%  in a circular neighborhood.
%
%  The format of the OilPaintImage method is:
%
%      Image *OilPaintImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o paint_image: Method OilPaintImage returns a pointer to the image
%      after it is `painted'.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o radius: The radius of the circular neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *OilPaintImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define OilPaintImageText  "  Oil paint image...  "

  Image
    *paint_image;

  long
    count,
    j,
    k,
    width,
    y;

  register const PixelPacket
    *p,
    *r;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    *histogram;

  /*
    Initialize painted image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionWarning,"Unable to oil paint",
      "image smaller than radius");
  paint_image=CloneImage(image,0,0,False,exception);
  if (paint_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(paint_image,TrueColorType);
  /*
    Allocate histogram and scanline.
  */
  histogram=(unsigned int *) AcquireMemory((MaxRGB+1)*sizeof(unsigned int));
  if (histogram == (unsigned int *) NULL)
    {
      DestroyImage(paint_image);
      ThrowImageException(ResourceLimitWarning,"Unable to oil paint",
        "Memory allocation failed")
    }
  /*
    Paint each row of the image.
  */
  k=0;
  for (y=width; y < (long) (image->rows-width); y++)
  {
    p=AcquireImagePixels(image,0,y-width,image->columns,2*width+1,exception);
    q=GetImagePixels(paint_image,0,y,paint_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    p+=width*image->columns+width;
    q+=width;
    for (x=width; x < (long) (image->columns-width); x++)
    {
      /*
        Determine most frequent color.
      */
      count=0;
      for (i=0; i < (MaxRGB+1); i++)
        histogram[i]=0;
      for (i=0; i < width; i++)
      {
        r=p-(width-i-1)*image->columns-i-1;
        for (j=0; j < (2*i+1); j++)
        {
          k=Intensity(*r);
          histogram[k]++;
          if ((int) histogram[k] > count)
            {
              *q=(*r);
              count=histogram[k];
            }
          r++;
        }
        r=p+(width-i-1)*image->columns-i-1;
        for (j=0; j < (2*i+1); j++)
        {
          k=Intensity(*r);
          histogram[k]++;
          if ((int) histogram[k] > count)
            {
              *q=(*r);
              count=histogram[k];
            }
          r++;
        }
      }
      r=p-width;
      for (j=0; j < (2*width+1); j++)
      {
        k=Intensity(*r);
        histogram[k]++;
        if ((int) histogram[k] > count)
          {
            *q=(*r);
            count=histogram[k];
          }
        r++;
      }
      p++;
      q++;
    }
    if (!SyncImagePixels(paint_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(OilPaintImageText,y,image->rows);
  }
  LiberateMemory((void **) &histogram);
  return(paint_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     P l a s m a I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PlasmaImage initializes an image with plasma fractal values.  The
%  image must be initialized with a base color and the random number generator
%  seeded before this method is called.
%
%  The format of the PlasmaImage method is:
%
%      unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
%        int attenuate,int depth)
%
%  A description of each parameter follows:
%
%    o status: Method PlasmaImage returns True when the fractal process
%      is complete.  Otherwise False is returned.
%
%    o image: the image.
%
%    o segment:  specifies a structure of type SegmentInfo that defines
%      the boundaries of the area where the plasma fractals are applied.
%
%    o attenuate:  specifies the plasma attenuation factor.
%
%    o depth: this integer values define the plasma recursion depth.
%
%
*/

static Quantum PlasmaPixel(const double pixel,const double noise)
{
  double
    value;

  value=pixel+(noise/2.0)-(noise ? (rand() % (int) noise) : 0.0);
  if (value <= 0.0)
    return(0);
  if (value >= MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

MagickExport unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
  int attenuate,int depth)
{
  double
    plasma;

  int
    x_mid,
    y_mid;

  PixelPacket
    pixel_1,
    pixel_2;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(segment != (SegmentInfo *) NULL);
  if (depth != 0)
    {
      SegmentInfo
        local_info;

      /*
        Divide the area into quadrants and recurse.
      */
      depth--;
      attenuate++;
      x_mid=(int) (segment->x1+segment->x2)/2;
      y_mid=(int) (segment->y1+segment->y2)/2;
      local_info=(*segment);
      local_info.x2=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.y1=y_mid;
      local_info.x2=x_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y1=y_mid;
      return(PlasmaImage(image,&local_info,attenuate,depth));
    }
  x_mid=(int) (segment->x1+segment->x2)/2;
  y_mid=(int) (segment->y1+segment->y2)/2;
  if ((segment->x1 == x_mid) && (segment->x2 == x_mid) &&
      (segment->y1 == y_mid) && (segment->y2 == y_mid))
    return(False);
  /*
    Average pixels and apply plasma.
  */
  plasma=(double) (MaxRGB+1)/(2.0*attenuate);
  if ((segment->x1 != x_mid) || (segment->x2 != x_mid))
    {
      /*
        Left pixel.
      */
      pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y1);
      pixel_2=GetOnePixel(image,(int) segment->x1,(int) segment->y2);
      q=SetImagePixels(image,(int) segment->x1,(int) y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel((int) (pixel_1.red+pixel_2.red)/2,plasma);
      q->green=PlasmaPixel((int) (pixel_1.green+pixel_2.green)/2,plasma);
      q->blue=PlasmaPixel((int) (pixel_1.blue+pixel_2.blue)/2,plasma);
      (void) SyncImagePixels(image);
      if (segment->x1 != segment->x2)
        {
          /*
            Right pixel.
          */
          pixel_1=GetOnePixel(image,(int) segment->x2,(int) segment->y1);
          pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y2);
          q=SetImagePixels(image,(int) segment->x2,(int) y_mid,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
          q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
          q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->y1 != y_mid) || (segment->y2 != y_mid))
    {
      if ((segment->x1 != x_mid) || (segment->y2 != y_mid))
        {
          /*
            Bottom pixel.
          */
          pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y2);
          pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y2);
          q=SetImagePixels(image,(int) x_mid,(int) segment->y2,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
          q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
          q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
          (void) SyncImagePixels(image);
        }
      if (segment->y1 != segment->y2)
        {
          /*
            Top pixel.
          */
          pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y1);
          pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y1);
          q=SetImagePixels(image,(int) x_mid,(int) segment->y1,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
          q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
          q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->x1 != segment->x2) || (segment->y1 != segment->y2))
    {
      /*
        Middle pixel.
      */
      pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y1);
      pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y2);
      q=SetImagePixels(image,(int) x_mid,(int) y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
      q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
      q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
      (void) SyncImagePixels(image);
    }
  if (((segment->x2-segment->x1) < 3.0) && ((segment->y2-segment->y1) < 3.0))
    return(True);
  return(False);
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
%  Method ReduceNoiseImage creates a new image that is a copy of an existing
%  one with the noise minified with a noise peak elimination filter.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The principal function of noise peak elimination filter is to smooth the
%  objects within an image without losing edge information and without
%  creating undesired structures.  The central idea of the algorithm is to
%  replace a pixel with its next neighbor in value within a window, if this
%  pixel has been found to be noise.  A pixel is defined as noise if and
%  only if this pixel is a maximum or minimum within the window.
%
%  The format of the ReduceNoiseImage method is:
%
%      Image *ReduceNoiseImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o noise_image: Method ReduceNoiseImage returns a pointer to the image
%      after the noise is minified.  A null image is returned if there is a
%      memory shortage.
%
%    o image: the address of a structure of type Image;  returned from
%      ReadImage.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReduceNoiseImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
#define ReduceNoiseImageText  "  Reduce the image noise...  "

  Image
    *noise_image;

  long
    center,
    width,
    y;

  PixelPacket
    pixel,
    *w,
    *window;

  register const PixelPacket
    *p,
    *r;

  register long
    i,
    u,
    v,
    x;

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
    ThrowImageException(OptionWarning,"Unable to noise filter image",
      "image smaller than kernel radius");
  noise_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(noise_image,TrueColorType);
  /*
    Allocate window.
  */
  window=(PixelPacket *) AcquireMemory(width*width*sizeof(PixelPacket));
  if (window == (PixelPacket *) NULL)
    {
      DestroyImage(noise_image);
      ThrowImageException(ResourceLimitWarning,"Unable to noise filter image",
        "Memory allocation failed")
    }
  /*
    Median filter each image row.
  */
  center=width*width/2;
  for (y=0; y < (long) noise_image->rows; y++)
  {
    i=Min(Max(y-width/2,0),(long) image->rows-width);
    p=AcquireImagePixels(image,0,i,image->columns,width,exception);
    q=SetImagePixels(noise_image,0,y,noise_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) noise_image->columns; x++)
    {
      w=window;
      r=p+Min(Max(x-width/2,0),(long) image->columns-width);
      for (v=0; v < width; v++)
      {
        for (u=0; u < width; u++)
          *w++=r[u];
        r+=image->columns;
      }
      qsort((void *) window,width*width,sizeof(PixelPacket),RedCompare);
      pixel=window[center];
      if (pixel.red == window[0].red)
        {
          for (i=1; i < (width*width-1); i++)
            if (window[i].red != window[0].red)
              break;
          pixel=window[i];
        }
      else
        if (pixel.red == window[width*width-1].red)
          {
            for (i=(width*width-2); i > 0; i--)
              if (window[i].red != window[width*width-1].red)
                break;
            pixel=window[i];
          }
      q->red=pixel.red;
      qsort((void *) window,width*width,sizeof(PixelPacket),GreenCompare);
      pixel=window[center];
      if (pixel.green == window[0].green)
        {
          for (i=1; i < (width*width-1); i++)
            if (window[i].green != window[0].green)
              break;
          pixel=window[i];
        }
      else
        if (pixel.green == window[width*width-1].green)
          {
            for (i=(width*width-2); i > 0; i--)
              if (window[i].green != window[width*width-1].green)
                break;
            pixel=window[i];
          }
      q->green=pixel.green;
      qsort((void *) window,width*width,sizeof(PixelPacket),BlueCompare);
      pixel=window[center];
      if (pixel.blue == window[0].blue)
        {
          for (i=1; i < (width*width-1); i++)
            if (window[i].blue != window[0].blue)
              break;
          pixel=window[i];
        }
      else
        if (pixel.blue == window[width*width-1].blue)
          {
            for (i=(width*width-2); i > 0; i--)
              if (window[i].blue != window[width*width-1].blue)
                break;
            pixel=window[i];
          }
      q->blue=pixel.blue;
      qsort((void *) window,width*width,sizeof(PixelPacket),OpacityCompare);
      pixel=window[center];
      if (pixel.opacity == window[0].opacity)
        {
          for (i=1; i < (width*width-1); i++)
            if (window[i].opacity != window[0].opacity)
              break;
          pixel=window[i];
        }
      else
        if (pixel.opacity == window[width*width-1].opacity)
          {
            for (i=(width*width-2); i > 0; i--)
              if (window[i].opacity != window[width*width-1].opacity)
                break;
            pixel=window[i];
          }
      q->opacity=pixel.opacity;
      q++;
    }
    if (!SyncImagePixels(noise_image))
      break;
    if (QuantumTick(y,noise_image->rows))
      MagickMonitor(ReduceNoiseImageText,y,noise_image->rows);
  }
  LiberateMemory((void **) &window);
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
%  Method ShadeImage creates a new image that is a copy of an existing
%  one with the image pixels shaded using a distance light source.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ShadeImage method is:
%
%      Image *ShadeImage(const Image *image,const unsigned int color_shading,
%        double azimuth,double elevation,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o shade_image: Method ShadeImage returns a pointer to the image
%      after it is shaded.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o color_shading: A value other than zero shades the red, green, and blue
%      components of the image.
%
%    o azimuth, elevation:  A double value that indicates the light source
%      direction.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ShadeImage(const Image *image,
  const unsigned int color_shading,double azimuth,double elevation,
  ExceptionInfo *exception)
{
#define ShadeImageText  "  Shade image...  "

  double
    distance,
    normal_distance,
    shade;

  Image
    *shade_image;

  long
    dx,
    dy,
    y;

  PointInfo
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
  shade_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (shade_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(shade_image,TrueColorType);
  /*
    Compute the light vector.
  */
  azimuth=DegreesToRadians(azimuth);
  elevation=DegreesToRadians(elevation);
  light.x=MaxRGB*cos(azimuth)*cos(elevation);
  light.y=MaxRGB*sin(azimuth)*cos(elevation);
  light.z=MaxRGB*sin(elevation);
  normal.z=2*MaxRGB;  /* constant Z of surface normal */
  /*
    Shade image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,Min(Max(y-1,0),(long) image->rows-3),
      image->columns,3,exception);
    q=SetImagePixels(shade_image,0,y,shade_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Shade this row of pixels.
    */
    s0=p;
    s1=s0+image->columns;
    s2=s1+image->columns;
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Determine the surface normal and compute shading.
      */
      dx=(x > 0) ? 1 : -1;
      dy=(x < ((long) image->columns-1)) ? 1 : -1;
      normal.x=Intensity(*(s0-dx))+Intensity(*(s1-dx))+Intensity(*(s2-dx))-
        Intensity(*(s0+dy))-Intensity(*(s1+dy))-Intensity(*(s2+dy));
      normal.y=Intensity(*(s2-dx))+Intensity(*s2)+Intensity(*(s2+dy))-
        Intensity(*(s0-dx))-Intensity(*s0)-Intensity(*(s0+dy));
      if ((normal.x == 0.0) && (normal.y == 0.0))
        shade=light.z;
      else
        {
          shade=0.0;
          distance=normal.x*light.x+normal.y*light.y+normal.z*light.z;
          if (distance > 0.0)
            {
              normal_distance=
                normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
              if (fabs(normal_distance) > 0.0000001)
                shade=distance/sqrt(normal_distance);
            }
        }
      if (!color_shading)
        {
          q->red=(Quantum) shade;
          q->green=(Quantum) shade;
          q->blue=(Quantum) shade;
        }
      else
        {
          q->red=((unsigned long) (shade*s1->red)/MaxRGB);
          q->green=((unsigned long) (shade*s1->green)/MaxRGB);
          q->blue=((unsigned long) (shade*s1->blue)/MaxRGB);
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
      MagickMonitor(ShadeImageText,y,image->rows);
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
%  Method SharpenImage creates a new image that is sharpened version of
%  the original image using a Laplacian convolution kernel.
%
%  The format of the SharpenImage method is:
%
%    Image *SharpenImage(const Image *image,const double radius,
%      const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o sharp_image: Method SharpenImage returns a pointer to the image
%      after it is sharp.  A null image is returned if there is a memory
%      shortage.
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
    *kernel,
    normalize;

  Image
    *sharp_image;

  int
    width;

  register int
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionWarning,"Unable to sharpen image",
      "image is smaller than radius");
  kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to sharpen image",
      "Memory allocation failed");
  i=0;
  normalize=0.0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      kernel[i]=exp(-(double) (u*u+v*v)/(sigma*sigma));
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
%     S o l a r i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SolarizeImage produces a 'solarization' effect seen when exposing
%  a photographic film to light during the development process.
%
%  The format of the SolarizeImage method is:
%
%      void SolarizeImage(Image *image,const double threshold)
%
%  A description of each parameter follows:
%
%    o image: the image.
%
%    o threshold:  An double value that defines the extent of the
%      solarization.
%
%
*/
MagickExport void SolarizeImage(Image *image,const double threshold)
{
#define SolarizeImageText  "  Solarize the image colors...  "

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Solarize DirectClass packets.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=q->red > threshold ? MaxRGB-q->red : q->red;
          q->green=q->green > threshold ? MaxRGB-q->green : q->green;
          q->blue=q->blue > threshold ? MaxRGB-q->blue : q->blue;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(SolarizeImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Solarize PseudoClass packets.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].red=image->colormap[i].red > threshold ?
          MaxRGB-image->colormap[i].red : image->colormap[i].red;
        image->colormap[i].green=image->colormap[i].green > threshold ?
          MaxRGB-image->colormap[i].green : image->colormap[i].green;
        image->colormap[i].blue=image->colormap[i].blue > threshold ?
          MaxRGB-image->colormap[i].blue : image->colormap[i].blue;
      }
      SyncImage(image);
      break;
    }
  }
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
%  Method SpreadImage creates a new image that is a copy of an existing
%  one with the image pixels randomly displaced.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the SpreadImage method is:
%
%      Image *SpreadImage(iconst Image *image,const unsigned int amount,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o spread_image: Method SpreadImage returns a pointer to the image
%      after it is spread.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o amount:  An unsigned value constraining the "vicinity" for choosing
%      a random pixel to swap.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SpreadImage(const Image *image,const unsigned int amount,
  ExceptionInfo *exception)
{
#define SpreadImageText  "  Spread image...  "

  Image
    *spread_image;

  long
    quantum,
    x_distance,
    y,
    y_distance;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 3) || (image->rows < 3))
    return((Image *) NULL);
  /*
    Initialize spread image attributes.
  */
  spread_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (spread_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(spread_image,TrueColorType);
  /*
    Convolve each row.
  */
  quantum=(amount+1) >> 1;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(spread_image,0,y,spread_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      do
      {
        x_distance=(rand() & (amount+1))-quantum;
        y_distance=(rand() & (amount+1))-quantum;
      } while (((x+x_distance) < 0) || ((y+y_distance) < 0) ||
               ((x+x_distance) >= (long) image->columns) ||
               ((y+y_distance) >= (long) image->rows));
      *q++=AcquireOnePixel(image,x+x_distance,y+y_distance,exception);
    }
    if (!SyncImagePixels(spread_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(SpreadImageText,y,image->rows);
  }
  return(spread_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t e g a n o I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SteganoImage hides a digital watermark within the image.
%
%  The format of the SteganoImage method is:
%
%      Image *SteganoImage(Image *image,Image *watermark,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o stegano_image: Method SteganoImage returns a pointer to the
%      steganographic image with the watermark hidden.  A null image is
%      returned if there is a memory shortage.
%
%    o image: The image.
%
%    o watermark: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SteganoImage(const Image *image,const Image *watermark,
  ExceptionInfo *exception)
{
#define GetBit(a,i) (((a) >> (i)) & 1L)
#define SetBit(a,i,set) a=(set) ? (a) | (1L << (i)) : (a) & ~(1L << (i))
#define SteganoImageText  "  Hide image...  "

  Image
    *stegano_image;

  long
    c,
    i,
    j,
    k,
    y;

  PixelPacket
    pixel;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize steganographic image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(watermark != (const Image *) NULL);
  assert(watermark->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  stegano_image=CloneImage(image,0,0,False,exception);
  if (stegano_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(stegano_image,TrueColorType);
  stegano_image->depth=QuantumDepth;
  /*
    Hide watermark in low-order bits of image.
  */
  c=0;
  i=0;
  j=0;
  k=image->offset;
  for (i=QuantumDepth-1; (i >= 0) && (j < QuantumDepth); i--)
  {
    for (y=0; (y < (long) watermark->rows) && (j < QuantumDepth); y++)
    {
      for (x=0; (x < (long) watermark->columns) && (j < QuantumDepth); x++)
      {
        pixel=AcquireOnePixel(watermark,x,y,exception);
        q=GetImagePixels(stegano_image,k % (long) stegano_image->columns,
        k/(long) stegano_image->columns,1,1);
        if (q == (PixelPacket *) NULL)
          break;
        switch (c)
        {
          case 0:
          {
            SetBit(q->red,j,GetBit(Intensity(pixel),i));
            break;
          }
          case 1:
          {
            SetBit(q->green,j,GetBit(Intensity(pixel),i));
            break;
          }
          case 2:
          {
            SetBit(q->blue,j,GetBit(Intensity(pixel),i));
            break;
          }
        }
        (void) SyncImage(stegano_image);
        c++;
        if (c == 3)
          c=0;
        k++;
        if (k == (long) (stegano_image->columns*stegano_image->columns))
          k=0;
        if (k == image->offset)
          j++;
      }
    }
    if (QuantumTick(i,QuantumDepth))
      MagickMonitor(SteganoImageText,i,QuantumDepth);
  }
  if (stegano_image->storage_class == PseudoClass)
    SyncImage(stegano_image);
  return(stegano_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t e r e o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StereoImage combines two images and produces a single image that
%  is the composite of a left and right image of a stereo pair.  The left
%  image is converted to gray scale and written to the red channel of the
%  stereo image.  The right image is converted to gray scale and written to the
%  blue channel of the stereo image.  View the composite image with red-blue
%  glasses to create a stereo effect.
%
%  The format of the StereoImage method is:
%
%      Image *StereoImage(const Image *image,const Image *offset_image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o stereo_image: Method StereoImage returns a pointer to the stereo
%      image.  A null image is returned if there is a memory shortage.
%
%    o image: The image.
%
%    o offset_image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *StereoImage(const Image *image,const Image *offset_image,
  ExceptionInfo *exception)
{
#define StereoImageText  "  Stereo image...  "

  Image
    *stereo_image;

  long
    y;

  register const PixelPacket
    *p,
    *q;

  register long
    x;

  register PixelPacket
    *r;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  assert(offset_image != (const Image *) NULL);
  if ((image->columns != offset_image->columns) ||
      (image->rows != offset_image->rows))
    ThrowImageException(ResourceLimitWarning,"Unable to create stereo image",
      "left and right image sizes differ");
  /*
    Initialize stereo image attributes.
  */
  stereo_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (stereo_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(stereo_image,TrueColorType);
  /*
    Copy left image to red channel and right image to blue channel.
  */
  for (y=0; y < (long) stereo_image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=AcquireImagePixels(offset_image,0,y,offset_image->columns,1,exception);
    r=SetImagePixels(stereo_image,0,y,stereo_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL) ||
        (r == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) stereo_image->columns; x++)
    {
      r->red=p->red;
      r->green=q->green;
      r->blue=q->blue;
      r->opacity=(p->opacity+q->opacity)/2;
      p++;
      q++;
      r++;
    }
    if (!SyncImagePixels(stereo_image))
      break;
    if (QuantumTick(y,stereo_image->rows))
      MagickMonitor(StereoImageText,y,stereo_image->rows);
  }
  return(stereo_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S w i r l I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SwirlImage creates a new image that is a copy of an existing
%  one with the image pixels "swirl" at a specified angle.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the SwirlImage method is:
%
%      Image *SwirlImage(const Image *image,double degrees,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o swirl_image: Method SwirlImage returns a pointer to the image
%      after it is swirl.  A null image is returned if there is a memory
%      shortage.
%
%    o image: the image.
%
%    o degrees:  An double value that defines the tightness of the swirling.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SwirlImage(const Image *image,double degrees,
  ExceptionInfo *exception)
{
#define SwirlImageText  "  Swirl image...  "

  double
    cosine,
    distance,
    factor,
    radius,
    sine,
    x_center,
    x_distance,
    x_scale,
    y_center,
    y_distance,
    y_scale;

  long
    y;

  Image
    *swirl_image;

  register PixelPacket
    *q;

  register long
    x;

  /*
    Initialize swirl image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  swirl_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (swirl_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(swirl_image,TrueColorType);
  /*
    Compute scaling factor.
  */
  x_center=image->columns/2.0;
  y_center=image->rows/2.0;
  radius=Max(x_center,y_center);
  x_scale=1.0;
  y_scale=1.0;
  if (image->columns > image->rows)
    y_scale=(double) image->columns/image->rows;
  else
    if (image->columns < image->rows)
      x_scale=(double) image->rows/image->columns;
  degrees=DegreesToRadians(degrees);
  /*
    Swirl each row.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(swirl_image,0,y,swirl_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    y_distance=y_scale*(y-y_center);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Determine if the pixel is within an ellipse.
      */
      x_distance=x_scale*(x-x_center);
      distance=x_distance*x_distance+y_distance*y_distance;
      if (distance >= (radius*radius))
        *q=AcquireOnePixel(image,x,y,exception);
      else
        {
          /*
            Swirl the pixel.
          */
          factor=1.0-sqrt(distance)/radius;
          sine=sin(degrees*factor*factor);
          cosine=cos(degrees*factor*factor);
          *q=InterpolateColor(image,
            (cosine*x_distance-sine*y_distance)/x_scale+x_center,
            (sine*x_distance+cosine*y_distance)/y_scale+y_center,exception);
        }
      q++;
    }
    if (!SyncImagePixels(swirl_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(SwirlImageText,y,image->rows);
  }
  return(swirl_image);
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
%  Method ThresholdImage thresholds the reference image.
%
%  The format of the ThresholdImage method is:
%
%      unsigned int ThresholdImage(Image *image,const double threshold)
%
%  A description of each parameter follows:
%
%    o image: the image.
%
%    o threshold: A double indicating the threshold value.
%
%
*/
MagickExport unsigned int ThresholdImage(Image *image,const double threshold)
{
#define ThresholdImageText  "  Threshold the image...  "

  IndexPacket
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
    ThrowBinaryException(ResourceLimitWarning,"Unable to threshold image",
      "Memory allocation failed");
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=Intensity(*q) < threshold ? 0 : 1;
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(ThresholdImageText,y,image->rows);
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
%  Method UnsharpMaskImage creates a new image that is sharpened version of
%  the original image using the unsharp mask algorithm.
%
%  The format of the UnsharpMaskImage method is:
%
%    Image *UnsharpMaskImage(const Image *image,const double radius,
%      const double sigma,const double amount,const double threshold,
%      ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o unsharp_image: Method UnsharpMaskImage returns a pointer to the image
%      after it is sharpened.  A null image is returned if there is a memory
%      shortage.
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

  double
    blue,
    green,
    opacity,
    red;

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
      red=p->red-(int) q->red;
      if (AbsoluteValue(2.0*red) < (MaxRGB*threshold))
        red=p->red;
      else
        red=p->red+(red*amount);
      green=p->green-(int) q->green;
      if (AbsoluteValue(2.0*green) < (MaxRGB*threshold))
        green=p->green;
      else
        green=p->green+(green*amount);
      blue=p->blue-(int) q->blue;
      if (AbsoluteValue(2.0*blue) < (MaxRGB*threshold))
        blue=p->blue;
      else
        blue=p->blue+(blue*amount);
      opacity=p->opacity-(int) q->opacity;
      if (AbsoluteValue(2.0*opacity) < (MaxRGB*threshold))
        opacity=p->opacity;
      else
        opacity=p->opacity+(opacity*amount);
      q->red=(Quantum) ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
      q->green=(Quantum)
        ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
      q->blue=(Quantum) ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
      q->opacity=(Quantum)
        ((opacity < 0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity+0.5);
      p++;
      q++;
    }
    if (!SyncImagePixels(sharp_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(SharpenImageText,y,image->rows);
  }
  return(sharp_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     W a v e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WaveImage creates a new image that is a copy of an existing
%  one with the image pixels altered along a sine wave.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the WaveImage method is:
%
%      Image *WaveImage(const Image *image,const double amplitude,
%        const double wave_length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o wave_image: Method WaveImage returns a pointer to the image after
%      it is waved.  A null image is returned if there is a memory shortage.
%
%    o image: the image.
%
%    o amplitude, frequency:  A double value that indicates the amplitude
%      and wave_length of the sine wave.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *WaveImage(const Image *image,const double amplitude,
  const double wave_length,ExceptionInfo *exception)
{
#define WaveImageText  "  Wave image...  "

  double
    *sine_map;

  Image
    *wave_image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize wave image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  wave_image=CloneImage(image,image->columns,(int)
    (int) (image->rows+2.0*fabs(amplitude)),False,exception);
  if (wave_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(wave_image,TrueColorType);
  /*
    Allocate sine map.
  */
  sine_map=(double *) AcquireMemory(wave_image->columns*sizeof(double));
  if (sine_map == (double *) NULL)
    {
      DestroyImage(wave_image);
      ThrowImageException(ResourceLimitWarning,"Unable to wave image",
        "Memory allocation failed")
    }
  for (x=0; x < (long) wave_image->columns; x++)
    sine_map[x]=fabs(amplitude)+amplitude*sin((2*MagickPI*x)/wave_length);
  /*
    Wave image.
  */
  for (y=0; y < (long) wave_image->rows; y++)
  {
    q=SetImagePixels(wave_image,0,y,wave_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) wave_image->columns; x++)
    {
      *q=InterpolateColor(image,(double) x,(double) y-sine_map[x],exception);
      q++;
    }
    if (!SyncImagePixels(wave_image))
      break;
    if (QuantumTick(y,wave_image->rows))
      MagickMonitor(WaveImageText,y,wave_image->rows);
  }
  LiberateMemory((void **) &sine_map);
  return(wave_image);
}
