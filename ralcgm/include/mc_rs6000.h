/*  RAL-CGM Interpreter header file: @(#) mc_rs6000.h  version 3.2
 *
 *  IBM RS6000 Specific setting for CGM Interpreter
 */

/*  Define Machine  */

#define RS6000

#define SYSV

/*  Define Graphics system(s)  */


/* Default output is Iris screen  */

#define DATADIR ""

#ifdef  IGL      /* Iris Graphics Library */

/*  Define Convex fill routine  */
#define CGMCONVEX IGLfill

/*  Define Edge drawing routine  */
#define CGMEDGE   IGLedge

/*  Set colour and pattern table sizes */

#define COLOUR_TAB  4096
#define COLOUR_BITS   12

/*  Set pattern table sizes */
#define PATT_TAB   4096
#define PATTERNS     40

#endif  /*  IGL  */

#define ANSI

#ifdef CGMOXW_C     /* X-Windows specific settings  *****/

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#endif  /*  XW  */
