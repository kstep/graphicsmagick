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

/** to_vga.c: VGA preview part of project "hp2xx" (PC only)
 **
 ** 91/06/16  V 1.00  HWW  Originating
 ** 91/10/15  V 1.01  HWW  ANSI_C
 ** 91/11/23  V 1.01b HWW  Echo off
 ** 92/01/29  V 1.02b HWW  More flexible screen placement
 ** 92/05/24  V 2.00b HWW  Color supported
 ** 92/07/12  V 2.01a HWW  REGPACK --> REGS, intr() --> int86()
 ** 93/07/11  V 2.01b HWW  May now be included by TO_OS2.C
 ** 93/10/23  V 2.01c HWW  getchar-fix for
 **				girlich@aix520.informatik.uni-leipzig.de
 ** 94/02/14  V 2.10a HWW  Adapted to changes in hp2xx.h
 **
 ** NOTES:
 **  1)   Use PicBuf_to_VGA() as a reference for access
 **       to the picture buffer (b/w & color) in other modules.
 **
 **  2)   Color bug: Somehow, I can set all colors EXCEPT magenta. Instead
 **       of magenta I always end up with brown. Does anybody know why???
 **
 **  2)   to_vga.c may be a misnomer. See R. Emmerich's "showit" for
 **       previewing on a Hercules card. I also suspect that this code here
 **       is sufficient for EGA previews as well (just change the mode byte
 **       and adjust for DPI and screen sizes), but cannot test it (no EGA
 **       card around amymore).
 **/


#ifndef	OS2
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>
#include <conio.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "pendef.h"
#endif	/* !OS2	*/


static Byte	buf[256][3];
static unsigned	bufaddr_lo,  bufaddr_hi;



static Byte
get_VGAmode (void)
{
union	REGS	inregs;
union	REGS	outregs;

  inregs.x.ax = 0x0f00;	/* get VGA mode	*/
  int86 (0x10, &inregs, &outregs);
  return (outregs.x.ax & 0xff);
}


static void
set_VGAmode (Byte mode)
{
union	REGS	inregs;
union	REGS	outregs;

  bufaddr_lo = (unsigned) ((long) buf		& 0xffff);
  bufaddr_hi = (unsigned) (((long)buf >>16)	& 0xffff);

  inregs.x.ax = 0x0000 | mode;	/* set VGA mode	*/
  int86 (0x10, &inregs, &outregs);
}



#if 0	/* Not needed anymore	*/
static void
get_color_regs (short codenum,
		Byte *p_red, Byte *p_green, Byte *p_blue)
{
union	REGS	inregs;
union	REGS	outregs;

  inregs.x.ax = 0x1015;
  inregs.x.bx = codenum;
  int86 (0x10, &inregs, &outregs);	/* get color codes */
  *p_red   = outregs.x.dx >> 8;
  *p_green = outregs.x.cx >> 8;
  *p_blue  = outregs.x.cx & 0xff;
}
#endif


static void
set_color_regs (short codenum,
		Byte red, Byte green, Byte blue)
{
union	REGS	inregs;
union	REGS	outregs;

  inregs.x.ax = 0x1010;
  inregs.x.bx = codenum;
  inregs.x.dx =  red << 8;
  inregs.x.cx = (green << 8) | blue;
  int86 (0x10, &inregs, &outregs);
}




static void
set_pixel (unsigned x, unsigned y, Byte colorcode)
{
union	REGS	inregs;
union	REGS	outregs;

  inregs.x.ax = 0x0c00 | colorcode;	/* Write dot */
  inregs.x.bx = 0;
  inregs.x.cx = x;
  inregs.x.dx = y;
  int86 (0x10, &inregs, &outregs);
}




int
PicBuf_to_VGA (const GEN_PAR *pg, const OUT_PAR *po)
{
int	row_c, x, y, xoff, yoff, color_index, err;
RowBuf	*row;
Byte	orig_mode;
short	i;
char	c;

  err = 0;
  if (!pg->quiet)
  {
	Eprintf ( "\nVGA preview follows.\n");
	Eprintf ( "Press <return> to start and end graphics mode\n");
	SilentWait();
  }

  xoff = po->xoff * po->dpi_x / 25.4;
  yoff = po->yoff * po->dpi_y / 25.4;

  if ((!pg->quiet) &&
      (((po->picbuf->nb << 3) + xoff > 639) || (po->picbuf->nr + yoff > 480)) )
  {
	Eprintf ( "\n\007WARNING: Picture won't fit on a standard VGA!\n");
	Eprintf ( "Current range: (%d..%d) x (%d..%d) pels\n",
		xoff, (po->picbuf->nb << 3) + xoff, yoff, po->picbuf->nr + yoff);
	Eprintf ( "Continue anyway (y/n)?: ");
	c = toupper(getchar());
	if (c != '\n')
		SilentWait();
	if (c == 'N')
		return 1;
  }

  orig_mode = get_VGAmode();
  set_VGAmode (po->vga_mode);


/**
 ** CLUT setting & special VGA adjustment
 **/
  if (pg->is_color)	/* Darker background for higher color contrast	*/
  {
	for (i=xxBackground; i <= xxYellow; i++)
		set_color_regs (i, pt.clut[i][0],pt.clut[i][1],pt.clut[i][2]);
	set_color_regs((short) xxBackground, 160, 160, 160); /* GRAY	*/
  }
  else
  {
	set_color_regs((short) xxBackground, 180, 180, 180); /* LIGHT GRAY*/
	set_color_regs((short) xxForeground,   0,   0,   0); /* BLACK	*/
  }

  for (row_c=0, y=po->picbuf->nr+yoff-1; row_c < po->picbuf->nr; row_c++, y--)
  {
	row = get_RowBuf (po->picbuf, row_c);
	for (x=0; x < po->picbuf->nc; x++)
	{
		color_index = index_from_RowBuf(row, x, po->picbuf);
		if (color_index != xxBackground)
			set_pixel(x+xoff, y, (Byte) color_index);
	}
  }

  SilentWait();
  /*
  while (kbhit())
	getch();
  getch();
  */

  set_VGAmode (orig_mode);
  return err;
}

