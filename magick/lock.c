
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
MagickMutex *Magick_CreateMutex(void)
{
	SECURITY_ATTRIBUTES security;
	MagickMutex *mutex;

	/* Allocate mutex memory */
	mutex = (MagickMutex *)malloc(sizeof(*mutex));
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
void Magick_DestroyMutex(MagickMutex *mutex)
{
	if ( mutex )
    {
		  CloseHandle(mutex->id);
		  free(mutex);
	  }
}
#else /* _MT */
/* Create a semaphore */
MagickMutex *Magick_CreateMutex(void)
{
	/* Allocate mutex memory */
	mutex = (MagickMutex *)malloc(sizeof(*mutex));
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
void Magick_DestroyMutex(MagickMutex *mutex)
{
  free(mutex);
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
MagickMutex * Magick_CreateMutex (void)
{
	MagickMutex *mutex;

	/* Allocate the structure */
	mutex = (MagickMutex *)malloc(sizeof(*mutex));
	if ( mutex == NULL )
		return(NULL);

	if ( pthread_mutex_init(&mutex->id, NULL) != 0 )
    {
		  free(mutex);
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
void Magick_DestroyMutex (MagickMutex *mutex)
{
	if ( mutex )
    {
		  pthread_mutex_destroy(&mutex->id);
		  free(mutex);
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
	{ 0, 0, 0 },			/* Wait for semaphore to reach 0 */
	{ 0, 1, SEM_UNDO },		/* Increment semaphore */
};
static struct sembuf op_unlock[1] = {
	{ 0, -1, (IPC_NOWAIT|SEM_UNDO) }	/* Decrement semaphore */
};

/* Create a blockable semaphore */
MagickMutex * Magick_CreateMutex (void)
{
	MagickMutex *mutex;
	union semun init;

	mutex = (MagickMutex *)malloc(sizeof(*mutex));
	if ( mutex == NULL )
		return(NULL);
	mutex->id = semget(IPC_PRIVATE, 1, (0600|IPC_CREAT));
	if ( mutex->id < 0 )
    {
		  free(mutex);
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
void Magick_DestroyMutex (MagickMutex *mutex)
{
	if ( mutex )
    {
#ifdef _SGI_SOURCE
		  semctl(mutex->id, 0, IPC_RMID);
#else
		  semctl(mutex->id, 0, IPC_RMID, (union semun)0);
#endif
		  free(mutex);
	  }
}

#endif /* HasPTHREADS */
#endif /* _VISUALC_ */
