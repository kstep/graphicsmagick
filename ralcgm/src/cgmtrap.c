/*   RAL-CGM Interpreter module:  @(#) cgmtrap.c  version 3.7
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
 * Description: Polygon Fill by Trapezoid Decomposition
 *
 *
 * Maintenance Log:
 *  16 Mar 93 KEVP Created and working for simple tests
 *  10 Feb 94 RTP  Move REQUAL etc macros to cgmout.h
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *                 Change type of edge_remove to Logical
 *  20 Jan 94 KEVP In et_validate, corrected expression determining
 *                 whether the two peaks touch or overlap.
 *  24 Jan 94 KEVP In et_validate and touching_peaks, made sure that
 *                 the sharpness is updated when required.
 *   1 Jun 95 KEVP Determine differences by local means rather than by REQUAL etc.
 *
 * Note on Terminology
 *
 *   The singular of edge will be taken to mean a single straight
 *   line segment between two consecutive vertices.
 *
 */

#define CGMTRAP_C

#undef DEBUG
#undef DEBUG_FULL

#include "cgmout.h"
#include "cgmerr.h"
#include "cgmpoly.h"
#include "cgmtrap.h"
#include "cgmelem.h"
#include "cgmpar.h"
#include "cgmpts.h"

/* Fixed Limit to the number trapezoids (protect CPU from bug) */
#define MAXTRAPS 100000

/* Clockwise function for 3 points */
#define CLOCK(a,b,c) ( ((b)->x - (a)->x)*((c)->y - (a)->y)\
                     < ((c)->x - (a)->x)*((b)->y - (a)->y) )

/* Test for equality */
#define TZEQUAL(x,y) ( FABS((x)-(y)) <= mindiff )

/* Definitions */
           /* Status of pass over edge table in et_validate */
#define FINALPASS   0      /* This pass is final */
#define NEWPASS     1      /* Start new pass */
#define ANOTHERPASS 2      /* Do another pass after this one */

/* Internal Typedefs */

/* Edge table entry */
typedef struct tzedge {
   float hix;      /* X-coord of edge top end */
   Point *hiye;    /* Point whose Y-coord is that of the edge top end */
   Point *loe;     /* Bottom end of edge */
   struct tzedge *next;   /* Next edge in edge table */
} Edge;

/* Peak edge in the edge table */
typedef struct {
   Edge  *previous, *first, *second, *left, *right;  /* Explicit Edges */
   Logical sharp;                     /* True if, peak is single point */
} Peakedge;

/* The first (highest) Peak in the edge table */
typedef struct {
   Edge *first, *second, *left, *right;
   Float   left_slope, right_slope;
} Hipeak;


/*  Macros  */

#define PEAKOK(a)    ((a)->first->hiye == (a)->second->hiye)

/* Statics for DEBUG */
#ifdef DEBUG
static Point *psv1 = NULL;   /* First vertex of polygon set */
static Enum *psf1 = NULL;    /* First flag of polygon set */
static char trigger[20] = "No Trapezoid";  /* Trapezoid trigger */
#endif

/* String for Error Messages */
static char *func = "CGMtrap";

/* Minimum difference (used by TZEQUAL) */
static Float mindiff;

/* Other statics */
static Edge *etab;    /* Pointer to edge table */
static Long n_et;     /* Upper bound to number of edges in edge table */
static Hipeak hipk;   /* Highest peak in edge table */

#ifdef PROTO
static void

  /* Truncate the highest peak */
  peak_truncate ( Trapezoid * ),

  /* Deal with pair of touching peak edges */
  touching_peak ( Peakedge *, Peakedge *, Enum *);


static Edge
  /* Make an edge table for trapezoid decomposition */
  * edge_table( Polygonset * );

static Hipeak
  /* Return highest peak in edge table */
  * hi_peak ();

static Logical
  /* Determine, if two sloping edges cross */
  edge_cross ( Edge *, Edge * ),

  /* Remove Edge from Edge Table */
  edge_remove ( Edge * ),

  /* Swap two edges touching at top, if required */
  edge_swap ( Edge *, Edge * ),

  /* Make edge table valid */
  et_validate (),

  /* Remove Horizontal Edges from top of edge table */
  horiz_edge_rm ( Int *),

  /* Extract highest trapezoid from edge table */
  trap_extract( Float, Trapezoid *);

#else
static Logical edge_remove(), peak_truncate(), touching_peak();
static Edge *edge_table();
static Hipeak *hi_peak();
static Logical edge_cross(),  edge_swap(),
               et_validate(), horiz_edge_rm(),
               trap_extract();
#endif

#ifdef DEBUG
static void et_diagnose();
#endif


/***************************************************** TRPfill ********/

#ifdef PROTO
void TRPfill (Polygonset *pset, Int numtz, Float ytol,
              void (*trpdraw) (Int, Trapezoid *) )
#else
void TRPfill ( pset, numtz, ytol, trpdraw )

/*  Fills a polygon(set) using trapezoid decomposition  */

Polygonset *pset;    /* Polygon Set */
Int numtz;           /* Maximum number of trapezoids to be stored */
Float ytol;          /* Y-tolerance for trapezoids */
void (*trpdraw)();   /* Device Trapezoid Filling routine */
#endif

{
   Trapezoid *trap;       /* Trapezoid array */
   Int i;                 /* Loop indices (never reffered to) */
   Trapezoid *curtrap;    /* Current trapezoid */
   Int maxpoly;           /* Maximum number of polygons after clipping */
   Polygonset *clipped,   /* Clipped Polygon(set) */
              copy;       /* Copy of polygon set */
   Rect       cl;         /* Clipping Rectangle */
   Float rytol;           /* Reciprocal of Y-Coord tolerance */
   Float lasttop;         /* Top of last trapezoid (Y-coord) */
   Logical found;         /* True if trapezoid found */
   Logical toluse;        /* True if Y-coord tolerance is in use */
   Long maxdo;            /* Maximum number of trapezoids extracted */

/*    ALGORITHM
 *    The Polygon Set is Clipped (to clipping rectangle or VDC extent).
 *    An Edge-table is constructed.
 *    Trapezoids are extracted from the edge table and stored in array.
 *    When array is full or edge table is empty, trapezoids are drawn.
 *
 *    Based on the RAL GKS Utility, GKFATZ
 */

#ifdef DEBUG  /* Set starts for debug statements */
   DMESS "\f*** Trapezoid decomposition fill utility TRPfill called.***\n");
   if(pset->eofl == NULL)
      DMESS "Single Polygon with %d vertices\n",pset->n);
   else
      DMESS "Polygon Set with %d vertices\n",pset->n);
   if(pset->n < 3) DMESS "TRPfill: Too few vertices %d \n",pset->n);
   if(numtz < 1) DMESS "TRPfill: Too few trapezoids to be stored %d \n",
                 numtz);
   else DMESS "TRPfill: Max number of Trapezoids %d\n", numtz);
#endif
   if(pset->n < 3)return; /* It must have at least 3 vertices */
   if(numtz < 1)return;   /* It must store at least one trapezoid */

   mindiff = cur.vdcmin*0.01; /* Initialize minimum difference */

   PTSclipget (NULL, &cl);    /* Get Clipping Rectangle */

   /* Clip the polygon(set) without joining overlapping polygons */
   clipped = POLhandle ( pset, &cl, TRUE, NULL );
   if(clipped == pset)
   {  /* Clipped polygon(set) same as unclipped, make copy */
#ifdef DEBUG
      DMESS "TRPfill: No clipping needed. Copy of polygon(set) used.\n");
