/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Exception Methods.
*/
#ifndef _MAGICK_ERROR_H
#define _MAGICK_ERROR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedException,
  WarningException = 300,
  ResourceLimitWarning = 300,
  TypeWarning = 305,
  OptionWarning = 310,
  DelegateWarning = 315,
  MissingDelegateWarning = 320,
  CorruptImageWarning = 325,
  FileOpenWarning = 330,
  BlobWarning = 335,
  StreamWarning = 340,
  CacheWarning = 345,
  CoderWarning = 350,
  ModuleWarning = 355,
  DrawWarning = 360,
  ImageWarning = 365,
  XServerWarning = 380,
  MonitorWarning = 385,
  RegistryWarning = 390,
  ConfigureWarning = 395,
  ErrorException = 400,
  ResourceLimitError = 400,
  TypeError = 405,
  OptionError = 410,
  DelegateError = 415,
  MissingDelegateError = 420,
  CorruptImageError = 425,
  FileOpenError = 430,
  BlobError = 435,
  StreamError = 440,
  CacheError = 445,
  CoderError = 450,
  ModuleError = 455,
  DrawError = 460,
  ImageError = 465,
  XServerError = 480,
  MonitorError = 485,
  RegistryError = 490,
  ConfigureError = 495,
  FatalErrorException = 700,
  ResourceLimitFatalError = 700,
  TypeFatalError = 705,
  OptionFatalError = 710,
  DelegateFatalError = 715,
  MissingDelegateFatalError = 720,
  CorruptImageFatalError = 725,
  FileOpenFatalError = 730,
  BlobFatalError = 735,
  StreamFatalError = 740,
  CacheFatalError = 745,
  CoderFatalError = 750,
  ModuleFatalError = 755,
  DrawFatalError = 760,
  ImageFatalError = 765,
  XServerFatalError = 780,
  MonitorFatalError = 785,
  RegistryFatalError = 790,
  ConfigureFatalError = 795
} ExceptionType;

/*
  Typedef declarations.
*/

/*
  ExceptionInfo is used to report exceptions to higher level routines,
  and to the user.
*/
typedef struct _ExceptionInfo
{
  /*
    Exception severity, reason, and description
  */
  ExceptionType
    severity;

  char
    *reason,
    *description;

  /*
    Value of errno (or equivalent) when exception was thrown.
  */
  int
    error_number;

  /*
    Reporting source module, function (if available), and source
    module line.
  */
  char
    *module,
    *function;

  unsigned long
    line;

  /*
    Structure sanity check
  */
  unsigned long
    signature;
} ExceptionInfo;

/*
  Exception typedef declarations.
*/
typedef void
  (*ErrorHandler)(const ExceptionType,const char *,const char *);

typedef void
  (*FatalErrorHandler)(const ExceptionType,const char *,const char *);

typedef void
  (*WarningHandler)(const ExceptionType,const char *,const char *);

/*
  Exception declarations.
*/
extern MagickExport const char
  *GetLocaleExceptionMessage(const ExceptionType,const char *),
  *GetLocaleMessage(const char *);

extern MagickExport ErrorHandler
  SetErrorHandler(ErrorHandler);

extern MagickExport FatalErrorHandler
  SetFatalErrorHandler(FatalErrorHandler);

extern MagickExport void
  CatchException(const ExceptionInfo *),
  CopyException(ExceptionInfo *copy, const ExceptionInfo *original),
  DestroyExceptionInfo(ExceptionInfo *),
  GetExceptionInfo(ExceptionInfo *),
  MagickError(const ExceptionType,const char *,const char *),
  MagickFatalError(const ExceptionType,const char *,const char *),
  MagickWarning(const ExceptionType,const char *,const char *),
  SetExceptionInfo(ExceptionInfo *,ExceptionType),
  ThrowException(ExceptionInfo *,const ExceptionType,const char *,const char *),
  ThrowLoggedException(ExceptionInfo *exception, const ExceptionType severity,
    const char *reason,const char *description,const char *module,
    const char *function,const unsigned long line);

extern MagickExport WarningHandler
  SetWarningHandler(WarningHandler);

/*
  Exception define definitions.
*/

#include <magick/log.h>

#if defined(macintosh)
#define ExceptionInfo MagickExceptionInfo
#endif

#if defined(MAGICK_IMPLEMENTATION)
#define ThrowException(exception_,severity_,reason_,description_) \
  (ThrowLoggedException(exception_,severity_,reason_,description_,GetMagickModule()))
#endif

#define ThrowBinaryException(severity_,reason_,description_) \
{ \
  if (image != (Image *) NULL) \
    { \
      ThrowException(&image->exception,severity_,reason_,description_); \
    } \
  return(False); \
}
#define ThrowImageException(code_,reason_,description_) \
{ \
  ThrowException(exception,code_,reason_,description_); \
  return((Image *) NULL); \
}
#define ThrowReaderException(code_,reason_,image_) \
{ \
  if ((image) == (Image *) NULL) \
    { \
      ThrowException(exception,code_,reason_,(char *) NULL); \
    } \
  else \
    { \
      ThrowException(exception,code_,reason_,(image)->filename); \
      if (image->blob->type != UndefinedStream) \
        CloseBlob(image); \
      DestroyImageList(image); \
    } \
  return((Image *) NULL); \
}
#define ThrowWriterException(code_,reason_,image_) \
{ \
  assert(image_ != (Image *) NULL); \
  ThrowException(&(image_)->exception,code_,reason_,(image_)->filename); \
  if (image_info->adjoin) \
    while ((image_)->previous != (Image *) NULL) \
      (image_)=(image_)->previous; \
  if (image_->blob->type != UndefinedStream) \
    CloseBlob(image_); \
  return(False); \
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
