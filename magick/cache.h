/*
  ImageMagick Cache Methods.
*/
#ifndef _CACHE_H
#define _CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(PixelCacheThreshold)
#define PixelCacheThreshold  80  /* megabytes */
#endif

/*
  Exported interfaces.
*/
extern Export CacheType
  GetCacheType(const Cache);

extern Export ClassType
  GetCacheClassType(const Cache);

extern Export IndexPacket
  *GetCacheIndexes(Cache,const unsigned int,const unsigned int),
  *GetVistaIndexes(const Cache,const unsigned int);

extern Export off_t
  GetCacheMemory(const off_t),
  GetCacheThreshold(void);

extern Export PixelPacket
  *GetCachePixels(const Cache,const unsigned int,const unsigned int),
  *GetVistaPixels(const Cache,const unsigned int);

extern Export unsigned int
  AllocateCache(Cache,const ClassType,const unsigned int,const unsigned int),
  GetCacheVista(Cache),
  ReadCachePixels(Cache,const unsigned int),
  ReadCacheIndexes(Cache,const unsigned int),
  WriteCachePixels(Cache,const unsigned int),
  WriteCacheIndexes(Cache,const unsigned int);

extern Export void
  CloseCache(Cache),
  DestroyCacheInfo(Cache),
  DestroyCacheVista(Cache,const unsigned int),
  GetCacheInfo(Cache *),
  SetCacheThreshold(const off_t),
  SetCacheType(Cache,const CacheType),
  SetCacheVista(Cache,const unsigned int,const RectangleInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
