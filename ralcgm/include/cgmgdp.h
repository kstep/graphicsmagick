/*  RAL-CGM Interpreter header file: @(#) cgmgdp.h  version 3.1
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
 * Description:  CGM GDP function declarations
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 *   3 Sep 92 KEVP Add function to convert GDP to CGM primitive.
 */

#ifndef CGMGDP_H
#define CGMGDP_H

#ifdef CGMGDP_C
#define Extern
#else
#define Extern extern
#endif


#ifdef PROTO
                               /*  GDP Utilities    */

   Extern void
   /* Find centre given three points  */
        GDPcentre( Point *cen, Double *rad, Double *ang,
                   Point *a, Point *b, Point *c ),

   /* Generate points for circular arc */
        GDParc( Point *cen, Double rad, Point *a, Point *b,
                Double *ang, Point *c, Point *d),

   /* Generate points for elliptical arc */
        GDPcircle( Point *cen, Double rad, Double ang, Point *a,
                   Double rat, long *np, LPOINT(pt), Enum close ),

   /* Convert GDP specification to CGM circle or ellipse primitive */
        GDPcgm ( Index gdp, Point *pt, Code *type, Enum *close),

   /* Gives start and end point of circular arc */
        GDPellipse( Point *cen, Point *cdp1, Point *cdp2,
                    Point *a, Point *b,
                    Double rat, long *np, LPOINT(pt), Enum close );
#else
   Extern void GDPcentre(), GDPcircle(), GDPellipse(), GDParc(),
               GDPcgm();
#endif

#undef Extern
#endif /* CGMGDP_H */
