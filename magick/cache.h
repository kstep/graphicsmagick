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
  *GetNexusIndexes(const Cache,const unsigned int);

extern Export off_t
  GetCacheMemory(const off_t),
  GetCacheThreshold(void);

extern Export PixelPacket
  *GetNexusPixels(const Cache,const unsigned int),
  *SetCacheNexus(Cache,const unsigned int,const RectangleInfo *);

extern Export unsigned int
  GetCacheNexus(Cache),
  IsNexusInCore(const Cache,const unsigned int),
  OpenCache(Cache,const ClassType,const unsigned int,const unsigned int),
  ReadCachePixels(Cache,const unsigned int),
  ReadCacheIndexes(Cache,const unsigned int),
  WriteCachePixels(Cache,const unsigned int),
  WriteCacheIndexes(Cache,const unsigned int);

extern Export void
  CloseCache(Cache),
  DestroyCacheInfo(Cache),
  DestroyCacheNexus(Cache,const unsigned int),
  GetCacheInfo(Cache *),
  SetCacheThreshold(const off_t),
  SetCacheType(Cache,const CacheType);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
