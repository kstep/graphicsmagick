/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of HP2xx.

HP2xx is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the GNU General Public License, Version 2 or later, for full details.

Everyone is granted permission to copy, modify and redistribute
HP2xx, but only under the conditions described in the GNU General Public
License.  A copy of this license is supposed to have been
given to you along with HP2xx so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/** to_rgip.c:   Converter to Uniplex RGIP format;
 **    (very loosely derived from "to_eps.c")
 **
 ** By Gilles Gravier (corwin@ensta.fr)
 ** 93/07/19  V 1.00       Derived from to_eps.c
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"

static  float  ax, ay;
static  int  startx, starty, endx, endy;
static  int  lwid, lsty, lcol;
static  int  firstmove;

#define RGIPXMAX 10000.0
#define RGIPYMAX 10000.0

/**
 ** Convert HP to RGIP coordinates
 **/
static int
rgipx(float x)
{
  return (int)(x*ax);
}

static int
rgipy(float y)
{
  return (int)(y*ay);
}

/**
 ** Set line width
 **/
static void
rgip_set_linewidth (int width)
{
  lwid=width;
}

/**
 ** Set line style
 **/
static void
rgip_set_linestyle (int style)
{
  lsty=style;
}

/**
 ** Set RGB color
 **/
static void
rgip_set_color (double red, double green, double blue)
{
  float max,min,delta,r,g,b,h,s,v;
  /**
   ** Of course, here, in HP format, colors go from:
   **   0<red<1
   **   0<green<1
   **   0<blue<1
   ** Whereas in RGIP, colors are numbered 16, and are:
   **    1 RED
   **    2 YELLOW
   **    3 GREEN
   **    4 CYAN
   **    5 BLUE
   **    6 MAGENTA
   **    7 WHITE
   **    8 BLACK
   **    9 LRED
   **   10 LYELLOW
   **   11 LGREEN
   **   12 LCYAN
   **   13 LBLUE
   **   14 LMAGENTA
   **   15 LGREY
   **   16 DGREY
   ** Some conversion needed...
   **/

  /**
   ** RGB to HSV conversion routine inspired from John Bradley
   ** (bradley@cis.upenn.edu) XV version 3.0
   **/

  if (red>=green)
  {
    if (red>=blue) max=(float)red;
    else max=(float)blue;
  }
  else
  {
    if (green>=blue) max=(float)green;
    else max=(float)blue;
  }
  if (red<=green)
  {
    if (red<=blue) min=(float)red;
    else min=(float)blue;
  }
  else
  {
    if (green<=blue) min=(float)green;
    else min=(float)blue;
  }
  delta=max-min;
  v=max;
  h=(-1);
  s=0.0;
  if (max!=0.0)
  {
    s=delta/max;
    if (s!=0.0)
    {
      r=(max-red)/delta;
      g=(max-green)/delta;
      b=(max-blue)/delta;
      if (red==max) h=b-g;
      else if (green==max) h=2+r-b;
      else h=4+g-r;
      h=h*60;
      if (h<0) h=h+360;
    }
  }
  /**
   ** Assign correct RGIP color...
   ** First case: if saturation reasonably high, then try for one of the
   ** main colors... then check if it's light... or dark...
   ** In case, prepare a default color...
   **/
  lcol=7;
  if(s>=.2)
  {
    if(((h>=0)&&(h<30))||(h>=330)) lcol=1;
    else if ((h>=30)&&(h<90)) lcol=2;
    else if ((h>=90)&&(h<150)) lcol=3;
    else if ((h>=150)&&(h<210)) lcol=4;
    else if ((h>=210)&&(h<270)) lcol=5;
    else lcol=6;
  }
  /**
   ** If not enough saturated, try for black, greys or white
   **/
  else
  {
    /**
     ** If only one color and this color is not saturated, then it
     ** is the default (black on white)...
     **/
    if (delta==0) lcol=7;
    else
    {
      if (v<0.15) lcol=8;
      else if (v<0.5) lcol=16;
      else if (v<0.75) lcol=15;
      else lcol=7;
    }
  }
}

/**
 ** Higher-level interface: Output Uniplex RGIP format
 **/