#endif
      copy.n = pset->n;
      copy.pts = ( Point *)MALLOC(pset->n, sizeof( Point));
      if(copy.pts == NULL)
         exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, "- pts"));
      for(i=0;i<pset->n;i++) copy.pts[i] = pset->pts[i];

      if(pset->eofl != NULL)
      { /* Make copy of edge-out flags too */
         copy.eofl = ( Enum *)MALLOC(pset->n, sizeof( Enum));
         if(copy.eofl == NULL)
            exit((int)CGMerror(func,ERR_NOMEMORY, FATAL,"- flags"));
         for(i=0;i<pset->n;i++) copy.eofl[i] = pset->eofl[i];
      }
      else copy.eofl = NULL;
      /* Use the copy for trap_extract, to preserve the original */
      clipped = &copy;
   }
   else if(clipped->n == 0) return;


   if(clipped != NULL)
   {  /* Clipping went OK, proceed with decomposition */
#ifdef DEBUG
      psv1 = clipped->pts;
      psf1 = clipped->eofl;
#endif

      /* Make the edge table */
      etab = edge_table (clipped);

#ifdef DEBUG
      DMESS "\n    Newly created edge table\n");
      et_diagnose();   /* Examine the edge table */
#endif

      /* Make space for the trapezoids */
      trap = (Trapezoid *)MALLOC( numtz, sizeof( Trapezoid));
      if(trap == NULL)
         exit((int)CGMerror(func,ERR_NOMEMORY,FATAL,
              "- too many trapezoids to store"));

      /* Set variables for loop */
      curtrap = trap;
      toluse = !TZEQUAL(ytol,0.0);
      if(toluse) rytol = 1.0/ytol;
      maxdo = (clipped->n)*(clipped->n);
      if(maxdo > MAXTRAPS) maxdo = MAXTRAPS;
#ifdef DEBUG
      if(clipped->eofl == NULL)
 DMESS "TRPfill: Clipped polygon(set) is single with %d vertices.\n"
       ,clipped->n);
      else
 DMESS "TRPfill: Clipped polygon(set) is multiple with %d vertices.\n"
       ,clipped->n);
      if (toluse) DMESS "Trapezoids less than %f ignored\n", ytol);
      else        DMESS "All trapezoids sent\n");
#endif

      /* Loop over trapezoids */
      for(i=0; i<maxdo; i++)
      {
         /* Extract trapezoid */
         found = trap_extract( cl.bot, curtrap);

         if(found)
         {  /* Trapezoid found, check its level */
            if((curtrap > trap) && (lasttop < curtrap->top))
            {
               CGMerror(func, ERR_BADEDGETAB, ERROR,
                        "- higher trapezoid");
               break;
            }
            lasttop = curtrap->top;
            /* Check its height: ignore it, if within tolerance */
            if(toluse)
            {  /* Round Y-coords to units of tolerance. */
               curtrap->top = ytol*((Int)(rytol*curtrap->top));
               curtrap->base = ytol*((Int)(rytol*curtrap->base));
               /* Keep trapezoid, if it has non-zero depth */
               if(curtrap->top > curtrap->base) curtrap++;
            }
            else
            {  /* Tolerance not in use, keep trapezoid */
               curtrap++;
            }
            if(curtrap-trap > numtz-1)
            { /* Array of trapezoids is full, draw them. */
               trpdraw (numtz, trap);
               curtrap = trap;
#ifdef DEBUG
               DMESS "TRPfill: %d trapezoids drawn\n",numtz);
#endif
            }
         }
         else
         {  /* Trapezoid not found, quit loop */

#ifdef DEBUG
   DMESS "TRPfill: Trapezoid %d not found, decomposition terminated.\n",
            i+1);
            if(etab->next != NULL)
            {  /* Edge table not empty */
              DMESS "TRPfill: Trapezoid top %f, base %f\n",
              curtrap->top,curtrap->base);
              DMESS "TRPfill: trigger: %s\n",trigger);
            }
#endif
            if(etab->next != NULL)
               CGMerror (func,ERR_BADEDGETAB,ERROR,NULLSTR);
            break;
         }
#ifdef DEBUG
         if(i == numtz*(i/numtz)) DMESS "TRPfill: trapezoid %d\n",i+1);
#endif

      }  /* End of loop over trapezoids */

      FREE(etab);            /* Edge Table no longer needed */

#ifdef DEBUG
      DMESS "TRPfill: %d trapezoids extracted,",i);
      DMESS " %d undrawn trapezoids\n",curtrap-trap);
#endif

      /* Draw any remaining trapezoids */
      if(curtrap > trap) trpdraw(curtrap-trap,trap);

      FREE(trap);            /* Trapezoids no longer needed */

      FREE(clipped->pts);    /* Clipped polygon(set) no longer needed */
      if( clipped->eofl != NULL ) FREE(clipped->eofl);

   }
#ifdef DEBUG
   else
   {  /* Error in clipping */
      DMESS "TRPfill: error from clipping polygon \n");
   }
#endif

   return;
}

/******************************************************* edge_cross ***/
#ifdef PROTO
static Logical edge_cross ( Edge *le, Edge *re )
#else
static Logical edge_cross ( le, re )

/* Determines if an edge from the left crosses one from the right
 * when the top end of one edge is on the other edge
 * and returns answer.
 */
Edge *le;      /* The edge from left */
Edge *re;      /* The edge from right */
#endif

/* ALGORITHM
 * The slopes of the two edges are compared, taking into account
 * the possibility of horizontal edges.
 *
 * The top of one edge is assumed to be on the other edge.
 * It does not matter which.
 */
{
   Logical left_horiz, right_horiz;  /* True if edge is horizontal */
   Float  left_slope, right_slope;   /* Slopes of edge */
   Logical cross;                    /* True if edges cross */

#ifdef DEBUG_FULL
   DMESS "\nedge_cross start");
#endif

   left_horiz  = VEQUAL(le->hiye->y,le->loe->y);
   right_horiz = VEQUAL(re->hiye->y,re->loe->y);

   if(left_horiz)
   {
      if(right_horiz)
      { /* Both edges horizontal */
         cross = ( le->loe->x > re->loe->x );
      }
      else
      {  /* Left edge horizontal and Right edge sloping */
         cross = ( le->hix < le->loe->x );
      }
   }
   else
   {
      if(right_horiz)
      { /* Left edge sloping and right edge horizontal */
         cross = ( re->hix > re->loe->x );
      }
      else
      { /* Both edges sloping, compare slopes to decide */
         left_slope  = (le->loe->x - le->hix)/(le->hiye->y - le->loe->y);
         right_slope = (re->loe->x - re->hix)/(re->hiye->y - re->loe->y);
         cross = (left_slope > right_slope);
      }
   }
   return cross;
}

/**************************************************** edge_remove *****/
#ifdef PROTO
static Logical edge_remove( Edge *rem )
#else
static Logical edge_remove ( rem )
/*
 *  Remove an edge truncated to zero length from the edge table
 *  while preserving the attached implicit peak edge
 *  Returns whether the edge is a trough
 */
Edge *rem;  /* Edge to be removed */
#endif

/* ALGORITHM
   Find the edge below the one to be removed
   copy it onto the edge to be removed (except ->hiye)
   and bypass edge below edge table.
   If no edge below is found,
   then truncate the edge to be removed to a single point
   and leave it.
 */
{
   Edge *below;          /* The edge below the one to be removed */
   Edge *prebelow;       /* The edge preceeding the edge below */
   Logical trough=TRUE;  /* True if edge bottom is a trough */
   Index i;              /* Loop index */

#ifdef DEBUG_FULL
   DMESS "\nedge_remove start\n");
