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

/** to_pcl.c: PCL converter part of project "hp2xx"
 **
 ** 91/01/19  V 1.00  HWW  Reorganized
 ** 91/01/29  V 1.01  HWW  Tested on SUN
 ** 91/02/01  V 1.02  HWW  Deskjet specials acknowledged
 ** 91/02/15  V 1.03  HWW  VAX_C support added
 ** 91/02/20  V 1.04b HWW  x & y positioning: Now absolute!
 **			   Some VAX_C changes
 ** 91/06/09  V 1.05  HWW  New options added
 ** 91/10/15  V 1.06  HWW  ANSI_C
 ** 91/10/25  V 1.07  HWW  VAX: fopen() augmentations used, open() removed
 ** 92/05/17  V 1.07b HWW  Output to stdout if outfile == '-'
 ** 92/05/19  V 1.07c HWW  Abort if color mode
 ** 92/12/23  V 1.08a HWW  Color for Deskjet (beginning)
 ** 93/04/02  V 1.08b HWW  DotBlock --> Byte
 ** 93/04/13  V 1.09a HWW  CMYK supported
 ** 93/04/25  V 1.09b HWW  End-of-raster-graphics code fixed: now ESC*rbC
 **                        This conforms with DJ550C doc. I hope it is
 **			   still compatible with other DJ models.
 **			   Please tell me if not -- I don't have all doc's.
 ** 93/07/18  V 1.10a HWW  TIFF compression
 ** 94/01/01  V 1.10b HWW  init_printer(), start_graphmode():
 **			   L. Lowe's modifications
 ** 94/02/14  V 1.20a HWW  Adapted to changes in hp2xx.h
 ** 97/12/1           MK   add initialization code for A3 paper size
 ** 99/05/10         RS/MK autoselect A4/A3/A2 paper, reduce margins
 **/

#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "pendef.h"
#include "hp2xx.h"



#define	PCL_FIRST 	1			/* Bit mask! 	*/
#define	PCL_LAST 	2			/* Bit mask!	*/


/**
 ** Used for compression ON/off switch:
 **/
static	int	Deskjet_specials = FALSE;


/**
 ** Buffers for color treatment
 **/
static	Byte	*p_K, *p_C, *p_M, *p_Y;		/* Buffer ptrs (CMYK bits) */



/**
 ** Data & functions for (TIFF) compression:
 **
 ** Note: Usually, the buffer p_B receives less data than the
 **	  original (i.e., < nb). However, "temporary" increases
 **	  are possible. To allow for them, we allocate a few extra bytes.
 **	  If the buffer eventually really grows, we won't use compression!
 **	  "n_B" keeps track of the amount of extra buffer space left.
 **/
#define	B_EXTRASPACE	16
static	Byte	*p_B;				/* Buffer for compression  */
static	int	n_B;				/* Counter for extra space */




static int
TIFF_n_repeats(Byte *p1, int nb)
/**
 **	There are "nb" bytes in buffer "p1"
 **	Return number of identical bytes in a sequence (0, 2 ... nb)
 **/
{
int	i;
Byte	*p2;

   p2 = p1+1;
   if (nb < 2 || *p2 != *p1)
	return 0;
   for (i=1; i < nb && *p1 == *p2; p1++, p2++)
	i++;
   return i;
}



static int
TIFF_n_irregs (Byte *p1, int nb)
/**
 **	There are "nb" bytes in buffer "p1"
 **	Return number of irregular (non-identical) bytes
 **	   in a sequence (0, 1, 2 ... nb)
 **/
{
int	i;
Byte	*p2;

   if (nb < 2)
	return nb;	/* 0 or 1 */
   p2 = p1+1;
   for (i=1; i < nb && *p1 != *p2; p1++, p2++)
	i++;
   return (i == nb) ? nb : i-1;
}



