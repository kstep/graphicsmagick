/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                EEEEE  FFFFF  FFFFF  EEEEE  CCCC  TTTTT  SSSSS               %
%                E      F      F      E     C        T    SS                  %
%                EEE    FFF    FFF    EEE   C        T     SSS                %
%                E      F      F      E     C        T       SS               %
%                EEEEE  F      F      EEEEE  CCCC    T    SSSSS               %
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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#include "defines.h"

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
%      Image *AddNoiseImage(Image *image,const NoiseType noise_type,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o noise_image: Method AddNoiseImage returns a pointer to the image after
%      the noise is minified.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o noise_type:  The type of noise: Gaussian, multiplicative Gaussian,
%      impulse, laplacian, or Poisson.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *AddNoiseImage(Image *image,const NoiseType noise_type,
  ExceptionInfo *exception)
{
#define AddNoiseImageText  "  Adding noise to the image...  "

  Image
    *noise_image;

  int
    y;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Initialize noise image attributes.
  */
  assert(image != (Image *) NULL);
  noise_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  noise_image->class=DirectClass;
  /*
    Add noise in each row.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(noise_image,0,y,noise_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (int) image->columns; x++)
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
      ProgressMonitor(AddNoiseImageText,y,image->rows);
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
%  Method BlurImage creates a new image that is a copy of an existing
%  one with the pixels blurred.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the BlurImage method is:
%
%      Image *BlurImage(Image *image,const double factor,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method BlurImage returns a pointer to the image
%      after it is blurred.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o order: An unsigned int that is the order of the pixel neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *BlurImage(Image *image,const unsigned int order,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *blur_image;

  int
    i;

  register int
    u,
    v;

  assert(image != (Image *) NULL);
  kernel=(double *) AllocateMemory(order*order*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to blur image",
      "Memory allocation failed");
  i=0;
  for (v=(-(int) order/2); v <= ((int) order/2); v++)
  {
    for (u=(-(int) order/2); u <= ((int) order/2); u++)
    {
      kernel[i]=exp(-(double) v*v-u*u);
      i++;
    }
  }
  kernel[i/2]=13.0/order;
  blur_image=ConvolveImage(image,order,kernel,exception);
  FreeMemory((void **) &kernel);
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
%      Image *ColorizeImage(Image *image,const char *opacity,
%        const PixelPacket target,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o opacity:  A character string indicating the level of opacity as a
%      percentage (0-100).
%
%    o target: A color value.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *ColorizeImage(Image *image,const char *opacity,
  const PixelPacket target,ExceptionInfo *exception)
{
#define ColorizeImageText  "  Colorizing the image...  "

  Image
    *colorize_image;

  int
    y;

  long
    blue,
    count,
    green,
    red;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Allocate colorized image.
  */
  assert(image != (Image *) NULL);
  colorize_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (colorize_image == (Image *) NULL)
    return((Image *) NULL);
  colorize_image->class=DirectClass;
  /*
    Determine RGB values of the pen color.
  */
  red=100;
  green=100;
  blue=100;
  count=sscanf(opacity,"%ld/%ld/%ld",&red,&green,&blue);
  if (count == 1)
    {
      if (red == 0)
        return(colorize_image);
      green=red;
      blue=red;
    }
  /*
    Colorize DirectClass image.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(colorize_image,0,y,colorize_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      q->red=((unsigned long) (p->red*(100-red)+target.red*red)/100);
      q->green=((unsigned long) (p->green*(100-green)+target.green*green)/100);
      q->blue=((unsigned long) (p->blue*(100-blue)+target.blue*blue)/100);
      p++;
      q++;
    }
    if (!SyncImagePixels(colorize_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(ColorizeImageText,y,image->rows);
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
%      Image *ConvolveImage(Image *image,const unsigned int order,
%        const double *kernel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o convolve_image: Method ConvolveImage returns a pointer to the image
%      after it is convolved.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o order:  The number of columns and rows in the filter kernel.
%
%    o kernel:  An array of double representing the convolution kernel.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *ConvolveImage(Image *image,const unsigned int order,
  const double *kernel,ExceptionInfo *exception)
{
#define ConvolveImageText  "  Convolving image...  "
#define Cx(x) \
  (x) < 0 ? (x)+image->columns : (x) >= image->columns ? (x)-image->columns : x
#define Cy(y) \
  (y) < 0 ? (y)+image->rows : (y) >= image->rows ? (y)-image->rows : y

  double
    blue,
    green,
    normalize,
    opacity,
    red;

  Image
    *convolve_image;

  int
    i,
    y;

  PixelPacket
    *p,
    pixel;

  register const double
    *k;

  register int
    u,
    v,
    x;

  register PixelPacket
    *q,
    *s;

  /*
    Initialize convolved image attributes.
  */
  assert(image != (Image *) NULL);
  if ((order % 2) == 0)
    ThrowImageException(ResourceLimitWarning,"Unable to convolve image",
      "kernel order must be an odd number");
  if ((image->columns < order) || (image->rows < order))
    ThrowImageException(ResourceLimitWarning,"Unable to convolve image",
      "image smaller than kernel order");
  convolve_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (convolve_image == (Image *) NULL)
    return((Image *) NULL);
  convolve_image->class=DirectClass;
  /*
    Convolve image.
  */
  normalize=0.0;
  for (i=0; i < (order*order); i++)
    normalize+=kernel[i];
  for (y=0; y < (int) convolve_image->rows; y++)
  {
    p=(PixelPacket *) NULL;
    q=SetImagePixels(convolve_image,0,y,convolve_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) convolve_image->columns; x++)
    {
      red=0.0;
      green=0.0;
      blue=0.0;
      opacity=0.0;
      k=kernel;
      if ((x < ((int) order/2)) || (x >= (image->columns-(int) order/2)) ||
          (y < ((int) order/2)) || (y >= (image->rows-(int) order/2)))
        {
          for (v=(-(int) order/2); v <= (int) order/2; v++)
          {
            for (u=(-(int) order/2); u <= (int) order/2; u++)
            {
              pixel=GetOnePixel(image,Cx(x+u),Cy(y+v));
              red+=(*k)*pixel.red;
              green+=(*k)*pixel.green;
              blue+=(*k)*pixel.blue;
              opacity+=(*k)*pixel.opacity;
              k++;
            }
          }
        }
      else
        {
          if (p == (PixelPacket *) NULL)
            p=GetImagePixels(image,0,y-(int) order/2,image->columns,order);
          s=p+x;
          for (v=(-(int) order/2); v <= (int) order/2; v++)
          {
            for (u=(-(int) order/2); u <= (int) order/2; u++)
            {
              red+=(*k)*s[u].red;
              green+=(*k)*s[u].green;
              blue+=(*k)*s[u].blue;
              opacity+=(*k)*s[u].opacity;
              k++;
            }
            s+=image->columns;
          }
        }
      if ((normalize != 0.0) && (normalize != 1.0))
        {
          red/=normalize;
          green/=normalize;
          blue/=normalize;
          opacity/=normalize;
        }
      q->red=(red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5;
      q->green=(green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5;
      q->blue=(blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5;
      q->opacity=(opacity < 0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity+0.5;
      q++;
    }
    if (!SyncImagePixels(convolve_image))
      break;
    if (QuantumTick(y,convolve_image->rows))
      ProgressMonitor(ConvolveImageText,y,convolve_image->rows);
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
%      Image *DespeckleImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o despeckle_image: Method DespeckleImage returns a pointer to the image
%      after it is despeckled.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *DespeckleImage(Image *image,ExceptionInfo *exception)
{
#define DespeckleImageText  "  Despeckling image...  "

  Image
    *despeckle_image;

  int
    y;

  Quantum
    *blue,
    *buffer,
    *green,
    *matte,
    *red;

  register int
    i,
    j,
    x;

  register PixelPacket
    *p,
    *q;

  static const int
    X[4]= {0, 1, 1,-1},
    Y[4]= {1, 0, 1, 1};

  unsigned int
    packets;

  /*
    Allocate despeckled image.
  */
  assert(image != (Image *) NULL);
  despeckle_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (despeckle_image == (Image *) NULL)
    return((Image *) NULL);
  despeckle_image->class=DirectClass;
  /*
    Allocate image buffers.
  */
  packets=(image->columns+2)*(image->rows+2);
  red=(Quantum *) AllocateMemory(packets*sizeof(Quantum));
  green=(Quantum *) AllocateMemory(packets*sizeof(Quantum));
  blue=(Quantum *) AllocateMemory(packets*sizeof(Quantum));
  matte=(Quantum *) AllocateMemory(packets*sizeof(Quantum));
  buffer=(Quantum *) AllocateMemory(packets*sizeof(Quantum));
  if ((red == (Quantum *) NULL) || (green == (Quantum *) NULL) ||
      (blue == (Quantum *) NULL) || (matte == (Quantum *) NULL) ||
      (buffer == (Quantum *) NULL))
    {
      DestroyImage(despeckle_image);
      ThrowImageException(ResourceLimitWarning,"Unable to despeckle image",
        "Memory allocation failed");
    }
  /*
    Zero image buffers.
  */
  for (i=0; i < (int) packets; i++)
  {
    red[i]=0;
    green[i]=0;
    blue[i]=0;
    matte[i]=0;
    buffer[i]=0;
  }
  /*
    Copy image pixels to color component buffers
  */
  j=image->columns+2;
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    j++;
    for (x=0; x < (int) image->columns; x++)
    {
      red[j]=p->red;
      green[j]=p->green;
      blue[j]=p->blue;
      matte[j]=p->opacity;
      p++;
      j++;
    }
    j++;
  }
  /*
    Reduce speckle in red channel.
  */
  for (i=0; i < 4; i++)
  {
    ProgressMonitor(DespeckleImageText,i,12);
    Hull(X[i],Y[i],1,image->columns,image->rows,red,buffer);
    Hull(-X[i],-Y[i],1,image->columns,image->rows,red,buffer);
    Hull(-X[i],-Y[i],-1,image->columns,image->rows,red,buffer);
    Hull(X[i],Y[i],-1,image->columns,image->rows,red,buffer);
  }
  /*
    Reduce speckle in green channel.
  */
  for (i=0; i < (int) packets; i++)
    buffer[i]=0;
  for (i=0; i < 4; i++)
  {
    ProgressMonitor(DespeckleImageText,i+4,12);
    Hull(X[i],Y[i],1,image->columns,image->rows,green,buffer);
    Hull(-X[i],-Y[i],1,image->columns,image->rows,green,buffer);
    Hull(-X[i],-Y[i],-1,image->columns,image->rows,green,buffer);
    Hull(X[i],Y[i],-1,image->columns,image->rows,green,buffer);
  }
  /*
    Reduce speckle in blue channel.
  */
  for (i=0; i < (int) packets; i++)
    buffer[i]=0;
  for (i=0; i < 4; i++)
  {
    ProgressMonitor(DespeckleImageText,i+8,12);
    Hull(X[i],Y[i],1,image->columns,image->rows,blue,buffer);
    Hull(-X[i],-Y[i],1,image->columns,image->rows,blue,buffer);
    Hull(-X[i],-Y[i],-1,image->columns,image->rows,blue,buffer);
    Hull(X[i],Y[i],-1,image->columns,image->rows,blue,buffer);
  }
  /*
    Copy color component buffers to despeckled image.
  */
  j=image->columns+2;
  for (y=0; y < (int) image->rows; y++)
  {
    q=SetImagePixels(despeckle_image,0,y,despeckle_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    j++;
    for (x=0; x < (int) image->columns; x++)
    {
      q->red=red[j];
      q->green=green[j];
      q->blue=blue[j];
      q->opacity=matte[j];
      q++;
      j++;
    }
    if (!SyncImagePixels(despeckle_image))
      break;
    j++;
  }
  /*
    Free memory.
  */
  FreeMemory((void **) &buffer);
  FreeMemory((void **) &matte);
  FreeMemory((void **) &blue);
  FreeMemory((void **) &green);
  FreeMemory((void **) &red);
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
%  one with the edges highlighted.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the EdgeImage method is:
%
%      Image *EdgeImage(Image *image,const unsigned int order,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o edge_image: Method EdgeImage returns a pointer to the image
%      after it is edge.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o order: An unsigned int that is the order of the pixel neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *EdgeImage(Image *image,const unsigned int order,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *edge_image;

  int
    i;

  assert(image != (Image *) NULL);
  kernel=(double *) AllocateMemory(order*order*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to detect edges",
      "Memory allocation failed");
  for (i=0; i < (order*order); i++)
    kernel[i]=(-1.0);
  kernel[i/2]=order*order-1.0;
  edge_image=ConvolveImage(image,order,kernel,exception);
  FreeMemory((void **) &kernel);
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
%      Image *EmbossImage(Image *image,unsigned int order,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o emboss_image: Method EmbossImage returns a pointer to the image
%      after it is embossed.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o order: An unsigned int that is the order of the pixel neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *EmbossImage(Image *image,const unsigned int order,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *emboss_image;

  int
    i,
    j;

  register int
    u,
    v;

  assert(image != (Image *) NULL);
  kernel=(double *) AllocateMemory(order*order*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to emboss image",
      "Memory allocation failed");
  i=0;
  j=(int) order/2;
  for (v=(-(int) order/2); v <= ((int) order/2); v++)
  {
    for (u=(-(int) order/2); u <= ((int) order/2); u++)
    {
      kernel[i]=(int) ((u < 0 || v < 0 ? -8.0 : 8.0)*exp(-(double) v*v-u*u));
      if (u == j)
        kernel[i]=0.0;
      i++;
    }
    j--;
  }
  emboss_image=ConvolveImage(image,order,kernel,exception);
  if (emboss_image != (Image *) NULL)
    EqualizeImage(emboss_image);
  FreeMemory((void **) &kernel);
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
%      Image *EnhanceImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o enhance_image: Method EnhanceImage returns a pointer to the image
%      after it is enhanced.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *EnhanceImage(Image *image,ExceptionInfo *exception)
{
#define Enhance(weight) \
  mean=(int) (s->red+red)/2; \
  distance=s->red-(int) red; \
  distance_squared= \
    (2.0*(MaxRGB+1)+mean)*distance*distance/(double) (MaxRGB+1); \
  distance=s->green-(int) green; \
  distance_squared+=4.0*distance*distance; \
  distance=s->blue-(int) blue; \
  distance_squared+= \
    (3.0*(MaxRGB+1)-1.0-mean)*distance*distance/(double) (MaxRGB+1); \
  if (distance_squared < Threshold) \
    { \
      total_red+=(weight)*s->red; \
      total_green+=(weight)*s->green; \
      total_blue+=(weight)*s->blue; \
      total_weight+=(weight); \
    } \
  s++;
#define EnhanceImageText  "  Enhancing image...  "
#define Threshold  2500

  double
    distance_squared,
    mean,
    total_blue,
    total_green,
    total_red,
    total_weight;

  Image
    *enhance_image;

  int
    y;

  long
    distance;

  Quantum
    blue,
    green,
    red;

  register int
    x;

  register PixelPacket
    *p,
    *q,
    *s;

  /*
    Initialize enhanced image attributes.
  */
  assert(image != (Image *) NULL);
  if ((image->columns < 5) || (image->rows < 5))
    return((Image *) NULL);
  enhance_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (enhance_image == (Image *) NULL)
    return((Image *) NULL);
  enhance_image->class=DirectClass;
  /*
    Enhance image.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    /*
      Read another scan line.
    */
    p=GetImagePixels(image,0,Min(Max(y-2,0),image->rows-5),image->columns,5);
    q=SetImagePixels(enhance_image,0,y,enhance_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Transfer first 2 pixels of the scanline.
    */
    *q++=(*(p+2*image->columns));
    *q++=(*(p+2*image->columns+1));
    for (x=2; x < (int) (image->columns-2); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0.0;
      total_green=0.0;
      total_blue=0.0;
      total_weight=0.0;
      s=p+2*image->columns+2;
      red=s->red;
      green=s->green;
      blue=s->blue;
      s=p;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      s=p+image->columns;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      s=p+2*image->columns;
      Enhance(10); Enhance(40); Enhance(80); Enhance(40); Enhance(10);
      s=p+3*image->columns;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      s=p+4*image->columns;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      q->red=(total_red+(total_weight/2)-1)/total_weight;
      q->green=(total_green+(total_weight/2)-1)/total_weight;
      q->blue=(total_blue+(total_weight/2)-1)/total_weight;
      q->opacity=(p+2*image->columns)->opacity;
      q++;
    }
    p++;
    *q++=(*p);
    p++;
    *q++=(*p);
    if (!SyncImagePixels(enhance_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(EnhanceImageText,y,image->rows-2);
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
%  Method GaussianBlurImage creates a blurred copy of the input image.  We
%  convolve the image with a gaussian operator of the given width and
%  standard deviation (sigma).
%
%  Each output pixel is set to a value that is the weighted average of the
%  input pixels in an area enclosing the pixel.  The width parameter
%  determines how large the area is.  Each pixel in the area is weighted
%  in the average according to its distance from the center, and the
%  standard deviation, sigma. The actual weight is calculated according to
%  the gaussian distribution (also called normal distribution), which
%  looks like a Bell Curve centered on a pixel.  The standard deviation
%  controls how 'pointy' the curve is.  The pixels near the center of the
%  curve (closer to the center of the area we are averaging) contribute
%  more than the distant pixels.
%
%  In general, the width should be wide enough to include most of the
%  total weight under the gaussian for the standard deviation you choose.
%  As a guideline about 90% of the weight lies within two standard
%  deviations, and 98% of the weight within 3 standard deviations. Since
%  the width parameter to the function specifies the radius of the
%  gaussian convolution mask in pixels, not counting the centre pixel, the
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
%  typically less blurred.
%
%  Since a 2d gaussian is seperable, we perform Gaussian Blurring by
%  convolving with two 1d gaussians, first in the x, then in the y
%  direction. For an n by n image and gaussian width w this requires 2wn^2
%  multiplications, while convolving with a 2d gaussian requres w^2n^2
%  mults.
%
%  We are blur the image into a copy, and the original is left untouched.
%  We must process the image in two passes, in each pass we change the
%  pixel based on its neightbours, but we need the pixel's original value
%  for the next pixel's calculation. For the first pass we could use the
%  original image but that's no good for the second pass, and it would
%  imply that the original image have to stay around in ram. Instead we
%  use a small (size=width) buffer to store the pixels we have
%  overwritten.
%
%  BlurImage deals with boundary pixels by assuming the neighbours not in
%  the image are zero (black) pixels. The function could be easily
%  modified to pre-fill the buffer with the first width pixels from the
%  image scan-line, in effect averaging only the available neighbours for
%  any pixel.
%
%  This method was contributed by runger@cs.mcgill.ca.
%
%  The format of the GaussianBlurImage method is:
%
%      Image *GaussianBlurImage(Image *image,const double width,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method GaussianBlurImage returns a pointer to the image
%      after it is blurred.  A null image is returned if there is a memory
%      shortage.
%
%    o width: The radius of the gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the gaussian, in pixels.
%
*/
Export Image *GaussianBlurImage(Image *image,const double width,
  const double sigma,ExceptionInfo *exception)
{
#define GaussianBlurImageText  "  Gaussian blur image...  "

  double
    blue,
    green,
    *kernel,
    normalize,
    red;

  Image
    *blur_image;

  int
    j,
    k,
    radius,
    y;

  PixelPacket
    *scanline;

  register int
    i,
    x;

  register PixelPacket
    *p;

  /*
    Initialize blurred image attributes.
  */
  assert(image != (Image *) NULL);
  blur_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (blur_image == (Image *) NULL)
    return((Image *) NULL);
  blur_image->class=DirectClass;
  if (width < 0.0)
    return(blur_image);
  /*
    Build convolution kernel.
  */
  radius=ceil(width);
  kernel=(double *) AllocateMemory((radius+1)*sizeof(double));
  scanline=(PixelPacket *) AllocateMemory(radius*sizeof(PixelPacket));
  if ((kernel == (double *) NULL) || (scanline == (PixelPacket *) NULL))
    {
      DestroyImage(blur_image);
      ThrowImageException(ResourceLimitWarning,"Unable to gaussian blur image",
        "Memory allocation failed");
    }
  for (i=0; i < (radius+1); i++)
    kernel[i]=exp(-((double) i*i)/(2*sigma*sigma));
  if (width < radius)
    kernel[radius]*=(width-(double) radius+1.0); /* adjust partial pixels */
  normalize=0.0;
  for (i=0; i < (radius+1); i++)
    normalize+=kernel[i];
  for (i=1; i < (radius+1); i++)
    normalize+=kernel[i];
  for (i=0; i < (radius+1); i++)
    kernel[i]/=normalize;
  /*
    Blur each row.
  */
  for (y=0; y < (int) blur_image->rows; y++)
  {
    p=GetImagePixels(blur_image,0,y,blur_image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    j=0;
    (void) memset(scanline,0,radius*sizeof(PixelPacket));
    for (x=0; x < (int) blur_image->columns; x++)
    {
      /*
        Convolve this pixel.
      */
      red=kernel[0]*p->red;
      green=kernel[0]*p->green;
      blue=kernel[0]*p->blue;
      k=j-1;
      if (k < 0)
        k+=radius;
      for (i=1; i < (radius+1); i++)
      {
        red+=kernel[i]*scanline[k].red;
        green+=kernel[i]*scanline[k].green;
        blue+=kernel[i]*scanline[k].blue;
        k--;
        if (k < 0)
          k+=radius;
      }
      for (i=1; i < (radius+1); i++)
      {
        if ((x+i) >= blur_image->columns)
          break;
        red+=kernel[i]*p[i].red;
        green+=kernel[i]*p[i].green;
        blue+=kernel[i]*p[i].blue;
      }
      scanline[j]=(*p);
      j++;
      if (j >= radius)
        j-=radius;
      p->red=red+0.5;
      p->green=green+0.5;
      p->blue=blue+0.5;
      p++;
    }
    if (!SyncImagePixels(blur_image))
      break;
    if (QuantumTick(y,blur_image->rows+blur_image->columns))
      ProgressMonitor(GaussianBlurImageText,y,blur_image->rows+
        blur_image->columns);
  }
  /*
    Blur each column.
  */
  for (x=0; x < (int) blur_image->columns; x++)
  {
    p=GetImagePixels(blur_image,x,0,1,blur_image->rows);
    if (p == (PixelPacket *) NULL)
      break;
    j=0;
    (void) memset(scanline,0,radius*sizeof(PixelPacket));
    for (y=0; y < (int) blur_image->rows; y++)
    {
      /*
        Convolve this pixel.
      */
      red=kernel[0]*p->red;
      green=kernel[0]*p->green;
      blue=kernel[0]*p->blue;
      k=j-1;
      if (k < 0)
        k+=radius;
      for (i=1; i < (radius+1); i++)
      {
        red+=kernel[i]*scanline[k].red;
        green+=kernel[i]*scanline[k].green;
        blue+=kernel[i]*scanline[k].blue;
        k--;
        if (k < 0)
          k+=radius;
      }
      for (i=1; i < (radius+1); i++)
      {
        if ((y+i) >= blur_image->rows)
          break;
        red+=kernel[i]*p[i].red;
        green+=kernel[i]*p[i].green;
        blue+=kernel[i]*p[i].blue;
      }
      scanline[j]=(*p);
      j++;
      if (j >= radius)
        j-=radius;
      p->red=red+0.5;
      p->green=green+0.5;
      p->blue=blue+0.5;
      p++;
    }
    if (!SyncImagePixels(blur_image))
      break;
    if (QuantumTick(blur_image->rows+x,blur_image->rows+blur_image->columns))
      ProgressMonitor(GaussianBlurImageText,blur_image->rows+x,
        blur_image->rows+blur_image->columns);
  }
  /*
    Free resources.
  */
  FreeMemory((void **) &kernel);
  FreeMemory((void **) &scanline);
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
%      Image *ImplodeImage(Image *image,const double factor,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o implode_image: Method ImplodeImage returns a pointer to the image
%      after it is implode.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o factor:  A double value that defines the extent of the implosion.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *ImplodeImage(Image *image,const double factor,
  ExceptionInfo *exception)
{
#define ImplodeImageText  "  Imploding image...  "

  double
    amount,
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

  int
    y;

  register PixelPacket
    *p,
    *q;

  register unsigned int
    x;

  /*
    Initialize implode image attributes.
  */
  assert(image != (Image *) NULL);
  if (!image->matte)
    MatteImage(image,Opaque);
  implode_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (implode_image == (Image *) NULL)
    return((Image *) NULL);
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
  amount=factor/10.0;
  if (amount >= 0)
    amount/=10.0;
  /*
    Implode each row.
  */
  for (y=0; y < image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(implode_image,0,y,implode_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    y_distance=y_scale*(y-y_center);
    for (x=0; x < image->columns; x++)
    {
      /*
        Determine if the pixel is within an ellipse.
      */
      *q=(*p);
      x_distance=x_scale*(x-x_center);
      distance=x_distance*x_distance+y_distance*y_distance;
      if (distance < (radius*radius))
        {
          double
            factor;

          /*
            Implode the pixel.
          */
          factor=1.0;
          if (distance > 0.0)
            factor=
              pow(sin(0.5000000000000001*M_PI*sqrt(distance)/radius),-amount);
          *q=InterpolateColor(image,factor*x_distance/x_scale+x_center,
            factor*y_distance/y_scale+y_center);
        }
      p++;
      q++;
    }
    if (!SyncImagePixels(implode_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(ImplodeImageText,y,image->rows);
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
%      Image *MedianFilterImage(Image *image,const unsigned int order,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o median_image: Method MedianFilterImage returns a pointer to the image
%      after it is `filtered'.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o order: An unsigned int that is the order of the pixel neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static int BlueCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) color_1->blue-color_2->blue);
}

static int GreenCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) color_1->green-color_2->green);
}

static int OpacityCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) color_1->opacity-color_2->opacity);
}

static int RedCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) color_1->red-color_2->red);
}

Export Image *MedianFilterImage(Image *image,const unsigned int order,
  ExceptionInfo *exception)
{
#define MedianFilterImageText \
  "  Filtering image with neighborhood ranking...  "

  Image
    *median_image;

  int
    center,
    y;

  PixelPacket
    *window,
    *p,
    *w;

  register int
    u,
    v,
    x;

  register PixelPacket
    *q,
    *s;

  /*
    Initialize mediand image attributes.
  */
  assert(image != (Image *) NULL);
  if ((image->columns < order) || (image->rows < order))
    ThrowImageException(ResourceLimitWarning,"Unable to median filter image",
      "image smaller than kernel order");
  median_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (median_image == (Image *) NULL)
    return((Image *) NULL);
  median_image->class=DirectClass;
  /*
    Allocate window.
  */
  window=(PixelPacket *) AllocateMemory(order*order*sizeof(PixelPacket));
  if (window == (PixelPacket *) NULL)
    {
      DestroyImage(median_image);
      ThrowImageException(ResourceLimitWarning,"Unable to median filter image",
        "Memory allocation failed");
    }
  /*
    Median filter each image row.
  */
  center=order*order/2;
  for (y=0; y < (int) median_image->rows; y++)
  {
    v=Min(Max(y-(int) order/2,0),image->rows-order);
    p=GetImagePixels(image,0,v,image->columns,order);
    q=SetImagePixels(median_image,0,y,median_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (int) median_image->columns; x++)
    {
      w=window;
      s=p+Min(Max(x-(int) order/2,0),image->columns-order);
      for (v=0; v < order; v++)
      {
        for (u=0; u < order; u++)
          *w++=s[u];
        s+=image->columns;
      }
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) RedCompare);
      q->red=window[center].red;
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) GreenCompare);
      q->green=window[center].green;
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) BlueCompare);
      q->blue=window[center].blue;
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) OpacityCompare);
      q->opacity=window[center].opacity;
      q++;
    }
    if (!SyncImagePixels(median_image))
      break;
    if (QuantumTick(y,median_image->rows))
      ProgressMonitor(MedianFilterImageText,y,median_image->rows);
  }
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
%      Image *MorphImages(Image *image,const unsigned int number_frames,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o morph_images: Method MorphImages returns an next sequence that
%      has linearly interpolated pixels and size between two input image.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o number_frames:  This unsigned integer reflects the number of in-between
%      image to generate.  The more in-between frames, the smoother
%      the morph.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *MorphImages(Image *image,const unsigned int number_frames,
  ExceptionInfo *exception)
{
#define MorphImageText  "  Morphing next sequence...  "

  double
    alpha,
    beta;

  Image
    *morph_image,
    *morph_images;

  int
    y;

  MonitorHandler
    handler;

  register Image
    *next;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  unsigned int
    scene;

  /*
    Clone first frame in sequence.
  */
  assert(image != (Image *) NULL);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionWarning,"Unable to morph image sequence",
      "next sequence required");
  morph_images=CloneImage(image,image->columns,image->rows,True,exception);
  if (morph_images == (Image *) NULL)
    return((Image *) NULL);
  /*
    Morph next.
  */
  scene=0;
  for (next=image; next->next != (Image *) NULL; next=next->next)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    for (i=0; i < (int) number_frames; i++)
    {
      beta=(double) (i+1.0)/(number_frames+1.0);
      alpha=1.0-beta;
      next->orphan=True;
      morph_images->next=ZoomImage(next,
        (unsigned int) (alpha*next->columns+beta*next->next->columns+0.5),
        (unsigned int) (alpha*next->rows+beta*next->next->rows+0.5),exception);
      if (morph_images->next == (Image *) NULL)
        {
          DestroyImages(morph_images);
          return((Image *) NULL);
        }
      morph_images->next->previous=morph_images;
      morph_images=morph_images->next;
      next->next->orphan=True;
      morph_image=ZoomImage(next->next,morph_images->columns,
        morph_images->rows,exception);
      if (morph_image == (Image *) NULL)
        {
          DestroyImages(morph_images);
          return((Image *) NULL);
        }
      morph_images->class=DirectClass;
      for (y=0; y < (int) morph_images->rows; y++)
      {
        p=GetImagePixels(morph_image,0,y,morph_image->columns,1);
        q=GetImagePixels(morph_images,0,y,morph_images->columns,1);
        if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          break;
        for (x=0; x < (int) morph_images->columns; x++)
        {
          q->red=(alpha*q->red+beta*p->red+0.5);
          q->green=(alpha*q->green+beta*p->green+0.5);
          q->blue=(alpha*q->blue+beta*p->blue+0.5);
          q->opacity=(alpha*q->opacity+beta*p->opacity+0.5);
          p++;
          q++;
        }
        if (!SyncImagePixels(morph_images))
          break;
      }
      DestroyImage(morph_image);
    }
    /*
      Clone last frame in sequence.
    */
    morph_images->next=CloneImage(next->next,next->next->columns,
      next->next->rows,True,exception);
    if (morph_images->next == (Image *) NULL)
      {
        DestroyImages(morph_images);
        return((Image *) NULL);
      }
    morph_images->next->previous=morph_images;
    morph_images=morph_images->next;
    (void) SetMonitorHandler(handler);
    ProgressMonitor(MorphImageText,scene,GetNumberScenes(image));
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
%      Image *OilPaintImage(Image *image,const unsigned int radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o paint_image: Method OilPaintImage returns a pointer to the image
%      after it is `painted'.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o radius: An unsigned int that is the radius of the circular
%      neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *OilPaintImage(Image *image,const unsigned int radius,
  ExceptionInfo *exception)
{
#define OilPaintImageText  "  Oil painting image...  "

  Image
    *paint_image;

  int
    count,
    j,
    k,
    y;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q,
    *s;

  unsigned int
    *histogram;

  /*
    Initialize painted image attributes.
  */
  assert(image != (Image *) NULL);
  if ((image->columns < (2*radius+1)) || (image->rows < (2*radius+1)))
    ThrowImageException(ResourceLimitWarning,"Unable to oil paint",
      "image smaller than radius");
  paint_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (paint_image == (Image *) NULL)
    return((Image *) NULL);
  paint_image->class=DirectClass;
  /*
    Allocate histogram and scanline.
  */
  histogram=(unsigned int *) AllocateMemory((MaxRGB+1)*sizeof(unsigned int));
  if (histogram == (unsigned int *) NULL)
    {
      DestroyImage(paint_image);
      ThrowImageException(ResourceLimitWarning,"Unable to oil paint",
        "Memory allocation failed");
    }
  /*
    Paint each row of the image.
  */
  k=0;
  for (y=radius; y < (int) (image->rows-radius-1); y++)
  {
    p=GetImagePixels(image,0,y-radius,image->columns,2*radius+1);
    q=SetImagePixels(paint_image,0,y,paint_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    p+=radius*image->columns+radius;
    q+=radius;
    for (x=radius; x < (int) (image->columns-radius); x++)
    {
      /*
        Determine most frequent color.
      */
      count=0;
      for (i=0; i < (int) (MaxRGB+1); i++)
        histogram[i]=0;
      for (i=0; i < (int) radius; i++)
      {
        s=p-(radius-i)*image->columns-i-1;
        for (j=0; j < (2*i+1); j++)
        {
          k=Intensity(*s);
          histogram[k]++;
          if ((int) histogram[k] > count)
            {
              *q=(*s);
              count=histogram[k];
            }
          s++;
        }
        s=p+(radius-i)*image->columns-i-1;
        for (j=0; j < (2*i+1); j++)
        {
          k=Intensity(*s);
          histogram[k]++;
          if ((int) histogram[k] > count)
            {
              *q=(*s);
              count=histogram[k];
            }
          s++;
        }
      }
      s=p-radius;
      for (j=0; j < (int) (radius+radius+1); j++)
      {
        k=Intensity(*s);
        histogram[k]++;
        if ((int) histogram[k] > count)
          {
            *q=(*s);
            count=histogram[k];
          }
        s++;
      }
      p++;
      q++;
    }
    if (!SyncImagePixels(paint_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(OilPaintImageText,y,image->rows);
  }
  FreeMemory((void **) &histogram);
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
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
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

  value=pixel+(noise/2.0)-((int) noise ? (rand() % (int) noise) : 0.0);
  if (value < 0.0)
    return(0);
  if (value > MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

Export unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
  int attenuate,int depth)
{
  double
    plasma,
    x_mid,
    y_mid;

  PixelPacket
    pixel_1,
    pixel_2;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  if (depth != 0)
    {
      SegmentInfo
        local_info;

      /*
        Divide the area into quadrants and recurse.
      */
      depth--;
      attenuate++;
      x_mid=(segment->x1+segment->x2)/2;
      y_mid=(segment->y1+segment->y2)/2;
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
  x_mid=(segment->x1+segment->x2)/2;
  y_mid=(segment->y1+segment->y2)/2;
  if ((segment->x1 == x_mid) && (segment->x2 == x_mid) &&
      (segment->y1 == y_mid) && (segment->y2 == y_mid))
    return(False);
  /*
    Average pixels and apply plasma.
  */
  plasma=(MaxRGB+1)/(2.0*(double) attenuate);
  if ((segment->x1 != x_mid) || (segment->x2 != x_mid))
    {
      /*
        Left pixel.
      */
      pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y1);
      pixel_2=GetOnePixel(image,(int) segment->x1,(int) segment->y2);
      q=SetImagePixels(image,(int) segment->x1,(int) y_mid,1,1);
      if (q != (PixelPacket *) NULL)
        {
          q->red=PlasmaPixel((int) (pixel_1.red+pixel_2.red)/2,plasma);
          q->green=PlasmaPixel((int) (pixel_1.green+pixel_2.green)/2,plasma);
          q->blue=PlasmaPixel((int) (pixel_1.blue+pixel_2.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
      if (segment->x1 != segment->x2)
        {
          /*
            Right pixel.
          */
          pixel_1=GetOnePixel(image,(int) segment->x2,(int) segment->y1);
          pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y2);
          q=SetImagePixels(image,(int) segment->x2,(int) y_mid,1,1);
          if (q != (PixelPacket *) NULL)
            {
              q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
              q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
              q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
              (void) SyncImagePixels(image);
            }
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
          if (q != (PixelPacket *) NULL)
            {
              q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
              q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
              q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
              (void) SyncImagePixels(image);
            }
        }
      if (segment->y1 != segment->y2)
        {
          /*
            Top pixel.
          */
          pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y1);
          pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y1);
          q=SetImagePixels(image,(int) x_mid,(int) segment->y1,1,1);
          if (q != (PixelPacket *) NULL)
            {
              q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
              q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
              q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
              (void) SyncImagePixels(image);
            }
        }
    }
  if ((segment->x1 != segment->x2) ||
      (segment->y1 != segment->y2))
    {
      /*
        Middle pixel.
      */
      pixel_1=GetOnePixel(image,(int) segment->x1,(int) segment->y1);
      pixel_2=GetOnePixel(image,(int) segment->x2,(int) segment->y2);
      q=SetImagePixels(image,(int) x_mid,(int) y_mid,1,1);
      if (q != (PixelPacket *) NULL)
        {
          q->red=PlasmaPixel((pixel_1.red+pixel_2.red)/2.0,plasma);
          q->green=PlasmaPixel((pixel_1.green+pixel_2.green)/2.0,plasma);
          q->blue=PlasmaPixel((pixel_1.blue+pixel_2.blue)/2.0,plasma);
        }
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
%      Image *ReduceNoiseImage(Image *image,const unsigned int,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o noise_image: Method ReduceNoiseImage returns a pointer to the image
%      after the noise is minified.  A null image is returned if there is a
%      memory shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o order: An unsigned int that is the order of the pixel neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static int ReduceNoiseCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) Intensity(*color_1)-(int) Intensity(*color_2));
}

Export Image *ReduceNoiseImage(Image *image,const unsigned int order,
  ExceptionInfo *exception)
{
#define ReduceNoiseImageText  "  Reducing the image noise...  "

  Image
    *noise_image;

  int
    center,
    i,
    y;

  PixelPacket
    *p,
    pixel,
    *w,
    *window;

  register int
    u,
    v,
    x;

  register PixelPacket
    *q,
    *s;

  /*
    Initialize noised image attributes.
  */
  assert(image != (Image *) NULL);
  if ((image->columns < order) || (image->rows < order))
    ThrowImageException(ResourceLimitWarning,"Unable to noise filter image",
      "image smaller than kernel order");
  noise_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  noise_image->class=DirectClass;
  /*
    Allocate window.
  */
  window=(PixelPacket *) AllocateMemory(order*order*sizeof(PixelPacket));
  if (window == (PixelPacket *) NULL)
    {
      DestroyImage(noise_image);
      ThrowImageException(ResourceLimitWarning,"Unable to noise filter image",
        "Memory allocation failed");
    }
  /*
    Median filter each image row.
  */
  center=order*order/2;
  for (y=0; y < (int) noise_image->rows; y++)
  {
    i=Min(Max(y-(int) order/2,0),image->rows-order);
    p=GetImagePixels(image,0,i,image->columns,order);
    q=SetImagePixels(noise_image,0,y,noise_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (int) noise_image->columns; x++)
    {
      w=window;
      s=p+Min(Max(x-(int) order/2,0),image->columns-order);
      for (v=0; v < order; v++)
      {
        for (u=0; u < order; u++)
          *w++=s[u];
        s+=image->columns;
      }
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) RedCompare);
      pixel=window[center];
      if (pixel.red == window[0].red)
        {
          for (i=1; i < (order*order-1); i++)
            if (window[i].red != window[0].red)
              break;
          pixel=window[i];
        }
      else
        if (pixel.red == window[order*order-1].red)
          {
            for (i=(order*order-2); i > 0; i--)
              if (window[i].red != window[order*order-1].red)
                break;
            pixel=window[i];
          }
      q->red=pixel.red;
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) GreenCompare);
      pixel=window[center];
      if (pixel.green == window[0].green)
        {
          for (i=1; i < (order*order-1); i++)
            if (window[i].green != window[0].green)
              break;
          pixel=window[i];
        }
      else
        if (pixel.green == window[order*order-1].green)
          {
            for (i=(order*order-2); i > 0; i--)
              if (window[i].green != window[order*order-1].green)
                break;
            pixel=window[i];
          }
      q->green=pixel.green;
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) BlueCompare);
      pixel=window[center];
      if (pixel.blue == window[0].blue)
        {
          for (i=1; i < (order*order-1); i++)
            if (window[i].blue != window[0].blue)
              break;
          pixel=window[i];
        }
      else
        if (pixel.blue == window[order*order-1].blue)
          {
            for (i=(order*order-2); i > 0; i--)
              if (window[i].blue != window[order*order-1].blue)
                break;
            pixel=window[i];
          }
      q->blue=pixel.blue;
      qsort((void *) window,order*order,sizeof(PixelPacket),
        (int (*)(const void *,const void *)) OpacityCompare);
      pixel=window[center];
      if (pixel.opacity == window[0].opacity)
        {
          for (i=1; i < (order*order-1); i++)
            if (window[i].opacity != window[0].opacity)
              break;
          pixel=window[i];
        }
      else
        if (pixel.opacity == window[order*order-1].opacity)
          {
            for (i=(order*order-2); i > 0; i--)
              if (window[i].opacity != window[order*order-1].opacity)
                break;
            pixel=window[i];
          }
      q->opacity=pixel.opacity;
      q++;
    }
    if (!SyncImagePixels(noise_image))
      break;
    if (QuantumTick(y,noise_image->rows))
      ProgressMonitor(ReduceNoiseImageText,y,noise_image->rows);
  }
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
%      Image *ShadeImage(Image *image,const unsigned int color_shading,
%        double azimuth,double elevation,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o shade_image: Method ShadeImage returns a pointer to the image
%      after it is shaded.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o color_shading: A value other than zero shades the red, green, and blue
%      components of the image.
%
%    o azimuth, elevation:  A double value that indicates the light source
%      direction.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *ShadeImage(Image *image,const unsigned int color_shading,
  double azimuth,double elevation,ExceptionInfo *exception)
{
#define ShadeImageText  "  Shading image...  "

  double
    distance,
    normal_distance,
    shade;

  Image
    *shade_image;

  int
    y;

  PointInfo
    light,
    normal;

  register int
    x;

  register PixelPacket
    *p,
    *s0,
    *s1,
    *s2,
    *q;

  /*
    Initialize shaded image attributes.
  */
  assert(image != (Image *) NULL);
  shade_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (shade_image == (Image *) NULL)
    return((Image *) NULL);
  shade_image->class=DirectClass;
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
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,Min(Max(y-1,0),image->rows-3),image->columns,3);
    q=SetImagePixels(shade_image,0,y,shade_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    /*
      Shade this row of pixels.
    */
    *q++=(*(p+image->columns));
    p++;
    s0=p;
    s1=p+image->columns;
    s2=p+2*image->columns;
    for (x=1; x < (int) (image->columns-1); x++)
    {
      /*
        Determine the surface normal and compute shading.
      */
      normal.x=Intensity(*(s0-1))+Intensity(*(s1-1))+Intensity(*(s2-1))-
        (double) Intensity(*(s0+1))-(double) Intensity(*(s1+1))-
        (double) Intensity(*(s2+1));
      normal.y=Intensity(*(s2-1))+Intensity(*s2)+Intensity(*(s2+1))-
        (double) Intensity(*(s0-1))-(double) Intensity(*s0)-
        (double) Intensity(*(s0+1));
      if ((normal.x == 0) && (normal.y == 0))
        shade=light.z;
      else
        {
          shade=0.0;
          distance=normal.x*light.x+normal.y*light.y+normal.z*light.z;
          if (distance > 0.0)
            {
              normal_distance=
                normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
              if (AbsoluteValue(normal_distance) > 0.0000001)
                shade=distance/sqrt(normal_distance);
            }
        }
      if (!color_shading)
        {
          q->red=shade;
          q->green=shade;
          q->blue=shade;
        }
      else
        {
          q->red=(shade*s1->red)/(MaxRGB+1);
          q->green=(shade*s1->green)/(MaxRGB+1);
          q->blue=(shade*s1->blue)/(MaxRGB+1);
        }
      q->opacity=s1->opacity;
      s0++;
      s1++;
      s2++;
      q++;
    }
    *q++=(*s1);
    if (!SyncImagePixels(shade_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(ShadeImageText,y,image->rows);
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
%  Method SharpenImage creates a new image that is a copy of an existing
%  one with the pixels sharpened.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the SharpenImage method is:
%
%      Image *SharpenImage(Image *image,const double factor,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o sharpen_image: Method SharpenImage returns a pointer to the image
%      after it is sharpened.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o factor:  An double value reflecting the percent weight to give to the
%      center pixel of the neighborhood.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *SharpenImage(Image *image,const unsigned int order,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *sharpen_image;

  int
    i;

  register int
    u,
    v;

  assert(image != (Image *) NULL);
  kernel=(double *) AllocateMemory(order*order*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to sharpen image",
      "Memory allocation failed");
  i=0;
  for (v=(-(int) order/2); v <= ((int) order/2); v++)
  {
    for (u=(-(int) order/2); u <= ((int) order/2); u++)
    {
      kernel[i]=(i % 2 ? -1 : 1)*exp(-(double) v*v-u*u);
      i++;
    }
  }
  kernel[i/2]=13.0/order;
  sharpen_image=ConvolveImage(image,order,kernel,exception);
  FreeMemory((void **) &kernel);
  return(sharpen_image);
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
%      void SolarizeImage(Image *image,const double factor)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o factor:  An double value that defines the extent of the solarization.
%
%
*/
Export void SolarizeImage(Image *image,const double factor)
{
#define SolarizeImageText  "  Solarizing the image colors...  "

  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    threshold;

  assert(image != (Image *) NULL);
  threshold=(unsigned int) (factor*(MaxRGB+1)/100.0);
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Solarize DirectClass packets.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          q->red=q->red > threshold ? MaxRGB-q->red : q->red;
          q->green=q->green > threshold ? MaxRGB-q->green : q->green;
          q->blue=q->blue > threshold ? MaxRGB-q->blue : q->blue;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(SolarizeImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Solarize PseudoClass packets.
      */
      for (i=0; i < (int) image->colors; i++)
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
%      Image *SpreadImage(Image *image,const unsigned int amount,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o spread_image: Method SpreadImage returns a pointer to the image
%      after it is spread.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o amount:  An unsigned value constraining the "vicinity" for choosing
%      a random pixel to swap.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *SpreadImage(Image *image,const unsigned int amount,
  ExceptionInfo *exception)
{
#define SpreadImageText  "  Spreading image...  "

  Image
    *spread_image;

  int
    quantum,
    y;

  long
    x_distance,
    y_distance;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  if ((image->columns < 3) || (image->rows < 3))
    return((Image *) NULL);
  /*
    Initialize spread image attributes.
  */
  spread_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (spread_image == (Image *) NULL)
    return((Image *) NULL);
  spread_image->class=DirectClass;
  /*
    Convolve each row.
  */
  quantum=(amount+1) >> 1;
  for (y=0; y < image->rows; y++)
  {
    q=SetImagePixels(spread_image,0,y,spread_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < image->columns; x++)
    {
      x_distance=(rand() & (amount+1))-quantum;
      y_distance=(rand() & (amount+1))-quantum;
      *q++=GetOnePixel(image,Min(x+x_distance,image->columns-1),
        Min(y+y_distance,image->rows-1));
    }
    if (!SyncImagePixels(spread_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SpreadImageText,y,image->rows);
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
%    o image: The address of a structure of type Image.
%
%    o watermark: The address of a structure of type Image.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *SteganoImage(Image *image,Image *watermark,
  ExceptionInfo *exception)
{
#define EmbedBit(byte) \
{ \
  q=GetImagePixels(watermark,j % watermark->columns,j/watermark->columns,1,1); \
  if (q == (PixelPacket *) NULL) \
    break;  \
  (byte)&=(~0x01); \
  (byte)|=(Intensity(*q) >> shift) & 0x01; \
  j++; \
  if (j == (watermark->columns*watermark->rows)) \
    { \
      j=0; \
      shift--; \
      if (shift < 0) \
        break; \
    } \
}
#define SteganoImageText  "  Hiding image...  "

  Image
    *stegano_image;

  int
    j,
    shift,
    y;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Initialize steganographic image attributes.
  */
  assert(image != (Image *) NULL);
  assert(watermark != (Image *) NULL);
  stegano_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (stegano_image == (Image *) NULL)
    return((Image *) NULL);
  if (stegano_image->class == PseudoClass)
    {
      if (stegano_image->colors > ((MaxRGB+1) >> 1))
        stegano_image->class=DirectClass;
      else
        {
          /*
            Shift colormap to make room for information hiding.
          */
          stegano_image->colors<<=1;
          stegano_image->colormap=(PixelPacket *) ReallocateMemory((char *)
            stegano_image->colormap,stegano_image->colors*sizeof(PixelPacket));
          if (stegano_image->colormap == (PixelPacket *) NULL)
            {
              DestroyImage(stegano_image);
              ThrowImageException(ResourceLimitWarning,
                "Unable to create steganograph image",
                "Memory allocation failed");
            }
          for (i=stegano_image->colors-1; i >= 0; i--)
            stegano_image->colormap[i]=stegano_image->colormap[i >> 1];
          for (y=0; y < (int) stegano_image->rows; y++)
          {
            q=GetImagePixels(stegano_image,0,y,stegano_image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(stegano_image);
            for (x=0; x < (int) stegano_image->columns; x++)
              indexes[x]*=2;
            if (!SyncImagePixels(stegano_image))
              break;
          }
        }
    }
  /*
    Hide watermark in low-order bits of image.
  */
  i=image->offset;
  j=0;
  shift=QuantumDepth-1;
  for (y=0; y < (int) image->rows; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      if (i == (stegano_image->columns*stegano_image->rows))
        i=0;
      p=GetImagePixels(stegano_image,i % stegano_image->columns,
        i/stegano_image->columns,1,1);
      if (p == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      if (stegano_image->class == PseudoClass)
        EmbedBit(*indexes)
      else
        {
          EmbedBit(p->red);
          EmbedBit(p->green);
          EmbedBit(p->blue);
        }
      if (!SyncImagePixels(stegano_image))
        break;
      i++;
    }
    if (shift < 0)
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SteganoImageText,y,image->rows);
  }
  if (stegano_image->class == PseudoClass)
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
%      Image *StereoImage(Image *image,Image *offset_image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o stereo_image: Method StereoImage returns a pointer to the stereo
%      image.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o offset_image: The address of a structure of type Image.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *StereoImage(Image *image,Image *offset_image,
  ExceptionInfo *exception)
{
#define StereoImageText  "  Stereo image...  "

  Image
    *stereo_image;

  int
    y;

  register int
    x;

  register PixelPacket
    *p,
    *q,
    *r;

  assert(image != (Image *) NULL);
  assert(offset_image != (Image *) NULL);
  if ((image->columns != offset_image->columns) ||
      (image->rows != offset_image->rows))
    ThrowImageException(ResourceLimitWarning,"Unable to create stereo image",
      "left and right image sizes differ");
  /*
    Initialize stereo image attributes.
  */
  stereo_image=
    CloneImage(image,image->columns,image->rows,False,exception);
  if (stereo_image == (Image *) NULL)
    return((Image *) NULL);
  stereo_image->class=DirectClass;
  /*
    Copy left image to red channel and right image to blue channel.
  */
  for (y=0; y < (int) stereo_image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=GetImagePixels(offset_image,0,y,offset_image->columns,1);
    r=SetImagePixels(stereo_image,0,y,stereo_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL) ||
        (r == (PixelPacket *) NULL))
      break;
    for (x=0; x < (int) stereo_image->columns; x++)
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
      ProgressMonitor(StereoImageText,y,stereo_image->rows);
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
%      Image *SwirlImage(Image *image,double degrees,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o swirl_image: Method SwirlImage returns a pointer to the image
%      after it is swirl.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o degrees:  An double value that defines the tightness of the swirling.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *SwirlImage(Image *image,double degrees,ExceptionInfo *exception)
{
#define SwirlImageText  "  Swirling image...  "

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

  int
    y;

  Image
    *swirl_image;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Initialize swirl image attributes.
  */
  assert(image != (Image *) NULL);
  if (!image->matte)
    MatteImage(image,Opaque);
  swirl_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (swirl_image == (Image *) NULL)
    return((Image *) NULL);
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
  for (y=0; y < image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(swirl_image,0,y,swirl_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    y_distance=y_scale*(y-y_center);
    for (x=0; x < image->columns; x++)
    {
      /*
        Determine if the pixel is within an ellipse.
      */
      *q=(*p);
      x_distance=x_scale*(x-x_center);
      distance=x_distance*x_distance+y_distance*y_distance;
      if (distance < (radius*radius))
        {
          /*
            Swirl the pixel.
          */
          factor=1.0-sqrt(distance)/radius;
          sine=sin(degrees*factor*factor);
          cosine=cos(degrees*factor*factor);
          *q=InterpolateColor(image,
            (cosine*x_distance-sine*y_distance)/x_scale+x_center,
            (sine*x_distance+cosine*y_distance)/y_scale+y_center);
        }
      p++;
      q++;
    }
    if (!SyncImagePixels(swirl_image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SwirlImageText,y,image->rows);
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
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o threshold: A double indicating the threshold value.
%
%
*/
Export unsigned int ThresholdImage(Image *image,const double threshold)
{
#define ThresholdImageText  "  Threshold the image...  "

  IndexPacket
    index;

  int
    y;

  PixelPacket
    *colormap;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  colormap=(PixelPacket *) AllocateMemory(2*sizeof(PixelPacket));
  if (colormap == (PixelPacket *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to threshold image",
      "Memory allocation failed");
  if (image->colormap != (PixelPacket *) NULL)
    FreeMemory((void **) &image->colormap);
  image->class=PseudoClass;
  image->colors=2;
  image->colormap=colormap;
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
    {
      index=Intensity(*q) < threshold ? 0 : 1;
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(ThresholdImageText,y,image->rows);
  }
  return(True);
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
%      Image *WaveImage(Image *image,const double amplitude,
%        const double wavelength,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o wave_image: Method WaveImage returns a pointer to the image after
%      it is waved.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o amplitude, frequency:  A double value that indicates the amplitude
%      and wavelength of the sine wave.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
Export Image *WaveImage(Image *image,const double amplitude,
  const double wavelength,ExceptionInfo *exception)
{
#define WaveImageText  "  Waving image...  "

  double
    *sine_map;

  Image
    *wave_image;

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  /*
    Initialize waved image attributes.
  */
  assert(image != (Image *) NULL);
  if (!image->matte)
    MatteImage(image,Opaque);
  wave_image=CloneImage(image,image->columns,image->rows+
    (int) (2*AbsoluteValue(amplitude)),False,exception);
  if (wave_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Allocate sine map.
  */
  sine_map=(double *) AllocateMemory(wave_image->columns*sizeof(double));
  if (sine_map == (double *) NULL)
    {
      DestroyImage(wave_image);
      ThrowImageException(ResourceLimitWarning,"Unable to wave image",
        "Memory allocation failed");
    }
  for (x=0; x < (int) wave_image->columns; x++)
    sine_map[x]=AbsoluteValue(amplitude)+amplitude*sin((2*M_PI*x)/wavelength);
  /*
    Wave image.
  */
  for (y=0; y < (int) wave_image->rows; y++)
  {
    q=SetImagePixels(wave_image,0,y,wave_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) wave_image->columns; x++)
    {
      *q=InterpolateColor(image,x,(int) (y-sine_map[x]));
      q++;
    }
    if (!SyncImagePixels(wave_image))
      break;
    if (QuantumTick(y,wave_image->rows))
      ProgressMonitor(WaveImageText,y,wave_image->rows);
  }
  FreeMemory((void **) &sine_map);
  return(wave_image);
}
