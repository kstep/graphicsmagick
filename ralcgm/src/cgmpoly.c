/*   RAL-CGM Interpreter module:  @(#) cgmpoly.c  version 3.7
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
 * Description: Utilities handling Polygon Clipping and Polygon Set
 *
 *
 * Maintenance Log:
 *   2 Feb 93 KEVP Original version working on simple tests
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *                 Change all ocurrences of =+ to +=
 *  23 Jan 95 KEVP Add utility to get data into polygon set structure
 *  24 Feb 95 KEVP In POLclip_sl, move freeing of crossings structure outside loop
 *                 In xing_edges, correct the double assignment of the polygon
 *                 to crossing (poly++).
 *
 * Note on Terminology
 *
 *   The singular of edge will be taken to mean a single straight
 *   line segment between two consecutive vertices.
 *
 */

#define CGMPOLY_C

#include "cgmout.h"
#include "cgmelem.h"
#include "cgmpar.h"
#include "cgmgraph.h"
#include "cgmpts.h"
#include "cgmpoly.h"

/* Macros for common Maths functions  */

/* Clipcode Macro */
#define CLIPCODE(x,xmin,xmax) (((x>xmax)<<1)|(x<xmin))
/* Values of Clipcode */
#define CLIPNO    (Enum) 0
#define CLIPMIN   (Enum) 1
#define CLIPMAX   (Enum) 2
/* Values of Clipstatus */
#define CLIPTOP   (Enum) 2
#define CLIPLEFT  (Enum) 3
#define CLIPBOT   (Enum) 4
#define CLIPRIGHT (Enum) 6
#define CLIPUL    (Enum) 5
#define CLIPLL    (Enum) 7
#define CLIPUR    (Enum) 8
#define CLIPLR    (Enum) 10
/* Clipstatus functions
   NB: Clipstatus = CLIPX(x Clipcode) + CLIPY(y Clipcode) */
#define CLIPX(i)  (Enum)(i+i+i)
#define CLIPY(i)  (Enum)(i*(7-i-i-i))
/* Clip function */
#define ECLIP(y1,y2,x1,x2,xm) y1+(xm-x1)*(y2-y1)/(x2-x1)

/* Clockwise function for 3 points */
#define CLOCK(a,b,c) ( ((b)->x - (a)->x)*((c)->y - (a)->y)\
                     < ((c)->x - (a)->x)*((b)->y - (a)->y) )

/* Line passes clip rectangle corner clockwise */
#define RCLOCK(a,b,cl,xs,ys) ( ((b)->x - (a)->x)*((cl)->ys - (a)->y)\
                             < ((cl)->xs - (a)->x)*((b)->y - (a)->y) )

/* Ordering from crossings */
#define CROSSORD(i1,f1,i2,f2)   ((i1<i2)||\
 ((i1==i2) && (((i1>3)&&(f1<f2))||((i1<4)&&(f1>f2)))))

/* Section type */
#define NOSECTION     0
#define CLIPINDUCED   1
#define FORWINRECT    2
#define BACKWINRECT   3

/* String for Error Messages */
static char *func = "CGMpoly";

#ifdef PROTO
static Logical goes_over( Point *end1, Point *end2, Point *test );
#else
static Logical goes_over();
#endif
#ifdef PROTO
/* Routines handling crossings of
   polygon(set) boundary over clipping rectangle boundary */
static void
    /* Add a clip a induced vertex */
   xing_addclin( Enum *, Float *, Rect *, Int, Point *),

    /* Count things useful for clipping a polygon set */
   xing_count( Polygonset *, Rect *, Int *, Int *),

    /* Get edges that cross clipping rectangle */
   xing_edge( Polygonset *, Rect *, Index *, Index *,
                 Int *, Enum *),

    /* Free the space allocated to crossings data */
   xing_free( Clipcross *),

    /* Sort crossings of polygon set boundary over clipping boundary */
   xing_sort( Int, Enum *, Float *, Int *);

static Xcursor
    /* Move Xcursor to the next pair of crossing */
   * xing_next ( Index *, Index *, Int *, Int *, Xcursor *),

    /* Get the first crossings of a polygon on the clipping boundary */
   * xing_start( Int, Index *, Index *, Int *, Xcursor *);

#else
static void
   xing_addclin(), xing_count(), xing_edge(), xing_free(), xing_sort();

static Xcursor
   *xing_next(), *xing_start();
#endif

/************************************************** POLget ***********/

#ifdef PROTO
void POLget( Long np, Long *pi, Float *pr, Logical set,
                    Logical cont, Polygonset *pset )
#else
void POLget( np, pi, pr, set, cont, pset)
 /*
    Get data for a polygon set structure from arguments.
 */

Long np;      /* Number of Points (got from *(pi-1)) */
Long *pi;     /* Integer Data */
Float *pr;    /* Real Data */
Logical set;  /* True, if polygon set */
Logical cont; /* True, if this set of points is a continuation */
Polygonset *pset;   /* Ponter to a polygon set structure */
#endif

{
   static Long start;               /* Starting place */
   static char *func = "POLget";    /* Function Name */

   /* Allocate space for polygon set stucture */
   if(!cont)
   {  /* First set of points, allocate space */
      start = 0;
      pset->n = abs(np);
      pset->pts = (Point *)MALLOC( pset->n, sizeof( Point));
      if(pset->pts == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      if(set)
      {  /* Edge-out flags needed */
         pset->eofl = (Enum *)MALLOC( pset->n, sizeof( Enum));
         if(pset->eofl == NULL)
            exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      }
      else pset->eofl = NULL;
   }
   else
   {  /* Additional set of points, reallocate */
      start = pset->n;
      pset->n += abs(np);
      pset->pts = (Point *)realloc( (void *)pset->pts,
                              (size_t)(pset->n*sizeof(Point)));
      if(pset->pts == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      if(set)
      {
         pset->eofl = (Enum *)realloc( (void *)pset->pts,
                                 (size_t)(pset->n*sizeof(Enum)));
         if(pset->eofl == NULL)
            exit ((int)CGMerror( func, ERR_NOMEMORY, FATAL, NULLSTR));
      }
   }

   /* Put data into polygon set stucture (get as Points) */
   PTSget (pset->n, pi, pr, pset->pts+start, pset->eofl+start);

   return;
}
/************************************************** POLhandle ********/

#ifdef PROTO

Polygonset * POLhandle( Polygonset *pset,
                        Rect *clip, Logical overlap,
                        Polygonset *space)
#else

Polygonset * POLhandle( pset, clip, overlap, space )

/*
Clip the given polygon or polygon set and/or adjust the polygonset
to prevent any two polygons from overlapping.
Edge visibility is ignored and edges may be all invisible in output.
*/

Polygonset *pset; /* The Input polygon Set */
Rect *clip;       /* Clipping rectangle or NULL if not clipping */
Logical overlap;  /* True, if output polygons may overlap */
Polygonset *space; /* Space for output polygonset, used if big enough */

/* WARNING: (overlap == FALSE) is UNTESTED */
#endif

/*
NOTE (1) If the device can not handle overlapping polygons,
         'overlap' can be set to (fill area style == HOLLOW).
         This saves on computing time, if hollow.

     (2) The pointer 'space' may be NULL, in which case space will
         be allocated for the output polygon set. Otherwise,
         it is assumed that the arrays space->pts and space->eofl have
         enough space for space->n elements.
         If more are needed a new allocation will be made for them.

*/
{
   Logical out = FALSE,     /* True, if vertex is outside of clip rect */
           clipind = FALSE; /* True, if clipping */
   Sectlist *ps = NULL;     /* Polygon Set in section list form */
   Point *vtx;              /* Current vertex */
   Index i;                 /* Loop index, not used inside loop */
   Polygonset *clipped;     /* Clipped polygonset */

#ifdef DEBUG
     DMESS "POLhandle:\n");
     DMESS "POLhandle: %d vertices\n",pset->n);
#endif
   if(clip != NULL)
   {   /*   Clipping requested */

#ifdef DEBUG
       DMESS "POLhandle: Rectangle l %f, r %f,  b %f, t %f\n",
              clip->left,clip->right,clip->bot,clip->top);
#endif
       /* Check if any vertex is outside clipping rectangle */

       for(i=0,vtx=pset->pts;(i<pset->n)&&(!out);i++,vtx++)
       {
#ifdef DEBUG_PTS
          DMESS "POLhandle: %d (%f,%f)\n",i,vtx->x,vtx->y);
#endif
          if((CLIPCODE(vtx->x,clip->left,clip->right) != 0) ||
             (CLIPCODE(vtx->y,clip->bot,clip->top) != 0) )
          {
              out = TRUE;
#ifdef DEBUG_PTS
          DMESS " point %d out %d\n",i,out);
#endif
              break;
          }
       }

       clipind = out;   /* Clip if vertex is outside */
#ifdef DEBUG
       DMESS " point outside therefore clipping %d\n",clipind);
#endif
   }

   if(clipind)
   {
#ifdef DEBUG
    DMESS "POLhandle: Clipping  l %f, r %f,   b %f, t %f\n",
          clip->left,clip->right,clip->bot,clip->top);
#endif
      /* Get clipped polygon set, in polygon section list form */
      ps = POLclip_sl ( pset, clip );

#ifdef DEBUG
      if(ps == NULL)
      DMESS "POLClip: No Polygon Section list from POLclip_sl\n");
#endif
      if(ps == NULL) return NULL;
   }

#ifdef DEBUG
     if(ps != NULL)
     {
       DMESS "POLhandle: %d polygons.\n",ps->npoly);
     }
#endif

   if(!overlap && (pset->eofl != NULL))
   {   /* Polygon set must not have overlapping polygons */
       /* WARNING this option is UNTESTED! */

       if(ps == NULL)
       {
          /* Get polygon set in section list form */
          ps = POLpoly_sl( pset, (pset->n)/3 );
          if(ps == NULL) return NULL;
       }

       /* Process section list to merge overlapping polygons */
       ps = POLnoverlap_sl( ps, (pset->eofl != NULL) );
       if(ps == NULL) return NULL;
   }


   if(ps == NULL)
   {   /* No section list */

       /* Return the input polygon set */

      clipped = pset;
   }
   else
   {  /* We have a section list */

      /* Derive output polygon set from section list */
      clipped = POLpts_sl ( ps,  space );

      /* Free The Polygon Section List */
      POLfree_sl( ps );

   }


   return clipped;
}
/***************************************************** POLedge ********/

#ifdef PROTO
void POLedge (Polygonset *pset, Rect *clip, void(*linefunc)(Long, Point*) )
#else
void POLedge ( pset, clip, linefunc )

/*  Draws and clips (if required) the edges of a Polygon Set  */

Polygonset *pset;    /* Polygon Set */
Rect *clip;          /* Clipping Rectangle or NULL if not clipping */
void (*linefunc)();  /* Polyline drawing function */
#endif

{
   Int i;             /* Loop index, not used within loop */
   Long nvis;         /* Number of vertices in a visible edge section */
   Point close[2];    /* Closing edge of polygon */
   Point *end=close,      /* End vertex of polygon */
         *start=close+1,  /* Start vertex of polygon */
         *pt=pset->pts;   /* Current vertex */
   Enum  *fl=pset->eofl;  /* Current edge-out flag */

#ifdef DEBUG
   DMESS "POLedge: %d vertices, flags %d \n",pset->n,pset->eofl);
#endif
   if( pset->eofl == NULL)
   {   /* No edge flags (not polygon set) */
         *end = *(pt+pset->n-1);      /* Define closing edge */
         *start = *pt;
         if(clip == NULL)
         { /* Not clipping, draw as polyline and close */
            linefunc ( pset->n, pt);
            linefunc ( 2L, close);
#ifdef DEBUG
            DMESS "POLedge: draw the polygon without clipping.\n");
#endif
         }
         else
         {  /* Clipping, draw and clip as polyline and close clipped */
            PTSlineclip( pset->n, pt, *clip, linefunc );
            PTSlineclip( 2L, close, *clip, linefunc);
#ifdef DEBUG
            DMESS "POLedge: draw the polygon with clipping.\n");
            DMESS "POLedge: rect l %f, r %f, b %f, t %f\n",
                   clip->left,clip->right,clip->bot,clip->top);
#endif
         }
   }
   else
   {   /* Polygon set */
      *start = *pt;                   /* Define start of polygon */
      for(i=0,nvis=1; i<pset->n-1; i++,pt++,fl++)
      {    /* Loop over vertices and edges */
         switch(*fl)
         {
            case VIS:
               nvis++;        /* Add vertex to visible section */
               break;
            case INVIS:
               if(nvis > 1)
               {
#ifdef DEBUG
   DMESS "POLedge: Drawing %d edges terminated by INVIS edge\n",
                 nvis-1);
#endif
                 if(clip == NULL)   /* Draw (& clip) visible section */
                    linefunc( nvis, pt-nvis+1);
                 else
                    PTSlineclip( nvis, pt-nvis+1, *clip, linefunc );
                 nvis = 1;
               }
               break;
            case CLOSEVIS:
               *end = *pt;  /* Define end of closure */
               if(nvis > 1)
               {
#ifdef DEBUG
   DMESS "POLedge: Drawing %d edges terminated by CLOSEVIS edge\n",
                 nvis-1);
#endif
                 if(clip == NULL)  /* Draw (& clip) visible section) */
                    linefunc( nvis, pt-nvis+1);
                 else
                    PTSlineclip( nvis, pt-nvis+1, *clip, linefunc );
                 nvis = 1;
               }
#ifdef DEBUG
               DMESS "POLedge: Drawing closing edge\n");
               DMESS "         (%f,%f) to (%f,%f)\n",
               close->x,close->y,(close+1)->x,(close+1)->y);
