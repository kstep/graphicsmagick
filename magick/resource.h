/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
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
  AcquireMagickResource(const ResourceType type,const magick_int64_t size),
  ListMagickResourceInfo(FILE *file,ExceptionInfo *exception);

extern MagickExport unsigned long
  GetMagickResource(const ResourceType type);

extern MagickExport void
  DestroyMagickResources(void),
  InitializeMagickResources(void),
  LiberateMagickResource(const ResourceType type,const magick_int64_t size),
  SetMagickResourceLimit(const ResourceType type,const unsigned long limit);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
