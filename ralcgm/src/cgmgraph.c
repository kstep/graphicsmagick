/*   RAL-CGM Interpreter module:  @(#) cgmgraph.c  version 3.4
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
 * Description: Output and graphics utilities
 *
 * Maintenance Log:
 *   3 Oct 88 RTP  Original version
 *  17 Nov 88 RTP  Use typedefs
 *                 Use pointers in SORT routine
 *   1 Feb 89 RTP  Put into separate file
 *  16 Aug 90 RTP  Add error Handler
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  14 May 91 RTP  Add ANSI declarations
 *  19 Jun 91 RTP  Add header file cgmatt.h to include function declarations
 *   2 Aug 91 RTP  Add SCCS id
 *  26 Feb 92 KEVP Add Cell Array Parameters Utility CELLparget
 *  26 Feb 92 KEVP Add Points list Utility PTSget
 *   5 Mar 92 KEVP Add Clipping Utilities CLIPget and LINEclip
 *   5 Mar 92 KEVP Made 'np' in PTSget and LINEclip into Long integer
 *   9 Mar 92 KEVP Add Marker Utility MARKer
 *  13 Mar 92 KEVP Add Cell Array Utilities CELLtran and CELLfill
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *  12 May 95 KEVP In GRAcross improve test for colinearity and use double
 *                 precision for internal variables
 *
 */

#define CGMGRAPH_C


#include "cgmout.h"
#include "cgmgraph.h"

/* Maximum number of points allocated for intersections */

static Long cgmmaxpts = MAXPTS;

/* Macros for common Maths functions  */

#define CLIPCODE(x,xmin,xmax) (((x>xmax)<<1)|(x<xmin))
/* Values of Clipcode */
#define CLIPNO    (Enum) 0
#define CLIPMIN   (Enum) 1
#define CLIPMAX   (Enum) 2
/* Values of edgestatus */
#define INCOMING  (Enum) 1
#define INSIDE    (Enum) 0
#define OUTGOING  (Enum) 2
#define BOTHOUT   (Enum) 3
/* Values of the marker clipping indicator */
#define DRAWALL   (Enum) 0
#define CLIPDRAW  (Enum) 1
#define NODRAW    (Enum) 2
/* Clip function */
#define ECLIP(y1,y2,x1,x2,xm) y1+(xm-x1)*(y2-y1)/(x2-x1)
/* Clockwise function for 3 points */
#define CLOCK(a,b,c) ( ((b)->x - (a)->x)*((c)->y - (a)->y)\
                     > ((c)->x - (a)->x)*((b)->y - (a)->y) )

/* Parameters for Marker utilities */
#define OCTS  0.41
#define MDMAX 9
#define MLMAX 9

/* statics for CGM Colour table */
static Long      num_colours = 0L;    /* length of colour table */
static RGBcolour *colour_table;       /* colour table */
static Logical   *colour_defd;        /* colour defined flags */

/****************************************************** GRAcross *******/

#ifdef PROTO
Logical GRAcross ( Point *a, Point *b, Point *c, Point *d,
                   Logical *coline, Point *pt )
#else
Logical GRAcross ( a, b, c, d, coline, pt )

/*  Given two lines a - b and c - d,
    returns TRUE if they intersect and finds intersection point x.
    if points are co-linear coline is TRUE */

Point *a, *b, *c, *d, *pt;
Logical *coline;
#endif

{
   register double det, grad;
   double x1, x2, x3, x4;

#ifdef DEBUG1
   DMESS "GRAcross: (%.4f,%.4f) (%.4f,%.4f) (%.4f,%.4f) (%.4f,%.4f)\n",
                       a->x, a->y, b->x, b->y, c->x, c->y, d->x, d->y);
#endif

/* First check if opposite cross products have same signs */

   x1 = XPROD ( a, c, a, d);
   x2 = XPROD ( b, c, b, d);
   if ( (x1 < 0.0 && x2 < 0.0) || (x1 > 0.0 && x2 > 0.0) ) return FALSE;
   x3 = XPROD ( c, a, c, b);
   x4 = XPROD ( d, a, d, b);
   if ( (x3 < 0.0 && x4 < 0.0) || (x3 > 0.0 && x4 > 0.0) ) return FALSE;

