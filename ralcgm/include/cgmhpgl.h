/*  RAL-CGM Interpreter header file: @(#) cgmhpgl.h  version 3.2
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * C D Seelig @ Rutherford Appleton Laboratory (cds@uk.ac.rl.ib)
 *
 * Description:
 *
 * Modification log:
 *
 *     16 Jun 95 RTP Remove REQUAL & VEQUAL definitions
 */

#ifdef CGMOHPGL_C
#ifndef CGMHPGL_H
#define CGMHPGL_H
#include "cgmatt.h"

/*
 * Types used within the HPGL driver
 */

/* Var for Viewport and Window */
static Float x_scale, y_scale, x_tran, y_tran;
static Float Wx1, Wx2, Wy1, Wy2;
static Float wvrat; /* window ratio */

/*  Attribute change flags */
static Logical att_flag = FALSE;

/* CGM attribute structures */

static Sharedatt hpglshared;
static Lineatt hpglline;
static Markeratt hpglmark;
static Textatt hpgltext;
static Fillatt hpglfill;
static Edgeatt hpgledge;

/* Points list */
static Point localpts[ARRAY_MAX];

#define MAXPENS 6+1

#define A4 1
#define A3 2
#define PORTRAIT 4
#define LANDSCAPE 8
#define AUTO 0
#define MANUAL 1

#define MARKER_STAR '*'
#define MARKER_DOT '.'
#define MARKER_OH 'O'
#define MARKER_PLUS '+'
#define MARKER_CROSS 'X'

#define NEW 0
#define CONT 1
#define END 2

#define IFNEED 0
#define FORCE 1

#define BACKMATCH 0
#define NOBACKMATCH 1

#define POINTS_PER_MM 40.2

typedef int Lptype;
typedef struct {
   Lptype x,y;
} Lpt;

typedef struct {
   Float red, green, blue;
} col_spec;

typedef struct {
   col_spec colour;
   Float width;
} pen_spec;

typedef struct {
   int pen;
   int linetype;
} hpglatt;

typedef struct {  /* Cell array Parameters */
   Point p, q, r, d;
   Index nx, ny;
} Cell;

static int pens=6, size=A4, orient=PORTRAIT, paperchange=MANUAL;
static int printcontrol=TRUE;
static Float max_pen_width=0.7;
static pen_spec *pen;
static hpglatt hpglcuratt;
static FILE *out;
static Index backpen = 0;

#define NLINEB  5
#define NMARKB  5
#define NTEXTB  4
#define NFILLB  4
#define NEDGEB  5
#define COLTBSIZE 300

/* default pen colours */
static pen_spec def_pen[] = {
   {{1.0, 1.0, 1.0}, 0.0},
   {{0.0, 0.0, 0.0}, 0.7},
   {{1.0, 0.0, 0.0}, 0.3},
   {{0.0, 1.0, 0.0}, 0.3},
   {{0.0, 0.0, 1.0}, 0.3},
   {{1.0, 1.0, 0.0}, 0.3},
   {{1.0, 0.0, 1.0}, 0.3}};

/* default attributes for the plotter */
static hpglatt hpgldefatt = {
   0,           /* default pen */
   0            /* default linetype */
};


static Point txtpos;  /* Display text position */

/* Text method available */

static Enum text_method[] = {
#ifdef BEZIER
           BEZ,
#endif
#ifdef HERSHEY
           HER,
#endif
           (Enum) 0 };

/* array to map cgm linetype onto HPGL linetype */
static int linestyle[] = {
   0,
   0,
   2,
   1,
   5,
   6
};

static Linebundle line_bundle[NLINEB] = {
      1, 1.0, 1, 1.0, 1.0, 1.0,
      2, 1.0, 2, 1.0, 0.0, 0.0,
      3, 1.0, 3, 0.0, 0.1, 0.0,
      4, 1.0, 4, 0.0, 0.0, 1.0,
      5, 1.0, 5, 1.0, 1.0, 1.0
  };


static Edgebundle edge_bundle[NEDGEB] = {
      1, 1.0, 1, 1.0, 1.0, 1.0,
      2, 1.0, 2, 1.0, 0.0, 0.0,
      3, 1.0, 3, 0.0, 0.1, 0.0,
      4, 1.0, 4, 0.0, 0.0, 1.0,
      5, 1.0, 5, 1.0, 1.0, 1.0
  };

static Markerbundle mark_bundle[NMARKB] = {
      1, 1.0, 1, 1.0, 1.0, 1.0,
      2, 1.0, 2, 1.0, 0.0, 0.0,
      3, 1.0, 3, 0.0, 0.1, 0.0,
      4, 1.0, 4, 0.0, 0.0, 1.0,
      5, 1.0, 5, 1.0, 1.0, 1.0
  };

static Fillbundle fill_bundle [NFILLB] = {
      1, 1, 1, 1, 1.0, 1.0, 1.0,
      1, 2, 2, 2, 1.0, 0.0, 0.0,
      1, 3, 3, 3, 0.0, 1.0, 0.0,
      1, 4, 4, 4, 0.0, 0.0, 1.0
  };

