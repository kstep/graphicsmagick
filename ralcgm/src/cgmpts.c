/*   RAL-CGM Interpreter module:  @(#) cgmpts.c  version 3.6
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
 * K E V palmen @ Rutherford Appleton Laboratory (kevp@uk.ac.rl.ib)
 *
 * Description:
 *
 * Maintenance Log:
 *   3 Sep 92 KEVP Original Version.
 *  28 Sep 92 KEVP Added PTSdefclip to set default clipping rectangle.
 *  29 Sep 92 KEVP Added PTSvdcextget to get VDC extent.
 *   9 Oct 92 KEVP In PTSclipget, made clipping indicator optional and
 *                 and corrected its non-ANSI name from 'clipflag'.
 *   4 Nov 92 RTP  Correct assignment for Integer VDCs
 *  16 Nov 92 KEVP Added PTSlist and PTSfree to handle stored points.
 *                 Moved include files out from cgmpts.h
 *  11 Jan 92 KEVP Split long logical expression in PTSlineclip into
 *                 three parts in attempt to satisfy compiler limits.
 *  15 Jan 93 RTP  Add assignment before CLIPCODE to prevent Iris Compile
 *                 error
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *  12 Sep 94 KEVP Enable PTSlist to return an unitialised points list to be
 *                  filled by such things as the GDP utilities, therby saving the output
 *                  driver from needing to have its own Points store for such.
 */


#define CGMPTS_C

#include "cgmout.h"
#include "cgmpts.h"

/* Macros for clipping */

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


/* Statics for points list */
#define NSTATPTS  ARRAY_MAX      /* Number of points in static store */
static Point statpts[NSTATPTS];  /* Static store for points */
static Enum  statfls[NSTATPTS];  /* Static store for edge-out flags */
static Int ndynpts=0,            /* Number of points in dynamic store */
           ndynfls=0;            /* Number of flags in dynamic store */
static Point *dynpts;            /* Pointer to dynamic point store */
static Enum  *dynfls;            /* Pointer to dynamic flag store */

/****************************************************** PTSfree ********/
#ifdef PROTO
void PTSfree ( )
#else
void PTSfree ()
/* Free space allocated to points. */
#endif

{
   if(ndynpts > 0)
   {
      FREE(dynpts);
      if(dynfls != NULL) FREE(dynfls);
      ndynpts = 0;
      ndynfls = 0;
   }
   return;
}
/****************************************************** PTSget *****/

#ifdef PROTO
void PTSget( Long np, Long *pi, Float *pr, Point *pt, Enum *fl )
#else
void PTSget( np, pi, pr, pt, fl)
 /*
    Get a points list out of the data provided.
    If 'fl' is NULL  a simple points list is got
    else  a polygon set point list is got.
 */

   /* Input Arguments */
Long np;     /* Number of Points (got from *(pi-1)) */
Long *pi;    /* Integer Data */
Float *pr;   /* Real Data */

   /* Output Arguments */
Point *pt;    /* The points (np Points needed) */
Enum  *fl;    /* Flags for Polygon set (np Enums needed, or NULL) */
#endif

/* COMMENTS
   This utility is ideal for POLYGON and POLYGONSET for other primitives
   such as POLYLINE, it is not recommended for very long point lists,
   but it can be used on chunks of such.
*/

