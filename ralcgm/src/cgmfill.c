/*   RAL-CGM Interpreter module:  @(#) cgmfill.c  version 3.5
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
 * Description: Machine independent Fill Area routines for CGM
 *
 * Maintenance Log:
 *   3 Oct 88 RTP  Original version
 *  17 Nov 88 RTP  Use typedefs
 *                 Use pointers in SORT routine
 *  24 Nov 88 RTP  Add fill routines
 *  20 Jan 89 RTP  Correct polycheck routine
 *                 Put check for Simple after Horiz check
 *  13 Feb 89 RTP  Complete Complex Fill routine
 *                 Correct Horiz convex routine for case of same y values
 *  24 Oct 89 RTP  Create new version using Local Points
 *  16 Aug 90 RTP  Add error handler
 *  12 Sep 90 RTP  Change name of routines
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calloc calls to MALLOC and cast to (size_t)
 *  19 Jun 91 RTP  Add header file cgmatt.h to include function declarations
 *   2 Aug 91 RTP  Add SCCS id
 *   2 Sep 92 RTP  Remove LPOINT, CGMFILL and CGMEDGE macros.
 *                 Replace with function pointers
 *  23 Apr 93 RTP  Add code for trapezium decomposition
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *                 Change BACKCOLOUR entry from conditional so it is
 *                 output when the default BAckground is used
 *  24 Jan 95 RTP  Add POLget routine to update Polygon edge table
 */

/*
#define DEBUG
#define DEBUG_TEST
#define DEBUG_FULL
*/
#define CGMFILL_C

#include "cgmout.h"
#include "cgmgraph.h"
#include "cgmfill.h"
#include "cgmpoly.h"
#include "cgmtrap.h"

#define TRAPFILL    /* Use Trapezoid decomposition */
#undef HORIZFILL
#undef SIMPLEFILL

/*  Polygon Types  */

#define SIMPLE      (Enum) 0
#define HORIZCONVEX (Enum) 1
#define VERTCONVEX  (Enum) 2
#define COMPLEX     (Enum) 3
#define TRAPEZOID   (Enum) 4

#define NTZ       1
#define YTOL    0.0

#ifndef SIMPLEFILL
#ifdef PROTO

/* Fill a Horizontally Convex Polygon  */
   static void FILmonotone( Long, Point*, Enum,
                            void (*)( Long, Point* ) );

/* Break down a Complex Polygon into trapeziums */
   static void FILcomplex( Long, Point*, Enum*,
                           void (*)( Long, Point* ) );

/* Convert trapeziums into polygons */
   static void FILtrap( Int, Trapezoid* );

#else
   static void FILmonotone();
   static void FILcomplex();
   static void FILtrap();
#endif  /* ANSI */

/* Number of points allocated for fill area processing */
   static Long cgmmaxpts = MAXPTS;
   static void (*fill_func)();

#endif  /* SIMPLEFILL */

/*  Other utilities used */

#include "cgmgraph.h"

static char *func="FILarea", mess[40];


/**************************************************** FILarea **********/

#ifdef PROTO
void FILarea ( Long np, Long *pi, Float *pr, Enum set, Logical cont,
               void (*fill_convex)( Long, Point* ),
               void (*draw_edge)( Long, Point*, Enum* ) )
#else
void FILarea ( np, pi, pr, set, cont, fill_convex, draw_edge )

/*   breaks down polygon into convex areas
     for polygon of n points starting at *pi/*pr  */

Long np, *pi;
Float *pr;
Enum set;
Logical cont;
void (*fill_convex)();
void (*draw_edge)();
#endif
{
   register Long j, k;
   Logical newedge, nomem = FALSE, localpoint = FALSE;
   Enum *edgevis=NULL;
   Long *edgeflag=NULL;
   Int firstpt;
   Point *pt, *pt0;
   static Polygonset polset;  /* The polygonset */
   static Long start;         /* Starting place */
   Rect clip;                 /* Clipping rectangle or VDC extent */

   fill_func = fill_convex;

   /* Allocate space for polygon set stucture */
   if(!cont)
   {  /* First set of points, allocate space */
      start = 0;
      polset.n = abs(np);
      polset.pts = (Point *)MALLOC( polset.n, sizeof( Point));
      if(polset.pts == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      if(set == SET)
      {  /* Edge-out flags needed */
         polset.eofl = (Enum *)MALLOC( polset.n, sizeof( Enum));
         if(polset.eofl == NULL)
            exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      }
   }
   else
   {  /* Additional set of points, reallocate */
      start = polset.n;
      polset.n += abs(np);
      polset.pts = (Point *)realloc( (void *)polset.pts,
                              (size_t)(polset.n*sizeof(Point)));
      if(polset.pts == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      if(set == SET)
      {
         polset.eofl = (Enum *)realloc( (void *)polset.pts,
                                 (size_t)(polset.n*sizeof(Enum)));
         if(polset.eofl == NULL)
            exit ((int)CGMerror( func, ERR_NOMEMORY, FATAL, NULLSTR));
      }
   }

   /* Put data into polygon set stucture (get as Points) */
   /*
   PTSget (polset.n, pi, pr, polset.pts+start, polset.eofl+start);
   */
   POLget( polset.n,  pi,  pr,  set==SET, cont,  &polset);


#ifdef DEBUG
   DFLUSH;
   DMESS "FILarea: %d points", np );
   if ( set ) DMESS " (set)");
   DFLUSH;
#endif

/*  Allocate memory for points and edge types */

   pt0 = pt = (Point *) MALLOC( (2*np+1), sizeof ( Point ) );
   if ( set )
   {
      edgeflag = (Long *) MALLOC( (2*np+1), sizeof( Long ) );
      nomem += ( edgeflag == NULL);
      edgevis = (Enum *) MALLOC( (2*np+1), sizeof( Enum ) );
      nomem += ( edgevis == NULL);
      if ( nomem )
      {
         (void) sprintf ( mess, "(for fill area set processing)");
         (void) CGMerror ( func, ERR_NOMEMORY, ERROR, mess);
         FREE ( edgeflag );
         FREE ( edgevis );
         return;
      }
#ifdef DEBUG_FULL
   DMESS " %d bytes allocated for Points list\n",
           (2*np+1) * (sizeof( Int ) + sizeof( Enum ) ) );
#endif

   }

/*  Store points as real points list *pt create edge flags */

   for (j = ZERO, newedge = TRUE; j < np; j++, pt++)
   {
      if (cur.vdc_type == INTEGER)
      {
         pt->x = *pi++;
         pt->y = *pi++;
      }
      else
      {
         pt->x = *pr++;
         pt->y = *pr++;
      }
      if ( newedge )
      {
        firstpt = j;
        newedge = FALSE;
      }
      if ( set )
      {
         k = *pi++;
         edgevis[j] = k;
         if ( k & EDGECLOSE )  /*  close */
         {
            edgeflag[j] = firstpt + 1;
            edgeflag[firstpt] = - j - 1;
            newedge = TRUE;
         }
         else edgeflag[j] = 0;
      }
   }

