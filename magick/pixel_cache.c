/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     PPPP   IIIII  X   X  EEEEE  L                           %
%                     P   P    I     X X   E      L                           %
%                     PPPP     I      X    EEE    L                           %
%                     P        I     X X   E      L                           %
%                     P      IIIII  X   X  EEEEE  LLLLL                       %
%                                                                             %
%                      CCCC   AAA    CCCC  H   H  EEEEE                       %
%                     C      A   A  C      H   H  E                           %
%                     C      AAAAA  C      HHHHH  EEE                         %
%                     C      A   A  C      H   H  E                           %
%                      CCCC  A   A   CCCC  H   H  EEEEE                       %
%                                                                             %
%                                                                             %
%                      ImageMagick Pixel Cache Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
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
%                                                                             %
%   G e t I n d e x e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetIndexes returns the colormap indexes associated with the last call
%  to the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetIndexes method is:
%
%      IndexPacket *GetIndexes(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexes returns the colormap indexes associated with
%      the last call to the SetPixelCache() or GetPixelCache() methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export IndexPacket *GetIndexes(const Image *image)
{
  assert(image != (Image *) NULL);
  return(GetNexusIndexes(image->cache,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O n e P i x e l                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOnePixel returns a single pixel at the specified (x,y) location.
%  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixel method is:
%
%      PixelPacket *GetOnePixel(const Image image,const int x,const int y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixel returns a pixel at the specified (x,y)
%      location.
%
%    o image: The address of a structure of type Image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
Export PixelPacket GetOnePixel(Image *image,const int x,const int y)
{
  register PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
  pixel=GetPixelCache(image,x,y,1,1);
  if (pixel != (PixelPacket *) NULL)
    return(*pixel);
  return(image->background_color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelCache gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetPixelCache method is:
%
%      PixelPacket *GetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
Export PixelPacket *GetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  PixelPacket
    *pixels;

  unsigned int
    status;

  /*
    Transfer pixels from the cache.
  */
  assert(image != (Image *) NULL);
  pixels=SetPixelCache(image,x,y,columns,rows);
  if (pixels == (PixelPacket *) NULL)
    return((PixelPacket *) NULL);
  if (IsNexusInCore(image->cache,0))
    return(pixels);
  status=ReadCachePixels(image->cache,0);
  if (image->class == PseudoClass)
    status|=ReadCacheIndexes(image->cache,0);
  if (status == False)
    {
      ThrowException(&image->exception,CacheWarning,
        "Unable to read pixels from cache",image->filename);
      return((PixelPacket *) NULL);
    }
  return(pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P i x e l s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixels returns the pixels associated with the last call to the
%  SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetPixels method is:
%
%      PixelPacket *GetPixels(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixels returns the pixels associated with the
%      last call to the SetPixelCache() or GetPixelCache() methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export PixelPacket *GetPixels(const Image *image)
{
  assert(image != (Image *) NULL);
  return(GetNexusPixels(image->cache,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPixelCache transfers one or more pixel components from a buffer
%  or file into the image pixel buffer of an image.  It returns True if the
%  pixels are successfully transferred, otherwise False.
%
%  The format of the ReadPixelCache method is:
%
%      unsigned int ReadPixelCache(const Image *image,
%        const QuantumTypes quantum,const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o status: Method ReadPixelCache returns True if the pixels are
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
Export unsigned int ReadPixelCache(const Image *image,
  const QuantumTypes quantum,const unsigned char *source)
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
  assert(source != (const unsigned char *) NULL);
  p=source;
  q=GetNexusPixels(image->cache,0);
  indexes=GetNexusIndexes(image->cache,0);
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
            q->opacity=UpScale(*p++);
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
        q->opacity=(*p++ << 8);
        q->opacity|=(*p++);
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
            q->opacity=(*p++);
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
        q->opacity=(*p++ << 8);
        q->opacity|=(*p++);
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
%   S e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetPixelCache allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with method SyncPixelCache.
%  A pointer to the pixels is returned if the pixels are transferred,
%  otherwise a NULL is returned.
%
%  The format of the SetPixelCache method is:
%
%      PixelPacket *SetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
Export PixelPacket *SetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  RectangleInfo
    region;

  unsigned int
    status;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+columns) > (int) image->columns) ||
      ((y+rows) > (int) image->rows) || (columns == 0) || (rows == 0))
    {
      ThrowException(&image->exception,CacheWarning,"Unable to set pixel cache",
        "image does not contain the cache geometry");
      return((PixelPacket *) NULL);
    }
  if ((image->cache == (Cache) NULL) ||
      (image->class != GetCacheClassType(image->cache)))
    {
      /*
        Allocate pixel cache.
      */
      status=OpenCache(image->cache,image->class,image->columns,image->rows);
      if (status == False)
        {
          ThrowException(&image->exception,CacheWarning,
            "Unable to allocate pixel cache",image->filename);
          return((PixelPacket *) NULL);
        }
    }
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  return(SetCacheNexus(image->cache,0,&region));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncPixelCache saves the image pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncPixelCache method is:
%
%      unsigned int SyncPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelCache returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int SyncPixelCache(Image *image)
{
  unsigned int
    status;

  /*
    Transfer pixels to the cache.
  */
  assert(image != (Image *) NULL);
  if (image->cache == (Cache) NULL)
    ThrowBinaryException(CacheWarning,"pixel cache is not allocated",
      image->filename);
  image->taint=True;
  if (image->fifo != (void (*)(Image *)) NULL)
    image->fifo(image);
  if (IsNexusInCore(image->cache,0))
    return(True);
  status=WriteCachePixels(image->cache,0);
  if (image->class == PseudoClass)
    status|=WriteCacheIndexes(image->cache,0);
  if (status == False)
    ThrowBinaryException(CacheWarning,"Unable to sync pixel cache",
      image->filename);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePixelCache transfers one or more pixel components from the image
%  pixel buffer to a buffer or file. It returns True if the pixels are
%  successfully transferred, otherwise False.
%
%  The format of the WritePixelCache method is:
%
%      unsigned int WritePixelCache(const Image *,const QuantumTypes quantum,
%        unsigned char *destination)
%
%  A description of each parameter follows:
%
%    o status: Method WritePixelCache returns True if the pixels are
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
Export unsigned int WritePixelCache(const Image *image,
  const QuantumTypes quantum,unsigned char *destination)
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
  assert(destination != (unsigned char *) NULL);
  p=GetNexusPixels(image->cache,0);
  indexes=GetNexusIndexes(image->cache,0);
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
            *q++=p->opacity;
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=indexes[x] >> 8;
        *q++=indexes[x];
        *q++=p->opacity >> 8;
        *q++=p->opacity;
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
            *q++=Intensity(*p);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=Intensity(*p) >> 8;
        *q++=Intensity(*p);
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
            *q++=Intensity(*p);
            *q++=p->opacity;
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=Intensity(*p) >> 8;
        *q++=Intensity(*p);
        *q++=p->opacity >> 8;
        *q++=p->opacity;
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
