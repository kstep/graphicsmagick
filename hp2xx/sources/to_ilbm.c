/*
   Copyright (c) 1992 - 1994 Claus H. Langhans.  All rights reserved.
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
 ** to_ilbm.c: InterChangeFileFormat: InterLeaveBitMap (IFF-ILBM) converter
 ** part of project "hp2xx"
 **
 ** 92/04/14  V 1.00  CHL  Originating: Copied from to_pbm.c
 ** 92/04/15  V 1.01  CHL  I read the IFF-ILBM Manuals! Word alligned, not byte!
 ** 92/04/16  V 1.02  CHL  Better error handling
 ** 94/02/14  V 1.10a HWW  Adapted to changes in hp2xx.h
 **/


#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"

typedef long    LONG;
typedef unsigned long ULONG;
typedef unsigned long LONGBITS;
typedef short   WORD;
typedef unsigned short UWORD;
typedef unsigned short WORDBITS;
typedef char    BYTE;
typedef unsigned char UBYTE;
typedef unsigned char BYTEBITS;
typedef unsigned char *STRPTR;
typedef STRPTR *APTR;
typedef ULONG   CPTR;
typedef short   SHORT;
typedef unsigned short USHORT;
typedef float   FLOAT;
typedef double  DOUBLE;
typedef short   COUNT;
typedef unsigned short UCOUNT;
typedef short   BOOL;
typedef unsigned char TEXT;
typedef long    BPTR;


#include "iff.h"
#include "ilbm.h"


#define CMAP
#define CAMG  /* AMIGA specific video mode */

#define GGE >>=

#define YES 1L;
#define NO  0L;

static int
put_LONG( LONG l, FILE *fd) {
int retval;
    if((retval=putc ((l >> 24) & 0xFF,fd))==EOF)
	return(retval);
    else
	if((retval=putc ((l >> 16) & 0xFF,fd))==EOF)
	    return(retval);
	else
	    if((retval=putc ((l >> 8) & 0xFF,fd))==EOF)
		  return(retval);
	    else
		return(putc(l,fd));
}

static int
put_ULONG( ULONG ul, FILE *fd) {
int retval;
    if((retval=putc ((ul >> 24) & 0xFF,fd))==EOF)
	return(retval);
    else
	if((retval=putc ((ul >> 16) & 0xFF,fd))==EOF)
	    return(retval);
	else
	    if((retval=putc ((ul >> 8) & 0xFF,fd))==EOF)
		  return(retval);
            else
                return(putc(ul,fd));
}

static int
put_WORD( WORD w, FILE *fd) {
int retval;
    if((retval=putc ((w >> 8) & 0xFF,fd))==EOF)
	  return(retval);
    else
	return(putc(w,fd));
}

static int
put_UWORD( UWORD uw, FILE *fd) {
int retval;
    if((retval=putc ((uw >> 8) & 0xFF,fd))==EOF)
	  return(retval);
    else
	return(putc(uw,fd));
}

static int
put_UBYTE( UBYTE u, FILE *fd) {
    return(putc (u,fd));
}

