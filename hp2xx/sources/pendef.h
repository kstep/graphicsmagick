

#ifndef __PENDEF_H
#define __PENDEF_H

#define NUMPENS 256

/**
 ** Color codes used within hp2xx
 **/

#define xxBackground    0
#define xxForeground    1
#define xxRed           2
#define xxGreen         3
#define xxBlue          4
#define xxCyan          5
#define xxMagenta       6
#define xxYellow        7


typedef unsigned short int PEN_N;
typedef unsigned short int PEN_W;
typedef unsigned short int PEN_C;
typedef unsigned char BYTE;

typedef struct {
   PEN_W width[NUMPENS+1];      /* -p xxxxxxxx / PW             */
   PEN_C color[NUMPENS+1];      /* -c xxxxxxxx / PC             */
   BYTE  clut[NUMPENS+9][3];    /* color lookup table           */
} PEN;

extern PEN pt;

void set_color_rgb(PEN_N index,BYTE r,BYTE g, BYTE b);
int load_pen_width_table(FILE *td);
int load_pen_color_table(FILE *td);

#endif /* __PENDEF_H */
