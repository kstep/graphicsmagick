/* The mutex structure, defined in lock.c */
struct MagickMutex;
typedef struct MagickMutex MagickMutex;

/* Create a mutex, initialized unlocked */
extern MagickExport MagickMutex * Magick_CreateMutex(void);

/* Lock the mutex  (Returns 0, or -1 on error) */
#define Magick_LockMutex(m)	\
{ \
  if (!m) \
    m=Magick_CreateMutex(); \
  MagickMutexP(m); \
}
extern MagickExport int MagickMutexP(MagickMutex *mutex);

/* Unlock the mutex  (Returns 0, or -1 on error) */
#define Magick_UnlockMutex(m) \
{ \
  if (!m) \
    m=Magick_CreateMutex(); \
  MagickMutexV(m); \
}
extern MagickExport int MagickMutexV(MagickMutex *mutex);

/* Destroy a mutex */
extern MagickExport void Magick_DestroyMutex(MagickMutex *mutex);


