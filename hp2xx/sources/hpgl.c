/* Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.  
   Parts Copyright (c) 1999 - 2001 Martin Kroeker All rights reserved.

   Distributed by Free Software Foundation, Inc.

   This file is part of HP2xx.

   HP2xx is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
   to anyone for the consequences of using it or for whether it serves any
   particular purpose or works at all, unless he says so in writing.  Refer
   to the GNU General Public License, Version 2 or later, for full details.

   Everyone is granted permission to copy, modify and redistribute
   HP2xx, but only under the conditions described in the GNU General Public
   License.  A copy of this license is supposed to have been
   given to you along with HP2xx so you can know your rights and
   responsibilities.  It should be in a file named COPYING.  Among other
   things, the copyright notice and this notice must be preserved on all
   copies.

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/** HPGL.c: HPGL parser & i/o part of HP2xx (based on D. Donath's "HPtoGF.c")
 **
 ** 91/01/13  V 1.00  HWW  Originating
 ** 91/01/19  V 1.01  HWW  reorganized
 ** 91/01/24  V 1.02  HWW  ESC.-Sequences acknowledged (preliminary!!)
 ** 91/01/29  V 1.03  HWW  Incl. SUN portation
 ** 91/01/31  V 1.04  HWW  Parser: ESC sequences should be skipped now
 ** 91/02/10  V 1.05  HWW  Parser renewed
 ** 91/02/15  V 1.06  HWW  stdlib.h supported
 ** 91/02/19  V 1.07a HWW  parser refined, bugs fixed
 ** 91/06/09  V 1.08  HWW  New options added; some restructuring
 ** 91/06/16  V 1.09  HWW  VGA mode added; some renaming; silent_mode!
 ** 91/06/20  V 1.10  HWW  Rotation added
 ** 91/10/15  V 1.11  HWW  ANSI_C; header files reorganized
 ** 91/10/20  V 1.11a HWW  VAX_C support
 ** 91/10/25  V 1.11b HWW  Support of LT; and LT0; (line type, partial)
 ** 91/11/20  V 1.12  HWW  SPn; support: many changes!
 ** 91/11/21  V 1.12b HWW  First comma in "PA,xxxx,yyyy..." accepted
 ** 91/12/22  V 1.13  HWW  Multiple MOVE compression; "plot_rel", "old_pen"
 ** 92/01/13  V 1.13c HWW  VAX problem with ungetc()/fscanf() fixed; bug fixed
 ** 92/01/15  V 1.13d HWW  "vga" --> "pre"
 ** 92/01/30  V 1.14c HWW  Parser: no need of ';', better portable
 ** 92/02/06  V 1.15a HWW  Parser: AR, AA, CI, read_float() added;
 **			   toupper() removed (MACH problems)
 ** 92/02/19  V 1.16c HWW  LB etc. supported
 ** 92/02/23  V 1.17b HWW  LB etc. improved, PG supported
 ** 92/02/25  V 1.17c HWW  Parser improved: SP, LT, multi-mv suppression
 ** 92/03/01  V 1.17d HWW  Char sizes: debugged
 ** 92/03/03  V 1.17e HWW  LB_Mode introduced
 ** 92/04/15  V 1.17f HWW  Width x Height limit assumed
 ** 92/05/21  V 1.18a HWW  Multiple-file usage
 ** 92/05/28  V 1.19a HWW  XT, YT, TL, SM added
 ** 92/10/20  V 1.20c HWW  More line types added (debugged)
 ** 92/11/08  V 1.20d HWW  Interval of active pages
 ** 92/12/13  V 1.20e HWW  truesize option added
 ** 93/02/10  V 1.21a HWW  Arcs & circles now properly closed;
 **			   Bug fixed: SC does not interfere with last move
 ** 93/03/10  V 1.21b HWW  Bug fixed in LT scanner part
 ** 93/03/22, V 1.21c HWW  HYPOT() workaround for a weird BCC behavior;
 ** 93/04/02		   Line_Generator(): Case *pb==*pa caught
 ** 93/04/13  V 1.22a HWW  UC supported (code by Alois Treindl)
 ** 93/04/25  V 1.22b HWW  LB/PR bug fix by E. Norum included
 ** 93/05/20  V 1.22c HWW  LT1 pattern adjustment (report by E. Frambach)
 ** 93/09/02  V 1.22d HWW  EA (rect) added (by Th. Hiller)
 ** 94/01/01  V 1.22e HWW  Misc. additions suggested by L. Lowe:
 **			    1) PlotCmd_from_tmpfile(): int --> PlotCmd
 **                         2) ES: 2nd parameter now optional
 **                         3) evaluate_HPGL(): center_mode introduced
 ** 94/02/14  V 1.30a HWW  Re-organized; many changes; min/max bug fixed
 ** 99/02/28          MK   IW,CA,CS,SA,SS commands added
 ** 99/04/24          MK   PC,PW commands added
 ** 99/05/10          MK   RO command added (code by rs@daveg.com) 
 ** 99/05/18          MK   partial PE support (by Eugene Doudine)
 ** 99/06/05          MK   PC improvements and fixes
 ** 99/11/30          MK   support for fractional PE; PS/RO fixes
 ** 00/02/06          MK   allow commandline overrides for PC/PW
 ** 00/02/13          MK   DV support (backport from delayed 3.4.prealpha)
 ** 00/02/26          MK   ER,EP,FP,FT,PM,PT,RA,RR,WG commands added
 ** 00/02/27          MK   WU command added
 ** 00/03/02          MK   SC types 1 and 2, more robust handling of PE,
 **                        removed PE_line(), split lines() into file reader
 **                        and common linedrawing backend for PD/PA and PE
 **                        added PJL parser and RTL escape sequences 
 **                        (all these patches provided by Eugene Doudine) 
 ** 00/03/03          MK   convert IW parameters if scaling is in effect 
 ** 00/03/05          MK   AT/RT (arc through three points) added
 ** 01/01/01	      MK   UL added and PW rewritten (Andrew J.Bird)
 ** 		           empty PUPD sequence now draws a small dot
 **			   linedrawing fixed (added two moves when IW was
 **	 		   in effect, ever since IW support was added)
 ** 01/04/01	      MK   BR/BZ added
 ** 01/04/22	      MK   reset PW and RO flags/values on reinitialization
 **			   (Yuri Strelenko)  
 ** 01/12/04	      MK   moved reinitialization of n_unknown and n_unexpected
 **			   from reset_HPGL to init_HPGL so that they are not overwritten
 **			   when a single drawing contains several IN statements
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "chardraw.h"
#include "clip.h"
#include "pendef.h"
#include "lindef.h"

#define	ETX		'\003'

/*
#define P1X_default	603.0	
#define P1Y_default	521.0
#define P2X_default	10603.0
#define P2Y_default	7721.0
*/
#define P1X_default	0.0	/* drop margins       */
#define P1Y_default	0.0
/*
#define P2X_default   11880. 
#define P2Y_default   16800.
*/
#define P2X_default   33600.	/* A0 media */
#define P2Y_default   47520.


#ifdef __TURBOC__
#define	HYPOT(x,y)	sqrt((x)*(x)+(y)*(y))
#else
#define	HYPOT(x,y)	hypot(x,y)
#endif

#ifdef NORINT
#define rint(a) (long)(a+0.5)
#endif

/**
 ** Globals needed in chardraw.c:
 **/
				/* Line type selected by HP-GL code     */
				/* Currently effective line type        */
LineType CurrentLineType = LT_solid;

HPGL_Pt HP_pos = { 0 };		/* Actual plotter pen position  */
HPGL_Pt P1 = { P1X_default, P1Y_default };	/* Scaling points */
HPGL_Pt P2 = { P2X_default, P2Y_default };
int iwflag = 0;			/*MK */
int mode_vert = 0;
HPGL_Pt C1 = { P1X_default, P1Y_default };	/* Clipping points        */
HPGL_Pt C2 = { P2X_default, P2Y_default };
HPGL_Pt S1 = { P1X_default, P1Y_default };	/* Scaled       */
HPGL_Pt S2 = { P2X_default, P2Y_default };	/* points       */
HPGL_Pt Q;			/* Delta-P/Delta-S: Initialized with first SC   */
HPGL_Pt M;			/* maximum coordinates set by PS instruction */
/**
 ** Global from chardraw.c:
 **/
extern TextPar tp;

/**
 ** "Local" globals (I know this is messy...) :
 **/
static float xmin, xmax, ymin, ymax, neg_ticklen, pos_ticklen;
static double Diag_P1_P2, pat_pos;
static HPGL_Pt p_last = { M_PI, M_PI };	/* Init. to "impossible" values */

static HPGL_Pt polygons[MAXPOLY];
static int vertices = -1;
static short polygon_mode = FALSE;
static int filltype = 1;
static float hatchspace = 0.;
static float hatchangle = 0.;
static float saved_hatchspace[2]={0.,0.};
static float saved_hatchangle[2]={0.,0.};
static float thickness = 0.;
static short polygon_penup = FALSE;
static HPGL_Pt anchor = {0.0 , 0.0};
static float rot_cos, rot_sin;

static short rotate_flag = FALSE;	/* Flags tec external to HP-GL  */
static short ps_flag = FALSE;
static double rot_ang = 0.;
static double rot_tmp = 0.;	/* saved RO value for resetting after drawing */
short scale_flag = FALSE;
static short mv_flag = FALSE;
static short ct_dist = FALSE;
#ifdef	ATARI
extern short silent_mode = FALSE;	/* Don't clobber ATARI preview! */
#else
static short silent_mode = FALSE;
#endif
static short fixedcolor = FALSE;
static short fixedwidth = FALSE;
static short record_off = FALSE;
static short first_page = 0;
static int last_page = 0;
static int n_unexpected = 0;
static int n_unknown = 0;
static int page_number = 1;
static long vec_cntr_r = 0L;
static long vec_cntr_w = 0L;
static short pen = -1;
static short pens_in_use[NUMPENS];
static short pen_down = FALSE;	/* Internal HP-GL book-keeping: */
static short plot_rel = FALSE;
static short saved_penstate = FALSE; /* to track penstate over polygon mode */
static short wu_relative = FALSE;
static int again=FALSE;
static char StrTerm = ETX;	/* String terminator char       */
static char *strbuf = NULL;
static unsigned int strbufsize = MAX_LB_LEN + 1;
static char symbol_char = '\0';	/* Char in Symbol Mode (0=off)  */

static FILE *td;


/* Known HPGL commands, ASCII-coded as High-byte/low-byte int's */

#define AA	0x4141
#define AC	0x4143
#define AD      0x4144
#define AF	0x4146
#define AH	0x4148
#define AR	0x4152
#define AT      0x4154
#define BL	0x424C
#define BP      0x4250
#define BR      0x4252
#define BZ      0x425A
#define CA      0x4341		/*MK */
#define CI	0x4349
#define CO	0x434F /*AJB*/
#define CP	0x4350
#define CS      0x4353		/*MK */
#define CT	0x4354
#define DF	0x4446
#define DI	0x4449
#define DR	0x4452
#define DT	0x4454
#define DV      0x4456
#define EA	0x4541
#define EC	0x4543 /*AJB*/
#define EP      0x4550
#define ER      0x4552
#define ES	0x4553
#define EW      0x4557		/*MK */
#define FP      0x4650
#define FT      0x4654
#define IN	0x494E
#define IP	0x4950
#define IW      0x4957		/*MK */
#define LB	0x4C42
#define LO	0x4C4F
#define LT	0x4C54
#define MG      0x4D47
#define NP      0x4E50
#define NR      0x4E52
#define OP	0x4F50
#define PA	0x5041
#define PB	0x5042
#define PC      0x5043		/*MK */
#define PD	0x5044
#define PE      0x5045
#define PG	0x5047
#define PM      0x504D
#define PR	0x5052
#define PS      0x5053
#define PT      0x5054
#define PU	0x5055
#define PW	0x5057		/*MK */
#define RA      0x5241
#define RO	0x524F		/*RS */
#define RR      0x5252
#define RT      0x5254
#define SA      0x5341		/*MK */
#define SC	0x5343
#define SD      0x5344
#define SI	0x5349
#define SL	0x534C
#define SM	0x534D
#define SP	0x5350
#define SR	0x5352
#define SS      0x5353		/*MK */
#define TL	0x544C
#define UC	0x5543
#define UL	0x554C /*AJB*/
#define WD	0x5744
#define WG      0x5747
#define WU      0x5755
#define XT	0x5854
#define YT	0x5954
  static void
par_err_exit (int code, int cmd)
/*par_err_exit (short code, short cmd)*/
{
  const char *msg;

  switch (code)
    {
    case 0:
      msg = "Illegal parameters";
      break;
    case 1:
      msg = "Error in first parameter";
      break;
    case 2:
      msg = "No second parameter";
      break;
    case 3:
      msg = "No third parameter";
      break;
    case 4:
      msg = "No fourth parameter";
      break;
    case 98:
      msg = "sscanf error: corrupted file?";
      break;
    case 99:
    default:
      msg = "Internal error";
      break;
    }
  Eprintf ("\nError in command %c%c: %s\n", cmd >> 8, cmd & 0xFF, msg);
  Eprintf (" @ Cmd %ld\n", vec_cntr_w);
  exit (ERROR);
}




static void
reset_HPGL (void)
{
  p_last.x = p_last.y = M_PI;
  pen_down = FALSE;
  plot_rel = FALSE;
  pen = -1;
/*  n_unexpected = 0;
  n_unknown = 0;*/
  mv_flag = FALSE;
  wu_relative = FALSE;
  ct_dist = FALSE;
  CurrentLineType = LT_solid;

  set_line_style_defaults ();

  StrTerm = ETX;
  if (strbuf == NULL)
    {
      strbuf = malloc (strbufsize);
      if (strbuf == NULL)
	{
	  fprintf (stderr, "\nNo memory !\n");
	  exit (ERROR);
	}
    }
  strbuf[0] = '\0';

  P1.x = P1X_default;
  P1.y = P1Y_default;
  P2.x = P2X_default;
  P2.y = P2Y_default;
  Diag_P1_P2 = HYPOT (P2.x - P1.x, P2.y - P1.y);
  CurrentLinePatLen = 0.04 * Diag_P1_P2;
  pat_pos = 0.0;
  scale_flag = FALSE;
  S1 = P1;
  S2 = P2;
  Q.x = Q.y = 1.0;
  HP_pos.x = HP_pos.y = 0.0;
  neg_ticklen = 0.005;		/* 0.5 %        */
  pos_ticklen = 0.005;
  symbol_char = '\0';
  rot_ang -= rot_tmp;
  rot_tmp=0.;
  if (rot_ang == 0.)
    rotate_flag = FALSE;
  if (rotate_flag)
    {
      rot_cos = cos (M_PI * rot_ang / 180.0);
      rot_sin = sin (M_PI * rot_ang / 180.0);
    }
  init_text_par ();
}



