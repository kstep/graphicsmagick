/*
  Copyright (C) 2005,2013 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
*/

/*
  Thread specific data support.
*/

#include "magick/studio.h"
#include "magick/utility.h"
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif /* defined(HAVE_PTHREAD) */
#if defined(MSWINDOWS)
#  include <windows.h>
#endif /* defined(MSWINDOWS) */
#include "magick/tsd.h"

#if !defined(HAVE_PTHREAD) && !defined(MSWINDOWS)
typedef struct _MagickTsdKeyStorage_t
{
  void *value;
  MagickFreeFunc destructor;
} MagickTsdKeyStorage_t;
#endif /* !defined(HAVE_PTHREAD) && !defined(MSWINDOWS) */

/*
  Create a thread specific data key (with destructor).
*/
MagickExport MagickPassFail MagickTsdKeyCreate2(MagickTsdKey_t *key,
                                                MagickFreeFunc destructor)
{
#if defined(HAVE_PTHREAD)
  return ((pthread_key_create(key, destructor) == 0) ? MagickPass : MagickFail);
#elif defined(MSWINDOWS)
  /* DWORD WINAPI TlsAlloc(void); */
  ARG_NOT_USED(destructor); /* FIXME: No solution yet */
  *key=TlsAlloc();
  return ((*key != TLS_OUT_OF_INDEXES) ? MagickPass : MagickFail);
#else
  MagickTsdKeyStorage_t **keyd = (MagickTsdKeyStorage_t **) key;
  *keyd=MagickAllocateMemory(MagickTsdKeyStorage_t *,sizeof(MagickTsdKeyStorage_t));
  if (*keyd != (void *) NULL)
    {
      (*keyd)->value=0;
      (*keyd)->destructor=destructor;
    }
  return ((*keyd != (MagickTsdKeyStorage_t *) NULL) ? MagickPass : MagickFail);
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
#if defined(HAVE_PTHREAD)
  return ((pthread_key_delete(key) == 0) ? MagickPass : MagickFail);
#elif defined(MSWINDOWS)
  /* BOOL WINAPI TlsFree(DWORD dwTlsIndex) */
  return ((TlsFree(key) != 0) ? MagickPass : MagickFail);
#else
  MagickTsdKeyStorage_t *keyd = (MagickTsdKeyStorage_t *) key;
  if ((keyd->value != 0) && (keyd->destructor != (MagickFreeFunc) NULL))
    {
      keyd->destructor(keyd->value);
      keyd->value=0;
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
#if defined(HAVE_PTHREAD)
  return ((pthread_setspecific(key, value) == 0) ? MagickPass : MagickFail);
#elif defined(MSWINDOWS)
  /* BOOL WINAPI TlsSetValue(DWORD dwTlsIndex,LPVOID lpTlsValue) */
  return ((TlsSetValue(key,(void *) value) != 0) ? MagickPass : MagickFail);
#else
  MagickTsdKeyStorage_t *keyd = (MagickTsdKeyStorage_t *) key;
  keyd->value=(void *) value;
  return MagickPass;
#endif
}

/*
  Get the thread-specific value associated with the key.
*/
MagickExport void *MagickTsdGetSpecific(MagickTsdKey_t key)
{
#if defined(HAVE_PTHREAD)
  return (pthread_getspecific(key));
#elif defined(MSWINDOWS)
  /* LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex) */
  return TlsGetValue(key);
#else
  MagickTsdKeyStorage_t *keyd = (MagickTsdKeyStorage_t *) key;
  return keyd->value;
#endif
}
