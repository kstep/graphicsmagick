/*
   Copyright (c) 1992 - 1994  Norbert Meyer.  All rights reserved.
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

/** to_pac.c:  converts the hp2xx-intern bitmap into a series of
 **            files with STAD-format (*.PAC) - part of project
 **            "hp2xx" (from Heinz Werntges).
 **
 **            STAD-format is very popular in the Atari-world and
 **            is supported by many commercial (for instance, the
 **            original STAD-programm (Appliction Systems,
 **            Heidelberg, FRG)) and PD pixel-graphic programms.
 **
 **            Each PAC-file represents a monochrome picture with
 **            640 x 400 pixels. Therefore, if the hp2xx-bitmap
 **            is larger than 640 x 400 pixels more than one PAC-
 **            file has to be generated.
 **
 ** NOTE:     This is an unsupported addition to hp2xx, not a regular
 **	      module!
 **
 ** 91/12/01  V 1.00  NM   Originating (using HWW's TO_PIC
 **                        as prototype)
 ** 92/02/23  V 1.10  NM   ANSI-style, bottom-up-style
 ** 92/02/27  V 1.10a NM   errno.h added
 ** 92/04/14  V 1.10b NM   New order for if (...) { ... }
 ** 92/05/19  V 1.10c HWW  Abort if color mode
 ** 94/02/14  V 1.20a HWW  Adapted to changes in hp2xx.h
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "bresnham.h"
#include "hp2xx.h"




#define PAC_XRES       640             /* defining a "screen"  */
#define PAC_YRES       400
#define PAC_BPL        (PAC_XRES>>3)   /* Byte Per Line        */


#define TESTHORI        0              /* modes for using      */
#define TESTVERTI       1              /* Pack_PAC             */
#define WRITEHORI       2
#define WRITEVERTI      3
#define WRITEUNPACKED   4



/* -------------------------------------------------------- */


static int
Screenpos_PAC (int *scr_x, int *scr_y, int mode)
/** *scr_x, *scr_y; position in unpacked screen-dump
 ** mode;           actual compression mode
 ** increases absolute and x/y byte-position in screen or
 ** returns 32000, if increase is not senseful
 **/
{
    int     nr;         /* absolute byte-number */
    int     x = *scr_x; /* just to ease my work */
    int     y = *scr_y; /* ...                  */

    if (mode == TESTVERTI || mode == WRITEVERTI)
    {
        y++;
	if (y >= 400)
	{
	    y = 0;
	    x++;
	}
    }
    else
    {
        x++;
	if (x >= 80)
	{
            x = 0;
            y++;
        }
    }

    if (x >= 80 || y >= 400)
    {
	nr = 32000;
    }
    else
    {
        nr = x + y * 80;
    }

    *scr_x = x;
    *scr_y = y;

    return nr;
}


/* -------------------------------------------------------- */


static int
Pack_PAC(Byte *screen, Byte packbyte, Byte lablbyte, Byte specbyte,
	      int mode, FILE *fd)
/** *screen;         unpacked 32k-bitmap
 ** packbyte;        most frequent byte in screen
 ** lablbyte;        indicates packbyte
 ** specbyte;        indicates repetitions and
 **                  pack-, labl- or specbyte
 ** mode;    TESTHORI     = test horiz. compression
 **          TESTVERTI    = test vert.  compression
 **          WRITEHORI    = write horiz. compr. file
 **          WRITEVERTI   = write verti. compr. file
 **          WRITEUNPACKED= write uncompressed  file
 ** *fd;             file descriptor of PAC file
 **/

/** result is:   calculated length of PAC-file
 **                  for mode = TESTxxx
 **              32000 if no efficient compression is
 **                  possible
 **              0 for mode = WRITExxx
 **              ERROR for occuring errors
 **/

