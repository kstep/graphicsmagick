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
  UndefinedEventMask = 0x0000,
  NoEventsMask = 0x0000,
  ConfigureEventMask = 0x0001,
  AnnotateEventMask = 0x0002,
  RenderEventMask = 0x0004,
  TransformEventMask = 0x0008,
  LocaleEventMask = 0x0010,
  CoderEventMask = 0x0020,
  X11EventMask = 0x0040,
  CacheEventMask = 0x0080,
  BlobEventMask = 0x0100,
  DeprecateEventMask = 0x0200,
  UserEventMask = 0x0400,
  ResourceEventMask = 0x0800,
  TemporaryFileEventMask = 0x01000,
  ExceptionEventMask = 0x02000,
  OptionEventMask = 0x04000,
  AllEventsMask = 0x7FFFFFFF
} LogEventType;

/*
  Method declarations.
*/
extern MagickExport unsigned int
  InitializeLogInfo(ExceptionInfo *exception),
  IsEventLogging(void),
#if defined(__GNUC__)
  LogMagickEvent(const ExceptionType,const char *,const char *,
    const unsigned long,const char *,...) __attribute__((format (printf,5,6)));
#else
  LogMagickEvent(const ExceptionType,const char *,const char *,
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