#endif

   /* Find the edge below */
   below = etab;
   for(i=0; i<n_et+1; i++)
   {
      prebelow = below;
      below = below->next;
      if(below == NULL) break;
      if(rem->loe == below->hiye)
      { /* Edge below has been found */
         trough = FALSE;
         break;
      }
   }

   if(trough)
   {  /* No edge below found, bottom must be a trough */
      /* Truncate it to single point and leave it */
      rem->hix = rem->loe->x;
      /* This rump edge will be dealt by touching_edge in et_validate. */
#ifdef DEBUG_FULL
      DMESS "edge_remove: rump edge %d (%d to %d) left at trough.\n",
            rem-etab,rem->hiye-psv1,rem->loe-psv1);
#endif
   }
   else
   {  /* Copy up edge below, except for top y vertex */
#ifdef DEBUG_FULL
      DMESS "edge_remove: removing edge %d (%d to %d)",rem-etab,
             rem->hiye-psv1,rem->loe-psv1);
      DMESS "down to %f.\n",rem->loe->y);
#endif
      rem->hix = below->hix;
      rem->loe = below->loe;
     /* Cut edge below out of edge table */
      prebelow->next = below->next;
    /* The replacement edge is already correctly placed in the
       edge table, owing to the truncation of the edge to be removed */
   }

   return trough;
}

/******************************************************* edge_swap ****/
#ifdef PROTO
static Logical edge_swap ( Edge *le, Edge *re )
#else
static Logical edge_swap ( le, re )

/* Swap two edges with tops at same level, if necessary,
 * to ensure that the left edge goes down left of the right edge
 * and return TRUE if the edges were swapped.
 */
Edge *le;      /* The left edge */
Edge *re;      /* The right edge */
#endif

/* ALGORITHM
 * The top ends of the edges are assumed to be level
 *
 * Determine if the edges need swapping (ie the left crosses right )
 *
 * Then swap them if necessary
 */
{
   Logical swap;      /* True, if edges need swapping */
   Point *loeswap;    /* Temporary for swapping bottom points */
   float  hixswap;    /* Temporary for swapping top x-coords */

#ifdef DEBUG_FULL
   DMESS "edge_swap: before %d (%d, %d) with %d (%d, %d) level %f\n",
         le-etab,le->hiye-psv1,le->loe-psv1,
         re-etab,re->hiye-psv1,re->loe-psv1,
         le->hiye->y);
#endif

   if(TZEQUAL(le->hix,re->hix))
   {  /* The two edges touch at the top */
      swap = edge_cross( le, re );
   }
   else swap = (le->hix > re->hix);

   if(swap)
   {  /* Edges need swapping, swap them */
      loeswap = le->loe;
      le->loe = re->loe;
      re->loe = loeswap;
      hixswap = le->hix;
      le->hix = re->hix;
      re->hix = hixswap;
     /* NB: upper y-vertex (->hiye) is unchanged */
   }
#ifdef DEBUG_FULL
   DMESS "edge_swap: after  %d (%d, %d) with %d (%d, %d)\n",
         le-etab,le->hiye-psv1,le->loe-psv1,
         re-etab,re->hiye-psv1,re->loe-psv1);
#endif
   return swap;
}

/***************************************************** edge_table ******/

#ifdef PROTO
static Edge * edge_table( Polygonset *pset )
#else
static Edge * edge_table( pset )

/* Makes a sorted edge table for trapezoid decomposition
   of a polygon(set)
   and return its first (highest edge) */

Polygonset *pset;     /* The polygon(set) */

#endif
/*     ALGORITHM
 *     Constructs a sorted Edge Table from the supplied polygon set.
 *     Each in the table edge consists of
 *          (loe)  pointer to edge-bottom vertex
 *          (hiye) pointer to vertex with same Y-coord as edge-top
 *          (hix)  X-coord of edge-top
 *          (next) edge index of next edge down
 *     All edges are included (including horizontal), when the
 *     edge table is made. However, a vertex that appears in the top
 *     of two edges (as hiye) may be later converted into a horizontal
 *     edge, which would not be included, but is implicit from the two
 *     edges having the same hiye element.
 *     Such an edge is referred to as a peak edge.
 */

{
   Point *bot,         /* Bottom vertex of edge */
         *next,        /* Next vertex in Polygon*/
         *polystart,   /* First vertex in polygon */
         *top,         /* Top vertex of edge */
         *curpt;       /* Current vertex */

   Enum *curfl=NULL;   /* Current flag */

   Index i,j;          /* Loop indices */

   Edge *e_poss,     /* Possible edge */
          *e_lastposs, /* Last possible edge */
          *e_new;      /* New edge */

   Index vertex;       /* Current Vertex Index */

   register Logical polyset; /* True, if polygon set */

#ifdef DEBUG_FULL
   Index le, ne, pe;    /* Edge Indices */

   DMESS "edge_table begin with %d vertex polygon(set)\n",pset->n);
#endif

   /* Make space for edge table */
   etab = (Edge *)MALLOC(pset->n+1, sizeof( Edge));
   if(etab == NULL)
      exit((int)CGMerror(func,ERR_NOMEMORY, FATAL,"- edge table"));

   etab->next = NULL; /* Start with an empty edge table */
   n_et = 0;
   e_new = etab+1;    /* First new edge */

   polystart = pset->pts;
   polyset = (pset->eofl != NULL);
   if(polyset)curfl=pset->eofl;
   for(vertex=0,curpt=pset->pts; vertex<pset->n; vertex++, curpt++)
   {    /* Find next vertex */
      if(polyset)
      {
         switch(*curfl)
         {
            case VIS:  case INVIS:
              next = curpt+1;       /* Next vertex in same polygon */
              break;
            case CLOSEVIS: case CLOSEINVIS:
              next = polystart;     /* Next vertex first of polygon */
              polystart = curpt+1;  /* Move start to next polygon */
              break;
            default:
#ifdef DEBUG_FULL
              DMESS "edge_table: Invalid edge out flag %d at vertex %d\n",
              *curfl,curfl-pset->eofl);
#endif
              next = NULL;
              break;
         }
         if(next == NULL)break;   /* Invalid flag */
         curfl++;                 /* Next flag */
      }
      else
      { /* Only one Polygon */
         if(vertex == pset->n-1) next = polystart;
         else                    next = curpt+1;
      }

#ifdef DEBUG_FULL
      DMESS "edge_table: vertex %d, edge %d to %d \n",
            vertex,curpt-psv1,next-psv1);
      DMESS "            coords (%f,%f) to (%f,%f)\n",
            curpt->x,curpt->y,next->x, next->y);
#endif

      /* Find higher end of edge */
      if(curpt->y > next->y)
      {  /* downward edge */
         top = curpt; bot = next;
      }
      else
      {  /* upward edge */
         top = next; bot = curpt;
      }

      /* Now find a home for it */
      e_lastposs = etab;
      e_poss = etab->next;

      for(j=0; j<n_et; j++)   /* Loop over edge table */
      {

        if(e_poss == NULL) break;
        if(e_poss->hiye->y <= top->y)  /* Sort on higher top-vertex */
        {
           /* Sort on earlier top-vertex (same vertices together) */
           if((e_poss->hiye->y < top->y) || (e_poss->hiye >= top))
           { /* Home found */
              break;
           }
        }
        e_lastposs = e_poss;
        e_poss = e_lastposs->next;
      }
#ifdef DEBUG_FULL
      le = e_lastposs - etab;
      if(e_poss == NULL) pe = -1;
      else               pe = e_poss - etab;
      ne = e_new - etab;
      DMESS "            last %d, new %d, next %d, top %d, bottom %d\n",le,ne,pe,
                         top-psv1, bot-psv1);
