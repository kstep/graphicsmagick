#include <stdio.h> 

#include "pendef.h"

PEN             pt;

int load_pen_width_table(FILE *td){
   PEN_N          tp;
   PEN_W          tw;
   int             i;

   if (fread( (void *) &tp,sizeof(tp),1,td) != 1) {
      return(0);
   }

   if (fread( (void *) &tw,sizeof(tw),1,td) != 1) {
      return(0);
   }

   if(tp >= NUMPENS) { /* don't check < 0 - unsigned */
      return(1);
   }

   if(tp == 0) {                               /* set all pens */
      for (i = 1; i < NUMPENS; ++i)
         pt.width[i] = tw;
   } else {
      pt.width[tp] = tw;                 /* set just the specified one */
   }

   return(1);
}

int load_pen_color_table(FILE *td){
   PEN_N          tp;
   PEN_C          r,g,b;

   if (fread( (void *) &tp,sizeof(tp),1,td) != 1) {
      return(-1);
   }

   if (fread( (void *) &r,sizeof(r),1,td) != 1) {
      return(-1);
   }

   if (fread( (void *) &g,sizeof(g),1,td) != 1) {
      return(-1);
   }
   
   if (fread( (void *) &b,sizeof(b),1,td) != 1) {
      return(-1);
   }

   if(tp >= NUMPENS) { /* don't check < 0 - unsigned */
      return(1);
   }

   set_color_rgb(tp,r,g,b);
   return(tp);
}

void set_color_rgb(PEN_N index,BYTE r,BYTE g, BYTE b) {
    pt.clut[index][0] = r;
    pt.clut[index][1] = g;
    pt.clut[index][2] = b;
    pt.color[index] = index;
}

