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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include "studio.h"
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
%  DistillRandomEvent() distills randomness from an event and stores in
%  in the reservoir.  This method must be called before GetRandomKey()
%  and it should be called a number of times using different random events
%  (e.g. thread completion time, fine grained time-of-day clock in a
%  tight loop, keystroke timing, etc.) to build up sufficient randomness
%  in the reservoir.
%
%  The format of the DistillRandomEvent method is:
%
%      DistillRandomEvent(const unsigned char *event,const size_t length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o event: A random event.
%
%    o length: The length of the event.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void DistillRandomEvent(const unsigned char *event,
  const size_t length,ExceptionInfo *exception)
{
  SignatureInfo
    digest_info;

  /*
    Distill a random event.
  */
  assert(event != (const unsigned char *) NULL);
  AcquireSemaphoreInfo(&random_semaphore);
  if (reservoir == (SignatureInfo *) NULL)
    reservoir=(SignatureInfo *) AcquireMemory(sizeof(SignatureInfo));
  if (roulette == (unsigned long *) NULL)
    roulette=(unsigned long *) AcquireMemory(sizeof(unsigned long));
  if ((reservoir == (SignatureInfo *) NULL) ||
      (roulette == (unsigned long *) NULL))
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToDistillRandomEvent");
  GetSignatureInfo(&digest_info);
  UpdateSignature(&digest_info,(unsigned char *) reservoir->digest,
    sizeof(reservoir->digest));
  UpdateSignature(&digest_info,event,length);
  FinalizeSignature(&digest_info);
  memcpy(reservoir->digest,digest_info.digest,sizeof(reservoir->digest));
  LiberateSemaphoreInfo(&random_semaphore);
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
%      unsigned int GetRandomKey(unsigned char *key,const size_t length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o key: The key.
%
%    o length: The key length.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport unsigned int GetRandomKey(unsigned char *key,const size_t length,
  ExceptionInfo *exception)
{
  SignatureInfo
    digest_info;

  long
    n;

  assert(key != (unsigned char *) NULL);
  if ((roulette == (unsigned long *) NULL) ||
      (reservoir == (SignatureInfo *) NULL))
    {
      char
        filename[MaxTextExtent];

      int
        file;

      pid_t
        pid;

      time_t
        seconds;

      /*
        Initialize random reservoir.
      */
      (void) strcpy(filename,"magic");
      (void) tmpnam(filename);
      DistillRandomEvent((const unsigned char *) filename,MaxTextExtent,
        exception);
      seconds=time(0);
      DistillRandomEvent((const unsigned char *) &seconds,sizeof(time_t),
        exception);
      pid=getpid();
      DistillRandomEvent((const unsigned char *) &pid,sizeof(pid_t),exception);
      DistillRandomEvent((const unsigned char *) *roulette,
        sizeof(unsigned long *),exception);
      file=open("/dev/random",O_RDONLY | O_BINARY,0777);
      if (file != -1)
        {
          unsigned char
            random[MaxTextExtent];

          (void) read(file,random,MaxTextExtent);
          (void) close(file);
          DistillRandomEvent(random,MaxTextExtent,exception);
        }
    }
  n=length;
  while (n > 0)
  {
    GetSignatureInfo(&digest_info);
    UpdateSignature(&digest_info,(unsigned char *) reservoir->digest,
      sizeof(reservoir->digest));
    UpdateSignature(&digest_info,(unsigned char *) roulette,sizeof(roulette));
    FinalizeSignature(&digest_info);
    (*roulette)++;
    memcpy(key,digest_info.digest,
      n < (long) sizeof(reservoir->digest) ? n : sizeof(reservoir->digest));
    n-=sizeof(reservoir->digest);
    key+=sizeof(reservoir->digest);
  }
  return(True);
}