int
to_rgip (const GEN_PAR* pg, const OUT_PAR* po)
{
PlotCmd  cmd;
HPGL_Pt  pt1 = {0};
FILE  *md;
int  pensize, pencolor, pen_no=0, err = 0;

  /* Give some news... */
  if (!pg->quiet)
  {
    Eprintf ("\n\n- Writing RGIP code to \"%s\"\n",
	    *po->outfile == '-' ? "stdout" : po->outfile);
  }

  /* Init. of RGIP file: */
  if (*po->outfile != '-')
  {
    if ((md = fopen(po->outfile, "w")) == NULL)
    {
      PError("hp2xx (rgip)");
      err = ERROR;
      goto RGIP_exit;
    }
  }
  else
  {
    md = stdout;
  }

  /* A begining is a very delicate time */
  firstmove=1;

  /* Factor for transformation of HP coordinates to RGIP */
  ax=RGIPXMAX/(po->xmax - po->xmin);
  ay=RGIPYMAX/(po->ymax - po->ymin);

  /* RGIP header */
  fprintf(md,"%%RGIP_METAFILE  :1.0a\n");
  pensize = pt.width[DEFAULT_PEN_NO];
  if (pensize != 0)
  {
    rgip_set_linewidth (pensize);
  }
  rgip_set_linestyle(1);
  rgip_set_color(1.0,1.0,1.0);

/**
 ** Command loop: While temporary file not empty process command.
 **/

  while ((cmd = PlotCmd_from_tmpfile()) != CMD_EOF)
  {
    switch (cmd)
    {
      case NOP:
	break;

      case SET_PEN:
	if ((pen_no = fgetc(pg->td)) == EOF)
	{
	  PError("Unexpected end of temp. file: ");
	  exit (ERROR);
	}
	pensize = pt.width[pen_no];
	if (pensize != 0)
	{
	  rgip_set_linewidth (pensize);
	}
	pencolor = pt.color[pen_no];
	rgip_set_color (pt.clut[pencolor][0]/255.0,
			pt.clut[pencolor][1]/255.0,
			pt.clut[pencolor][2]/255.0);
	break;

      case DEF_PW:
         if(!load_pen_width_table(pg->td)) {
             PError("Unexpected end of temp. file");
	     exit (ERROR);
         }
	pensize = pt.width[pen_no];
	if (pensize != 0)
	{
	  rgip_set_linewidth (pensize);
	}
	break;

      case MOVE_TO:
	HPGL_Pt_from_tmpfile (&pt1);
	if (firstmove==1)
	{
	  firstmove=0;
	}
	else
	{
	  fprintf(md,"%%%%RI_GROUPEND\n");
	}
	startx=rgipx(((&pt1)->x) - po->xmin);
	starty=rgipy(((&pt1)->y) - po->ymin);
	fprintf(md,"%%%%RI_GROUPSTART\n");
	break;

      case DRAW_TO:
	HPGL_Pt_from_tmpfile (&pt1);
	if (pensize != 0)
	  {
	    endx=rgipx(((&pt1)->x) - po->xmin);
	    endy=rgipy(((&pt1)->y) - po->ymin);
	    fprintf(md,
		    "%d %d %d %d %d %d %d LINE\n",
		    startx, starty, endx, endy, lwid, lsty, lcol);
	    startx=endx;
	    starty=endy;
	  }
	break;

      case PLOT_AT:
	HPGL_Pt_from_tmpfile (&pt1);
	if (pensize != 0)
	{
	  startx=rgipx(((&pt1)->x) - po->xmin);
	  starty=rgipy(((&pt1)->y) - po->ymin);
	  fprintf(md,
		  "[ %d %d ] %d DOTS\n",
		  startx, starty, lcol);

	}
	break;

      default:
	Eprintf ("Illegal cmd in temp. file!");
	err = ERROR;
	goto RGIP_exit;
    }
  }

    /* Finish up */
  fprintf(md,"%%%%RI_GROUPEND\n");

RGIP_exit:
  if (md != NULL && md != stdout)
	fclose (md);
  if (!pg->quiet)
	Eprintf ("\n");
  return err;
}

