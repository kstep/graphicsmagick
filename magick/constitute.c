/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     CCCC   OOO   N   N  SSSSS  TTTTT  IIIII  TTTTT  U   U  TTTTT  EEEEE     %
%    C      O   O  NN  N  SS       T      I      T    U   U    T    E         %
%    C      O   O  N N N  ESSS     T      I      T    U   U    T    EEE       %
%    C      O   O  N  NN     SS    T      I      T    U   U    T    E         %
%     CCCC   OOO   N   N  SSSSS    T    IIIII    T     UUU     T    EEEEE     %
%                                                                             %
%                                                                             %
%                       Methods to Consitute an Image                         %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                               October 1998                                  %
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
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "cache.h"
#include "color.h"
#include "constitute.h"
#include "delegate.h"
#include "magick.h"
#include "monitor.h"
#include "stream.h"
#include "utility.h"

static SemaphoreInfo
  *constitute_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static Image
  *ReadImages(const ImageInfo *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t i t u t e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstituteImage() returns an image from the the pixel data you supply.
%  The pixel data must be in scanline order top-to-bottom.  The data can be
%  char, short int, int, float, or double.  Float and double require the
%  pixels to be normalized [0..1], otherwise [0..MaxRGB].  For example, to
%  create a 640x480 image from unsigned red-green-blue character data, use
%
%      image=ConstituteImage(640,480,"RGB",CharPixel,pixels,&exception);
%
%  The format of the Constitute method is:
%
%      Image *ConstituteImage(const unsigned long width,
%        const unsigned long height,const char *map,const StorageType type,
%        const void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o width: width in pixels of the image.
%
%    o height: height in pixels of the image.
%
%    o map: This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha, C = cyan, Y = yellow, M = magenta, K = black, or
%      I = intensity (for grayscale).
%
%    o type: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ConstituteImage(const unsigned long width,
  const unsigned long height,const char *map,const StorageType type,
  const void *pixels,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  PixelPacket
    *q;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  size_t
    length;

  /*
    Allocate image structure.
  */
  assert(pixels != (void *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  SetExceptionInfo(exception,UndefinedException);
  image=AllocateImage((ImageInfo *) NULL);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((width == 0) || (height == 0))
    ThrowBinaryException(OptionError,"UnableToConstituteImage",
      "NonzeroWidthAndHeightRequired");
  image->columns=width;
  image->rows=height;
  length=strlen(map);
  for (i=0; i < (long) length; i++)
    switch (map[i])
    {
      case 'a':
      case 'A':
      {
        image->matte=True;
        break;
      }
      case 'c':
      case 'C':
      case 'm':
      case 'M':
      case 'y':
      case 'Y':
      case 'k':
      case 'K':
      {
        image->colorspace=CMYKColorspace;
        break;
      }
      case 'i':
      case 'I':
      {
        if (!AllocateImageColormap(image,MaxColormapSize))
          ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
            "UnableToConstituteImage");
        break;
      }
      default:
        break;
    }
  /*
    Transfer the pixels from the pixel data array to the image.
  */
  switch (type)
  {
    case CharPixel:
    {
      register unsigned char
        *p;

      p=(unsigned char *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=ScaleCharToQuantum(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=ScaleCharToQuantum(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=ScaleCharToQuantum(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=ScaleCharToQuantum(*p++);
                break;
              }
              case 'I':
              {
                indexes[x]=ScaleCharToQuantum(*p++);
                q->red=image->colormap[indexes[x]].red;
                q->green=image->colormap[indexes[x]].green;
                q->blue=image->colormap[indexes[x]].blue;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              }
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
    case ShortPixel:
    {
      register unsigned short
        *p;

      p=(unsigned short *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=ScaleShortToQuantum(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=ScaleShortToQuantum(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=ScaleShortToQuantum(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=ScaleShortToQuantum(*p++);
                break;
              }
              case 'I':
              {
                indexes[x]=ScaleShortToQuantum(*p++);
                q->red=image->colormap[indexes[x]].red;
                q->green=image->colormap[indexes[x]].green;
                q->blue=image->colormap[indexes[x]].blue;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              }
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
    case IntegerPixel:
    {
      register unsigned int
        *p;

      p=(unsigned int *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=ScaleLongToQuantum(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=ScaleLongToQuantum(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=ScaleLongToQuantum(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=ScaleLongToQuantum(*p++);
                break;
              }
              case 'I':
              {
                indexes[x]=ScaleLongToQuantum(*p++);
                q->red=image->colormap[indexes[x]].red;
                q->green=image->colormap[indexes[x]].green;
                q->blue=image->colormap[indexes[x]].blue;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              }
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
    case LongPixel:
    {
      register unsigned long
        *p;

      p=(unsigned long *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=ScaleLongToQuantum(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(Quantum) (*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=ScaleLongToQuantum(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=ScaleLongToQuantum(*p++);
                break;
              }
              case 'I':
              {
                indexes[x]=ScaleLongToQuantum(*p++);
                q->red=image->colormap[indexes[x]].red;
                q->green=image->colormap[indexes[x]].green;
                q->blue=image->colormap[indexes[x]].blue;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              }
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
    case FloatPixel:
    {
      register float
        *p;

      p=(float *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(Quantum) ((float) MaxRGB*(*p++));
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(Quantum) ((float) MaxRGB*(*p++));
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(Quantum) ((float) MaxRGB*(*p++));
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(Quantum) ((float) MaxRGB*(*p++));
                break;
              }
              case 'I':
              {
                indexes[x]=(Quantum) ((float) MaxRGB*(*p++));
                q->red=image->colormap[indexes[x]].red;
                q->green=image->colormap[indexes[x]].green;
                q->blue=image->colormap[indexes[x]].blue;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              }
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
    case DoublePixel:
    {
      register double
        *p;

      p=(double *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(Quantum) ((double) MaxRGB*(*p++));
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(Quantum) ((double) MaxRGB*(*p++));
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(Quantum) ((double) MaxRGB*(*p++));
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(Quantum) ((double) MaxRGB*(*p++));
                break;
              }
              case 'I':
              {
                indexes[x]=(Quantum) ((double) MaxRGB*(*p++));
                q->red=image->colormap[indexes[x]].red;
                q->green=image->colormap[indexes[x]].green;
                q->blue=image->colormap[indexes[x]].blue;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              }
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
    default:
    {
      DestroyImage(image);
      ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
    }
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y C o n s t i t u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyConstitute() destroys the constitute environment.
%
%  The format of the DestroyConstitute method is:
%
%      DestroyConstitute(void)
%
%
*/
MagickExport void DestroyConstitute(void)
{
  AcquireSemaphoreInfo(&constitute_semaphore);
  DestroySemaphoreInfo(&constitute_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p a t c h I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DispatchImage() extracts pixel data from an image and returns it to you.
%  The method returns False on success otherwise True if an error is
%  encountered.  The data is returned as char, short int, int, long, float,
%  or double in the order specified by map.
%
%  Suppose we want want to extract the first scanline of a 640x480 image as
%  character data in red-green-blue order:
%
%      DispatchImage(image,0,0,640,1,"RGB",0,pixels,exception);
%
%  The format of the DispatchImage method is:
%
%      unsigned int DispatchImage(const Image *image,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType type,
%        void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x_offset, y_offset, columns, rows:  These values define the perimeter
%      of a region of pixels you want to extract.
%
%    o map:  This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha, C = cyan, Y = yellow, M = magenta, K = black, or
%      I = intensity (for grayscale).
%
%    o type: Define the data type of the pixels.  Float and double types are
%      normalized to [0..1] otherwise [0..MaxRGB].  Choose from these types:
%      CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel, or
%      DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int DispatchImage(const Image *image,const long x_offset,
  const long y_offset,const unsigned long columns,const unsigned long rows,
  const char *map,const StorageType type,void *pixels,ExceptionInfo *exception)
{
  long
    y;

  register long
    i,
    x;

  register const PixelPacket
    *p;

  size_t
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  length=strlen(map);
  for (i=0; i < (long) length; i++)
    switch (map[i])
    {
      case 'c':
      case 'C':
      case 'm':
      case 'M':
      case 'y':
      case 'Y':
      case 'k':
      case 'K':
      {
        if (image->colorspace == CMYKColorspace)
          break;
        ThrowException(exception,OptionError,"ColorSeparatedImageRequired",map);
        return(False);
      }
      default:
        break;
    }
  switch (type)
  {
    case CharPixel:
    {
      register unsigned char
        *q;

      q=(unsigned char *) pixels;
      for (y=0; y < (long) rows; y++)
      {
        p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=ScaleQuantumToChar(p->red);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=ScaleQuantumToChar(p->green);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=ScaleQuantumToChar(p->blue);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=ScaleQuantumToChar(p->opacity);
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=ScaleQuantumToChar(PixelIntensityToQuantum(p));
                break;
              }
              default:
              {
                ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
                return(False);
              }
            }
          }
          p++;
        }
      }
      break;
    }
    case ShortPixel:
    {
      register unsigned short
        *q;

      q=(unsigned short *) pixels;
      for (y=0; y < (long) rows; y++)
      {
        p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=ScaleQuantumToShort(p->red);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=ScaleQuantumToShort(p->green);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=ScaleQuantumToShort(p->blue);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=ScaleQuantumToShort(p->opacity);
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=ScaleQuantumToShort(PixelIntensityToQuantum(p));
                break;
              }
              default:
              {
                ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
                return(False);
              }
            }
          }
          p++;
        }
      }
      break;
    }
    case IntegerPixel:
    {
      register unsigned int
        *q;

      q=(unsigned int *) pixels;
      for (y=0; y < (long) rows; y++)
      {
        p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=ScaleQuantumToLong(p->red);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=ScaleQuantumToLong(p->green);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=ScaleQuantumToLong(p->blue);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=ScaleQuantumToLong(p->opacity);
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=ScaleQuantumToLong(PixelIntensityToQuantum(p));
                break;
              }
              default:
              {
                ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
                return(False);
              }
            }
          }
          p++;
        }
      }
      break;
    }
    case LongPixel:
    {
      register unsigned long
        *q;

      q=(unsigned long *) pixels;
      for (y=0; y < (long) rows; y++)
      {
        p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=ScaleQuantumToLong(p->red);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=ScaleQuantumToLong(p->green);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=ScaleQuantumToLong(p->blue);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=ScaleQuantumToLong(p->opacity);
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=ScaleQuantumToLong(PixelIntensityToQuantum(p));
                break;
              }
              default:
              {
                ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
                return(False);
              }
            }
          }
          p++;
        }
      }
      break;
    }
    case FloatPixel:
    {
      register float
        *q;

      q=(float *) pixels;
      for (y=0; y < (long) rows; y++)
      {
        p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=(float) p->red/MaxRGB;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=(float) p->green/MaxRGB;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=(float) p->blue/MaxRGB;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=(float) p->opacity/MaxRGB;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=(float) PixelIntensityToQuantum(p)/MaxRGB;
                break;
              }
              default:
              {
                ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
                return(False);
              }
            }
          }
          p++;
        }
      }
      break;
    }
    case DoublePixel:
    {
      register double
        *q;

      q=(double *) pixels;
      for (y=0; y < (long) rows; y++)
      {
        p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=(double) p->red/MaxRGB;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=(double) p->green/MaxRGB;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=(double) p->blue/MaxRGB;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=(double) p->opacity/MaxRGB;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=(double) PixelIntensityToQuantum(p)/MaxRGB;
                break;
              }
              default:
              {
                ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
                return(False);
              }
            }
          }
          p++;
        }
      }
      break;
    }
    default:
    {
      ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
      return(False);
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i n g I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PingImage() returns all the attributes of an image or image sequence
%  except for the pixels.  It is much faster and consumes far less memory
%  than ReadImage().  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the PingImage method is:
%
%      Image *PingImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Ping the image defined by the file or filename members of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static unsigned int PingStream(const Image *image,const void *pixels,
  const size_t columns)
{
  return(True);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport Image *PingImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  clone_info=CloneImageInfo(image_info);
  clone_info->ping=True;
  image=ReadStream(clone_info,&PingStream,exception);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P o p I m a g e P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PopImagePixels() transfers one or more pixel components from the image pixel
%  cache to a user supplied buffer.   True is returned if the pixels are
%  successfully transferred, otherwise False.
%
%  The format of the PopImagePixels method is:
%
%      unsigned int PopImagePixels(const Image *,const QuantumType quantum,
%        unsigned char *destination)
%
%  A description of each parameter follows:
%
%    o status: Method PopImagePixels returns True if the pixels are
%      successfully transferred, otherwise False.
%
%    o image: The image.
%
%    o quantum: Declare which pixel components to transfer (RGB, RGBA, etc).
%
%    o destination:  The components are transferred to this buffer.
%
%
*/
MagickExport unsigned int PopImagePixels(const Image *image,
  const QuantumType quantum,unsigned char *destination)
{
  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  unsigned long
    number_pixels,
    pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(destination != (unsigned char *) NULL);
  number_pixels=GetPixelCacheArea(image);
  p=GetPixels(image);
  indexes=GetIndexes(image);
  q=destination;
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) number_pixels; x++)
            *q++=(Quantum) indexes[x];
          break;
        }
      if (image->colors <= 65536L)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            *q++=(Quantum) (indexes[x] >> 8);
            *q++=(Quantum) indexes[x];
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        *q++=(Quantum) (indexes[x] >> 24);
        *q++=(Quantum) (indexes[x] >> 16);
        *q++=(Quantum) (indexes[x] >> 8);
        *q++=(Quantum) indexes[x];
      }
      break;
    }
    case IndexAlphaQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            *q++=(Quantum) indexes[x];
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->colors <= 65536L)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            *q++=(Quantum) (indexes[x] >> 8);
            *q++=(Quantum) indexes[x];
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        *q++=(Quantum) (indexes[x] >> 24);
        *q++=(Quantum) (indexes[x] >> 16);
        *q++=(Quantum) (indexes[x] >> 8);
        *q++=(Quantum) indexes[x];
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case GrayQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(PixelIntensityToQuantum(p));
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(PixelIntensityToQuantum(p));
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(PixelIntensityToQuantum(p));
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case GrayAlphaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(PixelIntensityToQuantum(p));
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(PixelIntensityToQuantum(p));
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(PixelIntensityToQuantum(p));
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
         for (x=0; x < (long) number_pixels; x++)
         {
           pixel=ScaleQuantumToShort(p->red);
           *q++=(Quantum) (pixel >> 8);
           *q++=(Quantum) pixel;
           p++;
         }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->green);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->green);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->green);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case AlphaQuantum:
    {
      if (image->colorspace == CMYKColorspace)
        {
          if (image->depth <= 8)
            {
              for (x=0; x < (long) number_pixels; x++)
              {
                pixel=ScaleQuantumToChar(MaxRGB-indexes[x]);
                *q++=(Quantum) pixel;
                p++;
              }
              break;
            }
          if (image->depth <= 16)
            {
              for (x=0; x < (long) number_pixels; x++)
              {
                pixel=ScaleQuantumToShort(MaxRGB-indexes[x]);
                *q++=(Quantum) (pixel >> 8);
                *q++=(Quantum) pixel;
                p++;
              }
              break;
            }
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToLong(MaxRGB-indexes[x]);
            *q++=(Quantum) (pixel >> 24);
            *q++=(Quantum) (pixel >> 16);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->opacity);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case RGBAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->opacity);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
    case CMYKAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(p->opacity);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToChar(MaxRGB-indexes[x]);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(p->opacity);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            pixel=ScaleQuantumToShort(MaxRGB-indexes[x]);
            *q++=(Quantum) (pixel >> 8);
            *q++=(Quantum) pixel;
            p++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(p->opacity);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        pixel=ScaleQuantumToLong(MaxRGB-indexes[x]);
        *q++=(Quantum) (pixel >> 24);
        *q++=(Quantum) (pixel >> 16);
        *q++=(Quantum) (pixel >> 8);
        *q++=(Quantum) pixel;
        p++;
      }
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P u s h I m a g e P i x e l s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PushImagePixels() transfers one or more pixel components from a user
%  supplied buffer into the image pixel cache of an image.  It returns True if
%  the pixels are successfully transferred, otherwise False.
%
%  The format of the PushImagePixels method is:
%
%      unsigned int PushImagePixels(Image *image,const QuantumType quantum,
%        const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns True if the pixels are
%      successfully transferred, otherwise False.
%
%    o image: The image.
%
%    o quantum: Declare which pixel components to transfer (red, green, blue,
%      opacity, RGB, or RGBA).
%
%    o source:  The pixel components are transferred from this buffer.
%
*/

static inline IndexPacket PushColormapIndex(Image *image,
  const unsigned long index)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (index < image->colors)
    return((IndexPacket) index);
  ThrowException(&image->exception,CorruptImageError,"InvalidColormapIndex",
    image->filename);
  return(0);
}

