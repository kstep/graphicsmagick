/*   RAL-CGM Interpreter module:  @(#) cgmerr.c  version 3.4
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description:  Error handling routine for CGM
 *
 * Maintenance Log:
 *   4 Oct 90 RTP  Original version created from cgmmain.c
 *   8 Nov 90 RTP  Changed exit(99) to exit (0)
 *  14 May 91 RTP  Add ANSI declarations
 *  19 Jun 91 RTP  Add header file cgmatt.h to include function
 *                 declarations
 *   2 Aug 91 RTP  Add SCCS id
 *  11 Sep 91 CDS  Changed to use utility specific header CGMUERR.H
 *   3 Nov 94 KEVP Add function CGMcounterr to enable output driver to obtain
 *                 the error count. It may raise cgmerrcount by that amount to enable
 *                 picture to be reshown (useful for output driver with frame menu).
 *  15 Feb 95 KEVP Let MS Windows handle certain errors.
 *  22 Mar 95 KEVP Add Quiet output option
 */

#define CGMERR_C

#include "cgmmach.h"
#include "cgmpar.h"
#include "cgmerr.h"

#define ERRMESS         (void) fprintf(cgmerr,

#ifdef WIN
#ifdef PROTO
extern WINerror ( char*, int, Enum, char*, char*);
#else
extern WINerror();
#endif
extern Logical errfileopen;
#endif

extern Int cgmerrcount;
extern Logical cgmquiet;

static int errcount=ZERO;

/********************************************************* CGMerror ****/

#ifdef PROTO
int CGMerror ( char *func, int errnr, Enum errtype, char *mess )
#else
int CGMerror ( func, errnr, errtype, mess )

int errnr;
Enum errtype;
char *func, *mess;
#endif
{
   register struct errmessage *emess=cgmerrs;

   do
   {
      if ( errnr == emess->number )
      {
         break;
      }
      emess++;
   } while ( emess->number != ERR_NOTFOUND );

#ifdef WIN
   /* Allow Error to be displayed by MS Windows */
   if(errtype == FATAL || (!errfileopen && !cgmquiet))
   {
      WINerror ( func, errnr, errtype, emess->mess, mess );
      if(!errfileopen) return emess->rc;
   }
#endif
   if(cgmquiet)
   {   /* Just count error */
      if(errtype != WARNING) errcount++;
   }
   else
   {  /* Report error and count it */

      switch ( errtype )
      {
         case WARNING:
            ERRMESS "\n***WARNING:");
            break;

         case FATAL:
            ERRMESS "\n***FATAL ERROR:");
            errcount++;
            break;

         case ERROR:
         default:
            ERRMESS "\n***ERROR:");
            errcount++;
            break;
      }
      ERRMESS " %s [%d] %s", func, emess->number, emess->mess );
      if ( mess != NULL ) ERRMESS " %s", mess );
      ERRMESS "\n");

      if ( cgmerrcount && errcount > cgmerrcount )
      {
         ERRMESS "\n*** Maximum Error count exceeded ***\n");
         exit (0);
      }
   } /* Endif/else cgmquiet */

   return ( emess->rc );
}
/********************************************************* CGMcounterr ****/

#ifdef PROTO
int CGMcounterr ( void )
#else
int CGMcounterr ( void )
#endif
/* Return Error Count */
{
    return errcount;
}

