#undef False
#undef True
#define XLIB_ILLEGAL_ACCESS  1
#if !defined(macintosh)
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#if !defined(vms)
#include <X11/XWDFile.h>
#else
#include "XWDFile.h"
#endif
#else
#include <Xos.h>
#include <Xlib.h>
#include <Xutil.h>
#include <Xresource.h>
#include <Xproto.h>
#include <Xatom.h>
#include <cursorfont.h>
#include <keysym.h>
#include <XWDFile.h>
#endif
#if defined(HasShape)
#include <X11/extensions/shape.h>
#endif
#if defined(HasSharedMemory)
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif
#undef index
#if defined(_AIX) || defined(hpux9)
#define XFD_SET  int
#else
#define XFD_SET  fd_set
#endif
/*
  Define declarations.
*/
#define MaxNumberFonts  11
#define MaxNumberPens  11 
#define SuspendTime  50
#define XGammaPixel(map,gamma,color,dx)  (unsigned long) (map->base_pixel+ \
  ((gamma[(color)->red].red*map->red_max+(1 << (dx-1)))/((1 << dx)-1))* \
    map->red_mult+ \
  ((gamma[(color)->green].green*map->green_max+(1 << (dx-1)))/((1 << dx)-1))* \
    map->green_mult+ \
  ((gamma[(color)->blue].blue*map->blue_max+(1 << (dx-1)))/((1 << dx)-1))* \
    map->blue_mult)
#define XStandardPixel(map,color,dx)  (unsigned long) (map->base_pixel+ \
  (((color).red*map->red_max+(1 << (dx-1)))/((1 << dx)-1))*map->red_mult+ \
  (((color).green*map->green_max+(1 << (dx-1)))/((1 << dx)-1))*map->green_mult+\
  (((color).blue*map->blue_max+(1 << (dx-1)))/((1 << dx)-1))*map->blue_mult)

/*
  Default colors declarations.
*/
#define BackgroundColor  "#bdbdbd"  /* gray */
#define BorderColor  "#bdbdbd"  /* gray */
#define ForegroundColor  "#000"  /* black */
#define MatteColor  "#bdbdbd"  /* gray */
#define Pen0Color  "#bdbdbd"  /* gray */
#define Pen1Color  "#000000"  /* black */
#define Pen2Color  "#0000ff"  /* blue */
#define Pen3Color  "#00ffff"  /* cyan */
#define Pen4Color  "#00ff00"  /* green */
#define Pen5Color  "#bdbdbd"  /* gray */
#define Pen6Color  "#ff0000"  /* red */
#define Pen7Color  "#ff00ff"  /* magenta */
#define Pen8Color  "#ffff00"  /* yellow */
#define Pen9Color  "#ffffff"  /* white */

/*
  Display state declarations.
*/
#define DefaultState  0x0000
#define EscapeState  0x0001
#define ExitState  0x0002
#define FormerImageState  0x0004
#define ModifierState  0x0008
#define MontageImageState  0x0010
#define NextImageState  0x0020
#define RetainColorsState  0x0040
#define UpdateConfigurationState  0x0080
#define UpdateRegionState  0x0100
/*
  Animate state declarations.
*/
#define AutoReverseAnimationState 0x0004
#define ForwardAnimationState 0x0008
#define HighlightState  0x0010
#define PlayAnimationState 0x0020
#define RepeatAnimationState 0x0040
#define StepAnimationState 0x0080

/*
  Enumeration declarations.
*/
typedef enum
{
  ForegroundStencil,
  BackgroundStencil,
  OpaqueStencil,
  TransparentStencil
} AnnotationStencil;

typedef enum
{
  UndefinedMode,
  FrameMode,
  UnframeMode,
  ConcatenateMode
} MontageMode;

typedef enum
{
  UndefinedColormap,
  PrivateColormap,
  SharedColormap
} XColormapType;

/*
  Typedef declarations.
*/
typedef struct _DiversityPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned long
    count;
} DiversityPacket;

typedef struct _XAnnotateInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  AnnotationStencil
    stencil;

  double
    degrees;

  XFontStruct
    *font_info;

  char
    *text,
    geometry[MaxTextExtent];

  struct _XAnnotateInfo
    *previous,
    *next;
} XAnnotateInfo;

typedef struct _XDrawInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  char
    geometry[MaxTextExtent];

  double
    degrees;

  AnnotationStencil
    stencil;

  PrimitiveType
    primitive;

  Pixmap
    stipple;

  unsigned int
    line_width;

  XSegment
    line_info;

  RectangleInfo
    rectangle_info;

  unsigned int
    number_coordinates;

  XPoint
    *coordinate_info;
} XDrawInfo;

typedef struct _XImportInfo
{
  unsigned int
    frame,
    borders,
    screen,
    descend,
    silent;
} XImportInfo;

typedef struct _XMontageInfo
{
  char
    filename[MaxTextExtent];

  unsigned int
    shadow;

  CompositeOperator
    compose;

  unsigned int
    pointsize;
 
  char
    *frame,
    *tile,
    *texture;
} XMontageInfo;

