/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
   Parts Copyright (c) 1999  Martin Kroeker  All rights reserved.
   
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

/** to_pcx.c: PCX converter part of project "hp2xx"
 **
 ** 91/01/19  V 1.00  HWW  Originating: Format accepted by MS-Paintbrush,
 **                       but not by emTeX drivers
 **                       Use MS-Paintbrush "load/save" for conversion
 ** 91/02/15  V 1.01  HWW  VAX_C support added (not tested yet!)
 ** 91/02/18  V 1.02  HWW  PCX format: no zero run length allowed
 ** 91/02/20  V 1.03  HWW  Some VAX_C changes, debugged
 ** 91/06/09  V 1.04  HWW  New options added
 ** 91/06/16  V 1.05  HWW  Writing of PCX header now machine-independent
 ** 91/10/15  V 1.06  HWW  ANSI_C
 ** 91/10/25  V 1.07  HWW  VAX: fopen() augmentations used, open() removed
 ** 92/05/17  V 1.07b HWW  Output to stdout if outfile == '-'
 ** 92/05/19  V 1.07c HWW  Abort if color mode
 ** 92/06/08  V 1.08a HWW  First color version
 ** 93/11/22  V 1.10a HWW  Color version based on TO_PCL.C code. CLUT still
 **                        inactive !?
 ** 93/11/25  V 1.10b RF   PCX-Version set to 2: use palette info,
 **                        colors corrected
 ** 94/02/14  V 1.20a HWW  Adapted to changes in hp2xx.h
 **
 **           NOTE: According to my tests, setting of the
 **                 color lookup table is ignored by other programs,
 **                 so this code is *preliminary* when color is used.
 **                 Correct colors appeared only if the color setting corresponded to
 **                 PC conventions...
 **/


#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"


typedef enum {PCX_INIT, PCX_NORMAL, PCX_EXIT}
        PCXmode;




static void
RLEcode_to_file (int c, int repeat, FILE *fd)
{
  if ((repeat==1) && ((c & 0xC0) != 0xC0))
  {
        if (putc (c, fd) == EOF)
        {
		PError("RLEcode_to_file (1)");
                exit    (ERROR);
        }
  }
  else
  {
        if (putc (repeat | 0xC0, fd) == EOF)
        {
		PError("RLEcode_to_file (2)");
                exit    (ERROR);
        }
        if (putc (c, fd) == EOF)
        {
		PError("RLEcode_to_file (3)");
                exit    (ERROR);
        }
  }
}




static void
byte_to_PCX (Byte b, PCXmode mode, FILE *fd)
{
static int last_b, rept;

  switch (mode)
  {
  case PCX_NORMAL:
        if (b == last_b)
        {
                if (++rept == 63)
                {
                        RLEcode_to_file (last_b, rept, fd);
                        rept = 0;
                }
        }
        else
        {
                if (rept)
                        RLEcode_to_file (last_b, rept, fd);
                rept    = 1;
                last_b  = b;
        }
        break;

  case PCX_INIT:
        rept = 0;
        last_b = -2;    /* Init to impossible value     */
        break;

  case PCX_EXIT:
        if (rept)
        {
                RLEcode_to_file (last_b, rept, fd);
                rept = 0;
        }
        break;
  }
}





typedef struct {
        char    creator, version, encoding, bits;
        short   xmin, ymin, xmax, ymax, hres, vres;
        char    palette[16][3], vmode, planes;
	short   byteperline, paletteinfo;
        char    dummy[58];
} PCXheader;



