/*  RAL-CGM Interpreter header file: @(#) mc_vms.h  version 3.3
 *
 *  VMS Specific setting for CGM Interpreter
 */

/*  Define Machine  - VMS defined by compiler */

/*  Define Operating system/compiler  */

#define ANSI

/*  Define Graphics system(s)  */


#define XW
#define TEK4200
#define POSTSCRIPT
#define HPGL
#define BEZIER
#define HERSHEY

/* set default output to X windows */
#define DEFOUT  'X'

/*  Directory for Data and font files */

#define DATADIR  getenv("CGMDATA")

#define PATHDELIM ']'

#define NAMELGTH      80
#define EXTLGTH       20

/*  VMS specific settings */

#define NOSTDIN     /* No piping of stdin or stdout */
#define NOSTDOUT

/* include some ANSI C includes as they will not be picked up elsewhere
   as __STDC__ is not defined in VAX C */
#include <stddef.h>
#include <stdlib.h>

#define MALLOCH      /*  malloc() etc defined in 'stdlib.h'  */

#define SignChar char

#ifdef CGMOXW_C     /*  X Windows */
#include <decw$include/cursorfont.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#include <decw$include/Xatom.h>
#endif  /* XW  */

/* define error return codes as 0 */
#define RC_FATAL  (Index) 0
#define RC_FILERR (Index) 0
