/*  RAL-CGM Interpreter header file: @(#) cgmvga.h  version 3.1
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
 * Description:
 *
 * Modification log:
 *
 *  21 Aug 91 RTP  Initial version stabilised
 */

#include <graph.h>   /* Microsoft C graphics library */

#include "cgmatt.h"

struct clip
       {
         Point a,b;
       };
static struct clip  cl, vgaclip;
static struct videoconfig config;

/* Var for Viewport and Window */
static Float A, B, C, D;
static short Px1, Px2, Py1, Py2;
static Float Wx1, Wx2, Wy1, Wy2;
static Float wvrat; /* window ratio */

/*  Attribute change flags */
static Logical att_flag = FALSE;
static Logical new_linewidth, new_markersize,
          new_edgewidth, new_charheight;

/* CGM attribute structures */

static Sharedatt vgaatt;
static Lineatt vgaline;
static Markeratt vgamark;
static Textatt vgatext;
static Fillatt vgafill;
static Edgeatt vgaedge;
static Lpoint localpts[ARRAY_MAX];
/* available color */
static int nc, ncol=1000;

/*  High or Low Resolution */
#define HIGH   1
#define LOW    0
static Enum vgares = HIGH;

#define VGAREDMIN     0    /* VGA red   color min value */
#define VGAREDMAX     0X1F   /* VGA red   color max value */
#define VGABLUEMIN    0    /* VGA blue  color max value */
#define VGABLUEMAX    0X1F   /* VGA blue  color min value */
#define VGAGREENMIN   0    /* VGA green color max value */
#define VGAGREENMAX   0X1F   /* VGA green color min value */

static Float Ar,Ab,Ag,Br,Bb,Bg;


#define NLINEB  5
#define NMARKB  5
#define NTEXTB  4
#define NFILLB  4
#define NEDGEB  5
#define COLTBSIZE 300

static size_t coltbsize = COLTBSIZE;

typedef struct Rgbcolor{
           unsigned short int r,g,b;
                  }rgbcolor;

static rgbcolor *vgacolrtb = NULL; /* Local color table */

/* default color table */
rgbcolor defcolrtb[COLTBSIZE]={

/*   GKS  default colours */

    0,   0,   0,   /* Background */
  255, 255, 255,   /* Foreground */
  255,   0,   0,   /* Red */
    0, 255,   0,   /* Green */
    0,   0, 255,   /* Blue */
  255, 255,   0,   /* Yellow */
    0, 255, 255,   /* Cyan */
  255,   0, 255,   /* Magenta */

 /*    8 Grey scales ( White to Black )  */

    255, 255, 255,  177, 177, 177,  158, 158, 158,  128, 128, 128,
    100, 100, 100,   78,  78,  78,   64,  64,  64,    0,   0,   0,

 /*    GKS colour maps from Versatec  16 - 255 */

   98, 192, 255,  139, 122, 255,    92,  70, 255,    51,   0, 243,
   28,   0, 133,   20,   0,  93,    14,   0,  65,    11,   0,  56,
    9,   0,  46,  128, 138, 255,    85,  85, 255,     0,   0, 255,
    0,   0, 168,    0,  13, 116,     0,   8,  76,     0,   8,  68,

  122, 139, 255,   80,  93, 255,     0,  59, 252,     0,  39, 168,
    0,  28, 122,    0,  18,  80,     0,  16,  70,   122, 149, 255,
   70, 108, 255,    0,  83, 230,     0,  61, 175,     0,  44, 128,
    0,  29,  85,    0,  26,  74,   116, 154, 255,    59, 118, 255,

    0,  97, 219,    0,  77, 182,     0,  56, 133,     0,  37,  89,
    0,  33,  79,  110, 159, 255,    43, 131, 255,     0, 112, 225,
    0,  92, 186,    0,  68, 138,     0,  46,  93,     0,  40,  80,
  177, 213, 255,  104, 173, 255,     0, 151, 250,     0, 136, 225,

    0, 120, 198,    0,  89, 147,     0,  59,  97,     0,  52,  86,
    0,  43,  70,    0, 186, 186,     0, 160, 147,     0, 142, 129,
    0, 121, 106,    0,  89,  78,     0,  62,  55,     0,  53,  46,
    0, 192, 160,    0, 160, 123,     0, 138, 105,     0, 119,  91,

    0,  89,  68,    0,  59,  44,     0,  53,  40,     0, 243, 169,
    0, 192, 134,    0, 160, 113,     0, 138,  97,     0, 118,  83,
    0,  89,  63,    0,  59,  41,     0,  51,  36,     0,  43,  30,
    0, 216, 115,    0, 175, 113,     0, 151, 102,     0, 138,  91,

    0, 104,  68,    0,  70,  46,     0,  61,  40,     0, 230,  74,
    0, 189, 110,    0, 171, 100,     0, 156,  92,     0, 116,  69,
    0,  80,  47,    0,  56,  33,   105, 238,   0,     0, 216,  46,
    0, 202,  65,    0, 189,  71,     0, 142,  53,     0,  97,  36,

    0,  83,  31,  161, 240,   0,    99, 225,   0,   120, 219,   0,
   92, 208,   0,   69, 156,   0,    46, 104,   0,    41,  93,   0,
  197, 243,   0,  172, 232,   0,   168, 227,   0,   160, 219,   0,
  122, 168,   0,   80, 110,   0,    70,  97,   0,   255, 251, 175,

  255, 250, 110,  255, 249,  70,   254, 248,   0,   253, 245,   0,
  189, 183,   0,  128, 124,   0,   113, 110,   0,    89,  86,   0,
  255, 209, 116,  255, 194,  97,   255, 191,  85,   255, 177,  65,
  195, 136,   0,  133,  92,   0,   116,  81,   0,   255, 190, 119,

  255, 163, 104,  255, 168,  89,   255, 160,  80,   195, 113,   0,
  133,  77,   0,  116,  67,   0,   255, 173, 122,   255, 154, 107,
  255, 142,  89,  255, 112,   0,   195,  86,   0,   128,  56,   0,
  113,  50,   0,  255, 162, 122,   255, 125, 104,   255, 100,  65,

  247,  80,   0,  186,  60,   0,   122,  39,   0,   110,  35,   0,
  255, 147, 124,  255,  98,  89,   255,  56,  43,   238,  44,   0,
  182,  34,   0,  122,  22,   0,   104,  19,   0,   255, 168, 170,
  255, 122, 128,  255,  70,  80,   250,   0,  38,   225,   0,  35,

  168,   0,  26,  110,   0,  17,   100,   0,  15,    80,   0,  12,
  255, 122, 147,  255,  70, 108,   247,   0,  83,   225,   0,  72,
  168,   0,  54,  110,   0,  35,    97,   0,  31,   255, 166, 197,
  255, 110, 170,  255,  43, 150,   232,   0, 134,   211,   0, 122,

  160,   0,  92,  104,   0,  60,    93,   0,  54,    76,   0,  44,
  255,  80, 204,  227,   0, 189,   199,   0, 165,   168,   0, 142,
  122,   0, 104,   85,   0,  72,    74,   0,  63,   255,  70, 225,
  205,   0, 213,  175,   0, 175,   141,   0, 147,   106,   0, 110,

   71,   0,  74,   62,   0,  65,   245,  43, 255,   193,   0, 227,
  153,   0, 195,  107,   0, 147,    80,   0, 110,    55,   0,  76,
   47,   0,  65,  216,  89, 255,   150,  43, 255,   118,   0, 216,
   80,   0, 147,   59,   0, 107,    38,   0,  70,    35,   0,  64,

  187, 110, 255,  122,  59, 255,    86,   0, 230,    55,   0, 147,
   38,   0, 100,   25,   0,  68,    22,   0,  60,   177, 177, 177,
  158, 158, 158,  128, 128, 128,   100, 100, 100,    78,  78,  78,
    0,   0,   0,   61,  50,  59,    52,  45,  62,    36,  61,  59,

  127, 118, 109,   91,  70,  72,    71,  57,  64,    79,  62,  64,
   28,  33,  32,   34,  31,  36,    31,   0,  22,   252, 182, 142,
  223, 145, 135,  191, 133, 119,   155, 100,  94,   137,  94,  90,
   64,  51,  53,   48,  41,  42,   255, 148, 116,   151,  96,  91
};


