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

/** bresnham.c: Implementation of Bresenham's algorithm
 **
 ** 91/01/04  V 1.00  HWW  Due to pseudocode in D.F. Rogers (1986) McGraw Hill
 ** 91/10/15  V 1.01  HWW  ANSI_C
 **/

#define	TEST	0



#include <stdio.h>
#include "bresnham.h"


static	DevPt	p_act;
static	int	dx, dy, s1, s2, swapdir, err, i;


DevPt	*bresenham_init (DevPt *pp1, DevPt *pp2)
/**
 ** Init. generation of a straight line between *pp1 & *pp2
 **
 ** Returns pointer to internally generated actual point of line.
 ** Use this ptr for further reference! Example of use:
 **
 **	..
 **	#include <share/bresnham.h>
 **	...
 **	DevPt	p1, p2, *pp;
 **	...
 **	pp = bresenham_init (&p1, &p2);
 **	do {
 **		plot (pp);
 **	} while (bresenham_next() != BRESENHAM_ERR);
 **/
{
  p_act = *pp1;

  if ((dx = pp2->x - pp1->x) != 0)
  {
	if (dx < 0)
	{
		dx = -dx;
		s1 = -1;
	}
	else
		s1 = 1;
  }
  else
	s1 = 0;	/* dx = abs(x2-x1), s1 = sign(x2-x1)	*/

  if ((dy = pp2->y - pp1->y) != 0)
  {
	if (dy < 0)
	{
		dy = -dy;
		s2 = -1;
	}
	else
		s2 = 1;
  }
  else
	s2 = 0;	/* dy = abs(y2-y1), s2 = sign(y2-y1)	*/

  if (dy > dx)
  {
	swapdir = dx;	/* use swapdir as temp. var.	*/
	dx = dy;
	dy = swapdir;
	swapdir = 1;
  }
  else
	swapdir = 0;

  i   = dx;		/* Init. of loop cnt	*/
  dy <<=1;
  err = dy - dx;	/* Init. of error term	*/
  dx <<=1;

  return &p_act;
}



int	bresenham_next (void)
/**
 ** Move actual point to next position (if possible)
 **
 ** Returns 0		 if ok,
 **	   BRESENHAM_EOL if last point reached (p_act == *pp2),
 **	   BRESENHAM_ERR else (e.g. if moving past EOL attempted)
 **/
{
  if (i<=0)
	return (BRESENHAM_ERR);	/* Beyond last point! */

  while (err >= 0)
  {
	if (swapdir)
		p_act.x += s1;
	else
		p_act.y += s2;
	err -=  dx;
  }
  if (swapdir)
	p_act.y += s2;
  else
	p_act.x += s1;
  err +=  dy;

  i--;	/* i==0 indicates "last point reached"	*/
  return ((i) ? 0 : BRESENHAM_EOL);
}



	/* Test module */
#if TEST
#ifdef __TURBOC__ && __MSDOS__


#include <graphics.h>


void	b_line (DevPt *pp1, DevPt *pp2, int col)
{
DevPt	*pp;

 pp = bresenham_init (pp1, pp2);
 do {
	putpixel (pp->x, pp->y, col);
 } while (bresenham_next() != BRESENHAM_ERR);
}


void	main(void)
{
int	gdriver=DETECT, gmode, MaxX, MaxY;
DevPt	pa, pc;

  initgraph (&gdriver, &gmode, "");

  MaxX = getmaxx();
  MaxY = getmaxy();

  pc.x = (MaxX + 1) >> 1;
  pc.y = (MaxY + 1) >> 1;

  for (pa.x=0, pa.y=0; pa.x < MaxX ; pa.x += 4)
	b_line (&pc, &pa, RED);
  for (pa.x=MaxX, pa.y=0; pa.y < MaxY ; pa.y += 3)
	b_line (&pc, &pa, GREEN);

  for (pa.x=MaxX, pa.y=MaxY; pa.x >= 0 ; pa.x -= 4)
	b_line (&pc, &pa, LIGHTRED);
  for (pa.x=0, pa.y=MaxY; pa.y >= 0 ; pa.y -= 3)
	b_line (&pc, &pa, LIGHTGREEN);

  getchar();
}

#endif
#endif
