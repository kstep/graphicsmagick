/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
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
%  This example illustrates the use of the GetPixelCache() method:
%
%    for (y=0; y < (int) image->rows; y++)
%    {
%      q=GetPixelCache(image,0,y,image->columns,1);
%      if (q == (PixelPacket *) NULL)
%        break;
%      for (x=0; x < (int) image->columns; x++)
%      {
%        if (image->class == PseudoClass)
%          index=image->indexes[x];
%        red=q->red;
%        green=q->green;
%        blue=q->blue;
%        q++;
%      }
%      if (image->previous == (Image *) NULL)
%        if (QuantumTick(y,image->rows))
%          ProgressMonitor(LoadImageText,y,image->rows);
%    }
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

inline unsigned int MatchesLastRegionRequest(Image *image,const int x,
  const int y,const unsigned int columns,const unsigned int rows)
{
  if ((x == image->cache_info.x) && (y == image->cache_info.y) &&
      (columns == image->cache_info.width) &&
      (rows == image->cache_info.height))
    return(True);
  return(False);
}

Export PixelPacket *GetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  RectangleInfo
    region_info;

  unsigned int
    status;

  /*
    Transfer pixels from the pixel cache.
  */
  assert(image != (Image *) NULL);
  if (MatchesLastRegionRequest(image,x,y,columns,rows))
    return(image->pixels);
  if (!SetPixelCache(image,x,y,columns,rows))
    return((PixelPacket *) NULL);
  region_info.x=x;
  region_info.y=y;
  region_info.height=rows;
  region_info.width=columns;
  status=ReadCachePixels(image->cache_handle,&region_info,image->pixels);
  if (status == False)
    {
      MagickWarning(CacheWarning,"Unable to read pixels from cache",
        (char *) NULL);
      return((PixelPacket *) NULL);
    }
  if (image->class == PseudoClass)
    {
      /*
        Transfer colormap indexes from the pixel cache.
      */
      status=ReadCacheIndexes(image->cache_handle,&region_info,image->indexes);
      if (status == False)
        {
          MagickWarning(CacheWarning,"Unable to read indexes from cache",
            (char *) NULL);
          return((PixelPacket *) NULL);
        }
    }
  return(image->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p e n P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenPixelCache returns True if the pixel cache is already
%  opened.  Otherwise the pixel cache is created in memory or disk.
%  False is returned if there is not enough memory or a disk error
%  occurs.
%
%  The format of the OpenPixelCache method is:
%
%      unsigned int OpenPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method OpenPixelCache returns True if the image pixel
%      cache is already created or succesfully created in memory or on
%      disk.
%
%    o image: The address of a structure of type Image.
%
%
*/
static unsigned int OpenPixelCache(Image *image)
{
  unsigned int
    status;

  assert(image != (Image *) NULL);
  status=InitializePixelCache(image->cache_handle,image->class,image->columns,
    image->rows);
  return(status);
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
%      unsigned int ReadPixelCache(Image *image,const QuantumTypes quantum,
%        const unsigned char *source)
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
unsigned int ReadPixelCache(Image *image,const QuantumTypes quantum,
  const unsigned char *source)
{
  register const unsigned char
    *p;

  register IndexPacket
    index;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(source != (const unsigned char *) NULL);
  p=source;
  q=image->pixels;
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*p++);
            image->indexes[x]=index;
            *q++=image->colormap[index];
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8);
	index|=(*p++);
        image->indexes[x]=index;
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
            image->indexes[x]=index;
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
        image->indexes[x]=index;
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
            index=UpScale(*p++);
            image->indexes[x]=index;
            *q++=image->colormap[index];
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8);
	index|=(*p++);
        image->indexes[x]=index;
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
            index=UpScale(*p++);
            image->indexes[x]=index;
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
        image->indexes[x]=index;
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
%  subsequently transferred from the pixel cache with the SyncPixelCache.
%  A pointer to the pixels is returned if the pixels are transferred,
%  otherwise a NULL is returned.
%
%  This example illustrates the use of the SetPixelCache method:
%
%    for (y=0; y < (int) image->rows; y++)
%    {
%      q=SetPixelCache(image,0,y,image->columns,1);
%      if (q == (PixelPacket *) NULL)
%        break;
%      for (x=0; x < (int) image->columns; x++)
%      {
%        if (image->class == PseudoClass)
%          image->indexes[x]=0;
%        q->red=0;
%        q->green=0;
%        q->blue=0;
%        q++;
%      }
%      if (!SyncPixelCache(image))
%        break;
%      if (image->previous == (Image *) NULL)
%        if (QuantumTick(y,image->rows))
%          ProgressMonitor(LoadImageText,y,image->rows);
%    }
%
%  The format of the SetPixelCache method is:
%
%      PixelPacket *SetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method SetPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/