#endif
               if(clip == NULL)        /* Close Polygon */
                  linefunc ( 2L, close);
               else
                  PTSlineclip( 2L, close, *clip, linefunc );
               *start = *(pt+1);       /* Define start of next polygon */
               break;
            case CLOSEINVIS:
               if(nvis > 1)
               {
#ifdef DEBUG
   DMESS "POLedge: Drawing %d edges terminated by CLOSEINVIS edge\n",
                 nvis-1);
#endif
                 if(clip == NULL)       /* Draw visible section */
                    linefunc( nvis, pt-nvis+1);
                 else
                    PTSlineclip( nvis, pt-nvis+1, *clip, linefunc );
                 nvis = 1;
               }
               *start = *(pt+1);      /* Define start of next polygon */
               break;
         }  /* End of switch */
      }   /* End of loop over edges */

      switch (*fl)    /* Final Edge */
      {
         case VIS: case CLOSEVIS:
            if(nvis > 1)
            {
#ifdef DEBUG
   DMESS "POLedge: Drawing %d edges terminated by final VIS edge\n",
               nvis-1);
#endif
               if(clip == NULL)   /* Draw final visible section */
                  linefunc( nvis, pt-nvis+1 );
               else
                  PTSlineclip( nvis, pt-nvis+1, *clip, linefunc);
            }
            *end = *pt;                         /* define end */
#ifdef DEBUG
            DMESS "POLedge: Drawing final edge\n");
            DMESS "         (%f,%f) to (%f,%f)\n",
            close->x,close->y,(close+1)->x,(close+1)->y);
#endif
            if(clip == NULL)      /* Close Polygon */
               linefunc ( 2L, close);
            else
               PTSlineclip( 2L, close, *clip, linefunc);
            break;
         case INVIS: case CLOSEINVIS:
            if(nvis > 1)
            {
#ifdef DEBUG
   DMESS "POLedge: Drawing %d edges terminated by final INVIS edge\n",
               nvis-1);
#endif
               if(clip == NULL)
                  linefunc( nvis, pt-nvis+1 );
               else
                  PTSlineclip( nvis, pt-nvis+1, *clip, linefunc);
            }
            break;
       }            /* End of switch */
   }                /* Endif Polygon Set */
   return;
}
/***************************************************** POLend *********/

#ifdef PROTO
void POLend ( Long np, Enum *fl, Index xmp, Index *start, Index *end)
#else
void POLend ( np, fl, xmp, start, end )

/*  Outputs the indices of the start and end of a polygon in a
    polygon set */

Long np;    /* Number of vertices in polygon set */
Enum *fl;   /* Edge-out flags of polygon set (or NULL if one polygon) */
Index xmp;  /* Index of Example vertex */
Index *start;   /* Start Vertex of Polygon with Example Vertex */
Index *end;     /* End Vertex of Polygon with Example Vertex */

#endif

/* NB: if either the start or end index pointers is set to NULL,
       the relevent data will not be sought */

{
   Enum *flag; /* Current edge-out flag in loop */

   if(fl == NULL)
   { /* Single Polygon */
       if(start != NULL) *start = 0;
       if(end   != NULL) *end = np-1;
#ifdef DEBUG
       DMESS "POLend: single polygon 0 to %d\n",np-1);
#endif
   }
   else
   { /* Polygon Set with Flags */

     /* Find end of polygon */
     if(end != NULL)
     {
       for(*end = xmp, flag = fl + xmp;
           (*end < np-1) && (*flag != CLOSEVIS) && (*flag != CLOSEINVIS);
           (*end)++,flag++); /* Find the closing edge */
       {
       }
#ifdef DEBUG
       DMESS "POLend: polygon end vertex index %d\n",*end);
#endif
     }

     if(start != NULL)
     {
        /* Find Start of Polygon */
        for(*start = xmp, flag = fl + xmp - 1;
           (*start > 0) && (*flag != CLOSEVIS) && (*flag != CLOSEINVIS);
           (*start)--,flag--);  /* Find the 1st vertex of polygon */
       {
       }
#ifdef DEBUG
       DMESS "POLend: polygon start vertex index %d\n",*start);
#endif
     }
   }
   return;
}
/************************************************** POLin **********/
#ifdef PROTO
Logical POLin ( Polygonset *pset, Point *test )
#else
Logical POLin ( pset, test )

/*
Returns whether the given test point is inside the given polygon set.
*/

Polygonset *pset; /* Polygon Set */
Point *test;      /* The point being tested */
#endif

/* ALGORITHM
  For each edge of the polygon set, works out whether it goes over the
  test point.
  If so it reverses the truth of prospective return value, each time.
*/
{
   Logical oddcross = FALSE; /* Number of edges crossed is odd */
   Index i;                  /* Loop Index */
   Point *end1, *end2;       /* Ends of current edge */
   Enum *fl;                 /* Current flag */
   Point *first;             /* First vertex of polygon */

#ifdef DEBUG
   DMESS "POLin: test pt (%f,%f)\n",test->x,test->y);
   DMESS "POLin: %d vertices\n",pset->n);
#endif

   for(i=0,first=end1=pset->pts,fl=pset->eofl; i<pset->n;
       i++,end1++,fl++)
   {
      if(pset->eofl != NULL)
      {
         if((*fl == VIS) || (*fl == INVIS)) end2 = end1+1;
         else { end2 = first; first = end1+1;}
      }
      else
      {  /* One Polygon */
         if(i < (pset->n)-1) end2 = end1+1;
         else                end2 = first;
      }

      if(goes_over(end1,end2,test)) oddcross = !oddcross;
#ifdef DEBUG_PTS
      DMESS "POLin: (%f,%f) to (%f,%f), oddcross %d\n",
      end1->x,end1->y,end2->x,end2->y,oddcross);
#endif
   }
   return oddcross;
}
/************************************************** POLnumber *******/

#ifdef PROTO
Int POLnumber ( Long np, Enum *fl )
#else
Int POLnumber ( np, fl )

/* Returns the number of polygons in a polygon set (or 0 if invalid) */

Long np;   /* number of Vertices */
Enum *fl;  /* polygon set edge out flags */
#endif

/* ALGORITHM

  Checks that all edge out flags have valid values.
  Makes sure that the last edge out flag is a closure flag.
  Counts the number of closure flags.
  Checks that every closure flag is preceeded by a non-closure flag.
*/
{
   Int npoly = 0;                /* Number of polygons */
   Logical newpoly = TRUE;       /* First edge of polygon */
   Enum *last;                   /* Last edge out flag */
   Index i;                      /* Loop index */

   if(fl != NULL)
   {  /* PolygonSet */

      /* Make sure last edge out last flag is closed */
      last = fl+np-1;
      if((*last == VIS) || (*last == INVIS)) *last += EDGECLOSE;

      for(i=0;i<np;i++,fl++)
      {
         switch(*fl)
         {
           case VIS: case INVIS:
              newpoly = FALSE;    /* Next edge in same polygon */
              break;
           case CLOSEVIS: case CLOSEINVIS:
              if(newpoly) return 0;    /* Invalid closure flag */
              newpoly = TRUE;      /* Next edge in new polygon */
              npoly++;              /* Increment polygon count */
              break;
           default:
#ifdef DEBUG
              DMESS "Edge out flag  %d invalid with value %d",
                    i,(Int)*fl);
#endif
              return 0;               /* Invalid flag value */
         }                   /* End of switch(*fl) */
      }                      /* End of for() */
   }
   else
   {    /* fl == NULL:  One Polygon */
      return 1;
   }
   /* We have a valid polygon set */
   return npoly;
}
/****************************************************** POLselfx *******/

#ifdef PROTO
Long POLselfx ( Long np, Point *pt, Long limit )
#else
Long POLselfx ( np, pt, limit)
 /*
    Count the self intersections of a polygon
 */

Long np;      /* Number of Vertices */
Point *pt;    /* The vertices */
Long limit;   /* Limit of count (if positive) */
#endif

{
   Long count=0L;       /* The count of self intersections */
   Index i, j;          /* Loop indices */
   Point *end1, *end2;  /* Ends of current edge */
   Point *eo;           /* First end of other edge */
   Rect bbox;           /* Bounding box of current edge */
   Point sx;            /* Self intersection point */
   Logical colline;     /* True if edges are collinear */

   if(np < 4) return 0L;

   end1 = pt+np-1;    /* Start at closing edge */
   end2 = pt;

   for(i=0; i<np-1; i++)
   {
       /* Get bounding box of current edge */
       if(end1->x < end2->x)
       {
          bbox.left = end1->x; bbox.right = end2->x;
       }
       else
       {
          bbox.left = end2->x; bbox.right = end1->x;
       }
       if(end1->y < end2->y)
       {
          bbox.bot = end1->y; bbox.top = end2->y;
       }
       else
       {
          bbox.bot = end2->y; bbox.top = end1->y;
       }

       for(j=i+1, eo=end2; j<np; j++, eo++)
       {  /* determine whether the two edges intersect */
          if( (bbox.left <= eo->x) && (eo->x  <= bbox.right) &&
              (bbox.left <= (eo+1)->x) && ((eo+1)->x  <= bbox.right) )
          {
             if( (bbox.bot <= eo->y) && (eo->y  <= bbox.top) &&
                 (bbox.bot <= (eo+1)->y) && ((eo+1)->y  <= bbox.top) )
             {   /* Bounding boxes do intersect */
                if(GRAcross(end1,end2,eo,eo+1,&colline,&sx))
                {
                   if(!colline)
                   { /* Count crossing, if not collinear */
                      count++;
                      if(count == limit) return count;
                   }
                }
             }  /* End if bounding boxes intersect */
          }     /* End for(j... */
          end1 = end2;
          end2++;
       }        /* End for(i.. */
   }

   return count;
}
/************************************************** goes_over *********/
#ifdef PROTO
static Logical goes_over ( Point *end1, Point *end2, Point *test )
#else
static Logical goes_over ( end1, end2, test )

/*
Returns whether the given edge goes over (y-wise) a given point.
*/

Point *end1; /* 1st end of edge */
Point *end2; /* 2nd end of edge */
Point *test; /* The point being tested */
#endif

/* NOTE: The function is called by both POLin and POLinpoly_sl */

{
   Logical over = FALSE;     /* Edge goes over */

   if((end1->x < test->x) != (end2->x < test->x))
   {  /* Edge goes from one side of testpoint to other */
#ifdef DEBUG_PTS
      DMESS "goes_over: end1 (%f,%f), end2 (%f,%f), test (%f,%f)\n",
             end1->x,end1->y,end2->x,end2->y,test->x,test->y);
#endif
      if(( end1->y > test->y) && (end2->y > test->y))
      {   /* Both ends above test point */
         over = TRUE;
#ifdef DEBUG_PTS
         DMESS "both ends over\n");
#endif
      }
      else
      {
         if((end1->y > test->y) || (end2->y > test->y))
         {  /* One end above test point, other below */
            if(end1->x < end2->x)
            { /* Rightward Edge - (Over if passes test pt clockwise) */
                over = CLOCK(end1,end2,test);
#ifdef DEBUG_PTS
                DMESS "Rightward Edge, over %d\n",over);
#endif
            }
            else
            { /* Leftward Edge - (Over if passes t.pt anti-clockwise */
                over = CLOCK(end1,test,end2);
#ifdef DEBUG_PTS
                DMESS "Leftward Edge, over %d\n",over);
#endif
            }
         }                      /* end of if(one above) */
      }                         /* end of if(both above)else */
   }                            /* end of if(one side to other) */
   return over;
}
/************************************************** xing_addclin ********/
#ifdef PROTO
static void xing_addclin( Enum *cls, Float *xy, Rect *cl, Int index,
                          Point *freept)
#else
static void xing_addclin( cls, xy, cl, index, freept )

/* Add a clip induced vertex */

Enum *cls;     /* clip statii */
Float *xy;     /* significant x or y coordinates */
Rect *cl;      /* Clipping rectangle */
Int index;     /* Array index of crossing */
Point *freept; /* Clip induced vertex */
#endif
{

   switch( *(cls + index))
   {
      case CLIPTOP:
         freept->x = *(xy + index);
         freept->y = cl->top;
         break;
      case CLIPLEFT:
         freept->x = cl->left;
         freept->y = *(xy + index);
         break;
      case CLIPBOT:
         freept->x = *(xy + index);
         freept->y = cl->bot;
         break;
      case CLIPRIGHT:
         freept->x = cl->right;
         freept->y = *(xy + index);
         break;
      default:
         freept->x = freept->y = 0.0;  /* Invalid value from cls */
         break;
     }
     return;
  }

/************************************************** POLaddcnrs ******/
#ifdef PROTO
Point * POLaddcnrs ( Enum side1, Enum side2, Logical clockwise,
                            Rect *clip, Point *freept )
#else
Point * POLaddcnrs ( side1, side2, clockwise, clip, freept)

