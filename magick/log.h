/*
  Log methods.
*/
#ifndef _MAGICK_LOG_H
#define _MAGICK_LOG_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef enum
{ 
  UndefinedEvents = 0x0000,
  NoEvents = 0x0000,
  ConfigureEvent = 0x0001,
  AnnotateEvent = 0x0002,
  RenderEvent = 0x0004,
  LocaleEvent = 0x0008,
  CoderEvent = 0x0010,
  X11Event = 0x0020,
  CacheEvent = 0x0040,
  BlobEvent = 0x0080,
  UserEvent = 0x0100,
  AllEvents = 0xFFFF
} LogEventType;

/*
  Method declarations.
*/
extern MagickExport unsigned int
  IsEventLogging(void),
#if defined(__GNUC__)
  LogMagickEvent(const LogEventType,const char *,...)
    __attribute__((format (printf,2,3)));
#else
  LogMagickEvent(const LogEventType,const char *,...);
#endif

extern MagickExport unsigned long
  SetLogEventMask(const char *);

extern MagickExport void
  DestroyLogInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