static int
TIFF_compress (Byte *src, Byte *dst, int nb)
{
  /**
   ** Either there is a block of repetitions or non-repeating bytes
   ** at the buffer start. If repetitions, compress them. If not,
   ** buffer them and compress next block of repetitions.
   **/
int	i, l, count=0;

  l = TIFF_n_repeats (src, nb);	/* l == 0 or  l >= 2	*/
  while (l > 128 )
  {
	*dst++ = (-127);	/* 128 repetitions	*/
	*dst++ = *src;
	count += 2;
	l   -= 128;
	nb  -= 128;
	src += 128;
	n_B += 126;		/* 128 bytes coded as 2	*/

  }
  if (l > 0)
  {
	*dst++ = 1 - l	;	/* l repetitions	*/
	*dst++ = *src;
	count += 2;
	src += l;
	nb  -= l;
	n_B += (l - 2);		/* l bytes coded as 2	*/
  }

  if (nb < 0)
	return -1;		/* should never happen	*/
  if (nb == 0)
	return count;		/* "count" bytes buffered*/


  /* Irregular sequence	*/

  l = TIFF_n_irregs (src, nb);	/* l == 0 or  l >= 2	*/
  while (l > 128 )
  {
	n_B -= 1;
	if (n_B < 0)
		return -1;	/* Buffer overflow!	*/

	*dst++ = 127;		/* 128 repetitions	*/
	for (i=0; i < 128; i++)
		*dst++ = *src++;
	count += 129;
	l   -= 128;
	nb  -= 128;

  }
  if (l > 0)
  {
	n_B -= 1;
	if (n_B < 0)
		return -1;	/* Buffer overflow!	*/

	*dst++ = l - 1	;	/* l repetitions	*/
	for (i=0; i < l; i++)
		*dst++ = *src++;
	count += (l+1);
	nb  -= l;
  }

  if (nb < 0)
	return -1;		/* should never happen		*/

  if (nb == 0)			/* At end-of-buffer: evaluate	*/
  {
	if (n_B > B_EXTRASPACE)	/* Regular exit: Return		*/
		return count;	/*   number of compressed bytes	*/
	else
		return -1;	/* Nothing gained !		*/
  }
  i = TIFF_compress (src, dst, nb);	/* Recursion for rest	*/
  return (i == -1) ? -1 : i + count;
}



/**
 ** PCL data compression method #2 (TIFF)
 **
 ** Compress data in buf; leave compressed data there.
 ** Return number of valid bytes in buf of OK.
 ** Return -1 if no compression done.
 **/

static int
compress_buf_TIFF (Byte *buf, int nb)
{
  if (Deskjet_specials == FALSE)
	return -1;		/* Plain PLC L3 does not support compression! */

  if (p_B == NULL)		/* No buffer for compression!	*/
	return -1;

  n_B = B_EXTRASPACE;		/* Init. extra space counter	*/
  return TIFF_compress (buf, p_B, nb);	/* Recursive function!	*/
}




static void
Buf_to_PCL (Byte *buf, int nb, int mode, FILE *fd)
/**
 ** Output the raw bit stream
 **   (This should be an ideal place for data compression)
 **/
{
int	ncb;	/* Number of compressed bytes	*/
Byte	*p;	/* Buffer pointer		*/

  if (mode & PCL_FIRST)
	fprintf(fd,"\033*b");

  ncb = compress_buf_TIFF (buf, nb);
  if (ncb == -1)
  {
	ncb = nb;
	p = buf;		/* Use original buffer & length	*/
	fprintf(fd,"0m");	/* No compression		*/
  }
  else
  {
	p = p_B;		/* Use compression buffer	*/
	fprintf(fd,"2m");	/* Compression method 2 (TIFF)	*/
  }

  if (mode & PCL_LAST)
	fprintf(fd,"%dW", ncb);
  else
	fprintf(fd,"%dv", ncb);

/*  Following change keeps the VAX people happy:	*/
/*fwrite (p, 1, ncb, fd);		*/
  fwrite (p, ncb, 1, fd);
}




static void
KCMY_Buf_to_PCL (int nb, int is_KCMY, FILE *fd)
{
  if (is_KCMY)
  {
	Buf_to_PCL (p_K, nb, PCL_FIRST, fd);
	Buf_to_PCL (p_C, nb, 0,         fd);
  }
  else			/* is only CMY:	*/
	Buf_to_PCL (p_C, nb, PCL_FIRST, fd);

  Buf_to_PCL (p_M, nb, 0,        fd);
  Buf_to_PCL (p_Y, nb, PCL_LAST, fd);
}




static void
KCMY_to_K (int nb)
/**
 ** Color -> B/W conversion:
 ** Any set bit will show up black
 **/
{
int	i;
Byte	*pK = p_K, *pC = p_C, *pM = p_M, *pY = p_Y;

  for (i=0; i < nb; i++)
	*pK++ |= ((*pC++ | *pM++) | *pY++);
}