#endif

      /* Insert new edge before e_poss */
      n_et++;
      e_new->hix  = top->x;
      e_new->hiye = top;
      e_new->loe  = bot;
      e_new->next = e_poss;

      /* Insert new edge after e_lastposs */
      e_lastposs->next = e_new;

      e_new++;  /* Next new edge */
   }     /* End for */

#ifdef DEBUG
   DMESS "edge_table complete with %d edges\n",n_et);
#endif

   return etab;
}

/***************************************************** et_validate *****/
#ifdef PROTO
static Logical et_validate ()
#else
static Logical et_validate ()

/* Manipulate the edge table to ensure that the
   edges forming peaks are next to each other.
   Then the implicit peak edges do not overlap, thereby preventing
   trapezoids from overlapping.
   Return TRUE, if corrupt edge table is detected.
*/
#endif

/* ALGORITHM
 *
 *   If two implicit peak edges are found to touch or overlap,
 *   swap explicit edges or merge as required.
 *   Repeat till all implicit peak edges are separate.
 */
{
   Enum pass;           /* Status of this pass of the edge table */
   Logical a_dir_q;     /* True, if the direction of peak A is unknown */
   Peakedge a, b;       /* Peak edges A and B */
   Index j0,j1,j2;      /* Loop indices */

#ifdef DEBUG_FULL
   DMESS "\net_validate start\n");
#endif
   for(j0=0; j0<n_et; j0++)
   {  /* We may have to do several passes of the edge table */
      pass = FINALPASS;         /* Assume final pass, till contrary */
      a.sharp = FALSE;
      a.first  = etab;
      a.second = etab->next;
      if(a.second != NULL)
      {  /* Non-empty edge table */
         for(j1=0; j1<n_et+1; j1++)
         {  /* Look for peak edge A */
            a.previous = a.first;  /* Next possible peak edge */
            a.first    = a.second;
            a.second   = a.second->next;
            if(a.first == NULL) break;   /* Quit loop at end of */
            if(a.second == NULL) break;  /* edge table */
            if(a.first->hiye == a.second->hiye)
            {  /* Peak edge found, call it A */
#ifdef DEBUG_FULL
               DMESS "et_validate: Pass %d Peak A at %d level %f\n",
               j0+1, a.first->hiye-psv1, a.first->hiye->y);
#endif
               a_dir_q = TRUE;    /* Direction of A unknown */
               b.first = a.second;
               b.second = a.second->next;
               if(b.second == NULL) break;
               for(j2=0; j2<n_et+1; j2++)
               { /* Look for another peak edge at the same level */
                  b.previous = b.first; /* Next poss' peak edge */
                  b.first    = b.second;
                  b.second   = b.second->next;
                  if(b.second == NULL) break;
                  if(b.first->hiye == b.second->hiye)
                  {   /* Peak found, call it B */
                     if(TZEQUAL(a.first->hiye->y,b.first->hiye->y))
                     {  /* Peak A and B at same level */
#ifdef DEBUG_FULL
       DMESS "et_validate: Peaks A at %d & B at %d, level %f\n",
       a.first->hiye-psv1, b.first->hiye-psv1, b.first->hiye->y);
#endif
                        if(a_dir_q)
                        {
                           if(a.first->hix < a.second->hix)
                           {  /* 1st end of A left of 2nd */
                              a.left  = a.first;
                              a.right = a.second;
                           }
                           else
                           {  /* 2nd end of A left of 1st */
                              a.left  = a.second;
                              a.right = a.first;
                           }
                           a.sharp = TZEQUAL(a.first->hix,a.second->hix);
                           a_dir_q = FALSE;
                        }
                        if(b.first->hix < b.second->hix)
                        {  /* 1st end of B left of 2nd */
                           b.left  = b.first;
                           b.right = b.second;
                        }
                        else
                        {  /* 2nd end of B left of 1st */
                           b.left  = b.second;
                           b.right = b.first;
                        }
                        b.sharp = TZEQUAL(b.first->hix,b.second->hix);

#ifdef DEBUG_FULL
                        DMESS "et_validate: A %d %f:%f, B %d %f:%f\n",
               a.first->hiye-psv1,a.left->hix,a.right->hix,
               b.first->hiye-psv1,b.left->hix,b.right->hix);
#endif
                        if((b.left->hix < a.right->hix + mindiff) &&
                           (a.left->hix < b.right->hix + mindiff))
                        {  /* Peak edges A & B overlap or touch */
                          if(a.right->hix < b.left->hix + mindiff)
                          { /* A's right touches B's left */
                             touching_peak( &a, &b, &pass);
                             if(pass == NEWPASS) break;
                          }
                          else
                          {
                            if(b.right->hix < a.left->hix + mindiff)
                            { /* B's right touches A's left */
                               touching_peak( &b, &a, &pass);
                               if(pass == NEWPASS) break;
                            }
                            else
                            {   /* A and B overlap */
                               if(a.left->hix < b.left->hix)
                               { /* Overlap is from B-right to A-left */
                                  if(edge_swap( b.right, a.left))
                                  {
                                     a.sharp = TZEQUAL(a.left->hix,a.right->hix);
                                     b.sharp = TZEQUAL(b.left->hix,b.right->hix);
                                     pass = ANOTHERPASS;
                                  }
                               }
                               else
                               { /* Overlap is from A-right to B-left */
                                  if(edge_swap( a.right, b.left))
                                  {
                                     a.sharp = TZEQUAL(a.left->hix,a.right->hix);
                                     b.sharp = TZEQUAL(b.left->hix,b.right->hix);
                                     pass = ANOTHERPASS;
                                  }
                               }
                            }  /* End if B's right touching .. */
                          }    /* End if A's right touching .. */
                        }      /* End if peak edges overlap or touch */
                        if((a.sharp) && (a.left->loe == a.right->loe))
                        { /* Peak A degenerate, remove it */
                           a.previous->next = a.second->next;
                           pass = NEWPASS;
                        }
                        if((b.sharp) && (b.left->loe == b.right->loe))
                        { /* Peak B degenerate, remove it */
                           b.previous->next = b.second->next;
                           pass = NEWPASS;
                        }
                     }
                     else break;  /* Second peak edge lower, quit loop */
                  }    /* End if peak B */
               }       /* End of loop looking for peak edge B */
               if(pass == NEWPASS) break;
            }          /* End if peak A */
         }             /* End of loop looking for peak edge A */
      }                /* End if second edge of A is in edge table */
      if(pass == FINALPASS) break;  /* Quit loop is pass is final */
   }                   /* End loop over passes */
#ifdef DEBUG_FULL
   DMESS "\net_validate: completed\n");
   et_diagnose();
#endif
   return FALSE;
}

/***************************************************** hi_peak *********/
#ifdef PROTO
static Hipeak * hi_peak()
#else
static Hipeak * hi_peak ()

/* Return the highest peak in the edge table */

#endif

/* ALGORITHM
 */
{
   Edge *side[2];             /* The sides of the peak */
   Float slope[2];            /* The slopes of the sides */
   Int removed;               /* Number of horizontal edges removed */
   Logical side0_side1;       /* Side 0 is left of side 1 */
   Index i;                   /* Loop index */
   Float depth;               /* Depth of edge (end level difference) */

#ifdef DEBUG_FULL
   DMESS "\nhi_peak start");
