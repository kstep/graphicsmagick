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
  *GetIndexes(const Cache);

extern Export off_t
  GetCacheMemory(const off_t),
  GetCacheThreshold(void);

extern Export PixelPacket
  *GetCachePixels(const Cache,const unsigned int,const unsigned int),
  *GetPixels(const Cache);

extern Export unsigned int
  AllocateCache(Cache,const ClassType,const unsigned int,const unsigned int),
  GetCacheVista(Cache),
  ReadCachePixels(Cache),
  ReadCacheIndexes(Cache),
  WriteCachePixels(Cache),
  WriteCacheIndexes(Cache);

extern Export void
  CloseCache(Cache),
  DestroyCacheInfo(Cache),
  DestroyCacheVista(Cache),
  GetCacheInfo(Cache *),
  SetCacheThreshold(const off_t),
  SetCacheType(Cache,const CacheType),
  SetCacheVista(Cache,const unsigned int),
  SetCacheVistaRegion(Cache,const RectangleInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
