/*   RAL-CGM Interpreter module:  @(#) cgmgdp.c  version 3.3
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
 * Description:Machine independent routines for Circles etc
 *
 *
 * Maintenance Log:
 *   3 Oct 88 RTP  Original version
 *  17 Nov 88 RTP  Use typedefs
 *                 Use pointers in SORT routine
 *   1 Feb 89 RTP  Split into seperate file
 *  10 Nov 89 RTP  Use macros for local points
 *   9 Aug 90 RTP  Add ratio argument for pixel resolution
 *  12 Sep 90 RTP  Change all Point parameters to pointers
 *  12 Nov 90 RTP  Change close flag to be PIE or CHORD
 *                 do not assume close is non-zero
 *   3 Dec 90 RTP  Change float parameters to Double
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  11 Feb 91 RTP  Make sure number of points != 0
 *  14 May 91 RTP  Add ANSI declarations
 *  19 Jun 91 RTP  Add header file cgmatt.h to include function declarations
 *   2 Aug 91 RTP  Add SCCS id
 *   2 Apr 92 KEVP Add function GDPcgm, that converts the parameters for
 *                 a RAL GKS GDP to those for a CGM primitive.
 *   8 Oct 92 KEVP Updated and improved comments.
 *  10 Feb 94 RTP  Remove macros and get from cgmout.h
 *  14 Sep 94 KEVP Make all angles Double precision instead of float
 *                           (needed for sin and cos to work on PC Borland C)
 *  22 Sep 94 KEVP In GDPcentre, move test for full circle BEFORE the test
 *                            for collinearity. This enables full circles to drawn despite
 *                            the 3 points being collinear (because 1st = 3rd).
 */

#define CGMGDP_C

#include "cgmout.h"
#include "cgmgdp.h"
#include "cgmelem.h"
#include "cgmpar.h"


/********************************************************* GDPcentre ***/

#ifdef PROTO
void GDPcentre ( Point *cen, Double *rad, Double *ang,
                 Point *a, Point *b, Point *c )
#else
void GDPcentre ( cen, rad, ang, a, b, c )

/*
     Given three points a, b, and c on a circle
     returns centre, radius and angle subtended (radians)
*/

Point *cen;         /* Centre of circle */
Double *rad, *ang;  /* Radius and angle subtended by a through b to c
                       in radians, negative if and only if clockwise */
Point *a, *b, *c;   /* Three points on the arc */
#endif

{
   register float x, y, xab, yab, xac, yac, xbc, ybc, det;

#ifdef DEBUG
   DMESS "GDPcentre: (%f,%f) (%f,%f) (%f,%f)\n",
                                       a->x, a->y, b->x, b->y, c->x, c->y);
#endif
    if ( VEQUAL(a->x, c->x) && VEQUAL(a->y, c->y) )
   {
                /*   Start point == end point - Draw circle */
     cen->x = x = (b->x + a->x ) / 2.0;
     cen->y = y = (b->y + a->y ) / 2.0;
     *rad = SQRT( (a->x-x)*(a->x-x) + (a->y-y)*(a->y-y) );
     *ang = 2*PI;
     return;
   }

  xab = b->x - a->x;
   yab = b->y - a->y;
   xac = c->x - a->x;
   yac = c->y - a->y;
   xbc = c->x - b->x;
   ybc = c->y - b->y;

   det = 2.0 * ( xab*ybc - xbc*yab );  /*  cross product  */

   if ( VEQUAL(det, 0.0) || ( VEQUAL(a->x, b->x) && VEQUAL(a->y, b->y) )
        || ( VEQUAL(b->x, c->x) && VEQUAL(b->y, c->y) ) )
   {
/*                   points are co-linear   */
     *rad = cen->x = cen->y = *ang = 0.0;
     return;
   }


   cen->x = x = (-yab*ybc*yac + (a->x+b->x)*xab*ybc - (c->x+b->x)*xbc*yab ) /
