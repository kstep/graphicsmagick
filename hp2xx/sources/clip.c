/*
*   Parts Copyright (c) 1999  Martin Kroeker  All rights reserved.
*   (based on code written by Martin Skiba when working for Daveg GmbH,
*   used here with the permission of Daveg GmbH, Darmstadt,Germany) */   
/*	Clip.C	Release : 2.2	Date : 12/09/93	by sk	*/
/*--------------------------------------------------------------------
*	Clip.C
*
*	Clipping von Geraden in Integerdarstellung nach einen
*	Mischmasch der Algorythmen von Cohen-Sutherland ( Bereichstests)
*	und Liang-Barsky ( Durchfœhrung )
*/

#include <stdio.h>
#include <math.h>
#include "clip.h"

        
#define CLIP_INSIDE 0
#define CLIP_RIGHT 1
#define CLIP_BELOW 2
#define CLIP_ABOVE 4
#define CLIP_LEFT 8

#define ClipSurelyOutside(code1,code2) (code1 & code2)
#define ClipFullyInside(code1,code2) (! (code1 | code2 ))

static int ClipAreaCode (
	double x1,double y1,
	double x2,double y2,
	double px, double py)
{
	 short code = CLIP_INSIDE;

	if (px < x1-1.e-3 ) code |= CLIP_LEFT;
	else if ( px > x2+1.e-3 ) code |= CLIP_RIGHT;
	
	if (py < y1-1.e-3 ) code |= CLIP_BELOW;
	else if ( py > y2+1.e-3 ) code |= CLIP_ABOVE;
	
	return code;
}		
	 
static int ClipWithBorder(
	double delta,
	double diff,
	double *tEnter,
	double *tLeave)
{
	double t;
	t = diff / delta;

if (fabs(t)<1.e-5) return 0;
	if ( delta < 0.0 ) {
		if ( t > *tLeave ) return 0;
		else if ( t > *tEnter ) *tEnter = t;
		}
	else 	{
		if ( t < *tEnter ) return 0;
		else if ( t < *tLeave ) *tLeave = t;
		}
	return 1;
}
		

short DtClipLine(
	double xmin, double ymin,
	double xmax, double ymax,
	double *x1, double *y1, double *x2, double *y2)

{
	short area_code1,area_code2;
	double eps;

	eps = 1.e-10;
	area_code1= ClipAreaCode(xmin,ymin,xmax,ymax,*x1,*y1);
	area_code2= ClipAreaCode(xmin,ymin,xmax,ymax,*x2,*y2);

	if (ClipFullyInside(area_code1,area_code2) ) return CLIP_DRAW;
	
	if (ClipSurelyOutside(area_code1,area_code2)) {
/*	fprintf(stderr,"clipped line from %f %f to %f %f\n",*x1,*y1,*x2,*y2);*/
	 return CLIP_NODRAW;
	 }
	 else
	{
	double dx,dy;
	int dx0,dy0;
	double tEnter,tLeave;
	double *tE = &tEnter;
	double *tL = &tLeave;

		dx = *x2 - *x1;
		dy = *y2 - *y1;
		tEnter = 0.0 ; tLeave = 1.;
		dx0 = ( -eps < dx && dx < eps);
		dy0 = ( -eps < dy && dy < eps);

		if (dx0 ||  ClipWithBorder(-dx,-(xmin-*x1),tE,tL))
			if (dy0 || ClipWithBorder(dy,(ymax-*y1),tE,tL))
				if (dx0 || ClipWithBorder(dx,(xmax-*x1),tE,tL))
					if (dy0 || ClipWithBorder(-dy,-(ymin-*y1),tE,tL)) {
			*x2 = *x1 + (tLeave * dx);
			*y2 = *y1 + (tLeave * dy);
			*x1 = *x1 + (tEnter * dx);
			*y1 = *y1 + (tEnter * dy);
                 
		return CLIP_DRAW;
					}
	}
	return CLIP_NODRAW;
}



