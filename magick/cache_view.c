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
%                      GraphicsMagick Cache View Methods                      %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               February 2000                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/cache.h"
#include "magick/cache_view.h"
#include "magick/utility.h"

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
%  The format of the AcquireCacheView method is:
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
%  CloseCacheView() closes the specified view returned by a previous call
%  to OpenCacheView().
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
  MagickFreeMemory(view);
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
%  GetCacheView() gets pixels from the in-memory or disk pixel cache as
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
%  GetCacheViewPixels() returns the pixels associated with the specified
%  specified view.
%
%  The format of the GetCacheViewPixels method is:
%
%      PixelPacket *GetCacheViewPixels(const ViewInfo *view)
%
%  A description of each parameter follows:
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
%  OpenCacheView() opens a view into the pixel cache.
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
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateCacheView");
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
%  SetCacheView() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the SetCacheView method is:
%
%      PixelPacket *SetCacheView(ViewInfo *view,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
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
%  SyncCacheView() saves the view pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncCacheView method is:
%
%      unsigned int SyncCacheView(ViewInfo *view)
%
%  A description of each parameter follows:
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
