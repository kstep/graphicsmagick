#if defined(HasPTHREADS)
static pthread_mutex_t
  spinlock_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#if defined(WIN32)
#define SPINLOCK_DELAY_MILLI_SECS 10

static int
  spinlock_mutex = 0;

/* Wait for spin lock */
static void _spinlock_wait (int *sl)
{
  while (InterlockedCompareExchange (sl, 1, 0) != 0)
  {
    /* slight delay - just in case OS does not giveup CPU */
    Sleep (SPINLOCK_DELAY_MILLI_SECS);
  }
}
/* Release spin lock */
static void _spinlock_release (int *sl)
{
  InterlockedExchange (sl, 0);
}
#if defined(HasPTHREADS)
#define SPINLOCK_WAIT (void) pthread_mutex_lock(&spinlock_mutex)
#endif
#if defined(WIN32)
#define SPINLOCK_WAIT _spinlock_wait(&spinlock_mutex)
#endif
#if defined(HasPTHREADS)
#define SPINLOCK_RELEASE (void) pthread_mutex_unlock(&spinlock_mutex)
#endif
#if defined(WIN32)
#define SPINLOCK_RELEASE _spinlock_release(&spinlock_mutex)
#endif
#endif