static void
K_to_CMY (int nb)
/**
 ** CMYK-to-CMY conversion:
 ** Any set bit in the "black" layer sets all C,M,Y bits to emulate "black"
 **/
{
int	i;
Byte	*pK = p_K, *pC = p_C, *pM = p_M, *pY = p_Y;

  for (i=0; i < nb; i++, pK++)
  {
	*pC++ |= *pK;
	*pM++ |= *pK;
	*pY++ |= *pK;
  }
}




static void
init_printer (const OUT_PAR *po, FILE *fd)
{
int size;


	size=26; /* default to A4 paper */
	
 if ((po->width >= po->height && (po->width > 297. || po->height >210.)) || 
    (po->width < po->height && (po->height >297. || po->width >210.))) 
 	size=27; /* A3 format */
	
 if ((po->width >= po->height && (po->width > 420. || po->height >297.)) || 
    (po->width < po->height && (po->height >420. || po->width >297.))) 
 	size=28; /* A2 format */

 if ((po->width >= po->height && (po->width > 584. || po->height >420.)) || 
    (po->width < po->height && (po->height >584. || po->width >420.))) 
 	size=29; /* A1 format */


 if ((po->width >= po->height && (po->width > 820. || po->height >584.)) || 
    (po->width < po->height && (po->height >820. || po->width >584.))) 
 	size=30; /* A0 format :-) */


   /*  \033E      reset printer            */
   /*  \033&l26A  select paper size        */
   /*  \033&l0L   perforation skip off     */
   /*  \033&l0E   no top margin            */
   /*  \0339      no side margins          */
   /*  \033&a0V   vertical position  0     */
   fprintf(fd,"%cE%c&l%dA%c&l0L%c&l0E%c9%c&a0V", ESC, ESC, size, ESC, ESC, ESC, ESC);
 }





static void
start_graphmode (const OUT_PAR *po, FILE *fd)
{
/**
 ** X & Y offsets: Use "decipoints" as unit to stick to PCL level 3
 **		1 dpt = 0.1 pt = 1/720 in
 **/
  if (po->yoff != 0.0)
	fprintf(fd,"\033&a+%dV",(int)(po->yoff * 720.0 / 25.4) );
  if (po->xoff != 0.0)
	fprintf(fd,"\033&a+%dH",(int)(po->xoff * 720.0 / 25.4) );
/**
 ** Set Graphics Resolution (300 / 150 / 100 / 75):
 ** This is NO PCL level 3 feature, but LaserjetII and compatibles
 ** seem to accept it.
 **/
  fprintf(fd,"\033*t%dR", po->dpi_x);

/**
 ** Set Raster Width (in dots)
 **	Deskjet feature, good for saving internal memory!
 **/
  if (po->specials)
  {
	fprintf(fd,"\033*r%dS", po->picbuf->nc);
	switch (po->specials)
	{
	  case 4:	/* KCMY 			*/
		fprintf(fd,"\033*r-4U");
		break;
	  case 3:	/* CMY				*/
		fprintf(fd,"\033*r-3U");
		break;
	  default:	/* Single color plane		*/
		fprintf(fd,"\033*r1U");
		break;
	}
  }

/**
 ** Start Raster Graphics at current position
 ** This is NO PCL level 3 feature, but LaserjetII and compatibles
 ** seem to accept it.
 **/
  fprintf(fd,"\033*r1A");
}




static void
end_graphmode (FILE *fd)
{
/**
 ** End Raster Graphics
 **/
  fprintf(fd,"\033*rbC");	/* fix: *rB --> *rbC	*/
}





