/*
  ImageMagick Progress Monitor Methods.
*/
#ifndef _MAGICK_MONITOR_H
#define _MAGICK_MONITOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Monitor typedef declarations.
*/
typedef unsigned int
  (*MonitorHandler)(const char *,const ExtendedSignedIntegralType,const size_t,
    ExceptionInfo *);

/*
  Monitor declarations.
*/
extern MagickExport MonitorHandler
  SetMonitorHandler(MonitorHandler);

extern MagickExport unsigned int
  MagickMonitor(const char *,const ExtendedSignedIntegralType,const size_t,
    ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
