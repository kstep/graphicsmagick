/*  RAL-CGM Interpreter header file: @(#) mc_sun.h  version 3.2
 *
 *  MINGW Specific setting for CGM Interpreter
 */

/*  Define Machine  */

/* #define SUN */

/*  Define Graphics system(s)  */

#define POSTSCRIPT
#define BEZIER
#define HERSHEY

#define DATADIR "c:\\home\\bfriesen\\GraphicsMagick\\ralcgm\\data\\"

#define NAMELGTH   256
#define EXTLGTH    8

#define MSOFTC 1
#define ANSI
#define PROTO
#define MALLOCH
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>

#define BINFILE

#ifdef toupper
#undef toupper
#define toupper( c )  (islower( (c) )? (c) - 'a' + 'A' : (c) )
#undef tolower
#define tolower( c )  (isupper( (c) )? (c) - 'A' + 'a' : (c) )
#endif

#define SignChar char

/*  Define STDC typedefs ( size_t is defined wrongly ?) */

/*  typdedef unsigned int size_t  */

/* typedef long fpos_t; */

#ifdef CGMOXW_C     /* X-Windows specific settings  */

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#undef major
#undef minor
#endif  /*  CGMOXW_C  */
