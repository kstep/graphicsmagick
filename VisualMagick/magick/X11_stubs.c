/* $XConsortium: Xlib.h,v 11.237 94/09/01 18:44:49 kaleb Exp $ */
/* 

Copyright (c) 1985, 1986, 1987, 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/


/*
 *	Xlib.h - Header definition and support file for the C subroutine
 *	interface library (Xlib) to the X Window System Protocol (V11).
 *	Structures and symbols starting with "_" are private to the library.
 */
#define XlibSpecificationRelease 6

#include <stdlib.h>
#if !defined(WIN32)
#include <unistd.h>
#else
#if defined(_VISUALC_)
#	if defined(_MT) && defined(_DLL)
#		define Export __declspec(dllexport)
#	else
#		define Export
#	endif
#pragma warning(disable : 4035)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)
#else
#	define Export
#endif
#endif

#ifdef USG
#ifndef __TYPES__
#include <sys/types.h>			/* forgot to protect it... */
#define __TYPES__
#endif /* __TYPES__ */
#else
#if defined(_POSIX_SOURCE) && defined(MOTOROLA)
#undef _POSIX_SOURCE
#include <sys/types.h>
#define _POSIX_SOURCE
#else
#ifdef __MWERKS__
#include <types.h>
#else
#include <sys/types.h>
#endif
#endif
#endif /* USG */

#ifdef __MWERKS__
#   include <X.h>
#   define Cursor XCursor
#   define cursor xcursor
#   define Region XRegion
#else
#   include <X11/X.h>
#endif

/* applications should not depend on these two headers being included! */
#ifdef __MWERKS__
#include <Xfuncproto.h>
#include <Xosdefs.h>
#else
#include <X11/Xfuncproto.h>
#include <X11/Xosdefs.h>
#endif

#ifndef X_WCHAR
#ifdef X_NOT_STDC_ENV
#define X_WCHAR
#endif
#endif

#ifndef X_WCHAR
#include <stddef.h>
#else
/* replace this with #include or typedef appropriate for your system */
typedef unsigned long wchar_t;
#endif

typedef char *XPointer;

#define Bool int
#define Status int
#define True 1
#define False 0

#define QueuedAlready 0
#define QueuedAfterReading 1
#define QueuedAfterFlush 2

#define ConnectionNumber(dpy) 	(((_XPrivDisplay)dpy)->fd)
#define RootWindow(dpy, scr) 	(ScreenOfDisplay(dpy,scr)->root)
#define DefaultScreen(dpy) 	(((_XPrivDisplay)dpy)->default_screen)
#define DefaultRootWindow(dpy) 	(ScreenOfDisplay(dpy,DefaultScreen(dpy))->root)
#define DefaultVisual(dpy, scr) (ScreenOfDisplay(dpy,scr)->root_visual)
#define DefaultGC(dpy, scr) 	(ScreenOfDisplay(dpy,scr)->default_gc)
#define BlackPixel(dpy, scr) 	(ScreenOfDisplay(dpy,scr)->black_pixel)
#define WhitePixel(dpy, scr) 	(ScreenOfDisplay(dpy,scr)->white_pixel)
#define AllPlanes 		((unsigned long)~0L)
#define QLength(dpy) 		(((_XPrivDisplay)dpy)->qlen)
#define DisplayWidth(dpy, scr) 	(ScreenOfDisplay(dpy,scr)->width)
#define DisplayHeight(dpy, scr) (ScreenOfDisplay(dpy,scr)->height)
#define DisplayWidthMM(dpy, scr)(ScreenOfDisplay(dpy,scr)->mwidth)
#define DisplayHeightMM(dpy, scr)(ScreenOfDisplay(dpy,scr)->mheight)
#define DisplayPlanes(dpy, scr) (ScreenOfDisplay(dpy,scr)->root_depth)
#define DisplayCells(dpy, scr) 	(DefaultVisual(dpy,scr)->map_entries)
#define ScreenCount(dpy) 	(((_XPrivDisplay)dpy)->nscreens)
#define ServerVendor(dpy) 	(((_XPrivDisplay)dpy)->vendor)
#define ProtocolVersion(dpy) 	(((_XPrivDisplay)dpy)->proto_major_version)
#define ProtocolRevision(dpy) 	(((_XPrivDisplay)dpy)->proto_minor_version)
#define VendorRelease(dpy) 	(((_XPrivDisplay)dpy)->release)
#define DisplayString(dpy) 	(((_XPrivDisplay)dpy)->display_name)
#define DefaultDepth(dpy, scr) 	(ScreenOfDisplay(dpy,scr)->root_depth)
#define DefaultColormap(dpy, scr)(ScreenOfDisplay(dpy,scr)->cmap)
#define BitmapUnit(dpy) 	(((_XPrivDisplay)dpy)->bitmap_unit)
#define BitmapBitOrder(dpy) 	(((_XPrivDisplay)dpy)->bitmap_bit_order)
#define BitmapPad(dpy) 		(((_XPrivDisplay)dpy)->bitmap_pad)
#define ImageByteOrder(dpy) 	(((_XPrivDisplay)dpy)->byte_order)
#ifdef CRAY /* unable to get WORD64 without pulling in other symbols */
#define NextRequest(dpy)	XNextRequest(dpy)
#else
#define NextRequest(dpy)	(((_XPrivDisplay)dpy)->request + 1)
#endif
#define LastKnownRequestProcessed(dpy)	(((_XPrivDisplay)dpy)->last_request_read)

/* macros for screen oriented applications (toolkit) */
#define ScreenOfDisplay(dpy, scr)(&((_XPrivDisplay)dpy)->screens[scr])
#define DefaultScreenOfDisplay(dpy) ScreenOfDisplay(dpy,DefaultScreen(dpy))
#define DisplayOfScreen(s)	((s)->display)
#define RootWindowOfScreen(s)	((s)->root)
#define BlackPixelOfScreen(s)	((s)->black_pixel)
#define WhitePixelOfScreen(s)	((s)->white_pixel)
#define DefaultColormapOfScreen(s)((s)->cmap)
#define DefaultDepthOfScreen(s)	((s)->root_depth)
#define DefaultGCOfScreen(s)	((s)->default_gc)
#define DefaultVisualOfScreen(s)((s)->root_visual)
#define WidthOfScreen(s)	((s)->width)
#define HeightOfScreen(s)	((s)->height)
#define WidthMMOfScreen(s)	((s)->mwidth)
#define HeightMMOfScreen(s)	((s)->mheight)
#define PlanesOfScreen(s)	((s)->root_depth)
#define CellsOfScreen(s)	(DefaultVisualOfScreen((s))->map_entries)
#define MinCmapsOfScreen(s)	((s)->min_maps)
#define MaxCmapsOfScreen(s)	((s)->max_maps)
#define DoesSaveUnders(s)	((s)->save_unders)
#define DoesBackingStore(s)	((s)->backing_store)
#define EventMaskOfScreen(s)	((s)->root_input_mask)

/*
 * Extensions need a way to hang private data on some structures.
 */
typedef struct _XExtData {
	int number;		/* number returned by XRegisterExtension */
	struct _XExtData *next;	/* next item on list of data for structure */
	int (*free_private)();	/* called to free private storage */
	XPointer private_data;	/* data private to this extension. */
} XExtData;

/*
 * This file contains structures used by the extension mechanism.
 */
typedef struct {		/* public to extension, cannot be changed */
	int extension;		/* extension number */
	int major_opcode;	/* major op-code assigned by server */
	int first_event;	/* first event number for the extension */
	int first_error;	/* first error number for the extension */
} XExtCodes;

/*
 * Data structure for retrieving info about pixmap formats.
 */

typedef struct {
    int depth;
    int bits_per_pixel;
    int scanline_pad;
} XPixmapFormatValues;


/*
 * Data structure for setting graphics context.
 */
typedef struct {
	int function;		/* logical operation */
	unsigned long plane_mask;/* plane mask */
	unsigned long foreground;/* foreground pixel */
	unsigned long background;/* background pixel */
	int line_width;		/* line width */
	int line_style;	 	/* LineSolid, LineOnOffDash, LineDoubleDash */
	int cap_style;	  	/* CapNotLast, CapButt, 
				   CapRound, CapProjecting */
	int join_style;	 	/* JoinMiter, JoinRound, JoinBevel */
	int fill_style;	 	/* FillSolid, FillTiled, 
				   FillStippled, FillOpaeueStippled */
	int fill_rule;	  	/* EvenOddRule, WindingRule */
	int arc_mode;		/* ArcChord, ArcPieSlice */
	Pixmap tile;		/* tile pixmap for tiling operations */
	Pixmap stipple;		/* stipple 1 plane pixmap for stipping */
	int ts_x_origin;	/* offset for tile or stipple operations */
	int ts_y_origin;
        Font font;	        /* default text font for text operations */
	int subwindow_mode;     /* ClipByChildren, IncludeInferiors */
	Bool graphics_exposures;/* boolean, should exposures be generated */
	int clip_x_origin;	/* origin for clipping */
	int clip_y_origin;
	Pixmap clip_mask;	/* bitmap clipping; other calls for rects */
	int dash_offset;	/* patterned/dashed line information */
	char dashes;
} XGCValues;

/*
 * Graphics context.  The contents of this structure are implementation
 * dependent.  A GC should be treated as opaque by application code.
 */

typedef struct _XGC
#ifdef XLIB_ILLEGAL_ACCESS
{
    XExtData *ext_data;	/* hook for extension to hang data */
    GContext gid;	/* protocol ID for graphics context */
    /* there is more to this structure, but it is private to Xlib */
}
#endif
*GC;

/*
 * Visual structure; contains information about colormapping possible.
 */
typedef struct {
	XExtData *ext_data;	/* hook for extension to hang data */
	VisualID visualid;	/* visual id of this visual */
#if defined(__cplusplus) || defined(c_plusplus)
	int c_class;		/* C++ class of screen (monochrome, etc.) */
#else
	int class;		/* class of screen (monochrome, etc.) */
#endif
	unsigned long red_mask, green_mask, blue_mask;	/* mask values */
	int bits_per_rgb;	/* log base 2 of distinct color values */
	int map_entries;	/* color map entries */
} Visual;

/*
 * Depth structure; contains information for each possible depth.
 */	
typedef struct {
	int depth;		/* this depth (Z) of the depth */
	int nvisuals;		/* number of Visual types at this depth */
	Visual *visuals;	/* list of visuals possible at this depth */
} Depth;

/*
 * Information about the screen.  The contents of this structure are
 * implementation dependent.  A Screen should be treated as opaque
 * by application code.
 */

struct _XDisplay;		/* Forward declare before use for C++ */

typedef struct {
	XExtData *ext_data;	/* hook for extension to hang data */
	struct _XDisplay *display;/* back pointer to display structure */
	Window root;		/* Root window id. */
	int width, height;	/* width and height of screen */
	int mwidth, mheight;	/* width and height of  in millimeters */
	int ndepths;		/* number of depths possible */
	Depth *depths;		/* list of allowable depths on the screen */
	int root_depth;		/* bits per pixel */
	Visual *root_visual;	/* root visual */
	GC default_gc;		/* GC for the root root visual */
	Colormap cmap;		/* default color map */
	unsigned long white_pixel;
	unsigned long black_pixel;	/* White and Black pixel values */
	int max_maps, min_maps;	/* max and min color maps */
	int backing_store;	/* Never, WhenMapped, Always */
	Bool save_unders;	
	long root_input_mask;	/* initial root input mask */
} Screen;

/*
 * Format structure; describes ZFormat data the screen will understand.
 */
typedef struct {
	XExtData *ext_data;	/* hook for extension to hang data */
	int depth;		/* depth of this image format */
	int bits_per_pixel;	/* bits/pixel at this depth */
	int scanline_pad;	/* scanline must padded to this multiple */
} ScreenFormat;

/*
 * Data structure for setting window attributes.
 */
typedef struct {
    Pixmap background_pixmap;	/* background or None or ParentRelative */
    unsigned long background_pixel;	/* background pixel */
    Pixmap border_pixmap;	/* border of the window */
    unsigned long border_pixel;	/* border pixel value */
    int bit_gravity;		/* one of bit gravity values */
    int win_gravity;		/* one of the window gravity values */
    int backing_store;		/* NotUseful, WhenMapped, Always */
    unsigned long backing_planes;/* planes to be preseved if possible */
    unsigned long backing_pixel;/* value to use in restoring planes */
    Bool save_under;		/* should bits under be saved? (popups) */
    long event_mask;		/* set of events that should be saved */
    long do_not_propagate_mask;	/* set of events that should not propagate */
    Bool override_redirect;	/* boolean value for override-redirect */
    Colormap colormap;		/* color map to be associated with window */
    Cursor cursor;		/* cursor to be displayed (or None) */
} XSetWindowAttributes;

typedef struct {
    int x, y;			/* location of window */
    int width, height;		/* width and height of window */
    int border_width;		/* border width of window */
    int depth;          	/* depth of window */
    Visual *visual;		/* the associated visual structure */
    Window root;        	/* root of screen containing window */
#if defined(__cplusplus) || defined(c_plusplus)
    int c_class;		/* C++ InputOutput, InputOnly*/
#else
    int class;			/* InputOutput, InputOnly*/
#endif
    int bit_gravity;		/* one of bit gravity values */
    int win_gravity;		/* one of the window gravity values */
    int backing_store;		/* NotUseful, WhenMapped, Always */
    unsigned long backing_planes;/* planes to be preserved if possible */
    unsigned long backing_pixel;/* value to be used when restoring planes */
    Bool save_under;		/* boolean, should bits under be saved? */
    Colormap colormap;		/* color map to be associated with window */
    Bool map_installed;		/* boolean, is color map currently installed*/
    int map_state;		/* IsUnmapped, IsUnviewable, IsViewable */
    long all_event_masks;	/* set of events all people have interest in*/
    long your_event_mask;	/* my event mask */
    long do_not_propagate_mask; /* set of events that should not propagate */
    Bool override_redirect;	/* boolean value for override-redirect */
    Screen *screen;		/* back pointer to correct screen */
} XWindowAttributes;

/*
 * Data structure for host setting; getting routines.
 *
 */

typedef struct {
	int family;		/* for example FamilyInternet */
	int length;		/* length of address, in bytes */
	char *address;		/* pointer to where to find the bytes */
} XHostAddress;

/*
 * Data structure for "image" data, used by image manipulation routines.
 */
