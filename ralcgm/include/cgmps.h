/*  RAL-CGM Interpreter header file: @(#) cgmps.h  version 3.4
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
 * R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description: Local variables and constants for PostScript driver
 *
 * Maintenance Log:
 *   9 Feb 94 RTP  Changed default A4 sizes and added Dec LP20
 *  26 Jul 94 KEVP Make PUTMSG output to cgmerr instead of stderr
 */

/*  Include all files on which this is dependent.  */
#include "cgmtypes.h"
#include "cgmtext.h"
#include "cgmatt.h"

/*  Local macros */

#define PUTMSG           (void) fprintf( (FILE *) cgmerr,
#define PSCOLOUR(x, col) ( (float)(x - cur.min_rgb.col) \
                           / (cur.max_rgb.col - cur.min_rgb.col) )
#define PSOUT            (void) fprintf( cgmop,
#define SQR(num)         ( (num) * (num) )

/*  Local defines */
#define EO  (Enum) 0             /* eoclip        */
#define NZ  (Enum) 1             /* non-zero clip */
#define NOTCLOSED (Enum) 0       /* GDP closure flag */
#define CLOSED    (Enum) 1       /* GDP closure flag */
#define PT_PER_IN 72.0           /* Number of points per inch */
#define COLTAB_N 64              /* Default size of colour table */


/* Number of predefined bundles for each primitive */
#define NLINEB 5
#define NEDGEB 5
#define NMARKB 5
#define NFILLB 5
#define NTEXTB 5

/*  PostScript prefered Text methods */

static Enum PSprefmeth[]={PS,
#ifdef BEZIER
                          BEZ,
#endif
#ifdef HERSHEY
                          HER,
#endif
                          (Enum)0};

/*
   Structure for exchanging PS routines return values information.
*/

typedef struct Dpoint
{
 double x,y;
} DPoint;

typedef struct PSarcref
{
 DPoint cent,start,end; /* centre of circle, start and end points of the arc.*/
 double r;             /* radius */
 double startang;      /* start angle  */
 double endang;        /* End angle    */
}PS_arcref;


typedef struct PSpattablerec
{
 Index patidx;     /* Pattern index */
 int nx, ny;       /* Dimemsion of the pattern is nx X ny */
 Prec prec;        /* precision of pattern                */
 float *pathead;     /* pointer to the head of pattern data */
}PS_pattabtype;

struct PSdevtable
{
  int res;         /* resolution of PostScript device in dpi */
  int x, y, X, Y;  /* the limits of the drawable display surface in points */
                   /* specified by lower left and upper right corners */
};

/* Various output primitives bundles information  - Polyline */
static struct linebundle PSline_bundle[NLINEB] =
{
 1, 1.0, 1, 0.0000, 0.0000 ,0.0000,
 2, 1.0, 2, 0.0161, 0.0161, 0.0161,
 3, 1.0, 3, 0.0323, 0.0323, 0.0323,
 4, 1.0, 4, 0.0484, 0.0484, 0.0484,
 5, 1.0, 5, 0.0645, 0.0645, 0.0645
};

/* Edge Control Bundles */
static struct edgebundle PSedge_bundle[NEDGEB] =
{
 1, 1.0, 1, 0.0000, 0.0000 ,0.0000,
 2, 1.0, 2, 0.0161, 0.0161, 0.0161,
 3, 1.0, 3, 0.0323, 0.0323, 0.0323,
 4, 1.0, 4, 0.0484, 0.0484, 0.0484,
 5, 1.0, 5, 0.0645, 0.0645, 0.0645
};

/* Polymarker bundles */
static struct markerbundle PSmark_bundle[NMARKB] =
{
 1, 1.0, 1, 0.0000, 0.0000 ,0.0000,
 2, 1.0, 1, 0.0000, 0.0000 ,0.0000,
 3, 1.0, 1, 0.0000, 0.0000 ,0.0000,
 4, 1.0, 1, 0.0000, 0.0000 ,0.0000,
 5, 1.0, 1, 0.0000, 0.0000 ,0.0000
};

/* Fill area bundles */
static struct fillbundle PSfill_bundle[NFILLB] =
{
 1, 1, 1, 1, 0.0000, 0.0000 ,0.0000,
 2, 1, 1, 1, 0.0000, 0.0000 ,0.0000,
 3, 1, 1, 1, 0.0000, 0.0000 ,0.0000,
 4, 1, 1, 1, 0.0000, 0.0000 ,0.0000,
 5, 1, 1, 1, 0.0000, 0.0000 ,0.0000
};

/* Text bundles */
static struct textbundle PStext_bundle[NTEXTB] =
{
 1, STROKE, 1.0, 0.0 , 1, 0.0000, 0.0000 ,0.0000,
 2, STROKE, 1.0, 0.0 , 1, 0.0000, 0.0000 ,0.0000,
 3, STROKE, 1.0, 0.0 , 1, 0.0000, 0.0000 ,0.0000,
 4, STROKE, 1.0, 0.0 , 1, 0.0000, 0.0000 ,0.0000,
 5, STROKE, 1.0, 0.0 , 1, 0.0000, 0.0000 ,0.0000
};