MagickExport unsigned int PushImagePixels(Image *image,
  const QuantumType quantum,const unsigned char *source)
{
  register const unsigned char
    *p;

  register IndexPacket
    index,
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned long
    number_pixels,
    pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(source != (const unsigned char *) NULL);
  number_pixels=GetPixelCacheArea(image);
  p=source;
  q=GetPixels(image);
  indexes=GetIndexes(image);
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          for (x = (long) number_pixels; x > 0; --x)
          {
            index=(IndexPacket) (*p++);
            VerifyColormapIndex(image,index);
            *indexes++=index;
            *q++=image->colormap[index];
          }
          break;
        }
      if (image->colors <= 65536L)
        {
          for ( x= (long) number_pixels; x > 0; --x)
          {
            index=(*p << 8) | *(p+1);
            p+=2;
            VerifyColormapIndex(image,index);
            *indexes++=index;
            *q++=image->colormap[index];
          }
          break;
        }
      for (x = (long) number_pixels; x > 0; --x)
      {
        index=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        VerifyColormapIndex(image,index);
        *indexes++=index;
        *q++=image->colormap[index];
      }
      break;
    }
    case IndexAlphaQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            indexes[x]=PushColormapIndex(image,pixel);
            *q=image->colormap[indexes[x]];
            pixel=(*p++);
            q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
            q++;
          }
          break;
        }
      if (image->colors <= 65536L)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            indexes[x]=PushColormapIndex(image,pixel);
            *q=image->colormap[indexes[x]];
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(65535L-pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        indexes[x]=PushColormapIndex(image,pixel);
        *q=image->colormap[indexes[x]];
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(ScaleQuantumToLong(MaxRGB)-pixel);
        q++;
      }
      break;
    }
    case GrayQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            indexes[x]=PushColormapIndex(image,pixel);
            *q++=image->colormap[indexes[x]];
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            indexes[x]=PushColormapIndex(image,pixel);
            *q++=image->colormap[indexes[x]];
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        indexes[x]=PushColormapIndex(image,pixel);
        *q++=image->colormap[indexes[x]];
      }
      break;
    }
    case GrayAlphaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            indexes[x]=PushColormapIndex(image,pixel);
            *q=image->colormap[indexes[x]];
            pixel=(*p++);
            q->opacity=(Quantum) (MaxRGB-pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            indexes[x]=PushColormapIndex(image,pixel);
            *q=image->colormap[indexes[x]];
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(65535L-pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        indexes[x]=PushColormapIndex(image,pixel);
        *q=image->colormap[indexes[x]];
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(ScaleQuantumToLong(MaxRGB)-pixel);
        q++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->red=ScaleCharToQuantum(pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->red=ScaleShortToQuantum(pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->red=ScaleLongToQuantum(pixel);
        q++;
      }
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->green=ScaleCharToQuantum(pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->green=ScaleShortToQuantum(pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->green=ScaleLongToQuantum(pixel);
        q++;
      }
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->blue=ScaleCharToQuantum(pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->blue=ScaleShortToQuantum(pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->blue=ScaleLongToQuantum(pixel);
        q++;
      }
      break;
    }
    case AlphaQuantum:
    {
      if (image->colorspace == CMYKColorspace)
        {
          if (image->depth <= 8)
            {
              for (x=0; x < (long) number_pixels; x++)
              {
                pixel=(*p++);
                indexes[x]=ScaleCharToQuantum(pixel);
              }
              break;
            }
          if (image->depth <= 16)
            {
              for (x=0; x < (long) number_pixels; x++)
              {
                pixel=(*p << 8) | *(p+1);
                p+=2;
                indexes[x]=ScaleShortToQuantum(pixel);
              }
              break;
            }
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
            p+=4;
            indexes[x]=ScaleLongToQuantum(pixel);
          }
          break;
        }
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(65535L-pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(ScaleQuantumToLong(MaxRGB)-pixel);
        q++;
      }
      break;
    }
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->opacity=ScaleCharToQuantum(pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(pixel);
        q++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->red=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->green=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->blue=ScaleCharToQuantum(pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->red=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->green=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->blue=ScaleShortToQuantum(pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->red=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->green=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->blue=ScaleLongToQuantum(pixel);
        q++;
      }
      break;
    }
    case RGBAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->red=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->green=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->blue=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->red=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->green=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->blue=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(65535L-pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->red=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->green=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->blue=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(ScaleQuantumToLong(MaxRGB)-pixel);
        q++;
      }
      break;
    }
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->red=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->green=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->blue=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->opacity=ScaleCharToQuantum(pixel);
            q++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->red=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->green=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->blue=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->red=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->green=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->blue=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(pixel);
        q++;
      }
      break;
    }
    case CMYKAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p++);
            q->red=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->green=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->blue=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            q->opacity=ScaleCharToQuantum(pixel);
            pixel=(*p++);
            indexes[x]=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
            q++;
          }
          break;
        }
      if (image->depth <= 8)
        {
          for (x=0; x < (long) number_pixels; x++)
          {
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->red=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->green=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->blue=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            q->opacity=ScaleShortToQuantum(pixel);
            pixel=(*p << 8) | *(p+1);
            p+=2;
            indexes[x]=ScaleShortToQuantum(65535L-pixel);
            q++;
          }
          break;
        }
      for (x=0; x < (long) number_pixels; x++)
      {
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->red=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->green=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->blue=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        q->opacity=ScaleLongToQuantum(pixel);
        pixel=(*p << 24) | (*(p+1) << 16) | (*(p+2) << 8) | *(p+3);
        p+=4;
        indexes[x]=ScaleLongToQuantum(ScaleQuantumToLong(MaxRGB)-pixel);
        q++;
      }
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadImage() reads an image or image sequence from a file or file handle.
%  The method returns a NULL if there is a memory shortage or if the image
%  cannot be read.  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the ReadImage method is:
%
%      Image *ReadImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Read the image defined by the file or filename members of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReadImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent];

  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  Image
    *image,
    *next;

  ImageInfo
    *clone_info;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  if (*image_info->filename == '@')
    return(ReadImages(image_info,exception));
  clone_info=CloneImageInfo(image_info);
  (void) SetImageInfo(clone_info,False,exception);
  (void) strncpy(filename,clone_info->filename,MaxTextExtent-1);
  /*
    Call appropriate image reader based on image type.
  */
  magick_info=GetMagickInfo(clone_info->magick,exception);
  if ((magick_info != (const MagickInfo *) NULL) &&
      magick_info->seekable_stream)
    {
      unsigned int
        status;

      image=AllocateImage(clone_info);
      if (image == (Image *) NULL)
        return(False);
      (void) strncpy(image->filename,clone_info->filename,MaxTextExtent-1);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
        {
          DestroyImage(image);
          return(False);
        }
      if ((GetBlobStreamType(image) != FileStream) &&
          (GetBlobStreamType(image) != BlobStream))
        {
          /*
            Coder requires a random access stream.
          */
          TemporaryFilename(clone_info->filename);
          (void) ImageToFile(image,clone_info->filename,exception);
          clone_info->temporary=True;
        }
      CloseBlob(image);
      DestroyImage(image);
    }
  image=(Image *) NULL;
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->decoder != NULL))
    {
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  else
    {
      delegate_info=GetDelegateInfo(clone_info->magick,(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          if (IsAccessible(clone_info->filename))
            ThrowException(exception,MissingDelegateError,
              "NoDecodeDelegateForThisImageFormat",clone_info->filename);
          else
            ThrowException(exception,FileOpenError,"UnableToOpenFile",
              clone_info->filename);
          if (clone_info->temporary)
            (void) remove(clone_info->filename);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      /*
        Let our decoding delegate process the image.
      */
      image=AllocateImage(clone_info);
      if (image == (Image *) NULL)
        {
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      (void) strncpy(image->filename,clone_info->filename,MaxTextExtent-1);
      TemporaryFilename(clone_info->filename);
      (void) InvokeDelegate(clone_info,image,clone_info->magick,(char *) NULL,
        exception);
      DestroyImageList(image);
      image=(Image *) NULL;
      clone_info->temporary=True;
      (void) SetImageInfo(clone_info,False,exception);
      magick_info=GetMagickInfo(clone_info->magick,exception);
      if ((magick_info == (const MagickInfo *) NULL) ||
          (magick_info->decoder == NULL))
        {
          if (IsAccessible(clone_info->filename))
            ThrowException(exception,MissingDelegateError,
              "NoDecodeDelegateForThisImageFormat",clone_info->filename);
          else
            ThrowException(exception,FileOpenError,"UnableToOpenFile",
              clone_info->filename);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  if (clone_info->temporary)
    {
      (void) remove(clone_info->filename);
      clone_info->temporary=False;
      if (image != (Image *) NULL)
        (void) strncpy(image->filename,filename,MaxTextExtent-1);
    }
  if (image == (Image *) NULL)
    {
      DestroyImageInfo(clone_info);
      return(image);
    }
  if (image->blob->temporary)
    (void) remove(clone_info->filename);
  if ((image->next != (Image *) NULL) && IsSubimage(clone_info->tile,False))
    {
      char
        *q;

      Image
        *clone_image,
        *subimages;

      long
        quantum;

      register char
        *p;

      register long
        i;

      unsigned long
        first,
        last;

      /*
        User specified subimages (e.g. image.miff[1,3-5,7-6,2]).
      */
      subimages=NewImageList();
      p=clone_info->tile;
      for (q=p; *q != '\0'; p++)
      {
        while (isspace((int) *p) || (*p == ','))
          p++;
        first=strtol(p,&q,10);
        last=first;
        while (isspace((int) *q))
          q++;
        if (*q == '-')
          last=strtol(q+1,&q,10);
        quantum=first > last ? -1 : 1;
        for (p=q; first != (last+quantum); first+=quantum)
        {
          i=0;
          for (next=image; next != (Image *) NULL; next=next->next)
          {
            if (next->scene != 0)
              i=(long) next->scene;
            if (i != (long) first)
              {
                i++;
                continue;
              }
            clone_image=CloneImage(next,0,0,True,exception);
            if (clone_image == (Image *) NULL)
              break;
            AppendImageToList(&subimages,clone_image);
            i++;
          }
        }
      }
      if (subimages == (Image *) NULL)
        ThrowException(exception,OptionError,
          "SubimageSpecificationReturnsNoImages",clone_info->filename);
      else
        {
          while (subimages->previous != (Image *) NULL)
            subimages=subimages->previous;
          DestroyImageList(image);
          image=subimages;
        }
    }
  if (image->blob->status)
    {
      ThrowException(exception,CorruptImageError,
        "AnErrorHasOccurredReadingFromFile",clone_info->filename);
      DestroyImageInfo(clone_info);
      return((Image *) NULL);
    }
  for (next=image; next; next=next->next)
  {
    next->taint=False;
    (void) strncpy(next->magick_filename,filename,MaxTextExtent-1);
    if (image->blob->temporary)
      (void) strncpy(next->filename,filename,MaxTextExtent-1);
    if (next->magick_columns == 0)
      next->magick_columns=next->columns;
    if (next->magick_rows == 0)
      next->magick_rows=next->rows;
  }
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d I m a g e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadImages() reads a list of image names from a file and then returns the
%  images as a linked list.
%
%  The format of the ReadImage method is:
%
%      Image *ReadImages(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: The list of filenames are defined in the filename member of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static Image *ReadImages(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *command,
    **images;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    number_images;

  register Image
    *next;

  register long
    i;

  size_t
    length;

  /*
    Read image list from a file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  command=(char *) FileToBlob(image_info->filename+1,&length,exception);
  if (command == (char *) NULL)
    return((Image *) NULL);
  Strip(command);
  images=StringToArgv(command,&number_images);
  LiberateMemory((void **) &command);
  /*
    Read the images into a linked list.
  */
  image=(Image *) NULL;
  clone_info=CloneImageInfo(image_info);
  for (i=1; i < number_images; i++)
  {
    (void) strncpy(clone_info->filename,images[i],MaxTextExtent-1);
    next=ReadImage(clone_info,exception);
    if (next == (Image *) NULL)
      continue;
    if (image == (Image *) NULL)
      image=next;
    else
      {
        register Image
          *q;

        /*
          Link image into image list.
        */
        for (q=image; q->next != (Image *) NULL; q=q->next);
        next->previous=q;
        q->next=next;
      }
  }
  DestroyImageInfo(clone_info);
  for (i=1; i < number_images; i++)
    LiberateMemory((void **) &images[i]);
  LiberateMemory((void **) &images);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I n l i n e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadInlineImage() reads a Base64-encoded inline image or image sequence.
%  The method returns a NULL if there is a memory shortage or if the image
%  cannot be read.  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the ReadInlineImage method is:
%
%      Image *ReadInlineImage(const ImageInfo *image_info,const char *content,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o content: The image encoded in Base64.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReadInlineImage(const ImageInfo *image_info,
  const char *content,ExceptionInfo *exception)
{
  Image
    *image;

  MonitorHandler
    handler;

  unsigned char
    *blob;

  size_t
    length;

  register const char
    *p;

  SetExceptionInfo(exception,UndefinedException);
  image=(Image *) NULL;
  for (p=content; (*p != ',') && (*p != '\0'); p++);
  if (*p == '\0')
    ThrowReaderException(CorruptImageWarning,"CorruptInlineImage",image);
  p++;
  blob=Base64Decode(p,&length);
  if (length == 0)
    ThrowReaderException(CorruptImageWarning,"CorruptInlineImage",image);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  image=BlobToImage(image_info,blob,length,exception);
  (void) SetMonitorHandler(handler);
  LiberateMemory((void **) &blob);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use Write() to write an image or an image sequence to a file or
%  filehandle.  If writing to a file on disk, the name is defined by the
%  filename member of the image structure.  Write() returns 0 is there is a
%  memory shortage or if the image cannot be written.  Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the WriteImage method is:
%
%      unsigned int WriteImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int WriteImage(const ImageInfo *image_info,Image *image)
{
  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  unsigned int
    status;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  clone_info=CloneImageInfo(image_info);
  (void) strncpy(clone_info->filename,image->filename,MaxTextExtent-1);
  (void) strncpy(clone_info->magick,image->magick,MaxTextExtent-1);
  (void) SetImageInfo(clone_info,True,&image->exception);
  (void) strncpy(image->filename,clone_info->filename,MaxTextExtent-1);
  if (((image->next == (Image *) NULL) || clone_info->adjoin) &&
      (image->previous == (Image *) NULL) &&
      (clone_info->page == (char *) NULL) && !IsTaintImage(image))
    {
      delegate_info=GetDelegateInfo(image->magick,clone_info->magick,
        &image->exception);
      if ((delegate_info != (const DelegateInfo *) NULL) &&
          (delegate_info->mode == 0) && IsAccessible(image->magick_filename))
        {
          /*
            Let our bi-modal delegate process the image.
          */
          (void) strncpy(image->filename,image->magick_filename,
            MaxTextExtent-1);
          status=InvokeDelegate(clone_info,image,image->magick,
            clone_info->magick,&image->exception);
          DestroyImageInfo(clone_info);
          return(!status);
        }
      }
  /*
    Call appropriate image writer based on image type.
  */
  status=False;
  magick_info=GetMagickInfo(clone_info->magick,&image->exception);
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->encoder != NULL))
    {
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      status=(magick_info->encoder)(clone_info,image);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  else
    {
      delegate_info=GetDelegateInfo((char *) NULL,clone_info->magick,
        &image->exception);
      if (delegate_info != (DelegateInfo *) NULL)
        {
          /*
            Let our encoding delegate process the image.
          */
          TemporaryFilename(image->filename);
          status=InvokeDelegate(clone_info,image,(char *) NULL,
            clone_info->magick,&image->exception);
          (void) remove(image->filename);
          DestroyImageInfo(clone_info);
          return(!status);
        }
      magick_info=GetMagickInfo(clone_info->magick,&image->exception);
      if (!clone_info->affirm && (magick_info == (const MagickInfo *) NULL))
        magick_info=(MagickInfo *)
          GetMagickInfo(image->magick,&image->exception);
      if ((magick_info == (MagickInfo *) NULL) ||
          (magick_info->encoder == NULL))
        {
          DestroyImageInfo(clone_info);
          ThrowBinaryException(MissingDelegateError,
            "NoEncodeDelegateForThisImageFormat",image->filename)
        }
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      status=(magick_info->encoder)(clone_info,image);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  (void) strncpy(image->magick,clone_info->magick,MaxTextExtent-1);
  DestroyImageInfo(clone_info);
  if (image->blob->status)
    ThrowBinaryException(CorruptImageError,"AnErrorHasOccurredWritingToFile",
      image->filename);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteImages() writes an image sequence.
%
%  The format of the WriteImages method is:
%
%      unsigned int WriteImages(const ImageInfo *image_info,Image *image,
%        const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o images: The image list.
%
%    o filename: The image filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int WriteImages(ImageInfo *image_info,Image *image,
  const char *filename,ExceptionInfo *exception)
{
  register Image
    *p;

  unsigned int
    status;

  /*
    Write converted images.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (filename != (const char *) NULL)
    {
      (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
      for (p=image; p != (Image *) NULL; p=p->next)
        (void) strncpy(p->filename,filename,MaxTextExtent-1);
    }
  (void) SetImageInfo(image_info,True,exception);
  status=True;
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    status&=WriteImage(image_info,p);
    if(p->exception.severity > exception->severity)
      ThrowException(exception,p->exception.severity,
        p->exception.reason,p->exception.description);
    (void) CatchImageException(p);
    if (image_info->adjoin)
      break;
  }
  if (image_info->verbose)
    DescribeImage(image,stdout,False);
  return(status);
}