typedef struct _XImage {
    int width, height;		/* size of image */
    int xoffset;		/* number of pixels offset in X direction */
    int format;			/* XYBitmap, XYPixmap, ZPixmap */
    char *data;			/* pointer to image data */
    int byte_order;		/* data byte order, LSBFirst, MSBFirst */
    int bitmap_unit;		/* quant. of scanline 8, 16, 32 */
    int bitmap_bit_order;	/* LSBFirst, MSBFirst */
    int bitmap_pad;		/* 8, 16, 32 either XY or ZPixmap */
    int depth;			/* depth of image */
    int bytes_per_line;		/* accelarator to next line */
    int bits_per_pixel;		/* bits per pixel (ZPixmap) */
    unsigned long red_mask;	/* bits in z arrangment */
    unsigned long green_mask;
    unsigned long blue_mask;
    XPointer obdata;		/* hook for the object routines to hang on */
    struct funcs {		/* image manipulation routines */
	struct _XImage *(*create_image)();
#if NeedFunctionPrototypes
	int (*destroy_image)        (struct _XImage *);
	unsigned long (*get_pixel)  (struct _XImage *, int, int);
	int (*put_pixel)            (struct _XImage *, int, int, unsigned long);
	struct _XImage *(*sub_image)(struct _XImage *, int, int, unsigned int, unsigned int);
	int (*add_pixel)            (struct _XImage *, long);
#else
	int (*destroy_image)();
	unsigned long (*get_pixel)();
	int (*put_pixel)();
	struct _XImage *(*sub_image)();
	int (*add_pixel)();
#endif
	} f;
} XImage;

/* 
 * Data structure for XReconfigureWindow
 */
typedef struct {
    int x, y;
    int width, height;
    int border_width;
    Window sibling;
    int stack_mode;
} XWindowChanges;

/*
 * Data structure used by color operations
 */
typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  /* do_red, do_green, do_blue */
	char pad;
} XColor;

/* 
 * Data structures for graphics operations.  On most machines, these are
 * congruent with the wire protocol structures, so reformatting the data
 * can be avoided on these architectures.
 */
typedef struct {
    short x1, y1, x2, y2;
} XSegment;

typedef struct {
    short x, y;
} XPoint;
    
typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;
    
typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
} XArc;


/* Data structure for XChangeKeyboardControl */

typedef struct {
        int key_click_percent;
        int bell_percent;
        int bell_pitch;
        int bell_duration;
        int led;
        int led_mode;
        int key;
        int auto_repeat_mode;   /* On, Off, Default */
} XKeyboardControl;

/* Data structure for XGetKeyboardControl */

typedef struct {
        int key_click_percent;
	int bell_percent;
	unsigned int bell_pitch, bell_duration;
	unsigned long led_mask;
	int global_auto_repeat;
	char auto_repeats[32];
} XKeyboardState;

/* Data structure for XGetMotionEvents.  */

typedef struct {
        Time time;
	short x, y;
} XTimeCoord;

/* Data structure for X{Set,Get}ModifierMapping */

typedef struct {
 	int max_keypermod;	/* The server's max # of keys per modifier */
 	KeyCode *modifiermap;	/* An 8 by max_keypermod array of modifiers */
} XModifierKeymap;


/*
 * Display datatype maintaining display specific data.
 * The contents of this structure are implementation dependent.
 * A Display should be treated as opaque by application code.
 */
#ifndef XLIB_ILLEGAL_ACCESS
typedef struct _XDisplay Display;
#endif

struct _XPrivate;		/* Forward declare before use for C++ */
struct _XrmHashBucketRec;

typedef struct 
#ifdef XLIB_ILLEGAL_ACCESS
_XDisplay
#endif
{
	XExtData *ext_data;	/* hook for extension to hang data */
	struct _XPrivate *private1;
	int fd;			/* Network socket. */
	int private2;
	int proto_major_version;/* major version of server's X protocol */
	int proto_minor_version;/* minor version of servers X protocol */
	char *vendor;		/* vendor of the server hardware */
        XID private3;
	XID private4;
	XID private5;
	int private6;
	XID (*resource_alloc)();/* allocator function */
	int byte_order;		/* screen byte order, LSBFirst, MSBFirst */
	int bitmap_unit;	/* padding and data requirements */
	int bitmap_pad;		/* padding requirements on bitmaps */
	int bitmap_bit_order;	/* LeastSignificant or MostSignificant */
	int nformats;		/* number of pixmap formats in list */
	ScreenFormat *pixmap_format;	/* pixmap format list */
	int private8;
	int release;		/* release of the server */
	struct _XPrivate *private9, *private10;
	int qlen;		/* Length of input event queue */
	unsigned long last_request_read; /* seq number of last event read */
	unsigned long request;	/* sequence number of last request. */
	XPointer private11;
	XPointer private12;
	XPointer private13;
	XPointer private14;
	unsigned max_request_size; /* maximum number 32 bit words in request*/
	struct _XrmHashBucketRec *db;
	int (*private15)();
	char *display_name;	/* "host:display" string used on this connect*/
	int default_screen;	/* default screen for operations */
	int nscreens;		/* number of screens on this server*/
	Screen *screens;	/* pointer to list of screens */
	unsigned long motion_buffer;	/* size of motion buffer */
	unsigned long private16;
	int min_keycode;	/* minimum defined keycode */
	int max_keycode;	/* maximum defined keycode */
	XPointer private17;
	XPointer private18;
	int private19;
	char *xdefaults;	/* contents of defaults from server */
	/* there is more to this structure, but it is private to Xlib */
}
#ifdef XLIB_ILLEGAL_ACCESS
Display, 
#endif
*_XPrivDisplay;

#if NeedFunctionPrototypes	/* prototypes require event type definitions */
#undef _XEVENT_
#endif
#ifndef _XEVENT_
/*
 * Definitions of specific events.
 */
typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;	        /* "event" window it is reported relative to */
	Window root;	        /* root window that the event occured on */
	Window subwindow;	/* child window */
	Time time;		/* milliseconds */
	int x, y;		/* pointer x, y coordinates in event window */
	int x_root, y_root;	/* coordinates relative to root */
	unsigned int state;	/* key or button mask */
	unsigned int keycode;	/* detail */
	Bool same_screen;	/* same screen flag */
} XKeyEvent;
typedef XKeyEvent XKeyPressedEvent;
typedef XKeyEvent XKeyReleasedEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;	        /* "event" window it is reported relative to */
	Window root;	        /* root window that the event occured on */
	Window subwindow;	/* child window */
	Time time;		/* milliseconds */
	int x, y;		/* pointer x, y coordinates in event window */
	int x_root, y_root;	/* coordinates relative to root */
	unsigned int state;	/* key or button mask */
	unsigned int button;	/* detail */
	Bool same_screen;	/* same screen flag */
} XButtonEvent;
typedef XButtonEvent XButtonPressedEvent;
typedef XButtonEvent XButtonReleasedEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;	        /* "event" window reported relative to */
	Window root;	        /* root window that the event occured on */
	Window subwindow;	/* child window */
	Time time;		/* milliseconds */
	int x, y;		/* pointer x, y coordinates in event window */
	int x_root, y_root;	/* coordinates relative to root */
	unsigned int state;	/* key or button mask */
	char is_hint;		/* detail */
	Bool same_screen;	/* same screen flag */
} XMotionEvent;
typedef XMotionEvent XPointerMovedEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;	        /* "event" window reported relative to */
	Window root;	        /* root window that the event occured on */
	Window subwindow;	/* child window */
	Time time;		/* milliseconds */
	int x, y;		/* pointer x, y coordinates in event window */
	int x_root, y_root;	/* coordinates relative to root */
	int mode;		/* NotifyNormal, NotifyGrab, NotifyUngrab */
	int detail;
	/*
	 * NotifyAncestor, NotifyVirtual, NotifyInferior, 
	 * NotifyNonlinear,NotifyNonlinearVirtual
	 */
	Bool same_screen;	/* same screen flag */
	Bool focus;		/* boolean focus */
	unsigned int state;	/* key or button mask */
} XCrossingEvent;
typedef XCrossingEvent XEnterWindowEvent;
typedef XCrossingEvent XLeaveWindowEvent;

typedef struct {
	int type;		/* FocusIn or FocusOut */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;		/* window of event */
	int mode;		/* NotifyNormal, NotifyGrab, NotifyUngrab */
	int detail;
	/*
	 * NotifyAncestor, NotifyVirtual, NotifyInferior, 
	 * NotifyNonlinear,NotifyNonlinearVirtual, NotifyPointer,
	 * NotifyPointerRoot, NotifyDetailNone 
	 */
} XFocusChangeEvent;
typedef XFocusChangeEvent XFocusInEvent;
typedef XFocusChangeEvent XFocusOutEvent;

/* generated on EnterWindow and FocusIn  when KeyMapState selected */
typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	char key_vector[32];
} XKeymapEvent;	

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	int x, y;
	int width, height;
	int count;		/* if non-zero, at least this many more */
} XExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Drawable drawable;
	int x, y;
	int width, height;
	int count;		/* if non-zero, at least this many more */
	int major_code;		/* core is CopyArea or CopyPlane */
	int minor_code;		/* not defined in the core */
} XGraphicsExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Drawable drawable;
	int major_code;		/* core is CopyArea or CopyPlane */
	int minor_code;		/* not defined in the core */
} XNoExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	int state;		/* Visibility state */
} XVisibilityEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window parent;		/* parent of the window */
	Window window;		/* window id of window created */
	int x, y;		/* window location */
	int width, height;	/* size of window */
	int border_width;	/* border width */
	Bool override_redirect;	/* creation should be overridden */
} XCreateWindowEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
} XDestroyWindowEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
	Bool from_configure;
} XUnmapEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
	Bool override_redirect;	/* boolean, is override set... */
} XMapEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window parent;
	Window window;
} XMapRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
	Window parent;
	int x, y;
	Bool override_redirect;
} XReparentEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	Bool override_redirect;
} XConfigureEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
	int x, y;
} XGravityEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	int width, height;
} XResizeRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window parent;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	int detail;		/* Above, Below, TopIf, BottomIf, Opposite */
	unsigned long value_mask;
} XConfigureRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window event;
	Window window;
	int place;		/* PlaceOnTop, PlaceOnBottom */
} XCirculateEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window parent;
	Window window;
	int place;		/* PlaceOnTop, PlaceOnBottom */
} XCirculateRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	Atom atom;
	Time time;
	int state;		/* NewValue, Deleted */
} XPropertyEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	Atom selection;
	Time time;
} XSelectionClearEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window owner;
	Window requestor;
	Atom selection;
	Atom target;
	Atom property;
	Time time;
} XSelectionRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window requestor;
	Atom selection;
	Atom target;
	Atom property;		/* ATOM or None */
	Time time;
} XSelectionEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	Colormap colormap;	/* COLORMAP or None */
#if defined(__cplusplus) || defined(c_plusplus)
	Bool c_new;		/* C++ */
#else
	Bool new;
#endif
	int state;		/* ColormapInstalled, ColormapUninstalled */
} XColormapEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
	Atom message_type;
	int format;
	union {
		char b[20];
		short s[10];
		long l[5];
		} data;
} XClientMessageEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;		/* unused */
	int request;		/* one of MappingModifier, MappingKeyboard,
				   MappingPointer */
	int first_keycode;	/* first keycode */
	int count;		/* defines range of change w. first_keycode*/
} XMappingEvent;

typedef struct {
	int type;
	Display *display;	/* Display the event was read from */
	XID resourceid;		/* resource id */
	unsigned long serial;	/* serial number of failed request */
	unsigned char error_code;	/* error code of failed request */
	unsigned char request_code;	/* Major op-code of failed request */
	unsigned char minor_code;	/* Minor op-code of failed request */
} XErrorEvent;

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;/* Display the event was read from */
	Window window;	/* window on which event was requested in event mask */
} XAnyEvent;

/*
 * this union is defined so Xlib can always use the same sized
 * event structure internally, to avoid memory fragmentation.
 */
typedef union _XEvent {
        int type;		/* must not be changed; first element */
	XAnyEvent xany;
	XKeyEvent xkey;
	XButtonEvent xbutton;
	XMotionEvent xmotion;
	XCrossingEvent xcrossing;
	XFocusChangeEvent xfocus;
	XExposeEvent xexpose;
	XGraphicsExposeEvent xgraphicsexpose;
	XNoExposeEvent xnoexpose;
	XVisibilityEvent xvisibility;
	XCreateWindowEvent xcreatewindow;
	XDestroyWindowEvent xdestroywindow;
	XUnmapEvent xunmap;
	XMapEvent xmap;
	XMapRequestEvent xmaprequest;
	XReparentEvent xreparent;
	XConfigureEvent xconfigure;
	XGravityEvent xgravity;
	XResizeRequestEvent xresizerequest;
	XConfigureRequestEvent xconfigurerequest;
	XCirculateEvent xcirculate;
	XCirculateRequestEvent xcirculaterequest;
	XPropertyEvent xproperty;
	XSelectionClearEvent xselectionclear;
	XSelectionRequestEvent xselectionrequest;
	XSelectionEvent xselection;
	XColormapEvent xcolormap;
	XClientMessageEvent xclient;
	XMappingEvent xmapping;
	XErrorEvent xerror;
	XKeymapEvent xkeymap;
	long pad[24];
} XEvent;
#endif

#define XAllocID(dpy) ((*((_XPrivDisplay)dpy)->resource_alloc)((dpy)))

/*
 * per character font metric information.
 */
typedef struct {
    short	lbearing;	/* origin to left edge of raster */
    short	rbearing;	/* origin to right edge of raster */
    short	width;		/* advance to next char's origin */
    short	ascent;		/* baseline to top edge of raster */
    short	descent;	/* baseline to bottom edge of raster */
    unsigned short attributes;	/* per char flags (not predefined) */
} XCharStruct;

/*
 * To allow arbitrary information with fonts, there are additional properties
 * returned.
 */
typedef struct {
    Atom name;
    unsigned long card32;
} XFontProp;

typedef struct {
    XExtData	*ext_data;	/* hook for extension to hang data */
    Font        fid;            /* Font id for this font */
    unsigned	direction;	/* hint about direction the font is painted */
    unsigned	min_char_or_byte2;/* first character */
    unsigned	max_char_or_byte2;/* last character */
    unsigned	min_byte1;	/* first row that exists */
    unsigned	max_byte1;	/* last row that exists */
    Bool	all_chars_exist;/* flag if all characters have non-zero size*/
    unsigned	default_char;	/* char to print for undefined character */
    int         n_properties;   /* how many properties there are */
    XFontProp	*properties;	/* pointer to array of additional properties*/
    XCharStruct	min_bounds;	/* minimum bounds over all existing char*/
    XCharStruct	max_bounds;	/* maximum bounds over all existing char*/
    XCharStruct	*per_char;	/* first_char to last_char information */
    int		ascent;		/* log. extent above baseline for spacing */
    int		descent;	/* log. descent below baseline for spacing */
} XFontStruct;

/*
 * PolyText routines take these as arguments.
 */
typedef struct {
    char *chars;		/* pointer to string */
    int nchars;			/* number of characters */
    int delta;			/* delta between strings */
    Font font;			/* font to print it in, None don't change */
} XTextItem;

typedef struct {		/* normal 16 bit characters are two bytes */
    unsigned char byte1;
    unsigned char byte2;
} XChar2b;

typedef struct {
    XChar2b *chars;		/* two byte characters */
    int nchars;			/* number of characters */
    int delta;			/* delta between strings */
    Font font;			/* font to print it in, None don't change */
} XTextItem16;


typedef union { Display *display;
		GC gc;
		Visual *visual;
		Screen *screen;
		ScreenFormat *pixmap_format;
		XFontStruct *font; } XEDataObject;

