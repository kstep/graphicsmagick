/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
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
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/stream.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Type definitions
*/
typedef enum
{
  RedMapQuantum,
  GreenMapQuantum,
  BlueMapQuanum,
  OpacityMapQuantum,
  IntensityMapQuantum
} MapQuantumType;

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
%      I = intensity (for grayscale). Creation of an alpha channel for CMYK
%      images is currently not supported.
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

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

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

  /*
    Prepare a validated and more efficient version of the map.
  */
  length=strlen(map);
  length=Min(length,sizeof(switch_map)/sizeof(MapQuantumType));
  for (i=0; i < (long) length; i++)
    {
      switch ((int) toupper(map[i]))
        {
        case 'R':
          {
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'G':
          {
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'B':
          {
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'A':
          {
            switch_map[i]=OpacityMapQuantum;
            image->matte=True;
            break;
          }
        case 'C':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'M':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'Y':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'K':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'I':
          {
            if (!AllocateImageColormap(image,MaxColormapSize))
              ThrowImageException(ResourceLimitError,"MemoryAllocationFailed",
                "UnableToConstituteImage");
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        default:
          {
            DestroyImage(image);
            ThrowImageException(OptionError,"UnrecognizedPixelMap",map)
              break;
          }
        }
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
	  q->red=0;
	  q->green=0;
	  q->blue=0;
	  q->opacity=OpaqueOpacity;
          for (i=0; i < (long) length; i++)
          {
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                q->red=ScaleCharToQuantum(*p++);
                break;
              }
              case GreenMapQuantum:
              {
                q->green=ScaleCharToQuantum(*p++);
                break;
              }
              case BlueMapQuanum:
              {
                q->blue=ScaleCharToQuantum(*p++);
                break;
              }
              case OpacityMapQuantum:
              {
                q->opacity=ScaleCharToQuantum(*p++);
                break;
              }
              case IntensityMapQuantum:
              {
                indexes[x]=ScaleQuantumToIndex(ScaleCharToQuantum(*p++));
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
	  q->red=0;
	  q->green=0;
	  q->blue=0;
	  q->opacity=OpaqueOpacity;
          for (i=0; i < (long) length; i++)
          {
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                q->red=ScaleShortToQuantum(*p++);
                break;
              }
              case GreenMapQuantum:
              {
                q->green=ScaleShortToQuantum(*p++);
                break;
              }
              case BlueMapQuanum:
              {
                q->blue=ScaleShortToQuantum(*p++);
                break;
              }
              case OpacityMapQuantum:
              {
                q->opacity=ScaleShortToQuantum(*p++);
                break;
              }
              case IntensityMapQuantum:
              {
                indexes[x]=ScaleQuantumToIndex(ScaleShortToQuantum(*p++));
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
	  q->red=0;
	  q->green=0;
	  q->blue=0;
	  q->opacity=OpaqueOpacity;
          for (i=0; i < (long) length; i++)
          {
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                q->red=ScaleLongToQuantum(*p++);
                break;
              }
              case GreenMapQuantum:
              {
                q->green=ScaleLongToQuantum(*p++);
                break;
              }
              case BlueMapQuanum:
              {
                q->blue=ScaleLongToQuantum(*p++);
                break;
              }
              case OpacityMapQuantum:
              {
                q->opacity=ScaleLongToQuantum(*p++);
                break;
              }
              case IntensityMapQuantum:
              {
                indexes[x]=ScaleQuantumToIndex(ScaleLongToQuantum(*p++));
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
	  q->red=0;
	  q->green=0;
	  q->blue=0;
	  q->opacity=OpaqueOpacity;
          for (i=0; i < (long) length; i++)
          {
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                q->red=ScaleLongToQuantum(*p++);
                break;
              }
              case GreenMapQuantum:
              {
                q->green=(Quantum) (*p++);
                break;
              }
              case BlueMapQuanum:
              {
                q->blue=ScaleLongToQuantum(*p++);
                break;
              }
              case OpacityMapQuantum:
              {
                q->opacity=ScaleLongToQuantum(*p++);
                break;
              }
              case IntensityMapQuantum:
              {
                indexes[x]=ScaleQuantumToIndex(ScaleLongToQuantum(*p++));
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
	  q->red=0;
	  q->green=0;
	  q->blue=0;
	  q->opacity=OpaqueOpacity;
          for (i=0; i < (long) length; i++)
          {
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                q->red=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case GreenMapQuantum:
              {
                q->green=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case BlueMapQuanum:
              {
                q->blue=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case OpacityMapQuantum:
              {
                q->opacity=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case IntensityMapQuantum:
              {
                indexes[x]=(Quantum) ((MaxColormapSize-1)*(*p++)+0.5);
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
          q->red=0;
	  q->green=0;
          q->blue=0;
          q->opacity=OpaqueOpacity;
          for (i=0; i < (long) length; i++)
          {
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                q->red=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case GreenMapQuantum:
              {
                q->green=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case BlueMapQuanum:
              {
                q->blue=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case OpacityMapQuantum:
              {
                q->opacity=(Quantum) ((double) MaxRGB*(*p++)+0.5);
                break;
              }
              case IntensityMapQuantum:
              {
                indexes[x]=(Quantum) ((MaxColormapSize-1)*(*p++)+0.5);
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
  if (image->storage_class == PseudoClass)
    {
      /*
        Check and cache monochrome and grayscale status
      */
      (void) IsMonochromeImage(image,exception);
      if (image->is_monochrome)
        image->is_grayscale=True;
      else
        (void) IsGrayImage(image,exception);
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
%  The method returns False on success or True if an error is encountered.
%  The data is returned as char, short int, int, long, float, or double in
%  the order specified by map.
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

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

  size_t
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  length=strlen(map);
  length=Min(length,sizeof(switch_map)/sizeof(MapQuantumType));

  /*
    Prepare a validated and more efficient version of the map.
  */

  for (i=0; i < (long) length; i++)
    {
      switch ((int) toupper(map[i]))
        {
        case 'R':
          {
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'G':
          {
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'B':
          {
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'A':
          {
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'C':
          {
            switch_map[i]=RedMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,"ColorSeparatedImageRequired",map);
            return(False);
          }
        case 'M':
          {
            switch_map[i]=GreenMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,"ColorSeparatedImageRequired",map);
            return(False);
          }
        case 'Y':
          {
            switch_map[i]=BlueMapQuanum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,"ColorSeparatedImageRequired",map);
            return(False);
          }
        case 'K':
          {
            switch_map[i]=OpacityMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,"ColorSeparatedImageRequired",map);
            return(False);
          }
        case 'I':
          {
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        default:
          {
            ThrowException(exception,OptionError,"UnrecognizedPixelMap",map);
            return(False);
          }
        }
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
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                *q++=ScaleQuantumToChar(p->red);
                break;
              }
              case GreenMapQuantum:
              {
                *q++=ScaleQuantumToChar(p->green);
                break;
              }
              case BlueMapQuanum:
              {
                *q++=ScaleQuantumToChar(p->blue);
                break;
              }
              case OpacityMapQuantum:
              {
                *q++=ScaleQuantumToChar(p->opacity);
                break;
              }
              case IntensityMapQuantum:
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
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                *q++=ScaleQuantumToShort(p->red);
                break;
              }
              case GreenMapQuantum:
              {
                *q++=ScaleQuantumToShort(p->green);
                break;
              }
              case BlueMapQuanum:
              {
                *q++=ScaleQuantumToShort(p->blue);
                break;
              }
              case OpacityMapQuantum:
              {
                *q++=ScaleQuantumToShort(p->opacity);
                break;
              }
              case IntensityMapQuantum:
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
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                *q++=ScaleQuantumToLong(p->red);
                break;
              }
              case GreenMapQuantum:
              {
                *q++=ScaleQuantumToLong(p->green);
                break;
              }
              case BlueMapQuanum:
              {
                *q++=ScaleQuantumToLong(p->blue);
                break;
              }
              case OpacityMapQuantum:
              {
                *q++=ScaleQuantumToLong(p->opacity);
                break;
              }
              case IntensityMapQuantum:
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
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                *q++=ScaleQuantumToLong(p->red);
                break;
              }
              case GreenMapQuantum:
              {
                *q++=ScaleQuantumToLong(p->green);
                break;
              }
              case BlueMapQuanum:
              {
                *q++=ScaleQuantumToLong(p->blue);
                break;
              }
              case OpacityMapQuantum:
              {
                *q++=ScaleQuantumToLong(p->opacity);
                break;
              }
              case IntensityMapQuantum:
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
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                *q++=(double) p->red/MaxRGB;
                break;
              }
              case GreenMapQuantum:
              {
                *q++=(double) p->green/MaxRGB;
                break;
              }
              case BlueMapQuanum:
              {
                *q++=(double) p->blue/MaxRGB;
                break;
              }
              case OpacityMapQuantum:
              {
                *q++=(double) p->opacity/MaxRGB;
                break;
              }
              case IntensityMapQuantum:
              {
		*q++=(double) PixelIntensity(p)/MaxRGB;
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
            switch (switch_map[i])
            {
              case RedMapQuantum:
              {
                *q++=(double) p->red/MaxRGB;
                break;
              }
              case GreenMapQuantum:
              {
                *q++=(double) p->green/MaxRGB;
                break;
              }
              case BlueMapQuanum:
              {
                *q++=(double) p->blue/MaxRGB;
                break;
              }
              case OpacityMapQuantum:
              {
                *q++=(double) p->opacity/MaxRGB;
                break;
              }
              case IntensityMapQuantum:
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

  register unsigned int
    pixel;

  unsigned long
    number_pixels;

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
      assert(image->colors <= MaxColormapSize);
      if (image->colors <= 256)
        {
          for (x= (long) number_pixels; x > 0; --x)
            *q++=(unsigned char) *indexes++;
          break;
        }
#if MaxColormapSize > 256
      if (image->colors <= 65536L)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            *q++=(unsigned char) (*indexes >> 8);
            *q++=(unsigned char) *indexes++;
          }
          break;
        }
#endif /* MaxColormapSize > 256 */
#if MaxColormapSize > 65536
      for (x= (long) number_pixels; x > 0; --x)
      {
        *q++=(unsigned char) (*indexes >> 24);
        *q++=(unsigned char) (*indexes >> 16);
        *q++=(unsigned char) (*indexes >> 8);
        *q++=(unsigned char) *indexes++;
      }
      break;
#endif /* MaxColormapSize > 65536 */
    }
    case IndexAlphaQuantum:
    {
      assert(image->colors <= MaxColormapSize);
      if (image->colors <= 256)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            *q++=(unsigned char) *indexes++;
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
#if MaxColormapSize > 256
      if (image->colors <= 65536L)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            *q++=(unsigned char) (*indexes >> 8);
            *q++=(unsigned char) *indexes++;
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
#endif /* MaxColormapSize > 256 */
#if MaxColormapSize > 65536
      for (x= (long) number_pixels; x > 0; --x)
      {
        *q++=(unsigned char) (*indexes >> 24);
        *q++=(unsigned char) (*indexes >> 16);
        *q++=(unsigned char) (*indexes >> 8);
        *q++=(unsigned char) *indexes++;
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
#endif /* MaxColormapSize > 65536 */
    }
    case GrayQuantum:
    {
      if (image->depth <= 8)
        {
          if (image->is_grayscale)
            for (x= (long) number_pixels; x > 0; --x)
              *q++=(unsigned char) ScaleQuantumToChar(p++->red);
          else
            for (x= (long) number_pixels; x > 0; --x)
              {
                *q++=(unsigned char) ScaleQuantumToChar(PixelIntensityToQuantum(p));
                p++;
              }
          break;
        }
      if (image->depth <= 16)
        {
          if (image->is_grayscale)
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToShort(p->red);
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
          else
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToShort(PixelIntensityToQuantum(p));
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
          break;
        }
      if (image->is_grayscale)
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToLong(p->red);
                  *q++=(unsigned char) (pixel >> 24);
                  *q++=(unsigned char) (pixel >> 16);
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
      else
        {
          for (x= (long) number_pixels; x > 0; --x)
            {
              pixel=ScaleQuantumToLong(PixelIntensityToQuantum(p));
              *q++=(unsigned char) (pixel >> 24);
              *q++=(unsigned char) (pixel >> 16);
              *q++=(unsigned char) (pixel >> 8);
              *q++=(unsigned char) pixel;
              p++;
            }
        }
      break;
    }
    case GrayAlphaQuantum:
    {
      if (image->depth <= 8)
        {
          if (image->is_grayscale)
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToChar(p->red);
                  *q++=(unsigned char) pixel;
                  pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
          else
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToChar(PixelIntensityToQuantum(p));
                  *q++=(unsigned char) pixel;
                  pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
          break;
        }
      if (image->depth <= 16)
        {
          if (image->is_grayscale)
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToShort(p->red);
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
          else
            {
              for (x= (long) number_pixels; x > 0; --x)
                {
                  pixel=ScaleQuantumToShort(PixelIntensityToQuantum(p));
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
                  *q++=(unsigned char) (pixel >> 8);
                  *q++=(unsigned char) pixel;
                  p++;
                }
            }
          break;
        }
      if (image->is_grayscale)
        {
          for (x= (long) number_pixels; x > 0; --x)
            {
              pixel=ScaleQuantumToLong(p->red);
              *q++=(unsigned char) (pixel >> 24);
              *q++=(unsigned char) (pixel >> 16);
              *q++=(unsigned char) (pixel >> 8);
              *q++=(unsigned char) pixel;
              pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
              *q++=(unsigned char) (pixel >> 24);
              *q++=(unsigned char) (pixel >> 16);
              *q++=(unsigned char) (pixel >> 8);
              *q++=(unsigned char) pixel;
              p++;
            }
        }
      else
        {
          for (x= (long) number_pixels; x > 0; --x)
            {
              pixel=ScaleQuantumToLong(PixelIntensityToQuantum(p));
              *q++=(unsigned char) (pixel >> 24);
              *q++=(unsigned char) (pixel >> 16);
              *q++=(unsigned char) (pixel >> 8);
              *q++=(unsigned char) pixel;
              pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
              *q++=(unsigned char) (pixel >> 24);
              *q++=(unsigned char) (pixel >> 16);
              *q++=(unsigned char) (pixel >> 8);
              *q++=(unsigned char) pixel;
              p++;
            }
        }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
         for (x= (long) number_pixels; x > 0; --x)
         {
           pixel=ScaleQuantumToShort(p->red);
           *q++=(unsigned char) (pixel >> 8);
           *q++=(unsigned char) pixel;
           p++;
         }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->green);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->green);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->green);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
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
              for (x= (long) number_pixels; x > 0; --x)
              {
                pixel=ScaleQuantumToChar(MaxRGB-indexes[x]);
                *q++=(unsigned char) pixel;
                p++;
              }
              break;
            }
          if (image->depth <= 16)
            {
              for (x= (long) number_pixels; x > 0; --x)
              {
                pixel=ScaleQuantumToShort(MaxRGB-indexes[x]);
                *q++=(unsigned char) (pixel >> 8);
                *q++=(unsigned char) pixel;
                p++;
              }
              break;
            }
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToLong(MaxRGB-indexes[x]);
            *q++=(unsigned char) (pixel >> 24);
            *q++=(unsigned char) (pixel >> 16);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->opacity);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->opacity);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->opacity);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case RGBAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(MaxRGB-p->opacity);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(MaxRGB-p->opacity);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(MaxRGB-p->opacity);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->opacity);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->opacity);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->opacity);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        p++;
      }
      break;
    }
    case CMYKAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToChar(p->red);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->green);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->blue);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(p->opacity);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToChar(MaxRGB-(*indexes++));
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      if (image->depth <= 16)
        {
          for (x= (long) number_pixels; x > 0; --x)
          {
            pixel=ScaleQuantumToShort(p->red);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->green);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->blue);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(p->opacity);
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            pixel=ScaleQuantumToShort(MaxRGB-(*indexes++));
            *q++=(unsigned char) (pixel >> 8);
            *q++=(unsigned char) pixel;
            p++;
          }
          break;
        }
      for (x= (long) number_pixels; x > 0; --x)
      {
        pixel=ScaleQuantumToLong(p->red);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->green);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->blue);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(p->opacity);
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
        pixel=ScaleQuantumToLong(MaxRGB-(*indexes++));
        *q++=(unsigned char) (pixel >> 24);
        *q++=(unsigned char) (pixel >> 16);
        *q++=(unsigned char) (pixel >> 8);
        *q++=(unsigned char) pixel;
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
%      unsigned int PushImagePixels(Image *image,
%        const QuantumType quantum_type,
%        const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns True if the pixels are
%      successfully transferred, otherwise False.
%
%    o image: The image.
%
%    o quantum_type: Declare which pixel components to transfer (red, green,
%      blue, opacity, RGB, or RGBA).
%
%    o source:  The pixel components are transferred from this buffer.
%
*/
MagickExport unsigned int PushImagePixels(Image *image,
  const QuantumType quantum_type,const unsigned char *source)
{
  register const unsigned char
    *p;

  register unsigned int
    quantum;

  register IndexPacket
    index,
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned long
    number_pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(source != (const unsigned char *) NULL);
  number_pixels=GetPixelCacheArea(image);
  p=source;
  q=GetPixels(image);
  indexes=GetIndexes(image);
  switch (quantum_type)
    {
    case IndexQuantum:
      {
        assert(image->colors <= MaxColormapSize);
        if (image->colors <= 256)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q++=image->colormap[index];
              }
            break;
          }
#if MaxColormapSize > 256
        if (image->colors <= 65536L)
          {
            for ( x= (long) number_pixels; x > 0; --x)
              {
                index=(*p++ << 8);
                index|=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q++=image->colormap[index];
              }
            break;
          }
#endif /* MaxColormapSize > 256 */
#if MaxColormapSize > 65536
        for (x = (long) number_pixels; x > 0; --x)
          {
            index=(*p++ << 24);
            index|=(*p++ << 16);
            index|=(*p++ << 8);
            index|=(*p++);
            VerifyColormapIndex(image,index);
            *indexes++=index;
            *q++=image->colormap[index];
          }
        break;
#endif /* MaxColormapSize > 65536 */
      }
    case IndexAlphaQuantum:
      {
        assert(image->colors <= MaxColormapSize);
        if (image->colors <= 256)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q=image->colormap[index];
                q->opacity=ScaleCharToQuantum(255-(*p++));
                q++;
              }
            break;
          }
