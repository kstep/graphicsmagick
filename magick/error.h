/*
  Error typedef declarations.
*/
typedef void
  (*ErrorHandler)(const char *,const char *);

/*
  Enumeration declarations.
*/
typedef enum
{
  UndefinedError,
  ResourceLimitError = 400,
  XServerError = 405,
  OptionError = 410
} ErrorType;

typedef enum
{
  UndefinedWarning,
  ResourceLimitWarning = 300,
  XServerWarning = 305,
  OptionWarning = 310,
  DelegateWarning = 315,
  PluginWarning = 315,
  MissingDelegateWarning = 320,
  MissingPluginWarning = 320,
  CorruptImageWarning = 325,
  FileOpenWarning = 330
} WarningType;

/*
  Error declarations.
*/
extern Export int
  SetErrorStatus(int);

extern Export ErrorHandler
  SetErrorHandler(ErrorHandler),
  SetWarningHandler(ErrorHandler);

extern Export void
  MagickError(const int,const char *,const char *),
  MagickWarning(const int,const char *,const char *);
