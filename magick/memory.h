/*
  ImageMagick Memory allocation Methods.
*/
#ifndef _MAGICK_MEMORY_H
#define _MAGICK_MEMORY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
  Typedef declarations.
*/

/*
  Memory allocation methods.
*/

extern MagickExport void
  *AcquireMemory(const size_t),
  *CloneMemory(void *,const void *,const size_t),
  LiberateMemory(void **),
  ReacquireMemory(void **,const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* C++ */

#endif /* _MAGICK_MEMORY_H */