Point txtpos;  /* Display text position */

/* Text method available */

static Enum text_method[] = {
#ifdef BEZIER
           BEZ,
#endif
#ifdef HERSHEY
           HER,
#endif
           HDW,
           (Enum) 0 };

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

/* line style */
unsigned short linestyle[16] = {
      0x0000, /* invisable */
      0xffff, /* solid */
      0x0ff0, /* dash  */
      0x1000, /* dot */
      0xf080, /* dash-dot */
      0x3fc9, /* dash-dot-dot */
      0x1ff0, 0x3ff0, 0x7ff0, 0xfff0,
      0x1fff, 0x3fff, 0x7fff, 0xffff,
      0xffff, 0xffff
  };

unsigned short edgestyle[16] = {
      0x0000, /* invisable */
      0xffff, /* solid */
      0x0ff0, /* dash  */
      0x1000, /* dot */
      0xf080, /* dash-dot */
      0x3fc9, /* dash-dot-dot */
      0x1ff0, 0x3ff0, 0x7ff0, 0xfff0,
      0x1fff, 0x3fff, 0x7fff, 0xffff,
      0xffff, 0xffff
  };

#define HATCHES 11

static unsigned char hatch[HATCHES][8] = {
/*  Horizontal stripes  */
    {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
/*  Vertical stripes   */
    {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88},
/*  Positive slant  */
    {0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11},
/*  Negative slant  */
    {0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88},
/*  Horizontal crosshatch  */
    {0x88, 0x88, 0x88, 0xFF, 0x88, 0x88, 0x88, 0xFF},
/*  Diagonal crosshatch  */
    {0x55, 0x22, 0x55, 0x88, 0x55, 0x22, 0x55, 0x88},
/*  GKS type -7  */
    {0xFF, 0x41, 0x22, 0x14, 0xFF, 0x14, 0x22, 0x41},
/*  GKS type -8  */
    {0xFF, 0x90, 0x90, 0xFF, 0x90, 0x90, 0x90, 0x90},
/*  GKS type -10 */
    {0xFF, 0x50, 0x20, 0x50, 0xFF, 0x05, 0x02, 0x05},
/*  squares  */
    {0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00},
/*  Chequer board */
    {0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC},
};

/* Pattern data */
/*************************/


/************************/
