/*
  Log methods.
*/
#ifndef _MAGICK_LOG_H
#define _MAGICK_LOG_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
# define GNUC_Prerequiste(major,minor) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((major) << 16) + (minor))
#else
# define GNUC_Prerequiste(major,minor)  0
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 199901L)
# define GetMagickModule(severity)  severity,__FILE__,__func__,__LINE__
#else
# if defined(__cplusplus) ? GNUC_Prerequiste(2,6) : GNUC_Prerequiste(2,4)
#  define GetMagickModule(severity)  \
     severity,__FILE,__PRETTY_FUNCTION__,__LINE__
# else
#  define GetMagickModule(severity)  severity,__FILE__,"unknown",__LINE__
# endif
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
  LogMagickFormat(const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
