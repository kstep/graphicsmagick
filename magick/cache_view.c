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
%   A c q u i r e C a c h e V i e w                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquireCacheView gets pixels from the in-memory or disk pixel cache
%  as defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetCacheView method is:
%
%      const PixelPacket *AcquireCacheView(const ViewInfo *view,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        ExceptionInfo *exception)
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
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const PixelPacket *AcquireCacheView(const ViewInfo *view,
  const long x,const long y,const unsigned long columns,
  const unsigned long rows,ExceptionInfo *exception)
{
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  return(AcquireCacheNexus(view->image,x,y,columns,rows,view->id,exception));
}

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
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  DestroyCacheNexus(view->image->cache,view->id);
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
%      PixelPacket *GetCacheView(ViewInfo *view,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
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
MagickExport PixelPacket *GetCacheView(ViewInfo *view,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  return(GetCacheNexus(view->image,x,y,columns,rows,view->id));
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
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  return(GetNexusIndexes(view->image->cache,view->id));
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
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  return(GetNexusPixels(view->image->cache,view->id));
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
  ViewInfo
    *view;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  view=(ViewInfo *) AcquireMemory(sizeof(ViewInfo));
  if (view == (ViewInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate cache view",
      "Memory allocation failed");
  (void) memset(view,0,sizeof(ViewInfo));
  view->id=GetNexus(image->cache);
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
%      PixelPacket *SetCacheView(ViewInfo *view,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
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
MagickExport PixelPacket *SetCacheView(ViewInfo *view,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  return(SetCacheNexus(view->image,x,y,columns,rows,view->id));
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
  assert(view != (ViewInfo *) NULL);
  assert(view->signature == MagickSignature);
  return(SyncCacheNexus(view->image,view->id));
}
