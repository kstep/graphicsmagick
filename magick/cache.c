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
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
  cache_threshold = ~0,
  free_memory = 0;

static SemaphoreInfo
  *cache_semaphore = (SemaphoreInfo *) NULL;

/*
  Declare pixel cache interfaces.
*/
static IndexPacket
  *GetIndexesFromCache(const Image *);

static off_t
  GetCacheMemory(const off_t);

static PixelPacket
  GetOnePixelFromCache(Image *,const int,const int),
  *SetNexus(Image *,const RectangleInfo *,const unsigned int),
  *GetPixelsFromCache(const Image *),
  *GetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *SetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int);

static unsigned int
  CompressCache(Cache),
  IsNexusInCore(const Cache,const unsigned int),
  ReadCacheIndexes(Cache,const unsigned int),
  ReadCachePixels(Cache,const unsigned int),
  SyncPixelCache(Image *),
  UncompressCache(Cache),
  WriteCacheInfo(Image *),
  WriteCacheIndexes(Cache,const unsigned int),
  WriteCachePixels(Cache,const unsigned int);

static void
  ClosePixelCache(Image *),
  DestroyPixelCache(Image *);

/*
  Preload the image pixel methods.
*/
static ClosePixelHandler
  close_pixel_handler = ClosePixelCache;

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
+   C l o s e C a c h e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloseCache() closes the file handle associated with a disk pixel
%  cache.
%
%  The format of the CloseCache() method is:
%
%      void CloseCache(Cache cache)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
static void CloseCache(Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (cache_info->file != -1)
    (void) close(cache_info->file);
  cache_info->file=(-1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e I m a g e P i x e l s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloseImagePixels() closes the pixel cache.  Use this method to prevent
%  too many file descriptors from being allocated when reading an image
%  sequence.  File descriptors are only used for a disk-based cache.  This is
%  essentially a no-op for a memory-based cache.
%
%  The format of the CloseImagePixels() method is:
%
%      void CloseImagePixels(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void CloseImagePixels(Image *image)
{
  (close_pixel_handler)(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClosePixelCache() closes the pixel cache.  Use this method to prevent
%  too many file descriptors from being allocated when reading an image
%  sequence.  File descriptors are only used for a disk-based cache.  This is
%  essentially a no-op for a memory-based cache.
%
%  The format of the ClosePixelCache() method is:
%
%      void ClosePixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static void ClosePixelCache(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->cache == (void *) NULL)
    return;
  (void) CompressCache(image->cache);
  CloseCache(image->cache);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o m p r e s s C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CompressCache() compresses the disk-based pixel cache.
%
%  The format of the CompressCache() method is:
%
%      unsigned int CompressCache(Cache cache)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
static unsigned int CompressCache(Cache cache)
{
#if defined(HasZLIB)
  CacheInfo
    *cache_info;

  char
    filename[MaxTextExtent],
    *pixels;

  gzFile
    file;

  int
    y;

  long
    count;

  size_t
    length;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->type != DiskCache)
    return(True);
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  if (lseek(cache_info->file,0,SEEK_SET) == -1)
    return(False);
  FormatString(filename,"%s.gz",cache_info->cache_filename);
  file=gzopen(filename,WriteBinaryType);
  if (file == (gzFile) NULL)
    return(False);
  length=cache_info->columns*Max(sizeof(IndexPacket),sizeof(PixelPacket));
  pixels=(char *) AcquireMemory(length);
  if (pixels == (char *) NULL)
    {
      (void) gzclose(file);
      return(False);
    }
  for (y=0; y < (int) cache_info->rows; y++)
  {
    count=read(cache_info->file,pixels,cache_info->columns*sizeof(PixelPacket));
    if ((off_t) gzwrite(file,pixels,count) != count)
      break;
  }
  if (y == (int) cache_info->rows)
    if ((cache_info->storage_class == PseudoClass) ||
        (cache_info->colorspace == CMYKColorspace))
      for (y=0; y < (int) cache_info->rows; y++)
      {
        count=read(cache_info->file,pixels,
          cache_info->columns*sizeof(IndexPacket));
        if ((off_t) gzwrite(file,pixels,count) != count)
          break;
      }
  LiberateMemory((void **) &pixels);
  (void) gzclose(file);
  CloseCache(cache);
  if (y != (int) cache_info->rows)
    (void) remove(filename);
  else
    (void) remove(cache_info->cache_filename);
  return(y == (int) cache_info->rows);
#else
  return(True);
#endif
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
  DestroySemaphoreInfo(cache_semaphore);
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
%  Method DestroyCacheInfo() deallocates memory associated with the pixel cache.
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

  off_t
    length,
    number_pixels;

  register int
    id;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  number_pixels=cache_info->columns*cache_info->rows;
  length=number_pixels*sizeof(PixelPacket);
  if ((cache_info->storage_class == PseudoClass) ||
      (cache_info->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  switch (cache_info->type)
  {
    case MemoryCache:
    {
      LiberateMemory((void **) &cache_info->pixels);
      (void) GetCacheMemory(length);
      break;
    }
    case MemoryMappedCache:
      (void) UnmapBlob(cache_info->pixels,length);
    case DiskCache:
    {
      CloseCache(cache);
      if (cache_info->persist)
        break;
      (void) remove(cache_info->cache_filename);
      (void) strcat(cache_info->cache_filename,".gz");
      (void) remove(cache_info->cache_filename);
      break;
    }
    default:
      break;
  }
  if (cache_info->type != UndefinedCache)
    {
      for (id=0; id < (int) (cache_info->rows+3); id++)
        DestroyCacheNexus(cache,id);
      LiberateMemory((void **) &cache_info->nexus_info);
    }
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
%      void DestroyCacheNexus(Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o id: specifies which cache nexus to destroy.
%
%
*/
MagickExport void DestroyCacheNexus(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  register NexusInfo
    *nexus_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  nexus_info=cache_info->nexus_info+id;
  if (nexus_info->staging != (PixelPacket *) NULL)
    LiberateMemory((void **) &nexus_info->staging);
  memset(nexus_info,0,sizeof(NexusInfo));
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
  if (cache_info->persist)
    WriteCacheInfo(image);
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
  memset(cache_info,0,sizeof(CacheInfo));
  cache_info->colorspace=RGBColorspace;
  cache_info->file=(-1);
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
%  Method GetCacheMemory() adjusts the amount of free cache memory and then
%  returns the resulting value.
%
%  The format of the GetCacheMemory() method is:
%
%      off_t GetCacheMemory(const off_t memory)
%
%  A description of each parameter follows:
%
%    o memory: Specifies the adjustment to the cache memory.  Use 0 to
%      return the current free memory in the cache.
%
%
*/
static off_t GetCacheMemory(const off_t memory)
{
  AcquireSemaphoreInfo(&cache_semaphore);
  free_memory+=memory;
  LiberateSemaphoreInfo(&cache_semaphore);
  return(free_memory);
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
%      PixelPacket *GetCacheNexus(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
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
%
*/
MagickExport PixelPacket *GetCacheNexus(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows,const unsigned int id)
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
  pixels=SetCacheNexus(image,x,y,columns,rows,id);
  if (pixels == (PixelPacket *) NULL)
    return((PixelPacket *) NULL);
  if (IsNexusInCore(image->cache,id))
    return(pixels);
  status=ReadCachePixels(image->cache,id);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    status|=ReadCacheIndexes(image->cache,id);
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
%      PixelPacket *GetImagePixels(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
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
MagickExport PixelPacket *GetImagePixels(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
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
MagickExport unsigned int GetNexus(Cache cache)
{
  CacheInfo
    *cache_info;

  register unsigned int
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
%      IndexPacket *GetNexusIndexes(const Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetNexusIndexes returns the indexes associated with the
%      specified cache nexus.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o id: specifies which cache nexus to return the colormap indexes.
%
%
*/
MagickExport IndexPacket *GetNexusIndexes(const Cache cache,
  const unsigned int id)
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
  nexus_info=cache_info->nexus_info+id;
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
%      PixelPacket *GetNexusPixels(const Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetNexusPixels returns the pixels associated with the
%      specified cache nexus.
%
%    o id: specifies which cache nexus to return the pixels.
%
%
*/
MagickExport PixelPacket *GetNexusPixels(const Cache cache,
  const unsigned int id)
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
  nexus_info=cache_info->nexus_info+id;
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
%  Method GetOnePixel() returns a single pixel at the specified (x,y) location.
%  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixel() method is:
%
%      PixelPacket *GetOnePixel(const Image image,const int x,const int y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixel() returns a pixel at the specified (x,y)
%      location.
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
MagickExport PixelPacket GetOnePixel(Image *image,const int x,const int y)
{
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
%      PixelPacket *GetOnePixelFromCache(const Image image,const int x,
%        const int y)
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
static PixelPacket GetOnePixelFromCache(Image *image,const int x,const int y)
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
%  Method GetPixelCache() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetPixelCache() method is:
%
%      PixelPacket *GetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelCache() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *GetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
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
%      unsigned int IsNexusInCore(const Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o status: Method IsNexusInCore() returns True if the pixels are
%      non-strided and in core, otherwise False.
%
%    o id: specifies which cache nexus to return the pixels.
%
%
*/
static unsigned int IsNexusInCore(const Cache cache,const unsigned int id)
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
  nexus_info=cache_info->nexus_info+id;
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

  char
    null = 0;

  off_t
    length,
    number_pixels;

  PixelPacket
    *pixels;

  size_t
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (void *) NULL);
  if (cache_threshold == ~0)
    {
      size_t
        threshold;

      /*
        Set cache memory threshold.
      */
      threshold=PixelCacheThreshold;
#if defined(_SC_PAGESIZE) && defined(_SC_PHYS_PAGES)
      threshold=(size_t) (4.0*sysconf(_SC_PAGESIZE)*
        sysconf(_SC_PHYS_PAGES)/1048576);
#endif
      if (getenv("MAGICK_CACHE_THRESHOLD") != (char *) NULL)
        threshold=atoi(getenv("MAGICK_CACHE_THRESHOLD"));
      SetCacheThreshold(threshold);
    }
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  number_pixels=cache_info->columns*cache_info->rows;
  length=number_pixels*sizeof(PixelPacket);
  if ((cache_info->storage_class == PseudoClass) ||
      (cache_info->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  if (cache_info->storage_class != UndefinedClass)
    {
      /*
        Free memory-based cache resources.
      */
      if (cache_info->type == MemoryCache)
        (void) GetCacheMemory(length);
      if (cache_info->type == MemoryMappedCache)
        (void) UnmapBlob(cache_info->pixels,length);
    }
  cache_info->rows=image->rows;
  cache_info->columns=image->columns;
  number_pixels=cache_info->columns*cache_info->rows;
  if (cache_info->nexus_info == (NexusInfo *) NULL)
    {
      register int
        i;

      /*
        Allocate cache nexus.
      */
      cache_info->nexus_info=(NexusInfo *)
        AcquireMemory((cache_info->rows+3)*sizeof(NexusInfo));
      if (cache_info->nexus_info == (NexusInfo *) NULL)
        MagickError(ResourceLimitError,"Memory allocation failed",
          "unable to allocate cache nexus_info");
      memset(cache_info->nexus_info,0,(cache_info->rows+3)*sizeof(NexusInfo));
      for (i=1; i < (int) (cache_info->rows+3); i++)
        cache_info->nexus_info[i].available=True;
    }
  length=number_pixels*sizeof(PixelPacket);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  offset=length;
  if (length != offset)
    cache_info->type=DiskCache;
  if ((cache_info->type == MemoryCache) ||
      ((cache_info->type == UndefinedCache) && (length <= GetCacheMemory(0))))
    {
      if (cache_info->storage_class == UndefinedClass)
        pixels=(PixelPacket *) AcquireMemory(length);
      else
        {
          ReacquireMemory((void **) &cache_info->pixels,length);
          if (cache_info->pixels == (void *) NULL)
            return(False);
          pixels=cache_info->pixels;
        }
      if (pixels != (PixelPacket *) NULL)
        {
          /*
            Create in-memory pixel cache.
          */
          (void) GetCacheMemory(-length);
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
  if (cache_info->storage_class == UndefinedClass)
    TemporaryFilename(cache_info->cache_filename);
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        (void) UncompressCache(image->cache);
      if (cache_info->file == -1)
        cache_info->file=open(cache_info->cache_filename,O_RDWR | O_CREAT |
          O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  if (ftruncate(cache_info->file,length) == -1)
    return(False);
else
  if (lseek(cache_info->file,length,SEEK_SET) == -1)
    return(False);
  if (write(cache_info->file,&null,sizeof(null)) == -1)
    return(False);
#endif
  cache_info->storage_class=image->storage_class;
  cache_info->colorspace=image->colorspace;
  cache_info->type=DiskCache;
  if (cache_threshold != 0)
    {
      /*
        Create memory-mapped pixel cache.
      */
      pixels=(PixelPacket *) MapBlob(cache_info->file,IOMode,&offset);
      if (pixels != (PixelPacket *) NULL)
        {
          cache_info->type=MemoryMappedCache;
          cache_info->pixels=pixels;
          cache_info->indexes=(IndexPacket *) NULL;
          if ((cache_info->storage_class == PseudoClass) ||
              (cache_info->colorspace == CMYKColorspace))
            cache_info->indexes=(IndexPacket *) (pixels+number_pixels);
          CloseCache(image->cache);
        }
    }
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
%      unsigned int ReadCacheIndexes(Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o status: Method ReadCacheIndexes returns True if the colormap indexes
%      are successfully read from the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o id: specifies which cache nexus to read the colormap indexes.
%
%
*/
static unsigned int ReadCacheIndexes(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    number_pixels,
    offset;

  register IndexPacket
    *indexes;

  register int
    y;

  register NexusInfo
    *nexus_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if ((cache_info->storage_class != PseudoClass) &&
      (cache_info->colorspace != CMYKColorspace))
    return(False);
  if (IsNexusInCore(cache,id))
    return(True);
  nexus_info=cache_info->nexus_info+id;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  indexes=nexus_info->indexes;
  if (cache_info->type != DiskCache)
    {
      /*
        Read indexes from memory.
      */
      for (y=0; y < (int) nexus_info->rows; y++)
      {
        memcpy(indexes,cache_info->indexes+offset,
          nexus_info->columns*sizeof(IndexPacket));
        indexes+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read indexes from disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        (void) UncompressCache(cache);
      if (cache_info->file == -1)
        return(False);
    }
  number_pixels=cache_info->columns*cache_info->rows;
  for (y=0; y < (int) nexus_info->rows; y++)
  {
    count=lseek(cache_info->file,number_pixels*sizeof(PixelPacket)+offset*
      sizeof(IndexPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=read(cache_info->file,(char *) indexes,
      nexus_info->columns*sizeof(IndexPacket));
    if (count != (nexus_info->columns*sizeof(IndexPacket)))
      return(False);
    indexes+=nexus_info->columns;
    offset+=cache_info->columns;
  }
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
%  Method ReadCachePixels() reads pixels from the specified region of the pixel
%  cache.
%
%  The format of the ReadCachePixels() method is:
%
%      unsigned int ReadCachePixels(Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o status: Method ReadCachePixels() returns True if the pixels are
%      successfully read from the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o id: specifies which cache nexus to read the pixels.
%
%
*/
static unsigned int ReadCachePixels(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    offset;

  register int
    y;

  register NexusInfo
    *nexus_info;

  register PixelPacket
    *pixels;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (IsNexusInCore(cache,id))
    return(True);
  nexus_info=cache_info->nexus_info+id;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  pixels=nexus_info->pixels;
  if (cache_info->type != DiskCache)
    {
      /*
        Read pixels from memory.
      */
      for (y=0; y < (int) nexus_info->rows; y++)
      {
        memcpy(pixels,cache_info->pixels+offset,
          nexus_info->columns*sizeof(PixelPacket));
        pixels+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read pixels from disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        (void) UncompressCache(cache);
      if (cache_info->file == -1)
        return(False);
    }
  for (y=0; y < (int) nexus_info->rows; y++)
  {
    count=lseek(cache_info->file,offset*sizeof(PixelPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=read(cache_info->file,(char *) pixels,
      nexus_info->columns*sizeof(PixelPacket));
    if (count != (nexus_info->columns*sizeof(PixelPacket)))
      return(False);
    pixels+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  return(True);
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
  get_pixel_handler=GetPixelCache;
  set_pixel_handler=SetPixelCache;
  sync_pixel_handler=SyncPixelCache;
  get_pixels_from_handler=GetPixelsFromCache;
  get_indexes_from_handler=GetIndexesFromCache;
  get_one_pixel_from_handler=GetOnePixelFromCache;
  close_pixel_handler=ClosePixelCache;
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
%      PixelPacket *SetCacheNexus(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows,
%        const unsigned int id)
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
%    o id: specifies which cache nexus to set.
%
%
*/
MagickExport PixelPacket *SetCacheNexus(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    offset;

  RectangleInfo
    region;

  unsigned int
    status;

  assert(image != (Image *) NULL);
  assert(image->cache != (Cache) NULL);
  assert(image->signature == MagickSignature);
  if ((image->storage_class != GetCacheClass(image->cache)) ||
      (image->colorspace != GetCacheColorspace(image->cache)))
    {
      /*
        Allocate pixel cache.
      */
      status=OpenCache(image);
      if (status == False)
        {
          ThrowException(&image->exception,CacheWarning,
            "Unable to open pixel cache",image->filename);
          return((PixelPacket *) NULL);
        }
    }
  /*
    Validate pixel cache geometry.
  */
  cache_info=(CacheInfo *) image->cache;
  offset=y*cache_info->columns+x;
  if (offset < 0)
    return((PixelPacket *) NULL);
  offset+=(rows-1)*cache_info->columns+columns-1;
  if (offset > (cache_info->columns*cache_info->rows))
    return((PixelPacket *) NULL);
  /*
    Return pixel cache.
  */
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  return(SetNexus(image,&region,id));
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
%      void SetCacheThreshold(const long threshold)
%
%  A description of each parameter follows:
%
%    o threshold: The number of megabytes of memory available to the pixel
%      cache.
%
%
*/
MagickExport void SetCacheThreshold(const long threshold)
{
  off_t
    offset;

  if (cache_threshold == ~0)
    cache_threshold=0;
  offset=1048576*(cache_threshold-threshold);
  (void) GetCacheMemory(-offset);
  cache_threshold=threshold;
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
%      PixelPacket *SetImagePixels(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
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
MagickExport PixelPacket *SetImagePixels(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
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
%      PixelPacket SetNexus(Image *image,const RectangleInfo *region,
%        const unsigned int id)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetNexus() returns a pointer to the pixels
%      associated with the specified cache nexus.
%
%    o image: The image.
%
%    o id: specifies which cache nexus to set.
%
%    o region: A pointer to the RectangleInfo structure that defines the
%      region of this particular cache nexus.
%
%
*/
static PixelPacket *SetNexus(Image *image,const RectangleInfo *region,
  const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    length,
    number_pixels;

  register NexusInfo
    *nexus_info;

  assert(image != (Image *) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  nexus_info=cache_info->nexus_info+id;
  nexus_info->columns=region->width;
  nexus_info->rows=region->height;
  nexus_info->x=region->x;
  nexus_info->y=region->y;
  if ((cache_info->type != DiskCache) && (image->clip_mask == (Image *) NULL))
    if ((((nexus_info->x+nexus_info->columns) <= cache_info->columns) &&
        (nexus_info->rows == 1)) || ((nexus_info->x == 0) &&
        ((nexus_info->columns % cache_info->columns) == 0)))
      {
        off_t
          offset;

        /*
          Pixels are accessed directly from memory.
        */
        offset=nexus_info->y*cache_info->columns+nexus_info->x;
        nexus_info->pixels=cache_info->pixels+offset;
        nexus_info->indexes=(IndexPacket *) NULL;
        if ((cache_info->storage_class == PseudoClass) ||
            (cache_info->colorspace == CMYKColorspace))
          nexus_info->indexes=cache_info->indexes+offset;
        return(nexus_info->pixels);
      }
  /*
    Pixels are stored in a staging area until they are synced to the cache.
  */
  number_pixels=nexus_info->columns*nexus_info->rows;
  length=number_pixels*sizeof(PixelPacket);
  if ((cache_info->storage_class == PseudoClass) ||
      (cache_info->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  if (nexus_info->staging == (PixelPacket *) NULL)
    nexus_info->staging=(PixelPacket *) AcquireMemory(length);
  else
    if (nexus_info->length != length)
      ReacquireMemory((void **) &nexus_info->staging,length);
  if (nexus_info->staging == (PixelPacket *) NULL)
    MagickError(ResourceLimitError,"Memory allocation failed",
      "unable to allocate cache nexus_info");
  nexus_info->length=length;
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
%  Method SetPixelCache() allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with SyncPixelCache().  A
%  pointer to the pixels is returned if the pixels are transferred, otherwise
%  a NULL is returned.
%
%  The format of the SetPixelCache() method is:
%
%      PixelPacket *SetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
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
static PixelPacket *SetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
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
%      SetPixelCacheMethods(GetPixelHandler get_pixel,
%        SetPixelHandler set_pixel,SyncPixelHandler sync_pixel,
%        GetPixelsFromHandler get_pixels_from,
%        GetIndexesFromHandler get_indexes_from,
%        GetOnePixelFromHandler get_one_pixel_from,
%        ClosePixelHandler close_pixel,DestroyPixelHandler destroy_pixel)
%
%
*/
MagickExport void SetPixelCacheMethods(GetPixelHandler get_pixel,
  SetPixelHandler set_pixel,SyncPixelHandler sync_pixel,
  GetPixelsFromHandler get_pixels_from,GetIndexesFromHandler get_indexes_from,
  GetOnePixelFromHandler get_one_pixel_from,ClosePixelHandler close_pixel,
  DestroyPixelHandler destroy_pixel)
{
  /*
    Set image pixel methods.
  */
  assert(get_pixel != (GetPixelHandler) NULL);
  assert(set_pixel != (SetPixelHandler) NULL);
  assert(sync_pixel != (SyncPixelHandler) NULL);
  assert(get_pixels_from != (GetPixelsFromHandler) NULL);
  assert(get_indexes_from != (GetIndexesFromHandler) NULL);
  assert(get_one_pixel_from != (GetOnePixelFromHandler) NULL);
  assert(close_pixel != (ClosePixelHandler) NULL);
  assert(destroy_pixel != (DestroyPixelHandler) NULL);
  get_pixel_handler=get_pixel;
  set_pixel_handler=set_pixel;
  sync_pixel_handler=sync_pixel;
  get_pixels_from_handler=get_pixels_from;
  get_indexes_from_handler=get_indexes_from;
  get_one_pixel_from_handler=get_one_pixel_from;
  close_pixel_handler=close_pixel;
  destroy_pixel_handler=destroy_pixel;
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
%      unsigned int SyncCacheNexus(Image *image,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o status: Method SyncCacheNexus() returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The image.
%
%    o id: specifies which cache nexus to destroy.
%
%
*/
MagickExport unsigned int SyncCacheNexus(Image *image,const unsigned int id)
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
  if (IsNexusInCore(image->cache,id))
    return(True);
  if (image->clip_mask != (Image *) NULL)
    {
      CacheInfo
        *cache_info;

      NexusInfo
        *nexus_info;

      register PixelPacket
        *p,
        *q;

      ViewInfo
        *image_view,
        *mask_view;

      image_view=OpenCacheView(image);
      mask_view=OpenCacheView(image->clip_mask);
      if ((image_view == (ViewInfo *) NULL) || (mask_view == (ViewInfo *) NULL))
        ThrowBinaryException(CacheWarning,"Unable to open cache view",
          image->filename);
      cache_info=(CacheInfo *) image->cache;
      nexus_info=cache_info->nexus_info;
      p=GetCacheView(image_view,nexus_info->x,nexus_info->y,nexus_info->columns,
        nexus_info->rows);
      q=GetCacheView(mask_view,nexus_info->x,nexus_info->y,nexus_info->columns,
        nexus_info->rows);
      if ((p != (PixelPacket *) NULL) && (q != (PixelPacket *) NULL))
        {
          int
            y;

          register PixelPacket
            *r;

          register int
            x;

          /*
            Apply clip mask.
          */
          r=nexus_info->pixels;
          for (y=0; y < (int) nexus_info->rows; y++)
          {
            for (x=0; x < (int) nexus_info->columns; x++)
            {
              if (q->opacity == TransparentOpacity)
                *r=(*p);
              p++;
              q++;
              r++;
            }
          }
        }
      CloseCacheView(mask_view);
      CloseCacheView(image_view);
    }
  status=WriteCachePixels(image->cache,id);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    status|=WriteCacheIndexes(image->cache,id);
  if (status == False)
    ThrowBinaryException(CacheWarning,"Unable to sync pixel cache",
      image->filename);
  return(True);
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
%  Method SyncPixelCache() saves the image pixels to the in-memory or disk
%  cache.  The method returns True if the pixel region is synced, otherwise
%  False.
%
%  The format of the SyncPixelCache() method is:
%
%      unsigned int SyncPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelCache() returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
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
+   U n c o m p r e s s C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UncompressCache() uncompresses the disk-based pixel cache.
%
%  The format of the UncompressCache() method is:
%
%      unsigned int UncompressCache(Cache cache)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
static unsigned int UncompressCache(Cache cache)
{
#if defined(HasZLIB)
  CacheInfo
    *cache_info;

  char
    filename[MaxTextExtent],
    *pixels;

  gzFile
    file;

  int
    y;

  long
    count;

  size_t
    length;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->type != DiskCache)
    return(True);
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_CREAT |
        O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  if (lseek(cache_info->file,0,SEEK_SET) == -1)
    return(False);
  FormatString(filename,"%s.gz",cache_info->cache_filename);
  file=gzopen(filename,ReadBinaryType);
  if (file == (gzFile) NULL)
    return(False);
  length=cache_info->columns*Max(sizeof(IndexPacket),sizeof(PixelPacket));
  pixels=(char *) AcquireMemory(length);
  if (pixels == (char *) NULL)
    {
      (void) gzclose(file);
      return(False);
    }
  for (y=0; y < (int) cache_info->rows; y++)
  {
    count=gzread(file,pixels,cache_info->columns*sizeof(PixelPacket));
    if ((off_t) write(cache_info->file,pixels,count) != count)
      break;
  }
  if (y == (int) cache_info->rows)
    if ((cache_info->storage_class == PseudoClass) ||
        (cache_info->colorspace == CMYKColorspace))
      for (y=0; y < (int) cache_info->rows; y++)
      {
        count=gzread(file,pixels,cache_info->columns*sizeof(IndexPacket));
        if ((off_t) write(cache_info->file,pixels,count) != count)
          break;
      }
  LiberateMemory((void **) &pixels);
  (void) gzclose(file);
  (void) remove(filename);
  return(y == (int) cache_info->rows);
#else
  return(True);
#endif
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
%      unsigned int WriteCacheIndexes(Cache cache)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCacheIndexes() returns True if the indexes are
%      successfully written to the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o id: specifies which cache nexus to write the colormap indexes.
%
%
*/
static unsigned int WriteCacheIndexes(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    number_pixels,
    offset;

  register IndexPacket
    *indexes;

  register NexusInfo
    *nexus_info;

  register int
    y;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if ((cache_info->storage_class != PseudoClass) &&
      (cache_info->colorspace != CMYKColorspace))
    return(False);
  if (IsNexusInCore(cache,id))
    return(True);
  nexus_info=cache_info->nexus_info+id;
  indexes=nexus_info->indexes;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  if (cache_info->type != DiskCache)
    {
      /*
        Write indexes to memory.
      */
      for (y=0; y < (int) nexus_info->rows; y++)
      {
        memcpy(cache_info->indexes+offset,indexes,
          nexus_info->columns*sizeof(IndexPacket));
        indexes+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write indexes to disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        (void) UncompressCache(cache);
      if (cache_info->file == -1)
        return(False);
    }
  number_pixels=cache_info->columns*cache_info->rows;
  for (y=0; y < (int) nexus_info->rows; y++)
  {
    count=lseek(cache_info->file,number_pixels*sizeof(PixelPacket)+offset*
      sizeof(IndexPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=write(cache_info->file,(char *) indexes,
      nexus_info->columns*sizeof(IndexPacket));
    if (count != (nexus_info->columns*sizeof(IndexPacket)))
      return(False);
    indexes+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e C a c h e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCacheInfo() writes the persistent cache meta information to a
%  file on disk.
%
%  The format of the WriteCacheInfo() method is:
%
%      unsigned int WriteCacheInfo(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCacheInfo() returns True if the cache meta
%      information is written to the specified file, otherwise False.
%
%    o image: The image.
%
%
*/
static unsigned int WriteCacheInfo(Image *image)
{
  CacheInfo
    *cache_info;

  ImageAttribute
    *attribute;

  FILE
    *file;

  register int
    i;

  /*
    Write persistent cache meta-information.
  */
  cache_info=(CacheInfo *) image->cache;
  file=fopen(cache_info->meta_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    return(False);
  (void) fprintf(file,"id=MagickCache\n");
  (void) fprintf(file,"cache=%.1024s  quantum-depth=%d\n",
    cache_info->cache_filename,QuantumDepth);
  if (image->storage_class == PseudoClass)
    (void) fprintf(file,"class=PseudoClass  colors=%u  matte=%s\n",
      image->colors,image->matte ? "True" : "False");
  else
    if (image->colorspace == CMYKColorspace)
      (void) fprintf(file,"class=DirectClass  colorspace=CMYK  matte=%s\n",
        image->matte ? "True" : "False");
    else
      (void) fprintf(file,"class=DirectClass  matte=%s\n",
        image->matte ? "True" : "False");
  (void) fprintf(file,"compression=");
  switch (image->compression)
  {
    default:
    case NoCompression: (void) fprintf(file,"None\n"); break;
    case BZipCompression: (void) fprintf(file,"BZip\n"); break;
    case FaxCompression: (void) fprintf(file,"Fax\n"); break;
    case Group4Compression: (void) fprintf(file,"Group4\n"); break;
    case JPEGCompression: (void) fprintf(file,"JPEG\n"); break;
    case LosslessJPEGCompression: (void) fprintf(file,"Lossless\n"); break;
    case LZWCompression: (void) fprintf(file,"LZW\n"); break;
    case RunlengthEncodedCompression: (void) fprintf(file,"RLE\n"); break;
    case ZipCompression: (void) fprintf(file,"Zip\n"); break;
  }
  (void) fprintf(file,"columns=%u  rows=%u  depth=%u\n",image->columns,
    image->rows,image->depth);
  if ((image->x_resolution != 0) && (image->y_resolution != 0))
    {
      char
        units[MaxTextExtent];

      /*
        Set image resolution.
      */
      (void) strcpy(units,"undefined");
      if (image->units == PixelsPerInchResolution)
        (void) strcpy(units,"pixels-per-inch");
      if (image->units == PixelsPerCentimeterResolution)
        (void) strcpy(units,"pixels-per-centimeter");
      (void) fprintf(file,"Resolution=%gx%g  units=%.1024s\n",
        image->x_resolution,image->y_resolution,units);
    }
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"page=%ux%u%+d%+d\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  if ((image->next != (Image *) NULL) || (image->previous != (Image *) NULL))
    (void) fprintf(file,"scene=%u  iterations=%u  delay=%u  Dispose=%u\n",
      image->scene,image->iterations,image->delay,image->dispose);
  else
    {
      if (image->scene != 0)
        (void) fprintf(file,"scene=%u\n",image->scene);
      if (image->iterations != 1)
        (void) fprintf(file,"iterations=%u\n",image->iterations);
      if (image->delay != 0)
        (void) fprintf(file,"delay=%u\n",image->delay);
      if (image->dispose != 0)
        (void) fprintf(file,"dispose=%u\n",image->dispose);
    }
  if (image->mean_error_per_pixel != 0.0)
    (void) fprintf(file,"error=%g  mean-error=%g  maximum-error=%g\n",
      image->mean_error_per_pixel,image->normalized_mean_error,
      image->normalized_maximum_error);
  if (image->rendering_intent != UndefinedIntent)
    {
      if (image->rendering_intent == SaturationIntent)
        (void) fprintf(file,"rendering-intent=saturation\n");
      else
        if (image->rendering_intent == PerceptualIntent)
          (void) fprintf(file,"rendering-intent=perceptual\n");
        else
          if (image->rendering_intent == AbsoluteIntent)
            (void) fprintf(file,"rendering-intent=absolute\n");
          else
            (void) fprintf(file,"rendering-intent=relative\n");
    }
  if (image->gamma != 0.0)
    (void) fprintf(file,"gamma=%g\n",image->gamma);
  if (image->chromaticity.white_point.x != 0.0)
    {
      /*
        Note chomaticity points.
      */
      (void) fprintf(file,
        "red-primary=%g,%g  green-primary=%g,%g  blue-primary=%g,%g\n",
        image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
        image->chromaticity.green_primary.x,
        image->chromaticity.green_primary.y,
        image->chromaticity.blue_primary.x,
        image->chromaticity.blue_primary.y);
      (void) fprintf(file,"white-point=%g,%g\n",
        image->chromaticity.white_point.x,image->chromaticity.white_point.y);
    }
  if (image->color_profile.length > 0)
    (void) fprintf(file,"profile-icc=%u\n",image->color_profile.length);
  if (image->iptc_profile.length > 0)
    (void) fprintf(file,"profile-iptc=%u\n",image->iptc_profile.length);
  if (image->generic_profiles != 0)
    {
      /*
        Generic profile.
      */
      for (i=0; i < (int) image->generic_profiles; i++)
        (void) fprintf(file,"profile-%s=%u\n",
          image->generic_profile[i].name == (char *) NULL ? "generic" :
          image->generic_profile[i].name,image->generic_profile[i].length);
    }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"montage=%.1024s\n",image->montage);
  SignatureImage(image);
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (ImageAttribute *) NULL; attribute=attribute->next)
  {
    if (attribute->value != NULL)
      {
        (void) fprintf(file,"%.1024s=",attribute->key);
        for (i=0; i < (int) strlen(attribute->value); i++)
          if (isspace((int) attribute->value[i]))
            break;
        if (i < (int) strlen(attribute->value))
          (void) fputc('{',file);
        (void) fwrite(attribute->value,strlen(attribute->value),1,file);
        if (i < (int) strlen(attribute->value))
          (void) fputc('}',file);
        (void) fputc('\n',file);
      }
  }
  (void) fprintf(file,"\f\n:\032");
  if (image->montage != (char *) NULL)
    {
      /*
        Write montage tile directory.
      */
      if (image->directory != (char *) NULL)
        (void) fwrite(image->directory,strlen(image->directory),1,file);
      (void) fputc('\0',file);
    }
  if (image->color_profile.length > 0)
    (void) fwrite(image->color_profile.info,image->color_profile.length,1,file);
  if (image->iptc_profile.length > 0)
    (void) fwrite(image->iptc_profile.info,image->iptc_profile.length,1,file);
  if (image->generic_profiles != 0)
    {
      /*
        Generic profile.
      */
      for (i=0; i < (int) image->generic_profiles; i++)
      {
        if (image->generic_profile[i].length == 0)
          continue;
        (void) fwrite(image->generic_profile[i].info,
          image->generic_profile[i].length,1,file);
      }
    }
  if (image->storage_class == PseudoClass)
    {
      register unsigned char
        *q;

      unsigned char
        *colormap;

      unsigned int
        packet_size;

      /*
        Allocate colormap.
      */
      packet_size=image->colors > 256 ? 6 : 3;
      colormap=(unsigned char *) AcquireMemory(packet_size*image->colors);
      if (colormap == (unsigned char *) NULL)
        return(False);
      /*
        Write colormap to file.
      */
      q=colormap;
      if (image->colors <= 256)
        for (i=0; i < (int) image->colors; i++)
        {
          *q++=image->colormap[i].red;
          *q++=image->colormap[i].green;
          *q++=image->colormap[i].blue;
        }
      else
        for (i=0; i < (int) image->colors; i++)
        {
          *q++=image->colormap[i].red >> 8;
          *q++=image->colormap[i].red & 0xff;
          *q++=image->colormap[i].green >> 8;
          *q++=image->colormap[i].green & 0xff;
          *q++=image->colormap[i].blue >> 8;
          *q++=image->colormap[i].blue & 0xff;
        }
      (void) fwrite(colormap,packet_size,image->colors,file);
      LiberateMemory((void **) &colormap);
    }
  fclose(file);
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
%      unsigned int WriteCachePixels(Cache cache)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCachePixels() returns True if the pixels are
%      successfully written to the cache, otherwise False.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o id: specifies which cache nexus to write the pixels.
%
%
*/
static unsigned int WriteCachePixels(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    offset;

  register NexusInfo
    *nexus_info;

  register int
    y;

  register PixelPacket
    *pixels;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickSignature);
  if (IsNexusInCore(cache,id))
    return(True);
  nexus_info=cache_info->nexus_info+id;
  pixels=nexus_info->pixels;
  offset=nexus_info->y*cache_info->columns+nexus_info->x;
  if (cache_info->type != DiskCache)
    {
      /*
        Write pixels to memory.
      */
      for (y=0; y < (int) nexus_info->rows; y++)
      {
        memcpy(cache_info->pixels+offset,pixels,
          nexus_info->columns*sizeof(PixelPacket));
        pixels+=nexus_info->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write pixels to disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->cache_filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        (void) UncompressCache(cache);
      if (cache_info->file == -1)
        return(False);
    }
  for (y=0; y < (int) nexus_info->rows; y++)
  {
    count=lseek(cache_info->file,offset*sizeof(PixelPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=write(cache_info->file,(char *) pixels,
      nexus_info->columns*sizeof(PixelPacket));
    if (count != (nexus_info->columns*sizeof(PixelPacket)))
      return(False);
    pixels+=nexus_info->columns;
    offset+=cache_info->columns;
  }
  return(True);
}