/*  intersect - find intersection point */

   det = XPROD ( c, d, a, b );

   if ( FABS(det) < 0.5*cur.vdcmin*cur.vdcmin ) /* lines are colinear */
   {
/*  check if they are distinct */

      if ( (a->x < c->x && a->x < d->x && b->x < c->x && b->x < d->x ) ||
           (a->x > c->x && a->x > d->x && b->x > c->x && b->x > d->x ) ||
           (a->y < c->y && a->y < d->y && b->y < c->y && b->y < d->y ) ||
           (a->y > c->y && a->y > d->y && b->y > c->y && b->y > d->y ) )
                 return FALSE;

/*  Check if end points are the same  */

      if ( PEQUAL ( a, c ) || PEQUAL ( a, d ) )
      {
         *pt = *a;
         return TRUE;
      }

      if ( PEQUAL ( b, c ) || PEQUAL ( b, d ) )
      {
         *pt = *b;
         return TRUE;
      }
      pt->x = a->x + b->x + c->x + d->x / 4.0;
      pt->y = a->y + b->y + c->y + d->y / 4.0;
#ifdef DEBUG
      DMESS " Lines are colinear and overlapping ");
#endif
      *coline = TRUE;
      return TRUE;
   }
   *coline = FALSE;

   grad = XPROD ( a, b, a, c ) / det;
   pt->x = c->x + (d->x - c->x) * grad;
   pt->y = c->y + (d->y - c->y) * grad;

   return TRUE;
}

/****************************************************** GRAinsect ******/

#ifdef PROTO
Logical GRAinsect ( Long np, Long *nip, Point *pt, Enum *edgeflag )
#else
Logical GRAinsect ( np, nip, pt, edgeflag )

/* Find intersections and add to points list */

Long np, *nip;
Enum *edgeflag;
Point *pt;
#endif

{
   Long register i, j, m, n, npts = np;
   Logical coline, set;
   Point pi;

#ifdef DEBUG2
   DMESS "GRAinsect: %d points\n", np );
#endif

   set = ( edgeflag != NULL);

   for ( *nip = i = 0; i < np - 1; i++ )
   {
      for ( j = m = i+1; j < np; j++ )
      {
         n = j + 1;

         if ( set )
         {
            if ( edgeflag[i] > 0 ) m = edgeflag[i] - 1;
            if ( edgeflag[j] > 0 ) n = edgeflag[j] - 1;
         }
         else if ( n == np ) n = 0;

/* Ignore if any end points coincide */

         if ( i == j || i == n || m == j || m == n ) continue;

/* Ignore if line intersects at either end  */

         if ( PEQUAL(&pt[i], &pt[j]) || PEQUAL(&pt[i], &pt[n]) ||
              PEQUAL(&pt[m], &pt[j]) || PEQUAL(&pt[m], &pt[n]) ) continue;

         if ( GRAcross( &pt[i], &pt[m], &pt[j], &pt[n], &coline, &pi ) &&
              ! coline )
         {
            npts++; (*nip)++;
            if ( npts >= cgmmaxpts )
            {
               (void) CGMerror ( "GRAinsect", ERR_BIGINTS, ERROR, NULLSTR);
               *nip = 0;
               return TRUE;
            }
            pt[npts] = pi;
#ifdef DEBUG1
   DMESS "   Points %d/%d & %d/%d Intersect", i, m, j, n );
   DMESS "   Add Point: %d (%f,%f)\n", npts, pi.x, pi.y );
#endif
         }
      }

/*  prevent finding intersection of first and second line */

/*
      if ( ! i ) k++;
*/
   }

   return ( npts > np );
}

/****************************************************** GRAsort ********/

#ifdef PROTO
void GRAsort ( Long np, Point *pt, Int *po )
#else
void GRAsort ( np, pt, po )

/*   Sorts points in array pt, returns sorted points in index array px
     using a 'shell' sort technique  */

Long np;
Int *po;
Point *pt;
#endif
{
   register Long i, k, t, incr;
   register Int *px = po;

#ifdef DEBUG
   Long exch = 0, comp = 0;

   DMESS "GRAsort: %d points", np);
#endif

/*  Preload index with current points  */

   for ( i = 0; i < np; ) *px++ = i++;

   for ( incr = np>>1; incr > 0; incr >>= 1 )
   {
      for ( i = incr; i < np; i++ )
      {
         px = po + i - incr;
         while ( px >= po )
         {
            t = *px;
            k = *(px + incr);
#ifdef DEBUG
            comp++;
#endif

/*  Sort condition y descending and x ascending  */

            if ( ( pt[t].y < pt[k].y ) ||
                   ( VEQUAL(pt[t].y, pt[k].y) && pt[t].x > pt[k].x ) )
            {
#ifdef DEBUG
               exch++;
#endif
               *px = k;
               *(px+incr) = t;
               px -= incr;
            }
            else break;
         }
      }
   }
#ifdef DEBUG
   DMESS "  %d comparisons  %d exchanges\n", comp, exch);
#endif
   return;
}
