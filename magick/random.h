/*
  Random number methods.
*/
#ifndef _MAGICK_RANDOM_H
#define _MAGICK_RANDOM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport unsigned int
  GetRandomKey(unsigned char *,const size_t,ExceptionInfo *);

extern MagickExport void
  DestroyRandomReservoir(void),
  DistillRandomEvent(const unsigned char *,const size_t,ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
