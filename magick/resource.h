/*
  Resource methods.
*/
#ifndef _MAGICK_RESOURCE_H
#define _MAGICK_RESOURCE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef enum
{
  UndefinedResource,
  MemoryResource,
  CacheResource,
  DiskResource,
  MemoryMappedResource
} ResourceType;

typedef struct _ResourceInfo
{
  off_t
    memory,
    cache,
    disk,
    memory_map;
} ResourceInfo;

/*
  Method declarations.
*/
extern MagickExport off_t
  GetMagickResources(const ResourceType);

extern MagickExport unsigned int
  AcquireMagickResources(const ResourceType,const off_t);

extern MagickExport void
  DestroyMagickResources(void),
  LiberateMagickResources(const ResourceType,const off_t),
  SetMagickResources(const ResourceType,const off_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
