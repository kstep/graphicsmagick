/*
  ImageMagick Pixel Methods.
*/
#ifndef _CACHEIO_H
#define _CACHEIO_H

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
  GetCacheType(Cache);

extern Export ClassType
  GetCacheClassType(Cache);

extern Export IndexPacket
  *GetCacheIndexes(Cache,const unsigned int,const unsigned int);

extern Export off_t
  GetCacheMemory(const off_t),
  GetCacheThreshold();

extern Export PixelPacket
  *GetCachePixels(Cache,const unsigned int,const unsigned int);

extern Export unsigned int
  AllocateCache(Cache,const ClassType,const unsigned int,const unsigned int),
  ReadCachePixels(Cache,PixelPacket *),
  ReadCacheIndexes(Cache,IndexPacket *),
  WriteCachePixels(Cache,const PixelPacket *),
  WriteCacheIndexes(Cache,const IndexPacket *);

extern Export void
  CloseCache(Cache),
  DestroyCacheInfo(Cache),
  GetCacheInfo(Cache *),
  *GetCacheStash(Cache,unsigned int),
  SetCacheThreshold(const off_t),
  SetCacheType(Cache,const CacheType),
  SetCacheView(Cache,const unsigned int,const RectangleInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