/*  Reset Points lists */

   pt = pt0;

/*  Close polygon, if last point does not equal first  */

   if ( ! PEQUAL( pt+np-1, pt+firstpt ) )
   {
      if ( set )
      {
         edgeflag[firstpt] = -np;
         edgeflag[np-1] = firstpt + 1;
      }
      else
      {
         pt[np].x = pt[firstpt].x;
         pt[np].y = pt[firstpt].y;
         np++;
      }
   }

/* Now fill polygon, if not hollow or empty  */

   if (curatt.int_style != HOLLOW && curatt.int_style != EMPTY)
   {

#ifdef SIMPLEFILL
      fill_convex ( np, pt);
#ifdef DEBUG
    DMESS "Polygon fill: %d address: 0x%x\n", np, pt);
#endif
#else
/*  If Polygon Set and more than one polygon then use Complex routine */

      if ( set && firstpt )
      {
#ifdef DEBUG_TEST
          for ( j = 0; j < np; j++ )
          {
             DMESS "  Point: %d (%f,%f)  close: %d ",
                        j, pt[j].x, pt[j].y, edgeflag[j] );
             if ( edgevis[j] & VIS ) DMESS " (close)");
             if ( edgevis[j] & EDGECLOSE ) DMESS " (vis)\n");
             else DMESS " (invis)\n");
          }
#endif
#ifdef TRAPFILL
          TRPfill( &polset, NTZ, YTOL, &FILtrap );
#else
#ifdef COMPLEXFILL
          FILcomplex ( np, pt, edgeflag, fill_convex );
#else
          fill_convex ( np, pt);
#endif
#endif
      }
      else

/*  clasify polygon type  */

         switch ( FILpolycheck ( np, pt ) )
         {
            case SIMPLE:
#ifdef DEBUG
               DMESS " Polygon is Convex\n");
#endif
               fill_convex ( np, pt);
               break;

#ifdef HORIZFILL
            case HORIZCONVEX:
#ifdef DEBUG
               DMESS " Polygon is Horizontally Convex\n");
#endif
               FILmonotone ( np, pt, HORIZCONVEX, fill_convex );
               break;

            case VERTCONVEX:
#ifdef DEBUG
               DMESS " Polygon is Vertically Convex\n");
#endif
               FILmonotone ( np, pt, VERTCONVEX, fill_convex );
               break;
#endif

            case COMPLEX:
#ifdef DEBUG
               DMESS " Polygon is Complex\n");
#endif

            default:
#ifdef TRAPFILL
               TRPfill( &polset, NTZ, YTOL, &FILtrap );
#else
#ifdef COMPLEXFILL
               FILcomplex ( np, pt, (Enum*) NULL, fill_convex );
#else
               fill_convex ( np, pt);
#endif
#endif
               break;
        }
#endif
   }

/*  Now draw edge if visible  */

   if ( curatt.edge_vis == ON || curatt.int_style == HOLLOW )
   {
#ifdef DEBUG
      DMESS "Draw edge: %d address: 0x%x\n", np, pt);
#endif
      draw_edge( np, pt, edgevis );
   }

/*  free memory  */

   FREE( polset.pts );
   FREE( pt0 );
   if ( set )
   {
#ifdef DEBUG_FULL
   DMESS "  Free edge list: %x %x \n", edgeflag, edgevis );
#endif
      FREE(polset.eofl);
      FREE ( edgeflag );
      FREE ( edgevis );
   }

   return;
}


/****************************************************** FILpolycheck ***/

#ifdef PROTO
Enum FILpolycheck ( Long np, Point *pt )
#else
Enum FILpolycheck ( np, pt )

/*  Checks type of polygon - returns type value  */

Long np;
Point *pt;
#endif

{
   register Long i, j;
   register Float xprod, diff;
   register Long pos = 0, neg = 0;
   register Long changes, sign;

#ifdef DEBUG
    DMESS "FILpolycheck: %d points\n", np);
#endif

/*  trivial case of triangle  */

   if ( np <= 3 ) return (SIMPLE);

/* Check for Horizontally Convex  */

   sign = (pt[0].y > pt[np].y ? 1 : -1);

   for (i = 1, changes = ZERO; i < np; i++)
   {
      diff = pt[i].y - pt[i-1].y;
      if ( diff != ZERO )
      {
         if ( diff > ZERO )
         {
            if ( sign == -1 )  /*  Local minimum  */
            {
               changes++;
               if ( changes > 2 ) break;
               else sign = 1;
            }
         }
         else
         if ( sign == 1 )   /*  Local Maximum  */
         {
            changes++;
            if ( changes > 2 ) break;
            else sign = -1;
         }
      }
   }
   if ( changes <= 2 )
   {

/*  Check if this is a simple Polygon */

      for ( j = 0; j < np - 1; j++)
      {
         if ( j ) xprod = XPROD ( (pt+j-1), (pt+j), (pt+j), (pt+j+1) );
         else     xprod = XPROD ( (pt+np-1), (pt+0), (pt+0), (pt+1) );
         if ( xprod > 0 ) pos++;
         if ( xprod < 0 ) neg++;
      }

#ifdef DEBUG_TEST
   DMESS "  Polygon changes: %d %d\n", pos, neg );
#endif

/*  if either pos or neg is zero polygon is simple */

      return ( pos && neg ? HORIZCONVEX : SIMPLE );
   }

#ifdef DEBUG_TEST
   DMESS "  Horiz check failed at point: %d\n", i );
#endif

/* check for Vertically Convex  */

   sign = (pt[0].x > pt[np].x ? 1 : -1);

   for (i = 1, changes = ZERO; i < np; i++)
   {
      diff = pt[i].x - pt[i-1].x;
      if ( diff != 0 )
      {
         if ( diff > 0 )
         {
            if ( sign == -1 )  /*  Local minimum  */
            {
               changes++;
               if ( changes > 2 ) break;
               else sign = 1;
            }
         }
         else
         if ( sign == 1 )   /*  Local Maximum  */
         {
            changes++;
            if ( changes > 2 ) break;
            else sign = -1;
         }
      }
   }
   if ( changes <= 2 ) return (VERTCONVEX);

#ifdef DEBUG_TEST
   DMESS "  Vert check failed at point: %d\n", i );
#endif

   return (COMPLEX);
}


#ifdef HORIZFILL
/**************************************************** FILmonotone ******/

#ifdef PROTO
static void FILmonotone ( Long np, Point *pt, Enum type,
                          void (*fill_convex)( Long p, Point *pt ) )
#else
static void FILmonotone ( np, pt, type, fill_convex )

/*   Fills a horizontally Convex closed polygon of
     'np' points in Point Array '*pt' including close point,
     'type' is either Horiz or Vert Convex  */