int
PicBuf_to_PCL (const GEN_PAR *pg, const OUT_PAR *po)
/**
 ** Main interface routine
 **/
{
FILE	*fd = stdout;
RowBuf	*row;
int	row_c, i, x, color_index, offset, err;
Byte	mask;

  err = 0;
  if (!pg->quiet)
	Eprintf ("\nWriting PCL output\n");

  if (po->picbuf->depth > 1 && po->specials < 3)
	Eprintf ("\nWARNING: Monochrome output despite active colors selected!\n");

  Deskjet_specials = (po->specials != 0) ? TRUE : FALSE;

  /**
   ** Allocate buffers for CMYK conversion
   **/
  if (po->picbuf->depth > 1)
  {
	p_K = calloc (po->picbuf->nb, sizeof(Byte));
	p_C = calloc (po->picbuf->nb, sizeof(Byte));
	p_M = calloc (po->picbuf->nb, sizeof(Byte));
	p_Y = calloc (po->picbuf->nb, sizeof(Byte));
	if (p_K == NULL || p_C == NULL || p_M == NULL || p_Y == NULL)
	{
		Eprintf ("\nCannot 'calloc' CMYK memory -- sorry, use B/W!\n");
		goto PCL_exit;
	}
  }
  /**
   ** Optional memory; for compression
   **/
  n_B = B_EXTRASPACE;
  p_B = calloc (po->picbuf->nb + n_B, sizeof(Byte));


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
		goto PCL_exit;
	}
  }

  if (po->init_p)
	init_printer (po, fd);

  start_graphmode (po, fd);

  /**
   ** Loop for all rows:
   ** Counting back since highest index is lowest line on paper...
   **/

  for (row_c = po->picbuf->nr - 1; row_c >= 0; row_c--)
  {
	if ((!pg->quiet) && (row_c % 10 == 0))
		  /* For the impatients among us ...	*/
		Eprintf(".");

	row = get_RowBuf(po->picbuf, row_c);

	if (po->picbuf->depth == 1)
		Buf_to_PCL (row->buf, po->picbuf->nb, PCL_FIRST | PCL_LAST, fd);
	else
	{
		for (x=0; x < po->picbuf->nb; x++)
			p_K[x] = p_C[x] = p_M[x] = p_Y[x] = 0;

		for (x=offset=0; x < (po->picbuf->nb << 3); x++, offset = (x >> 3))
		{
			color_index = index_from_RowBuf(row, x, po->picbuf);

			if (color_index == xxBackground)
				continue;
			else
			{
				mask = 0x80;
				if ((i = x & 0x07) != 0)
					mask >>= i;
					
if (pt.clut[color_index][0]+pt.clut[color_index][1]+pt.clut[color_index][2] == 0){
                *(p_K +offset) |= mask;
}else{					
		*(p_C + offset ) |= ( mask ^ ( pt.clut[color_index][0] & mask ) );
		*(p_M + offset ) |= ( mask ^ ( pt.clut[color_index][1] & mask ) );
		*(p_Y + offset ) |= ( mask ^ ( pt.clut[color_index][2] & mask ) );
}					                        
/*					
				switch (color_index)
				{
				  case xxForeground:
					*(p_K + offset) |= mask;
					break;
				  case xxRed:
					*(p_M + offset) |= mask;
					*(p_Y + offset) |= mask;
					break;
				  case xxGreen:
					*(p_C + offset) |= mask;
					*(p_Y + offset) |= mask;
					break;
				  case xxBlue:
					*(p_C + offset) |= mask;
					*(p_M + offset) |= mask;
					break;
				  case xxCyan:
					*(p_C + offset) |= mask;
					break;
				  case xxMagenta:
					*(p_M + offset) |= mask;
					break;
				  case xxYellow:
					*(p_Y + offset) |= mask;
					break;
				  default:
					break;
				}
*/
			}
		}

		switch (po->specials)
		{
		  case 3:
			K_to_CMY (po->picbuf->nb);
			/* drop thru	*/
		  case 4:
			KCMY_Buf_to_PCL (po->picbuf->nb, (po->specials == 4), fd);
			break;
		  default:
			KCMY_to_K (po->picbuf->nb);
			Buf_to_PCL (p_K, po->picbuf->nb, PCL_FIRST | PCL_LAST, fd);
			break;
		}
	}
  }

  end_graphmode (fd);
  if (po->formfeed)
	putc (FF, fd);
  if (!pg->quiet)
	Eprintf ("\n");
  if (fd != stdout)
	fclose (fd);

PCL_exit:
  if (p_Y != NULL)	free(p_Y);
  if (p_M != NULL)	free(p_M);
  if (p_C != NULL)	free(p_C);
  if (p_K != NULL)	free(p_K);

  if (p_B != NULL)	free(p_B);

  p_K = p_C = p_M = p_Y = NULL;
  return err;
}
