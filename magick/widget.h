/*
  X11 User Interface Methods for ImageMagick.
*/
#ifndef _MAGICK_WIDGET_H
#define _MAGICK_WIDGET_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

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
  XCommandWidget(Display *,XWindows *,char const **,XEvent *),
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
  XListBrowserWidget(Display *,XWindows *,XWindowInfo *,char const **,
    const char *,const char *,char *),
  XMonitorWidget(Display *,XWindows *,const char *,const off_t,const off_t),
  XNoticeWidget(Display *,XWindows *,const char *,const char *),
  XTextViewWidget(Display *,const XResourceInfo *,XWindows *,const unsigned int,
    const char *,char const **);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
