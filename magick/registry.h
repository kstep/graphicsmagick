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
  long
    id;

  RegistryType
    type;

  void
    *blob;

  size_t
    length;

  unsigned long
    signature;

  struct _RegistryInfo
    *previous,
    *next;
} RegistryInfo;

/*
  Magick registry methods.
*/
extern MagickExport Image
  *GetImageFromMagickRegistry(const char *,long *id,ExceptionInfo *);

extern MagickExport long
  SetMagickRegistry(const RegistryType,const void *,const size_t,
    ExceptionInfo *);

extern MagickExport unsigned int
  DeleteMagickRegistry(const long);

extern MagickExport void
  DestroyMagickRegistry(void),
  *GetMagickRegistry(const long,RegistryType *,size_t *,ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
