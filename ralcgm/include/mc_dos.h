/*  RAL-CGM Interpreter header file: @(#) mc_dos.h  version 3.1
 *
 *  DOS Specific setting for CGM Interpreter
 */

/*  Define Machine  */

#define VGA
#define DOS

/*  Define Compiler */

#define MSOFTC

/*  Postscript driver */
#define POSTSCRIPT

#define BEZIER
#define HERSHEY

/*  Define DATADIR and FILESEP for file name resolution  */

#define DATADIR ""

#ifdef MSOFTC
#define ANSI
#define MALLOCH
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>

#define BINFILE
#endif

/*  Replace fabs and abs with macros */
#undef fabs
#define fabs(x)  ( (x) > 0.0 ? x : -(x) )
#undef abs
#define abs(x)  ( (x) > 0 ? x : -(x) )
