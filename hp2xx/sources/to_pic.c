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

/** to_pic.c: PIC (ATARI "32k" bitmap) formatter part of project "hp2xx"
 **
 ** 91/08/28  V 1.00  HWW  Originating
 ** 91/09/08  V 1.01  HWW  Bug fixed: Repeated block columns
 ** 91/10/09  V 1.02  HWW  ANSI-C definitions; new ATARI file name convention
 ** 91/10/16  V 1.03b HWW  STAD mode (output file packing) added
 ** 91/10/21  V 1.03d HWW  Plain "pic", packing done by pic2pac; VAX_C active
 ** 91/03/01  V 1.03e NM   Bug fixed: numbering of files was incorrect
 ** 91/05/19  V 1.03f HWW  Abort if color mode
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 **
 **	      NOTE:   This code is not part of the supported modules
 **		      of hp2xx. Include it if needed only.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef	TURBO_C
  #include <io.h>
#endif
#include "bresnham.h"
#include "hp2xx.h"



#define ATARI_XRES	640	/* MUST be multiple of 8	*/
#define ATARI_YRES	400
#define BYTES_PER_LINE	(ATARI_XRES>>3)




static int
Init_PIC_files (const char *basename, FILE **fd, int nb, int nr, int yb)
{
#define	FNAME_LEN	80
char	fname[FNAME_LEN], ext[8];
int	i, n, yb_tot;
#ifdef VAX
int	hd;
#endif


  yb_tot = 1 + (nr - 1) / ATARI_YRES;    /* Total # of y blocks */


  for (i=0; nb > 0; i++, nb -= BYTES_PER_LINE)
  {
	if (fd[i])
	{
		fclose (fd[i]);
		fd[i] = NULL;
	}

	n = yb + i * yb_tot;
	if (n > 99)
	{
		Eprintf ("ERROR: Too many PIC files per column!\n");
		for (; i > -1; i--)
			if (fd[i])
			{
				fclose (fd[i]);
				fd[i] = NULL;
			}
		return ERROR;
	}

	sprintf(ext,"%02d.pic", n);

	strcpy (fname, basename);
	strncat (fname, ext, FNAME_LEN - strlen(basename) - 1);

#ifdef VAX
	if ((fd[i] = fopen(fname, WRITE_BIN, "rfm=var","mrs=512")) == NULL)
	{
#else
	if ((fd[i] = fopen(fname, WRITE_BIN)) == NULL)
	{
#endif
		PError ("hp2xx -- opening PIC file(s)");
		return ERROR;
	}
  }
  return 0;
}




static void
RowBuf_to_PIC (RowBuf *row, int nb, FILE **fd)
{
int	i,j, n_pad=0, n_wr=BYTES_PER_LINE;

/* VAX peculiarity: Writing one big object is faster than many smaller */

  if (nb % BYTES_PER_LINE)	/* padding required	*/
	n_pad = (nb/BYTES_PER_LINE + 1)*BYTES_PER_LINE - nb;

  for (i=0; nb > 0; i++, nb -= n_wr)
	fwrite ((char *) &row->buf[i*BYTES_PER_LINE],
		 n_wr=MIN(nb,BYTES_PER_LINE), 1, fd[i]);

  for (i--, j=0; j < n_pad; j++)/* Fill last block with zero	*/
	fputc ('\0', fd[i]);
}





int
PicBuf_to_PIC (const GEN_PAR *pg, const OUT_PAR *po)
{
#define	N_BLOCKS 10

FILE		*fd[N_BLOCKS];
int		row_c, i, nb, nr, yb;
const PicBuf	*pb;

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  if (pb->depth > 1)
  {
	Eprintf ("\nPIC mode does not support colors yet -- sorry\n");
	return ERROR;
  }

  if (pb->nb > (ATARI_XRES * N_BLOCKS) / 8)
  {
	Eprintf ("hp2xx -- Too many PIC files per row");
	return ERROR;
  }

  if (!pg->quiet)
	Eprintf ("\nWriting PIC output: %d rows of %d bytes\n",
		pb->nr, pb->nb);

  for (i=0, nb = pb->nb; nb > 0; i++, nb -= BYTES_PER_LINE)
	fd[i] = NULL;


  /* Backward since highest index is lowest line on screen! */

  for (yb=nr=0, row_c = pb->nr - 1; row_c >= 0; nr++, row_c--)
  {
	if (nr % ATARI_YRES == 0)
	{
		if (Init_PIC_files (		/* Default name	*/
			(*po->outfile != '-') ? po->outfile : "bitmap",
			fd, pb->nb, pb->nr, yb))
				return ERROR;
		yb++;
	}
	if ((!pg->quiet) && (row_c % 10 == 0))
		  /* For the impatients among us ...	*/
		Eprintf (".");
	RowBuf_to_PIC (get_RowBuf(pb, row_c), pb->nb, fd);
  }

  get_RowBuf(pb, 0);		/* Use row 0 for padding */
  for (i=0; i < pb->nb; i++)	/* Clear it		 */
	pb->row[0].buf[i] = '\0';

  while (nr % ATARI_YRES != 0)
  {
	RowBuf_to_PIC (&pb->row[0], pb->nb, fd);
	nr++;
  }


  if (!pg->quiet)
	Eprintf ("\n");

  for (i=0, nb = pb->nb; nb > 0; i++, nb -= BYTES_PER_LINE)
  {
	fclose (fd[i]);
	fd[i] = NULL;
  }
  return 0;
}