/* Adds corners of the cliping  rectangle 'clip' to a list of points
   starting at 'freept' and returns the next vacant point.

   The corners added run from 'side1' to 'side2' of the rectangle.
   If 'side1' is equal to 'side2' all four corners are added in the
   appropriate order.
*/

Enum side1;          /* Side of first end */
Enum side2;          /* Side of other end */
Logical clockwise;   /* True, if corners to be added clockwise */
Rect *clip;          /* Clipping rectangle */
Point *freept;       /* Place to start putting corners */
#endif
{
   Point *start;     /* Start of points added */

   start = freept;
   if(clockwise)
   {                        /* Add in Clockwise Order */
      switch(side1)
      {                     /* starting and ending appropriately */
         case CLIPRIGHT:
           freept->x = clip->right;      /* Add Lower-Right Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPBOT)break;
         case CLIPBOT:
           freept->x = clip->left;       /* Add Lower-Left Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPLEFT)break;
         case CLIPLEFT:
           freept->x = clip->left;       /* Add Upper-Left Corner */
           freept->y = clip->top;
           freept++;
           if(side2 == CLIPTOP)break;
         case CLIPTOP:
           freept->x = clip->right;      /* Add Upper-Right Corner */
           freept->y = clip->top;
           freept++;
           if(side2 == CLIPRIGHT)break;

           freept->x = clip->right;      /* Add Lower-Right Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPBOT)break;

           freept->x = clip->left;       /* Add Lower-Left Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPLEFT)break;

           freept->x = clip->left;       /* Add Upper-Left Corner */
           freept->y = clip->top;
           freept++;
           if(side2 == CLIPTOP)break;
        default:
           return NULL;  /* Invalid value for side1 or side2 */
      }
   }
   else
   {                         /* Add in Anticlockwise Order */
      switch(side1)
      {                      /* starting and ending approprately */
         case CLIPLEFT:
           freept->x = clip->left;      /* Add Lower-Left Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPBOT)break;
         case CLIPBOT:
           freept->x = clip->right;      /* Add Lower-Right Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPRIGHT)break;
         case CLIPRIGHT:
           freept->x = clip->right;      /* Add Upper-Right Corner */
           freept->y = clip->top;
           freept++;
           if(side2 == CLIPTOP)break;
         case CLIPTOP:
           freept->x = clip->left;       /* Add Upper-Left Corner */
           freept->y = clip->top;
           freept++;
           if(side2 == CLIPLEFT)break;

           freept->x = clip->left;       /* Add Lower-Left Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPBOT)break;

           freept->x = clip->right;      /* Add Lower-Right Corner */
           freept->y = clip->bot;
           freept++;
           if(side2 == CLIPRIGHT)break;

           freept->x = clip->right;      /* Add Upper-Right Corner */
           freept->y = clip->top;
           freept++;
           if(side2 == CLIPTOP)break;
        default:
           return NULL;    /* Invalid value for 'side1' or 'side2' */
      }
   }
   return freept;
}
/************************************************** xing_count *******/

#ifdef PROTO
static void xing_count ( Polygonset *pset, Rect *cl,
                         Int *ncross, Int *nsect)
#else
static void xing_count (pset, cl, ncross, nsect)

/* Determine the number of sections needed to clip a polygonset
   and the number of times it crosses the clipping rectangle boundary */

Polygonset *pset;  /* Polygon Set */
Rect *cl;          /* clipping rectangle */
Int *ncross;    /* Number of crossings polygon and clipping rectangle */
Int *nsect;       /* Number of sections */
#endif
{
   Index i;               /* Current edge index */
   Point *end1, *end2;    /* Ends of current edge */
   Enum *fl;              /* Current edge flag */
   Enum cl1x,cl1y,        /* Clipcodes of 1st end */
        cl2x,cl2y;                 /* and 2nd end */
   Point *first;           /* First vertex in current polygon */
   Logical endsection;     /* True, if end of section */

   /* Initialise variables for loop over edges of the polygon */
   *ncross = 0; *nsect = 0;
   endsection = FALSE;
   first = pset->pts;
   /* loop over edges of the polygons */
   for(i=0,end1=pset->pts,fl=pset->eofl; i<pset->n; i++,end1++,fl++)
   {
      cl1x = CLIPCODE(end1->x,cl->left,cl->right); /* get clipcodes */
      cl1y = CLIPCODE(end1->y,cl->bot,cl->top);     /* for 1st end */

#ifdef DEBUG_PTS
      DMESS "xing_count: Point %d, stat (%d,%d), %d crossings, %d sections.\n"
            ,i,cl1x,cl1y,*ncross,*nsect);
#endif
      /* Find other end */
      if(pset->eofl != NULL)
      {
         if((*fl == VIS) || (*fl == INVIS)) end2 = end1+1;
         else
         { end2 = first; endsection = TRUE; first = end1+1;}
      }
      else
      {  /* One Polygon */
         if(i < pset->n-1) end2 = end1+1;
         else
         {  end2 = pset->pts; endsection = TRUE;} /* Close the polygon */
      }

      cl2x = CLIPCODE(end2->x,cl->left,cl->right); /* get clipcodes */
      cl2y = CLIPCODE(end2->y,cl->bot,cl->top);     /* for 2nd end */

      /* Determine edge status and count as required */
      if((cl1x == 0) && (cl1y == 0))
      {  /* 1nd End Inside */
         if((cl2x != 0) || (cl2y != 0))
         {  /* Outgoing Edge */
#ifdef DEBUG_CROSS
            DMESS "xing_count: Outward Crossing %d (%d to %d)\n",
            *ncross,end1-pset->pts,end2-pset->pts);
#endif
            (*ncross)++;          /* Crossing Found */
            endsection = TRUE;  /* New section */
         }
      }
      else
      {  /* 1st End Outside */
         if((cl2x == 0) && (cl2y == 0))
         {  /* Incoming edge */
#ifdef DEBUG_CROSS
            DMESS "xing_count: Inward Crossing %d (%d to %d)\n",
            *ncross,end1-pset->pts,end2-pset->pts);
#endif
             (*ncross)++;         /* Crossing found */
             endsection = TRUE; /* New section */
         }
         else
         {  /* Both ends outside */
            /* Test for edge crossing rectangle, simple tests first */
            if(((cl1x != cl2x) || (cl1x == 0)) &&
               ((cl1y != cl2y) || (cl1y == 0)) &&
               ((RCLOCK(end1,end2,cl,left,top)  ||
                 RCLOCK(end1,end2,cl,right,top) ||
                 RCLOCK(end1,end2,cl,left,bot)  ||
                 RCLOCK(end1,end2,cl,right,bot) ) &&
                (!RCLOCK(end1,end2,cl,left,top)  ||
                 !RCLOCK(end1,end2,cl,right,top) ||
                 !RCLOCK(end1,end2,cl,left,bot)  ||
                 !RCLOCK(end1,end2,cl,right,bot) )   ))
            {  /* Edge crosses rectangle */
#ifdef DEBUG_CROSS
        DMESS "xing_count: Inward & Outward Crossings %d, %d (%d to %d)\n",
            *ncross,*(ncross)+1,end1-pset->pts,end2-pset->pts);
#endif
               (*ncross) += 2;
               (*nsect)++;          /* New section for edge */
               endsection = TRUE; /* Another new section */
            }
         }             /* End of if(cl2x..) */
      }                /* End of if(cl1x..) */

#ifdef DEBUG_PTS
      DMESS "xing_count: end of Section %d, %d sections\n",
      endsection, *nsect);
#endif

      if(endsection)
      {  /* End of section: count it */
          (*nsect)++;
          endsection = FALSE;
      }

   }  /* End of for() */

   return;
}
/************************************************** xing_edge  *******/
#ifdef PROTO
static void xing_edge ( Polygonset *pset, Rect *cl,
                        Index *in, Index *out, Int *poly, Enum *cls )
#else
static void xing_edge ( pset, cl, in, out, poly, cls )

/*
Get the ends of the edges, that cross the clipping rectangle boundary
and the clip status of the outer end of each such edge.
Also indentify the polygon of each crossing.
*/
Polygonset *pset; /* Polygonset */
Rect *cl;         /* The clipping rectangle */
Index *in;        /* Inner ends of the crossing edges */
Index *out;       /* Outer ends of the crossing edges */
Int   *poly;      /* The polygon of the crossing */
Enum  *cls;       /* The clip stati of the crossings */
#endif
{
   Index index;               /* Current edge index */
   Point *end1, *end2;        /* Ends of current edge */
   Enum *fl;                  /* Current edge flag */
   Enum cl1x,cl1y,            /* Clipcodes of end1 */
        cl2x,cl2y;                     /* and end2 */
   Point *first;              /* First vertex in current polygon */
   Int curpoly = 0;           /* Current polygon */
   register Logical polyset;  /* True if Polygon Set */

   first = pset->pts;
   polyset = (pset->eofl != NULL);
   for(index=0,end1=pset->pts,fl=pset->eofl; index<pset->n;
       index++, end1++, fl++)
   {
      /* Get clip status for 1st End */
      cl1x = CLIPCODE(end1->x,cl->left,cl->right);
      cl1y = CLIPCODE(end1->y,cl->bot,cl->top);

      /* Find 2nd end */
      if(polyset)
      {
         if((*fl == VIS) || (*fl == INVIS)) end2 = end1+1;
         else { end2 = first; first = end1+1;}
      }
      else
      {
         if(index < pset->n-1) end2 = end1+1;
         else                     end2 = pset->pts;
      }

      /* Get clip status for 2nd End */
      cl2x = CLIPCODE(end2->x,cl->left,cl->right);
      cl2y = CLIPCODE(end2->y,cl->bot,cl->top);

      if(( cl1x == 0) && (cl1y == 0))
      {   /* 1st end Inside */
         if((cl2x != 0) || (cl2y != 0))
         { /* Outgoing edge */
           *(in++)  = (Index)(end1-pset->pts);
           *(out++) = (Index)(end2-pset->pts);
           *(cls++) = CLIPX(cl2x) + CLIPY(cl2y);
           if(polyset) *(poly++) = curpoly;
         }
      }
      else
      {  /* 1st end outside */
         if((cl2x == 0) && (cl2y == 0))
         {  /* incoming edge */
           *(out++) = (Index)(end1-pset->pts);
           *(in++)  = (Index)(end2-pset->pts);
           *(cls++) = CLIPX(cl1x) + CLIPY(cl1y);
           if(polyset) *(poly++) = curpoly;
         }
         else
         {  /* Both ends out */
            /* Test for edge crossing rectangle, simple tests first */
            if(((cl1x != cl2x) || (cl1x == 0)) &&
               ((cl1y != cl2y) || (cl1y == 0)) &&
               ((RCLOCK(end1,end2,cl,left,top)  ||
                 RCLOCK(end1,end2,cl,right,top) ||
                 RCLOCK(end1,end2,cl,left,bot)  ||
                 RCLOCK(end1,end2,cl,right,bot) ) &&
                (!RCLOCK(end1,end2,cl,left,top)  ||
                 !RCLOCK(end1,end2,cl,right,top) ||
                 !RCLOCK(end1,end2,cl,left,bot)  ||
                 !RCLOCK(end1,end2,cl,right,bot) )   ))
            {  /* Edge crosses rectangle */
               *(out++) = (Index)(end1-pset->pts);
               *(in++)  = (Index)(end2-pset->pts);
               *(cls++) = CLIPX(cl1x) + CLIPY(cl1y);
               *(in++)  = (Index)(end1-pset->pts);
               *(out++) = (Index)(end2-pset->pts);
               *(cls++) = CLIPX(cl2x) + CLIPY(cl2y);
               if(polyset)
               {
                  *(poly++) = curpoly;
                  *(poly++) = curpoly;
               }
            }
         }             /* End of if(cl2x..) */
      }                /* End of if(cl1x..) */

      if(polyset && (end2 != end1+1)) curpoly++; /* New polygon */

   }  /* End of for() */

   return;
}
/************************************************** xing_free *******/
#ifdef PROTO
static void xing_free (Clipcross *cr)
#else
static void xing_free ( cr )

/* Free the space allocated to crossings data */

Clipcross *cr;

#endif

{
   if(cr->in != NULL) FREE(cr->in);
   if(cr->out != NULL) FREE(cr->out);
   if(cr->cl != NULL) FREE(cr->cl);
   if(cr->xy != NULL) FREE(cr->xy);
   if(cr->ord != NULL) FREE(cr->ord);
   if(cr->pol != NULL) FREE(cr->pol);
   if(cr->inoth != NULL) FREE(cr->inoth);
   if(cr->outoth != NULL) FREE(cr->outoth);
   if(cr->clock != NULL) FREE(cr->clock);
   return;
}
/************************************************** xing_next ******/
#ifdef PROTO
static Xcursor * xing_next( Index *in, Index *out,
                            Int *inoth, Int *outoth, Xcursor *xc )
#else
static Xcursor * xing_next( in, out, inoth, outoth, xc )

/*
Move an Xcursor to the next pair of crossings on a clipped polygon.
*/

Index *in;      /* Inward ends of crossing edges */
Index *out;     /* Outward ends of crossing edges */
Int *inoth;     /* Crossings at other end of inside section */
Int *outoth;    /* Crossings at other end of clip induced section */
Xcursor *xc;    /* The Xcursor to move */
#endif

