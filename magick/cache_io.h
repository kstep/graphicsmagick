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
extern Export ClassType
  GetCacheClassType(CacheHandle);

extern Export off_t
  GetCacheMemory(off_t),
  GetCacheThreshold();

extern Export unsigned int
  OpenPixelCache(CacheHandle,ClassType,const unsigned int,const unsigned int),
  ReadCachePixels(CacheHandle,RectangleInfo *,PixelPacket *),
  ReadCacheIndexes(CacheHandle,RectangleInfo *,IndexPacket *),
  WriteCachePixels(CacheHandle,RectangleInfo *,PixelPacket *),
  WriteCacheIndexes(CacheHandle,RectangleInfo *,IndexPacket *);

extern Export void
  ClosePixelCache(CacheHandle),
  DestroyCacheInfo(CacheHandle),
  GetCacheInfo(CacheHandle *),
  SetCacheThreshold(off_t),
  SetCacheClassType(CacheHandle,ClassType);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
