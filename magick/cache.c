/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      CCCC   AAA    CCCC  H   H  EEEEE                       %
%                     C      A   A  C      H   H  E                           %
%                     C      AAAAA  C      HHHHH  EEE                         %
%                     C      A   A  C      H   H  E                           %
%                      CCCC  A   A   CCCC  H   H  EEEEE                       %
%                                                                             %
%                                                                             %
%                      ImageMagick Pixel Cache Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#if defined(HasZLIB)
#include "zlib.h"
#endif

/*
  Global declarations.
*/
static off_t
  cache_memory = ~0;

static SemaphoreInfo
  *cache_semaphore = (SemaphoreInfo *) NULL;

/*
  Declare pixel cache interfaces.
*/
static const PixelPacket
  *AcquirePixelCache(const Image *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

static IndexPacket
  *GetIndexesFromCache(const Image *);

static inline unsigned int
  IsNexusInCore(const Cache,const unsigned long);

static off_t
  GetCacheMemory(const off_t);

static PixelPacket
  AcquireOnePixelFromCache(const Image *,const long,const long,ExceptionInfo *),
  GetOnePixelFromCache(Image *,const long,const long),
  *GetPixelsFromCache(const Image *),
  *GetPixelCache(Image *,const long,const long,const unsigned long,
    const unsigned long),
  *SetNexus(const Image *,const RectangleInfo *,const unsigned long),
  *SetPixelCache(Image *,const long,const long,const unsigned long,
    const unsigned long);

static unsigned int
  ReadCacheIndexes(const Cache,const unsigned long),
  ReadCachePixels(const Cache,const unsigned long),
  SyncCache(Image *),
  SyncPixelCache(Image *),
  WriteCacheIndexes(Cache,const unsigned long),
  WriteCachePixels(Cache,const unsigned long);

static void
  DestroyPixelCache(Image *);

/*
  Preload the image pixel methods.
*/
static AcquireOnePixelFromHandler
  acquire_one_pixel_from_handler = AcquireOnePixelFromCache;

static AcquirePixelHandler
  acquire_pixel_handler = AcquirePixelCache;

static DestroyPixelHandler
  destroy_pixel_handler = DestroyPixelCache;

static GetIndexesFromHandler
  get_indexes_from_handler = GetIndexesFromCache;

static GetOnePixelFromHandler
  get_one_pixel_from_handler = GetOnePixelFromCache;

static GetPixelHandler
  get_pixel_handler = GetPixelCache;

static GetPixelsFromHandler
  get_pixels_from_handler = GetPixelsFromCache;

static SetPixelHandler
  set_pixel_handler = SetPixelCache;

static SyncPixelHandler
  sync_pixel_handler = SyncPixelCache;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e C a c h e N e x u s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireCacheNexus() acquires pixels from the in-memory or disk pixel cache
%  as defined by the geometry parameters.   A pointer to the pixels is
%  returned if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the AcquireCacheNexus() method is:
%
%      PixelPacket *AcquireCacheNexus(const Image *image,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        const unsigned long nexus,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method AcquireCacheNexus() returns a pointer to the pixels if
%      they are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o nexus: specifies which cache nexus to acquire.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const PixelPacket *AcquireCacheNexus(const Image *image,
  const long x,const long y,const unsigned long columns,
	const unsigned long rows,const unsigned long nexus,ExceptionInfo *exception)
{
#define Cx(x) ((x) < 0 ? 0 : (x) >= (long) cache_info->columns ? \
  (long) cache_info->columns-1 : (x))
#define Cy(y) ((y) < 0 ? 0 : (y) >= (long) cache_info->rows ? \
  (long) cache_info->rows-1 : (y))

  CacheInfo
    *cache_info;

  IndexPacket
    *indexes,
    *nexus_indexes;

  off_t
    offset;

  PixelPacket
    *pixels;

  RectangleInfo
    region;

  register const PixelPacket
    *p;

  register long
    u,
    v;

  register PixelPacket
    *q;

  unsigned long
    image_nexus,
    number_pixels,
    span;

  /*
    Acquire pixels.
  */
  assert(image != (const Image *) NULL);
  assert(image->cache != (Cache) NULL);
  assert(image->signature == MagickSignature);
  cache_info=(CacheInfo *) image->cache;
  if (cache_info->type == UndefinedCache)
    {
      ThrowException(exception,CacheWarning,"Pixel cache is not open",
        image->filename);
      return((PixelPacket *) NULL);
    }
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  pixels=SetNexus(image,&region,nexus);
  if (IsNexusInCore(image->cache,nexus))
    return(pixels);
  offset=y*cache_info->columns+x;
  span=(rows-1)*cache_info->columns+columns-1;
  number_pixels=cache_info->columns*cache_info->rows;
  if ((offset >= 0) && (offset+span) <= (off_t) number_pixels)
    {
      unsigned int
        status;

      /*
        Pixel request is inside cache extents.
      */
      status=ReadCachePixels(image->cache,nexus);
      if ((image->storage_class == PseudoClass) ||
          (image->colorspace == CMYKColorspace))
        status|=ReadCacheIndexes(image->cache,nexus);
      if (status == False)
        {
          ThrowException(exception,CacheWarning,
            "Unable to acquire pixels from cache",image->filename);
          return((PixelPacket *) NULL);
        }
      return(pixels);
    }
  /*
    Pixel request is outside cache extents.
  */
  image_nexus=GetNexus(image->cache);
  if (image_nexus == 0)
    return(False);
  q=pixels;
  indexes=GetNexusIndexes(image->cache,nexus);
  for (v=0; v < (long) rows; v++)
  {
    for (u=0; u < (long) columns; u+=span)
    {
      span=Min(cache_info->columns-(x+u),columns-u);
      if ((((y+v) < 0) || ((y+v) >= (long) cache_info->rows)) ||
          (((x+u) < 0) || ((x+u) >= (long) cache_info->columns)) || (span == 1))
        {
          /*
            Transfer a single pixel.
          */
          span=1;
          p=AcquireCacheNexus(image,Cx(x+u),Cy(y+v),1,1,image_nexus,exception);
          if (p == (const PixelPacket *) NULL)
            break;
          *q=(*p);
          if (indexes != (IndexPacket *) NULL)
            {
              nexus_indexes=GetNexusIndexes(image->cache,image_nexus);
              if (nexus_indexes != (IndexPacket *) NULL)
                *indexes++=(*nexus_indexes);
            }
          q++;
          continue;
        }
      /*
        Transfer a run of pixels.
      */
      p=AcquireCacheNexus(image,x+u,y+v,span,1,image_nexus,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      (void) memcpy(q,p,span*sizeof(PixelPacket));
      q+=span;
      nexus_indexes=GetNexusIndexes(image->cache,image_nexus);
      if ((nexus_indexes != (IndexPacket *) NULL) &&
          (indexes != (IndexPacket *) NULL))
        {
          (void) memcpy(indexes,nexus_indexes,span*sizeof(IndexPacket));
          indexes+=span;
        }
    }
  }
  DestroyCacheNexus(image->cache,image_nexus);
  return(pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e I m a g e P i x e l s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquireImagePixels() acquires pixels from the in-memory or disk
%  pixel cache as defined by the geometry parameters.   A pointer to the
%  pixels is returned if the pixels are transferred, otherwise a NULL is
%  returned. If you plan to change the pixels, use GetImagePixels() instead.
%
%  The format of the AcquireImagePixels() method is:
%
%      const PixelPacket *AcquireImagePixels(const Image *image,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method AcquireImagePixels() returns a pointer to the pixels
%      if they are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const PixelPacket *AcquireImagePixels(const Image *image,
  const long x,const long y,const unsigned long columns,
  const unsigned long rows,ExceptionInfo *exception)
{
  if (acquire_pixel_handler == (AcquirePixelHandler) NULL)
    return((const PixelPacket *) NULL);
  return((*acquire_pixel_handler)(image,x,y,columns,rows,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e P i x e l C a c h e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquirePixelCache() acquires pixels from the in-memory or disk pixel
%  cache as defined by the geometry parameters.   A pointer to the pixels
%  is returned if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the AcquirePixelCache() method is:
%
%      const PixelPacket *AcquirePixelCache(const Image *image,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static const PixelPacket *AcquirePixelCache(const Image *image,const long x,
  const long y,const unsigned long columns,const unsigned long rows,
  ExceptionInfo *exception)
{
  return(AcquireCacheNexus(image,x,y,columns,rows,0,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e O n e P i x e l                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquireOnePixel() returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%  If you plan to change the pixel, use GetOnePixel() instead.
%
%  The format of the AcquireOnePixel() method is:
%
%      PixelPacket AcquireOnePixel(const Image image,const long x,
%        const long y,ExceptionInfo exception)
%
%  A description of each parameter follows:
%
%    o pixels: Method AcquireOnePixel() returns a pixel at the
%      specified (x,y) location.
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport PixelPacket AcquireOnePixel(const Image *image,const long x,
  const long y,ExceptionInfo *exception)
{
  if (acquire_one_pixel_from_handler == (AcquireOnePixelFromHandler) NULL)
    return(image->background_color);
  return((*acquire_one_pixel_from_handler)(image,x,y,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e O n e P i x e l F r o m C a c h e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquireOnePixelFromCache() returns a single pixel at the specified
%  (x,y) location.  The image background color is returned if an error occurs.
%
%  The format of the AcquireOnePixelFromCache() method is:
%
%      PixelPacket *AcquireOnePixelFromCache(const Image image,const long x,
%        const long y,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o pixels: Method AcquireOnePixelFromCache returns a pixel at the
%      specified (x,y) location.
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static PixelPacket AcquireOnePixelFromCache(const Image *image,const long x,
  const long y,ExceptionInfo *exception)
{
  register const PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pixel=AcquirePixelCache(image,x,y,1,1,exception);
  if (pixel != (PixelPacket *) NULL)
    return(*pixel);
  return(image->background_color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l i p C a c h e N e x u s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClipCacheNexus() clips the image pixels of the in-memory or disk
%  cache as defined by the image clip mask.  The method returns True if the
%  pixel region is clipped, otherwise False.
%
%  The format of the ClipCacheNexus() method is:
%
%      unsigned int ClipCacheNexus(Image *image,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method ClipCacheNexus() returns True if the image pixels are
%      clipped, otherwise False.
%
%    o image: The image.
%
%    o nexus: specifies which cache nexus to clip.
%
%
*/
static unsigned int ClipCacheNexus(Image *image,const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  long
    y;

  NexusInfo
    *nexus_info;

  register long
    x;

  register PixelPacket
    *p,
    *q,
    *r;

  unsigned long
    image_nexus,
    mask_nexus;

  /*
    Apply clip mask.
  */
  image_nexus=GetNexus(image->cache);
  mask_nexus=GetNexus(image->clip_mask->cache);
  if ((image_nexus == 0) || (mask_nexus == 0))
    ThrowBinaryException(CacheWarning,"Unable to get nexus",image->filename);
  cache_info=(CacheInfo *) image->cache;
  nexus_info=cache_info->nexus_info+nexus;
  p=GetCacheNexus(image,nexus_info->x,nexus_info->y,nexus_info->columns,
    nexus_info->rows,image_nexus);
  q=GetCacheNexus(image->clip_mask,nexus_info->x,nexus_info->y,
    nexus_info->columns,nexus_info->rows,mask_nexus);
  r=nexus_info->pixels;
  if ((p != (PixelPacket *) NULL) && (q != (PixelPacket *) NULL))
    for (y=0; y < (long) nexus_info->rows; y++)
    {
      for (x=0; x < (long) nexus_info->columns; x++)
      {
        if (q->red == TransparentOpacity)
          r->red=p->red;
        if (q->green == TransparentOpacity)
          r->green=p->green;
        if (q->blue == TransparentOpacity)
          r->blue=p->blue;
        if (q->opacity == TransparentOpacity)
          r->opacity=p->opacity;
        p++;
        q++;
        r++;
      }
    }
  DestroyCacheNexus(image->cache,image_nexus);
  DestroyCacheNexus(image->clip_mask->cache,mask_nexus);
  return((p != (PixelPacket *) NULL) && (q != (PixelPacket *) NULL));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C a c h e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyCache() destroys the cache environment.
%
%  The format of the DestroyCache() method is:
%
%      DestroyCache(void)
%
%
*/
MagickExport void DestroyCache(void)
{
  AcquireSemaphoreInfo(&cache_semaphore);
  DestroySemaphoreInfo(&cache_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C a c h e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyCacheInfo() deallocates memory associated with the pixel
%  cache.
%
%  The format of the DestroyCacheInfo() method is:
%
%      void DestroyCacheInfo(Cache cache)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
static void DestroyCacheInfo(Cache cache)
{
  CacheInfo
    *cache_info;

  unsigned int
    destroy;

  unsigned long
    number_pixels;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  destroy=False;
  AcquireSemaphoreInfo(&cache_info->semaphore);
  cache_info->reference_count--;
  if (cache_info->reference_count == 0)
    destroy=True;
  LiberateSemaphoreInfo(&cache_info->semaphore);
  if (!destroy)
    return;
  number_pixels=cache_info->columns*cache_info->rows;
  switch (cache_info->type)
  {
    case MemoryCache:
    {
      LiberateMemory((void **) &cache_info->pixels);
      (void) GetCacheMemory(-cache_info->length);
      break;
    }
    case MemoryMappedCache:
      (void) UnmapBlob(cache_info->pixels,cache_info->length);
    case DiskCache:
    {
      (void) remove(cache_info->cache_filename);
      break;
    }
    default:
      break;
  }
  if (cache_info->type != UndefinedCache)
    {
      register unsigned long
        id;

      for (id=0; id < (cache_info->rows+3); id++)
        DestroyCacheNexus(cache,id);
      LiberateMemory((void **) &cache_info->nexus_info);
    }
  if (cache_info->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo(&cache_info->semaphore);
  LiberateMemory((void **) &cache_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C a c h e N e x u s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyCacheNexus() destroys a cache nexus.
%
%  The format of the DestroyCacheNexus() method is:
%
%      void DestroyCacheNexus(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o nexus: specifies which cache nexus to destroy.
%
%
*/
MagickExport void DestroyCacheNexus(Cache cache,const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  register NexusInfo
    *nexus_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  nexus_info=cache_info->nexus_info+nexus;
  if (nexus_info->staging != (PixelPacket *) NULL)
    LiberateMemory((void **) &nexus_info->staging);
  (void) memset(nexus_info,0,sizeof(NexusInfo));
  nexus_info->available=True;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e P i x e l s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyImagePixels() deallocates memory associated with the pixel
%  cache.
%
%  The format of the DestroyImagePixels() method is:
%
%      void DestroyImagePixels(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyImagePixels(Image *image)
{
  if (destroy_pixel_handler == (DestroyPixelHandler) NULL)
    return;
  (*destroy_pixel_handler)(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y P i x e l C a c h e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPixelCache() deallocates memory associated with the pixel
%  cache.
%
%  The format of the DestroyPixelCache() method is:
%
%      void DestroyPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static void DestroyPixelCache(Image *image)
{
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->cache == (void *) NULL)
    return;
  cache_info=(CacheInfo *) image->cache;
  DestroyCacheInfo(image->cache);
  image->cache=(Cache) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C a c h e C l a s s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheClass() returns the class type of the pixel cache.
%
%  The format of the GetCacheClass() method is:
%
%      ClassType GetCacheClass(Cache cache)
%
%  A description of each parameter follows:
%
%    o type: Method GetCacheClass returns DirectClass or PseudoClass.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport ClassType GetCacheClass(const Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  return(cache_info->storage_class);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C a c h e C o l o r s p a c e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheColorspace() returns the class type of the pixel cache.
%
%  The format of the GetCacheColorspace() method is:
%
%      Colorspace GetCacheColorspace(Cache cache)
%
%  A description of each parameter follows:
%
%    o type: Method GetCacheColorspace returns DirectClass or PseudoClass.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport ColorspaceType GetCacheColorspace(const Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  return(cache_info->colorspace);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C a c h e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheInfo() initializes the Cache structure.
%
%  The format of the GetCacheInfo() method is:
%
%      void GetCacheInfo(Cache *cache)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport void GetCacheInfo(Cache *cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) AcquireMemory(sizeof(CacheInfo));
  if (cache_info == (CacheInfo *) NULL)
    MagickError(ResourceLimitError,"Memory allocation failed",
      "unable to allocate cache info");
  (void) memset(cache_info,0,sizeof(CacheInfo));
  cache_info->colorspace=RGBColorspace;
  cache_info->reference_count=1;
  cache_info->signature=MagickSignature;
  *cache=cache_info;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C a c h e M e m o r y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheMemory() acquires memory from, or returns memory to, the
%  cache memory pool.  The method returns the available memory in the cache
%  pool.
%
%  The format of the GetCacheMemory() method is:
%
%      off_t GetCacheMemory(const off_t memory)
%
%  A description of each parameter follows:
%
%    o memory: Specifies how much memory to acquire from, or return to,
%      the cache memory pool.
%
%
*/
static off_t GetCacheMemory(const off_t memory)
{
  AcquireSemaphoreInfo(&cache_semaphore);
  cache_memory-=memory;
  LiberateSemaphoreInfo(&cache_semaphore);
  return(cache_memory);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C a c h e N e x u s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheNexus() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetCacheNexus() method is:
%
%      PixelPacket *GetCacheNexus(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetCacheNexus() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o nexus: specifies which cache nexus to return.
%
%
*/
MagickExport PixelPacket *GetCacheNexus(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows,
  const unsigned long nexus)
{
  PixelPacket
    *pixels;

  unsigned int
    status;

  /*
    Transfer pixels from the cache.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pixels=SetCacheNexus(image,x,y,columns,rows,nexus);
  if (pixels == (PixelPacket *) NULL)
    return((PixelPacket *) NULL);
  if (IsNexusInCore(image->cache,nexus))
    return(pixels);
  status=ReadCachePixels(image->cache,nexus);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    status|=ReadCacheIndexes(image->cache,nexus);
  if (status == False)
    {
      ThrowException(&image->exception,CacheWarning,
        "Unable to get pixels from cache",image->filename);
      return((PixelPacket *) NULL);
    }
  return(pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImagePixels() gets pixels from the in-memory or disk pixel cache
%  as defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetImagePixels() method is:
%
%      PixelPacket *GetImagePixels(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetImagePixels() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
MagickExport PixelPacket *GetImagePixels(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  if (get_pixel_handler == (GetPixelHandler) NULL)
    return((PixelPacket *) NULL);
  return((*get_pixel_handler)(image,x,y,columns,rows));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I n d e x e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetIndexes() returns the indexes associated with the last call to
%  SetImagePixels() or GetImagePixels().
%
%  The format of the GetIndexes() method is:
%
%      IndexPacket *GetIndexes(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexes() returns the indexes associated with the
%      last call to SetImagePixels() or GetImagePixels().
%
%    o image: The image.
%
%
*/
MagickExport IndexPacket *GetIndexes(const Image *image)
{
  if (get_indexes_from_handler == (GetIndexesFromHandler) NULL)
    return((IndexPacket *) NULL);
  return((*get_indexes_from_handler)(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I n d e x e s F r o m C a c h e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetIndexesFromCache() returns the indexes associated with the last
%  call to SetPixelCache() or GetPixelCache().
%
%  The format of the GetIndexesFromCache() method is:
%
%      IndexPacket *GetIndexesFromCache(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexesFromCache() returns the indexes associated
%      with the last call to SetPixelCache() or GetPixelCache().
%
%    o image: The image.
%
%
*/
static IndexPacket *GetIndexesFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  return(GetNexusIndexes(image->cache,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t N e x u s                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNexus() returns an available cache nexus.
%
%  The format of the GetNexus() method is:
%
%      unsigned int GetNexus(Cache cache)
%
%  A description of each parameter follows:
%
%    o id:  Method GetNexus returns an available cache nexus slot.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport unsigned long GetNexus(Cache cache)
{
  CacheInfo
    *cache_info;

  register unsigned long
    id;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  for (id=1; id < (cache_info->rows+3); id++)
    if (cache_info->nexus_info[id].available)
      {
        cache_info->nexus_info[id].available=False;
        return(id);
      }
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t N e x u s I n d e x e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNexusIndexes() returns the indexes associated with the specified
%  cache nexus.
%
%  The format of the GetNexusIndexes() method is:
%
%      IndexPacket *GetNexusIndexes(const Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetNexusIndexes returns the indexes associated with the
%      specified cache nexus.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o nexus: specifies which cache nexus to return the colormap indexes.
%
%
*/
MagickExport IndexPacket *GetNexusIndexes(const Cache cache,
  const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  register NexusInfo
    *nexus_info;

  if (cache == (Cache) NULL)
    return((IndexPacket *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->storage_class == UndefinedClass)
    return((IndexPacket *) NULL);
  nexus_info=cache_info->nexus_info+nexus;
  return(nexus_info->indexes);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t N e x u s P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNexusPixels() returns the pixels associated with the specified
%  cache nexus.
%
%  The format of the GetNexusPixels() method is:
%
%      PixelPacket *GetNexusPixels(const Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetNexusPixels returns the pixels associated with the
%      specified cache nexus.
%
%    o nexus: specifies which cache nexus to return the pixels.
%
%
*/
MagickExport PixelPacket *GetNexusPixels(const Cache cache,
  const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  register NexusInfo
    *nexus_info;

  if (cache == (Cache) NULL)
    return((PixelPacket *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->storage_class == UndefinedClass)
    return((PixelPacket *) NULL);
  nexus_info=cache_info->nexus_info+nexus;
  return(nexus_info->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O n e P i x e l                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetOnePixel() returns a single pixel at the specified (x,y) location.
%  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixel() method is:
%
%      PixelPacket *GetOnePixel(const Image image,const long x,const long y)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
MagickExport PixelPacket GetOnePixel(Image *image,const long x,const long y)
{
  if (get_one_pixel_from_handler == (GetOnePixelFromHandler) NULL)
    return(image->background_color);
  return((*get_one_pixel_from_handler)(image,x,y));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t O n e P i x e l F r o m C a c h e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOnePixelFromCache() returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixelFromCache() method is:
%
%      PixelPacket GetOnePixelFromCache(const Image image,const long x,
%        const long y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixelFromCache returns a pixel at the specified
%      (x,y) location.
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
static PixelPacket GetOnePixelFromCache(Image *image,const long x,const long y)
{
  register PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pixel=GetPixelCache(image,x,y,1,1);
  if (pixel != (PixelPacket *) NULL)
    return(*pixel);
  return(image->background_color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P i x e l s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixels() returns the pixels associated with the last call to
%  SetImagePixels() or GetImagePixels().
%
%  The format of the GetPixels() method is:
%
%      PixelPacket *GetPixels(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixels() returns the pixels associated with the
%      last call to SetImagePixels() or GetImagePixels().
%
%    o image: The image.
%
%
*/
MagickExport PixelPacket *GetPixels(const Image *image)
{
  if (get_pixels_from_handler == (GetPixelsFromHandler) NULL)
    return((PixelPacket *) NULL);
  return((*get_pixels_from_handler)(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPixelCache() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetPixelCache() method is:
%
%      PixelPacket *GetPixelCache(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *GetPixelCache(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  return(GetCacheNexus(image,x,y,columns,rows,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t P i x e l s F r o m C a c h e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelsFromCache() returns the pixels associated with the last call
%  to the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetPixelsFromCache() method is:
%
%      PixelPacket *GetPixelsFromCache(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixelsFromCache() returns the pixels associated with
%      the last call to SetPixelCache() or GetPixelCache().
%
%    o image: The image.
%
%
*/
static PixelPacket *GetPixelsFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  return(GetNexusPixels(image->cache,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I s N e x u s I n C o r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsNexusInCore() returns true if the pixels associated with the
%  specified cache nexus is non-strided and in core.
%
%  The format of the IsNexusInCore() method is:
%
%      unsigned int IsNexusInCore(const Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method IsNexusInCore() returns True if the pixels are
%      non-strided and in core, otherwise False.
%
%    o nexus: specifies which cache nexus to return the pixels.
%
%
*/
static inline unsigned int IsNexusInCore(const Cache cache,
  const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  off_t
    offset;

  register NexusInfo
    *nexus_info;

  if (cache == (Cache) NULL)
    return(False);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->storage_class == UndefinedClass)
    return(False);
  nexus_info=cache_info->nexus_info+nexus;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  if (nexus_info->pixels == (cache_info->pixels+offset))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d i f y C a c h e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ModifyCache() ensures that there is only a single reference to the pixel
%  cache to be modified, updating the provided cache pointer to point to
%  a clone of the original pixel cache if necessary.
%
%  The format of the ModifyCache method is:
%
%      unsigned int ModifyCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static unsigned int ModifyCache(Image *image)
{
  CacheInfo
    *cache_info;

  Image
    *clone_image;

  int
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *clone_indexes,
    *indexes;

  register PixelPacket
    *q;

  if (SyncCache(image) == False)
    return(True);
  cache_info=(CacheInfo *) image->cache;
  AcquireSemaphoreInfo(&cache_info->semaphore);
  if (cache_info->reference_count <= 1)
    {
      LiberateSemaphoreInfo(&cache_info->semaphore);
      return(True);
   }
  cache_info->reference_count--;
  clone_image=(Image *) AcquireMemory(sizeof(Image));
  if (clone_image == (Image *) NULL)
    MagickError(ResourceLimitError,"Unable to clone image",
      "Memory allocation failed");
  *clone_image=(*image);
  GetCacheInfo(&image->cache);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(clone_image,0,y,image->columns,1,&image->exception);
    q=SetImagePixels(image,0,y,image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    (void) memcpy(q,p,clone_image->columns*sizeof(PixelPacket));
    clone_indexes=GetIndexes(clone_image);
    indexes=GetIndexes(image);
    if ((clone_indexes != (IndexPacket *) NULL) &&
        (indexes != (IndexPacket *) NULL))
      (void) memcpy(indexes,clone_indexes,
        image->columns*sizeof(IndexPacket));
    if (!SyncImagePixels(image))
      break;
  }
  LiberateMemory((void **) &clone_image);
  LiberateSemaphoreInfo(&cache_info->semaphore);
  if (y < (long) image->rows)
    ThrowBinaryException(CacheWarning,"Unable to clone cache",image->filename);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p e n C a c h e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenCache() allocates the pixel cache.  This includes defining the
%  cache dimensions, allocating space for the image pixels and optionally the
%  colormap indexes, and memory mapping the cache if it is disk based.  The
%  cache nexus array is initialized as well.
%
%  The format of the OpenCache() method is:
%
%      unsigned int OpenCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method OpenCache returns True if the pixel cache is initialized
%      successfully otherwise False.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int OpenCache(Image *image)
{
  CacheInfo
    *cache_info;

  int
    c,
    file;

  off_t
    offset,
    size;

  PixelPacket
    *pixels;

  unsigned long
    number_pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (void *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    return(False);
  if (cache_memory == ~0)
    {
      char
        *threshold;

      /*
        Set cache memory threshold.
      */
      SetCacheThreshold(((off_t) 1L) << (8*sizeof(off_t)-21));
#if defined(PixelCacheThreshold)
      SetCacheThreshold(PixelCacheThreshold);
#endif
      threshold=getenv("MAGICK_CACHE_THRESHOLD");
      if (threshold != (char *) NULL)
        SetCacheThreshold(atol(threshold));
    }
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->storage_class != UndefinedClass)
    switch (cache_info->type)
    {
      case MemoryCache:
      {
        (void) GetCacheMemory(-cache_info->length);
        break;
      }
      case MemoryMappedCache:
      {
        (void) UnmapBlob(cache_info->pixels,cache_info->length);
        break;
      }
      default:
        break;
    }
  cache_info->rows=image->rows;
  cache_info->columns=image->columns;
  number_pixels=cache_info->columns*cache_info->rows;
  if (cache_info->nexus_info == (NexusInfo *) NULL)
    {
      unsigned long
        id;

      /*
        Allocate cache nexus.
      */
      cache_info->nexus_info=(NexusInfo *)
        AcquireMemory((cache_info->rows+3)*sizeof(NexusInfo));
      if (cache_info->nexus_info == (NexusInfo *) NULL)
        MagickError(ResourceLimitError,"Memory allocation failed",
          "unable to allocate cache nexus_info");
      (void) memset(cache_info->nexus_info,0,
        (cache_info->rows+3)*sizeof(NexusInfo));
      for (id=1; id < (cache_info->rows+3); id++)
        cache_info->nexus_info[id].available=True;
    }
  size=sizeof(PixelPacket);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    size+=sizeof(IndexPacket);
  offset=size*number_pixels;
  if (cache_info->columns != (offset/cache_info->rows/size))
    ThrowBinaryException(ResourceLimitWarning,"Pixel cache allocation failed",
      image->filename);
  cache_info->length=offset;
  if (cache_info->length != (size_t) cache_info->length)
    cache_info->type=DiskCache;
  if ((cache_info->type == MemoryCache) ||
      ((cache_info->type == UndefinedCache) &&
       (cache_info->length <= GetCacheMemory(0))))
    {
      if (cache_info->storage_class == UndefinedClass)
        pixels=(PixelPacket *) AcquireMemory(cache_info->length);
      else
        {
          ReacquireMemory((void **) &cache_info->pixels,cache_info->length);
          if (cache_info->pixels == (void *) NULL)
            ThrowBinaryException(ResourceLimitWarning,
              "Memory allocation failed",image->filename);
          pixels=cache_info->pixels;
        }
      if (pixels != (PixelPacket *) NULL)
        {
          /*
            Create in-memory pixel cache.
          */
          (void) GetCacheMemory(cache_info->length);
          cache_info->storage_class=image->storage_class;
          cache_info->colorspace=image->colorspace;
          cache_info->type=MemoryCache;
          cache_info->pixels=pixels;
          cache_info->indexes=(IndexPacket *) NULL;
          if ((cache_info->storage_class == PseudoClass) ||
              (cache_info->colorspace == CMYKColorspace))
            cache_info->indexes=(IndexPacket *) (pixels+number_pixels);
          return(True);
        }
    }
  /*
    Create pixel cache on disk.
  */
  if (*cache_info->cache_filename == '\0')
    TemporaryFilename(cache_info->cache_filename);
  file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
  if (file == -1)
    file=open(cache_info->cache_filename,O_RDWR | O_CREAT | O_BINARY,0777);
  if (file == -1)
    ThrowBinaryException(CacheWarning,"Unable to open cache",image->filename);
  if (lseek(file,cache_info->offset+cache_info->length,SEEK_SET) == -1)
    {
      (void) close(file);
      ThrowBinaryException(CacheWarning,"Unable to seek cache",image->filename)
    }
  if (write(file,&c,1) == -1)
    {
      (void) close(file);
      ThrowBinaryException(CacheWarning,"Unable to write cache",
        image->filename)
    }
  cache_info->storage_class=image->storage_class;
  cache_info->colorspace=image->colorspace;
  cache_info->type=DiskCache;
  if (cache_info->length == (size_t) cache_info->length)
    {
      pixels=(PixelPacket *) MapBlob(file,IOMode,cache_info->offset,
        cache_info->length);
      if (pixels != (PixelPacket *) NULL)
        {
          /*
            Create memory-mapped pixel cache.
          */
          cache_info->type=MemoryMappedCache;
          cache_info->pixels=pixels;
          cache_info->indexes=(IndexPacket *) NULL;
          if ((cache_info->storage_class == PseudoClass) ||
              (cache_info->colorspace == CMYKColorspace))
            cache_info->indexes=(IndexPacket *) (pixels+number_pixels);
        }
    }
  (void) close(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P e r s i s t C a c h e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PersistCache() attaches to or initializes a persistent pixel cache.  A
%  persistent pixel cache is one that resides on disk and is not destroyed
%  when the program exits.
%
%  The format of the PersistCache() method is:
%
%      unsigned int PersistCache(Image *image,const char *filename,
%        const unsigned int attach,off_t *offset,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: PersistCache() returns True if the persistent pixel cache is
%      attached or initialized successfully otherwise False.
%
%    o image: The image.
%
%    o filename: The persistent pixel cache filename.
%
%    o initialize: A value other than zero initializes the persistent pixel
%      cache.
%
%    o offset: The offset in the persistent cache to store pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int PersistCache(Image *image,const char *filename,
  const unsigned int attach,off_t *offset,ExceptionInfo *exception)
{
  CacheInfo
    *cache_info;

  Image
    *clone_image;

  IndexPacket
    *clone_indexes,
    *indexes;

  long
    y;

  register const PixelPacket
    *p;

  register PixelPacket
    *q;

  unsigned long
    pagesize;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (void *) NULL);
  assert(filename != (const char *) NULL);
  assert(offset != (off_t *) NULL);
  pagesize=4096;
#if defined(HAVE_GETPAGESIZE)
  pagesize=getpagesize();
#endif
#if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
  pagesize=sysconf(_SC_PAGE_SIZE);
#endif
  if (attach)
    {
      /*
        Attach persistent pixel cache.
      */
      cache_info=(CacheInfo *) image->cache;
      (void) strncpy(cache_info->cache_filename,filename,MaxTextExtent-1);
      cache_info->type=DiskCache;
      cache_info->offset=(*offset);
      if (!OpenCache(image))
        return(False);
      (void) ReferenceCache(cache_info);
      *offset+=(cache_info->length+1)+pagesize-
        ((cache_info->length+1) % pagesize);
      return(True);
    }
  /*
    Initialize persistent pixel cache.
  */
  clone_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (clone_image == (Image *) NULL)
    return(False);
  cache_info=(CacheInfo *) clone_image->cache;
  (void) strncpy(cache_info->cache_filename,filename,MaxTextExtent-1);
  cache_info->type=DiskCache;
  cache_info->offset=(*offset);
  if (!OpenCache(clone_image))
    return(False);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    q=SetImagePixels(clone_image,0,y,clone_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    (void) memcpy(q,p,image->columns*sizeof(PixelPacket));
    clone_indexes=GetIndexes(clone_image);
    indexes=GetIndexes(image);
    if ((clone_indexes != (IndexPacket *) NULL) &&
        (indexes != (IndexPacket *) NULL))
      (void) memcpy(clone_indexes,indexes,image->columns*sizeof(IndexPacket));
    if (!SyncImagePixels(clone_image))
      break;
  }
  (void) ReferenceCache(cache_info);
  DestroyImage(clone_image);
  if (y < (long) image->rows)
    return(False);
  *offset+=(cache_info->length+1)+pagesize-((cache_info->length+1) % pagesize);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C a c h e I n d e x e s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCacheIndexes() reads colormap indexes from the specified region
%  of the pixel cache.
%
%  The format of the ReadCacheIndexes() method is:
%
%      unsigned int ReadCacheIndexes(const Cache cache,
%        const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method ReadCacheIndexes returns True if the colormap indexes
%      are successfully read from the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o nexus: specifies which cache nexus to read the colormap indexes.
%
%
*/
static unsigned int ReadCacheIndexes(const Cache cache,
  const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  int
    file;

  off_t
    count,
    number_pixels,
    offset;

  register IndexPacket
    *indexes;

  register long
    y;

  register NexusInfo
    *nexus_info;

  register size_t
    i;

  size_t
    length;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if ((cache_info->storage_class != PseudoClass) &&
      (cache_info->colorspace != CMYKColorspace))
    return(False);
  if (IsNexusInCore(cache,nexus))
    return(True);
  nexus_info=cache_info->nexus_info+nexus;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  indexes=nexus_info->indexes;
  if (cache_info->type != DiskCache)
    {
      /*
        Read indexes from memory.
      */
      for (y=0; y < (long) nexus_info->rows; y++)
      {
        (void) memcpy(indexes,cache_info->indexes+offset,
          nexus_info->columns*sizeof(IndexPacket));
        indexes+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read indexes from disk.
  */
  file=open(cache_info->cache_filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    return(False);
  number_pixels=cache_info->columns*cache_info->rows;
  length=nexus_info->columns*sizeof(IndexPacket);
  for (y=0; y < (long) nexus_info->rows; y++)
  {
    count=lseek(file,cache_info->offset+number_pixels*sizeof(PixelPacket)+
      offset*sizeof(IndexPacket),SEEK_SET);
    if (count == -1)
      return(False);
    for (i=0; i < length; i+=count)
    {
      count=read(file,(char *) indexes+i,length-i);
      if (count <= 0)
        break;
    }
    if (i < length)
      return(False);
    indexes+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  (void) close(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C a c h e P i x e l s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCachePixels() reads pixels from the specified region of the
%  pixel cache.
%
%  The format of the ReadCachePixels() method is:
%
%      unsigned int ReadCachePixels(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method ReadCachePixels() returns True if the pixels are
%      successfully read from the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o nexus: specifies which cache nexus to read the pixels.
%
%
*/
static unsigned int ReadCachePixels(const Cache cache,const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  int
    file;

  off_t
    count,
    offset;

  register long
    y;

  register NexusInfo
    *nexus_info;

  register PixelPacket
    *pixels;

  register size_t
	  i;

  size_t
    length;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (IsNexusInCore(cache,nexus))
    return(True);
  nexus_info=cache_info->nexus_info+nexus;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  pixels=nexus_info->pixels;
  if (cache_info->type != DiskCache)
    {
      /*
        Read pixels from memory.
      */
      for (y=0; y < (long) nexus_info->rows; y++)
      {
        (void) memcpy(pixels,cache_info->pixels+offset,
          nexus_info->columns*sizeof(PixelPacket));
        pixels+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read pixels from disk.
  */
  file=open(cache_info->cache_filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    return(False);
  length=nexus_info->columns*sizeof(PixelPacket);
  for (y=0; y < (long) nexus_info->rows; y++)
  {
    count=lseek(file,cache_info->offset+offset*sizeof(PixelPacket),SEEK_SET);
    if (count == -1)
      return(False);
    for (i=0; i < length; i+=count)
    {
      count=read(file,(char *) pixels+i,length-i);
      if (count <= 0)
        break;
    }
    if (i < length)
      return(False);
    pixels+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  (void) close(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f e r e n c e C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReferenceCache() increments the reference count associated with the pixel
%  cache returning a pointer to the cache.
%
%  The format of the ReferenceCache method is:
%
%      Cache ReferenceCache(Cache cache_info)
%
%  A description of each parameter follows:
%
%    o cache_info: The cache_info.
%
%
*/
MagickExport Cache ReferenceCache(Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  AcquireSemaphoreInfo(&cache_info->semaphore);
  cache_info->reference_count++;
  LiberateSemaphoreInfo(&cache_info->semaphore);
  return(cache_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e s e t P i x e l C a c h e M e t h o d s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ResetPixelCacheMethods() sets the default image pixel methods to
%  use the pixel cache.
%
%
*/
MagickExport void ResetPixelCacheMethods(void)
{
  /*
    Reset image pixel methods.
  */
  acquire_pixel_handler=AcquirePixelCache;
  get_pixel_handler=GetPixelCache;
  set_pixel_handler=SetPixelCache;
  sync_pixel_handler=SyncPixelCache;
  get_pixels_from_handler=GetPixelsFromCache;
  get_indexes_from_handler=GetIndexesFromCache;
  acquire_one_pixel_from_handler=AcquireOnePixelFromCache;
  get_one_pixel_from_handler=GetOnePixelFromCache;
  destroy_pixel_handler=DestroyPixelCache;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t C a c h e N e x u s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetCacheNexus() allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with SyncPixelCache().  A
%  pointer to the pixels is returned if the pixels are transferred, otherwise
%  a NULL is returned.
%
%  The format of the SetCacheNexus() method is:
%
%      PixelPacket *SetCacheNexus(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows,
%        const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetCacheNexus() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o nexus: specifies which cache nexus to set.
%
%
*/
MagickExport PixelPacket *SetCacheNexus(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows,
  const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  off_t
    offset;

  RectangleInfo
    region;

  unsigned long
    number_pixels;

  assert(image != (Image *) NULL);
  assert(image->cache != (Cache) NULL);
  assert(image->signature == MagickSignature);
  if (ModifyCache(image) == False)
    return((PixelPacket *) NULL);
  /*
    Validate pixel cache geometry.
  */
  cache_info=(CacheInfo *) image->cache;
  offset=y*cache_info->columns+x;
  if (offset < 0)
    return((PixelPacket *) NULL);
  number_pixels=cache_info->columns*cache_info->rows;
  offset+=(rows-1)*cache_info->columns+columns-1;
  if (offset > number_pixels)
    return((PixelPacket *) NULL);
  /*
    Return pixel cache.
  */
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  return(SetNexus(image,&region,nexus));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t C a c h e T h e s h o l d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetCacheThreshold() sets the amount of free memory allocated for the
%  pixel cache.  Once this threshold is exceeded, all subsequent pixels cache
%  operations are to/from disk.
%
%  The format of the SetCacheThreshold() method is:
%
%      void SetCacheThreshold(const size_t threshold)
%
%  A description of each parameter follows:
%
%    o threshold: The number of megabytes of memory available to the pixel
%      cache.
%
%
*/
MagickExport void SetCacheThreshold(const off_t threshold)
{
  cache_memory=1024L*1024L*threshold-1L;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImagePixels() allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with SyncImagePixels().  A
%  pointer to the pixels is returned if the pixels are transferred, otherwise
%  a NULL is returned.
%
%  The format of the SetImagePixels() method is:
%
%      PixelPacket *SetImagePixels(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetImagePixels returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
MagickExport PixelPacket *SetImagePixels(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  if (set_pixel_handler == (SetPixelHandler) NULL)
    return((PixelPacket *) NULL);
  return((*set_pixel_handler)(image,x,y,columns,rows));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t N e x u s                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetNexus() defines the region of the cache for the specified
%  cache nexus.
%
%  The format of the SetNexus() method is:
%
%      PixelPacket SetNexus(const Image *image,const RectangleInfo *region,
%        const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetNexus() returns a pointer to the pixels
%      associated with the specified cache nexus.
%
%    o image: The image.
%
%    o nexus: specifies which cache nexus to set.
%
%    o region: A pointer to the RectangleInfo structure that defines the
%      region of this particular cache nexus.
%
%
*/
static PixelPacket *SetNexus(const Image *image,const RectangleInfo *region,
  const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  off_t
    offset;

  register NexusInfo
    *nexus_info;

  unsigned long
    number_pixels;

  assert(image != (Image *) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  nexus_info=cache_info->nexus_info+nexus;
  nexus_info->columns=region->width;
  nexus_info->rows=region->height;
  nexus_info->x=region->x;
  nexus_info->y=region->y;
  if ((cache_info->type != DiskCache) && (image->clip_mask == (Image *) NULL))
    {
      unsigned long
        number_pixels,
        span;

      offset=nexus_info->y*cache_info->columns+nexus_info->x;
      span=(nexus_info->rows-1)*cache_info->columns+nexus_info->columns-1;
      number_pixels=cache_info->columns*cache_info->rows;
      if ((offset >= 0) && ((offset+span) <= (off_t) number_pixels))
        if ((((nexus_info->x+nexus_info->columns) <= cache_info->columns) &&
            (nexus_info->rows == 1)) || ((nexus_info->x == 0) &&
            ((nexus_info->columns % cache_info->columns) == 0)))
          {
            /*
              Pixels are accessed directly from memory.
            */
            nexus_info->pixels=cache_info->pixels+offset;
            nexus_info->indexes=(IndexPacket *) NULL;
            if ((cache_info->storage_class == PseudoClass) ||
                (cache_info->colorspace == CMYKColorspace))
              nexus_info->indexes=cache_info->indexes+offset;
            return(nexus_info->pixels);
          }
    }
  /*
    Pixels are stored in a staging area until they are synced to the cache.
  */
  number_pixels=nexus_info->columns*nexus_info->rows;
  offset=number_pixels*sizeof(PixelPacket);
  if ((cache_info->storage_class == PseudoClass) ||
      (cache_info->colorspace == CMYKColorspace))
    offset+=number_pixels*sizeof(IndexPacket);
  if (nexus_info->staging == (PixelPacket *) NULL)
    nexus_info->staging=(PixelPacket *) AcquireMemory(offset);
  else
    if (nexus_info->length != offset)
      ReacquireMemory((void **) &nexus_info->staging,offset);
  if (nexus_info->staging == (PixelPacket *) NULL)
    MagickError(ResourceLimitError,"Memory allocation failed",
      "unable to allocate cache nexus_info");
  nexus_info->length=offset;
  nexus_info->pixels=nexus_info->staging;
  nexus_info->indexes=(IndexPacket *) NULL;
  if ((cache_info->storage_class == PseudoClass) ||
      (cache_info->colorspace == CMYKColorspace))
    nexus_info->indexes=(IndexPacket *) (nexus_info->pixels+number_pixels);
  return(nexus_info->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetPixelCache() allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with SyncPixelCache().  A
%  pointer to the pixels is returned if the pixels are transferred, otherwise
%  a NULL is returned.
%
%  The format of the SetPixelCache() method is:
%
%      PixelPacket *SetPixelCache(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetPixelCache() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *SetPixelCache(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  return(SetCacheNexus(image,x,y,columns,rows,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t P i x e l C a c h e M e t h o d s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetPixelCacheMethods() sets the image pixel methods to the specified
%  ones.
%
%      SetPixelCacheMethods(AcquirePixelHandler acquire_pixel,
%        GetPixelHandler get_pixel,SetPixelHandler set_pixel,
%        SyncPixelHandler sync_pixel,GetPixelsFromHandler get_pixels_from,
%        GetIndexesFromHandler get_indexes_from,
%        AcquireOnePixelFromHandler acquire_one_pixel_from,
%        GetOnePixelFromHandler get_one_pixel_from,
%        ClosePixelHandler close_pixel,DestroyPixelHandler destroy_pixel)
%
%
*/
MagickExport void SetPixelCacheMethods(AcquirePixelHandler acquire_pixel,
  GetPixelHandler get_pixel,SetPixelHandler set_pixel,
  SyncPixelHandler sync_pixel,GetPixelsFromHandler get_pixels_from,
  GetIndexesFromHandler get_indexes_from,
  AcquireOnePixelFromHandler acquire_one_pixel_from,
  GetOnePixelFromHandler get_one_pixel_from,DestroyPixelHandler destroy_pixel)
{
  /*
    Set image pixel methods.
  */
  assert(acquire_pixel != (AcquirePixelHandler) NULL);
  assert(get_pixel != (GetPixelHandler) NULL);
  assert(set_pixel != (SetPixelHandler) NULL);
  assert(sync_pixel != (SyncPixelHandler) NULL);
  assert(get_pixels_from != (GetPixelsFromHandler) NULL);
  assert(get_indexes_from != (GetIndexesFromHandler) NULL);
  assert(acquire_one_pixel_from != (AcquireOnePixelFromHandler) NULL);
  assert(get_one_pixel_from != (GetOnePixelFromHandler) NULL);
  assert(destroy_pixel != (DestroyPixelHandler) NULL);
  acquire_pixel_handler=acquire_pixel;
  get_pixel_handler=get_pixel;
  set_pixel_handler=set_pixel;
  sync_pixel_handler=sync_pixel;
  get_pixels_from_handler=get_pixels_from;
  get_indexes_from_handler=get_indexes_from;
  acquire_one_pixel_from_handler=acquire_one_pixel_from;
  get_one_pixel_from_handler=get_one_pixel_from;
  destroy_pixel_handler=destroy_pixel;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c C a c h e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncCache() synchronizes the image with the pixel cache.
%
%  The format of the SyncCache() method is:
%
%      unsigned int SyncCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncCache returns True if the pixel cache is synchronized
%      successfully otherwise False.
%
%    o image: The image.
%
%
*/
static unsigned int SyncCache(Image *image)
{
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (void *) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if ((image->storage_class != cache_info->storage_class) ||
      (image->colorspace != cache_info->colorspace))
    if (!OpenCache(image))
      return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c C a c h e N e x u s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncCacheNexus() saves the image pixels to the in-memory or disk
%  cache.  The method returns True if the pixel region is synced, otherwise
%  False.
%
%  The format of the SyncCacheNexus() method is:
%
%      unsigned int SyncCacheNexus(Image *image,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method SyncCacheNexus() returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The image.
%
%    o nexus: specifies which cache nexus to sync.
%
%
*/
MagickExport unsigned int SyncCacheNexus(Image *image,const unsigned long nexus)
{
  unsigned int
    status;

  /*
    Transfer pixels to the cache.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->cache == (Cache) NULL)
    ThrowBinaryException(CacheWarning,"pixel cache is not open",
      image->filename);
  image->taint=True;
  if (IsNexusInCore(image->cache,nexus))
    return(True);
  if (image->clip_mask != (Image *) NULL)
    if (!ClipCacheNexus(image,nexus))
      return(False);
  status=WriteCachePixels(image->cache,nexus);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    status|=WriteCacheIndexes(image->cache,nexus);
  if (status == False)
    ThrowBinaryException(CacheWarning,"Unable to sync pixel cache",
      image->filename);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c I m a g e P i x e l s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncImagePixels() saves the image pixels to the in-memory or disk
%  cache.  The method returns True if the pixel region is synced, otherwise
%  False.
%
%  The format of the SyncImagePixels() method is:
%
%      unsigned int SyncImagePixels(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncImagePixels() returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int SyncImagePixels(Image *image)
{
  if (sync_pixel_handler == (SyncPixelHandler) NULL)
    return(False);
  return((*sync_pixel_handler)(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SyncPixelCache() saves the image pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncPixelCache() method is:
%
%      unsigned int SyncPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static unsigned int SyncPixelCache(Image *image)
{
  return(SyncCacheNexus(image,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e C a c h e I n d e x e s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCacheIndexes() writes the colormap indexes to the specified
%  region of the pixel cache.
%
%  The format of the WriteCacheIndexes() method is:
%
%      unsigned int WriteCacheIndexes(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCacheIndexes() returns True if the indexes are
%      successfully written to the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o nexus: specifies which cache nexus to write the colormap indexes.
%
%
*/
static unsigned int WriteCacheIndexes(Cache cache,const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  int
    file;

  off_t
    count,
    number_pixels,
    offset;

  register IndexPacket
    *indexes;

  register long
    y;

  register NexusInfo
    *nexus_info;

  register size_t
    i;

  size_t
    length;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if ((cache_info->storage_class != PseudoClass) &&
      (cache_info->colorspace != CMYKColorspace))
    return(False);
  if (IsNexusInCore(cache,nexus))
    return(True);
  nexus_info=cache_info->nexus_info+nexus;
  indexes=nexus_info->indexes;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  if (cache_info->type != DiskCache)
    {
      /*
        Write indexes to memory.
      */
      for (y=0; y < (long) nexus_info->rows; y++)
      {
        (void) memcpy(cache_info->indexes+offset,indexes,
          nexus_info->columns*sizeof(IndexPacket));
        indexes+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write indexes to disk.
  */
  file=open(cache_info->cache_filename,O_WRONLY | O_BINARY,0777);
  if (file == -1)
    return(False);
  number_pixels=cache_info->columns*cache_info->rows;
  length=nexus_info->columns*sizeof(IndexPacket);
  for (y=0; y < (long) nexus_info->rows; y++)
  {
    count=lseek(file,cache_info->offset+number_pixels*sizeof(PixelPacket)+
      offset*sizeof(IndexPacket),SEEK_SET);
    if (count == -1)
      return(False);
    for (i=0; i < length; i+=count)
    {
      count=write(file,(char *) indexes+i,length-i);
      if (count <= 0)
        break;
    }
    if (i < length)
      return(False);
    indexes+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  (void) close(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e C a c h e P i x e l s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCachePixels() writes image pixels to the specified region of the
%  pixel cache.
%
%  The format of the WriteCachePixels() method is:
%
%      unsigned int WriteCachePixels(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCachePixels() returns True if the pixels are
%      successfully written to the cache, otherwise False.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o nexus: specifies which cache nexus to write the pixels.
%
%
*/
static unsigned int WriteCachePixels(Cache cache,const unsigned long nexus)
{
  CacheInfo
    *cache_info;

  int
    file;

  off_t
    count,
    offset;

  register long
    y;

  register NexusInfo
    *nexus_info;

  register PixelPacket
    *pixels;

  register size_t
	  i;

  size_t
    length;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (IsNexusInCore(cache,nexus))
    return(True);
  nexus_info=cache_info->nexus_info+nexus;
  pixels=nexus_info->pixels;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  if (cache_info->type != DiskCache)
    {
      /*
        Write pixels to memory.
      */
      for (y=0; y < (long) nexus_info->rows; y++)
      {
        (void) memcpy(cache_info->pixels+offset,pixels,
          nexus_info->columns*sizeof(PixelPacket));
        pixels+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write pixels to disk.
  */
  file=open(cache_info->cache_filename,O_WRONLY | O_BINARY,0777);
  if (file == -1)
    return(False);
  length=nexus_info->columns*sizeof(PixelPacket);
  for (y=0; y < (long) nexus_info->rows; y++)
  {
    count=lseek(file,cache_info->offset+offset*sizeof(PixelPacket),SEEK_SET);
    if (count == -1)
      return(False);
    for (i=0; i < length; i+=count)
    {
      count=write(file,(char *) pixels+i,length-i);
      if (count <= 0)
        break;
    }
    if (i < length)
      return(False);
    pixels+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  (void) close(file);
  return(True);
}
