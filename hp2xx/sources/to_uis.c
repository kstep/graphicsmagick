/*
   Copyright (c) 1992 - 1994  Heinz W. Werntges.  All rights reserved.
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

/** to_uis.c: VAX/VMS UIS previewer of project "hp2xx"
 **
 ** 92/04/15  V 1.00  HWW  Originating, based on mandel.c (V 1.02)
 **                        Raw coding, not tested yet!
 ** 92/04/24  V 1.01  HWW  Tested and accelerated on uVAX, VMS 4.7
 ** 92/04/27  V 1.02  HWW  Cleaned up
 ** 92/05/25  V 1.02b HWW  Abort if color mode (due to lack of
 **			   test facilities) -- Color support desired!
 ** 93/09/01  V 1.02c HWW  Minor fixes (courtesy G. Steger)
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 **
 ** NOTE: Due to lack of testing facilities, I will not be able to maintain
 **       this file any longer. Volunteers are welcome!
 **/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <descrip.h>
#include <uisentry.h>
/* #include <uisusrdef.h> */
#include "bresnham.h"
#include "hp2xx.h"



int
PicBuf_to_UIS (const GEN_PAR *pg, const OUT_PAR *po)
{
int		byte_c, xoff, yoff;
unsigned long	row_c, x1, x2, rw, rh, bpp, zero=0, two=2;
const RowBuf	*row;
const PicBuf	*pb;

float		x0f, y0f, x1f, y1f, w, h;
int		c_old, c_new, i;
unsigned	vd_id, wd_id;
char		*target = "sys$workstation";
static float	intens[2] = {1.0, 0.0};
static unsigned	atb	= 1;

struct dsc$descriptor_s	s_desc;

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  if (pb->depth > 1)
  {
	Eprintf ("\nUIS preview does not support colors yet -- sorry\n");
	return ERROR;
  }

  if (!pg->quiet)
  {
	Eprintf ("\nUIS preview follows\n");
	Eprintf ("Press <return> to end\n");
  }

  xoff = po->xoff * po->dpi_x / 25.4;
  yoff = po->yoff * po->dpi_y / 25.4;

  if ((!pg->quiet) &&
      (((pb->nb << 3) + xoff > 1024) || (pb->nr + yoff > 1024)) )
  {
	Eprintf ("\n\007WARNING: Picture won't fit!\n");
	Eprintf ("Current range: (%d..%d) x (%d..%d) pels\n",
		xoff, (pb->nb << 3) + xoff, yoff, pb->nr + yoff);
	Eprintf ("Continue anyway (y/n)?: ");
	if (toupper(getchar()) != 'Y')
		return;
  }

  x0f = y0f = 0.0;			/* No offsets yet	*/
  x1f = (float) (pb->nb << 3);
  y1f = (float) pb->nr;
  w   = (float) po->width / 10.0;	/* VAX needs cm, not mm */
  h   = (float) po->height/ 10.0;

  vd_id = uis$create_display (&x0f, &y0f, &x1f, &y1f,&w, &h);
  uis$disable_display_list (&vd_id);
  uis$set_intensities (&vd_id, &zero, &two, intens);

  s_desc.dsc$w_length = strlen (target);
  s_desc.dsc$a_pointer= target;
  s_desc.dsc$b_class  = DSC$K_CLASS_S;
  s_desc.dsc$b_dtype  = DSC$K_DTYPE_T;
  wd_id = uis$create_window (&vd_id,&s_desc);

  x1 = 0;
  x2 = pb->nc;
  rw = pb->nc;
  rh = 1;
  bpp = 1;

  for (row_c = 0; row_c < pb->nr; row_c++)	/* for all pixel rows */
  {
/**
 ** Unfortunately, we need a bit reversal in each byte here:
 **/
	row = get_RowBuf (pb, row_c);
	if (row == NULL)
		continue;
	for (byte_c=0; byte_c < pb->nb; byte_c++)
	{
		c_old = row->buf[byte_c];

		if (c_old == 0)		/* all white	*/
		    continue;
		if (c_old == 0xff)	/* all black	*/
		    continue;

		for (i=0, c_new=0; ; )
		{
		    if (c_old & 1)
			c_new |= 1;
		    if (++i == 8)	/* 8 bits, 7 shifts	*/
			break;
		    c_new <<= 1;
		    c_old >>= 1;
		}
		row->buf[byte_c] = c_new;
	}

	uisdc$image(&wd_id, &atb, &x1, &row_c, &x2, &row_c,
			&rw, &rh, &bpp, row->buf);
  }
  getchar();
  uis$delete_display (&vd_id);
  return 0;
}

