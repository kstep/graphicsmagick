/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      EEEEE  RRRR   RRRR   OOO   RRRR                        %
%                      E      R   R  R   R O   O  R   R                       %
%                      EEE    RRRR   RRRR  O   O  RRRR                        %
%                      E      R R    R R   O   O  R R                         %
%                      EEEEE  R  R   R  R   OOO   R  R                        %
%                                                                             %
%                                                                             %
%                       ImageMagick Exception Methods                         %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                                July 1993                                    %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
#include "define.h"

/*
  Forward declarations.
*/
static void
  DefaultErrorHandler(const ExceptionType,const char *,const char *),
  DefaultWarningHandler(const ExceptionType,const char *,const char *);

/*
  Global declarations.
*/
static ErrorHandler
  error_handler = DefaultErrorHandler;

static WarningHandler
  warning_handler = DefaultWarningHandler;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C a t c h I m a g e E x c e p t i o n                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CatchImageException returns if no exceptions are found in the
%  image sequence, otherwise it determines the most severe exception and
%  reports it as a warning or error depending on the severity.
%
%  The format of the CatchImageException method is:
%
%      CatchImageException(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a list of one or more images.
%
%
*/
MagickExport void CatchImageException(const Image *image)
{
  ExceptionInfo
    exception;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  GetExceptionInfo(&exception);
  GetImageException(image,&exception);
  if (exception.severity == UndefinedException)
    return;
  if (exception.severity >= FatalException)
    MagickError(exception.severity,exception.reason,exception.description);
  MagickWarning(exception.severity,exception.reason,exception.description);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f a u l t E r r o r H a n d l e r                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DefaultErrorHandler displays an error reason and then terminates
%  the program.
%
%  The format of the DefaultErrorHandler method is:
%
%      void MagickError(const ExceptionType error,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o exception: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
static void DefaultErrorHandler(const ExceptionType error,const char *reason,
  const char *description)
{
  if (reason == (char *) NULL)
    {
      DestroyMagick();
      Exit(error);
    }
  (void) fprintf(stderr,"%.1024s: %.1024s",SetClientName((char *) NULL),
    reason);
  if (description != (char *) NULL)
    (void) fprintf(stderr," (%.1024s)",description);
  if ((error != OptionError) && errno)
    (void) fprintf(stderr," [%.1024s]",strerror(errno));
  (void) fprintf(stderr,".\n");
  DestroyMagick();
  Exit(error);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f a u l t W a r n i n g H a n d l e r                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DefaultWarningHandler displays a warning reason.
%
%  The format of the DefaultWarningHandler method is:
%
%      void DefaultWarningHandler(const ExceptionType warning,
%        const char *reason,const char *description)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
static void DefaultWarningHandler(const ExceptionType warning,
  const char *reason,const char *description)
{
  if (reason == (char *) NULL)
    return;
  (void) fprintf(stderr,"%.1024s: %.1024s",SetClientName((char *) NULL),
    reason);
  if (description != (char *) NULL)
    (void) fprintf(stderr," (%.1024s)",description);
  if ((warning != OptionWarning) && errno)
    (void) fprintf(stderr," [%.1024s]",strerror(errno));
  (void) fprintf(stderr,".\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y E x c e p t i o n I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyExceptionInfo deallocates memory associated with an
%  ExceptionInfo structure.
%
%  The format of the DestroyExceptionInfo method is:
%
%      void DestroyExceptionInfo(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Specifies a pointer to an ExceptionInfo structure.
%
%
*/
MagickExport void DestroyExceptionInfo(ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (exception->reason != (char *) NULL)
    LiberateMemory((void **) &exception->reason);
  if (exception->description != (char *) NULL)
    LiberateMemory((void **) &exception->description);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E x c e p t i o n I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetExceptionInfo initializes the ExceptionInfo structure.
%
%  The format of the GetExceptionInfo method is:
%
%      GetExceptionInfo(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o error_info: Specifies a pointer to a ExceptionInfo structure.
%
%
*/
MagickExport void GetExceptionInfo(ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  (void) memset(exception,0,sizeof(ExceptionInfo));
  exception->severity=UndefinedException;
  exception->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e E x c e p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageException traverses an image sequence and returns any
%  error more severe than noted by the exception parameter.
%
%  The format of the GetImageException method is:
%
%      GetImageException(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a list of one or more images.
%
%    o exception: return the highest severity exception.
%
%
*/
MagickExport void GetImageException(const Image *image,ExceptionInfo *exception)
{
  register const Image
    *next;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if (next->exception.severity == UndefinedException)
      continue;
    if (next->exception.severity > exception->severity)
      ThrowException(exception,next->exception.severity,
        next->exception.reason,next->exception.description);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E r r o r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickError calls the error handler methods with an error reason.
%
%  The format of the MagickError method is:
%
%      void MagickError(const ExceptionType error,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o exception: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void MagickError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (error_handler != (ErrorHandler) NULL)
    (*error_handler)(error,reason,description);
  errno=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W a r n i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickWarning calls the warning handler methods with a warning
%  reason.
%
%  The format of the MagickWarning method is:
%
%      void MagickWarning(const ExceptionType warning,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void MagickWarning(const ExceptionType warning,const char *reason,
  const char *description)
{
  if (warning_handler != (WarningHandler) NULL)
    (*warning_handler)(warning,reason,description);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t E r r o r H a n d l e r                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetErrorHandler sets the error handler to the specified method
%  and returns the previous error handler.
%
%  The format of the SetErrorHandler method is:
%
%      ErrorHandler SetErrorHandler(ErrorHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: Specifies a pointer to a method to handle errors.
%
%
*/
MagickExport ErrorHandler SetErrorHandler(ErrorHandler handler)
{
  ErrorHandler
    previous_handler;

  previous_handler=error_handler;
  error_handler=handler;
  return(previous_handler);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t W a r n i n g H a n d l e r                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetWarningHandler sets the warning handler to the specified method
%  and returns the previous warning handler.
%
%  The format of the SetWarningHandler method is:
%
%      ErrorHandler SetWarningHandler(ErrorHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: Specifies a pointer to a method to handle warnings.
%
%
*/
MagickExport WarningHandler SetWarningHandler(WarningHandler handler)
{
  WarningHandler
    previous_handler;

  previous_handler=warning_handler;
  warning_handler=handler;
  return(previous_handler);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T h r o w E x c e p t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ThrowException throws an exception with the specified severity code,
%  reason, and optional description.
%
%  The format of the ThrowException method is:
%
%      void ThrowException(ExceptionInfo *exception,
%        const ExceptionType severity,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o exception: Specifies a pointer to the ExceptionInfo structure.
%
%    o severity: This ExceptionType declares the severity of the exception.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void ThrowException(ExceptionInfo *exception,
  const ExceptionType severity,const char *reason,const char *description)
{
  assert(exception != (ExceptionInfo *) NULL);
  if (severity < exception->severity)
    return;
  exception->severity=(ExceptionType) severity;
  (void) CloneString(&exception->reason,reason);
  (void) CloneString(&exception->description,description);
}
