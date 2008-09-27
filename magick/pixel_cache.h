/*
  Copyright (C) 2003 - 2008 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Pixel Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#include "magick/forward.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  /*
    Enum declaractions.
  */

  typedef enum
    {
      UndefinedVirtualPixelMethod,
      ConstantVirtualPixelMethod,
      EdgeVirtualPixelMethod,
      MirrorVirtualPixelMethod,
      TileVirtualPixelMethod
    } VirtualPixelMethod;

  /*
    Typedef declaractions.
  */

  typedef _CacheInfoPtr_ Cache;
  typedef void *ViewInfo;


  /*****
   *
   * Default View interfaces (Single-threaded access)
   *
   *****/

  /*
    Read only access to a rectangular pixel region.
  */
  MagickExport const PixelPacket
  *AcquireImagePixels(const Image *image,const long x,const long y,
                      const unsigned long columns,
                      const unsigned long rows,ExceptionInfo *exception);

  /*
    Return one pixel at the the specified (x,y) location.
    Similar function as GetOnePixel();
  */
  extern MagickExport PixelPacket
  AcquireOnePixel(const Image *image,const long x,const long y,
                  ExceptionInfo *exception);

  /*
    DestroyImagePixels() deallocates memory associated with the pixel cache.

    Used only by DestroyImage().
  */
  extern MagickExport void
  DestroyImagePixels(Image *image);

  /*
    GetImagePixels() obtains a pixel region for read/write access.
  */
  extern MagickExport PixelPacket
  *GetImagePixels(Image *image,const long x,const long y,
                  const unsigned long columns,const unsigned long rows);

  /*
    GetImageVirtualPixelMethod() gets the "virtual pixels" method for
    the image.
  */
  extern MagickExport VirtualPixelMethod
  GetImageVirtualPixelMethod(const Image *image);

  /*
    GetIndexes() returns the colormap indexes associated with the last
    call to SetImagePixels() or GetImagePixels().
  */
  extern MagickExport IndexPacket
  *GetIndexes(const Image *image);

  /*
    GetOnePixel() returns a single pixel at the specified (x,y) location.
    Similar to AcquireOnePixel().
  */
  extern MagickExport PixelPacket
  GetOnePixel(Image *image,const long x,const long y);

  /*
    GetPixelCacheArea() returns the area (width * height in pixels)
    consumed by the current pixel cache.
  */
  extern MagickExport magick_off_t
  GetPixelCacheArea(const Image *image);

  /*
    GetPixels() returns the pixels associated with the last call to
    SetImagePixels() or GetImagePixels().
  */
  extern MagickExport PixelPacket
  *GetPixels(const Image *image);

  /*
    PersistCache() attaches to or initializes a persistent pixel cache.

    Used only by ReadMPCImage() and WriteMPCImage().
  */
  extern MagickExport MagickPassFail
  PersistCache(Image *image,const char *filename,const MagickBool attach,
               magick_off_t *offset,ExceptionInfo *exception);

  /*
    SetImagePixels() initializes a pixel region for write-only access.
  */
  extern MagickExport PixelPacket
  *SetImagePixels(Image *image,const long x,const long y,
                  const unsigned long columns,const unsigned long rows);

  /*
    SetImageVirtualPixelMethod() sets the "virtual pixels" method for
    the image.
  */
  extern MagickExport MagickPassFail
  SetImageVirtualPixelMethod(const Image *image,
                             const VirtualPixelMethod method);

  /*
    SyncImagePixels() saves the image pixels to the in-memory or disk
    cache.
  */
  extern MagickExport MagickPassFail
  SyncImagePixels(Image *image);

  /****
   *
   * Cache view interfaces (Thread safe for multiple access).
   *
   ****/

  /*
    OpenCacheView() opens a cache view.
  */
  extern MagickExport ViewInfo
  *OpenCacheView(Image *image);

  /*
    CloseCacheView() closes a cache view.
  */
  extern MagickExport void
  CloseCacheView(ViewInfo *view);

  /*
    AcquireCacheViewPixels() obtains a pixel region from a cache view
    for read-only access.
  */
  extern MagickExport const PixelPacket
  *AcquireCacheViewPixels(const ViewInfo *view,
                          const long x,const long y,
                          const unsigned long columns,
                          const unsigned long rows,
                          ExceptionInfo *exception);

  /*
    AcquireCacheViewIndexes() returns read-only indexes associated
    with a cache view.
  */
  extern MagickExport const IndexPacket
  *AcquireCacheViewIndexes(const ViewInfo *view);

  /*
    AcquireOneCacheViewPixel() returns one pixel from a cache view.
  */
  extern MagickExport PixelPacket
  AcquireOneCacheViewPixel(const ViewInfo *view,const long x,const long y,
                           ExceptionInfo *exception);

  /*
    GetCacheViewPixels() obtains a pixel region from a cache view for
    read/write access.
  */
  extern MagickExport PixelPacket
  *GetCacheViewPixels(ViewInfo *view,const long x,const long y,
                      const unsigned long columns,const unsigned long rows,
                      ExceptionInfo *exception);

  /*
    GetCacheViewIndexes() returns the indexes associated with a cache view.
  */
  extern MagickExport IndexPacket
  *GetCacheViewIndexes(const ViewInfo *view);

  /*
    SetCacheViewPixels() gets blank writeable pixels from the pixel
    cache view.
  */
  extern MagickExport PixelPacket
  *SetCacheViewPixels(ViewInfo *view,const long x,const long y,
                      const unsigned long columns,const unsigned long rows,
                      ExceptionInfo *exception);

  /*
    SyncCacheViewPixels() saves any changes to the pixel cache view.
  */
  extern MagickExport MagickPassFail
  SyncCacheViewPixels(ViewInfo *view,ExceptionInfo *exception);

  /*
    Simplified support for OpenMP thread view sets.
    These interfaces are subject to change.
  */
  typedef struct _ThreadViewSet
  { 
    ViewInfo
    *views;
    
    unsigned int
    nviews;
  } ThreadViewSet;


  extern MagickExport void
  DestroyThreadViewSet(ThreadViewSet *view_set);
  
  extern MagickExport ThreadViewSet
  *AllocateThreadViewSet(Image *image,ExceptionInfo *exception);
  
  extern MagickExport ViewInfo
  *AccessThreadView(ThreadViewSet *view_set);

  extern MagickExport unsigned int
  GetThreadViewSetAllocatedViews(ThreadViewSet *view_set);

  /*
    Simplified support for OpenMP thread-specific data
    These interfaces are subject to change.
  */
  typedef struct _ThreadViewDataSet
  {
    void
    **view_data;
    
    unsigned int
    nviews;
  } ThreadViewDataSet;

  extern MagickExport void
  DestroyThreadViewDataSet(ThreadViewDataSet *data_set);
  
  extern MagickExport ThreadViewDataSet
  *AllocateThreadViewDataSet(const Image *image,ExceptionInfo *exception);

  extern MagickExport unsigned int
  GetThreadViewDataSetAllocatedViews(ThreadViewDataSet *data_set);

  extern MagickExport void
  *AccessThreadViewData(ThreadViewDataSet *data_set);

  extern MagickExport void
  AssignThreadViewData(ThreadViewDataSet *data_set, unsigned int index,
                       void *data);


  /*
    Stream interfaces (not thread/OpenMP safe).
  */
  
  /*
    ReadStream() makes the image pixels available to a user supplied
    callback method immediately upon reading a scanline with the
    ReadImage() method.

    Used only by ReadXTRNImage(), PingBlob(), and PingImage().
  */
  extern MagickExport Image
  *ReadStream(const ImageInfo *image_info,StreamHandler stream,
              ExceptionInfo *exception);
  
  /*
    WriteStream() makes the image pixels available to a user supplied
    callback method immediately upon writing pixel data with the
    WriteImage() method.

    Used only by WriteXTRNImage()
  */
  extern MagickExport MagickPassFail
  WriteStream(const ImageInfo *image_info,Image *image,
              StreamHandler stream);

