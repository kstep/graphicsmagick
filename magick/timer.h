/*
  ImageMagick Timer Methods.
*/
#ifndef _TIMER_H
#define _TIMER_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Utilities methods.
*/
extern MagickExport double
  GetElapsedTime(TimerInfo *),
  GetUserTime(TimerInfo *);

extern MagickExport unsigned int
  ContinueTimer(TimerInfo *);

extern MagickExport void
  GetTimerInfo(TimerInfo *),
  ResetTimer(TimerInfo *),
  StartTimer(TimerInfo *,const unsigned int),
  StopTimer(TimerInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