static Textbundle text_bundle [NTEXTB] ={
      1, STROKE, 1.0, 0.0, 1, 1.0, 1.0, 1.0,
      2, STROKE, 1.0, 0.0, 2, 1.0, 0.0, 0.0,
      3, STROKE, 1.0, 0.0, 3, 0.0, 1.0, 0.0,
      4, STROKE, 1.0, 0.0, 4, 0.0, 0.0, 1.0
  };

#ifdef PROTO       /* Standard C use function prototyping */

void CGMOhpgl(FILE *, Code, Long*, Float*, char* ),
     HPGLoptions( char* );

static void HPGLinit(void),
            HPGLclose(void),
            HPGLnewpic(char *),
            HPGLmenu(void),
            HPGLattrib(Code),
            HPGLline(Long , Long *, Float *, Code),
            HPGLmarker(Long, Long *, Float * ),
            HPGLrect(Long *, Float *),
            HPGLtext(Textitem *),
            HPGLcell(Cell, Long *, Float *),
            HPGLgdp(Code , Long *, Float *, Enum),
            HPGLfill(Long , Long *, Float *),
            HPGLwindow(void),
            HPGLtxline(Int, Point *, Tmatrix, int),
            HPGLtxfill(Int ,Point *,Tmatrix,int),
            setcolour(Index, RGBcolour),
            setclip(int),
            HPGLsetdef(void),
            line_r(Long, Lpt *),
            fill_r(Long, Lpt *),
            rect_r(Lpt, Lpt),
            sendP1P2(Lpt, Lpt),
            sendclip(struct rect, int),
            sendcolour(Index),
            sendlinestyle(Index n),
            compoints(int, char *, Lpt);

static Index get_pen_num(int, RGBcolour);

#else

void CGMOhpgl(),      /* Main driver routine */
     HPGLoptions();   /* HPGL Options */

static void HPGLinit(),      /* Initialise Graphics system */
            HPGLclose(),     /* Close down Graphics system */
            HPGLnewpic(),    /* Start a new frame  */
            HPGLmenu(),      /* Select Options */
            HPGLattrib(),    /* Set Attributes */
            HPGLline(),      /* Line draw routine */
            HPGLmarker(),    /* draw polymarkers */
            HPGLrect(),      /* draw rectanlges */
            HPGLtext(),      /* output text */
            HPGLcell(),      /* Draw cell arrays */
            HPGLgdp(),       /* draw GDPs */
            HPGLfill(),      /* polygon fill */
            HPGLwindow(),    /* Set VDC extend */
            setcolour(),     /* Set or match colour */
            setclip(),       /* store clipping information */
            HPGLtxline(),    /* Display hershey text */
            HPGLtxfill(),    /* Display bezier text */
            HPGLsetdef(),    /* Set default locally */
            line_r(),        /* routine to do polylines relative */
            fill_r(),        /* routine to do fillareas relative */
            rect_r(),        /* routine to draw rectangles */
            sendP1P2(),      /* send hpgl P1 and P2 */
            sendclip(),      /* send clipping instruction */
            sendcolour(),    /* send out request for pen change */
            sendlinestyle(), /* send out request for new line style */
            compoints();     /* send out command and points list */

static Index get_pen_num();
#endif

/** Macros which should maybe be global **/
#define HPGLABS(a) (((a)<0)?-(a):(a))

/**  Local Macros - Colour Noramalization **/
#define RGBNORM(c,k) (Float)(c.k-cur.min_rgb.k)/(cur.max_rgb.k-cur.min_rgb.k)

/**   Local Macros - Convert World coordinate to screen coordinate **/
#define XTRAN(wx)   ( (double) ( x_scale * (double)(wx) + x_tran ) )
#define YTRAN(wy)   ( (double) ( y_scale * (double)(wy) + y_tran ) )

/****** Debug Flags *****/
/* #define DEBUG        */
/* #define DEBUG_MARKER */
/* #define DEBUG_FILL   */
/* #define DEBUG_LINE   */
/* #define DEBUG_CLIP   */
/* #define DEBUG_COLOUR */
/* #define DEBUG_BACK   */
/* #define DEBUG_CELL   */
/* #define DEBUG_TXTF   */
/* #define DEBUG_PENS   */
/* #define DEBUG_FILES  */
/************************/
#undef DEBUG
#undef DEBUG_OPTIONS
#undef DEBUG_TXLINE
#undef DEBUG_FILL
#undef DEBUG_PENS
#undef DEBUG_CLIP
#undef DEBUG_FILES
#undef DEBUG_MARKER
#undef DEBUG_COLOUR
#undef DEBUG_CELL

static char *func = "CGMOhpgl", emess[48];

/***      Local Variable      ***/

Lpt P1, P2, ext1, ext2;

static Cell cell;

Logical newpic=FALSE;
static double rt_wid, rt_hgt;
static Logical rt_mode=FALSE;

#endif
#else
This is a private include file!!!
#endif
