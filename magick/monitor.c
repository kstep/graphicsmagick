/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               M   M   OOO   N   N  IIIII  TTTTT   OOO   RRRR                %
%               MM MM  O   O  NN  N    I      T    O   O  R   R               %
%               M M M  O   O  N N N    I      T    O   O  RRRR                %
%               M   M  O   O  N  NN    I      T    O   O  R R                 %
%               M   M   OOO   N   N  IIIII    T     OOO   R  R                %
%                                                                             %
%                                                                             %
%                   GraphicsMagick Progress Monitor Methods                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               December 1995                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/monitor.h"

/*
  Global declarations.
*/
static MonitorHandler
  monitor_handler = (MonitorHandler) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M o n i t o r                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMonitor() calls the monitor handler method with a text string that
%  describes the task and a measure of completion.  The method returns True
%  on success otherwise False if an error is encountered, e.g. if there was a
%  user interrupt.
%
%  The format of the MagickMonitor method is:
%
%      unsigned int MagickMonitor(const char *text,
%        const ExtendedSignedIntegralType quantum,
%        const ExtendedUnsignedIntegralType span,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o quantum: The position relative to the span parameter which represents
%      how much progress has been made toward completing a task.
%
%    o span: The span relative to completing a task.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport unsigned int MagickMonitor(const char *text,
  const ExtendedSignedIntegralType quantum,
  const ExtendedUnsignedIntegralType span,ExceptionInfo *exception)
{
  unsigned int
    status;

  assert(text != (const char *) NULL);
  ProcessPendingEvents(text);
  status=True;
  if (monitor_handler != (MonitorHandler) NULL)
    status=(*monitor_handler)(text,quantum,span,exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M o n i t o r H a n d l e r                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetMonitorHandler() sets the monitor handler to the specified method
%  and returns the previous monitor handler.
%
%  The format of the SetMonitorHandler method is:
%
%      MonitorHandler SetMonitorHandler(MonitorHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: Specifies a pointer to a method to handle monitors.
%
%
*/
MagickExport MonitorHandler SetMonitorHandler(MonitorHandler handler)
{
  MonitorHandler
    previous_handler;

  previous_handler=monitor_handler;
  monitor_handler=handler;
  return(previous_handler);
}