#endif

   /* Check that edge table has at least two edges */
   if(etab->next == NULL) return NULL;
   if(etab->next->next == NULL) return NULL;

   /* Get the sides */
   side[0] = etab->next;
   side[1] = side[0]->next;

   /* Remove horizontal edges from top */
   if( horiz_edge_rm( &removed ) )
   {  /* Pair of sloping edges found at top of edge table */
      if(removed > 0)
      {  /* Sort edge table to bring peaks together */
         if(et_validate()) return NULL;
         if(etab->next == NULL) return NULL;
         if(etab->next->next == NULL) return NULL;
      }

      /* The highest peak should be the first two edges */
      /* Get the sides */
      side[0] = etab->next;
      side[1] = side[0]->next;

   }
   else return NULL;

   /* Check that it is indeed a peak */
   if(side[0]->hiye != side[1]->hiye)
   { /* Error corrupt edge table */
      CGMerror (func,ERR_BADEDGETAB,ERROR,"- should find peak");
      return NULL;
   }

   /* Determine the slopes of the two edges */
   for(i=0; i<2; i++)
   {    /* For each of the two edges */
      depth = side[i]->hiye->y - side[i]->loe->y;
      if(depth > mindiff)
         slope[i] = (side[i]->loe->x - side[i]->hix)/depth;
      else
      {   /* Ah! but the trapezoid could be very very small !!! */
#ifdef DEBUG
         DMESS "\nhi_peak: edge %d not really sloping, top %d base %d\n",
               side[i]-etab,
               side[i]->hiye-psv1,
               side[i]->loe-psv1);
         et_diagnose();
#endif
         return NULL;
      }
   }

   /* Determine left and right */
   if(side[0]->hix+mindiff  < side[1]->hix)
   { /* Top 0 left of top 1 */
      side0_side1 = TRUE;
   }
   else
   {
      if(side[1]->hix+mindiff  < side[0]->hix)
      {  /* Top 1 left of top 0 */
         side0_side1 = FALSE;
      }
      else
      {  /* Tops coincide */
         side0_side1 = (slope[0] < slope[1] );
      }
   }

   /* Put data into peak */
   hipk.first = side[0];
   hipk.second = side[1];
   if(side0_side1)
   { /* Side 0 left of side 1 */
      hipk.left  = side[0];
      hipk.right = side[1];
      hipk.left_slope  = slope[0];
      hipk.right_slope = slope[1];
   }
   else
   { /* Side 1 left of side 0 */
      hipk.left  = side[1];
      hipk.right = side[0];
      hipk.left_slope  = slope[1];
      hipk.right_slope = slope[0];
   }

   return &hipk;
}

/***************************************************** horiz_edge_rm ***/
#ifdef PROTO
static Logical horiz_edge_rm ( Int *rm )
#else
static Logical horiz_edge_rm ( rm )

/* Remove any horizontal edges from the top of the edge table */
/* Return ( at least one pair of sloping edges found ) */

Int *rm;      /* Number of horizontal edges removed */
#endif
{
   Edge *pk0, *pk1, *pk2;     /* Possible Peak */
   Edge *side;                /* current side of peak */
   Edge *edge;                /* current edge */
   Point *bot;                /* Bottom end of edge */
   Index i, j1, j2;           /* Loop indices */
   Float top_y;               /* Level of top */
   Int nslope=0;              /* Number of sloping edges found */
   Int npeak=0;               /* Number of peaks found */
   Logical pkreset=FALSE;     /* True if peak has been reset */

   *rm = 0;  /* Initially no edges have been removed */

#ifdef DEBUG_FULL
   DMESS "\nhoriz_edge_rm start");
   et_diagnose();
#endif

   if(etab->next == NULL)return FALSE;  /* Check for empty edge table */

   pk1 = etab;
   pk2 = pk1->next;
   top_y = pk1->next->hiye->y;

   for(j2=0; j2<n_et; j2++)
   {
      /* Find next peak */
      pk0 = pk1; pk1 = pk2;
      if(pk1 == NULL) break;       /* If edge table end, quit loop */
      pk2 = pk1->next;          /* Next edge */
      if(pk2 == NULL) break;       /* If edge table end, quit loop */
#ifdef DEBUG_FULL
      DMESS "horiz_edge_rm: edge1 %d (%d to %d) level %f top %f\n",
         pk1-etab, pk1->hiye-psv1, pk1->loe-psv1, pk1->hiye->y, top_y);
      DMESS "               edge2 %d (%d to %d) level %f\n",
         pk2-etab, pk2->hiye-psv1, pk2->loe-psv1, pk2->hiye->y);
#endif
      if(top_y > pk2->hiye->y)
      { /* New edge not at top of edge table */
#ifdef DEBUG_FULL
         DMESS "horiz_edge_rm: non-top edge\n");
#endif
         if(nslope > 0) break;   /* If sloping edge found, quit loop */
         else
         { /* Top of edge table completely removed, down to new top */
            top_y = pk1->hiye->y;
#ifdef DEBUG_FULL
            DMESS "horiz_edge_rm: New top, level %f\n",top_y);
#endif
         }
      }
      if(pk1->hiye == pk2->hiye)
      {  /* Peak found */
         npeak++;               /* Count the peak */
         side = pk1;
         pkreset = FALSE;
         for(i=0; i<2; i++)
         { /* For each side of the peak */
           /* Remove all horizontal edges till, sloping edge or trough */
           for(j1=0,edge=side; j1<n_et; j1++)
           {
#ifdef DEBUG_FULL
             DMESS "horiz_edge_rm: side %d of peak at %d level %f\n",
                    i+1,edge->hiye-psv1,edge->hiye->y);
#endif
             bot=edge->loe;
             if(VEQUAL(edge->hiye->y,edge->loe->y))
             { /* Edge horizontal, remove it */
                (void) edge_remove( edge );
                (*rm)++;                        /* Count removed edge */
                if(edge->loe == bot)
                {  /* Trough reached */
#ifdef DEBUG_FULL
                  DMESS "horiz_edge_rm: trough at vertex %d\n",bot-psv1);
#endif
                   if(pk1->loe == pk2->loe)
                   {  /* Trough connects peak, remove peak completely */
#ifdef DEBUG_FULL
DMESS "horiz_edge_em: peak at %d removed completely.\n",pk1->hiye-psv1);
#endif
                      pk0->next = pk2->next;
                      (*rm)++;       /* Count the other removed edge */
                      /* Reset peak */
                      pk1 = pk0;
                      pk2 = pk1->next;
                      pkreset = TRUE;   /* Break from two loops */
                   }
                   else pkreset = FALSE;
                   break;   /* Quit loop */
                }     /* End if trough reached */
             }
             else
             {  /* Edge is sloping */
                nslope++;
#ifdef DEBUG_FULL
   DMESS "horiz_edge_rm: sloping edge %d found %d (%d to %d)\n",
          nslope,edge-etab, edge->hiye-psv1,edge->loe-psv1);
#endif
                break;
             }        /* End if edge is horizontal/sloping */
           }          /* End loop over edges */

           if(pkreset) break;  /* Quit loop, if peak reset */
           side = pk2;         /* Move to other side */
         }            /* End loop over sides */
      }               /* End if peak found */
   }                  /* End loop over possible peaks */

   if(npeak < 1)
   {  /* No peaks found, corrupt edge-table */
      CGMerror (func, ERR_BADEDGETAB, ERROR,"- bad horizontal edges");
      return FALSE;
   }

#ifdef DEBUG_FULL
   DMESS "horiz_edge_rm: %d edges removed\n", *rm );
/*
etab->next->hiye->y);
*/
   if(!((nslope > 1) && (nslope == 2*(nslope/2))) )
   {
       DMESS "\nhoriz_edge_rm: %d slopes and %d peaks found!\n",
              nslope,npeak);
       et_diagnose();
   }
