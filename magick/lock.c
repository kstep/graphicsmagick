/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        L      OOO    CCCC  K   K                            %
%                        L     O   O  C      K  K                             %
%                        L     O   O  C      KKK                              %
%                        L     O   O  C      K  K                             %
%                        LLLL   OOO    CCCC  K   K                            %
%                                                                             %
%                                                                             %
%                        ImageMagick Mutex Methods                            %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                             William Radcliffe                               %
%                                 June 2000                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

#if defined(_VISUALC_)
#include <windows.h>

struct MagickMutex 
{
  HANDLE id;
};

#if defined(_MT)

/* This is a binary semphore -- increase for a counting semaphore */
#define MAXSEMLEN	1

/* Create a semaphore */
MagickMutex *MagickCreateMutex(void)
{
  SECURITY_ATTRIBUTES security;
  MagickMutex *mutex;

  /* Allocate mutex memory */
  mutex = (MagickMutex *)AllocateMemory(sizeof(*mutex));
  if ( mutex == NULL )
    {
      return(NULL);
    }

  /* Allow the semaphore to be inherited */
  security.nLength = sizeof(security);
  security.lpSecurityDescriptor = NULL;
  security.bInheritHandle = TRUE;

  /* Create the semaphore, with initial value signaled */
  mutex->id = CreateSemaphore(&security, 1, MAXSEMLEN, NULL);
  if ( mutex->id == NULL )
    {
      free(mutex);
      return(NULL);
    }
	return(mutex);
}

/* Lock the semaphore */
int MagickMutexP(MagickMutex *mutex)
{
  if ( WaitForSingleObject(mutex->id, INFINITE) == WAIT_FAILED )
    return(-1);
  return(0);
}

/* Unlock the semaphore */
int MagickMutexV(MagickMutex *mutex)
{
  if ( ReleaseSemaphore(mutex->id, 1, NULL) == FALSE )
    return(-1);
  return(0);
}

/* Free the semaphore */
void MagickDestroyMutex(MagickMutex *mutex)
{
  if ( mutex )
    {
      CloseHandle(mutex->id);
      free(mutex);
    }
}
#else /* _MT */
/* Create a semaphore */
MagickMutex *MagickCreateMutex(void)
{
  MagickMutex *mutex;

  /* Allocate mutex memory */
  mutex = (MagickMutex *)AllocateMemory(sizeof(*mutex));
  if ( mutex == NULL )
    return(NULL);
  return(mutex);
}

/* Lock the semaphore */
int MagickMutexP(MagickMutex *mutex)
{
  return(0);
}

/* Unlock the semaphore */
int MagickMutexV(MagickMutex *mutex)
{
  return(0);
}

/* Free the semaphore */
void MagickDestroyMutex(MagickMutex *mutex)
{
  FreeMemory((void **) &mutex);
}
#endif /* _MT */
#else /* _VISUALC_ */

/* Simple mutex lock */
#if defined(HasPTHREADS)

#if (__GLIBC__ == 2) && (__GLIBC_MINOR__ == 0)
#warning Working around a bug in glibc 2.0 pthreads
#undef HasPTHREADS
/* The bug is actually a problem where threads are suspended,
   but don't wake up when the thread manager sends them a signal.
   This is a problem with thread creation too, but it happens
   less often. We avoid this by using System V IPC for mutexes.
*/
#endif /* glibc 2.0 */
#endif /* linux */

#if defined(HasPTHREADS)

#include <pthread.h>

struct MagickMutex
{
  pthread_mutex_t id;
};

/* Create a blockable semaphore */
MagickMutex * MagickCreateMutex (void)
{
  MagickMutex *mutex;

  /* Allocate the structure */
  mutex = (MagickMutex *)AllocateMemory(sizeof(*mutex));
  if ( mutex == NULL )
    return(NULL);

  if ( pthread_mutex_init(&mutex->id, NULL) != 0 )
  {
    FreeMemory((void **) &mutex);
    return(NULL);
  }
  return(mutex);
}

/* Lock the semaphore */
int MagickMutexP (MagickMutex *mutex)
{
  if ( ! mutex )
    return(-1);
  if ( pthread_mutex_lock(&mutex->id) != 0 )
    return(-1);
  return(0);
}

/* Unlock the semaphore */
int MagickMutexV (MagickMutex *mutex)
{
  if ( ! mutex )
    return(-1);
  if ( pthread_mutex_unlock(&mutex->id) != 0 )
    return(-1);
  return(0);
}

/* Free the semaphore */
void MagickDestroyMutex (MagickMutex *mutex)
{
  if ( mutex )
    {
      pthread_mutex_destroy(&mutex->id);
      FreeMemory((void **) &mutex);
    }
}

#else /* System V IPC based implementation */

#include <sys/ipc.h>
#include <sys/sem.h>

struct MagickMutex
{
  int id;
};

/* Not defined by Solaris or later versions of Linux */
#if defined(__SVR4) || defined(_SEM_SEMUN_UNDEFINED)
union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};
#endif

static struct sembuf op_lock[2] = {
  { 0, 0, 0 },          /* Wait for semaphore to reach 0 */
  { 0, 1, SEM_UNDO },   /* Increment semaphore */
};
static struct sembuf op_unlock[1] = {
  { 0, -1, (IPC_NOWAIT|SEM_UNDO) }  /* Decrement semaphore */
};

/* Create a blockable semaphore */
MagickMutex * MagickCreateMutex (void)
{
  MagickMutex *mutex;
  union semun init;

  mutex = (MagickMutex *)AllocateMemory(sizeof(*mutex));
  if ( mutex == NULL )
    return(NULL);
  mutex->id = semget(IPC_PRIVATE, 1, (0600|IPC_CREAT));
  if ( mutex->id < 0 )
    {
      FreeMemory((void **) &mutex);
      return(NULL);
    }
  init.val = 0;		/* Initialize semaphore */
  semctl(mutex->id, 0, SETVAL, init);
  return(mutex);
}

/* Lock the semaphore */
int MagickMutexP (MagickMutex *mutex)
{
  if ( ! mutex )
    return(-1);
tryagain:
  if ( semop(mutex->id, op_lock, 2) < 0 )
    {
      if ( errno == EINTR )
        {
          goto tryagain;
        }
      return(-1);
    }
  return(0);
}

/* Unlock the semaphore */
int MagickMutexV (MagickMutex *mutex)
{
  if ( ! mutex )
    return(-1);
  if ( semop(mutex->id, op_unlock, 1) < 0 )
    return(-1);
  return(0);
}

/* Free the semaphore */
void MagickDestroyMutex (MagickMutex *mutex)
{
  if ( mutex )
    {
#ifdef _SGI_SOURCE
      semctl(mutex->id, 0, IPC_RMID);
#else
      semctl(mutex->id, 0, IPC_RMID, (union semun)0);
#endif
      FreeMemory((void **) &mutex);
    }
}

#endif /* HasPTHREADS */
#endif /* _VISUALC_ */