Long np;
Point *pt;
Enum type;
void (*fill_convex)();
#endif
{
   Logical first = TRUE, close;
   register Long i;
   register Int *lp, *rp;
   Long maxv, xp = np - 1;
   Point *pl;
   Int *lp0;
   struct edges {
      Int cur, nxt;
      Point last, current, next;
   } left, right;
   Point xint;
   Float grad, dx, dy;
   Logical leftpt, rightpt, lastpt = FALSE;
   Long nlp = ZERO, nrp = ZERO, initcurve;
   Logical curveleft, curveright, coline, extra = FALSE;;

/* Note: The use of left/right is for convenience actual sides may
         be reversed without affecting the algorithm */

#ifdef DEBUG
   if ( type == HORIZCONVEX )
      DMESS "\n Decompose Horizontally convex\n");
   else
      DMESS "\n Decompose Vertically convex\n");
#endif

/* Allocate memory for chained points list and working points list */

   lp0 = (Int *) MALLOC( 2*np,sizeof( Int ) );
   pl = (Point *) MALLOC( np+1, sizeof( Point ) );
   if ( lp0 == NULL || pl == NULL )
   {
      (void) sprintf ( mess, "(for fill area processing)");
      (void) CGMerror ( func, ERR_NOMEMORY, ERROR, mess);
      return;
   }
   rp = lp = lp0 + np;

/*  find max y value  */

   if ( type == HORIZCONVEX )
   {
      for ( i = 1, maxv = ZERO; i < np; i++ )
         if ( ( VEQUAL(pt[i].y,pt[maxv].y) && pt[i].x < pt[maxv].x )
              || pt[i].y > pt[maxv].y ) maxv = i;
   }
   else
   {
      for ( i = 1, maxv = ZERO; i < np; i++ )
         if ( ( VEQUAL(pt[i].x,pt[maxv].x) && pt[i].y < pt[maxv].y )
              || pt[i].x > pt[maxv].x ) maxv = i;
   }

/*  Set pointers to first point  */

   *lp = *rp = maxv;
   left.cur = right.cur = maxv;

/*  store first point in left list  */

   nlp++;

/*  set initial Points values */

   left.current = right.current = pt[maxv];

/*    left.nxt is index to next left point  */

   left.nxt = maxv-1; if ( left.nxt < ZERO ) left.nxt = np - 1;
   left.next = pt[left.nxt];

/*    right.nxt is index to next right point */

   right.nxt = maxv+1; if ( right.nxt > np - 1 ) right.nxt = ZERO;
   right.next = pt[right.nxt];

/* Get first two points regardless  */

   leftpt = rightpt = TRUE;

/*  Find curvature at top point (clockwise)  */

   initcurve = ( XPROD (left.next,left.current,
                        left.current,right.next) >= 0.0 ? 1 : -1 );
   curveleft = curveright = FALSE;

#ifdef DEBUG_TEST
   DMESS "  Top point: %d = (%f,%f)  initcurve: %d next left: %d right: %d\n",
                      maxv, left.current.x, left.current.y, initcurve,
                      left.nxt, right.nxt );
#endif

/*  cycle through all the points  */

   while ( TRUE )
   {
      close = coline = FALSE;

      if (leftpt)
      {
 /*  Get Point, left.cur = current, left.nxt = next */

         left.cur = left.nxt;
         left.nxt--; if ( left.nxt < ZERO ) left.nxt = np - 1;

/*  swap points */

         left.last = left.current;
         left.current = left.next;
         left.next = pt[left.nxt];

/*  add current point to temp points list */

         *(++lp) = left.cur; nlp++;

/*  work out if curvature change at point pt[left.cur] */

         curveleft = ( ( XPROD(left.next,left.current,
                               left.current,left.last) >= 0.0 ? 1 : -1 )
                       != initcurve );

#ifdef DEBUG_FULL
         DMESS "  New Point (left) : %d (%f,%f)",
                             left.cur, left.current.x, left.current.y);
         DMESS "  next: %d  curve: %d\n", left.nxt, curveleft );
#endif
      }
      if (rightpt)     /*  Get next right Point */
      {
         right.cur = right.nxt;
         right.nxt++; if ( right.nxt > np - 1 ) right.nxt = ZERO;

         right.last = right.current;
         right.current = right.next;
         right.next = pt[right.nxt];

         *(--rp) = right.cur; nrp++;

/*  work out curvature at current point */

         curveright = ( ( XPROD(right.last,right.current,
                                right.current,right.next ) >= 0.0 ? 1 : -1 )
                        != initcurve );

#ifdef DEBUG_FULL
         DMESS "  New Point (right): %d (%f,%f)",
                            right.cur, right.current.x, right.current.y);
         DMESS "  next: %d  curve: %d\n", right.nxt, curveright );
#endif
      }

/* last point when current points are adjacent */

      lastpt = ( right.cur == left.nxt && left.cur == right.nxt );

#ifdef DEBUG_FULL
      DMESS "  Current Points: %d %d", left.cur, right.cur );
      if ( curveleft )  DMESS " Leftcurve ");
      if ( curveright ) DMESS " Rightcurve ");
      if ( lastpt ) DMESS " Last Point ");
      DMESS "  (left: %d) (right: %d)", nlp, nrp);
      DFLUSH;
      if (GRAcross( &left.last, &left.current, &right.current, &right.last,
                    &coline, &xint ) )
      {
           DMESS "\n  intersect: (%f,%f)", xint.x, xint.y);
           if ( coline ) DMESS "  (colinear)");
           DFLUSH;
      }
#endif

/*  last point after intersection  */
      if ( right.cur == left.cur )
      {
         if ( leftpt )
         {
            nlp--; lp--;
         }
         else
         {
            nrp--; rp++;
         }
         lastpt = close = TRUE;
      }

/*  Don't check for intersections first time */

      else if ( first ) first = FALSE;

      else if ( GRAcross( &left.last, &left.current,
                          &right.current, &right.last, &coline, &xint ) )
      {

/*     if intersection add Intersection point to Left Pointslist */

         if ( coline )
         {               /* if colinear add highest point */

/*  start with current left and right points  */

            *(lp) = ++xp;
            if ( leftpt )
            {
               pt[xp] = pt[right.cur];
            }
            else
            {
               pt[xp] = pt[left.cur];
            }
#ifdef DEBUG_FULL
            DMESS " Colinear Point: %d (%f,%f)\n",
                            *lp, pt[xp].x, pt[xp].y );
#endif

         }
         else   /*  add intersection point  */
         {

            *(lp) = ++xp;
            pt[xp] = xint;
#ifdef DEBUG_FULL
            DMESS "  Add Intersection point: %d (%f,%f)\n",
                                   xp, pt[xp].x, pt[xp].y );
#endif
            leftpt = rightpt = close = TRUE;
         }

/*  Make sure last point is covered */

         lastpt = FALSE;
         extra = TRUE;

      }
      else if ( lastpt )
      {
         close = TRUE;
      }

/*  if curvature change and point other edge is lower
    then  add horizontal intersection and close */

      else if ( curveright )
      {

#ifdef DEBUG_FULL
         DMESS "Curve change on right: %f %f\n", left.next.y, right.current.y);
