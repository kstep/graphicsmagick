/*  RAL-CGM Interpreter header file: @(#) cgmcolr.h  version 3.2
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
 * K E V Palmen @ Rutherford Appleton Laboratory (kevp@uk.ac.rl.ib)
 *
 * Description: CGM Colour table utilities
 *
 * Modification log:
 *
 *   3 Sep 92 KEVP Created
 *  14 Jul 93 KEVP Modified for Metafile Defaults Replacement
 */

#ifndef CGMCOLR_H
#define CGMCOLR_H

#ifdef CGMCOLR_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO
Extern void
      /* Put colours into CGM colour table */
      COLRput (Long , Long *),

      /* Free space allocated to CGM colour table */
      COLRfree ( void );

Extern RGBcolour *
      /* Get entry of CGM colour table */
      COLRtab (Index);

#else
Extern void COLRput(), COLRfree();
Extern RGBcolour *COLRtab();
#endif

#undef Extern
#endif /* CGMCOLR_H */