/* Note the Xcursor supplied is assumed to be validly on the
   crossings data supplied
   Crossings already done are marked by setting 'out+index' to -1. */
{
   Int ediff;  /* Difference in crossing edge-end vertex indices */

   *(out+xc->cross1) = -1;   /* Mark 1st crossing as done */
   xc->cross1 = xc->cross2;  /* Move 1st crossing to 2nd */

   if(*(out+xc->cross1) == -1)
   {
#ifdef DEBUG_CRSEC
       DMESS "xing_next returns NULL - End of clipped polygon \n");
#endif
       return NULL;          /* End of clipped polygon */
   }

   /* Find next crossing for 2nd */
   if(xc->sectype == CLIPINDUCED)
   { /* Entering Clipping Rectangle */
      xc->cross2 = *(inoth+xc->cross1);
#ifdef DEBUG_CRSEC
      DMESS "xing_next: (i) xing %d (in %d out %d)",
      xc->cross1,*(in+xc->cross1),*(out+xc->cross1));
      DMESS " to xing %d (in %d out %d)\n",
      xc->cross2,*(in+xc->cross2),*(out+xc->cross2));
#endif
      if( *(out+xc->cross1) == *(in+xc->cross2))
         xc->sectype = NOSECTION;
      else
      {
         ediff = ( *(in+xc->cross1) - *(out+xc->cross1) );
         if((ediff == 1) || (ediff < -1))
            xc->sectype = FORWINRECT;
         else
            xc->sectype = BACKWINRECT;
      }
   }
   else
   {   /* Leaving Clipping Rectangle */
         xc->cross2 = *(outoth + xc->cross1);
#ifdef DEBUG_CRSEC
         DMESS "xing_next: (o) xing %d (in %d out %d)",
         xc->cross1,*(in+xc->cross1),*(out+xc->cross1));
         DMESS " to xing %d (in %d out %d)\n",
         xc->cross2,*(in+xc->cross2),*(out+xc->cross2));
#endif
         xc->sectype = CLIPINDUCED;
   }
   return xc;
}
/************************************************** xing_start ******/
#ifdef PROTO
static Xcursor * xing_start( Int ncross, Index *in, Index *out,
                             Int *inoth, Xcursor *xc )
#else
static Xcursor * xing_start( ncross, in, out, inoth, xc )

/*
Return a cursor on the crossings data for a new clipped polygon.
*/

Int ncross;     /* Number of crossings */
Index *in;      /* Inward ends of crossing edges */
Index *out;     /* Outward ends of crossing edges */
Int *inoth;     /* Crossings at other end of inside section */
Xcursor *xc;    /* Pointer to Xcursor to return */
#endif
{
   Index ediff;  /* Difference in end indices */
   Int icross;   /* Current crossing */


   /* Find first remaining crossing */
   /* (crossings may have been removed by setting 'out+icross' to -1) */

   for(icross=0; icross<ncross; icross++)
   {
#ifdef DEBUG_CROSS
      DMESS "xing_start: searching - icross %d out %d\n",icross,*(out+icross));
#endif
      if(*(out+icross) > -1 )
      {
         ediff = (*(in+icross) - *(out+icross));
         xc->cross1 = icross;
         xc->cross2 = *(inoth+icross);
#ifdef DEBUG_CRSEC
         DMESS "xing_start: xing %d (in %d out %d)",
         icross,*(in+icross),*(out+icross));
         DMESS " to xing %d (in %d out %d)\n",
         *(inoth+icross),*(in+xc->cross2),*(out+xc->cross2));
#endif
         if(*(out+xc->cross1) == *(in+xc->cross2))
            xc->sectype = NOSECTION;
         else
         {
            if((ediff == 1) || (ediff > -1))
               xc->sectype = FORWINRECT;
            else
               xc->sectype = BACKWINRECT;
         }
         return xc;
      }
   }
#ifdef DEBUG
   DMESS "xing_start returns NULL - No Polygon Found \n");
#endif
   return NULL;
}
/************************************************** xing_sort *******/
#ifdef PROTO
static void xing_sort ( Int ncross, Enum *cls, Float *xy, Int *ord)
#else
static void xing_sort ( ncross, cls, xy, ord )

/*
Determine the order of the crossings around the clip rectangle
boundary, anticlockwise from upper right corner.
*/
Int ncross;    /* Number of crossings */
Enum  *cls;    /* Clip stati of crossings */
Float *xy;     /* Significant Coordinates of crossings */
Int *ord;      /* The indices of the crossings sorted in order */
#endif

/* ALGORITHM

Initially all crossings are defined as unsorted.
Later the earliest 'icross' crossings in the order
will be sorted in the first 'icross' places.

The earliest of the unsorted crossings is found.
If it is different from the first unsorted crossing they are swapped.
The earliest unsorted crossing has become first unsorted crossing
and is counted as sorted.

The above paragraph is repeated till there is only one unsorted
crossing (which is assumed sorted).

*/
{
   Int iplace,          /* Place of first unsorted crossing */
       jplace,          /* Place of another unsorted crossing */
       mplace;          /* Place of earliest unsorted crossing found */
   Int *icr, icross;    /* First unsorted crossing */
   Int *jcr, jcross;    /* Crossing at 'jcross' */
   Int *mcr, mcross;    /* Earliest unsorted crossing */
   Enum  *em, *ej;      /* Clip status for 'mcross' and 'jcross' */
   Float *rm, *rj;      /* Signicant coord for 'mcross' or 'jcross' */

   /* Initialise Ordering (as order of crossing indices) */
   for(icross=0; icross<ncross; icross++) *(ord+icross) = icross;

   for(iplace=0;iplace<ncross-1;iplace++)
   {  /* NB: 'iplace' crossings have been sorted */
      mplace = iplace;
      icr = ord+iplace; icross = *icr; /* First unsorted crossing */
      mcr = icr;
      em = cls + *mcr;
      rm = xy  + *mcr;
      for(jplace=iplace+1,jcr=icr+1;
          jplace<ncross;
          jplace++,jcr++)
      { /* Loop over the other unsorted crossings */
         ej = cls + *jcr;
         rj = xy  + *jcr;
         if(CROSSORD(*ej,*rj,*em,*rm))
         { /* Earlier crossing found: make it the earliest */
            mplace = jplace;
            mcr = jcr;
            em = cls + *mcr;
            rm = xy  + *mcr;
         }
      }  /* Earliest unsorted crossing has been found */
      if(mplace != iplace)
      {  /* Earliest crossing not 'iplace': swap them */
         mcross = *mcr;
         /* Swap in ordering */
         *mcr = icross; *icr = mcross;
      }
   }                      /* End of for(iplace...) */

   return;
}
/************************************************** POLbbox_sl *******/
#ifdef PROTO
void POLbbox_sl ( Sectlist *ps, Long np, Scursor a, Rect *box)
#else
void POLbbox_sl ( ps, np, a, box)

/* Generate the bounding box for a polygon in section list form,
   specified by a scursor
*/
Sectlist *ps;    /* The section list */
Long     np;     /* Number of vertices in polygon */
Scursor   a;     /* Scursor on the polygon */
Rect    *box;    /* The bounding box */
#endif
{
   Index ip;  /* Loop index not used inside loop */

   box->left = box->right = a.p->x;
   box->bot  = box->top   = a.p->y;
   for(ip=1; ip<np; ip++)
   {
      MOVE_SCURSOR(ps,a);
      if(a.p->x < box->left)  box->left  = a.p->x;
      if(a.p->x > box->right) box->right = a.p->x;
      if(a.p->y < box->bot)   box->bot   = a.p->y;
      if(a.p->y > box->top)   box->top   = a.p->y;
   }
   return;
}
/************************************************** POLclip_sl *******/

#ifdef PROTO

Sectlist * POLclip_sl( Polygonset *pset, Rect *cl)
#else

Sectlist * POLclip_sl( pset, cl)

/*
Return a clipped polygon section list for an unclipped polygon
*/

Polygonset *pset;       /* Polygon Set */
Rect *cl;               /* Clipping rectangle */

#endif

{
   Int ncross = 0;      /* Number crossings of polygon and clip rect */
   Int nupoly,          /* Number of unclipped polygons */
       npoly,           /* Number of polygons allowed for */
       nsect,           /* Number of sections allowed for */
       npts;            /* Number of clip induced vertices allowed for */
   Int icross, *icr;    /* Index of current crossing */
   Int jcross;          /* Index of other crossing */
   Sectlist *ps;        /* Polygon section list */
   Clipcross cross;     /* Data on crossings of polygon on clip rect */
   Index *indin,
         *indout;       /* Indices of current Inner and Outer ends */
   Enum *cls;           /* Current Clip Status */
   Float *xy;           /* Current Signicant Coordinate */
   Int *pol, *oth;      /* Current Polygon and Other crossing */
   Point *in, *out;     /* Current Inner and Outer ends */
   Int ntop;            /* Number of crossings at top */
   Int ntotop;          /* Number of crossings between top point and
                           upper right hand corner */
   Point top;           /* Top point */
   Logical earlin,      /* Upper-right corner inside polygon */
           polyset,     /* Polygon Set */
       round4 = FALSE;  /* Clip induced edge section round 4 corners */
   Int firstcross;      /* First crossing of current polygon */
   Int ediff;           /* End difference */
   short join;          /* Joining direction */
   Logical inwards;     /* True, if inwards */
   Section **firsec;    /* First section in polygon */
   Enum sectiontype;    /* Type of section, in rect' or clip induced */
   Enum side1,side2;    /* Clip rectangle side for the two ends of
                           clip induced section */
   Enum *flag;          /* Edge-out flag */
   Logical clockwise;   /* True, if as clockeise clip induced section */
   Index ip;            /* Loop index for vertex */

/*
        ALGORITHM

The intersections between the polygon edges and the clipping boundary
are listed along with associated data.
Then a list of sections of vertices is produced.

Sections are either within the rectangle
and hence consist of already defined vertices
or they are clip induced sections that run along the boundary
of the clipping rectangle,
and hence consist of new vertices.



                    CLIP STATUS

    The clip status of a point is determined by its relationship to
the clipping rectange as shown below.

                   |                    |
          5        |         2          |       8
   ________________|____________________|________________
                   |                    |
                   |                    |
          3        |         0          |       6
                   |                    |
   ________________|____________________|________________
                   |                    |
          7        |         4          |      10
                   |                    |

      The clip status of a crossing initially that of its outer end.
 Later it becomes that of a point on the edge just outside the
 clipping rectangle.
      When this is the case the clipstatus is in order from
 upper right anticlockwise, so that the crossings can easily be sorted
 by xing_sort, with the CROSSORD macro.
*/
#ifdef DEBUG
   DMESS "POLclip_sl:  %d points,",pset->n);
#endif
   /* Count the polygons */

   nupoly = POLnumber ( pset->n, pset->eofl );
   if(nupoly == 0) return NULL;   /* Invalid edge out flag */

#ifdef DEBUG
   DMESS "%d unclipped polygons.\n", nupoly );
#endif

   /* Count number of intersections between polygon and clipping
      rectangle boundaries and estimate number of sections required */
   xing_count( pset, cl, &ncross, &nsect);
#ifdef DEBUG
   DMESS "POLclip_sl: %d crossings, %d sections estimated.\n",ncross,nsect);
#endif

   /* Determine space needed for clip induced vertices */
   npts = ncross + 4;

   polyset = (pset->eofl != NULL);   /* Is it a polygon set? */

   /* Initialise crossings data */
   cross.in = NULL;  cross.out = NULL;
   cross.cl = NULL;  cross.xy  = NULL;
   cross.ord = NULL;
   cross.pol = NULL;
   cross.inoth = NULL; cross.outoth = NULL;
   cross.clock = NULL;

   if(ncross > 0)
   {  /* There is at least one crossing of the clipping rectangle
         boundary by a polygon boundary: deal with these */

      /* Allocate space for crossing edges and clip stati */
      cross.in  =  (Index *)MALLOC(ncross, sizeof( Index ));
      cross.out =  (Index *)MALLOC(ncross, sizeof( Index ));
      if(polyset) cross.pol = (Int *)MALLOC(ncross, sizeof( Int ));
      cross.cl  =   (Enum *)MALLOC(ncross, sizeof( Enum));
      if((cross.in  == NULL) ||
         (cross.out == NULL) ||
         (polyset && (cross.pol == NULL)) ||
         (cross.cl  == NULL) )
         exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, "- crossings"));

      /* Get End points of each crossing edge, polygon & clip status */
      xing_edge( pset, cl,
               cross.in, cross.out, cross.pol, cross.cl );

      /* Allocate space for significant coordinates */
      cross.xy = (Float *)MALLOC(ncross, sizeof( Float));
      if(cross.xy == NULL)
         exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, "- clip coords"));

      /* Calculate crossing points */
      for(icross=0,indin=cross.in,indout=cross.out,
          cls=cross.cl,xy=cross.xy;
          icross<ncross;
          icross++,indin++,indout++,cls++,xy++)
      {
         in  = pset->pts + *indin;
         out = pset->pts + *indout;

#ifdef DEBUG_CROSS
         DMESS "POLclip_sl: crossing %d, in %d, out %d, status %d to",
         icross,*indin,*indout,*cls);
