/*   RAL-CGM Interpreter header file:  @(#) mc_iris.h  version 3.3
 *
 *  Iris Specific setting for CGM Interpreter
 */

/*  Define Machine either IRIS, IRIS4D or INDIGO */

#define IRIS
#define SYSV

/*  Define Graphics system(s)  */

#define IGL

#define POSTSCRIPT
#define BEZIER
#define HERSHEY

/* Default output is Iris screen  */

#define DEFOUT  'I'

#define DATADIR ""

#define NAMELGTH 14
#define EXTLGTH   8

#ifdef  IGL      /* Iris Graphics Library */

/*  Set colour and pattern table sizes */

#define COLOUR_TAB  4096
#define COLOUR_BITS   12

/*  Set pattern table sizes */
#define PATT_TAB   4096
#define PATTERNS     40

#endif  /*  IGL  */

#ifdef CGMIGL_H
#include "gl.h"
#include "device.h"
#endif

#define DEFVIRTUALRGB FALSE

#ifdef __mips   /*  Iris 4D  */

#ifdef INDIGO
#undef DEFVIRTUALRGB
#define DEFVIRTUALRGB TRUE
#define IRIS
#endif
#ifdef IRIS4D
#define IRIS
#endif

#define XW
#define ANSI

#ifdef CGMIGL_H
#undef FATAL
#undef WARNING
#endif

#else          /* Iris 3130 */

typedef unsigned int size_t;
typedef long fpos_t;

#define SQRT(x)  sqrt( (float) (x) )

#endif

#ifdef XW     /* X-Windows specific settings  *****/

#ifdef CGMOXW_C
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

#endif  /*  XW  */
