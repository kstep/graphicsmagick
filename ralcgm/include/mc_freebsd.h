/*  RAL-CGM Interpreter header file: @(#) mc_sun.h  version 3.2
 *
 *  Sun 3 & 4 Specific setting for CGM Interpreter
 */

/*  Define Machine  */

#define SUN

/*  Define Graphics system(s)  */

#define POSTSCRIPT
#define XW

#define ANSI
#include <stdlib.h>
#include <stddef.h>

#define DATADIR "/usr/local/share/ralcgm/"

#define NAMELGTH  32
#define EXTLGTH    8

#ifdef toupper
#undef toupper
#define toupper( c )  (islower( (c) )? (c) - 'a' + 'A' : (c) )
#undef tolower
#define tolower( c )  (isupper( (c) )? (c) - 'A' + 'a' : (c) )
#endif

#define SignChar char

#ifdef CGMOXW_C     /* X-Windows specific settings  */

#include "X11/cursorfont.h"
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/Xatom.h"

#undef major
#undef minor
#endif  /*  CGMOXW_C  */
