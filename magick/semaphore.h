/*
  Methods to lock and unlock semaphores.
*/
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

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
extern MagickExport int
  LockSemaphore(SemaphoreInfo *),
  UnlockSemaphore(SemaphoreInfo *);

extern MagickExport SemaphoreInfo
  *AllocateSemaphoreInfo(void);

extern MagickExport void
  DestroySemaphoreInfo(SemaphoreInfo *),
  AcquireSemaphore(SemaphoreInfo **),
  LiberateSemaphore(SemaphoreInfo **);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
