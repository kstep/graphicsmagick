/**
 ** to_pbm.c: PortableBitMap (PBM) converter part of project "hp2xx"
 **
 ** 92/04/14  V 1.00  CHL  Originating: Copied from to_pcx.c and to_gnu.c
 ** 92/04/16  V 1.01  CHL  Better error handling
 ** 92/05/17  V 1.01b HWW  Output to stdout if outfile == '-'
 ** 92/05/19  V 1.01c HWW  Abort if color mode
 ** 94/02/10  V 2.00  IJMP Add colour/use binary mode
 **			   (IJMP = Ian_MacPhedran@engr.usask.ca)
 ** 94/02/14  V 2.10  HWW  Adapted to changes in hp2xx.h
 **/

#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "pendef.h"
#define GGE >>=
#define MAXOUTPUTROWS 70



int
PicBuf_to_PBM (const GEN_PAR *pg, const OUT_PAR *po)
{
FILE           *fd;
int             row_c, byte_c, x;
const RowBuf   *row;
const PicBuf   *pb;
#ifdef PBMascii
int             bit, row_count = 0;
char	       *ppm[] = { "1 1 1", "0 0 0", "1 0 0", "0 1 0",
		"0 0 1", "0 1 1", "1 0 1", "1 1 0"};
#else
int	       ppm[][3] = { {255, 255, 255}, {0,0,0}, {255,0,0}, {0,255,0},
		{0,0,255},{0,255,255},{255,0,255},{255,255,0}};
#endif /*PBMascii*/
int		colour;

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  if (!pg->quiet)
	Eprintf("\nWriting PBM output: %s\n", po->outfile);
  if (*po->outfile != '-')
  {

#ifdef VAX
	if ((fd = fopen(po->outfile, WRITE_BIN, "rfm=var", "mrs=512")) == NULL)
#else
	if ((fd = fopen(po->outfile, WRITE_BIN)) == NULL)
#endif
		goto ERROR_EXIT;
  }
  else
	fd = stdout;

  if (pb->depth > 1)
  {
#ifdef PBMascii
    if (fprintf(fd, "P3\n")== EOF)
	goto ERROR_EXIT;
    if (fprintf(fd, "%d %d\n1\n", pb->nc, pb->nr)== EOF)
	goto ERROR_EXIT;
#else
    if (fprintf(fd, "P6\n")== EOF)
	goto ERROR_EXIT;
    if (fprintf(fd, "%d %d\n255\n", pb->nc, pb->nr)== EOF)
	goto ERROR_EXIT;
#endif /* PBMascii */

    for (row_c = 0; row_c < pb->nr; row_c++)
    {
	row = get_RowBuf(pb, pb->nr - row_c - 1);
	if (row == NULL)
		continue;

	for (x = 0; x < pb->nc; x++)
	{
	    colour = index_from_RowBuf(row, x, pb);
#ifdef PBMascii
	    if (fprintf(fd,"%s",ppm[colour]) == EOF) goto ERROR_EXIT;
#else
/*	    if (fprintf(fd,"%c%c%c",ppm[colour][0],ppm[colour][1],
		ppm[colour][2]) == EOF) goto ERROR_EXIT;
*/
	    if (fprintf(fd,"%c%c%c",pt.clut[colour][0],pt.clut[colour][1],
		pt.clut[colour][2]) == EOF) goto ERROR_EXIT;
#endif /* PBMascii */
#ifdef PBMascii
	    row_count++;
	    if (row_count >= MAXOUTPUTROWS)
	    {
		row_count = 0;
		if(putc('\n', fd)== EOF) goto ERROR_EXIT;
	    }
	    else
	    {
		if(putc(' ', fd)== EOF) goto ERROR_EXIT;
	    }
#endif /* PBMascii */
	}
	if ((!pg->quiet) && (row_c % 10 == 0))
	    /* For the impatients among us ...	 */
	    Eprintf(".");
#ifdef PBMascii
	row_count = 0;
	putc('\n', fd);
#endif /* PBMascii */
    }
  }
  else
  {
#ifdef PBMascii
    if (fprintf(fd, "P1\n")== EOF)
#else
    if (fprintf(fd, "P4\n")== EOF)
#endif /* PBMascii */
	goto ERROR_EXIT;
    if (fprintf(fd, "%d %d\n", (pb->nb) * 8, pb->nr)== EOF)
	goto ERROR_EXIT;

    for (row_c = 0; row_c < pb->nr; row_c++)
    {
	row = get_RowBuf(pb, pb->nr - row_c - 1);
	if (row == NULL)
		continue;

	for (byte_c = x = 0; byte_c < pb->nb; byte_c++)
#ifdef PBMascii
	{
	    for (bit = 128; bit; bit GGE 1, x++)
		if (bit & row->buf[byte_c])
		{
		    if(putc('1', fd)== EOF) goto ERROR_EXIT;
		    row_count++;
		    if (row_count >= MAXOUTPUTROWS)
		    {
			row_count = 0;
			if(putc('\n', fd)== EOF) goto ERROR_EXIT;
		    }
		}
		else
		{
		    putc('0', fd);
		    row_count++;
		    if (row_count >= MAXOUTPUTROWS)
		    {
			row_count = 0;
			if(putc('\n', fd)== EOF) goto ERROR_EXIT;
		    }
		}
	}
#else
	{
		if(putc(row->buf[byte_c], fd)== EOF) goto ERROR_EXIT;
	}
#endif /* PBMascii */
	if ((!pg->quiet) && (row_c % 10 == 0))
	    /* For the impatients among us ...	 */
	    Eprintf(".");
#ifdef PBMascii
	row_count = 0;
	putc('\n', fd);
#endif /* PBMascii */
    }
  }
  fflush(fd);

  if (!pg->quiet)
	Eprintf("\n");
  if (fd != stdout)
	fclose(fd);
  return 0;

ERROR_EXIT:
  PError ("write_PBM");
/*ERROR_EXIT_2:*/
  return ERROR;
}

