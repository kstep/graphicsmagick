/*
   Copyright (c) 1992 - 1994  Norbert Meyer & Heinz W. Werntges.
   All rights reserved. Distributed by Free Software Foundation, Inc.

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

/** to_img.c:   converts the HP2xx-intern bitmap into a GEM bit-
 **             image file, normally labelled with the extension
 **             IMG --- part of project "hp2xx" (from Heinz Werntges).
 **
 **             The IMG-format was originally proposed as standard
 **             bit-image format for the graphical enviroment GEM
 **             by Digital Research which is used by the Atari ST
 **             and TT computers (GEM and the IMG-format are also
 **             available for IBM PC-compatible computers). But
 **             during the first years other pixel graphic formats
 **             became popular. Now, the Atari ST, TT-series begins
 **             to diversify and the IMG-format has a Renaissance,
 **             since it is independent from the used screen type.
 **             In future the IMG-format will be one of the most
 **             important pixel graphic-formats for Atari ST, TT-
 **             computers.
 **
 **             A second reason why the IMG-format is important is
 **             the fact, that the Public Domain Atari-TeX supports
 **             it - and HP2xx has the intent to be a tool for TeX.
 **
 **
 ** 91/12/13  V 1.00  NM   Originating (Friday, 13th!)
 ** 92/01/26  V 1.01  NM   Bottom-up order, ANSI style calls
 ** 92/02/28  V 1.02  NM   Percent information instead of activity-points
 ** 92/05/17  V 1.02b HWW  Output to stdout if outfile == '-'
 ** 92/05/19  V 1.02c HWW  Abort if color mode
 ** 92/11/08  V 1.02d HWW  File opening changed to standard
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 **
 ** NOTE by HWW: This file is maintained by NM (due to lack of time,
 **              only occasionally). Recent changes done by myself
 **              only resulted out of necessity. Volunteer maintainers for
 **              this file should contact NM (for his address, see file
 **              hp2xx.c). I guess he'll be willing to hand this job over.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bresnham.h"
#include "hp2xx.h"



static Byte
get_byte_IMG (int row_c, int pos, const PicBuf *picbuf)
/* yields one byte which should be analyzed */
{
    int     row_nr;     /* "real" row number    */
    RowBuf  *row;       /* pointer to one row   */
    Byte    get_byte;   /* byte to be returned  */

    row_nr = picbuf->nr - (row_c + 1);
    row = get_RowBuf(picbuf, row_nr);
    get_byte = (Byte) row->buf[pos];

    return (get_byte);
}


/* ---------------------------------------------------------------- */


static int
vert_rep_IMG(int row_c, const PicBuf *picbuf)
/* determines number of vertical repetitions of a row */
{
    int     vert_rep = 0;       /* vertical repetition factor   */

    int     Img_w = picbuf->nb; /* number of bytes/rows         */
    int     Img_h = picbuf->nr; /* number of rows               */

    int     pos;                /* position in row              */

    Byte    org_byte;           /* byte in original row         */
    Byte    cmp_byte;           /* byte in compared row         */

    int     cmp_row_c = row_c +1;   /* row number of compared row   */


    while (cmp_row_c < Img_h)
    {
	/* following row(s) available   */

	for (pos = 0; pos < Img_w; pos++)
	{
	    org_byte = get_byte_IMG(row_c, pos, picbuf);
	    cmp_byte = get_byte_IMG(cmp_row_c, pos, picbuf);

	    if (org_byte != cmp_byte)
		/* found first difference!  */
		return vert_rep;
	}

	cmp_row_c++;
	vert_rep++;

	if(vert_rep >= 254) break;

    }

    return vert_rep;
}


/* ---------------------------------------------------------------- */


static int
empty_SR_IMG (int row_c, int pos, const PicBuf *picbuf)
/* determines number of empty solid runs starting at actual position    */
{
    int     empty_SR_c = 0; /* counts empty solid runs  */
    int     cmp_pos;        /* it's position            */

    int     Img_w = picbuf->nb; /* number of bytes/row  */


    for (cmp_pos = pos; cmp_pos < Img_w; cmp_pos++)
    {
	if(get_byte_IMG(row_c, cmp_pos, picbuf) != (Byte)0)
	    break;
	empty_SR_c++;
	if (empty_SR_c >= 127) break;
    }
    return empty_SR_c;
}


/* ---------------------------------------------------------------- */


