/*
  Random number methods.
*/
#ifndef _MAGICK_RANDOM_H
#define _MAGICK_RANDOM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport void
  DestroyRandomReservoir(void),
  GetRandomKey(unsigned char *,const size_t),
  DistillRandomEvent(const unsigned char *,const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