det;
   cen->y = y = (xab*xbc*xac + (c->y+b->y)*xab*ybc - (a->y+b->y)*xbc*yab ) /
det;
   *rad = SQRT( (a->x-x)*(a->x-x) + (a->y-y)*(a->y-y) );
   *ang = atan2 ( (a->x-x)*(c->y-y) - (a->y-y)*(c->x-x),
                  (a->x-x)*(c->x-x) + (a->y-y)*(c->y-y) );
#ifdef DEBUG
   DMESS " Centre: (%f %f) radius: %f  angle: %f\n",
                 cen->x, cen->y, *rad, *ang);
#endif

/*  Adjust angle according to direction of arc */

   if ( det < 0.0 && *ang > 0.0 ) *ang -= 2*PI;
   if ( det > 0.0 && *ang < 0.0 ) *ang += 2*PI;

#ifdef DEBUG
     DMESS "  New angle: %f radians (det: %f)\n", *ang, det);
#endif

   return;
}

/********************************************************* GDParc ******/

#ifdef PROTO
void GDParc ( Point *centre, Double rad,
              Point *a, Point *b, Double *ang, Point *c, Point *d )
#else
void GDParc ( centre, rad, a, b, ang, c, d )

/*
    Given the centre, radius and
    start vector (a->x, a->y) and end vector (b->x, b->y)
    returns start and end points on circle (c->x,c->y) and (d->x,d->y)
    and angle subtended
*/

Point *centre;          /* centre */
Double rad;             /* radius */
Double *ang;            /* angle subtanded by arc to centre in radians */
Point *a, *b, *c, *d;   /* start & end vectors, start & end points */
#endif

{
   register Double ang1, ang2;

#ifdef DEBUG
   DMESS " GDParc: centre: (%f,%f) start (%f,%f) end (%f,%f)\n",
                         centre->x, centre->y,  a->x, a->y, b->x, b->y);
#endif

   ang1 = atan2( a->y, a->x );
   ang2 = atan2( b->y, b->x );
   *ang = ang2 - ang1;
   if ( *ang < 0 ) *ang += 2*PI;

#ifdef DEBUG
   DMESS " Angles: %f rads %f degrees", ang1, ang2);
   DMESS " : %f (%f)\n", *ang, *ang * 180.0/PI);
#endif

/*  Work out end points  */

   c->x = rad * cos( ang1 ) + centre->x;
   c->y = rad * sin( ang1 ) + centre->y;
   d->x = rad * cos( ang2 ) + centre->x;
   d->y = rad * sin( ang2 ) + centre->y;

#ifdef DEBUG
   DMESS  "  Arc between: (%f,%f) and (%f, %f)\n",
                           c->x, c->y, d->x, d->y);
#endif

   return;
}

/********************************************************* GDPcircle ***/

#ifdef PROTO
void GDPcircle ( Point *centre, Double rad, Double angle,
                 Point *a, Double rat,
                 Long *np, LPOINT(pt), Enum close )
#else
void GDPcircle ( centre, rad, angle, a, rat, np, pt, close )

Point *centre, *a;        /* Centre and starting point of arc */
Double rad, angle, rat;   /* Radius, Angle in Radians and Resolution */
Long *np;                 /* Number of points output */
LPOINT(pt);               /* Points output */
Enum close;               /* CGM closure type */
#endif

/*
    Works out points on circular arc from (a->x, a->y)
    given centre, radius and angle (clockwise -ve).
    If the angle is 2PI, then full circle is returned
*/

{
    register long j;
    register Double theta1, dtheta, ang;

#ifdef DEBUG
    DMESS "GDPcircle: centre (%f,%f) rad %g angle %g\n",
                                 centre->x, centre->y, rad, angle);
#endif

