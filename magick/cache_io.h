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
  GetCacheClassType(Cache);

extern Export off_t
  GetCacheMemory(off_t),
  GetCacheThreshold();

extern Export unsigned int
  AllocateCache(Cache,ClassType,const unsigned int,const unsigned int),
  ReadCachePixels(Cache,RectangleInfo *,PixelPacket *),
  ReadCacheIndexes(Cache,RectangleInfo *,IndexPacket *),
  WriteCachePixels(Cache,RectangleInfo *,PixelPacket *),
  WriteCacheIndexes(Cache,RectangleInfo *,IndexPacket *);

extern Export void
  CloseCache(Cache),
  DestroyCacheInfo(Cache),
  GetCacheInfo(Cache *),
  SetCacheClassType(Cache,ClassType),
  SetCacheThreshold(off_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
