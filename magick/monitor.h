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
typedef void
  (*MonitorHandler)(const char *,const unsigned long,const unsigned long);

/*
  Monitor declarations.
*/
extern MagickExport MonitorHandler
  SetMonitorHandler(MonitorHandler);

extern MagickExport void
  MagickMonitor(const char *,const unsigned long,const unsigned long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
