/*
  ImageMagick Pixel Methods.
*/
#ifndef _CACHE_H
#define _CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

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
  ReadPixelCache(Image *,QuantumTypes,unsigned char *),
  WritePixelCache(Image *,QuantumTypes,unsigned char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
