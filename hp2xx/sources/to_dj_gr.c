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

/** to_dj_gr.c: GNU-C++ (DJ's DOS port) preview part of project "hp2xx",
 **             based on DJ Delorie's grapics lib "gr"
 **
 ** 92/01/29  V 1.00  HWW  Originating
 ** 92/05/19  V 1.01  HWW  Abort if color mode
 ** 92/05/25  V 1.02  HWW  B/W mode also if color; index_from_RowBuf() used
 ** 92/06/12  V 1.02c HWW  getchar(), B/W warning ...
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 **
 ** NOTE: This previewer worked fine on my machine. However, I do not intend
 **       to maintain DJ Delorie's go32 DOS extender version of hp2xx in the
 **       future as I favor E. Mattes' extender "emx" which supports OS/2
 **	  as well.
 **
 **       For those of you who are familiar with go32 and don't need
 **	  OS/2 support -- this previewer is what you might prefer.
 **/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <graphics.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "pendef.h"



int
PicBuf_to_DJ_GR (const GEN_PAR *pg, const OUT_PAR *po)
{
int		row_c, i, x, xoff, y, yoff, color_index;
const PicBuf	*pb;
const RowBuf	*row;

  if (!pg->quiet)
  {
	Eprintf ("\nDJ_GR preview follows.\n");
	Eprintf ("Press <return> to start and end graphics mode\n");
	SilentWait();
  }
  pb = po->picbuf;

  xoff = po->xoff * po->dpi_x / 25.4;
  yoff = po->yoff * po->dpi_y / 25.4;

  if ((!pg->quiet) &&
      (((pb->nb << 3) + xoff > 639) || (pb->nr + yoff > 480)) )
  {
	Eprintf ("\n\007WARNING: Picture won't fit on a standard VGA!\n");
	Eprintf ("Current range: (%d..%d) x (%d..%d) pels\n",
		xoff, (pb->nb << 3) + xoff, yoff, pb->nr + yoff);
	Eprintf ("Continue anyway (y/n)?: ");
	while (getchar() != '\n')
		;      /* Simple: Chance for ^C */
  }

  if (pg->is_color)
	GrSetColor(0, 160, 160, 160);
  else
	GrSetColor(0, 180, 180, 180);

  GrSetColor(1, 0, 0, 0);
  for (i=2; i < 8; i++)  /* assuming that we indeed get indices 2 ... 7 */
	if (i!=GrAllocColor(pt.clut[i][0], pt.clut[i][1], pt.clut[i][2]))
		Eprintf ("WARNING: Color code %d may yield wrong color!\n", i);

  GrSetMode (GR_default_graphics, 800, 600);

  for (row_c=0, y=pb->nr+yoff-1; row_c < pb->nr; row_c++, y--)
  {
	row = get_RowBuf (pb, row_c);
	if (row == NULL)
		continue;
	for (x=0; x < pb->nc; x++)
	{
		color_index = index_from_RowBuf(row, x, pb);
		if (color_index != xxBackground)
			GrPlot(x+xoff, y, color_index);
	}
  }

  SilentWait();
  GrSetMode (GR_80_25_text, 80, 25);
  return 0;
}

