/*
  Magick registry methods.
*/
#ifndef _MAGICK_REGISTRY_H
#define _MAGICK_REGISTRY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _RegistryInfo
{
  struct _RegistryInfo
    *previous,
    *next;

  long
    id;

  RegistryType
    type;

  void
    *blob;

  size_t
    length;
} RegistryInfo;

/*
  Magick registry methods.
*/
extern MagickExport const void
  *GetMagickRegistry(const long,RegistryType *,size_t *);

extern MagickExport long
  SetMagickRegistry(const RegistryType,const void *,const size_t,
    ExceptionInfo *);

extern MagickExport unsigned int
  DeleteMagickRegistry(const long);

extern MagickExport void
  DestroyMagickRegistry(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
