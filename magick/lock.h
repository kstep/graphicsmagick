
/* Simple mutex lock */
typedef pthread_mutex_t SimpleMutexLock;

/* Recursive mutex lock (may be locked multiple times by same thread) */
typedef struct _RecursiveMutexLock
{
  /* Guards the state of the nesting level and thread id. */
  pthread_mutex_t nesting_mutex;

  /* This condition variable suspends other waiting threads until the
     mutex is available. */
  pthread_cond_t lock_available;

  /* Current nesting level of the recursion. */
  unsigned int  nesting_level;

  /* Current owner of the lock. */
  pthread_t owner_id;

} RecursiveMutexLock;


extern MagickExport int
  RecursiveMutexInit(RecursiveMutexLock *lock),
  RecursiveMutexDestroy(RecursiveMutexLock *lock),
  RecursiveMutexLock(RecursiveMutexLock *lock),
  RecursiveMutexTryLock(RecursiveMutexLock *lock),
  RecursiveMutexUnLock(RecursiveMutexLock *lock),
  SimpleMutexInit(SimpleMutexLock *lock),
  SimpleMutexDestroy(SimpleMutexLock *lock),
  SimpleMutexLock(SimpleMutexLock *lock),
  SimpleMutexTryLock(SimpleMutexLock *lock),
  SimpleMutexUnLock(SimpleMutexLock *lock);