static int
start_PCX (const OUT_PAR *po, const GEN_PAR *pg, FILE *fd)
{
PCXheader       h;
int             i , j ;

  h.creator     = 0x0A;         /* ZSoft label                  */
  h.version     = '\002';       /* V 2.8/3.0, with palette info */
  h.encoding    = 1;            /* RLE                          */
  h.bits        = 1;            /* Bits per pixel               */
  h.xmin        = 0;            /* Range of bitmap              */
  h.ymin        = 0;
  h.xmax        = po->picbuf->nc - 1;
  h.ymax        = po->picbuf->nr - 1;
  h.hres        = po->dpi_x;     /* Resolution                   */
  h.vres        = po->dpi_y;

/**  for (i=0; i<8; i++)
     for (j=0; j<3; j++)
     {
	h.palette[i  ][j] = 255-p->Clut[i][j];
	h.palette[i+8][j] = 255-p->Clut[i][j];
     }  RF **/

  h.palette[ 0][0] =   0; h.palette[ 0][1] =   0; h.palette[ 0][2] =   0; /* white */
/*  h.palette[ 1][0] = 128; h.palette[ 1][1] =   0; h.palette[ 1][2] =   0;
  gave an ugly red background in b/w mode ? */
  h.palette[ 1][0] = 255; h.palette[ 1][1] = 255; h.palette[ 1][2] = 255;
  h.palette[ 2][0] =   0; h.palette[ 2][1] = 128; h.palette[ 2][2] =   0;
  h.palette[ 3][0] = 128; h.palette[ 3][1] = 128; h.palette[ 3][2] =   0;
  h.palette[ 4][0] =   0; h.palette[ 4][1] =   0; h.palette[ 4][2] = 128;
  h.palette[ 5][0] = 128; h.palette[ 5][1] =   0; h.palette[ 5][2] = 128;
  h.palette[ 6][0] =   0; h.palette[ 6][1] = 128; h.palette[ 6][2] = 128;
  h.palette[ 7][0] = 192; h.palette[ 7][1] = 192; h.palette[ 7][2] = 192;
/*  h.palette[ 8][0] = 128; h.palette[ 8][1] = 128; h.palette[ 8][2] = 128; gray*/
  h.palette[ 8][0] =   0; h.palette[ 8][1] =   0; h.palette[ 8][2] =   0; /*black*/
  h.palette[ 9][0] = 255; h.palette[ 9][1] =   0; h.palette[ 9][2] =   0; /* red*/
  h.palette[10][0] =   0; h.palette[10][1] = 255; h.palette[10][2] =   0; /* green */
  h.palette[11][0] = 255; h.palette[11][1] = 255; h.palette[11][2] =   0;
  h.palette[12][0] =   0; h.palette[12][1] =   0; h.palette[12][2] = 255; /* blue */
  h.palette[13][0] = 255; h.palette[13][1] =   0; h.palette[13][2] = 255;
  h.palette[14][0] =   0; h.palette[14][1] = 255; h.palette[14][2] = 255;
  h.palette[15][0] = 255; h.palette[15][1] = 255; h.palette[15][2] = 255; 

/****************************************************************
     for (i=0; i<16; i++)
     for (j=0; j<3; j++)
     {
	h.palette[i  ][j] = pg->Clut[ pg->pencolor[i] ][j];
     }  
*******************************        MK     *****************/

  h.vmode       = 0;            /* Reserved                        */
  h.planes      = po->picbuf->depth; /* Number of color planes	   */
  h.byteperline = po->picbuf->nb;    /* Number of bytes per line   */
  h.paletteinfo = 1;            /* 1 = color & b/w, 2 = gray scale */
  for (i=0; i<58; )             /* Filler for a max. of 128 bytes  */
  {
        h.dummy[i++] = 'H';
        h.dummy[i++] = 'W';
  }

/**
 ** For complete machine independence, a bytewise writing of this header
 ** is mandatory. Else, fill bytes or HIGH/LOW-endian machines must be
 ** considered. A simple "fwrite(h,128,1,fd)" may not suffice!
 **/

  if (fputc (h.creator, fd) == EOF)             goto ERROR_EXIT;
  if (fputc (h.version, fd) == EOF)             goto ERROR_EXIT;
  if (fputc (h.encoding,fd) == EOF)             goto ERROR_EXIT;
  if (fputc (h.bits,    fd) == EOF)             goto ERROR_EXIT;
  if (fputc ((h.xmin & 0xff),   fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.xmin >> 8),     fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.ymin & 0xff),   fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.ymin >> 8),     fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.xmax & 0xff),   fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.xmax >> 8),     fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.ymax & 0xff),   fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.ymax >> 8),     fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.hres & 0xff),   fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.hres >> 8),     fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.vres & 0xff),   fd) == EOF)     goto ERROR_EXIT;
  if (fputc ((h.vres >> 8),     fd) == EOF)     goto ERROR_EXIT;
  if (fwrite((VOID *) h.palette,48,1,fd) != 1)  goto ERROR_EXIT;
  if (fputc (h.vmode,   fd) == EOF)             goto ERROR_EXIT;
  if (fputc (h.planes,  fd) == EOF)             goto ERROR_EXIT;
  if (fputc ((h.byteperline & 0xff),fd) == EOF) goto ERROR_EXIT;
  if (fputc ((h.byteperline >> 8),  fd) == EOF) goto ERROR_EXIT;
  if (fputc ((h.paletteinfo & 0xff),fd) == EOF) goto ERROR_EXIT;
  if (fputc ((h.paletteinfo >> 8),  fd) == EOF) goto ERROR_EXIT;
  if (fwrite((VOID *) h.dummy,58,1,fd) != 1)    goto ERROR_EXIT;
  return 0;

ERROR_EXIT:
  PError ("start_PCX");
  return ERROR;
}



static void
Buf_to_PCX(Byte* pb, int nb, FILE* fd)
{
int     x;

  byte_to_PCX (0, PCX_INIT, fd);
  for (x=0; x < nb; x++)
	byte_to_PCX (~*pb++, PCX_NORMAL, fd);
  byte_to_PCX (0, PCX_EXIT, fd);        /* Flush        */
}



