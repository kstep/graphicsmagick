/*  RAL-CGM Interpreter header file: @(#) cgmigl.h  version 3.2
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
 * Description: Local variables for Iris Graphics Library
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
     6 Dec 91 RTP  Change logic for Iris 4D
 */

/*    Iris Graphics Library include files */

#ifndef CGMIGL_H
#define CGMIGL_H

/*  Include the files on which this is dependent.  */

#include "cgmmach.h"
#include "cgmtypes.h"
#include "cgmstruc.h"
#include "cgmatt.h"
#include "cgmtext.h"

/*  Define some constants */

#define WAIT     (Logical) TRUE
#define NOWAIT   (Logical) FALSE

/*  Screen extent in Pixels  */
static Scoord sc_left = 0, sc_right = YMAXSCREEN,
              sc_bot = 0, sc_top = YMAXSCREEN;

/*  default World Co-ordinates  */
static Coord wc_left = 0.0, wc_right = 1.0,
             wc_bot = 0.0, wc_top = 1.0;

/*  Aspect ratio  */
static float asp_rat;

/*  World Coordinate to Pixel ratio for GDPs etc */

static float iglwvrat;

/* Local flags for options */

static Logical iglmovie = DEFMOVIE,
               iglfullscrn = DEFFULLSCRN,
               igldouble = DEFDOUBLE,
               iglvirtualrgb = DEFVIRTUALRGB,
               igldebug = FALSE;
#ifdef CMS
Logical cgmwindow = FALSE;
#endif

/*  Text methods available */

static Enum text_method[] = {
#ifdef HERSHEY
              HER,
#endif
#ifdef BEZIER
              BEZ,
#endif
              HDW,
              (Enum) 0 };

static Matrix tmat = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};

/*  Flag to mark if graphics system is open */

static Logical iglopen = FALSE;

/*  Picture number for window manager */

static Long iglpic = ZERO;

/*  Flag to show if in Double Buffer mode  */


/*  Fixed array for local points list */

static Point iglpts[ARRAY_MAX];

/*  Current Clip rectangle in screen co-ordinates  */
static Scoord cl_left = 0, cl_right = YMAXSCREEN,
              cl_bot = 0, cl_top = YMAXSCREEN;

/*  Minimum and maximum colours changed */
static Index mincol, maxcol, ncolours = COLOUR_TAB;

/*   Colour Tables */
extern RGBcolour GKScol[];

static struct {
   short flag, r, g, b;
} defcolor[COLOUR_TAB], colors[COLOUR_TAB];

/*  Attribute change flags */
static Logical att_flag = FALSE;
static Logical new_linewidth, new_markersize,
               new_edgewidth, new_charheight;

/* CGM sttribute structures */

static Sharedatt iglatt;
static Lineatt iglline;
static Markeratt iglmark;
static Textatt igltext;
static Fillatt iglfill;
static Edgeatt igledge;

/*   GL Line drawing Functions  */

#if defined(IRIS4D) || defined(INDIGO)

#define BEGINLINE     bgnline()
#define BEGINPOLYGON  bgnpolygon()
#define MOVE(x, y)    point[0]=x; point[1]=y; v2f(point)
#define DRAW(x, y)    point[0]=x; point[1]=y; v2f(point)
#define ENDLINE       endline()
#define ENDPOLYGON    endpolygon()
#else
#define BEGINLINE
#define BEGINPOLYGON
#define MOVE(x, y)   move2( (Float)(x), (Float)(y))
#define DRAW(x, y)   draw2( (Float)(x), (Float)(y))
#define ENDLINE
#define ENDPOLYGON
#define concave(x)
#endif

/*  Iris local attributes  */

static struct {
   Linestyle linestyle;
   short linewidth, marktype, fillstyle, texture;
   Float markersize;
   short font, prec;
   float expansion, space;
   Colorindex colr;
   short red, green, blue;
} igl, current, igldefault = {
      0, 1, 3, 0, 0,
      1,
      0, 0,
      1.0, 1.0,
      1,
      255, 255, 255
  };

/*  Cell array Parameters  */
static struct {
   Point p, q, r, d, dd;
   unsigned short nx, ny, px, py;
} cell;

/*  Preset Markers  */
static Object marker, markers[5];
static float markscale;

/*  Text Characteristics  */

