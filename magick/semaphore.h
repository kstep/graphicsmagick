/*
  Methods to lock and unlock semaphores.
*/
#ifndef _MAGICK_SEMAPHORE_H
#define _MAGICK_SEMAPHORE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Semaphore methods.
*/
extern MagickExport SemaphoreInfo
  *AllocateSemaphoreInfo(void);

extern MagickExport unsigned int
  LockSemaphoreInfo(SemaphoreInfo *),
  UnlockSemaphoreInfo(SemaphoreInfo *);

extern MagickExport void
  AcquireSemaphoreInfo(SemaphoreInfo **),
  DestroySemaphore(void),
  DestroySemaphoreInfo(SemaphoreInfo *),
  InitializeSemaphore(void),
  LiberateSemaphoreInfo(SemaphoreInfo **);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
