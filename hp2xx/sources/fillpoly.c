#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "bresnham.h"
#include "hp2xx.h"

void fill(HPGL_Pt polygon[MAXPOLY], int numpoints,HPGL_Pt P1, HPGL_Pt P2,
int scale_flag,int filltype,float spacing,float hatchangle)
{
double pxmin,pxmax,pymin,pymax;
double scanx1,scanx2,scany1,scany2;
HPGL_Pt segment[MAXPOLY];
double segx,segy;
int i, j, k, jj;
int numlines;
double penwidth = 1.;
HPGL_Pt p;
double denominator;
double tmp,rot_ang;
double pxdiff=0.,pydiff=0.;
double A1,B1,C1,A2,B2,C2;
double tmp2;

if (filltype >2) penwidth=spacing;
/*fprintf(stderr,"edges to test : %d\n",numpoints);*/

pxmin=P2.x;
pymin=P2.y;
pxmax=P1.x;
pymax=P1.y;
pxmin=P1.x;
pymin=P1.y;
pxmax=P2.x;
pymax=P2.y;

pxmin=100000.;
pymin=100000.;
pxmax=-100000.;
pymax=-100000.;

if (hatchangle >89.9 && hatchangle < 180.) {  
	hatchangle = hatchangle-90.;
/*fprintf(stderr,"vertical fill\n");*/
        goto FILL_VERT;
        }

for (i = 0 ; i <= numpoints; i++ ) {
pxmin=MIN(pxmin,polygon[i].x);
pymin=MIN(pymin,polygon[i].y);
pxmax=MAX(pxmax,polygon[i].x);
pymax=MAX(pymax,polygon[i].y);
}
pxmin=P1.x;
pymin=P1.y;
if (pxmin == pxmax && pymin == pymax){
/*fprintf(stderr,"zero area polygon\n");*/
return;
}



/*****?*pymin=P1.y-1.;pymax=P2.y+1.;*****?*/

pydiff=pymax-pymin;
pxdiff=pxmax-pxmin;
if (hatchangle != 0.) {
rot_ang=tan(M_PI*hatchangle/180.);
pymin=pymin-rot_ang*pxdiff;
pymax=pymax+rot_ang*pxdiff;
}
/*********
pxmin=pxmin-10.;
pxmax=pxmax+10.;
pymin=pymin-10.;
pymax=pymax+10.;
pydiff=pymax-pymin;
**********/
numlines = fabs(1. +  ( pymax - pymin +penwidth) / penwidth);
#if 0
/* debug code to show shade box */
	p.x=pxmin;
	p.y=pymin;
   	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	p.x=pxmin;
	p.y=pymax;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
	p.x=pxmax;
	p.y=pymax;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
	p.x=pxmax;
	p.y=pymin;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
	p.x=pxmin;
	p.y=pymin;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
#endif

/* start at lowest y , run scanlines parallel x across polygon */
/* looking for intersections with edges */

pydiff=0.;

if (hatchangle != 0.) pydiff=tan(M_PI*hatchangle/180.)*pxdiff;
for (i = 0; i <= numlines; i++) { /* for all scanlines ...*/ 
k=-1;
scany1 = pymin + (double)i *  penwidth;
scany2=scany1+pydiff;
if(scany1 >= pymax || scany1<=pymin) {
/*fprintf(stderr,"zu weit\n");*/
continue;
}
/* coefficients for current scan line */
A1=scany2-scany1;
B1=pxmin-pxmax;
C1=pxmin*(scany1-scany2) + scany1*(pxmax-pxmin);
	   
for (j =0 ; j <= numpoints  ; j=j+2) { /*for all polygon edges*/

/* coefficients for this edge */
A2=polygon[j+1].y-polygon[j].y;
B2=polygon[j].x-polygon[j+1].x;
C2=polygon[j].x*(polygon[j].y-polygon[j+1].y) + polygon[j].y*(polygon[j+1].x-polygon[j].x);

#if 0
/* debug code to show outline */
	p.x=polygon[j].x;
	p.y=polygon[j].y;
   	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	p.x=polygon[j+1].x;
	p.y=polygon[j+1].y;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
#endif

/*determine coordinates of intersection */
denominator = A1*B2-A2*B1;
if (fabs(denominator)>1.e-20){ /* zero means parallel lines */

segx= (B1*C2-B2*C1) /denominator;     /*x coordinate of intersection */
segy= (C1*A2-C2*A1) /denominator;     /*y coordinate of intersection */



 if ((segy < MIN(polygon[j].y,polygon[j+1].y)) ||
     (segy > MAX(polygon[j].y,polygon[j+1].y)) ||
     (segx < MIN(polygon[j].x,polygon[j+1].x)) ||
     (segx > MAX(polygon[j].x,polygon[j+1].x)) ) {
/*fprintf(stderr,"intersection  at %f %f is not within (%f,%f)-(%f,%f)\n",segx,segy,polygon[j].x,polygon[j].y,polygon[j+1].x,polygon[j+1].y ) ; */
     }else{
                        k++;
          		segment[k].x=segx;
			segment[k].y=segy;

/*fprintf(stderr,"fill: intersection %d with line %d at (%f %f)\n",k,j,segx,segy);*/
			if (k >0) {
			for (jj=0;jj<k;jj++){
if ( (fabs(segment[jj].x-segment[k].x) < 1.e-10 )
			&& (fabs(segment[jj].y-segment[k].y) < 1.e-10) ){
			k--;
			break;
			}
		        }

         		for (jj=0;jj<k;jj++){
	         		if (segment[k].x<segment[jj].x){
		          		tmp=segment[jj].x;
		          		tmp2=segment[jj].y;
			         	segment[jj].x=segment[k].x;
			         	segment[jj].y=segment[k].y;
				        segment[k].x=tmp;
				        segment[k].y=tmp2;
				}
			}
		  } /* if not the first intersection */
          	} /* if crossing withing range */ 
    } /*if not parallel*/
#if 0    
else if (scany1 == polygon[j].y) {
fprintf(stderr,"para\n");
		segment[++k].x=MAX(polygon[j].x,polygon[j+1].x);
		segment[k].y=polygon[j].y;
			for (jj=0;jj<k;jj++){
if ( (fabs(segment[jj].x-segment[k].x) < 1.e-10 )
			&& (fabs(segment[jj].y-segment[k].y) < 1.e-10) ){
			k--;
			break;
			}
		        }
         		for (jj=0;jj<k;jj++){
	         		if (segment[k].x<segment[jj].x){
		          		tmp=segment[jj].x;
		          		tmp2=segment[jj].y;
			         	segment[jj].x=segment[k].x;
			         	segment[jj].y=segment[k].y;
				        segment[k].x=tmp;
				        segment[k].y=tmp2;
				}
			}
		segment[++k].x=MIN(polygon[j].x,polygon[j+1].x);
		segment[k].y=polygon[j].y;
			for (jj=0;jj<k;jj++){
if ( (fabs(segment[jj].x-segment[k].x) < 1.e-10 )
			&& (fabs(segment[jj].y-segment[k].y) < 1.e-10) ){
			k--;
			break;
			}
		        }
         		for (jj=0;jj<k;jj++){
	         		if (segment[k].x<segment[jj].x){
		          		tmp=segment[jj].x;
		          		tmp2=segment[jj].y;
			         	segment[jj].x=segment[k].x;
			         	segment[jj].y=segment[k].y;
				        segment[k].x=tmp;
				        segment[k].y=tmp2;
				}
			}
	}
#endif
 } /*next edge */   

if (k>0) {
/* fprintf(stderr, "%d segments for scanline %d\n",k,i);*/
   	for (j=0;j<k;j=j+2) {
/*fprintf(stderr, "segment (%f,%f)-(%f,%f)\n",segment[j].x,segment[j].y,segment[j+1].x,segment[j+1].y);*/
	p.x=segment[j].x;
	p.y=segment[j].y;
   	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	p.x=segment[j+1].x;
	p.y=segment[j+1].y;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
  	}
}else{
#if 0
/* debug code to show scanlines :*/
   	p.x=pxmin;
   	p.y=scany1;
   	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
   	p.x=pxmax;
   	p.y=scany2;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
#endif	
}

} /* next scanline */


if (filltype !=4) return;


FILL_VERT:

pxmin=P2.x;
pymin=P2.y;
pxmax=P1.x;
pymax=P1.y;

for (i = 0 ; i <= numpoints; i++ ) {
pxmin=MIN(pxmin,polygon[i].x);
pymin=MIN(pymin,polygon[i].y);
pxmax=MAX(pxmax,polygon[i].x);
pymax=MAX(pymax,polygon[i].y);
}

/*pxmin=P1.x-1.;pxmax=P2.x+1.;*/


/*pymin=P1.y-10.;pymax=P2.y+10.; ???*/

pxmin=P1.x;
pymin=P1.y;

pydiff=pymax-pymin;
if (hatchangle != 0.) {
rot_ang=tan(M_PI*hatchangle/180.);
pxmin=pxmin-rot_ang*pydiff;
pxmax=pxmax+rot_ang*pydiff;
}

pymin=pymin-1.;
pymax=pymax+1.;

numlines = fabs(1. + ( pxmax - pxmin  +penwidth) / penwidth);
/*fprintf(stderr,"numlines = %d\n",numlines);*/

pxdiff=0.;
if (hatchangle != 0.) pxdiff=tan(M_PI*hatchangle/180.)*(pymax-pymin);
for (i = 0; i <= numlines; ++i) { /* for all scanlines ...*/ 
k=-1;
scanx1=pxmin + (double)i * penwidth;
scanx2=scanx1 - pxdiff;

/* coefficients for current scan line */
A1=pymax-pymin;
B1=scanx1-scanx2;
C1=scanx1*(pymin-pymax) + pymin*(scanx2-scanx1);
	   
for (j =0 ; j <= numpoints  ; j=j+2) { /*for all polygon edges*/

/* coefficients for this edge */
A2=polygon[j+1].y-polygon[j].y;
B2=polygon[j].x-polygon[j+1].x;
C2=polygon[j].x*(polygon[j].y-polygon[j+1].y) + polygon[j].y*(polygon[j+1].x-polygon[j].x);


/*determine coordinates of intersection */
denominator = A1*B2-A2*B1;
if (fabs(denominator)>0.){ /* zero means parallel lines */

segx= (B1*C2-B2*C1) /denominator;     /*x coordinate of intersection */
segy= (C1*A2-C2*A1) /denominator;     /*y coordinate of intersection */

 if ((segy < MIN(polygon[j].y,polygon[j+1].y)) ||
     (segy > MAX(polygon[j].y,polygon[j+1].y)) ||
     (segx < MIN(polygon[j].x,polygon[j+1].x)) ||
     (segx > MAX(polygon[j].x,polygon[j+1].x)) ) {
/*fprintf(stderr,"intersection  at %f %f is not within (%f,%f)-(%f,%f)\n",segx,segy,polygon[j].x,polygon[j].y,polygon[j+1].x,polygon[j+1].y ) ; */
     }else{
                        k++;
          		segment[k].x=segx;
			segment[k].y=segy;

/*fprintf(stderr,"fill: intersection %d with line %d at (%f %f)\n",k,j,segx,segy);*/
			if (k >0) {
			for (jj=0;jj<k;jj++){
if ( (fabs(segment[jj].x-segment[k].x) < 1.e-10 )
			&& (fabs(segment[jj].y-segment[k].y) < 1.e0) ){
			k--;
			break;
			}
		        }

         		for (jj=0;jj<k;jj++){
	         		if (segment[k].y<segment[jj].y){
		          		tmp=segment[jj].x;
		          		tmp2=segment[jj].y;
			         	segment[jj].x=segment[k].x;
			         	segment[jj].y=segment[k].y;
				        segment[k].x=tmp;
				        segment[k].y=tmp2;
				}
			}
		  } /* if not the first intersection */
          	} /* if crossing withing range */ 
    } /*if not parallel*/
else if (scanx1 == polygon[j].x) {
	k=-1;
	}
 } /*next edge */   
if (k>0) {
/* fprintf(stderr, "%d segments for scanline %d\n",k,i);*/
   	for (j=0;j<k;j=j+2) {
/*fprintf(stderr, "segment (%f,%f)-(%f,%f)\n",segment[j].x,segment[j].y,segment[j+1].x,segment[j+1].y);*/
	p.x=segment[j].x;
	p.y=segment[j].y;
   	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	p.x=segment[j+1].x;
	p.y=segment[j+1].y;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
  	}
}else{
#if 0
/* debug code to show scanlines :*/
   	p.x=pxmin;
   	p.y=scany1;
   	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
   	p.x=pxmax;
   	p.y=scany2;
   	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
#endif	
}

} /* next scanline */
}

