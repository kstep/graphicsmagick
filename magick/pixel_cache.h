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
  *GetIndexes(Image *);

extern Export PixelPacket
  *GetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *GetPixels(Image *),
  *SetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int);

extern Export unsigned int
  ReadPixelCache(Image *,const QuantumTypes,const unsigned char *),
  SyncPixelCache(Image *),
  WritePixelCache(Image *,const QuantumTypes,unsigned char *);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
