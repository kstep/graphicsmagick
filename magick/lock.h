/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                    M   M   AAA    GGGG  IIIII   CCCC                        %
%                    MM MM  A   A  G        I    C                            %
%                    M M M  AAAAA  G GGG    I    C                            %
%                    M   M  A   A  G   G    I    C                            %
%                    M   M  A   A   GGGG  IIIII   CCCC                        %
%                                                                             %
%                                                                             %
%                    Methods to Recognize Image formats                       %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                             Bob Friesenhahn                                 %
%                               March 2000                                    %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include <pthread.h>

/* Simple mutex lock */
typedef pthread_mutex_t SimpleMutexLock_t;

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

} RecursiveMutexLock_t;


extern MagickExport int
  RecursiveMutexInit(RecursiveMutexLock_t *lock),
  RecursiveMutexDestroy(RecursiveMutexLock_t *lock),
  RecursiveMutexLock(RecursiveMutexLock_t *lock),
  RecursiveMutexTryLock(RecursiveMutexLock_t *lock),
  RecursiveMutexUnLock(RecursiveMutexLock_t *lock),
  SimpleMutexInit(SimpleMutexLock_t *lock),
  SimpleMutexDestroy(SimpleMutexLock_t *lock),
  SimpleMutexLock(SimpleMutexLock_t *lock),
  SimpleMutexTryLock(SimpleMutexLock_t *lock),
  SimpleMutexUnLock(SimpleMutexLock_t *lock);

