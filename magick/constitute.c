/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
%                                                                             %
%   C o n s t i t u t e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConstituteImage is a convenience routine that creates an image from
%  the pixel data you supply and returns it.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.  The
%  pixel data must be in scanline order top-to-bottom.  The data can be
%  character, short int, integer, float, or double.  Float and double require
%  the pixels to be normalized [0..1].  The other types are [0..MaxRGB].  For
%  example, to create a 640x480 image from unsigned red-green-blue character
%  data, use
%
%      image=ConstituteImage(640,480,"RGB",0,pixels,&exception);
%
%  The format of the Constitute method is:
%
%      Image *ConstituteImage(const unsigned int width,
%        const unsigned int height,const char *map,const StorageType type,
%        const void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method Constitute returns a pointer to the image.  A null
%      image is returned if there is a memory shortage or if the image cannot
%      be read.
%
%    o width: Specifies the width in pixels of the image.
%
%    o height: Specifies the height in pixels of the image.
%
%    o map:  This character string can be any combination or order of
%      R = red, G = green, B = blue, A = alpha, C = cyan, Y = yellow,
%      M = magenta, and K = black.  The ordering reflects the order of the
%      pixels in the supplied pixel array.
%
%    o type: pixel type where 0 = unsigned char, 1 = short int, 2 = int,
%      3 = float, and 4 = double.  Float and double types are expected to be
%      normalized [0..1] otherwise [0..MaxRGB].
%
%    o pixels: This array of values contain the pixel components as defined
%      by the map and type parameters.  The length of the arrays must
%      equal the area specified by the width and height values and type
%      parameters.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ConstituteImage(const unsigned int width,
  const unsigned int height,const char *map,const StorageType type,
  const void *pixels,ExceptionInfo *exception)
{
  Image
    *image;

  int
    y;

  register int
    i,
    x;

  PixelPacket
    *q;

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
  for (i=0; i < strlen(map); i++)
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          for (i=0; i < strlen(map); i++)
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
              case 'y':
              case 'Y':
              {
                q->green=(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'm':
              case 'M':
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
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map);
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          for (i=0; i < strlen(map); i++)
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
              case 'y':
              case 'Y':
              {
                q->green=(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'm':
              case 'M':
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
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map);
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          for (i=0; i < strlen(map); i++)
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
              case 'y':
              case 'Y':
              {
                q->green=(*p++);
                break;
              }
              case 'b':
              case 'B':
              case 'm':
              case 'M':
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
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map);
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          for (i=0; i < strlen(map); i++)
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
              case 'y':
              case 'Y':
              {
                q->green=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'b':
              case 'B':
              case 'm':
              case 'M':
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
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map);
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          for (i=0; i < strlen(map); i++)
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
              case 'y':
              case 'Y':
              {
                q->green=(Quantum) (MaxRGB*(*p++));
                break;
              }
              case 'b':
              case 'B':
              case 'm':
              case 'M':
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
              default:
              {
                DestroyImage(image);
                ThrowImageException(OptionWarning,"Invalid pixel map",map);
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
      ThrowImageException(OptionWarning,"Invalid pixel map",map);
    }
  }
  return(image);
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
%  Method DispatchImage is a convenience routine.  Use it to extract pixel
%  data from an image and place it in a buffer you supply.  The data is saved
%  either as char, short int, integer, float or double format in the order
%  specified by the type parameter.  For example, we want to extract
%  scanline 1 of a 640x480 image as character data in red-green-blue order:
%
%      DispatchImage(image,0,0,640,1,"RGB",0,pixels);
%
%  The format of the DispatchImage method is:
%
%      unsigned int DispatchImage(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows,
%        const char *map,const StorageType type,void *pixels)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels you want to extract.
%
%    o map:  This character string can be any combination or order of
%      R = red, G = green, B = blue, A = alpha, C = cyan, Y = yellow,
%      M = magenta, and K = black.  The ordering reflects the order of the
%      pixels in the supplied pixel array.
%
%    o type: pixel type where 0 = unsigned char, 1 = short int, 2 = int,
%      3 = float, and 4 = double.  Float and double types are expected to
%      be normalized [0..1] otherwise [0..MaxRGB].
%
%    o pixels: This array of values contain the pixel components as defined
%      by the map and type parameters.  The length of the arrays must
%      equal the area specified by the width and height values and type
%      parameters.
%
%
*/
MagickExport unsigned int DispatchImage(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows,const char *map,
  const StorageType type,void *pixels)
{
  register int
    i,
    j;

  register PixelPacket
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (i=0; i < strlen(map); i++)
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
        if (image->colorspace != CMYKColorspace)
          RGBTransformImage(image,CMYKColorspace);
        break;
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

      p=GetImagePixels(image,x,y,columns,rows);
      if (p == (PixelPacket *) NULL)
        break;
      q=(unsigned char *) pixels;
      for (i=0; i < (columns*rows); i++)
      {
        for (j=0; j < strlen(map); j++)
        {
          switch (map[j])
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
            case 'y':
            case 'Y':
            {
              *q++=p->green;
              break;
            }
            case 'b':
            case 'B':
            case 'm':
            case 'M':
            {
              *q++=p->blue;
              break;
            }
            case 'a':
            case 'A':
            {
              if (!image->matte)
                *q++=MaxRGB;
              else
                *q++=p->opacity;
              break;
            }
            case 'k':
            case 'K':
            {
              *q++=p->opacity;
              break;
            }
            default:
              ThrowBinaryException(OptionWarning,"Invalid pixel map",map);
          }
        }
        p++;
      }
      break;
    }
    case ShortPixel:
    {
      register unsigned short
        *q;

      p=GetImagePixels(image,x,y,columns,rows);
      if (p == (PixelPacket *) NULL)
        break;
      q=(unsigned short *) pixels;
      for (i=0; i < (columns*rows); i++)
      {
        for (j=0; j < strlen(map); j++)
        {
          switch (map[j])
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
            case 'y':
            case 'Y':
            {
              *q++=p->green;
              break;
            }
            case 'b':
            case 'B':
            case 'm':
            case 'M':
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
            default:
              ThrowBinaryException(OptionWarning,"Invalid pixel map",map);
          }
        }
        p++;
      }
      break;
    }
    case IntegerPixel:
    {
      register unsigned int
        *q;

      p=GetImagePixels(image,x,y,columns,rows);
      if (p == (PixelPacket *) NULL)
        break;
      q=(unsigned int *) pixels;
      for (i=0; i < (columns*rows); i++)
      {
        for (j=0; j < strlen(map); j++)
        {
          switch (map[j])
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
            case 'y':
            case 'Y':
            {
              *q++=p->green;
              break;
            }
            case 'b':
            case 'B':
            case 'm':
            case 'M':
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
            default:
              ThrowBinaryException(OptionWarning,"Invalid pixel map",map);
          }
        }
        p++;
      }
      break;
    }
    case FloatPixel:
    {
      register float
        *q;

      p=GetImagePixels(image,x,y,columns,rows);
      if (p == (PixelPacket *) NULL)
        break;
      q=(float *) pixels;
      for (i=0; i < (columns*rows); i++)
      {
        for (j=0; j < strlen(map); j++)
        {
          switch (map[j])
          {
            case 'r':
            case 'R':
            case 'c':
            case 'C':
            {
              *q++=p->red/MaxRGB;
              break;
            }
            case 'g':
            case 'G':
            case 'y':
            case 'Y':
            {
              *q++=p->green/MaxRGB;
              break;
            }
            case 'b':
            case 'B':
            case 'm':
            case 'M':
            {
              *q++=p->blue/MaxRGB;
              break;
            }
            case 'a':
            case 'A':
            case 'k':
            case 'K':
            {
              *q++=p->opacity/MaxRGB;
              break;
            }
            default:
              ThrowBinaryException(OptionWarning,"Invalid pixel map",map);
          }
        }
        p++;
      }
      break;
    }
    case DoublePixel:
    {
      register double
        *q;

      p=GetImagePixels(image,x,y,columns,rows);
      if (p == (PixelPacket *) NULL)
        break;
      q=(double *) pixels;
      for (i=0; i < (columns*rows); i++)
      {
        for (j=0; j < strlen(map); j++)
        {
          switch (map[j])
          {
            case 'r':
            case 'R':
            case 'c':
            case 'C':
            {
              *q++=p->red/MaxRGB;
              break;
            }
            case 'g':
            case 'G':
            case 'y':
            case 'Y':
            {
              *q++=p->green/MaxRGB;
              break;
            }
            case 'b':
            case 'B':
            case 'm':
            case 'M':
            {
              *q++=p->blue/MaxRGB;
              break;
            }
            case 'a':
            case 'A':
            case 'k':
            case 'K':
            {
              *q++=p->opacity/MaxRGB;
              break;
            }
            default:
              ThrowBinaryException(OptionWarning,"Invalid pixel map",map);
          }
        }
        p++;
      }
      break;
    }
    default:
      ThrowBinaryException(OptionWarning,"Invalid pixel map",map);
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
%  Method PingImage returns the image size in bytes if it exists and can be
%  read (at %  least up until it reveals it's size).  The width and height of
%  the image is returned as well.  Note, only the first image in a multi-frame
%  image file is pinged.
%
%  The format of the PingImage method is:
%
%      Image *PingImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o Image: Method PingImage returns the image size in bytes if the
%      image file exists and it size can be determined otherwise 0.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *PingImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *ping_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  ping_info=CloneImageInfo(image_info);
  ping_info->ping=True;
  ping_info->verbose=False;
  ping_info->subimage=0;
  ping_info->subrange=0;
  image=ReadImage(ping_info,exception);
  DestroyImageInfo(ping_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if (image_info->verbose)
    DescribeImage(image,stdout,False);
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
%    o image: The address of a structure of type Image.
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

  register int
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
          for (x=0; x < (int) image->columns; x++)
            *q++=indexes[x];
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=indexes[x] >> 8;
        *q++=indexes[x];
      }
      break;
    }
    case IndexOpacityQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=indexes[x];
            *q++=MaxRGB-p->opacity;
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
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
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=(unsigned char) Intensity(*p);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=(unsigned char) (Intensity(*p)/256);
        *q++=(unsigned char) Intensity(*p);
        p++;
      }
      break;
    }
    case GrayOpacityQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=(unsigned char) Intensity(*p);
            *q++=MaxRGB-p->opacity;
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=(unsigned char) (Intensity(*p)/256);
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
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red;
        p++;
      }
      break;
    }
    case GreenQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->green);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->green >> 8;
        *q++=p->green;
        p++;
      }
      break;
    }
    case BlueQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->blue);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->blue >> 8;
        *q++=p->blue;
        p++;
      }
      break;
    }
    case OpacityQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(MaxRGB-p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
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
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->opacity >> 8;
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
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
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
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            *q++=DownScale(MaxRGB-p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
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
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            *q++=DownScale(p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
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
%      unsigned int PushImagePixels(const Image *image,
%        const QuantumType quantum,const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns True if the pixels are
%      successfully transferred, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o quantum: Declare which pixel components to transfer (red, green, blue,
%      opacity, RGB, or RGBA).
%
%    o source:  The pixel components are transferred from this buffer.
%
*/
MagickExport unsigned int PushImagePixels(const Image *image,
  const QuantumType quantum,const unsigned char *source)
{
  IndexPacket
    index;

  register const unsigned char
    *p;

  register IndexPacket
    *indexes;

  register int
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
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*p++);
            indexes[x]=index;
            *q++=image->colormap[index];
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8);
        index|=(*p++);
        indexes[x]=index;
        *q++=image->colormap[index];
      }
      break;
    }
    case IndexOpacityQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*p++);
            indexes[x]=index;
            *q=image->colormap[index];
            q->opacity=UpScale(MaxRGB-*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8);
        index|=(*p++);
        indexes[x]=index;
        *q=image->colormap[index];
        q->opacity=(MaxRGB-(*p++)) << 8;
        q->opacity|=MaxRGB-(*p++);
        q++;
      }
      break;
    }
    case GrayQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*p++);
            indexes[x]=index;
            *q++=image->colormap[index];
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8);
        index|=(*p++);
        indexes[x]=index;
        *q++=image->colormap[index];
      }
      break;
    }
    case GrayOpacityQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*p++);
            indexes[x]=index;
            *q=image->colormap[index];
            q->opacity=MaxRGB-(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8);
        index|=(*p++);
        indexes[x]=index;
        *q=image->colormap[index];
        q->opacity=(MaxRGB-(*p++)) << 8;
        q->opacity|=MaxRGB-(*p++);
        q++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8);
        q->red|=(*p++);
        q++;
      }
      break;
    }
    case GreenQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->green=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->green=(*p++ << 8);
        q->green|=(*p++);
        q++;
      }
      break;
    }
    case BlueQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->blue=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->blue=(*p++ << 8);
        q->blue|=(*p++);
        q++;
      }
      break;
    }
    case OpacityQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->opacity=UpScale(MaxRGB-*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->opacity=(MaxRGB-(*p++)) << 8;
        q->opacity|=MaxRGB-(*p++);
        q++;
      }
      break;
    }
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->opacity=(*p++ << 8);
        q->opacity|=(*p++);
        q++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8);
        q->red|=(*p++);
        q->green=(*p++ << 8);
        q->green|=(*p++);
        q->blue=(*p++ << 8);
        q->blue|=(*p++);
        q++;
      }
      break;
    }
    case RGBAQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->opacity=UpScale(MaxRGB-*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8);
        q->red|=(*p++);
        q->green=(*p++ << 8);
        q->green|=(*p++);
        q->blue=(*p++ << 8);
        q->blue|=(*p++);
        q->opacity=(MaxRGB-(*p++)) << 8;
        q->opacity|=MaxRGB-(*p++);
        q++;
      }
      break;
    }
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8);
        q->red|=(*p++);
        q->green=(*p++ << 8);
        q->green|=(*p++);
        q->blue=(*p++ << 8);
        q->blue|=(*p++);
        q->opacity=(*p++ << 8);
        q->opacity|=(*p++);
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
%  Method ReadImage reads an image and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.  By default, the image format is determined by its magic
%  number. To specify a particular image format, precede the filename with an
%  explicit image format name and a colon (i.e.  ps:image) or as the filename
%  suffix  (i.e. image.ps).
%
%  The format of the ReadImage method is:
%
%      Image *ReadImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReadImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent];

  DelegateInfo
    delegate_info;

  Image
    *image,
    *next;

  ImageInfo
    *clone_info;

  MagickInfo
    *magick_info;

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
  clone_info=CloneImageInfo(image_info);
  if (*clone_info->filename == '@')
    return(ReadImages(clone_info,exception));
  SetImageInfo(clone_info,False);
  (void) strcpy(filename,clone_info->filename);
  /*
    Call appropriate image reader based on image type.
  */
  image=(Image *) NULL;
  magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick);
  if ((magick_info != (MagickInfo *) NULL) &&
      (magick_info->decoder !=
        (Image *(*)(const ImageInfo *,ExceptionInfo *)) NULL))
    image=(magick_info->decoder)(clone_info,exception);
  else
    if (!GetDelegateInfo(clone_info->magick,(char *) NULL,&delegate_info))
      {
        ThrowException(exception,MissingDelegateWarning,
          "no delegate for this image format",clone_info->filename)
        DestroyImageInfo(clone_info);
        return((Image *) NULL);
      }
    else
      {
        unsigned int
          status;

        /*
          Let our decoding delegate process the image.
        */
        image=AllocateImage(clone_info);
        if (image == (Image *) NULL)
          return((Image *) NULL);
        (void) strcpy(image->filename,clone_info->filename);
        TemporaryFilename(clone_info->filename);
        status=
          InvokeDelegate(clone_info,image,clone_info->magick,(char *) NULL);
        ThrowException(exception,image->exception.severity,
           image->exception.message,image->exception.qualifier);
        DestroyImages(image);
        image=(Image *) NULL;
        if (status != False)
          clone_info->temporary=True;
        SetImageInfo(clone_info,False);
        magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick);
        if ((magick_info == (MagickInfo *) NULL) ||
            (magick_info->decoder ==
              (Image *(*)(const ImageInfo *,ExceptionInfo *)) NULL))
          {
            ThrowException(exception,MissingDelegateWarning,
              "no delegate for this image format",clone_info->filename);
            DestroyImageInfo(clone_info);
            return((Image *) NULL);
          }
        image=(magick_info->decoder)(clone_info,exception);
      }
  if (clone_info->temporary)
    {
      (void) remove(clone_info->filename);
      clone_info->temporary=False;
      if (image != (Image *) NULL)
        (void) strcpy(image->filename,filename);
    }
  if (image == (Image *) NULL)
    return(image);
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

      unsigned int
        last,
        target;

      /*
        User specified subimages (e.g. image.miff[1,3-5,7-6,2]).
      */
      subimages=(Image *) NULL;
      target=atoi(clone_info->tile);
      for (p=clone_info->tile; *p != '\0'; p+=Max(offset,1))
      {
        offset=0;
        count=sscanf(p,"%u%n-%u%n",&target,&offset,&last,&offset);
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

            if (next->scene != target)
              continue;
            /*
              Clone this subimage.
            */
            clone_image=
              CloneImage(next,next->columns,next->rows,True,exception);
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
  DestroyBlobInfo(&image->blob);
  for (next=image; next; next=next->next)
  {
    GetBlobInfo(&next->blob);
    next->taint=False;
    (void) strcpy(next->magick_filename,clone_info->filename);
    if (image->temporary)
      (void) strcpy(next->filename,clone_info->filename);
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
%   R e a d I m a g e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadImages reads a list of image names from a file and then returns
%  the images as a linked list.
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
MagickExport Image *ReadImages(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    *command,
    **images;

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    c,
    length,
    number_images;

  register char
    *p;

  register Image
    *next;

  register int
    i;

  /*
    Read image list from a file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  file=(FILE *) fopen(image_info->filename+1,"r");
  if (file == (FILE *) NULL)
    {
      ThrowException(exception,ResourceLimitWarning,"Unable to read image list",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  length=MaxTextExtent;
  command=(char *) AcquireMemory(length);
  for (p=command; command != (char *) NULL; p++)
  {
    c=fgetc(file);
    if (c == EOF)
      break;
    if ((p-command+1) >= length)
      {
        *p='\0';
        length<<=1;
        ReacquireMemory((void **) &command,length);
        if (command == (char *) NULL)
          break;
        p=command+Extent(command);
      }
    *p=c;
  }
  (void) fclose(file);
  if (command == (char *) NULL)
    {
      ThrowException(exception,ResourceLimitWarning,"Unable to read image list",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  *p='\0';
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
    (void) strcpy(clone_info->filename,images[i]);
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
%  Method WriteImage writes an image to a file as defined by image->filename.
%  You can specify a particular image format by prefixing the file with the
%  image type and a colon (i.e. ps:image) or specify the image type as the
%  filename suffix (i.e. image.ps).  The image may be modified to adapt it
%  to the requirements of the image format.  For example, DirectClass images
%  must be color-reduced to PseudoClass if the format is GIF.
%
%  The format of the WriteImage method is:
%
%      unsigned int WriteImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/
MagickExport unsigned int WriteImage(const ImageInfo *image_info,Image *image)
{
  DelegateInfo
    delegate_info;

  ImageInfo
    *clone_info;

  MagickInfo
    *magick_info;

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
  (void) strcpy(clone_info->filename,image->filename);
  (void) strcpy(clone_info->magick,image->magick);
  SetImageInfo(clone_info,True);
  (void) strcpy(image->filename,clone_info->filename);
  if ((image->next == (Image *) NULL) || clone_info->adjoin)
    if ((image->previous == (Image *) NULL) && !IsTainted(image))
      if (IsAccessible(image->magick_filename))
        if (GetDelegateInfo(image->magick,clone_info->magick,&delegate_info))
          if (delegate_info.direction == 0)
            {
              /*
                Let our bi-directional delegate process the image.
              */
              (void) strcpy(image->filename,image->magick_filename);
              status=InvokeDelegate(clone_info,image,image->magick,
                clone_info->magick);
              DestroyImageInfo(clone_info);
              return(!status);
            }
  /*
    Call appropriate image writer based on image type.
  */
  status=False;
  magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick);
  if ((magick_info != (MagickInfo *) NULL) &&
      (magick_info->encoder !=
        (unsigned int (*)(const ImageInfo *,Image *)) NULL))
    status=(magick_info->encoder)(clone_info,image);
  else
    if (!GetDelegateInfo((char *) NULL,clone_info->magick,&delegate_info))
      {
        magick_info=(MagickInfo *) GetMagickInfo(image->magick);
        if ((magick_info == (MagickInfo *) NULL) ||
            (magick_info->encoder ==
              (unsigned int (*)(const ImageInfo *,Image *)) NULL))
          ThrowBinaryException(MissingDelegateWarning,
            "no encode delegate for this image format",clone_info->magick);
        status=(magick_info->encoder)(clone_info,image);
      }
    else
      {
        /*
          Let our encoding delegate process the image.
        */
        TemporaryFilename(image->filename);
        status=
          InvokeDelegate(clone_info,image,(char *) NULL,clone_info->magick);
        (void) remove(image->filename);
        DestroyImageInfo(clone_info);
        return(!status);
      }
  (void) strcpy(image->magick,clone_info->magick);
  DestroyImageInfo(clone_info);
  if (image->status)
    ThrowBinaryException(CorruptImageWarning,
      "An error has occurred writing to file",image->filename);
  return(status);
}
