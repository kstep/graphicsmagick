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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#include "defines.h"

/*
  Global declarations.
*/
static off_t
  cache_threshold = PixelCacheThreshold;

/*
  Declare pixel cache interfaces.
*/
static IndexPacket
  *GetIndexesFromCache(const Image *);

static off_t
  GetCacheMemory(const off_t);

static PixelPacket
  GetOnePixelFromCache(Image *,const int,const int),
  *GetPixelsFromCache(const Image *),
  *GetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *SetPixelCache(Image *,const int,const int,const unsigned int,
    const unsigned int);

static unsigned int
  SyncPixelCache(Image *);

static void
  ClosePixelCache(Image *),
  DestroyPixelCache(Image *);

/*
  Initialize the image pixel methods.
*/
MagickExport IndexPacket
  *(*GetIndexes)(const Image *) = GetIndexesFromCache;

MagickExport PixelPacket
  *(*GetImagePixels)(Image *,const int,const int,const unsigned int,
    const unsigned int) = GetPixelCache,
  (*GetOnePixel)(Image *,const int,const int) = GetOnePixelFromCache,
  *(*GetPixels)(const Image *) = GetPixelsFromCache,
  *(*SetImagePixels)(Image *,const int,const int,const unsigned int,
    const unsigned int) = SetPixelCache;

MagickExport unsigned int
  (*SyncImagePixels)(Image *) = SyncPixelCache;

