
/* The semaphore structure, defined in semaphore.c */
struct SemaphoreInfo;
typedef struct SemaphoreInfo SemaphoreInfo;

/* Create a semaphore, initialized unlocked */
extern MagickExport SemaphoreInfo * CreateSemaphoreInfo(void);

/* Lock the semaphore  (Returns 0, or -1 on error) */
#define EngageSemaphore(semaphore)	\
{ \
  if (!semaphore) \
    semaphore=CreateSemaphoreInfo(); \
  LockSemaphore(semaphore); \
}
extern MagickExport int LockSemaphore(SemaphoreInfo *semaphore);

/* Unlock the semaphore  (Returns 0, or -1 on error) */
#define DisengageSemaphore(semaphore) \
{ \
  if (!semaphore) \
    semaphore=CreateSemaphoreInfo(); \
  UnlockSemaphore(semaphore); \
}
extern MagickExport int UnlockSemaphore(SemaphoreInfo *semaphore);

/* Destroy a semaphore */
extern MagickExport void DestroySemaphoreInfo(SemaphoreInfo *semaphore);