int
PicBuf_to_PCX (const GEN_PAR *pg, const OUT_PAR *po)
{
FILE    *fd=NULL;
RowBuf  *row=NULL;
int     row_c, i, x, color_index, offset, err;
Byte    mask, *p_R=NULL, *p_G=NULL, *p_B=NULL, *p_I=NULL;

  err = 0;
  if (!pg->quiet)
	Eprintf ("\nWriting PCX output\n");
  if (*po->outfile != '-')
  {
#ifdef VAX
	if ((fd = fopen(po->outfile, WRITE_BIN, "rfm=var","mrs=512")) == NULL)
	{
#else
	if ((fd = fopen(po->outfile, WRITE_BIN)) == NULL)
	{
#endif
		PError ("hp2xx -- opening output file");
		return ERROR;
	}
  }
  else  fd = stdout;

  if (start_PCX (po, pg, fd))
  {
	err = ERROR;
	goto PCX_exit;
  }

#if 0
  /* Backward since highest index is lowest line on screen! */
  for (row_c = po->picbuf->nr - 1; row_c >= 0; row_c--)
  {
	if ((!pg->quiet) && (row_c % 10 == 0))
		  /* For the impatient among us ...     */
		Eprintf(".");
	row = get_RowBuf (po->picbuf, row_c);
	byte_to_PCX (0, PCX_INIT, fd);
	pb = row->buf;
	for (np=0; np < picbuf->depth; np++)
		for (x=0; x < po->picbuf->nb; x++)
			byte_to_PCX (~*pb++, PCX_NORMAL, fd);
	byte_to_PCX (0, PCX_EXIT, fd);  /* Flush        */
  }
#endif

  /**
   ** Allocate buffers for temporary conversion
   **/
  if (po->picbuf->depth > 1)
  {
	p_I = calloc (po->picbuf->nb, sizeof(Byte));
	p_B = calloc (po->picbuf->nb, sizeof(Byte));
	p_G = calloc (po->picbuf->nb, sizeof(Byte));
	p_R = calloc (po->picbuf->nb, sizeof(Byte));
	if (p_I == NULL || p_B == NULL || p_G == NULL || p_R == NULL)
	{
		Eprintf(
		  "\nCannot 'calloc' color conversion memory -- sorry, use B/W!\n");
		err = ERROR;
		goto PCX_exit;
	}
  }
  /**
   ** Loop for all rows:
   ** Counting back since highest index is lowest line on paper...
   **/

  for (row_c = po->picbuf->nr - 1; row_c >= 0; row_c--)
  {
	if ((!pg->quiet) && (row_c % 10 == 0))
		  /* For the impatients among us ...    */
		Eprintf(".");

	row = get_RowBuf(po->picbuf, row_c);
	byte_to_PCX (0, PCX_INIT, fd);

	if (po->picbuf->depth == 1)
		Buf_to_PCX (row->buf, po->picbuf->nb, fd);
	else
	{
		for (x=0; x < po->picbuf->nb; x++)
			p_I[x] = p_R[x] = p_G[x] = p_B[x] = 0;

		for (x=offset=0; x < (po->picbuf->nb << 3); x++, offset = (x >> 3))
		{
			color_index = index_from_RowBuf(row, x, po->picbuf);

				mask = 0x80;
				if ((i = x & 0x07) != 0)
					mask >>= i;

/****	*(p_R + offset ) |= ( mask ^ ( pg->Clut[color_index][0] & mask ) );
	*(p_G + offset ) |= ( mask ^ ( pg->Clut[color_index][1] & mask ) );
        *(p_B + offset ) |= ( mask ^ ( pg->Clut[color_index][2] & mask ) ); 
****/



				switch (color_index)
				{
				  case xxForeground:
					*(p_I + offset) |= mask;
					*(p_R + offset) |= mask;
					*(p_G + offset) |= mask;
					*(p_B + offset) |= mask;
					break;
				  case xxRed:
					*(p_G + offset) |= mask;
					*(p_B + offset) |= mask;
					break;
				  case xxGreen:
					*(p_R + offset) |= mask;
					*(p_B + offset) |= mask;
					break;
				  case xxBlue:
					*(p_G + offset) |= mask;
					*(p_R + offset) |= mask;
					break;
				  case xxCyan:
					*(p_R + offset) |= mask;
					break;
				  case xxMagenta:
					*(p_G + offset) |= mask;
					break;
				  case xxYellow:
					*(p_B + offset) |= mask;
					break;
				  default:
					break;
				}
		}
		Buf_to_PCX (p_R, po->picbuf->nb, fd);
		Buf_to_PCX (p_G, po->picbuf->nb, fd);
		Buf_to_PCX (p_B, po->picbuf->nb, fd);
		Buf_to_PCX (p_I, po->picbuf->nb, fd);
	}
  }
  if (pg->is_color && !pg->quiet)
	Eprintf ("\nWARNING: PCX colors should be OK now!\n");  /* RF */

  if (!pg->quiet)
	Eprintf("\n");

PCX_exit:
  if (fd != stdout)
	fclose (fd);

  if (p_R != NULL)      free(p_R);
  if (p_G != NULL)      free(p_G);
  if (p_B != NULL)      free(p_B);
  if (p_I != NULL)      free(p_I);
  return err;
}


