
#include "lock.h"

MagickExport int RecursiveMutexInit(RecursiveMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  static pthread_t NULL_thread;
  if(pthread_mutex_init(&lock->nesting_mutex,NULL) == -1)
    return -1;
  else if(pthread_cond_init(&lock->lock_available,NULL) == -1)
    return -1;
  else
    {
      lock->nesting_level=0;
      lock->owner_id=NULL_thread;
      return 0;
    }
#endif /* defined(HasPTHREADS) */
}
MagickExport int RecursiveMutexDestroy(RecursiveMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  if(pthread_mutex_destroy(&lock->nesting_mutex) == -1)
    return -1;
  else if(pthread_cond_destroy(&lock->lock_available) == -1)
    return -1;
  else
    return 0;
#endif /* defined(HasPTHREADS) */
}
MagickExport int RecursiveMutexLock(RecursiveMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  int result=0;
  int oerrno=0;
  pthread_t t_id=pthread_self();

  /* Acquire the guard. */
  if(pthread_mutex_lock(&lock->nesting_mutex) == -1)
    result=-1;
  else
    {
      /* If there's no contention, just grab the lock immediately (since
         this is the common case we'll optimize for it). */
      if(&lock->nesting_level == 0)
        lock->owner_id = t_id;
      /* If we already own the lock, then increment the nesting level and
         return. */
      else if(pthread_equal(t_id,lock->owner_id) == 0)
        {
          /* Wait until the nesting level has dropped to zero, at which
             point we can acquire the lock. */
          while(lock->nesting_level > 0)
            pthread_cond_wait(&lock->lock_available,
                              &lock->nesting_mutex);
          /* At this point the nesting_mutex_ is held... */
          lock->owner_id=t_id;
        }
      /* At this point, we can safely increment the nesting_level_ no
         matter how we got here! */
      lock->nesting_level++;
    }
  oerrno=errno;
  pthread_mutex_unlock(&lock->nesting_mutex);
  errno=oerrno;
  return result;
#endif /* defined(HasPTHREADS) */
}
MagickExport int RecursiveMutexTryLock(RecursiveMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  int result=0;
  int oerrno=0;
  pthread_t t_id=pthread_self();

  /* Acquire the guard. */
  if(pthread_mutex_lock(&lock->nesting_mutex) == -1)
    result=-1;
  else
    {
      /* If there's no contention, just grab the lock immediately. */
      if(lock->nesting_level == 0)
        {
          lock->owner_id=t_id;
          lock->nesting_level=1;
        }
      /* If we already own the lock, then increment the nesting level and
         proceed. */
      else if(pthread_equal(t_id,lock->owner_id))
        lock->nesting_level++;
      else
        {
          errno=EBUSY;
          result=-1;
        }
    }
  oerrno=errno;
  pthread_mutex_unlock(&lock->nesting_mutex);
  errno=oerrno;
  return result;
#endif /* defined(HasPTHREADS) */
}
MagickExport int RecursiveMutexUnLock(RecursiveMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  int result=0;
  int oerrno=0;
  static pthread_t NULL_thread;
  pthread_t t_id=pthread_self();

  if(pthread_mutex_lock(&lock->nesting_mutex) == -1)
    result=-1;
  else
    {
      if(lock->nesting_level == 0
         || pthread_equal(t_id,lock->owner_id) == 0)
        {
          errno=EINVAL;
          result=-1;
        }
      else
        {
          lock->nesting_level--;
          if(lock->nesting_level == 0)
            {
              /* This may not be strictly necessary, but it does put
                 the mutex into a known state... */
              lock->owner_id=NULL_thread;
              /* Inform waiters that the lock is free. */
              if(pthread_cond_signal(&lock->lock_available) == -1)
                result=-1;
            }
        }
    }
  oerrno=errno;
  pthread_mutex_unlock(&lock->nesting_mutex);
  errno=oerrno;
  return result;
#endif /* defined(HasPTHREADS) */
}

MagickExport int SimpleMutexInit(SimpleMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  pthread_mutex_init(lock,NULL);
#endif /* defined(HasPTHREADS) */
}
MagickExport int SimpleMutexDestroy(SimpleMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  pthread_mutex_destroy(lock);
#endif /* defined(HasPTHREADS) */
}
MagickExport int SimpleMutexLock(SimpleMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  pthread_mutex_lock(lock);
#endif /* defined(HasPTHREADS) */
}
MagickExport int SimpleMutexTryLock(SimpleMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  pthread_mutex_trylock(lock);
#endif /* defined(HasPTHREADS) */
}
MagickExport int SimpleMutexUnLock(SimpleMutexLock_t *lock)
{
#if defined(HasPTHREADS)
  pthread_mutex_unlock(lock);
#endif /* defined(HasPTHREADS) */
}
