/*
  Log methods.
*/
#ifndef _MAGICK_LOG_H
#define _MAGICK_LOG_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


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
