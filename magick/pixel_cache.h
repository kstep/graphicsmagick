/*
  ImageMagick Pixel Cache Methods.
*/
#ifndef _PIXELCACHE_H
#define _PIXELCACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Export pixel cache interfaces.
*/
extern Export IndexPacket
  *GetIndexes(const Image *);

extern Export PixelPacket
  GetOnePixel(Image *,const int,const int),
  *GetPixels(const Image *),
  *GetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *SetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int);

extern Export unsigned int
  ReadPixelCache(const Image *,const QuantumTypes,const unsigned char *),
  SyncPixelCache(Image *),
  WritePixelCache(const Image *,const QuantumTypes,unsigned char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
