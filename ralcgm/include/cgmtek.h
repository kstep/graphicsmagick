/*  RAL-CGM Interpreter header file: @(#) cgmtek.h  version 3.2
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * K E V Palmen @ Rutherford Appleton Laboratory (kevp@uk.ac.rl.ib)
 *
 * Description: Local variables and constants for Tektronix driver
 *
 */

/*  Include all files on which this is dependent.  */
#include "cgmtypes.h"  /* Types */
#include "cgmtext.h"   /* Text prototypes and types */
#include "cgmatt.h"    /* Attribute prototypes and types */
#ifdef BEZIER
#include "cgmbez.h"    /* Bezier Text Utility */
#endif
#ifdef HERSHEY
#include "cgmher.h"    /* Hershey Text Utility */
#endif

/* Character macros */
#ifdef EBCDIC
#define TCHAR(i) (char)(*(cgmebcdic+(i&0xFF)))
#else
#define TCHAR(i) (char)(i)
#endif

/*  Local macros */
#define TEKOUT(s)         (void) fprintf(cgmot, s)   /* Unsuitable for '%' */
#define TEKCHOUT(c)       if ( c == 0x7f )  /* DEL */ \
                             (void) fprintf(cgmot,"%c%c", TCHAR(0x1B), \
                                                          TCHAR(0x3F) );\
                          else  /* Single character */\
                             (void) fputc(TCHAR(c), cgmot)

#define TSCX(x)           (int) (vdcux*((x)-xvdca))
#define TSCY(y)           (int) (vdcuy*((y)-yvdca))
#define T_I(i)            (void) TEKint(i)
#define T_P(x,y)          (void) TEKpoint(x,y)
#define T_R(r)            (void) TEKreal(r)

#define NOW               TEKOUT ("\n")
#define ESC      TEKCHOUT ( 0x1B )

/*  TEK Commands */
#define T_ALPHAMODE            TEKCHOUT ( 0x1F )
#define T_ANSICODE             ESC; TEKOUT ("%%!1")
#define T_ARC(x1,y1,x2,y2)     ESC; TEKOUT ("UC12"); \
                               T_P(x1,y1); T_P(x2,y2)
#define T_BACKINDEX(i)         ESC; TEKOUT ("MB"); T_I(i); T_I(i)
#define T_BEGINPANEL(x,y,i)    ESC; TEKOUT ("LP"); T_P(x,y); T_I(i)
#define T_BORDER               ESC; TEKOUT ("RE0")
#define T_CBACKGROUND          ESC; TEKOUT ("TB000")
#define T_CMAP(i,c)            ESC; TEKOUT ("TG14"); \
                         T_I(i); T_I(c.red); T_I(c.green); T_I(c.blue)
#define T_CMODE                ESC; TEKOUT ("TM111")
#define T_CHORD(x1,y1,x2,y2)   ESC; TEKOUT ("UC22"); \
                               T_P(x1,y1); T_P(x2,y2)
#define T_DAENABLEON           ESC; TEKOUT ("KA1")
#define T_DAVISIBILITY         ESC; TEKOUT ("LV1")
#define T_DRAW(x,y)            ESC; TEKOUT ("LG"); T_P(x,y)
#define T_ECHO                 ESC; TEKOUT ("KE0")
#define T_EOLSTRING            ESC; TEKOUT ("NT1=")
#define T_EOMCHAR              ESC; TEKOUT ("NC=0")
#define T_ENDPANEL             ESC; TEKOUT ("LE")
#define T_ERRORLEVEL           ESC; TEKOUT ("KT2")
#define T_FILLPATTERN(i)       ESC; TEKOUT ("MP"); T_I(i)
#define T_GAMODE               ESC; TEKOUT ("MG1")
#define T_IGNOREDEL            ESC; TEKOUT ("KI1")
#define T_LINEINDEX(i)         ESC; TEKOUT ("ML"); T_I(i)
#define T_LINESTYLE(i)         ESC; TEKOUT ("MV"); T_I(i)
#define T_MOVE(x,y)            ESC; TEKOUT ("LF"); T_P(x,y)
#define T_PIE(x1,y1,x2,y2)     ESC; TEKOUT ("UC32"); \
                               T_P(x1,y1); T_P(x2,y2)