/*  work out start angle */

    theta1 = atan2( a->y - centre->y, a->x - centre->x );
    dtheta = angle;
    if ( REQUAL(dtheta, 0.0) ) dtheta = 2*PI;

/*  work out number of lines */

    *np = (long) FABS(dtheta * SQRT(rad * rat) );
    if ( *np == 0 ) (*np)++;
    if ( *np > ARRAY_MAX ) *np = ARRAY_MAX;

    dtheta /= (*np)++;

#ifdef DEBUG
   DMESS " Angles (degrees) theta1: %f (%f) dtheta: %f (%f) ang: %f\n",
              theta1, 180.0*theta1/PI, dtheta, 180.0*dtheta/PI, angle);
   DMESS " Points: %d\n", *np);
#endif

/*  Generate points into Local form  */

    BEGINLIST(pt);
    if ( close == PIE )
    {
/*   If close is PIE start at  centre  */

       STOREPOINT( pt, (*centre) );
    }
    for ( j = 0, ang = theta1 ; j < *np ; j++, ang += dtheta )
    {
       STOREVALUE ( pt, centre->x + rad*cos(ang),
                        centre->y + rad*sin(ang) );
    }

/*   If close make sure last point = first point */
    if ( close == PIE )
    {
       (*np)++;  /*  extra point needed for pie */
       STOREPOINT( pt, (*centre) );
       (*np)++;
    }
    else if ( close == CHORD )
    {
       STOREPOINT( pt, (*a) );
       (*np)++;
    }
    ENDLIST(pt);

    return;
}

/********************************************************* GDPellipse **/

#ifdef PROTO
void GDPellipse ( Point *centre, Point *cdp1, Point *cdp2,
                  Point *d1, Point *d2, Double rat,
                  Long *np, LPOINT(pt), Enum close )
#else
void GDPellipse ( centre, cdp1, cdp2, d1, d2, rat, np, pt, close )

Point *centre, *cdp1, *cdp2, *d1, *d2;
Double rat;
long *np;
LPOINT(pt);
Enum close;
#endif

/*
    Works out points on elliptical arc for ellipse specifified by
    centre and 2 CDP points. The arc is specified by direction vectors
    (d1->x,d1->y) and (dx2,dy2).
    Returns set of points in *pt
    if (d1->x,d1->y) == (dx2,dy2) then full ellipse is returned
*/

{
    register long j;
    register Double theta1, theta2, dtheta, ang;
    Point p1, p2;
    float maxcd, a, b, c, d, det;

#ifdef DEBUG
    DMESS "GDPellipse: centre (%f,%f) cdp1 (%f,%f) cdp2 (%f,%f)\n",
           centre->x, centre->y, cdp1->x, cdp1->y, cdp2->x, cdp2->y);
    DMESS " Direction Vector: (%f,%f) (%f,%f)\n",
                                       d1->x, d1->y, d2->x, d2->y );
#endif

/*  work out CDP lengths   */

    p1.x = cdp1->x - centre->x;
    p2.x = cdp2->x - centre->x;
    p1.y = cdp1->y - centre->y;
    p2.y = cdp2->y - centre->y;

    maxcd = SQRT( (p1.x*p1.x+p1.y*p1.y) + (p2.x*p2.x+p2.y*p2.y) );

#ifdef DEBUG
    DMESS "  x1: %f  y1: %f  x2: %f y2: %f max: %f\n",
                       p1.x, p1.y, p2.x, p2.y, maxcd);
#endif

/*  work out Inverse of transformation Matrix  */

   det = p2.y*p1.x - p1.y*p2.x;
   a = p2.y / det;
   b = -p2.x / det;
   c = -p1.y / det;
   d = p1.x / det;

#ifdef DEBUG
   DMESS "  a: %f b: %f c: %f d: %f det: %f\n",
                         a, b, c, d, det );
#endif