typedef struct {
    XRectangle      max_ink_extent;
    XRectangle      max_logical_extent;
} XFontSetExtents;

typedef void (*XOMProc)();

typedef struct _XOM *XOM;
typedef struct _XOC *XOC, *XFontSet;

typedef struct {
    char           *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XmbTextItem;

typedef struct {
    wchar_t        *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XwcTextItem;

#define XNRequiredCharSet "requiredCharSet"
#define XNQueryOrientation "queryOrientation"
#define XNBaseFontName "baseFontName"
#define XNOMAutomatic "omAutomatic"
#define XNMissingCharSet "missingCharSet"
#define XNDefaultString "defaultString"
#define XNOrientation "orientation"
#define XNDirectionalDependentDrawing "directionalDependentDrawing"
#define XNContextualDrawing "contextualDrawing"
#define XNFontInfo "fontInfo"

typedef struct {
    int charset_count;
    char **charset_list;
} XOMCharSetList;

typedef enum {
    XOMOrientation_LTR_TTB,
    XOMOrientation_RTL_TTB,
    XOMOrientation_TTB_LTR,
    XOMOrientation_TTB_RTL,
    XOMOrientation_Context
} XOrientation;

typedef struct {
    int num_orient;
    XOrientation *orient;	/* Input Text description */
} XOMOrientation;

typedef struct {
    int num_font;
    XFontStruct **font_struct_list;
    char **font_name_list;
} XOMFontInfo;

typedef void (*XIMProc)();

typedef struct _XIM *XIM;
typedef struct _XIC *XIC;

typedef unsigned long XIMStyle;

typedef struct {
    unsigned short count_styles;
    XIMStyle *supported_styles;
} XIMStyles;

#define XIMPreeditArea		0x0001L
#define XIMPreeditCallbacks	0x0002L
#define XIMPreeditPosition	0x0004L
#define XIMPreeditNothing	0x0008L
#define XIMPreeditNone		0x0010L
#define XIMStatusArea		0x0100L
#define XIMStatusCallbacks	0x0200L
#define XIMStatusNothing	0x0400L
#define XIMStatusNone		0x0800L

#define XNVaNestedList "XNVaNestedList"
#define XNQueryInputStyle "queryInputStyle"
#define XNClientWindow "clientWindow"
#define XNInputStyle "inputStyle"
#define XNFocusWindow "focusWindow"
#define XNResourceName "resourceName"
#define XNResourceClass "resourceClass"
#define XNGeometryCallback "geometryCallback"
#define XNDestroyCallback "destroyCallback"
#define XNFilterEvents "filterEvents"
#define XNPreeditStartCallback "preeditStartCallback"
#define XNPreeditDoneCallback "preeditDoneCallback"
#define XNPreeditDrawCallback "preeditDrawCallback"
#define XNPreeditCaretCallback "preeditCaretCallback"
#define XNPreeditStateNotifyCallback "preeditStateNotifyCallback"
#define XNPreeditAttributes "preeditAttributes"
#define XNStatusStartCallback "statusStartCallback"
#define XNStatusDoneCallback "statusDoneCallback"
#define XNStatusDrawCallback "statusDrawCallback"
#define XNStatusAttributes "statusAttributes"
#define XNArea "area"
#define XNAreaNeeded "areaNeeded"
#define XNSpotLocation "spotLocation"
#define XNColormap "colorMap"
#define XNStdColormap "stdColorMap"
#define XNForeground "foreground"
#define XNBackground "background"
#define XNBackgroundPixmap "backgroundPixmap"
#define XNFontSet "fontSet"
#define XNLineSpace "lineSpace"
#define XNCursor "cursor"

#define XNQueryIMValuesList "queryIMValuesList"
#define XNQueryICValuesList "queryICValuesList"
#define XNVisiblePosition "visiblePosition"
#define XNR6PreeditCallback "r6PreeditCallback"
#define XNStringConversionCallback "stringConversionCallback"
#define XNStringConversion "stringConversion"
#define XNResetState "resetState"
#define XNHotKey "hotKey"
#define XNHotKeyState "hotKeyState"
#define XNPreeditState "preeditState"
#define XNSeparatorofNestedList "separatorofNestedList"

#define XBufferOverflow		-1
#define XLookupNone		1
#define XLookupChars		2
#define XLookupKeySym		3
#define XLookupBoth		4

#if NeedFunctionPrototypes
typedef void *XVaNestedList;
#else
typedef XPointer XVaNestedList;
#endif

typedef struct {
    XPointer client_data;
    XIMProc callback;
} XIMCallback;

typedef unsigned long XIMFeedback;

#define XIMReverse		1L
#define XIMUnderline		(1L<<1) 
#define XIMHighlight		(1L<<2)
#define XIMPrimary	 	(1L<<5)
#define XIMSecondary		(1L<<6)
#define XIMTertiary	 	(1L<<7)
#define XIMVisibleToForward 	(1L<<8)
#define XIMVisibleToBackword 	(1L<<9)
#define XIMVisibleToCenter 	(1L<<10)

typedef struct _XIMText {
    unsigned short length;
    XIMFeedback *feedback;
    Bool encoding_is_wchar; 
    union {
	char *multi_byte;
	wchar_t *wide_char;
    } string; 
} XIMText;

typedef	unsigned long	 XIMPreeditState;

#define	XIMPreeditUnKnown	0L
#define	XIMPreeditEnable	1L
#define	XIMPreeditDisable	(1L<<1)

typedef	struct	_XIMPreeditStateNotifyCallbackStruct {
    XIMPreeditState state;
} XIMPreeditStateNotifyCallbackStruct;

typedef	unsigned long	 XIMResetState;

#define	XIMInitialState		1L
#define	XIMPreserveState	(1L<<1)

typedef unsigned long XIMStringConversionFeedback;

#define	XIMStringConversionLeftEdge	(0x00000001)
#define	XIMStringConversionRightEdge	(0x00000002)
#define	XIMStringConversionTopEdge	(0x00000004)
#define	XIMStringConversionBottomEdge	(0x00000008)
#define	XIMStringConversionConcealed	(0x00000010)
#define	XIMStringConversionWrapped	(0x00000020)

typedef struct _XIMStringConversionText {
    unsigned short length;
    XIMStringConversionFeedback *feedback;
    Bool encoding_is_wchar; 
    union {
	char *mbs;
	wchar_t *wcs;
    } string; 
} XIMStringConversionText;

typedef	unsigned short	XIMStringConversionPosition;

typedef	unsigned short	XIMStringConversionType;

#define	XIMStringConversionBuffer	(0x0001)
#define	XIMStringConversionLine		(0x0002)
#define	XIMStringConversionWord		(0x0003)
#define	XIMStringConversionChar		(0x0004)

typedef	unsigned short	XIMStringConversionOperation;

#define	XIMStringConversionSubstitution	(0x0001)
#define	XIMStringConversionRetrival	(0x0002)

typedef struct _XIMStringConversionCallbackStruct {
    XIMStringConversionPosition position;
    XIMStringConversionType type;
    XIMStringConversionOperation operation;
    unsigned short factor;
    XIMStringConversionText *text;
} XIMStringConversionCallbackStruct;

typedef struct _XIMPreeditDrawCallbackStruct {
    int caret;		/* Cursor offset within pre-edit string */
    int chg_first;	/* Starting change position */
    int chg_length;	/* Length of the change in character count */
    XIMText *text;
} XIMPreeditDrawCallbackStruct;

typedef enum {
    XIMForwardChar, XIMBackwardChar,
    XIMForwardWord, XIMBackwardWord,
    XIMCaretUp, XIMCaretDown,
    XIMNextLine, XIMPreviousLine,
    XIMLineStart, XIMLineEnd, 
    XIMAbsolutePosition,
    XIMDontChange
} XIMCaretDirection;

typedef enum {
    XIMIsInvisible,	/* Disable caret feedback */ 
    XIMIsPrimary,	/* UI defined caret feedback */
    XIMIsSecondary	/* UI defined caret feedback */
} XIMCaretStyle;

typedef struct _XIMPreeditCaretCallbackStruct {
    int position;		 /* Caret offset within pre-edit string */
    XIMCaretDirection direction; /* Caret moves direction */
    XIMCaretStyle style;	 /* Feedback of the caret */
} XIMPreeditCaretCallbackStruct;

typedef enum {
    XIMTextType,
    XIMBitmapType
} XIMStatusDataType;
	
typedef struct _XIMStatusDrawCallbackStruct {
    XIMStatusDataType type;
    union {
	XIMText *text;
	Pixmap  bitmap;
    } data;
} XIMStatusDrawCallbackStruct;

typedef struct _XIMHotKeyTrigger {
    KeySym	 keysym;
    int		 modifier;
    int		 modifier_mask;
} XIMHotKeyTrigger;

typedef struct _XIMHotKeyTriggers {
    int			 num_hot_key;
    XIMHotKeyTrigger	*key;
} XIMHotKeyTriggers;

typedef	unsigned long	 XIMHotKeyState;

#define	XIMHotKeyStateON	(0x0001L)
#define	XIMHotKeyStateOFF	(0x0002L)

typedef struct {
    unsigned short count_values;
    char **supported_values;
} XIMValuesList;

#if defined(WIN32) && !defined(_XLIBINT_)
#define _Xdebug (*_Xdebug_p)
#endif

int _Xdebug;

XFontStruct *XLoadQueryFont(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* name */
#endif
){}

XFontStruct *XQueryFont(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	b		/* font_ID */
#endif
){}


XTimeCoord *XGetMotionEvents(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Time c		/* start */,
    Time d		/* stop */,
    int* e		/* nevents_return */
#endif
){}

XModifierKeymap *XDeleteModifiermapEntry(
#if NeedFunctionPrototypes
    XModifierKeymap* a	/* modmap */,
#if NeedWidePrototypes
    unsigned int b	/* keycode_entry */,
#else
    KeyCode	c	/* keycode_entry */,
#endif
    int	d		/* modifier */
#endif
){}

XModifierKeymap	*XGetModifierMapping(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XModifierKeymap	*XInsertModifiermapEntry(
#if NeedFunctionPrototypes
    XModifierKeymap* b	/* modmap */,
#if NeedWidePrototypes
    unsigned int c	/* keycode_entry */,
#else
    KeyCode	d	/* keycode_entry */,
#endif
    int	e		/* modifier */    
#endif
){}

XModifierKeymap *XNewModifiermap(
#if NeedFunctionPrototypes
    int	a		/* max_keys_per_mod */
#endif
){}

XImage *XCreateImage(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Visual*	b	/* visual */,
    unsigned int c	/* depth */,
    int	d		/* format */,
    int	e		/* offset */,
    char* f		/* data */,
    unsigned int g	/* width */,
    unsigned int h	/* height */,
    int	i		/* bitmap_pad */,
    int	j		/* bytes_per_line */
#endif
){}
Status XInitImage(
#if NeedFunctionPrototypes
    XImage*	a	/* image */
#endif
){}
XImage *XGetImage(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    int	c		/* x */,
    int	d		/* y */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned long g	/* plane_mask */,
    int	h		/* format */
#endif
){}
XImage *XGetSubImage(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    int	c		/* x */,
    int	d		/* y */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned long g	/* plane_mask */,
    int	h		/* format */,
    XImage*	i	/* dest_image */,
    int	j		/* dest_x */,
    int	k		/* dest_y */
#endif
){}

/* 
 * X function declarations.
 */
#if defined(_VISUALC_)
extern Export Display *XOpenDisplay(server_name)
#else
Display *XOpenDisplay(server_name)
#endif
const char
  *server_name;
{
  return((Display *) NULL);
}

void XrmInitialize(
#if NeedFunctionPrototypes
    void * a
#endif
){}

char *XFetchBytes(
#if NeedFunctionPrototypes
    Display* b		/* display */,
    int* c		/* nbytes_return */
#endif
){}
char *XFetchBuffer(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* nbytes_return */,
    int	c		/* buffer */
#endif
){}
char *XGetAtomName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Atom b		/* atom */
#endif
){}
Status XGetAtomNames(
#if NeedFunctionPrototypes
    Display* a		/* dpy */,
    Atom* b		/* atoms */,
    int	c		/* count */,
    char** d		/* names_return */
#endif
){}
char *XGetDefault(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* program */,
    _Xconst char* c	/* option */		  
#endif
){}
#if defined(_VISUALC_)
extern Export char *XDisplayName(_Xconst char* a)
#else
char *XDisplayName(_Xconst char* a)
#endif
{}
char *XKeysymToString(
#if NeedFunctionPrototypes
    KeySym	a	/* keysym */
#endif
){}

int (*XSynchronize(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Bool b		/* onoff */
#endif
)){};
int (*XSetAfterFunction(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int (* b) (
#if NeedNestedPrototypes
	     Display* a	/* display */
#endif
            )		/* procedure */
#endif
)){};
Atom XInternAtom(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* atom_name */,
    Bool c		/* only_if_exists */		 
#endif
){}
Status XInternAtoms(
#if NeedFunctionPrototypes
    Display* a		/* dpy */,
    char** b		/* names */,
    int	c		/* count */,
    Bool d		/* onlyIfExists */,
    Atom* e		/* atoms_return */
#endif
){}
Colormap XCopyColormapAndFree(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */
#endif
){}
Colormap XCreateColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Visual*	c	/* visual */,
    int	d		/* alloc */			 
#endif
){}
Cursor XCreatePixmapCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Pixmap b		/* source */,
    Pixmap c	/* mask */,
    XColor*	d	/* foreground_color */,
    XColor*	e	/* background_color */,
    unsigned int f	/* x */,
    unsigned int g	/* y */			   
#endif
){}
Cursor XCreateGlyphCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Font b		/* source_font */,
    Font c		/* mask_font */,
    unsigned int d	/* source_char */,
    unsigned int e	/* mask_char */,
    XColor*	f	/* foreground_color */,
    XColor*	g	/* background_color */
#endif
){}
Cursor XCreateFontCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    unsigned int b	/* shape */
#endif
){}
Font XLoadFont(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* name */
#endif
){}
GC XCreateGC(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    unsigned long c	/* valuemask */,
    XGCValues* d		/* values */
#endif
){}
GContext XGContextFromGC(
#if NeedFunctionPrototypes
    GC a			/* gc */
#endif
){}
void XFlushGC(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC	b		/* gc */
#endif
){}
Pixmap XCreatePixmap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    unsigned int c	/* width */,
    unsigned int d	/* height */,
    unsigned int e	/* depth */		        
#endif
){}
Pixmap XCreateBitmapFromData(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    _Xconst char* c	/* data */,
    unsigned int d	/* width */,
    unsigned int e	/* height */
#endif
){}
Pixmap XCreatePixmapFromBitmapData(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    char* c		/* data */,
    unsigned int d	/* width */,
    unsigned int e	/* height */,
    unsigned long f	/* fg */,
    unsigned long g	/* bg */,
    unsigned int h	/* depth */
#endif
){}
Window XCreateSimpleWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* parent */,
    int	c		/* x */,
    int	d		/* y */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned int g	/* border_width */,
    unsigned long h	/* border */,
    unsigned long i	/* background */
