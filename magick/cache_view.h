/*
  ImageMagick Cache View Methods.
*/
#ifndef _CACHEVIEW_H
#define _CACHEVIEW_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _ViewInfo
{
  unsigned int
    id;

  Image
    *image;
} ViewInfo;

/*
  Export cache view interfaces.
*/
extern Export IndexPacket
  *GetCacheViewIndexes(const ViewInfo *);

extern Export PixelPacket
  *GetCacheViewPixels(const ViewInfo *),
  *GetCacheView(ViewInfo *,const int,const int,const unsigned int,
    const unsigned int),
  *SetCacheView(ViewInfo *,const int,const int,const unsigned int,
    const unsigned int);

extern Export unsigned int
  SyncCacheView(ViewInfo *);

extern Export ViewInfo
  *OpenCacheView(Image *);

extern Export void
  CloseCacheView(ViewInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
