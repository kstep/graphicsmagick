/*   RAL-CGM Interpreter header file:  @(#) cgmubez.h  version 3.1
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the authors.
 *
 * It is not in the public domain.
 *
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the authors should be marked as
 * such.
 *
 * Description: Definitions of all variables in the Bezier text
 *              system used by the utility.
 *
 * Modification log:
 *
 *  11 Sep 91 CDS  Split of the utility stuff from old CGMBEZ.H
 *  02 Oct 91 CDS  Changed use of int to Index in type BezDir & BezFont
 *  16 Nov 92 CDS  Changed ifdef ANSI to ifdef PROTO in function prototypes
 *
 */

     /*  The following #define's may be system-dependent  */

#define MaxCoef      8         /*  number of coefficients in B_coef  */
#define MaxComp    100         /*  number of Bezier points in 1 component  */
#define MaxExp     460         /*  number of exp. coordinates in 1 component  */
#define MaxFonts    10         /*  maximum number of fonts  */
#define MaxChars    96         /*  maximum number of characters in a font  */
#define MaxDefs   3800         /*  maximum number of points in font  */
#define MaxResFonts  3         /*  maximum number of fonts resident in mem  */
#define DefBezFnt    200       /*  Default bezier font if we cant find the
                                   required one, should never happen!!!     */

typedef enum
{
     BezCLOSED,
     BezOPEN
} BezSTATE;

typedef struct bezdir
{
    Index number;                   /*  reference number of font  */

    char name[MAXFNSIZE],           /*  name of font  */
         filename[MAXFLSIZE];       /*  name of file holding font  */
} BezDir;

typedef struct bezfont
{
     Index number;                  /*  reference number of font  */

     int charids,                   /*  number of character definitions  */
         defs;                      /*  number of point sets  */

     float top,                     /*  top line y value  */
           cap,                     /*  cap line y value  */
           bottom,                  /*  bottom line y value  */
           width,                   /*  average width ?  */
           shear,                   /*  shear parameter  */
           height;                  /*  derived height parameter  */
} BezFont;

typedef struct bezchar
{
    int location,                   /*  start point in character definition  */
        numcomp;                    /*  number of components in character  */

    float centre,                   /*  position of centre  */
          width;                    /*  character width  */
} BezChar;

typedef struct bezdef
{
    int flag;                       /*  curve / straight flag  */

    float x,y;                      /*  point  */
} BezDef;

typedef struct bezflist
{
    BezFont *bfont;                 /*  font details for this font  */
    BezChar *bchar;                 /*  character details for this font  */
    BezDef *bdef;                   /*  character definitions for this font  */

    struct bezflist *next;
} BezFlist;


static BezChar   *B_char=NULL;             /*  descriptors of current chars  */

static BezDef    *B_def=NULL;              /*  Bezier definitions of ditto  */

static char *bezext = "bez",
            *srcext = "bfs",
            *binext = "bfb";

#ifndef GENBEZ_C

static BezSTATE  B_state = BezCLOSED;      /*  Bezier system state  */

static int       B_coef[MaxCoef][MaxCoef]; /*  Pascal's triangle coeffs   */

static int       B_numfonts = 0;           /*  number of fonts in  memory */

static BezDir    *B_dir=NULL;              /*  descriptions of all fonts  */

static BezFont   *B_font=NULL;             /*  descriptor for current font  */

static BezFlist  *B_fontlist=NULL;         /*  list of fonts in memory  */
     /*
      |  Include the user include file to get the definitions
      |  of the user callable routines
      */

#include "cgmbez.h"

     /*
      |  The following routines are internal, so do not need to have
      |  their names modified to the BEZxxx notation.
      */

#ifdef PROTO

static void BezOpen( void ),
            BezArc( int, int, BezDef *, Point* ),
            BezDraw( int, Tmatrix, struct textatt*,
                     void (*)( Int, Point*, Tmatrix, int ));

static int BezLoad( Index );

static double BezNest( int, BezDef*, struct textatt* );

#else

static void BezOpen(),        /*  Open up the system, load first font  */
            BezArc(),         /*  Derive a Bezier curve from control points  */
            BezDraw();          /*  Expand a component to curves  */

static int BezLoad();         /*  Load a new font  */

static double BezNest();      /*  Calculate nominal point allowance  */

#endif

#endif
