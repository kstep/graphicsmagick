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

/** to_escp2.c: Epson Esc/P2 converter part of project "hp2xx"
 **             derived from to_pcl.c
 **
 ** 00/02/27          MK initial version based on to_pcl.c
 **/

#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "pendef.h"

#define	ESCP2_FIRST 	1			/* Bit mask! 	*/
#define	ESCP2_LAST 	2			/* Bit mask!	*/


/**
 ** Used for compression ON/off switch:
 **/
/*static	int	Deskjet_specials = TRUE;*/


/**
 ** Buffers for color treatment
 **/
static	Byte	*p_K, *p_C, *p_M, *p_Y;		/* Buffer ptrs (CMYK bits) */



/**
 ** Data & functions for (RLE) compression:
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
RLE_n_repeats(Byte *p1, int nb)
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
RLE_n_irregs (Byte *p1, int nb)
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
RLE_compress (Byte *src, Byte *dst, int nb)
{
  /**
   ** Either there is a block of repetitions or non-repeating bytes
   ** at the buffer start. If repetitions, compress them. If not,
   ** buffer them and compress next block of repetitions.
   **/
int	i, l, count=0;

  l = RLE_n_repeats (src, nb);	/* l == 0 or  l >= 2	*/
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

  l = RLE_n_irregs (src, nb);	/* l == 0 or  l >= 2	*/
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
  i = RLE_compress (src, dst, nb);	/* Recursion for rest	*/
  return (i == -1) ? -1 : i + count;
}



/**
 ** ESCP2 data compression method #2 (RLE)
 **
 ** Compress data in buf; leave compressed data there.
 ** Return number of valid bytes in buf of OK.
 ** Return -1 if no compression done.
 **/

static int
compress_buf_RLE (Byte *buf, int nb)
{

  if (p_B == NULL)		/* No buffer for compression!	*/
	return -1;

  n_B = B_EXTRASPACE;		/* Init. extra space counter	*/
  return RLE_compress (buf, p_B, nb);	/* Recursive function!	*/
}




static void
Buf_to_ESCP2 (Byte *buf, int nb, int mode, FILE *fd)
/**
 ** Output the raw bit stream
 **/
{
int	ncb;	/* Number of compressed bytes	*/
Byte	*p;	/* Buffer pointer		*/


  ncb = compress_buf_RLE (buf, nb);
  if (ncb == -1)
  {
	ncb = nb;
	p = buf;		/* Use original buffer & length	*/
	fprintf(stderr,"compression failed\n");
  }
  else
  {
	p = p_B;		/* Use compression buffer	*/
/*fprintf(stderr,"sending compressed data (%d bytes from %d bytes)\n",ncb,nb);*/
  }
  fwrite (p, ncb, 1, fd);
}




static void
KCMY_Buf_to_ESCP2 (int nb, int is_KCMY, int width, FILE *fd)
{

/*  if (is_KCMY)
  {*/
if (p_K[0] == 0 && memcmp(p_K, p_K+1,nb)==0) {
/*fprintf(stderr,"skipping empty line of black\n");*/
}else{
        putc('\r',fd); /* move print head to start of line*/
	fwrite("\033r\000",3,1,fd); /* set color black*/
	fwrite("\033.\001\005\005\001",6,1,fd);    /* announce RLE data */
	putc(width & 255, fd); /*width of raster line in pixels*/
	putc(width >> 8,fd);
   Buf_to_ESCP2 (p_K,nb,0,fd); /* compress and send black pixels */
}
/*}*/

if (p_M[0] == 0 && memcmp(p_M, p_M+1,nb)==0) {
/*fprintf(stderr,"skipping empty line of magenta\n");*/
}else{
        putc('\r',fd); /* move print head to start of line*/
	fprintf(fd,"\033r\001");   /* set color magenta */
	fwrite("\033.\001\005\005\001",6,1,fd);   /* announce RLE data */
	putc(width & 255, fd); /*width of raster line in pixels*/
	putc(width >> 8,fd);
   Buf_to_ESCP2 (p_M,nb,0,fd);
}
if (p_C[0] == 0 && memcmp(p_C, p_C+1,3*nb-1)==0) {
/*fprintf(stderr,"skipping empty line of cyan\n");*/
}else{

        putc('\r',fd);
  
	fprintf(fd,"\033r\002");   /* set color cyan */
	fwrite("\033.\001\005\005\001",6,1,fd);
	putc(width & 255, fd); /*width of raster line in pixels*/
	putc(width >> 8,fd);
   Buf_to_ESCP2 (p_C,nb,0,fd);
}
if (p_Y[0] == 0 && memcmp(p_Y, p_Y+1,3*nb-1)==0) {
/*fprintf(stderr,"skipping empty line of yellow\n");*/
}else{

        putc('\r',fd);
  
	fprintf(fd,"\033r\004");   /* set color yellow */
	fwrite("\033.\001\005\005\001",6,1,fd);
	putc(width & 255, fd); /*width of raster line in pixels*/
	putc(width >> 8,fd);
   Buf_to_ESCP2 (p_Y,nb,0,fd);
/*        putc('\r',fd);*/
}
}



