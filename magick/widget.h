/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  X11 User Interface Methods for ImageMagick.
*/
#ifndef _MAGICK_WIDGET_H
#define _MAGICK_WIDGET_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  ControlState = 0x0001,
  InactiveWidgetState = 0x0004,
  JumpListState = 0x0008,
  RedrawActionState = 0x0010,
  RedrawListState = 0x0020,
  RedrawWidgetState = 0x0040,
  UpdateListState = 0x0100
} WidgetState;

/*
  Typedef declarations.
*/
typedef struct _XWidgetInfo
{
  char
    *cursor,
    *text,
    *marker;

  int
    id;

  unsigned int
    bevel_width,
    width,
    height;

  int
    x,
    y,
    min_y,
    max_y;

  unsigned int
    raised,
    active,
    center,
    trough,
    highlight;
} XWidgetInfo;

/*
  X utilities routines.
*/
extern MagickExport int
  XCommandWidget(Display *,XWindows *,const char **,XEvent *),
  XConfirmWidget(Display *,XWindows *,const char *,const char *),
  XDialogWidget(Display *,XWindows *,const char *,const char *,char *),
  XMenuWidget(Display *,XWindows *,const char *,const char **,char *);

extern MagickExport unsigned int
  XPreferencesWidget(Display *,XResourceInfo *,XWindows *);

extern MagickExport void
  XColorBrowserWidget(Display *,XWindows *,const char *,char *),
  XFileBrowserWidget(Display *,XWindows *,const char *,char *),
  XFontBrowserWidget(Display *,XWindows *,const char *,char *),
  XInfoWidget(Display *,XWindows *,const char *),
  XListBrowserWidget(Display *,XWindows *,XWindowInfo *,const char **,
    const char *,const char *,char *),
  XMonitorWidget(Display *,XWindows *,const char *,
    const ExtendedSignedIntegralType,const ExtendedUnsignedIntegralType),
  XNoticeWidget(Display *,XWindows *,const char *,const char *),
  XTextViewWidget(Display *,const XResourceInfo *,XWindows *,const unsigned int,
    const char *,const char **);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
