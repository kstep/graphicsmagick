/*
   Copyright (c) 1991 - 1993 Claus H. Langhans.  All rights reserved.
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
 ** to_pbm.c: PortableBitMap (PBM) converter part of project "hp2xx"
 **
 ** 92/04/14  V 1.00  CHL  Originating: Copied from to_pcx.c and to_gnu.c
 ** 92/04/16  V 1.01  CHL  Better error handling
 ** 92/05/17  V 1.01b HWW  Output to stdout if outfile == '-'
 ** 92/05/19  V 1.01c HWW  Abort if color mode
 **/


#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"

#define GGE >>=
#define MAXOUTPUTROWS 70



int
PicBuf_to_PBM (const GEN_PAR* pg, const OUT_PAR* po)
{
FILE           *fd = NULL;
int             err = 0, row_count = 0;
int             row_c, byte_c, bit, x;
RowBuf         *row;

  if (po->picbuf->depth > 1)
  {
	Eprintf ( "\nPBM mode does not support colors yet -- sorry\n");
	err = ERROR;
	goto ERROR_EXIT_2;
  }

  if (!pg->quiet)
	Eprintf ( "\nWriting PBM output: %s\n",po->outfile);
  if (*po->outfile != '-')
  {

#ifdef VAX
	if ((fd = fopen(po->outfile, WRITE_BIN, "rfm=var", "mrs=512")) == NULL)
#else
	if ((fd = fopen(po->outfile, WRITE_BIN)) == NULL)
#endif
	{
		err = ERROR;
		goto ERROR_EXIT;
	}
  }
  else
	fd = stdout;

  if (fprintf(fd, "P1\n")== EOF)
  {
	err = ERROR;
	goto ERROR_EXIT;
  }
  if (fprintf(fd, "%d %d\n", (po->picbuf->nb) * 8, po->picbuf->nr)== EOF)
  {
	err = ERROR;
	goto ERROR_EXIT;
  }

  for (row_c = 0; row_c < po->picbuf->nr; row_c++)
  {
	row = get_RowBuf(po->picbuf, po->picbuf->nr - row_c - 1);

	for (byte_c = x = 0; byte_c < po->picbuf->nb; byte_c++)
	{
	    for (bit = 128; bit; bit GGE 1, x++)
		if (bit & row->buf[byte_c])
		{
		    if(putc('1', fd)== EOF)
		    {
			err = ERROR;
			goto ERROR_EXIT;
		    }
		    row_count++;
		    if (row_count >= MAXOUTPUTROWS)
		    {
			row_count = 0;
			if(putc('\n', fd)== EOF)
			{
				err = ERROR;
				goto ERROR_EXIT;
			}
		    }
		}
		else
		{
		    putc('0', fd);
		    row_count++;
		    if (row_count >= MAXOUTPUTROWS)
		    {
			row_count = 0;
			if(putc('\n', fd)== EOF)
			{
				err = ERROR;
				goto ERROR_EXIT;
			}
		    }
		}
	}
	if ((!pg->quiet) && (row_c % 10 == 0))
	    /* For the impatients among us ...	 */
	    putc('.', stderr);
	row_count = 0;
	putc('\n', fd);
  }

  if (!pg->quiet)
	Eprintf ("\n");

ERROR_EXIT:
  PError ("write_PBM");
ERROR_EXIT_2:
  if (fd != NULL && fd != stdout)
	fclose(fd);
  return err;
}
