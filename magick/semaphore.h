/*
  Methods to lock and unlock semaphores.
*/
#ifndef _MAGICK_SEMAPHORE_H
#define _MAGICK_SEMAPHORE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
  Typedef declarations.
*/
struct
  SemaphoreInfo;

typedef struct SemaphoreInfo SemaphoreInfo;

/*
  Semaphore methods.
*/

extern MagickExport SemaphoreInfo
  *AllocateSemaphoreInfo(void);

extern MagickExport unsigned int
  LockSemaphore(SemaphoreInfo *),
  UnlockSemaphore(SemaphoreInfo *);

extern MagickExport void
  AcquireSemaphore(SemaphoreInfo **,void (*)(void)),
  DestroySemaphore(SemaphoreInfo *),
  InitializeSemaphore(void),
  LiberateSemaphore(SemaphoreInfo **);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
