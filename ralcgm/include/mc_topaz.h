/*  RAL-CGM Interpreter header file: @(#) mc_topaz.h  version 3.1
 *
 *  Topaz Specific setting for CGM Interpreter
 */

/*  Define Machine */

#define TOPAZ
#define CDO
#define FONTSYSTEM
#define TESTXWF

/*  Define Operating system/compiler  */

#define SYSV

/*  Define graphics libraries to be included  */

#define POSTSCRIPT

/*  Define values needed by the compiler  */

#define DATADIR ""

#define NAMELGTH 10

#define MALLOCH
#include <malloc.h>
#include <sys/types.h>

/*  to fool lint, a few extras  */

struct mount { char x; };
struct stdata { char x; };
struct fstypsw { char x; };
struct rcvd { char x; };
#include <sys/inode.h>
#include <sys/file.h>

/*  Turn on CDO so that on the Topaz we test FNTmatch code  */

#define CDO

     /*  DO NOT MOVE THESE DEFINITIONS - they are required by the */
     /*  #includes that follow  */

#define COLOUR_TAB  4096
#define COLOUR_BITS   12

#define PATT_TAB    4096
#define PATTERNS      10

#ifdef GDP
#include <tpz/topaz.h>

#include "../Video/vid.def.h"
#include "../Video/vid.top.h"
#include "../Video/vid.vdc.h"
#endif  /* GDP */

/*  Following definition for development system; should be

       "/extra/ralcgm/data/"

    when installed.

*/

#define LPOINT(p)           short *p
#define BEGINLIST(p)        p = t_subp = t_dlbuff
#define STOREPOINT(p,q)     *p++ = CONVX(q.x);       *p++ = CONVY(q.y)
#define STOREVALUE(p,q,r)   *p++ = CONVX(q);         *p++ = CONVY(r)
#define CLOSELIST(p)        *p++ = t_dlbuff[t_subp]; *p++ = t_dlbuff[t_subp+1]
#define NEWSUBLIST(p)       *p++ = DLINVIS;          t_subp = p
#define ENDLIST(p)          *p = DLEND