#endif

       /* Ammend clip status, to indicate the side of clipping rectangle,
          that the edge crosses */
         switch(*cls)
         {
            case CLIPTOP+CLIPLEFT:
               if(RCLOCK(in,out,cl,left,top))
                                   *cls = CLIPLEFT;
               else                *cls = CLIPTOP;
               break;
            case CLIPTOP+CLIPRIGHT:
               if(RCLOCK(in,out,cl,right,top))
                                   *cls = CLIPTOP;
               else                *cls = CLIPRIGHT;
               break;
            case CLIPBOT+CLIPRIGHT:
               if(RCLOCK(in,out,cl,right,bot))
                                   *cls = CLIPRIGHT;
               else                *cls = CLIPBOT;
               break;
            case CLIPBOT+CLIPLEFT:
               if(RCLOCK(in,out,cl,left,bot))
                                   *cls = CLIPBOT;
               else                *cls = CLIPLEFT;
               break;
         }

#ifdef DEBUG_CROSS
        DMESS " %d,", *cls);
#endif
         /* Calculate significant coordinate (Y if left or right)
                                             (X if top or bottom ) */
         switch(*cls)
         {
           case CLIPLEFT:
              *xy = ECLIP(in->y,out->y,in->x,out->x,cl->left);
              break;
           case CLIPRIGHT:
              *xy = ECLIP(in->y,out->y,in->x,out->x,cl->right);
              break;
           case CLIPTOP:
              *xy = ECLIP(in->x,out->x,in->y,out->y,cl->top);
              break;
           case CLIPBOT:
              *xy = ECLIP(in->x,out->x,in->y,out->y,cl->bot);
              break;
           default:
#ifdef DEBUG
              DMESS
              "POLhandle: trimmed clip indicator %d wrong.\n",*cls);
#endif
              *xy = 0.0;
              break;
         }
#ifdef DEBUG_CROSS
        DMESS " sigcoo %f.\n", *xy);
#endif
      }   /* End of for() */

      /* Allocate space to ordering of crossings */
      cross.ord = ( Int *)MALLOC( ncross, sizeof( Int ));
      if(cross.ord == NULL)
         exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, "- orderings"));

      /* Find the ordering of the crossings around the clip rect */
      xing_sort(ncross, cross.cl, cross.xy, cross.ord );
#ifdef DEBUG
      DMESS "POLclip_sl: 1st Crossing %d, Sigcoo %f.\n",
      *(cross.ord),*(cross.xy + *(cross.ord)));
#endif

      /* Count number of crossings through top of clipping rectangle */
      for(ntop=0;ntop<ncross;ntop++)
      {
         if( *(cross.cl + *(cross.ord + ntop)) != CLIPTOP) break;
      }
#ifdef DEBUG
      DMESS "POLclip_sl: %d crossings through top.\n",ntop);
#endif

      /* Select a point from the top of the clipping rectangle
         and determine whether it would be filled in the polygon set */
      top.y = cl->top;
      ntotop = 0;
      xy = cross.xy;
      if(ntop == 0)
      {  /* No top crossings: select mid point of top */
         top.x = (0.5)*(cl->left + cl->right);
      }
      else
      {  /* Find a point not on a polygon edge */
         if(VEQUAL(cl->right,*(xy + *(cross.ord))))
         {  /* Earliest crossing on upper-right corner (unlikely) */
            for(icross=0,icr=cross.ord;icross<ntop-1;icross++,icr++)
            {
              if(!VEQUAL( *(xy + *icr), *(xy+ *(icr+1))))
              { /* Crossings have a gap between them */
                  top.x = (0.5)*( *(xy + *icr) + *(xy + *(icr+1)));
                  ntotop = icross+1;
                  break;
              }
            }
            if(ntotop == 0)
            {  /* No gaps between crossings */
                top.x = (0.5)*( *(xy + *icr) + cl->left);
                ntotop = ntop;
            }
         }
         else
         {   /* Gap between earliest edge and upper-right corner */
#ifdef DEBUG
            DMESS "POLclip_sl: Topgap 1st Crossing %d, Sigcoo %f\n",
            *(cross.ord),*(xy + *(cross.ord)));
#endif
            top.x = (0.5)*( cl->right + *(xy + *(cross.ord)));
         }
      }
      /* Determine whether the selected point would be filled */
      earlin = POLin( pset, &top );
#ifdef DEBUG
      DMESS "POLclip_sl: selected top point (%f,%f) %d from 1st corner.\n",
      top.x,top.y,ntotop);
      if(earlin) DMESS "POLclip_sl: selected top pt INSIDE poly");
      else       DMESS "POLclip_sl: selected top pt OUTSIDE poly");
#endif

      /* Determine whether upper right corner would be filled */
      for(icross=0;icross<ntotop;icross++) earlin = !earlin;
#ifdef DEBUG
         DMESS " 1st corner in poly %d\n",earlin);
         if(earlin) DMESS " 1st corner INSIDE poly\n");
         else       DMESS " 1st corner OUTSIDE poly\n");
#endif

      /* Determine whether there is a clip induced edge section that
         goes round all four corners of the clip rectangle.

         Such an edge section must have the first sorted crossing at
         one end.
      */
      if(earlin)
      {        /* First corner of clip rectangle is inside polygon */
         for(icross=0,cls=cross.cl;icross<ncross;icross++,cls++)
         {
           if(*cls != *(cross.cl)) break;
         }
         round4 = TRUE;  /* All clip stati are equal */
      }

      /* Identify the crossings at the other ends of
      each clipped section */
      cross.inoth = ( Int *)MALLOC( ncross, sizeof( Int ));
      if(cross.inoth == NULL)
       exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, "- other ends in"));
      /* Prepare for First polygon */
      firstcross = 0;
      inwards = ( *(cross.in) > *(cross.out) );
      for(icross=0,pol=cross.pol, oth=cross.inoth;
          icross<ncross-1;
          icross++,pol++,oth++)
      {
         if(!polyset || (*pol == *(pol+1)))
         {  /* Crossing in same polygon as the next crossing */
            if(inwards)
            {  /* Inward crossing */
               *oth = icross+1;   /* inward end index of outward end */
               *(oth+1) = icross; /* outward end index of inward end */
#ifdef DEBUG_CROSS
               DMESS "POLclip_sl: crossing %d inwards, inoth %d at %d\n",
                      icross, *oth, oth-cross.inoth);
               DMESS "POLclip_sl: crossing %d outwrds, inoth %d at %d\n",
                      icross+1, *(oth+1), oth+1-cross.inoth);
#endif
            }
            inwards = !inwards;  /* Crossings alternate in and out */
         }
         else
         {  /* Last crossing of current polygon & new polygon to follow */
            if(inwards)
            {  /* Inward crossing */
               *oth = firstcross;
               *(cross.inoth+firstcross) = icross;
#ifdef DEBUG_CROSS
        DMESS "POLclip_sl: crossing %d inwards, inoth %d at %d close\n",
                      icross, *oth, oth-cross.inoth);
        DMESS "POLclip_sl: crossing %d outwrds, inoth %d at %d close\n",
                      firstcross, *(cross.inoth+firstcross), firstcross);
#endif
            }
            /* Prepare for next polygon */
            firstcross = icross+1;
            ediff = *(cross.in + firstcross) - *(cross.out + firstcross);
            inwards = (ediff == 1) || (ediff < -1);
         }
      }
      /* Final crossing */
      if(inwards)
      {  /* Inward crossing */
         *oth = firstcross;
         *(cross.inoth+firstcross) = icross;
#ifdef DEBUG_CROSS
          DMESS "POLclip_sl: crossing %d inwards, inoth %d at %d\n",
                 icross, *oth, oth-cross.inoth);
          DMESS "POLclip_sl: crossing %d outwrds, inoth %d at %d\n",
                 firstcross, *(cross.inoth+firstcross), firstcross);
#endif
      }


   /* Identify the crossings at the other end of clip induced sections */
      cross.outoth = ( Int *)MALLOC( ncross, sizeof( Int ));
      cross.clock = ( Logical *)MALLOC( ncross, sizeof( Logical ));
      if((cross.outoth == NULL) || (cross.clock == NULL))
       exit((int)CGMerror(func, ERR_NOMEMORY, FATAL,"- other ends out"));
      if(earlin) join = -1; else join = 1;
      for(icross=0;
          icross<ncross;
          icross++,join =- join)
      {
          oth = cross.outoth + *(cross.ord +icross);
          jcross = icross+join;
          if(jcross == -1) jcross = ncross-1;
          if(jcross == ncross) jcross = 0;
          *oth = *(cross.ord + jcross);
          *(cross.clock + *(cross.ord + icross)) = (join == -1);
#ifdef DEBUG_CROSS
          DMESS "POLclip_sl: icross %d, jcross %d, ord %d, ordj %d,",
                 icross,jcross,*(cross.ord+icross),*(cross.ord+jcross));
          DMESS " outoth %d at %d\n",
                 *oth,oth-cross.outoth);
#endif
      }

#ifdef DEBUG_CROSS
      for(icross=0; icross<ncross; icross++) /* Print Crossings Data */
      {
          DMESS "Xing %d, in %d, out %d, cl %d, xy %f,",
                icross,*(cross.in+icross),*(cross.out+icross),
                *(cross.cl+icross),*(cross.xy+icross));
          DMESS "ord %d, oth %d %d, clock %d\n",
                *(cross.ord+icross),*(cross.inoth+icross),
                *(cross.outoth+icross),*(cross.clock+icross));
      }
#endif
   }                           /* End of if(ncross > 0) */

   /* Get space for the sectionlist structure */
   npoly = nupoly + ncross/2 - 1;
#ifdef DEBUG
   DMESS "POLclip_sl:, %d crossings %d polys max\n",ncross,npoly );
#endif
   ps = POLgrow_sl(NULL,npoly,nsect,npts);
   if(ps == NULL)
   {
      xing_free(&cross);
      return NULL;
   }


   {  /* Define the Sections of Clipped Polygons */
      Xcursor xcr;            /* Cursor on crossing data */
      Index *polsta,          /* Start and End of Unclipped Polygon */
            *polend;
      Index spl1, spl2;       /* Split of section */
      Int curpol;             /* Current Polygon Index */
      Int lopol,hipol;        /* Polygon Indices */
      Logical allin;   /* True, if polygon is entirly within rectangle */

      ps->freept = ps->newpts;
      firsec = ps->poly;
      *firsec = ps->vacant;

      /* Make a section for each polygon entirely within the rectangle */
      if(polyset)
      {   /* Must be polygonset, if any polygons are entirely inside */

          /* Find any polygons ommitted from the crossings */
          lopol = -1;
          for(icross=0; icross<ncross+1; icross++)
          {
             if(icross < ncross) hipol = *(cross.pol + icross);
             else                hipol = nupoly;
             if(hipol > lopol + 1 )
             {  /* Block of consecutive Ommitted Polygons Detected */
#ifdef DEBUG
                DMESS "POLclip_sl: Block of Ommited Polys (%d to %d)\n",
                       lopol+1,hipol-1);
#endif
                if(icross > 0)
                {
                   ip = *(cross.in + icross-1);
                   POLend ( pset->n, pset->eofl, ip, NULL, &ip);
                   ip++;  /* Start of first ommitted polygon */
                }
                else ip = 0; /* First polygon ommitted */

                /* Add each ommitted polygon in consecutive block,
                   as a section if its 1st vertex is inside the
                   clipping rectangle */
                for(curpol=lopol+1; curpol<hipol; curpol++)
                {
                   in = pset->pts + ip;
                   ps->vacant->start = in;
                   POLend(pset->n, pset->eofl, ip, NULL, &ip);
                   if(!CLIPCODE(in->x,cl->left,cl->right) &&
                      !CLIPCODE(in->y,cl->bot,cl->top))
                   {  /* Polygon is Inside Rect, add it as section */
                      ps->vacant->end = pset->pts+ip;
# ifdef DEBUG_SECT
                      DMESS "POLclip_sl: polygon %d, ",firsec-ps->poly);
                      DMESS "section %d ( %d to %d )\n",
                      ps->vacant,ps->vacant->start,ps->vacant->end);
#endif
                      ps->vacant++;
                      firsec++;
                      *firsec = ps->vacant;
                   }
                   ip++; /* Start of next polygon */
                }     /* End of loop over curpol */
             }       /* End of if(hipol > lopol+1) */
             lopol = hipol;
          }          /* End of loop over icross */
      }


      if(ncross > 0)
      {  /* There are crossings */
         if(xing_start(ncross,cross.in,cross.out,cross.inoth,&xcr) == NULL)
            return NULL;  /* Nothing found */

         for(icross=0; icross<ncross+1; icross++)
         {
#ifdef DEBUG_CROSS
            DMESS "POLclip_sl: XX Crossing Index in Loop %d XX\n",icross);
#endif
            if(xcr.sectype != CLIPINDUCED)
            {
               /* Section of polygon inside clipping rectangle */
               ps->vacant->start = pset->pts + *(cross.in + xcr.cross1);
#ifdef DEBUG_CROSS
               DMESS "(i) Point %d (xing %d) to point %d (xing %d) \n",
               *(cross.in + xcr.cross1), xcr.cross1,
               *(cross.in + xcr.cross2), xcr.cross2);
#endif
               if((xcr.sectype == FORWINRECT) &&
               (*(cross.in + xcr.cross1) > *(cross.in + xcr.cross2)) )
               {
                  polsta = &spl1;    polend = &spl2;
#ifdef DEBUG_CROSS
                  DMESS "POLclip_sl forward section (%d,%d) split\n",
                  *(cross.in + xcr.cross1),*(cross.in+xcr.cross2));
#endif
               }
               else
               {
                  if((xcr.sectype == BACKWINRECT) &&
                     (*(cross.in+xcr.cross2) > *(cross.in+xcr.cross1)) )
                  {
                     polend = &spl1;    polsta = &spl2;
#ifdef DEBUG_CROSS
                     DMESS "POLclip_sl backward section (%d,%d) split\n",
                     *(cross.in + xcr.cross1),*(cross.in+xcr.cross2));
#endif
                  }
                  else
                  {
                     polsta = NULL;  polend = NULL;
                  }
               }
