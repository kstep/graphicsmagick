/*  RAL-CGM Interpreter header file: @(#) mc_cray.h  version 3.2
 *
 *  CMS Specific setting for CGM Interpreter
 *  Cray Specific setting for CGM Interpreter
 */

/*  Define Machine  */

/*  Define Operating system/compiler  */

#define UNICOS
#define SYSV
/*  use new C compiler  */
#define SCC20


/*  Define Graphics system(s)  */

#define POSTSCRIPT
#define XW

#define DATADIR ""

#define NAMELGTH   14
#define EXTLGTH     8

#ifdef SCC20   /*  new ANSI compatable compiler */
#define ANSI
#include <stddef.h>
#include <stdlib.h>
#define MALLOCH
#else
#define SignChar char
#endif

#ifdef CGMOXW_C     /* X-Windows specific settings  *******/

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#endif  /*  XW  */