typedef struct _XPixelInfo
{
  unsigned int
    colors;

  unsigned long
    *pixels;

  XColor
    foreground_color,
    background_color,
    border_color,
    matte_color,
    highlight_color,
    shadow_color,
    depth_color,
    trough_color,
    box_color,
    pen_color,
    pen_colors[MaxNumberPens],
    *gamma_map;

  unsigned short
    box_index,
    pen_index;

  GC
    annotate_context,
    highlight_context,
    widget_context;
} XPixelInfo;

typedef struct _XResourceInfo
{
  XrmDatabase
    resource_database;

  ImageInfo
    image_info;

  QuantizeInfo
    quantize_info;

  unsigned int
    colors;

  unsigned int
    close_server,
    backdrop;

  char
    *background_color,
    *border_color;

  unsigned int
    border_width;

  char
    *browse_command,
    *client_name;

  XColormapType
    colormap;

  unsigned int
    color_recovery;

  unsigned int
    confirm_exit,
    debug,
    delay;

  char
    *display_gamma;

  unsigned int
    display_warnings;

  char
    *editor_command,
    *font,
    *font_name[MaxNumberFonts],
    *foreground_color;

  unsigned int
    gamma_correct;

  int
    gravity;

  char
    home_directory[MaxTextExtent],
    *icon_geometry;

  unsigned int
    iconic,
    immutable;

  char
    *image_geometry,
    *launch_command;

  unsigned int
    magnify;

  char
    *map_type,
    *matte_color;

  char
    *name;

  unsigned int
    pause;

  char
    *pen_colors[MaxNumberPens],
    *print_command;

  int
    quantum;

  char
    *text_font,
    *title;

  unsigned int
    update,
    undo_cache,
    use_pixmap,
    use_shared_memory;

  char
    *visual_type,
    *window_group,
    *window_id,
    *write_filename;
} XResourceInfo;

typedef struct _XWindowInfo
{
  Window
    id;

  int
    screen;

  Window
    root;

  Visual
    *visual;

  int
    class,
    depth;

  XVisualInfo
    *visual_info;

  XStandardColormap
    *map_info;

  XPixelInfo
    *pixel_info;

  XFontStruct
    *font_info;

  GC
    annotate_context,
    highlight_context,
    widget_context;

  Cursor
    cursor,
    busy_cursor;

  char
    *name,
    *geometry,
    *icon_name,
    *icon_geometry,
    *crop_geometry;

  unsigned int
    data;

  unsigned long
    flags;

  int
    x,
    y;

  unsigned int
    width,
    height,
    min_width,
    min_height,
    width_inc,
    height_inc,
    border_width,
    use_pixmap,
    immutable,
    shape,
    shared_memory;

  XImage
    *ximage,
    *matte_image;

  Pixmap
    highlight_stipple,
    shadow_stipple,
    pixmap,
    matte_pixmap,
    *pixmaps;

  int
    mask;

  XSetWindowAttributes
    attributes;

  XWindowChanges
    window_changes;

#if defined(HasSharedMemory)
  XShmSegmentInfo
    segment_info[2];
#endif

  unsigned int
    orphan,
    mapped,
    stasis;
} XWindowInfo;

typedef struct _XWindows
{
  Display
    *display;

  XStandardColormap
    *map_info,
    *icon_map;

  XVisualInfo
    *visual_info,
    *icon_visual;

  XPixelInfo
    *pixel_info,
    *icon_pixel;

  XFontStruct
    *font_info;

  XResourceInfo
    *icon_resources;

  XClassHint
    *class_hints;

  XWMHints
    *manager_hints;

  XWindowInfo
    context,
    group_leader,
    backdrop,
    icon,
    image,
    info,
    magnify,
    pan,
    command,
    widget,
    popup;

  Atom
    wm_protocols,
    wm_delete_window,
    wm_take_focus,
    im_protocols,
    im_remote_command,
    im_update_widget,
    im_update_colormap,
    im_update_signature,
    im_former_image,
    im_retain_colors,
    im_next_image,
    im_exit,
    dnd_protocols;
} XWindows;

/*
  X utilities routines.
*/
extern Export char
  *XGetResourceClass(XrmDatabase,const char *,const char *,char *),
  *XGetResourceInstance(XrmDatabase,const char *,const char *,char *),
  *XGetScreenDensity(Display *),
  *XVisualClassName(const int);

extern Export Cursor
  XMakeCursor(Display *,Window,Colormap,char *,char *);

extern Export Image
  *XAnimateImages(Display *,XResourceInfo *,char **,const int,Image *),
  *XDisplayImage(Display *,XResourceInfo *,char **,int,Image **,
    unsigned long *),
  *XMontageImages(const XResourceInfo *,XMontageInfo *,Image *),
  *ReadXImage(ImageInfo *,XImportInfo *),
  *XGetWindowImage(Display *,const Window,const unsigned int,
    const unsigned int);

