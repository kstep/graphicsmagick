/**
 ** to_png.c: Portable Network Graphics (PNG) converter part of project "hp2xx"
 ** Michael Schmitz, schmitz@simul.biophys.uni-duesseldorf.de
 ** Derived from: to_gif.c (GIF module; no longer supported due to copyright 
 **                         problems with the GIF-internal LZW compression)
 ** 94/08/22 V 1.00  MS   started from scratch, using to_pbm.c and Tom Boutells
 **                       gd.c gifdraw module; simple merge of both modules :-)
 **                       (see gd.c and gifencod copyright notes below)
 **                       Bresnham line drawing and fonts: own hp2xx code used
 **                       already _before_ this routine is called.
 **                       
 ** 95/09/26 V 1.10  MS   Major cleanup; removed unused code portions.
 **                       Routines used from gifdraw code:
 **                       gdImageCreate, gdImageColorAllocate, gdImageSetPixel,
 **                       gdImageBoundsSafe, gdImageGetPixel, gdImageGif, 
 **                       gdImageDestroy.
 **                       (gdImageGif in turn uses routines from GIFEncode that
 **                       is part of the pbmplus package.
 **
 ** 97/11/26 V 1.0   MS   Rewrite to libpng 0.96 using Tom Boutell's pixel 
 **                       drawing primitives
 **                       
 **/

#include <stdio.h>
#include <stdlib.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "to_png.h"
#include "pendef.h"
#define GGE >>=
#define MAXOUTPUTROWS 70

#define PDNCOL 256 

int pdImageColorAllocate(pdImagePtr, int, int , int);

int
PicBuf_to_PNG (const GEN_PAR *pg, const OUT_PAR *po)
{
FILE           *fd;
int             row_c, /*byte_c,*/ x;
const RowBuf   *row;
const PicBuf   *pb;
int	       ppm[][3] = { {255, 255, 255}, {0,0,0} };
/*, {255,0,0}, {0,255,0},
		{0,0,255},{0,255,255},{255,0,255},{255,255,0}};
*/
int		colour;

/**
 ** gifdraw-parts
 **/
pdImagePtr im;
int pdcol;

  if (pg == NULL || po == NULL)
	return ERROR;
  pb = po->picbuf;
  if (pb == NULL)
	return ERROR;

  if (!pg->quiet)
	Eprintf("\nWriting PNG output: %s\n", po->outfile);
  if (*po->outfile != '-')
  {

/*
#ifdef VAX
	if ((fd = fopen(po->outfile, WRITE_BIN, "rfm=var", "mrs=512")) == NULL)
#else
*/
	if ((fd = fopen(po->outfile, WRITE_BIN)) == NULL)
/*
#endif
*/
		goto ERROR_EXIT;
  }
  else
	fd = stdout;

/**
 ** create image structure
 **/
  im = pdImageCreate(pb->nc, pb->nr);

  if (pb->depth > 1)
  {
/** 
 ** allocate some colors ( ?? eight colors supported by hp2xx ?? )
 **/
  for ( colour = 0; colour < PDNCOL; colour++ ) 
/*  pdcol = pdImageColorAllocate(im, ppm[colour][0], ppm[colour][1], 
				    ppm[colour][2]);
*/
	pdcol = pdImageColorAllocate(im, pt.clut[colour][0],pt.clut[colour][1],
					pt.clut[colour][2]);
    for (row_c = 0; row_c < pb->nr; row_c++)
    {
	row = get_RowBuf(pb, pb->nr - row_c - 1);
	if (row == NULL)
		continue;

	for (x = 0; x < pb->nc; x++)
	{
	    colour = index_from_RowBuf(row, x, pb);
            pdImageSetPixel(im, x, row_c, colour);
	}
	if ((!pg->quiet) && (row_c % 10 == 0))
	    /* For the impatients among us ...	 */
	    Eprintf(".");
    }
  }
  else
  {
/** 
 ** allocate two colors ( ?? eight colors supported by hp2xx ?? )
 **/
    for ( colour = 0; colour < 2; colour++ ) 
    pdcol = pdImageColorAllocate(im, ppm[colour][0], ppm[colour][1], 
				    ppm[colour][2]);

    for (row_c = 0; row_c < pb->nr; row_c++)
    {
	row = get_RowBuf(pb, pb->nr - row_c - 1);
	if (row == NULL)
		continue;

	for (x = 0; x < pb->nc; x++)
	{
	    colour = index_from_RowBuf(row, x, pb);
            pdImageSetPixel(im, x, row_c, colour);
	}

	if ((!pg->quiet) && (row_c % 10 == 0))
	    /* For the impatients among us ...	 */
	    Eprintf(".");
    }
  }

  pdImagePNG(im, fd);

  pdImageDestroy(im);

  fflush(fd);

  if (!pg->quiet)
	Eprintf("\n");
  if (fd != stdout)
	fclose(fd);
  return 0;

ERROR_EXIT:
  PError ("write_PNG");
  return ERROR;
}

/**
 ** PNG image support routines, derived from:
 **/

/* gd.c: implementation of the gifdraw module. Version 0.9.

        Written by Tom Boutell, 5/94-6/94.
        Copyright 1994, Cold Spring Harbor Labs.
        Permission granted to use this code in any fashion provided
        that this notice is retained and any alterations are
        labeled as such. It is requested, but not required, that
        you share extensions to this module with us so that we
        can incorporate them into new versions. */
/**
 ** PNG: use sy rows of sx pixels (instead of sx columns of sy pixels)
 **/

pdImagePtr pdImageCreate(sx, sy)
	int sx;
	int sy;
{
	int i;
	pdImagePtr im;
	im = (pdImage *) malloc(sizeof(pdImage));
	im->pixels = (unsigned char **) malloc(sizeof(unsigned char *) * sy);
	for (i=0; (i<sy); i++) {
		im->pixels[i] = (unsigned char *) calloc(
			sx, sizeof(unsigned char));
	}	
	im->sx = sx;
	im->sy = sy;
	im->colorsTotal = 0;
	im->transparent = (-1);
	return im;
}

void pdImageDestroy(im)
	pdImagePtr im;
{
	int i;
	for (i=0; (i<im->sy); i++) {
		free(im->pixels[i]);
	}	
	free(im->pixels);
	free(im);
}

int pdImageColorAllocate(im, r, g, b)
	pdImagePtr im;
	int r;
	int g;
	int b; 
{
	int i;
	int ct = (-1);
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			ct = i;
			break;
		}
	}	
	if (ct == (-1)) {
		ct = im->colorsTotal;
		if (ct == pdMaxColors) {
			return -1;
		}
		im->colorsTotal++;
	}
	im->red[ct] = r;
	im->green[ct] = g;
	im->blue[ct] = b;
	im->open[ct] = 0;
	return ct;
}

void pdImageColorTransparent(im, color)
	pdImagePtr im;
	int color;
{
	im->transparent = color;
}

void pdImageSetPixel(im, x, y, color)
	pdImagePtr im;
	int x; 
	int y;
	int color;
{
	if (pdImageBoundsSafe(im, x, y)) {
		 im->pixels[y][x] = color;
	}
}

int pdImageGetPixel(im, x, y)
	pdImagePtr im; 
	int x; 
	int y; 
{
	if (pdImageBoundsSafe(im, x, y)) {
		return im->pixels[y][x];
	} else {
		return 0;
	}
}

int pdImageBoundsSafe(im, x, y)
	pdImagePtr im;
	int x;
	int y; 
{
	return (!(((y < 0) || (y >= im->sy)) ||
		((x < 0) || (x >= im->sx))));
}

