/*   RAL-CGM Interpreter header file:  @(#) cgmuher.h  version 3.4
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
 * Description: Definitions of all shared variables in the Hershey text
 *              system
 *
 *  Maintenance log:
 *
 *  11 Sep 91 CDS  Split off this utility stuff from cgmher.h
 *  02 Oct 91 CDS  Changed use of int to Index in type HerDir & HerFont
 *  17 Oct 91 RTP  Change ANSI definition for HerDraw
 *  16 Nov 92 CDS  Changed ifdef ANSI to ifdef PROTO in function prototypes
 *  16 Jun 95 RTP  Add 'huge' to Herdef for PC version
 *
 */


     /*  The following #define's may be system-dependent  */

#define MaxExp      50     /*  number of coordinates in 1 character */
#define MaxFonts    25     /*  maximum number of fonts */
#define MaxDefs   5000     /*  maximum number of points in font */
#define MaxChars    95     /*  maximum number of characters in a font */
#define MaxResFonts  3     /*  maximum number of fonts resident in mem */
#define DefHerFnt  101     /*  Default Hershey font if we cant find the
                               required one, should never happen!!!  */

typedef enum
{
    HerCLOSED,
    HerOPEN
} HerSTATE;

typedef struct herdir
{
    Index number;                     /*  reference number of font  */

    char name[MAXFNSIZE],           /*  name of the font  */
         filename[MAXFLSIZE];       /*  name of file holding font  */
} HerDir;

typedef struct herfont
{
    Index number;                   /*  reference number of font  */

    int charids,                    /*  number of character definitions  */
        defs;                       /*  no. of points in font def */

    float top,                      /*  top line y value  */
          cap,                      /*  cap line y value  */
          bottom,                   /*  bottom line y value  */
          width,                    /*  average width ?  */
          height;                   /*  derived height parameter  */
} HerFont;

typedef struct herchar
{
    int location,                   /*  start point in character definition  */
        numcomp;                    /*  number of components in character  */

  SignChar centre;                  /*  position of centre  */

  unsigned char width;              /*  character width  */
} HerChar;


typedef struct herdef
{
    huge SignChar x,y;                   /*  x and y coordinates of a point  */
} HerDef;


typedef struct herflist
{
    HerFont *hfont;                 /*  font details for this font  */
    HerChar *hchar;                 /*  character details for this font  */
    HerDef *hdef;                   /*  character definitions for this font  */

    struct herflist *next;

} HerFlist;


static HerChar  *H_char=NULL;              /*  descriptors of current chars  */

static char *herext = "her",
            *srcext = "hfs",
            *binext = "hfb";

#ifndef GENHER_C

static HerSTATE  H_state = HerCLOSED;      /*  Hershey system state  */

static int       H_numfonts = 0;           /*  number of fonts read from directory  */

static HerDir   *H_dir=NULL;               /*  descriptions of all fonts  */

static HerFont  *H_font=NULL;              /*  descriptor for current font  */

static HerFlist *H_fontlist=NULL;          /*  list of fonts in memory  */

static HerDef   *H_def=NULL;               /*  Hershey definitions of ditto  */

     /*
      |  Include user include for user callable function definitions
      */

#include "cgmher.h"

     /*
      |  The following routines are internal, so do not need to have
      |  their names modified to the HERxxx notation.
      */

#ifdef PROTO

static void HerOpen( void ),
            HerDraw( int, Tmatrix,
                   void (*)(int, Point *, Tmatrix, int));

static int  HerLoad( Index );

#else

static void HerOpen(),      /*  Open up the system, load first font  */
            HerDraw();

static int  HerLoad();      /*  Load a new font  */

#endif

#endif
