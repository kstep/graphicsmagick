/*   RAL-CGM Interpreter Header file:  @(#) cgmcell.h  version 3.1
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
 * Description: CGM Cell array utilities
 *
 * Modification log:
 *
 *   3 Sep 92 KEVP Created
 *   7 Oct 92 KEVP Change first row argument of CELLfill to pointer
 *                 so that CELLfill can update it.
 */

#ifndef CGMCELL_H
#define CGMCELL_H

#ifdef CGMCELL_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO
Extern void
/* Get cell array parameters from driver argument list */
      CELLparget( Long *, Float *, Point *, Point *, Point *,
                  Int *, Int *, Int * ),

/* Get cell array transformation */
      CELLtran ( Point, Point , Point , Int , Int , Tmatrix ),

/* Draw cell array using the device fill function */
      CELLfill ( Long, Long *, Tmatrix, Int , Int , Int *,
                 void (*)(Index, RGBcolour, Code),      /* Set colour */
                 void (*)(Int, Point*, Tmatrix, Int) ); /* linedraw */

#else
Extern void CELLparget(), CELLtran(), CELLfill();
#endif

#undef Extern
#endif /* CGMCELL_H */
