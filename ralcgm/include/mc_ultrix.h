/*  RAL-CGM Interpreter header file:  @(#) mc_ultrix.h  version 3.2
 *
 *  DECstation/Ultrix Specific setting for CGM Interpreter
 */

/*  Define Machine  */

#define DEC
#undef ANSI
#define HAVE_SETPOS
#define PROTO

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

#ifdef ANSI
char *calloc( unsigned int, unsigned int );
#else
char *calloc();
#endif

/*  Define STDC typedefs ( size_t is defined wrongly ?) */

/*  typdedef unsigned int size_t  */

typedef long fpos_t;

#ifdef CGMOXW_C     /* X-Windows specific settings  */

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#undef major
#undef minor
#endif  /*  CGMOXW_C  */