/*  work out angles from direction vectors */

    theta1 = atan2( c*d1->x+d*d1->y, a*d1->x+b*d1->y );
    theta2 = atan2( c*d2->x+d*d2->y, a*d2->x+b*d2->y );
    dtheta = theta2 - theta1;
    if ( REQUAL(dtheta, 0.0) ) dtheta = 2*PI;
    else
    {
       if ( det < 0.0 && dtheta > 0.0 ) dtheta -= 2*PI;
       if ( det > 0.0 && dtheta < 0.0 ) dtheta += 2*PI;
    }

#ifdef DEBUG
    DMESS "   Angles:  %f  %f  diff: %f\n",
                                 theta1, theta2, dtheta);
#endif

/*  Estimate number of points from CDPs  */


    *np = FABS(dtheta * SQRT(maxcd * rat) );
    if ( *np == 0 ) (*np)++;
    if ( *np >= ARRAY_MAX ) *np = ARRAY_MAX;
    dtheta /= (*np)++;

#ifdef DEBUG
   DMESS "   dtheta: %f  points: %d", dtheta, *np);
   if ( close == PIE ) DMESS " (pie)");
   if ( close == CHORD ) DMESS " (chord)");
   DMESS "\n");
#endif

/*  Produce list of points */

    BEGINLIST(pt);
    if ( close == PIE )
    {   /*   If close is PIE first point is centre */
       STOREPOINT( pt, (*centre) );
    }
    for ( j = 0, ang = theta1 ; j < *np ; j++, ang += dtheta )
    {
       STOREVALUE ( pt, centre->x + p1.x * cos(ang) + p2.x * sin(ang),
                        centre->y + p1.y * cos(ang) + p2.y * sin(ang) );
    }

/*   If close add first point  */
    if ( close == PIE )
    {
       STOREPOINT( pt, (*centre) );
       (*np)+=2;
    }
    else if ( close == CHORD )
    {
       STOREVALUE ( pt, centre->x + p1.x * cos(theta1) + p2.x * sin(theta1),
                        centre->y + p1.y * cos(theta1) + p2.y * sin(theta1) );
       (*np)++;
    }
    ENDLIST(pt);

    return;
}
/***************************************************** GDPcgm **********/

#ifdef PROTO
void GDPcgm ( Index gdp, Point *pt, Code *type, Enum *close)
#else
void GDPcgm ( gdp, pt, type, close)

/*  converts the parameters of a RAL GDP (including transformation pts)
    to those of the corresponding CGM primitive */

/* NOTE: Once a routine that achieves this purpose is put into RAL GKS,
         this will only be required in RAL CGM, for compatibility with
         CGMs produced by previous versions of RAL GKS.
*/

Index gdp;                    /* GDP type */
Point *pt;          /* IN as GDP points - OUT as CGM points */
Code *type;                    /* CGM type */
Enum *close;                  /* CGM closure code */

#endif

/* DEFINITIONS: CDP point = End point of an ellipse's Conjugate Diameter.

                CDP vector = A vector from the origin parallel to a
                             conjugate diameter.

  Note: The tangent of an ellipse on a CDP point is
        parallel to the other conjugate diameter. */