#endif
){}
Window XGetSelectionOwner(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Atom b		/* selection */
#endif
){}
Window XCreateWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* parent */,
    int	c		/* x */,
    int	d		/* y */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned int g	/* border_width */,
    int	h		/* depth */,
    unsigned int i	/* class */,
    Visual*	j	/* visual */,
    unsigned long k	/* valuemask */,
    XSetWindowAttributes* l	/* attributes */
#endif
){} 
Colormap *XListInstalledColormaps(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int* c		/* num_return */
#endif
){}
char **XListFonts(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* pattern */,
    int	c		/* maxnames */,
    int* d		/* actual_count_return */
#endif
){}
char **XListFontsWithInfo(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* pattern */,
    int	c		/* maxnames */,
    int* d		/* count_return */,
    XFontStruct** e	/* info_return */
#endif
){}
char **XGetFontPath(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* npaths_return */
#endif
){}
char **XListExtensions(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* nextensions_return */
#endif
){}
Atom *XListProperties(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int* c		/* num_prop_return */
#endif
){}
XHostAddress *XListHosts(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* nhosts_return */,
    Bool* c		/* state_return */
#endif
){}
KeySym XKeycodeToKeysym(
#if NeedFunctionPrototypes
    Display* a		/* display */,
#if NeedWidePrototypes
    unsigned int b	/* keycode */,
#else
    KeyCode	c	/* keycode */,
#endif
    int	d		/* index */
#endif
){}
KeySym XLookupKeysym(
#if NeedFunctionPrototypes
    XKeyEvent* a		/* key_event */,
    int	b		/* index */
#endif
){}
KeySym *XGetKeyboardMapping(
#if NeedFunctionPrototypes
    Display* a		/* display */,
#if NeedWidePrototypes
    unsigned int b	/* first_keycode */,
#else
    KeyCode	c	/* first_keycode */,
#endif
    int	d		/* keycode_count */,
    int* e		/* keysyms_per_keycode_return */
#endif
){}
KeySym XStringToKeysym(
#if NeedFunctionPrototypes
    _Xconst char* a	/* string */
#endif
){}
long XMaxRequestSize(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
long XExtendedMaxRequestSize(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
char *XResourceManagerString(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
char *XScreenResourceString(
#if NeedFunctionPrototypes
	Screen*	a	/* screen */
#endif
){}
unsigned long XDisplayMotionBufferSize(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
VisualID XVisualIDFromVisual(
#if NeedFunctionPrototypes
    Visual*	a	/* visual */
#endif
){}

/* multithread routines */

Status XInitThreads(
#if NeedFunctionPrototypes
    void *a
#endif
){}

void XLockDisplay(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

void XUnlockDisplay(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

/* routines for dealing with extensions */

XExtCodes *XInitExtension(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* name */
#endif
){}

XExtCodes *XAddExtension(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
XExtData *XFindOnExtensionList(
#if NeedFunctionPrototypes
    XExtData** a		/* structure */,
    int	b		/* number */
#endif
){}
XExtData **XEHeadOfExtensionList(
#if NeedFunctionPrototypes
    XEDataObject a	/* object */
#endif
){}

/* these are routines for which there are also macros */
#if defined(_VISUALC_)
extern Export Window XRootWindow(Display* a,int b)
#else
Window XRootWindow(Display* a,int b)
#endif
{}
Window XDefaultRootWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
Window XRootWindowOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
Visual *XDefaultVisual(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}
Visual *XDefaultVisualOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
GC XDefaultGC(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}
GC XDefaultGCOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
unsigned long XBlackPixel(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}
unsigned long XWhitePixel(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}
unsigned long XAllPlanes(
#if NeedFunctionPrototypes
    void *a
#endif
){}
unsigned long XBlackPixelOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
unsigned long XWhitePixelOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
unsigned long XNextRequest(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
unsigned long XLastKnownRequestProcessed(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
char *XServerVendor(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
char *XDisplayString(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}
Colormap XDefaultColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}
Colormap XDefaultColormapOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
Display *XDisplayOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}
Screen *XScreenOfDisplay(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}
#if defined(_VISUALC_)
extern Export Screen *XDefaultScreenOfDisplay(Display* a)
#else
Screen *XDefaultScreenOfDisplay(Display* a)
#endif
{}
long XEventMaskOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}

int XScreenNumberOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}

typedef int (*XErrorHandler) (	    /* WARNING, this type not in Xlib spec */
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XErrorEvent* b	/* error_event */
#endif
);


#if defined(_VISUALC_)
extern Export XErrorHandler XSetErrorHandler (
#else
XErrorHandler XSetErrorHandler (
#endif
#if NeedFunctionPrototypes
    XErrorHandler a	/* handler */
#endif
){}


typedef int (*XIOErrorHandler) (    /* WARNING, this type not in Xlib spec */
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
);

XIOErrorHandler XSetIOErrorHandler (
#if NeedFunctionPrototypes
    XIOErrorHandler	a/* handler */
#endif
){}


XPixmapFormatValues *XListPixmapFormats(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* count_return */
#endif
){}
int *XListDepths(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */,
    int* c		/* count_return */
#endif
){}

/* ICCCM routines for things that don't require special include files; */
/* other declarations are given in Xutil.h                             */
Status XReconfigureWMWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* screen_number */,
    unsigned int d	/* mask */,
    XWindowChanges*	e /* changes */
#endif
){}

Status XGetWMProtocols(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Atom** c		/* protocols_return */,
    int* d		/* count_return */
#endif
){}
Status XSetWMProtocols(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Atom* c		/* protocols */,
    int	d		/* count */
#endif
){}
Status XIconifyWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* screen_number */
#endif
){}
Status XWithdrawWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* screen_number */
#endif
){}
Status XGetCommand(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    char***	c	/* argv_return */,
    int* d		/* argc_return */
#endif
){}
Status XGetWMColormapWindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Window** c		/* windows_return */,
    int* d		/* count_return */
#endif
){}
Status XSetWMColormapWindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Window*	c	/* colormap_windows */,
    int	d		/* count */
#endif
){}
void XFreeStringList(
#if NeedFunctionPrototypes
    char** a		/* list */
#endif
){}
XSetTransientForHint(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Window c		/* prop_window */
#endif
){}

/* The following are given in alphabetical order */

XActivateScreenSaver(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XAddHost(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XHostAddress* b	/* host */
#endif
){}

XAddHosts(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XHostAddress* b	/* hosts */,
    int	c		/* num_hosts */    
#endif
){}

XAddToExtensionList(
#if NeedFunctionPrototypes
    struct _XExtData** a	/* structure */,
    XExtData* b		/* ext_data */
#endif
){}

XAddToSaveSet(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

Status XAllocColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    XColor*	c	/* screen_in_out */
#endif
){}

Status XAllocColorCells(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    Bool c	        /* contig */,
    unsigned long* d	/* plane_masks_return */,
    unsigned int e	/* nplanes */,
    unsigned long* f	/* pixels_return */,
    unsigned int g	/* npixels */
#endif
){}

Status XAllocColorPlanes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    Bool c		/* contig */,
    unsigned long* x	/* pixels_return */,
    int	d		/* ncolors */,
    int	e		/* nreds */,
    int	f		/* ngreens */,
    int	g		/* nblues */,
    unsigned long* h	/* rmask_return */,
    unsigned long* i	/* gmask_return */,
    unsigned long* j	/* bmask_return */
#endif
){}

Status XAllocNamedColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    _Xconst char* c	/* color_name */,
    XColor*	d	/* screen_def_return */,
    XColor*	e	/* exact_def_return */
#endif
){}

XAllowEvents(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* event_mode */,
    Time c		/* time */
#endif
){}

XAutoRepeatOff(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XAutoRepeatOn(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XBell(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* percent */
#endif
){}

int XBitmapBitOrder(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XBitmapPad(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XBitmapUnit(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XCellsOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}

XChangeActivePointerGrab(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    unsigned int b	/* event_mask */,
    Cursor c		/* cursor */,
    Time d		/* time */
#endif
){}

XChangeGC(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC b			/* gc */,
    unsigned long c	/* valuemask */,
    XGCValues* d	/* values */
#endif
){}

XChangeKeyboardControl(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    unsigned long b	/* value_mask */,
    XKeyboardControl* c	/* values */
#endif
){}

XChangeKeyboardMapping(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* first_keycode */,
    int	c		/* keysyms_per_keycode */,
    KeySym*	d	/* keysyms */,
    int	e		/* num_codes */
#endif
){}

XChangePointerControl(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Bool b		/* do_accel */,
    Bool c		/* do_threshold */,
    int	d		/* accel_numerator */,
    int	e		/* accel_denominator */,
    int	f		/* threshold */
#endif
){}

XChangeProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Atom c		/* property */,
    Atom d		/* type */,
    int	e		/* format */,
    int	f		/* mode */,
    _Xconst unsigned char* g	/* data */,
    int	h		/* nelements */
#endif
){}

XChangeSaveSet(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* change_mode */
#endif
){}

XChangeWindowAttributes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    unsigned long c	/* valuemask */,
    XSetWindowAttributes* d /* attributes */
#endif
){}

Bool XCheckIfEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XEvent* b		/* event_return */,
    Bool (* c) (
#if NeedNestedPrototypes
	       Display*	x		/* display */,
               XEvent* y			/* event */,
               XPointer	z		/* arg */
#endif
             )		/* predicate */,
    XPointer d		/* arg */
#endif
){}

Bool XCheckMaskEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    long b		/* event_mask */,
    XEvent*	c	/* event_return */
#endif
){}

Bool XCheckTypedEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* event_type */,
    XEvent*	c	/* event_return */
#endif
){}

Bool XCheckTypedWindowEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* event_type */,
    XEvent*	d	/* event_return */
#endif
){}

Bool XCheckWindowEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    long c		/* event_mask */,
    XEvent*	d	/* event_return */
#endif
){}

XCirculateSubwindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* direction */
#endif
){}

XCirculateSubwindowsDown(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XCirculateSubwindowsUp(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XClearArea(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* x */,
    int	d		/* y */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    Bool g		/* exposures */
#endif
){}

XClearWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XCloseDisplay(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XConfigureWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    unsigned int c	/* value_mask */,
    XWindowChanges*	d /* values */		 
#endif
){}

int XConnectionNumber(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XConvertSelection(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Atom b		/* selection */,
    Atom c		/* target */,
    Atom d		/* property */,
    Window e	/* requestor */,
    Time f		/* time */
#endif
){}

XCopyArea(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* src */,
    Drawable c		/* dest */,
    GC	d		/* gc */,
    int	 e		/* src_x */,
    int	 f		/* src_y */,
    unsigned int g	/* width */,
    unsigned int h	/* height */,
    int	i		/* dest_x */,
    int	j		/* dest_y */
#endif
){}

XCopyGC(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC	b		/* src */,
    unsigned long c	/* valuemask */,
    GC	d		/* dest */
#endif
){}

XCopyPlane(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* src */,
    Drawable c		/* dest */,
    GC	d		/* gc */,
    int	e		/* src_x */,
    int	f		/* src_y */,
    unsigned int g	/* width */,
    unsigned int h	/* height */,
    int	i		/* dest_x */,
    int	j		/* dest_y */,
    unsigned long k	/* plane */
#endif
){}

int XDefaultDepth(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

int XDefaultDepthOfScreen(
#if NeedFunctionPrototypes
    Screen*	a	/* screen */
#endif
){}

#if defined(_VISUALC_)
extern Export int XDefaultScreen(Display* a)
#else
int XDefaultScreen(Display* a)
#endif
{}

XDefineCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Cursor c		/* cursor */
#endif
){}

XDeleteProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    Atom c		/* property */
#endif
){}

XDestroyWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XDestroySubwindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

int XDoesBackingStore(
#if NeedFunctionPrototypes
    Screen* c		/* screen */    
#endif
){}

Bool XDoesSaveUnders(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
#endif
){}

XDisableAccessControl(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}


int XDisplayCells(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

int XDisplayHeight(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

int XDisplayHeightMM(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

XDisplayKeycodes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* min_keycodes_return */,
    int* c		/* max_keycodes_return */
#endif
){}

int XDisplayPlanes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

int XDisplayWidth(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

int XDisplayWidthMM(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */
#endif
){}

XDrawArc(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    GC c			/* gc */,
    int	d		/* x */,
    int	e		/* y */,
    unsigned int f	/* width */,
    unsigned int g	/* height */,
    int	h		/* angle1 */,
    int	i		/* angle2 */
#endif
){}

XDrawArcs(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    GC c			/* gc */,
    XArc* d		/* arcs */,
    int	e		/* narcs */
#endif
){}

XDrawImageString(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    GC c			/* gc */,
    int	d		/* x */,
    int	e		/* y */,
    _Xconst char* f	/* string */,
    int	g		/* length */
#endif
){}

XDrawImageString16(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    GC c			/* gc */,
    int	d		/* x */,
    int	e		/* y */,
    _Xconst XChar2b* f	/* string */,
    int	g		/* length */
#endif
){}

XDrawLine(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable b		/* d */,
    GC c			/* gc */,
    int	d		/* x1 */,
    int	e		/* x2 */,
    int	f		/* y1 */,
    int	g		/* y2 */
#endif
){}

XDrawLines(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XPoint*	f	/* points */,
    int	g		/* npoints */,
    int	h		/* mode */
#endif
){}

XDrawPoint(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */
#endif
){}

XDrawPoints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XPoint*	f	/* points */,
    int	g		/* npoints */,
    int	h		/* mode */
#endif
){}

XDrawRectangle(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */,
    unsigned int h	/* width */,
    unsigned int i	/* height */
#endif
){}

XDrawRectangles(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XRectangle*	f	/* rectangles */,
    int	g		/* nrectangles */
#endif
){}

XDrawSegments(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XSegment* f		/* segments */,
    int g			/* nsegments */
#endif
){}

XDrawString(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */,
    _Xconst char* h	/* string */,
    int	i		/* length */
#endif
){}

XDrawString16(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */,
    _Xconst XChar2b* h	/* string */,
    int	i		/* length */
#endif
){}

XDrawText(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */,
    XTextItem* h		/* items */,
    int	i		/* nitems */
#endif
){}

XDrawText16(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int f			/* x */,
    int	g		/* y */,
    XTextItem16* h	/* items */,
    int	i		/* nitems */
#endif
){}

XEnableAccessControl(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XEventsQueued(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* mode */
#endif
){}

Status XFetchName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    char** c		/* window_name_return */
#endif
){}

XFillArc(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */,
    unsigned int h	/* width */,
    unsigned int i	/* height */,
    int	j		/* angle1 */,
    int	k		/* angle2 */
#endif
){}

XFillArcs(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XArc* f		/* arcs */,
    int	g		/* narcs */
#endif
){}

XFillPolygon(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XPoint*	f	/* points */,
    int g			/* npoints */,
    int	h		/* shape */,
    int	i		/* mode */
#endif
){}

XFillRectangle(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	f		/* x */,
    int	g		/* y */,
    unsigned int h	/* width */,
    unsigned int i	/* height */
#endif
){}

XFillRectangles(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XRectangle*	f	/* rectangles */,
    int	g		/* nrectangles */
#endif
){}

XFlush(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XForceScreenSaver(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* mode */
#endif
){}

XFree(
#if NeedFunctionPrototypes
    void* a		/* data */
#endif
){}

XFreeColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */
#endif
){}

XFreeColors(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    unsigned long* c	/* pixels */,
    int	d		/* npixels */,
    unsigned long e	/* planes */
#endif
){}

XFreeCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Cursor b		/* cursor */
#endif
){}

XFreeExtensionList(
#if NeedFunctionPrototypes
    char** a		/* list */    
#endif
){}

XFreeFont(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XFontStruct* b	/* font_struct */
#endif
){}

XFreeFontInfo(
#if NeedFunctionPrototypes
    char** a		/* names */,
    XFontStruct* b	/* free_info */,
    int	c		/* actual_count */
#endif
){}

XFreeFontNames(
#if NeedFunctionPrototypes
    char** a		/* list */
#endif
){}

XFreeFontPath(
#if NeedFunctionPrototypes
    char** a		/* list */
#endif
){}

XFreeGC(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */
#endif
){}

XFreeModifiermap(
#if NeedFunctionPrototypes
    XModifierKeymap* a	/* modmap */
#endif
){}

XFreePixmap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Pixmap b		/* pixmap */
#endif
){}

int XGeometry(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen */,
    _Xconst char* c	/* position */,
    _Xconst char* d	/* default_position */,
    unsigned int e	/* bwidth */,
    unsigned int f	/* fwidth */,
    unsigned int g	/* fheight */,
    int	h		/* xadder */,
    int	i		/* yadder */,
    int* j		/* x_return */,
    int* k		/* y_return */,
    int* l		/* width_return */,
    int* m		/* height_return */
#endif
){}

XGetErrorDatabaseText(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* name */,
    _Xconst char* c	/* message */,
    _Xconst char* d	/* default_string */,
    char* e		/* buffer_return */,
    int	f		/* length */
#endif
){}

XGetErrorText(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* code */,
    char* c		/* buffer_return */,
    int	d		/* length */
#endif
){}

Bool XGetFontProperty(
#if NeedFunctionPrototypes
    XFontStruct* a	/* font_struct */,
    Atom b		/* atom */,
    unsigned long* c	/* value_return */
#endif
){}

Status XGetGCValues(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    unsigned long f	/* valuemask */,
    XGCValues* g		/* values_return */
#endif
){}

Status XGetGeometry(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    Window*	e	/* root_return */,
    int* f		/* x_return */,
    int* g		/* y_return */,
    unsigned int* h	/* width_return */,
    unsigned int* i	/* height_return */,
    unsigned int* j	/* border_width_return */,
    unsigned int* k	/* depth_return */
#endif
){}

Status XGetIconName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    char** c		/* icon_name_return */
#endif
){}

XGetInputFocus(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window*	b	/* focus_return */,
    int* c		/* revert_to_return */
#endif
){}

XGetKeyboardControl(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XKeyboardState*	b /* values_return */
#endif
){}

XGetPointerControl(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* accel_numerator_return */,
    int* c		/* accel_denominator_return */,
    int* d		/* threshold_return */
#endif
){}

int XGetPointerMapping(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    unsigned char* b	/* map_return */,
    int	c		/* nmap */
#endif
){}

XGetScreenSaver(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int* b		/* timeout_return */,
    int* c		/* interval_return */,
    int* d		/* prefer_blanking_return */,
    int* e		/* allow_exposures_return */
#endif
){}

Status XGetTransientForHint(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    Window*	 c	/* prop_window_return */
#endif
){}

int XGetWindowProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Atom c		/* property */,
    long d		/* long_offset */,
    long e		/* long_length */,
    Bool f		/* delete */,
    Atom g		/* req_type */,
    Atom* h		/* actual_type_return */,
    int* i		/* actual_format_return */,
    unsigned long* j	/* nitems_return */,
    unsigned long* k	/* bytes_after_return */,
    unsigned char**	l /* prop_return */
#endif
){}