static int
full_SR_IMG(int row_c, int pos, const PicBuf *picbuf)
/* determines number of full solid runs starting at actual position */
{
    int     full_SR_c = 0;  /* counts full solid runs   */
    int     cmp_pos;        /* it's position            */

    int     Img_w = picbuf->nb; /* number of bytes/row  */

    for (cmp_pos = pos; cmp_pos < Img_w; cmp_pos++)
    {
	if(get_byte_IMG(row_c, cmp_pos, picbuf) != (Byte)255)
	    break;
	full_SR_c++;
	if (full_SR_c >= 127) break;
    }
    return full_SR_c;
}


/* ---------------------------------------------------------------- */


static int
PR_IMG(int row_c, int pos, const PicBuf *picbuf)
/* determines number of pattern runs starting at actual position    */
{
    int     PR_c = 0;       /* counts full solid runs           */
    Byte    first_byte;     /* first byte of pattern (2 bytes)  */
    Byte    second_byte;    /* second byte of pattern           */
    Byte    cmp_1st_byte;   /* 1st byte for comparision         */
    Byte    cmp_2nd_byte;   /* 2nd byte for comparision         */
    int     cmp_pos;        /* actual position                  */

    int     Img_w = picbuf->nb; /* number of bytes/row  */

    if (pos + 3 < Img_w)
    {
	/* actual position is not end of the row    */
	first_byte  = get_byte_IMG(row_c, pos, picbuf);
	second_byte = get_byte_IMG(row_c, pos + 1, picbuf);

	for(cmp_pos = pos + 2; cmp_pos + 1 < Img_w; cmp_pos += 2)
	{
	    cmp_1st_byte  = get_byte_IMG(row_c, cmp_pos, picbuf);
	    cmp_2nd_byte  = get_byte_IMG(row_c, cmp_pos + 1, picbuf);

	    if (cmp_1st_byte != first_byte || cmp_2nd_byte != second_byte)
		break;
	    PR_c++;
	    if (PR_c >= 254)
		break;
	}
	return PR_c;
    } else
	/* actual position to close to the end of the row   */
	return 0;
}


/* ---------------------------------------------------------------- */


static int
write_byte_IMG (Byte write_byte, FILE *fd)
/* Writes one Byte to the opened IMG-file   */
{
    if (fputc((int)write_byte, fd) == EOF)
    {
	PError("\nhp2xx --- writing IMG file:");
	return ERROR;
    }
    return 0;
}


/* ---------------------------------------------------------------- */


static int
write_VR_IMG (Byte number, FILE *fd)
/* Writes vertical repetition label to the opened IMG-file  */
{
int err;

    err =  write_byte_IMG((Byte)0, fd);
    if (err)	return err;
    err =  write_byte_IMG((Byte)0, fd);
    if (err)	return err;
    err =  write_byte_IMG((Byte)255, fd);
    if (err)	return err;
    return write_byte_IMG(number, fd);
}


/* ---------------------------------------------------------------- */


static int
write_PR_IMG (  Byte number, Byte first_byte, Byte second_byte, FILE *fd)
/* Writes pattern run label to the opened IMG-file  */
{
int err;

    err =  write_byte_IMG((Byte)0, fd);
    if (err)	return err;
    err =  write_byte_IMG(number, fd);
    if (err)	return err;
    err =  write_byte_IMG(first_byte, fd);
    if (err)	return err;
    return write_byte_IMG(second_byte, fd);
}


/* ---------------------------------------------------------------- */


static int
write_empty_SR_IMG (Byte number, FILE *fd)
/* Writes empty solid run to the opened IMG-file    */
{
    return write_byte_IMG(number, fd);
}


/* ---------------------------------------------------------------- */


static int
write_full_SR_IMG (Byte number, FILE *fd)
/* Writes empty solid run to the opened IMG-file    */
{
    Byte    write_byte = (Byte)128;

    write_byte = write_byte | number;
    return write_byte_IMG(write_byte, fd);
}


/* ---------------------------------------------------------------- */


static int
write_BS_IMG (Byte number, FILE *fd)
/* Writes bit string label to the opened IMG-file   */
{
int	err;

    err =  write_byte_IMG((Byte)128, fd);
    if (err)	return err;
    return write_byte_IMG(number, fd);
}


/* ---------------------------------------------------------------- */