#if MaxColormapSize > 256
        if (image->colors <= 65536L)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++ << 8);
                index|=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q=image->colormap[index];
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(65535-quantum);
                q++;
              }
            break;
          }
#endif /* MaxColormapSize > 256 */
#if MaxColormapSize > 65536
        for (x = (long) number_pixels; x > 0; --x)
          {
            index=(*p++ << 24);
            index|=(*p++ << 16);
            index|=(*p++ << 8);
            index|=(*p++);
            VerifyColormapIndex(image,index);
            *indexes++=index;
            *q=image->colormap[index];
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->opacity=ScaleLongToQuantum(4294967295U-quantum);
            q++;
          }
      break;
#endif /* MaxColormapSize > 65536 */
      }
    case GrayQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q++=image->colormap[index];
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++ << 8);
                index|=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q++=image->colormap[index];
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            index=(*p++ << 24);
            index|=(*p++ << 16);
            index|=(*p++ << 8);
            index|=(*p++);
            VerifyColormapIndex(image,index);
            *indexes++=index;
            *q++=image->colormap[index];
          }
        break;
      }
    case GrayAlphaQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q=image->colormap[index];
                q->opacity=ScaleCharToQuantum(255-(*p++));
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                index=(*p++ << 8);
                index|=(*p++);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q=image->colormap[index];
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(65535-quantum);
                p++;
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            index=(*p++ << 24);
            index|=(*p++ << 16);
            index|=(*p++ << 8);
            index|=(*p++);
            VerifyColormapIndex(image,index);
            *indexes++=index;
            *q=image->colormap[index];
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->opacity=ScaleLongToQuantum(4294967295U-quantum);
            q++;
          }
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->red=ScaleCharToQuantum(*p++);
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->red=ScaleShortToQuantum(quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->red=ScaleLongToQuantum(quantum);
            q++;
          }
        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->green=ScaleCharToQuantum((*p++));
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->green=ScaleShortToQuantum(quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->green=ScaleLongToQuantum(quantum);
            q++;
          }
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->blue=ScaleCharToQuantum(*p++);
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->blue=ScaleShortToQuantum(quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->blue=ScaleLongToQuantum(quantum);
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
                for (x = (long) number_pixels; x > 0; --x)
                  {
                    *indexes++=ScaleCharToQuantum(*p++);
                  }
                break;
              }
            if (image->depth <= 16)
              {
                for (x = (long) number_pixels; x > 0; --x)
                  {
                    quantum=(*p++ << 8);
                    quantum|=(*p++);
                    *indexes++=ScaleShortToQuantum(quantum);
                  }
                break;
              }
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 24);
                quantum|=(*p++ << 16);
                quantum|=(*p++ << 8);
                quantum|=(*p++);
                *indexes++=ScaleLongToQuantum(quantum);
              }
            break;
          }
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->opacity=ScaleCharToQuantum(255-(*p++));
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(65535-quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=*p++;
            q->opacity=ScaleLongToQuantum(4294967295U-quantum);
            q++;
          }
        break;
      }
    case BlackQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->opacity=ScaleCharToQuantum(*p++);
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->opacity=ScaleLongToQuantum(quantum);
            q++;
          }
        break;
      }
    case RGBQuantum:
    default:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->red=ScaleCharToQuantum(*p++);
                q->green=ScaleCharToQuantum(*p++);
                q->blue=ScaleCharToQuantum(*p++);
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->red=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->green=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->blue=ScaleShortToQuantum(quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->red=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->green=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->blue=ScaleLongToQuantum(quantum);
            q++;
          }
        break;
      }
    case RGBAQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->red=ScaleCharToQuantum(*p++);
                q->green=ScaleCharToQuantum(*p++);
                q->blue=ScaleCharToQuantum(*p++);
                q->opacity=ScaleCharToQuantum(255-(*p++));
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->red=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->green=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->blue=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(65535-quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->red=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->green=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->blue=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->opacity=ScaleLongToQuantum(4294967295U-quantum);
            q++;
          }
        break;
      }
    case CMYKQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->red=ScaleCharToQuantum(*p++);
                q->green=ScaleCharToQuantum(*p++);
                q->blue=ScaleCharToQuantum(*p++);
                q->opacity=ScaleCharToQuantum(*p++);
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->red=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->green=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->blue=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->red=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->green=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->blue=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->opacity=ScaleLongToQuantum(quantum);
            q++;
          }
        break;
      }
    case CMYKAQuantum:
      {
        if (image->depth <= 8)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                q->red=ScaleCharToQuantum(*p++);
                q->green=ScaleCharToQuantum(*p++);
                q->blue=ScaleCharToQuantum(*p++);
                q->opacity=ScaleCharToQuantum(*p++);
                *indexes++=(IndexPacket) ScaleCharToQuantum(255-(*p++));
                q++;
              }
            break;
          }
        if (image->depth <= 16)
          {
            for (x = (long) number_pixels; x > 0; --x)
              {
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->red=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->green=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->blue=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                q->opacity=ScaleShortToQuantum(quantum);
                quantum=(*p++ << 8);
                quantum|=(*p++);
                *indexes++=ScaleShortToQuantum(65535-quantum);
                q++;
              }
            break;
          }
        for (x = (long) number_pixels; x > 0; --x)
          {
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->red=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->green=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->blue=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            q->opacity=ScaleLongToQuantum(quantum);
            quantum=(*p++ << 24);
            quantum|=(*p++ << 16);
            quantum|=(*p++ << 8);
            quantum|=(*p++);
            *indexes++=(IndexPacket) ScaleLongToQuantum(4294967295U-quantum);
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
    filename[MaxTextExtent],
    magick[MaxTextExtent];

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
  (void) strncpy(magick,clone_info->magick,MaxTextExtent-1);
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
          if(!AcquireTemporaryFileName(clone_info->filename))
            {
              ThrowException(exception,FileOpenError,
                "UnableToCreateTemporaryFile",clone_info->filename);
              CloseBlob(image);
              DestroyImageInfo(clone_info);
              DestroyImage(image);
              return((Image *) NULL);
            }
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
      LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s)",magick_info->name,
        magick_info->description);
      image=(magick_info->decoder)(clone_info,exception);
      LogMagickEvent(CoderEvent,GetMagickModule(),
        "Returned from \"%.1024s\" decoder",magick_info->name);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  else
    {
      delegate_info=GetDelegateInfo(clone_info->magick,(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          if (IsAccessibleAndNotEmpty(clone_info->filename))
            ThrowException(exception,MissingDelegateError,
              "NoDecodeDelegateForThisImageFormat",clone_info->filename);
          else
            ThrowException(exception,FileOpenError,"UnableToOpenFile",
              clone_info->filename);
          if (clone_info->temporary)
            LiberateTemporaryFile(clone_info->filename);
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
      if(!AcquireTemporaryFileName(clone_info->filename))
        {
          ThrowException(exception,FileOpenError,"UnableToCreateTemporaryFile",
            clone_info->filename);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
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
          if (IsAccessibleAndNotEmpty(clone_info->filename))
            ThrowException(exception,MissingDelegateError,
              "NoDecodeDelegateForThisImageFormat",clone_info->filename);
          else
            ThrowException(exception,FileOpenError,"UnableToOpenFile",
              clone_info->filename);
          if (clone_info->temporary)
            LiberateTemporaryFile(clone_info->filename);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s)",magick_info->name,
        magick_info->description);
      image=(magick_info->decoder)(clone_info,exception);
      LogMagickEvent(CoderEvent,GetMagickModule(),
        "Returned from \"%.1024s\" decoder",magick_info->name);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
      /*
        Restore original input file magick in case read is from a
        temporary file prepared by an external delegate.  The user
        will expect that the format reported is that of the input
        file.
      */
      (void) strncpy(image->magick,magick,MaxTextExtent-1);
    }
  if (clone_info->temporary)
    {
      if(!LiberateTemporaryFile(clone_info->filename))
        remove(clone_info->filename); /* Must be user-provided temporary */
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
    remove(clone_info->filename); /* Maybe this should be LiberateTemporaryFile? */
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
    if (next->storage_class == PseudoClass)
      {
        /*
          Check and cache monochrome and grayscale status
        */
        (void) IsMonochromeImage(next,exception);
        if (next->is_monochrome)
          next->is_grayscale=True;
        else
          (void) IsGrayImage(next,exception);
      }
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
  image->dither=image_info->dither;
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
      LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" encoder (%.1024s)",magick_info->name,
        magick_info->description);
      status=(magick_info->encoder)(clone_info,image);
      LogMagickEvent(CoderEvent,GetMagickModule(),
        "Returned from \"%.1024s\" encoder",magick_info->name);
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
          if(!AcquireTemporaryFileName(image->filename))
            {
              ThrowException(&image->exception,FileOpenError,
                "UnableToCreateTemporaryFile",image->filename);
              DestroyImageInfo(clone_info);
              return(False);
            }
          status=InvokeDelegate(clone_info,image,(char *) NULL,
            clone_info->magick,&image->exception);
          LiberateTemporaryFile(image->filename);
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
