/*
  Resource methods.
*/
#ifndef _MAGICK_RESOURCE_H
#define _MAGICK_RESOURCE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef enum
{
  UndefinedResource,
  MemoryResource,
  DiskResource,
  MemoryMappedResource
} ResourceType;

typedef struct _ResourceInfo
{
  off_t
    memory,
    disk,
    memory_map;
} ResourceInfo;

/*
  Method declarations.
*/
extern MagickExport ResourceInfo
  GetResourceLimits(const ResourceInfo *);

extern MagickExport void
  AcquireResources(const ResourceType,const off_t),
  LiberateResources(const ResourceType,const off_t),
  SetResourceLimits(const ResourceInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
