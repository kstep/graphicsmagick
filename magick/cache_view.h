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
  MagickExport cache view interfaces.
*/
extern MagickExport IndexPacket
  *GetCacheViewIndexes(const ViewInfo *);

extern MagickExport PixelPacket
  *GetCacheViewPixels(const ViewInfo *),
  *GetCacheView(ViewInfo *,const int,const int,const unsigned int,
    const unsigned int),
  *SetCacheView(ViewInfo *,const int,const int,const unsigned int,
    const unsigned int);

extern MagickExport unsigned int
  SyncCacheView(ViewInfo *);

extern MagickExport ViewInfo
  *OpenCacheView(Image *);

extern MagickExport void
  CloseCacheView(ViewInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