#if 0
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

#endif


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

/* \033@       reset printer                */
/* \033(G      select graphics mode         */
/* \033(i00011n set microweave on/off (off) */
/* \033(U10 set unidirectional off          */   
fputs("\033@", fd);
fwrite("\033(G\001\000\001", 6, 1, fd);      /* Enter graphics mode */
/*        fwrite("\033(U\001\000\005", 5, 1, fd);*/ /*set unidirectional off*/
        fwrite("\033(U\001\000", 5, 1, fd); /* set resolution, unidirectional off*/
	size=3600/po->dpi_y;
	putc(size,fd);
         fwrite("\033(i\001\000\001", 6, 1, fd);      /* Microweave mode on */
           fwrite("\033(C\002\000", 5, 1, fd);          /* Page length */
             size = po->dpi_y * po->height ;
               putc(size & 255, fd);
                 putc(size >> 8, fd);
                 
  fwrite("\033(c\004\000", 5, 1, fd);          /* Top/bottom margins */
    size = po->dpi_y * (po->height- 10) *.003937;
      putc(size & 255, fd);
        putc(size >> 8, fd);
          size = po->dpi_y * (po->height - 10) * .003937;
      putc(size & 255, fd);
        putc(size >> 8, fd);

      fwrite("\033(V\002\000", 5, 1, fd);      /* Absolute vertical position */
            size = po->dpi_y * (po->height - 10) *.003937;
	size=10;
                  putc(size & 255, fd);
                        putc(size >> 8, fd);
                                            
                    

 }





static void
end_graphmode (FILE *fd)
{
/**
 ** End Raster Graphics
 **/
/*  fprintf(fd,"\033*rbC");*/
fprintf(fd,"\f\033@");	
}





int
PicBuf_to_ESCP2 (const GEN_PAR *pg, const OUT_PAR *po)
/**
 ** Main interface routine
 **/
{
FILE	*fd = stdout;
RowBuf	*row;
int	row_c, i, x, color_index, offset, err;
Byte	mask;
int width;

  err = 0;
  if (!pg->quiet)
	Eprintf ("\nWriting Esc/P2 output\n");
 /*
  if (po->picbuf->depth > 1 && po->specials < 3)
	Eprintf ("\nWARNING: Monochrome output despite active colors selected!\n");

  Deskjet_specials = (po->specials != 0) ? TRUE : FALSE;
*/
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
		goto ESCP2_exit;
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
		goto ESCP2_exit;
	}
  }

	init_printer (po, fd);


  /**
   ** Loop for all rows:
   ** Counting back since highest index is lowest line on paper...
   **/
			width=8*po->picbuf->nb; /*line width in pixels*/

  for (row_c = po->picbuf->nr - 1; row_c >= 0; row_c--)
  {
	if ((!pg->quiet) && (row_c % 10 == 0))
		  /* For the impatients among us ...	*/
		Eprintf(".");

	row = get_RowBuf(po->picbuf, row_c);

	if (po->picbuf->depth == 1){
        putc('\r',fd); /* move print head to start of line*/
	fwrite("\033r\000",3,1,fd); /* set color black*/
	fwrite("\033.\001\005\005\001",6,1,fd);    /* announce RLE data */
	putc(width & 255, fd); /*width of raster line in pixels*/
	putc(width >> 8,fd);
		Buf_to_ESCP2 (row->buf, po->picbuf->nb, ESCP2_FIRST | ESCP2_LAST, fd);
          fwrite("\033(v\002\000\001\000", 7, 1, fd);  
	}
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

/*		switch (po->specials)
		{
		  case 3:
			K_to_CMY (po->picbuf->nb);
			/ * drop thru	* /
		  case 4:
		  fprintf(stderr, "case 4: KCMY\n");
*/		  
/*fprintf(stderr,"calculated width %d pixels, nb is %d (%d)\n",width,po->picbuf->nb,8*po->picbuf->nb);
*/
			KCMY_Buf_to_ESCP2 (po->picbuf->nb, (po->specials == 4),width, fd);
/*			fprintf(stderr,"sent line %d from buffer\n",row_c);*/
          fwrite("\033(v\002\000\001\000", 7, 1, fd);  
 /*         
			break;
		  default:
			KCMY_to_K (po->picbuf->nb);
			Buf_to_ESCP2 (p_K, po->picbuf->nb, ESCP2_FIRST | ESCP2_LAST, fd);
			break;
		}
*/
	}
  }
/*fprintf(stderr,"end graphmode\n");*/
  end_graphmode (fd);
/*  if (po->formfeed)
	putc (FF, fd);*/
  if (!pg->quiet)
	Eprintf ("\n");
  if (fd != stdout)
	fclose (fd);

ESCP2_exit:
  if (p_Y != NULL)	free(p_Y);
  if (p_M != NULL)	free(p_M);
  if (p_C != NULL)	free(p_C);
  if (p_K != NULL)	free(p_K);

  if (p_B != NULL)	free(p_B);

  p_K = p_C = p_M = p_Y = NULL;
  return err;
}
