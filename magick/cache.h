/*
  ImageMagick Cache Methods.
*/
#ifndef _CACHE_H
#define _CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(PixelCacheThreshold)
#define PixelCacheThreshold  2047  /* megabytes */
#endif

/*
  Typedef declaractions.
*/
typedef void* Cache;

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

  void
    *line;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;
} NexusInfo;

typedef struct _CacheInfo
{
  ClassType
#if defined(__cplusplus) || defined(c_plusplus)
    c_class;
#else
    class;
#endif

  CacheType
    type;

  unsigned int
    columns,
    rows;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;

  unsigned int
    persist;

  char
    filename[MaxTextExtent];

  int
    file;

  NexusInfo
    *nexus;
} CacheInfo;

/*
  MagickExported interfaces.
*/
extern MagickExport ClassType
  GetCacheClassType(const Cache);

extern MagickExport IndexPacket
  *GetNexusIndexes(const Cache,const unsigned int);

extern MagickExport off_t
  GetCacheMemory(const off_t);

extern MagickExport PixelPacket
  *GetNexusPixels(const Cache,const unsigned int),
  *SetCacheNexus(Cache,const unsigned int,const RectangleInfo *);

extern MagickExport unsigned int
  GetCacheNexus(Cache),
  IsNexusInCore(const Cache,const unsigned int),
  OpenCache(Cache,const ClassType,const unsigned int,const unsigned int),
  ReadCachePixels(Cache,const unsigned int),
  ReadCacheIndexes(Cache,const unsigned int),
  WriteCachePixels(Cache,const unsigned int),
  WriteCacheIndexes(Cache,const unsigned int);

extern MagickExport void
  DestroyCacheNexus(Cache,const unsigned int),
  GetCacheInfo(Cache *),
  SetCacheThreshold(const off_t),
  SetPixelCacheMethods(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
