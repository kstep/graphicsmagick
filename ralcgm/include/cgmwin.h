/*  RAL-CGM Interpreter header file: @(#) cgmwin.h  version 3.6
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
 * Description: Local variables and constants for MS Windows driver
 *
 * Maintenance Log:
 *   10 Oct 94   KEVP  Add variables to allow viewing of multi-picture CGMs.
 *   12 Oct 94   KEVP  Revise Default Colour Table
 *   21 Oct 94   KEVP  Use purple instead of black in bundles of index 5.
  */
/*  Include all other files on which this is dependent.  */
#include "cgmtypes.h"
#include "cgmtext.h"
#include "cgmfile.h"
#include "cgmcolr.h"
#include "cgmcell.h"
#include "cgmpoly.h"
#include "cgmpts.h"
#include "cgmgdp.h"
#include "cgmtext.h"
/* #include "wcgm.h" */
#include "windowsx.h"
#include "cgmwinme.h"

/* Externals from the windows main module, which may contain the Window Procedure */
extern HWND hwndOut;      /* Handle to Window (if created before driver is called) */
extern BOOL bMetaFile;       /* True if outputting to Windows MetaFile */
extern HDC hdcOut;         /* Handle to Output Device Context */
extern int cxClient, cyClient;   /* Client Area of Window hwnd */
extern BOOL bNewSize;      /* True, if client area has changed size & new size is worth saving */
 /* The Client area is used for the default Windows Viewport */
extern HANDLE hCurrInstance;
extern LPSTR lpszWinFileName;

/* External from CGMerr */
extern cgmerrcount;   /* Limit for error count */

/* Circle/Ellipse Macro (should be included in file available to all output drivers) */
const short closoffs[] = {0,0,6,0,7,0,0,10};
#define CLOSURE(t)  (closoffs[(t)&0x7] >0?(Enum)(*(pi + closoffs[(t)&0x7])):NOCLOSE)

/*  Local macros */
#define WINCOL(x, col) ( (UINT)(255.999*(x - cur.min_rgb.col) \
                                / (cur.max_rgb.col - cur.min_rgb.col)) )
#define FOREGROUND RGB(0,0,0)
#define BACKGROUND RGB(255,255,255)
#define LWIDTH (UINT)(attline.type==1&&cur.linewidth_mode==ABS?vdc_unit*attline.width:0)
#define EWIDTH (UINT)(attedge.type==1&&cur.edgewidth_mode==ABS?vdc_unit*attedge.width:0)

/*  Local defines */
#define COORD_LIMIT 32767      /* Limit of Windows Logical Coordinates */
#define DRAW_LIMIT 10000      /* Limit of VDC extent in Logical Coordinates */
#define PTS_INTS  (Enum)1      /* WinPoints from integer VDCs */
#define PTS_REALS (Enum)2      /* WinPoints from real VDCs */
#define PTS_PTS (Enum)3      /* WinPoints from VDC Points */

/* Windows Points Store */
static POINT ptList[ARRAY_MAX];
static PPOINT ptAlloc;
static UINT nAlloc;
#define WINFREEPTS PTSfree(); if(nAlloc>0)FREE(ptAlloc); nAlloc = 0

/* VDC to Windows Logical Coordinates Transformation */
static int cxDisplay, cyDisplay;    /* Display Extent */
static float vdc_unit;            /* Size of one VDC unit in Windows logical coordinate units  */
static int sgnXdir, sgnYdir;         /* Directions of Coordinates */

/* Needed for Clipping */
static short xWindowOrg, yWindowOrg, cxWindowExt, cyWindowExt;
static WORD wMapMode;

/* Windows Objects (Pens etc) Variables */
static Code selected_pen, selected_brush;
#define UNKNOWN -1;
static HPEN hpenLine=NULL, hpenMarker=NULL, hpenText=NULL,
        hpenEdge=NULL, hpenHollow=NULL;