#endif
         if ( (type == HORIZCONVEX && VEQUAL(left.next.y, right.current.y) ) ||
              (type == VERTCONVEX  && VEQUAL(left.next.x, right.current.x) ) )
         {
            leftpt = TRUE;
         }
         else
         {

/* if curvature is on right add nearest point on left */

            dx = (left.current.x - left.last.x);
            dy = (left.current.y - left.last.y);
            if ( type == HORIZCONVEX && ! VEQUAL(dy, 0.0) )
            {
               *(++lp) = ++xp;

/*  find intersection point  */

               pt[xp].y = right.current.y;
               grad = dx / dy;
               if ( REQUAL(grad, 0.0) ) pt[xp].x = left.last.x;
               else pt[xp].x = left.current.x +
                               (right.current.y - left.current.y) * grad ;

#ifdef DEBUG_FULL
               DMESS "   Add Extra point: %d (%f,%f)\n",
                                          xp, pt[xp].x, pt[xp].y );
#endif
               extra = TRUE;
            }
            if ( type == VERTCONVEX && ! VEQUAL(dx, 0.0) )
            {
               *(++lp) = ++xp;

/*  find intersection point  */

               pt[xp].x = right.current.x;
               grad = dy / dx;
               if ( REQUAL(grad, 0.0) ) pt[xp].y = left.last.y;
               else pt[xp].y = left.current.y +
                               (right.current.x - left.current.x) * grad ;

#ifdef DEBUG_FULL
               DMESS "   Add Extra point: %d (%f,%f)\n",
                                          xp, pt[xp].x, pt[xp].y );
#endif
               extra = TRUE;
            }
            leftpt = rightpt = close = TRUE;
         }
      }
      else if ( curveleft )
      {
#ifdef DEBUG_FULL
         DMESS "Curve change on left: %f %f\n", right.next.y, left.current.y);
#endif
         if ( (type == HORIZCONVEX && VEQUAL(right.next.y, left.current.y) ) ||
              (type == VERTCONVEX  && VEQUAL(right.next.x, left.current.x) ) )
         {
            rightpt = TRUE;
         }
         else
         {
            dx = (right.current.x - right.last.x);
            dy = (right.current.y - right.last.y);
            if ( type == HORIZCONVEX && ! VEQUAL(dy, 0.0) )
            {
               *(--rp) = ++xp;
               pt[xp].y = left.current.y;
               grad = dx / dy;
#ifdef DEBUG_FULL
          DMESS " left:  %d ( %f,%f )\n", *lp, left.current.x, left.current.y);
          DMESS " right: %d ( %f,%f )\n", *rp, right.current.x, right.current.y);
          DMESS " last left:  ( %f,%f )\n", left.last.x, left.last.y);
          DMESS " last right: ( %f,%f )\n",  right.last.x, right.last.y);
          DMESS "  grad: %f ( %f / %f )\n", grad, dx, dy);
#endif
               if ( REQUAL(grad, 0.0) ) pt[xp].x = right.last.x;
               else pt[xp].x = right.current.x +
                              (left.current.y - right.current.y) * grad;
#ifdef DEBUG_FULL
               DMESS "   Add Extra point: %d (%f,%f)\n",
                                          xp, pt[xp].x, pt[xp].y );
#endif
            }
            if ( type == VERTCONVEX && ! VEQUAL(dx, 0.0) )
            {
               *(--rp) = ++xp;
               pt[xp].x = left.current.x;
               grad = dy / dx;
               if ( REQUAL(grad, 0.0) ) pt[xp].y = right.last.y;
               else pt[xp].y = right.current.y +
                              (left.current.x - right.current.x) * grad;
#ifdef DEBUG_FULL
               DMESS "   Add Extra point: %d (%f,%f)\n",
                                          xp, pt[xp].x, pt[xp].y );
#endif
            }
            leftpt = rightpt = close = TRUE;
         }
      }

/*  Do not close but start new polygon */

      first = coline;

      if ( close )
      {
         Long n;
#ifdef DEBUG_FULL
         DMESS
          "(Close) Left points: %d right points: %d (%d) Points:", nlp, nrp,
                                lp-rp);
#endif

/*  Copy points to Local points list */

         n = nlp+nrp;
#ifdef DEBUG_FULL
         for ( i=0; i < n; i++, rp++ )
         {
            DMESS " %d", *rp );
         }
         rp -= n;
#endif
         for ( i=0; i < n; i++ )
         {
            *pl++ = pt[*rp++];
         }
         pl -= n;
#ifdef DEBUG_FULL
         DFLUSH;
#endif

/*  Output convex polygon if more than 2 points */

         if ( n > 2 ) fill_convex ( n, pl);

         if ( lastpt ) break;
      }

      if ( close || coline )
      {           /*  Reset temporary pointlist pointers */
         lp = lp0 + np;
         rp = lp;
         nlp = 1; nrp = 1;
         if ( extra )
         {
            *(++lp) = xp; nlp++;
            left.last = pt[xp];
            extra = FALSE;
         }
         else
         {
            left.last = right.current;
         }
         *(++lp) = left.cur;
         *rp = right.cur;
         right.last = left.current;
         curveleft = curveright = FALSE;
         first = TRUE;
/*
         *lp = xp; rp--;
         if ( leftpt )
         {
            *(++lp) = left.cur; nlp++;
         }
         if ( rightpt )
         {
            *(--rp) = right.cur; nrp++;
         }
*/
#ifdef DEBUG_FULL
         DMESS "  Initial Points:");
         if ( leftpt ) DMESS " (left) %d", left.cur );
         if ( rightpt ) DMESS " (right) %d", right.cur );
         DMESS " Extra: %d\n", xp );
#endif
      }

  /*  check which point to get next ( max y value ) */

      if ( type == HORIZCONVEX )
         leftpt = ( VEQUAL(left.next.y,right.next.y)
                                ? left.next.x > right.next.x
                                : left.next.y > right.next.y );
      else
         leftpt = ( VEQUAL(left.next.x,right.next.x)
                                ? left.next.y > right.next.y
                                : left.next.x > right.next.x );
      rightpt = !leftpt;

   }

/*  free space */

   FREE ( lp0 );
   FREE ( pl );

   return;
}
#undef DEBUG
#undef DEBUG_TEST
#undef DEBUG_FULL
#endif

#ifdef COMPLEXFILL
/**************************************************** FILcomplex *******/

#ifdef PROTO
static void FILcomplex ( Long np, Point *pt, Long *edgeflag,
                         void (*fill_convex)( Long, Point* ) )
#else
static void FILcomplex ( np, pt, edgeflag, fill_convex)

/*   Fills a Complex closed polygon of
     'np' points in Point Array '*pt' including close point
     'edgeflag' is an array of edge closure flags  */

