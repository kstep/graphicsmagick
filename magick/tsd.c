/*
  Copyright (C) 2005,2013,2014 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
*/

/*
  Thread specific data support.

  The OpenMP case also compiles when OpenMP is not available.
*/

#include "magick/studio.h"
#include "magick/utility.h"

#if defined(HAVE_PTHREAD)
/* Use pthreads TSD APIs */
#  define USE_PTHREAD_TSD 1
#elif defined(MSWINDOWS)
/* Use WIN32 TSD APIs */
#  define USE_WIN32_TSD 1
#else
/* Use OpenMP APIs or default to single threaded */
#  define USE_OPENMP_TSD 1
#endif

#if USE_PTHREAD_TSD
#include <pthread.h>
#endif /* USE_PTHREAD_TSD */
#if USE_WIN32_TSD
#  include <windows.h>
#endif /* USE_WIN32_TSD */
#include "magick/tsd.h"

#if USE_OPENMP_TSD
typedef struct _MagickTsdKeyStorage_t
{
  void **values;
  MagickFreeFunc destructor;
  unsigned int nvalues;
} MagickTsdKeyStorage_t;
#endif /* USE_OPENMP_TSD */

/*
  Create a thread specific data key (with destructor).
*/
MagickExport MagickPassFail MagickTsdKeyCreate2(MagickTsdKey_t *key,
                                                MagickFreeFunc destructor)
{
#if USE_PTHREAD_TSD
  return ((pthread_key_create(key, destructor) == 0) ? MagickPass : MagickFail);
#elif USE_WIN32_TSD
  /* DWORD WINAPI TlsAlloc(void); */
  ARG_NOT_USED(destructor); /* FIXME: No solution yet */
  *key=TlsAlloc();
  return ((*key != TLS_OUT_OF_INDEXES) ? MagickPass : MagickFail);
#else
  MagickTsdKeyStorage_t **keyd = (MagickTsdKeyStorage_t **) key;
  MagickPassFail status = MagickPass;
  *keyd=MagickAllocateMemory(MagickTsdKeyStorage_t *,sizeof(MagickTsdKeyStorage_t));
  if (*keyd != (void *) NULL)
    {
      (*keyd)->nvalues=omp_get_max_threads();
      (*keyd)->values=MagickAllocateArray(void *,sizeof(void *),(*keyd)->nvalues);
      (*keyd)->destructor=destructor;
      if ((*keyd)->values == (void *) NULL)
        {
          MagickFreeMemory(*keyd);
        }
      else
        {
          (void) memset((*keyd)->values,0,sizeof(void *)*(*keyd)->nvalues);
        }
    }
  status = ((*keyd != (MagickTsdKeyStorage_t *) NULL) ? MagickPass : MagickFail);

  return status;
#endif
}

/*
  Create a thread specific data key.
*/
MagickExport MagickPassFail MagickTsdKeyCreate(MagickTsdKey_t *key)
{
  return MagickTsdKeyCreate2(key,0);
}

/*
  Delete a thread-specific data key, invoking the registered
  destructor on any remaining non-NULL key values.
*/
MagickExport MagickPassFail MagickTsdKeyDelete(MagickTsdKey_t key)
{
#if USE_PTHREAD_TSD
  return ((pthread_key_delete(key) == 0) ? MagickPass : MagickFail);
#elif USE_WIN32_TSD
  /* BOOL WINAPI TlsFree(DWORD dwTlsIndex) */
  return ((TlsFree(key) != 0) ? MagickPass : MagickFail);
#else
  MagickTsdKeyStorage_t *keyd = (MagickTsdKeyStorage_t *) key;
  if (keyd->destructor != (MagickFreeFunc) NULL)
    {
      unsigned int index;
      for (index = 0; index < keyd->nvalues; index++)
        {
          if (keyd->values[index] != (void *) NULL)
            {
              keyd->destructor(keyd->values[index]);
              keyd->values[index]=(void *) NULL;
            }
        }
    }
  MagickFreeMemory(keyd);
  return MagickPass;
#endif
}

/*
  Set the thread-specific value associated with the key.
*/
MagickExport MagickPassFail MagickTsdSetSpecific(MagickTsdKey_t key, const void *value)
{
#if USE_PTHREAD_TSD
  return ((pthread_setspecific(key, value) == 0) ? MagickPass : MagickFail);
#elif USE_WIN32_TSD
  /* BOOL WINAPI TlsSetValue(DWORD dwTlsIndex,LPVOID lpTlsValue) */
  return ((TlsSetValue(key,(void *) value) != 0) ? MagickPass : MagickFail);
#else
  MagickTsdKeyStorage_t *keyd = (MagickTsdKeyStorage_t *) key;
  unsigned int index=omp_get_thread_num();
  keyd->values[index]=(void *) value;
  return MagickPass;
#endif
}

/*
  Get the thread-specific value associated with the key.
*/
MagickExport void *MagickTsdGetSpecific(MagickTsdKey_t key)
{
#if USE_PTHREAD_TSD
  return (pthread_getspecific(key));
#elif USE_WIN32_TSD
  /* LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex) */
  return TlsGetValue(key);
#else
  MagickTsdKeyStorage_t *keyd = (MagickTsdKeyStorage_t *) key;
  unsigned int index=omp_get_thread_num();
  return keyd->values[index];
#endif
}
