/*
  ImageMagick Memory Methods.
*/
#ifndef _MAGICK_MEMORY_H
#define _MAGICK_MEMORY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport void
  *AcquireMemory(const size_t),
  *CopyMemory(void *,const void *,const size_t),
  LiberateMemory(void **),
  ReacquireMemory(void **,const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
