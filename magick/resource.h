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
  MemoryMapResource
} ResourceType;

/*
  Method declarations.
*/
extern MagickExport off_t
  GetMagickResourceUsage(const ResourceType);

extern MagickExport unsigned int
  AcquireMagickResource(const ResourceType,const off_t);

extern MagickExport void
  DestroyMagickResources(void),
  LiberateMagickResource(const ResourceType,const off_t),
  SetMagickResourceLimit(const ResourceType,const off_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
