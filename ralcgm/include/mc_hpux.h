/*  RAL-CGM Interpreter header file: @(#) mc_hpux.h  version 3.2
 *
 *  HPUX Specific setting for CGM Interpreter
 */

/*  Define Machine  */

#define HPUX

/*  Define Graphics system(s)  */

#define POSTSCRIPT
#define BEZIER
#define HERSHEY
#define XW

#define DATADIR ""

#define NAMELGTH  32
#define EXTLGTH    8

#ifdef toupper
#undef toupper
#define toupper( c )  (islower( (c) )? (c) - 'a' + 'A' : (c) )
#undef tolower
#define tolower( c )  (isupper( (c) )? (c) - 'A' + 'a' : (c) )
#endif

#define SignChar char

#define MALLOCH
#include <malloc.h>

#ifdef CGMOXW_C     /* X-Windows specific settings  */

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#undef major
#undef minor
#endif  /*  XW  */




include/mc_hpux.h--------------------------------------------------