{
   Point *curpt;  /* current point */
   Point *limit;  /* limit of points */
   Enum  *curfl;  /* current flag */

   if(pt == NULL)  /* Check pt */
   {   /* No points allocated!*/
     return;
   }
   if(np < 1)
   {  /* Number of points less than 1 */
     return;
   }

   limit = pt + (int)np;  /* Set limit to range of points */

   if(fl == NULL)
   {   /* Simple Points List (not polygon set) */
       if(cur.vdc_type == REAL)  /* Real VDCs */
       {
           for(curpt=pt;curpt<limit;curpt++)
           {
               curpt->x = *pr++;
               curpt->y = *pr++;
           }
       }
       else       /* Integer VDCs */
       {
           for(curpt=pt;curpt<limit;curpt++)
           {
               curpt->x = (Float)*pi++;
               curpt->y = (Float)*pi++;
           }
       }
   }
   else
   {    /* Polygon Set Points List */
       if(cur.vdc_type == REAL)  /* Real VDCs */
       {
           for(curpt=pt,curfl=fl;curpt<limit;curpt++,curfl++)
           {
               curpt->x = *pr++;
               curpt->y = *pr++;
               *curfl   = *pi++;
           }
       }
       else                      /* Integer VDCs */
       {
           for(curpt=pt,curfl=fl;curpt<limit;curpt++,curfl++)
           {
               curpt->x = (Float)*pi++;
               curpt->y = (Float)*pi++;
               *curfl   = *pi++;
           }
       }
   }
   return;
}
/**************************************************** PTSclipget ******/

#ifdef PROTO
void PTSclipget ( Enum *clipind, Rect *cliprect )
#else
void PTSclipget ( clipind, cliprect )
/*
     If required,
     get a copy of the current CLIPPING INDICATOR and
     if Clipping is ON,
     get a copy of the current CLIPPING RECTANGLE, else VDC extent.
*/
Enum *clipind;         /* Clipping Indicator (NULL, if not required) */
Rect *cliprect;        /* Clipping rectangle */
#endif

{
  float swap;          /*  Buffer for swapping */

  /* Get Clipping Indicator, if required */
  if(clipind != NULL) *clipind = cur.clip_ind;

  /* Get rectangle */
  if ( cur.clip_ind == ON )
  {                         /* Clipping */
    if (cur.vdc_type==REAL)
    {
        cliprect->left  = (Float)cur.clip_rect.a.x.real;
        cliprect->right = (Float)cur.clip_rect.b.x.real;
        cliprect->top   = (Float)cur.clip_rect.b.y.real;
        cliprect->bot   = (Float)cur.clip_rect.a.y.real;
    }
    else
    {
        cliprect->left  = (Float)cur.clip_rect.a.x.intr;
        cliprect->right = (Float)cur.clip_rect.b.x.intr;
        cliprect->top   = (Float)cur.clip_rect.b.y.intr;
        cliprect->bot   = (Float)cur.clip_rect.a.y.intr;
    }
  }
  else  /* cur.clip_ind == OFF */
  {                     /* Not clipping, output current VDC extent */
     if ( cur.vdc_type == REAL )
     {
         cliprect->left  = (Float)cur.vdc_extent.a.x.real;
         cliprect->right = (Float)cur.vdc_extent.b.x.real;
         cliprect->top   = (Float)cur.vdc_extent.b.y.real;
         cliprect->bot   = (Float)cur.vdc_extent.a.y.real;
     }
     else
     {
         cliprect->left  = (Float) cur.vdc_extent.a.x.intr;
         cliprect->right = (Float) cur.vdc_extent.b.x.intr;
         cliprect->top   = (Float) cur.vdc_extent.b.y.intr;
         cliprect->bot   = (Float) cur.vdc_extent.a.y.intr;
     }
  }

  /* Make sure that the sides are correctly ordered */
  if(cliprect->right < cliprect->left)
  {
     swap = cliprect->right;
     cliprect->right = cliprect->left;
     cliprect->left = swap;
  }
  if(cliprect->top < cliprect->bot)
  {
     swap = cliprect->top;
     cliprect->top = cliprect->bot;
     cliprect->bot = swap;
  }
  return;
}
/****************************************************** PTSdefclip *****/
#ifdef PROTO
void PTSdefclip ()
#else
void PTSdefclip ()
/*
Set default clipping rectangle (which is equal to the VDC extent).
*/
#endif