int
PicBuf_to_ILBM (const GEN_PAR *pg, const OUT_PAR *po)
{
    FILE           *fd;
    int             row_count = 0;
    int             row_c, byte_c, bit, x, xoff, yoff;
    const RowBuf   *row;
    const PicBuf   *pb;
    int              i;
    unsigned char  *memptr;
    int             BitPlaneSize;
    int             OddBmp;

    struct SimpleHdr {
	ChunkHeader     FORM_Hdr;
	LONG            ILBM_Type;
	ChunkHeader     BMHD_CkHdr;
	BitMapHeader    BMHD_Ck;
#ifdef CAMG
	ChunkHeader     CAMG_CkHdr;
	CamgChunk       CAMG_Ck;
#endif
#ifdef CMAP
	ChunkHeader     CMAP_CkHdr;
	UBYTE           Map0red;
	UBYTE           Map0green;
	UBYTE           Map0blue;
	UBYTE           Map1red;
	UBYTE           Map1green;
	UBYTE           Map1blue;
#endif
	ChunkHeader     BODY_CkHdr;
    }               MyHdr;

    if (pg == NULL || po == NULL)
	return ERROR;

    pb = po->picbuf;
    if (pb == NULL)
	return ERROR;

    if (pb->depth > 1)
    {
	Eprintf ("\nILBM mode does not support colors yet -- sorry\n");
	goto ERROR_EXIT_2;
    }

    if ((((pb->nb) % 2 ) == 1) ){
	OddBmp=YES;
	BitPlaneSize = (pb->nb) * (pb->nr) + (pb->nr);
    }
    else {
	OddBmp=NO;
	BitPlaneSize = (pb->nb) * (pb->nr);
    }

    MyHdr.FORM_Hdr.ckID = FORM;
    MyHdr.FORM_Hdr.ckSize =  sizeof(MyHdr)
			   - sizeof(MyHdr.FORM_Hdr.ckID)
			   - sizeof(MyHdr.FORM_Hdr.ckSize)
			   + BitPlaneSize;

    MyHdr.ILBM_Type = ID_ILBM;

    MyHdr.BMHD_CkHdr.ckID = ID_BMHD;
    MyHdr.BMHD_CkHdr.ckSize = sizeof(MyHdr.BMHD_Ck);

    MyHdr.BMHD_Ck.w = (pb->nb) * 8;        /* raster width & height in pixels */
    MyHdr.BMHD_Ck.h = pb->nr;
    MyHdr.BMHD_Ck.x = 0L;                  /* position for this image */
    MyHdr.BMHD_Ck.y = 0L;
    MyHdr.BMHD_Ck.nPlanes = 1L;            /* # source bitplanes */
    MyHdr.BMHD_Ck.masking = mskNone;       /* masking technique */
    MyHdr.BMHD_Ck.compression = cmpNone;   /* compression algoithm */
    MyHdr.BMHD_Ck.pad1 = 0L;               /* UNUSED.  For consistency, put 0 here.*/
    MyHdr.BMHD_Ck.transparentColor = 1L;   /* transparent "color number" */
    MyHdr.BMHD_Ck.xAspect = 1L;            /* aspect ratio, a rational number x/y */
    MyHdr.BMHD_Ck.yAspect = 1L;

    MyHdr.BMHD_Ck.pageWidth =MyHdr.BMHD_Ck.w; /* source "page" size in pixels */
    MyHdr.BMHD_Ck.pageHeight =MyHdr.BMHD_Ck.h;

#ifdef CAMG
    MyHdr.CAMG_CkHdr.ckID = ID_CAMG;
    MyHdr.CAMG_CkHdr.ckSize = sizeof(MyHdr.CAMG_Ck);
    MyHdr.CAMG_Ck.ViewModes = 0x8004; /* = HIRES LACE */
#endif

#ifdef CMAP
    MyHdr.CMAP_CkHdr.ckID = ID_CMAP;
    MyHdr.CMAP_CkHdr.ckSize = sizeof(MyHdr.Map0red) * 6;
    MyHdr.Map0red   = 0;
    MyHdr.Map0green = 0;
    MyHdr.Map0blue  = 0;
    MyHdr.Map1red   = 255;
    MyHdr.Map1green = 255;
    MyHdr.Map1blue  = 255;
#endif

    MyHdr.BODY_CkHdr.ckID = ID_BODY;
    MyHdr.BODY_CkHdr.ckSize = BitPlaneSize;


    if (!pg->quiet)
	Eprintf ("\nWriting ILBM output: %s\n",po->outfile);
    if (*po->outfile != '-') {
#ifdef VAX
	if ((fd = fopen(po->outfile, WRITE_BIN, "rfm=var", "mrs=512")) == NULL) {
#else
	if ((fd = fopen(po->outfile, WRITE_BIN)) == NULL) {
#endif
	    PError("hp2xx -- opening output file");
	    return ERROR;
	}
    } else
	fd = stdout;

    if (((MyHdr.FORM_Hdr.ckSize) % 2 ) == 1) {
	MyHdr.FORM_Hdr.ckSize +=1;
    }



    if(put_LONG(MyHdr.FORM_Hdr.ckID, fd)== EOF) goto ERROR_EXIT;
    if(put_LONG(MyHdr.FORM_Hdr.ckSize, fd)== EOF) goto ERROR_EXIT;
    if(put_LONG(MyHdr.ILBM_Type, fd)== EOF) goto ERROR_EXIT;

    if(put_LONG(MyHdr.BMHD_CkHdr.ckID, fd)== EOF) goto ERROR_EXIT;
    if(put_LONG(MyHdr.BMHD_CkHdr.ckSize, fd)== EOF) goto ERROR_EXIT;

    if(put_UWORD(MyHdr.BMHD_Ck.w, fd)== EOF) goto ERROR_EXIT;
    if(put_UWORD(MyHdr.BMHD_Ck.h, fd)== EOF) goto ERROR_EXIT;
    if(put_WORD(MyHdr.BMHD_Ck.x, fd)== EOF) goto ERROR_EXIT;
    if(put_WORD(MyHdr.BMHD_Ck.y, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.BMHD_Ck.nPlanes, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.BMHD_Ck.masking, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.BMHD_Ck.compression, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.BMHD_Ck.pad1, fd)== EOF) goto ERROR_EXIT;
    if(put_UWORD(MyHdr.BMHD_Ck.transparentColor, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.BMHD_Ck.xAspect, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.BMHD_Ck.yAspect, fd)== EOF) goto ERROR_EXIT;
    if(put_WORD(MyHdr.BMHD_Ck.pageWidth, fd)== EOF) goto ERROR_EXIT;
    if(put_WORD(MyHdr.BMHD_Ck.pageHeight, fd)== EOF) goto ERROR_EXIT;

#ifdef CAMG
    if(put_LONG(MyHdr.CAMG_CkHdr.ckID, fd)== EOF) goto ERROR_EXIT;
    if(put_LONG(MyHdr.CAMG_CkHdr.ckSize, fd)== EOF) goto ERROR_EXIT;
    if(put_ULONG(MyHdr.CAMG_Ck.ViewModes, fd)== EOF) goto ERROR_EXIT;
#endif

#ifdef CMAP
    if(put_LONG(MyHdr.CMAP_CkHdr.ckID, fd)== EOF) goto ERROR_EXIT;
    if(put_LONG(MyHdr.CMAP_CkHdr.ckSize, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.Map0red, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.Map0green, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.Map0blue, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.Map1red, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.Map1green, fd)== EOF) goto ERROR_EXIT;
    if(put_UBYTE(MyHdr.Map1blue, fd)== EOF) goto ERROR_EXIT;
#endif

    if(put_LONG(MyHdr.BODY_CkHdr.ckID, fd)== EOF) goto ERROR_EXIT;
    if(put_LONG(MyHdr.BODY_CkHdr.ckSize, fd)== EOF) goto ERROR_EXIT;

/*
    memptr = (unsigned char *) &MyHdr;
    for (i=0; i < sizeof(MyHdr) ; i++)
	     putc(memptr[i],fd);
*/
    for (row_c = 0; row_c < pb->nr; row_c++) {

	row = get_RowBuf(pb, pb->nr - row_c - 1);

	for (byte_c = x = 0; byte_c < pb->nb; byte_c++)
	     if( putc(row->buf[byte_c],fd) == EOF) goto ERROR_EXIT;
	if (OddBmp) {
	    putc(0,fd);
	}

	if ((!pg->quiet) && (row_c % 10 == 0))
	    Eprintf (".");
    }
    if (!pg->quiet)
	Eprintf ("\n");

    if (fd != stdout)
	fclose(fd);

  return 0;

ERROR_EXIT:
  PError ("write_ILBM");
ERROR_EXIT_2:
  return ERROR;
}
