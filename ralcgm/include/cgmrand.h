/*  RAL-CGM Interpreter header file:  @(#) cgmrand.h  version 3.1
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
 * Description:  CGM Random access external functions
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 */

#ifndef CGMRAND_H
#define CGMRAND_H

#ifdef CGMRAND_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO

/*  CGM Utilities in File  */

   Extern Code CGMframe( Code );
   Extern void CGMstframe( void );

#else

/*  CGM Utilities in File  */

   Extern Code CGMframe();    /* Random Access end of frame processimg */
   Extern void CGMstframe();  /* Random frame access initialisation */
#endif

#undef Extern
#endif /* CGMRAND_H */