#ifdef DEBUG_CRSEC
               DMESS "POLclip_sl: Sectiontype %d (xing %d to xing %d)\n",
               xcr.sectype,xcr.cross1,xcr.cross2);
#endif
               if( polsta != NULL)
               {  /* Section includes closing edge of unclipped polyon:
                     split there */
                  POLend ( pset->n, pset->eofl, *(cross.in + xcr.cross1),
                           polsta, polend );
                  ps->vacant->end = pset->pts + spl2;
                  /* Add first part of section as a section */
#ifdef DEBUG_SECT
                  DMESS "POLclip_sl: polsta %d at %d, polend %d at %d\n",
                         *polsta,polsta, *polend,polend);
                  DMESS "POLclip_sl: section %d ( %d to %d )\n",
                         ps->vacant,ps->vacant->start,ps->vacant->end);
        DMESS "POLclip_sl: section split at closing edge: sectype %d,",
                         xcr.sectype);
                  DMESS " edge (%d to %d)\n",spl2,spl1);
#endif
                  ps->vacant++;
                  /* Start 2nd part of section as a section */
                  ps->vacant->start = pset->pts + spl1;
               }

               if(xcr.sectype != NOSECTION)
               { /* Crossings form the ends of a section,
                    add the section */
                  ps->vacant->end = pset->pts + *(cross.in + xcr.cross2);
#ifdef DEBUG_SECT
                  DMESS "POLclip_sl: section %d ( %d to %d )\n",
                         ps->vacant,ps->vacant->start,ps->vacant->end);
#endif
                  ps->vacant++;  /* Next Section */
               }

               /* Move to next pair of crossings */
               if(xing_next(cross.in,cross.out,cross.inoth,cross.outoth,
                          &xcr) == NULL)
               {   /* End of clipped polygon reached, find next one */
                  firsec++;
                  *firsec = ps->vacant;
                  if(xing_start(ncross,cross.in,cross.out,cross.inoth,&xcr)
                     == NULL ) break;
               }
            }
            else
            {   /* Clip induced section */
               xing_addclin(cross.cl,cross.xy,cl,xcr.cross1,ps->freept);
               ps->vacant->start = ps->freept;
               ps->freept++;
#ifdef DEBUG_CROSS
               DMESS "(o) Point %d (xing %d) to point %d (xing %d)\n",
               *(cross.out + xcr.cross1), xcr.cross1,
               *(cross.out + xcr.cross2), xcr.cross2);
#endif
               side1 = *(cross.cl+xcr.cross1);  /* Rect'side for end 1 */
               side2 = *(cross.cl+xcr.cross2);  /* Rect'side for end 2 */
               clockwise = *(cross.clock+xcr.cross1);
                                                 /* Clockwise section? */
               if(side1 == side2)
               { /* Both ends on same side - need to see if edge section
                                             goes round rectangle */
                  if(round4)
                  {
                     if((*cross.ord == xcr.cross1) ||
                         *cross.ord == xcr.cross2)
                     { /* This clip induced section goes round
                          all four corners of the clipping rectangle */
                        ps->freept =
                        POLaddcnrs(side1,side2,clockwise,cl,ps->freept);
                     }
                  }
               }
               else
               { /* Ends are on different sides - need to add corner(s)
                                                 of clipping rectangle */
                  ps->freept =
                  POLaddcnrs(side1,side2,clockwise,cl,ps->freept);
               }
               /* Add 2nd crossing point */
               xing_addclin(cross.cl,cross.xy,cl,xcr.cross2,ps->freept);
               ps->vacant->end = ps->freept;
               ps->freept++;
#ifdef DEBUG_SECT
               DMESS "POLclip_sl: section %d ( %d to %d )\n",
                       ps->vacant,ps->vacant->start,ps->vacant->end);
#endif
               ps->vacant++;  /* Next Section */

               /* Move to next pair of crossings */
               if(xing_next(cross.in,cross.out,cross.inoth,cross.outoth,
                          &xcr) == NULL)
               {   /* End of clipped polygon reached, find next one */
                  firsec++;
                  *firsec = ps->vacant;
                  if(xing_start(ncross,cross.in,cross.out,cross.inoth,&xcr)
                     == NULL ) break;
               }
            }                        /* End of if(xcr.sectype ...) */
         }                           /* End of loop over icross */
         /* No more need from crossings data */
         xing_free(&cross);
       }
      else
      {   /* No crossings of the clipping rectangle boundary by
             polygon boundary. */

          /* Determine if upper-right corner is inside polygon set */
          top.x = cl->right;  top.y = cl->top;
          if(POLin ( pset, &top))
          { /* Corner is inside, add rectangle as a section */
             ps->vacant->start = ps->freept;
             ps->freept = POLaddcnrs (CLIPTOP,CLIPTOP,FALSE,cl,
                                      ps->freept);
             ps->vacant->end = ps->freept-1;
             ps->vacant++;
             firsec++;
             *firsec = ps->vacant;
          }
      }                              /* End of if(ncross > 0) */

      /* Label end of polygon list */
      if(firsec < ps->poly + ps->mpoly + 1) *firsec = ps->vacant;
      ps->npoly = firsec - ps->poly;
#ifdef DEBUG
      DMESS "POLclip_sl: %d polygons after clipping \n",ps->npoly);
#endif
   }  /* Sections have been defined */

   /* Return the sectionlist */
   return ps;
}
/************************************************** POLcontig_sl *****/
#ifdef PROTO
void POLcontig_sl ( Sectlist *ps )
#else
void POLcontig_sl ( ps )

/* Make the sections in a section list contiguous */

Sectlist *ps;  /* The sectionlist */
#endif

/* WARNING

   Do not call this function while using a scursor on the section list.
   Any such scursor could lose its place.
*/

/* ALGORITHM

After the first gap sections are moved back up the section list
to fill the gaps. All pointers to sections within the sectionlist
structure are updated.
No additional space is allocated during the execution of this function.
*/
{
   Int i;             /* Loop variable, not used in loop */
   Int poly = 0;      /* Current polygon */
   Section *fromsec;  /* Current section (before moving) */
   Section *tosec;    /* Current section (after moving) */
   Logical ingap=FALSE;  /* Section 'fromsec' is in gap (ie, deleted) */
   Logical shift=FALSE;  /* Next section would be moved */

   tosec = ps->first;

   for(i=0,           fromsec=ps->first;
       i < ps->msect && fromsec < ps->vacant;
       i++,           fromsec++)
   {
      if(fromsec->start == NULL)
      {       /* Gap has been encontered */
         ingap = TRUE;
         shift = TRUE;
      }

      if(*(ps->poly+poly+1) == fromsec)
      {        /* Next polygon (or end) has been reached */
         ingap=FALSE;
         poly++;
         if(shift)
         {
           *(ps->poly+poly+1) = tosec;
         }
         if(poly > ps->npoly -1) break;  /* End reached */
      }

      if(!ingap)
      {  /* Not in gap */
         if(shift)
         {
           *tosec = *fromsec;   /* Move back section */
         }
         tosec++;
      }
   }
   if(shift) ps->vacant = tosec;  /* Move back vacancies */
   ps->contig = TRUE;             /* Mark section list as contiguous */

   return;
}
/************************************************** POLcount_sl ******/
#ifdef PROTO
void POLcount_sl ( Sectlist *ps, Int poly, Int *ns, Long *np)
#else
void POLcount_sl ( ps, poly, ns, np )

/* Count the number of sections and/or vertices in the polygon
   as requested.
*/

Sectlist *ps;   /* The section list */
Int poly;       /* The polygon */
Int *ns;        /* Number of sections (NULL, if not requested) */
Long *np;       /* Number of vertices (NULL, if not requested) */

#endif
{
   Int i;              /* Loop variable - not used inside loop */
   Section *cursec;    /* Curent section */

   if(np == NULL)
   { /* Number of vertices is not requested - simpler algorithm */
     if(ns != NULL)
     {
        if(ps->contig) *ns = *(ps->poly + poly+1) - *(ps->poly + poly);
        else
        { /* non-contiguous section list */
           for(cursec = *(ps->poly + poly), i=0;
               cursec < *(ps->poly + poly+1);
               cursec++, i++)
           {
              if(cursec->start == NULL)break;
           }
           *ns = i;
        }
     }
   }
   else
   {
#ifdef DEBUG_SECT
     DMESS "POLcount_sl: Polygon %d from section %d to %d\n",
      poly,*(ps->poly + poly),*(ps->poly + poly+1));
#endif
      if( *(ps->poly + poly+1) < *(ps->poly + poly) )
      {  /* Bad Polygon List Structure Detected */
#ifdef DEBUG
         DMESS "POLcount_sl: Bad Polygon in PSL %d (%d to %d)\n",
                poly,*(ps->poly + poly), *(ps->poly + poly + 1));
#endif
         if(ns != NULL) *ns = 0;
         *np = 0;
         return;
      }

      for(cursec = *(ps->poly + poly), i=0, *np=0;
          cursec < *(ps->poly + poly+1);
          cursec++, i++)
      {
          if(!ps->contig && (cursec->start == NULL)) break;
          if(cursec->start < cursec->end)
            *np += cursec->end - cursec->start + 1;
          else
            *np += cursec->start - cursec->end + 1;

#ifdef DEBUG_SECT
          DMESS "POLcount_sl: vertex count %d, cursec %d to %d\n",
                *np, cursec->start, cursec->end);
#endif
      }
      if(ns != NULL) *ns = i;
   }
   return;
}
/*************************************************** POLfree_sl *******/
#ifdef PROTO
void POLfree_sl (Sectlist *ps)
#else
void POLfree_sl ( ps )

/* Free the space allocated to a sectionlist */

Sectlist *ps;   /* The section list */

#endif

{
   if(ps->first != NULL) FREE(ps->first);
   if(ps->newpts != NULL) FREE(ps->newpts);
   if(ps->poly != NULL) FREE(ps->poly);
   FREE(ps);
}
/************************************************** POLgrow_sl *******/
#ifdef PROTO
Sectlist * POLgrow_sl ( Sectlist *ps, Int npoly, Int nsect, Int npts )
#else
Sectlist * POLgrow_sl ( ps, npoly, nsect, npts )

/* Enables a section list to hold a specified number of
   polygons, sections or points
   Reallocating memory when necessary

   If no section list is supplied, one is created.
*/

Sectlist *ps;  /* The section list */
Int npoly;     /* Number of polygons needed */
Int nsect;     /* Number of sections needed */
Int npts;      /* Number of new points needed */
#endif

/* WARNING

   Do not call this function while using a scursor on the section list.
   Any such scursor may lose its place.
*/
{
   Sectlist oldps;   /* Used to save old values of the Sectionlist */
   Int shsect;       /* shift of sections */
   Int shpts;        /* shift of newpoints */
   Int ipoly;        /* Current polygon */
   Section *cursec;  /* Current section */

#ifdef DEBUG
     DMESS "POLgrow_sl: %d sections, %d polygons, %d points.\n"
     ,nsect,npoly,npts);