/*
This utility needs to be called at the VDCEXT entry-point,
to ensure that PTSclipget gets the true clipping rectangle.
*/

{
   if(cur.vdc_type == REAL)  /* Real VDCs */
   {
      cur.clip_rect.a.x.real = cur.vdc_extent.a.x.real;
      cur.clip_rect.a.y.real = cur.vdc_extent.a.y.real;
      cur.clip_rect.b.x.real = cur.vdc_extent.b.x.real;
      cur.clip_rect.b.y.real = cur.vdc_extent.b.y.real;
   }
   else  /* Integer VDCs */
   {
      cur.clip_rect.a.x.intr = cur.vdc_extent.a.x.intr;
      cur.clip_rect.a.y.intr = cur.vdc_extent.a.y.intr;
      cur.clip_rect.b.x.intr = cur.vdc_extent.b.x.intr;
      cur.clip_rect.b.y.intr = cur.vdc_extent.b.y.intr;
   }
   return;
}
/****************************************************** PTSlineclip ***/

#ifdef PROTO
void PTSlineclip ( Long np, Point *pt, Rect cl,
                void (*linefunc)(Long, Point *))
#else
void PTSlineclip ( np, pt, cl, linefunc )
/*
     CLIPs a polyline and DRAWs it with the
     supplied polyline drawing function.
*/
Long  np;            /* Number of vertices in Polyline Polygon */
Point *pt;           /* The vertices in the Polyline Polygon   */
Rect  cl;            /* Clipping rectangle */
void  (*linefunc)(); /* The polyline drawing function */
#endif

