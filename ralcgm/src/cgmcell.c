/*   RAL-CGM Interpreter module:  @(#) cgmcell.c  version 3.2
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
 * Description: CGM Cell array utilities
 *
 * Maintenance Log:
 *   3 Sep 92 KEVP Original version
 *   7 Oct 92 KEVP Changed first row argument of CELLfill to a pointer
 *                 and made CELLfill update it.
 *   7 Oct 92 KEVP Made CELLfill determine correct end of colour data
 *                 for direct colour.
 *  17 Dec 92 CDS  Corrected non ANSI declaration for CELLfill
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 */

#define CGMCELL_C

#include "cgmout.h"
#include "cgmcell.h"

/****************************************************** CELLfill ***/
#ifdef PROTO
void CELLfill ( Long ncells, Long *pi, Tmatrix celtrans,
                Int nx, Int ny, Int *iy,
                void (*setcolour)(Index, RGBcolour, Code),
                void (*filldraw)(Int, Point *, Tmatrix, Int))
#else
void CELLfill ( ncells, pi, celtrans, nx, ny, iy, setcolour, filldraw )

/*  Draws a Cell array of num values 'pi' by means of filled areas */

Long ncells;           /* Number of Cells to be Coloured */
Long *pi;              /* Cell Colour Data */
Tmatrix celtrans;      /* Cell Array transformation */
Int nx, ny;            /* Cell Array dimensions */
Int *iy;               /* Cell Array start row (0 unless continuing) */
void (*setcolour)();   /* Device Colour Setting function */
void (*filldraw)();    /* Device Fill routine for cells */
#endif

{
  Point  celb[4];         /* Boundary of Current Cell */
  register int i,j;       /* Indices of Current Cell */
  Long *end;              /* End of Colour Data */
  RGBcolour dircol;       /* Direct Colour */
  Logical cellfill=TRUE;  /* True, if cell sequence ready to be filled */
  Logical newcell=TRUE;   /* True, if new cell array */

  if((nx > 0) && (ny > 0))
  {      /* Cell array has positive dimensions */

     /* Find end of colour data */
     if(cur.color_mode == DIRECT) end = pi+ncells+ncells+ncells;
     else                         end = pi+ncells;

     for ( j = *iy; j<ny && pi<end; j++)
     {
       /* Define cell sequence start */
       celb[0].x = 0.0;
       celb[0].y = (float)(j);
       celb[1].x = 0.0;
       celb[1].y = (float)(j+1);
      /* and Y-coords of cell sequence end */
       celb[2].y = celb[1].y;
       celb[3].y = celb[0].y;

       for (i=0; i<nx && pi<end; i++)
       {
          /* Deal with colour */
          if ( cur.color_mode == DIRECT )
          {
             dircol.red   = (Posint)*pi++;
             dircol.green = (Posint)*pi++;
             dircol.blue  = (Posint)*pi++;
             setcolour( 0, dircol, CELLARRAY);
             /* Define X-coords of cell sequence end (one cell) */
             celb[2].x = celb[1].x + 1.0;
             celb[3].x = celb[0].x + 1.0;
          }
          else
          {  /* Indexed colour */
             if(((Index)(*pi) != (Index)(*(pi+1))) || (i+1 == nx))
             {  /* Next Cell in row different colour or not found */
                setcolour((Index)*pi,dircol,CELLARRAY);
                /* Define X-coords of cell sequence end */
                celb[3].x = (float)(i+1);
                celb[2].x = celb[3].x;
                cellfill = TRUE;  /* Cell sequence ready to fill */
             }
             else
             { /* Next cell is same colour: extend cell sequence */
                cellfill = FALSE; /* Cell sequence incomplete */
             }
             pi++;       /* Next Cell */
          }

          if(cellfill)
          {
             filldraw( 4, celb, celtrans, newcell);
             newcell = FALSE;
             if(i+1 != nx)
             { /* Cell row incomplete */
               /* Define next cell sequence start */
               celb[0].x = (float)(i+1);
               celb[0].y = (float)(j);
               celb[1].x = celb[0].x;
               celb[1].y = celb[0].y + 1.0;
               /* and Y-coords of its end */
               celb[2].y = celb[1].y;
               celb[3].y = celb[0].y;
      /* NB: We do not assume that filldraw has left celb unchanged. */
             }
          }
       }             /* End of loop over columns (i) */
     }               /* End of loop over rows (j) */

     *iy = j;   /* Update first row for next chunk */
  }
  else
  {
#ifdef DEBUG
     DMESS "Cell Array has invalid dimensions %d by %d \n",nx,ny);
#endif
  }
  return;
}
/****************************************************** CELLparget *****/

