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

#if !defined(GetMagickModule)
#  if defined(HAS___func__)
#    define GetMagickModule()  __FILE__,__func__,__LINE__
#  else
#    define GetMagickModule()  __FILE__,"unknown",__LINE__
#  endif
#endif

typedef enum
{ 
  UndefinedEvents       = 0x0000000,
  NoEvents              = 0x0000000,
  ConfigureEvent        = 0x00000001,
  AnnotateEvent         = 0x00000002,
  RenderEvent           = 0x00000004,
  TransformEvent        = 0x00000008,
  LocaleEvent           = 0x00000010,
  CoderEvent            = 0x00000020,
  X11Event              = 0x00000040,
  CacheEvent            = 0x00000080,
  BlobEvent             = 0x00000100,
  DeprecateEvent        = 0x00000200,
  UserEvent             = 0x00000400,
  ResourceEvent         = 0x00000800,
  TemporaryFileEvent    = 0x00001000,
  ExceptionEvent        = 0x00002000,
  AllEvents             = 0x7FFFFFFF
} LogEventType;

/*
  Method declarations.
*/
extern MagickExport unsigned int
  InitializeLogInfo(ExceptionInfo *exception),
  IsEventLogging(void),
#if defined(__GNUC__)
  LogMagickEvent(const LogEventType,const char *,const char *,
    const unsigned long,const char *,...) __attribute__((format (printf,5,6)));
#else
  LogMagickEvent(const LogEventType,const char *,const char *,
    const unsigned long,const char *,...);
#endif


extern MagickExport unsigned long
  SetLogEventMask(const char *);

extern MagickExport void
  DestroyLogInfo(void),
  SetLogFormat(const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