Long np;
Point *pt;
Long *edgeflag;
void (*fill_convex)();
#endif
{
   register Int *ip;
   register i, j, k, el;
   Long nel = ZERO, maxel;
   Long nip = ZERO, npts = np, nxp = np;
   Int   *edgeint, *edge0,
        p, plp, prp, pxl, pxr, pnl, pnr, pn, px, lp = ZERO;

   Logical set, nomem, newedge, inside, close, extra, intersect,
           leftpnt, rightpnt, leftint, rightint, lefthoriz, righthoriz,
           edgecross, edgeupdate;

   Point xt0[4], *xt;
   Float xl, xr, yt, xp, yp, xpn, ypn, gl, gr;

   struct aet
   {
      Long points,           /* number of points 0 indicates inactive */
            left, right,      /* next left and right points */
            lt, lb,           /* left top and bottom points */
            rt, rb;           /* right top and bottom points */
      Float dxl, dyl,         /* left differences in x and y (top - bottom) */
            dxr, dyr;         /* right differences in x and y   "   " */
   } *ael0, *ael, *aelx;

/* Check for Polygon set */

   set = ( edgeflag != NULL );

#ifdef DEBUG
   DMESS " Polygon is Complex");
   if ( set ) DMESS " (set)");
   DFLUSH;
#endif

/* set initial number of extra points */

   nxp = npts = np;

/* Check for intersection */

   if ( GRAinsect ( np, &nip, pt, edgeflag ) )
   {
      if ( ! nip ) return;  /*  no space left */
      npts = nxp = np + nip;
      npts++;
   }

/*  Allocate memory for points index and edge list  */

   ip = (Int *) MALLOC( npts,sizeof( Int ) );
   nomem = ( ip == NULL);
   maxel = np/2 + 1;

   ael0 = (struct aet *) MALLOC ( maxel, sizeof (struct aet) );
   nomem += ( ael0 == NULL );

   edge0 = (Int *) MALLOC ( maxel, sizeof( Int ) );
   nomem += ( edge0 == NULL );

#ifdef DEBUG_FULL
   DMESS "  %d points %d intersections   total points: %d\n",
              np, nip, npts );
   DMESS "  Pointers  ip: %x (%d bytes)", ip, npts*sizeof(Int) );
   DMESS "  ael: %x (%d bytes) edge0: %x (%d bytes)\n",
                      ael0, maxel*sizeof (struct aet),
                      edge0, maxel*sizeof( Int ) );
#endif

   if ( nomem )
   {
      (void) sprintf ( mess, "(for fill area processing)");
      (void) CGMerror ( func, ERR_NOMEMORY, ERROR, mess);
      goto giveup;
   }

/*  Sort points in decreasing y and increasing x into pointer array ip */

   GRAsort ( npts, pt, ip);

#ifdef DEBUG_FULL
   DMESS "   Sorted Points:\n");
   for( i = ZERO; i < npts ; i++ )
   {
      p = ip[i];
      if ( p == np ) continue;  /* ignore closing point */
      DMESS " %4d : (%f,%f)\n", p, pt[p].x, pt[p].y );
   }
#endif

/*  Check through each point  */

   for ( i = ZERO; i < npts; i++)
   {

#ifdef DEBUG_FULL
   DMESS "\n*******************************************************\n");
#endif

/* get next point  */

      p = ip[i];

/* ignore closing point */

      if ( p == np ) continue;
      intersect = ( p > np );

/* Ignore repeated intersection points */

      if ( intersect && PEQUAL( pt[p], pt[lp] ) )
      {
#ifdef DEBUG_FULL
   DMESS "\n Point %d (%f,%f) repeated\n", p, pt[p].x, pt[p].y );
#endif
         continue;
      }

/* Get next point for checking whether to close trapezium */

      pn = ( i+1 < npts ? ip[i+1] : -1 );
      if ( pn == np ) pn = (intersect ? ip[i+2] : -1);
      if ( pn > 0 )
      {
         xpn = pt[pn].x; ypn = pt[pn].y;
      }

/*  Store current x and y value  */

      lp = p;
      xp = pt[p].x;
      yp = pt[p].y;

#ifdef DEBUG_FULL
   DMESS "\n Point: %d (%f,%f)  next: %d\n", p, xp, yp, pn );
#endif

/*  check if this is intersection point */

      if ( intersect )
      {

/*  set initial conditions for intersect, don't know adjacent points */

         plp = prp = -1;
      }
      else
      {

/*  Get adjacent points  */

         plp = ( p == ZERO ? np - 1: p - 1 );
         if ( set && edgeflag[p] < 0 )  plp = - edgeflag[p] - 1;

         prp = ( p == np - 1 ? ZERO : p + 1 );
         if ( set && edgeflag[p] > 0 )  prp =  edgeflag[p] - 1;

/*  Mark left and/or right as inactive if y value higher */

         if ( VEQUAL ( pt[plp].y, yp ) )
         {
            if ( pt[plp].x < xp ) plp = -1;
         }
         else if ( pt[plp].y > yp ) plp = -1;

         if ( VEQUAL ( pt[prp].y, yp ) )
         {
            if ( pt[prp].x < xp ) prp = -1;
         }
         else if ( pt[prp].y > yp ) prp = -1;

         if ( plp >= 0 && prp >= 0 )
         {

/* swap left and right if left has higher gradient */

            gl = atan2 ( pt[plp].y - yp, pt[plp].x - xp );
            if ( REQUAL( gl, PI ) ) gl = - PI;
            gr = atan2 ( pt[prp].y - yp, pt[prp].x - xp );
            if ( REQUAL( gr, PI ) ) gr = - PI;

            if ( gl > gr )
            {
               px = plp;
               plp = prp;
               prp = px;
#ifdef DEBUG_FULL
   DMESS " Swap left (%f) and right (%f)\n", gl, gr );
#endif
            }
         }
      }

/*  Set initial conditions for extra points */

      pxl = pxr = -1;
      px = -1;
      edgeint = edge0;
      newedge = TRUE;

#ifdef DEBUG_FULL
   if ( intersect ) DMESS " (intersect)" );
   else  DMESS " left: %d  right: %d ", plp, prp );
   DMESS " (next: %d)\n", pn);
#endif

/*  check through Active edge table */

      for ( el = 1, ael = ael0; el <= nel; el++, ael++ )
      {
#ifdef DEBUG_FULL
    DMESS "   Edge check: %d\n", el);
#endif

/*  skip if edge is not active */

         if ( !ael->points ) continue;

/*  initialise values for each edge check */

         leftint = rightint = FALSE;
         inside = edgeupdate = FALSE;
         edgecross = FALSE;
         close = extra = FALSE;

         lefthoriz = REQUAL (ael->dyl, 0.0 );
         righthoriz = REQUAL (ael->dyr, 0.0 );

/*  Check if point is adjacent in original polygon */

         leftpnt = ( p == ael->left );
         rightpnt = ( p == ael->right );

