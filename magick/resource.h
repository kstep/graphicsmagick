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
  FileResource,
  MemoryResource,
  MapResource,
  DiskResource
} ResourceType;

/*
  Method declarations.
*/
extern MagickExport unsigned int
  AcquireMagickResource(const ResourceType,const ExtendedSignedIntegralType),
  ListMagickResourceInfo(FILE *file,ExceptionInfo *exception);

extern MagickExport unsigned long
  GetMagickResource(const ResourceType);

extern MagickExport void
  DestroyMagickResources(void),
  InitializeMagickResources(void),
  LiberateMagickResource(const ResourceType,const ExtendedSignedIntegralType),
  SetMagickResourceLimit(const ResourceType,const unsigned long);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
