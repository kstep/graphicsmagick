

#ifndef __LINDEF_H
#define __LINDEF_H

typedef enum { LT_solid, LT_adaptive, LT_plot_at, LT_fixed } LineType;   

extern double     CurrentLinePatLen;
extern LineType   CurrentLineType;
extern signed int CurrentLinePattern;

#define LT_MIN -8
#define LT_ZERO 0
#define LT_MAX  8

#define LT_PATTERNS  ((LT_MIN*-1) + 1 + LT_MAX)         /* -8 .. 0 ..  +8  */
#define LT_ELEMENTS   20

#define LT_PATTERN_TOL 0.005  /* 0.5% of pattern length */

typedef signed char SCHAR;    

typedef double LINESTYLE[LT_PATTERNS][LT_ELEMENTS+1];

extern  LINESTYLE lt;

void set_line_style_defaults(void);
void set_line_style(SCHAR index, ...);
void set_line_style_by_UL(FILE *hd);

void print_line_style(void);

#endif /* __LINDEF_H */
