/*
  Methods to lock and unlock semaphores.
*/
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.
*/
#define EngageSemaphore(semaphore_info)	\
{ \
  if (semaphore_info == (SemaphoreInfo *) NULL) \
    semaphore_info=AllocateSemaphoreInfo(); \
  (void) LockSemaphore(semaphore_info); \
}

#define DisengageSemaphore(semaphore_info) \
{ \
  if (semaphore_info == (SemaphoreInfo *) NULL) \
    semaphore_info=AllocateSemaphoreInfo(); \
  (void) UnlockSemaphore(semaphore_info); \
}

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
   *DestroySemaphoreInfo(SemaphoreInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