#endif

   /* Check, that number of sloping edges found is valid */
   return ((nslope > 1) && (nslope == 2*(nslope/2)));
}

/***************************************************** peak_truncate ***/
#ifdef PROTO
static void peak_truncate( Trapezoid *tz )
#else
static void peak_truncate ( tz )
/*
 * Truncate the highest peak down to the base of
 * the trapezoid being extracted
 */
Trapezoid *tz;  /* Trapezoid being extracted */
#endif

/* ALGORITHM
 */
{
    Edge *edge;   /* Current edge */
    Edge *prev;   /* Previous edge */
    Index i;      /* Loop index */

#ifdef DEBUG_FULL
   DMESS "\npeak_truncate start");
#endif

   if(tz->base < hipk.left->hiye->y)
   {  /* Truncation is below peak */
      etab->next = hipk.second->next;  /* Temporarily remove peak */

      edge = etab;
      /* Search through edges to find place for truncated peak */
      for(i=0; i<n_et; i++)
      {
         prev = edge;
         edge = edge->next;
         if(edge == NULL) break;  /* Quit if at end of edge table */
         /* Pass higher Y, */
         if(edge->hiye->y <= tz->base)
         { /* but not lower Y */
            if(edge->hiye->y < tz->base) break;
            /* Else, pass earlier top vertex */
            if(edge->hiye >= hipk.left->hiye) break;
         }
      }  /* End loop over edge table */

      /* Put peak back into edge table at new place */
      prev->next = hipk.first;
      hipk.second->next = edge;

#ifdef DEBUG_FULL
      DMESS "\npeak_truncate: %d (now %d %d)", prev-etab,  hipk.first-etab,  edge-etab );
      if ( prev != etab) DMESS " (%d to %d)",  prev->hiye-psv1, prev->loe-psv1);
      DMESS " peak at %d,", hipk.left->hiye-psv1 );
      if ( edge != NULL )
         DMESS " %d ( %d to %d)", edge-etab,edge->hiye-psv1,edge->loe-psv1);
      if(prev != etab) et_diagnose(); else DMESS "\n");
      et_diagnose();
#endif

      /* Truncate edges of peak */
      hipk.first->hiye->y  = tz->base;
      hipk.left->hix  = tz->base_left;
      hipk.right->hix = tz->base_right;
   }
   /* Else base too high, do nothing */

   return;
}

/*********************************************** touching_peak *********/
#ifdef PROTO
static void touching_peak ( Peakedge *lp, Peakedge *rp, Enum *pass)
#else
static void touching_peak ( lp, rp, pass )

/* Deal with touching peak edges and return TRUE if error */

Peakedge *lp;  /* The left touching peak edge */
Peakedge *rp;  /* The right touching peak edge */
Enum  *pass;   /* Pass status */
#endif

/* ALGORITHM
 *  Either joins the two implicit peak edges together
 *  or swaps the touching explicit edges as needed
 *  to ensure that the peak edges, hence trapezoids do not overlap.
 */
{
   Edge *eswap;   /* Temporary pointer for swapping */

#ifdef DEBUG_FULL
   DMESS "touching_peak: left peak at %d and right peak at %d\n",
         lp->right->hiye-psv1,
         rp->left->hiye-psv1);
#endif
   /* First find out if they need joining */
   if(lp->sharp)
   {  /* Left Peak is a single point */
      if(lp->left->loe == rp->left->loe )
      { /* left sides share bottom end, swap sides of left peak */
         eswap     = lp->left;
         lp->left  = lp->right;
         lp->right = eswap;
      }
      else
      {
         if(rp->sharp)
         { /* Both peaks are a single points */
            if(lp->left->loe == rp->right->loe )
            {  /* outer sides share bottom end, swap sides to inner */
               eswap = lp->left;
               lp->left = lp->right;
               lp->right = eswap;
               eswap = rp->left;
               rp->left = rp->right;
               rp->right = eswap;
            }
         }
      }
   }  /* End if peak A single point */
   if(rp->sharp)
   { /* Right peak B a single point */
     if(lp->right->loe == rp->right->loe)
     { /* Both right sides share bottom end, swap sides of right peak */
        eswap = rp->left;
        rp->left = rp->right;
        rp->right = eswap;
     }
   }
   if(lp->right->loe == rp->left->loe)
   {  /* The peak edges need joining, join them */
      lp->right->hix = rp->right->hix;  /* Copy across right top-x */
      lp->right->loe = rp->right->loe;  /* Copy across bottom end */
      if(lp->sharp) lp->sharp = rp->sharp;  /* sharp, only if both were sharp */
#ifdef DEBUG_FULL
      DMESS "touching_peaks: remove peak at %d.\n",rp->left->hiye-psv1);
#endif
      rp->previous->next = rp->second->next;    /* Remove right peak */
      *pass = NEWPASS;
   }
   else
   { /* Swap two edges, if necessary */
      if(edge_swap(lp->right,rp->left))
      {
         lp->sharp = TZEQUAL(lp->left->hix,lp->right->hix);
         rp->sharp = TZEQUAL(rp->left->hix,rp->right->hix);
         *pass = ANOTHERPASS;
      }
   }
   return;  /* No error detected */
}

/***************************************************** trap_extract ****/
#ifdef PROTO
static Logical trap_extract( Float ymin, Trapezoid *tz )
#else
static Logical trap_extract( ymin, tz )

Float ymin;    /* Base of clipping rectangle or VDC extent (minimum Y) */
Trapezoid *tz; /* Place for the trapezoid */

/* ALGORITHM
 */
#endif
{
   Hipeak * hpeak;             /* The highest peak */
   Long n_old, n_new;          /* Number of edges in old & new table */
   Edge old[4];                /* Copy of first 4 edges of old table */
   Edge *new[4];               /* First 4 edges of new table */
   Edge *edge;                 /* Current edge */
   Logical rm_left, rm_right;  /* Edge remove flags */
   Logical base_ends_off;      /* Ends of prov' trap' base off */
   Logical base_cross;         /* Base of prov' trap' is crossing */
   Index i, j;                 /* Loop indices */
   Float depth;                /* Depth below top of prov' trap' */
   Float tzleft, tzright;      /* Possible ends of prov' trap' base */
   Logical possible;           /* Possible edge, not eliminated */
   Logical peak_change;        /* Peak vertex change flag */
   Logical found;              /* Trapezoid found and edge table
                                  proven changed by its removal */

   /* Find highest peak. The trapezoid will be extracted from this */
   hpeak = hi_peak();
#ifdef DEBUG
   DMESS "\ntrap_extract start\n");

   if(hpeak == NULL)
   {
      if(etab->next == NULL)
      {
         DMESS "DONE IT! No more trapezoids to extract.\n");
      }
      else
      {
        DMESS "trap_extract: No peak found for trapezoid\n");
        et_diagnose();
      }
   }
#endif
   if(hpeak == NULL) return FALSE;   /* Return, if no peak found */

#ifdef DEBUG_FULL
   DMESS "trap_extract: *** New trapezoid to extract from peak at %d\n",
   hpeak->left->hiye-psv1);
