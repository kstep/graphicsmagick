/*
  ImageMagick Timer Methods.
*/
#ifndef _MAGICK_TIMER_H
#define _MAGICK_TIMER_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _Timer
{
  double
    start,
    stop,
    total;
} Timer;

typedef struct _TimerInfo
{
  Timer
    user,
    elapsed;

  TimerState
    state;

  unsigned long
    signature;
} TimerInfo;

/*
  Timer methods.
*/
extern MagickExport double
  GetElapsedTime(TimerInfo *),
  GetUserTime(TimerInfo *);

extern MagickExport unsigned int
  ContinueTimer(TimerInfo *);

extern MagickExport void
  GetTimerInfo(TimerInfo *),
  ResetTimer(TimerInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