#define T_RENEW                ESC; TEKOUT ("KN0")
#define T_SDEFINITIONS(i)      ESC; TEKOUT ("RD1"); T_I(i)
#define T_TEKCODE              ESC; TEKOUT ("%%!0")
#define T_VECTORMODE           TEKCHOUT ( 0x1D )
#define T_VIEWPORT(jx,jy,kx,ky)  ESC; TEKOUT ("RV"); \
                                 TEKpoint(jx,jy); TEKpoint(kx,ky)
#define T_WINDOW(jx,jy,kx,ky)    ESC; TEKOUT ("RW"); \
                                 TEKpoint(jx,jy); TEKpoint(kx,ky)

/*  Local defines */
#define D_WIDTH  4095.999   /* Display Width in TSC Units */
#define D_HEIGHT 3071.999   /* Display Height in TSC Units */

#define TSCRAST 0.125       /* TSC unit in Raster Units */

#define FINALPOLY 0   /* Final polygon of set */
#define FOLLOWED  1   /* Polygon followed by another in set */

/* Clipping */
#define CLIPPING(x,xmin,xmax) ((x>xmax)||(x<xmin))


/* Colour Defines and Macros */
#define DEF_COLLEV 4      /* Default num' distinct RGB levels per RGB */
#define DEF_BITPLANES 4   /* Default number of bit-planes */
#define NCOLVAL 101       /* Number of valid RGB settings per RGB */
#define LEVCOL(col,lrgb) ((col*lrgb)/NCOLVAL)
                          /* Conversion to distinct RGB level */
#define DEF_N_COLTAB 16   /* Length of hardware default colour table */
#define MAX_N_COLTAB 256  /* Maximum length of hardware colour table */

/* Values of TEKcoluse */
#define NOT_USED      0  /* Colour index not in use */
#define OUT_OF_DATE   1  /* Colour index in use & CMAP needs updating */
#define UP_TO_DATE    2  /* Colour index in use & CMAP up to date */

/* Minimum, Nominal and Maximum Attribute Values */
#define MNMKSZ 0.5                /* Minimum Marker Size in Nominals */
#define NMMKSZ (50.0/(vdc_unit))  /* Nominal Marker Size in VDC units */

/* Number of predefined bundles for each primitive */
#define NLINEB 5
#define NEDGEB 5
#define NMARKB 5
#define NFILLB 5
#define NTEXTB 5
#define NDEVS  4

/* Index and RGB Values of Colours in Default Bundles */
#define WHITE   1, 1.0, 1.0, 1.0
#define RED     2, 1.0, 0.0, 0.0
#define GREEN   3, 0.0, 1.0, 0.0
#define BLUE   11, 0.0, 0.6, 1.0
#define YELLOW  5, 1.0, 1.0, 0.0

/* Various output primitives bundles information  - Polyline */
static struct linebundle line_bundle[NLINEB] =
{
 1, 1.0, WHITE,
 2, 1.0, RED,
 3, 1.0, GREEN,
 4, 1.0, BLUE,
 5, 1.0, YELLOW
};

/* Edge Control Bundles */
static struct edgebundle edge_bundle[NEDGEB] =
{
 1, 1.0,  WHITE,
 2, 1.0,  RED,
 3, 1.0,  GREEN,
 4, 1.0,  BLUE,
 5, 1.0,  YELLOW
};

/* Polymarker bundles */
static struct markerbundle mark_bundle[NMARKB] =
{
 1, 1.0, WHITE,
 2, 1.0, RED,
 3, 1.0, GREEN,
 4, 1.0, BLUE,
 5, 1.0, YELLOW
};

/* Fill area bundles */
static struct fillbundle fill_bundle[NFILLB] =
{
 1, 1, 1, WHITE,
 1, 2, 2, RED,
 1, 3, 3, GREEN,
 1, 4, 4, BLUE,
 1, 5, 5, YELLOW
};

/* Text bundles */
static struct textbundle text_bundle[NTEXTB] =
{
 1, STROKE, 1.0, 0.0 , WHITE,
 2, STROKE, 1.0, 0.0 , RED,
 3, STROKE, 1.0, 0.0 , GREEN,
 4, STROKE, 1.0, 0.0 , BLUE,
 5, STROKE, 1.0, 0.0 , YELLOW
};

/* Tektronix colour table variables*/
static n_coltab = DEF_N_COLTAB;
static RGBcolour TEKcoltab[MAX_N_COLTAB];   /* The TEK colour table */
static Enum      TEKcoluse[MAX_N_COLTAB];   /* The TEK colour flags */
static Int colset_n;        /* Number of colour table entries set */
static Logical cmap_change; /* Surface colour map is out of date */
static Int n_bitp = DEF_BITPLANES;          /* Number of bitplanes */
static Int n_collev = DEF_COLLEV;           /* Number of colour levels */
static RGBcolour black = { 0, 0, 0 };  /* black  */

