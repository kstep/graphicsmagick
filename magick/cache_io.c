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
%                            IIIII     /  OOO                                 %
%                              I      /  O   O                                %
%                              I     /   O   O                                %
%                              I    /    O   O                                %
%                            IIIII /      OOO                                 %
%                                                                             %
%                                                                             %
%                    ImageMagick Pixel Cache I/O Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                             William Radcliffe                               %
%                               November 1999                                 %
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
#if defined(HasPTHREADS)
#include <pthread.h>
#endif

/*
  Typedef declarations.
*/
typedef struct _CacheInfo
{
  char
    filename[MaxTextExtent];

  FILE
    *file;

  ClassType
#if defined(__cplusplus) || defined(c_plusplus)
    c_class;
#else
    class;
#endif

  off_t
    number_pixels;

  unsigned int
    columns,
    rows,
    mapped;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;
} CacheInfo;

/*
  Global declarations.
*/
static off_t
  cache_threshold = PixelCacheThreshold;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AllocateCache initializes the pixel cache.  This includes defining
%  the cache dimensions, allocating space for the image pixels and optionally
%  the colormap indexes, and memory mapping the cache if it is disk based.
%
%  The format of the AllocateCache method is:
%
%      unsigned int AllocateCache(Cache cache,ClassType type,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method AllocateCache returns True if the pixel cache is
%      initialized successfully otherwise False.
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o type: DirectClass or PseudoClass.
%
%    o columns: This unsigned integer defines the number of columns in the
%      pixel cache.
%
%    o rows: This unsigned integer defines the number of rows in the pixel
%      cache.
%
%
*/
Export unsigned int AllocateCache(Cache cache,ClassType type,
  const unsigned int columns,const unsigned int rows)
{
  CacheInfo
    *cache_info;

  int
    status;

  off_t
    length,
    offset;

  size_t
    sans;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->class == UndefinedClass)
    {
      cache_info->rows=rows;
      cache_info->columns=columns;
      cache_info->number_pixels=columns*rows;
      length=cache_info->number_pixels*sizeof(PixelPacket);
      if (type == PseudoClass)
        length+=cache_info->number_pixels*sizeof(IndexPacket);
      if (length <= GetCacheMemory(0))
        {
          /*
            Create in-memory pixel cache.
          */
          cache_info->pixels=(PixelPacket *)
            AllocateMemory(cache_info->number_pixels*sizeof(PixelPacket));
          if (cache_info->pixels != (PixelPacket *) NULL)
            {
              cache_info->class=DirectClass;
              offset=cache_info->number_pixels*sizeof(PixelPacket);
              (void) GetCacheMemory(-offset);
              if (type == PseudoClass)
                {
                  cache_info->indexes=(IndexPacket *) AllocateMemory(
                    cache_info->number_pixels*sizeof(IndexPacket));
                  if (cache_info->indexes != (IndexPacket *) NULL)
                    {
                      cache_info->class=PseudoClass;
                      offset=cache_info->number_pixels*sizeof(IndexPacket);
                      (void) GetCacheMemory(-offset);
                    }
                }
            }
        }
      if (cache_info->class == UndefinedClass)
        {
          char
            filename[MaxTextExtent];

          /*
            Create pixel cache on disk.
          */
          TemporaryFilename(filename);
          cache_info->file=fopen(filename,"wb+");
          if (cache_info->file != (FILE *) NULL)
            {
              status=fseek(cache_info->file,length-1,SEEK_SET);
              if (status != 0)
                return(False);
              cache_info->class=DirectClass;
              (void) strcpy(cache_info->filename,filename);
              (void) fputc(0,cache_info->file);
              (void) fclose(cache_info->file);
              cache_info->file=(FILE *) NULL;
              cache_info->pixels=(PixelPacket *)
                MapBlob(cache_info->filename,IOMode,&sans);
              if (cache_info->pixels == (PixelPacket *) NULL)
                {
                  if (type == PseudoClass)
                    SetCacheClassType(cache,PseudoClass);
                }
              else
                {
                  cache_info->mapped=True;
                  if (type == PseudoClass)
                    {
                      SetCacheClassType(cache,PseudoClass);
                      cache_info->indexes=(IndexPacket *)
                        (cache_info->pixels+cache_info->number_pixels);
                    }
                }
            }
        }
    }
  if (type == PseudoClass)
    {
      if (cache_info->class != PseudoClass)
        {
          if (cache_info->filename[0] == '\0')
            {
              /*
                Create in-memory colormap index cache.
              */
              cache_info->indexes=(IndexPacket *)
                AllocateMemory(cache_info->number_pixels*sizeof(IndexPacket));
              if (cache_info->indexes != (IndexPacket *) NULL)
                {
                  SetCacheClassType(cache,PseudoClass);
                  offset=cache_info->number_pixels*sizeof(IndexPacket);
                  (void) GetCacheMemory(-offset);
                }
            }
          else
            {
              /*
                Create colormap index cache on disk.
              */
              if (cache_info->file == (FILE *) NULL)
                cache_info->file=fopen(cache_info->filename,"rb+");
              if (cache_info->file != (FILE *) NULL)
                {
                  length=cache_info->number_pixels*sizeof(PixelPacket)+
                    cache_info->number_pixels*sizeof(IndexPacket);
                  status=fseek(cache_info->file,length-1,SEEK_SET);
                  if (status == 0)
                    {
                      SetCacheClassType(cache,PseudoClass);
                      (void) fputc(0,cache_info->file);
                      (void) fclose(cache_info->file);
                      cache_info->file=(FILE *) NULL;
                      offset=cache_info->number_pixels*sizeof(PixelPacket);
                      if (cache_info->mapped)
                        (void) UnmapBlob(cache_info->pixels,offset);
                      cache_info->pixels=(PixelPacket *)
                        MapBlob(cache_info->filename,IOMode,&sans);
                      cache_info->mapped=
                        cache_info->pixels != (PixelPacket *) NULL;
                      if (cache_info->mapped)
                        cache_info->indexes=(IndexPacket *)
                          (cache_info->pixels+cache_info->number_pixels);
                    }
                }
            }
        }
      if (cache_info->class != PseudoClass)
        return(False);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e C a c h e                                                       %
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
Export void CloseCache(Cache cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->file != (FILE *) NULL)
    (void) fclose(cache_info->file);
  cache_info->file=(FILE *) NULL;
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
Export void DestroyCacheInfo(Cache cache)
{
  CacheInfo
    *cache_info;

  size_t
    length;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  if (cache_info->mapped)
    {
      /*
        Unmap memory-mapped pixels and indexes.
      */
      length=cache_info->number_pixels*sizeof(PixelPacket);
      if (cache_info->class == PseudoClass)
        length+=cache_info->number_pixels*sizeof(IndexPacket);
      (void) UnmapBlob(cache_info->pixels,length);
      cache_info->pixels=(PixelPacket *) NULL;
      cache_info->indexes=(IndexPacket *) NULL;
    }
  CloseCache(cache);
  if (*cache_info->filename != '\0')
    (void) remove(cache_info->filename);
  if (cache_info->indexes != (IndexPacket *) NULL)
    {
      FreeMemory(cache_info->indexes);
      (void) GetCacheMemory(cache_info->number_pixels*sizeof(IndexPacket));
    }
  if (cache_info->pixels != (PixelPacket *) NULL)
    {
      FreeMemory(cache_info->pixels);
      (void) GetCacheMemory(cache_info->number_pixels*sizeof(PixelPacket));
    }
  FreeMemory(cache_info);
  cache=(void *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e C l a s s T y p e                                         %
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
Export ClassType GetCacheClassType(Cache cache)
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
%   G e t C a c h e I n f o                                                   %
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
Export void GetCacheInfo(Cache *cache)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) AllocateMemory(sizeof(CacheInfo));
  *cache_info->filename='\0';
  cache_info->file=(FILE *) NULL;
  cache_info->class=UndefinedClass;
  cache_info->mapped=False;
  cache_info->pixels=(PixelPacket *) NULL;
  cache_info->indexes=(IndexPacket *) NULL;
  cache_info->number_pixels=0;
  cache_info->rows=0;
  cache_info->columns=0;
  *cache=cache_info;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e M e m o r y                                               %
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
%      off_t GetCacheMemory(off_t memory)
%
%  A description of each parameter follows:
%
%    o memory: Specifies the adjustment to the cache memory.  Use 0 to
%      return the current free memory in the cache.
%
%
*/
Export off_t GetCacheMemory(off_t memory)
{
  static off_t
    free_memory = PixelCacheThreshold*1024*1024;

#if defined(HasPTHREADS)
  {
    static pthread_mutex_t
      memory_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&memory_mutex);
    free_memory+=memory;
    pthread_mutex_unlock(&memory_mutex);
  }
#else
  free_memory+=memory;
#endif
  return(free_memory);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e T h e s h o l d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheThreshold gets the amount of free memory allocated for the
%  pixel cache.  Once this threshold is exceeded, all subsequent pixels cache
%  operations are to/from disk.
%
%  The format of the GetCacheThreshold method is:
%
%      off_t GetCacheThreshold()
%
%  A description of each parameter follows:
%
%    o threshold: The number of megabytes of memory available to the pixel
%      cache.
%
%
*/
Export off_t GetCacheThreshold()
{
  return(cache_threshold);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C a c h e I n d e x e s                                           %
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
%      unsigned int ReadCacheIndexes(Cache cache,RectangleInfo *region_info,
%        IndexPacket *indexes)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadCacheIndexes returns True if the colormap indexes
%      are successfully read from the pixel cache, otherwise False.
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%    o region_info:  The address of a RectangleInfo structure that defines
%      the cache region to read.
%
%    o indexes: The colormap indexes are copied from this IndexPacket address
%      to the pixel cache.
%
%
*/
Export unsigned int ReadCacheIndexes(Cache cache,RectangleInfo *region_info,
  IndexPacket *indexes)
{
  CacheInfo
    *cache_info;

  long
    count;

  off_t
    offset;

  register int
    y;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  offset=region_info->y*cache_info->columns+region_info->x;
  for (y=0; y < (int) region_info->height; y++)
  {
    if (cache_info->indexes != (IndexPacket *) NULL)
      (void) memcpy(indexes,cache_info->indexes+offset,
        region_info->width*sizeof(IndexPacket));
    else
      {
        if (cache_info->file == (FILE *) NULL)
          {
            cache_info->file=fopen(cache_info->filename,"rb+");
            if (cache_info->file == (FILE *) NULL)
              return(False);
          }
        count=fseek(cache_info->file,cache_info->number_pixels*
          sizeof(PixelPacket)+offset*sizeof(IndexPacket),SEEK_SET);
        if (count != 0)
          return(False);
        count=fread(indexes,sizeof(IndexPacket),region_info->width,
          cache_info->file);
        if (count != region_info->width)
          return(False);
      }
    indexes+=region_info->width;
    offset+=cache_info->columns;
  }
  if (y < (int) region_info->height)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C a c h e P i x e l s                                             %
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
%      unsigned int ReadCachePixels(Cache cache,RectangleInfo *region_info,
%        IndexPacket *indexes)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadCachePixels returns True if the pixels are
%      successfully read from the pixel cache, otherwise False.
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%    o region_info:  The address of a RectangleInfo structure that defines
%      the cache region to read.
%
%    o pixels: The pixels are copied from this PixelPacket address to the
%      pixel cache.
%
%
*/
Export unsigned int ReadCachePixels(Cache cache,RectangleInfo *region_info,
  PixelPacket *pixels)
{
  CacheInfo
    *cache_info;

  long
    count;

  off_t
    offset;

  register int
    y;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  offset=region_info->y*cache_info->columns+region_info->x;
  for (y=0; y < (int) region_info->height; y++)
  {
    if (cache_info->pixels != (PixelPacket *) NULL)
      (void) memcpy(pixels,cache_info->pixels+offset,
        region_info->width*sizeof(PixelPacket));
    else
      {
        if (cache_info->file == (FILE *) NULL)
          {
            cache_info->file=fopen(cache_info->filename,"rb+");
            if (cache_info->file == (FILE *) NULL)
              return(False);
          }
        count=fseek(cache_info->file,offset*sizeof(PixelPacket),SEEK_SET);
        if (count != 0)
          return(False);
        count=
          fread(pixels,sizeof(PixelPacket),region_info->width,cache_info->file);
        if (count != region_info->width)
          return(False);
      }
    pixels+=region_info->width;
    offset+=cache_info->columns;
  }
  if (y < (int) region_info->height)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C a c h e C l a s s T y p e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetCacheClassType sets the cache type:  DirectClass or PseudoClass.
%
%  The format of the SetCacheClassType method is:
%
%      void SetCacheClassType(Cache cache)
%
%  A description of each parameter follows:
%
%    o cache: Specifies a pointer to a Cache structure.
%
%    o type: The pixel cache type DirectClass or PseudoClass.
%
%
*/
Export void SetCacheClassType(Cache cache,ClassType type)
{
  CacheInfo
    *cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  cache_info->class=type;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C a c h e T h e s h o l d                                           %
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
%      void SetCacheThreshold(off_t threshold)
%
%  A description of each parameter follows:
%
%    o threshold: The number of megabytes of memory available to the pixel
%      cache.
%
%
*/
Export void SetCacheThreshold(off_t threshold)
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
%   W r i t e C a c h e I n d e x e s                                         %
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
%      unsigned int WriteCachePixels(Cache cache,RectangleInfo *region_info,
%        IndexPacket *indexes)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteCachePixels returns True if the colormap indexes
%      are successfully written to the pixel cache, otherwise False.
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%    o region_info:  The address of a RectangleInfo structure that defines
%      the cache region to write.
%
%    o indexes: The colormap indexes are copied from the pixel cache to this
%      IndexPacket address.
%
%
*/
Export unsigned int WriteCacheIndexes(Cache cache,RectangleInfo *region_info,
  IndexPacket *indexes)
{
  CacheInfo
    *cache_info;

  long
    count;

  off_t
    offset;

  register int
    y;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  offset=region_info->y*cache_info->columns+region_info->x;
  for (y=0; y < (int) region_info->height; y++)
  {
    if (cache_info->indexes != (IndexPacket *) NULL)
      (void) memcpy(cache_info->indexes+offset,indexes,
        region_info->width*sizeof(IndexPacket));
    else
      {
        if (cache_info->file == (FILE *) NULL)
          {
            cache_info->file=fopen(cache_info->filename,"rb+");
            if (cache_info->file == (FILE *) NULL)
              return(False);
          }
        count=fseek(cache_info->file,cache_info->number_pixels*
          sizeof(PixelPacket)+offset*sizeof(IndexPacket),SEEK_SET);
        if (count != 0)
          return(False);
        count=fwrite(indexes,sizeof(IndexPacket),region_info->width,
          cache_info->file);
        if (count != region_info->width)
          return(False);
      }
    indexes+=region_info->width;
    offset+=cache_info->columns;
  }
  if (y < (int) region_info->height)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C a c h e P i x e l s                                           %
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
%      unsigned int WriteCachePixels(Cache cache,RectangleInfo *region_info,
%        PixelPacket *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteCachePixels returns True if the pixels are
%      successfully written to the cache, otherwise False.
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%    o region_info:  The address of a RectangleInfo structure that defines
%      the cache region to write.
%
%    o pixels: The pixels are copied from the pixel cache to this PixelPacket
%      address.
%
%
*/
Export unsigned int WriteCachePixels(Cache cache,RectangleInfo *region_info,
  PixelPacket *pixels)
{
  CacheInfo
    *cache_info;

  long
    count;

  off_t
    offset;

  register int
    y;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  offset=region_info->y*cache_info->columns+region_info->x;
  for (y=0; y < (int) region_info->height; y++)
  {
    if (cache_info->pixels != (PixelPacket *) NULL)
      (void) memcpy(cache_info->pixels+offset,pixels,
        region_info->width*sizeof(PixelPacket));
    else
      {
        if (cache_info->file == (FILE *) NULL)
          {
            cache_info->file=fopen(cache_info->filename,"rb+");
            if (cache_info->file == (FILE *) NULL)
              return(False);
          }
        count=fseek(cache_info->file,offset*sizeof(PixelPacket),SEEK_SET);
        if (count != 0)
          return(False);
        count=fwrite(pixels,sizeof(PixelPacket),region_info->width,
          cache_info->file);
        if (count != region_info->width)
          return(False);
      }
    pixels+=region_info->width;
    offset+=cache_info->columns;
  }
  if (y < (int) region_info->height)
    return(False);
  return(True);
}