static Logical new_line_pen, new_marker_pen, new_text_pen,
                    new_edge_pen, new_hollow_pen;
static DWORD dwPenColor;
static HBRUSH hbrushFill=NULL, hbrushText=NULL, hbrushBack=NULL;
static Logical new_fill_brush, new_text_brush, new_back_brush=FALSE;

/* Background Colour Flags */
static Logical background_visible=TRUE,  background_needed,
    background_always_needed;

/* Pure colour option flag */
static Logical always_pure = FALSE;

/* Metric scaling allowed option flag */
static Logical always_abstract=TRUE;

/* Attribute Flags */
static Logical init_need;
static Logical att_flag=FALSE;
static Logical  new_BkMode, new_BkColor;
static Logical new_clip;

/* Drawing mode for WINdraw and WINtxdraw */
Enum draw_mode;
#define LINE_DRAW 1
#define FILL_DRAW 2

/*Nominal  Marker Size */
#define MKUNIT 0.02      /*  as Proportion of  VDC extent (shorter dimension) */
static Float nom_mark_size;   /*  in VDC units */

/* Array of Windows PenStyles [linetype-1] */
static WORD nPenStyles[] = {  PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT};

/* Array of Windows BrushStyles [filltype] */
static WORD nBrushStyles[] = { BS_HOLLOW, BS_SOLID, BS_PATTERN, BS_HATCHED, BS_HOLLOW };

/* Array of Windows HatchStyles [hatchtype] */
static WORD nHatchStyles[] = { HS_HORIZONTAL,HS_VERTICAL,HS_BDIAGONAL,HS_FDIAGONAL,
            HS_CROSS, HS_DIAGCROSS };
#define MAX_HATCH 6

/* Patterns*/
#define MAX_PATT 16
static Index patt_index[MAX_PATT];      /* Pattern Indices */
static HBITMAP hPattBitmap[MAX_PATT];   /* Bitmaps of Patterns */
static Int patt_defined=0;         /* Number of Defined Patterns */
static Int patt_fixed=0;            /* Number of Defined Default Patterns */
static Logical default_pattern = FALSE;      /* True, if making default pattern  */
/*NB:  Default patterns are defined algorithmically by WINpattin */

/* Default Colour Table (first 16 entries) */
#define NACOLORS 16
static COLORREF dwDefCols[NACOLORS] = {
   BACKGROUND,
   FOREGROUND,
   RGB(255,0,0),
   RGB(0,255,0),
   RGB(0,0,255),
   RGB(255,255,0),
   RGB(0,255,255),
   RGB(255,0,255),
   RGB(128,0,0),
   RGB(0,128,0),
   RGB(0,0,128),
   RGB(128,0,128),
   RGB(128,255,128),
   RGB(128,128,255),
   RGB(191,191,191),
   RGB(128,128,128) };
/* Entries 16 and beyond are repeated with a period of 16 by WinColor */



/*   Prefered Text methods */

static Enum prefmeth[]={
#ifdef HERSHEY
                          HER,
#endif
 #ifdef BEZIER
                          BEZ,
#endif
                         HDW,
                          (Enum)0};

/* Number of predefined bundles for each primitive */
#define NLINEB 5
#define NEDGEB 5
#define NMARKB 5
#define NFILLB 5
#define NTEXTB 5


/* Colour Data in Attribute Bundles */
#define BLACK   1, 0.0, 0.0, 0.0
#define RED     2, 1.0, 0.0, 0.0
#define GREEN   3, 0.0, 1.0, 0.0
#define BLUE    4, 0.0, 0.0, 1.0
#define PURPLE 11, 0.5, 0.0, 0.5
 /* NB: Colours 5(yellow), 6(cyan) and 7(magenta) are not used in bundles,
           because they could appear White on a Monochrome Display */

  /*Attribute Bundles*/
