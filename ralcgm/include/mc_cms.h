/*  RAL-CGM Interpreter header file: @(#) mc_cms.h  version 3.3
 *
 *  CMS Specific setting for CGM Interpreter
 */

/*  Define Machine  */

#define CMS

/*  Define Operating system/compiler  */

#define C370

/*  Define Graphics system(s)  */

#define POSTSCRIPT
#define XW
#define HPGL
#define TEK4200

#define BEZIER
#define HERSHEY

#ifdef WATC
#undef XW    /*  can't use X-Windows with Waterloo C */
#endif /* not WATC */

/*  Define DATADIR and FILESEP for file name resolution  */

#define DATADIR ""
#define FILESEP " "
#define PATHDELIM ' '
#define EXTLGTH 8
#define UPPERCASE_FILENAME

#define BINFILE     /* Need to reopen Binary files */

#define NOSTDIN     /* No piping of stdin od stdout */
#define NOSTDOUT

#define DEFOUT  'C' /*  Production version output default is character */

#define EBCDIC      /*  EBCDIC characters */

#define MALLOCH            /*  malloc calls defined in 'stdlib.h' */

#include "cgmmap.h"        /*  Map long variable and function names */

#ifdef WATC  /* Waterloo C settings */

#ifdef CGMINITH   /*  Increase Stack size if in main */
  const int _staksize = (40*1024);
#endif

#define clear CMSclear     /* Ignore clear in 'stdlib.h */

#define LFILEDEF 22        /* Extra information added to filename */
#endif  /*  WATC */

#ifdef IBMC   /* IBM C settings */

/*  include files not quite ANSI */
#include <stdefs.h>
#include <stdlib.h>

#ifdef XW
#define XINCLUDE
#undef CMS
#include <cursfont.h>
#include <Xlib.h>
#include <Xutil.h>
#include <Xatom.h>
#define  Xw_menu_parent    Xw_mnu_parent
#endif
#undef __STDC__   /*  Avoid picking up standard include files  */
#define ANSI

#endif  /* IBMC  */

#ifdef C370   /* IBM SAA C settings */

#ifdef  CGMOXW_C
#define XINCLUDE
#include <cursorfo.h>
#include <Xlib.h>
#include <Xutil.h>
#include <Xatom.h>
#endif

#define ANSI
/*  __STDC__ is not defined */
#include <stddef.h>
#include <stdlib.h>

#endif  /* C370  */