/* ALGORITHM

The closure type (NOCLOSE, CHORD or PIE) is determined from GDP type.

It is determined whether the GDP uses 3 Points or is a complete
circle/ellipse using 2 pts.

For the GDP transformation points it is determined, whether the GDP is
CIRCULAR or ELLIPTICAL. If elliptical the inverse transformation is
calculated (as backtransformed unit vectors).
Since the GDPs are invariant under translation the translation part
of the transformation is ignored.

The four cases are dealt separately:

3 Points & Circular:  (pt+3,pt+4,pt+5) ==> (pt,pt+1,pt+2).
                       If close = NOCLOSE, Type = ARC3PT.
                       else,               Type = ARC3PTCLOSE.

3 Points & Elliptical: (pt+3,pt4,pt+5) are backtransformed, radius
                       and centre are then found.
                       Centre transformed and output as (pt).
                       Radius transformed to CDP points (pt+1,pt+2).
                       Direction vectors (pt+3) and (pt+5) - centre
                       output as (pt+3) and (pt+4).
                       If close = NOCLOSE, Type = ELLIPARC.
                       else,               Type = ELLIPARCCLOSE.

2 Points & Circular:   Centre (pt+4) output as (pt).
                       Radius obtained from (pt+3) and (pt+4) and
                       output as x component of (pt+1).
                       Type = CIRCLE.

2 Points & elliptical: Radius vector obtained from (pt+3) and (pt+4)
                       and backtansformed radius then found.
                       Centre (pt+4) output as (pt).
                       Radius transformed to CDP points (pt+1,pt+2).
                       Type = ELLIPSE.

3 pts  collinear       Type = LINE of 3pts.

Tranformation pts collinear:  Type = LINE of 3pts.

Non-GDP types:         No change.

End of Algorithm */

{
   Point gx0, g0y, gxy;        /* Transformed (1,0), (0,1) & (1,1) */
   Point *start, *on, *end, *cen;    /* RAL GKS GDP parameters */
   Point *centre, *cdp1,*cdp2,       /* CGM primitive parameters */
         *startvec, *endvec;
   Logical gdp3pts,            /* True, if 3 GDP points */
           circular = FALSE;   /* True, if circular */
   Point cdv1, cdv2;           /* CDP Vectors */
   Point stv, onv, env;        /* Vectors to GDP points */
   Float radius;               /* CGM radius */
   Float cdvl1, cdvl2;         /* CPD Vector Lengths */
   Double prodmin;             /* Minimum non-zero product */
   Double det, invdet;         /* Determinant and its inverse */
   Point bux, buy;             /* Backtransformed unit vectors */
   Point bst, bon, ben;        /* Backtransformed Points */
   Point bonv;                 /* Backtransformed radius vector */
   Point bcentre;              /* Backtransformed centre */
   Double bradius, junk;       /* Backtransformed Radius & Junk*/

#ifdef DEBUG
   DMESS " GDPcgm: GDP type %d\n",gdp);
   DMESS " GDP reg pts (%f,%f),(%f,%f),(%f,%f)\n",(pt)->x,(pt+1)->y,
           (pt)->y,(pt+2)->x,(pt+1)->x,(pt+2)->y);
   DMESS " GDP     pts (%f,%f),(%f,%f),(%f,%f)\n",(pt+3)->x,(pt+3)->y,
           (pt+4)->x,(pt+4)->y,(pt+5)->x,(pt+5)->y);
#endif

   /* Check type and determine closure */
   switch(gdp)
   {
      case GKSARC: case GKSCIRCLE:
         *close = NOCLOSE;
         break;
      case GKSCHORD:
         *close = CHORD;
         break;
      case GKSPIE:
         *close = PIE;
         break;
      default:
         return;
   }

   gx0.x =     pt->x;     /* Get Transformed Points */
   g0y.x =     pt->y;     /* These were put into the CGM rather oddly. */
   gxy.x = (pt+1)->x;     /* This will not be changed, */
   gx0.y = (pt+1)->y;     /* in order to retain compatibility */
   g0y.y = (pt+2)->x;     /* old CGMs with RAL GKS GDPs in them. */
   gxy.y = (pt+2)->y;     /* Eventually GKS GDPs will be withdrawn. */
#ifdef DEBUG
   DMESS " gx0(%f,%f), g0y(%f,%f), gxy(%f,%f)\n",gx0.x,gx0.y,
          g0y.x,g0y.y,gxy.x,gxy.y);
   DMESS " gdptype = %d",gdp);
