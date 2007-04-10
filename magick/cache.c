/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
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
%                     GraphicsMagick Pixel Cache Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/list.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/resource.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#if defined(HasZLIB)
#include "zlib.h"
#endif

/*
  Define declarations.
*/
#define MaxCacheViews  (Max(cache_info->columns,cache_info->rows)+3)

#if defined(POSIX) && defined(S_IRUSR)
#  define S_MODE     (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#elif defined (WIN32)
#  define S_MODE     (_S_IREAD | _S_IWRITE)
#else
# define S_MODE      0644 
#endif

/*
 Windows open extensions
*/
#if defined(__BORLANDC__)
/* These do not appear to work with Borland C++ */
#  undef _O_RANDOM
#  undef _O_SEQUENTIAL
#  undef _O_SHORT_LIVED
#  undef _O_TEMPORARY
#endif

#if !defined(_O_RANDOM)
# define _O_RANDOM 0
#endif
#if !defined(_O_SEQUENTIAL)
# define _O_SEQUENTIAL 0
#endif
#if !defined(_O_SHORT_LIVED)
# define _O_SHORT_LIVED 0
#endif
#if !defined(_O_TEMPORARY)
# define _O_TEMPORARY 0
#endif


/*
  Declare pixel cache interfaces.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static const PixelPacket
  *AcquirePixelCache(const Image *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

static IndexPacket
  *GetIndexesFromCache(const Image *);

static PixelPacket
  AcquireOnePixelFromCache(const Image *,const long,const long,ExceptionInfo *),
  GetOnePixelFromCache(Image *,const long,const long),
  *GetPixelCache(Image *,const long,const long,const unsigned long,
    const unsigned long),
  *GetPixelsFromCache(const Image *),
  *SetPixelCache(Image *,const long,const long,const unsigned long,
    const unsigned long);

static unsigned int
  SyncPixelCache(Image *);

static void
  DestroyPixelCache(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
  Forward declaration.
*/
static inline unsigned int
  IsNexusInCore(const Cache,const unsigned long);

static PixelPacket
  *SetNexus(const Image *,const RectangleInfo *,const unsigned long);

static unsigned int
  ReadCacheIndexes(const Cache,const unsigned long),
  ReadCachePixels(const Cache,const unsigned long),
  SyncCache(Image *),
  WriteCacheIndexes(Cache,const unsigned long),
  WriteCachePixels(Cache,const unsigned long);

/*

  Read data into buffer 'buffer', with size 'length', from file
  descriptor 'file' at offset 'offset'. Care is taken to make sure
  that all data requested is read if system calls are interrupted by
  signals.

*/
static inline long FilePositionRead(int file, void *buffer, size_t length,
  magick_off_t offset)
{
  register long
    count=0;

  register size_t
    total_count;

#if !HAVE_PREAD
  if ((MagickSeek(file,offset,SEEK_SET)) < 0)
    return -1;
#endif

  for (total_count=0; total_count < length; total_count+=count)
    {
#if HAVE_PREAD
      count=pread(file,(char *) buffer+total_count,length-total_count,
        offset+total_count);
#else
      count=read(file,(char *) buffer+total_count,length-total_count);
#endif
      if (count <= 0)
        break;
    }
  if (count < 0)
    return -1;
  return (long) total_count;
}
/*

  Write data pointed to by 'buffer', with size 'length', to file
  descriptor 'file' at specified offset 'offset'.  Care is taken to
  make sure that all data is written if system calls are interrupted
  by signals.

*/
static inline long FilePositionWrite(int file, const void *buffer,
  size_t length,magick_off_t offset)
{
  register long
    count=0;

  register size_t
    total_count;

#if !HAVE_PWRITE
  if ((MagickSeek(file,offset,SEEK_SET)) < 0)
    return -1;
#endif /* !HAVE_PWRITE */
  for (total_count=0; total_count < length; total_count+=count)
    {
#if HAVE_PWRITE
      count=pwrite(file,(char *) buffer+total_count,length-total_count,
        offset+total_count);
#else
      count=write(file,(char *) buffer+total_count,length-total_count);
#endif
      if (count <= 0)
        break;
    }
  if (count < 0)
    return -1;
  return (long) total_count;
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
%  IsNexusInCore() returns true if the pixels associated with the specified
%  cache nexus is non-strided and in core.
%
%  The format of the IsNexusInCore() method is:
%
%      unsigned int IsNexusInCore(const Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: IsNexusInCore() returns True if the pixels are non-strided and
%      in core, otherwise False.
%
%    o nexus: specifies which cache nexus to return the pixels.
%
%
*/
static inline unsigned int IsNexusInCore(const Cache cache,
  const unsigned long nexus)
{
  unsigned int
    status=False;

  CacheInfo
    *cache_info=(CacheInfo *) cache;

  if (cache_info && (cache_info->storage_class != UndefinedClass))
  {
    register NexusInfo
      *nexus_info;

    magick_off_t
      offset;

    assert(cache_info->signature == MagickSignature);
    nexus_info=cache_info->nexus_info+nexus;
    offset=nexus_info->y*(magick_off_t) cache_info->columns+nexus_info->x;
    if (nexus_info->pixels == (cache_info->pixels+offset))
      status=True;
  }
  return(status);
}

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
%    o status: AcquireCacheNexus() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
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
#define EdgeX(x) ((x) < 0 ? 0 : (x) >= (long) cache_info->columns ? \
  (long) cache_info->columns-1 : (x))
#define EdgeY(y) ((y) < 0 ? 0 : (y) >= (long) cache_info->rows ? \
  (long) cache_info->rows-1 : (y))
#define MirrorX(x) ((((x) >= 0) && (x) < (long) cache_info->columns) ? \
  (x) : (long) cache_info->columns-TileX(x))
#define MirrorY(y) ((((y) >= 0) && (y) < (long) cache_info->rows) ? \
  (y) : (long) cache_info->rows-TileY(y))
#define TileX(x) (((x) >= 0) ? ((x) % (long) cache_info->columns) : \
  (long) cache_info->columns-(-(x) % (long) cache_info->columns))
