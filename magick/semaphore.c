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

struct SemaphoreInfo 
{
  HANDLE id;
};

#if defined(_MT)

/* This is a binary semphore -- increase for a counting semaphore */
#define MAXSEMLEN	1

/* Create a semaphore */
SemaphoreInfo *CreateSemaphoreInfo(void)
{
  SECURITY_ATTRIBUTES security;
  SemaphoreInfo *semaphore;

  /* Allocate semaphore memory */
  semaphore = (SemaphoreInfo *)AllocateMemory(sizeof(*semaphore));
  if ( semaphore == NULL )
    {
      return(NULL);
    }

  /* Allow the semaphore to be inherited */
  security.nLength = sizeof(security);
  security.lpSecurityDescriptor = NULL;
  security.bInheritHandle = TRUE;

  /* Create the semaphore, with initial value signaled */
  semaphore->id = CreateSemaphore(&security, 1, MAXSEMLEN, NULL);
  if ( semaphore->id == NULL )
    {
      free(semaphore);
      return(NULL);
    }
	return(semaphore);
}

/* Lock the semaphore */
int LockSemaphore(SemaphoreInfo *semaphore)
{
  if ( WaitForSingleObject(semaphore->id, INFINITE) == WAIT_FAILED )
    return(-1);
  return(0);
}

/* Unlock the semaphore */
int UnlockSemaphore(SemaphoreInfo *semaphore)
{
  if ( ReleaseSemaphore(semaphore->id, 1, NULL) == FALSE )
    return(-1);
  return(0);
}

/* Free the semaphore */
void DestroySemaphoreInfo(SemaphoreInfo *semaphore)
{
  if ( semaphore )
    {
      CloseHandle(semaphore->id);
      free(semaphore);
    }
}
#else /* _MT */
/* Create a semaphore */
SemaphoreInfo *CreateSemaphoreInfo(void)
{
  SemaphoreInfo *semaphore;

  /* Allocate semaphore memory */
  semaphore = (SemaphoreInfo *)AllocateMemory(sizeof(*semaphore));
  if ( semaphore == NULL )
    return(NULL);
  return(semaphore);
}

/* Lock the semaphore */
int LockSemaphore(SemaphoreInfo *semaphore)
{
  return(0);
}

/* Unlock the semaphore */
int UnlockSemaphore(SemaphoreInfo *semaphore)
{
  return(0);
}

/* Free the semaphore */
void DestroySemaphoreInfo(SemaphoreInfo *semaphore)
{
  FreeMemory((void **) &semaphore);
}
#endif /* _MT */
#else /* _VISUALC_ */

/* Simple semaphore lock */
#if defined(HasPTHREADS)

#if (__GLIBC__ == 2) && (__GLIBC_MINOR__ == 0)
#warning Working around a bug in glibc 2.0 pthreads
#undef HasPTHREADS
/* The bug is actually a problem where threads are suspended,
   but don't wake up when the thread manager sends them a signal.
   This is a problem with thread creation too, but it happens
   less often. We avoid this by using System V IPC for semaphorees.
*/
#endif /* glibc 2.0 */
#endif /* linux */

#if defined(HasPTHREADS)

#include <pthread.h>

struct SemaphoreInfo
{
  pthread_mutex_t id;
};

/* Create a blockable semaphore */
SemaphoreInfo * CreateSemaphoreInfo (void)
{
  SemaphoreInfo *semaphore;

  /* Allocate the structure */
  semaphore = (SemaphoreInfo *)AllocateMemory(sizeof(*semaphore));
  if ( semaphore == NULL )
    return(NULL);

  if ( pthread_mutex_init(&semaphore->id, NULL) != 0 )
  {
    FreeMemory((void **) &semaphore);
    return(NULL);
  }
  return(semaphore);
}

/* Lock the semaphore */
int LockSemaphore (SemaphoreInfo *semaphore)
{
  if ( ! semaphore )
    return(-1);
  if ( pthread_mutex_lock(&semaphore->id) != 0 )
    return(-1);
  return(0);
}

/* Unlock the semaphore */
int UnlockSemaphore (SemaphoreInfo *semaphore)
{
  if ( ! semaphore )
    return(-1);
  if ( pthread_mutex_unlock(&semaphore->id) != 0 )
    return(-1);
  return(0);
}

/* Free the semaphore */
void DestroySemaphoreInfo (SemaphoreInfo *semaphore)
{
  if ( semaphore )
    {
      pthread_mutex_destroy(&semaphore->id);
      FreeMemory((void **) &semaphore);
    }
}

#else /* System V IPC based implementation */

#include <sys/ipc.h>
#include <sys/sem.h>

struct SemaphoreInfo
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
SemaphoreInfo * CreateSemaphoreInfo (void)
{
  SemaphoreInfo *semaphore;
  union semun init;

  semaphore = (SemaphoreInfo *)AllocateMemory(sizeof(*semaphore));
  if ( semaphore == NULL )
    return(NULL);
  semaphore->id = semget(IPC_PRIVATE, 1, (0600|IPC_CREAT));
  if ( semaphore->id < 0 )
    {
      FreeMemory((void **) &semaphore);
      return(NULL);
    }
  init.val = 0;		/* Initialize semaphore */
  semctl(semaphore->id, 0, SETVAL, init);
  return(semaphore);
}

/* Lock the semaphore */
int LockSemaphore (SemaphoreInfo *semaphore)
{
  if ( ! semaphore )
    return(-1);
tryagain:
  if ( semop(semaphore->id, op_lock, 2) < 0 )
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
int UnlockSemaphore (SemaphoreInfo *semaphore)
{
  if ( ! semaphore )
    return(-1);
  if ( semop(semaphore->id, op_unlock, 1) < 0 )
    return(-1);
  return(0);
}

/* Free the semaphore */
void DestroySemaphoreInfo (SemaphoreInfo *semaphore)
{
  if ( semaphore )
    {
#ifdef _SGI_SOURCE
      semctl(semaphore->id, 0, IPC_RMID);
#else
      semctl(semaphore->id, 0, IPC_RMID, (union semun)0);
#endif
      FreeMemory((void **) &semaphore);
    }
}

#endif /* HasPTHREADS */
#endif /* _VISUALC_ */
