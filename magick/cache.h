/*
  ImageMagick Pixel Methods.
*/
#ifndef _PIXEL_H
#define _PIXEL_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.
*/
#define PixelCacheThreshold  80  /* megabytes */

/*
  Exported pixel methods.
*/
extern Export PixelPacket
  *GetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *SetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int);

extern Export unsigned int
  SyncPixelCache(Image *image);

extern Export void
  ClosePixelCache(CacheInfo *),
  DestroyCacheInfo(CacheInfo *),
  GetCacheInfo(CacheInfo *),
  ReadPixelCache(Image *,QuantumTypes,unsigned char *),
  SetCacheThreshold(unsigned int),
  WritePixelCache(Image *,QuantumTypes,unsigned char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