#define TileY(y) (((y) >= 0) ? ((y) % (long) cache_info->rows) : \
  (long) cache_info->rows-(-(y) % (long) cache_info->rows))

  CacheInfo
    *cache_info;

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  IndexPacket
    *indexes,
    *nexus_indexes;

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

  size_t
    length;

  unsigned long
    image_nexus;

  /*
    Acquire pixels.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  if (cache_info->type == UndefinedCache)
    {
      ThrowException(exception,CacheError,PixelCacheIsNotOpen,
        image->filename);
      return((const PixelPacket *) NULL);
    }
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  pixels=SetNexus(image,&region,nexus);
  offset=region.y*(magick_off_t) cache_info->columns+region.x;
  length=(region.height-1)*cache_info->columns+region.width-1;
  number_pixels=(magick_uint64_t) cache_info->columns*cache_info->rows;
  if ((offset >= 0) && ((offset+length) < number_pixels))
    if ((x >= 0) && ((x+columns) <= cache_info->columns) &&
        (y >= 0) && ((y+rows) <= cache_info->rows))
      {
        unsigned int
          status;

        /*
          Pixel request is inside cache extents.
        */
        if (IsNexusInCore(cache_info,nexus))
          return(pixels);
        status=ReadCachePixels(cache_info,nexus);
        if ((image->storage_class == PseudoClass) ||
            (image->colorspace == CMYKColorspace))
          status|=ReadCacheIndexes(cache_info,nexus);
        if (status == False)
          {
            ThrowException(exception,CacheError,UnableToReadPixelCache,
              image->filename);
            return((const PixelPacket *) NULL);
          }
        return(pixels);
      }
  /*
    Pixel request is outside cache extents.
  */
  indexes=GetNexusIndexes(cache_info,nexus);
  image_nexus=GetNexus(cache_info);
  if (image_nexus == 0)
    {
      ThrowException(exception,CacheError,UnableToGetCacheNexus,
        image->filename);
      return((const PixelPacket *) NULL);
    }
  cache_info->virtual_pixel=image->background_color;
  q=pixels;
  for (v=0; v < (long) rows; v++)
  {
    for (u=0; u < (long) columns; u+=length)
    {
      length=Min(cache_info->columns-(x+u),columns-u);
      if ((((x+u) < 0) || ((x+u) >= (long) cache_info->columns)) ||
          (((y+v) < 0) || ((y+v) >= (long) cache_info->rows)) || (length == 0))
        {
          /*
            Transfer a single pixel.
          */
          length=1;
          switch (cache_info->virtual_pixel_method)
          {
            case ConstantVirtualPixelMethod:
            {
              (void) AcquireCacheNexus(image,EdgeX(x+u),EdgeY(y+v),1,1,
                image_nexus,exception);
              p=(&cache_info->virtual_pixel);
              break;
            }
            case EdgeVirtualPixelMethod:
            default:
            {
              p=AcquireCacheNexus(image,EdgeX(x+u),EdgeY(y+v),1,1,image_nexus,
                exception);
              break;
            }
            case MirrorVirtualPixelMethod:
            {
              p=AcquireCacheNexus(image,MirrorX(x+u),MirrorY(y+v),1,1,
                image_nexus,exception);
              break;
            }
            case TileVirtualPixelMethod:
            {
              p=AcquireCacheNexus(image,TileX(x+u),TileY(y+v),1,1,image_nexus,
                exception);
              break;
            }
          }
          if (p == (const PixelPacket *) NULL)
            break;
          *q++=(*p);
          if (indexes == (IndexPacket *) NULL)
            continue;
          nexus_indexes=GetNexusIndexes(cache_info,image_nexus);
          if (nexus_indexes == (IndexPacket *) NULL)
            continue;
          *indexes++=(*nexus_indexes);
          continue;
        }
      /*
        Transfer a run of pixels.
      */
      p=AcquireCacheNexus(image,x+u,y+v,length,1,image_nexus,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      (void) memcpy(q,p,length*sizeof(PixelPacket));
      q+=length;
      if (indexes == (IndexPacket *) NULL)
        continue;
      nexus_indexes=GetNexusIndexes(cache_info,image_nexus);
      if (nexus_indexes == (IndexPacket *) NULL)
        continue;
      (void) memcpy(indexes,nexus_indexes,length*sizeof(IndexPacket));
      indexes+=length;
    }
  }
  DestroyCacheNexus(cache_info,image_nexus);
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
%  AcquireImagePixels() acquires pixels from the in-memory or disk pixel cache
%  as defined by the geometry parameters.   A pointer to the pixel is returned
%  if the pixels are transferred, otherwise a NULL is returned.  If you plan
%  to modify the pixels, use GetImagePixels() instead.
%
%  The format of the AcquireImagePixels() method is:
%
%      const PixelPacket *AcquireImagePixels(const Image *image,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: AcquireImagePixels() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
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
  CacheInfo
    *cache_info;

  const PixelPacket
    *pixels;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.acquire_pixel_handler == (AcquirePixelHandler) NULL)
    return((const PixelPacket *) NULL);
  pixels=cache_info->methods.
    acquire_pixel_handler(image,x,y,columns,rows,exception);
  return(pixels);
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
%  AcquireOnePixel() returns a single pixel at the specified (x,y) location.
%  The image background color is returned if an error occurs.  If you plan to
%  modify the pixel, use GetOnePixel() instead.
%
%  The format of the AcquireOnePixel() method is:
%
%      PixelPacket AcquireOnePixel(const Image image,const long x,
%        const long y,ExceptionInfo exception)
%
%  A description of each parameter follows:
%
%    o pixels: AcquireOnePixel() returns a pixel at the specified (x,y)
%      location.
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
  CacheInfo
    *cache_info;

  PixelPacket
    pixel;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.acquire_one_pixel_from_handler ==
      (AcquireOnePixelFromHandler) NULL)
    return(image->background_color);
  pixel=cache_info->methods.acquire_one_pixel_from_handler(image,x,y,exception);
  return(pixel);
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
%  AcquireOnePixelFromCache() returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%
%  The format of the AcquireOnePixelFromCache() method is:
%
%      PixelPacket *AcquireOnePixelFromCache(const Image image,const long x,
%        const long y,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o pixels: AcquireOnePixelFromCache returns a pixel at the specified (x,y)
%      location.
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
%  ClipCacheNexus() clips the image pixels of the in-memory or disk cache as
%  defined by the image clip mask.  The method returns True if the pixel region
%  is clipped, otherwise False.
%
%  The format of the ClipCacheNexus() method is:
%
%      unsigned int ClipCacheNexus(Image *image,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: ClipCacheNexus() returns True if the image pixels are clipped,
%      otherwise False.
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

  register const PixelPacket
    *r;

  register long
    x;

  register PixelPacket
    *p,
    *q;

  unsigned long
    image_nexus,
    mask_nexus;

  /*
    Apply clip mask.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image_nexus=GetNexus(image->cache);
  mask_nexus=GetNexus(image->clip_mask->cache);
  if ((image_nexus == 0) || (mask_nexus == 0))
    ThrowBinaryException(CacheError,UnableToGetCacheNexus,image->filename);
  cache_info=(CacheInfo *) image->cache;
  nexus_info=cache_info->nexus_info+nexus;
  p=GetCacheNexus(image,nexus_info->x,nexus_info->y,nexus_info->columns,
    nexus_info->rows,image_nexus);
  q=nexus_info->pixels;
  r=AcquireCacheNexus(image->clip_mask,nexus_info->x,nexus_info->y,
    nexus_info->columns,nexus_info->rows,mask_nexus,&image->exception);
  if ((p != (PixelPacket *) NULL) && (r != (const PixelPacket *) NULL))
    for (y=0; y < (long) nexus_info->rows; y++)
    {
      for (x=0; x < (long) nexus_info->columns; x++)
      {
        if (r->red == TransparentOpacity)
          q->red=p->red;
        if (r->green == TransparentOpacity)
          q->green=p->green;
        if (r->blue == TransparentOpacity)
          q->blue=p->blue;
        if (r->opacity == TransparentOpacity)
          q->opacity=p->opacity;
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
+   C l o n e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClonePixelCache() clones the pixel cache pixels from one cache to another.
%
%  The format of the ClonePixelCache() method is:
%
%      unsigned int ClonePixelCache(Image *image,Image *clone_image)
%
%  A description of each parameter follows:
%
%    o clone: Specifies a pointer to a Cache structure.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
static unsigned int ClonePixelCache(Image *image,Image *clone_image)
{
#define MaxBufferSize  65541

  CacheInfo
    *cache_info,
    *clone_info;

  char
    *buffer;

  ssize_t
    count;

  int
    cache_file,
    clone_file;

  register size_t
    i;

  size_t
    length;

  cache_info=(CacheInfo *) image->cache;
  clone_info=(CacheInfo *) clone_image->cache;
  if (cache_info->length != clone_info->length)
    {
      Image
        *clip_mask;

      long
        y;

      register const PixelPacket
        *p;

      register IndexPacket
        *clone_indexes,
        *indexes;

      register PixelPacket
        *q;

      /*
        Unoptimized pixel cache clone.
      */
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),"unoptimized");
      clip_mask=clone_image->clip_mask;
      clone_image->clip_mask=(Image *) NULL;
      length=Min(image->columns,clone_image->columns);
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        q=SetImagePixels(clone_image,0,y,image->columns,1);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          break;
        (void) memcpy(q,p,length*sizeof(PixelPacket));
        indexes=GetIndexes(image);
        clone_indexes=GetIndexes(clone_image);
        if ((indexes != (IndexPacket *) NULL) &&
            (clone_indexes != (IndexPacket *) NULL))
          (void) memcpy(clone_indexes,indexes,length*sizeof(IndexPacket));
        if (!SyncImagePixels(clone_image))
          break;
      }
      clone_image->clip_mask=clip_mask;
      return(y == (long) image->rows);
    }
  /*
    Optimized pixel cache clone.
  */
  if ((cache_info->type != DiskCache) && (clone_info->type != DiskCache))
    {
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),"memory => memory");
      (void) memcpy(clone_info->pixels,cache_info->pixels,
        (size_t) cache_info->length);
      return(True);
    }
  cache_file=cache_info->file;
  if (cache_info->type == DiskCache)
    {
      if (cache_info->file == -1)
        {
          cache_file=open(cache_info->cache_filename,O_RDONLY | O_BINARY);
          if (cache_file == -1)
            ThrowBinaryException(FileOpenError,UnableToOpenFile,
              cache_info->cache_filename);
        }
      (void) MagickSeek(cache_file,cache_info->offset,SEEK_SET);
      if (clone_info->type != DiskCache)
        {
          (void) LogMagickEvent(CacheEvent,GetMagickModule(),"disk => memory");
          for (i=0; i < cache_info->length; i+=count)
          {
            count=read(cache_file,(char *) clone_info->pixels+i,
              (size_t) (cache_info->length-i));
            if (count <= 0)
              break;
          }
          if (cache_info->file == -1)
            (void) close(cache_file);
          if (i < cache_info->length)
            ThrowBinaryException(CacheError,UnableToCloneCache,
              image->filename);
          return(True);
        }
    }
  clone_file=clone_info->file;
  if (clone_info->type == DiskCache)
    {
      if (clone_info->file == -1)
        {
          clone_file=open(clone_info->cache_filename,O_WRONLY | O_BINARY |
            O_EXCL,S_MODE);
          if (clone_file == -1)
            clone_file=open(clone_info->cache_filename,O_WRONLY | O_BINARY,
              S_MODE);
          if (clone_file == -1)
            {
              if (cache_info->file == -1)
                (void) close(cache_file);
              ThrowBinaryException(FileOpenError,UnableToOpenFile,
                clone_info->cache_filename)
            }
        }
      (void) MagickSeek(clone_file,cache_info->offset,SEEK_SET);
      if (cache_info->type != DiskCache)
        {
          (void) LogMagickEvent(CacheEvent,GetMagickModule(),"memory => disk");
          for (i=0; i < clone_info->length; i+=count)
          {
            count=write(clone_file,(char *) cache_info->pixels+i,
              (size_t) (clone_info->length-i));
            if (count <= 0)
              break;
          }
          if (clone_info->file == -1)
            (void) close(clone_file);
          if (i < clone_info->length)
            ThrowBinaryException(CacheError,UnableToCloneCache,
              image->filename);
          return(True);
        }
    }
  (void) LogMagickEvent(CacheEvent,GetMagickModule(),"disk => disk");
  buffer=MagickAllocateMemory(char *,MaxBufferSize);
  if (buffer == (char *) NULL)
    {
      if (cache_info->file == -1)
        (void) close(cache_file);
      if (clone_info->file == -1)
        (void) close(clone_file);
      ThrowBinaryException3(ResourceLimitFatalError,MemoryAllocationFailed,
        UnableToCloneImage)
    }
  (void) MagickSeek(cache_file,cache_info->offset,SEEK_SET);
  (void) MagickSeek(clone_file,cache_info->offset,SEEK_SET);
  for (i=0; (length=read(cache_file,buffer,MaxBufferSize)) > 0; )
  {
    for (i=0; i < length; i+=count)
    {
      count=write(clone_file,buffer+i,length-i);
      if (count <= 0)
        break;
    }
    if (i < length)
      break;
  }
  if (cache_info->file == -1)
    (void) close(cache_file);
  if (clone_info->file == -1)
    (void) close(clone_file);
  MagickFreeMemory(buffer);
  if (i < length)
    ThrowBinaryException(CacheError,UnableToCloneCache,image->filename);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o n e P i x e l C a c h e M e t h o d s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClonePixelCacheMethods() clones the pixel cache methods from one cache to