/* Default Colour Table */
/* TEKdftcoltab should contain DEF_N_COLTAB entries of r g b triples  */
/* The r g b triples are integers in the range 0 to 100 inclusive */
/* as required by the TEK Set Surface Colour Map command. */
static RGBcolour TEKdftcoltab[DEF_N_COLTAB] =
{
      0,   0,   0,  /* 0 Black */
    100, 100, 100,  /* 1 White */
    100,   0,   0,  /* 2 Red   */
      0, 100,   0,  /* 3 Green */
      0,   0, 100,  /* 4 Blue  */
    100, 100,   0,  /* 5 Yellow */
      0, 100, 100,  /* 6 Cyan   */
    100,   0, 100,  /* 7 Magenta */
    100,  50,   0,  /* 8 Orange */
     51, 100,   0,  /* 9 Yellow-Green */
      0, 100,  51,  /* 10 Cyan-Green */
      0,  51, 100,  /* 11 Blue */
     50,   0, 100,  /* 12 Violet */
    100,   0,  50,  /* 13 Red */
     33,  33,  33,  /* 14 Dark Grey */
     67,  67,  67,  /* 15 Light Grey */
};

/*  External variables used by the driver */

extern char *cgmofile;                  /* The name of the output file */

/* Variables seen by all modules in cgmotek.c */

static Logical tl_first = TRUE;  /* TEKline first chunk flag */
static Logical tl_final;         /* TEKline final chunk flag */
static int tlstyle[] = {0,3,1,2,5};  /* Tek line style from linestyle */

static Point tekpt[ARRAY_MAX];  /* Points list array */

static Float vdc_unit; /* VDC Unit in Terminal Space Units */
static Float vdcux;  /* VDC Unit Vector in Terminal Space Units (X) */
static Float vdcuy;  /* VDC Unit Vector in Terminal Space Units (Y) */
static Float xvdca;  /* X-coord of 1st corner of VDC extent in VDCs */
static Float yvdca;  /* Y-coord of 1st corner of VDC extent in VDCs */
static Float nrast;  /* Number of raster units per VDC */

static struct lineatt tekline;
static struct markeratt tekmark;
static struct textatt tektxt;
static struct fillatt tekfill;
static struct edgeatt tekedge;
static struct sharedatt tekshare;

static Logical edgeincl;   /* Edge to be drawn by TEKfill */
static Logical edgeafter;  /* Edge needed to be drawn after TEKfill */
static Logical edgesetaft; /* Edge needed to be drawn after TEKfset */

static Code cdraw_mode; /* Drawing mode for TEKcdraw */

static Point txtpos;    /* Text position */
static Point txtcorner; /* Corner opposite & relative to text position */
static Logical restrtxt; /* True if, text has been restricted */

/* Text method available */

static Enum preftxmeth[] = {
#ifdef BEZIER
           BEZ,
#endif
#ifdef HERSHEY
           HER,
#endif
/*         HDW,             Hardware Text Removed till developed */
           (Enum) 0 };


/*  Cell array Parameters  */
static struct {
   Point p, q, r;
   Int nx, ny, lp;
   Tmatrix tm;
} cell;


/* To be included in cgmgraph */
#define CLIPCODE(x,xmin,xmax) (((x>xmax)<<1)|(x<xmin))
/* Values of Clipcode */
#define CLIPNO    (Enum) 0
#define CLIPMIN   (Enum) 1
#define CLIPMAX   (Enum) 2
/* Values of edgestatus */
#define INCOMING  (Enum) 1
#define INSIDE    (Enum) 0
#define OUTGOING  (Enum) 2
#define BOTHOUT   (Enum) 3
/* Clip function */
#define ECLIP(y1,y2,x1,x2,xm) y1+(xm-x1)*(y2-y1)/(x2-x1)
/* Clockwise function for 3 points */
#define CLOCK(a,b,c) ( ((b)->x - (a)->x)*((c)->y - (a)->y)\
                     > ((c)->x - (a)->x)*((b)->y - (a)->y) )

#ifdef DEBUG_CMAP
#define TEKCOLTAB(i) TEKcoltab[i].red, TEKcoltab[i].green, \
                     TEKcoltab[i].blue, TEKcoluse[i]
#endif
/* End of Tektronix include file */

