/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.
		 1993        Horst Szillat
   All rights reserved.
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

/** to_os2.c: VGA preview part of project "hp2xx" (OS/2 full screen only)
 **
 ** 93/05/22  V 1.00  HSz, derived from HWW's to_vga v. 2.01a
 ** 93/06/05  V 1.08  HSz, DOS included
 ** 93/07/09  V 1.09  HSz, kbhit works with _int86()-bugfix
			   KBDCHARIN used in the right way
 ** 93/07/11  V 1.10  HWW  dos86 realized as macro;
			   easier cooperation with to_vga.c;
			   kbhit() not needed anymore (removed)
			   some typos fixed
 ** 94/02/14  V 1.20a HWW  Adapted to changes in hp2xx.h


	  !!! BEFORE COMPILING READ COMMENTS IN kbhit() !!!

 **
 ** NOTES:
 **  1) to_os2 was being made for the the E. Mattes OS/2 version of GNU
 **     gcc 2.3.3. with emx version 0.8f.
 **     To get an OS/2 executable compile it with ("\" means the line is
 **     continued, so you have two command lines):
 **       gcc -O to_vga.c to_mf.c to_eps.c picbuf.c chardraw.c bresnham.c\
 **       getopt.c getopt1.c to_os2.c to_pcx.c to_pcl.c to_img.c to_pbm.c\
 **       hpgl.c hp2xx.c std_main.c -o hp2xx.exe -DHAS_OS2_EMX -Wall
 **       emxbind -aq hp2xx.exe -acim
 **
 **     Comment by HWW: Use the makefile "generic.mak" if possible
 **
 **  2) Implementation restrictions:
 **       - In OS/2 mode only two colors white on black in 640x480-VGA mode are
 **         used.
 **       - DOS is supported only with a coprocessor
 **  3) The code is far from being optimized but is not runtime critical as
 **     well.
 **  4) to_os2 uses the source code of to_vga.c by including it.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>
#include <conio.h>
#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"

#define INCL_VIO
#define INCL_KBD
#include <os2.h>

#define VIOGETMODE    VioGetMode
#define VIOSETMODE    VioSetMode
#define VIOGETPHYSBUF VioGetPhysBuf
#define VIOSCRLOCK    VioScrLock
#define VIOSCRUNLOCK  VioScrUnLock
#define KBDCHARIN     KbdCharIn

static unsigned char *screen;
static VIOMODEINFO graph_mode;

/** The bound version of PicBuf_to_OS2 works as follows:
 **   o  It uses the same parameters as PicBuf_to_VGA.
 **   o  At the beginning PicBuf_to_OS2 checks the variable _osmode.
 **      If run under DOS, only PicBuf_to_VGA is called.
 **      Else it runs the OS2 code.
 **/

#define	int86(a,b,c)	_int86(a,b,c)




#define put_it(a,b) screen[a] |= (b)

static void
set_pixel_os2 (unsigned int x, unsigned int y, Byte b)
{
  if((x<graph_mode.hres)&&(y<graph_mode.vres))
  {
    switch(x%8)
    {
      case 0: put_it(x/8+graph_mode.hres*y/8,128); break;
      case 1: put_it(x/8+graph_mode.hres*y/8, 64); break;
      case 2: put_it(x/8+graph_mode.hres*y/8, 32); break;
      case 3: put_it(x/8+graph_mode.hres*y/8, 16); break;
      case 4: put_it(x/8+graph_mode.hres*y/8,  8); break;
      case 5: put_it(x/8+graph_mode.hres*y/8,  4); break;
      case 6: put_it(x/8+graph_mode.hres*y/8,  2); break;
      case 7: put_it(x/8+graph_mode.hres*y/8,  1); break;
    }
  }
};

#undef put_it



int
PicBuf_to_OS2 (const GEN_PAR *pg, const OUT_PAR* po)
{
int		row_c, x, y, xoff, yoff, color_index;
const RowBuf	*row;
const PicBuf	*pb;

VIOMODEINFO text_mode;
VIOPHYSBUF  vpb;
int status;
unsigned char rubbish;
KBDKEYINFO kki;

  if(_osmode==DOS_MODE)
	return PicBuf_to_VGA (pg, po);

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  if (!pg->quiet)
  {
	Eprintf ("\nVGA preview follows.\n");
	Eprintf ("Press <return> to start and end graphics mode\n");
	SilentWait();
  }

  xoff = po->xoff * po->dpi_x / 25.4;
  yoff = po->yoff * po->dpi_y / 25.4;

  if ((!pg->quiet) &&
      (((pb->nb << 3) + xoff > 639) || (pb->nr + yoff > 480)) )
  {
	Eprintf ("\n\007WARNING: Picture won't fit on a standard VGA!\n");
	Eprintf ("Current range: (%d..%d) x (%d..%d) pels\n",
		xoff, (pb->nb << 3) + xoff, yoff, pb->nr + yoff);
	Eprintf ("Continue anyway (y/n)?: ");
	if (toupper(getchar()) == 'N')
		return 1;
  }

  status=VIOGETMODE(&text_mode,0);
  if(status!=0){
    if (!pg->quiet)
      Eprintf ("Sorry, I have trouble with the graphics mode(1)!\n");
    return ERROR;
  }

  graph_mode.cb=14;
  graph_mode.fbType=3;
  graph_mode.color=1;
  graph_mode.col=80;
  graph_mode.row=25;
  graph_mode.hres=640;
  graph_mode.vres=480;
  graph_mode.fmt_ID=0;
  graph_mode.attrib=1;
  status=VIOSETMODE(&graph_mode,0);
  if(status!=0)
  {
    if (!pg->quiet)
      Eprintf ("Sorry, I have trouble with the graphics mode(2)!\n");
    return ERROR;
  }

  vpb.pBuf=(PBYTE)0xa0000;
  vpb.cb  =graph_mode.hres*graph_mode.vres/8;
  vpb.asel[0]=0;
  status=VIOGETPHYSBUF(&vpb,0);
  if(status!=0)
  {
    if (!pg->quiet)
      Eprintf ("Sorry, I have trouble with the graphics mode(3)!\n");
    return ERROR;
  }
  VIOSCRLOCK(1,&rubbish,0);
  screen=MAKEP(vpb.asel[0],0);
  for(x=0;x<(graph_mode.hres*graph_mode.vres/8);x++){
    screen[x]=0;
  }

  for (row_c=0, y=pb->nr+yoff-1; row_c < pb->nr; row_c++, y--)
  {
	row = get_RowBuf (pb, row_c);
	for (x=0; x < pb->nc; x++)
	{
		color_index = index_from_RowBuf(row, x, pb);
		if (color_index != xxBackground)
			set_pixel_os2 (x+xoff, y, (Byte) color_index);
	}
  }

  do{
    KBDCHARIN(&kki,1,0);
  }while( kki.fbStatus & 64 );
  KBDCHARIN(&kki,0,0);
  do{
    KBDCHARIN(&kki,1,0);
  }while( kki.fbStatus &64 );

  VIOSCRUNLOCK(0);
  status=VIOSETMODE(&text_mode,0);
  if(status!=0)
  {
    if (!pg->quiet)
      Eprintf ("Sorry, I have trouble with the graphics mode(4)!\n");
    return ERROR;
  }
  return 0;
}


#include "to_vga.c"