#endif

   peak_change = FALSE;   /* Initialize Peak Vertex Change Flag */

   /* Count the number of edges in the edge table */
   for(i=0,edge=etab; (edge != NULL) && i<n_et; i++,edge=edge->next)
   n_old = i;
   if(n_old == 0L) return FALSE;  /* Empty edge table */
   /* Save the first 4 edges, to check for change afterwards */
   edge = etab;
   for(i=0; i<4 && i<n_old-1; i++)
   {
      old[i] = *edge;
      edge = edge->next;
   }


   /* Set upper edge of trapezoid */
   tz->top_left  = hpeak->left->hix;
   tz->top       = hpeak->left->hiye->y;
   tz->top_right = hpeak->right->hix;
   /* Initialize edge remove flags */
   rm_left = rm_right = TRUE;
   /* Mark the X-values of provisional trapezoid base, out of date */
   base_ends_off = TRUE;
   /* Base is not the intersection of two sides */
   base_cross = FALSE;

   /* Set the Y-value for provisional trapezoid (may be raised) */
   tz->base = hpeak->right->loe->y;
#ifdef DEBUG
   sprintf(trigger,"right loe->y");
#endif
   if(hpeak->left->loe->y < hpeak->right->loe->y)
   {
      rm_left = FALSE;
   }
   if(hpeak->left->loe->y > hpeak->right->loe->y)
   {
      tz->base = hpeak->left->loe->y;
      rm_right = FALSE;
#ifdef DEBUG
      sprintf(trigger,"left loe->y");
#endif
   }


   /* Do any edges have their tops inside the provisional trapezoid? */
   /* If yes, raise base of provisional trapezoid to it. */

   {
      /* Temporarily remove the peak from the edge table */
      etab->next = hpeak->second->next;

      /* Go through the other edges */
      for(i=0,edge=etab; i<n_et; i++)
      {
         edge = edge->next;
         if(edge == NULL)break;
         depth = tz->top - edge->hiye->y;
         if(depth >= cur.vdcmin)
         {
           tzleft = tz->top_left + depth*(hpeak->left_slope);
           tzright = tz->top_right + depth*(hpeak->right_slope);
           possible = TRUE;
         }
         else
         {
           tzleft = tz->top_left + mindiff;
           tzright = tz->top_right - mindiff;
           possible = (tzleft <= tzright);
         }
         if(possible)
         {   /* Edge  possibly has top inside provisional trapezoid */
            if(edge->hiye->y > tz->base)
            {  /* Edge is high enough to be inside prov' trap' */
               if((tzleft < edge->hix) && (edge->hix < tzright))
               { /* Such an edge found, raise base to it and quit loop.*/
                  tz->base = edge->hiye->y;
                  peak_change = TRUE;
                  rm_left = rm_right = FALSE;
#ifdef DEBUG
                  sprintf(trigger,"inside edge %d",edge-etab);
#endif
                  /* We have X-coords of base, so might as well update */
                  tz->base_left = tzleft;
                  tz->base_right = tzright;
                  base_ends_off = FALSE;
                  break;
               }
               else
               {
                  if(tzleft > tzright)
                  {  /* The sides of the prov' trap' have crossed */
                     /* Raise base quit loop */
                     tz->base = edge->hiye->y;
#ifdef DEBUG
                     sprintf(trigger,"trap cross");
#endif
                     /* Base not final, X-coords not updated now */
                     base_ends_off = TRUE;
                     base_cross = TRUE;
                     rm_left = rm_right = FALSE;
                     peak_change = FALSE;
                     /* Check depth of provisional trapezoid */
                     if( tz->base >= tz->top) return FALSE;
                     break;
                  }
               }
            }
            else
            { /* Now below base - quit loop */
               break;
            }
         }             /* End of if(possible) */
      }                /* End of loop */
      /* Put peak back into edge table */
      etab->next = hpeak->first;
   }


   /* Do any Peak (Implied) Edges Intersect the left or right */
   /* sides of the provisional trapezoid? (if yes, raise base to it) */

   {
      Edge *side;    /* Current side of provisional trapezoid */
      Float   *slope;   /* Slope of current side of prov' trap' */
      Edge *e1, *e2; /* 1st & 2nd edges */
      Edge *eleft,   /* Left edge */
              *eright;  /* Right edge */
      Float   tzend;    /* Possible end of prov' trap' base */
      Logical cross;    /* True if edges cross */

      /* Temporarily remove the peak from the edge table */
      etab->next = hpeak->second->next;

      side=hpeak->left; slope=&(hpeak->left_slope);
      for(j=0; j<2; j++ )
      { /* For each side of the provisional trapezoid */
         if(etab->next == NULL) break;
         for(i=0, e2=etab->next; i<n_et-2; i++)
         {  /* For each of the other edges */
            e1 = e2;
            e2 = e1->next;
            if(e2 == NULL) break;
            if(e2->hiye->y < tz->base) break;

            if(e1->hiye == e2->hiye)
            { /* Implicit Peak edge found */
               /* Find left and right */
               if(e1->hix <= e2->hix )
               {
                  eleft = e1; eright = e2;
               }
               else
               {
                  eleft = e2; eright = e1;
               }
               depth = tz->top - e2->hiye->y;
               tzend = side->hix + depth*(*slope);
               cross = FALSE;
               if(TZEQUAL(eleft->hix,tzend))
               {
                  if(TZEQUAL(eright->hix,tzend))
                  {  /* side intersects peak */
                     cross = edge_cross( side, eleft) &&
                             edge_cross( eright, side);
                  }
                  else
                  {  /* side intersects left-end of peak edge */
                     if(side == hpeak->right)
                     {  /* peak edge from outside prov' trap' */
                        cross = edge_cross( side, eleft);
                     }
                     else
                     {  /* peak edge from inside prov' trap' */
                        cross = (tz->top_left < tz->top_right) ||
                                (tz->top > side->hiye->y);
                     }  /* may be tangent */
                  }
               }
               else
               {
                  if(TZEQUAL(eright->hix,tzend))
                  { /* Side intersects right end of peak edge */
                     if(side == hpeak->left)
                     {  /* Peak edge from outside prov' trap' */
                        cross = edge_cross( eright, side);
                     }
                     else
                     {  /* Peak edge from inside prov' trap' */
                        cross = (tz->top_left < tz->top_right) ||
                                (tz->top > side->hiye->y);
                     }  /* may be tangent */
                  }
                  else  /* Simple case */
                  { /* Side intersects neither end of peak edge */
                     cross = (eleft->hix < tzend) &&
                                           (tzend < eright->hix);
                  }
               }

               if(cross)
               { /* Crossing found, raise base of prov' trap' to it */
                  tz->base = e1->hiye->y;
                  base_ends_off = TRUE;
                  base_cross = FALSE;
                  peak_change = TRUE;
                  rm_left = rm_right = FALSE;
#ifdef DEBUG
                  sprintf(trigger,"peak edge X %d",e1->hiye-psv1);
#endif
               }
            }    /* End if peak edge */
         }       /* End loop over other edges */

         /* Move to Right hand side */
         side=hpeak->right; slope=&(hpeak->right_slope);
      }          /* End loop over two sides */

      /* Put the peak back into the edge table */
      etab->next = hpeak->first;
   }


  /* Do any Ordinary Edges Intersect the left or right sides of */
  /* the provisional trapezoid? (if yes, raise base to intersection) */

   {
      Edge    *side;     /* Current side of provisional trapezoid */
      Point   ehie,shie; /* Higher ends of edges */
      Point   crossing;  /* Intersection point */
      Logical collin;    /* True, if collinear */

      side = hpeak->left;
      for(j=0; j<2; j++)
      { /* For each side of the provisional trapezoid? */
         for(i=0, edge=etab; i<n_et+1; i++)
         {  /* For each edge */
            edge = edge->next;
            if(edge == side) edge = edge->next; /* Skip self same edge */
            if(edge == NULL) break;

            /* Determine common Y-range */
            if(edge->hiye->y <= tz->base) break; /* Gone past */
            if(edge->loe->y <= tz->top)
            { /* There is a common Y-range: determine intersection */
               shie.x = side->hix;  shie.y = side->hiye->y;
               ehie.x = edge->hix;  ehie.y = edge->hiye->y;
               if(GRAcross(&shie,side->loe, &ehie,edge->loe,
                           &collin, &crossing))
               {
                  if(!collin)
                  {  /* Intersection found, raise base to it */
                     if(crossing.y > tz->base)
                     { /* Crossing above base of provisional trapezoid */
                        if(tz->top > crossing.y+mindiff)
                        { /* Crossing below top of prov' trap' */
                           tz->base = crossing.y;
                           base_ends_off = TRUE;
                           base_cross = (edge->hiye == side->hiye);
                           rm_left = rm_right = FALSE;
                           peak_change = FALSE;
#ifdef DEBUG
                           if(side == hpeak->left)
                             sprintf(trigger,"left side X %d",edge-etab);
                           else
                            sprintf(trigger,"right side X %d",edge-etab);
#endif
                           if(tz->base >= tz->top) return FALSE;
                        }    /* End if crossing below top  */
                     }       /* End if crossing above base */
                  }          /* End if(!collin) */
               }             /* End if(GRAcross ... */
            }                /* End if common Y-range */
         }                   /* End loop over edges */

         /* Move to right hand side */
         side = hpeak->right;
      }                      /* End loop over two sides */
   }


   /* Here the Provisional Trapezoid is Finalized */
   {
      if(base_ends_off)
      {  /* Calculate base ends */
         tz->base_left = tz->top_left +
                        hpeak->left_slope*(tz->top - tz->base);
         if(base_cross)
         { /* Base is an intersection, just one point */
            tz->base_right = tz->base_left;
         }
         else
         { /* Base has length */
            tz->base_right = tz->top_right +
                             hpeak->right_slope*(tz->top - tz->base);
         }
      }

#ifdef DEBUG_FULL
     DMESS "\ntrap_extract: trapezoid top %f bottom %f", tz->top,  tz->base);
     DMESS "\n              bottom left %f bottom right %f",  tz->base_left,
                                                              tz->base_right);
     if ( rm_right ) DMESS "\n remove right edge");
     if ( rm_left ) DMESS "\n remove left edge");