Status XGetWindowAttributes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XWindowAttributes* c	/* window_attributes_return */
#endif
){}

XGrabButton(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    unsigned int b	/* button */,
    unsigned int c	/* modifiers */,
    Window d		/* grab_window */,
    Bool e		/* owner_events */,
    unsigned int f	/* event_mask */,
    int	g		/* pointer_mode */,
    int	h		/* keyboard_mode */,
    Window	i	/* confine_to */,
    Cursor	j	/* cursor */
#endif
){}

XGrabKey(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* keycode */,
    unsigned int c	/* modifiers */,
    Window	d	/* grab_window */,
    Bool e		/* owner_events */,
    int	f		/* pointer_mode */,
    int	g		/* keyboard_mode */
#endif
){}

int XGrabKeyboard(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* grab_window */,
    Bool c		/* owner_events */,
    int	d		/* pointer_mode */,
    int	e		/* keyboard_mode */,
    Time f		/* time */
#endif
){}

int XGrabPointer(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* grab_window */,
    Bool c		/* owner_events */,
    unsigned int d	/* event_mask */,
    int	e		/* pointer_mode */,
    int	f		/* keyboard_mode */,
    Window g		/* confine_to */,
    Cursor h		/* cursor */,
    Time i		/* time */
#endif
){}

XGrabServer(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XHeightMMOfScreen(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
#endif
){}

int XHeightOfScreen(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
#endif
){}

XIfEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XEvent*	b	/* event_return */,
    Bool (* c) (
#if NeedNestedPrototypes
	       Display* x			/* display */,
               XEvent* y			/* event */,
               XPointer	z		/* arg */
#endif
             )		/* predicate */,
    XPointer d		/* arg */
#endif
){}

int XImageByteOrder(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XInstallColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */
#endif
){}

KeyCode XKeysymToKeycode(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    KeySym b		/* keysym */
#endif
){}

XKillClient(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	b		/* resource */
#endif
){}

unsigned long XXLastKnownRequestProcessed(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

Status XLookupColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    _Xconst char* c	/* color_name */,
    XColor*	d	/* exact_def_return */,
    XColor*	e	/* screen_def_return */
#endif
){}

XLowerWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */
#endif
){}

XMapRaised(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XMapSubwindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XMapWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XMaskEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    long b		/* event_mask */,
    XEvent*	c	/* event_return */
#endif
){}

int XMaxCmapsOfScreen(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
#endif
){}

int XMinCmapsOfScreen(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
#endif
){}

XMoveResizeWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int c			/* x */,
    int	d		/* y */,
    unsigned int e	/* width */,
    unsigned int f	/* height */
#endif
){}

XMoveWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    int	c		/* x */,
    int	d		/* y */
#endif
){}

XNextEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XEvent*	b	/* event_return */
#endif
){}

XNoOp(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

Status XParseColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    _Xconst char* c	/* spec */,
    XColor*	d	/* exact_def_return */
#endif
){}

static int
ReadInteger(string, NextString)
register char *string;
char **NextString;
{
    register int Result = 0;
    int Sign = 1;
    
    if (*string == '+')
	string++;
    else if (*string == '-')
    {
	string++;
	Sign = -1;
    }
    for (; (*string >= '0') && (*string <= '9'); string++)
    {
	Result = (Result * 10) + (*string - '0');
    }
    *NextString = string;
    if (Sign >= 0)
	return (Result);
    else
	return (-Result);
}

#define NoValue		0x0000
#define XValue  	0x0001
#define YValue		0x0002
#define WidthValue  	0x0004
#define HeightValue  	0x0008
#define AllValues 	0x000F
#define XNegative 	0x0010
#define YNegative 	0x0020


#if NeedFunctionPrototypes
#if defined(_VISUALC_)
extern Export int XParseGeometry (
#else
int XParseGeometry (
#endif
_Xconst char *string,
int *x,
int *y,
unsigned int *width,    /* RETURN */
unsigned int *height)    /* RETURN */
#else
int XParseGeometry (string, x, y, width, height)
char *string;
int *x, *y;
unsigned int *width, *height;    /* RETURN */
#endif
{
	int mask = NoValue;
	register char *strind;
	unsigned int tempWidth, tempHeight;
	int tempX, tempY;
	char *nextCharacter;

	if ( (string == NULL) || (*string == '\0')) return(mask);
	if (*string == '=')
		string++;  /* ignore possible '=' at beg of geometry spec */

	strind = (char *)string;
	if (*strind != '+' && *strind != '-' && *strind != 'x') {
		tempWidth = ReadInteger(strind, &nextCharacter);
		if (strind == nextCharacter) 
		    return (0);
		strind = nextCharacter;
		mask |= WidthValue;
	}

	if (*strind == 'x' || *strind == 'X') {	
		strind++;
		tempHeight = ReadInteger(strind, &nextCharacter);
		if (strind == nextCharacter)
		    return (0);
		strind = nextCharacter;
		mask |= HeightValue;
	}

	if ((*strind == '+') || (*strind == '-')) {
		if (*strind == '-') {
  			strind++;
			tempX = -ReadInteger(strind, &nextCharacter);
			if (strind == nextCharacter)
			    return (0);
			strind = nextCharacter;
			mask |= XNegative;

		}
		else
		{	strind++;
			tempX = ReadInteger(strind, &nextCharacter);
			if (strind == nextCharacter)
			    return(0);
			strind = nextCharacter;
		}
		mask |= XValue;
		if ((*strind == '+') || (*strind == '-')) {
			if (*strind == '-') {
				strind++;
				tempY = -ReadInteger(strind, &nextCharacter);
				if (strind == nextCharacter)
			    	    return(0);
				strind = nextCharacter;
				mask |= YNegative;

			}
			else
			{
				strind++;
				tempY = ReadInteger(strind, &nextCharacter);
				if (strind == nextCharacter)
			    	    return(0);
				strind = nextCharacter;
			}
			mask |= YValue;
		}
	}
	
	/* If strind isn't at the end of the string the it's an invalid
		geometry specification. */

	if (*strind != '\0') return (0);

	if (mask & XValue)
	    *x = tempX;
 	if (mask & YValue)
	    *y = tempY;
	if (mask & WidthValue)
            *width = tempWidth;
	if (mask & HeightValue)
            *height = tempHeight;
	return (mask);
}

XPeekEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XEvent*	b	/* event_return */
#endif
){}

XPeekIfEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XEvent*	b	/* event_return */,
    Bool (* c) (
#if NeedNestedPrototypes
	       Display* x		/* display */,
               XEvent* y		/* event */,
               XPointer	z	/* arg */
#endif
             )		/* predicate */,
    XPointer d		/* arg */
#endif
){}

int XPending(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XPlanesOfScreen(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
    
#endif
){}

int XProtocolRevision(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

int XProtocolVersion(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}


XPutBackEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XEvent*	b	/* event */
#endif
){}

XPutImage(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    XImage*	f	/* image */,
    int	g		/* src_x */,
    int	h		/* src_y */,
    int	i		/* dest_x */,
    int	j		/* dest_y */,
    unsigned int k	/* width */,
    unsigned int l	/* height */	  
#endif
){}

int XQLength(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

Status XQueryBestCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    unsigned int e       /* width */,
    unsigned int f	/* height */,
    unsigned int* g	/* width_return */,
    unsigned int* h	/* height_return */
#endif
){}

Status XQueryBestSize(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* class */,
    Drawable d		/* which_screen */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned int* g	/* width_return */,
    unsigned int* h	/* height_return */
#endif
){}

Status XQueryBestStipple(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* which_screen */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned int* g	/* width_return */,
    unsigned int* h	/* height_return */
#endif
){}

Status XQueryBestTile(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* which_screen */,
    unsigned int e	/* width */,
    unsigned int f	/* height */,
    unsigned int* g	/* width_return */,
    unsigned int* h	/* height_return */
#endif
){}

XQueryColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    XColor*	c	/* def_in_out */
#endif
){}

XQueryColors(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    XColor*	c	/* defs_in_out */,
    int	d		/* ncolors */
#endif
){}

Bool XQueryExtension(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* name */,
    int* c		/* major_opcode_return */,
    int* d		/* first_event_return */,
    int* e		/* first_error_return */
#endif
){}

XQueryKeymap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    char b[32]		/* keys_return */
#endif
){}

Bool XQueryPointer(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Window*	c	/* root_return */,
    Window*	d	/* child_return */,
    int* e		/* root_x_return */,
    int* f		/* root_y_return */,
    int* g		/* win_x_return */,
    int* h		/* win_y_return */,
    unsigned int*  i     /* mask_return */
#endif
){}

XQueryTextExtents(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	b		/* font_ID */,
    _Xconst char* c	/* string */,
    int	d		/* nchars */,
    int* e		/* direction_return */,
    int* f		/* font_ascent_return */,
    int* g		/* font_descent_return */,
    XCharStruct* h	/* overall_return */    
#endif
){}

XQueryTextExtents16(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	b		/* font_ID */,
    _Xconst XChar2b* c	/* string */,
    int	d		/* nchars */,
    int* e		/* direction_return */,
    int* f		/* font_ascent_return */,
    int* g		/* font_descent_return */,
    XCharStruct* h	/* overall_return */
#endif
){}

Status XQueryTree(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Window*	c	/* root_return */,
    Window*	d	/* parent_return */,
    Window** e		/* children_return */,
    unsigned int* f	/* nchildren_return */
#endif
){}

XRaiseWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */
#endif
){}

int XReadBitmapFile(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d 		/* d */,
    _Xconst char* e	/* filename */,
    unsigned int* f	/* width_return */,
    unsigned int* g	/* height_return */,
    Pixmap*	h	/* bitmap_return */,
    int* i		/* x_hot_return */,
    int* j		/* y_hot_return */
#endif
){}

int XReadBitmapFileData(
#if NeedFunctionPrototypes
    _Xconst char* a	/* filename */,
    unsigned int* b	/* width_return */,
    unsigned int* c	/* height_return */,
    unsigned char**	d /* data_return */,
    int* e		/* x_hot_return */,
    int* f		/* y_hot_return */
#endif
){}

XRebindKeysym(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    KeySym b		/* keysym */,
    KeySym*	c	/* list */,
    int	d		/* mod_count */,
    _Xconst unsigned char* e	/* string */,
    int	f		/* bytes_string */
#endif
){}

XRecolorCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Cursor b		/* cursor */,
    XColor*	c	/* foreground_color */,
    XColor*	d	/* background_color */
#endif
){}

XRefreshKeyboardMapping(
#if NeedFunctionPrototypes
    XMappingEvent* a	/* event_map */    
#endif
){}

XRemoveFromSaveSet(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XRemoveHost(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XHostAddress* b	/* host */
#endif
){}

XRemoveHosts(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XHostAddress* b	/* hosts */,
    int	c		/* num_hosts */
#endif
){}

XReparentWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Window c		/* parent */,
    int	d		/* x */,
    int	e		/* y */
#endif
){}

XResetScreenSaver(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XResizeWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    unsigned int c	/* width */,
    unsigned int d	/* height */
#endif
){}

XRestackWindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window*	b	/* windows */,
    int	c		/* nwindows */