static struct {
   Point pos;
   float height, width;
   float bottom, base, half, cap, top;
   float left, centre, right;
} igltxt;

/*  Preset Bundle Tables  */

#define NLINEB  5
static Linebundle line_bundle[NLINEB] = {
      1, 1.0, 1, 1.0, 1.0, 1.0,
      2, 1.0, 2, 1.0, 0.0, 0.0,
      3, 1.0, 3, 0.0, 1.0, 0.0,
      4, 1.0, 4, 0.0, 0.0, 1.0,
      5, 1.0, 5, 1.0, 1.0, 0.0
  };

#define NMARKB  5
static Markerbundle mark_bundle[NMARKB] = {
      1, 1.0, 1, 1.0, 1.0, 1.0,
      2, 1.0, 2, 1.0, 0.0, 0.0,
      3, 1.0, 3, 0.0, 1.0, 0.0,
      4, 1.0, 4, 0.0, 0.0, 1.0,
      5, 1.0, 5, 1.0, 1.0, 0.0
  };

#define NTEXTB  4
static Textbundle text_bundle[NTEXTB] = {
      1, STROKE, 1.0, 0.0, 1, 1.0, 1.0, 1.0,
      2, STROKE, 1.0, 0.0, 2, 1.0, 0.0, 0.0,
      3, STROKE, 1.0, 0.0, 3, 0.0, 1.0, 0.0,
      4, STROKE, 1.0, 0.0, 4, 0.0, 0.0, 1.0
  };

#define NFILLB  4
static Fillbundle fill_bundle[NFILLB] = {
      1, 1, 1, 1, 1.0, 1.0, 1.0,
      1, 2, 2, 2, 1.0, 0.0, 0.0,
      1, 3, 3, 3, 0.0, 1.0, 0.0,
      1, 4, 4, 4, 0.0, 0.0, 1.0
   };

#define NEDGEB  5
static Edgebundle edge_bundle[NEDGEB] = {
      1, 1.0, 1, 1.0, 1.0, 1.0,
      2, 1.0, 2, 1.0, 0.0, 0.0,
      3, 1.0, 3, 0.0, 1.0, 0.0,
      4, 1.0, 4, 0.0, 0.0, 1.0,
      5, 1.0, 5, 1.0, 1.0, 0.0
  };

#define HATCHES 15

/*  Map RAL-GKS Hatches to SGI patterns  */

static short GKShatch[10] = { 1, 2, 3, 5, 4, 6, 7, 8, 9, 10};

static unsigned short hatch[HATCHES][PATTERN_16] = {
/*  Horizontal stripes  */
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
     0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000},
/*  Vertical stripes   */
    {0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888,
     0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888},
/*  Positive slant  */
    {0x4444, 0x2222, 0x1111, 0x8888, 0x4444, 0x2222, 0x1111, 0x8888,
     0x4444, 0x2222, 0x1111, 0x8888, 0x4444, 0x2222, 0x1111, 0x8888},
/*  Negative slant  */
    {0x1111, 0x2222, 0x4444, 0x8888, 0x1111, 0x2222, 0x4444, 0x8888,
     0x1111, 0x2222, 0x4444, 0x8888, 0x1111, 0x2222, 0x4444, 0x8888},
/*  Horizontal crosshatch  */
    {0x8888, 0x8888, 0xFFFF, 0x8888, 0x8888, 0x8888, 0xFFFF, 0x8888,
     0x8888, 0x8888, 0xFFFF, 0x8888, 0x8888, 0x8888, 0xFFFF, 0x8888},
/*  Diagonal crosshatch  */
    {0x5555, 0x2222, 0x5555, 0x8888, 0x5555, 0x2222, 0x5555, 0x8888,
     0x5555, 0x2222, 0x5555, 0x8888, 0x5555, 0x2222, 0x5555, 0x8888},
/*  GKS type -7  */
    {0xFFFF, 0x4141, 0x2222, 0x1414, 0xFFFF, 0x1414, 0x2222, 0x4141,
     0xFFFF, 0x4141, 0x2222, 0x1414, 0xFFFF, 0x1414, 0x2222, 0x4141},
/*  GKS type -8  */
    {0xFFFF, 0x9090, 0x9090, 0xFFFF, 0x9090, 0x9090, 0x9090, 0x9090,
     0xFFFF, 0x9090, 0x9090, 0xFFFF, 0x9090, 0x9090, 0x9090, 0x9090},