#ifdef PROTO
void CELLparget ( Long *pi, Float *pr,
                  Point *p, Point *q, Point *r,
                  Int *nx, Int *ny, Int *lp )
#else
void CELLparget ( pi, pr, p, q, r, nx, ny, lp )
 /*
    Get Cell Array Parameters
 */

   /* Input Arguments */
Long  *pi;  /* Integer Data */
Float *pr;  /* Real Data */

   /* Output Arguments */
Point *p;   /* Corner P by First row and First Column */
Point *q;   /* Corner Q by Last row  and Last Column */
Point *r;   /* Corner R by First row and Last Column */
Int  *nx;   /* Number of Rows */
Int  *ny;   /* Number of Columns */
Int  *lp;   /* Local precision */
#endif

{
   if (cur.vdc_type == REAL)
   {
      p->x = *pr++;  /* Corner P:  First row, First column */
      p->y = *pr++;
      q->x = *pr++;  /* Corner Q:  Last row,  Last column */
      q->y = *pr++;
      r->x = *pr++;  /* Corner R:  First row, Last column */
      r->y = *pr++;
      pi += 6;
   }
   else
   {
      p->x = *pi++;   /* Corner P: First row, First column */
      p->y = *pi++;
      q->x = *pi++;   /* Corner Q: Last row,  Last column */
      q->y = *pi++;
      r->x = *pi++;   /* Corner R: First row, Last column */
      r->y = *pi++;
   }
   *nx = *pi++;      /* Number of cells in each Row */
   *ny = *pi++;      /* Number of cells in each Column */
   *lp = *pi++;      /* Local Precision */

#ifdef DEBUG
   DMESS "CELLparget: (%f,%f),(%f,%f),(%f,%f) \n",
                      p->x,p->y,q->x,q->y,r->x,r->y);
   DMESS "CELLparget: %d by %d \n",*nx,*ny);
#endif

   return;
}
/****************************************************** CELLtran *******/
#ifdef PROTO
void CELLtran ( Point p, Point q, Point r, Int nx, Int ny, Tmatrix tm)
#else
void CELLtran ( p, q, r, nx, ny, tm )

/* Gets the Cell array transformation from the cell array parameters */

Point p,q,r;           /* Cell array corners P, Q and R */
Int   nx,ny;           /* Cell array dimensions */
Tmatrix  tm;           /* Cell array transformation matrix */
#endif

{

  if((nx > 0) && (ny > 0))
  {      /* Cell array has positive dimensions */

     /* Determine the cell array transformation (cell-space to VDC) */
     tm[0][0] = (double)((r.x-p.x)/(float)nx);
     tm[0][1] = (double)((r.y-p.y)/(float)nx);
     tm[1][0] = (double)((q.x-r.x)/(float)ny);
     tm[1][1] = (double)((q.y-r.y)/(float)ny);
     tm[2][0] = (double)(p.x);
     tm[2][1] = (double)(p.y);
#ifdef DEBUG_TM
     DMESS "CELLtran: Cell Array Transformation \n");
     DMESS "%f %f %f \n", tm[0][0],tm[1][0],tm[2][0]);
     DMESS "%f %f %f \n", tm[0][1],tm[1][1],tm[2][1]);
#endif
  }
  else
  {
#ifdef DEBUG
     DMESS "Cell Array has invalid dimensions %d by %d \n",nx,ny);
#endif
  }
  return;
}
