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
  NoEvents = 0x0000,
  ConfigureEvent = 0x0001,
  RenderEvent = 0x0002,
  LocaleEvent = 0x0004,
  CoderEvent = 0x0008,
  X11Event = 0x0010,
  BlobEvent = 0x0020,
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
