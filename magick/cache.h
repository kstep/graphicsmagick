/*
  ImageMagick Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(PixelCacheThreshold)
#define PixelCacheThreshold  2047  /* megabytes */
#endif

/*
  Typedef declaractions.
*/
typedef IndexPacket
  *(*GetIndexesFromHandler)(const Image *);

typedef struct _NexusInfo
{
  unsigned int
    available,
    columns,
    rows;

  int
    x,
    y;

  off_t
    length;

  PixelPacket
    *staging,
    *pixels;

  IndexPacket
    *indexes;
} NexusInfo;

typedef struct _CacheInfo
{
  ClassType
    storage_class;

  ColorspaceType
    colorspace;

  CacheType
    type;

  unsigned int
    columns,
    rows;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;

  char
    cache_filename[MaxTextExtent],
    meta_filename[MaxTextExtent];

  int
    file;

  unsigned int
    persist;

  NexusInfo
    *nexus_info;

  unsigned long
    signature;
} CacheInfo;

typedef PixelPacket
  *(*GetPixelHandler)(Image *,const int,const int,const unsigned int,
    const unsigned int),
  (*GetOnePixelFromHandler)(Image *,const int,const int),
  *(*GetPixelsFromHandler)(const Image *),
  *(*SetPixelHandler)(Image *,const int,const int,const unsigned int,
    const unsigned int);

typedef unsigned int
  (*SyncPixelHandler)(Image *);

typedef void
  *Cache,
  (*ClosePixelHandler)(Image *),
  (*DestroyPixelHandler)(Image *);

/*
  MagickExported interfaces.
*/
extern MagickExport ClassType
  GetCacheClass(const Cache);

extern MagickExport ColorspaceType
  GetCacheColorspace(const Cache);

extern MagickExport IndexPacket
  *GetNexusIndexes(const Cache,const unsigned int);

extern MagickExport PixelPacket
  *GetCacheNexus(Image *,const int,const int,const unsigned int,
    const unsigned int,const unsigned int),
  *GetNexusPixels(const Cache,const unsigned int),
  *SetCacheNexus(Image *,const int,const int,const unsigned int,
    const unsigned int,const unsigned int),
  *SetNexus(Image *,const unsigned int,const RectangleInfo *);

extern MagickExport unsigned int
  GetNexus(Cache),
  IsNexusInCore(const Cache,const unsigned int),
  OpenCache(Image *),
  ReadCachePixels(Cache,const unsigned int),
  ReadCacheIndexes(Cache,const unsigned int),
  SyncCacheNexus(Image *,const unsigned int),
  WriteCacheInfo(Image *),
  WriteCachePixels(Cache,const unsigned int),
  WriteCacheIndexes(Cache,const unsigned int);

extern MagickExport void
  AllocateCacheNexus(CacheInfo *),
  DestroyCache(void),
  DestroyCacheNexus(Cache,const unsigned int),
  GetCacheInfo(Cache *),
  ResetPixelCacheMethods(void),
  SetCacheThreshold(const long),
  SetPixelCacheMethods(GetPixelHandler,SetPixelHandler,SyncPixelHandler,
    GetPixelsFromHandler,GetIndexesFromHandler,GetOnePixelFromHandler,
    ClosePixelHandler,DestroyPixelHandler);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