static struct linebundle line_bundle[NLINEB] =
{
 1, 1.0, BLACK,
 2, 1.0, RED,
 3, 1.0, GREEN,
 4, 1.0, BLUE,
 5, 1.0, PURPLE
};

/* Edge Control Bundles */
static struct edgebundle edge_bundle[NEDGEB] =
{
 1, 1.0, BLACK,
 2, 1.0, RED,
 3, 1.0, GREEN,
 4, 1.0, BLUE,
 5, 1.0, PURPLE
};

/* Polymarker bundles */
static struct markerbundle mark_bundle[NMARKB] =
{
 1, 1.0, BLACK,
 2, 1.0, RED,
 3, 1.0, GREEN,
 4, 1.0, BLUE,
 5, 1.0, PURPLE
};

/* Fill area bundles */
static struct fillbundle fill_bundle[NFILLB] =
{
 1, 1, 1, BLACK,
 1, 2, 2, RED,
 1, 3, 3, GREEN,
 1, 4, 4, BLUE,
 2, 1, 1, PURPLE
 };

/* Text bundles */
static struct textbundle text_bundle[NTEXTB] =
{
 1, STROKE, 1.0, 0.0 , BLACK,
 2, STROKE, 1.0, 0.0 , RED,
 3, STROKE, 1.0, 0.0 , GREEN,
 4, STROKE, 1.0, 0.0 , BLUE,
 5, STROKE, 1.0, 0.0 , PURPLE
};

/* Undefine colours used above */
#undef BLACK
#undef RED
#undef GREEN
#undef BLUE
#undef PURPLE

/* default colour table */
static Colourentry *WINcolour_table;
extern const RGBcolour GKScol[];


/* Attributes post-ASF */
static struct lineatt attline;
static struct markeratt attmark;
static struct textatt atttext;
static struct fillatt attfill;
static struct edgeatt attedge;
static struct sharedatt attshared;

/* Pointer to Points list from Utility */
Point *upts;

/* Text Variables */
Point txt_point, txt_corner;
Textitem* txt_item;
Logical restrtxt;

/* Error Message Variables */
static char *func = "CGMOwin", emess[48];

/* True if using CGMOwin Window */
static Logical win_use;

/*  Picture Vaiables*/
static Logical last_pic, replay=FALSE;
static Logical display_empty;
static Int num_pic;

/* Cursors */
static HCURSOR hcursorWait, hcursorArrow;