%  another.
%
%  The format of the ClonePixelCacheMethods() method is:
%
%      void ClonePixelCacheMethods(Cache clone,const Cache cache)
%
%  A description of each parameter follows:
%
%    o clone: Specifies a pointer to a Cache structure.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport void ClonePixelCacheMethods(Cache clone,const Cache cache)
{
  CacheInfo
    *cache_info,
    *clone_info;

  assert(clone != (Cache) NULL);
  clone_info=(CacheInfo *) clone;
  assert(clone_info->signature == MagickSignature);
  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  clone_info->methods=cache_info->methods;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y C a c h e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyCacheInfo() deallocates memory associated with the pixel cache.
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
MagickExport void DestroyCacheInfo(Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  AcquireSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  cache_info->reference_count--;
  if (cache_info->reference_count > 0)
    {
      LiberateSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
      return;
    }
  LiberateSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  switch (cache_info->type)
  {
    default:
    {
      if (cache_info->pixels == (PixelPacket *) NULL)
        break;
    }
    case MemoryCache:
    {
      MagickFreeMemory(cache_info->pixels);
      LiberateMagickResource(MemoryResource,cache_info->length);
      break;
    }
    case MapCache:
    {
      (void) UnmapBlob(cache_info->pixels,(size_t) cache_info->length);
      LiberateMagickResource(MapResource,cache_info->length);
    }
    case DiskCache:
    {
      if (cache_info->file != -1)
        {
          close(cache_info->file);
          LiberateMagickResource(FileResource,1);
        }
      cache_info->file=(-1);
      LiberateTemporaryFile(cache_info->cache_filename);
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),
        "remove %.1024s (%.1024s)",cache_info->filename,
        cache_info->cache_filename);
      LiberateMagickResource(DiskResource,cache_info->length);
      break;
    }
  }
  if (cache_info->type != UndefinedCache)
    {
      register long
        id;

      for (id=0; id < (long) MaxCacheViews; id++)
        DestroyCacheNexus(cache,id);
      MagickFreeMemory(cache_info->nexus_info);
    }
  if (cache_info->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  (void) LogMagickEvent(CacheEvent,GetMagickModule(),"destroy %.1024s",
     cache_info->filename);
  MagickFreeMemory(cache_info);
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
%  DestroyCacheNexus() destroys a cache nexus.
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
    MagickFreeMemory(nexus_info->staging);
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
%  DestroyImagePixels() deallocates memory associated with the pixel cache.
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
  CacheInfo
    *cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.destroy_pixel_handler == (DestroyPixelHandler) NULL)
    return;
  cache_info->methods.destroy_pixel_handler(image);
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
%  DestroyPixelCache() deallocates memory associated with the pixel cache.
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
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->cache == (void *) NULL)
    return;
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
%  GetCacheClass() returns the class type of the pixel cache.
%
%  The format of the GetCacheClass() method is:
%
%      ClassType GetCacheClass(Cache cache)
%
%  A description of each parameter follows:
%
%    o type: GetCacheClass returns DirectClass or PseudoClass.
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
%  GetCacheColorspace() returns the class type of the pixel cache.
%
%  The format of the GetCacheColorspace() method is:
%
%      Colorspace GetCacheColorspace(Cache cache)
%
%  A description of each parameter follows:
%
%    o type: GetCacheColorspace returns DirectClass or PseudoClass.
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
%  GetCacheInfo() initializes the Cache structure.
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
  cache_info=MagickAllocateMemory(CacheInfo *,sizeof(CacheInfo));
  if (cache_info == (CacheInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateCacheInfo);
  (void) memset(cache_info,0,sizeof(CacheInfo));
  cache_info->colorspace=RGBColorspace;
  cache_info->reference_count=1;
  cache_info->file=(-1);
  cache_info->signature=MagickSignature;
  SetPixelCacheMethods(cache_info,AcquirePixelCache,GetPixelCache,
    SetPixelCache,SyncPixelCache,GetPixelsFromCache,GetIndexesFromCache,
    AcquireOnePixelFromCache,GetOnePixelFromCache,DestroyPixelCache);
  *cache=cache_info;
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
%  GetCacheNexus() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetCacheNexus() method is:
%
%      PixelPacket *GetCacheNexus(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o status: GetCacheNexus() returns a pointer to the pixels if they are
%      transferred, otherwise a NULL is returned.
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
      ThrowException(&image->exception,CacheError,UnableToGetPixelsFromCache,
        image->filename);
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
%  GetImagePixels() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetImagePixels() method is:
%
%      PixelPacket *GetImagePixels(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o status: GetImagePixels() returns a pointer to the pixels if they are
%      transferred, otherwise a NULL is returned.
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
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.get_pixel_handler == (GetPixelHandler) NULL)
    return((PixelPacket *) NULL);
  return(cache_info->methods.get_pixel_handler(image,x,y,columns,rows));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e V i r t u a l P i x e l M e t h o d                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageVirtualPixelMethod() gets the "virtual pixels" method for the
%  image.  A virtual pixel is any pixel access that is outside the boundaries
%  of the image cache.
%
%  The format of the GetImageVirtualPixelMethod() method is:
%
%      VirtualPixelMethod GetImageVirtualPixelMethod(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport VirtualPixelMethod GetImageVirtualPixelMethod(const Image *image)
{
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  return(cache_info->virtual_pixel_method);
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
%  GetIndexes() returns the indexes associated with the last call to
%  SetImagePixels() or GetImagePixels().
%
%  The format of the GetIndexes() method is:
%
%      IndexPacket *GetIndexes(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: GetIndexes() returns the indexes associated with the last
%      call to SetImagePixels() or GetImagePixels().
%
%    o image: The image.
%
%
*/
MagickExport IndexPacket *GetIndexes(const Image *image)
{
  CacheInfo
    *cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.get_indexes_from_handler ==
       (GetIndexesFromHandler) NULL)
    return((IndexPacket *) NULL);
  return(cache_info->methods.get_indexes_from_handler(image));
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
%  GetIndexesFromCache() returns the indexes associated with the last call to
%  SetPixelCache() or GetPixelCache().
%
%  The format of the GetIndexesFromCache() method is:
%
%      IndexPacket *GetIndexesFromCache(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: GetIndexesFromCache() returns the indexes associated with the
%      last call to SetPixelCache() or GetPixelCache().
%
%    o image: The image.
%
%
*/
static IndexPacket *GetIndexesFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
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
%  GetNexus() returns an available cache nexus.
%
%  The format of the GetNexus() method is:
%
%      unsigned int GetNexus(Cache cache)
%
%  A description of each parameter follows:
%
%    o id:  GetNexus returns an available cache nexus slot.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport unsigned long GetNexus(Cache cache)
{
  CacheInfo
    *cache_info;

  register long
    id;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  for (id=1; id < (long) MaxCacheViews; id++)
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
%  GetNexusIndexes() returns the indexes associated with the specified cache
%  nexus.
%
%  The format of the GetNexusIndexes() method is:
%
%      IndexPacket *GetNexusIndexes(const Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o indexes: GetNexusIndexes returns the indexes associated with the
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
%  GetNexusPixels() returns the pixels associated with the specified cache
%  nexus.
%
%  The format of the GetNexusPixels() method is:
%
%      PixelPacket *GetNexusPixels(const Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o pixels: GetNexusPixels returns the pixels associated with the specified
%      cache nexus.
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
%      PixelPacket GetOnePixel(const Image image,const long x,const long y)
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
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.get_one_pixel_from_handler ==
      (GetOnePixelFromHandler) NULL)
    return(image->background_color);
  return(cache_info->methods.get_one_pixel_from_handler(image,x,y));
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
%  GetOnePixelFromCache() returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixelFromCache() method is:
%
%      PixelPacket GetOnePixelFromCache(const Image image,const long x,
%        const long y)
%
%  A description of each parameter follows:
%
%    o pixels: GetOnePixelFromCache returns a pixel at the specified (x,y)
%      location.
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
%  GetPixels() returns the pixels associated with the last call to
%  SetImagePixels() or GetImagePixels().
%
%  The format of the GetPixels() method is:
%
%      PixelPacket *GetPixels(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: GetPixels() returns the pixels associated with the last call
%      to SetImagePixels() or GetImagePixels().
%
%    o image: The image.
%
%
*/
MagickExport PixelPacket *GetPixels(const Image *image)
{
  CacheInfo
    *cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.get_pixels_from_handler ==
      (GetPixelsFromHandler) NULL)
    return((PixelPacket *) NULL);
  return(cache_info->methods.get_pixels_from_handler(image));
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
+   G e t P i x e l C a c h e A r e a                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPixelCacheArea() returns the area (width * height in pixels) consumed by
%  the current pixel cache.
%
%  The format of the GetPixelCacheArea() method is:
%
%      magick_off_t GetPixelCacheArea(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport magick_off_t GetPixelCacheArea(const Image *image)
{
  CacheInfo
    *cache_info;

  register NexusInfo
    *nexus_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->nexus_info == (NexusInfo *) NULL)
    return((magick_off_t) cache_info->columns*cache_info->rows);
  nexus_info=cache_info->nexus_info+cache_info->id;
  return((magick_off_t) nexus_info->columns*nexus_info->rows);
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
%  GetPixelsFromCache() returns the pixels associated with the last call to
%  the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetPixelsFromCache() method is:
%
%      PixelPacket *GetPixelsFromCache(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: GetPixelsFromCache() returns the pixels associated with the
%      last call to SetPixelCache() or GetPixelCache().
%
%    o image: The image.
%
%
*/
static PixelPacket *GetPixelsFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  return(GetNexusPixels(image->cache,0));
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
    clone_image;

  unsigned int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  /* These calls were removed for performance reasons. The most common case
     for the code was causing a cache semaphore to be allocated and locked
     for EACH AND EVERY scanline being read by a coder. This was despite the
     fact that the reference count on the cache was always 1. Big WOT. */
  /* AcquireSemaphoreInfo(&cache_info->semaphore); */
  if (cache_info->reference_count <= 1)
    {
      /* LiberateSemaphoreInfo(&cache_info->semaphore); */
      return(True);
    }
  /* we don't actually lock until we know for sure that someone else is using
     the same cache. */
  AcquireSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  if (!cache_info->nexus_info)
    OpenCache(image,IOMode);
  cache_info->reference_count--;
  clone_image=(*image);
  GetCacheInfo(&image->cache);
  AcquireImagePixels(&clone_image,0,0,image->columns,1,&image->exception);
  status=OpenCache(image,IOMode);
  if (status != False)
    {
      NexusInfo
        nexus_info;

      /*
        More than one reference, clone the pixel cache.
      */
      nexus_info=(*cache_info->nexus_info);
      status=ClonePixelCache(&clone_image,image);
      if ((nexus_info.columns != 0) && (nexus_info.rows != 0))
        (void) AcquireImagePixels(&clone_image,nexus_info.x,nexus_info.y,
          nexus_info.columns,nexus_info.rows,&image->exception);
    }
  LiberateSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  return(status);
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
%  OpenCache() allocates the pixel cache.  This includes defining the cache
%  dimensions, allocating space for the image pixels and optionally the
%  colormap indexes, and memory mapping the cache if it is disk based.  The
%  cache nexus array is initialized as well.
%
%  The format of the OpenCache() method is:
%
%      unsigned int OpenCache(Image *image,const MapMode mode)
%
%  A description of each parameter follows:
%
%    o status: OpenCache() returns True if the pixel cache is initialized
%      successfully otherwise False.
%
%    o image: The image.
%
%    o mode: ReadMode, WriteMode, or IOMode.
%
%
*/

static unsigned int ExtendCache(int file,magick_off_t length)
{
  ssize_t
    count;

  magick_off_t
    offset;

  offset=MagickSeek(file,0,SEEK_END);
  if (offset < 0)
    return(False);
  if (offset >= length)
    return(True);
  offset=MagickSeek(file,length-1,SEEK_SET);
  if (offset < 0)
    return(False);
  count=write(file,(void *) "",1);
  if (count != 1)
    return(False);
  return(True);
}

#if defined(SIGBUS)

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static void CacheSignalHandler(int status)
{
  MagickFatalError3(CacheFatalError,UnableToExtendPixelCache,
    DiskAllocationFailed);
  DestroyMagick();
  Exit(status);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* defined(SIGBUS) */

MagickExport unsigned int OpenCache(Image *image,const MapMode mode)
{
  char
    format[MaxTextExtent];

  CacheInfo
    *cache_info;

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  int
    file;

  PixelPacket
    *pixels;

  size_t
    packet_size;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (void *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowBinaryException(ResourceLimitError,NoPixelsDefinedInCache,
                         image->filename);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  FormatString(cache_info->filename,"%.1024s[%ld]",image->filename,
               GetImageIndexInList(image));
  cache_info->rows=image->rows;
  cache_info->columns=image->columns;
  number_pixels=(magick_uint64_t) cache_info->columns*cache_info->rows;
  if (cache_info->nexus_info == (NexusInfo *) NULL)
    {
      register long
        id;

      /*
        Allocate cache nexus.
      */
      cache_info->nexus_info=MagickAllocateMemory(NexusInfo *,
                                                  MaxCacheViews*sizeof(NexusInfo));
      if (cache_info->nexus_info == (NexusInfo *) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                          UnableToAllocateCacheInfo);
      (void) memset(cache_info->nexus_info,0,MaxCacheViews*sizeof(NexusInfo));
      for (id=1; id < (long) MaxCacheViews; id++)
        cache_info->nexus_info[id].available=True;
    }
  if (cache_info->storage_class != UndefinedClass)
    {
      /*
        Free cache resources.
      */
      switch (cache_info->type)
        {
        case MemoryCache:
          {
            LiberateMagickResource(MemoryResource,cache_info->length);
            break;
          }
        case MapCache:
          {
            (void) UnmapBlob(cache_info->pixels,(size_t) cache_info->length);
            LiberateMagickResource(MapResource,cache_info->length);
            break;
          }
        case DiskCache:
          {
            LiberateMagickResource(DiskResource,cache_info->length);
            if (cache_info->file == -1)
              break;
            close(cache_info->file);
            cache_info->file=(-1);
            LiberateMagickResource(FileResource,1);
            break;
          }
        default:
          break;
        }
    }
  packet_size=sizeof(PixelPacket);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    packet_size+=sizeof(IndexPacket);
  offset=number_pixels*packet_size;
  if (cache_info->columns != (offset/cache_info->rows/packet_size))
    ThrowBinaryException(ResourceLimitError,PixelCacheAllocationFailed,
                         image->filename);
  cache_info->length=offset;
  /*
    Assure that there is sufficient address space available to contain
    the PseudoClass representation (PixelPacket array + colormap
    indexes array), and assure that there are sufficient memory
    resources remaining. Pre-allocate sufficient memory to store the
    PseudoClass representation in order to reduce the chance of
    running out of virtual memory while processing the image. If the
    operating system does not reserve heap allocations then it is
    still possible to run out of virtual memory (the process will be
    killed) even after the memory has been allocated.
  */
  offset=number_pixels*(sizeof(PixelPacket)+sizeof(IndexPacket));
  if ((offset == (magick_off_t) ((size_t) offset)) &&
      ((cache_info->type == UndefinedCache) ||
       (cache_info->type == MemoryCache)) &&
      (AcquireMagickResource(MemoryResource,offset)))
    {
      MagickReallocMemory(cache_info->pixels,(size_t) offset);
      pixels=cache_info->pixels;
      if (pixels == (PixelPacket *) NULL)
        LiberateMagickResource(MemoryResource,offset);
      else
        {
          /*
            Create in-memory pixel cache.
          */
          cache_info->length=offset;
          cache_info->storage_class=image->storage_class;
          cache_info->colorspace=image->colorspace;
          cache_info->type=MemoryCache;
          cache_info->pixels=pixels;
          cache_info->indexes=(IndexPacket *) NULL;
          if ((cache_info->storage_class == PseudoClass) ||
              (cache_info->colorspace == CMYKColorspace))
            cache_info->indexes=(IndexPacket *) (pixels+number_pixels);
          FormatSize(cache_info->length,format);
          (void) LogMagickEvent(CacheEvent,GetMagickModule(),
                                "open %.1024s (%.1024s)",cache_info->filename,
                                format);
          return(True);
        }
    }
  /*
    Create pixel cache on disk.
  */
  if (!AcquireMagickResource(DiskResource,cache_info->length))
    ThrowBinaryException(ResourceLimitError,CacheResourcesExhausted,
                         image->filename);
  if (*cache_info->cache_filename == '\0')
    if(!AcquireTemporaryFileName(cache_info->cache_filename))
      {
        LiberateMagickResource(DiskResource,cache_info->length);
        ThrowBinaryException(FileOpenError,UnableToCreateTemporaryFile,
                             cache_info->cache_filename)
          }
  switch (mode)
    {
    case ReadMode:
      {
        file=open(cache_info->cache_filename,O_RDONLY | O_BINARY | _O_SEQUENTIAL);
        break;
      }
    case WriteMode:
      {
        file=open(cache_info->cache_filename,O_WRONLY | O_CREAT | O_BINARY |
                  O_EXCL | _O_SEQUENTIAL,S_MODE);
        if (file == -1)
          file=open(cache_info->cache_filename,O_WRONLY | O_BINARY,S_MODE);
        break;
      }
    case IOMode:
    default:
      {
        file=open(cache_info->cache_filename,O_RDWR | O_CREAT | O_BINARY |
                  O_EXCL | _O_SEQUENTIAL, S_MODE);
        if (file == -1)
          file=open(cache_info->cache_filename,O_RDWR | O_BINARY | _O_RANDOM,
                    S_MODE);
        break;
      }
    }
  if (file == -1)
    {
      LiberateMagickResource(DiskResource,cache_info->length);
      ThrowBinaryException(CacheError,UnableToOpenCache,image->filename)
        }
  if (!ExtendCache(file,cache_info->offset+cache_info->length))
    {
      close(file);
      LiberateTemporaryFile(cache_info->cache_filename);
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),
                            "remove %.1024s (%.1024s)",cache_info->filename,
                            cache_info->cache_filename);
      LiberateMagickResource(DiskResource,cache_info->length);
      ThrowBinaryException(CacheError,UnableToExtendCache,image->filename)
    }
  cache_info->storage_class=image->storage_class;
  cache_info->colorspace=image->colorspace;
  cache_info->type=DiskCache;
  if ((cache_info->length > MinBlobExtent) &&
      (cache_info->length == (size_t) cache_info->length) &&
      AcquireMagickResource(MapResource,cache_info->length))
    {
      pixels=(PixelPacket *) MapBlob(file,mode,(off_t) cache_info->offset,
                                     (size_t) cache_info->length);
      if (pixels == (PixelPacket *) NULL)
        LiberateMagickResource(MapResource,cache_info->length);
      else
        {
          /*
            Create memory-mapped pixel cache.
          */
          (void) close(file);
          cache_info->type=MapCache;
          cache_info->pixels=pixels;
          cache_info->indexes=(IndexPacket *) NULL;
          if ((cache_info->storage_class == PseudoClass) ||
              (cache_info->colorspace == CMYKColorspace))
            cache_info->indexes=(IndexPacket *) (pixels+number_pixels);
        }
    }
  if (cache_info->type == DiskCache)
    {
      if (AcquireMagickResource(FileResource,1))
        cache_info->file=file;
      else
        (void) close(file);
    }
#if defined(SIGBUS)
  (void) signal(SIGBUS,CacheSignalHandler);
#endif
  FormatSize(cache_info->length,format);
  (void) LogMagickEvent(CacheEvent,GetMagickModule(),
                        "open %.1024s (%.1024s[%d], %.1024s, %.1024s)",
                        cache_info->filename,cache_info->cache_filename,
                        cache_info->file,
                        cache_info->type == MapCache ? "memory-mapped" : "disk",
                        format);
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
%        const unsigned int attach,magick_off_t *offset,
%        ExceptionInfo *exception)
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
%    o attach: A value other than zero initializes the persistent pixel
%      cache.
%
%    o offset: The offset in the persistent cache to store pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int PersistCache(Image *image,const char *filename,
  const unsigned int attach,magick_off_t *offset,
  ExceptionInfo *exception)
{
  CacheInfo
    *cache_info;

  Image
    *clone_image;

  IndexPacket
    *clone_indexes,
    *indexes;

  long
    pagesize,
    y;

  register const PixelPacket
    *p;

  register PixelPacket
    *q;

  unsigned int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (void *) NULL);
  assert(filename != (const char *) NULL);
  assert(offset != (magick_off_t *) NULL);
  pagesize=0;
#if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
  pagesize=sysconf(_SC_PAGE_SIZE);
#elif defined(HAVE_GETPAGESIZE) && defined(POSIX)
  pagesize=getpagesize();
#endif
  if (pagesize <= 0)
    pagesize=16384;
  cache_info=(CacheInfo *) image->cache;
  if (attach)
    {
      /*
        Attach persistent pixel cache.
      */
      (void) strncpy(cache_info->cache_filename,filename,MaxTextExtent-1);
      cache_info->type=DiskCache;
      cache_info->offset=(*offset);
      if (!OpenCache(image,ReadMode))
        return(False);
      cache_info=ReferenceCache(cache_info);
      *offset+=cache_info->length+pagesize-(cache_info->length % pagesize);
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),
        "Attach persistent cache");
      return(True);
    }
  AcquireSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  if ((cache_info->reference_count == 1) &&
      (cache_info->type != MemoryCache))
    {
      /*
        Usurp resident persistent pixel cache.
      */
      status=rename(cache_info->cache_filename,filename);
      if (status == 0)
        {
          (void) strncpy(cache_info->cache_filename,filename,MaxTextExtent-1);
          LiberateSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
          cache_info=ReferenceCache(cache_info);
          *offset+=cache_info->length+pagesize-(cache_info->length % pagesize);
          (void) LogMagickEvent(CacheEvent,GetMagickModule(),
            "Usurp resident persistent cache");
          return(True);
        }
    }
  LiberateSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  /*
    Clone persistent pixel cache.
  */
  clone_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (clone_image == (Image *) NULL)
    return(False);
  cache_info=(CacheInfo *) clone_image->cache;
  (void) strncpy(cache_info->cache_filename,filename,MaxTextExtent-1);
  cache_info->type=DiskCache;
  cache_info->offset=(*offset);
  if (!OpenCache(clone_image,IOMode))
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
  cache_info=ReferenceCache(cache_info);
  DestroyImage(clone_image);
  if (y < (long) image->rows)
    return(False);
  *offset+=cache_info->length+pagesize-(cache_info->length % pagesize);
  (void) LogMagickEvent(CacheEvent,GetMagickModule(),"Clone persistent cache");
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
%  ReadCacheIndexes() reads colormap indexes from the specified region of the
%  pixel cache.
%
%  The format of the ReadCacheIndexes() method is:
%
%      unsigned int ReadCacheIndexes(const Cache cache,
%        const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: ReadCacheIndexes returns True if the colormap indexes are
%      successfully read from the pixel cache, otherwise False.
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

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  int
    file;

  register IndexPacket
    *indexes;

  register long
    y;

  register NexusInfo
    *nexus_info;

  size_t
    length;

  unsigned long
    rows;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if ((cache_info->storage_class != PseudoClass) &&
      (cache_info->colorspace != CMYKColorspace))
    return(False);
  nexus_info=cache_info->nexus_info+nexus;
  if (IsNexusInCore(cache,nexus))
    return(True);
  offset=nexus_info->y*(magick_off_t) cache_info->columns+nexus_info->x;
  length=nexus_info->columns*sizeof(IndexPacket);
  rows=nexus_info->rows;
  number_pixels=(magick_uint64_t) length*rows;
  if ((cache_info->columns == nexus_info->columns) &&
      (number_pixels == (size_t) number_pixels))
    {
      length=number_pixels;
      rows=1;
    }
  indexes=nexus_info->indexes;
  if (cache_info->type != DiskCache)
    {
      /*
        Read indexes from memory.
      */
      for (y=0; y < (long) rows; y++)
      {
        (void) memcpy(indexes,cache_info->indexes+offset,length);
        indexes+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read indexes from disk.
  */
  file=cache_info->file;
  if (cache_info->file == -1)
    {
      file=open(cache_info->cache_filename,O_RDONLY | O_BINARY);
      if (file == -1)
        return(False);
    }
  number_pixels=(magick_uint64_t) cache_info->columns*cache_info->rows;
  for (y=0; y < (long) rows; y++)
  {
    if ((FilePositionRead(file,indexes,length,cache_info->offset+
          number_pixels*sizeof(PixelPacket)+offset*sizeof(IndexPacket))) <= 0)
      break;
    indexes+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  if (cache_info->file == -1)
    (void) close(file);
  if (QuantumTick(nexus_info->y,cache_info->rows))
    (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%lux%lu%+ld%+ld",
      nexus_info->columns,nexus_info->rows,nexus_info->x,nexus_info->y);
  return(y == (long) rows);
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
%  ReadCachePixels() reads pixels from the specified region of the pixel cache.
%
%  The format of the ReadCachePixels() method is:
%
%      unsigned int ReadCachePixels(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: ReadCachePixels() returns True if the pixels are
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

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  int
    file;

  register long
    y;

  register NexusInfo
    *nexus_info;

  register PixelPacket
    *pixels;

  size_t
    length;

  unsigned long
    rows;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  nexus_info=cache_info->nexus_info+nexus;
  if (IsNexusInCore(cache,nexus))
    return(True);
  offset=nexus_info->y*(magick_off_t) cache_info->columns+nexus_info->x;
  length=nexus_info->columns*sizeof(PixelPacket);
  rows=nexus_info->rows;  
  number_pixels=(magick_uint64_t) length*rows;
  if ((cache_info->columns == nexus_info->columns) &&
      (number_pixels == (size_t) number_pixels))
    {
      length=number_pixels;
      rows=1;
    }
  pixels=nexus_info->pixels;
  if (cache_info->type != DiskCache)
    {
      /*
        Read pixels from memory.
      */
      for (y=0; y < (long) rows; y++)
      {
        (void) memcpy(pixels,cache_info->pixels+offset,length);
        pixels+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read pixels from disk.
  */
  file=cache_info->file;
  if (cache_info->file == -1)
    {
      file=open(cache_info->cache_filename,O_RDONLY | O_BINARY);
      if (file == -1)
        return(False);
    }
  for (y=0; y < (long) rows; y++)
  {
    if ((FilePositionRead(file,pixels,length,
       cache_info->offset+offset*sizeof(PixelPacket))) < length)
      break;
    pixels+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  if (cache_info->file == -1)
    (void) close(file);
  if (QuantumTick(nexus_info->y,cache_info->rows))
    (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%lux%lu%+ld%+ld",
      nexus_info->columns,nexus_info->rows,nexus_info->x,nexus_info->y);
  return(y == (long) rows);
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
  AcquireSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  cache_info->reference_count++;
  LiberateSemaphoreInfo((SemaphoreInfo **) &cache_info->semaphore);
  return(cache_info);
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
%  SetCacheNexus() allocates an area to store image pixels as defined by the
%  region rectangle and returns a pointer to the area.  This area is
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
%    o pixels: SetCacheNexus() returns a pointer to the pixels if they are
%      transferred, otherwise a NULL is returned.
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

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  RectangleInfo
    region;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  if (ModifyCache(image) == False)
    return((PixelPacket *) NULL);
  if (SyncCache(image) == False)
    return((PixelPacket *) NULL);
  /*
    Validate pixel cache geometry.
  */
  cache_info=(CacheInfo *) image->cache;
  offset=y*(magick_off_t) cache_info->columns+x;
  if (offset < 0)
    return((PixelPacket *) NULL);
  number_pixels=(magick_uint64_t) cache_info->columns*cache_info->rows;
  offset+=(rows-1)*(magick_off_t) cache_info->columns+columns-1;
  if (offset >= number_pixels)
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
%   S e t I m a g e P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImagePixels() allocates an area to store image pixels as defined by the
%  region rectangle and returns a pointer to the area.  This area is
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
%    o pixels: SetImagePixels returns a pointer to the pixels if they are
%      transferred, otherwise a NULL is returned.
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
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.set_pixel_handler == (SetPixelHandler) NULL)
    return((PixelPacket *) NULL);
  return(cache_info->methods.set_pixel_handler(image,x,y,columns,rows));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e V i r t u a l P i x e l M e t h o d                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageVirtualPixelMethod() sets the "virtual pixels" method for the
%  image.  A virtual pixel is any pixel access that is outside the boundaries
%  of the image cache.
%
%  The format of the SetImageVirtualPixelMethod() method is:
%
%      SetImageVirtualPixelMethod(const Image *image,
%        const VirtualPixelMethod method)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o type: choose from these access types:
%
%        EdgeVPType:  the edge pixels of the image extend infinitely.
%        Any pixel outside the image is assigned the same value as the
%        pixel at the edge closest to it.
%
%        TileVPType:  the image extends periodically or tiled.  The pixels
%        wrap around the edges of the image.
%
%        MirrorVPType:  mirror the image at the boundaries.
%
%        ConstantVPType:  every value outside the image is a constant as
%        defines by the pixel parameter.
%
%
*/
MagickExport unsigned int SetImageVirtualPixelMethod(const Image *image,
  const VirtualPixelMethod method)
{
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  cache_info->virtual_pixel_method=method;
  return True;
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
%  SetNexus() defines the region of the cache for the specified cache nexus.
%
%  The format of the SetNexus() method is:
%
%      PixelPacket SetNexus(const Image *image,const RectangleInfo *region,
%        const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o pixels: SetNexus() returns a pointer to the pixels associated with
%      the specified cache nexus.
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

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  register NexusInfo
    *nexus_info;

  size_t
    length;

  assert(image != (Image *) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  cache_info->id=nexus;
  nexus_info=cache_info->nexus_info+nexus;
  nexus_info->columns=region->width;
  nexus_info->rows=region->height;
  nexus_info->x=region->x;
  nexus_info->y=region->y;
  if ((cache_info->type != DiskCache) && (image->clip_mask == (Image *) NULL))
    {
      offset=nexus_info->y*(magick_off_t) cache_info->columns+nexus_info->x;
      length=(nexus_info->rows-1)*cache_info->columns+nexus_info->columns-1;
      number_pixels=(magick_uint64_t) cache_info->columns*cache_info->rows;
      if ((offset >= 0) && ((offset+length) < number_pixels))
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
  number_pixels=(magick_uint64_t)
    Max(nexus_info->columns*nexus_info->rows,cache_info->columns);
  length=number_pixels*sizeof(PixelPacket);
  if ((cache_info->storage_class == PseudoClass) ||
      (cache_info->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  if (nexus_info->staging == (PixelPacket *) NULL)
    {
      nexus_info->staging=MagickAllocateMemory(PixelPacket *,length);
      nexus_info->length=length;
    }
  else
    if (nexus_info->length < length)
      {
        MagickReallocMemory(nexus_info->staging,length);
        nexus_info->length=length;
      }
  if (nexus_info->staging == (PixelPacket *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateCacheInfo);
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
%    o pixels: SetPixelCache() returns a pointer to the pixels if they are
%      transferred, otherwise a NULL is returned.
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
%  SetPixelCacheMethods() sets the image pixel methods to the specified ones.
%
%  The format of the SetPixelCacheMethods() method is:
%
%      SetPixelCacheMethods(Cache *,AcquirePixelHandler acquire_pixel,
%        GetPixelHandler get_pixel,SetPixelHandler set_pixel,
%        SyncPixelHandler sync_pixel,GetPixelsFromHandler get_pixels_from,
%        GetIndexesFromHandler get_indexes_from,
%        AcquireOnePixelFromHandler acquire_one_pixel_from,
%        GetOnePixelFromHandler get_one_pixel_from,
%        ClosePixelHandler close_pixel,DestroyPixelHandler destroy_pixel)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport void SetPixelCacheMethods(Cache cache,
  AcquirePixelHandler acquire_pixel,GetPixelHandler get_pixel,
  SetPixelHandler set_pixel,SyncPixelHandler sync_pixel,
  GetPixelsFromHandler get_pixels_from,GetIndexesFromHandler get_indexes_from,
  AcquireOnePixelFromHandler acquire_one_pixel_from,
  GetOnePixelFromHandler get_one_pixel_from,DestroyPixelHandler destroy_pixel)
{
  CacheInfo
    *cache_info;

  /*
    Set image pixel methods.
  */
  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  assert(acquire_pixel != (AcquirePixelHandler) NULL);
  assert(get_pixel != (GetPixelHandler) NULL);
  assert(set_pixel != (SetPixelHandler) NULL);
  assert(sync_pixel != (SyncPixelHandler) NULL);
  assert(get_pixels_from != (GetPixelsFromHandler) NULL);
  assert(get_indexes_from != (GetIndexesFromHandler) NULL);
  assert(acquire_one_pixel_from != (AcquireOnePixelFromHandler) NULL);
  assert(get_one_pixel_from != (GetOnePixelFromHandler) NULL);
  assert(destroy_pixel != (DestroyPixelHandler) NULL);
  cache_info->methods.acquire_pixel_handler=acquire_pixel;
  cache_info->methods.get_pixel_handler=get_pixel;
  cache_info->methods.set_pixel_handler=set_pixel;
  cache_info->methods.sync_pixel_handler=sync_pixel;
  cache_info->methods.get_pixels_from_handler=get_pixels_from;
  cache_info->methods.get_indexes_from_handler=get_indexes_from;
  cache_info->methods.acquire_one_pixel_from_handler=acquire_one_pixel_from;
  cache_info->methods.get_one_pixel_from_handler=get_one_pixel_from;
  cache_info->methods.destroy_pixel_handler=destroy_pixel;
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
%  SyncCache() synchronizes the image with the pixel cache.
%
%  The format of the SyncCache() method is:
%
%      unsigned int SyncCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: SyncCache() returns True if the pixel cache is synchronized
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
    if (!OpenCache(image,IOMode))
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
%  SyncCacheNexus() saves the image pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncCacheNexus() method is:
%
%      unsigned int SyncCacheNexus(Image *image,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: SyncCacheNexus() returns True if the image pixels are
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
    ThrowBinaryException(CacheError,PixelCacheIsNotOpen,image->filename);
  image->taint=True;
  image->is_grayscale=False;
  image->is_monochrome=False;
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
    ThrowBinaryException(CacheError,UnableToSyncCache,image->filename);
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
%  SyncImagePixels() saves the image pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncImagePixels() method is:
%
%      unsigned int SyncImagePixels(Image *image)
%
%  A description of each parameter follows:
%
%    o status: SyncImagePixels() returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int SyncImagePixels(Image *image)
{
  CacheInfo
    *cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->methods.sync_pixel_handler == (SyncPixelHandler) NULL)
    return(False);
  return(cache_info->methods.sync_pixel_handler(image));
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
%  WriteCacheIndexes() writes the colormap indexes to the specified region of
%  the pixel cache.
%
%  The format of the WriteCacheIndexes() method is:
%
%      unsigned int WriteCacheIndexes(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: WriteCacheIndexes() returns True if the indexes are
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

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  int
    file;

  register IndexPacket
    *indexes;

  register long
    y;

  register NexusInfo
    *nexus_info;

  size_t
    length;

  unsigned long
    rows;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if ((cache_info->storage_class != PseudoClass) &&
      (cache_info->colorspace != CMYKColorspace))
    return(False);
  nexus_info=cache_info->nexus_info+nexus;
  if (IsNexusInCore(cache,nexus))
    return(True);
  offset=nexus_info->y*(magick_off_t) cache_info->columns+nexus_info->x;
  length=nexus_info->columns*sizeof(IndexPacket);
  rows=nexus_info->rows;  
  number_pixels=(magick_uint64_t) length*rows;
  if ((cache_info->columns == nexus_info->columns) &&
      (number_pixels == (size_t) number_pixels))
    {
      length=number_pixels;
      rows=1;
    }
  indexes=nexus_info->indexes;
  if (cache_info->type != DiskCache)
    {
      /*
        Write indexes to memory.
      */
      for (y=0; y < (long) rows; y++)
      {
        (void) memcpy(cache_info->indexes+offset,indexes,length);
        indexes+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write indexes to disk.
  */
  file=cache_info->file;
  if (cache_info->file == -1)
    {
      file=open(cache_info->cache_filename,O_WRONLY | O_BINARY | O_EXCL,S_MODE);
      if (file == -1)
        file=open(cache_info->cache_filename,O_WRONLY | O_BINARY,S_MODE);
      if (file == -1)
        return(False);
    }
  number_pixels=(magick_uint64_t) cache_info->columns*cache_info->rows;
  for (y=0; y < (long) rows; y++)
  {
    if ((FilePositionWrite(file,indexes,length,cache_info->offset+
       number_pixels*sizeof(PixelPacket)+offset*sizeof(IndexPacket))) < length)
       break;
    indexes+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  if (cache_info->file == -1)
    (void) close(file);
  if (QuantumTick(nexus_info->y,cache_info->rows))
    (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%lux%lu%+ld%+ld",
      nexus_info->columns,nexus_info->rows,nexus_info->x,nexus_info->y);
  return(y == (long) rows);
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
%  WriteCachePixels() writes image pixels to the specified region of the pixel
%  cache.
%
%  The format of the WriteCachePixels() method is:
%
%      unsigned int WriteCachePixels(Cache cache,const unsigned long nexus)
%
%  A description of each parameter follows:
%
%    o status: WriteCachePixels() returns True if the pixels are
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

  magick_uint64_t
    number_pixels;

  magick_off_t
    offset;

  int
    file;

  register long
    y;

  register NexusInfo
    *nexus_info;

  register PixelPacket
    *pixels;

  size_t
    length;

  unsigned long
    rows;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  nexus_info=cache_info->nexus_info+nexus;
  if (IsNexusInCore(cache,nexus))
    return(True);
  offset=nexus_info->y*(magick_off_t) cache_info->columns+nexus_info->x;
  length=nexus_info->columns*sizeof(PixelPacket);
  rows=nexus_info->rows;  
  number_pixels=(magick_uint64_t) length*rows;
  if ((cache_info->columns == nexus_info->columns) &&
      (number_pixels == (size_t) number_pixels))
    {
      length=number_pixels;
      rows=1;
    }
  pixels=nexus_info->pixels;
  if (cache_info->type != DiskCache)
    {
      /*
        Write pixels to memory.
      */
      for (y=0; y < (long) rows; y++)
      {
        (void) memcpy(cache_info->pixels+offset,pixels,length);
        pixels+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write pixels to disk.
  */
  file=cache_info->file;
  if (cache_info->file == -1)
    {
      file=open(cache_info->cache_filename,O_WRONLY | O_BINARY | O_EXCL,S_MODE);
      if (file == -1)
        file=open(cache_info->cache_filename,O_WRONLY | O_BINARY,S_MODE);
      if (file == -1)
        return(False);
    }
  for (y=0; y < (long) rows; y++)
  {
    if ((FilePositionWrite(file,pixels,length,
        cache_info->offset+offset*sizeof(PixelPacket))) < length)
       break;
    pixels+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  if (cache_info->file == -1)
    (void) close(file);
  if (QuantumTick(nexus_info->y,cache_info->rows))
    (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%lux%lu%+ld%+ld",
      nexus_info->columns,nexus_info->rows,nexus_info->x,nexus_info->y);
  return(y == (long) rows);
}