MagickExport void
  (*CloseImagePixels)(Image *) = ClosePixelCache,
  (*DestroyImagePixels)(Image *) = DestroyPixelCache;

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
%  Method CloseCache closes the file handle associated with a disk pixel cache.
%
%  The format of the CloseCache method is:
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
  if (cache_info->file != -1)
    (void) close(cache_info->file);
  cache_info->file=(-1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClosePixelCache closes the pixel cache.  Use this method to prevent
%  the too many file descriptors from being allocated when reading an image
%  sequence.  File descriptors are only used for a disk-based cache.  This is
%  essentially a no-op for a memory-based cache.
%
%  The format of the ClosePixelCache method is:
%
%      void ClosePixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void ClosePixelCache(Image *image)
{
  assert(image != (Image *) NULL);
  if (image->cache == (void *) NULL)
    return;
  CloseCache(image->cache);
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
%  Method DestroyCacheInfo deallocates memory associated with the pixel cache.
%
%  The format of the DestroyCacheInfo method is:
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
    number_pixels;

  register int
    id;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  number_pixels=cache_info->columns*cache_info->rows;
  switch (cache_info->type)
  {
    case MemoryCache:
    {
      FreeMemory((void **) &cache_info->pixels);
      if (cache_info->class == PseudoClass)
        (void) GetCacheMemory(number_pixels*sizeof(IndexPacket));
      (void) GetCacheMemory(number_pixels*sizeof(PixelPacket));
      break;
    }
    case MemoryMappedCache:
    {
      size_t
        length;

      /*
        Unmap memory-mapped pixels and indexes.
      */
      length=number_pixels*sizeof(PixelPacket);
      if (cache_info->class == PseudoClass)
        length+=number_pixels*sizeof(IndexPacket);
      (void) UnmapBlob(cache_info->pixels,length);
    }
    case DiskCache:
    {
      CloseCache(cache);
      if (!cache_info->persist)
        (void) remove(cache_info->filename);
      break;
    }
    default:
      break;
  }
  if (cache_info->type != UndefinedCache)
    {
      for (id=0; id <= cache_info->rows; id++)
        DestroyCacheNexus(cache,id);
      FreeMemory((void **) &cache_info->nexus);
    }
  FreeMemory((void **) &cache_info);
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
%  Method DestroyCacheNexus destroys a cache nexus.
%
%  The format of the DestroyCacheNexus method is:
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
    *nexus;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  nexus=cache_info->nexus+id;
  nexus->available=True;
  if (nexus->line != (void *) NULL)
    FreeMemory((void **) &nexus->line);
  nexus->line=(void *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y P i x e l C a c h e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPixelCache deallocates memory associated with the pixel cache.
%
%  The format of the DestroyPixelCache method is:
%
%      void DestroyPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void DestroyPixelCache(Image *image)
{
  assert(image != (Image *) NULL);
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
+   G e t C a c h e C l a s s T y p e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheClassType returns the class type of the pixel cache.
%
%  The format of the GetCacheClassType method is:
%
%      ClassType GetCacheClassType(Cache cache)
%
%  A description of each parameter follows:
%
%    o type: Method GetCacheClassType returns DirectClass or PseudoClass.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport ClassType GetCacheClassType(const Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  return(cache_info->class);
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
%  Method GetCacheInfo initializes the Cache structure.
%
%  The format of the GetCacheInfo method is:
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
  cache_info=(CacheInfo *) AllocateMemory(sizeof(CacheInfo));
  if (cache_info == (CacheInfo *) NULL)
    MagickError(ResourceLimitError,"Memory allocation failed",
      "unable to allocate cache info");
  cache_info->class=UndefinedClass;
  cache_info->type=UndefinedCache;
  cache_info->rows=0;
  cache_info->columns=0;
  cache_info->pixels=(PixelPacket *) NULL;
  cache_info->indexes=(IndexPacket *) NULL;
  cache_info->persist=False;
  *cache_info->filename='\0';
  cache_info->file=(-1);
  cache_info->nexus=(NexusInfo *) NULL;
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
%  Method GetCacheMemory adjusts the amount of free cache memory and then
%  returns the resulting value.
%
%  The format of the GetCacheMemory method is:
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
  static off_t
    free_memory = PixelCacheThreshold*1024*1024;

#if defined(HasPTHREADS)
  static pthread_mutex_t
    magick_mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&magick_mutex);
#endif
  free_memory+=memory;
#if defined(HasPTHREADS)
  pthread_mutex_unlock(&magick_mutex);
#endif
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
%  Method GetCacheNexus returns an available cache nexus.
%
%  The format of the GetCacheNexus method is:
%
%      unsigned int GetCacheNexus(Cache cache)
%
%  A description of each parameter follows:
%
%    o id:  Method GetCacheNexus returns an available cache nexus slot.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%
*/
MagickExport unsigned int GetCacheNexus(Cache cache)
{
  CacheInfo
    *cache_info;

  register unsigned int
    id;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  for (id=1; id <= cache_info->rows; id++)
    if (cache_info->nexus[id].available)
      {
        cache_info->nexus[id].available=False;
        return(id);
      }
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I n d e x e s F r o m C a c h e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetIndexesFromCache returns the colormap indexes associated with
%  the last call to the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetIndexesFromCache method is:
%
%      IndexPacket *GetIndexesFromCache(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexesFromCache returns the colormap indexes
%      associated with the last call to the SetPixelCache() or GetPixelCache()
%      methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
static IndexPacket *GetIndexesFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
  return(GetNexusIndexes(image->cache,0));
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
%  Method GetNexusIndexes returns the colormap indexes associated with the
%  specified nexus.
%
%  The format of the GetNexusIndexes method is:
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
    *nexus;

  if (cache == (Cache) NULL)
    return((IndexPacket *) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->class == UndefinedClass)
    return((IndexPacket *) NULL);
  nexus=cache_info->nexus+id;
  return(nexus->indexes);
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
%  Method GetNexusPixels returns the pixels associated with the specified cache
%  nexus.
%
%  The format of the GetNexusPixels method is:
%
%      PixelPacket *GetNexusPixels(const Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetNexusPixels returns the indexes associated with the
%      the specified cache nexus.
%
%    o id: specifies which cache nexus to return the pixels. 
%
%
%
*/
MagickExport PixelPacket *GetNexusPixels(const Cache cache,
  const unsigned int id)
{
  CacheInfo
    *cache_info;

  register NexusInfo
    *nexus;

  if (cache == (Cache) NULL)
    return((PixelPacket *) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->class == UndefinedClass)
    return((PixelPacket *) NULL);
  nexus=cache_info->nexus+id;
  return(nexus->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O n e P i x e l F r o m C a c h e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOnePixelFromCache returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixelFromCache method is:
%
%      PixelPacket *GetOnePixelFromCache(const Image image,const int x,
%        const int y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixelFromCache returns a pixel at the
%      specified (x,y) location.
%
%    o image: The address of a structure of type Image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
static PixelPacket GetOnePixelFromCache(Image *image,const int x,const int y)
{
  register PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
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
%   G e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelCache gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetPixelCache method is:
%
%      PixelPacket *GetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *GetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  PixelPacket
    *pixels;

  unsigned int
    status;

  /*
    Transfer pixels from the cache.
  */
  assert(image != (Image *) NULL);
  pixels=SetPixelCache(image,x,y,columns,rows);
  if (pixels == (PixelPacket *) NULL)
    return((PixelPacket *) NULL);
  if (IsNexusInCore(image->cache,0))
    return(pixels);
  status=ReadCachePixels(image->cache,0);
  if (image->class == PseudoClass)
    status|=ReadCacheIndexes(image->cache,0);
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
%   G e t P i x e l s F r o m C a c h e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelsFromCache returns the pixels associated with the last call
%  to the SetPixelCache() or GetPixelCache() methods.
%
%  The format of the GetPixelsFromCache method is:
%
%      PixelPacket *GetPixelsFromCache(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixelsFromCache returns the pixels associated with
%      the last call to the SetPixelCache() or GetPixelCache() methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
static PixelPacket *GetPixelsFromCache(const Image *image)
{
  assert(image != (Image *) NULL);
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
%  Method IsNexusInCore returns true if the pixels associated with the
%  specified cache nexus is non-strided and in core.
%
%  The format of the IsNexusInCore method is:
%
%      unsigned int IsNexusInCore(const Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o status: Method IsNexusInCore returns True if the pixels are non-strided
%      and in core, otherwise False.
%
%    o id: specifies which cache nexus to return the pixels. 
%
%
%
*/
MagickExport unsigned int IsNexusInCore(const Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    offset;

  register NexusInfo
    *nexus;

  if (cache == (Cache) NULL)
    return(False);
  cache_info=(CacheInfo *) cache;
  if (cache_info->class == UndefinedClass)
    return(False);
  nexus=cache_info->nexus+id;
  offset=nexus->y*cache_info->columns+nexus->x;
  if (cache_info->type != DiskCache)
    if (nexus->pixels == (cache_info->pixels+offset))
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
%  Method OpenCache allocates the pixel cache.  This includes defining
%  the cache dimensions, allocating space for the image pixels and optionally
%  the colormap indexes, and memory mapping the cache if it is disk based.
%  The cache nexus array is initialized as well.
%
%  The format of the OpenCache method is:
%
%      unsigned int OpenCache(Cache cache,const ClassType class_type,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method OpenCache returns True if the pixel cache is
%      initialized successfully otherwise False.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o class_type: DirectClass or PseudoClass.
%
%    o columns: This unsigned integer defines the number of columns in the
%      pixel cache.
%
%    o rows: This unsigned integer defines the number of rows in the pixel
%      cache.
%
%
*/
MagickExport unsigned int OpenCache(Cache cache,const ClassType class_type,
  const unsigned int columns,const unsigned int rows)
{
  CacheInfo
    *cache_info;

  char
    null = 0;

  off_t
    length,
    number_pixels;

  void
    *allocation;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  number_pixels=cache_info->columns*cache_info->rows;
  length=number_pixels*sizeof(PixelPacket);
  if (cache_info->class == PseudoClass)
    length+=number_pixels*sizeof(IndexPacket);
  cache_info->rows=rows;
  cache_info->columns=columns;
  number_pixels=cache_info->columns*cache_info->rows;
  if (cache_info->class != UndefinedClass)
    {
      /*
        Free memory-based cache resources.
      */
      if (cache_info->type == MemoryCache)
        (void) GetCacheMemory(length);
      if (cache_info->type == MemoryMappedCache)
        (void) UnmapBlob(cache_info->pixels,(size_t) length);
    }
  else
    {
      register int
        i;

      /*
        Allocate cache nexus.
      */
      cache_info->nexus=(NexusInfo *)
        AllocateMemory((cache_info->rows+1)*sizeof(NexusInfo));
      if (cache_info->nexus == (NexusInfo *) NULL)
        MagickError(ResourceLimitError,"Memory allocation failed",
          "unable to allocate cache nexus");
      for (i=0; i <= cache_info->rows; i++)
      {
        cache_info->nexus[i].available=True;
        cache_info->nexus[i].columns=0;
        cache_info->nexus[i].rows=0;
        cache_info->nexus[i].x=0;
        cache_info->nexus[i].y=0;
        cache_info->nexus[i].length=0;
        cache_info->nexus[i].line=(void *) NULL;
        cache_info->nexus[i].pixels=(PixelPacket *) NULL;
        cache_info->nexus[i].indexes=(IndexPacket *) NULL;
      }
      cache_info->nexus[0].available=False;
    }
  length=number_pixels*sizeof(PixelPacket);
  if (class_type == PseudoClass)
    length+=number_pixels*sizeof(IndexPacket);
  if ((cache_info->type == MemoryCache) ||
      ((cache_info->type == UndefinedCache) && (length <= GetCacheMemory(0))))
    {
      if (cache_info->class == UndefinedClass)
        allocation=AllocateMemory(length);
      else
        {
          cache_info->pixels=ReallocateMemory(cache_info->pixels,length);
          if (cache_info->pixels == (void *) NULL)
            return(False);
          allocation=cache_info->pixels;
        }
      if (allocation != (void *) NULL)
        {
          /*
            Create in-memory pixel cache.
          */
          (void) GetCacheMemory(-length);
          cache_info->class=class_type;
          cache_info->type=MemoryCache;
          cache_info->pixels=(PixelPacket *) allocation;
          if (cache_info->class == PseudoClass)
            cache_info->indexes=(IndexPacket *)
              (cache_info->pixels+number_pixels);
          return(True);
        }
    }
  /*
    Create pixel cache on disk.
  */
  if (cache_info->class == UndefinedClass)
    TemporaryFilename(cache_info->filename);
  if (cache_info->file == -1)
    {
      cache_info->file=
        open(cache_info->filename,O_RDWR | O_CREAT | O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  if (lseek(cache_info->file,length,SEEK_SET) == -1)
    return(False);
  if (write(cache_info->file,&null,sizeof(null)) == -1)
    return(False);
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  (void) ftruncate(cache_info->file,length);
#endif
  cache_info->class=class_type;
  if (cache_info->type != DiskCache)
    {
      size_t
        offset;

      cache_info->type=DiskCache;
      allocation=MapBlob(cache_info->file,IOMode,&offset);
      if (allocation != (void *) NULL)
        {
          /*
            Create memory-mapped pixel cache.
          */
          cache_info->type=MemoryMappedCache;
          cache_info->pixels=(PixelPacket *) allocation;
          if (cache_info->class == PseudoClass)
            cache_info->indexes=(IndexPacket *)
              (cache_info->pixels+number_pixels);
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
%  Method ReadCacheIndexes reads colormap indexes from the specified region
%  of the pixel cache.
%
%  The format of the ReadCacheIndexes method is:
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
MagickExport unsigned int ReadCacheIndexes(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    number_pixels,
    offset;

  register NexusInfo
    *nexus;

  register IndexPacket
    *indexes;

  register int
    y;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->class != PseudoClass)
    return(False);
  nexus=cache_info->nexus+id;
  offset=nexus->y*cache_info->columns+nexus->x;
  indexes=nexus->indexes;
  if (cache_info->type != DiskCache)
    {
      /*
        Read indexes from memory.
      */
      if (indexes == (cache_info->indexes+offset))
        return(True);
      for (y=0; y < (int) nexus->rows; y++)
      {
        memcpy(indexes,cache_info->indexes+offset,
          nexus->columns*sizeof(IndexPacket));
        indexes+=nexus->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read indexes from disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  number_pixels=cache_info->columns*cache_info->rows;
  for (y=0; y < (int) nexus->rows; y++)
  {
    count=lseek(cache_info->file,number_pixels*sizeof(PixelPacket)+offset*
      sizeof(IndexPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=read(cache_info->file,(char *) indexes,
      nexus->columns*sizeof(IndexPacket));
    if (count != (nexus->columns*sizeof(IndexPacket)))
      return(False);
    indexes+=nexus->columns;
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
%  Method ReadCachePixels reads pixels from the specified region of the pixel
%  cache.
%
%  The format of the ReadCachePixels method is:
%
%      unsigned int ReadCachePixels(Cache cache,const unsigned int id)
%
%  A description of each parameter follows:
%
%    o status: Method ReadCachePixels returns True if the pixels are
%      successfully read from the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o id: specifies which cache nexus to read the pixels. 
%
%
*/
MagickExport unsigned int ReadCachePixels(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    offset;

  register NexusInfo
    *nexus;

  register int
    y;

  register PixelPacket
    *pixels;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  nexus=cache_info->nexus+id;
  offset=nexus->y*cache_info->columns+nexus->x;
  pixels=nexus->pixels;
  if (cache_info->type != DiskCache)
    {
      /*
        Read pixels from memory.
      */
      if (pixels == (cache_info->pixels+offset))
        return(True);
      for (y=0; y < (int) nexus->rows; y++)
      {
        memcpy(pixels,cache_info->pixels+offset,
          nexus->columns*sizeof(PixelPacket));
        pixels+=nexus->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Read pixels from disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  for (y=0; y < (int) nexus->rows; y++)
  {
    count=lseek(cache_info->file,offset*sizeof(PixelPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=read(cache_info->file,(char *) pixels,
      nexus->columns*sizeof(PixelPacket));
    if (count != (nexus->columns*sizeof(PixelPacket)))
      return(False);
    pixels+=nexus->columns;
    offset+=cache_info->columns;
  }
  return(True);
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
%  Method SetCacheThreshold sets the amount of free memory allocated for the
%  pixel cache.  Once this threshold is exceeded, all subsequent pixels cache
%  operations are to/from disk.
%
%  The format of the SetCacheThreshold method is:
%
%      void SetCacheThreshold(const off_t threshold)
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
  off_t
    offset;

  offset=1024*1024*(cache_threshold-threshold);
  (void) GetCacheMemory(-offset);
  cache_threshold=threshold;
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
%  Method SetCacheNexus defines the region of the cache for the specified
%  cache nexus.
%
%  The format of the SetCacheNexus method is:
%
%      PixelPacket SetCacheNexus(Cache cache,const unsigned int id,
%        const RectangleInfo *region)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetCacheNexus returns a pointer to the pixels associated
%      with the specified cache nexus.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o id: specifies which cache nexus to set. 
%
%    o region: A pointer to the RectangleInfo structure that defines the
%      region of this particular cache nexus.
%
%
*/
MagickExport PixelPacket *SetCacheNexus(Cache cache,const unsigned int id,
  const RectangleInfo *region)
{
  CacheInfo
    *cache_info;

  off_t
    length,
    number_pixels;

  register NexusInfo
    *nexus;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  nexus=cache_info->nexus+id;
  nexus->columns=region->width;
  nexus->rows=region->height;
  nexus->x=region->x;
  nexus->y=region->y;
  if (cache_info->type != DiskCache)
    if ((((nexus->x+nexus->columns) <= cache_info->columns) &&
        (nexus->rows == 1)) || ((nexus->x == 0) &&
        ((nexus->columns % cache_info->columns) == 0)))
      {
        /*
          Pixels are accessed directly from memory.
        */
        nexus->pixels=cache_info->pixels+nexus->y*cache_info->columns+nexus->x;
        nexus->indexes=(IndexPacket *) NULL;
        if (cache_info->class == PseudoClass)
          nexus->indexes=
            cache_info->indexes+nexus->y*cache_info->columns+nexus->x;
        return(nexus->pixels);
      }
  /*
    Pixels are stored in a temporary buffer until they are synced to the cache.
  */
  number_pixels=nexus->columns*nexus->rows;
  length=number_pixels*sizeof(PixelPacket);
  if (cache_info->class == PseudoClass)
    length+=number_pixels*sizeof(IndexPacket);
  if (nexus->line == (void *) NULL)
    nexus->line=AllocateMemory(length);
  else
    if (nexus->length != length)
      nexus->line=ReallocateMemory(nexus->line,length);
  if (nexus->line == (void *) NULL)
    MagickError(ResourceLimitError,"Memory allocation failed",
      "unable to allocate cache nexus");
  nexus->length=length;
  nexus->pixels=(PixelPacket *) nexus->line;
  nexus->indexes=(IndexPacket *) (nexus->pixels+nexus->columns*nexus->rows);
  return(nexus->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetPixelCache allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with method SyncPixelCache.
%  A pointer to the pixels is returned if the pixels are transferred,
%  otherwise a NULL is returned.
%
%  The format of the SetPixelCache method is:
%
%      PixelPacket *SetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *SetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  RectangleInfo
    region;

  unsigned int
    status;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+columns) > (int) image->columns) ||
      ((y+rows) > (int) image->rows) || (columns == 0) || (rows == 0))
    {
      ThrowException(&image->exception,CacheWarning,"Unable to set pixel cache",
        "image does not contain the cache geometry");
      return((PixelPacket *) NULL);
    }
  if (image->cache == (Cache) NULL)
    GetCacheInfo(&image->cache);
  if (image->class != GetCacheClassType(image->cache))
    {
      /*
        Allocate pixel cache.
      */
      status=OpenCache(image->cache,image->class,image->columns,image->rows);
      if (status == False)
        {
          SetCacheThreshold((image->columns*image->rows-1)/1024/1024);
          status=
            OpenCache(image->cache,image->class,image->columns,image->rows);
        }
      if (status == False)
        {
          ThrowException(&image->exception,CacheWarning,
            "Unable to open pixel cache",image->filename);
          return((PixelPacket *) NULL);
        }
    }
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  return(SetCacheNexus(image->cache,0,&region));
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
%  Method SetPixelCacheMethods set the default image pixel methods to
%  use the pixel cache.
%
%
*/
MagickExport void SetPixelCacheMethods(void)
{
  /*
    Reset image pixel methods.
  */
  CloseImagePixels=ClosePixelCache;
  DestroyImagePixels=DestroyPixelCache;
  GetImagePixels=GetPixelCache;
  GetIndexes=GetIndexesFromCache;
  GetOnePixel=GetOnePixelFromCache;
  GetPixels=GetPixelsFromCache;
  SetImagePixels=SetPixelCache;
  SyncImagePixels=SyncPixelCache;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncPixelCache saves the image pixels to the in-memory or disk cache.
%  The method returns True if the pixel region is synced, otherwise False.
%
%  The format of the SyncPixelCache method is:
%
%      unsigned int SyncPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelCache returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
static unsigned int SyncPixelCache(Image *image)
{
  unsigned int
    status;

  /*
    Transfer pixels to the cache.
  */
  assert(image != (Image *) NULL);
  if (image->cache == (Cache) NULL)
    ThrowBinaryException(CacheWarning,"pixel cache is not open",
      image->filename);
  image->taint=True;
  if (IsNexusInCore(image->cache,0))
    return(True);
  status=WriteCachePixels(image->cache,0);
  if (image->class == PseudoClass)
    status|=WriteCacheIndexes(image->cache,0);
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
+   W r i t e C a c h e I n d e x e s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCachePixels writes the colormap indexes to the specified region
%  of the pixel cache.
%
%  The format of the WriteCachePixels method is:
%
%      unsigned int WriteCachePixels(Cache cache)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCachePixels returns True if the colormap indexes
%      are successfully written to the pixel cache, otherwise False.
%
%    o cache: Specifies a pointer to a CacheInfo structure.
%
%    o id: specifies which cache nexus to write the colormap indexes. 
%
%
*/
MagickExport unsigned int WriteCacheIndexes(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    number_pixels,
    offset;

  register NexusInfo
    *nexus;

  register IndexPacket
    *indexes;

  register int
    y;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->class != PseudoClass)
    return(False);
  nexus=cache_info->nexus+id;
  indexes=nexus->indexes;
  offset=nexus->y*cache_info->columns+nexus->x;
  if (cache_info->type != DiskCache)
    {
      /*
        Write indexes to memory.
      */
      if (indexes == (cache_info->indexes+offset))
        return(True);
      for (y=0; y < (int) nexus->rows; y++)
      {
        memcpy(cache_info->indexes+offset,indexes,
          nexus->columns*sizeof(IndexPacket));
        indexes+=nexus->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write indexes to disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  number_pixels=cache_info->columns*cache_info->rows;
  for (y=0; y < (int) nexus->rows; y++)
  {
    count=lseek(cache_info->file,number_pixels*sizeof(PixelPacket)+offset*
      sizeof(IndexPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=write(cache_info->file,(char *) indexes,
      nexus->columns*sizeof(IndexPacket));
    if (count != (nexus->columns*sizeof(IndexPacket)))
      return(False);
    indexes+=nexus->columns;
    offset+=cache_info->columns;
  }
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
%  Method WriteCachePixels writes image pixels to the specified region of the
%  pixel cache.
%
%  The format of the WriteCachePixels method is:
%
%      unsigned int WriteCachePixels(Cache cache)
%
%  A description of each parameter follows:
%
%    o status: Method WriteCachePixels returns True if the pixels are
%      successfully written to the cache, otherwise False.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o id: specifies which cache nexus to write the pixels. 
%
%
*/
MagickExport unsigned int WriteCachePixels(Cache cache,const unsigned int id)
{
  CacheInfo
    *cache_info;

  off_t
    count,
    offset;

  register NexusInfo
    *nexus;

  register int
    y;

  register PixelPacket
    *pixels;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  nexus=cache_info->nexus+id;
  pixels=nexus->pixels;
  offset=nexus->y*cache_info->columns+nexus->x;
  if (cache_info->type != DiskCache)
    {
      /*
        Write pixels to memory.
      */
      if (pixels == (cache_info->pixels+offset))
        return(True);
      for (y=0; y < (int) nexus->rows; y++)
      {
        memcpy(cache_info->pixels+offset,pixels,
          nexus->columns*sizeof(PixelPacket));
        pixels+=nexus->columns;
        offset+=cache_info->columns;
      }
      return(True);
    }
  /*
    Write pixels to disk.
  */
  if (cache_info->file == -1)
    {
      cache_info->file=open(cache_info->filename,O_RDWR | O_BINARY,0777);
      if (cache_info->file == -1)
        return(False);
    }
  for (y=0; y < (int) nexus->rows; y++)
  {
    count=lseek(cache_info->file,offset*sizeof(PixelPacket),SEEK_SET);
    if (count == -1)
      return(False);
    count=write(cache_info->file,(char *) pixels,
      nexus->columns*sizeof(PixelPacket));
    if (count != (nexus->columns*sizeof(PixelPacket)))
      return(False);
    pixels+=nexus->columns;
    offset+=cache_info->columns;
  }
  return(True);
}
