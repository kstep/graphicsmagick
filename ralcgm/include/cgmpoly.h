/*   RAL-CGM Interpreter header file:  @(#) cgmpoly.h  version 3.4
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
 * Description:  CGM Polygon set and clipping function typedefs and
 *               declarations
 *
 * Modification log:
 *
 *   2 Feb 93 KEVP Original Version working on Simple Tests
 *  23 Jan 95 KEVP Add utility to get data into polygon set structure.
 */

#ifndef CGMPOLY_H
#define CGMPOLY_H

#ifdef CGMPOLY_C
#define Extern
#else
#define Extern extern
#endif

/* Typedef for Polygon Set */
typedef struct {
   Long n;          /* Number of points or edge-out flags */
   Point *pts;      /* The points */
   Enum  *eofl;     /* The edge-out flags */
   } Polygonset;

/* Typedef from data on crossings of the clip rectangle boundary
   by the polygon edges */

typedef struct {
   Index *out;      /* Outer ends of crossing edges */
   Index *in;       /* Inner ends of crossing edges */
   Int   *pol;      /* Polygon of crossing */
   Int   *inoth;    /* Crossing at Other end of clipped section */
   Int   *outoth;   /* .. at Other end of clip induced section */
   Enum  *cl;       /* Clip status of crossings (See below) */
   Float *xy;       /* Signifacant Coordinates */
   Int   *ord;      /* Order of crossings on clip rectangle */
   Logical *clock;  /* True, if clockwise on clip induced sect' */
   } Clipcross;

/* Typedefs for polygon section lists */

typedef struct {
   Point *start, *end;      /* Start and End vertices */
} Section;

/*
A section is a consecutive group of edges in a polygon.
If .start < .end,  Edges are .start to .start+1 to .start+2 .. to .end .
If .start > .end,  Edges are .start to .start-1 to .start-2 .. to .end .
Finally there is an edge from .end to .start of the next section.
*/

typedef struct
{
   Int cross1, cross2;   /* Pair of consecutive crossings */
   Enum sectype;         /* Type of section to be formed by crossings */
} Xcursor;  /* Cursor on Clipcross to define sections */

typedef struct
{
   Point *p;          /* Current Vertex */
   Section *s;        /* Current Section */
   Section *firstsec; /* The last section of current polygon */
   Section *lastsec;  /* The last section of current polygon */
   short d;           /* p+d is next vertex, if in same section */
} Scursor;  /* Cursor on section list */

/* Macro to move Scursor on step forward */
#define MOVE_SCURSOR(ps,a) if(a.p!=a.s->end){a.p+=a.d;}\
else POLnext_sl(ps,&a);

typedef struct sectlist
{
   Section *first;        /* First section in list */
   Section *vacant;       /* First vacancy in list */
   Section **poly;        /* Starts of polygons (NULL if not known) */
   Point *newpts;         /* New points used in sections */
   Point *freept;         /* First vacant point in new points */
   Int msect, mpts;       /* Number of sections & new points allocated */
   Int mpoly;             /* Number of Polygons allocated */
   Int npoly;             /* Actual number of Polygons */
   Logical contig;        /* True, if sections are contiguous */
} Sectlist;  /* Polygon Section List */

#ifdef PROTO
                          /* PolygonSet utilities */

   Extern void
      /* Get data into a polygon set structure */
      POLget ( Long, Long *, Float *, Logical, Logical, Polygonset *),

      /* Draw edges of a polygon set */
      POLedge ( Polygonset *, Rect *, void(*)(long, Point *) ),

      /* Find either or both the end vertex indices of a polygon */
      POLend ( Long, Enum *, Index, Index *, Index *);

   Extern Polygonset
      /* Clip a polygon (set) */
      * POLhandle( Polygonset *, Rect *, Logical, Polygonset * );

   Extern Int
      /* Return the number of polygons (or 0 if invalid) */
      POLnumber( Long, Enum *);

   Extern Long
      /* Count the number of self intersections of one polygon */
      POLselfx( Long, Point *, Long);

   Extern Logical
     /* Determine if a particular point is inside a polygon (set) */
      POLin( Polygonset *, Point *);

   Extern Point
    /* Add selected corners of the rectangle to a list of points */
      * POLaddcnrs( Enum, Enum, Logical, Rect *, Point *);



#else
   Extern void POLget(), POLedge(), POLend();
   Extern Polygonset * POLhandle();
   Extern Int  POLnumber();
   Extern Long POLselfx();
   Extern Logical POLin();
   Extern Point *POLaddcnrs();
#endif


#ifdef PROTO
                          /*  Polygon Sectionlist Utilities */

   Extern void
   /* Derive bounding box of the polygon that the scursor is on */
        POLbbox_sl ( Sectlist *, Long, Scursor, Rect * ),

   /* Make a section list contiguous */
        POLcontig_sl ( Sectlist *),

   /* Count things about a polygon in the section list */
        POLcount_sl ( Sectlist *, Int, Int *, Long *),

   /* Free the space allocated to a section list */
        POLfree_sl ( Sectlist *),

   /* Find poly that is inside another, neither crossing */
        POLinfind_sl ( Sectlist *, Int *, Int * ),

   /* Join two polygons one inside the other */
        POLinjoin_sl ( Sectlist *, Long, Int, Long, Int ),

   /* Join two polygons in the section list at specified point */
        POLjoin_sl ( Sectlist *, Scursor *, Scursor *, Point *),

   /* Move scursor to the next section */
        POLnext_sl ( Sectlist *, Scursor *),

   /* Put a scursor on a polygon */
        POLscursor_sl ( Sectlist *, Int, Scursor *);

   Extern Polygonset
   /* Return the polygonset corresponding to a given section list */
      * POLpts_sl( Sectlist *, Polygonset *);


   Extern Sectlist
   /* Return a section list for a clipped polygon set */
      * POLclip_sl( Polygonset *, Rect *),

   /* Create a section list or make sure that is suffiently large */
      * POLgrow_sl( Sectlist *, Int, Int, Int ),

   /* Return a section list to join overlapping polygons */
      * POLnoverlap_sl( Sectlist *, Logical ),

   /* Return a section list for a polygon set */
      * POLpoly_sl( Polygonset *, Int );

   Extern Logical
   /* Determine if one polygon is inside other, neither crossing */
      POLinpoly_sl ( Sectlist *, Int, Int );


#else
   Extern void POLbbox_sl(), POLcontig_sl(), POLcount_sl(),
               POLfree_sl(), POLinfind_sl(), POLinjoin_sl(),
               POLjoin_sl(), POLnext_sl(), POLscursor_sl();
   Extern Polygonset *POLpts_sl();
   Extern Sectlist *POLclip_sl(), *POLgrow_sl(), *POLnoverlap_sl(),
                   *POLpoly_sl();
   Extern Logical POLinpoly_sl();
#endif

#undef Extern
#endif /* CGMPOLY_H */