/* NOTE on TERMINOLOGY
                    The term EDGE in this utility refers to a
                    a single line segment in the boundary of a
                    polygon and not the whole boundary itself.

   NOTE on "linefunc"
                    The device function "linefunc" is different
                    from the device function "linedraw" used by
                    BEZtext or HERtext.  Unlike "linedraw",
                    "linefunc" has no transformation argument and
                    assumes clipping has been done.
*/
{
   Point *ptclip;                /* Component of clipped polyline */
   Long  npcl;                   /* Number of vertices in component */
   Point *end1, *end2;           /* Current Edge */
   Point *eclp;                  /* End of current edge to be clipped */
   Point *eoth;                  /* Other end of current edge */
   Point ptsave1,ptsave2;        /* Saved points */
   Point ul,ur,ll,lr;            /* Corners of the clipping rectangle */
   Point tpt;
   Enum  clx1,cly1,clx2,cly2;    /* Clip codes */
   Enum  clysave;                /* Saved clip code */
   Enum  *clpx,*clpy;            /* Clip codes for end to be clipped */
   Enum  edgestatus;             /* Status of the current edge */
   Int   nendcl=0;               /* Number of ends to be clipped */
   Int   i,j;                    /* Loop indices over edges & ends */

  /* Set the four corners */
   ll.x = cl.left;  ll.y = cl.bot;
   lr.x = cl.right; lr.y = cl.bot;
   ul.x = cl.left;  ul.y = cl.top;
   ur.x = cl.right; ur.y = cl.top;

   /* Initialise variables for loop over edges of the polygon */
   end1 = pt; end2 = pt+1;         /* 1st edge between 1st two points */
   npcl = 1;                             /* 1st component is 1st edge */
   ptclip = end1;                 /* 1st clipped component from start */
   tpt = *end1;
   clx1 = CLIPCODE(tpt.x,cl.left,cl.right);  /* Get clipcodes */
   cly1 = CLIPCODE(tpt.y,cl.bot,cl.top);     /* for 1st end */
   if((clx1 == 0) && (cly1 == 0))
   {  /* 1st end inside: initialise edge status as INSIDE */
      edgestatus = INSIDE;
   }
   else
   {  /* 1st end outside: initialise edge status as BOTHOUT */
      edgestatus = BOTHOUT;
   }
   /* loop over edges of the polygon */
   for (i=1;i<np;i++,end1++,end2++)
   {
     tpt = *end2;
     clx2 = CLIPCODE(tpt.x,cl.left,cl.right);   /* get clipcodes */
     cly2 = CLIPCODE(tpt.y,cl.bot,cl.top);       /* for 2nd end */


     /* Determine status of new current edge */
     if((clx2 == 0) && (cly2 == 0))
     { /* 2nd end inside clipping rectangle */
        switch(edgestatus) /* status of last edge or as initialised */
        {
           case INCOMING:
              edgestatus = INSIDE;     /* Incoming becomes inside */
              break;
           case OUTGOING: case BOTHOUT:
              edgestatus = INCOMING;   /* Out becomes incoming */
              break;
        }
     }
     else
     { /* 2nd end outside clipping rectangle */
        switch(edgestatus) /* status of last edge or as initialised */
        {
           case INCOMING: case INSIDE:
              edgestatus = OUTGOING;  /* In becomes outgoing */
              break;
           case OUTGOING:
              edgestatus = BOTHOUT;   /* Outgoing becomes Bothout */
              break;
        }
     } /* Edgestatus now updated */

     /* Prepare current edge for (not) clipping  */
     switch(edgestatus)   /* Act according to edgestatus */
     {
       case INSIDE:    /* Both ends Inside */
          npcl++;                    /* Increment count for clipped */
          nendcl = 0;                /* compoment and clip no ends. */
          break;
       case INCOMING:  /* 1st end Outside and 2nd end Inside */
         npcl = 2;                   /* Start new clipped component. */
         ptclip = end1;
         eclp = end1;  eoth = end2;  /* Set First end to be clipped. */
         clpx = &clx1; clpy = &cly1;
         ptsave1 = *end1;            /* Save first end. */
         nendcl = 1;                 /* Clip one end. */
         break;
       case OUTGOING:  /* 1st end Inside and 2nd end Outside */
         eclp = end2;  eoth = end1;  /* Set Second end to be clipped */
         clpx = &clx2; clpy = &cly2;
         ptsave2 = *end2;            /* Save Second end and clipcode */
         clysave = cly2;
         npcl++;                     /* Increment count for clipped */
         nendcl = 1;                 /* component and clip one end */
         break;
       case BOTHOUT:  /* Both ends Outside */
          /* Set variables for edge not crossing clipping rectangle */
          /* (changing them if edge if found to cross) */
          npcl = 0; nendcl = 0;
          /* Test for edge crossing rectangle, simple tests first */
          if(((clx1 != clx2) || (clx1 == 0)) &&
             ((cly1 != cly2) || (cly1 == 0)) )
          {  /* edge ends by different sides of rectangle */
            if(CLOCK(end1,&ul,end2) ||
               CLOCK(end1,&ur,end2) ||
               CLOCK(end1,&ll,end2) ||
               CLOCK(end1,&lr,end2) )
            {  /* edge does not pass rectangle clockwise */
              if(CLOCK(end1,end2,&ul) ||
                 CLOCK(end1,end2,&ur) ||
                 CLOCK(end1,end2,&ll) ||
                 CLOCK(end1,end2,&lr) )
              {    /* edge crosses clipping rectangle */
                 ptclip = end1;     /* Start new clipped component */
                 npcl = 2;
                 eclp  = end1;   /* Set first end to be clipped first */
                 eoth  = end2;
                 clpx = &clx1; clpy = &cly1;
                 ptsave1 = *end1;   /* Save both ends */
                 ptsave2 = *end2;   /* and clipcode for 2nd */
                 clysave = cly2;
                 nendcl = 2;        /* Clip both ends */
              }
            }
          }
          break;
     }  /* End of switch on Edge Status */

     /* If at least one end is to be clipped, clip the current edge. */
     if((nendcl > 0) && (nendcl < 3))
     {
       for(j=0;j<nendcl;j++) /* Backward and Forward clip as required */
       {
         switch(*clpx)   /* Clip on X and revise Y clipcodes */
         {
           case CLIPMIN:    /* Clip Left */
             if(!VEQUAL(eoth->x,eclp->x))
               eclp->y = ECLIP(eoth->y,eclp->y,eoth->x,eclp->x,cl.left);
             eclp->x = cl.left;
             if((cl.bot <= eclp->y) && (eclp->y <= cl.top))
                 *clpy=CLIPNO;
             break;
           case CLIPMAX:    /* Clip Right */
             if(!VEQUAL(eoth->x,eclp->x))
               eclp->y = ECLIP(eoth->y,eclp->y,eoth->x,eclp->x,cl.right);
             eclp->x = cl.right;
             if((cl.bot <= eclp->y) && (eclp->y <= cl.top))
                 *clpy=CLIPNO;
             break;
         }
         switch(*clpy)  /* Clip in Y */
         {
           case CLIPMIN:    /* Clip Below */
             if(!VEQUAL(eoth->y,eclp->y))
               eclp->x = ECLIP(eoth->x,eclp->x,eoth->y,eclp->y,cl.bot);
             eclp->y = cl.bot;
             break;
           case CLIPMAX:    /* Clip Above */
             if(!VEQUAL(eoth->y,eclp->y))
               eclp->x = ECLIP(eoth->x,eclp->x,eoth->y,eclp->y,cl.top);
             eclp->y = cl.top;
             break;
         }
         if(j < nendcl-1)
         {   /* prepare for 2nd run of loop */
           eclp = end2;   /* Set Second end for clipping second */
           eoth = end1;
           clpx = &clx2; clpy = &cly2;
         }
       }  /* End of for loop over j (ends to be clipped) */

       /* Draw clipped component, if complete */
       switch(edgestatus)
       {
          case OUTGOING: case BOTHOUT: /* Component complete */
            if(npcl > 1)linefunc(npcl,ptclip); /* Draw it */
            npcl = 0;          /* No clipped component to draw */
            *ptclip = ptsave1; /* Restore First point Unclipped */
            *end2 = ptsave2;   /* Restore Last point Unclipped */
            cly2 = clysave;    /* Restore Last point's clip y code */
            break;
       }
     } /* End of clipping for current edge */
     clx1 = clx2; cly1 = cly2; /* Set Clipcodes for next 1st end */

   } /* End of for loop over i (the edges) */

   /* Draw any remaining edges */
   if(npcl>1)linefunc(npcl,ptclip);
   return;
}
/****************************************************** PTSlist ****/

