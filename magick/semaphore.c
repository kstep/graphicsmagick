/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%        SSSSS  EEEEE  M   M   AAA   PPPP   H   H   OOO   RRRR   EEEEE        %
%        SS     E      MM MM  A   A  P   P  H   H  O   O  R   R  E            %
%         SSS   EEE    M M M  AAAAA  PPPP   HHHHH  O   O  RRRR   EEE          %
%           SS  E      M   M  A   A  P      H   H  O   O  R R    E            %
%        SSSSS  EEEEE  M   M  A   A  P      H   H   OOO   R  R   EEEEE        %
%                                                                             %
%                                                                             %
%                     ImageMagick Semaphore Methods                           %
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
#if defined(HasPTHREADS)
#include <pthread.h>
#endif
#if defined(_VISUALC_)
#include <windows.h>
#endif

struct SemaphoreInfo
{
#if defined(HasPTHREADS)
  pthread_mutex_t
    id;
#endif

#if defined(_VISUALC_)
  HANDLE
    id;
#endif

  unsigned int
    signature;
};

#if defined(_MT)
#define MAXSEMLEN  1
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e S e m a p h o r e I n f o                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AllocateSemaphoreInfo initializes the SemaphoreInfo structure.
%
%  The format of the AllocateSemaphoreInfo method is:
%
%      SemaphoreInfo *AllocateSemaphoreInfo(void)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Method AllocateSemaphoreInfo returns a pointer to an
%      initialized SemaphoreInfo structure.
%
%
*/
MagickExport SemaphoreInfo *AllocateSemaphoreInfo(void)
{
  SemaphoreInfo
    *semaphore_info;

  /*
    Allocate semaphore.
  */
  semaphore_info=(SemaphoreInfo *) AcquireMemory(sizeof(semaphore_info));
  if (semaphore_info == (SemaphoreInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate semaphore_info info",
      "Memory allocation failed");
#if defined(_VISUALC_) && defined(_MT)
  {
    SECURITY_ATTRIBUTES
      security;

    /*
      Create the semaphore, with initial value signaled.
    */
    security.nLength=sizeof(security);
    security.lpSecurityDescriptor=NULL;
    security.bInheritHandle=TRUE;
    semaphore_info->id=CreateSemaphore(&security,1,MAXSEMLEN,NULL);
    if (semaphore_info->id == NULL)
      {
        LiberateMemory((void **) &semaphore_info);
        return((SemaphoreInfo *) NULL);
      }
    }
#endif
#if defined(HasPTHREADS)
    if (pthread_mutex_init(&semaphore_info->id,NULL) != 0)
      {
        LiberateMemory((void **) &semaphore_info);
        return((SemaphoreInfo *) NULL);
      }
#endif
  semaphore_info->signature=MagickSignature;
  return(semaphore_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y S e m a p h o r e I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroySemaphore destroys a semaphore_info.
%
%  The format of the DestroySemaphore method is:
%
%      DestroySemaphore(SemaphoreInfo *semaphore_info)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport void DestroySemaphoreInfo(SemaphoreInfo *semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo *) NULL);
  assert(semaphore_info->signature == MagickSignature);
  if (semaphore_info == (SemaphoreInfo *) NULL)
    return;
#if defined(_VISUALC_) && defined(_MT)
  CloseHandle(semaphore_info->id);
#endif
#if defined(HasPTHREADS)
  pthread_mutex_destroy(&semaphore_info->id);
#endif
  LiberateMemory((void **) &semaphore_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c k S e m a p h o r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LockSemaphore locks a semaphore_info.
%
%  The format of the LockSemaphore method is:
%
%      int LockSemaphore(SemaphoreInfo *semaphore_info)
%
%  A description of each parameter follows:
%
%    o status:  Method LockSemaphore returns True on success otherwise
%      False.
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport int LockSemaphore(SemaphoreInfo *semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo *) NULL);
  assert(semaphore_info->signature == MagickSignature);
#if defined(_VISUALC_) && defined(_MT)
  if (WaitForSingleObject(semaphore_info->id,INFINITE) == WAIT_FAILED)
    return(False);
#endif
#if defined(HasPTHREADS)
  if (pthread_mutex_lock(&semaphore_info->id) != 0)
    return(False);
#endif
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n l o c k S e m a p h o r e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnlockSemaphore unlocks a semaphore_info.
%
%  The format of the LockSemaphore method is:
%
%      int UnlockSemaphore(SemaphoreInfo *semaphore_info)
%
%  A description of each parameter follows:
%
%    o status:  Method UnlockSemaphore returns True on success otherwise
%      False.
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport int UnlockSemaphore(SemaphoreInfo *semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo *) NULL);
  assert(semaphore_info->signature == MagickSignature);
#if defined(_VISUALC_) && defined(_MT)
  if (ReleaseSemaphore(semaphore_info->id,1,NULL) == FALSE)
    return(False);
#endif
#if defined(HasPTHREADS)
  if (pthread_mutex_unlock(&semaphore_info->id) != 0)
    return(-1);
#endif
  return(True);
}