static void
init_HPGL (const GEN_PAR * pg, const IN_PAR * pi)
{
/**
 ** Re-init. global var's for multiple-file applications
 **/

  td = pg->td;
  silent_mode = pg->quiet;
  xmin = pi->x0;
  ymin = pi->y0;
  xmax = pi->x1;
  ymax = pi->y1;
  fixedcolor = pi->hwcolor;
  fixedwidth = pi->hwsize;
/*  pens_in_use = 0; */

  /**
   ** Record ON if no page selected (pg->page == 0)!
   **/
  first_page = pi->first_page;	/* May be 0     */
  last_page = pi->last_page;	/* May be 0     */
  page_number = 1;
  record_off = (first_page > page_number)
    || ((last_page < page_number) && (last_page > 0));

  rot_ang = pi->rotation;
  rotate_flag = (rot_ang != 0.0) ? TRUE : FALSE;
  if (rotate_flag)
    {
      rot_cos = cos (M_PI * rot_ang / 180.0);
      rot_sin = sin (M_PI * rot_ang / 180.0);
    }

  vec_cntr_r = 0L;
  vec_cntr_w = 0L;
  n_unexpected = 0;
  n_unknown = 0;

  reset_HPGL ();
}



static void
User_to_Plotter_coord (const HPGL_Pt * p_usr, HPGL_Pt * p_plot)
/**
 ** 	Utility: Transformation from (scaled) user coordinates
 **	to plotter coordinates
 **/
{
  p_plot->x = P1.x + (p_usr->x - S1.x) * Q.x;
  p_plot->y = P1.y + (p_usr->y - S1.y) * Q.y;
}



static void
Plotter_to_User_coord (const HPGL_Pt * p_plot, HPGL_Pt * p_usr)
/**
 ** 	Utility: Transformation from plotter coordinates
 **	to (scaled) user coordinates
 **/
{
  p_usr->x = S1.x + (p_plot->x - P1.x) / Q.x;
  p_usr->y = S1.y + (p_plot->y - P1.y) / Q.y;
}



