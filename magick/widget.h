
/*
  Define declarations.
*/
#define BorderOffset  4
#define DoubleClick  250

/*
  Typedef declarations.
*/
typedef struct _XWidgetInfo
{
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

  char
    *text,
    *cursor,
    *marker;
} XWidgetInfo;

/*
  X utilities routines.
*/
extern Export int
  XCommandWidget(Display *,XWindows *,char **,XEvent *),
  XConfirmWidget(Display *,XWindows *,char *,char *),
  XDialogWidget(Display *,XWindows *,char *,char *,char *),
  XMenuWidget(Display *,XWindows *,char *,char **,char *);

extern Export unsigned int
  XPreferencesWidget(Display *,XResourceInfo *,XWindows *);

extern Export void
  XColorBrowserWidget(Display *,XWindows *,char *,char *),
  XFileBrowserWidget(Display *,XWindows *,char *,char *),
  XFontBrowserWidget(Display *,XWindows *,char *,char *),
  XInfoWidget(Display *,XWindows *,char *),
  XListBrowserWidget(Display *,XWindows *,XWindowInfo *,char **,char *,char *,
    char *),
  XMonitorWidget(Display *,XWindows *,char *,const unsigned int,
    const unsigned int),
  XNoticeWidget(Display *,XWindows *,char *,char *),
  XTextViewWidget(Display *,const XResourceInfo *,XWindows *,const unsigned int,
    char *,char **);
