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
#include "cache.h"

/*
  Declare pixel cache interfaces.
*/
static IndexPacket
  *GetIndexesFromCache(const Image *);

static PixelPacket
  GetOnePixelFromCache(Image *,const int,const int),
  *GetPixelsFromCache(const Image *),
  *GetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *SetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int);

static unsigned int
  SyncPixelCache(Image *);

static void
  ClosePixelCache(Image *),
  DestroyPixelCache(Image *);

/*
  Initialize the image pixel methods.
*/
Export IndexPacket
  *(*GetIndexes)(const Image *) = GetIndexesFromCache;

Export PixelPacket
  *(*GetImagePixels)(Image *,const int,const int,const unsigned int,
    const unsigned int) = GetPixelCache,
  (*GetOnePixel)(Image *,const int,const int) = GetOnePixelFromCache,
  *(*GetPixels)(const Image *) = GetPixelsFromCache,
  *(*SetImagePixels)(Image *,const int,const int,const unsigned int,
    const unsigned int) = SetPixelCache;

Export unsigned int
  (*SyncImagePixels)(Image *) = SyncPixelCache;

Export void
  (*CloseImagePixels)(Image *) = ClosePixelCache,
  (*DestroyImagePixels)(Image *) = DestroyPixelCache;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClosePixelCache closes the pixel cache.  Use this method to prevent
%  the too many file descriptors from being allocated when reading an image
%  sequence.  File descriptors are only used for a disk-based cache.  This is
%  essentially a no-op for a memory-based cache.
%
%  The format of the ClosePixelCache method is:
%
%      void ClosePixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void ClosePixelCache(Image *image)
{
  assert(image != (Image *) NULL);
  if (image->cache == (void *) NULL)
    return;
  CloseCache(image->cache);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y P i x e l C a c h e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPixelCache deallocates memory associated with the pixel cache.
%
%  The format of the DestroyPixelCache method is:
%
%      void DestroyPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void DestroyPixelCache(Image *image)
{
  assert(image != (Image *) NULL);
  if (image->cache == (void *) NULL)
    return;
  DestroyCacheInfo(image->cache);
  image->cache=(Cache) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I n d e x e s F r o m C a c h e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetIndexesFromCache returns the colormap indexes associated with
%  the last call to the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetIndexesFromCache method is:
%
%      IndexPacket *GetIndexesFromCache(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexesFromCache returns the colormap indexes
%      associated with the last call to the SetPixelCache() or GetPixelCache()
%      methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
static IndexPacket *GetIndexesFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  return(GetNexusIndexes(image->cache,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t O n e P i x e l F r o m C a c h e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOnePixelFromCache returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixelFromCache method is:
%
%      PixelPacket *GetOnePixelFromCache(const Image image,const int x,
%        const int y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixelFromCache returns a pixel at the
%      specified (x,y) location.
%
%    o image: The address of a structure of type Image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
static PixelPacket GetOnePixelFromCache(Image *image,const int x,const int y)
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
+   G e t P i x e l C a c h e                                                 %
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
static PixelPacket *GetPixelCache(Image *image,const int x,const int y,
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
        "Unable to get pixels from cache",image->filename);
      return((PixelPacket *) NULL);
    }
  return(pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t P i x e l s F r o m C a c h e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelsFromCache returns the pixels associated with the last call
%  to the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetPixelsFromCache method is:
%
%      PixelPacket *GetPixelsFromCache(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixelsFromCache returns the pixels associated with
%      the last call to the SetPixelCache() or GetPixelCache() methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
static PixelPacket *GetPixelsFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  return(GetNexusPixels(image->cache,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t P i x e l C a c h e                                                 %
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
static PixelPacket *SetPixelCache(Image *image,const int x,const int y,
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
  if (image->cache == (Cache) NULL)
    GetCacheInfo(&image->cache);
  if (image->class != GetCacheClassType(image->cache))
    {
      /*
        Allocate pixel cache.
      */
      status=OpenCache(image->cache,image->class,image->columns,image->rows);
      if (status == False)
        {
          ThrowException(&image->exception,CacheWarning,
            "Unable to open pixel cache",image->filename);
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
%   S e t P i x e l C a c h e M e t h o d s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetPixelCacheMethods set the default image pixel methods to
%  use the pixel cache.
%
%
*/
Export void SetPixelCacheMethods(void)
{
  /*
    Reset image pixel methods.
  */
  CloseImagePixels=ClosePixelCache;
  DestroyImagePixels=DestroyPixelCache;
  GetImagePixels=GetPixelCache;
  GetIndexes=GetIndexesFromCache;
  GetOnePixel=GetOnePixelFromCache;
  GetPixels=GetPixelsFromCache;
  SetImagePixels=SetPixelCache;
  SyncImagePixels=SyncPixelCache;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c P i x e l C a c h e                                               %
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
static unsigned int SyncPixelCache(Image *image)
{
  unsigned int
    status;

  /*
    Transfer pixels to the cache.
  */
  assert(image != (Image *) NULL);
  if (image->cache == (Cache) NULL)
    ThrowBinaryException(CacheWarning,"pixel cache is not open",
      image->filename);
  image->taint=True;
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