#endif

     /* Remove trapezoid from edge table */
     if((rm_left && rm_right) && (hpeak->left->loe == hpeak->right->loe))
     {  /* The two sides of the trapezoid meet at base */
        etab->next = hpeak->second->next; /* Just, remove the peak */
     }
     else
     {  /* Truncate peak to base of trapezoid */
        peak_truncate( tz );
        /* Remove any edges that need removing (should be zero length) */
        if(rm_left)  peak_change |= edge_remove(hpeak->left);
        if(rm_right) peak_change |= edge_remove(hpeak->right);
     }
   }
  /* If necessary, validate edge table */
   if(peak_change && et_validate())
   {
#ifdef DEBUG
       DMESS "trap_extract: Error in making edge table valid.\n");
#endif
       return FALSE;
   }

   /* Check the edge table for removal of trapezoid */
   {
      /* Count the number of edges in the edge table */
      for(i=0, edge=etab; (edge != NULL) && i<n_et; i++, edge=edge->next)
      n_new = i;

      /* Count trapezoid as found, if its removal has resulted
         in the edge table shorter. */
      found = (n_new < n_old);

      if(n_new > 0)
      { /* At least one edge still in edge table */
        if(!found)
         {  /* Trapezoid may have been removed without shortening the
               edge-table, but then it may not lengthened */
            if(n_new > n_old)
            { /* Removal has caused edge table to become longer! */
#ifdef DEBUG
  DMESS "trap_extract: edge table had %d edges now has %d edges\n",
               n_old,n_new);
               et_diagnose();
#endif
               return FALSE;
            }

           /* Count trapezoid as found, only if a change has occurred
               in the first four edges as a result of the trapezoid
               being removed. */
            new[0] = etab;
            for(i=0; i<3 && i<n_new-1; i++) new[i+1] = new[i]->next;
            for(i=0; i<4 && i<n_new; i++)
            { /* Look for change in array indices of edges */
               found = (new[i]->next != old[i].next);
#ifdef DEBUG_FULL
               if(found) DMESS "ETCHCH %d next %d  %d\n",
               i,old[i].next-etab,new[i]->next-etab);
#endif
               if(found) break;
            }
            if(!found)
            {  /* No change found, look for others */
               for(i=1; i<4 && i<n_new; i++)
               { /* Look for a change in the edges */
                  found = ( (new[i]->loe != old[i].loe) ||
                            !TZEQUAL(new[i]->hix,    old[i].hix) ||
                            !TZEQUAL(new[i]->hiye->y,old[i].hiye->y) );
#ifdef DEBUG_FULL
                  if(found)DMESS "ETCHCH %d loe %d %d\n",
                  i,old[i].loe-psv1,new[i]->loe-psv1);
                  if(found)DMESS "ETCHCH %d hix %f %f hiy %f %f\n",
                  i,old[i].hix,new[i]->hix,
                  i,old[i].hiye->y,new[i]->hiye->y);
#endif
                  if(found) break;
               }
            }      /* End if (!found) */
         }         /* End if (!found) */
      }            /* End if edge table not emptied */
#ifdef DEBUG
      else DMESS "DONE IT! Last trapezoid extracted successfully.\n");
#endif
   }
  /* We could have, just checked if the edge table was empty beforehand,
     but in case of bug, a stricter condition is imposed,
     to prevent unwanted looping */

#ifdef DEBUG
   if(!found)
   {
     DMESS "\ntrap_extract: Trapezoid at %d ignored, \n",
     hpeak->left->hiye-psv1);
     DMESS "because no change was found in edge table.\n");
     DMESS "trigger: %s\n",trigger);
     et_diagnose();
   }
#endif
#ifdef DEBUG_FULL
   DMESS "TZ top  TZ (%f:%f) %f, trigger %s\n",
          tz->top_left,tz->top_right,tz->top,trigger);
   DMESS "TZ base TZ (%f:%f) %f\n",
          tz->base_left,tz->base_right,tz->base);
#endif

   return found;
}

/*************************************************** et_diagnose *****/
#ifdef DEBUG
static void et_diagnose ()

/* Print and diagnose edge table, when in debug mode */

{
   Index j;      /* Loop indices */
   Edge *edge;   /* Current edge */

   DMESS "\n   Edge Table Ordering w.r.t Array Index \n");
   for(j=0; j <= n_et; j++)
   {
      if((etab+j)->next == NULL)
         DMESS "end at %d; ",j);
      else
         DMESS "from %d to %d; ",j,(etab+j)->next - etab);
      if(j+1 == 4*((j+1)/4)) DMESS "\n");
   }

   edge = etab->next;
   if(edge == NULL)
   {
      DMESS "\n et_diagnose: Empty edge table!\n");
   }
   else
   {
      DMESS "\n   Edge Table (as sorted) \n");
      for(j=0; (j <= n_et) && (edge != NULL); j++, edge=edge->next)
      {
        DMESS "%3d (%3d to %3d)",
              edge-etab, edge->hiye-psv1, edge->loe-psv1);
        DMESS " (%f,%f), to (%f,%f) ",
              edge->hix,edge->hiye->y, edge->loe->x,edge->loe->y);
        if( VEQUAL(edge->hiye->y,edge->loe->y) ) DMESS "-\n");
        else                                     DMESS "/\n");
      }
      DMESS "%d edges in edge table.\n",j);
   }

   return;
}
#endif