#endif

   if(ps == NULL)
   {  /* No section list supplied - create one */
      ps = (Sectlist *)MALLOC(1, sizeof(Sectlist));
      if(ps == NULL)
      {
         CGMerror(func, ERR_NOMEMPSL, ERROR, "- section list");
         return NULL;
      }
      /* Create sections */
      ps->first = (Section *)MALLOC(nsect, sizeof(Section));
      if(ps->first == NULL)
      {
         CGMerror(func, ERR_NOMEMPSL, ERROR, "- sections");
         FREE(ps);
         return NULL;
      }
      ps->vacant = ps->first;
      ps->msect = nsect;
      /* Create polygons */
      ps->poly = (Section **)MALLOC(npoly+1, sizeof(Section *));
      if(ps->poly == NULL)
      {
         CGMerror(func, ERR_NOMEMPSL, ERROR, "- polygons");
         POLfree_sl(ps);
         return NULL;
      }
      ps->npoly = 0;
      ps->mpoly = npoly;
      /* Create new points, if requested */
      if(npts > 0)
      {
         ps->newpts = (Point *)MALLOC(npts, sizeof(Point));
         if(ps->newpts == NULL)
         {
            CGMerror(func, ERR_NOMEMPSL, ERROR,"- new points");
            POLfree_sl(ps);
            return NULL;
         }
         ps->freept = ps->newpts;
         ps->mpts = npts;
      }
      else
      {
        ps->newpts = NULL;
      }
      ps->contig = TRUE;   /* Sections are contiguous */
   }
   else
   {  /* Section list already exists */
      if(ps->msect < nsect)
      {  /* More sections needed */
         oldps = *ps;
         ps->first = (Section *)realloc((void *)ps->first,
                                        (size_t)(nsect*sizeof(Section)));
         if(ps->first == NULL)
         {
            CGMerror(func, ERR_NOMEMPSL, ERROR,"- more sections");
            POLfree_sl(ps);
            return NULL;
         }
         shsect = ps->first - oldps.first;
         ps->vacant = ps->vacant + shsect;
         for(ipoly=0; ipoly < ps->npoly+1; ipoly++)
         {
            *(ps->poly+ipoly) = *(oldps.poly+ipoly) + shsect;
         }
      }

      if(ps->mpoly < npoly)
      {   /* More polygons needed */
         ps->poly = (Section **)realloc((void *)ps->poly,
                                  (size_t)((npoly+1)*sizeof(Section *)));
         if(ps->poly == NULL)
         {
            CGMerror(func, ERR_NOMEMPSL, ERROR,"- more polygons");
            POLfree_sl(ps);
            return NULL;
         }
         ps->msect = nsect;
      }

      if(ps->mpts < npts)
      {  /* More points needed */
         oldps = *ps;
         ps->newpts = (Point *)realloc((void *)ps->newpts,
                                       (size_t)(npts*sizeof(Point)));

         if(ps->newpts == NULL)
         {
            CGMerror(func, ERR_NOMEMPSL, ERROR,"- more new points");
            POLfree_sl(ps);
            return NULL;
         }
         shpts = ps->newpts - oldps.newpts;
         for(cursec=ps->first; cursec < ps->vacant; cursec++)
         {  /* Repoint sections that use newpoints */
            if((cursec->start >= oldps.newpts) &&
               (cursec->start <  oldps.freept))
            {  /* Section uses newpoints */
               cursec->start = cursec->start + shpts;
               cursec->end = cursec->end + shpts;
            }
         }
      }
   }
   return ps;
}
/************************************************** POLinfind_sl ******/
#ifdef PROTO
void POLinfind_sl (Sectlist *ps, Int *inpoly, Int *outpoly)
#else
void POLinfind_sl ( ps, inpoly, outpoly)

/* Find a pair of polygons, where one is entirely inside the other,
   the polygons are in section list form
   and no two polygon boundaries cross each other.
*/
Sectlist *ps;        /* The section list */
Int *inpoly;         /* The inside polygon (-1 if not found) */
Int *outpoly;        /* The outside polygon */
#endif
{
   Int poly1,poly2;        /* The two candidate polygons */
   Logical found=FALSE;    /* Pair of polygons found */

   for(poly1=0; poly1 < ps->npoly-1; poly1++)
   {
      for(poly2=poly1+1; poly2 < ps->npoly; poly2++)
      {
         if( POLinpoly_sl(ps, poly1, poly2))
         { /* 1st polygon is inside 2nd */
           *inpoly = poly1;
           *outpoly = poly2;
           found = TRUE;
         }
         else
         {  /* 1st polygon not inside 2nd */
            if( POLinpoly_sl(ps, poly2, poly1))
            {  /* 2nd polygon is inside 1st */
                *inpoly = poly2;
                *outpoly = poly1;
                found = TRUE;
            }
         }
         if(found)break;

      }
      if(found)break;
   }

   if(!found) *inpoly = -1;   /* Set inpoly to -1 if not found */
   return;
}
/************************************************** POLinjoin_sl ******/
#ifdef PROTO
void POLinjoin_sl ( Sectlist *ps,
                 Long nin, Int inpoly, Long nout, Int outpoly)
#else
void POLinjoin_sl ( ps, nin, inpoly, nout, outpoly)

/* Join two polygons together, one which is inside the other
   along a pair of coincident edges.
*/
Sectlist *ps;     /* The section list */
Long nin;         /* Number of vertices on the inside polygon */
Int inpoly;       /* The inside polygon */
Long nout;         /* Number of vertices on the outside polygon */
Int outpoly;      /* The outside polygon */

/* This function appends the joined polygon onto the end of the
    section list and assumes there is enough space for it.
   It deletes the unjoined pair by putting NULL pointer in
   the start section of each and removing them from the polygon list.

   POLcontig_sl can be used to make the section list contiguous.
*/
#endif

/* ALGORITHM

   Finds the closest pair of vertices, such that one is in
   each polygon and joins the two polygons on the edge between these
   two vertices by not inserting a joining point.
   This edge may be visible on some displays.
*/
{
   Scursor a, b;    /* Scursors for the two polygons */
   Scursor b1;      /* Initial Scursor for  polygon */
   Scursor na, nb;  /* Scursors for the Closest pair of vertices */
   Point d;         /* Difference vector */
   Float sqdist;    /* Minimum Squared distance */
   Float cursqdist; /* Current Squared distance */
   Index i,j;       /* Loop indices not used inside loop */

   /* Find closest pair of vertices */
   POLscursor_sl( ps, outpoly, &a);
   POLscursor_sl( ps, inpoly, &b1);
   d.x = b1.p->x - a.p->x;
   d.y = b1.p->y - a.p->y;
   sqdist = (d.x)*(d.x) + (d.y)*(d.y);
   na = a; nb = b1;
   for(j=0; j<nout; j++)  /* Vertices of outer polygon */
   {
      for(i=0, b=b1; i<nin; i++)  /* Vertices of inner polygon */
      {
         d.x = b.p->x - a.p->x;
         cursqdist = (d.x)*(d.x);
         if(sqdist > cursqdist)
         {
            d.y = b.p->y - a.p->y;
            cursqdist += (d.y)*(d.y);
            if(sqdist > cursqdist)
            {
               sqdist = cursqdist;
               nb = b; na = a;
            }
         }
         MOVE_SCURSOR(ps,b);
      }
      MOVE_SCURSOR(ps,a);
   }

  /* Join the polygons without adding a joining point.
     This causes them to be joined along the edge
     between the joining vertices */
   POLjoin_sl(ps, &na , &nb, NULL);

   return;
}
/************************************************** POLinpoly_sl ******/
#ifdef PROTO
Logical POLinpoly_sl ( Sectlist *ps, Int inpoly, Int outpoly)
#else
Logical POLinpoly_sl ( ps, inpoly, outpoly)

/* Determine whether a specified polygon is inside another
   specified polygon,
   where the polygons are in section list form
   and neither polygon boundary crosses the other.
*/

Sectlist *ps;     /* The section list */
Int inpoly;       /* The polygon that may be inside */
Int outpoly;      /* The polygon that may surround */

/* ALGORITHM
The first vertex of 'inpoly' is tested to see if
it is inside 'outpoly'. The result is then returned.
*/
#endif
{
   Point *test;         /* Point to be tested for insideness */
   Scursor a;           /* Scursor for polygon that may surround */
   Logical oddcross = FALSE;    /* Number of edges crossed is odd */
   Index ip;                    /* Loop Index */
   Point *end1, *end2;          /* Ends of current edge */
   Long np;             /* Number of vertices */
   Int  ns;             /* Number of sections */
   Rect box;           /* Bounding box of polygon that may surround */

   test = ( *(ps->poly) + inpoly)->start;
   if(test == NULL) return FALSE;
   POLscursor_sl(ps, outpoly, &a);
   POLcount_sl(ps, outpoly, &ns, &np);
   POLbbox_sl (ps, np, a, &box);
   if(!( (test->x < box.left) || (box.right < test->x) ||
         (test->y < box.bot)  || (box.top   < test->y) ) )
   {  /* test point is in bounding box of polygon */
      end1 = a.p;
      for(ip=0;ip<np;ip++)
      {
         MOVE_SCURSOR(ps,a);
         end2 = a.p;
         if(goes_over(end1,end2,test)) oddcross = !oddcross;
         end1 = end2;
      }
   }
   return oddcross;
}
/************************************************** POLjoin_sl ********/
#ifdef PROTO
void POLjoin_sl(Sectlist *ps, Scursor *sc1, Scursor *sc2, Point *join)
#else
void POLjoin_sl ( ps, sc1, sc2, join)

/* Join two polygons insert a joining point, if provided */

Sectlist *ps;        /* The section list with the two polygons in it */
Scursor *sc1, *sc2;  /* Scursors at were the join will take place */
Point *join;         /* The joining point, if inserted else NULL */

/* This function appends the joined polygon onto the end of the
    section list and assumes there is enough space for it.
   It deletes the unjoined pair by putting NULL pointer in
   the start section of each and removing them from the polygon list.

   POLcontig_sl can be used to make the section list contiguous.
*/

#endif
{
    Scursor a;             /* Scursor to run through unjoined polygons */
    Int ipoly;             /* Polygon in old polygon list */
    Int jpoly;             /* Polygon in new polygon list */
    Logical shift=FALSE;   /* Polygon needs shifting in polygon list */
    Int i;                 /* Loop index */

    /* check that the polygons are different */
    if(sc1->firstsec == sc2->firstsec)
    {
       return;
    }

    /* Start joined polygon at joining point, if there is one */
    if(join != NULL)
    {
      *(ps->freept) = *join;
      join = ps->freept;
      ps->freept++;
      ps->vacant->start = ps->vacant->end = join;
      ps->vacant++;
    }
   /* Put in 1st polygon starting just after joining place */
    a = *sc1;
    MOVE_SCURSOR(ps,a);
    ps->vacant->start = a.p;
    ps->vacant->end = a.s->end;
    ps->vacant++;
    for(i=0; i < sc1->lastsec - sc1->firstsec + 1; i++)
    {
        POLnext_sl(ps,&a);  /* Move to start of next section */
        ps->vacant->start = a.p;
        if(a.s == sc1->s)
        {  /* We're back at the first section */
           ps->vacant->end = sc1->p;
           ps->vacant++;
           break;
        }
        else
        {
          ps->vacant->end = a.s->end;
          ps->vacant++;
        }
    }
    /* Put in crossing point in again, if provided */
    if(join != NULL)
    {
       ps->vacant->start = join;
       ps->vacant->end = join;
       ps->vacant++;
    }
   /* Put in 2nd polygon just after starting point */
    a = *sc2;
    MOVE_SCURSOR(ps,a);
    ps->vacant->start = a.p;
    ps->vacant->end = a.s->end;
    ps->vacant++;
    for(i=0; i < sc2->lastsec - sc2->firstsec + 1; i++)
    {
        POLnext_sl(ps,&a);  /* Move to start of next section */
        ps->vacant->start = a.p;
        if(a.s == sc2->s)
        {  /* We're back at the first section */
           ps->vacant->end = sc2->p;
           ps->vacant++;
           break;
        }
        else
        {
          ps->vacant->end = a.s->end;
          ps->vacant++;
        }
    }

    /* Now the joined polygon has been put into the section list */
    /* Delete the unjoined pair of polygons */
    sc1->firstsec->start = NULL;
    sc2->firstsec->start = NULL;
    /* Section list is no longer contiguous */
    ps->contig = FALSE;
    /* Update the polygon list */
    for(ipoly=0,jpoly=0; ipoly < ps->npoly+1; ipoly++)
    {
       if((*(ps->poly+ipoly) == sc1->firstsec)||
          (*(ps->poly+ipoly) == sc2->firstsec))
       {
          shift = TRUE;
       }
       else
       {
          if(shift)
          {
             *(ps->poly + jpoly) = *(ps->poly + ipoly);
          }
          jpoly++;
       }

    }
    *(ps->poly + jpoly) = ps->vacant;
    ps->npoly = jpoly;

    return;
}

/************************************************** POLnext_sl ********/
#ifdef PROTO
void POLnext_sl( Sectlist *ps, Scursor *sc )
#else
void POLnext_sl( ps, sc)

/* Move the scursor supplied to the start of the next section */

Sectlist *ps;  /* The sectionlist */
Scursor *sc;   /* The scursor */

/* This function is called by the MOVE_SCURSOR macro, when the scursor
   is at the end of a section.

   It is assumed that the sursor is validly on the sectionlist.
   ie, there is no error checking.
*/
#endif
{

    /* Determine next section in polygon */
    if((sc->s == sc->lastsec) || ((sc->s+1)->start == NULL))
    { /* End of polygon reached */
       sc->s = sc->firstsec;   /* Back to start of polygon */
    }
    else
    { /* Next section in polygon is next in list */
       sc->s++;
    }
    /* Move to start of section */
    sc->p = sc->s->start;
    /* Determine direction */
    if(sc->p > sc->s->end) sc->d = -1;  /* Going backwards */
    else sc->d = 1;                     /* Going forward */

}
/************************************************** POLnoverlap_sl ****/
#ifdef PROTO
Sectlist * POLnoverlap_sl ( Sectlist *ps, Logical possin )
#else
Sectlist * POLnoverlap_sl ( ps, possin )

Sectlist *ps;      /* The section list */
Logical possin;    /* True, if one polygon may be inside another */

/* Joins up overlapping polygons in a polygon set to eliminate overlaps.
   Both the input and returned polygon sets are expressed as
   section lists.
*/
#endif