/* GDI Failure Message Box Flags */
static Logical warn_cell_fail = TRUE;
/* Logical Palette */
/* This is used only to increase the number of pure colours (when possible)
    The most important colours come first.
    The palette is only ever referred to indirectly, so can be modified without
     having to make any alterations to the driver.
    It is NOT used as a colour table.
*/
#define PALETTE_SIZE 75
 static const LOGPALETTE lpalette = {
0x300, 1, { 0x00, 0x00, 0x00, 0 } };
 static const PALETTEENTRY palData[] = {
0x00, 0x00, 0x00, 0,   /* black */
0xFF, 0xFF, 0xFF, 0,   /* white */
0xFF, 0x00, 0x00, 0,   /* red */
0x00, 0xFF, 0x00, 0,   /* green */
0x00, 0x00, 0xFF, 0,   /* blue */
0xFF, 0xFF, 0x00, 0,   /* yellow */
0x00, 0xFF, 0xFF, 0,   /* cyan */
0xFF, 0x00, 0xFF, 0,   /* magenta */
0x80, 0x80, 0x80, 0,   /* grey */
0xBF, 0xBF, 0xBF, 0,   /* light grey */
0x40, 0x40, 0x40, 0,   /* dark grey */
0xFF, 0x80, 0x00, 0,   /* orange */
0x80, 0x00, 0x80, 0,   /* purple (dark magenta) */
0x80, 0x00, 0x00, 0,   /* dark red */
0x00, 0x80, 0x00, 0,   /* dark green */
0x00, 0x00, 0x80, 0,   /* dark blue */
0xFF, 0x80, 0x80, 0,   /* pink (pale red) */
0x80, 0x80, 0xFF, 0,   /* pale blue */
0x80, 0xFF, 0x80, 0,   /* pale green */
0x9F, 0x9F, 0x9F, 0,   /* moderately pale grey */
0x60, 0x60, 0x60, 0,   /* moderately dark grey */
0xDF, 0xDF, 0xDF, 0,   /* very light grey */
0x20, 0x20, 0x20, 0,   /* very dark grey */
0xFF, 0x80, 0xFF, 0,   /* lilac (pale magenta) */
0xFF, 0xFF, 0x80, 0,   /* pale yellow */
0x80, 0xFF, 0xFF, 0,   /* pale cyan */
0x00, 0x80, 0x80, 0,   /* dark cyan */
0x80, 0x80, 0x00, 0,   /* dark yellow */
0x80, 0xFF, 0x00, 0,   /* yellow-green */
0x80, 0x00, 0xFF, 0,   /* violet (magenta-blue) */
0x00, 0xFF, 0x80, 0,   /* cyan-green */
0xFF, 0x00, 0x80, 0,   /* crimson (magenta-red) */
0x00, 0x80, 0xFF, 0,    /* azure (cyan-blue) */
0xBF, 0x40, 0x40, 0,    /* grey-red */
0x40, 0xBF, 0x40, 0,    /* grey-green */
0x40, 0x40, 0xBF, 0,    /* grey-blue */
0xBF, 0xBF, 0x40, 0,    /* grey-yellow */
0x40, 0xBF, 0xBF, 0,    /* grey-cyan */
0xBF, 0x40, 0xBF, 0,    /* grey-magenta */
0xFF, 0xBF, 0xBF, 0,    /* 6 very pale colours */
0xBF, 0xBF, 0xFF, 0,
0xBF, 0xFF, 0xBF, 0,
0xFF, 0xFF, 0xBF, 0,
0xFF, 0xBF, 0xFF, 0,
0xBF, 0xFF, 0xFF, 0,
0x40, 0x00, 0x00, 0,    /* 6 very dark colours */
0x00, 0x00, 0x40, 0,
0x00, 0x40, 0x00, 0,
0x40, 0x40, 0x00, 0,
0x40, 0x00, 0x40, 0,
0x00, 0x40, 0x40, 0,
0xFF, 0x40, 0x40, 0,    /* 12 moderately pale colours */
0xFF, 0xFF, 0x40, 0,
0x40, 0xFF, 0x40, 0,
0x40, 0xFF, 0xFF, 0,
0x40, 0x40, 0xFF, 0,
0xFF, 0x40, 0xFF, 0,
0xFF, 0x9F, 0x40, 0,
0x9F, 0xFF, 0x40, 0,
0x40, 0xFF, 0x9F, 0,
0x40, 0x9F, 0xFF, 0,
0x9F, 0x40, 0xFF, 0,
0xFF, 0x40, 0x9F, 0,
0xBF, 0x00, 0x00, 0,   /* 12 moderately dark colours */
0xBF, 0xBF, 0x00, 0,
0x00, 0xBF, 0x00, 0,
0x00, 0xBF, 0xBF, 0,
0x00, 0x00, 0xBF, 0,
0xBF, 0x00, 0xBF, 0,
0xBF, 0x60, 0x00, 0,
0x60, 0xBF, 0x00, 0,
0x00, 0xBF, 0x60, 0,
0x00, 0x60, 0xBF, 0,
0x60, 0x00, 0xBF, 0,
0xBF, 0x00, 0x60, 0 };


HPALETTE hpaletteWin;   /*Handle to Logical Palette */

/* Macro to allow dithering of palette relative colour */
#define DITHER(c) ((COLORREF)((0xFFFFFF)&(c)))

/* End of WIN include file */