extern Export int
  Latin1Compare(const char *,const char *),
  XError(Display *,XErrorEvent *);

extern Export unsigned int
  IsTrue(const char *),
  XAnnotateImage(Display *,const XPixelInfo *,XAnnotateInfo *,Image *),
  XDisplayBackgroundImage(Display *,XResourceInfo *,Image *),
  XDrawImage(Display *,const XPixelInfo *,XDrawInfo *,Image *),
  XGetWindowColor(Display *,char *),
  XMakeImage(Display *,const XResourceInfo *,XWindowInfo *,Image *,
    unsigned int,unsigned int),
  XMakePixmap(Display *,const XResourceInfo *,XWindowInfo *),
  XQueryColorDatabase(char *,XColor *);

extern Export void
  XAnimateBackgroundImage(Display *,XResourceInfo *,Image *),
  XBestIconSize(Display *,XWindowInfo *,Image *),
  XBestPixel(Display *,const Colormap,XColor *,unsigned int,XColor *),
  XCheckRefreshWindows(Display *,XWindows *),
  XClientMessage(Display *,const Window,const Atom,const Atom,const Time),
  XConfigureImageColormap(Display *,XResourceInfo *,XWindows *,Image *),
  XConstrainWindowPosition(Display *,XWindowInfo *),
  XDelay(Display *,const unsigned long),
  XDestroyWindowColors(Display *,Window),
  XDisplayImageInfo(Display *,const XResourceInfo *,XWindows *,Image *,Image *),
  XFreeResources(Display *,XVisualInfo *,XStandardColormap *,XPixelInfo *,
    XFontStruct *,XResourceInfo *,XWindowInfo *),
  XFreeStandardColormap(Display *,const XVisualInfo *,XStandardColormap *,
    XPixelInfo *),
  XGetAnnotateInfo(XAnnotateInfo *),
  XGetImportInfo(XImportInfo *),
  XGetMapInfo(const XVisualInfo *,const Colormap,XStandardColormap *),
  XGetMontageInfo(XMontageInfo *),
  XGetPixelInfo(Display *,const XVisualInfo *,const XStandardColormap *,
    const XResourceInfo *,Image *,XPixelInfo *),
  XGetResourceInfo(XrmDatabase,char *,XResourceInfo *),
  XGetWindowInfo(Display *,XVisualInfo *,XStandardColormap *,XPixelInfo *,
    XFontStruct *,XResourceInfo *,XWindowInfo *),
  XHighlightEllipse(Display *,Window,GC,const RectangleInfo *),
  XHighlightLine(Display *,Window,GC,const XSegment *),
  XHighlightRectangle(Display *,Window,GC,const RectangleInfo *),
  XMakeMagnifyImage(Display *,XWindows *),
  XMakeStandardColormap(Display *,XVisualInfo *,XResourceInfo *,Image *,
    XStandardColormap *,XPixelInfo *),
  XMakeWindow(Display *,Window,char **,int,XClassHint *,XWMHints *,
    XWindowInfo *),
  XProgressMonitor(char *,const unsigned int,const unsigned int),
  XQueryPosition(Display *,const Window,int *,int *),
  XRefreshWindow(Display *,const XWindowInfo *,const XEvent *),
  XRemoteCommand(Display *,const char *,const char *),
  XRetainWindowColors(Display *,const Window),
  XUserPreferences(XResourceInfo *),
  XSetCursorState(Display *,XWindows *,const unsigned int),
  XSignalHandler(int),
  XWarning(const char *,const char *);

extern Export Window
  XClientWindow(Display *,Window),
  XGetSubwindow(Display *,Window,int,int),
  XSelectWindow(Display *,RectangleInfo *),
  XWindowByID(Display *,const Window,const unsigned long),
  XWindowByName(Display *,const Window,const char *),
  XWindowByProperty(Display *,const Window,const Atom);

extern Export XFontStruct
  *XBestFont(Display *,const XResourceInfo *,const unsigned int);

extern Export XrmDatabase
  XGetResourceDatabase(Display *,char *);

extern Export XVisualInfo
  *XBestVisualInfo(Display *,XStandardColormap *,XResourceInfo *);

extern Export XWindows
  *XInitializeWindows(Display *,XResourceInfo *),
  *XSetWindows(XWindows *);

/*
  Invoke pre-X11R6 ICCCM routines if XlibSpecificationRelease is not 6.
*/
#if XlibSpecificationRelease < 6
#if !defined(PRE_R6_ICCCM)
#define PRE_R6_ICCCM
#endif
#endif
/*
  Invoke pre-X11R5 ICCCM routines if XlibSpecificationRelease is not defined.
*/
#if !defined(XlibSpecificationRelease)
#define PRE_R5_ICCCM
#endif
/*
  Invoke pre-X11R4 ICCCM routines if PWinGravity is not defined.
*/
#if !defined(PWinGravity)
#define PRE_R4_ICCCM
#endif
#include "PreRvIcccm.h"