#if defined(MAGICK_IMPLEMENTATION)

  /****
   *
   * Private interfaces.
   *
   ****/

  /*
    ClonePixelCacheMethods() clones the pixel cache methods from one
    cache to another

    Used only by AllocateImage() in the case where a pixel cache is
    passed via ImageInfo.
  */
  extern void
  ClonePixelCacheMethods(Cache clone_info,const Cache cache_info);

  /*
    DestroyCacheInfo() deallocates memory associated with the pixel
    cache.

    Used only by DestroyImageInfo() to destroy a pixel cache
    associated with ImageInfo.
  */
  extern void
  DestroyCacheInfo(Cache cache);

  /*
    GetCacheInfo() initializes the Cache structure.

    Used only by AllocateImage() and CloneImage().
  */
  extern void
  GetCacheInfo(Cache *cache);

  /*
    GetCacheViewImage() obtains the image used to allocate the cache view.

    Used by deprecated view interfaces.
  */
  extern Image *
  GetCacheViewImage(const ViewInfo *view);

  /*
    GetPixelCachePresent() tests to see the pixel cache is present
    and contains pixels.
  */
  extern MagickBool
  GetPixelCachePresent(const Image *image);

  /*
    ReferenceCache() increments the reference count associated with
    the pixel cache.  Thread safe.

    Used only by CloneImage() and CloneImageInfo().
  */
  extern Cache
  ReferenceCache(Cache cache);

  /*
   *
   * Inline convenience functions for using thread view sets
   *
   */

  /*
    AcquireThreadViewPixels() obtains a read-only pixel region from a
    cache thread view.
  */
  static inline const PixelPacket
  *AcquireThreadViewPixels(ThreadViewSet *view_set,
                           const long x,const long y,
                           const unsigned long columns,
                           const unsigned long rows,ExceptionInfo *exception)
  {
    return AcquireCacheViewPixels(AccessThreadView(view_set),x,y,columns,rows,
                                  exception);
  }

  /*
    AcquireThreadViewViewIndexes() returns the read-only indexes
    associated with a cache thread view.
  */
  static inline const IndexPacket
  *AcquireThreadViewIndexes(ThreadViewSet *view_set)
  {
    return AcquireCacheViewIndexes(AccessThreadView(view_set));
  }

  /*
    AcquireOneThreadViewPixel() returns one pixel from a cache thread
    view.
  */
  static inline PixelPacket
  AcquireOneThreadViewPixel(ThreadViewSet *view_set,const long x,const long y,
                            ExceptionInfo *exception)
  {
    return AcquireOneCacheViewPixel(AccessThreadView(view_set),x,y,exception);
  }

  /*
    GetThreadViewPixels() obtains a writeable pixel region from a
    cache thread view.
  */
  static inline PixelPacket
  *GetThreadViewPixels(ThreadViewSet *view_set,const long x,const long y,
                       const unsigned long columns,const unsigned long rows,
                       ExceptionInfo *exception)
  {
    return GetCacheViewPixels(AccessThreadView(view_set),x,y,columns,rows,
                              exception);
  }

  /*
    GetThreadViewIndexes() returns the writeable indexes associated
    with a cache thread view.
  */
  static inline IndexPacket
  *GetThreadViewIndexes(ThreadViewSet *view_set)
  {
    return GetCacheViewIndexes(AccessThreadView(view_set));
  }

  /*
    SetThreadViewPixels() gets blank writeable pixels from a cache
    thread view.
  */
  static inline PixelPacket
  *SetThreadViewPixels(ThreadViewSet *view_set,const long x,const long y,
                       const unsigned long columns,const unsigned long rows,
                       ExceptionInfo *exception)
  {
    return SetCacheViewPixels(AccessThreadView(view_set),x,y,columns,rows,
                              exception);
  }

  /*
    SyncThreadViewPixels() saves any changes to pixel cache thread
    view.
  */
  static inline MagickPassFail
  SyncThreadViewPixels(ThreadViewSet *view_set,
                       ExceptionInfo *exception)
  {
    return SyncCacheViewPixels(AccessThreadView(view_set),exception);
  }

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CACHE_H */