static void
PlotCmd_to_tmpfile (PlotCmd cmd)
{
  if (record_off)		/* Wrong page!  */
    return;

  if (!silent_mode)
    switch (vec_cntr_w++)
      {
      case 0:
	Eprintf ("Writing Cmd: ");
	break;
      case 1:
	Eprintf ("1 ");
	break;
      case 2:
	Eprintf ("2 ");
	break;
      case 5:
	Eprintf ("5 ");
	break;
      case 10:
	Eprintf ("10 ");
	break;
      case 20:
	Eprintf ("20 ");
	break;
      case 50:
	Eprintf ("50 ");
	break;
      case 100:
	Eprintf ("100 ");
	break;
      case 200:
	Eprintf ("200 ");
	break;
      case 500:
	Eprintf ("500 ");
	break;
      case 1000:
	Eprintf ("1k ");
	break;
      case 2000:
	Eprintf ("2k ");
	break;
      case 5000:
	Eprintf ("5k ");
	break;
      case 10000:
	Eprintf ("10k ");
	break;
      case 20000:
	Eprintf ("20k ");
	break;
      case 50000L:
	Eprintf ("50k ");
	break;
      case 100000L:
	Eprintf ("100k ");
	break;
      case 200000L:
	Eprintf ("200k ");
	break;
      case 500000L:
	Eprintf ("500k ");
	break;
      case 1000000L:
	Eprintf ("1000k ");
	break;
      case 2000000L:
	Eprintf ("2000k ");
	break;
      }

  if (fputc ((int) cmd, td) == EOF)
    {
      PError ("PlotCmd_to_tmpfile");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
}


static void
Pen_Width_to_tmpfile (int pen, int width)
{
  PEN_N tp;
  PEN_W tw;

  tp = pen;
  tw = width;

  if (record_off)		/* Wrong page!  */
    return;

  if (fwrite (&tp, sizeof (tp), 1, td) != 1)
    {
      PError ("Pen_Width_to_tmpfile - pen");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
  if (fwrite (&tw, sizeof (tw), 1, td) != 1)
    {
      PError ("Pen_Width_to_tmpfile - width");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
}

static void
Pen_Color_to_tmpfile (int pen, int red, int green, int blue)
{
  PEN_N tp;
  PEN_C r, g, b;

  tp = pen;
  r = red;
  g = green;
  b = blue;

  if (record_off)		/* Wrong page!  */
    return;

  if (fwrite (&tp, sizeof (tp), 1, td) != 1)
    {
      PError ("Pen_Color_to_tmpfile - pen");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
  if (fwrite (&r, sizeof (r), 1, td) != 1)
    {
      PError ("Pen_Color_to_tmpfile - red component");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
  if (fwrite (&g, sizeof (g), 1, td) != 1)
    {
      PError ("Pen_Color_to_tmpfile - green component");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
  if (fwrite (&b, sizeof (b), 1, td) != 1)
    {
      PError ("Pen_Color_to_tmpfile - blue component");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
}

static void
HPGL_Pt_to_tmpfile (const HPGL_Pt * pf)
{
  if (record_off)		/* Wrong page!  */
    return;

  if (fwrite ((VOID *) pf, sizeof (*pf), 1, td) != 1)
    {
      PError ("HPGL_Pt_to_tmpfile");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
  xmin = MIN (pf->x, xmin);
  ymin = MIN (pf->y, ymin);
  xmax = MAX (pf->x, xmax);
  ymax = MAX (pf->y, ymax);
}




/**
 **	Low-level vector generation & file I/O
 **/

static void
LPattern_Generator (HPGL_Pt * pa,
		    double dx, double dy,
		    double start_of_pat, double end_of_pat)
/**
 **	Generator of Line type patterns:
 **
 **	pa:		Start point (ptr) of current line segment
 **	dx, dy:		Components of c * (*pb - *pa), c holding
 **				dx^2 + dy^2 = pattern_length^2
 **	start_of_pat:	Fraction of start point within pattern
 **	end_of_pat:	Fraction of end   point within pattern
 **			Valid: 0 <= start_of_pat <= end_of_pat <= 1
 **
 **	A pattern consists of alternating "line"/"point" and "gap" elements,
 **	always starting with a line/point. A point is a line of zero length.
 **	The table below contains the relative lengths of the elements
 **	of all line types except LT0; and LT; (7), which are treated separately.
 **	These lengths always add up to 1. A negative value terminates a pattern.
 **/
{
  double length_of_ele, start_of_action, end_of_action;
  static double *p_cur_pat;

  p_cur_pat = lt[(LT_MIN * -1) + (int) CurrentLinePattern];	/* was CurrentLineType */

  if (CurrentLineType == LT_adaptive)
    for (;;)
      {
	length_of_ele = (double) *p_cur_pat++ / 100;	/* Line or point        */
	if (length_of_ele < 0)
	  return;
	if (length_of_ele == 0.0)
	  PlotCmd_to_tmpfile (PLOT_AT);
	else
	  PlotCmd_to_tmpfile (DRAW_TO);

	pa->x += dx * length_of_ele;
	pa->y += dy * length_of_ele;
	HPGL_Pt_to_tmpfile (pa);

	length_of_ele = (double) *p_cur_pat++ / 100;	/* Gap        */
	if (length_of_ele < 0)
	  return;
	pa->x += dx * length_of_ele;
	pa->y += dy * length_of_ele;
	PlotCmd_to_tmpfile (MOVE_TO);
	HPGL_Pt_to_tmpfile (pa);
      }
  else				/* LT_fixed */
    for (end_of_action = 0.0;;)
      {
	    /**
	     ** Line or point:
	     **/
	start_of_action = end_of_action;
	length_of_ele = (double) *p_cur_pat++ / 100;
	if (length_of_ele < 0)
	  return;

         if (length_of_ele == 0.0) {             /* Dot Only */
            PlotCmd_to_tmpfile (PLOT_AT);
          HPGL_Pt_to_tmpfile (pa);
         } else {                                /* Line Segment */
	end_of_action += length_of_ele;

	if (end_of_action > start_of_pat)	/* If anything to do:   */
	  {
	    if (start_of_pat <= start_of_action)
	      {			/* If start is valid    */
		if (end_of_action <= end_of_pat)
		  {		/* Draw full element    */
		    pa->x += dx * length_of_ele;
		    pa->y += dy * length_of_ele;
		      PlotCmd_to_tmpfile (DRAW_TO);
		    HPGL_Pt_to_tmpfile (pa);
		  }
		else
		  /* End_of_action beyond End_of_pattern:   */
		  {		/* --> Draw only first part of element: */
		    pa->x += dx * (end_of_pat - start_of_action);
		    pa->y += dy * (end_of_pat - start_of_action);
		      PlotCmd_to_tmpfile (DRAW_TO);
		    HPGL_Pt_to_tmpfile (pa);
		    return;
		  }
	      }
	    else
	      /* Start_of_action before Start_of_pattern:       */
	      {
		if (end_of_action <= end_of_pat)
		  {		/* Draw remainder of element            */
		    pa->x += dx * (end_of_action - start_of_pat);
		    pa->y += dy * (end_of_action - start_of_pat);
		      PlotCmd_to_tmpfile (DRAW_TO);
		    HPGL_Pt_to_tmpfile (pa);
		  }
		else
		  /* End_of_action beyond End_of_pattern:   */
		  /* Draw central part of element & leave   */
		  {
		    if (end_of_pat == start_of_pat)
		      PlotCmd_to_tmpfile (PLOT_AT);
		    else
		      PlotCmd_to_tmpfile (DRAW_TO);
		    pa->x += dx * (end_of_pat - start_of_pat);
		    pa->y += dy * (end_of_pat - start_of_pat);

		    HPGL_Pt_to_tmpfile (pa);
		    return;
		  }
	      }
	  }
	}
	    /**
	     ** Gap (analogous to line/point):
	     **/
	start_of_action = end_of_action;
	length_of_ele = (double) *p_cur_pat++ / 100;
	if (length_of_ele < 0)
	  return;
	end_of_action += length_of_ele;
	if (end_of_action > start_of_pat)	/* If anything to do:   */
	  {
	    if (start_of_pat <= start_of_action)
	      {			/* If start is valid    */
		if (end_of_action <= end_of_pat)
		  {		/* Full gap             */
		    pa->x += dx * length_of_ele;
		    pa->y += dy * length_of_ele;
		    PlotCmd_to_tmpfile (MOVE_TO);
		    HPGL_Pt_to_tmpfile (pa);
		  }
		else
		  /* End_of_action beyond End_of_pattern:   */
		  {		/* --> Apply only first part of gap:    */
		    pa->x += dx * (end_of_pat - start_of_action);
		    pa->y += dy * (end_of_pat - start_of_action);
		    PlotCmd_to_tmpfile (MOVE_TO);
		    HPGL_Pt_to_tmpfile (pa);
		    return;
		  }
	      }
	    else
	      /* Start_of_action before Start_of_pattern:       */
	      {
		if (end_of_action <= end_of_pat)
		  {		/* Apply remainder of gap               */
		    pa->x += dx * (end_of_action - start_of_pat);
		    pa->y += dy * (end_of_action - start_of_pat);
		    PlotCmd_to_tmpfile (MOVE_TO);
		    HPGL_Pt_to_tmpfile (pa);
		  }
		else
		  /* End_of_action beyond End_of_pattern:   */
		  /* Apply central part of gap & leave      */
		  {
		    if (end_of_pat == start_of_pat)
		      return;	/* A null move  */
		    pa->x += dx * (end_of_pat - start_of_pat);
		    pa->y += dy * (end_of_pat - start_of_pat);
		    PlotCmd_to_tmpfile (MOVE_TO);
		    HPGL_Pt_to_tmpfile (pa);
		    return;
		  }
	      }
	  }
      }
}

/*
   struct PE_flags{
   int abs;
   int up;
   int sbmode;
   int fract;
   int pen;
   } ;
 */
int
read_PE_flags (const GEN_PAR * pg, int c, FILE * hd, PE_flags * fl)
{
  short old_pen;
  float ftmp;
  int ctmp;
  switch (c)
    {
    case 183:
    case '7':
      /* seven bit mode */
      fl->sbmode = 1;
      break;

    case 186:
    case ':':
      /* select pen */
      if (EOF == (fl->pen = getc (hd)))
	{
	  par_err_exit (98, PE);
	}
      old_pen = pen;
      read_PE_coord (fl->pen, hd, fl, &ftmp);
      pen = ftmp;
      if (pen < 0 || pen > pg->maxpens)
	{
	  Eprintf ("\nIllegal pen number %d: replaced by %d\n", pen,
		   pen % pg->maxpens);
	  n_unexpected++;
	  pen = pen % pg->maxpens;
	}
      if (old_pen != pen)
	{
	  if ((fputc (SET_PEN, td) == EOF) || (fputc (pen, td) == EOF))
	    {
	      PError ("Writing to temporary file:");
	      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
	      exit (ERROR);
	    }
	}
      if (pen)
	pens_in_use[pen] = 1;
      /*         pens_in_use |= (1 << (pen-1)); */
/*MK */
      break;

    case 190:
    case '>':
      /* fractional data */

      if (EOF == (ctmp = getc (hd)))
	{
	  par_err_exit (98, PE);
	}
      fl->fract = decode_PE_char (ctmp, fl);
      fl->fract = ((fl->fract >> 1) * ((fl->fract & 0x01) ? -1 : 1));
/*      fprintf(stderr,"PE > flag, fract =%d (%d decimals) ",fl->fract, fl->fract/3); */
      break;

    case 188:
    case '<':
      /* pen up */
      fl->up = 1;
      break;

    case 189:
    case '=':
      /* abs plotting */

      fl->abs = 1;
      break;

    default:
      return (0);
    }
  return (1);
}

int
isPEterm (int c, PE_flags * fl)
{
  if ((fl->sbmode) && ((c > 94) || (c < 63)))
    return 1;
  if ((!fl->sbmode) && ((c > 190) || (c < 63)))
    return 1;
  return (0);
}


int
decode_PE_char (int c, PE_flags * fl)
{
  if (fl->sbmode)
    {
      c &= 0x7f;
      return ((c > 94) ? (c - 95) : (c - 63));
    }
  else
    {
      return ((c > 190) ? (c - 191) : (c - 63));
    }
}

int
read_PE_coord (int c, FILE * hd, PE_flags * fl, float *fv)
{
  long lv = 0;
  int i = 0;
  int shft = (fl->sbmode) ? 5 : 6;

  for (;;)
    {
      if (c < 63)
	{
	  if (!i)
	    {			/* avoid endless getc/ungetc loop with broken files */
	      Eprintf ("error in PE data!\n");
	      return 0;
	    }
	  ungetc (c, hd);
	  break;
	}
      lv |= ((long) decode_PE_char (c, fl)) << (i * shft);
      i++;
      if (isPEterm (c, fl))
	{
	  break;
	}
      if (EOF == (c = getc (hd)))
	{
	  par_err_exit (98, PE);
	}
    }
  *fv = (float) (((lv >> 1) * ((lv & 0x01) ? -1 : 1)) << fl->fract);
  return (1);
}


int
read_PE_pair (int c, FILE * hd, PE_flags * fl, HPGL_Pt * p)
{
  if (!read_PE_coord (c, hd, fl, &(p->x)))
    return 0;
  if (EOF == (c = getc (hd)))
    {
      par_err_exit (98, PE);
    }
  if (!read_PE_coord (c, hd, fl, &(p->y)))
    return 0;
  return (1);
}




void
read_PE (const GEN_PAR * pg, FILE * hd)
{
  int c;

  HPGL_Pt p;
  PE_flags fl;

  fl.fract = 0;
  fl.sbmode = 0;
  fl.abs = 0;
  fl.up = 0;
  fl.pen = 0;

  for (c = getc (hd); (c != EOF) && (c != ';'); c = getc (hd))
    {
      if (!read_PE_flags (pg, c, hd, &fl))
	{
	  if (!read_PE_pair (c, hd, &fl, &p))
	    continue;
	  pen_down = (fl.up) ? FALSE : TRUE;
	  line (!fl.abs, p);
	  tp->CR_point = HP_pos;
	  fl.abs = 0;
	  fl.up = 0;
	}
    }
}


double ceil_with_tolerance(double x,double tol) {
    double rounded;
                 
    rounded=rint(x);
                 
    if(fabs(rounded - x) <= tol )
       return(rounded);
    else         
       return(ceil(x));
}     

static void
Line_Generator (HPGL_Pt * pa, const HPGL_Pt * pb, int mv_flag)
{
  double seg_len, dx, dy, quot;
  int n_pat, i;

  dx = pb->x - pa->x;
  dy = pb->y - pa->y;
  seg_len = HYPOT (dx, dy);

  switch (CurrentLineType)
    {

    case LT_solid:
      if (seg_len == 0.0) return;/***???***/
      PlotCmd_to_tmpfile (DRAW_TO);
      HPGL_Pt_to_tmpfile (pb);
      return;

    case LT_adaptive:
      if (seg_len == 0.0)
	{
	  if (!silent_mode)
	    Eprintf ("Warning: Zero line segment length -- skipped\n");
	  return;		/* No line to draw ??           */
	}
      pat_pos = 0.0;		/* Reset to start-of-pattern    */
      n_pat = ceil_with_tolerance (seg_len / CurrentLinePatLen, CurrentLinePatLen * LT_PATTERN_TOL);
      dx /= n_pat;
      dy /= n_pat;
      /* Now draw n_pat complete line patterns */
      for (i = 0; i < n_pat; i++)
	LPattern_Generator (pa, dx, dy, 0.0, 1.0);
      return;

    case LT_plot_at:
      PlotCmd_to_tmpfile (PLOT_AT);
      HPGL_Pt_to_tmpfile (pb);
      return;

    case LT_fixed:
      if (seg_len == 0.0)
	{
	  if (!silent_mode)
	    Eprintf ("Warning: Zero line segment length -- skipped\n");
	  return;		/* No line to draw ??           */
	}

      if (mv_flag)		/* Last move ends old line pattern      */
	pat_pos = 0.0;
      quot = seg_len / CurrentLinePatLen;
      dx /= quot;
      dy /= quot;
      while (quot >= 1.0)
	{
	  LPattern_Generator (pa, dx, dy, pat_pos, 1.0);
	  quot -= (1.0 - pat_pos);
	  pat_pos = 0.0;
	}
      quot += pat_pos;
      if (quot >= 1.0)
	{
	  LPattern_Generator (pa, dx, dy, pat_pos, 1.0);
	  quot -= 1.0;
	  pat_pos = 0.0;
	}
      if (quot > LT_PATTERN_TOL) {
      	LPattern_Generator (pa, dx, dy, pat_pos, quot);
      	pat_pos = quot;
      } else {
      	PlotCmd_to_tmpfile (MOVE_TO);
	HPGL_Pt_to_tmpfile (pb);
      }
      return;

    default:
      break;
    }

}




void
Pen_action_to_tmpfile (PlotCmd cmd, const HPGL_Pt * p, int scaled)
{
  static HPGL_Pt P_last;
  HPGL_Pt P;
  double tmp;

  if (record_off)		/* Wrong page!  */
    return;

  if (scaled)			/* Rescaling    */
    User_to_Plotter_coord (p, &P);
  else
    P = *p;			/* Local copy   */


  HP_pos = P;			/* Actual plotter pos. in plotter coord */
  if (rotate_flag)		/* hp2xx-specific global rotation       */
    {
      tmp = rot_cos * P.x - rot_sin * P.y;
      P.y = rot_sin * P.x + rot_cos * P.y;
      P.x = tmp;
    }


  /* Extreme values needed for later scaling:    */

  switch (cmd)
    {
    case MOVE_TO:
      mv_flag = TRUE;
      break;

  /**
   ** Multiple-move suppression. In addition,
   ** a move only precedes a draw -- nothing else!
   **/

    case DRAW_TO:
      if (mv_flag)
	{
	  PlotCmd_to_tmpfile (MOVE_TO);
	  HPGL_Pt_to_tmpfile (&P_last);
	}
      /* drop through */
    case PLOT_AT:
      Line_Generator (&P_last, &P, mv_flag);
      mv_flag = FALSE;
      break;

    default:
      Eprintf ("Illegal Pen Action: %d\n", cmd);
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
      exit (ERROR);
    }
  P_last = P;
}





int
read_float (float *pnum, FILE * hd)
/**
 ** Main work-horse for parameter input:
 **
 ** Search for next number, skipping white space but return if mnemonic met.
 ** If found, read in number
 **	returns	0 if valid number
 **		1 if command ended
 **		2 if scanf failed (possibly corrupted file)
 **	      EOF if EOF met
 **/
{
  int c;
  char *ptr, numbuf[80];

  for (c = getc (hd);
       (c != '.') && (c != '+') && (c != '-') && ((c < '0') || (c > '9'));
       c = getc (hd))
    {
      if (c == EOF)		/* Wait for number      */
	return EOF;		/* Should not happen    */
      if (c == ';')
	return 1;		/* Terminator reached   */
      if (((c >= 'A') && (c <= 'Z')) ||
	  ((c >= 'a') && (c <= 'a')) || (c == ESC))
	{
	  ungetc (c, hd);
	  return 1;		/* Next Mnemonic reached */
	}
    }
  /* Number found: Get it */
  ptr = numbuf;
  for (*ptr++ = c, c = getc (hd); ((c >= '0') && (c <= '9')) || (c == '.');
       c = getc (hd))
    *ptr++ = c;			/* Read number          */
  *ptr = '\0';
  if (c != EOF)
    ungetc (c, hd);

  if (sscanf (numbuf, "%f", pnum) != 1)
    return 11;			/* Should never happen  */
  return 0;
}




static void
read_string (char *buf, FILE * hd)
{
  int c, n;

  for (n = 0, c = getc (hd); (c != EOF) && (c != StrTerm); c = getc (hd))
    {
      if (n > strbufsize / 2)
	{
	  strbufsize *= 2;
	  strbuf = realloc (strbuf, strbufsize);
	  if (strbuf == NULL)
	    {
	      fprintf (stderr, "\nNo memory !\n");
	      exit (ERROR);
	    }
	  buf = strbuf + n;
	}
      if (c == '\0')
	continue;		/* ignore \0 */
      if (n++ < strbufsize)
	*buf++ = c;
    }
  if (c == StrTerm && c != ETX)
    *buf++ = c;
  *buf = '\0';
}




static void
read_symbol_char (FILE * hd)
{
  int c;

  for (c = getc (hd); /* ended by switch{} */ ; c = getc (hd))
    switch (c)
      {
      case ' ':
      case _HT:
      case _LF:
	break;			/* Skip white space             */
      case _CR:
      case EOF:
      case ';':		/* CR or "term" end symbol mode */
	symbol_char = '\0';
	return;
      default:
	if (c < ' ' || c > '~')
	  break;		/* Ignore unprintable chars     */
	else
	  {
	    symbol_char = c;
	    return;
	  }
      }
}




static void
read_ESC_HP7550A (FILE * hd)
/*
 * Read & skip HP 7550A control commands (ESC.-Commands)
 */
{
  int c;

  switch (getc (hd))
    {
    case EOF:
      n_unexpected++;
      Eprintf ("\nUnexpected EOF!\n");
      return;
      break;
    case 'A':
    case 'B':
    case 'E':
    case 'J':
    case 'K':
    case 'L':
    case 'O':
    case 'U':
    case 'Y':
    case '(':
    case ')':
      return;			/* Commands without parameters  */
    case '@':
    case 'H':
    case 'I':
    case 'M':
    case 'N':
    case 'P':
    case 'Q':
    case 'S':
    case 'T':
      do
	{			/* Search for terminator ':'    */
	  c = getc (hd);
	}
      while ((c != ':') && (c != EOF));
      if (c == EOF)
	{
	  n_unexpected++;
	  Eprintf ("\nUnexpected EOF!\n");
	}
      return;
    default:
      n_unknown++;
      return;
    }
}


static int
read_PJL (FILE * hd)
/*
 * a simple PJL parser
 * just reads PJL header and
 * return
 *   TRUE if PJL enters HPGL context,
 *   FALSE  if not
 *  *
 * PJL lines are like this:
 * @PJL[ command][ args][\r]\n
 * (however I've seen some wrong PJL files with \n\r )
 * @PJL must be uppercase, ther rest of string is not
 * case sensitive
 * The last line of a PJL header is like these:
 * @PJL ENTER LANGUAGE = HPGL2
 * @PJL EOJ [NAME = "something"]
 */
{
#define PJLBS 80

  char strbuf[PJLBS];
  int i, j, ov, ctmp, qt, el = 0, nw = 0, rc = -2, nl = 0;
  for (;;)
    {
      /* read word */
      for (i = ov = qt = 0;; i++)
	{
	  ctmp = getc (hd);
	  if (PJLBS - 1 == i)
	    {
	      if (!silent_mode)
		Eprintf ("PJL buffer overflow, rest of token dropped\n");
	      ov = 1;
	      strbuf[i] = '\0';
	    }
	  if (!ov)
	    strbuf[i] = (0 == nw || qt) ? ctmp : toupper (ctmp);
	  if (EOF == ctmp)
	    {
	      if (!ov)
		strbuf[i] = 0;
	      break;
	    }
	  else if ('=' == ctmp && 0 == i)
	    {
	      strbuf[i] = '=';
	      strbuf[++i] = '\0';
	      ctmp = ' ';
	      break;
	    }
	  else if (strchr (" \t=", ctmp))
	    {
	      if (!qt)
		{
		  if (!ov)
		    strbuf[i] = 0;
		  break;
		}
	    }
	  else if ('\n' == ctmp || '\r' == ctmp)
	    {
	      if (!ov)
		strbuf[i] = 0;
	      nl = 1;
	      break;
	    }
	  else if ('"' == ctmp)
	    {
	      qt = !qt;
	    }
	}
      /* handle word */
      if (i)
	{
#ifdef DEBUG_ESC
	  Eprintf ("word %d: read %d bytes: '%s'\n", nw, i, strbuf);
#endif
	  if (0 == nw && strcmp (strbuf, "@PJL"))
	    {
	      Eprintf ("unexpected end of a PJL header!\n");
	      return (TRUE);
	    }
	  else if (1 == nw && !strcmp (strbuf, "EOJ"))
	    {
	      if (!silent_mode)
		Eprintf ("end of a PJL job\n");
	      rc = TRUE;
	    }
	  else if (1 == nw && !strcmp (strbuf, "ENTER"))
	    {
	      el++;
	    }
	  else if (2 == nw && 1 == el && !strcmp (strbuf, "LANGUAGE"))
	    {
	      el++;
	    }
	  else if (3 == nw && 2 == el && !strcmp (strbuf, "="))
	    {
	      el++;
	    }
	  else if (4 == nw && 3 == el)
	    {
	      if (!silent_mode)
		Eprintf ("Entering %s context\n", strbuf);
	      rc = strncmp (strbuf, "HPGL", 4) ? FALSE : TRUE;
	    }
	  nw++;
	}
      /* read separator */
      for (j = 0; EOF != ctmp; j++)
	{
	  if (!strchr (" \t\n\r", ctmp))
	    {
	      ungetc (ctmp, hd);
	      break;
	    }
	  ctmp = getc (hd);
	  if ('\n' == ctmp)
	    {
	      nl = 1;
	    }
	}
#ifdef DEBUG_ESC
      if (j)
	Eprintf ("separator: read %d bytes\n", j);
#endif

      if (nl)
	{
	  nw = el = nl = 0;
	  if (-2 != rc)
	    return rc;
	}
      if (EOF == ctmp)
	{
	  if (!silent_mode)
	    Eprintf ("EOF in PJL context\n");
	  return (FALSE);
	}
    }
}

static void
read_ESC_RTL (FILE * hd, int c1, int hp)
/*
 *read and skip ESC% control commands
 */
{
  /*
   * known escapes:
   * ESC%-12345X    UEL (Universal Escape Language)
   *                followed by @PJL..
   *
   * ESC%-1B        Enter HPGL/2 context
   * ESC%0B         -
   * ESC%1B         -
   *
   * ESC%1A         Exit HPGL/2 context
   * ESC%0A         -
   * ESC%-1A        -

   * how a PCL escape looks like:
   * ESC, lowercase letters and digits, an Upper case letter

   */
  int c0, c2, ctmp = 0, nf;

  for (c0 = ESC, c2 = getc (hd), nf = 0;
       EOF != c2; c0 = c1, c1 = c2, c2 = getc (hd))
    {

      if ((ESC == c0) && (c1 == '%'))
	{
	  if ('-' == c2)
	    {
	      c2 = getc (hd);
	      nf = 1;
	    }
	  switch (c2)
	    {
	    case EOF:
	      n_unexpected++;
	      Eprintf ("\nUnexpected EOF!\n");
	      return;
	      break;
	    case '1':
	    case '0':
	      switch (ctmp = getc (hd))
		{
		case 'A':

		  if (hp && !silent_mode)
		    {
#ifdef ESC_DEBUG
		      Eprintf ("leaving HPGL context\n");
#endif
		      hp = FALSE;
		    }
		  continue;
		case 'B':
#ifdef ESC_DEBUG
		  if (!silent_mode && !hp)
		    Eprintf ("entering HPGL context\n");
#endif
		  return;
		case '2':
		  /* check for UEL */
		  if (nf && '1' == c2 &&
		      '3' == (c2 = getc (hd)) &&
		      '4' == (c2 = getc (hd)) &&
		      '5' == (c2 = getc (hd)) && 'X' == (c2 = getc (hd)))
		    {
#ifdef ESC_DEBUG
		      if (!silent_mode)
			Eprintf ("UEL found\n");
#endif
		      if (read_PJL (hd))
			{
			  return;
			}
		      else
			{
			  hp = 0;
			  continue;
			}
		    }
		  else
		    {
		      ungetc (ctmp, hd);
		      if (hp)
			return;
		    }
		  break;
		default:
		  Eprintf ("unknown escape: ESC%%%s%c%c\n", nf ? "-" : "", c2,
			   ctmp);
		  ungetc (ctmp, hd);
		  if (hp)
		    return;
		}
	      break;
	    default:
	      Eprintf ("unknown escape: ESC%%%s%c", nf ? "-" : "", c2);
	      ungetc (ctmp, hd);
	      if (hp)
		return;
	      break;
	    }
	}
    }
}

static void
read_ESC_cmd (FILE * hd, int hp)
/*
 * Read & skip device control commands (ESC.-Commands)

 */
{
  int ctmp;
  switch (ctmp = getc (hd))
    {
    case '.':
      read_ESC_HP7550A (hd);
      break;
    case EOF:
      n_unexpected++;
      Eprintf ("\nUnexpected EOF!\n");
      return;
    default:
      read_ESC_RTL (hd, ctmp, hp);
      break;
    }
}



/****************************************************************************/



/**
 **	lines:	Process PA-, PR-, PU-, and  PD- commands
 **/
static void
lines (int relative, FILE * hd)
/**
 ** Examples of anticipated commands:
 **
 **	PA PD0,0,80,50,90,20PU140,30PD150,80;
 **	PU0,0;PD20.53,40.32,30.08,60.2,40,90,;PU100,300;PD120,340...
 **/
{
  HPGL_Pt p;
  int numcmds = 0;

  for (;;)
    {
      if (read_float (&p.x, hd))
	{			/* No number found      */
	  if (numcmds > 0)
	    return;
	  if (pen_down)
	    {			/*simulate dot created by 'real' pen on PD;PU; */
	      p.x=p_last.x+0.01;
	      p.y=p_last.y+0.01;
	      Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
	      Pen_action_to_tmpfile (DRAW_TO, &p_last, scale_flag);
	    }
	  return;
	}

      if (read_float (&p.y, hd))	/* x without y invalid! */
	par_err_exit (2, PA);
      line (relative, p);
      numcmds++;
    }
}


/*
 * line : process a pair of coordinates
 */
void
line (int relative, HPGL_Pt p)
{
  HPGL_Pt pl, porig;
  int outside = 0;
  double x1, y1, x2, y2;

  if (relative)
    {
      p.x += p_last.x;
      p.y += p_last.y;
    }

  porig.x = p.x;
  porig.y = p.y;

  if (iwflag)
    {
      x1 = P1.x + (p_last.x - S1.x) * Q.x;
      y1 = P1.y + (p_last.y - S1.y) * Q.y;
      x2 = P1.x + (p.x - S1.x) * Q.x;
      y2 = P1.y + (p.y - S1.y) * Q.y;
      outside =
	(DtClipLine (C1.x, C1.y, C2.x, C2.y, &x1, &y1, &x2, &y2) ==
	 CLIP_NODRAW);

      if (!outside)
	{
	  p.x = (x2 - P1.x) / Q.x + S1.x;
	  p.y = (y2 - P1.y) / Q.y + S1.y;
	  pl.x = (x1 - P1.x) / Q.x + S1.x;
	  pl.y = (y1 - P1.y) / Q.y + S1.y;
	  if (pl.x != p_last.x || pl.y != p_last.y)
	    Pen_action_to_tmpfile (MOVE_TO, &pl, scale_flag);

	}

    }

  if (polygon_mode && polygon_penup) 
    pen_down = FALSE;

  if (pen_down && !outside)
    {
      if (polygon_mode)
	{
	  polygons[++vertices] = p_last;
	  polygons[++vertices] = p;
/*	      fprintf(stderr,"polygon line1: %f %f - %f %f\n",p_last.x,p_last.y,p.x,p.y);*/
	}
      else
	{
	  Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);
/*	      fprintf(stderr,"std line1: %f %f - %f %f\n",p_last.x,p_last.y,p.x,p.y); */
	}
    }
  else
    {
      if (iwflag)
	{
	  Pen_action_to_tmpfile (MOVE_TO, &porig, scale_flag);
	}
      else
	{
	  Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
	}
    }

  if (polygon_mode && !polygon_penup)
    {
      polygons[++vertices] = p_last;
      polygons[++vertices] = p;
    }

  if (polygon_mode && polygon_penup)
    {
      polygon_penup = FALSE;
      pen_down = TRUE;
    }


  if (symbol_char)
    {
      plot_symbol_char (symbol_char);
      Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
    }
  outside = 0;
  p_last = porig;

}


/**
 **	Arcs, circles and alike
 **/


static void
arc_increment (HPGL_Pt * pcenter, double r, double phi)
{
  HPGL_Pt p;
  int outside = 0;
  p.x = pcenter->x + r * cos (phi);
  p.y = pcenter->y + r * sin (phi);

  if (iwflag)
    {
      if (P1.x + (p.x - S1.x) * Q.x > C2.x
	  || P1.y + (p.y - S1.y) * Q.y > C2.y)
	{
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
	  outside = 1;
	}
      if (P1.x + (p.x - S1.x) * Q.x < C1.x
	  || P1.y + (p.y - S1.y) * Q.y < C1.y)
	{
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
	  outside = 1;
	}
    }

  if (polygon_mode)
    {
      if (polygon_penup)
	polygon_penup = FALSE;
      else if (pen_down && !outside)
	{
	  polygons[++vertices] = p_last;
	  polygons[++vertices] = p;
/*fprintf(stderr,"arcpoint %f %f\n",p.x,p.y);*/

	}
      else if ((p.x != p_last.x) || (p.y != p_last.y))
	{
	  /*polygon_penup=TRUE; */
	  polygons[++vertices] = p_last;
	  polygons[++vertices] = p;
/*fprintf(stderr,"final arcpoint %f %f\n",p.x,p.y);*/
	}
    }
  else
    {
      if (pen_down && !outside)
	Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);
      else if (!outside &&((p.x != p_last.x) || (p.y != p_last.y)))
	Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
    }
  p_last = p;
}

static void
bezier (int relative, FILE * hd)
{
  HPGL_Pt p, p1, p2, p3, polyp;
  int i, outside;
  float t;
/*  double SafeLinePatLen = CurrentLinePatLen;*/

  for (;;)
    {				/* parameter set may contain several bezier curves */
      if (read_float (&p1.x, hd))	/* No number found      */
	return;

      if (read_float (&p1.y, hd))	/* x without y invalid! */
	par_err_exit (2, BZ);

      if (read_float (&p2.x, hd))	/* No number found      */
	return;

      if (read_float (&p2.y, hd))	/* x without y invalid! */
	par_err_exit (2, BZ);

      if (read_float (&p3.x, hd))	/* No endpoint */
	par_err_exit (3, BZ);

      if (read_float (&p3.y, hd))	/* No endpoint */
	par_err_exit (3, BZ);

      if (relative)		/* Transform coordinates  */
	{
	  p1.x = p1.x + p_last.x;
	  p1.y = p1.y + p_last.y;
	  p2.x = p2.x + p_last.x;
	  p2.y = p2.y + p_last.y;
	  p3.x = p3.x + p_last.x;
	  p3.y = p3.y + p_last.y;
	}

/*    
p(t) = t^3*P3 + 3*t^2*(1-t)*P2 + 3*t*(1-t)^2* P1 + (1-t)^3 * P0
*/

      polyp = p_last;
      outside = 0;

      for (i = 0; i < 51; i++)
	{
	  t = (float) i / 50.;
	  p.x =
	    t * t * t * p3.x + 3 * t * t * (1. - t) * p2.x + 3 * t * (1. -
								      t) *
	    (1. - t) * p1.x + (1. - t) * (1. - t) * (1. - t) * p_last.x;
	  p.y =
	    t * t * t * p3.y + 3 * t * t * (1. - t) * p2.y + 3 * t * (1. -
								      t) *
	    (1. - t) * p1.y + (1. - t) * (1. - t) * (1. - t) * p_last.y;

/*fprintf(stderr,"bezier point %f %f\n",p.x,p.y);*/
	  if (iwflag)
	    {
	      if (P1.x + (p.x - S1.x) * Q.x > C2.x
		  || P1.y + (p.y - S1.y) * Q.y > C2.y)
		{
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
		  outside = 1;
		}
	      if (P1.x + (p.x - S1.x) * Q.x < C1.x
		  || P1.y + (p.y - S1.y) * Q.y < C1.y)
		{
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
		  outside = 1;
		}
	    }

	  if (!outside)
	    {
	    if (polygon_mode){
	      polygons[++vertices] = polyp;
	      polygons[++vertices] = p;
	      polyp.x = p.x;
	      polyp.y = p.y;
	    }else{
	     Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);  
	    }
	  }else
	    Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
	  outside = 0;
	}

      p_last.x = p3.x;
      p_last.y = p3.y;

    }
}

static void
tarcs (int relative, FILE * hd)
{
  HPGL_Pt p, p2, p3, center, d;
  float alpha, eps;
  double phi, phi0, r;
  double SafeLinePatLen = CurrentLinePatLen;

  if (read_float (&p2.x, hd))	/* No number found      */
    return;

  if (read_float (&p2.y, hd))	/* x without y invalid! */
    par_err_exit (2, AT);

  if (read_float (&p3.x, hd))	/* No endpoint */
    par_err_exit (3, AT);

  if (read_float (&p3.y, hd))	/* No endpoint */
    par_err_exit (3, AT);

  switch (read_float (&eps, hd))	/* chord angle is optional */
    {
    case 0:
      break;
    case 1:			/* No resolution option */
      eps = 5.0;		/*    so use default!   */
      break;
    case 2:			/* Illegal state        */
      par_err_exit (98, AT);
    case EOF:
      return;
    default:			/* Illegal state        */
      par_err_exit (99, AT);
    }
     if (ct_dist == FALSE)
  eps *= M_PI / 180.0;		/* Deg-to-Rad           */

  d = p_last;

  if (!relative)		/* Transform coordinates  */
    {
      p2.x = p2.x - p_last.x;
      p2.y = p2.y - p_last.y;
      p3.x = p3.x - p_last.x;
      p3.y = p3.y - p_last.y;
    }

/*    
    2*p2.x*h+2*p2.y*k = p2.x^2 + p2.y^2
    
    k= (p2.x^2 + p2.y^2 - 2*p2.x*h) /2*p2.y
     
    2*p3.x*h+2*p3.y*k = p3.x^2 + p3.y^2 
   

2* p3.x*h +2*p3.y * (p2.x^2 + p2.y^2 -2*p2.x*h) / 2*p2.y = (p3.x^2 + p3.y^2)   
2* p3.x*h +2*p3.y * (p2.x^2 + p2.y^2 -2*p2.x*h)  = (p3.x^2 + p3.y^2) *2*p2.y  

2*p3.x*h + 2*p3.y*p2.x^2 + 2*p3.y*p2.y^2 - 4*p2.x*p3.y*h =...
(2*p3.x-4*p2.y*p3.y)*h  + 2*p3.y*p2.x^2 + 2*p3.y*p2.y^2 = ...
h = ( 2*p2.y*(p2.x^2 + p2.y^2) -2*p3.y*p2.x^2 - 2*p3.y*p2.y^2 )  / 2*p3.x-4*p2.x*p3.y
*/
  center.x =
    (2. * p2.y * (p3.x * p3.x + p3.y * p3.y) - 2. * p3.y * p2.x * p2.x -
     2. * p3.y * p2.y * p2.y) / (2. * p3.x - 4. * p2.x * p3.y);
  center.y = (p2.x * p2.x + p2.y * p2.y - 2. * p2.x * center.x) / (2. * p2.y);

  r = sqrt (center.x * center.x + center.y * center.y);

  if (ct_dist == TRUE)
	eps = 2. * acos((r-eps)/r);
  center.x = center.x + p_last.x;
  center.y = center.y + p_last.y;


  d.x = p_last.x - center.x;
  d.y = p_last.y - center.y;

  phi0 = atan2 (d.y, d.x);

  d.x = p3.x + p_last.x - center.x;
  d.y = p3.y + p_last.y - center.y;

  alpha = 2. * atan2 (d.y, d.x);
/*
fprintf(stderr,"AT: P1 at %f %f , P2 %f %f, P3 %f %f, center %f %f radius %f\n",
p_last.x,p_last.y,p2.x+p_last.x,p2.y+p_last.y,p3.x+p_last.x,p3.y+p_last.y,
center.x,center.y,r);
*/
  if (CurrentLineType == LT_adaptive)	/* Adaptive patterns:   */
    {
      p.x = r * cos (eps);	/* A chord segment      */
      p.y = r * sin (eps);
      if (scale_flag)
	User_to_Plotter_coord (&p, &p);

      /*      Pattern length = chord length           */
      CurrentLinePatLen = HYPOT (p.x, p.y);
    }

  if (alpha > 0.0)
    {
      for (phi = phi0 + MIN (eps, alpha); phi < phi0 + alpha; phi += eps)
	arc_increment (&center, r, phi);
      arc_increment (&center, r, phi0 + alpha);	/* to endpoint */
    }
  else
    {
      for (phi = phi0 - MIN (eps, -alpha); phi > phi0 + alpha; phi -= eps)
	arc_increment (&center, r, phi);
      arc_increment (&center, r, phi0 + alpha);	/* to endpoint */
    }

  CurrentLinePatLen = SafeLinePatLen;	/* Restore */

  p_last.x = p_last.x + p3.x;
  p_last.y = p_last.y + p3.y;

}

static void
arcs (int relative, FILE * hd)
{
  HPGL_Pt p, d, center;
  float alpha, eps;
  double phi, phi0, r;
  double SafeLinePatLen = CurrentLinePatLen;

  if (read_float (&p.x, hd))	/* No number found      */
    return;

  if (read_float (&p.y, hd))	/* x without y invalid! */
    par_err_exit (2, AA);

  if (read_float (&alpha, hd))	/* Invalid without angle */
    par_err_exit (3, AA);
  else
    alpha *= M_PI / 180.0;	/* Deg-to-Rad           */

  switch (read_float (&eps, hd))
    {
    case 0:
      break;
    case 1:			/* No resolution option */
      eps = 5.0;		/*    so use default!   */
      break;
    case 2:			/* Illegal state        */
      par_err_exit (98, AA);
    case EOF:
      return;
    default:			/* Illegal state        */
      par_err_exit (99, AA);
    }

	if (ct_dist == FALSE)
  	eps *= M_PI / 180.0;		/* Deg-to-Rad           */


  if (relative)			/* Process coordinates  */
    {
      d = p;			/* Difference vector    */
      center.x = d.x + p_last.x;
      center.y = d.y + p_last.y;
    }
  else
    {
      d.x = p.x - p_last.x;
      d.y = p.y - p_last.y;
      center.x = p.x;
      center.y = p.y;
    }

  if (((r = sqrt (d.x * d.x + d.y * d.y)) == 0.0) || (alpha == 0.0))
    return;			/* Zero radius or zero arc angle given  */

  if (ct_dist == TRUE) eps = 2.* acos((r-eps/r));

  phi0 = atan2 (-d.y, -d.x);

  if (CurrentLineType == LT_adaptive)	/* Adaptive patterns:   */
    {
      p.x = r * cos (eps);	/* A chord segment      */
      p.y = r * sin (eps);
      if (scale_flag)
	User_to_Plotter_coord (&p, &p);

      /*      Pattern length = chord length           */
      CurrentLinePatLen = HYPOT (p.x, p.y);
    }

  if (alpha > 0.0)
    {
      for (phi = phi0 + MIN (eps, alpha); phi < phi0 + alpha; phi += eps)
	arc_increment (&center, r, phi);
      arc_increment (&center, r, phi0 + alpha);	/* to endpoint */
    }
  else
    {
      for (phi = phi0 - MIN (eps, -alpha); phi > phi0 + alpha; phi -= eps)
	arc_increment (&center, r, phi);
      arc_increment (&center, r, phi0 + alpha);	/* to endpoint */
    }

  CurrentLinePatLen = SafeLinePatLen;	/* Restore */
}

static void
fwedges (FILE * hd, float cur_pensize)	/*derived from circles */
{
  HPGL_Pt p, center, wpolygon[MAXPOLY];
  float eps, r, start, sweep;
  double phi;
  double SafeLinePatLen = CurrentLinePatLen;
  int outside = 0;
  int i;

  if (read_float (&r, hd))	/* No radius found      */
    return;
  if (read_float (&start, hd))	/* No start angle found */
    return;

  if (read_float (&sweep, hd))	/* No sweep angle found */
    return;

  switch (read_float (&eps, hd))	/* chord angle */
    {
    case 0:
      break;
    case 1:			/* No resolution option */
      eps = 5.0;		/*    so use default!   */
      break;
    case 2:			/* Illegal state        */
      par_err_exit (98, EW);
    case EOF:
      return;
    default:			/* Illegal state        */
      par_err_exit (99, EW);
    }

	if (ct_dist == TRUE)
	eps = 2.*acos((r-eps)/r);
	else
 	 eps *= M_PI / 180.0;		/* Deg-to-Rad           */
  start *= M_PI / 180.0;	/* Deg-to-Rad           */
  sweep *= M_PI / 180.0;	/* Deg-to-Rad           */


  center = p_last;		/* reference point is last position */
  wpolygon[0] = p_last;
  if (r == 0.0)			/* Zero radius given    */
    return;

  wpolygon[1].x = center.x + r * cos (start);
  wpolygon[1].y = center.y + r * sin (start);

  if (CurrentLineType == LT_adaptive)	/* Adaptive patterns    */
    {
      p.x = r * cos (eps);	/* A chord segment      */
      p.y = r * sin (eps);
      if (scale_flag)
	User_to_Plotter_coord (&p, &p);

      /*      Pattern length = chord length           */
      CurrentLinePatLen = HYPOT (p.x, p.y);
    }
  i = 1;
  for (phi = eps; phi <= sweep; phi += eps)
    {
      p.x = center.x + r * cos (start + phi);
      p.y = center.y + r * sin (start + phi);
      if (iwflag)
	{
	  if (P1.x + (p.x - S1.x) * Q.x > C2.x
	      || P1.y + (p.y - S1.y) * Q.y > C2.y)
	    {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
	      outside = 1;
	    }
	  if (P1.x + (p.x - S1.x) * Q.x < C1.x
	      || P1.y + (p.y - S1.y) * Q.y < C1.y)
	    {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
	      outside = 1;
	    }
	}
      if (!outside)
	{
	  i++;
	  wpolygon[i] = wpolygon[i - 1];
	  i++;
	  wpolygon[i] = p;
	}
      outside = 0;
    }
  i++;
  wpolygon[i] = p;
  i++;
  wpolygon[i] = center;
  if (hatchspace == 0.)
    hatchspace = cur_pensize;
  if (filltype < 3 && thickness > 0.)
    hatchspace = thickness;
  fill (wpolygon, i, anchor, P2, scale_flag, filltype, hatchspace, hatchangle);

  CurrentLinePatLen = SafeLinePatLen;	/* Restore */

}



static void
circles (FILE * hd)
{
  HPGL_Pt p, center, polyp;
  float eps, r;
  double phi;
  double SafeLinePatLen = CurrentLinePatLen;
  int outside = 0;

  if (read_float (&r, hd))	/* No radius found      */
    return;

  switch (read_float (&eps, hd))
    {
    case 0:
      break;
    case 1:			/* No resolution option */
      eps = 5.0;		/*    so use default!   */
      break;
    case 2:			/* Illegal state        */
      par_err_exit (98, CI);
    case EOF:
      return;
    default:			/* Illegal state        */
      par_err_exit (99, CI);
    }

	if (ct_dist == TRUE)
	eps = 2.*acos((r-eps)/r);
	else
	  eps *= M_PI / 180.0;		/* Deg-to-Rad           */


  center = p_last;

  if (r == 0.0)			/* Zero radius given    */
    return;

  p.x = center.x + r;
  p.y = center.y;
  Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
  if (polygon_mode)
    {
      polyp.x = p.x;
      polyp.y = p.y;
    }
  if (CurrentLineType == LT_adaptive)	/* Adaptive patterns    */
    {
      p.x = r * cos (eps);	/* A chord segment      */
      p.y = r * sin (eps);
      if (scale_flag)
	User_to_Plotter_coord (&p, &p);

      /*      Pattern length = chord length           */
      CurrentLinePatLen = HYPOT (p.x, p.y);
    }

  for (phi = eps; phi < 2.0 * M_PI; phi += eps)
    {
      p.x = center.x + r * cos (phi);
      p.y = center.y + r * sin (phi);
      if (iwflag)
	{
	  if (P1.x + (p.x - S1.x) * Q.x > C2.x
	      || P1.y + (p.y - S1.y) * Q.y > C2.y)
	    {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
	      outside = 1;
	    }
	  if (P1.x + (p.x - S1.x) * Q.x < C1.x
	      || P1.y + (p.y - S1.y) * Q.y < C1.y)
	    {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
	      outside = 1;
	    }
	}

      if (!outside)
	{
	  if (polygon_mode)
	    {
	      polygons[++vertices] = polyp;
	      polygons[++vertices] = p;
	      polyp.x = p.x;
	      polyp.y = p.y;
	    }
	  else
	    {
	      Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);
	    }
	}
      else
	Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
      outside = 0;
    }
  p.x = center.x + r;		/* Close circle at r * (1, 0)   */
  p.y = center.y;
  if (polygon_mode)
    {
      polygons[++vertices] = polyp;
      polygons[++vertices] = p;
    }
  else
    Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);

  Pen_action_to_tmpfile (MOVE_TO, &center, scale_flag);

  CurrentLinePatLen = SafeLinePatLen;	/* Restore */
}

static void
wedges (FILE * hd)		/*derived from circles */
{
  HPGL_Pt p, center;
  float eps, r, start, sweep;
  double phi;
  double SafeLinePatLen = CurrentLinePatLen;
  int outside = 0;

  if (read_float (&r, hd))	/* No radius found      */
    return;

  if (read_float (&start, hd))	/* No start angle found */
    return;

  if (read_float (&sweep, hd))	/* No sweep angle found */
    return;

  switch (read_float (&eps, hd))	/* chord angle */
    {
    case 0:
      break;
    case 1:			/* No resolution option */
      eps = 5.0;		/*    so use default!   */
      break;
    case 2:			/* Illegal state        */
      par_err_exit (98, EW);
    case EOF:
      return;
    default:			/* Illegal state        */
      par_err_exit (99, EW);
    }

	if (ct_dist == TRUE)
	eps = 2.*acos((r-eps)/r);
	else
  	eps *= M_PI / 180.0;		/* Deg-to-Rad           */
  start *= M_PI / 180.0;	/* Deg-to-Rad           */
  sweep *= M_PI / 180.0;	/* Deg-to-Rad           */


  center = p_last;		/* reference point is last position */

  if (r == 0.0)			/* Zero radius given    */
    return;

  p.x = center.x + r * cos (start);
  p.y = center.y + r * sin (start);
  Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);

  if (CurrentLineType == LT_adaptive)	/* Adaptive patterns    */
    {
      p.x = r * cos (eps);	/* A chord segment      */
      p.y = r * sin (eps);
      if (scale_flag)
	User_to_Plotter_coord (&p, &p);

      /*      Pattern length = chord length           */
      CurrentLinePatLen = HYPOT (p.x, p.y);
    }

  for (phi = eps; phi <= sweep; phi += eps)
    {
      p.x = center.x + r * cos (start + phi);
      p.y = center.y + r * sin (start + phi);
      if (iwflag)
	{
	  if (P1.x + (p.x - S1.x) * Q.x > C2.x
	      || P1.y + (p.y - S1.y) * Q.y > C2.y)
	    {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
	      outside = 1;
	    }
	  if (P1.x + (p.x - S1.x) * Q.x < C1.x
	      || P1.y + (p.y - S1.y) * Q.y < C1.y)
	    {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
	      outside = 1;
	    }
	}
      if (!outside)
	Pen_action_to_tmpfile (DRAW_TO, &p, scale_flag);
      else
	Pen_action_to_tmpfile (MOVE_TO, &p, scale_flag);
      outside = 0;
    }

  Pen_action_to_tmpfile (DRAW_TO, &center, scale_flag);

  CurrentLinePatLen = SafeLinePatLen;	/* Restore */
}


/**
 ** Rectangles --  by Th. Hiller (hiller@tu-harburg.d400.de)
 **/

static void
rect (int relative, int filled, float cur_pensize, FILE * hd)
{
  HPGL_Pt p;
  HPGL_Pt p1;

  for (;;)
    {
      if (read_float (&p.x, hd))	/* No number found */
	return;

      if (read_float (&p.y, hd))	/* x without y invalid! */
	par_err_exit (2, EA);


      if (relative)		/* Process coordinates */
	{
	  p.x += p_last.x;
	  p.y += p_last.y;
	}
      if (!filled)
	{
	  p1.x = p_last.x;
	  p1.y = p.y;
	  Pen_action_to_tmpfile (DRAW_TO, &p1, scale_flag);
	  p1.x = p.x;
	  p1.y = p.y;
	  Pen_action_to_tmpfile (DRAW_TO, &p1, scale_flag);
	  p1.x = p.x;
	  p1.y = p_last.y;
	  Pen_action_to_tmpfile (DRAW_TO, &p1, scale_flag);
	  p1.x = p_last.x;
	  p1.y = p_last.y;
	  Pen_action_to_tmpfile (DRAW_TO, &p1, scale_flag);
	}
      else
	{
	  vertices = 0;
	  polygons[vertices] = p_last;
	  p1.x = p_last.x;
	  p1.y = p.y;
	  polygons[++vertices] = p1;
	  polygons[++vertices] = p1;
	  polygons[++vertices] = p;
	  polygons[++vertices] = p;
	  p1.x = p.x;
	  p1.y = p_last.y;
	  polygons[++vertices] = p1;
	  polygons[++vertices] = p1;
	  polygons[++vertices] = p_last;
	  if (hatchspace == 0.)
	    hatchspace = cur_pensize;
	  if (filltype < 3 && thickness > 0.)
	    hatchspace = thickness;
	  fill (polygons, vertices, anchor, P2, scale_flag, filltype, hatchspace,
		hatchangle);
	}
      Pen_action_to_tmpfile (MOVE_TO, &p_last, scale_flag);
    }
}




static void
ax_ticks (int mode)
{
  HPGL_Pt p0, p1, p2;
  LineType  SafeLineType = CurrentLineType;

  p0 = p1 = p2 = p_last;
/**
 ** According to the HP-GL manual,
 ** XT & YT are not affected by LT
 **/
  CurrentLineType = LT_solid;

  if (mode == 0)		/* X tick       */
    {
      if (scale_flag)
	{
	  p1.y -= neg_ticklen * (P2.y - P1.y) / Q.y;
	  p2.y += pos_ticklen * (P2.y - P1.y) / Q.y;
	}
      else
	{
	  p1.y -= neg_ticklen * (P2.y - P1.y);
	  p2.y += pos_ticklen * (P2.y - P1.y);
	}
    }
  else
    /* Y tick */
    {
      if (scale_flag)
	{
	  p1.x -= neg_ticklen * (P2.x - P1.x) / Q.x;
	  p2.x += pos_ticklen * (P2.x - P1.x) / Q.x;
	}
      else
	{
	  p1.x -= neg_ticklen * (P2.x - P1.x);
	  p2.x += pos_ticklen * (P2.x - P1.x);
	}
    }

  Pen_action_to_tmpfile (MOVE_TO, &p1, scale_flag);
  Pen_action_to_tmpfile (DRAW_TO, &p2, scale_flag);
  Pen_action_to_tmpfile (MOVE_TO, &p0, scale_flag);

  CurrentLineType = SafeLineType;
}



/**
 **	Process a single HPGL command
 **/

static void
read_HPGL_cmd (GEN_PAR * pg, short cmd, FILE * hd)
{
  short old_pen;
  HPGL_Pt p1={0.,0.}, p2={0.,0.};
  float ftmp;
  float csfont;
  int mypen, myred, mygreen, myblue, i;
  float mywidth, myheight;
  char tmpstr[1024];
  char SafeTerm;
/**
 ** Each command consists of 2 characters. We unite them here to a single int
 ** to allow for easy processing within a big switch statement:
 **/

  switch (cmd & 0xDFDF)		/* & forces to upper case       */
    {
  /**
   ** Commands appear in alphabetical order within each topic group
   ** except for command synonyms.
   **/
    case AA:			/* Arc Absolute                 */
      arcs (FALSE, hd);
      tp->CR_point = HP_pos;
      break;
    case AR:			/* Arc Relative                 */
      arcs (TRUE, hd);
      tp->CR_point = HP_pos;
      break;
    case AT:			/* Arc Absolute, through Three points */
      tarcs (FALSE, hd);
      break;
    case BR:			/* cubic bezier curve, relative control points */
      bezier (TRUE, hd);
      break;
    case BZ:			/* cubic bezier curve, absolute control points */
      bezier (FALSE, hd);
      break;
    case AC:			/* anchor corner of fill patterns */
    	if (read_float(&ftmp, hd)){ /* just AC - default 0,0 */
    	  anchor.x=0.;
    	  anchor.y=0.;
    	  break;
    	  }else{
    	anchor.x=ftmp;
    	}
    	if (read_float(&ftmp,hd))
    	  anchor.y=0.;
    	  else
    	  anchor.y=ftmp;
    	break;  
    case AD:
	if (read_float(&ftmp, hd)) /* just AD - defaults */
	tp->altfont = 0;
	else {
	switch((int)ftmp){
	case 1: /* charset */
	if (read_float(&csfont, hd))
	 par_err_exit (2, cmd);
	else 
	 tp->altfont=(int)csfont;
	break;
	case 2: /* fixed or variable spacing */
	if (read_float(&csfont, hd))
         par_err_exit (2, cmd);
	else
	if ((int)csfont==1 && !silent_mode) 
		fprintf(stderr,"only fixed fonts available\n");
	break;
	case 3: /* font pitch */
        case 4: /* font height */
	case 5: /* posture */
	if (read_float(&csfont, hd))
         par_err_exit (2, cmd);
        else
	if (!silent_mode) fprintf(stderr,"pitch/height/posture unsupported\n");	
	break;
	default:
	par_err_exit(1,cmd);
	}
	}
	break;
    case CA:			/* Alternate character set      */
      if (read_float (&csfont, hd))	/* just CA;    */
	tp->altfont = 0;
      else
	tp->altfont = csfont;
      break;
    case CI:			/* Circle                       */
      circles (hd);
      break;
    case CO:			/* Comment                      */
      SafeTerm = StrTerm;
      StrTerm = ';';
      read_string (tmpstr, hd);
      StrTerm = SafeTerm;
	if (strlen(tmpstr)>0) tmpstr[strlen(tmpstr)-1]='\0';
      if (!silent_mode)
	printf ("\n%s\n", tmpstr);
      break;
    case CS:			/*character set selection       */
      if (read_float (&csfont, hd))	/* just CS;     */
	tp->font = 0;
      else
	tp->font = csfont;
      tp->stdfont = csfont;
      break;
    case CT:			/* chord tolerance */
	if (read_float (&ftmp,hd) || ftmp != 1.)
		ct_dist = FALSE;
	else
		ct_dist = TRUE;
      break;
    case EP:			/* edge polygon */
      for (i = 0; i < vertices; i = i + 2)
	{			/*for all polygon edges */
	  p1.x = polygons[i].x;
	  p1.y = polygons[i].y;
	  Pen_action_to_tmpfile (MOVE_TO, &p1, scale_flag);
	  p1.x = polygons[i + 1].x;
	  p1.y = polygons[i + 1].y;
	  Pen_action_to_tmpfile (DRAW_TO, &p1, scale_flag);
	}
      Pen_action_to_tmpfile (MOVE_TO, &p_last, scale_flag);
      break;

    case EW:			/* Edge Wedge                   */
      wedges (hd);
      break;

    case EC:
      /*  printf("cut paper\n"); */
      break;

    case FP:			/* fill polygon */
      if (pg->nofill)
	{
	  if (!silent_mode)
	    fprintf (stderr, "FP : suppressed\n");
	  break;
	}
      if (hatchspace == 0.)
	hatchspace = pt.width[pen] / 10.;
      if (filltype < 3 && thickness > 0.)
	hatchspace = thickness;
      fill (polygons, vertices, anchor, P2, scale_flag, filltype, hatchspace,
	    hatchangle);
      Pen_action_to_tmpfile (MOVE_TO, &p_last, scale_flag);
      break;
    case FT:			/* Fill Type */
      if (read_float (&ftmp, hd))
	{			/* just FT -> FT=1 */
	  filltype = 1;
	  break;
	}
      else
	{
	  filltype = ftmp;
	}
      if (filltype < 3)
	break;

      if (filltype > 4)
	{
	  if (!silent_mode)
	    fprintf (stderr,
		     "No support for user-defined fill types, using type 1 instead\n");
	  filltype = 1;
	  break;
	}

      if (read_float (&ftmp, hd)){
      	hatchspace = saved_hatchspace[filltype-3];
	if (hatchspace == 0.) hatchspace = 0.01*Diag_P1_P2;
        hatchangle = saved_hatchangle[filltype-3];
	break;
	}
      else
        {
	if (ftmp<=0.) ftmp=0.01*Diag_P1_P2;
	hatchspace = ftmp;
	saved_hatchspace[filltype-3] = hatchspace;
	}
      if (read_float (&ftmp, hd)){
        hatchangle = saved_hatchangle[filltype-3];
	break;
	}
      else
        {
	hatchangle = ftmp;
	saved_hatchangle[filltype-3] = hatchangle;
	}
      break;
    case NP:			/* Number of Pens                    */
      if (read_float (&ftmp, hd) || ftmp > NUMPENS)	/* invalid or missing */
	break;
      else
	{
	  pg->maxpens = ftmp;
	  if (!silent_mode)
	    fprintf (stderr, "NP: %d pens requested\n", pg->maxpens);
	}
      break;
    case NR:			/*Not ready - pause plotter (noop) */
      if (read_float (&ftmp, hd))
	break;
      break;
    case PA:			/* Plot Absolute                */
      lines (plot_rel = FALSE, hd);
      tp->CR_point = HP_pos;
      break;
    case PC:			/* Pen Color                    */
      if (read_float (&ftmp, hd) || fixedcolor || ftmp > pg->maxpens )
	{			/* invalid or missing */
	  break;
	}
      else
	{
	  mypen = ftmp;
	  if (read_float (&ftmp, hd))	/* no red component  */
	    myred = 0;
	  else
	    myred = ftmp;
	  if (read_float (&ftmp, hd))	/* no green component  */
	    mygreen = 0;
	  else
	    mygreen = ftmp;
	  if (read_float (&ftmp, hd))	/* no blue component  */
	    myblue = 0;
	  else
	    myblue = ftmp;
	  pg->is_color = TRUE;
	  PlotCmd_to_tmpfile (DEF_PC);
	  Pen_Color_to_tmpfile (mypen, myred, mygreen, myblue);
/*          set_color_rgb(mypen,myred,mygreen,myblue);
	  pt.color[mypen] = mypen;
*/
	  break;
	}
    case PD:			/* Pen  Down                    */
      pen_down = TRUE;
      lines (plot_rel, hd);
      tp->CR_point = HP_pos;
      break;
    case PE:
      read_PE (pg, hd);
      tp->CR_point = HP_pos;
      break;
    case PM:
      if (read_float (&ftmp, hd) || ftmp == 0)
	{			/* no parameters or PM0 */
	  polygon_mode = TRUE;
	  polygon_penup = FALSE;
	  saved_penstate = pen_down;
	  vertices = -1;
	  break;
	}
      if (ftmp == 1)
	{
	  if(vertices>0)polygon_penup = TRUE;
	  pen_down = FALSE;
	  break;
	}
      if (ftmp == 2)
	{
	  polygon_mode = FALSE;
	  pen_down = saved_penstate;	  
	}
      break;
    case PR:			/* Plot Relative                */
      lines (plot_rel = TRUE, hd);
      tp->CR_point = HP_pos;
      break;
    case PS:
      if (read_float (&ftmp, hd))
	{			/* no parameters */
	  break;
	}
      else
	{
	  myheight = ftmp;
	}
      if (read_float (&ftmp, hd))
	{			/* no parameters */
	  mywidth = P2.y;
	}
      else
	{
	  mywidth = ftmp;
	  if (mywidth > myheight)
	    {
	      mywidth = myheight;
	      myheight = ftmp;
	    }
	}
      if (pg->no_ps == TRUE) {
      	if (!silent_mode)
      		Eprintf("PS: suppressed\n");
      	break;
      }			
      ps_flag = 1;
/*      fprintf(stderr,"min,max vor PS: %f %f %f %f\n",xmin,ymin,xmax,ymax);*/
      M.x = myheight;
      M.y = mywidth;
      p1.x = 0;
      p1.y = 0;

      if (scale_flag)		/* Rescaling    */
	User_to_Plotter_coord (&p1, &p2);
      else
	p2 = p1;		/* Local copy   */


      if (rotate_flag)		/* hp2xx-specific global rotation       */
	{
	  ftmp = rot_cos * p2.x - rot_sin * p2.y;
	  p2.y = rot_sin * p2.x + rot_cos * p2.y;
	  p2.x = ftmp;
	}
      xmin = MIN (p2.x, xmin);
      ymin = MIN (p2.y, ymin);
      xmax = MAX (p2.x, xmax);
      ymax = MAX (p2.y, ymax);
      p1.x = myheight;
      p1.y = mywidth;
#if 1
/* add the following - to get the correct linetype scale etc */
      P1.x = 0;
      P1.y = 0;
      P2.x = myheight;
      P2.y = mywidth;
      Diag_P1_P2 = HYPOT (P2.x - P1.x, P2.y - P1.y);
      CurrentLinePatLen = 0.04 * Diag_P1_P2;
      S1 = P1;
      S2 = P2;
/* ajb */
#endif
#if 1
      if (scale_flag)		/* Rescaling    */
	User_to_Plotter_coord (&p1, &p2);
      else
	p2 = p1;		/* Local copy   */
#endif
#if 1
      if (rotate_flag)		/* hp2xx-specific global rotation       */
	{
	  ftmp = rot_cos * p2.x - rot_sin * p2.y;
	  p2.y = rot_sin * p2.x + rot_cos * p2.y;
	  p2.x = ftmp;
	}
#endif
#if 1
      xmin = MIN (p2.x, xmin);
      ymin = MIN (p2.y, ymin);
      xmax = MAX (p2.x, xmax);
      ymax = MAX (p2.y, ymax);
#endif
      break;
    case PT:			/* Pen thickness (for solid fills - current pen only */
      if (read_float (&ftmp, hd))
	{			/* no parameters */
	  thickness = 0.3;
	  break;
	}
      else
	{
	  if (ftmp >= 0.1 && ftmp <= 5.)
	    thickness = ftmp;
	}
    case PU:			/* Pen  Up                      */
      pen_down = FALSE;
      lines (plot_rel, hd);
      tp->CR_point = HP_pos;
      break;
    case PW:			/* Pen Width                    */
      if (fixedwidth)
	{
	  if (!silent_mode)
	    fprintf (stderr, "PW: ignored (hardware mode)\n");
	  break;
	}
      if (read_float (&ftmp, hd))
	{			/* no parameters -> set defaults */
	  mywidth = 0.35;
	  if (wu_relative)
	    mywidth = Diag_P1_P2 / 1000.;
	  if (mywidth < 0.1)
	    mywidth = 0.1;
	  PlotCmd_to_tmpfile (DEF_PW);
	  Pen_Width_to_tmpfile (0, (int) (mywidth * 10.));
/*	 
          fprintf(stderr,"PW: defaulting to 0.35 for all pens\n");
*/
	  break;
	}
      else
	{
	  mywidth = ftmp;	/* first or only parameter is width */
	  if (wu_relative)
	    mywidth = Diag_P1_P2 * ftmp / 1000.;
	  if (mywidth < 0.1)
	    mywidth = 0.1;
	}

      if (read_float (&ftmp, hd))
	{			/* width only, applies to all pens */
	  PlotCmd_to_tmpfile (DEF_PW);
	  Pen_Width_to_tmpfile (0, (int) (mywidth * 10.));
	  if (pg->maxpensize < mywidth * 10.)
	    pg->maxpensize = mywidth * 10.;
/*	 
          fprintf(stderr,"PW: defaulting to %d for all pens\n",(int) (mywidth*10.));
*/
	}
      else
	{			/* second parameter is pen */
	  PlotCmd_to_tmpfile (DEF_PW);
	  Pen_Width_to_tmpfile (ftmp, (int) (mywidth * 10.));
	  if (ftmp <= pg->maxpens)
	    {
	      if (pg->maxpensize < mywidth * 10.)
		pg->maxpensize = mywidth * 10.;
	    }
/*
         fprintf(stderr,"pen%d, size now %d\n",(int) ftmp,(int) (mywidth * 10.));
*/
	}
      break;
    case TL:			/* Tick Length                  */
      if (read_float (&ftmp, hd))	/* No number found  */
	{
	  neg_ticklen = pos_ticklen = 0.005;
	  return;
	}
      else
	pos_ticklen = ftmp / 100.0;

      if (read_float (&ftmp, hd))	/* pos, but not neg */
	{
	  neg_ticklen = 0.0;
	  return;
	}
      else
	neg_ticklen = ftmp / 100.0;
      break;
    case WG:			/* Filled Wedge                 */
      fwedges (hd, pt.width[pen] / 10.);
      break;
    case WU:			/* pen Width Unit is relative  */
      if (read_float (&ftmp, hd) || ftmp == 0.)	/* Zero or no number  */
	wu_relative = FALSE;
      else
	wu_relative = TRUE;
      break;
    case XT:			/* X Tick                       */
      ax_ticks (0);
      break;
    case YT:			/* Y Tick                       */
      ax_ticks (1);
      break;


    case IP:			/* Input reference Points P1,P2 */
      tp->width /= (P2.x - P1.x);
      tp->height /= (P2.y - P1.y);
      if (read_float (&p1.x, hd))	/* No number found  */
	{
	  P1.x = P1X_default;
	  P1.y = P1Y_default;
	  P2.x = P2X_default;
	  P2.y = P2Y_default;
	  goto IP_Exit;
	}
      if (read_float (&p1.y, hd))	/* x without y! */
	par_err_exit (2, cmd);

      if (read_float (&p2.x, hd))	/* No number found  */
	{
	  P2.x += p1.x - P1.x;
	  P2.y += p1.y - P1.y;
	  P1 = p1;
	  goto IP_Exit;
	}
      if (read_float (&p2.y, hd))	/* x without y! */
	par_err_exit (4, cmd);

      P1 = p1;
      P2 = p2;


    IP_Exit:
/*L+M      if (rotate_flag!=0){
      ftmp=P2.x;
      P2.x=P1.x;
      P1.x=ftmp;
      ftmp=P2.y;
      P2.y=P1.y;
      P1.y=ftmp;
      }
*/      
      Q.x = (P2.x - P1.x) / (S2.x - S1.x);
      Q.y = (P2.y - P1.y) / (S2.y - S1.y);
      Diag_P1_P2 = HYPOT (P2.x - P1.x, P2.y - P1.y);
      CurrentLinePatLen = 0.04 * Diag_P1_P2;
      tp->width *= (P2.x - P1.x);
      tp->height *= (P2.y - P1.y);
      adjust_text_par ();
      return;

    case IW:
      iwflag = 1;
      if (read_float (&C1.x, hd))	/* No number found  */
	{
	  if (scale_flag)
	    {
	      if (rotate_flag)
		{
		  C1.x = S1.y;
		  C1.y = S1.x;
		  C2.x = S2.y;
		  C2.y = S2.x;
		}
	      else
		{
		  C1.x = S1.x;
		  C1.y = S1.y;
		  C2.x = S2.x;
		  C2.y = S2.y;
		}
	    }
	  else
	    {
		if (rotate_flag) {
		C1.x= P1.y;
		C1.y= P1.x;
		C2.x= P2.y;
		C2.y= P2.x;
		}else{	
	      C1.x = P1.x;
	      C1.y = P1.y;
	      C2.x = P2.x;
	      C2.y = P2.y;
	      } 
/*fprintf (stderr," clip limits (%f,%f)(%f,%f)\n",C1.x,C1.y,C2.x,C2.y);*/
	    }
	}
      else
	{
	  if (read_float (&C1.y, hd))	/* x without y! */
	    par_err_exit (2, cmd);
	  if (read_float (&C2.x, hd))	/* No number found  */
	    par_err_exit (3, cmd);
	  if (read_float (&C2.y, hd))	/* x without y! */
	    par_err_exit (4, cmd);
	}
	
#if 1
	if ( C1.x > C2.x) {
		ftmp=C1.x;
		C1.x=C2.x;
		C2.x=ftmp;
		}
	if ( C1.y > C2.y) {	
		ftmp=C1.y;
		C1.y=C2.y;
		C2.y=ftmp;
		}	
#endif		
/*      if (scale_flag)****/
	{
	  User_to_Plotter_coord (&C1, &C1);
	  User_to_Plotter_coord (&C2, &C2);
	}

      break;

    case OP:			/* Output reference Points P1,P2 */
      if (!silent_mode)
	{
	  Eprintf ("\nP1 = (%g, %g)\n", P1.x, P1.y);
	  Eprintf ("P2 = (%g, %g)\n", P2.x, P2.y);
	}
      break;

    case AF:
    case AH:
    case PG:			/* new PaGe                     */
      /* record ON happens only once! */
      page_number++;
      record_off = (first_page > page_number)
	|| ((last_page < page_number) && (last_page > 0));
      break;

    case EA:			/* Edge Rectangle absolute */
      rect (plot_rel = FALSE, 0, pt.width[pen] / 10., hd);
      tp->CR_point = HP_pos;
      break;

    case ER:			/* Edge Rectangle relative */
      rect (TRUE, 0, 0., hd);
      tp->CR_point = HP_pos;
      break;

    case RA:			/* Fill Rectangle absolute */
      rect (plot_rel = FALSE, 1, pt.width[pen] / 10., hd);
      tp->CR_point = HP_pos;
      break;

    case RR:			/* Fill Rectangle relative */
      rect (plot_rel = TRUE, 1, pt.width[pen] / 10., hd);
      tp->CR_point = HP_pos;
      break;

    case RT:			/* Relative arc, through Three points */
      tarcs (TRUE, hd);
      break;

    case LT:			/* Line Type:                   */
      if (read_float (&p1.x, hd))	/* just LT;     */
	CurrentLineType = LT_solid;
      else
	{
	  if ((((int) p1.x) >= LT_MIN) && (((int) p1.x) < LT_ZERO))
	    CurrentLineType = LT_adaptive;
	  else if (((int) p1.x) == LT_ZERO)
	    CurrentLineType = LT_plot_at;
	  else if ((((int) p1.x) > LT_ZERO) && (((int) p1.x) <= LT_MAX))
	    CurrentLineType = LT_fixed;
	  else
	    {
	      Eprintf ("Illegal line type:\t%d\n", (int) p1.x);
	      CurrentLineType = LT_solid;	/* set to something sane */
	    }
	  CurrentLinePattern = p1.x;

	  if (!read_float (&p1.y, hd))
	    {			/* optional pattern length?     */
	      if (p1.y <= 0.0)
		Eprintf ("Illegal pattern length:\t%g\n", p1.y);
	      else
		{
		  Diag_P1_P2 = HYPOT (P2.x - P1.x, P2.y - P1.y);

		  if (!read_float (&ftmp, hd))
		    {
		      if (ftmp == 1.0)
			{
			  CurrentLinePatLen = p1.y * 40;	/* absolute */
			}
		      else
			{
			  CurrentLinePatLen = Diag_P1_P2 * p1.y / 100.0;	/* relative */
			}
		    }
		  else
		    {
		      CurrentLinePatLen = Diag_P1_P2 * p1.y / 100.0;	/* relative */
		    }
		}
	    }
	}

      break;

    case SC:			/* Input Scale Points S1,S2     */
      User_to_Plotter_coord (&p_last, &p_last);
      if (read_float (&S1.x, hd))	/* No number found  */
	{
	  S1.x = P1X_default;
	  S1.y = P1Y_default;
	  S2.x = P2X_default;
	  S2.y = P2Y_default;
	  scale_flag = FALSE;
	  Q.x = Q.y = 1.0;
	  return;
	}
      if (read_float (&S2.x, hd))	/* x without y! */
	par_err_exit (2, cmd);
      if (read_float (&S1.y, hd))	/* No number found  */
	par_err_exit (3, cmd);
      if (read_float (&S2.y, hd))	/* x without y! */
	par_err_exit (4, cmd);

      if (read_float (&ftmp, hd))
	ftmp = 0;		/*scaling defaults to type 0 */

      switch ((int) ftmp)
	{
	case 0:		/* anisotropic scaling */
	  Q.x = (P2.x - P1.x) / (S2.x - S1.x);
	  Q.y = (P2.y - P1.y) / (S2.y - S1.y);
	  break;

	case 1:		/* isotropic scaling */
	  if (read_float (&ftmp, hd))	/* percentage of unused space on the left */
	    ftmp = 50.0;	/* of the isotropic area defaults to 50%  */
	  Q.x = (P2.x - P1.x) / (S2.x - S1.x);
	  Q.y = (P2.y - P1.y) / (S2.y - S1.y);
	  if (Q.x < Q.y)
	    {
	      if (read_float (&ftmp, hd))
		ftmp = 50.0;	/* percentage of unused space below the plot */
	      S1.y +=
		ftmp * ((P2.y - P1.y) / Q.y - (P2.y - P1.y) / Q.x) / 100.0;
	      Q.y = Q.x;
	      S2.y = S1.y + (P2.y - P1.y) / Q.y;
	    }
	  else
	    {
	      S1.x +=
		ftmp * ((P2.x - P1.x) / Q.x - (P2.x - P1.x) / Q.y) / 100.0;
	      read_float (&ftmp, hd);	/* mandatory 'bottom' value is unused */
	      Q.x = Q.y;
	      S2.x = S1.x + (P2.x - P1.x) / Q.x;
	    }
	  break;
	case 2:		/* point factor scaling */
	  Q.x = S2.x;
	  Q.y = S2.y;
	  S2.x = S1.x + (P2.x - P1.x) / Q.x;
	  S2.y = S1.y + (P2.y - P1.y) / Q.y;
	  break;
	default:
	  par_err_exit (0, cmd);
	}
      scale_flag = TRUE;
      Plotter_to_User_coord (&p_last, &p_last);
      break;

    case SP:			/* Select pen: none/0, or 1...8 */
      old_pen = pen;
      thickness = 0.;		/* clear any PT setting (should we default to 0.3 here ??) */
      if (read_float (&p1.x, hd))	/* just SP;     */
	pen = 0;
      else
	pen = (int) p1.x;

      if (pen < 0 || pen > pg->maxpens)
	{
	  Eprintf ("\nIllegal pen number %d: replaced by %d\n", pen,
		   pen % pg->maxpens);
	  n_unexpected++;
	  pen = pen % pg->maxpens;
	}
      if (old_pen != pen)
	{
	  if ((fputc (SET_PEN, td) == EOF) || (fputc (pen, td) == EOF))
	    {
	      PError ("Writing to temporary file:");
	      Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
	      exit (ERROR);
	    }
	}
      if (pen)
	pens_in_use[pen] = 1;
/*              pens_in_use |= (1 << (pen-1)); */
      break;

    case BP:			/* Begin Plot */
   	if (read_float(&ftmp,hd))	/* No number found */
	{}
   	else {
   		switch((int)ftmp) {
   		case 1: /* picture name follows */
   			tmpstr[0]=fgetc(hd); /* skip comma */
   			tmpstr[0]=fgetc(hd);
   			if (!silent_mode)fprintf(stderr,"HPGL picture name: %c",tmpstr[0]);
   			if (tmpstr[0] == '"'){
   			tmpstr[0]=' ';
   			do {tmpstr[0]=fgetc(hd);
   			    if (!silent_mode) fputc(tmpstr[0],stderr);
   			}    
   			 while (tmpstr[0] != '"'); 
			}
			if (!silent_mode) fprintf(stderr,"\n");
   			break;
   		case 2: /* number of copies */
   		case 3: /* disposition code */
   		case 4: /* render unfinished */
			if (read_float (&ftmp, hd )) break;
			break;
		default:
			break;
		}
	}	 
		/* fall through to initialization code now */	
    case DF:			/* Set to default               */
    case IN:			/* Initialize */
      reset_HPGL ();
      tp->CR_point = HP_pos;
      break;
    case RO:
      if (read_float (&ftmp, hd))	/* No number found  */
	{
	  break;
	}
      else
	{
	  /*if (!silent_mode)
	    fprintf (stderr, "RO encountered, rotating P1,P2 by %f\n", ftmp);
*/
	  rotate_flag = 1;
	  rot_ang += ftmp;
	  rot_tmp = ftmp;
	  switch ((int) ftmp)
	    {
	    case 90:
	    case 270:
	      ftmp = M.x;
	      M.x = M.y;
	      M.y = ftmp;
	      break;
	    case 180:
	    default:
	      break;
	    }
	 /* if (!silent_mode)
	    fprintf (stderr, "cumulative rot_ang now %f\n", rot_ang);*/
	  rot_cos = cos (M_PI * rot_ang / 180.0);
	  rot_sin = sin (M_PI * rot_ang / 180.0);
	  rotate_flag = 1;

	  if (ps_flag)
	    {			/* transform extents from previous PS statement */

	      xmin = 1e10;
	      ymin = 1e10;
	      xmax = 1e-10;
	      ymax = 1e-10;

	      p1.x = 0;
	      p1.y = 0;
	      if (scale_flag)	/* Rescaling    */
		User_to_Plotter_coord (&p1, &p2);
	      else
		p2 = p1;	/* Local copy   */
	      HP_pos = p2;	/* Actual plotter pos. in plotter coord */
	      ftmp = rot_cos * p2.x - rot_sin * p2.y;
	      p2.y = rot_sin * p2.x + rot_cos * p2.y;
	      p2.x = ftmp;
	      xmin = MIN (p2.x, xmin);
	      ymin = MIN (p2.y, ymin);
	      xmax = MAX (p2.x, xmax);
	      ymax = MAX (p2.y, ymax);
	      p1.x = M.x;
	      p1.y = M.y;
 	      if (scale_flag)	/* Rescaling    */
		User_to_Plotter_coord (&p1, &p2);
	      else
		p2 = p1;	/* Local copy   */
	      HP_pos = p2;	/* Actual plotter pos. in plotter coord */
	      ftmp = rot_cos * p2.x - rot_sin * p2.y;
	      p2.y = rot_sin * p2.x + rot_cos * p2.y;
	      p2.x = ftmp;
	      xmin = MIN (p2.x, xmin);
	      ymin = MIN (p2.y, ymin);
	      xmax = MAX (p2.x, xmax);
	      ymax = MAX (p2.y, ymax);
	    }
	}
      break;
    case BL:			/* Buffer label string          */
      read_string (strbuf, hd);
      break;
    case CP:			/* Char Plot (rather: move)     */
      if (read_float (&p1.x, hd))	/* No number found  */
	{
	  plot_string ("\n\r", LB_direct);
	  return;
	}
      else if (read_float (&p1.y, hd))
	par_err_exit (2, cmd);

      p2.x = p1.x * tp->chardiff.x - p1.y * tp->linediff.x + HP_pos.x;
      p2.y = p1.x * tp->chardiff.y - p1.y * tp->linediff.y + HP_pos.y;
      Pen_action_to_tmpfile (MOVE_TO, &p2, FALSE);
      break;
    case DI:			/* Char plot Dir (absolute)     */
      if (read_float (&p1.x, hd))	/* No number found  */
	{
	  tp->dir = 0.0;
	  tp->CR_point = HP_pos;
	  adjust_text_par ();
	  break;
	}
      if (read_float (&p1.y, hd))	/* x, but not y */
	par_err_exit (2, cmd);
      if ((p1.x == 0.0) && (p1.y == 0.0))
	par_err_exit (0, cmd);
      tp->dir = atan2 (p1.y, p1.x);
      tp->CR_point = HP_pos;
      adjust_text_par ();
      break;
    case DR:			/* Char plot Dir (rel P1,P2)    */
      if (read_float (&p1.x, hd))	/* No number found  */
	{
	  tp->dir = 0.0;
	  tp->CR_point = HP_pos;
	  adjust_text_par ();
	  break;
	}
      if (read_float (&p1.y, hd))
	par_err_exit (2, cmd);	/* x, but not y */
      if ((p1.x == 0.0) && (p1.y == 0.0))
	par_err_exit (0, cmd);
      tp->dir = atan2 (p1.y * (P2.y - P1.y), p1.x * (P2.x - P1.x));
      tp->CR_point = HP_pos;
      adjust_text_par ();
      break;
    case DT:			/* Define string terminator     */
      StrTerm = getc (hd);
      break;
    case DV:			/* Text direction vertical      */
      if (read_float (&ftmp, hd) || ftmp == 0)
	mode_vert = 0;
      else
	mode_vert = 1;
      break;
    case ES:			/* Extra Space                  */
      if (read_float (&tp->espace, hd))	/* No number found */
	{
	  tp->espace = 0.0;
	  tp->eline = 0.0;
	}
      else if (read_float (&tp->eline, hd))
	tp->eline = 0.0;	/* Supply default       */
      adjust_text_par ();
      break;
    case LB:			/* Label string                 */
      read_string (strbuf, hd);
      plot_string (strbuf, LB_direct);
      /*
       * Bug fix by W. Eric Norum:
       * Update the position so that subsequent `PR's will work.
       */
      if (scale_flag)
	Plotter_to_User_coord (&HP_pos, &p_last);
      else
	p_last = HP_pos;
      break;
    case LO:			/* Label Origin                 */
      if (read_float (&p1.x, hd))	/* No number found */
	tp->orig = 1;
      else
	{
	  tp->orig = (int) p1.x;
	  if (tp->orig < 1 || tp->orig == 10 || tp->orig > 19)
	    tp->orig = 1;	/* Error        */
	}
      adjust_text_par ();
      break;
    case PB:			/* Plot Buffered label string   */
      plot_string (strbuf, LB_buffered);
      break;
    case SI:			/* Char cell Sizes (absolute)   */
      if (read_float (&tp->width, hd))	/* No number found */
	{
	  tp->width = 0.187;	/* [cm], A4     */
	  tp->height = 0.269;	/* [cm], A4     */
	}
      else
	{
	  if (read_float (&tp->height, hd))
	    par_err_exit (2, cmd);
	  if ((tp->width == 0.0) || (tp->height == 0.0))
	    par_err_exit (0, cmd);
	}
      tp->width *= 400.0;	/* [cm] --> [plotter units]        */
      tp->height *= 400.0;	/* [cm] --> [plotter units]        */
      adjust_text_par ();
      break;
    case SL:			/* Char Slant                   */
      if (read_float (&tp->slant, hd))	/* No number found     */
	tp->slant = 0.0;
      adjust_text_par ();
      break;
    case SM:			/* Symbol Mode                  */
      read_symbol_char (hd);
      break;
    case SR:			/* Character  sizes (Rel P1,P2) */
      if (read_float (&tp->width, hd))	/* No number found */
	{
	  tp->width = 0.75;	/* % of (P2-P1)_x    */
	  tp->height = 1.5;	/* % of (P2-P1)_y    */
	}
      else
	{
	  if (read_float (&tp->height, hd))
	    par_err_exit (2, (short) cmd);
	  if ((tp->width == 0.0) || (tp->height == 0.0))
	    par_err_exit (0, (short) cmd);
	}
      tp->width *= (P2.x - P1.x) / 100.0;	/* --> [pl. units]     */
      tp->height *= (P2.y - P1.y) / 100.0;
      adjust_text_par ();
      break;
    case SA:			/* Select designated alternate charset */
      if (tp->altfont)
	tp->font = tp->altfont;
      else			/* Was never designated, default to 0 */
	tp->font = 0;
      break;
    case SD:
	if (read_float(&ftmp, hd)) /* just SD - defaults */
	tp->stdfont = 0;
	else {
	switch((int)ftmp){
	case 1: /* charset */
	if (read_float(&csfont, hd))
	 par_err_exit (2, cmd);
	else 
	 tp->stdfont=(int)csfont;
	break;
	case 2: /* fixed or variable spacing */
	if (read_float(&csfont, hd))
         par_err_exit (2, cmd);
	else
	if ((int)csfont==1 && !silent_mode) 
		fprintf(stderr,"only fixed fonts available\n");
	break;
	case 3: /* font pitch */
        case 4: /* font height */
	case 5: /* posture */
	if (read_float(&csfont, hd))
         par_err_exit (2, cmd);
        else
	if (!silent_mode) fprintf(stderr,"pitch/height/posture unsupported\n");	
	break;
	default:
	par_err_exit(1,cmd);
	}
	}	
	break;
    case SS:			/* Select designated standard character set */
      if (tp->stdfont)
	tp->font = tp->stdfont;
      else			/* Was never designated, default to 0 */
	tp->font = 0;
      break;
    case UC:			/* User defined character       */
      plot_user_char (hd);
      break;
    case UL:			/* User defined line style      */
      set_line_style_by_UL (hd);
      break;
    case MG:
    case WD:			/* Write string to display      */
      read_string (strbuf, hd);
      if (!silent_mode)
	Eprintf ("\nLABEL: %s\n", strbuf);
      break;

    default:			/* Skip unknown HPGL command: */
      n_unknown++;
      if (!silent_mode)
	Eprintf ("  %c%c: ignored  ", cmd >> 8, cmd & 0xFF);
      if (cmd == EOF)
	{
	  n_unexpected++;
	  if (!silent_mode)
	    Eprintf ("\nUnexpected EOF!\t");
	}
      break;
    }
}


void
read_HPGL (GEN_PAR * pg, const IN_PAR * pi)
/**
 ** This routine is the high-level entry for HP-GL processing.
 ** It reads the input stream character-by-character, identifies
 ** ESC. commands (device controls) and HP-GL mnemonics, reads
 ** parameters (if expected), and initiates processing of these
 ** commands. It finally reports on this parsing process.
 **/
{
  int c;
  short cmd;

  init_HPGL (pg, pi);

  if (!pg->quiet)
    Eprintf ("\nReading HPGL file\n");

  /**
   ** MAIN parser LOOP!!
   **/
  while ((c = getc (pi->hd)) != EOF)
    {
      switch (c)
	{
#ifdef MUTOH_KLUGE
	case '\a':
	  Eprintf ("Mutoh header found\n");
	  read_ESC_cmd (pi->hd, FALSE);	/* ESC sequence */
	  break;
#endif
	case ESC:
	  read_ESC_cmd (pi->hd, TRUE);	/* ESC sequence */
	  break;
	default:
	  if ((c < 'A') || (c > 'z') || ((c > 'Z') && (c < 'a')))
	    break;
	  cmd = c << 8;
	  if ((c = getc (pi->hd)) == EOF)
	    return;
	  if ((c < 'A') || (c > 'z') || ((c > 'Z') && (c < 'a')))
	    {
	      ungetc (c, pi->hd);
	      break;
	    }
	  cmd |= (c & 0xFF);
	  read_HPGL_cmd (pg, cmd, pi->hd);
	}
    }

  if (!pg->quiet)
    {
      Eprintf ("\nHPGL command(s) ignored: %d\n", n_unknown);
      Eprintf ("Unexpected event(s):  %d\n", n_unexpected);
      Eprintf ("Internal command(s):  %ld\n", vec_cntr_w);
      Eprintf ("Pens used: ");
/*      for (c=0; c < NUMPENS; c++, pens_in_use >>= 1)
   if (pens_in_use & 1)
 */
      for (c = 0; c < NUMPENS; c++)
	if (pens_in_use[c] == 1)
	  Eprintf ("%d ", c);
/*                      Eprintf ("%d ", c+1); */
      Eprintf ("\nMax. number of pages: %d\n", page_number);
    }
}




void
adjust_input_transform (const GEN_PAR * pg, const IN_PAR * pi, OUT_PAR * po)
{
/**
 ** The temporary input data of the temp. file may be re-used multiple
 ** times by calling this function with varying parameters,
 ** mainly in pi.
 **
 ** Some conversion factors for transformation from HP-GL coordinates
 ** (as given in the temp. file) into mm or pel numbers are set here.
 ** There are both global parameters and elemts of po set here.
 ** DPI-related factors only apply if the current mode is a raster mode.
 **
 ** # points (dots) in any direction = range [mm] * 1in/25.4mm * #dots/in
 **/

  double dot_ratio, Dx, Dy, tmp_w, tmp_h;
  char *dir_str;

  Dx = xmax - xmin;
  Dy = ymax - ymin;
  dot_ratio = (double) po->dpi_y / (double) po->dpi_x;
  po->width = pi->width;
  po->height = pi->height;
  po->xoff = pi->xoff;
  po->yoff = pi->yoff;

  /* Width  assuming given height:      */
  tmp_w = pi->height * Dx / Dy * pi->aspectfactor;
/*  tmp_w     = pi->height * Dx / Dx / pi->aspectfactor; */
  /* Height assuming given width:       */
  tmp_h = pi->width * Dy / Dx / pi->aspectfactor;

  /**
   ** EITHER width OR height MUST be the correct limit. The other will
   ** be adapted. Adaptation of both is inconsistent, except in truesize mode.
   **/

  if (pi->truesize)
    {
      po->width = Dx / 40.0;	/* Ignore -w, take natural HP-GL range  */
      po->height = Dy / 40.0;	/* Ignore -h, take natural HP-GL range  */
      po->HP_to_xdots = (float) (po->dpi_x / 1016.0);	/* dots per HP unit */
      po->HP_to_ydots = (float) (po->dpi_y / 1016.0);	/*  (1/40 mm)       */
      dir_str = "true sizes";
      if (pi->center_mode)
	{
	if (!pg->quiet) {
	  fprintf (stderr, "trying to center image\n");
	  fprintf (stderr, "po->width ?<? tmp_w: %f %f\n", po->width, tmp_w);
	  fprintf (stderr, "po->height ?<? tmp_h: %f %f\n", po->height,
		   tmp_h);
		  } 
	  if (po->width < tmp_w)
	    po->xoff += (tmp_w - po->width) / 2.0;
	  if (po->height < tmp_h)
	    po->yoff += (tmp_h - po->height) / 2.0;
	}

    }
  else
    {
/*    if (po->width > tmp_w) */
      if (Dy > Dx)
	{
	  po->HP_to_ydots = (float) (po->dpi_y * po->height) / Dy / 25.4;
	  po->HP_to_xdots = po->HP_to_ydots * pi->aspectfactor / dot_ratio;
	  if (pi->center_mode)
	    po->xoff += (po->width - tmp_w) / 2.0;	/* by L. Lowe   */
	  po->width = tmp_w;
	  dir_str = "width adapted";	/* Height fits, adjust width    */
	}
      else
	{
	  po->HP_to_xdots = (float) (po->dpi_x * po->width) / Dx / 25.4;
	  po->HP_to_ydots = po->HP_to_xdots * dot_ratio / pi->aspectfactor;
	  if (pi->center_mode)
	    po->yoff += (po->height - tmp_h) / 2.0;	/* by L. Lowe   */
	  po->height = tmp_h;
	  dir_str = "height adapted";	/* Width  fits, adjust height   */
	}
    }

  if (!pg->quiet)
    {
      Eprintf ("\nWidth  x  height: %5.2f x %5.2f mm, %s\n",
	       po->width, po->height, dir_str);
      Eprintf ("Coordinate range: (%g, %g) ... (%g, %g)\n",
	       xmin, ymin, xmax, ymax);
    }

  po->xmin = xmin;
  po->xmax = xmax;
  po->ymin = ymin;
  po->ymax = ymax;
}

#ifdef EMF
void reset_tmpfile(void)
{
   long r=lseek(fileno(td),0L,SEEK_SET);
   if(vec_cntr_r)
   again=TRUE;
   vec_cntr_r=0;
}
#endif

PlotCmd PlotCmd_from_tmpfile (void)
{
  PlotCmd cmd;

  if (!silent_mode && !again)
    switch (vec_cntr_r++)
      {
      case 0:
	Eprintf ("\nProcessing Cmd: ");
	break;
      case 1:
	Eprintf ("1 ");
	break;
      case 2:
	Eprintf ("2 ");
	break;
      case 5:
	Eprintf ("5 ");
	break;
      case 10:
	Eprintf ("10 ");
	break;
      case 20:
	Eprintf ("20 ");
	break;
      case 50:
	Eprintf ("50 ");
	break;
      case 100:
	Eprintf ("100 ");
	break;
      case 200:
	Eprintf ("200 ");
	break;
      case 500:
	Eprintf ("500 ");
	break;
      case 1000:
	Eprintf ("1k ");
	break;
      case 2000:
	Eprintf ("2k ");
	break;
      case 5000:
	Eprintf ("5k ");
	break;
      case 10000:
	Eprintf ("10k ");
	break;
      case 20000:
	Eprintf ("20k ");
	break;
      case 50000L:
	Eprintf ("50k ");
	break;
      case 100000L:
	Eprintf ("100k ");
	break;
      case 200000L:
	Eprintf ("200k ");
	break;
      case 500000L:
	Eprintf ("500k ");
	break;
      }

  switch (cmd = fgetc (td))
    {
    case NOP:
    case MOVE_TO:
    case DRAW_TO:
    case PLOT_AT:
    case SET_PEN:
    case DEF_PW:
    case DEF_PC:
      return cmd;
    case EOF:
    default:
      return CMD_EOF;
    }
}



void
HPGL_Pt_from_tmpfile (HPGL_Pt * pf)
{
  if (fread ((VOID *) pf, sizeof (*pf), 1, td) != 1)
    {
      PError ("HPGL_Pt_from_tmpfile");
      Eprintf ("Error @ Cmd %ld\n", vec_cntr_r);
      exit (ERROR);
    }
  if (pf->x < xmin || pf->x > xmax)
    Eprintf ("HPGL_Pt_from_tmpfile: x out of range (%g not in [%g, %g])\n",
            pf->x, xmin, xmax);
   if (pf->y < ymin || pf->y > ymax)
     Eprintf ("HPGL_Pt_from_tmpfile: y out of range (%g not in [%g, %g])\n",
            pf->y, ymin, ymax);
}