#ifdef PROTO
Point * PTSlist( Long np, Long *pi, Float *pr, Enum **fl )
#else
Point * PTSlist( np, pi, pr, fl)
 /*
Get points from arguments, store them and return pointer to store.
If fl is not NULL assume polygonset, get the edge-out flags too
and set *fl to point to them.
Returns an unitialised points list if all pointer arguments are NULL.
 */

   /* Input Arguments */
Long np;     /* Number of Points (got from *(pi-1)) */
Long *pi;    /* Integer Data */
Float *pr;   /* Real Data */

   /* Output Arguments */
Enum  **fl;    /* Flags for Polygon set (np Enums needed, or NULL) */
#endif

/* NOTE much of the code here is duplicated in PTSget */
{
   char *func = "PTSlist";    /* Function name for error reporting */
   Point *pt1;      /* first point */
   Point *curpt;    /* current point */
   Point *limit;    /* limit of points */
   Enum  *curfl;    /* current edge-out flag */

   /* Allow number of points to be negative, takingg absolute value */
   if(np < 0) np = -np;

   /* Select Points-list */
   if(np > NSTATPTS)
   {   /* Static store is insufficient, use dynamic */
       if(np > ndynpts)
       {  /* Current allocation is insufficient, free and make new */
          if(dynpts != NULL) FREE(dynpts);
          dynpts = (Point *)MALLOC( np, sizeof( Point));
          if(dynpts == NULL)
          {
             exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
          }
          ndynpts = np;
       }
       /* Points already allocated */
       pt1 = dynpts;
   }
   else
   {   /* The static store has sufficient space, use it */
       pt1 = statpts;
   }

  /* If argument pointers are NULL, just return the pointer to the points list */
   if( pi == NULL && pr == NULL) return pt1;

   /* Select edge-out flag list, if required */
   if(fl != NULL)
   {    /* Flags are required */
      if(np > NSTATPTS)
      {   /* Static store is insufficient, use dynamic */
          if(np > ndynfls)
          {  /* Current dynation is insufficient, free and make new */
             if(dynfls != NULL) FREE(dynfls);
             dynfls = (Enum *)MALLOC( np, sizeof( Enum));
             if(dynfls == NULL)
             {
                exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
             }
             ndynfls = np;
          }
          /* Enums already allocated */
          *fl = dynfls;
      }
      else
      {   /* The static store has sufficient space, use it */
          *fl = statfls;
      }
   }


   /* Get Points (and edge-out flags if required) from arguments */
   if(np < 1)
   {  /* Number of points less than 1 */
     return pt1;
   }

   limit = pt1 + (int)np;  /* Set limit to range of points */

   if(fl == NULL)
   {   /* Simple Points List (not polygon set) */
       if(cur.vdc_type == REAL)  /* Real VDCs */
       {
           for(curpt=pt1;curpt<limit;curpt++)
           {
               curpt->x = *pr++;
               curpt->y = *pr++;
           }
       }
       else       /* Integer VDCs */
       {
           for(curpt=pt1;curpt<limit;curpt++)
           {
               curpt->x = (float)*pi++;
               curpt->y = (float)*pi++;
           }
       }
   }
   else
   {    /* Polygon Set Points List */
       if(cur.vdc_type == REAL)  /* Real VDCs */
       {
           for(curpt=pt1,curfl= *fl;curpt<limit;curpt++,curfl++)
           {
               curpt->x = *pr++;
               curpt->y = *pr++;
               *curfl   = *pi++;
           }
       }
       else                      /* Integer VDCs */
       {
           for(curpt=pt1,curfl= *fl;curpt<limit;curpt++,curfl++)
           {
               curpt->x = (Float)*pi++;
               curpt->y = (Float)*pi++;
               *curfl   = *pi++;
           }
       }
   }
   return pt1;
}
/****************************************************** PTSmarker ****/

