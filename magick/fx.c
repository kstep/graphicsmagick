/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                 FFFFF  X   X                                %
%                                 F       X X                                 %
%                                 FFF      X                                  %
%                                 F       X X                                 %
%                                 F      X   X                                %
%                                                                             %
%                                                                             %
%                  ImageMagick Image Special Effects Methods                  %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 October 1996                                %
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
#include "cache.h"
#include "gem.h"
#include "log.h"
#include "monitor.h"
#include "utility.h"

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
  charcoal_image=CloneImage(image,0,0,True,exception);
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
%  ColorizeImage() blends the fill color with each pixel in the image.
%  A percentage blend is specified with opacity.  Control the application
%  of different color components by specifying a different percentage for
%  each component (e.g. 90/100/10 is 90% red, 100% green, and 10% blue).
%
%  The format of the ColorizeImage method is:
%
%      Image *ColorizeImage(const Image *image,const char *opacity,
%        const PixelPacket target,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity:  A character string indicating the level of opacity as a
%      percentage.
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

  DoublePixelPacket
    pixel;

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
  colorize_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (colorize_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(colorize_image,TrueColorType);
  if (opacity == (const char *) NULL)
    return(colorize_image);
  /*
    Determine RGB values of the pen color.
  */
  pixel.red=100.0;
  pixel.green=100.0;
  pixel.blue=100.0;
  pixel.opacity=100.0;
  count=sscanf(opacity,"%lf%*[/,]%lf%*[/,]%lf%*[/,]%lf",
    &pixel.red,&pixel.green,&pixel.blue,&pixel.opacity);
  if (count == 1)
    {
      if (pixel.red == 0.0)
        return(colorize_image);
      pixel.green=pixel.red;
      pixel.blue=pixel.red;
      pixel.opacity=pixel.red;
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
      q->red=(Quantum)
        ((p->red*(100.0-pixel.red)+target.red*pixel.red)/100.0);
      q->green=(Quantum)
        ((p->green*(100.0-pixel.green)+target.green*pixel.green)/100.0);
      q->blue=(Quantum)
        ((p->blue*(100.0-pixel.blue)+target.blue*pixel.blue)/100.0);
      q->opacity=(Quantum)
        ((p->opacity*(100.0-pixel.opacity)+target.opacity*pixel.opacity)/100.0);
      p++;
      q++;
    }
    if (!SyncImagePixels(colorize_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ColorizeImageText,y,image->rows,exception))
        break;
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
%  ConvolveImage() applies a custom convolution kernel to the image.
%
%  The format of the ConvolveImage method is:
%
%      Image *ConvolveImage(const Image *image,const unsigned int order,
%        const double *kernel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o order: The number of columns and rows in the filter kernel.
%
%    o kernel: An array of double representing the convolution kernel.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ConvolveImage(const Image *image,const unsigned int order,
  const double *kernel,ExceptionInfo *exception)
{
#define ConvolveImageText  "  Convolving image...  "

  DoublePixelPacket
    pixel,
    zero;

  double
    normalize,
    *normal_kernel;

  Image
    *convolve_image;

  long
    width,
    y;

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

  unsigned int
    logging,
    range_check;

  /*
    Initialize convolve image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=(long) order;
  if ((width % 2) == 0)
    ThrowImageException(OptionError,"UnableToConvolveImage",
      "KernelWidthMustBeAnOddNumber");
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException(OptionError,"UnableToConvolveImage",
      "ImageSmallerThanKernelWidth");
  convolve_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (convolve_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(convolve_image,TrueColorType);
  /*
    Convolve image.
  */
  normal_kernel=(double *) AcquireMemory(width*width*sizeof(double));
  if (normal_kernel == (double *) NULL)
    {
      DestroyImage(convolve_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToConvolveImage")
    }
  normalize=0.0;
  for (i=0; i < (width*width); i++)
    normalize+=kernel[i];
  if (AbsoluteValue(normalize) <= MagickEpsilon)
    normalize=1.0;
  normalize=1.0/normalize;
  range_check=False;
  for (i=0; i < (width*width); i++)
  {
    normal_kernel[i]=normalize*kernel[i];
    if (normal_kernel[i] <= 0.0)
      range_check=True;
  }

  logging=LogMagickEvent(TransformEvent,GetMagickModule(),
        "  ConvolveImage with %dx%d kernel:",width,width);
  if (logging)
    {
      char
        cell_text[13],
        row_text[65];

      k=kernel;
      for (v=0; v<width; v++)
      {
         *row_text='\0';
         for (u=0; u<width; u++)
         {
           FormatString(cell_text,"%#12.4g",*k++);
           (void) strncat(row_text,cell_text,12);
           if (u%5 == 4)
             {
                (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                    "   %.64s", row_text);
                *row_text='\0';
             }
         }
         if (u > 5)
           (void) strcat(row_text,"\n");
         if (row_text[0] != '\0')
           (void) LogMagickEvent(TransformEvent,GetMagickModule(),
               "   %.64s", row_text);
      }
    }
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) convolve_image->rows; y++)
  {
    p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
      exception);
    q=SetImagePixels(convolve_image,0,y,convolve_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    if (!range_check)
      for (x=0; x < (long) convolve_image->columns; x++)
      {
        pixel=zero;
        r=p;
        k=normal_kernel;
        for (v=0; v < width; v++)
        {
          for (u=0; u < width; u++)
          {
            pixel.red+=(*k)*r[u].red;
            pixel.green+=(*k)*r[u].green;
            pixel.blue+=(*k)*r[u].blue;
            pixel.opacity+=(*k)*r[u].opacity;
            k++;
          }
          r+=image->columns+width;
        }
        q->red=(Quantum) (pixel.red+0.5);
        q->green=(Quantum) (pixel.green+0.5);
        q->blue=(Quantum) (pixel.blue+0.5);
        q->opacity=(Quantum) (pixel.opacity+0.5);
        p++;
        q++;
      }
    else
      for (x=0; x < (long) convolve_image->columns; x++)
      {
        pixel=zero;
        r=p;
        k=normal_kernel;
        for (v=0; v < width; v++)
        {
          for (u=0; u < width; u++)
          {
            pixel.red+=(*k)*r[u].red;
            pixel.green+=(*k)*r[u].green;
            pixel.blue+=(*k)*r[u].blue;
            pixel.opacity+=(*k)*r[u].opacity;
            k++;
          }
          r+=image->columns+width;
        }
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
    if (!SyncImagePixels(convolve_image))
      break;
    if (QuantumTick(y,convolve_image->rows))
      if (!MagickMonitor(ConvolveImageText,y,convolve_image->rows,exception))
        break;
  }
  LiberateMemory((void **) &normal_kernel);
  return(convolve_image);
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
%    o image: The image.
%
%    o amount:  Define the extent of the implosion.
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
  implode_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (implode_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(implode_image,implode_image->background_color.opacity !=
    OpaqueOpacity ? TrueColorMatteType : TrueColorType);
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
            factor=pow(sin(MagickPI*sqrt(distance)/radius/2),-amount);
          *q=InterpolateColor(image,factor*x_distance/x_scale+x_center,
            factor*y_distance/y_scale+y_center,exception);
        }
      q++;
    }
    if (!SyncImagePixels(implode_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ImplodeImageText,y,image->rows,exception))
        break;
  }
  return(implode_image);
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
%  The MorphImages() method requires a minimum of two images.  The first
%  image is transformed into the second by a number of intervening images
%  as specified by frames.
%
%  The format of the MorphImage method is:
%
%      Image *MorphImages(const Image *image,const unsigned long number_frames,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o number_frames:  Define the number of in-between image to generate.
%      The more in-between frames, the smoother the morph.
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

  unsigned long
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
            DestroyImageList(morph_images);
            return((Image *) NULL);
          }
        morph_images->next->previous=morph_images;
        morph_images=morph_images->next;
        if (!MagickMonitor(MorphImageText,i,number_frames,exception))
          break;
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
      beta=((double) i+1.0)/(number_frames+1.0);
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
    if (!MagickMonitor(MorphImageText,scene,GetImageListLength(image),exception))
      break;
    scene++;
  }
  while (morph_images->previous != (Image *) NULL)
    morph_images=morph_images->previous;
  if (next->next != (Image *) NULL)
    {
      DestroyImageList(morph_images);
      return((Image *) NULL);
    }
  return(morph_images);
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
%  OilPaintImage() applies a special effect filter that simulates an oil
%  painting.  Each pixel is replaced by the most frequent color occurring
%  in a circular region defined by radius.
%
%  The format of the OilPaintImage method is:
%
%      Image *OilPaintImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
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
    k,
    width,
    y;

  register const PixelPacket
    *p,
    *r;

  register long
    u,
    v,
    x;

  register PixelPacket
    *q;

  unsigned long
    count,
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
    ThrowImageException(OptionError,"UnableToPaintImage",
      "ImageSmallerThanRadius");
  paint_image=CloneImage(image,0,0,True,exception);
  if (paint_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(paint_image,TrueColorType);
  /*
    Allocate histogram and scanline.
  */
  histogram=(unsigned long *) AcquireMemory(65536L*sizeof(unsigned long));
  if (histogram == (unsigned long *) NULL)
    {
      DestroyImage(paint_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToOilPaintImage")
    }
  /*
    Paint each row of the image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
      exception);
    q=SetImagePixels(paint_image,0,y,paint_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Determine most frequent color.
      */
      count=0;
      (void) memset(histogram,0,65536L*sizeof(unsigned long));
      r=p;
      for (v=0; v < width; v++)
      {
        for (u=0; u < width; u++)
        {
          k=ScaleQuantumToShort(PixelIntensityToQuantum(r+u));
          histogram[k]++;
          if (histogram[k] > count)
            {
              *q=(r[u]);
              count=histogram[k];
            }
          k++;
        }
        r+=image->columns+width;
      }
      p++;
      q++;
    }
    if (!SyncImagePixels(paint_image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(OilPaintImageText,y,image->rows,exception))
        break;
  }
  LiberateMemory((void **) &histogram);
  return(paint_image);
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
%  SolarizeImage() applies a special effect to the image, similar to the effect
%  achieved in a photo darkroom by selectively exposing areas of photo
%  sensitive paper to light.  Threshold ranges from 0 to MaxRGB and is a
%  measure of the extent of the solarization.
%
%  The format of the SolarizeImage method is:
%
%      void SolarizeImage(Image *image,const double threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold:  Define the extent of the solarization.
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
          q->red=(Quantum) (q->red > threshold ? MaxRGB-q->red : q->red);
          q->green=(Quantum)
            (q->green > threshold ? MaxRGB-q->green : q->green);
          q->blue=(Quantum) (q->blue > threshold ? MaxRGB-q->blue : q->blue);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(SolarizeImageText,y,image->rows,&image->exception))
            break;
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
        image->colormap[i].red=(Quantum) (image->colormap[i].red > threshold ?
          MaxRGB-image->colormap[i].red : image->colormap[i].red);
        image->colormap[i].green=(Quantum) (image->colormap[i].green > threshold ?
          MaxRGB-image->colormap[i].green : image->colormap[i].green);
        image->colormap[i].blue=(Quantum) (image->colormap[i].blue > threshold ?
          MaxRGB-image->colormap[i].blue : image->colormap[i].blue);
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
%                                                                             %
%   S t e g a n o I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use SteganoImage() to hide a digital watermark within the image.
%  Recover the hidden watermark later to prove that the authenticity of
%  an image.  Offset defines the start position within the image to hide
%  the watermark.
%
%  The format of the SteganoImage method is:
%
%      Image *SteganoImage(const Image *image,Image *watermark,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o watermark: The watermark image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SteganoImage(const Image *image,const Image *watermark,
  ExceptionInfo *exception)
{
#define GetBit(a,i) (((a) >> (i)) & 0x01)
#define SetBit(a,i,set) \
  a=(Quantum) ((set) ? (a) | (1UL << (i)) : (a) & ~(1UL << (i)))
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
  stegano_image=CloneImage(image,0,0,True,exception);
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
        switch ((int) c)
        {
          case 0:
          {
            SetBit(q->red,j,GetBit(PixelIntensityToQuantum(&pixel),i));
            break;
          }
          case 1:
          {
            SetBit(q->green,j,GetBit(PixelIntensityToQuantum(&pixel),i));
            break;
          }
          case 2:
          {
            SetBit(q->blue,j,GetBit(PixelIntensityToQuantum(&pixel),i));
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
      if (!MagickMonitor(SteganoImageText,i,QuantumDepth,exception))
        break;
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
%  StereoImage() combines two images and produces a single image that is the
%  composite of a left and right image of a stereo pair.  Special red-green
%  stereo glasses are required to view this effect.
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
%    o offset_image: Another image.
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
    ThrowImageException(ImageError,"UnableToCreateStereoImage",
      "LeftAndRightImageSizesDiffer");
  /*
    Initialize stereo image attributes.
  */
  stereo_image=CloneImage(image,image->columns,image->rows,True,exception);
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
      if (!MagickMonitor(StereoImageText,y,stereo_image->rows,exception))
        break;
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
%  SwirlImage() swirls the pixels about the center of the image, where
%  degrees indicates the sweep of the arc through which each pixel is moved.
%  You get a more dramatic effect as the degrees move from 1 to 360.
%
%  The format of the SwirlImage method is:
%
%      Image *SwirlImage(const Image *image,double degrees,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o degrees: Define the tightness of the swirling effect.
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
  swirl_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (swirl_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(swirl_image,swirl_image->background_color.opacity !=
    OpaqueOpacity ? TrueColorMatteType : TrueColorType);
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
      if (!MagickMonitor(SwirlImageText,y,image->rows,exception))
        break;
  }
  return(swirl_image);
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
%  The WaveImage() filter creates a "ripple" effect in the image by shifting
%  the pixels vertically along a sine wave whose amplitude and wavelength
%  is specified by the given parameters.
%
%  The format of the WaveImage method is:
%
%      Image *WaveImage(const Image *image,const double amplitude,
%        const double wave_length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o amplitude, frequency:  Define the amplitude and wave_length of the
%      sine wave.
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

  VirtualPixelMethod
    virtual_pixel_method;

  /*
    Initialize wave image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  wave_image=CloneImage(image,image->columns,(long)
    (image->rows+2.0*fabs(amplitude)),False,exception);
  if (wave_image == (Image *) NULL)
    return((Image *) NULL);
  SetImageType(wave_image,wave_image->background_color.opacity !=
    OpaqueOpacity ? TrueColorMatteType : TrueColorType);
  /*
    Allocate sine map.
  */
  sine_map=(double *) AcquireMemory(wave_image->columns*sizeof(double));
  if (sine_map == (double *) NULL)
    {
      DestroyImage(wave_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
        "UnableToWaveImage")
    }
  for (x=0; x < (long) wave_image->columns; x++)
    sine_map[x]=fabs(amplitude)+amplitude*sin((2*MagickPI*x)/wave_length);
  /*
    Wave image.
  */
  virtual_pixel_method=GetImageVirtualPixelMethod(image);
  if (virtual_pixel_method == UndefinedVirtualPixelMethod)
    SetImageVirtualPixelMethod(image,ConstantVirtualPixelMethod);
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
      if (!MagickMonitor(WaveImageText,y,wave_image->rows,exception))
        break;
  }
  SetImageVirtualPixelMethod(image,virtual_pixel_method);
  LiberateMemory((void **) &sine_map);
  return(wave_image);
}