/*  Check if point is next left or right point in original polygon */

         if ( leftpnt || rightpnt )
         {

/*  Find next left or right point */

            if ( rightpnt ) ael->right =  ( prp < 0 ? plp : prp );
            if ( leftpnt )  ael->left = ( plp < 0 ? prp : plp );

/*  Add point to current edge list */

            switch ( ael->points )
            {
               case 1:
               case 2:
                  if ( VEQUAL( yp, pt[ael->lt].y ) )
                  {
/*  Same top y value, set right top values */

                     ael->rt = p;
                     if ( ael->right >= 0 )
                     {
                        ael->dxr = pt[ael->right].x - xp;
                        ael->dyr = pt[ael->right].y - yp;
                     }
                     ael->points = 2;
                  }
                  else  /*  new bottom to trapezium */
                  {
                     ael->points = 3;
                     if ( rightpnt ) ael->rb = p;
                     else            ael->lb = p;

/*  must be last point of edge */

                     if ( leftpnt && rightpnt ) close = TRUE;

/*  if not then extra point may be needed   */

                     else extra = TRUE;
                  }
                  break;

               case 3:  /*  last point  */
                  if ( ael->rb < 0 ) ael->rb = p;
                  else               ael->lb = p;
                  ael->points = 4;
                  close = TRUE;
                  break;
             }

/*  No new edge needed */

             newedge = FALSE;

#ifdef DEBUG_FULL
   DMESS "   Existing edge: left %d right: %d",
                                   ael->left, ael->right );
   if ( leftpnt ) DMESS " (leftpnt)");
   if ( rightpnt ) DMESS " (rightpnt)");
   if ( close ) DMESS " (close)");
   if ( extra ) DMESS " (extra)");
   DMESS "  Points (%d): %d %d %d %d\n",
                      ael->points, ael->lb, ael->lt, ael->rt, ael->rb);
#endif
         }

/* If not original point may need extra point */

         else extra = TRUE;

         if ( extra )
         {

/*  Find range of intersections of edge with horizontal through point */

            xr = xl = pt[ael->lt].x;
            yt = pt[ael->lt].y;

/* if edge is horizontal then must be intersection */

            if ( lefthoriz ) xl = xp;

/* else find left intersection point */

            else xl += (yp - yt) * ael->dxl / ael->dyl;


/* if right point is defined use it */

            if ( ael->rt >= 0 ) xr = pt[ael->rt].x;

/* Now work out right point */

            if ( righthoriz ) xr = xp;
            else xr += (yp - yt) * ael->dxr / ael->dyr;

#ifdef DEBUG_FULL
   DMESS "  Extra point check  left: %f right: %f", xl, xr );
#endif

/* if polygon point may still need extra point */

            extra = ( leftpnt || rightpnt );

/*  check if intersection with either edge or both */

            leftint = VEQUAL( xl, xp );
            rightint = VEQUAL( xr, xp );
            edgecross = leftint && rightint;
            if ( edgecross ) leftint = rightint = FALSE;

#ifdef DEBUG_FULL
   if ( edgecross ) DMESS " (edgecross)");
   if ( leftint ) DMESS " (leftint)");
   if ( rightint ) DMESS " (rightint)");
#endif

            if ( leftint && VEQUAL(yp, ypn) && VEQUAL(xr, xpn) )
            {

/* intersection on left and right equals next point, ignore intersect */

               leftint = rightint = FALSE;
               newedge = extra = FALSE;

/*  but need to update edges if this is an intersection */

               if ( intersect )
               {
                  ael->lb = p;
                  *edgeint++ = el;
               }
               edgeupdate = TRUE;
            }

            else if ( intersect )
            {
               newedge = FALSE;
               if ( edgecross )
               {

/*  if intersection on both edges update points */

                  if ( ael->lb < 0 ) ael->lb = p;
                  else ael->rb = p;

                  *edgeint++ = el;

/*  Close but do not need extra point */

                  extra = FALSE;
                  close = TRUE;

/* But need to swap edges */

                  edgeupdate = TRUE;
                  k = ael->left;
                  ael->left = ael->right;
                  ael->right = k;
               }
               else if ( rightint && righthoriz )
               {
/* intersection on horizontal add to top */

                  ael->rt = p;
                  *edgeint++ = el;
                  rightint = extra = FALSE;
               }
               else if ( leftint )
               {
/* Left intersection on one edge */

                  pxl = ael->right;
                  ael->lb = p;
                  *edgeint++ = el;
                  extra = TRUE;
               }
               else if ( rightint )
               {
/* Right intersection on one edge */

                  pxr = ael->left;
                  ael->rb = p;
                  *edgeint++ = el;
                  extra = TRUE;
               }
            }
            else if ( xp > xl && xp < xr )
            {

/* Original point and trapezium bottom and within edge */

               leftint = rightint = TRUE;
               if ( ++nxp > cgmmaxpts )
               {
                  (void) CGMerror ( func, ERR_BIGPOINTS, ERROR, NULLSTR);
                  goto giveup;
               }

/*  get point on right  */

               pt[nxp].x = xr; pt[nxp].y = yp;
               ael->rb = nxp;
#ifdef DEBUG_FULL
   DMESS " (inside)\n");
   DMESS "   Add Points: (right) %d (%f,%f)", nxp, xr, yp );
#endif
               if ( ++nxp > cgmmaxpts )
               {
                  (void) CGMerror ( func, ERR_BIGPOINTS, ERROR, NULLSTR);
                  goto giveup;
               }

/*  get point on left */

               pt[nxp].x = xl; pt[nxp].y = yp;
               ael->lb = nxp;
#ifdef DEBUG_FULL
   DMESS " (left) %d (%f,%f)", nxp, xl, yp );
#endif
               extra = FALSE;
            }
#ifdef DEBUG_FULL
   DFLUSH;
#endif
         }   /*  end of extra point check */

#ifdef DEBUG_FULL
   if ( lefthoriz ) DMESS "    Left" );
   if ( lefthoriz && righthoriz ) DMESS " and" );
   if ( righthoriz ) DMESS "    Right" );
   if ( lefthoriz || righthoriz ) DMESS " edge is horizontal" );
   if ( extra ) DMESS " (extra)" );
   DFLUSH;
#endif

         if ( extra )
         {
            leftint = rightint = FALSE;
            newedge = FALSE;

/*  Add extra point to complete trapezium  */

            if ( ++nxp > cgmmaxpts )
            {
               (void) CGMerror ( func, ERR_BIGPOINTS, ERROR, NULLSTR);
               goto giveup;
            }
            if ( ael->rb < 0 )
            {
               pt[nxp].x = xr; pt[nxp].y = yp;
               ael->rb = nxp;
               rightint = TRUE;
#ifdef DEBUG_FULL
   DMESS "   Add right Point: %d (%f,%f)\n", nxp, xr, yp );
#endif
            }
            else
            {
               pt[nxp].x = xl; pt[nxp].y = yp;
               ael->lb = nxp;
               leftint = TRUE;
#ifdef DEBUG_FULL
   DMESS "   Add left Point: %d (%f,%f)\n", nxp, xl, yp );
#endif
            }
         }

/* if extra point generated update edges */

         if ( leftint || rightint )
         {
             edgeupdate = TRUE;

/* if not horizontal, force close */

             if ( ! lefthoriz && ! righthoriz )
                 close = inside = TRUE;
         }

/*  close edge table and output  */

         if ( close )
         {
#ifdef DEBUG_TEST
   DMESS "Output points:");
