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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

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

  register long
    i,
    x;

  /*
    Allocate image structure.
  */
  assert(pixels != (void *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  GetExceptionInfo(exception);
  image=AllocateImage((ImageInfo *) NULL);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((width*height) == 0)
    ThrowBinaryException(OptionWarning,"Unable to create image",
      "impossible image size");
  image->columns=width;
  image->rows=height;
  for (i=0; i < (long) strlen(map); i++)
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
        if (!AllocateImageColormap(image,MaxRGB+1))
          ThrowImageException(ResourceLimitWarning,"Unable to constitute image",
            "Memory allocation failed");
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
      register char
        *p;

      p=(char *) pixels;
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=UpScale(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=UpScale(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=UpScale(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=UpScale(*p++);
                break;
              }
              case 'I':
              {
                q->red=UpScale(*p++);
                q->green=q->red;
                q->blue=q->red;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(*p++);
                break;
              }
              case 'I':
              {
                q->red=(*p++);
                q->green=q->red;
                q->blue=q->red;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(*p++);
                break;
              }
              case 'I':
              {
                q->red=(*p++);
                q->green=q->red;
                q->blue=q->red;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(*p++);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(*p++);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(*p++);
                break;
              }
              case 'I':
              {
                q->red=(*p++);
                q->green=q->red;
                q->blue=q->red;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'I':
              {
                q->red=(Quantum) (MaxRGB*(*p++));
                q->green=q->red;
                q->blue=q->red;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
        for (x=0; x < (long) image->columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                q->red=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                q->green=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                q->blue=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                q->opacity=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'I':
              {
                q->red=(Quantum) (MaxRGB*(*p++));
                q->green=q->red;
                q->blue=q->red;
                break;
              }
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
      ThrowImageException(OptionWarning,"Invalid pixel map",map)
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
%  Method DestroyConstitute destroys the constitute environment.
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
  DestroySemaphoreInfo(constitute_semaphore);
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

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register const PixelPacket
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (i=0; i < (long) strlen(map); i++)
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
        ThrowException(exception,OptionWarning,
          "color separation image type required",map);
        return(False);
      }
      case 'i':
      case 'I':
      {
        if (image->storage_class == PseudoClass)
          break;
        ThrowException(exception,OptionWarning,"palette image type required",
          map);
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
        indexes=GetIndexes(image);
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=(unsigned char) DownScale(p->red);
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=(unsigned char) DownScale(p->green);
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=(unsigned char) DownScale(p->blue);
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=(unsigned char) DownScale(p->opacity);
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=(unsigned char) indexes[x];
                break;
              }
              default:
              {
                ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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
        indexes=GetIndexes(image);
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=p->red;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=p->green;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=p->blue;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=p->opacity;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=indexes[x];
                break;
              }
              default:
              {
                ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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
        indexes=GetIndexes(image);
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=p->red;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=p->green;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=p->blue;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=p->opacity;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=indexes[x];
                break;
              }
              default:
              {
                ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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
        indexes=GetIndexes(image);
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=p->red;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=p->green;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=p->blue;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=p->opacity;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=indexes[x];
                break;
              }
              default:
              {
                ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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
        indexes=GetIndexes(image);
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=p->red;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=p->green;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=p->blue;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=p->opacity;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=indexes[x];
                break;
              }
              default:
              {
                ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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
        indexes=GetIndexes(image);
        for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) strlen(map); i++)
          {
            switch (map[i])
            {
              case 'r':
              case 'R':
              case 'c':
              case 'C':
              {
                *q++=p->red;
                break;
              }
              case 'g':
              case 'G':
              case 'm':
              case 'M':
              {
                *q++=p->green;
                break;
              }
              case 'b':
              case 'B':
              case 'y':
              case 'Y':
              {
                *q++=p->blue;
                break;
              }
              case 'a':
              case 'A':
              case 'k':
              case 'K':
              {
                *q++=p->opacity;
                break;
              }
              case 'i':
              case 'I':
              {
                *q++=indexes[x];
                break;
              }
              default:
              {
                ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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
      ThrowException(exception,OptionWarning,"Invalid pixel map",map);
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

static int StreamHandler(const Image *image,const void *pixels,
  const size_t columns)
{
  return(True);
}

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
  GetExceptionInfo(exception);
  clone_info=CloneImageInfo(image_info);
  if (clone_info->size == (char *) NULL)
    clone_info->size=AllocateString(DefaultTileGeometry);
  image=ReadStream(clone_info,&StreamHandler,exception);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P o p I m a g e P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PopImagePixels transfers one or more pixel components from the image
%  pixel cache to a user supplied buffer.   It returns True if the pixels are
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
%    o quantum: Declare which pixel components to transfer (red, green, blue,
%      opacity, RGB, or RGBA).
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

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(destination != (unsigned char *) NULL);
  p=GetPixels(image);
  indexes=GetIndexes(image);
  q=destination;
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) image->columns; x++)
            *q++=indexes[x];
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=indexes[x] >> 8;
        *q++=indexes[x];
      }
      break;
    }
    case IndexAlphaQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=indexes[x];
            *q++=DownScale(MaxRGB-p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=indexes[x] >> 8;
        *q++=indexes[x];
        *q++=(MaxRGB-p->opacity) >> 8;
        *q++=MaxRGB-p->opacity;
        p++;
      }
      break;
    }
    case GrayQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=(unsigned char) DownScale(Intensity(*p));
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=(unsigned char) (Intensity(*p) >> 8);
        *q++=(unsigned char) Intensity(*p);
        p++;
      }
      break;
    }
    case GrayAlphaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=(unsigned char) DownScale(Intensity(*p));
            *q++=DownScale(MaxRGB-p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=(unsigned char) (Intensity(*p) >> 8);
        *q++=(unsigned char) Intensity(*p);
        *q++=(MaxRGB-p->opacity) >> 8;
        *q++=MaxRGB-p->opacity;
        p++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->red);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red;
        p++;
      }
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->green);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->green >> 8;
        *q++=p->green;
        p++;
      }
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->blue);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->blue >> 8;
        *q++=p->blue;
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
              for (x=0; x < (long) image->columns; x++)
              {
                *q++=DownScale(MaxRGB-indexes[x]);
                p++;
              }
              break;
            }
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=(MaxRGB-indexes[x]) >> 8;
            *q++=MaxRGB-indexes[x];
            p++;
          }
          break;
        }
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(MaxRGB-p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=(MaxRGB-p->opacity) >> 8;
        *q++=MaxRGB-p->opacity;
        p++;
      }
      break;
    }
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=(p->opacity) >> 8;
        *q++=p->opacity;
        p++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red;
        *q++=p->green >> 8;
        *q++=p->green;
        *q++=p->blue >> 8;
        *q++=p->blue;
        p++;
      }
      break;
    }
    case RGBAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            *q++=DownScale(MaxRGB-p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red;
        *q++=p->green >> 8;
        *q++=p->green;
        *q++=p->blue >> 8;
        *q++=p->blue;
        *q++=(MaxRGB-p->opacity) >> 8;
        *q++=MaxRGB-p->opacity;
        p++;
      }
      break;
    }
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            *q++=DownScale(p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red;
        *q++=p->green >> 8;
        *q++=p->green;
        *q++=p->blue >> 8;
        *q++=p->blue;
        *q++=p->opacity >> 8;
        *q++=p->opacity;
        p++;
      }
      break;
    }
    case CMYKAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            *q++=DownScale(p->opacity);
            *q++=DownScale(indexes[x]);
            p++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red;
        *q++=p->green >> 8;
        *q++=p->green;
        *q++=p->blue >> 8;
        *q++=p->blue;
        *q++=p->opacity >> 8;
        *q++=p->opacity;
        *q++=indexes[x] >> 8;
        *q++=indexes[x];
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
+   P u s h I m a g e P i x e l s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PushImagePixels transfers one or more pixel components from a user
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
MagickExport unsigned int PushImagePixels(Image *image,
  const QuantumType quantum,const unsigned char *source)
{
  IndexPacket
    index;

  register const unsigned char
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(source != (const unsigned char *) NULL);
  p=source;
  q=GetPixels(image);
  indexes=GetIndexes(image);
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            index=ValidateColormapIndex(image,*p);
            indexes[x]=index;
            *q=image->colormap[index];
            p++;
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        index=ValidateColormapIndex(image,(*p << 8) | *(p+1));
        indexes[x]=index;
        *q=image->colormap[index];
        p+=2;
        q++;
      }
      break;
    }
    case IndexAlphaQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            index=ValidateColormapIndex(image,*p);
            indexes[x]=index;
            *q=image->colormap[index];
            p++;
            q->opacity=MaxRGB-UpScale(*p);
            p++;
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        index=ValidateColormapIndex(image,
          (DownScale(*p) << 8) | DownScale(*(p+1)));
        indexes[x]=index;
        *q=image->colormap[index];
        p+=2;
        q->opacity=XDownScale(XUpScale(MaxRGB)-((*p << 8) | *(p+1)));
        p+=2;
        q++;
      }
      break;
    }
    case GrayQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            index=ValidateColormapIndex(image,*p);
            indexes[x]=index;
            *q=image->colormap[index];
            p++;
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        index=ValidateColormapIndex(image,(*p << 8) | *(p+1));
        indexes[x]=index;
        *q=image->colormap[index];
        p+=2;
        q++;
      }
      break;
    }
    case GrayAlphaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            index=ValidateColormapIndex(image,*p);
            indexes[x]=index;
            *q=image->colormap[index];
            p++;
            q->opacity=MaxRGB-(*p);
            p++;
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        index=ValidateColormapIndex(image,(*p << 8) | *(p+1));
        indexes[x]=index;
        *q=image->colormap[index];
        p+=2;
        q->opacity=XDownScale(XUpScale(MaxRGB)-((*p << 8) | *(p+1)));
        p+=2;
        q++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q++;
      }
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->green=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->green=XDownScale(((*p) << 8) | *(p+1));
        p+=2;
        q++;
      }
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->blue=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->blue=XDownScale((*p << 8) | *(p+1));
        p+=2;
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
              for (x=0; x < (long) image->columns; x++)
                indexes[x]=UpScale(*p++);
              break;
            }
          for (x=0; x < (long) image->columns; x++)
          {
            indexes[x]=XDownScale((*p << 8) | *(p+1));
            p+=2;
          }
          break;
        }
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->opacity=MaxRGB-UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->opacity=XDownScale(XUpScale(MaxRGB)-(((*p) << 8) | *(p+1)));
        p+=2;
        q++;
      }
      break;
    }
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->opacity=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->green=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->blue=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q++;
      }
      break;
    }
    case RGBAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->opacity=MaxRGB-UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->green=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->blue=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->opacity=XDownScale(XUpScale(MaxRGB)-(((*p) << 8) | *(p+1)));
        p+=2;
        q++;
      }
      break;
    }
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->green=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->blue=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->opacity=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q++;
      }
      break;
    }
    case CMYKAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->opacity=UpScale(*p++);
            indexes[x]=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->green=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->blue=XDownScale((*p << 8) | *(p+1));
        p+=2;
        q->opacity=XDownScale((*p << 8) | *(p+1));
        p+=2;
        indexes[x]=XDownScale((*p << 8) | *(p+1));
        p+=2;
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

  register char
    *p;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  GetExceptionInfo(exception);
  if (*image_info->filename == '@')
    return(ReadImages(image_info,exception));
  clone_info=CloneImageInfo(image_info);
  (void) SetImageInfo(clone_info,False,exception);
  (void) strncpy(filename,clone_info->filename,MaxTextExtent-1);
  /*
    Call appropriate image reader based on image type.
  */
  image=(Image *) NULL;
  magick_info=GetMagickInfo(clone_info->magick,exception);
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->decoder !=
        (Image *(*)(const ImageInfo *,ExceptionInfo *)) NULL))
    {
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      clone_info->client_data=magick_info->client_data;
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  else
    {
      unsigned int
        status;

      delegate_info=GetDelegateInfo(clone_info->magick,(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          if (IsAccessible(clone_info->filename))
            ThrowException(exception,MissingDelegateWarning,
              "no delegate for this image format",clone_info->filename);
          else
            ThrowException(exception,FileOpenWarning,"Unable to open file",
              clone_info->filename);
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
      status=InvokeDelegate(clone_info,image,clone_info->magick,(char *) NULL,
        exception);
      DestroyImages(image);
      image=(Image *) NULL;
      if (status != False)
        clone_info->temporary=True;
      (void) SetImageInfo(clone_info,False,exception);
      magick_info=GetMagickInfo(clone_info->magick,exception);
      if ((magick_info == (const MagickInfo *) NULL) ||
          (magick_info->decoder ==
            (Image *(*)(const ImageInfo *,ExceptionInfo *)) NULL))
        {
          if (IsAccessible(clone_info->filename))
            ThrowException(exception,MissingDelegateWarning,
              "no delegate for this image format",clone_info->filename);
          else
            ThrowException(exception,FileOpenWarning,"Unable to open file",
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
  if (image->temporary)
    (void) remove(clone_info->filename);
  if (IsSubimage(clone_info->tile,False))
    {
      int
        count,
        offset,
        quantum;

      Image
        *subimages;

      unsigned long
        last,
        target;

      /*
        User specified subimages (e.g. image.miff[1,3-5,7-6,2]).
      */
      subimages=(Image *) NULL;
      target=atol(clone_info->tile);
      for (p=clone_info->tile; *p != '\0'; p+=Max(offset,1))
      {
        offset=0;
        count=sscanf(p,"%lu%n-%lu%n",&target,&offset,&last,&offset);
        if (count == 0)
          continue;
        if (count == 1)
          last=target;
        quantum=target > last ? -1 : 1;
        for ( ; target != (last+quantum); target+=quantum)
        {
          for (next=image; next; next=next->next)
          {
            Image
              *clone_image;

            if (next->scene != (unsigned long) target)
              continue;
            /*
              Clone this subimage.
            */
            clone_image=CloneImage(next,0,0,True,exception);
            if (clone_image == (Image *) NULL)
              break;
            if (subimages == (Image *) NULL)
              {
                subimages=clone_image;
                continue;
              }
            subimages->next=clone_image;
            subimages->next->previous=subimages;
            subimages=subimages->next;
          }
        }
      }
      DestroyImages(image);
      image=(Image *) NULL;
      if (subimages == (Image *) NULL)
        ThrowException(exception,OptionWarning,
          "Subimage specification returns no images",clone_info->filename);
      while (subimages->previous != (Image *) NULL)
        subimages=subimages->previous;
      image=subimages;
    }
  else
    if ((clone_info->subrange != 0) && (image->next != (Image *) NULL))
      {
        int
          retain;

        /*
          User specified subimages (e.g. image.miff[1]).
        */
        for ( ; ; )
        {
          retain=(image->scene >= clone_info->subimage) &&
            (image->scene <= (clone_info->subimage+clone_info->subrange-1));
          if (image->next != (Image *) NULL)
            {
              image=image->next;
              if (!retain)
                DestroyImage(image->previous);
              continue;
            }
          if (image->previous != (Image *) NULL)
            {
              image=image->previous;
              if (!retain)
                DestroyImage(image->next);
              break;
            }
          if (!retain)
            {
              DestroyImage(image);
              image=(Image *) NULL;
            }
          break;
        }
        if (image == (Image *) NULL)
          {
            ThrowException(exception,OptionWarning,
              "Subimage specification returns no images",clone_info->filename);
            DestroyImageInfo(clone_info);
            return((Image *) NULL);
          }
        while (image->previous != (Image *) NULL)
          image=image->previous;
      }
  if (image->status)
    {
      ThrowException(exception,CorruptImageWarning,
        "An error has occurred reading file",clone_info->filename);
      DestroyImageInfo(clone_info);
      return((Image *) NULL);
    }
  for (next=image; next; next=next->next)
  {
    next->taint=False;
    (void) strncpy(next->magick_filename,clone_info->filename,MaxTextExtent-1);
    if (image->temporary)
      (void) strncpy(next->filename,clone_info->filename,MaxTextExtent-1);
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

  register int
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
%  filehandle.  Write() returns 0 is there is a memory shortage or if the 
%  image cannot be written.  Check the exception member of image to determine
%  the cause for any failure.
%
%  The format of the WriteImage method is:
%
%      unsigned int WriteImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: Write the image defined by the file or filename members of
%      this structure.
%
%    o image: A pointer to a Image structure.
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
      (clone_info->page == (char *) NULL) &&
      !IsTaintImage(image) && IsAccessible(image->magick_filename))
    {
      delegate_info=GetDelegateInfo(image->magick,clone_info->magick,
        &image->exception);
      if ((delegate_info != (const DelegateInfo *) NULL) &&
          (delegate_info->mode == 0))
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
      (magick_info->encoder !=
        (unsigned int (*)(const ImageInfo *,Image *)) NULL))
    {
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      clone_info->client_data=magick_info->client_data;
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
      if (magick_info == (const MagickInfo *) NULL)
        magick_info=(MagickInfo *)
          GetMagickInfo(image->magick,&image->exception);
      if ((magick_info == (MagickInfo *) NULL) ||
          (magick_info->encoder ==
           (unsigned int (*)(const ImageInfo *,Image *)) NULL))
        {
          DestroyImageInfo(clone_info);
          ThrowBinaryException(MissingDelegateWarning,
            "no encode delegate for this image format",clone_info->magick)
        }
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      status=(magick_info->encoder)(clone_info,image);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  (void) strncpy(image->magick,clone_info->magick,MaxTextExtent-1);
  DestroyImageInfo(clone_info);
  if (image->status)
    ThrowBinaryException(CorruptImageWarning,
      "An error has occurred writing to file",image->filename);
  return(status);
}
