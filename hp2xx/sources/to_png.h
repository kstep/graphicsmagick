/*
** to_png.h : include file for hp2xx PNG export module to_png.c
**
** 95/09/26 V 1.00  MS   Produced from Tom Boutells gd.h; deleted all
**                       declarations that correspond to functions that
**                       are not used by to_gif.c
**
** 97/11/26 V 1.00  MS   Copied from to_gif.h, changed to gdImagePNF proto.
*/

/* gd.h: declarations file for the gifdraw module. Version 0.91.

	Written by Tom Boutell, 5/94-6/94.
	Copyright 1994, Cold Spring Harbor Labs.
	Please do not release software using this module
	in other packages until a public release of gd
	itself (standalone) is made. This will occur
	very shortly. Our latest version has many new features
	which you will want to take advantage of, not included
	in this older version. */
 
/* This can't be changed, it's part of the GIF specification. */

#define pdMaxColors 256

/* Image type. See functions below; you will not need to change
	the elements directly. You may trust that sx, sy,
	and colorsTotal can be safely inspected for read-only purposes. */

typedef struct {
	unsigned char ** pixels;
	int sx;
	int sy;
	int colorsTotal;
	int red[pdMaxColors];
	int green[pdMaxColors];
	int blue[pdMaxColors]; 
	int open[pdMaxColors];
	int transparent;
} pdImage;

typedef pdImage * pdImagePtr;

/* Width and height of a character in the gd font, for use in your
	computations. (The font is derived from a public domain
	font in the X11 distribution.) */

#define pdFontWidth 8
#define pdFontHeight 16

/* Functions to manipulate images. */

pdImagePtr pdImageCreate(int , int);
void pdImageDestroy(pdImagePtr);
void pdImageSetPixel(pdImagePtr, int, int, int);
int pdImageGetPixel(pdImagePtr, int, int);
int pdImageBoundsSafe(pdImagePtr, int, int);
int gdImageColorAllocate(pdImagePtr, int, int, int);
void pdImageColorTransparent(pdImagePtr, int);
void pdImagePNG(pdImagePtr, FILE *);