#endif
            xt = xt0;
            k = 0;
            pnl = ael->lb;
            pnr = ael->rb;

            if ( pnl >= 0 )
            {
               (*xt).x = pt[pnl].x;
               (*xt++).y = pt[pnl].y;
               k++;
#ifdef DEBUG_TEST
   DMESS " %d", pnl);
#endif
            }
            if ( ael->lt >= 0 )
            {
               (*xt).x = pt[ael->lt].x;
               (*xt++).y = pt[ael->lt].y;
               k++;
#ifdef DEBUG_TEST
   DMESS " %d", ael->lt);
#endif
            }
            if ( ael->rt >= 0 )
            {
               (*xt).x = pt[ael->rt].x;
               (*xt++).y = pt[ael->rt].y;
               k++;
#ifdef DEBUG_TEST
   DMESS " %d", ael->rt);
#endif
            }

            if ( pnr >= 0 )
            {
               (*xt).x = pt[pnr].x;
               (*xt++).y = pt[pnr].y;
               k++;
#ifdef DEBUG_TEST
   DMESS " %d", pnr);
#endif
            }

/* check if edge is still active, inactive if lb & rb are consecutive*/

            inside = ! ( ael->rb < np && ael->rb >= 0 &&
                         ael->lb < np && ael->lb >= 0 &&
                         abs ( ael->rb - ael->lb ) < 2 ) &&
                         ! ( leftpnt && rightpnt );

/* set new left and right values */

            pnl = ael->left;
            pnr = ael->right;
#ifdef DEBUG_FULL
   DFLUSH;
   DMESS " Output %d points  new left: %d new right: %d\n", k, pnl, pnr);
#endif

/*  Output trapezium/triangle  */

            if ( k > 2 ) fill_convex ( k, pt );

/*  Check each active edge for same end points */

            if ( ! intersect && inside )
            {
               for ( j = 1, aelx = ael0; j <= nel ; j++, aelx++ )
                  if ( aelx->points && aelx != ael )
                  {
                     if ( aelx->rt >= 0 && aelx->rt == ael->lb )
                     {
                        aelx->rt = ael->rb;
                        aelx->right = pnr;
                        aelx->dxr = pt[aelx->right].x - pt[aelx->rt].x;
                        aelx->dyr = pt[aelx->right].y - pt[aelx->rt].y;

/* Make current edge inactive  */

                        inside = FALSE;
#ifdef DEBUG_FULL
   DMESS "  Change right point for edge %d to %d\n", j, aelx->right );
#endif
                     }
                     else if ( aelx->lt >= 0 && aelx->lt == ael->rb )
                     {
                        aelx->lt = ael->lb;
                        aelx->left = pnl;
                        aelx->dxl = pt[aelx->left].x - pt[aelx->lt].x;
                        aelx->dyl = pt[aelx->left].y - pt[aelx->lt].y;

/* Make current edge inactive  */

                        inside = FALSE;
#ifdef DEBUG_FULL
   DMESS "  Change left point for edge %d to %d\n", j, aelx->left );
#endif
                     }
                  }
            }

/*  Make new edge if edge is still visible */

            if ( inside && ( ael->right >= 0 || ael->left >= 0 ) )
            {

/* top point is left bottom unless intersect and left edge horizontal */

               ael->lt = ( edgecross && lefthoriz ? ael->rb : ael->lb );

               if ( ! intersect && leftint && rightint )
               {

/*  Update current edge for extra point */

                  ael->rt = p;
                  pxr = ael->right;
                  ael->right = plp;

/*  Store values for new edge */

                  pxl = prp;
                  px = ael->rb;
                  newedge = TRUE;
               }
               else
               {

/*  only update current edge for one new point */

                  if ( edgecross ) ael->rt = -1;
                  else ael->rt = ael->rb;

                  pxr = ael->right;
                  pxl = -1;
               }
               edgeupdate = TRUE;

               ael->lb = ael->rb = -1;
               ael->points = 2;

#ifdef DEBUG_FULL
   DMESS "Point inside edge %d (left: %d  right: %d)\n", el, pxl, pxr );
   DMESS "   edge values  left: %d right: %d", ael->left, ael->right );
   DMESS "  Points (%d): %d %d %d %d\n",
                      ael->points, ael->lb, ael->lt, ael->rt, ael->rb);
#endif
            }
            else
            {

/*  Make edge inactive */

               ael->points = 0;
            }
         }  /*  end of close processing */

         if ( edgeupdate )
         {

/*  Recalculate edge increments  */

            k = ( ael->lb >= 0 ? ael->lb : ael->lt);
            if ( ael->left >= 0 )
            {
               ael->dxl = pt[ael->left].x - pt[k].x;
               ael->dyl = pt[ael->left].y - pt[k].y;
            }
            k = ( ael->rb >= 0 ? ael->rb :
                               ( ael->rt >= 0 ? ael->rt
                                              : ael->lt ) );
            if ( ael->right >= 0 )
            {
               ael->dxr = pt[ael->right].x - pt[k].x;
               ael->dyr = pt[ael->right].y - pt[k].y;
            }
#ifdef DEBUG_FULL
   DMESS " Edge increments dxl: %.2f dyl: %.2f  dxr: %.2f dyr: %.2f\n",
                   ael->dxl, ael->dyl, ael->dxr, ael->dyr );
#endif
         }
      }  /*  end of loop for each edge */

/*  start new edge list  */

      if ( newedge )
      {
         /* search for first inactive edge */

         for ( el = 1, ael = ael0; el <= maxel; el++, ael++ )
            if ( el > nel || ! ael->points )
            {
               if ( el > nel ) nel = el;
               break;
            }

/*  no of edges exceeds edge list capacity  */

         if ( el > maxel )
         {
            (void) CGMerror ( func, ERR_BIGEDGES, ERROR, NULLSTR);
            goto giveup;
         }

         ael->points = 1;
         if ( pxl >= 0 ) plp = pxl;
         if ( pxr >= 0 ) prp = pxr;
         ael->left =  plp;
         ael->right = prp;
         ael->lt = p;

/*  rt is set to -1 unless the edge already has two points */

         ael->rt = px;
         ael->lb = ael->rb = -1;
         if ( plp >= 0 )
         {
            ael->dxl = pt[plp].x - pt[p].x;
            ael->dyl = pt[plp].y - pt[p].y;
         }
         if ( px < 0 ) px = p;
         if ( px >= 0 )
         {
            ael->dxr = pt[prp].x - pt[px].x;
            ael->dyr = pt[prp].y - pt[px].y;
         }

#ifdef DEBUG_FULL
   DMESS "   New edge: %d\n", el );
#endif
      }

/* Check edges for possible swaps
   because of intersection through same point */

      if ( intersect )
      {
         int edges, *edge1, *edge2;
         Long p0, *p1, *p2;
         struct aet *e1, *e2;
         Float gl1, gl2, gr1, gr2,
               px0, *px1, *px2;
         Logical left1, right1, left2, right2;

         edges = edgeint - edge0;

#ifdef DEBUG_FULL
   DMESS "   Check %d edges:", edges);
   for ( edge1 = edge0; edge1 < edgeint; edge1++)
      DMESS " %d", *edge1);
   DFLUSH;
