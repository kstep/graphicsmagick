/*   RAL-CGM Interpreter header file:  @(#) cgmtrap.h  version 3.3
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
 * Description:  CGM Polygon set Trapezoid Decomposition typedefs and
 *               declarations
 *
 * Modification log:
 *
 *   15 Feb 93 KEVP Created
 */

#ifndef CGMTRAP_H
#define CGMTRAP_H

#ifdef CGMTRAP_C
#define Extern
#else
#define Extern extern
#endif

/* Trapezoid (Horizontal Trapezium) */
typedef struct {
   float top_left, top_right, top, base_left, base_right, base;
} Trapezoid;

#ifdef PROTO
                          /* Trapezoid Decomposition utilities */

   Extern void
      /* Fill Polygon(set) by trapezoid decomposition */
      TRPfill(Polygonset *, Int ,Float, void (*) ( Int, Trapezoid *) );

#else
   Extern void TRPfill();
#endif



#undef Extern
#endif /* CGMTRAP_H */