#define NDEVS  10
static struct PSdevtable PSdev[NDEVS] =
{
 300, 15, 21, 580, 826,  /*  1 - Tektronix Phaser A4  (Dye-sub) */
 300, 15, 95, 581, 782,  /*  2 - QMS colorscript 100 */
 300, 15, 21, 826, 1175, /*  3 - Tektronix Phaser A3 */
 300, 19, 23, 572, 816,  /*  4 - Normal A4 (biggest common margin) */
 300, 14, 95, 582, 816,  /*  5 - QMS colorscript 100 - extended A4 */
 300, 19, 19, 578, 824,  /*  6 - dl1  - HP Laserjet IIID */
 300, 18, 23, 572, 816,  /*  7 - al1  - Apple Laserwriter */
 300, 14, 15, 581, 828,  /*  8 - Tektronix Phaser A4  (Dye-sub) */
 600,  6,  7, 590, 832,  /*  9 - Dec LP20 - A4  */
 600,  6,  7, 835, 1182  /* 10 - Dec LP20 - A3  */
};
static int PSdevice = 3;  /* default device */

/* default PostScript colour table */
static Colourentry *PScolour_table;
extern const RGBcolour GKScol[];

/* PSdftcoltab should contain COLTAB_N entries of r g b triples */
static Colourentry PSdftcoltab[COLTAB_N] =
{
    1.0, 1.0 ,1.0 ,
    0.0000, 0.0000 ,0.0000,
    0.0161, 0.0161, 0.0161,
    0.0323, 0.0323, 0.0323,
    0.0484, 0.0484, 0.0484,
    0.0645, 0.0645, 0.0645,
    0.0806, 0.0806, 0.0806,
    0.0968, 0.0968, 0.0968,
    0.1129 ,0.1129, 0.1129,
    0.1290, 0.1290, 0.1290,
    0.1452, 0.1452, 0.1452,
    0.1613, 0.1613, 0.1613,
    0.1774, 0.1774, 0.1774,
    0.1935, 0.1935, 0.1935,
    0.2097, 0.2097, 0.2097,
    0.2258, 0.2258, 0.2258,
    0.2419, 0.2419, 0.2419,
    0.2581, 0.2581, 0.2581,
    0.2742, 0.2742, 0.2742,
    0.2903 ,0.2903, 0.2903,
    0.3065, 0.3065, 0.3065,
    0.3226, 0.3226 ,0.3226,
    0.3387, 0.3387, 0.3387,
    0.3548, 0.3548, 0.3548,
    0.3710, 0.3710, 0.3710,
    0.3871, 0.3871, 0.3871,
    0.4032, 0.4032, 0.4032,
    0.4194, 0.4194, 0.4194,
    0.4355, 0.4355, 0.4355,
    0.4516, 0.4516, 0.4516,
    0.4677, 0.4677, 0.4677,
    0.4839, 0.4839, 0.4839,
    0.5000, 0.5000, 0.5000,
    0.5161, 0.5161, 0.5161,
    0.5323, 0.5323, 0.5323,
    0.5484, 0.5484, 0.5484,
    0.5645, 0.5645, 0.5645,
    0.5806, 0.5806, 0.5806,
    0.5968, 0.5968, 0.5968,
    0.6129 ,0.6129, 0.6129,
    0.6290, 0.6290, 0.6290,
    0.6452, 0.6452, 0.6452,
    0.6613, 0.6613, 0.6613,
    0.6774, 0.6774, 0.6774,
    0.6935, 0.6935, 0.6935,
    0.7097, 0.7097, 0.7097,
    0.7258, 0.7258, 0.7258,
    0.7419, 0.7419, 0.7419,
    0.7581, 0.7581, 0.7581,
    0.7742, 0.7742, 0.7742,
    0.7903, 0.7903, 0.7903,
    0.8065, 0.8065, 0.8065,
    0.8226, 0.8226, 0.8226,
    0.8387, 0.8387, 0.8387,
    0.8548, 0.8548, 0.8548,
    0.8710, 0.8710, 0.8710,
    0.8871, 0.8871, 0.8871,
    0.9032, 0.9032, 0.9032,
    0.9194, 0.9194, 0.9194,
    0.9355, 0.9355, 0.9355,
    0.9516, 0.9516, 0.9516,
    0.9677, 0.9677, 0.9677,
    0.9839, 0.9839, 0.9839,
    1.0000, 1.0000, 1.0000,
};

/*  External variables used by the driver */

extern char *cgmofile;                  /* The name of the output file */

/* Variables seen by all modules in cgmops.c */
static struct lineatt PSlineatt;
static struct markeratt PSmarkatt;
static struct textatt PStextatt;
static struct fillatt PSfillatt;
static struct edgeatt PSedgeatt;
static struct sharedatt share;

/* Define return values from PSarcref */
#define ARC_OK       (Enum) 0
#define ARC_LINE    (Enum) 1
#define ARC_SLINE   (Enum) 2
#define ARC_DOT     (Enum) 3

#define INDPATTERN  -1.0

#define PORTRAIT 1
#define LANDSCAPE 0

#define COLOUR 1
#define MONOCHROME 0
#define EPSF 1
#define NORMALPS 0
#define VISIBLE 1
#define INVISIBLE 0
#define WHOLE 0
#define DECOMPOSE 1

/* Maximum numbers for polygons drawn whole, if poly_mode == DECOMPOSE */
#define NTZ    40   /* Max num of trapezoids for one path */
#define MAXARB 1    /* Max num of vertices for arbitrary polygon(set)*/
#define MAXSIM 1    /* Max num of vertices for simple polygon */

/* End of Postscript include file */