#endif
){}

XRotateBuffers(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* rotate */
#endif
){}

XRotateWindowProperties(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Atom* c		/* properties */,
    int	d		/* num_prop */,
    int	e		/* npositions */
#endif
){}

int XScreenCount(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XSelectInput(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    long c		/* event_mask */
#endif
){}

Status XSendEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Bool c		/* propagate */,
    long d		/* event_mask */,
    XEvent*	e	/* event_send */
#endif
){}

XSetAccessControl(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* mode */
#endif
){}

XSetArcMode(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	f		/* arc_mode */
#endif
){}

XSetBackground(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    unsigned long g	/* background */
#endif
){}

XSetClipMask(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    Pixmap f		/* pixmap */
#endif
){}

XSetClipOrigin(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	f		/* clip_x_origin */,
    int	g		/* clip_y_origin */
#endif
){}

XSetClipRectangles(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	b		/* clip_x_origin */,
    int	c		/* clip_y_origin */,
    XRectangle*	d	/* rectangles */,
    int	x		/* n */,
    int	f		/* ordering */
#endif
){}

XSetCloseDownMode(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* close_mode */
#endif
){}

XSetCommand(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    char**	c	/* argv */,
    int	d		/* argc */
#endif
){}

XSetDashes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	b		/* dash_offset */,
    _Xconst char* c	/* dash_list */,
    int	d		/* n */
#endif
){}

XSetFillRule(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	f		/* fill_rule */
#endif
){}

XSetFillStyle(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	b		/* fill_style */
#endif
){}

XSetFont(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    Font b		/* font */
#endif
){}

XSetFontPath(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    char**	b	/* directories */,
    int	c		/* ndirs */	     
#endif
){}

XSetForeground(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    unsigned long b	/* foreground */
#endif
){}

XSetFunction(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	b		/* function */
#endif
){}

XSetGraphicsExposures(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    Bool b		/* graphics_exposures */
#endif
){}

XSetIconName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    _Xconst char* c	/* icon_name */
#endif
){}

XSetInputFocus(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* focus */,
    int	c		/* revert_to */,
    Time d		/* time */
#endif
){}

XSetLineAttributes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    unsigned int b	/* line_width */,
    int	c		/* line_style */,
    int	d		/* cap_style */,
    int	f		/* join_style */
#endif
){}

int XSetModifierMapping(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XModifierKeymap* b	/* modmap */
#endif
){}

XSetPlaneMask(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    unsigned long b	/* plane_mask */
#endif
){}

int XSetPointerMapping(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst unsigned char* b	/* map */,
    int	 c		/* nmap */
#endif
){}

XSetScreenSaver(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* timeout */,
    int	c		/* interval */,
    int	d		/* prefer_blanking */,
    int	e		/* allow_exposures */
#endif
){}

XSetSelectionOwner(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Atom b	        /* selection */,
    Window c		/* owner */,
    Time d		/* time */
#endif
){}

XSetState(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC x			/* gc */,
    unsigned long b	/* foreground */,
    unsigned long c	/* background */,
    int	d		/* function */,
    unsigned long e	/* plane_mask */
#endif
){}

XSetStipple(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    Pixmap	b	/* stipple */
#endif
){}

XSetSubwindowMode(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	b		/* subwindow_mode */
#endif
){}

XSetTSOrigin(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    int	b		/* ts_x_origin */,
    int	c		/* ts_y_origin */
#endif
){}

XSetTile(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    Pixmap	b	/* tile */
#endif
){}

XSetWindowBackground(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window	b	/* w */,
    unsigned long c	/* background_pixel */
#endif
){}

XSetWindowBackgroundPixmap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Pixmap c		/* background_pixmap */
#endif
){}

XSetWindowBorder(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window c		/* w */,
    unsigned long d	/* border_pixel */
#endif
){}

XSetWindowBorderPixmap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Pixmap c		/* border_pixmap */
#endif
){}

XSetWindowBorderWidth(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    unsigned int c	/* width */
#endif
){}

XSetWindowColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    Colormap c		/* colormap */
#endif
){}

XStoreBuffer(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* bytes */,
    int	c		/* nbytes */,
    int	d		/* buffer */
#endif
){}

XStoreBytes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* bytes */,
    int	c		/* nbytes */
#endif
){}

XStoreColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    XColor*	c	/* color */
#endif
){}

XStoreColors(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    XColor*	c	/* color */,
    int	d		/* ncolors */
#endif
){}

XStoreName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    _Xconst char* c	/* window_name */
#endif
){}

XStoreNamedColor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */,
    _Xconst char* c	/* color */,
    unsigned long d	/* pixel */,
    int	e		/* flags */
#endif
){}

#if defined(_VISUALC_)
extern Export XSync(Display* a,Bool b)
#else
XSync(Display* a,Bool b)
#endif
{}

XTextExtents(
#if NeedFunctionPrototypes
    XFontStruct* a	/* font_struct */,
    _Xconst char* b	/* string */,
    int	c		/* nchars */,
    int* d		/* direction_return */,
    int* e		/* font_ascent_return */,
    int* f		/* font_descent_return */,
    XCharStruct* g	/* overall_return */
#endif
){}

XTextExtents16(
#if NeedFunctionPrototypes
    XFontStruct* a	/* font_struct */,
    _Xconst XChar2b* b	/* string */,
    int	c		/* nchars */,
    int* d		/* direction_return */,
    int* e		/* font_ascent_return */,
    int* f		/* font_descent_return */,
    XCharStruct* g	/* overall_return */
#endif
){}

int XTextWidth(
#if NeedFunctionPrototypes
    XFontStruct* a	/* font_struct */,
    _Xconst char* b	/* string */,
    int	c		/* count */
#endif
){}

int XTextWidth16(
#if NeedFunctionPrototypes
    XFontStruct* a	/* font_struct */,
    _Xconst XChar2b* b	/* string */,
    int	c		/* count */
#endif
){}

Bool XTranslateCoordinates(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* src_w */,
    Window c		/* dest_w */,
    int	d		/* src_x */,
    int	e		/* src_y */,
    int* f		/* dest_x_return */,
    int* g		/* dest_y_return */,
    Window*	h	/* child_return */
#endif
){}

