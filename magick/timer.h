/*
  ImageMagick Timer Methods.
*/
#ifndef _TIMER_H
#define _TIMER_H

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

#endif
