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

#if defined(HAS___func__)
#  define GetMagickModule()  __FILE__,__func__,__LINE__
#else
#  define GetMagickModule()  __FILE__,"unknown",__LINE__
#endif

typedef enum
{ 
  UndefinedEvents = 0x0000,
  NoEvents = 0x0000,
  ConfigureEvent = 0x0001,
  AnnotateEvent = 0x0002,
  RenderEvent = 0x0004,
  TransformEvent = 0x0008,
  LocaleEvent = 0x0010,
  CoderEvent = 0x0020,
  X11Event = 0x0040,
  CacheEvent = 0x0080,
  BlobEvent = 0x0100,
  DeprecateEvent = 0x0200,
  UserEvent = 0x0400,
  ResourceEvent = 0x0800,
  TemporaryFileEvent = 0x01000,
  AllEvents = 0xFFFFF
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