#endif

         for ( edge1 = edge0, j = 0; j < edges - 1; j++, edge1++ )
         {
            e1 = &ael0[*edge1 - 1];

/* Check which edge passes through current point */

            left1 = ( ( e1->lt >= 0 && PEQUAL(pt[e1->lt], pt[p]) ) ||
                      ( e1->lb >= 0 && PEQUAL(pt[e1->lb], pt[p]) ) );

            right1 = ( ( e1->rt >= 0 && PEQUAL(pt[e1->rt], pt[p]) ) ||
                       ( e1->rb >= 0 && PEQUAL(pt[e1->rb], pt[p]) ) );

            if ( ! left1 && ! right1 ) continue;

/*  find true left and right gradients  */

            gl1 = atan2 ( e1->dyl, e1->dxl );
            gr1 = atan2 ( e1->dyr, e1->dxr );

#ifdef DEBUG_FULL
   DMESS "   Gradient edge %d  left: %f right: %f",
                         *edge1, gl1, gr1 );
   if (left1) DMESS " (left1)");
   if (right1) DMESS " (right1)");
   DFLUSH;
#endif
            edge2 = edge1;
            for ( edge2++; edge2 < edgeint; edge2++ )
            {
               e2 = &ael0[*edge2 - 1];

/* Check which edge passes through current point */

               left2 = ( ( e2->lt >= 0 && PEQUAL(pt[e2->lt], pt[p]) ) ||
                         ( e2->lb >= 0 && PEQUAL(pt[e2->lb], pt[p]) ) );

               right2 = ( ( e2->rt >= 0 && PEQUAL(pt[e2->rt], pt[p]) ) ||
                          ( e2->rb >= 0 && PEQUAL(pt[e2->rb], pt[p]) ) );

               if ( ! left2 && ! right2 ) continue;

/*  find left and right gradients */

               gl2 = atan2 ( e2->dyl, e2->dxl );
               gr2 = atan2 ( e2->dyr, e2->dxr );

#ifdef DEBUG_FULL
   DMESS "            edge %d  left: %f right: %f",
                         *edge2, gl2, gr2 );
   if (left2) DMESS " (left2)");
   if (right2) DMESS " (right2)");
   DFLUSH;
#endif


/*  Find which edges overlap */

               if ( left1 && right1 && left2 && right2 )
               {
                  if ( ( gl1 < gl2 && gr1 < gl2 ) ||
                       ( gl1 > gl2 && gr1 > gl2 ) ) continue;
                  left1 = ( gr1 > gl2 );
                  right1 = ! left1;
                  left2 = ( gr2 > gl1 );
                  right2 = ! left2;
               }
               else if ( left1 && right1 && left2 )
               {
                  if ( gl2 < gl1 || gl2 > gr1 ) continue;
                  left1 = FALSE;
               }
               else if ( left1 && right1 && right2 )
               {
                  if ( gr2 < gl1 || gr2 > gr1 ) continue;
                  right1 = FALSE;
               }
               else if ( left1 && left2 && right2 )
               {
                  if ( gl1 < gl2 || gl1 > gr2 ) continue;
                  left2 = FALSE;
               }
               else if ( right1 && left2 && right2 )
               {
                  if ( gr1 < gl2 || gr1 > gr2 ) continue;
                  right2 = FALSE;
               }
               else if ( right1 && left2 )
               {
                  if ( gr1 < gl2 ) continue;
               }
               else if ( left1 && right2 )
               {
                  if ( gr2 < gl1 ) continue;
               }
               else continue;

#ifdef DEBUG_FULL
   if (left1) DMESS " (left1)");
   if (right1) DMESS " (right1)");
   if (left2) DMESS " (left2)");
   if (right2) DMESS " (right2)");
   DFLUSH;
   DMESS "  Swap edges %d", *edge1 );
#endif

/*  work out which edges overlap */

               if ( left1 )
               {
                  p1 = &e1->left;
                  px1 = &e1->dxl;
#ifdef DEBUG_FULL
   DMESS " (left)" );
#endif
               }
               else
               {
                  p1 = &e1->right;
                  px1 = &e1->dxr;
#ifdef DEBUG_FULL
   DMESS " (right)" );
#endif
               }

#ifdef DEBUG_FULL
   DMESS " and %d", *edge2 );
#endif
               if ( left2 )
               {
                  p2 = &e2->left;
                  px2 = &e2->dxl;
#ifdef DEBUG_FULL
   DMESS " (left)\n" );
#endif
               }
               else
               {
                  p2 = &e2->right;
                  px2 = &e2->dxr;
#ifdef DEBUG_FULL
   DMESS " (right)\n" );
#endif
               }

/*  Swap left and right points */

               p0 = *p1;
               *p1 = *p2;
               *p2 = p0;

/*  Swap x increments */

               px0 = *px1;
               *px1 = *px2;
               *px2 = px0;

/*  Swap y increments */

               px1++; px2++;

               px0 = *px1;
               *px1 = *px2;
               *px2 = px0;
            }
         }
      }

#ifdef DEBUG_FULL
   DMESS "\n Active Edges (%d):\n", nel);
      for ( j = 1, ael = ael0; j <= nel ; j++, ael++ )
         if ( ael->points )
         {
            DMESS "   edge: %d  left: %d right: %d",
                             j, ael->left, ael->right );
            DMESS "  Points (%d): %d %d %d %d\n",
                      ael->points, ael->lb, ael->lt, ael->rt, ael->rb);
            DMESS "             dxl: %.2f dyl: %.2f  dxr: %.2f dyr: %.2f\n",
                      ael->dxl, ael->dyl, ael->dxr, ael->dyr );
         }
#endif
   }

giveup:

#ifdef DEBUG_FULL
   DMESS "  Free pointers ip: %x ael: %x\n", ip, ael0 );
#endif

   FREE ( ip );

   FREE ( ael0 );

   FREE ( edge0 );

   return;
}
#endif

#ifdef TRAPFILL
/***************************************************** FILtrap ***************/
#ifdef PROTO
static void FILtrap ( Int nt, Trapezoid *trap )
#else
static void FILtrap ( nt, trap )

/*  Fills nt Trapezoids *pt using device dependent Convex fill routine */
Int nt;
Trapezoid *trap;
#endif

{
   int i;
   Point p[4];
   Trapezoid *pt = trap;

#ifdef DEBUG
   DMESS "%d Trapeziums sent to device\n",nt);
#endif

   for ( i = 0; i<nt; i++, pt++ )
   {
      p[0].x = pt->top_left;
      p[1].x = pt->top_right;
      p[0].y = p[1].y = pt->top;

      p[2].x = pt->base_right;
      p[3].x = pt->base_left;
      p[2].y = p[3].y = pt->base;

      fill_func( (Long)4, p );
   }

   return;
}
#endif
