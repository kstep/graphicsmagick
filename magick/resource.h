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
  MapResource,
  DiskResource;
} ResourceType;

/*
  Method declarations.
*/
extern MagickExport unsigned int
  AcquireMagickResource(const ResourceType,const off_t);

extern MagickExport unsigned long
  GetMagickResource(const ResourceType);

extern MagickExport void
  DestroyMagickResources(void),
  LiberateMagickResource(const ResourceType,const off_t),
  SetMagickResourceLimit(const ResourceType,const unsigned long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