/*  GKS type -9  */
    {0x6969, 0x6666, 0x9696, 0x0909, 0x9696, 0x6666, 0x6969, 0x9090,
     0x6969, 0x6666, 0x9696, 0x0909, 0x9696, 0x6666, 0x6969, 0x9090},
/*  GKS type -10 */
    {0xFFFF, 0x5050, 0x2020, 0x5050, 0xFFFF, 0x0505, 0x0202, 0x0505,
     0xFFFF, 0x5050, 0x2020, 0x5050, 0xFFFF, 0x0505, 0x0202, 0x0505},
/*  noughts and crossses  */
    {0x1C41, 0x2222, 0x4114, 0x4108, 0x4114, 0x2222, 0x1C41, 0x0000,
     0x411C, 0x2222, 0x1441, 0x0841, 0x1441, 0x2222, 0x411C, 0x0000},
/*  noughts  */
    {0x1C1C, 0x2222, 0x4141, 0x4141, 0x4141, 0x2222, 0x1C1C, 0x0000,
     0x1C1C, 0x2222, 0x4141, 0x4141, 0x4141, 0x2222, 0x1C1C, 0x0000},
/*  squares  */
    {0x7F7F, 0x4141, 0x5D5D, 0x5555, 0x5D5D, 0x4141, 0x7F75, 0x0000,
     0x7F7F, 0x4141, 0x5D5D, 0x5555, 0x5D5D, 0x4141, 0x7F75, 0x0000},
/*  Chequer board */
    {0x3333, 0x3333, 0xCCCC, 0xCCCC, 0x3333, 0x3333, 0xCCCC, 0xCCCC,
     0x3333, 0x3333, 0xCCCC, 0xCCCC, 0x3333, 0x3333, 0xCCCC, 0xCCCC},
/*  large circles  */
    {0x001C, 0x002B, 0x004A, 0x007F, 0x004A, 0x002B, 0x001C, 0x0000,
     0x1C00, 0x2B00, 0x4A00, 0x7F00, 0x4A00, 0x2B00, 0x1C00, 0x0000}
};


/*  Pattern Parameters  */

static short maxpat = 2048,    /* Current top of Pattern Table */
             patprec = 8;      /* Current pattern precision (bits) */
static Pattern16 patmask[8];   /* Current pattern masks */
static Index patind = HATCHES; /* Base Iris pattern for patterns */

typedef short pattab[PATT_TAB];

/*  Define 5 Preset Patterns (all 16 x 16) */

static pattab patts = {
/*  Pattern 1  */
    1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* Pattern 2 */
    1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* Pattern 3 */
    1, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 1, 3, 3, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 1, 3, 3, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 1, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 1, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* Pattern 4 */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 4, 1, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 4, 1, 4, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 4, 1, 4, 4,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 4, 1, 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 4, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 4, 1, 1, 4, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 4, 1, 4, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*  Pattern 5  */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
    8, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8,
    8, 7, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 3, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 3, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8,
    8, 7, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8,
    8, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8,
    8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
/*  Pattern 6  */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 0, 0, 0, 0, 5, 5, 3, 5, 5, 5, 0, 0, 0, 0, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 5, 5, 3, 5, 5, 5, 6, 6, 6, 6, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  Pattern 7 */
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0,
    0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0,
    0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*  Pattern 8  */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0,
    0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
    0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
    0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0,
    0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0,
    0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0,
    0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0,
    0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0,
    0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0,
    0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0,
    0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0,
    0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0,
    0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static struct {
   Index row, col;
   Prec prec;
   short *ptab;
} iglpat[PATTERNS] = {
   16, 16, 4, &patts[0],      /*  Pattern 1   */
   16, 16, 4, &patts[256],    /*  Pattern 2   */
   16, 16, 4, &patts[512],    /*  Pattern 3   */
   16, 16, 4, &patts[768],    /*  Pattern 4   */
   16, 16, 4, &patts[1024],   /*  Pattern 5   */
   16, 16, 4, &patts[1280],   /*  Pattern 6   */
   16, 16, 4, &patts[1536],   /*  Pattern 7   */
   16, 16, 4, &patts[1792]    /*  Pattern 8   */
};

#endif  /* end of cgmigl.h */
