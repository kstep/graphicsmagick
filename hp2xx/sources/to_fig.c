/* Copyright (c) 1994-1995 Ian McPhedran
   Parts Copyright (c) 1999  Martin Kroeker  All rights reserved.
*/   
/** to_fig.c:    Converter to the FIG format (version 3.1)
 **
 ** 94/08/08  V 1.0 IJMP  Adapted from to_vec.c
 **			  (Ian_MacPhedran@engr.USask.CA)
 ** 95/01/13  V 1.1 IJMP  Convert to FIG 3.1 from FIG 2.1
 ** 95/01/22  V 1.2 IJMP  Fix bugs for colours
 ** 99/06/12  V 1.3 MK    user-defined colours (for PenColour support)
 **/

#include <stdio.h>
#include <stdlib.h>

#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"

#define FIG_NONE 0
#define FIG_PLOT 1
#define FIG_MOVE 2

#define MAX_FIG_POINTS 500


int
to_fig (const GEN_PAR *pg, const OUT_PAR *po)
{

PlotCmd         cmd;
HPGL_Pt         pt1;
float           xcoord2mm, ycoord2mm;
FILE            *md = NULL;
int             pensize, pen_no;
int		err = 0, figmode, colour;
/*int		fig_colour[8];*/
int		i;
char            *ftype="";
int		npoints = 0;
long		x[MAX_FIG_POINTS], y[MAX_FIG_POINTS];

/* Okay how do we want to do this ?  Allocation of points for writing */

/*
  fig_colour[xxForeground] = 0;
  fig_colour[xxBlue] = 1;
  fig_colour[xxGreen] = 2;
  fig_colour[xxCyan] = 3;
  fig_colour[xxRed] = 4;
  fig_colour[xxMagenta] = 5;
  fig_colour[xxYellow] = 6;
  fig_colour[xxBackground] = 7;
*/

  if (*po->outfile != '-')
  {
	if ((md = fopen(po->outfile, "w")) == NULL)
	{
		PError("hp2xx (mf/tex)");
		err = ERROR;
		goto FIG_exit;
	}
  }
  else
  {
	md = stdout;
  }

  if (!pg->quiet)
	Eprintf ("\n\n- Writing %s code to \"%s\"\n", ftype,
		*po->outfile == '-' ? "stdout" : po->outfile);

/* Start of FIG code */
  fprintf(md,"#FIG 3.2\n");
  fprintf(md,"Landscape\nCenter\nInches\nA4\n100 100\nSingle\n-1\n");
  fprintf(md,"1200 2\n");

for (i=0;i<8;++i)
    fprintf (md,"0 %d #%2.2X%2.2X%2.2X\n",32+i, 
             pt.clut[i][0],pt.clut[i][1],pt.clut[i][2]);


  pen_no  = DEFAULT_PEN_NO;
  pensize = pt.width[pen_no];
  if (pensize != 0)
  {
	if (pg->is_color)
	  colour = 32 + pt.color[pen_no];
/*	  colour = fig_colour[pt.color[pen_no]];*/
	else
	  colour = -1;
  }
  else
  {colour = 7;}

/* mm to 1/1200 inch */
	xcoord2mm = (1200.0 / 25.4) * po->width  / (po->xmax - po->xmin);
	ycoord2mm = (1200.0 / 25.4) * po->height / ( po->ymax - po->ymin);

  figmode = FIG_NONE;

  while ((cmd = PlotCmd_from_tmpfile()) != CMD_EOF)
	switch (cmd)
	{
	  case NOP:
		break;

	  case SET_PEN:
		if ((pen_no = fgetc(pg->td)) == EOF)
		{
			PError("Unexpected end of temp. file: ");
			err = ERROR;
			goto FIG_exit;
		}
		if (figmode == FIG_PLOT)
		{
			fig_poly_end(pensize, colour, md, npoints, x, y);
			npoints = 0;
		}
		pensize = pt.width[pen_no];
		if (pensize == 0)
		{
			colour = 7; /* Draw in white */
		}
		else
		{
			if (pg->is_color)
/*	  colour = fig_colour[pg->pencolor[pen_no]];*/
			  colour = 32 + pt.color[pen_no];
			else
			  colour = -1;
		}
		if (figmode != FIG_MOVE) figmode = FIG_NONE;
		break;
 
          case DEF_PW:
                if(!load_pen_width_table(pg->td)) {
                    PError("Unexpected end of temp. file");
                    err = ERROR;
                    goto FIG_exit;
                }
		pensize=pt.width[pen_no];
                break;

	  case MOVE_TO:
		HPGL_Pt_from_tmpfile (&pt1);
		if (figmode == FIG_PLOT)          /* Finish up old polygon */
		{
			fig_poly_end(pensize, colour, md, npoints, x, y);
			npoints = 0;
		}
		figmode = FIG_MOVE;
		break;

	  case DRAW_TO:
		if (figmode == FIG_NONE)
		{ /* Problem - no current point */
			PError("No currentpoint in draw from: ");
			err = ERROR;
			goto FIG_exit;
		}
		else if (figmode == FIG_MOVE)
		{ /* Write out intro, first point, this point */
			  x[0] = (int)((pt1.x - po->xmin) * xcoord2mm);
			  y[0] = (int)((po->ymax - pt1.y) * ycoord2mm);
			npoints = 1;
		}
		/* Write out this point */
		HPGL_Pt_from_tmpfile (&pt1);
		x[npoints] = (int)((pt1.x - po->xmin) * xcoord2mm);
		y[npoints] = (int)((po->ymax - pt1.y) * ycoord2mm);
		npoints++;
		if (npoints == MAX_FIG_POINTS)
		{
			fig_poly_end(pensize, colour, md, npoints, x, y);
			npoints = 0;
		}
		figmode = FIG_PLOT;
		break;

	  case PLOT_AT:
		HPGL_Pt_from_tmpfile (&pt1);
		if (figmode == FIG_PLOT)          /* Finish up old polygon */
		{
			fig_poly_end(pensize, colour, md, npoints, x, y);
			npoints = 0;
		}
/*		if (pensize == 0) break; */

		x[0] = (int)((pt1.x - po->xmin) * xcoord2mm);
		y[0] = (int)((po->ymax - pt1.y) * ycoord2mm);
		npoints = 1;
		fig_poly_end(pensize, colour, md, npoints, x, y);
		figmode = FIG_MOVE;
		break;

	  default:
		Eprintf ("Illegal cmd in temp. file!");
		err = ERROR;
		goto FIG_exit;
	}


	if (figmode == FIG_PLOT)	/* Finish up old polygon */
	{
		fig_poly_end(pensize, colour, md, npoints, x, y);
		npoints = 0;
	}

FIG_exit:

  if (md != stdout && md != NULL)
	fclose(md);

  if (!pg->quiet)
	Eprintf ("\n");
  return err;
}

void fig_poly_end(int pensize, int colour, FILE *md, int npoints, long *x, long *y)
{ /* Write out entire polygon to file */
	int i,j;
	fprintf(md,"2 1 0 %d %d %d 0 0 -1 0.000 0 0 0 0 0 %d\n",
		pensize,colour,colour,npoints);
	j = 0;
	for (i=0;i<npoints;i++)
	{
		fprintf(md," %ld %ld",x[i],y[i]);
		if (++j > 10)
		{
			fprintf(md,"\n");
			j = 0;
		}
	}
	if (j != 0) fprintf(md,"\n");
}