{
    int     scr_x = 0, scr_y = 0;   /* x/y-position in screen   */
    int     scr_pos = 0;            /* byte-position in screen  */

    int     pac_pos = 0;            /* byte-position in PAC    */
    Byte    actbyte;                /* actual byte              */
    int     rep;                    /* repetitions (of bytes)   */
    Byte    repbyte;                /* repetition-byte          */
    Byte    nullbyte = 0;           /* contains 0               */

    int     pac_pos_org;            /* to store the original    */
    Byte    actbyte_org;            /* state of ...             */

    char    horimark[4]  = {'p', 'M', '8', '5'};
    char    vertimark[4] = {'p', 'M', '8', '6'};
				    /* STAD was written by      */
				    /* Peter Melzer in 1985/86  */

    if (mode != WRITEUNPACKED)
    {
        /* compression required */

        /*                          */
        /* Label: 'pM85' or 'pM86'  */
        /*                          */

	if (mode == WRITEHORI || mode == WRITEVERTI)
	{
            /* writing required */

	    if (mode == WRITEHORI)
	    {
		/* write 'pM85' (horizontal compression)    */
		if (fwrite((char *) horimark, (size_t) 1, (size_t) 4, fd) != (size_t) 4)
		    return ERROR;

	    }
	    else
	    {
                /* write 'pM86' (vertical compression)  */
                if (fwrite((char *) vertimark, (size_t) 1, (size_t) 4, fd) != (size_t) 4)
                    return ERROR;
            }
        }
        pac_pos += 4;

        /*                                  */
        /* label-, pack-, and special-byte  */
        /*                                  */

	if (mode == WRITEHORI || mode == WRITEVERTI)
	{
            /* write pack-, label-, and special-byte    */
            if (fwrite((Byte *) &lablbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                return ERROR;
            if (fwrite((Byte *) &packbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                return ERROR;
            if (fwrite((Byte *) &specbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
		return ERROR;
        }
        pac_pos += 3;

	do {
            actbyte = *(screen + scr_pos);

	    if (actbyte == packbyte)
	    {
                /*                          */
                /* special case: packbyte   */
                /*                          */
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
		    /* write lablbyte (indicates packbyte-repetition)   */
			if (fwrite((Byte *) &lablbyte, (size_t) 1,
				   (size_t) 1, fd) != (size_t) 1)
				return ERROR;
                }
                /* determine number of packbyte-repetitions:    */
                pac_pos++;
                scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                rep = 0;
		if (scr_pos < 32000)
		{
                    /* not at the end of the screen-dump    */
                    actbyte = *(screen + scr_pos);
		    while (actbyte == packbyte && rep < 255)
		    {
                        scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                        rep++;
                        if (scr_pos >= 32000) break;
                        actbyte = *(screen + scr_pos);
                    }
                }
                repbyte = (Byte) rep;
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write number of packbyte-repetions   */
                    if (fwrite((Byte *) &repbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                pac_pos++;

	    }
	    else if (actbyte == lablbyte)
	    {
                /*                                              */
                /* special case: label-byte (needs a special    */
		/*               representation)                */
                /*                                              */
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write specbyte   */
                    if (fwrite((Byte *) &specbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                pac_pos++;
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write lablbyte (which should be indicated) */
                    if (fwrite((Byte *) &lablbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                /* determine number of lablbytes:   */
                pac_pos++;
                scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                rep = 0;
		if (scr_pos < 32000)
		{
                    /* not at the end of the screen-dump    */
                    actbyte = *(screen + scr_pos);
		    while (actbyte == lablbyte && rep < 255)
		    {
                        scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                        rep++;
                        if (scr_pos >= 32000) break;
                        actbyte = *(screen + scr_pos);
                    }
                }
                repbyte = (Byte) rep;
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write number of lablbyte-repetions   */
                    if (fwrite((Byte *) &repbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                pac_pos++;

	    }
	    else if (actbyte == specbyte)
	    {
                /*                                              */
                /* special case: special-byte   (needs a        */
		/*                special representation)       */
                /*                                              */
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write specbyte   */
                    if (fwrite((Byte *) &specbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                pac_pos++;
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write specbyte (which should be indicated) */
                    if (fwrite((Byte *) &specbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                /* determine number of specbytes:   */
                pac_pos++;
                scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                rep = 0;
		if (scr_pos < 32000)
		{
                    /* not at the end of the screen-dump    */
                    actbyte = *(screen + scr_pos);
		    while (actbyte == specbyte && rep < 255)
		    {
                        scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                        rep++;
                        if (scr_pos >= 32000) break;
                        actbyte = *(screen + scr_pos);
                    }
                }
                repbyte = (Byte) rep;
		if (mode == WRITEHORI || mode == WRITEVERTI)
		{
                    /* write number of specbyte-repetions   */
                    if (fwrite((Byte *) &repbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                        return ERROR;
                }
                pac_pos++;

	    }
	    else
	    {
		/*              */
		/* normal byte  */
		/*              */
		if (scr_pos < 31999)
		{
		    /* byte-repetition possible (not at the end)    */

		    actbyte_org = actbyte;  /* conservate state     */
		    pac_pos_org = pac_pos;  /* before searching     */
					    /* for repetitions      */

		    /* determine number of byte-repetitions:    */
		    pac_pos++;
		    scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
		    rep = 0;
		    if (scr_pos < 32000)
		    {
                        /* not at the end of the screen-dump    */
                        actbyte = *(screen + scr_pos);
			while (actbyte == actbyte_org && rep < 255)
			{
                            scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                            rep++;
                            if (scr_pos >= 32000) break;
                            actbyte = *(screen + scr_pos);
                        }
                    }

		    if (rep == 0)         /* no repetition    */
		    {
			actbyte = actbyte_org;  /* restore old state    */
			pac_pos = pac_pos_org;

			if (mode == WRITEHORI || mode == WRITEVERTI) {
			    /* write single byte    */
			if (fwrite((Byte *) &actbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
			    return ERROR;
			}
			pac_pos++;
		    }
		    else
		    {                /* with repetition  */
                        actbyte = actbyte_org;  /* restore old state    */
                        pac_pos = pac_pos_org;

			if (mode == WRITEHORI || mode == WRITEVERTI)
			{
			    /* write specbyte   */
                            if (fwrite((Byte *) &specbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                                return ERROR;
                        }
                        pac_pos++;

			if (mode == WRITEHORI || mode == WRITEVERTI)
			{
                            /* write repeating byte */
                            if (fwrite((Byte *) &actbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                                return ERROR;
                        }
                        pac_pos++;

                        repbyte = (Byte) rep;
			if (mode == WRITEHORI || mode == WRITEVERTI)
			{
                            /* write repetition number  */
                            if (fwrite((Byte *) &repbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                                return ERROR;
                        }
                        pac_pos++;
		    }
		}
		else
		{
                    /* byte-repetition not possible due to the end  */

			if (mode == WRITEHORI || mode == WRITEVERTI)
			{
                            /* write single byte    */
                            if (fwrite((Byte *) &actbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                                return ERROR;
                        }
                        scr_pos = Screenpos_PAC(&scr_x, &scr_y, mode);
                        pac_pos++;
                }
            }

        } while (pac_pos < 31997 && scr_pos < 32000);

	if (pac_pos < 31997)  /* compression was effective    */
	{
	    if (mode == WRITEHORI || mode == WRITEVERTI)
	    {
		/* write final: specbyte + Ox00 + Ox00  */
                if (fwrite((Byte *) &specbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                    return ERROR;
                if (fwrite((Byte *) &nullbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                    return ERROR;
                if (fwrite((Byte *) &nullbyte, (size_t) 1, (size_t) 1, fd) != (size_t) 1)
                    return ERROR;
                return 0;
            }
            pac_pos += 3;
            return pac_pos;
	}
	else
	{                /* compression was ineffective  */
	    if (mode == WRITEHORI || mode == WRITEVERTI)
	    {
		/* in WRITExxx-mode (with compression) you should   */
		/* never reach this place !                         */
		return ERROR;
	    }
	    return 32000;
	}

	}
	else
	{
            /* writing unpacked screen-dump */
            if (fwrite((Byte *) screen, (size_t) 1, (size_t) 32000, fd) < (size_t) 32000)
                return ERROR;

            return 0;
    }
}


/* -------------------------------------------------------- */


static void
Analyze_PAC(Byte *screen, Byte *packbyte, Byte *lablbyte, Byte *specbyte)
/** *screen;         unpacked 32k-bitmap
 ** *packbyte;       most frequent byte in screen
 ** *lablbyte;       indicates packbyte
 ** *specbyte;       indicates repetitions and
 **                  pack-, labl- or specbyte
 **/
{
    int     i;
    int     freq[256];      /* frequency of bytes in picture */

    *packbyte = (Byte) 0;
    *lablbyte = (Byte) 0;
    *specbyte = (Byte) 0;

    /* initialize frequency-array */
    for (i = 0; i < 256; i++)
        freq[i] = 0;

    /* determine frequency of bytes */
    for (i = 0; i < 32000; i++)
        freq[(int) *(screen + i)]++;

    /* determine most frequent byte (packbyte) */
    for (i = 0; i < 256; i++)
        if (freq[i] > freq[(int) *packbyte])
            *packbyte = (Byte) i;

    /* determine byte with lowest frequency (lablbyte)  */
    while (*lablbyte == *packbyte)  /* avoid identity   */
        (*lablbyte)++;

    for (i = 0; i < 256; i++)
        if (freq[i] < freq[(int) *lablbyte] && (Byte)i != *packbyte)
            *lablbyte = (Byte) i;

    /* determine byte with second lowest frequency (specbyte)   */
    while (*specbyte == *packbyte || *specbyte == *lablbyte)
        (*specbyte)++;                      /* avoid identity   */

    for (i = 0; i < 256; i++)
        if (freq[i] < freq[(int) *specbyte]
            && (Byte)i != *packbyte && (Byte)i != *lablbyte)
            *specbyte = (Byte) i;
}


/* -------------------------------------------------------- */


static void
Screen_for_PAC (const PicBuf *picbuf, Byte *screen, int x, int y)
/** *picbuf;         structure of hp2xx-bitmap
 ** *screen;         32000 byte buffer (1 screen)
 ** x, y;            actual screen
 ** copies bitmap section into screen-buffer
 **/
{
  RowBuf        *row;           /* pointer to one row   */
  int           buf_x, buf_y;   /* position in bitmap   */
  int           scr_x, scr_y;   /* position in screen   */
  int           row_nr;         /* actual row-number    */


  for (buf_y = PAC_YRES * y, scr_y = 0; scr_y < PAC_YRES; buf_y++, scr_y++)
  {
     for (buf_x = PAC_BPL * x, scr_x = 0; scr_x < PAC_BPL; buf_x++, scr_x++)
     {

	if (buf_y < picbuf->nr && buf_x < picbuf->nb)
	{
	    row_nr = picbuf->nr - (buf_y + 1);
	    row = get_RowBuf(picbuf, row_nr);
	    screen[scr_x + scr_y * PAC_BPL] = (Byte) row->buf[buf_x];
	}
	else
	    screen[scr_x + scr_y * PAC_BPL] = (Byte) 0;
     }
  }
}


/* -------------------------------------------------------- */


static void
Name_PAC (char *filename, const char *basename, int y_screens,
		  int x, int y)
/* adds number and extension to basename, giving the complete filename */
{
  char  ext[9];	/* file-number and extension	*/
  int	nr;		/* file-number					*/

  nr = x * (y_screens + 1) + y;

  sprintf(ext,"%02d.pac", nr);
  strcpy (filename, basename);
  strcat (filename, ext);

}


/* -------------------------------------------------------- */


int
PicBuf_to_PAC (const GEN_PAR *pg, const OUT_PAR *po)
{

#define BLOCKS 100       /* max. 100 files        */

FILE    *fd;                    /* stream handle                */
int     x_screens, y_screens;   /* max. of file-counters        */
int     x, y;                   /* file-counters                */
char    basename[32];           /* filename without extension   */
char    filename[96];           /* filename, complete           */

static Byte screen[32000];      /* sorry for wasting memory,    */
				/* but there's no convincing    */
				/* STAD-packaging mechanism     */
				/* without at least 32000 bytes */
				/* of the valuable memory       */

Byte    packbyte;               /* most frequent byte in screen */
Byte    lablbyte;               /* indicates packbyte           */
Byte    specbyte;               /* indicates repetitions and    */
				/* pack-, labl- or specbyte     */

int     horicompr;              /* length horiz. compr. picture */
int     verticompr;             /* length verti. compr. picture */

const PicBuf *pb;

#ifdef VAX
int     hd;                     /* file handle                  */
#endif

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  if (pb->depth > 1)
  {
	Eprintf ("\nPAC mode does not support colors yet -- sorry\n");
	return ERROR;
  }

 /**
  **  check number of screens (rows * columns)
  **/
  x_screens = (pb->nb - 1) / PAC_BPL;
  y_screens = (pb->nr - 1) / PAC_YRES;

  if (((x_screens + 1) * (y_screens + 1)) > BLOCKS)
  {
	PError("hp2xx -- Too many PAC files necessary");
	return ERROR;
  }

 /**
  **  action message
  **/
  if (!pg->quiet)
  {
      Eprintf ("\n\nWriting PAC output: %d rows of %d bytes\n",
		      pb->nr, pb->nb);
      Eprintf ("corresponding to %d x %d PAC-files\n",
		      x_screens + 1, y_screens + 1);
  }

 /**
  **  creat basename (filename without extension)
  **/
  if (*po->outfile != '-')
  {
	strncpy (basename, po->outfile, 6);
	basename[6] = '\0';
  }
  else
	strcpy  (basename, "stad");     /* Default name */

 /**
  **  run over all screens
  **/
  for (y = 0; y <= y_screens; y++)
  {
    for (x = 0; x <= x_screens; x++)
    {
	/* make actual filename */
	Name_PAC (filename, basename, y_screens, x, y);
	if (!pg->quiet)
	    Eprintf ("%s-> ", filename);

	/* collect data from hp2xx-bitmap   */
	if (!pg->quiet)
	    Eprintf ("data: fetch, ");
	Screen_for_PAC (pb, screen, x, y);

	/* determine pack-, label-, special-byte */
	if (!pg->quiet)
	    Eprintf ("analyse [1");
	Analyze_PAC(screen, &packbyte, &lablbyte, &specbyte);
	if (!pg->quiet)
	    Eprintf ("] ");

	/* preset file-descriptor (to give him a defined state) */
	fd = NULL;

	/* test horizontal compression mode */
	if (!pg->quiet)
	    Eprintf ("[2");
	if ((horicompr = Pack_PAC(screen, packbyte, lablbyte, specbyte,
			 TESTHORI, fd)) == ERROR)
	{
	    PError ("\nhp2xx -- test horiz.-PAC file");
	    return ERROR;
	}
	if (!pg->quiet)
	    Eprintf ("] ");

	/* test vertical compression mode */
	if (!pg->quiet)
	    Eprintf ("[3");
	if ((verticompr = Pack_PAC(screen, packbyte, lablbyte, specbyte,
			  TESTVERTI, fd)) == ERROR)
	{
	    PError ("\nhp2xx -- test vert.-PAC file");
	    return ERROR;
	}
	if (!pg->quiet)
	    Eprintf ("] ;");

	/* open file to write   */
	if (!pg->quiet)
	    Eprintf ("file: open, ");
#ifdef VAX
	if ((hd = creat(filename, 0,"rfm=var","mrs=512")) == -1)
	    PError ("\nhp2xx -- creating PAC-output file");
	if( (fd = fdopen(hd, WRITE_BIN)) == NULL)
	{
#else
	if ((fd = fopen(filename, WRITE_BIN)) == NULL)
	{
#endif
	    PError ("\nhp2xx -- opening PAC file");
	    return ERROR;
	}

	/* decide which file shall be written   */
	if (!pg->quiet)
	    Eprintf ("write, ");
	if (horicompr >= 32000 && verticompr >= 32000)
	{
	    /* no compression was sucessfull, write unpacked file   */
	    if (Pack_PAC(screen, packbyte, lablbyte, specbyte,
			  WRITEUNPACKED, fd))
	    {
		PError ("\nhp2xx -- writing PAC file");
		return ERROR;
	    }
	}
	else if (horicompr <= verticompr)
	{
	    /* horizontal compression was better, write it  */
	    if (Pack_PAC(screen, packbyte, lablbyte, specbyte,
			  WRITEHORI, fd))
	    {
		PError ("\nhp2xx -- writing PAC file");
		return ERROR;
	    }
	}
	else
	{
	    /* vertical compression was better, write it    */
	    if (Pack_PAC(screen, packbyte, lablbyte, specbyte,
			  WRITEVERTI, fd))
	    {
		PError ("\nhp2xx -- writing PAC file");
		return ERROR;
	    }
	}


	/* close file   */
	if (!pg->quiet)
	    Eprintf ("close");
	fclose(fd);

	/* line feed    */
	if (!pg->quiet)
	    Eprintf ("\n");

    }
  }

    /* final message    */
    if (!pg->quiet)
	Eprintf ("(End of PAC)\n");
  return 0;
}
