/*
  Error declarations.
*/
#ifndef _ERROR_H
#define _ERROR_H

/*
  Error define definitions.
*/
#if defined(sun) && !defined(SVR4)
#if !defined(strerror)
#define strerror(n) \
  (((n) >= 0 && (n) < sys_nerr) ? sys_errlist[n] : "unknown error")

extern char
  *sys_errlist[];

extern int
  sys_nerr;
#endif
#endif

/*
  Error typedef declarations.
*/
typedef void
  (*ErrorHandler)(const unsigned int,const char *,const char *);

/*
  Enumeration declarations.
*/
typedef enum
{
  UndefinedError,
  ResourceLimitError = 400,
  XServerError = 405,
  OptionError = 410,
  DelegateError = 415,
  MissingDelegateError = 420,
  CorruptImageError = 425,
  FileOpenError = 430
} ErrorType;

typedef enum
{
  UndefinedWarning,
  ResourceLimitWarning = 300,
  XServerWarning = 305,
  OptionWarning = 310,
  DelegateWarning = 315,
  MissingDelegateWarning = 320,
  CorruptImageWarning = 325,
  FileOpenWarning = 330
} WarningType;

/*
  Error declarations.
*/
extern Export ErrorHandler
  SetErrorHandler(ErrorHandler),
  SetWarningHandler(ErrorHandler);

extern Export void
  MagickError(const unsigned int,const char *,const char *),
  MagickWarning(const unsigned int,const char *,const char *);

#endif
