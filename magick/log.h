/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Log methods.
*/
#ifndef _MAGICK_LOG_H
#define _MAGICK_LOG_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(__GNUC__) && !defined(__attribute__)
#  define __attribute__(x) /*nothing*/
#endif

#if !defined(GetMagickModule)
#  if defined(HAS___func__)
#    define GetMagickModule()  __FILE__,__func__,__LINE__
#  else
#    if defined(_VISUALC_)
#      define GetMagickModule()  __FILE__,__FUNCTION__,__LINE__
#    else
#      define GetMagickModule()  __FILE__,"unknown",__LINE__
#    endif
#  endif
#endif

/* NOTE: any changes to this effect PerlMagick */
typedef enum
{ 
  UndefinedEventMask     = 0x00000000,
  NoEventsMask           = 0x00000000,
  ConfigureEventMask     = 0x00000001,
  AnnotateEventMask      = 0x00000002,
  RenderEventMask        = 0x00000004,
  TransformEventMask     = 0x00000008,
  LocaleEventMask        = 0x00000010,
  CoderEventMask         = 0x00000020,
  X11EventMask           = 0x00000040,
  CacheEventMask         = 0x00000080,
  BlobEventMask          = 0x00000100,
  DeprecateEventMask     = 0x00000200,
  UserEventMask          = 0x00000400,
  ResourceEventMask      = 0x00000800,
  TemporaryFileEventMask = 0x00001000,
  ExceptionEventMask     = 0x00002000,
  OptionEventMask        = 0x00004000,
  InformationEventMask   = 0x00008000,
  WarningEventMask       = 0x00010000,
  ErrorEventMask         = 0x00020000,
  FatalErrorEventMask    = 0x00040000,
  AllEventsMask          = 0x7FFFFFFF
} LogEventType;

/*
  Method declarations.
*/
extern MagickExport unsigned int
  InitializeLogInfo(ExceptionInfo *exception),
  IsEventLogging(void),
  LogMagickEvent(const ExceptionType,const char *,const char *,
    const unsigned long,const char *,...) __attribute__((format (printf,5,6)));

extern MagickExport unsigned long
  SetLogEventMask(const char *);

extern MagickExport void
  DestroyLogInfo(void),
  SetLogFormat(const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