XUndefineCursor(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XUngrabButton(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    unsigned int b	/* button */,
    unsigned int c	/* modifiers */,
    Window	d	/* grab_window */
#endif
){}

XUngrabKey(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* keycode */,
    unsigned int c	/* modifiers */,
    Window d		/* grab_window */
#endif
){}

XUngrabKeyboard(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Time b		/* time */
#endif
){}

XUngrabPointer(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Time b		/* time */
#endif
){}

XUngrabServer(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XUninstallColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Colormap b		/* colormap */
#endif
){}

XUnloadFont(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Font b		/* font */
#endif
){}

XUnmapSubwindows(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

XUnmapWindow(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */
#endif
){}

int XVendorRelease(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XWarpPointer(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* src_w */,
    Window c		/* dest_w */,
    int	d		/* src_x */,
    int	e		/* src_y */,
    unsigned int f	/* src_width */,
    unsigned int g	/* src_height */,
    int	h		/* dest_x */,
    int	i		/* dest_y */	     
#endif
){}

int XWidthMMOfScreen(
#if NeedFunctionPrototypes
    Screen* c		/* screen */
#endif
){}

int XWidthOfScreen(
#if NeedFunctionPrototypes
    Screen*	c	/* screen */
#endif
){}

XWindowEvent(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    long c		/* event_mask */,
    XEvent*	d	/* event_return */
#endif
){}

int XWriteBitmapFile(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* filename */,
    Pixmap c		/* bitmap */,
    unsigned int d	/* width */,
    unsigned int e	/* height */,
    int	f		/* x_hot */,
    int	g		/* y_hot */		     
#endif
){}

Bool XSupportsLocale(
#if NeedFunctionPrototypes
    void *a
#endif
){}

char *XSetLocaleModifiers(
#if NeedFunctionPrototypes
    _Xconst char* a	/* modifier_list */
#endif
){}

XOM XOpenOM(
#if NeedFunctionPrototypes
    Display* a			/* display */,
    struct _XrmHashBucketRec* b	/* rdb */,
    _Xconst char* c		/* res_name */,
    _Xconst char* d		/* res_class */
#endif
){}

Status XCloseOM(
#if NeedFunctionPrototypes
    XOM a			/* om */
#endif
){}

char *XSetOMValues(
#if NeedVarargsPrototypes
    XOM a			/* om */,
    ...
#endif
){}

char *XGetOMValues(
#if NeedVarargsPrototypes
    XOM a			/* om */,
    ...
#endif
){}

Display *XDisplayOfOM(
#if NeedFunctionPrototypes
    XOM a			/* om */
#endif
){}

char *XLocaleOfOM(
#if NeedFunctionPrototypes
    XOM a			/* om */
#endif
){}

XOC XCreateOC(
#if NeedVarargsPrototypes
    XOM a			/* om */,
    ...
#endif
){}

void XDestroyOC(
#if NeedFunctionPrototypes
    XOC a			/* oc */
#endif
){}

XOM XOMOfOC(
#if NeedFunctionPrototypes
    XOC a			/* oc */
#endif
){}

char *XSetOCValues(
#if NeedVarargsPrototypes
    XOC a			/* oc */,
    ...
#endif
){}

char *XGetOCValues(
#if NeedVarargsPrototypes
    XOC a			/* oc */,
    ...
#endif
){}

XFontSet XCreateFontSet(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    _Xconst char* b	/* base_font_name_list */,
    char***	c	/* missing_charset_list */,
    int* d		/* missing_charset_count */,
    char** e		/* def_string */
#endif
){}

void XFreeFontSet(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XFontSet b		/* font_set */
#endif
){}

int XFontsOfFontSet(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    XFontStruct*** b	/* font_struct_list */,
    char***	c	/* font_name_list */
#endif
){}

char *XBaseFontNameListOfFontSet(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */
#endif
){}

char *XLocaleOfFontSet(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */
#endif
){}

Bool XContextDependentDrawing(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */
#endif
){}

Bool XDirectionalDependentDrawing(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */
#endif
){}

Bool XContextualDrawing(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */
#endif
){}

XFontSetExtents *XExtentsOfFontSet(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */
#endif
){}

int XmbTextEscapement(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    _Xconst char* b	/* text */,
    int	c		/* bytes_text */
#endif
){}

int XwcTextEscapement(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    _Xconst wchar_t* b	/* text */,
    int	c		/* num_wchars */
#endif
){}

int XmbTextExtents(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    _Xconst char* b	/* text */,
    int	c		/* bytes_text */,
    XRectangle*	d	/* overall_ink_return */,
    XRectangle*	e	/* overall_logical_return */
#endif
){}

int XwcTextExtents(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    _Xconst wchar_t* b	/* text */,
    int	c		/* num_wchars */,
    XRectangle*	d	/* overall_ink_return */,
    XRectangle*	e	/* overall_logical_return */
#endif
){}

Status XmbTextPerCharExtents(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    _Xconst char* b	/* text */,
    int	c		/* bytes_text */,
    XRectangle*	d	/* ink_extents_buffer */,
    XRectangle*	e	/* logical_extents_buffer */,
    int	f		/* buffer_size */,
    int* g		/* num_chars */,
    XRectangle*	h	/* overall_ink_return */,
    XRectangle*	i	/* overall_logical_return */
#endif
){}

Status XwcTextPerCharExtents(
#if NeedFunctionPrototypes
    XFontSet a		/* font_set */,
    _Xconst wchar_t* b	/* text */,
    int	c		/* num_wchars */,
    XRectangle*	d	/* ink_extents_buffer */,
    XRectangle*	e	/* logical_extents_buffer */,
    int	f		/* buffer_size */,
    int* g		/* num_chars */,
    XRectangle*	h	/* overall_ink_return */,
    XRectangle*	i	/* overall_logical_return */
#endif
){}

void XmbDrawText(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	x		/* x */,
    int	y		/* y */,
    XmbTextItem* z	/* text_items */,
    int	w		/* nitems */
#endif
){}

void XwcDrawText(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    GC e			/* gc */,
    int	x		/* x */,
    int	y		/* y */,
    XwcTextItem* z	/* text_items */,
    int	w		/* nitems */
#endif
){}

void XmbDrawString(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    XFontSet f		/* font_set */,
    GC e			/* gc */,
    int	x		/* x */,
    int	y		/* y */,
    _Xconst char* z	/* text */,
    int	w		/* bytes_text */
#endif
){}

void XwcDrawString(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    XFontSet f		/* font_set */,
    GC e			/* gc */,
    int	x		/* x */,
    int	y		/* y */,
    _Xconst wchar_t* z	/* text */,
    int	w		/* num_wchars */
#endif
){}

void XmbDrawImageString(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    XFontSet f		/* font_set */,
    GC e			/* gc */,
    int	x		/* x */,
    int	y		/* y */,
    _Xconst char* z	/* text */,
    int	w		/* bytes_text */
#endif
){}

void XwcDrawImageString(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Drawable d		/* d */,
    XFontSet f		/* font_set */,
    GC e			/* gc */,
    int	x		/* x */,
    int	y		/* y */,
    _Xconst wchar_t* z	/* text */,
    int	w		/* num_wchars */
#endif
){}

XIM XOpenIM(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    struct _XrmHashBucketRec* b	/* rdb */,
    char* c			/* res_name */,
    char* d			/* res_class */
#endif
){}

Status XCloseIM(
#if NeedFunctionPrototypes
    XIM a /* im */
#endif
){}

char *XGetIMValues(
#if NeedVarargsPrototypes
    XIM a /* im */, ...
#endif
){}

Display *XDisplayOfIM(
#if NeedFunctionPrototypes
    XIM a /* im */
#endif
){}

char *XLocaleOfIM(
#if NeedFunctionPrototypes
    XIM a /* im*/
#endif
){}

XIC XCreateIC(
#if NeedVarargsPrototypes
    XIM a /* im */, ...
#endif
){}

void XDestroyIC(
#if NeedFunctionPrototypes
    XIC a /* ic */
#endif
){}

void XSetICFocus(
#if NeedFunctionPrototypes
    XIC a /* ic */
#endif
){}

void XUnsetICFocus(
#if NeedFunctionPrototypes
    XIC a /* ic */
#endif
){}

wchar_t *XwcResetIC(
#if NeedFunctionPrototypes
    XIC a /* ic */
#endif
){}

char *XmbResetIC(
#if NeedFunctionPrototypes
    XIC a /* ic */
#endif
){}

char *XSetICValues(
#if NeedVarargsPrototypes
    XIC a /* ic */, ...
#endif
){}

char *XGetICValues(
#if NeedVarargsPrototypes
    XIC a /* ic */, ...
#endif
){}

XIM XIMOfIC(
#if NeedFunctionPrototypes
    XIC a /* ic */
#endif
){}

Bool XFilterEvent(
#if NeedFunctionPrototypes
    XEvent*	a /* event */,
    Window b	/* window */
#endif
){}

int XmbLookupString(
#if NeedFunctionPrototypes
    XIC a			/* ic */,
    XKeyPressedEvent* b	/* event */,
    char* c		/* buffer_return */,
    int	d		/* bytes_buffer */,
    KeySym*	e	/* keysym_return */,
    Status*	f	/* status_return */
#endif
){}

int XwcLookupString(
#if NeedFunctionPrototypes
    XIC a			/* ic */,
    XKeyPressedEvent* b	/* event */,
    wchar_t* c		/* buffer_return */,
    int	d		/* wchars_buffer */,
    KeySym*	e	/* keysym_return */,
    Status*	f	/* status_return */
#endif
){}

XVaNestedList XVaCreateNestedList(
#if NeedVarargsPrototypes
    int a /*unused*/, ...
#endif
){}

/* internal connections for IMs */

Bool XRegisterIMInstantiateCallback(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    struct _XrmHashBucketRec* b	/* rdb */,
    char* c			/* res_name */,
    char* d			/* res_class */,
    XIMProc	e		/* callback */,
    XPointer* f			/* client_data */
#endif
){}

Bool XUnregisterIMInstantiateCallback(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    struct _XrmHashBucketRec* x	/* rdb */,
    char* b			/* res_name */,
    char* c			/* res_class */,
    XIMProc	d		/* callback */,
    XPointer* e			/* client_data */
#endif
){}

typedef void (*XConnectionWatchProc)(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    XPointer b			/* client_data */,
    int	c			/* fd */,
    Bool d			/* opening */,	 /* open or close flag */
    XPointer* e			/* watch_data */ /* open sets, close uses */
#endif
);
    

Status XInternalConnectionNumbers(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    int** b			/* fd_return */,
    int* c			/* count_return */
#endif
){}

void XProcessInternalConnection(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    int	b			/* fd */
#endif
){}

Status XAddConnectionWatch(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    XConnectionWatchProc b	/* callback */,
    XPointer c			/* client_data */
#endif
){}

void XRemoveConnectionWatch(
#if NeedFunctionPrototypes
    Display* a			/* dpy */,
    XConnectionWatchProc b	/* callback */,
    XPointer c			/* client_data */
#endif
){}

/* * * * * * */


/* 
 * Bitmask returned by XParseGeometry().  Each bit tells if the corresponding
 * value (x, y, width, height) was found in the parsed string.
 */
#define NoValue		0x0000
#define XValue  	0x0001
#define YValue		0x0002
#define WidthValue  	0x0004
#define HeightValue  	0x0008
#define AllValues 	0x000F
#define XNegative 	0x0010
#define YNegative 	0x0020

/*
 * new version containing base_width, base_height, and win_gravity fields;
 * used with WM_NORMAL_HINTS.
 */
typedef struct {
    	long flags;	/* marks which fields in this structure are defined */
	int x, y;		/* obsolete for new window mgrs, but clients */
	int width, height;	/* should set so old wm's don't mess up */
	int min_width, min_height;
	int max_width, max_height;
    	int width_inc, height_inc;
	struct {
		int x;	/* numerator */
		int y;	/* denominator */
	} min_aspect, max_aspect;
	int base_width, base_height;		/* added by ICCCM version 1 */
	int win_gravity;			/* added by ICCCM version 1 */
} XSizeHints;

/*
 * The next block of definitions are for window manager properties that
 * clients and applications use for communication.
 */

/* flags argument in size hints */
#define USPosition	(1L << 0) /* user specified x, y */
#define USSize		(1L << 1) /* user specified width, height */

#define PPosition	(1L << 2) /* program specified position */
#define PSize		(1L << 3) /* program specified size */
#define PMinSize	(1L << 4) /* program specified minimum size */
#define PMaxSize	(1L << 5) /* program specified maximum size */
#define PResizeInc	(1L << 6) /* program specified resize increments */
#define PAspect		(1L << 7) /* program specified min and max aspect ratios */
#define PBaseSize	(1L << 8) /* program specified base for incrementing */
#define PWinGravity	(1L << 9) /* program specified window gravity */

/* obsolete */
#define PAllHints (PPosition|PSize|PMinSize|PMaxSize|PResizeInc|PAspect)



typedef struct {
	long flags;	/* marks which fields in this structure are defined */
	Bool input;	/* does this application rely on the window manager to
			get keyboard input? */
	int initial_state;	/* see below */
	Pixmap icon_pixmap;	/* pixmap to be used as icon */
	Window icon_window; 	/* window to be used as icon */
	int icon_x, icon_y; 	/* initial position of icon */
	Pixmap icon_mask;	/* icon mask bitmap */
	XID window_group;	/* id of related window group */
	/* this structure may be extended in the future */
} XWMHints;

/* definition for flags of XWMHints */

#define InputHint 		(1L << 0)
#define StateHint 		(1L << 1)
#define IconPixmapHint		(1L << 2)
#define IconWindowHint		(1L << 3)
#define IconPositionHint 	(1L << 4)
#define IconMaskHint		(1L << 5)
#define WindowGroupHint		(1L << 6)
#define AllHints (InputHint|StateHint|IconPixmapHint|IconWindowHint| \
IconPositionHint|IconMaskHint|WindowGroupHint)
#define XUrgencyHint		(1L << 8)

/* definitions for initial window state */
#define WithdrawnState 0	/* for windows that are not mapped */
#define NormalState 1	/* most applications want to start this way */
#define IconicState 3	/* application wants to start as an icon */

/*
 * Obsolete states no longer defined by ICCCM
 */
#define DontCareState 0	/* don't know or care */
#define ZoomState 2	/* application wants to start zoomed */
#define InactiveState 4	/* application believes it is seldom used; */
			/* some wm's may put it on inactive menu */


/*
 * new structure for manipulating TEXT properties; used with WM_NAME, 
 * WM_ICON_NAME, WM_CLIENT_MACHINE, and WM_COMMAND.
 */
typedef struct {
    unsigned char *value;		/* same as Property routines */
    Atom encoding;			/* prop type */
    int format;				/* prop data format: 8, 16, or 32 */
    unsigned long nitems;		/* number of data items in value */
} XTextProperty;

#define XNoMemory -1
#define XLocaleNotSupported -2
#define XConverterNotFound -3

typedef enum {
    XStringStyle,		/* STRING */
    XCompoundTextStyle,		/* COMPOUND_TEXT */
    XTextStyle,			/* text in owner's encoding (current locale)*/
    XStdICCTextStyle		/* STRING, else COMPOUND_TEXT */
} XICCEncodingStyle;

typedef struct {
	int min_width, min_height;
	int max_width, max_height;
	int width_inc, height_inc;
} XIconSize;

typedef struct {
	char *res_name;
	char *res_class;
} XClassHint;

/*
 * These macros are used to give some sugar to the image routines so that
 * naive people are more comfortable with them.
 */
#define XDestroyImage(ximage) \
	((*((ximage)->f.destroy_image))((ximage)))
#define XGetPixel(ximage, x, y) \
	((*((ximage)->f.get_pixel))((ximage), (x), (y)))
#define XPutPixel(ximage, x, y, pixel) \
	((*((ximage)->f.put_pixel))((ximage), (x), (y), (pixel)))
#define XSubImage(ximage, x, y, width, height)  \
	((*((ximage)->f.sub_image))((ximage), (x), (y), (width), (height)))
#define XAddPixel(ximage, value) \
	((*((ximage)->f.add_pixel))((ximage), (value)))

/*
 * Compose sequence status structure, used in calling XLookupString.
 */
typedef struct _XComposeStatus {
    XPointer compose_ptr;	/* state table pointer */
    int chars_matched;		/* match state */
} XComposeStatus;

/*
 * Keysym macros, used on Keysyms to test for classes of symbols
 */
#define IsKeypadKey(keysym) \
  (((KeySym)(keysym) >= XK_KP_Space) && ((KeySym)(keysym) <= XK_KP_Equal))

#define IsPrivateKeypadKey(keysym) \
  (((KeySym)(keysym) >= 0x11000000) && ((KeySym)(keysym) <= 0x1100FFFF))

#define IsCursorKey(keysym) \
  (((KeySym)(keysym) >= XK_Home)     && ((KeySym)(keysym) <  XK_Select))

#define IsPFKey(keysym) \
  (((KeySym)(keysym) >= XK_KP_F1)     && ((KeySym)(keysym) <= XK_KP_F4))

#define IsFunctionKey(keysym) \
  (((KeySym)(keysym) >= XK_F1)       && ((KeySym)(keysym) <= XK_F35))

#define IsMiscFunctionKey(keysym) \
  (((KeySym)(keysym) >= XK_Select)   && ((KeySym)(keysym) <= XK_Break))

#define IsModifierKey(keysym) \
  ((((KeySym)(keysym) >= XK_Shift_L) && ((KeySym)(keysym) <= XK_Hyper_R)) \
   || ((KeySym)(keysym) == XK_Mode_switch) \
   || ((KeySym)(keysym) == XK_Num_Lock))
/*
 * opaque reference to Region data type 
 */
typedef struct _XRegion *Region; 

/* Return values from XRectInRegion() */
 
#define RectangleOut 0
#define RectangleIn  1
#define RectanglePart 2
 

/*
 * Information used by the visual utility routines to find desired visual
 * type from the many visuals a display may support.
 */

typedef struct {
  Visual *visual;
  VisualID visualid;
  int screen;
  int depth;
#if defined(__cplusplus) || defined(c_plusplus)
  int c_class;					/* C++ */
#else
  int class;
#endif
  unsigned long red_mask;
  unsigned long green_mask;
  unsigned long blue_mask;
  int colormap_size;
  int bits_per_rgb;
} XVisualInfo;

#define VisualNoMask		0x0
#define VisualIDMask 		0x1
#define VisualScreenMask	0x2
#define VisualDepthMask		0x4
#define VisualClassMask		0x8
#define VisualRedMaskMask	0x10
#define VisualGreenMaskMask	0x20
#define VisualBlueMaskMask	0x40
#define VisualColormapSizeMask	0x80
#define VisualBitsPerRGBMask	0x100
#define VisualAllMask		0x1FF

/*
 * This defines a window manager property that clients may use to
 * share standard color maps of type RGB_COLOR_MAP:
 */
typedef struct {
	Colormap colormap;
	unsigned long red_max;
	unsigned long red_mult;
	unsigned long green_max;
	unsigned long green_mult;
	unsigned long blue_max;
	unsigned long blue_mult;
	unsigned long base_pixel;
	VisualID visualid;		/* added by ICCCM version 1 */
	XID killid;			/* added by ICCCM version 1 */
} XStandardColormap;

#define ReleaseByFreeingColormap ((XID) 1L)  /* for killid field above */


/*
 * return codes for XReadBitmapFile and XWriteBitmapFile
 */
#define BitmapSuccess		0
#define BitmapOpenFailed 	1
#define BitmapFileInvalid 	2
#define BitmapNoMemory		3

/****************************************************************
 *
 * Context Management
 *
 ****************************************************************/


/* Associative lookup table return codes */

#define XCSUCCESS 0	/* No error. */
#define XCNOMEM   1    /* Out of memory */
#define XCNOENT   2    /* No entry in table */

typedef int XContext;

#define XUniqueContext()       ((XContext) XrmUniqueQuark())
#define XStringToContext(string)   ((XContext) XrmStringToQuark(string))

/* The following declarations are alphabetized. */

XClassHint *XAllocClassHint (
#if NeedFunctionPrototypes
    void
#endif
){}

XIconSize *XAllocIconSize (
#if NeedFunctionPrototypes
    void
#endif
){}

XSizeHints *XAllocSizeHints (
#if NeedFunctionPrototypes
    void
#endif
){}

XStandardColormap *XAllocStandardColormap (
#if NeedFunctionPrototypes
    void
#endif
){}

XWMHints *XAllocWMHints (
#if NeedFunctionPrototypes
    void
#endif
){}

XClipBox(
#if NeedFunctionPrototypes
    Region a		/* r */,
    XRectangle*	b	/* rect_return */
#endif
){}

Region XCreateRegion(
#if NeedFunctionPrototypes
    void *a
#endif
){}

char *XDefaultString(
#if NeedFunctionPrototypes
    void *a
#endif
){}

int XDeleteContext(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	 b		/* rid */,
    XContext c		/* context */
#endif
){}

XDestroyRegion(
#if NeedFunctionPrototypes
    Region a		/* r */
#endif
){}

XEmptyRegion(
#if NeedFunctionPrototypes
    Region a		/* r */
#endif
){}

XEqualRegion(
#if NeedFunctionPrototypes
    Region a		/* r1 */,
    Region b		/* r2 */
#endif
){}

int XFindContext(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	b		/* rid */,
    XContext c		/* context */,
    XPointer* d		/* data_return */
#endif
){}

Status XGetClassHint(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XClassHint*	c	/* class_hints_return */
#endif
){}

Status XGetIconSizes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XIconSize**	c	/* size_list_return */,
    int* d		/* count_return */
#endif
){}

Status XGetNormalHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints_return */
#endif
){}

Status XGetRGBColormaps(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XStandardColormap** d /* stdcmap_return */,
    int* e		/* count_return */,
    Atom f		/* property */
#endif
){}

Status XGetSizeHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints_return */,
    Atom d		/* property */
#endif
){}

Status XGetStandardColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XStandardColormap* c	/* colormap_return */,
    Atom d		/* property */			    
#endif
){}

Status XGetTextProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* window */,
    XTextProperty* c	/* text_prop_return */,
    Atom d		/* property */
#endif
){}

XVisualInfo *XGetVisualInfo(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    long b		/* vinfo_mask */,
    XVisualInfo* c	/* vinfo_template */,
    int* d		/* nitems_return */
#endif
){}

Status XGetWMClientMachine(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop_return */
#endif
){}

XWMHints *XGetWMHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */		      
#endif
){}

Status XGetWMIconName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop_return */
#endif
){}

Status XGetWMName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop_return */
#endif
){}

Status XGetWMNormalHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints_return */,
    long* d		/* supplied_return */ 
#endif
){}

Status XGetWMSizeHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints_return */,
    long* d		/* supplied_return */,
    Atom e		/* property */
#endif
){}

Status XGetZoomHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* zhints_return */
#endif
){}

XIntersectRegion(
#if NeedFunctionPrototypes
    Region a		/* sra */,
    Region b		/* srb */,
    Region c		/* dr_return */
#endif
){}

void XConvertCase(
#if NeedFunctionPrototypes
    KeySym	a	/* sym */,
    KeySym*	b	/* lower */,
    KeySym*	c	/* upper */
#endif
){}

int XLookupString(
#if NeedFunctionPrototypes
    XKeyEvent* a		/* event_struct */,
    char* b		/* buffer_return */,
    int	c		/* bytes_buffer */,
    KeySym*	d	/* keysym_return */,
    XComposeStatus*	e /* status_in_out */
#endif
){}

Status XMatchVisualInfo(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen */,
    int	c		/* depth */,
    int	d		/* class */,
    XVisualInfo* e	/* vinfo_return */
#endif
){}

XOffsetRegion(
#if NeedFunctionPrototypes
    Region a		/* r */,
    int	b		/* dx */,
    int	c		/* dy */
#endif
){}

