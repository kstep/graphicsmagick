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
%                        V   V  IIIII  EEEEE  W   W                           %
%                        V   V    I    E      W   W                           %
%                        V   V    I    EEE    W W W                           %
%                         V V     I    E      WW WW                           %
%                          V    IIIII  EEEEE  W   W                           %
%                                                                             %
%                       ImageMagick Cache View Methods                        %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               February 2000                                 %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e C a c h e V i e w                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloseCacheView closes the specified view returned by a previous
%  call to OpenCacheView().
%
%  The format of the CloseCacheView method is:
%
%      void CloseCacheView(ViewInfo *view)
%
%  A description of each parameter follows:
%
%    o view: The address of a structure of type ViewInfo.
%
*/
MagickExport void CloseCacheView(ViewInfo *view)
{
  Image
    *image;

  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  image=view->image;
  DestroyCacheNexus(image->cache,view->id);
  LiberateMemory((void **) &view);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e V i e w                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheView gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetCacheView method is:
%
%      PixelPacket *GetCacheView(ViewInfo *view,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetCacheView returns a null pointer if an error
%      occurs, otherwise a pointer to the view pixels.
%
%    o view: The address of a structure of type ViewInfo.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
MagickExport PixelPacket *GetCacheView(ViewInfo *view,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  Image
    *image;

  PixelPacket
    *pixels;

  unsigned int
    status;

  /*
    Read pixels from the cache.
  */
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  pixels=SetCacheView(view,x,y,columns,rows);
  if (pixels == (PixelPacket *) NULL)
    return((PixelPacket *) NULL);
  image=view->image;
  if (IsNexusInCore(image->cache,view->id))
    return(pixels);
  status=ReadCachePixels(image->cache,view->id);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    status|=ReadCacheIndexes(image->cache,view->id);
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
%   G e t C a c h e V i e w I n d e x e s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheViewIndexes returns the indexes associated with the specified
%  view.
%
%  The format of the GetCacheViewIndexes method is:
%
%      IndexPacket *GetCacheViewIndexes(const ViewInfo *view)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetCacheViewIndexes returns the indexes associated with
%      the specified view.
%
%    o view: The address of a structure of type ViewInfo.
%
%
*/
MagickExport IndexPacket *GetCacheViewIndexes(const ViewInfo *view)
{
  Image
    *image;

  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  image=view->image;
  return(GetNexusIndexes(image->cache,view->id));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e V i e w P i x e l s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheViewPixels returns the pixels associated with the specified
%  specified view.
%
%  The format of the GetCacheViewPixels method is:
%
%      PixelPacket *GetCacheViewPixels(const ViewInfo *view)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetCacheViewPixels returns the pixels associated with
%      the specified view.
%
%    o view: The address of a structure of type ViewInfo.
%
%
*/
MagickExport PixelPacket *GetCacheViewPixels(const ViewInfo *view)
{
  Image
    *image;

  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  image=view->image;
  return(GetNexusPixels(image->cache,view->id));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n C a c h e V i e w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenCacheView opens a view into the pixel cache.
%
%  The format of the OpenCacheView method is:
%
%      ViewInfo *OpenCacheView(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport ViewInfo *OpenCacheView(Image *image)
{
  unsigned int
    status;

  ViewInfo
    *view;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->cache == (Cache) NULL)
    GetCacheInfo(&image->cache);
  if ((image->storage_class != GetCacheClass(image->cache)) ||
      (image->colorspace != GetCacheColorspace(image->cache)))
    {
      /*
        Allocate pixel cache.
      */
      status=OpenCache(image);
      if (status == False)
        {
          ThrowException(&image->exception,CacheWarning,
            "Unable to allocate pixel cache",image->filename);
          return((ViewInfo *) NULL);
        }
    }
  view=(ViewInfo *) AcquireMemory(sizeof(ViewInfo));
  if (view == (ViewInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate cache view",
      "Memory allocation failed");
  view->id=GetCacheNexus(image->cache);
  view->image=image;
  view->signature=MagickSignature;
  if (view->id == 0)
    {
      CloseCacheView(view);
      return((ViewInfo *) NULL);
    }
  return(view);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C a c h e V i e w                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetCacheView gets pixels from the in-memory or disk pixel cache
%  as defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the SetCacheView method is:
%
%      PixelPacket *SetCacheView(ViewInfo *view,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetCacheView returns a null pointer if an error
%      occurs, otherwise a pointer to the view pixels.
%
%    o view: The address of a structure of type ViewInfo.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
MagickExport PixelPacket *SetCacheView(ViewInfo *view,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  Image
    *image;

  RectangleInfo
    region;

  /*
    Validate pixel cache geometry.
  */
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  image=view->image;
  if (image->cache == (Cache) NULL)
    ThrowBinaryException(CacheWarning,"pixel cache is undefined",
      image->filename);
  if ((x < 0) || (y < 0) || ((x+(int) columns) > (int) image->columns) ||
      ((y+(int) rows) > (int) image->rows) || (columns == 0) || (rows == 0))
    {
      ThrowException(&image->exception,CacheWarning,"Unable to set pixel cache",
        "image does not contain the cache geometry");
      return((PixelPacket *) NULL);
    }
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  return(SetCacheNexus(image,view->id,&region));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c C a c h e V i e w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncCacheView saves the view pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncCacheView method is:
%
%      unsigned int SyncCacheView(ViewInfo *view)
%
%  A description of each parameter follows:
%
%    o status: Method SyncCacheView returns True if the view pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o view: The address of a structure of type ViewInfo.
%
%
*/
MagickExport unsigned int SyncCacheView(ViewInfo *view)
{
  Image
    *image;

  unsigned int
    status;

  /*
    Transfer pixels to the cache.
  */
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  image=view->image;
  if (image->cache == (Cache) NULL)
    ThrowBinaryException(CacheWarning,"pixel cache is undefined",
      image->filename);
  image->taint=True;
  if (IsNexusInCore(image->cache,view->id))
    return(True);
  status=WriteCachePixels(image->cache,view->id);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    status|=WriteCacheIndexes(image->cache,view->id);
  if (status == False)
    ThrowBinaryException(CacheWarning,"Unable to sync pixel cache",
      image->filename);
  return(True);
}