#ifdef PROTO
void PTSmarker ( Long nmk, Point *pt, Float nomsize, Markeratt *ma,
              Enum clipind,
              void (*linefunc)(Long, Point *))
#else
void PTSmarker ( nmk, pt, nomsize, ma, clipind, linefunc )

/*   draws 'nmk' markers starting at 'pt',                     */
/*   using the supplied line drawing function 'linefunc'       */
/*   for any one of the mandatory marker types                 */
/*   markers are clipped if 'clipind' & clipping are both on   */

Long nmk;             /* Number of Markers */
Point *pt;            /* The Marker Points */
Float nomsize;        /* Nominal Marker Size */
Markeratt *ma;        /* Marker attributes */
Enum clipind;         /* Clipping indicator */
void (* linefunc)();  /* Linedrawing function */
#endif

/*
   CLIPPING POLICY
   If clipping, markers whose centres are outside the clipping rectangle,
                are not drawn at all.
                other markers are drawn and clipped if necessary.

   NOTE: This routine does not set
         the hardware linetype to solid,
         the hardware linewidth to a value suitable for drawing markers
         the hardware (line)colour to the marker colour.
*/

{
  Long i; Int j,k;    /* For loop variables. Not used inside loop */
  Double msize;       /* Actual markersize (halved) */
  Point *mkcen;       /* Current Marker Position */
  Point tpt;
  Int nmkl, nlp;      /* Number Markerlines and Points in Markerline */
  Point mkdef[MDMAX];    /* Marker Definition */
  Point mkline[MLMAX];   /* Marker Line as Drawn */
  Point *mkdp;        /* Current Marker Definition Point */
  Point *mklp;        /* Current Marker Line Point */
  Rect clip;          /* Clipping Rectangle */
  Rect inclip;        /* Interior (markerwise) of Clipping Rectangle */
  Enum aclipind;      /* Actual Clipping Indicator */
  Enum clipmark;      /* Clipping indicator for Current Marker */

  /* Find the actual marker size (halved) in VDC units */
  if ( ma->sizemode == ABSOLUTE )
  {  /* Marker size absolute ( VDC units ) */
       msize = (Double)(0.5*ma->size);
  }
  else
  {  /* Marker size scaled ( units of nominal size ) */
       msize=(Double)(0.5*nomsize*ma->size);
  }

 /* Initialise Marker Definition Array */
  mkdp  = mkdef;
  switch(ma->type)  /* Switch on Marker Type */
  {
     case 1:  /* "." */
       nmkl = 1; nlp = 2;
       mkdp->x = 0.0; mkdp->y = 0.0; mkdp++;
       mkdp->x = 0.0; mkdp->y = 0.0;
       break;
     case 2:  /* "+" */
       nmkl = 2; nlp = 2;
       mkdp->x = -msize; mkdp->y =  0.0;   mkdp++;
       mkdp->x =  msize; mkdp->y =  0.0;   mkdp++;
       mkdp->x =  0.0;   mkdp->y = -msize; mkdp++;
       mkdp->x =  0.0;   mkdp->y =  msize;
       break;
     case 3:  /* "*" */
       nmkl = 4; nlp = 2;
       mkdp->x = -msize; mkdp->y =  0.0;   mkdp++;
       mkdp->x =  msize; mkdp->y =  0.0;   mkdp++;
       mkdp->x =  0.0;   mkdp->y = -msize; mkdp++;
       mkdp->x =  0.0;   mkdp->y =  msize; mkdp++;
       mkdp->x = -msize; mkdp->y = -msize; mkdp++;
       mkdp->x =  msize; mkdp->y =  msize; mkdp++;
       mkdp->x = -msize; mkdp->y =  msize; mkdp++;
       mkdp->x =  msize; mkdp->y = -msize;
       break;
     case 4:  /* "O" */
       nmkl = 1; nlp = 9; /* Simulate with Octagon */
       mkdp->x =  msize;      mkdp->y =  OCTS*msize; mkdp++;
       mkdp->x =  OCTS*msize; mkdp->y =  msize;      mkdp++;
       mkdp->x = -OCTS*msize; mkdp->y =  msize;      mkdp++;
       mkdp->x = -msize;      mkdp->y =  OCTS*msize; mkdp++;
       mkdp->x = -msize;      mkdp->y = -OCTS*msize; mkdp++;
       mkdp->x = -OCTS*msize; mkdp->y = -msize;      mkdp++;
       mkdp->x =  OCTS*msize; mkdp->y = -msize;      mkdp++;
       mkdp->x =  msize;      mkdp->y = -OCTS*msize; mkdp++;
       mkdp->x =  msize;      mkdp->y =  OCTS*msize;
       break;
     case 5:  /* "X" */
       nmkl = 2; nlp = 2;
       mkdp->x = -msize; mkdp->y = -msize; mkdp++;
       mkdp->x =  msize; mkdp->y =  msize; mkdp++;
       mkdp->x = -msize; mkdp->y =  msize; mkdp++;
       mkdp->x =  msize; mkdp->y = -msize;
       break;
     default:
       nmkl = 0; nlp = 0;
   }  /* End of Switch on Marker Type */

   /* In case of no clipping, draw all of all markers */
   clipmark = DRAWALL;

   /* Get Clipping Rectangle, if required */
   if(clipind == ON)
   {        /* The calling routine requests clipping */
      PTSclipget( &aclipind, &clip );
      if(aclipind == ON)
      {        /* The CGM requests clipping */
         if((clip.right-clip.left > 2*msize) &&
            (clip.top-clip.bot > 2*msize))
         {        /* Markers within inclip would not be clipped */
            inclip.left  = clip.left  + msize;
            inclip.right = clip.right - msize;
            inclip.bot   = clip.bot   + msize;
            inclip.top   = clip.top   - msize;
         }
         else
         {        /* All markers could be clipped */
            inclip.left=inclip.right=inclip.bot=inclip.top = 0.0;
         }
      }
      else
      {  /* The CGM requests no clipping: do not clip */
         clipind = OFF;
      }
   } /* Clipping Rectangle got if required */


  /* Draw Markers */
  for ( i=0,mkcen=pt; i<nmk; i++,mkcen++ )
  {                          /* For each Marker */
     if(clipind == ON)
     { /* Determine if Marker is to be drawn and/or clipped */
        tpt = *mkcen;
        if( (CLIPCODE(tpt.x,clip.left,clip.right)) ||
            (CLIPCODE(tpt.y,clip.bot, clip.top)))
        {  /* Marker Centre is outside clipping rectangle */
           clipmark = NODRAW;  /* Do not draw it */
        }
        else
        {
           if( (CLIPCODE(tpt.x,inclip.left,inclip.right)) ||
               (CLIPCODE(tpt.y,inclip.bot, inclip.top)))
           {  /* Marker may be partially clipped */
              clipmark = CLIPDRAW;   /* Clip Marker */
           }
           else
           {  /* Marker entirely within clipping rectangle */
              clipmark = DRAWALL;   /* Draw without clipping */
           }
        }
     }

     if(clipmark != NODRAW)
     {     /* If current Marker is being drawn */
        for( j=0,mkdp=mkdef; j<nmkl; j++)
        {                       /* For each Marker Line */
            for( k=0,mklp=mkline; k<nlp; k++,mkdp++,mklp++)
            {                   /* For each Marker Line Point */
               mklp->x = mkdp->x + mkcen->x;
               mklp->y = mkdp->y + mkcen->y;
            }                   /* End of loop over marker line points */
            switch(clipmark)
            {
              case DRAWALL:
                   linefunc( nlp, mkline );   /* Draw without clipping */
                   break;
              case CLIPDRAW:
                   PTSlineclip(nlp, mkline, clip, linefunc );
                   break;                               /* Clip & draw */
              default:
                   break;                               /* Error */
            }               /* End of switch on clipmark */
        }                   /* End of loop over Marker Lines */
     }                      /* End of if current marker is being drawn */
  }                         /* End of loop over Markers */
  return;
}
/***************************************************** PTSvdcextget ****/
#ifdef PROTO
void PTSvdcextget(Rect *vdcxt )
#else
void PTSvdcextget( vdcxt )

/* Gets the VDC extent */

Rect *vdcxt;  /* VDC extent */

#endif
{

   if(cur.vdc_type == REAL)
   {
      vdcxt->left  = cur.vdc_extent.a.x.real;
      vdcxt->right = cur.vdc_extent.b.x.real;
      vdcxt->bot   = cur.vdc_extent.a.y.real;
      vdcxt->top   = cur.vdc_extent.b.y.real;
   }
   else
   {
      vdcxt->left  = (Float) cur.vdc_extent.a.x.intr;
      vdcxt->right = (Float) cur.vdc_extent.b.x.intr;
      vdcxt->bot   = (Float) cur.vdc_extent.a.y.intr;
      vdcxt->top   = (Float) cur.vdc_extent.b.y.intr;
   }
   return;
}