int
PicBuf_to_IMG (const GEN_PAR *pg, const OUT_PAR *po)
{
    FILE    *fd;            /* file descriptor                  */

    int     row_c;                  /* row counter              */
    int     Img_w = po->picbuf->nb; /* bytes per row            */
    int     Img_h = po->picbuf->nr; /* number of lines          */

    int	    percent = 0;	    /* progression indicator	*/

    int     Dpi_x = po->dpi_x;      /* dots per inch            */
    int     Dpi_y = po->dpi_y;

    int     vert_rep;       /* vertical repetitions             */

    Byte    act_byte;       /* actual byte                      */

    int     act_pos;        /* actual byte-position in the row  */
    int     first_pos = 0;  /* first pos. of bit string         */
    int     last_pos = 0;   /* last pos. of bit string          */
    int     i_pos;          /* loop index                       */

    int     open_BS;            /* was a bit string opened?     */

    int     empty_SR_len;       /* length of empty solid run    */
    int     full_SR_len;        /* length of full solid run     */
    int     PR_len;             /* length of pattern run        */
    int     BS_len;             /* length of bit string         */

    int     idummy;
    int	    err;
    double  ddummy;
    const   PicBuf* pbuf;

    err = 0;
    pbuf = po->picbuf;
    if (pbuf->depth > 1)
    {
	Eprintf ( "\nIMG mode does not support colors yet -- sorry\n");
	return ERROR;
    }

    /*                  */
    /* action message   */
    /*                  */
    if (!pg->quiet)
    {
	Eprintf ( "\n\nWriting IMG output: %d rows of %d bytes\n",
		pbuf->nr, pbuf->nb);
	Eprintf ("\n%s:\no open",
		*po->outfile == '-' ? "<stdout>" : po->outfile);
    }


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
    else	fd = stdout;


    /**
     ** write header (8 words)
     **/

    if (!pg->quiet)
	Eprintf ("\no write\n  - header");

    /* version number = 1   */
    if ((err = write_byte_IMG((Byte)0, fd)) != 0)
	goto IMG_exit;
    if ((err = write_byte_IMG((Byte)1, fd)) != 0)
	goto IMG_exit;

    /* length of header (in words) = 8  */
    if ((err = write_byte_IMG((Byte)0, fd)) != 0)
	goto IMG_exit;
    if ((err = write_byte_IMG((Byte)8, fd)) != 0)
	goto IMG_exit;

    /* number of bits/pixel (bit-planes) = 1    */
    if ((err = write_byte_IMG((Byte)0, fd)) != 0)
	goto IMG_exit;
    if ((err = write_byte_IMG((Byte)1, fd)) != 0)
	goto IMG_exit;

    /* length of pattern run (in bytes) = 2     */
    if ((err = write_byte_IMG((Byte)0, fd)) != 0)
	goto IMG_exit;
    if ((err = write_byte_IMG((Byte)2, fd)) != 0)
	goto IMG_exit;

    /* width of pixel in 1/1000 mm  */
    ddummy = (double)Dpi_x;
    ddummy = 25.4 / ddummy * 1000.0 / 256;
    act_byte = (Byte)ddummy;
    if ((err = write_byte_IMG (act_byte, fd)) != 0)
	goto IMG_exit;
    ddummy = (double)Dpi_x;
    ddummy = 25.4 / ddummy * 1000.0;
    idummy = (int)ddummy & 255;
    act_byte = (Byte)idummy;
    if ((err = write_byte_IMG (act_byte, fd)) != 0)
	goto IMG_exit;

    /* height of pixel in 1/1000 mm */
    ddummy = (double)Dpi_y;
    ddummy = 25.4 / ddummy * 1000.0 / 256;
    act_byte = (Byte)ddummy;
    if ((err = write_byte_IMG(act_byte, fd)) != 0)
	goto IMG_exit;
    ddummy = (double)Dpi_y;
    ddummy = 25.4 / ddummy * 1000.0;
    idummy = (int)ddummy & 255;
    act_byte = (Byte)idummy;
    if ((err = write_byte_IMG(act_byte, fd)) != 0)
	goto IMG_exit;

    /* width of row in pixel    */
    idummy = Img_w * 8 / 256;
    act_byte = (Byte)idummy;
    if ((err = write_byte_IMG(act_byte, fd)) != 0)
	goto IMG_exit;
    idummy = Img_w * 8;
    idummy = idummy & 255;
    act_byte = (Byte)idummy;
    if ((err = write_byte_IMG(act_byte, fd)) != 0)
	goto IMG_exit;

    /* number of rows */
    idummy = Img_h  / 256;
    act_byte = (Byte)idummy;
    if ((err = write_byte_IMG(act_byte, fd)) != 0)
	goto IMG_exit;
    idummy = Img_h;
    idummy = idummy & 255;
    act_byte = (Byte)idummy;
    if ((err = write_byte_IMG(act_byte, fd)) != 0)
	goto IMG_exit;

    if (!pg->quiet)
	Eprintf ("\n  - data: ");

    /**
     **  Loop over all lines
     **/

    for (row_c = 0; row_c < Img_h; row_c++)
    {
	/*      */
	/* ...  */
	/*      */
	if(!pg->quiet)
	{
	    if((int)(((float)row_c * 100.0) / (float)Img_h) >= (percent + 10))
	    {
		percent += 10;
		Eprintf ( "%d%% ",percent);
	    }
	}

	/*                                  */
	/* Determine vertical repetition    */
	/*                                  */
	if ((vert_rep = vert_rep_IMG(row_c, pbuf)) > 0)
	{
	    row_c += vert_rep;
	    if(!pg->quiet)
	    {
		if((int)(((float)row_c * 100.0) / (float)Img_h) >= (percent + 10))
		{
			percent += 10;
			Eprintf ( "%d%% ",percent);
		}
	    }
	    if ((err = write_VR_IMG((Byte)(vert_rep + 1), fd)) != 0)
		goto IMG_exit;
	}

	/**
	 **  Analyse actual row in detail
	 **/

	/* prepare bit string switch        */

	open_BS = FALSE;

	/* prepare byte position            */

	act_pos = 0;

	/**
	 ** Loop over all bytes in actual row
	 **/
	do {
	    if(open_BS == TRUE  )
	    {
		/* bit string was opened before */

		if (  (empty_SR_len = empty_SR_IMG(row_c, act_pos, pbuf)) > 3
		   || (full_SR_len = full_SR_IMG(row_c, act_pos, pbuf)) > 3
		   || (PR_len = PR_IMG(row_c, act_pos, pbuf)) > 2
		   || act_pos >= Img_w - 1
		   || last_pos - first_pos + 1 >= 254)
		{
		    /* it's worth to stop the bit string    */
		    open_BS = FALSE;

		    if (act_pos >= Img_w -1)    /* special case:    */
		    {
			last_pos = act_pos;     /* last byte in row */
			act_pos++;
		    }

		    BS_len = last_pos - first_pos + 1;
		    if ((err = write_BS_IMG((Byte)BS_len, fd)) != 0)
			goto IMG_exit;
		    for (i_pos = first_pos; i_pos <= last_pos; i_pos++)
		    {
			act_byte = get_byte_IMG(row_c, i_pos, pbuf);
			if ((err = write_byte_IMG(act_byte, fd)) != 0)
				goto IMG_exit;
		    }

		}
		else
		{
		    /* the bit string should continue   */
		    last_pos = act_pos;
		    act_pos++;
		}

	    } else {
		/* no bit string open   */

		if ((empty_SR_len = empty_SR_IMG(row_c, act_pos, pbuf)) > 0)
		{
		    act_pos += empty_SR_len;
		    if ((err = write_empty_SR_IMG((Byte)empty_SR_len, fd)) != 0)
			goto IMG_exit;

		}
	      else
		if ((full_SR_len = full_SR_IMG(row_c, act_pos, pbuf)) > 0)
		{
		    act_pos += full_SR_len;
		    if ((err = write_full_SR_IMG((Byte)full_SR_len, fd)) != 0)
			goto IMG_exit;

		}
	      else
		if ((PR_len = PR_IMG(row_c, act_pos, pbuf)) > 0)
		{
		    if ((err = write_PR_IMG((Byte)(PR_len + 1),
				 get_byte_IMG(row_c, act_pos, pbuf),
				 get_byte_IMG(row_c, act_pos +1 , pbuf),
				 fd)) != 0)
			goto IMG_exit;
		    act_pos += (PR_len + 1) * 2;

		}
	      else
		{    /* remaining: bit string, open it   */
		    open_BS = TRUE;
		    first_pos = act_pos;
		    last_pos  = act_pos;
		    if (act_pos < Img_w - 1)
			act_pos++;
		}

	    }
	} while (act_pos < Img_w);

    }

    if(!pg->quiet)
	if (percent < 100)
		Eprintf ( "100%%");
	Eprintf ( "\no close\n");

IMG_exit:
    if (fd != NULL && fd != stdout)
	if (fclose(fd) != 0)
	{
		PError("\nhp2xx -- closing IMG:");
		return ERROR;
	}
    if(!pg->quiet)
	Eprintf ( "\n(End of IMG)\n");
    return err;
}

