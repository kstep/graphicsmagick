/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "magick/blob.h"

/*
  Enum declaractions.
*/
typedef enum
{
  UndefinedCache,
  MemoryCache,
  DiskCache,
  MapCache
} CacheType;

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
typedef void
  *Cache;

typedef IndexPacket
  *(*GetIndexesFromHandler)(const Image *);

typedef const PixelPacket
  *(*AcquirePixelHandler)(const Image *,const long,const long,
    const unsigned long,const unsigned long,ExceptionInfo *);

typedef PixelPacket
  (*AcquireOnePixelFromHandler)(const Image *,const long,const long,
    ExceptionInfo *);

typedef PixelPacket
  (*GetOnePixelFromHandler)(Image *,const long,const long);

typedef PixelPacket
  *(*GetPixelHandler)(Image *,const long,const long,const unsigned long,
    const unsigned long);

typedef PixelPacket
  *(*GetPixelsFromHandler)(const Image *);

typedef PixelPacket
  *(*SetPixelHandler)(Image *,const long,const long,const unsigned long,
    const unsigned long);

typedef unsigned int
  (*SyncPixelHandler)(Image *);

typedef void
  (*DestroyPixelHandler)(Image *);

typedef struct _CacheMethods
{
  AcquireOnePixelFromHandler
    acquire_one_pixel_from_handler;

  AcquirePixelHandler
    acquire_pixel_handler;

  DestroyPixelHandler
    destroy_pixel_handler;

  GetIndexesFromHandler
    get_indexes_from_handler;

  GetOnePixelFromHandler
    get_one_pixel_from_handler;

  GetPixelHandler
    get_pixel_handler;

  GetPixelsFromHandler
    get_pixels_from_handler;

  SetPixelHandler
    set_pixel_handler;

  SyncPixelHandler
    sync_pixel_handler;
} CacheMethods;

typedef struct _NexusInfo
{
  long
    x,
    y;

  magick_off_t
    length;

  PixelPacket
    *staging,
    *pixels;

  IndexPacket
    *indexes;

  unsigned long
    columns,
    rows;

  unsigned int
    available;
} NexusInfo;

typedef struct _CacheInfo
{
  unsigned long
    id;

  NexusInfo
    *nexus_info;

  ClassType
    storage_class;

  ColorspaceType
    colorspace;

  CacheType
    type;

  unsigned long
    columns,
    rows;

  magick_off_t
    offset,
    length;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;

  VirtualPixelMethod
    virtual_pixel_method;

  PixelPacket
    virtual_pixel;

  int
    file;

  char
    filename[MaxTextExtent],
    cache_filename[MaxTextExtent];

  CacheMethods
    methods;

  long
    reference_count;

  void
    *semaphore;

  unsigned long
    signature;
} CacheInfo;

/*
  MagickExported interfaces.
*/
extern MagickExport Cache
  ReferenceCache(Cache cache);

extern MagickExport ClassType
  GetCacheClass(const Cache cache);

extern MagickExport ColorspaceType
  GetCacheColorspace(const Cache cache);

extern MagickExport const PixelPacket
  *AcquireCacheNexus(const Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows,const unsigned long nexus,
    ExceptionInfo *exception);

extern MagickExport magick_off_t
  GetPixelCacheArea(const Image *image);

extern MagickExport IndexPacket
  *GetIndexes(const Image *image),
  *GetNexusIndexes(const Cache cache,const unsigned long nexus);

MagickExport const PixelPacket
  *AcquireImagePixels(const Image *image,const long x,const long y,
    const unsigned long columns,
    const unsigned long rows,ExceptionInfo *exception);

extern MagickExport PixelPacket
   AcquireOnePixel(const Image *image,const long x,const long y,
     ExceptionInfo *exception),
  *GetCacheNexus(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows,
    const unsigned long nexus),
  *GetImagePixels(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows),
  *GetNexusPixels(const Cache cache,const unsigned long nexus),
    GetOnePixel(Image *image,const long x,const long y),
  *GetPixels(const Image *image),
  *SetImagePixels(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows),
  *SetCacheNexus(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows,
    const unsigned long nexus);

extern MagickExport unsigned int
  SetImageVirtualPixelMethod(const Image *image,
    const VirtualPixelMethod method),
  OpenCache(Image *image,const MapMode mode),
  PersistCache(Image *image,const char *filename,const unsigned int attach,
    magick_off_t *offset,ExceptionInfo *exception),
  SyncCacheNexus(Image *image,const unsigned long nexus),
  SyncImagePixels(Image *image);

extern MagickExport unsigned long
  GetNexus(Cache cache);

extern MagickExport VirtualPixelMethod
  GetImageVirtualPixelMethod(const Image *image);

extern MagickExport void
  ClonePixelCacheMethods(Cache clone,const Cache cache),
  DestroyCacheInfo(Cache cache),
  DestroyCacheNexus(Cache cache,const unsigned long nexu),
  DestroyImagePixels(Image *image),
  GetCacheInfo(Cache *cache),
  SetPixelCacheMethods(Cache cache,AcquirePixelHandler acquire_pixel,
    GetPixelHandler get_pixel,SetPixelHandler set_pixel,SyncPixelHandler sync_pixel,
    GetPixelsFromHandler get_pixels_from,GetIndexesFromHandler get_indexes_from,
    AcquireOnePixelFromHandler acquire_one_pixel_from,
    GetOnePixelFromHandler get_one_pixel_from,DestroyPixelHandler destroy_pixel);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CACHE_H */
