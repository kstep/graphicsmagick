/*
% Copyright (C) 2003, 2004 GraphicsMagick Group
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
%                      Methods to Constitute an Image                         %
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
#include "magick/bit_stream.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/semaphore.h"
#include "magick/stream.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Type definitions
*/
typedef enum
{
  BlueMapQuanum,
  GreenMapQuantum,
  IntensityMapQuantum,
  OpacityInvertedMapQuantum,
  PadMapQuantum,
  RedMapQuantum,
  OpacityMapQuantum
} MapQuantumType;

typedef enum {
  UndefinedDispatchType,
  BGRDispatchType,
  BGRODispatchType,
  BGRPDispatchType,
  RGBDispatchType,
  RGBODispatchType,
  IDispatchType
} DispatchType;


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
%  ConstituteImage() returns an Image corresponding to an image stored
%  in a raw memory array format. The pixel data must be in scanline order
%  top-to-bottom. The data can be unsigned char, unsigned short int, unsigned
%  int, unsigned long, float, or double.  Float and double require the pixels
%  to be normalized to the range [0..1], otherwise the range is [0..MaxVal]
%  where MaxVal is the maximum possible value for that type.
%
%  Note that for most 32-bit architectures the size of an unsigned long is
%  the same as unsigned int, but for 64-bit architectures observing the LP64
%  standard, an unsigned long is 64 bits, while an unsigned int remains 32
%  bits. This should be considered when deciding if the data should be
%  described as "Integer" or "Long".
%
%  For example, to create a 640x480 image from unsigned red-green-blue
%  character data, use
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
%      A = alpha (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, or I = intensity
%      (for grayscale). Specify "P" = pad, to skip over a quantum which is
%      intentionally ignored. Creation of an alpha channel for CMYK images
%      is currently not supported.
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

  register Quantum
    quantum;

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
    ThrowBinaryException3(OptionError,UnableToConstituteImage,
      NonzeroWidthAndHeightRequired);
  image->columns=width;
  image->rows=height;

  /*
    Handle a few common special cases in order to improve performance.
  */
  if (type == CharPixel)
    {
      DispatchType
        dispatch_type=UndefinedDispatchType;

      if (LocaleCompare(map,"BGR") == 0)
        dispatch_type=BGRDispatchType;
      else if (LocaleCompare(map,"BGRO") == 0)
        dispatch_type=BGRODispatchType;
      else if (LocaleCompare(map,"BGRP") == 0)
        dispatch_type=BGRPDispatchType;
      else if (LocaleCompare(map,"RGB") == 0)
        dispatch_type=RGBDispatchType;
      else if (LocaleCompare(map,"RGBO") == 0)
        dispatch_type=RGBODispatchType;
      else if (LocaleCompare(map,"I") == 0)
        {
          if (!AllocateImageColormap(image,MaxColormapSize))
            ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                                 UnableToConstituteImage);
          dispatch_type=IDispatchType;
        }

      if (dispatch_type != UndefinedDispatchType)
        {
          register const unsigned char
            *p = pixels;

          for (y=0; y < (long) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);

              switch (dispatch_type)
                {
                case BGRDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        q->blue=ScaleCharToQuantum(*p++);
                        q->green=ScaleCharToQuantum(*p++);
                        q->red=ScaleCharToQuantum(*p++);
                        q->opacity=OpaqueOpacity;
                        q++;
                      }
                    break;
                  }
                case BGRODispatchType:
                  { 
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        q->blue=ScaleCharToQuantum(*p++);
                        q->green=ScaleCharToQuantum(*p++);
                        q->red=ScaleCharToQuantum(*p++);
                        q->opacity=(Quantum) MaxRGB-ScaleCharToQuantum(*p++);
                        q++;
                      }
                    break;
                  }
                case BGRPDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        q->blue=ScaleCharToQuantum(*p++);
                        q->green=ScaleCharToQuantum(*p++);
                        q->red=ScaleCharToQuantum(*p++);
                        p++;
                        q->opacity=OpaqueOpacity;
                        q++;
                      }
                    break;
                  }
                case RGBDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        q->red=ScaleCharToQuantum(*p++);
                        q->green=ScaleCharToQuantum(*p++);
                        q->blue=ScaleCharToQuantum(*p++);
                        q->opacity=OpaqueOpacity;
                        q++;
                      }
                    break;
                  }
                case RGBODispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        q->red=ScaleCharToQuantum(*p++);
                        q->green=ScaleCharToQuantum(*p++);
                        q->blue=ScaleCharToQuantum(*p++);
                        q->opacity=MaxRGB-ScaleCharToQuantum(*p++);
                        q++;
                      }
                    break;
                  }
                case IDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        *indexes=ScaleQuantumToIndex(ScaleCharToQuantum(*p++));
                        q->red=q->green=q->blue=image->colormap[*indexes].red;
                        q->opacity=OpaqueOpacity;;
                        indexes++;
                        q++;
                      }
                    break;
                  }
                case UndefinedDispatchType:
                  {
                    break;
                  }
                } /* end switch */
              if (!SyncImagePixels(image))
                break;
            } /* end for (y=0; y < (long) image->rows; y++) */
          if (dispatch_type == IDispatchType)
            {
              (void) IsMonochromeImage(image,exception);
              image->is_grayscale=True;
            }
          return (image);
        } /* end if (dispatch_type != UndefinedDispatchType) */
    } /* end if (type == CharPixel) */

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
              ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                UnableToConstituteImage);
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        case 'O':
          {
            switch_map[i]=OpacityInvertedMapQuantum;
            image->matte=True;
            break;
          }
        case 'P':
          {
            switch_map[i]=PadMapQuantum;
            break;
          }
        case 'T':
          {
            switch_map[i]=OpacityMapQuantum;
            image->matte=True;
            break;
          }
        default:
          {
            DestroyImage(image);
            ThrowImageException(OptionError,UnrecognizedPixelMap,map)
          }
        }
    }

  /*
    Transfer the pixels from the pixel data array to the image.
  */
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
              /*
                Input a quantum
              */
              quantum=0U;
              
              switch (type)
                {
                case CharPixel:
                  {
                    register const unsigned char *p = pixels;
                    quantum=ScaleCharToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case ShortPixel:
                  {
                    register const unsigned short *p = pixels;
                    quantum=ScaleShortToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case IntegerPixel:
                  {
                    register const unsigned int *p = pixels;
                    quantum=ScaleLongToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case LongPixel:
                  {
                    register const unsigned long *p = pixels;
                    quantum=ScaleLongToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case FloatPixel:
                  {
                    double quantum_float;
                    register const float *p = pixels;
                    quantum_float=(double) MaxRGB*(*p++);
                    quantum=RoundSignedToQuantum(quantum_float);
                    pixels = (const void *) p;
                    break;
                  }
                case DoublePixel:
                  {
                    double quantum_float;
                    register const double *p = pixels;
                    quantum_float=(double) MaxRGB*(*p++);
                    quantum=RoundSignedToQuantum(quantum_float);
                    pixels = (const void *) p;
                    break;
                  }
                }
              
              /*
                Transfer quantum to image
              */
              switch (switch_map[i])
                {
                case RedMapQuantum:
                  {
                    q->red=quantum;
                    break;
                  }
                case GreenMapQuantum:
                  {
                    q->green=quantum;
                    break;
                  }
                case BlueMapQuanum:
                  {
                    q->blue=quantum;
                    break;
                  }
                case OpacityMapQuantum:
                  {
                    q->opacity=quantum;
                    break;
                  }
                case OpacityInvertedMapQuantum:
                  {
                    q->opacity=MaxRGB-quantum;
                    break;
                  }
                case IntensityMapQuantum:
                  {
                    *indexes=ScaleQuantumToIndex(quantum);
                    q->red=q->green=q->blue=image->colormap[*indexes].red;
                    break;
                  }
                case PadMapQuantum:
                  {
                    /* Discard quantum */
                    break;
                  }
                }
            }
          indexes++;
          q++;
        }
      if (!SyncImagePixels(image))
        break;
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
#if defined(JUST_FOR_DOCUMENTATION)
  /* The first two calls should bracket any code that deals with the data
     structurees being released */
  AcquireSemaphoreInfo(&constitute_semaphore);
  LiberateSemaphoreInfo(&constitute_semaphore);
#endif
  /* The final call actually releases the associated mutex used to prevent
     multiple threads from accessing the data */
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
%  DispatchImage() extracts pixel data from an Image into a raw memory array.
%  The pixel data is written in scanline order top-to-bottom using an 
%  arbitrary quantum order specified by 'map', and with quantum size
%  specified by 'type'.
%
%  The output array data may be unsigned char, unsigned short int, unsigned
%  int, unsigned long, float, or double.  Float and double require the pixels
%  to be normalized to the range [0..1], otherwise the range is [0..MaxVal]
%  where MaxVal is the maximum possible value for that type.
%
%  The method returns False on success or True if an error is encountered.
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
%    o map: This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha  (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, I = intensity (for
%      grayscale). Specify "P" = pad, to output a pad quantum. Pad quantums
%      are zero-value.
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
MagickExport MagickPassFail DispatchImage(const Image *image,const long x_offset,
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

  register Quantum
    quantum;

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

  size_t
    length;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Handle a few common special cases in order to improve performance.
  */
  if (type == CharPixel)
    {
      DispatchType
        dispatch_type=UndefinedDispatchType;

      if (LocaleCompare(map,"BGR") == 0)
        dispatch_type=BGRDispatchType;
      else if (LocaleCompare(map,"BGRO") == 0)
        dispatch_type=BGRODispatchType;
      else if (LocaleCompare(map,"BGRP") == 0)
        dispatch_type=BGRPDispatchType;
      else if (LocaleCompare(map,"RGB") == 0)
        dispatch_type=RGBDispatchType;
      else if (LocaleCompare(map,"RGBO") == 0)
        dispatch_type=RGBODispatchType;
      else if (LocaleCompare(map,"I") == 0)
        dispatch_type=IDispatchType;

      if (dispatch_type != UndefinedDispatchType)
        {
          register unsigned char
            *q=pixels;
          
          for (y=0; y < (long) rows; y++)
            {
              p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
              if (p == (const PixelPacket *) NULL)
                {
                  status=MagickFail;
                  break;
                }
              
              switch (dispatch_type)
                {
                case BGRDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(p->blue);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->red);
                        p++;
                      }
                    break;
                  }
                case BGRODispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(p->blue);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=ScaleQuantumToChar(MaxRGB-p->opacity);
                        p++;
                      }
                    break;
                  }
                case BGRPDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(p->blue);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=0;
                        p++;
                      }
                    break;
                  }
                case RGBDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->blue);
                        p++;
                      }
                    break;
                  }
                case RGBODispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->blue);
                        *q++=ScaleQuantumToChar(MaxRGB-p->opacity);
                        p++;
                      }
                    break;
                  }
                case IDispatchType:
                  {
                    if (image->is_grayscale)
                      {
                        for (x=(long) columns; x != 0; x--)
                          {
                            *q++=ScaleQuantumToChar(p->red);
                            p++;
                          }
                      }
                    else
                      {
                        for (x=(long) columns; x != 0; x--)
                          {
                            *q++=ScaleQuantumToChar(PixelIntensity(p));
                            p++;
                          }
                      }
                    break;
                  }
                case UndefinedDispatchType:
                  {
                  }
                }
            }
          return (status);
        }
    }

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
        case 'T':
          {
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'C':
          {
            switch_map[i]=RedMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'M':
          {
            switch_map[i]=GreenMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'Y':
          {
            switch_map[i]=BlueMapQuanum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'K':
          {
            switch_map[i]=OpacityMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'I':
          {
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        case 'O':
          {
            switch_map[i]=OpacityInvertedMapQuantum;
            break;
          }
        case 'P':
          {
            switch_map[i]=PadMapQuantum;
            break;
          }
        default:
          {
            ThrowException(exception,OptionError,UnrecognizedPixelMap,map);
            return(MagickFail);
          }
        }
    }
  
  for (y=0; y < (long) rows; y++)
    {
      p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
            {
              /*
                Obtain quantum value
              */
              quantum=0U;
              switch (switch_map[i])
                {
                case RedMapQuantum:
                  {
                    quantum=p->red;
                    break;
                  }
                case GreenMapQuantum:
                  {
                    quantum=p->green;
                    break;
                  }
                case BlueMapQuanum:
                  {
                    quantum=p->blue;
                    break;
                  }
                case IntensityMapQuantum:
                  {
                    if (image->is_grayscale)
                      {
                        quantum=p->red;
                      }
                    else
                      {
                        double intensity = PixelIntensity(p);
                        quantum=RoundToQuantum(intensity);
                      }
                    break;
                  }
                case OpacityInvertedMapQuantum:
                  {
                    if (image->matte)
                      quantum=p->opacity;
                    quantum=MaxRGB-quantum;
                    break;
                  }
                case OpacityMapQuantum:
                  {
                    if (image->matte)
                      quantum=p->opacity;
                    break;
                  }
                case PadMapQuantum:
                  {
                    /* Zero quantum */
                    break;
                  }
                }

              /*
                Output quantum
              */
              switch (type)
                {
                case CharPixel:
                  {
                    register unsigned char *q = pixels;
                    *q++=ScaleQuantumToChar(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case ShortPixel:
                  {
                    register unsigned short *q = pixels;
                    *q++=ScaleQuantumToShort(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case IntegerPixel:
                  {
                    register unsigned int *q = pixels;
                    *q++=ScaleQuantumToLong(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case LongPixel:
                  {
                    register unsigned long *q = pixels;
                    *q++=ScaleQuantumToLong(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case FloatPixel:
                  {
                    register float *q = pixels;
                    *q++=(float) ((double) quantum/MaxRGB);
                    pixels=(void *) q;
                    break;
                  }
                case DoublePixel:
                  {
                    register double *q = pixels;
                    *q++=(double) quantum/MaxRGB;
                    pixels=(void *) q;
                    break;
                  }
                }
            }
          p++;
        }
    }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p o r t I m a g e P i x e l A r e a                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExportImagePixelArea() transfers one or more pixel components from the
%  image pixel cache to a user supplied buffer.  MagickPass is returned if
%  the pixels are successfully transferred, otherwise MagickFail.
%
%  The format of the ExportImagePixelArea method is:
%
%      MagickPassFail ExportImagePixelArea(const Image *image,
%                                          const QuantumType quantum_type,
%                                          unsigned int quantum_size,
%                                          unsigned char *destination)
%
%  A description of each parameter follows:
%
%    o status: Returns MagickPass if the pixels are successfully transferred,
%              otherwise MagickFail.
%
%    o image: The image.
%
%    o quantum_type: Declare which pixel components to transfer (RGB, RGBA, etc).
%
%    o quantum_size: Bits per quantum sample.
%
%    o destination:  The components are transferred to this buffer.
%
%
*/
#define ExportModulo8Quantum(q,quantum_size,quantum) \
{ \
  register unsigned int \
    shift=quantum_size; \
\
  do \
    { \
      shift -= 8U; \
      *q++=(unsigned char) (((unsigned int) quantum) >> shift); \
    } while( shift > 0U); \
}

MagickExport MagickPassFail ExportImagePixelArea(const Image *image,
  const QuantumType quantum_type,const unsigned int quantum_size,
  unsigned char *destination)
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
    pixel,
    sample_scale = 1U;

  long
    number_pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(destination != (unsigned char *) NULL);

  if (QuantumDepth == quantum_size)
    {
    }
  else if (QuantumDepth > quantum_size)
    {
      /* Divide to scale down */
      sample_scale=(MaxRGB / (MaxRGB >> (QuantumDepth-quantum_size)));
    }
  else if (QuantumDepth < quantum_size)
    {
      /* Multiply to scale up */
      sample_scale=(MaxRGBGivenBits(quantum_size)/ MaxRGB);
    }

  number_pixels=(long) GetPixelCacheArea(image);
  p=GetPixels(image);
  indexes=GetIndexes(image);
  q=destination;
  switch (quantum_type)
    {
    case IndexQuantum:
      {
        assert(image->colors <= MaxColormapSize);
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            for (x = number_pixels; x > 0; --x)
              {
                ExportModulo8Quantum(q,quantum_size,*indexes);
                indexes++;
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;
            
            BitStreamInitializeWrite(&stream,q);
            for (x = number_pixels; x > 0; --x)
              {
                BitStreamMSBWrite(&stream,quantum_size,*indexes);
                indexes++;
              }
          }
        break;
      }
    case IndexAlphaQuantum:
      {
        assert(image->colors <= MaxColormapSize);
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            for (x = number_pixels; x > 0; --x)
              {
                ExportModulo8Quantum(q,quantum_size,*indexes);
                pixel=MaxRGB-p->opacity;
                if (QuantumDepth >  quantum_size)
                  pixel /= sample_scale;
                else if (QuantumDepth <  quantum_size)
                  pixel *= sample_scale;
                ExportModulo8Quantum(q,quantum_size,pixel);
                indexes++;
                p++;
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;
            
            BitStreamInitializeWrite(&stream,q);
            for (x = number_pixels; x > 0; --x)
              {
                BitStreamMSBWrite(&stream,quantum_size,*indexes);
                pixel=MaxRGB-p->opacity;
                if (QuantumDepth >  quantum_size)
                  pixel /= sample_scale;
                else if (QuantumDepth <  quantum_size)
                  pixel *= sample_scale;
                BitStreamMSBWrite(&stream,quantum_size,pixel);
                indexes++;
                p++;
              }
          }
        break;
      }
    case GrayInvertedQuantum:
    case GrayQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel /= sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel *= sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    BitStreamMSBWrite(&stream,quantum_size,pixel);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel /= sample_scale;
                    BitStreamMSBWrite(&stream,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel *= sample_scale;
                    BitStreamMSBWrite(&stream,quantum_size,pixel);
                    p++;
                  }
              }
          }
        break;
      }
    case GrayAlphaQuantum:
    case GrayInvertedAlphaQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=MaxRGB-p->opacity;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel /= sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=(MaxRGB-p->opacity)/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel *= sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=(MaxRGB-p->opacity)*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;
            
            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    BitStreamMSBWrite(&stream,quantum_size,pixel);
                    BitStreamMSBWrite(&stream,quantum_size,MaxRGB-p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel /= sample_scale;
                    BitStreamMSBWrite(&stream,quantum_size,pixel);
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-p->opacity)/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    if (image->is_grayscale)
                      pixel=p->red;
                    else
                      pixel=PixelIntensityToQuantum(p);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      pixel=MaxRGB-pixel;
                    pixel *= sample_scale;
                    BitStreamMSBWrite(&stream,quantum_size,pixel);
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-p->opacity)*sample_scale);
                    p++;
                  }
              }
            
          }
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->red);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->red);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->red/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->red*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->green);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->green/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->green*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->green);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->green/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->green*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->blue);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->blue/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->blue*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->blue);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->blue/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->blue*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case AlphaQuantum:
      {
        if (image->colorspace == CMYKColorspace)
          {
            if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
              {
                /*
                  Modulo-8 sample sizes
                */
                if( QuantumDepth == quantum_size)
                  {
                    /* Unity scale */
                    for (x = number_pixels; x > 0; --x)
                      {
                        pixel=MaxRGB-*indexes;
                        ExportModulo8Quantum(q,quantum_size,pixel);
                        indexes++;
                      }
                  }
                else if (QuantumDepth >  quantum_size)
                  {
                    /* Scale down */
                    for (x = number_pixels; x > 0; --x)
                      {
                        pixel=(MaxRGB-*indexes)/sample_scale;
                        ExportModulo8Quantum(q,quantum_size,pixel);
                        indexes++;
                      }
                  }
                else
                  {
                    /* Scale up */
                    for (x = number_pixels; x > 0; --x)
                      {
                        pixel=(MaxRGB-*indexes)*sample_scale;
                        ExportModulo8Quantum(q,quantum_size,pixel);
                        indexes++;
                      }
                  }
              }
            else
              {
                /*
                  Arbitrary sample size
                */
                BitStreamWriteHandle
                  stream;

                BitStreamInitializeWrite(&stream,q);
                if( QuantumDepth == quantum_size)
                  {
                    /* Unity scale */
                    for (x = number_pixels; x > 0; --x)
                      {
                        BitStreamMSBWrite(&stream,quantum_size,
                                          MaxRGB-*indexes);
                        indexes++;
                      }
                  }
                else if (QuantumDepth >  quantum_size)
                  {
                    /* Scale down */
                    for (x = number_pixels; x > 0; --x)
                      {
                        BitStreamMSBWrite(&stream,quantum_size,
                                          (MaxRGB-*indexes)/sample_scale);
                        indexes++;
                      }
                  }
                else
                  {
                    /* Scale up */
                    for (x = number_pixels; x > 0; --x)
                      {
                        BitStreamMSBWrite(&stream,quantum_size,
                                          (MaxRGB-*indexes)*sample_scale);
                        indexes++;
                      }
                  }
              }
            break;
          }

        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,MaxRGB-p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=(MaxRGB-p->opacity)/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=(MaxRGB-p->opacity)*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      MaxRGB-p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-p->opacity)/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-p->opacity)*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case BlackQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->opacity/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->opacity*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->opacity/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,
                                      p->opacity*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case RGBQuantum:
    default:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->red);
                    ExportModulo8Quantum(q,quantum_size,p->green);
                    ExportModulo8Quantum(q,quantum_size,p->blue);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red);
                    BitStreamMSBWrite(&stream,quantum_size,p->green);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case RGBAQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->red);
                    ExportModulo8Quantum(q,quantum_size,p->green);
                    ExportModulo8Quantum(q,quantum_size,p->blue);
                    pixel=MaxRGB-p->opacity;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=(MaxRGB-p->opacity)/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=(MaxRGB-p->opacity)*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red);
                    BitStreamMSBWrite(&stream,quantum_size,p->green);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue);
                    BitStreamMSBWrite(&stream,quantum_size,MaxRGB-p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-p->opacity)/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-p->opacity)*sample_scale);
                    p++;

                  }
              }
          }
        break;
      }
    case CMYKQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->red);
                    ExportModulo8Quantum(q,quantum_size,p->green);
                    ExportModulo8Quantum(q,quantum_size,p->blue);
                    ExportModulo8Quantum(q,quantum_size,p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->opacity/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->opacity*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red);
                    BitStreamMSBWrite(&stream,quantum_size,p->green);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue);
                    BitStreamMSBWrite(&stream,quantum_size,p->opacity);
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->opacity/sample_scale);
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->opacity*sample_scale);
                    p++;
                  }
              }
          }
        break;
      }
    case CMYKAQuantum:
      {

        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ExportModulo8Quantum(q,quantum_size,p->red);
                    ExportModulo8Quantum(q,quantum_size,p->green);
                    ExportModulo8Quantum(q,quantum_size,p->blue);
                    ExportModulo8Quantum(q,quantum_size,p->opacity);
                    pixel=MaxRGB-*indexes;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    indexes++;
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->opacity/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=(MaxRGB-*indexes)/sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    indexes++;
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    pixel=p->red*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->green*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->blue*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=p->opacity*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    pixel=(MaxRGB-*indexes)*sample_scale;
                    ExportModulo8Quantum(q,quantum_size,pixel);
                    indexes++;
                    p++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamWriteHandle
              stream;

            BitStreamInitializeWrite(&stream,q);
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red);
                    BitStreamMSBWrite(&stream,quantum_size,p->green);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue);
                    BitStreamMSBWrite(&stream,quantum_size,p->opacity);
                    BitStreamMSBWrite(&stream,quantum_size,MaxRGB-*indexes);
                    indexes++;
                    p++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->opacity/sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-*indexes)/sample_scale);
                    indexes++;
                    p++;
                  }
              }
            else
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    BitStreamMSBWrite(&stream,quantum_size,p->red*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->green*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->blue*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,p->opacity*sample_scale);
                    BitStreamMSBWrite(&stream,quantum_size,
                                      (MaxRGB-*indexes)*sample_scale);
                    indexes++;
                    p++;
                  }
              }
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
%   I m p o r t I m a g e P i x e l A r e a                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImagePixelArea() transfers one or more pixel components from a user
%  supplied buffer into the image pixel cache of an image. The pixels are
%  read in network (big-endian) byte order. It returns MagickPass if the
%  pixels are successfully transferred, otherwise MagickFail.
%
%  The format of the ImportImagePixelArea method is:
%
%      MagickPassFail ImportImagePixelArea(Image *image,
%                                          const QuantumType quantum_type,
%                                          unsigned int quantum_size,
%                                          const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns MagickPass if the pixels are
%      successfully transferred, otherwise MagickFail.
%
%    o image: The image.
%
%    o quantum_type: Declare which pixel components to transfer (AlphaQuantum,
%        BlackQuantum, BlueQuantum, CMYKAQuantum, CMYKQuantum, CyanQuantum,
%        GrayAlphaQuantum, GrayQuantum, GreenQuantum, IndexAlphaQuantum,
%        IndexQuantum, MagentaQuantum, RGBAQuantum, RGBQuantum,
%        RedQuantum, YellowQuantum)
%
%    o quantum_size: Bits per quantum sample (range 1-31)
%
%    o source:  The pixel components are transferred from this buffer.
%
*/
#define ImportModulo8Quantum(quantum,quantum_size,p) \
{ \
  register unsigned int \
    shift=quantum_size; \
\
  quantum=0; \
  do \
    { \
      shift -= 8U; \
      quantum |= (*p++ << shift); \
    } while( shift > 0U); \
}

MagickExport MagickPassFail ImportImagePixelArea(Image *image,
  const QuantumType quantum_type,const unsigned int quantum_size,
  const unsigned char *source)
{
  register const unsigned char
    *p;

  register unsigned int
    index,
    quantum,
    quantum_scale = 1U;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  long
    number_pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(source != (const unsigned char *) NULL);

  if (QuantumDepth == quantum_size)
    {
    }
  else if (QuantumDepth > quantum_size)
    {
      /* Multiply to scale up */
      quantum_scale=(MaxRGB / (MaxRGB >> (QuantumDepth-quantum_size)));
    }
  else if (QuantumDepth < quantum_size)
    {
      /* Divide to scale down */
      quantum_scale=(MaxRGBGivenBits(quantum_size)/ MaxRGB);
    }

  number_pixels=(long) GetPixelCacheArea(image);
  p=source;
  q=GetPixels(image);
  indexes=GetIndexes(image);
  switch (quantum_type)
    {
    case IndexQuantum:
      {
        assert(image->colors <= MaxColormapSize);

        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            for (x = number_pixels; x > 0; --x)
              {
                ImportModulo8Quantum(index,quantum_size,p);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q++=image->colormap[index];
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            for (x = number_pixels; x > 0; --x)
              {
                index=BitStreamMSBRead(&stream,quantum_size);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q++=image->colormap[index];
              }
          }
        break;
      }
    case IndexAlphaQuantum:
      {
        assert(image->colors <= MaxColormapSize);
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            for (x = number_pixels; x > 0; --x)
              {
                ImportModulo8Quantum(index,quantum_size,p);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q=image->colormap[index];

                ImportModulo8Quantum(quantum,quantum_size,p);
                if (QuantumDepth >  quantum_size)
                  quantum *= quantum_scale;
                else if (QuantumDepth <  quantum_size)
                  quantum /= quantum_scale;
                q->opacity=MaxRGB-quantum;
                q++;
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;
            
            BitStreamInitializeRead(&stream,p);
            for (x = number_pixels; x > 0; --x)
              {
                index=BitStreamMSBRead(&stream,quantum_size);
                VerifyColormapIndex(image,index);
                *indexes++=index;
                *q=image->colormap[index];

                quantum=BitStreamMSBRead(&stream,quantum_size);
                if (QuantumDepth >  quantum_size)
                  quantum *= quantum_scale;
                else if (QuantumDepth <  quantum_size)
                  quantum /= quantum_scale;
                q->opacity=MaxRGB-quantum;
                q++;
              }
          }
        break;
      }
    case GrayInvertedQuantum:
    case GrayQuantum:
      {
        register unsigned int
          indexes_scale = 1U;

        assert(image->colors <= MaxColormapSize);

        if (MaxRGBGivenBits(quantum_size) > MaxColormapSize)
          indexes_scale=(1U << quantum_size) / image->colors + 1;

        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if (indexes_scale == 1U)
              {
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(index,quantum_size,p);
                    VerifyColormapIndex(image,index);
                    if (GrayInvertedQuantum == quantum_type)
                      index=(image->colors-1)-index;
                    *indexes++=index;
                    *q++=image->colormap[index];
                  }
              }
            else
              {
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(index,quantum_size,p);
                    index /= indexes_scale;
                    VerifyColormapIndex(image,index);
                    if (GrayInvertedQuantum == quantum_type)
                      index=(image->colors-1)-index;
                    *indexes++=index;
                    *q++=image->colormap[index];
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;
            
            BitStreamInitializeRead(&stream,p);
            for (x = number_pixels; x > 0; --x)
              {
                index=BitStreamMSBRead(&stream,quantum_size);
                index /= indexes_scale;
                VerifyColormapIndex(image,index);
                if (GrayInvertedQuantum == quantum_type)
                  index=(image->colors-1)-index;
                *indexes++=index;
                *q++=image->colormap[index];
              }
          }
        break;
      }
    case GrayInvertedAlphaQuantum:
    case GrayAlphaQuantum:
      {
        /*
          Input is organized as a gray level followed by opacity level
          Colormap array is pre-stuffed with ascending or descending gray
          levels according to the gray quantum representation.
        */
        register unsigned int
          indexes_scale = 1U;

        assert(image->colors <= MaxColormapSize);

        if (MaxRGBGivenBits(quantum_size) > MaxColormapSize)
          indexes_scale=(1U << quantum_size) / image->colors + 1;

        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if (indexes_scale == 1U)
              {
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(index,quantum_size,p);
                    VerifyColormapIndex(image,index);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      index=(image->colors-1)-index;
                    *indexes++=index;
                    *q=image->colormap[index];
                    
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    if (QuantumDepth >  quantum_size)
                      quantum *= quantum_scale;
                    else if (QuantumDepth <  quantum_size)
                      quantum /= quantum_scale;
                    q->opacity=MaxRGB-quantum;
                    q++;
                  }
              }
            else
              {
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(index,quantum_size,p);
                    index /= indexes_scale;
                    VerifyColormapIndex(image,index);
                    if (GrayInvertedAlphaQuantum == quantum_type)
                      index=(image->colors-1)-index;
                    *indexes++=index;
                    *q=image->colormap[index];
                    
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    if (QuantumDepth >  quantum_size)
                      quantum *= quantum_scale;
                    else if (QuantumDepth <  quantum_size)
                      quantum /= quantum_scale;
                    q->opacity=MaxRGB-quantum;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;
            
            BitStreamInitializeRead(&stream,p);
            for (x = number_pixels; x > 0; --x)
              {
                index=BitStreamMSBRead(&stream,quantum_size);
                index /= indexes_scale;
                VerifyColormapIndex(image,index);
                if (GrayInvertedAlphaQuantum == quantum_type)
                  index=(image->colors-1)-index;
                *indexes++=index;
                *q=image->colormap[index];

                quantum=BitStreamMSBRead(&stream,quantum_size);
                if (QuantumDepth >  quantum_size)
                  quantum *= quantum_scale;
                else if (QuantumDepth <  quantum_size)
                  quantum /= quantum_scale;
                q->opacity=MaxRGB-quantum;
                q++;
              }
          }
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->green=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->green=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->blue=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->blue=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case AlphaQuantum:
      {
        if (image->colorspace == CMYKColorspace)
          {
            if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
              {
                /*
                  Modulo-8 sample sizes
                */
                if( QuantumDepth == quantum_size)
                  {
                    /* Unity scale */
                    for (x = number_pixels; x > 0; --x)
                      {
                        ImportModulo8Quantum(quantum,quantum_size,p);
                        *indexes++=quantum;
                      }
                  }
                else if (QuantumDepth >  quantum_size)
                  {
                    /* Scale up */
                    for (x = number_pixels; x > 0; --x)
                      {
                        ImportModulo8Quantum(quantum,quantum_size,p);
                        *indexes++=quantum*quantum_scale;
                      }
                  }
                else
                  {
                    /* Scale down */
                    for (x = number_pixels; x > 0; --x)
                      {
                        ImportModulo8Quantum(quantum,quantum_size,p);
                        *indexes++=quantum/quantum_scale;
                      }
                  }
              }
            else
              {
                /*
                  Arbitrary sample size
                */
                BitStreamReadHandle
                  stream;
                
                BitStreamInitializeRead(&stream,p);
                if (QuantumDepth >=  quantum_size)
                  {
                    /* Scale up */
                    for (x = number_pixels; x > 0; --x)
                      {
                        *indexes++=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                      }
                  }
                else
                  {
                    /* Scale down */
                    for (x = number_pixels; x > 0; --x)
                      {
                        *indexes++=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                      }
                  }
              }
            break;
          }

        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case BlackQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case RGBQuantum:
    default:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum;
                    /* q->opacity=0U; */
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum*quantum_scale;
                    /* q->opacity=0U; */
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum/quantum_scale;
                    /* q->opacity=0U; */
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    /* q->opacity=0U; */
                    q++;
                  }
              }
            else if (QuantumDepth <  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    /* q->opacity=0U; */
                    q++;
                  }
              }
          }
        break;
      }
    case RGBAQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=MaxRGB-quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=MaxRGB-(quantum*quantum_scale);
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=MaxRGB-(quantum/quantum_scale);
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->opacity=MaxRGB-BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else if (QuantumDepth <  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->opacity=MaxRGB-BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case CMYKQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else if (QuantumDepth <  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
          }
        break;
      }
    case CMYKAQuantum:
      {
        if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
          {
            /*
              Modulo-8 sample sizes
            */
            if( QuantumDepth == quantum_size)
              {
                /* Unity scale */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    *indexes++=(IndexPacket) MaxRGB-quantum;
                    q++;
                  }
              }
            else if (QuantumDepth >  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum*quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    *indexes++=(IndexPacket) MaxRGB-quantum*quantum_scale;
                    q++;
                  }
              }
            else
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->red=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->green=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->blue=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    q->opacity=quantum/quantum_scale;
                    ImportModulo8Quantum(quantum,quantum_size,p);
                    *indexes++=(IndexPacket) MaxRGB-quantum/quantum_scale;
                    q++;
                  }
              }
          }
        else
          {
            /*
              Arbitrary sample size
            */
            BitStreamReadHandle
              stream;

            BitStreamInitializeRead(&stream,p);
            if (QuantumDepth >=  quantum_size)
              {
                /* Scale up */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    *indexes++=BitStreamMSBRead(&stream,quantum_size)*quantum_scale;
                    q++;
                  }
              }
            else if (QuantumDepth <  quantum_size)
              {
                /* Scale down */
                for (x = number_pixels; x > 0; --x)
                  {
                    q->red=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->green=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->blue=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q->opacity=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    *indexes++=BitStreamMSBRead(&stream,quantum_size)/quantum_scale;
                    q++;
                  }
              }
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
static void RemoveTemporaryInputFile(ImageInfo *image_info)
{
  int
    filename_length;

  /*
    Remove normal file name.
  */
  if(!LiberateTemporaryFile(image_info->filename))
    (void) remove(image_info->filename);

  /*
    Remove a .cache file corresponding to a .mpc file.
    This stupidity is necessary because MPC "files" are comprised of two
    separate files.
  */
  filename_length=strlen(image_info->filename);
  if ((filename_length > 4) &&
      (LocaleCompare(image_info->filename+filename_length-4,".mpc") == 0))
    {
      char remove_name[MaxTextExtent];
      (void) strcpy(remove_name,image_info->filename);
      remove_name[filename_length-4]=0;
      (void) strcat(remove_name,".cache");
      (void) printf("removing %s\n", remove_name);
      (void) remove(remove_name);
    }
  else if (LocaleCompare(image_info->magick,"mpc") == 0)
    {
      char remove_name[MaxTextExtent];
      (void) strcpy(remove_name,image_info->filename);
      (void) strcat(remove_name,".cache");
      (void) printf("removing %s\n", remove_name);
      (void) remove(remove_name);
    }

  errno=0;
}

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

  /*
    Obtain file magick from filename
  */
  (void) SetImageInfo(clone_info,False,exception);
  (void) strncpy(filename,clone_info->filename,MaxTextExtent-1);
  (void) strncpy(magick,clone_info->magick,MaxTextExtent-1);
  /*
    Call appropriate image reader based on image type.
  */
  {
    ExceptionInfo
      module_exception,
      delegate_exception;
    
    GetExceptionInfo(&module_exception);
    GetExceptionInfo(&delegate_exception);
    magick_info=GetMagickInfo(clone_info->magick,&module_exception);
    delegate_info=(const DelegateInfo *) NULL;
    if ((magick_info == (const MagickInfo *) NULL) ||
        (magick_info->decoder == NULL))
      delegate_info=GetDelegateInfo(clone_info->magick,(char *) NULL,&delegate_exception);
    
    if (((magick_info == (const MagickInfo *) NULL) ||
         (magick_info->decoder == NULL)) &&
        ((delegate_info == (const DelegateInfo *) NULL) ||
         (delegate_info->decode == NULL)))
      {
        /*
          Module loader ConfigureError errors are intentionally
          ignored here in order to provide the user with familiar "no
          delegate" error messages.  This may be re-considered later.
        */
        if ((module_exception.severity != UndefinedException) &&
            (module_exception.severity != ConfigureError))
          CopyException(exception,&module_exception);
        else if (delegate_exception.severity != UndefinedException)
          CopyException(exception,&delegate_exception);
        else
          {
            /*
              Try to choose a useful error type
            */
            if (clone_info->filename[0] == 0)
              {
                errno=0;
                ThrowException(exception,MissingDelegateError,
                               NoDecodeDelegateForThisImageFormat,clone_info->magick);
              }
            else if (IsAccessibleAndNotEmpty(clone_info->filename))
              {
                errno=0;
                ThrowException(exception,MissingDelegateError,
                               NoDecodeDelegateForThisImageFormat,clone_info->filename);
              }
            else
              {
              ThrowException(exception,FileOpenError,UnableToOpenFile,
                             clone_info->filename);
              }
          }
        DestroyExceptionInfo(&module_exception);
        DestroyExceptionInfo(&delegate_exception);
        if (clone_info->temporary)
          RemoveTemporaryInputFile(clone_info);
        DestroyImageInfo(clone_info);
        return((Image *) NULL);
      }
    
    DestroyExceptionInfo(&module_exception);
    DestroyExceptionInfo(&delegate_exception);
  }

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
      if (!BlobIsSeekable(image))
        {
          /*
            Coder requires a random access stream.
          */
          if(!AcquireTemporaryFileName(clone_info->filename))
            {
              ThrowException(exception,FileOpenError,
                UnableToCreateTemporaryFile,clone_info->filename);
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
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s)",magick_info->name,
        magick_info->description);
      image=(magick_info->decoder)(clone_info,exception);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Returned from \"%.1024s\" decoder",magick_info->name);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  else
    {
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          if (clone_info->temporary)
            RemoveTemporaryInputFile(clone_info);
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
          ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,
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
      /*
        If there is no magick info entry for this format, or there is
        no decoder for the format, or an error is reported, then
        attempt to return a reasonable error report.
      */
      if ((magick_info == (const MagickInfo *) NULL) ||
          (magick_info->decoder == NULL) ||
          (exception->severity != UndefinedException))
        {
          if (exception->severity == UndefinedException)
          {
            if (IsAccessibleAndNotEmpty(clone_info->filename))
              ThrowException(exception,MissingDelegateError,
                NoDecodeDelegateForThisImageFormat,clone_info->filename);
            else
              ThrowException(exception,FileOpenError,UnableToOpenFile,
                clone_info->filename);
          }
          if (clone_info->temporary)
            RemoveTemporaryInputFile(clone_info);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      /*
        Invoke decoder for format
      */
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s)",magick_info->name,
        magick_info->description);
      image=(magick_info->decoder)(clone_info,exception);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
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
      RemoveTemporaryInputFile(clone_info);
      clone_info->temporary=False;
      if (image != (Image *) NULL)
        (void) strncpy(image->filename,filename,MaxTextExtent-1);
    }
  if (image == (Image *) NULL)
    {
      DestroyImageInfo(clone_info);
      return(image);
    }
  if (GetBlobTemporary(image))
    RemoveTemporaryInputFile(clone_info);
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
                       SubimageSpecificationReturnsNoImages,
                       clone_info->filename);
      else
        {
          while (subimages->previous != (Image *) NULL)
            subimages=subimages->previous;
          DestroyImageList(image);
          image=subimages;
        }
    }
  if (GetBlobStatus(image))
    {
      ThrowException(exception,CorruptImageError,
                     AnErrorHasOccurredReadingFromFile,
                     clone_info->filename);
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
    if (GetBlobTemporary(image))
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
  if (images == (char **) NULL)
    return((Image *) NULL);
  MagickFreeMemory(command);
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
    MagickFreeMemory(images[i]);
  MagickFreeMemory(images);
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
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  p++;
  blob=Base64Decode(p,&length);
  if (length == 0)
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  image=BlobToImage(image_info,blob,length,exception);
  (void) SetMonitorHandler(handler);
  MagickFreeMemory(blob);
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
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" encoder (%.1024s)",magick_info->name,
        magick_info->description);
      status=(magick_info->encoder)(clone_info,image);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
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
              ThrowException(&image->exception,FileOpenError,UnableToCreateTemporaryFile,image->filename);
              DestroyImageInfo(clone_info);
              return(False);
            }
          status=InvokeDelegate(clone_info,image,(char *) NULL,
            clone_info->magick,&image->exception);
          (void) LiberateTemporaryFile(image->filename);
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
          ThrowBinaryException(MissingDelegateError,NoEncodeDelegateForThisImageFormat,image->filename)
        }
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      status=(magick_info->encoder)(clone_info,image);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  (void) strncpy(image->magick,clone_info->magick,MaxTextExtent-1);
  DestroyImageInfo(clone_info);
  if (GetBlobStatus(image))
    ThrowBinaryException(CorruptImageError,AnErrorHasOccurredWritingToFile,
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
      CopyException(exception,&p->exception);
    GetImageException(p,exception);
    if (image_info->adjoin)
      break;
  }
  if (image_info->verbose)
    (void) DescribeImage(image,stdout,False);
  return(status);
}
