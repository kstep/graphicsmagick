/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                 RRRR    AAA   N   N  DDDD    OOO   M   M                    %
%                 R   R  A   A  NN  N  D   D  O   O  MM MM                    %
%                 RRRR   AAAAA  N N N  D   D  O   O  M M M                    %
%                 R R    A   A  N  NN  D   D  O   O  M   M                    %
%                 R  R   A   A  N   N  DDDD    OOO   M   M                    %
%                                                                             %
%                                                                             %
%                   Methods to Generate Random Numbers                        %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              December 2001                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,            %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "random.h"
#include "signature.h"
#include "utility.h"

/*
  Global declarations.
*/
static SemaphoreInfo
  *random_semaphore = (SemaphoreInfo *) NULL;

static SignatureInfo
  *reservoir = (SignatureInfo *) NULL;

static unsigned long
  *roulette = (unsigned long *) NULL;

/*
  Forward declarations.
*/
static void
  InitializeRandomReservoir(void);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y R a n d o m R e s e r v i o r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyRandomReservoir() deallocates memory associated with the random
%  reservoir.
%
%  The format of the DestroyRandomReservoir method is:
%
%      DestroyRandomReservoir(void)
%
%
*/
MagickExport void DestroyRandomReservoir(void)
{
  AcquireSemaphoreInfo(&random_semaphore);
  if (reservoir == (SignatureInfo *) NULL)
    LiberateMemory((void **) &reservoir);
  if (roulette == (unsigned long *) NULL)
    LiberateMemory((void **) &roulette);
  DestroySemaphoreInfo(&random_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s t i l l R a n d o m E v e n t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DistillRandomEvent() distills randomness from an event and stores it
%  in the reservoir.  This method should be called before GetRandomKey()
%  and it should be called a number of times using different random events
%  (e.g. thread completion time, fine grained time-of-day clock in a
%  tight loop, keystroke timing, etc.) to build up sufficient randomness
%  in the reservoir.
%
%  The format of the DistillRandomEvent method is:
%
%      DistillRandomEvent(const unsigned char *event,const size_t length)
%
%  A description of each parameter follows:
%
%    o event: A random event.
%
%    o length: The length of the event.
%
*/
MagickExport void DistillRandomEvent(const unsigned char *event,
  const size_t length)
{
  SignatureInfo
    digest_info;

  /*
    Distill a random event.
  */
  assert(event != (const unsigned char *) NULL);
  if ((reservoir == (SignatureInfo *) NULL) ||
      (roulette == (unsigned long *) NULL))
    {
      AcquireSemaphoreInfo(&random_semaphore);
      if (reservoir == (SignatureInfo *) NULL)
        reservoir=(SignatureInfo *) AcquireMemory(sizeof(SignatureInfo));
      if (roulette == (unsigned long *) NULL)
        roulette=(unsigned long *) AcquireMemory(sizeof(unsigned long));
      LiberateSemaphoreInfo(&random_semaphore);
    }
  if ((reservoir == (SignatureInfo *) NULL) ||
      (roulette == (unsigned long *) NULL))
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToDistillRandomEvent");
  GetSignatureInfo(&digest_info);
  UpdateSignature(&digest_info,(const unsigned char *) reservoir->digest,
    sizeof(reservoir->digest));
  UpdateSignature(&digest_info,event,length);
  FinalizeSignature(&digest_info);
  memcpy(reservoir->digest,digest_info.digest,sizeof(reservoir->digest));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t R a n d o m K e y                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetRandomKey() gets a random key from the reservoir.
%
%  The format of the GetRandomKey method is:
%
%      GetRandomKey(unsigned char *key,const size_t length)
%
%  A description of each parameter follows:
%
%    o key: The key.
%
%    o length: The key length.
%
%
*/
MagickExport void GetRandomKey(unsigned char *key,const size_t length)
{
  SignatureInfo
    digest_info;

  long
    n;

  assert(key != (unsigned char *) NULL);
  if ((roulette == (unsigned long *) NULL) ||
      (reservoir == (SignatureInfo *) NULL))
    InitializeRandomReservoir();
  n=length;
  while (n > 0)
  {
    GetSignatureInfo(&digest_info);
    UpdateSignature(&digest_info,(const unsigned char *) reservoir->digest,
      sizeof(reservoir->digest));
    UpdateSignature(&digest_info,(const unsigned char *) roulette,
      sizeof(roulette));
    FinalizeSignature(&digest_info);
    (*roulette)++;
    memcpy(key,digest_info.digest,
      n < (long) sizeof(reservoir->digest) ? n : sizeof(reservoir->digest));
    n-=sizeof(reservoir->digest);
    key+=sizeof(reservoir->digest);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e R a n d o m R e s e r v i o r                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeRandomReservoir() initializes the random reservoir with entropy.
%
%  The format of the InitializeRandomReservoir method is:
%
%      InitializeRandomReservoir(void)
%
%
*/
static void InitializeRandomReservoir(void)
{
  char
    filename[MaxTextExtent];

  int
    file;

  long
    pid;

  time_t
    seconds;

  unsigned char
    random[MaxTextExtent];

  /*
    Initialize random reservoir.
  */
  (void) strcpy(filename,"magic");
  (void) tmpnam(filename);
  DistillRandomEvent((const unsigned char *) filename,MaxTextExtent);
  seconds=time(0);
  DistillRandomEvent((const unsigned char *) &seconds,sizeof(time_t));
#if HAVE_FTIME
  {
    struct timeb
      timer;

    (void) ftime(&timer);
    DistillRandomEvent((const unsigned char *) &timer.millitm,
      sizeof(unsigned short));
  }
#endif
  pid=getpid();
  DistillRandomEvent((const unsigned char *) &pid,sizeof(long));
  DistillRandomEvent((const unsigned char *) &roulette,sizeof(unsigned long *));
  file=open("/dev/random",O_RDONLY | O_BINARY,0777);
  if (file == -1)
    return;
  (void) read(file,random,MaxTextExtent);
  (void) close(file);
  DistillRandomEvent(random,MaxTextExtent);
}
