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

/**
 ** to_sunview.c: Sunview previewer for hp2xx
 **
 ** 92/01/28  V 1.00a HWW  Derived from project njm
 ** 92/05/19  V 1.00b HWW  Abort if color mode
 ** 92/05/25  V 1.10a HWW  Colors supported
 ** 93/01/06  V 1.10b HWW  "xx" added in front of color names
 ** 94/02/14  V 1.20a HWW  Adapted to changes in hp2xx.h
 **
 ** NOTE:     Sunview itself is a dying standard (to be replaced by OpenLook
 **           and/or X11). Therefore, I won't continue to maintain this file.
 **           Please use to_x11.c for future previewing on Suns.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <suntool/alert.h>
#include <ctype.h>
#include "bresnham.h"
#include "hp2xx.h"


#define	CMS_SIZE	16
#define	BLACK		0
#define	GRAY		1
#define	RED		2
#define	GREEN		3
#define	BLUE		4
#define	CYAN		5
#define	MAGENTA		6
#define	YELLOW		7
#define	LIGHTGRAY	8
#define	LIGHTRED	10
#define	LIGHTGREEN	11
#define	LIGHTBLUE	12
#define	LIGHTCYAN	13
#define	LIGHTMAGENTA	14
#define	WHITE		15

#define	DestX		320
#define	DestY		30


static Frame	frame;
static Canvas	canvas;

static	Pixwin	*pw;
static	int	my_op;




static void
setcolor (col)
int	col;
{
  if (col == BLACK)
	my_op = PIX_COLOR (col) | PIX_CLR;
  else
	my_op = PIX_COLOR (col) | (PIX_SRC ^ PIX_DST);
}


#if 0
void	PointW (x0, y0, color)
double x0, y0;
int color;
{
  pw_put (pw, TransX (x0), TransY (y0), color);
}


void	FillBoxW ( x0,  y0,  x1,  y1)
double x0, y0, x1, y1;
{
int	i0,i1, j0,j1;

  i0 = TransX (x0);	i1 = TransX (x1);
  j0 = TransY (y0);	j1 = TransY (y1);
  pw_rop (pw, i0,j1, i1-i0, j0-j1, my_op, NULL, 0, 0);
}
#endif


static void
Init_SunView (int xoff, int yoff, int width, int height)
{
char	st1 [100];
Rect    r;
u_char	red[CMS_SIZE], green[CMS_SIZE], blue[CMS_SIZE];

	sprintf ((char *) st1, "hp2xx Sunview previewer");
	frame = window_create(NULL, FRAME,
			WIN_X,		xoff,
			WIN_Y,		yoff,
			WIN_ERROR_MSG,	"Cannot create frame",
			FRAME_LABEL,	st1,
			0);

	canvas = window_create(frame, CANVAS,
			WIN_WIDTH,	width,
			WIN_HEIGHT,	height,
			0);
	pw = canvas_pixwin(canvas);
	window_fit (frame);

	/* Init. color map	*/

	red[BLACK] = 0;		green[BLACK] = 0;	blue[BLACK] = 0;
	red[GRAY] = 100;	green[GRAY] = 100;	blue[GRAY] = 100;
	red[RED] = 128;		green[RED] = 0;		blue[RED] = 0;
	red[GREEN]= 0;		green[GREEN]= 128;	blue[GREEN]= 0;
	red[BLUE] = 0;		green[BLUE] = 0;	blue[BLUE] = 128;
	red[CYAN] = 0;		green[CYAN] = 128;	blue[CYAN] = 128;
	red[MAGENTA] = 128;	green[MAGENTA] = 0;	blue[MAGENTA] = 128;
	red[YELLOW] = 255;	green[YELLOW] = 255;	blue[YELLOW] = 0;
	red[LIGHTGRAY] = 200;	green[LIGHTGRAY] = 200;	blue[LIGHTGRAY] = 200;
	red[LIGHTRED] = 255;	green[LIGHTRED] = 0;	blue[LIGHTRED] = 0;
	red[LIGHTGREEN]= 0;	green[LIGHTGREEN]= 255;	blue[LIGHTGREEN]= 0;
	red[LIGHTBLUE] = 0;	green[LIGHTBLUE] = 0;	blue[LIGHTBLUE] = 255;
	red[LIGHTCYAN] = 0;	green[LIGHTCYAN] = 255;	blue[LIGHTCYAN] = 255;
	red[LIGHTMAGENTA]=255;	green[LIGHTMAGENTA]=0;	blue[LIGHTMAGENTA] = 255;
	red[WHITE] = 255;	green[WHITE] = 255;	blue[WHITE] = 255;

	pw_setcmsname (pw, "MyColorMap");
	pw_putcolormap(pw, 0, CMS_SIZE, red, green, blue);
	pw_get_region_rect (pw,&r);
}



#if 0
int	user_alert (msg)
char	*msg;
{
int	result;

  result = alert_prompt (frame, (Event *)NULL,
		ALERT_MESSAGE_STRINGS, "ERROR: ", msg,
		0,
		ALERT_BUTTON_YES, "Please acknowledge",
		0);
  return (result);
}
#endif




int
PicBuf_to_Sunview (const GEN_PAR *pg, const OUT_PAR* po)
{
int		row_c, byte_c, bit, x, xoff, y, yoff;
const RowBuf	*row;
const PicBuf	*pb;

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  xoff = po->xoff * po->dpi_x / 25.4;
  yoff = po->yoff * po->dpi_y / 25.4;
  if (!pg->quiet)
  {
	Eprintf ("\nStarting preview. Use menu bar to quit!\n");
  }
  Init_SunView (xoff, yoff, pb->nc, pb->nr);
  pw_writebackground (pw, 0, 0, pb->nc, pb->nr, PIX_CLR);
  setcolor (WHITE);

  /* Backward since highest index is lowest line on screen! */
  for (row_c=0, y=pb->nr-1; row_c < pb->nr; row_c++, y--)
  {
	row = get_RowBuf (pb, row_c);
	if (row == NULL)
		continue;
	for (x=0; x < pb->nc; x++)
	{
		switch (index_from_RowBuf(row, x, pb))
		{
		case xxBackground:break;
		case xxForeground:pw_put (pw, x, y, WHITE);	break;
		case xxRed:	pw_put (pw, x, y, LIGHTRED);	break;
		case xxGreen:	pw_put (pw, x, y, LIGHTGREEN);	break;
		case xxBlue:	pw_put (pw, x, y, LIGHTBLUE);	break;
		case xxCyan:	pw_put (pw, x, y, LIGHTCYAN);	break;
		case xxMagenta:	pw_put (pw, x, y, LIGHTMAGENTA);break;
		case xxYellow:	pw_put (pw, x, y, YELLOW);	break;
		}
	}
  }
  window_main_loop (frame);
  return 0;
}

