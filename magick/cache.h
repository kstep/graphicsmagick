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

  ExtendedSignedIntegralType
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

  ExtendedSignedIntegralType
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

  SemaphoreInfo
    *semaphore;

  unsigned long
    signature;
} CacheInfo;

/*
  MagickExported interfaces.
*/
extern MagickExport Cache
  ReferenceCache(Cache);

extern MagickExport ClassType
  GetCacheClass(const Cache);

extern MagickExport ColorspaceType
  GetCacheColorspace(const Cache);

extern MagickExport const PixelPacket
  *AcquireCacheNexus(const Image *,const long,const long,const unsigned long,
    const unsigned long,const unsigned long,ExceptionInfo *);

extern MagickExport ExtendedSignedIntegralType
  GetPixelCacheArea(const Image *);

extern MagickExport IndexPacket
  *GetIndexes(const Image *),
  *GetNexusIndexes(const Cache,const unsigned long);

MagickExport const PixelPacket
  *AcquireImagePixels(const Image *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

extern MagickExport PixelPacket
   AcquireOnePixel(const Image *,const long,const long,ExceptionInfo *),
  *GetCacheNexus(Image *,const long,const long,const unsigned long,
    const unsigned long,const unsigned long),
  *GetImagePixels(Image *,const long,const long,const unsigned long,
    const unsigned long),
  *GetNexusPixels(const Cache,const unsigned long),
   GetOnePixel(Image *,const long,const long),
  *GetPixels(const Image *),
  *SetImagePixels(Image *,const long,const long,const unsigned long,
     const unsigned long),
  *SetCacheNexus(Image *,const long,const long,const unsigned long,
    const unsigned long,const unsigned long);

extern MagickExport unsigned int
  OpenCache(Image *,const MapMode),
  PersistCache(Image *,const char *,const unsigned int,
    ExtendedSignedIntegralType *,ExceptionInfo *),
  SyncCacheNexus(Image *,const unsigned long),
  SyncImagePixels(Image *);

extern MagickExport unsigned long
  GetNexus(Cache);

extern MagickExport VirtualPixelMethod
  GetImageVirtualPixelMethod(const Image *);

extern MagickExport void
  AllocateCacheNexus(CacheInfo *),
  ClonePixelCacheMethods(Cache,const Cache),
  DestroyCacheInfo(Cache),
  DestroyCacheNexus(Cache,const unsigned long),
  DestroyImagePixels(Image *),
  GetCacheInfo(Cache *),
  SetImageVirtualPixelMethod(const Image *,const VirtualPixelMethod),
  SetPixelCacheMethods(Cache,AcquirePixelHandler,GetPixelHandler,
    SetPixelHandler,SyncPixelHandler,GetPixelsFromHandler,GetIndexesFromHandler,
    AcquireOnePixelFromHandler,GetOnePixelFromHandler,DestroyPixelHandler);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CACHE_H */
