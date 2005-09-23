/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Pixel Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declaractions.
*/

typedef enum
{
  UndefinedVirtualPixelMethod,
  ConstantVirtualPixelMethod,
  EdgeVirtualPixelMethod,
  MirrorVirtualPixelMethod,
  TileVirtualPixelMethod
} VirtualPixelMethod;

/*
  Typedef declaractions.
*/
typedef void
  *Cache;


/*
  Default View interfaces
*/
extern MagickExport Cache
  ReferenceCache(Cache cache);

extern MagickExport magick_off_t
  GetPixelCacheArea(const Image *image);

extern MagickExport IndexPacket
  *GetIndexes(const Image *image);

MagickExport const PixelPacket
  *AcquireImagePixels(const Image *image,const long x,const long y,
    const unsigned long columns,
    const unsigned long rows,ExceptionInfo *exception);

extern MagickExport PixelPacket
   AcquireOnePixel(const Image *image,const long x,const long y,
     ExceptionInfo *exception),
  *GetImagePixels(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows),
   GetOnePixel(Image *image,const long x,const long y),
  *GetPixels(const Image *image),
  *SetImagePixels(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows);

extern MagickExport unsigned int
  SetImageVirtualPixelMethod(const Image *image,
    const VirtualPixelMethod method),
  PersistCache(Image *image,const char *filename,const unsigned int attach,
    magick_off_t *offset,ExceptionInfo *exception),
  SyncImagePixels(Image *image);

extern MagickExport VirtualPixelMethod
  GetImageVirtualPixelMethod(const Image *image);

extern MagickExport void
  ClonePixelCacheMethods(Cache clone,const Cache cache),
  DestroyCacheInfo(Cache cache),
  DestroyImagePixels(Image *image),
  GetCacheInfo(Cache *cache);

/*
  Cache view interfaces.
*/
typedef void *ViewInfo;

extern MagickExport const PixelPacket
  *AcquireCacheView(const ViewInfo *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

extern MagickExport IndexPacket
  *GetCacheViewIndexes(const ViewInfo *);

extern MagickExport PixelPacket
  *GetCacheViewPixels(const ViewInfo *),
  *GetCacheView(ViewInfo *,const long,const long,const unsigned long,
    const unsigned long),
  *SetCacheView(ViewInfo *,const long,const long,const unsigned long,
    const unsigned long);

extern MagickExport unsigned int
  SyncCacheView(ViewInfo *);

extern MagickExport ViewInfo
  *OpenCacheView(Image *);

extern MagickExport void
  CloseCacheView(ViewInfo *);

/*
  Stream interfaces.
*/

extern MagickExport Image
  *ReadStream(const ImageInfo *,StreamHandler,ExceptionInfo *);

extern MagickExport unsigned int
  WriteStream(const ImageInfo *,Image *,StreamHandler);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_CACHE_H */