inline unsigned int MatchesLastRegionRequestSize(Image *image,
  const size_t number_pixels)
{
  if ((image->cache_info.width*image->cache_info.height) < number_pixels)
    return(True);
  return(False);
}

Export PixelPacket *SetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  size_t
    number_pixels;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+columns) > (int) image->columns) ||
      ((y+rows) > (int) image->rows) || (columns == 0) || (rows == 0))
    {
      MagickWarning(CacheWarning,"Unable to set pixel cache",
        "image does not contain the cache geometry");
      return((PixelPacket *) NULL);
    }
  if (!OpenPixelCache(image))
    {
      MagickWarning(CacheWarning,"Unable to initialize pixel cache",
        (char *) NULL);
      return((PixelPacket *) NULL);
    }
  /*
    Allocate buffer to get/put pixels from the pixel cache.
  */
  number_pixels=columns*rows;
  if (image->pixels == (PixelPacket *) NULL)
    image->pixels=(PixelPacket *)
      AllocateMemory(number_pixels*sizeof(PixelPacket));
  else
    if (MatchesLastRegionRequestSize(image,number_pixels))
      image->pixels=(PixelPacket *)
        ReallocateMemory(image->pixels,number_pixels*sizeof(PixelPacket));
  /*
    Allocate buffer to get/put colormap indexes from the pixel cache.
  */
  if (image->class == PseudoClass)
    if (image->indexes == (IndexPacket *) NULL)
      image->indexes=(IndexPacket *)
        AllocateMemory(number_pixels*sizeof(IndexPacket));
  if (image->indexes != (IndexPacket *) NULL)
    if (MatchesLastRegionRequestSize(image,number_pixels))
      image->indexes=(IndexPacket *) ReallocateMemory(image->indexes,
        number_pixels*sizeof(IndexPacket));
  if (((image->class == PseudoClass) &&
       (image->indexes == (IndexPacket *) NULL)) ||
      (image->pixels == (PixelPacket *) NULL))
    {
      MagickWarning(CacheWarning,"Unable to set pixel cache",
        "Memory allocation failed");
      return((PixelPacket *) NULL);
    }
  /*
    Set the pixel cache geometry.
  */
  image->cache_info.x=x;
  image->cache_info.y=y;
  image->cache_info.width=columns;
  image->cache_info.height=rows;
  return(image->pixels);
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
%  Method SyncPixelCache saves the image pixels to the in-memory or disk
%  cache.  The method returns True if the pixel region is set, otherwise False.
%  See the SetPixelCache method for an example usage of SyncPixelCache.
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
  register PixelPacket
    *p;

  unsigned int
    status;

  assert(image != (Image *) NULL);
  p=SetPixelCache(image,image->cache_info.x,image->cache_info.y,
    image->cache_info.width,image->cache_info.height);
  if (p == (PixelPacket *) NULL)
    return(False);
  /*
    Transfer pixels to pixel cache.
  */
  image->tainted=True;
  status=WriteCachePixels(image->cache_handle,&image->cache_info,p);
  if (status == False)
    {
      MagickWarning(CacheWarning,"Unable to sync pixel cache",(char *) NULL);
      return(False);
    }
  if (image->class == PseudoClass)
    {
      /*
        Transfer colormap indexes to pixel cache.
      */
      status=WriteCacheIndexes(image->cache_handle,&image->cache_info,
        image->indexes);
      if (status == False)
        {
          MagickWarning(CacheWarning,"Unable to sync pixel cache",
            (char *) NULL);
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
%      unsigned int WritePixelCache(Image *,const QuantumTypes quantum,
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
unsigned int WritePixelCache(Image *image,const QuantumTypes quantum,
  unsigned char *destination)
{
  register int
    x;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  assert(image != (Image *) NULL);
  assert(destination != (unsigned char *) NULL);
  p=image->pixels;
  q=destination;
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          for (x=0; x < (int) image->columns; x++)
            *q++=image->indexes[x];
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=image->indexes[x] >> 8;
        *q++=image->indexes[x];
      }
      break;
    }
    case IndexOpacityQuantum:
    {
      if (image->depth <= 8)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=image->indexes[x];
            *q++=p->opacity;
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=image->indexes[x] >> 8;
        *q++=image->indexes[x];
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