#endif
   if(gdp != GKSCIRCLE)
   {  /* GDP with start/on/end points */
      start = pt+3; on = pt+4; end = pt+5;      /* Name points */
      gdp3pts = TRUE;                           /* 3pt GDP */
   }
   else
   {  /* GDP circle with centre/on points */
      cen = pt+4; on = pt+3;                 /* Name points */
      gdp3pts = FALSE;                       /* 2pt GDP */
   }

   /* Set the minimum non-zero product for vectors */
   prodmin = (cur.vdcmin)*(cur.vdcmin);
   if(prodmin < cur.realmin) prodmin = cur.realmin;

#ifdef DEBUG
   if(gdp3pts) DMESS "st(%f,%f), on(%f,%f), en(%f,%f)\n",start->x,
   start->y,on->x,on->y,end->x,end->y);
   else DMESS "cen(%f,%f), on(%f,%f)\n",cen->x,cen->y,on->x,on->y);
#endif

   /* Check for Collinearity, if three points */
   if(gdp3pts && (FABS(XPROD(start,on,on,end)) < prodmin) )
   {  /* Points are Collinear: return as line */
       *pt = *start; *(pt+1) = *on; *(pt+2) = *end;
       *type = LINE;
#ifdef DEBUG
       DMESS "LINE from collin pts.");
#endif
       return;
   }

   /* Determine if GDP is circular */
   cdv1.x = gxy.x - g0y.x;   /* Get CDP Vectors. */
   cdv1.y = gxy.y - g0y.y;   /* These are the non-translation */
   cdv2.x = gxy.x - gx0.x;   /* parts of the transformation   */
   cdv2.y = gxy.y - gx0.y;   /* matrix. */
   /* CDP Vectors perpendicular ? */
   circular = ( FABS(cdv1.x*cdv2.x + cdv1.y*cdv2.y) < prodmin);
   /* CDP Vectors same length ? */
   if(circular)
   {
      cdvl1 = SQRT(cdv1.x*cdv1.x + cdv1.y*cdv1.y);
      cdvl2 = SQRT(cdv2.x*cdv2.x + cdv2.y*cdv2.y);
      circular = VEQUAL(cdvl1,cdvl2);
   }

   /* If not circular, get inverse transformation */
   if( !circular )
   {  /* Elliptical Curve needed */
      det = ( cdv1.x*cdv2.y - cdv1.y*cdv2.x );
      if( FABS(det) < prodmin)
      {  /* Tranformation has no inverse: return as line */
         pt->x = cen->x + cen->x - on->x;
         pt->y = cen->y + cen->y - on->y;
         *(pt+1) = *on;
         *type = LINE;
#ifdef DEBUG
         DMESS "LINE from collin reg pts, ");
#endif
         return;
      }

      /* Get backtransformed unit vectors (inverse transformation) */
      invdet = 1.0/det;
      bux.x =  cdv2.y*invdet; bux.y = -cdv1.y*invdet;
      buy.x = -cdv2.x*invdet; buy.y =  cdv1.x*invdet;

#ifdef DEBUG
      DMESS " cdv1(%f,%f), cdv2(%f,%f)\n",cdv1.x,cdv1.y,cdv2.x,cdv2.y);
      DMESS "  bux(%f,%f),  buy(%f,%f)\n",bux.x,bux.y,buy.x,buy.y);