/* ALGORITHM

   Searches for two overlapping polygons, if found they are joined
   at one of there intersection points.
   This is reapeated until there are no overlaping polygons.

   In each case the joined polygon is appended to the section list,
   and the original unjoined pair are deleted from the section list.
   The section list is made contiguous at the end or if it gets too long.
   This function is hence more efficient if a long section list is
   supplied.

   If there are possibly any polygons inside other polygon, such pairs
   are searched for and joined by a pair of coincident edges inside
   one but not both the polygons.
   On some devices the coincident edges may show up as a narrow gap.
*/
{
   Int poly1, poly2;   /* The two polygons being dealt with */
   Int polymin;        /* The earliest polygon that needs processing */
   Int joincount;      /* Joining count - protect against looping bug */

   Scursor a1, a2;     /* Current place in the two polygons */
   Rect box1, box2;    /* Bounding boxes of the polygons */

   Int inpoly, outpoly;  /* Two polygons one inside the other */
   Int ns1, ns2;         /* number of sections in polygon */
   Long np1, np2;        /* number of vertices in polygon */
   Int nsmax1, nsmax2;   /* maximum number of sections in polygon */
   Int needed;           /* Number of sections needed */
   Logical joined;       /* True, if joining has been done */
   Logical cross;        /* True, if the edges cross */
   Logical coline;       /* True, if they are collinear */
   Point crosspt;        /* The intersection point */
   Point *end1a, *end1b,
         *end2a, *end2b;   /* Ends of edges to be tested for crossing */
   Index ip1,ip2;        /* Loop indices */
   Int   i;              /* Loop index */

#ifdef DEBUG
   DMESS "POLnoverlap_sl: %d polygons\n",ps->npoly);
#endif

   /* Quit, if only one polygon */
   if(ps->npoly < 2) return ps;

   polymin = 1;
   joincount = 0;

   for(joincount=0;joincount<ps->npoly;joincount++)
   {
      joined = FALSE;  /* Set joined to false, to stay in loop */

      /* Find the number of sections in by the two polygons
         with the most sections */
      nsmax1 = 0;
      nsmax2 = 0;
      for(poly1=polymin; poly1<ps->npoly-1; poly1++)
      {
          POLcount_sl (ps, poly1, &ns1, NULL);
          if(ns1 > nsmax1)
          {
             nsmax2 = nsmax1;
             nsmax1 = ns1;
          }
          else
          {
             if(ns1 > nsmax2) nsmax2 = ns1;
          }
      }

      /* Make sure there is enough space in the sectionlist */
      needed = ps->vacant - ps->first + nsmax1 + nsmax2 + 4;
      if(needed > ps->msect) POLcontig_sl(ps);
      needed = ps->vacant - ps->first + nsmax1 + nsmax2 + 4;
      ps = POLgrow_sl(ps,ps->mpoly,needed,
                                ps->freept-ps->newpts +1);
      if(ps == NULL) return NULL;

      /* Search for two overlapping polyons, join them if found */
      for(poly1=polymin; poly1<ps->npoly-1; poly1++)
      {

         POLscursor_sl (ps, poly1, &a1);
         POLcount_sl (ps, poly1, &ns1, &np1);
         POLbbox_sl (ps, np1, a1, &box1);
         for(ip1=0; ip1<np1; ip1++)
         {
            end1a = a1.p;
            MOVE_SCURSOR(ps,a1);
            end1b = a1.p;

            for(poly2=poly1+1; poly2<ps->npoly; poly2++)
            {
               POLscursor_sl (ps, poly2, &a2);
               POLcount_sl (ps, poly2, &ns2, &np2);
               POLbbox_sl (ps, np2, a2, &box2);
               if(!((box1.right < box2.left)|| (box2.right < box1.left)||
                    (box1.top   < box2.bot) || (box2.top   < box1.bot) ))
               {  /* Bounding boxes intersect */
                  for(ip2=0; ip2<np2; ip2++)
                  {
                     end2a = a2.p;
                     MOVE_SCURSOR(ps,a2);
                     end2b = a2.p;

                     /* Do the edges cross ? */
                     cross = GRAcross(end1a,end1b, end2a,end2b,
                                      &coline, &crosspt);
                     if(cross || coline)
                     {
                        POLjoin_sl(ps, &a1, &a2, &crosspt);
                        polymin = poly1;
                        joined = TRUE;
                     }
                     if(joined)break;
                  }
               }
               if(joined)break;
            }
            if(joined)break;
         }
         if(joined)break;
      }
   }

   if(possin && (ps->npoly > 1))
   {  /* One polygon may be entirely inside another */
      for(i=0; i < ps->npoly; i++)
      {
         POLinfind_sl( ps, &inpoly, &outpoly);
         if(inpoly == -1) break;   /* No such pair of polygons found */

         /* Make space for joined polygons */
         POLcount_sl(ps, inpoly, &ns1, &np1);
         POLcount_sl(ps, outpoly, &ns2, &np1);
         needed = ps->vacant - ps->first + ns1 + ns2 + 2;
         if(ps->mpoly < needed) POLcontig_sl(ps);
         needed = ps->vacant - ps->first + ns1 + ns2 + 2;
         ps = POLgrow_sl( ps, ps->mpoly, needed, ps->mpts);
         if(ps == NULL) return NULL;

         /* Join the polygons */
         POLinjoin_sl( ps, np1,inpoly, np2,outpoly);  /* Join the pair */
         if(ps == NULL) return NULL;
      }
   }

   return ps;

}
/************************************************** POLpoly_sl ********/

#ifdef PROTO

Sectlist * POLpoly_sl( Polygonset *pset, Int nsect )
#else

Sectlist * POLpoly_sl( pset, nsect )

/*
Return a polygon section list for a polygon set
with one section per polygon.
*/

Polygonset *pset;  /* Polygon Set */
Int nsect;         /* Number of sections needed */

#endif

{
    Section **firsec;  /* First section of polygon */
    Sectlist *ps;      /* Polygon section list */
    Enum *curfl;       /* Current vertex edge flag */
    Int npoly = 0;     /* Number of polygons */
    Index i;           /* Loop index */

#ifdef DEBUG
    DMESS "POLpoly_sl: %d points, %d sections needed.\n",pset->n,nsect);
#endif

    /* Check validity of edgeflags & count polygons */
    npoly = POLnumber( pset->n, pset->eofl );
    if(npoly == 0) return NULL;  /* Edge out flags Invalid */

#ifdef DEBUG
    DMESS "POLpoly_sl: %d polygons.\n",npoly);
#endif

    /* Estimate number of sections required */
    if(nsect < npoly) nsect = npoly;

    /* Allocate space to section list structure */
    ps = POLgrow_sl(NULL,npoly,nsect,0);
    if(ps == NULL)
    {
      return NULL;
    }

    /* Make the sections */
    ps->vacant = ps->first;       /* Start list at begining */
    firsec = ps->poly;
    *firsec = ps->vacant;
    if(npoly == 1)
    {  /* Single polygon: one section for whole */
       ps->vacant->start = pset->pts;            /* 1st Point */
       ps->vacant->end = pset->pts+pset->n;   /* Last Point */
       ps->vacant++;
    }
    else
    {  /* Polygon Set: One section per polygon */
       ps->vacant->start = pset->pts;   /* 1st Point */
       curfl = pset->eofl;          /* Initialise Current edge flag */
       for(i=0; i<pset->n-1; i++,pset->pts++,curfl++)
       {
          if(*curfl == CLOSEVIS || *curfl == CLOSEINVIS)
          {    /* End of polygon: close section */
               ps->vacant++;                  /* Next section */
               firsec++;                      /* Next polygon */
               if(ps->vacant >= ps->first + ps->msect)
               {  /* Bug: Section List overflow! */
                  CGMerror(func, ERR_NOMEMPSL, ERROR,
                  "- end of section list");
                  POLfree_sl(ps);
                  return NULL;
               }
               *firsec = ps->vacant;
               ps->vacant->start = (ps->vacant-1)->end + 1;

          }                    /* End of if(*curfl...) */
       }                       /* End of for() */
       ps->vacant->end = pset->pts;
       ps->vacant++;
    }                          /* End of if(npoly == 1)else  */
    firsec++;
    *firsec = ps->vacant;
    ps->npoly = firsec - ps->poly;

    return ps;
}
/************************************************** POLpts_sl *********/
#ifdef PROTO
Polygonset *POLpts_sl( Sectlist *ps, Polygonset *space)
#else
Polygonset *POLpts_sl ( ps, space)

/* Convert a polygon set from section list form
   to the standard polygon set form.
*/
Sectlist   *ps;      /* The polygon set in sectionlist form */
Polygonset *space;   /* Space for output, used if sufficient */

/* The sectionlist need not be contiguous */
#endif
{
   Scursor a;        /* Scursor for sectionlist */
   Int ipoly;        /* Current polygon */
   Index i;          /* Loop index - not used inside loop */
   Point *pt;        /* Current vertex */
   Enum  *fl;        /* Current flag */
   Long npp;         /* Number of vertices in polygon */
   Long np;          /* Total number of vertices in all polygons */
   static Polygonset pset;  /* Polygon Set structure to return */


  /* Count the total number vertices */
  np = 0L;
  for(ipoly=0; ipoly < ps->npoly; ipoly++)
  {
     POLcount_sl(ps,ipoly,NULL,&npp); /* Count from current polygon */
     np += npp;                    /* Add to total */
  }

#ifdef DEBUG
   DMESS "POLpts_sl: %d vertices, %d polygons.\n",np,ps->npoly);
#endif

  /* Make sure there is enough space for them */
  if((space == NULL) || (np > space->n))
  {  /* Insufficent space supplied, make space here */
      pset.pts = (Point *)MALLOC(np, sizeof(Point));
      if(pset.pts == NULL)
      {
         exit((int)CGMerror(func, ERR_NOMEMORY, FATAL,"- vertices"));
      }
      if(ps->npoly > 1)
      {  /* More than one polygon, used edge-out flags */
         pset.eofl = (Enum *)MALLOC(np, sizeof(Enum));
         if(pset.eofl == NULL)
         {
            exit((int)CGMerror(func, ERR_NOMEMORY, FATAL,"- flags"));
         }
      }
      else  pset.eofl = NULL;  /* No flags needed */
   }
   else
   {  /* Supplied space is sufficient, use it */
      pset = *space;
   }
   pset.n = np;  /* Put in number of vertices */


   /* Generate polygon set from section list */
   if(ps->npoly == 1)
   { /* Only one polygon */
     pset.eofl = NULL;       /* No edge-out flags needed */
     POLscursor_sl(ps,0,&a);     /* Put scursor on polygon */
     for(i=0, pt=pset.pts; i<np; i++, pt++)
     {
#ifdef DEBUG_PTS
           DMESS "Poly (%d to %d) Sect %d, (%f,%f) \n",
           a.firstsec, a.lastsec, a.s, (a.p)->x, (a.p)->y);
#endif
        *pt = *(a.p);
        MOVE_SCURSOR(ps,a);
     }
   }
   else
   {  /* Multiple polygons */
      for(ipoly=0, pt=pset.pts, fl=pset.eofl; ipoly < ps->npoly;
         ipoly++,pt++,fl++)
      {
        POLscursor_sl(ps,ipoly,&a);   /* Put scursor on current polygon */
        POLcount_sl(ps,ipoly,NULL,&npp); /* Count the vertices */

        for(i=0; i < npp-1; i++, pt++, fl++)
        {
#ifdef DEBUG_PTS
           DMESS "Poly (%d to %d) Sect %d, (%f,%f) IN\n",
           a.firstsec, a.lastsec, a.s, (a.p)->x, (a.p)->y);
#endif
           *pt = *(a.p);
           *fl = INVIS;
           MOVE_SCURSOR(ps,a);
        }
        /* End of polygon reached */
#ifdef DEBUG_PTS
           DMESS "Poly (%d to %d) Sect %d, (%f,%f) CL\n",
           a.firstsec, a.lastsec, a.s, (a.p)->x, (a.p)->y);
#endif
        *pt = *(a.p);
        *fl = CLOSEINVIS;
      }
   }
   return &pset;
}
/************************************************** POLscursor_sl *****/
#ifdef PROTO
void POLscursor_sl (Sectlist *ps, Int ipoly, Scursor *sc)
#else
void POLscursor_sl ( ps, ipoly, sc)

/* Put the scursor on the 1st vertex of a polygon pointing to the 2nd
   vertex in a polygon set expressed as a section list.

   The scursor is a stucture that can be easily moved along the edges
   of polygons in the form of a section list.
*/
Sectlist *ps;     /* The polygon set as a section list */
Int ipoly;        /* The polygon identifier */
Scursor  *sc;     /* The scursor */
#endif

/* If error the scursor pointers are set to NULL */

{
   Section *sec;   /* The current section */

   if((-1<ipoly)&&(ipoly<ps->npoly)&&(ipoly<ps->mpoly))
   {  /* Polygon is in polygon list */
      sc->firstsec = sc->s = *(ps->poly + ipoly);
      sc->lastsec = *(ps->poly + ipoly + 1) -1;
      if(!ps->contig)
      {  /* Sections not contiguous: find true last section */
         for(sec=sc->firstsec; sec<sc->lastsec+1; sec++)
         {
            if(sec->start == NULL)
            {
               sc->lastsec = sec-1;
               break;
            }
         }
      }
      sc->p = sc->s->start;
      if(sc->p < sc->s->end) sc->d = 1;
      else sc->d = -1;
   }
   else
   {  /* Polygon not in polygon list */
      sc->firstsec = sc->lastsec = sc->s = NULL;
      sc->p = NULL;
      sc->d = 0;
#ifdef DEBUG
      DMESS "POLscursor_sl: Polygon %d of %d max %d\n",
      ipoly,ps->npoly,ps->mpoly);
#endif
   }
   return;
}