Bool XPointInRegion(
#if NeedFunctionPrototypes
    Region a		/* r */,
    int	b		/* x */,
    int	c		/* y */
#endif
){}

Region XPolygonRegion(
#if NeedFunctionPrototypes
    XPoint*	a	/* points */,
    int	b		/* n */,
    int	c		/* fill_rule */
#endif
){}

int XRectInRegion(
#if NeedFunctionPrototypes
    Region a		/* r */,
    int	b		/* x */,
    int	c		/* y */,
    unsigned int d	/* width */,
    unsigned int e	/* height */
#endif
){}

int XSaveContext(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XID	b		/* rid */,
    XContext c		/* context */,
    _Xconst char* d	/* data */
#endif
){}

XSetClassHint(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XClassHint*	c	/* class_hints */
#endif
){}

XSetIconSizes(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XIconSize* c		/* size_list */,
    int	d		/* count */    
#endif
){}

XSetNormalHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints */
#endif
){}

void XSetRGBColormaps(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XStandardColormap* c	/* stdcmaps */,
    int	d		/* count */,
    Atom e		/* property */
#endif
){}

XSetSizeHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints */,
    Atom d		/* property */
#endif
){}

XSetStandardProperties(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    _Xconst char* c	/* window_name */,
    _Xconst char* d	/* icon_name */,
    Pixmap e		/* icon_pixmap */,
    char** f		/* argv */,
    int	g		/* argc */,
    XSizeHints*	h	/* hints */
#endif
){}

void XSetTextProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop */,
    Atom d		/* property */
#endif
){}

void XSetWMClientMachine(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop */
#endif
){}

XSetWMHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XWMHints* c		/* wm_hints */
#endif
){}

void XSetWMIconName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop */
#endif
){}

void XSetWMName(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* text_prop */
#endif
){}

void XSetWMNormalHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints */
#endif
){}

void XSetWMProperties(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XTextProperty* c	/* window_name */,
    XTextProperty* d	/* icon_name */,
    char** e		/* argv */,
    int	f		/* argc */,
    XSizeHints*	g	/* normal_hints */,
    XWMHints* h		/* wm_hints */,
    XClassHint*	i	/* class_hints */
#endif
){}

void XmbSetWMProperties(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    _Xconst char* c	/* window_name */,
    _Xconst char* d	/* icon_name */,
    char** e		/* argv */,
    int	f		/* argc */,
    XSizeHints*	g	/* normal_hints */,
    XWMHints* h		/* wm_hints */,
    XClassHint*	i	/* class_hints */
#endif
){}

void XSetWMSizeHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* hints */,
    Atom d		/* property */
#endif
){}

XSetRegion(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    GC e			/* gc */,
    Region x		/* r */
#endif
){}

void XSetStandardColormap(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XStandardColormap* c	/* colormap */,
    Atom d		/* property */
#endif
){}

XSetZoomHints(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    Window b		/* w */,
    XSizeHints*	c	/* zhints */
#endif
){}

XShrinkRegion(
#if NeedFunctionPrototypes
    Region x		/* r */,
    int	y		/* dx */,
    int	z		/* dy */
#endif
){}

Status XStringListToTextProperty(
#if NeedFunctionPrototypes
    char** a		/* list */,
    int	b		/* count */,
    XTextProperty* c	/* text_prop_return */
#endif
){}

XSubtractRegion(
#if NeedFunctionPrototypes
    Region x		/* sra */,
    Region y		/* srb */,
    Region z		/* dr_return */
#endif
){}

int XmbTextListToTextProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    char** b		/* list */,
    int	c		/* count */,
    XICCEncodingStyle d	/* style */,
    XTextProperty* e	/* text_prop_return */
#endif
){}

int XwcTextListToTextProperty(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    wchar_t** b		/* list */,
    int	c		/* count */,
    XICCEncodingStyle d	/* style */,
    XTextProperty* e	/* text_prop_return */
#endif
){}

void XwcFreeStringList(
#if NeedFunctionPrototypes
    wchar_t** a		/* list */
#endif
){}

Status XTextPropertyToStringList(
#if NeedFunctionPrototypes
    XTextProperty* a	/* text_prop */,
    char***	b	/* list_return */,
    int* c		/* count_return */
#endif
){}

int XmbTextPropertyToTextList(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XTextProperty* b	/* text_prop */,
    char***	c	/* list_return */,
    int* d		/* count_return */
#endif
){}

int XwcTextPropertyToTextList(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XTextProperty* b	/* text_prop */,
    wchar_t*** c		/* list_return */,
    int* d		/* count_return */
#endif
){}

XUnionRectWithRegion(
#if NeedFunctionPrototypes
    XRectangle*	a	/* rectangle */,
    Region b		/* src_region */,
    Region c		/* dest_region_return */
#endif
){}

XUnionRegion(
#if NeedFunctionPrototypes
    Region c		/* sra */,
    Region d		/* srb */,
    Region e		/* dr_return */
#endif
){}

int XWMGeometry(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    int	b		/* screen_number */,
    _Xconst char* c	/* user_geometry */,
    _Xconst char* d	/* default_geometry */,
    unsigned int e	/* border_width */,
    XSizeHints*	f	/* hints */,
    int* g		/* x_return */,
    int* h		/* y_return */,
    int* i		/* width_return */,
    int* j		/* height_return */,
    int* k		/* gravity_return */
#endif
){}

XXorRegion(
#if NeedFunctionPrototypes
    Region a		/* sra */,
    Region b		/* srb */,
    Region c		/* dr_return */
#endif
){}

/* * * * * * */

/****************************************************************
 ****************************************************************
 ***                                                          ***
 ***                                                          ***
 ***          X Resource Manager Intrinsics                   ***
 ***                                                          ***
 ***                                                          ***
 ****************************************************************
 ****************************************************************/

_XFUNCPROTOBEGIN

/****************************************************************
 *
 * Memory Management
 *
 ****************************************************************/

char *Xpermalloc(
#if NeedFunctionPrototypes
    unsigned int a	/* size */
#endif
){}

/****************************************************************
 *
 * Quark Management
 *
 ****************************************************************/

typedef int     XrmQuark, *XrmQuarkList;
#define NULLQUARK ((XrmQuark) 0)

typedef char *XrmString;
#define NULLSTRING ((XrmString) 0)

/* find quark for string, create new quark if none already exists */
XrmQuark XrmStringToQuark(
#if NeedFunctionPrototypes
    _Xconst char* a	/* string */
#endif
){}

XrmQuark XrmPermStringToQuark(
#if NeedFunctionPrototypes
    _Xconst char* a	/* string */
#endif
){}

/* find string for quark */
XrmString XrmQuarkToString(
#if NeedFunctionPrototypes
    XrmQuark a		/* quark */
#endif
){}

XrmQuark XrmUniqueQuark(
#if NeedFunctionPrototypes
    void *a
#endif
){}

#define XrmStringsEqual(a1, a2) (strcmp(a1, a2) == 0)


/****************************************************************
 *
 * Conversion of Strings to Lists
 *
 ****************************************************************/

typedef enum {XrmBindTightly, XrmBindLoosely} XrmBinding, *XrmBindingList;

void XrmStringToQuarkList(
#if NeedFunctionPrototypes
    _Xconst char* a	/* string */,
    XrmQuarkList b	/* quarks_return */
#endif
){}

void XrmStringToBindingQuarkList(
#if NeedFunctionPrototypes
    _Xconst char* a	/* string */,
    XrmBindingList b	/* bindings_return */,
    XrmQuarkList c	/* quarks_return */
#endif
){}

/****************************************************************
 *
 * Name and Class lists.
 *
 ****************************************************************/

typedef XrmQuark     XrmName;
typedef XrmQuarkList XrmNameList;
#define XrmNameToString(name)		XrmQuarkToString(name)
#define XrmStringToName(string)		XrmStringToQuark(string)
#define XrmStringToNameList(str, name)	XrmStringToQuarkList(str, name)

typedef XrmQuark     XrmClass;
typedef XrmQuarkList XrmClassList;
#define XrmClassToString(c_class)	XrmQuarkToString(c_class)
#define XrmStringToClass(c_class)	XrmStringToQuark(c_class)
#define XrmStringToClassList(str,c_class) XrmStringToQuarkList(str, c_class)



/****************************************************************
 *
 * Resource Representation Types and Values
 *
 ****************************************************************/

typedef XrmQuark     XrmRepresentation;
#define XrmStringToRepresentation(string)   XrmStringToQuark(string)
#define	XrmRepresentationToString(type)   XrmQuarkToString(type)

typedef struct {
    unsigned int    size;
    XPointer	    addr;
} XrmValue, *XrmValuePtr;


/****************************************************************
 *
 * Resource Manager Functions
 *
 ****************************************************************/

typedef struct _XrmHashBucketRec *XrmHashBucket;
typedef XrmHashBucket *XrmHashTable;
typedef XrmHashTable XrmSearchList[];
typedef struct _XrmHashBucketRec *XrmDatabase;


void XrmDestroyDatabase(
#if NeedFunctionPrototypes
    XrmDatabase a		/* database */    
#endif
){}

void XrmQPutResource(
#if NeedFunctionPrototypes
    XrmDatabase* a	/* database */,
    XrmBindingList b	/* bindings */,
    XrmQuarkList c	/* quarks */,
    XrmRepresentation d	/* type */,
    XrmValue* e		/* value */
#endif
){}

void XrmPutResource(
#if NeedFunctionPrototypes
    XrmDatabase* a	/* database */,
    _Xconst char* b	/* specifier */,
    _Xconst char* c	/* type */,
    XrmValue* d		/* value */
#endif
){}

void XrmQPutStringResource(
#if NeedFunctionPrototypes
    XrmDatabase* a	/* database */,
    XrmBindingList b     /* bindings */,
    XrmQuarkList c	/* quarks */,
    _Xconst char* d	/* value */
#endif
){}

void XrmPutStringResource(
#if NeedFunctionPrototypes
    XrmDatabase* a	/* database */,
    _Xconst char* b	/* specifier */,
    _Xconst char* c	/* value */
#endif
){}

void XrmPutLineResource(
#if NeedFunctionPrototypes
    XrmDatabase* a	/* database */,
    _Xconst char* b	/* line */
#endif
){}

 XrmQGetResource(
#if NeedFunctionPrototypes
    XrmDatabase	a	/* database */,
    XrmNameList	b	/* quark_name */,
    XrmClassList c	/* quark_class */,
    XrmRepresentation* d	/* quark_type_return */,
    XrmValue* e		/* value_return */
#endif
){}

Bool XrmGetResource(
#if NeedFunctionPrototypes
    XrmDatabase	a	/* database */,
    _Xconst char* b	/* str_name */,
    _Xconst char* c	/* str_class */,
    char**	d	/* str_type_return */,
    XrmValue*	e	/* value_return */
#endif
)
{
  return(0);
}

Bool XrmQGetSearchList(
#if NeedFunctionPrototypes
    XrmDatabase	a	/* database */,
    XrmNameList	b	/* names */,
    XrmClassList c	/* classes */,
    XrmSearchList d	/* list_return */,
    int	e		/* list_length */
#endif
){}

Bool XrmQGetSearchResource(
#if NeedFunctionPrototypes
    XrmSearchList a	/* list */,
    XrmName	b	/* name */,
    XrmClass c		/* class */,
    XrmRepresentation* d	/* type_return */,
    XrmValue* e		/* value_return */
#endif
){}

/****************************************************************
 *
 * Resource Database Management
 *
 ****************************************************************/

void XrmSetDatabase(
#if NeedFunctionPrototypes
    Display* a		/* display */,
    XrmDatabase	b	/* database */
#endif
){}

XrmDatabase XrmGetDatabase(
#if NeedFunctionPrototypes
    Display* a		/* display */
#endif
){}

XrmDatabase XrmGetFileDatabase(
#if NeedFunctionPrototypes
    _Xconst char* a	/* filename */
#endif
){}

Status XrmCombineFileDatabase(
#if NeedFunctionPrototypes
    _Xconst char* b	/* filename */,
    XrmDatabase* a	/* target */,
    Bool c		/* override */
#endif
){}

XrmDatabase XrmGetStringDatabase(
#if NeedFunctionPrototypes
    _Xconst char* a	/* data */  /*  null terminated string */
#endif
){}

void XrmPutFileDatabase(
#if NeedFunctionPrototypes
    XrmDatabase	a	/* database */,
    _Xconst char* b	/* filename */
#endif
){}

void XrmMergeDatabases(
#if NeedFunctionPrototypes
    XrmDatabase	b	/* source_db */,
    XrmDatabase* a	/* target_db */
#endif
){}

void XrmCombineDatabase(
#if NeedFunctionPrototypes
    XrmDatabase	b	/* source_db */,
    XrmDatabase* a	/* target_db */,
    Bool c		/* override */
#endif
){}

#define XrmEnumAllLevels 0
#define XrmEnumOneLevel  1

Bool XrmEnumerateDatabase(
#if NeedFunctionPrototypes
    XrmDatabase	a	/* db */,
    XrmNameList	b	/* name_prefix */,
    XrmClassList c	/* class_prefix */,
    int	d		/* mode */,
    Bool (* e)(
#if NeedNestedPrototypes
	     XrmDatabase* q	/* db */,
	     XrmBindingList r	/* bindings */,
	     XrmQuarkList s	/* quarks */,
	     XrmRepresentation*	t /* type */,
	     XrmValue* u		/* value */,
	     XPointer v		/* closure */
#endif
	     )		/* proc */,
    XPointer x		/* closure */
#endif
){}

char *XrmLocaleOfDatabase(
#if NeedFunctionPrototypes
    XrmDatabase a	/* database */
#endif
){}


/****************************************************************
 *
 * Command line option mapping to resource entries
 *
 ****************************************************************/

typedef enum {
    XrmoptionNoArg,	/* Value is specified in OptionDescRec.value	    */
    XrmoptionIsArg,     /* Value is the option string itself		    */
    XrmoptionStickyArg, /* Value is characters immediately following option */
    XrmoptionSepArg,    /* Value is next argument in argv		    */
    XrmoptionResArg,	/* Resource and value in next argument in argv      */
    XrmoptionSkipArg,   /* Ignore this option and the next argument in argv */
    XrmoptionSkipLine,  /* Ignore this option and the rest of argv	    */
    XrmoptionSkipNArgs	/* Ignore this option and the next 
			   OptionDescRes.value arguments in argv */
} XrmOptionKind;

typedef struct {
    char	    *option;	    /* Option abbreviation in argv	    */
    char	    *specifier;     /* Resource specifier		    */
    XrmOptionKind   argKind;	    /* Which style of option it is	    */
    XPointer	    value;	    /* Value to provide if XrmoptionNoArg   */
} XrmOptionDescRec, *XrmOptionDescList;


void XrmParseCommand(
#if NeedFunctionPrototypes
    XrmDatabase* a	/* database */,
    XrmOptionDescList b	/* table */,
    int	c		/* table_count */,
    _Xconst char* d	/* name */,
    int* e		/* argc_in_out */,
    char**	f	/* argv_in_out */		     
#endif
){}
