/*  RAL-CGM Interpreter header file: @(#) cgmgraph.h  version 3.1
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
 * Description:  CGM Graphics utilities
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 */

#ifndef CGMGRAPH_H
#define CGMGRAPH_H

#ifdef CGMGRAPH_C
#define Extern
#else
#define Extern extern
#endif

#ifdef PROTO

/* Check if lines intersect and find intersection */
   Extern Logical GRAcross( Point *a, Point *b, Point *c, Point *d,
                     Logical *i, Point *x ),

/* Check polygon for intersections and add points */
                  GRAinsect( Long n, Long *ni, Point *x, Enum *edge );

/* Sort points list */
   Extern void    GRAsort( Long np, Point *x, Int *px );

#else
   Extern Logical GRAcross(),   /* Work out cross product */
                  GRAinsect();  /* Find line intersection */
   Extern void    GRAsort();    /* Sort a Points list */
#endif

#undef Extern
#endif /* CGMGRAPH_H */