#endif

      /* Identify Centre and CDP points in Output */
      centre = pt; cdp1 = pt+1; cdp2 = pt+2;
   }


   /* Determine CGM parameters */
   if(gdp3pts)
   {  /* GDP Arc, Chord or Pie */
      if(circular)
      {  /* Part of Circle */
         *pt = *start; *(pt+1) = *on; *(pt+2) = *end;
         if(*close == NOCLOSE) *type = ARC3PT;
         else *type = ARC3PTCLOSE;
      }
      else
      {  /* elliptical curve */

         /* Backtransform the three points */
         bst.x = (start->x)*bux.x + (start->y)*buy.x;
         bst.y = (start->x)*bux.y + (start->y)*buy.y;
         bon.x = (on->x)*bux.x    + (on->y)*buy.x;
         bon.y = (on->x)*bux.y    + (on->y)*buy.y;
         ben.x = (end->x)*bux.x   + (end->y)*buy.x;
         ben.y = (end->x)*bux.y   + (end->y)*buy.y;

         /* Get the Bracktransformed radius & centre */
         GDPcentre( &bcentre, &bradius, &junk, &bst, &bon, &ben);

         /* Transform centre and Output it */
         centre->x = cdv1.x*bcentre.x + cdv2.x*bcentre.y;
         centre->y = cdv1.y*bcentre.x + cdv2.y*bcentre.y;

#ifdef DEBUG
         DMESS "bst(%f,%f), bon(%f,%f), ben(%f,%f)\n",bst.x,bst.y,
                bon.x,bon.y,ben.x,ben.y);
         DMESS "bradius %f, bcentre(%f,%f), centre(%f,%f)\n",bradius,
                bcentre.x,bcentre.y,centre->x,centre->y);
#endif
         /* Multiply CDP vectors by backtransformed radius */
         /* and add to centre to obtain CDP points and output them */
         cdp1->x = bradius*cdv1.x + centre->x;
         cdp1->y = bradius*cdv1.y + centre->y;
         cdp2->x = bradius*cdv2.x + centre->x;
         cdp2->y = bradius*cdv2.y + centre->y;

         /* Make sure that the curve is anticlockwise */
         if(XPROD(start,on,on,end) > 0)
            { startvec = pt+3; endvec = pt+4; }
         else
            { startvec = pt+4; endvec = pt+3; }

         /* Output direction vectors and type */
         startvec->x = start->x - centre->x;
         startvec->y = start->y - centre->y;
         endvec->x = end->x - centre->x;
         endvec->y = end->y - centre->y;
         if(*close == NOCLOSE) *type = ELLIPARC;
         else *type = ELLIPARCCLOSE;
      }
   }
   else
   { /* GDP circle */
      if(circular)
      {  /* complete circle */
         onv.x = on->x - cen->x;
         onv.y = on->y - cen->y;
         radius = SQRT(onv.x*onv.x + onv.y*onv.y);
         *pt = *cen;
         (pt+1)->x = radius;
         *type = CIRCLE;
#ifdef DEBUG
         DMESS "onv(%f,%f)\n",onv.x,onv.y);
         DMESS "radius %f, centre(%f,%f)\n",radius,cen->x,cen->y);
#endif

      }
      else
      {  /* complete ellipse */

         /* Get backtransformed radius */
         onv.x = on->x - cen->x;           /* Get a radius vector */
         onv.y = on->y - cen->y;
         bonv.x = onv.x*bux.x + onv.y*buy.x;  /* Backtransform it */
         bonv.y = onv.x*bux.y + onv.y*buy.y;
         bradius = SQRT(bonv.x*bonv.x + bonv.y*bonv.y); /* Magnitude */

#ifdef DEBUG
         DMESS "onv(%f,%f)\n",onv.x,onv.y);
         DMESS "bradius %f, bonv(%f,%f), centre(%f,%f)\n",bradius,
                bonv.x,bonv.y,cen->x,cen->y);
#endif

         /* Output centre */
         *centre = *cen;

         /* Multiply CDP vectors by backtransformed radius */
         /* and add centre to obtain CDP points and output them. */
         cdp1->x = bradius*cdv1.x + centre->x;
         cdp1->y = bradius*cdv1.y + centre->y;
         cdp2->x = bradius*cdv2.x + centre->x;
         cdp2->y = bradius*cdv2.y + centre->y;
#ifdef DEBUG
DMESS "cdp1(%f,%f), cdp2(%f,%f)\n",cdp1->x,cdp1->y,cdp2->x,cdp2->y);
#endif

         /* Output Type */
         *type = ELLIPSE;
      }
   }

   return;
}
