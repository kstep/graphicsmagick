/*   RAL-CGM Interpreter module:  @(#) cgmops.c  version 3.6
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
 * William K Lui @ Rutherford Appleton Laboratory (wkl@uk.ac.rl.inf)
 *
 * Description: Postscript output driver
 *
 *
 * Maintenance Log:
 *   4 Jun 90 RTP  Original version tested on CMS.
 *  16 Aug 90 RTP  Incorporated into master source.
 *  21 Aug 90 WKL  fixed the 3 point arc bug, put in comments for functions
 *  22 Aug 90 WKL  Added PSBEZtext and PSHERtext -- PS output for Bezier and
 *                 Hershey methods.
 *  26 Aug 90 WKL  Bug fix for gdp for gdp number -1 to -4;
 *                 draw text in correct text colour.
 *  27 Aug 90 PLP  Completed introduction of cgmatt.c and cgmfont.c.
 *  13 Sep 90 WKL  Fixed problems with RAL-CGM dictionary; fixed the
 *                 setrgbcolor bug.
 *  15 Sep 90 CDO  Replaced malloc calls to calloc (1, ...) calls to stop
 *                 some compilers complaining about boundary problems when
 *                 space is being reserved for a non-char variable.
 *  16 Sep 90 WKL  Fixed the COLRTABLE element and the colour normalization
 *                 bug.
 *  16 Sep 90 PLP  Straightened out the time of day retrieval.
 *  17 Sep 90 PLP  Inroduced cgmtext.c functions which really belong here:
 *                 PSalias(), Psgetfd(), PSreadafm(), PSskp2dgt() and
 *                 PSskpdgt(). Also declared functions local to this
 *                 module as statics.
 *  18 Sep 90 PLP  Reconstructed the RESTRTEXT minimal solution and
 *                 MESSAGE.
 *  18 Sep 90 WKL  Completed colour table code reconstruction.
 *  19 Sep 90 PLP  Sorted out functions and variables declarations.
 *  19 Sep 90 WKL  Completed clip rectangle and initial clip reconstruction.
 *                 Uses CGMfopen.
 *  20 Sep 90 WKL  Basic handling for multiple metafiles.
 *  21 Sep 90 PLP  Removed TEXTOUT and renamed TEXTOUT1 to PSOUT.
 *   3 Oct 90 CDS  Added code to drive Bezier system.
 *  14 Oct 90 CDS  Corrected bugs in PSstktext, i.e. memory allocation
 *                 and reseting of pointers.
 *  19 Oct 90 RTP  Add new Attribute checking routines for text only
 *  22 Oct 90 RTP  Correct colour attribute handling
 *                 Add Bundled Attribute routines for Line & Marker
 *                 Change PSline_att() and PSmarker_att() to PSattrib()
 *                 Move <time.h> to cgmmach.h
 *   1 Nov 90 PLP  Fixed start angle/end angle, pie and chord problems in
 *                 arcs.
 *   2 Nov 90 PLP  Fixed start angle/end angle problems in ellipses, improved
 *                 code efficiency.
 *   5 Nov 90 PLP  Added special cases handling in PSelarc (more work needed).
 *  15 Nov 90 PLP  PSfill() - HATCH: fixed positive and negative slope
 *                 hatching problems; improved code efficiency; added
 *                 comments.
 *  16 Nov 90 PLP  Added three RAL GKS like hatching styles.
 *  20 Nov 90 PLP  Introduced buffering in PStext. In PSreadafm, changed
 *                 strcmp() calls to strncmp(), to allow for possible .afm
 *                 format changes in the future.
 *  22 Nov 90 RTP  Add Hershey text routines
 *   7 Dec 90 RTP  Use 'Point' for text routines parameters
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  17 Dec 90 RTP  Correct parameters for GKSCIRCLE and GKSARC
 *  24 Jan 91 AMR  Removed redundant functions from prologue.ps
 *  28 Jan 91 AMR  Standardised PSmfname
 *  13 Feb 91 AMR  Altered PSarcref to spot all special cases.
 *  15 Feb 91 AMR  Added PSspecarc to handle output for arc special cases
 *                 and made appropriate changes to code that calls PSarcref.
 *  21 Feb 91 AMR  Introduced dot into prologue.ps, draws fixed size
 *                 dot for PSspecarc.
 *  22 Feb 91 AMR  Improved PSmfname to use EXTLGTH in generating extensions
 *  27 Feb 91 AMR  Fixed PSmfname bug, now it'll work on CMS and VAXes,
 *                 introduced PSmodextn to keep system dependant file handling
 *                 seperate.
 *   1 Mar 91 AMR  PSmfname now asks user for a new filename when it runs out
 *                 of extensions for the existing one, and starts again. Added
 *                 PSmodfname to separate system dependant code.
 *   6 Mar 91 AMR  Implement colour in PScellarray.
 *  28 Mar 91 AMR  Added code for EPSF.
 *   8 Apr 91 AMR  Fixed scaling bugs in marker code of PSattrib.
 *  10 Apr 91 AMR  Changed 'normal?' PS output so it conforms to Adobe2.0
 *                 commenting conventions.
 *  12 Apr 91 AMR  Moved PSmodextn, PSmodfname and PSgetfname to CGMfile,
 *                 renamed them to CGMmodfextn, CGMmodfname and CGMgetfname
 *                 respectively.
 *  22 Apr 91 AMR  Introduced workstation style settings for PostScript devices
 *  15 May 91 AMR  Changed BEZtext, HERtext and PSstktext to allow for extra
 *                 argument.
 *  15 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change all calloc routines to use MALLOC and add
 *                 casts to (size_t) where necessary
 *                 Change test for fpin to test for NULL pointer
 *  20 May 91 RTP  Change free() calls to Macro FREE
 *   5 Jun 91 AMR  Added routines PSinitpsfile and PSendpsfile to put headers
 *                 and footers onto the output files, used in several places
 *                 due to EPSF/ordinary output.
 *  25 Jun 91 AMR  Implemented Colour patterns, completes implementation of
 *                 colour PostScript. Bug fixed where x, and y values for
 *                 pattern vectors were swapped.
 *   1 Jul 91 CDS  PSstktext: Changed to use newchar, and improve performance.
 *   1 Jul 91 AMR  Background colour implemented.
 *  16 Jul 91 AMR  Added PSoptions to allow for local options.
 *  17 Jul 91 AMR  Added PostScript output border switch.
 *  18 Jul 91 AMR  Fixed a bug with DISJTLINE's where they exceeded the 80 char.
 *                 limit in the output file. Can be further optimised if more
 *                 points (than 2) are passed to PSpoints in one go.
 *  18 Jul 91 AMR  Fixed Colour Table bug where when extra values were defined
 *                 with MAXCOLRINDEX, they weren't initialised.
 *  30 Jul 91 AMR  Added NONOP element.
 *  22 Aug 91 AMR  Moved external declarations and local defines to cgmps.h
 *  12 Sep 91 AMR  Landscape option added.
 *  16 Sep 91 AMR  Added warning for GDP's when present, but not invoked.
 *  17 Oct 91 AMR  Changed psvp from Prec to int
 *  29 Jul 92 RTP  Add options B & F to show how to react to background colr
 *                 Change colour number to ATT* calls to use current value
 *   1 AUG 92 MAS  Add retricted text code
 *  27 Aug 92 RTP  Change default colour table if Colour mode is set
 *                 Add new device for QMS extended A4
 *                 Add device options to access Device table
 *                 Correct pointsa for GKS GDP -4 (circle)
 *   1 Mar 93 KEVP Improve handling of real points precision
 *                 psvp replaced by dec_right and dec_all
 *  12 Mar 93 KEVP Introduce fill area by trapezoid decoposition
 *                 New functions PStrapfill, PSTRPdraw, PSLINEfunc
 *                               PSdodecomp
 *                 Rewrite PSpset to be more efficient and maintainable
 *                 Make edge visibility an argument of PSfill
 *  23 Mar 93 KEVP Make Trapezoid Decomposition a device option 'd'
 *                 the default option being 'w' polygon drawn WHOLE.
 *  21 Apr 93 RTP  Change 'clippath' to 'clip' to correct clipping
 *  20 JUL 93 CDS  PSgetfd: Changed so that only reads fontname file once,
 *                 saves time
 *                 and cures memory allocation probs.
 *  08 Oct 93 RTP  Add -O3 option for A3 PostScript
 *  21 Oct 93 KEVP Corrected Bounding Box calculation,
 *                 replacing static cgm_ori with touching_sides.
 *  25 Oct 93 KEVP Deferred clip setting to attribute setting. This
 *                 prevents statements appearing before header in EPSFs.
 *   7 Dec 93 KEVP Corrected pick-up of PSelarc integer argment 9.
 *   9 Feb 94 RTP  Change all occurrences of =+ to +=
 *  20 Jul 94 KEVP In print statements, change %d to %ld, when necessary
 *                           and cast prec to int when printing real vdc points.
 *  26 Sep 94 KEVP Ditto for PSLINEfunc
 *  18 Jan 95 KEVP Cast prec to int in PSTRPfill.
 *  23 Jan 95 KEVP Use new POLget utility to initialise the polygon set
 *                 structure correctly.
 *  24 Feb 95 KEVP In PSpset and PStrapfill, draw egdes AFTER polygon set
 *                 and use the edge attributes (with new function PSedge).
 */

#include "cgmout.h"
#include "cgmfile.h"
#include "cgmps.h"
#include "cgmpar.h"
#include "cgmbez.h"
#include "cgmher.h"
#include "cgmgraph.h"
#include "cgmpts.h"
#include "cgmpoly.h"
#include "cgmtrap.h"

#ifdef PROTO

/* Functions in this Module  */

static void  PSpoints ( long n, long *ip, float *fp, Enum set,
                        char *str1, char *str2 ),
             PSprprolog ( char * str ),
             PSbegpic ( void ),
             PSattrib ( Code ),
             PSfill ( Enum, Enum ),
             PSedge ( void ),
             PScellarray ( Long*, Float*, Long, Logical ),
             PSins_coltab ( void ),
             PSins_pattab ( int i, int d1, int d2, int d3, int d4,
                            int d5, int d6, int d7, int d8 ),
             PSellipse ( Long *, Float * ),
             PSarcctr ( Long *, Float *, Enum ),
             PSelarc ( Long *, Float *, Enum ),
             PSpset ( long, Long *, Float *, Logical ),
             PSpattable ( long , Long *, Logical ),
             PStxtdraw ( Textitem *, Point ),
             PStext ( Textitem *, Point ),
             PSreadafm ( char *, Textitem * ),
             PSskp2dgt ( char ** ),
             PSskpdgt ( char ** ),
             PSspecarc ( Enum , PS_arcref* ),
             PSinitpsfile ( char * ),
             PSendpsfile (),
             PSstktext(int n, Point *pt, Tmatrix mat,
                       int methodfg, int newchar),
             PStrapfill( Long, Long *, Float *, Enum, Logical);
static Enum  PSarcref ( PS_arcref*, double, double, double,
                        double, double, double );
static int   PScoltab ( long *, long );
static char *PSalias ( char * ),
            *PSmfname ( );
static Logical PSdodecomp( Long, Long *, Float *, Enum);

void         PSLINEfunc( Long, Point *);
void         PSBEZtext( int, Point *, Tmatrix, int );
void         PSHERtext( int, Point *, Tmatrix, int );
void         PSTRPdraw( Int, Trapezoid *);

/*  External routines */
extern FILE *CGMfopen ( char *, char, Enum  );

#else

static void PSpoints(),
            PSprprolog(),
            PSbegpic(),
            PSattrib(),
            PSfill(),
            PSedge(),
            PScellarray(),
            PSins_coltab(),
            PSins_pattab() ,
            PSellipse(),
            PSarcctr(),
            PSelarc(),
            PSpset(),
            PSpattable(),
            PStxtdraw(),
            PStext(),
            PSreadafm(),
            PSskp2dgt(),
            PSskpdgt(),
            PSinitpsfile(),
            PSendpsfile(),
            PSspecarc(),
            PStrapfill();
static Enum PSarcref();
static int  PScoltab();
static char *PSalias(),
            *PSmfname ();
static Logical PSdodecomp();

void         PSLINEfunc();
void         PSBEZtext();
void         PSHERtext();
void         PSTRPdraw();

extern FILE *CGMfopen (  );

#endif  /* ANSI prototyping */

/* PostScript driver global variables  */

static double radtodegree = 180.0/PI;
static float dd_linewidth;               /* device dependent line width */
static float vdc_xrange, vdc_yrange;     /* the extents of the VDC system */
static float psbb;                       /* size of PS box */
static Point pstxt;
static int no_of_page = 0, no_of_mf = 0; /* no. of pages and metafile counters*/
static int npspat = 0, backflag = 0;
static int dev_ori = PORTRAIT,
           touching_sides,
           orient  = PORTRAIT;
static char **ftnames = NULL;                     /* list of PostScript fonts */
static Logical line_att_changed = TRUE;
static Logical fill_att_have = FALSE;
static FILE *cgmop;
static PS_pattabtype *PSpat_table;
static int dec_all=6;       /* length of decimal real number for point */
static int dec_right=4;                /* ditto right of decimal point */
static char col_mode = MONOCHROME;         /* MONOCHROME / COLOUR */
static char ps_mode = NORMALPS;            /* NORMALPS / EPSF */
static char border_mode = INVISIBLE;       /* INVISIBLE / VISIBLE */
static char background_colour = VISIBLE;   /* INVISIBLE / VISIBLE */
static char poly_mode = WHOLE;             /* WHOLE / DECOMPOSE */
static double rt_wid,rt_hgt,rt_scalex,rt_scaley;
static Logical rt_mode=FALSE;
static Logical clip_change=FALSE;  /* Clip Status or Rectangle changed */
static float tzytol;              /* Tolerance for trapezoids */
static Int   tzdec_right;         /* decimal places for trapezoid */
static Int   tzno;                /* Number of trapezoids per line */

/*********      Externally accessible functions          ***********/

/********************************************************************
*  CGMOps() : This is the main PS driver routine which takes in the
*             CGM code and process the CGM code accordingly.
*             input arguments:
*                stream : File descriptor for PS output file
*                Code   : CGM element code
*                pi     : array of data (integers)
*                pr     : array of data (real numbers).
*                str    : data (string)
*
*             output arguments:  None
*             Return value : None
********************************************************************/


#ifdef PROTO
void CGMOps (FILE *stream, Code c, Long *pi, Float *pr, char *str)
#else
void CGMOps (stream, c, pi, pr, str)
Code c;
long *pi;
float *pr;
char *str;
FILE *stream;
#endif
{
   static Logical cont_list = FALSE;
   static char *func = "CGMops";
   char mess[80];
   PS_arcref arcref;
   Enum arctype;
   register long num;
   Code major;
   int i;
   Textitem *titem;


   if (c == (Code) EOF)
   {
      if ( cgmverbose ) PUTMSG "Interpreter Ended OK\n");
      exit(0);
   }

   cgmop = stream;
   major = c>>8;
   num = *pi++;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */
         switch (c)
         {
            case NONOP:
               return;

            case LINE:         /*  Polyline   */
               PSattrib( LINE );
               if ( !cont_list )
               {
                 PSOUT "gsave newpath\n");
                 PSpoints ( num, pi, pr, NOSET," m "," l ");
                 cont_list = ( num < ZERO );
                 if (!cont_list) PSOUT " S\n");
               }
               else
               {
                 PSpoints ( num, pi, pr, NOSET," l "," l ");
                 cont_list = ( num < ZERO );
                 if (!cont_list) PSOUT " S\n");
               }
               PSOUT "grestore\n");
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               PSattrib( LINE );
               cont_list = ( num < ZERO );
               PSOUT "gsave newpath\n");
               for (i=0; i < ( num > ZERO ? num : -num ); i+=2, pi+=4, pr+=4)
                 PSpoints ( (long)2, pi, pr, NOSET," m "," l\n");
               PSOUT "S grestore\n");

               break;

            case MARKER:         /*  Polymarker  */
               PSattrib( MARKER );
               line_att_changed = TRUE;
               PSpoints ( num, pi, pr, NOSET," pm "," pm ");
               cont_list = ( num < ZERO );
               if (!cont_list) PSOUT "\n");
               break;

            case TEXT:         /*  Text   */
               if (cur.vdc_type == REAL)
               { pstxt.x = pr[0];
                 pstxt.y = pr[1];
               } else
               { pstxt.x = (float) pi[0];
                 pstxt.y = (float) pi[1];
               };
               rt_mode=FALSE;
               ATTtext(NTEXTB, PStext_bundle, cur.max_colind, &PStextatt);
               titem = TXTaccinf(num, str, &PStextatt, PSprefmeth);
               if (num == FINAL) PStxtdraw(titem, pstxt);
               break;

            case RESTRTEXT:               /*  Restricted Text - check */
               if (cur.vdc_type == REAL)
               { pstxt.x = pr[2];
                 pstxt.y = pr[3];
                 rt_wid=(double)pr[0];
                 rt_hgt=(double)pr[1];
               }
               else
               { pstxt.x = (float) pi[2];
                 pstxt.y = (float) pi[3];
                 rt_wid=(double)pi[0];
                 rt_hgt=(double)pi[1];
               };
               rt_mode=TRUE;
               ATTtext(NTEXTB, PStext_bundle, cur.max_colind, &PStextatt);
               titem = TXTaccinf(num, str, &PStextatt, PSprefmeth);
               if (num == FINAL) PStxtdraw(titem, pstxt);
               break;

            case APNDTEXT:         /*  Append Text   */
               ATTtext(NTEXTB, PStext_bundle, cur.max_colind, &PStextatt);
               titem = TXTaccinf(num, str, &PStextatt, PSprefmeth);
               if (num == FINAL) PStxtdraw(titem, pstxt);
               break;

            case POLYGON:         /*  Polygon   */
               if ( !cont_list )
               {
                  PSattrib (POLYGON);
                  if( poly_mode == DECOMPOSE &&
                     PSdodecomp(num, pi, pr, NOSET) )
                   { /* Trapezoid decomposition to be done */
                      PStrapfill( num, pi, pr, NOSET, FALSE);
                   }
                   else
                   {
                      PSOUT "gsave newpath  %%create path of polygon\n");
                      PSpoints ( num, pi, pr, NOSET," m "," l ");
                      PSOUT "\n");
                      if(num > 0)
                      { /* No more points, complete the fill */
                         PSfill(EO, curatt.edge_vis);
                         PSOUT "grestore\n");
                      }
                   }
               }
               else
               {
                  if(poly_mode == DECOMPOSE)
                     PStrapfill( num, pi, pr, NOSET, TRUE);
                  else
                  {
                      PSpoints ( num, pi, pr, NOSET," l "," l ");
                      PSOUT "\n");
                      if(num > 0)
                      { /* No more points, complete the fill */
                         PSfill(EO, curatt.edge_vis);
                         PSOUT "grestore\n");
                      }
                  }
               }
               cont_list = ( num < ZERO );
               break;

            case POLYGONSET:         /*  Polygon Set  */
               if(!cont_list) PSattrib(POLYGON);
               if(poly_mode == DECOMPOSE &&
                  (cont_list || PSdodecomp(num, pi, pr, SET)) )
               {  /* Do trapezoid decomposition */
                  PStrapfill( num, pi, pr, SET, cont_list);
               }
               else PSpset ( num, pi, pr, cont_list );
               cont_list = ( num < ZERO );
               break;

            case CELLARRAY:         /*  Cell Array  */
               PScellarray (pi,pr,num,cont_list);
               cont_list = (num < ZERO );
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               if ( cgmralgks )
                 switch (*pi)
                 {
                   case GKSARC:
                     PSattrib ( LINE );
                      PSOUT "gsave newpath  %%create path of arc\n");
                      if (cur.vdc_type == REAL)
                      {
                         arctype = PSarcref(&arcref, (double)pr[6],
                                 (double)pr[7], (double)pr[8], (double)pr[9],
                                 (double)pr[10], (double)pr[11]);
                      }
                      else
                      {
                         arctype = PSarcref(&arcref, (double)pi[7],
                                 (double)pi[8], (double)pi[9], (double)pi[10],
                                 (double)pi[11], (double)pi[12]);
                      }

                      /*
                       If the points describe a valid arc, then draw it.
                      */
                      if (arctype == ARC_OK)
                      {
                         PSOUT " %f %f %f ",
                               arcref.cent.x, arcref.cent.y, arcref.r);
                         PSOUT " %f %f arc S\n",
                               radtodegree*arcref.startang,
                               radtodegree*arcref.endang);
                      }

                      /*
                       otherwise, do as the addendum says!
                      */
                      else
                      {
                         PSspecarc ( arctype, &arcref );
                      }

                      PSOUT "grestore\n");
                      break;

                   case GKSCHORD:
                      PSOUT "gsave newpath  %%create path of arc and chord\n");
                      if (cur.vdc_type == REAL)
                      {
                         arctype = PSarcref(&arcref, (double)pr[6],
                                 (double)pr[7], (double)pr[8], (double)pr[9],
                                 (double)pr[10], (double)pr[11]);
                      }
                      else
                      {
                         arctype = PSarcref(&arcref, (double)pi[7],
                                 (double)pi[8], (double)pi[9], (double)pi[10],
                                 (double)pi[11], (double)pi[12]);
                      }

                      /*
                       If the points describe a valid arc, then draw it.
                      */
                      if (arctype == ARC_OK)
                      {
                         PSOUT " %f %f %f ",
                               arcref.cent.x, arcref.cent.y, arcref.r);
                         PSOUT " %f %f arc closepath\n",
                               radtodegree*arcref.startang,
                               radtodegree*arcref.endang);
                         PSfill (NZ, curatt.edge_vis);
                      }

                      /*
                       otherwise, do as the addendum says!
                      */
                      else
                      {
                          PSspecarc ( arctype, &arcref );
                      }

                      PSOUT "grestore\n");
                      break;

                   case GKSPIE:
                      PSOUT "gsave newpath  %%create path of pie slice\n");
                      if (cur.vdc_type == REAL)
                      {
                         arctype = PSarcref(&arcref, (double)pr[6],
                                 (double)pr[7], (double)pr[8], (double)pr[9],
                                 (double)pr[10], (double)pr[11]);
                      }
                      else
                      {
                         arctype = PSarcref(&arcref, (double)pi[7],
                                 (double)pi[8], (double)pi[9], (double)pi[10],
                                 (double)pi[11], (double)pi[12]);
                      }

                      /*
                       If the points describe a valid arc, then draw it.
                      */
                      if (arctype == ARC_OK)
                      {
                         PSOUT " %f %f moveto ", arcref.cent.x, arcref.cent.y);
                         PSOUT " %f %f lineto \n",
                               arcref.cent.x+arcref.r*cos(arcref.startang),
                               arcref.cent.y+arcref.r*sin(arcref.startang));
                         PSOUT " %f %f %f ",
                               arcref.cent.x, arcref.cent.y, arcref.r);
                         PSOUT " %f %f arc closepath\n",
                               radtodegree*arcref.startang,
                               radtodegree*arcref.endang);
                         PSfill (NZ, curatt.edge_vis);
                      }

                      /*
                       otherwise, do as the addendum says!
                      */
                      else
                      {
                         PSspecarc ( arctype, &arcref );
                      }

                      PSOUT "grestore\n");
                      break;

                   case GKSCIRCLE:
                      PSOUT "gsave newpath  %%create path of circle\n");
                      if (cur.vdc_type == REAL)
                      {
                        arcref.cent.x = pr[8];
                        arcref.cent.y = pr[9];
                        arcref.r = SQRT( (pr[8]-pr[6])*(pr[8]-pr[6])
                                       + (pr[9]-pr[7])*(pr[9]-pr[7]) );
                      }
                      else
                      {
                        arcref.cent.x = pi[9];
                        arcref.cent.y = pi[10];
                        arcref.r = SQRT( (pi[9]-pi[7])*(pi[9]-pi[7])
                                       + (pi[10]-pi[8])*(pi[10]-pi[8]) );
                      }
                      PSOUT " %f %f %f 0 360 arc ",
                               arcref.cent.x, arcref.cent.y, arcref.r);
                      PSfill (NZ, curatt.edge_vis);
                      PSOUT "grestore\n");
                      break;
                 }
               else /* no ral gdp's */
               {
                 (void) sprintf ( mess, "(type: %d)", (short)*pi);
                 (void) CGMerror ( func, ERR_NOGDP, ERROR, mess);
               }

               break;

            case RECT:         /*  Rectangle   */
               PSOUT "gsave newpath  %%create path of polygon\n");
               if (cur.vdc_type == REAL)
               {
                  pr[4]=pr[2]; pr[5]=pr[3]; pr[3]=pr[1];
                  pr[6]=pr[0]; pr[7]=pr[5]; pr[8]=pr[0]; pr[9]=pr[1];
               }
               else
               {
                  pi[4]=pi[2]; pi[5]=pi[3]; pi[3]=pi[1];
                  pi[6]=pi[0]; pi[7]=pi[5]; pi[8]=pi[0]; pi[9]=pi[1];
               }
               PSpoints ( (long)5, pi, pr, NOSET," m "," l ");
               PSOUT "\n");
               PSfill (NZ, curatt.edge_vis);
               PSOUT "grestore\n");
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVPRIM, ERROR, mess);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */
         switch (c)
         {
            case BEGMF:         /*  Begin Metafile   */
               if (ps_mode == NORMALPS)
                 PSinitpsfile(str);

               no_of_mf++;

               /* Make sure shared attributes are being picked up */
               PSlineatt.shared = &share;
               PSmarkatt.shared = &share;
               PStextatt.shared = &share;
               PSfillatt.shared = &share;
               PSedgeatt.shared = &share;

               /* install default colour table  */
               PSins_coltab();
               /* install default pattern table  */
               PSpat_table = (PS_pattabtype *)
                                 MALLOC( 5, sizeof(PS_pattabtype) );
               PSins_pattab(0,0x3c,0x3c,0xc3,0xc3,0xc3,0xc3,0x3c,0x3c);
               PSins_pattab(1,0xd1,0xe3,0xc5,0x88,0x5c,0x3e,0x1d,0x88);
               PSins_pattab(2,0x3e,0x41,0x80,0x80,0xe3,0x14,0x08,0x08);
               PSins_pattab(3,0x00,0x3c,0x42,0x42,0x42,0x42,0x3c,0x00);
               PSins_pattab(4,0x00,0x3c,0x00,0x7e,0x00,0x00,0x3c,0x00);
               npspat = 5;
               break;

            case ENDMF:         /*  End Metafile   */
               /* Print the trailer of the PostScript output file */
               if (ps_mode==NORMALPS)
                 PSendpsfile();
               FREE(PScolour_table);
               for(i=0;i<npspat;i++)
                 FREE(PSpat_table[i].pathead);
               FREE(PSpat_table);
               break;

            case BEGPIC:         /*  Begin Picture Descriptor   */
               if (ps_mode==EPSF)
               {
                 PSinitpsfile(str);
                 no_of_page++;
               }
               else
               {
                 PSOUT "%s %d\n","%%Page: ? ", ++no_of_page);
                 PSOUT "%s\n","%%PageBoundingBox: (atend)");
               }

               if (col_mode==COLOUR)
                  PSOUT "/colorflag true def\n");
               else
                  PSOUT "/colorflag false def\n");

               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               curtext = mftext;  /* Set Text picture defaults */
               PSbegpic();
               break;

            case ENDPIC:         /*  End  Picture    */

               PSOUT "EndPage\n");

               if (ps_mode == EPSF)
                 PSendpsfile();
               else
               {
                 Int x, y;

                 if ( touching_sides == LANDSCAPE)
                 {
                   x = psbb;
                   y = psbb * ( vdc_yrange / vdc_xrange );
                 }
                 else
                 {
                   x = psbb * ( vdc_xrange / vdc_yrange );
                   y = psbb;
                 }
                 PSOUT "%s\n","%%PageTrailer");
                 if( dev_ori == orient )
                 {  /* Axes not rotated */
                    PSOUT "%s %d %d %d %d\n","%%PageBoundingBox:",
                          PSdev[PSdevice].x,   PSdev[PSdevice].y,
                          PSdev[PSdevice].x+x, PSdev[PSdevice].y+y);
                 }
                 else
                 {  /* Axes rotated, but PageBoundingBox NOT rotated */
                    PSOUT "%s %d %d %d %d\n","%%PageBoundingBox:",
                          PSdev[PSdevice].x,   PSdev[PSdevice].y,
                          PSdev[PSdevice].x+y, PSdev[PSdevice].y+x);
                 }
               }
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVDELIM, ERROR, mess);
               break;
         }


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {  /* NB: Nothing that writes into the Postscript file
                   should be included here. */
            case MFVERSION:         /*  Metafile version   */
            case MFDESC:            /*  Metafile Description  */
            case VDCTYPE:           /*  VDC Type   */
            case INTEGERPREC:       /*  Integer Precision  */
            case REALPREC:          /*  Real Precision   */
            case INDEXPREC:         /*  Index Precision   */
            case COLRPREC:          /*  Colour Precision  */
            case COLRINDEXPREC:     /*  Colour Index Precision  */
            case COLRVALUEEXT:      /*  Colour value extent  */
            case MFELEMLIST:        /*  Metafile element List  */
            case BEGMFDEFAULTS:     /*  Begin Metafile defaults Replacement */
            case ENDMFDEFAULTS:     /*  End Metafile defaults Replacement  */
            case FONTLIST:          /*  Font List   */
            case CHARSETLIST:       /*  Character set list  */
            case CHARCODING:        /*  Character coding Announcer  */
               /* no action for above Metafile Descriptor Elements  */
               break;
            case MAXCOLRINDEX:      /*  Maximum Colour Index  */
               PScolour_table = (Colourentry*)realloc((char *)PScolour_table,
                             (size_t)((cur.max_colind+1)*sizeof(Colourentry)));

               if (PScolour_table == NULL)
                 exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

               if ( col_mode == MONOCHROME && cur.max_colind >= COLTAB_N)
                 for (i=COLTAB_N; i<=cur.max_colind; i++)
                   PScolour_table[i] = PScolour_table[1];
               else if ( cur.max_colind >= 256)
                 for (i=256; i<=cur.max_colind; i++)
                   PScolour_table[i] = PScolour_table[1];
               break;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {  /* NB: Nothing that writes into the Postscript file
                   should be included here. */
            case SCALEMODE:          /*  Scaling Mode   */
            case COLRMODE:           /*  Colour Selection Mode */
            case LINEWIDTHMODE:      /*  Line width Specification  */
            case MARKERSIZEMODE:     /*  Marker size Specification  */
            case EDGEWIDTHMODE:      /*  Edge width Specification  */
            case VDCEXT:             /*  VDC Extent    */
               break;
            case BACKCOLR:           /*  Background Colour  */
               backflag = ( background_colour == VISIBLE );
               break;
            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVPDESC, ERROR, mess);
               break;
         }
         break;

      case 0x33:  /* Control Elements  */
         switch(c)
         {   /* NB: Nothing that writes into the Postscript file
                    should be included here. */
            case VDCINTEGERPREC:       /* VDC Integer Precision  */
            case VDCREALPREC:          /* VDC Real Precision  */
            case AUXCOLR:              /* Auxiliary Colour  */
            case TRANSPARENCY:         /* Transparency  */
               break;
            case CLIP:                 /* Clip Indicator  */
            case CLIPRECT:             /* Clip Rectangle  */
               clip_change = TRUE;
               break;
            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVCONT, ERROR, mess);
               break;
         }
         break;

      case 0x34:  /* Circles and Ellipses  */
         switch(c)
         {
            case CIRCLE:       /* Circle      */
               PSOUT "gsave newpath  %%create path of circle\n");
               if (cur.vdc_type == REAL)
                     PSOUT " %f %f %f 0 360 arc ", pr[0], pr[1] ,pr[2] );
               else
                     PSOUT " %ld %ld %ld 0 360 arc ", pi[0], pi[1] ,pi[2]);
               PSfill(NZ, curatt.edge_vis);
               PSOUT "grestore\n");
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               PSattrib( LINE );
               PSOUT "gsave newpath  %%create path for arc3pt\n");
               if (cur.vdc_type == REAL)
               {
                  arctype = PSarcref(&arcref,(double)pr[0], (double)pr[1],
                          (double)pr[2], (double)pr[3], (double)pr[4],
                          (double)pr[5]);
               }
               else
               {
                  arctype = PSarcref(&arcref, (double)pi[0], (double)pi[1],
                          (double)pi[2], (double)pi[3], (double)pi[4],
                          (double)pi[5]);
               }

               /*
                If the points describe a valid arc, then draw it.
               */
               if (arctype == ARC_OK)
               {
                  PSOUT " %f %f %f ",
                        arcref.cent.x, arcref.cent.y, arcref.r);
                  PSOUT " %f %f arc S\n",
                       radtodegree*arcref.startang,
                       radtodegree*arcref.endang);
               }

               /*
                otherwise, do as the addendum says!
               */
               else
               {
    PSspecarc( arctype, &arcref );
               }

               PSOUT "grestore\n");
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               PSOUT "gsave newpath  %%create path of arc3ptclose\n");

               if (cur.vdc_type == REAL)
               {
                 arctype = PSarcref(&arcref,(double)pr[0], (double)pr[1],
                         (double)pr[2], (double)pr[3], (double)pr[4],
                         (double)pr[5]);
               }
               else
               {
                 arctype = PSarcref(&arcref,(double)pi[0],(double)pi[1],
                         (double)pi[2],(double)pi[3],(double)pi[4],
                         (double)pi[5]);
               }

               /*
                If the points describe a valid arc, then draw it.
               */
               if (arctype == ARC_OK)
               {
                  /*
                   pi[7]==1 for chord close type
                  */
                  if (*(pi+6))
                  {
                     PSOUT " %f %f %f ",
                            arcref.cent.x, arcref.cent.y, arcref.r);
                     PSOUT " %f %f arc closepath\n",
                           radtodegree*arcref.startang,
                           radtodegree*arcref.endang);

                  }

                  /*
                   pi[7]==0 for pie close type
                  */
                  else
                  {
                     PSOUT " %f %f moveto ", arcref.cent.x, arcref.cent.y);
                     PSOUT " %f %f lineto \n",
                           arcref.cent.x+arcref.r*cos(arcref.startang),
                           arcref.cent.y+arcref.r*sin(arcref.startang));
                     PSOUT " %f %f %f ",
                           arcref.cent.x, arcref.cent.y, arcref.r);
                     PSOUT " %f %f arc closepath\n",
                           radtodegree*arcref.startang,
                           radtodegree*arcref.endang);
                  }
                  PSfill(NZ, curatt.edge_vis);
               }

               /*
                otherwise, do as the addendum says!
               */
               else
               {
                  PSspecarc( arctype, &arcref );
               }

               PSOUT "grestore\n");
               break;

            case ARCCTR:       /* Circle Arc centre */
               PSarcctr(pi,pr,NOTCLOSED);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               PSarcctr(pi,pr,CLOSED);
               break;

            case ELLIPSE:       /* Ellipse    */
               PSellipse(pi,pr);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               PSelarc(pi,pr,NOTCLOSED);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               PSelarc(pi,pr,CLOSED);
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVOUT, ERROR, mess);
               break;
         }
         break;

      case 0x35:  /* Attributes  */
         switch(c)
         {  /* NB: Nothing that writes into the Postscript file
                   should be included here. */
            case LINEINDEX:       /*  Line Bundle index  */
            case LINETYPE:       /*  Line Type   */
            case LINEWIDTH:       /*  Line Width */
            case LINECOLR:       /*  Line Colour  */
               line_att_changed = TRUE;
               break;

            case MARKERINDEX:       /*  Marker Bundle index  */
            case MARKERTYPE:       /*  Marker Type   */
            case MARKERSIZE:       /*  Marker Size */
            case MARKERCOLR:       /*  Marker Colour  */
            case TEXTINDEX:       /*  Text Bundle index  */
            case TEXTFONTINDEX:       /*  Text Font Index  */
            case TEXTPREC:       /*  Text Precision   */
            case CHAREXPAN:       /*  Character Expansion Factor  */
            case CHARSPACE:       /*  Character Spacing  */
            case TEXTCOLR:       /*  Text Colour   */
            case CHARHEIGHT:       /*  Character Height   */
            case CHARORI:       /*  Character Orientation */
            case TEXTPATH:       /*  Text Path */
            case TEXTALIGN:       /*  Text Alignment */
            case CHARSETINDEX:       /*  Character Set Index */
            case ALTCHARSETINDEX:   /*  Alternative Character Set Index */
               break;
            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVATT, ERROR, mess);
               break;
         }
         break;

      case 0x36:  /* More Attributes  */
         switch(c)
         {  /* NB: Nothing that writes into the Postscript file
                   should be included here. */
            case FILLINDEX:       /*  Fill Bundle index  */
            case INTSTYLE:       /*  Interior Style  */
            case FILLCOLR:       /*  Fill Colour */
            case HATCHINDEX:       /*  Hatch Index  */
            case PATINDEX:       /*  Pattern Index  */
            case EDGEINDEX:       /*  Edge Bundle index  */
            case EDGETYPE:       /*  Edge Type  */
            case EDGEWIDTH:       /*  Edge Width */
            case EDGECOLR:       /*  Edge Colour  */
            case EDGEVIS:       /*  Edge Visibility  */
            case FILLREFPT:       /*  Fill Reference Point  */
               break;

            case PATTABLE:       /*  Pattern Table  */
               PSpattable( num, pi, cont_list );
               cont_list = (num < ZERO );
               break;

            case COLRTABLE:       /*  Colour Table  */
               if (PScoltab(pi,num))
               {
                  (void) sprintf(mess, "(code: 0x%x)", c);
                  (void) CGMerror(func, ERR_MAXCOLR, ERROR, mess);
               }
               break;
            case ASF:       /*  Aspect source flags  */
            case PATSIZE:       /*  Pattern Size  */
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVATT, ERROR, mess);
               break;
           }
           break;

        case 0x37:  /* Escape And External Elements  */
          switch (c)
          {
            case ESCAPE:         /*  Escape  */
               break;

            case MESSAGE:         /*  Message  */
               /* Ignore the action flag - always put message out */
               PUTMSG "%s\n", str);
               break;

            case APPLDATA:         /*  Application Data  */
               break;

            default:
               (void) sprintf( mess, "(code: 0x%x)", c);
               (void) CGMerror(func, ERR_INVESC, ERROR, mess);
               break;
          }
          break;

       default:
          (void) sprintf( mess, "(code: 0x%x)", c);
          (void) CGMerror(func, ERR_INVELEM, ERROR, mess);
          break;
   }
   return;
}

/*************************************************************************
 *     PSgetfd():   Function which provides the PostScript font details.
 *
 *                  IN: Pointer to a Textitem structure.
 *                  OUT: None.
 *                  RETURN VALUE: None.
 *
 *                  Author: WKL
 *
 * ALGORITHM
 * ---------
 *
 * Find out which font is requested, then read its font metrics file,
 * extract and process the font data information.
 *
 * LOCALS
 * ------
 * fnamestr
 * fpin
 * func
 * i
 * lnbuf
 * parsep
 * pathname
 */

#ifdef PROTO
void PSgetfd ( Textitem *thisitem )
#else
void PSgetfd (thisitem)
Textitem  *thisitem ;
#endif
{
   int i;
   char lnbuf[80], *parsep, fnamestr[100], *pathname;
   char *func="PSgetfd";
   FILE *fpin;

   if (ftnames==NULL)
      {
      /*
         Open the file which contains the list of supported PS fonts,
         one name per line.
      */
      fpin = CGMfopen((pathname=CGMpath("fontlist", "psd")), F_READ, CLEAR_TEXT);
      if (fpin == NULL)
         exit((int)CGMerror(func, ERR_OPENFILE, FATAL, pathname));

      /* free pathname - no longer needed */
      FREE(pathname);

      /*
         Read the entire file; dynamically create and store the list of
         all the fontnames; have ftnames (external pointer) point to the head
         of the list.
      */
      i=0;
      while (fgets(lnbuf,80,fpin))
      {
         /* Skip to the end and terminate the string. */
         parsep=lnbuf;
         while(*parsep!='\0' && *parsep!='\n')
            parsep++;
         *parsep='\0';

         /* First time allocate space space for ftnames, later reallocate */
         if (i==0)
            ftnames = (char**) MALLOC ( 1, sizeof(char*) );
         else
            ftnames=(char**)
                    realloc( (char*)ftnames, (size_t)(sizeof(char*)*(i+1)) );

         if (!ftnames)
            exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
         else
         {
            /* Allocate memory for one font name */
            ftnames[i] = (char *) MALLOC (1, 80 );
            if (!ftnames[i])
               exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
         }

         /* Store a fontname */
         (void)strcpy(ftnames[i],lnbuf);
         i++;
      }

      /* All fontnames read - close the file */
      (void)fclose(fpin);
   }

   /* Pick up default if user requested font outside our capabilities */
   if (thisitem->text_font >= i)
      thisitem->text_font = 0;

   /* Store the selected fontname into a local string */
   (void)strcpy(fnamestr,ftnames[thisitem->text_font]);

   /* Call routine to read font details for the selected font */
   PSreadafm(fnamestr, thisitem);
}

/************************************************************************
* PSoptions() : Parse options local to the PostScript driver.
*
* Input Arguments:
*     opts : Points to the options string.
* Output Arguments:
*     None.
* Return value:
*     None.
*
* Algorithm :
*
*
************************************************************************/

#ifdef PROTO
void PSoptions( char *opts )
#else
void PSoptions(opts)
char *opts;
#endif

{
   int i, len = strlen(opts);
   char *func="PSoptions",*temp=" ";

   for ( i=0; i < len; i++ )
   {
      switch ( toupper( opts[i] ) )
      {
         case 'C':  /*  Colour PostScript */
           col_mode = COLOUR;
           break;
         case 'M':  /*  Monochrome PostScript */
           col_mode = MONOCHROME;
           break;
         case 'E':  /*  Encapsulated PostScript */
           ps_mode = EPSF;
           break;
         case 'N':  /*  Normal PostScript */
           ps_mode = NORMALPS;
           break;
         case 'V':  /* Switch Border On */
           border_mode = VISIBLE;
           break;
         case 'I':  /* Make Border Transparent */
           border_mode = INVISIBLE;
           break;
         case 'P':  /* Set Paper to be Portrait */
           orient = PORTRAIT;
           break;
         case 'L':  /* Set Paper to be Landscape */
           orient = LANDSCAPE;
           break;
         case 'F':  /* Handle Background colour */
           background_colour = VISIBLE;
           break;
         case 'B':  /* Ignore Background colour */
           background_colour = INVISIBLE;
           break;
         case 'Q':
           PSdevice = 2;
           break;
         case '1':  /* Set different device */
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
           PSdevice = opts[i] - '1';
           if ( PSdevice > NDEVS ) PSdevice = 3;
           break;
         case 'D':  /* Decompose polygon if complex */
           poly_mode = DECOMPOSE;
           break;
         case 'W':  /* Polygon always sent as whole */
           poly_mode = WHOLE;
           break;
         default:   /*  Unknown Option */
           *temp = opts[i];
           (void)CGMerror(func,ERR_INVOPT,ERROR,temp);
           break;
      }
   }
   return;
}

/*********       Functions local to this module         ***********/

/********************************************************************
*  PSpoints() : Routine for outputing a list of points to the PostScript
*               output file. It is used in a few occassions such as set
*               of points for polygon polymarkers etc.
*               input arguments:
* Input Arguments:
*                n : number of points to be output
*                pi  : array of data (integers)
*                pr  : array of data (real numbers).
*                      pi and pr contains the coordinates of the
*                      points in this case. The sequence of the
*                      coordinates are stored as x1, y1, x2, y2...
*                      ... xn, yn .
*                set : SET/NOSET which is a flag to signify whether
*                      the list of points represents polygon set.
*                str1, str2 : string separators between points.
*                      str1 is the separator between the first and
*                      second point and str2 is the separator between
*                      subsequent points.
*  Output arguments:  None
*  Return value : None
********************************************************************/


#ifdef PROTO
static void PSpoints ( long n, Long *pi, Float *pr, Enum set,
                       char *str1, char *str2)
#else
static void PSpoints (n, pi, pr, set, str1, str2)

/*  Output 'n' points starting at pi/pr
    'set' indicates if this is a POLYGONSET   */

long n, *pi;
float *pr;
Enum set;
char *str1, *str2;
#endif

{
    register long i, k, no;
    float xmin, xmax, ymin, ymax;
    register Prec prec;

/*  Set number of points to print on each line  */

    if (cur.vdc_type == REAL)
    {
       prec = dec_right;
       no = 40 / (dec_all + 6*set + 4);
    }
    else
    {
       prec = cur.vdcint_bits*LOG2 + 1;
       no = 40 / (prec + 6*set + 5);
    }

    if ( n < 0 ) n = -n;
    for (i = 0, k = no+1 ; i < n ; i++ )
    {
       if ( !--k )
       {
           PSOUT "\n" );
           k = no;
       }
       if (cur.vdc_type == REAL)
       {
          PSOUT " %.*f %.*f ", (int)prec, (*pr), (int)prec, (*(pr+1)));
          if (i==0)
          {  xmax = xmin = (*pr);
             ymax=ymin =(*(pr+1));
          }
          else
          {  if ( *pr>xmax ) xmax = (*pr);
             else if ((*pr)<xmin) xmin = (*pr);
             if ( *(pr+1)>ymax ) ymax = (*(pr+1));
             else if ((*(pr+1))<ymin) ymin = (*(pr+1));
          }
          pr += 2;
       }
       else
       {
          PSOUT " %ld %ld ", (*pi), (*(pi+1)) );
          if ( i == 0 )
          {  xmax = xmin = (float)(*pi);
             ymax=ymin =(float)(*(pi+1));
          }
          else
          {  if ( (float)*pi > xmax ) xmax = (float)*pi;
             else if ((float)(*pi)<xmin) xmin =(float)*pi;
             if ( (float)*(pi+1) > ymax ) ymax = (float)*(pi+1);
             else if ( (float)*(pi+1) < ymin ) ymin =(float)*(pi+1);
          }
          pi += 2;
       }
       if (!i) PSOUT "%s",str1); else PSOUT"%s",str2);
    }
    return;
}

/********************************************************************
*  PSprprolog() : Output the time, creator etc to the PostScript file
*                 Read from the file prologue.ps and concatenate to
*                 the PostScript file
*  Input Argument:
*             str    : contains the name of the creator
*
* Output arguments:  None
* Return value : None
*
********************************************************************/

#ifdef PROTO
static void PSprprolog ( char *str )
#else
static void PSprprolog (str)
char *str ;
#endif
{
  char str1[256], *func="PSprprolog", *proname;

  FILE   *plog;
#ifdef ANSI
  time_t pstime;
#else
  long  pstime;
#endif

  if (ps_mode == EPSF)
    PSOUT  "%s\n","%!PS-Adobe-2.0 EPSF-2.0");
  else
    PSOUT  "%s\n","%!PS-Adobe-2.0");
  (void) strcpy(str1,"%%Creator: ");
  (void) strcat(str1, cgmversion);
  (void) strcat(str1, " from: ");
  (void) strcat(str1,str);
  PSOUT  "%s\n",str1 );

  (void) strcpy(str1,"%%Creation Date: ");
#ifdef ANSI
  pstime = time( NULL );
#else
  pstime = (long) time( (long *)NULL );
#endif
  (void) strcat (str1, asctime(localtime(&pstime)));
  PSOUT  "%s",str1);

  if (ps_mode == NORMALPS)
    PSOUT  "%s\n","%%Pages: (atend)");

  PSOUT  "%s\n","%%DocumentFonts: (atend)");

  if (ps_mode == EPSF)
    PSOUT  "%s\n","%%BoundingBox: (atend)");

  PSOUT  "%s\n","%%EndComments");

  /* Open file */
  plog = CGMfopen((proname = CGMpath("prologue","ps")), F_READ, CLEAR_TEXT);
  if( plog == NULL)
      /* Exit on the spot if open failed */
      exit (CGMerror(func, ERR_OPENFILE, FATAL, proname));

  /* free pathname - no longer needed */
  FREE(proname);

  while ( fgets(str1,100,plog))
     PSOUT  "%s",str1);

  (void)fclose(plog);


}

/*********************************************************************
* PSbegpic() :  Function to do the CGM BEGPIC element
* Input Arguments:
*     None.
* Output Arguments:
*     None.
* Return value:
*     None.
*
* NOTE:
*       dd_linewidth is the device dependant linewidth, based on the CGM
*       default for absolute/abstract width mode lines.

* Modification Log:
*   9 Sep 91 RTP  Changed aspect ratio calculations to allow output to fill
*                 the output page.
*  21 Oct 93 KEVP Corrected bounding Box Calculation and changed misleading
*                 variable name 'scale' to 'vdcbb'.
*
*********************************************************************/

#ifdef PROTO
static void PSbegpic ( void )
#else
static void PSbegpic ()
#endif
{
  double left, right, top, bot, vdcbb, scale1;
  float r=1.0, g=1.0, b=1.0;
  int l_marg, r_marg, u_marg, b_marg;
  float ps_xrange, ps_yrange;
  register float t, rpmin, rpmax, decrpmin;

       PSOUT "StartPage\n");
       if (cur.vdc_type == REAL)
       {
          left   = (double) cur.vdc_extent.a.x.real;
          bot    = (double) cur.vdc_extent.a.y.real;
          right  = (double) cur.vdc_extent.b.x.real;
          top    = (double) cur.vdc_extent.b.y.real;
       }
       else
       {
          left   = (double) cur.vdc_extent.a.x.intr;
          bot    = (double) cur.vdc_extent.a.y.intr;
          right  = (double) cur.vdc_extent.b.x.intr;
          top    = (double) cur.vdc_extent.b.y.intr;
       }

       if (backflag)
       {
         r = PSCOLOUR( cur.back.red, red);
         g = PSCOLOUR( cur.back.green, green);
         b = PSCOLOUR( cur.back.blue, blue);
       }

       if (cur.color_mode == INDEXED)
       {
         PScolour_table[0].red = r;
         PScolour_table[0].green = g;
         PScolour_table[0].blue = b;
       }

       vdc_xrange = FABS(right - left);
       vdc_yrange = FABS(top - bot);

       if ( dev_ori != orient )
       {
          l_marg = PSdev[PSdevice].y; r_marg = PSdev[PSdevice].Y;
          b_marg = PSdev[PSdevice].x; u_marg = PSdev[PSdevice].X;
       }
       else
       {
          l_marg = PSdev[PSdevice].x; r_marg = PSdev[PSdevice].X;
          b_marg = PSdev[PSdevice].y; u_marg = PSdev[PSdevice].Y;
       }
       ps_xrange = r_marg - l_marg;
       ps_yrange = u_marg - b_marg;

       /* RTP Work out scale in x and y and take minimum */
       if ( ps_yrange / ps_xrange > vdc_yrange / vdc_xrange )
       {  /* Bounding box touches left and right of display */
          touching_sides = LANDSCAPE;
          psbb = ps_xrange;
          vdcbb = vdc_xrange;
       }
       else
       {  /* Bounding box touches top and bottom of display */
          touching_sides = PORTRAIT;
          psbb = ps_yrange;
          vdcbb = vdc_yrange;
       }
       scale1 = psbb / vdcbb;
       share.ygrain = share.xgrain = PSdev[PSdevice].res*scale1/PT_PER_IN;
       dd_linewidth = vdcbb / 1000;

       /* Rotate paper anticlockwise translation!! */
       if (dev_ori != orient)
         PSOUT "90 rotate\n");

       /* Translate to margins if not an EPSF */
       if (ps_mode == NORMALPS)
       {
         if (dev_ori != orient)
           PSOUT "%d %d", PSdev[PSdevice].y, -PSdev[PSdevice].X);
         else
           PSOUT "%d %d", PSdev[PSdevice].x, PSdev[PSdevice].y);

         PSOUT " translate newpath\n");
       }
       else
       {
          if (dev_ori != orient)
          {
             PSOUT"0 -612");   /* Replace with EPSF rot. when dev's impl'd.*/
             PSOUT" translate newpath\n");
          }
       }

       /* Scale to VDC units */
       PSOUT "%f %f scale\n",(right-left)/vdc_xrange*scale1
                            ,(top-bot)/vdc_yrange*scale1);

       /* Translate to new origin */
       PSOUT "%f %f translate\n",-left,-bot);

       /* Set a default linewidth in VDC units */
       PSOUT "/dd_linewidth %.10f def\n",dd_linewidth);
       PSOUT "dd_linewidth setlinewidth\n");
       PSOUT "1 setlinejoin 1 setlinecap\n");
       PSOUT "%f %f m ",left, bot);
       PSOUT "%f %f l\n",right, bot);
       PSOUT "%f %f l ",right, top);
       PSOUT "%f %f l\n",left, top);
       PSOUT "closepath gsave gsave %f %f %f\n",r,g,b);
       PSOUT "setrgbcolor fill grestore 0 setgray\n");
       if (border_mode == VISIBLE)
         PSOUT "S ");
       PSOUT "grestore clip newpath\n");
       clip_change = FALSE;

       /* Set number of decimal digits for real points */
       if(share.xgrain >= share.ygrain) rpmin = 1.0/share.xgrain;
       else                             rpmin = 1.0/share.ygrain;

       for(decrpmin=1.0; decrpmin > rpmin; decrpmin = 0.1*decrpmin);

       tzytol = rpmin;
       if(rpmin < cur.vdcmin) rpmin = cur.vdcmin;

       if(FABS(left) < FABS(right)) rpmax = FABS(right);
       else                         rpmax = FABS(left);
       if(rpmax < FABS(top))        rpmax = FABS(top);
       if(rpmax < FABS(bot))        rpmax = FABS(bot);

       for(dec_all=0,   t=decrpmin; t<=rpmax; dec_all++,   t *= 10.0);
       for(dec_right=0, t=rpmin; t<1.0;   dec_right++, t *= 10.0);
       if(dec_right != 0) dec_all++;  /* Count decimal point */

       return;
}

/*********************************************************************
* PSattrib() :  Function to change the attributes for PostScript
*               output.
* Input Arguments:
*     type : type of attribute ( LINE, MARKER, POLYGON, EDGE, TEXT )
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSattrib ( Code prim )
#else
static void PSattrib ( prim )
Code prim;
#endif
{
   float red, green, blue, width, size;
   Index type;

   /* Update Clipping, If necessary */
   if(clip_change)
   {
       PSOUT "initclip\n" );
       if ( cur.clip_ind == ON )
       {
          PSOUT "newpath" );
          if (cur.vdc_type == REAL)
          {
                PSOUT " %f %f m", cur.clip_rect.a.x.real,
                           cur.clip_rect.a.y.real);
                PSOUT " %f %f l\n", cur.clip_rect.b.x.real,
                           cur.clip_rect.a.y.real);
                PSOUT " %f %f l", cur.clip_rect.b.x.real,
                           cur.clip_rect.b.y.real);
                PSOUT " %f %f l", cur.clip_rect.a.x.real,
                           cur.clip_rect.b.y.real);
          }
          else
          {
                PSOUT " %ld %ld m", cur.clip_rect.a.x.intr,
                           cur.clip_rect.a.y.intr);
                PSOUT " %ld %ld l\n", cur.clip_rect.b.x.intr,
                           cur.clip_rect.a.y.intr);
                PSOUT " %ld %ld l", cur.clip_rect.b.x.intr,
                           cur.clip_rect.b.y.intr);
                PSOUT " %ld %ld l", cur.clip_rect.a.x.intr,
                                cur.clip_rect.b.y.intr);
          }
          PSOUT " closepath clip newpath\n");
      }
      clip_change = FALSE;
   }

   /* Set Attributes for Primitive */
   switch ( prim )
   {
      case LINE:
         if (line_att_changed == FALSE) return;

   /* set attributes according the line bundle and index curatt.line_ind */
         ATTline(NLINEB, PSline_bundle, cur.max_colind, &PSlineatt);
         if ( cur.color_mode == INDEXED )
         {
            red   = PScolour_table[PSlineatt.colindex].red;
            green = PScolour_table[PSlineatt.colindex].green;
            blue  = PScolour_table[PSlineatt.colindex].blue;
         }
         else
         {
            red   = PSCOLOUR( PSlineatt.col.red, red );
            green = PSCOLOUR( PSlineatt.col.green, green );
            blue  = PSCOLOUR( PSlineatt.col.blue, blue );
         }
         if (PSlineatt.widthmode == SCALED)
            width = dd_linewidth * PSlineatt.width;
         else
            width = PSlineatt.width;

         type = PSlineatt.type;

         PSOUT "%f %f %f %ld %f clnstat\n", red, green, blue, (long)type, width);
         line_att_changed = FALSE;
         break;

      case MARKER:
         ATTmarker(NMARKB, PSmark_bundle, cur.max_colind, &PSmarkatt );
         if ( cur.color_mode == INDEXED )
         {
            red   = PScolour_table[PSmarkatt.colindex].red;
            green = PScolour_table[PSmarkatt.colindex].green;
            blue  = PScolour_table[PSmarkatt.colindex].blue;
         }
         else
         {
            red   = PSCOLOUR( PSmarkatt.col.red, red );
            green = PSCOLOUR( PSmarkatt.col.green, green );
            blue  = PSCOLOUR( PSmarkatt.col.blue, blue );
         }
         if (PSmarkatt.sizemode == SCALED)
            /*
             Use abstract size default as device dependant default for PS.
             i.e. 100th of the largest side of the vdc rectangle.
            */
            size = (vdc_xrange < vdc_yrange ? vdc_yrange : vdc_xrange)
                   / 100.0 * PSmarkatt.size;
         else
            size = PSmarkatt.size;

         type = PSmarkatt.type;

         PSOUT "%f %f %f %ld %f pmstat\n", red, green, blue,(long)type, size);
         line_att_changed = TRUE;
         break;

      case TEXT:
         break;
      case POLYGON:
         ATTfill(NFILLB, PSfill_bundle, cur.max_colind, &PSfillatt);
         fill_att_have = TRUE;
         break;
      case EDGEVIS:
      default:
         break;
   }
   return;
}


/*********************************************************************
* PSfill() :  Function to change the fill areas  for  PostScript
*             output.
* Input Arguments:
*     None.
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSfill ( Enum rule, Enum edge_vis )
#else
static void PSfill (rule, edge_vis)
Enum rule;       /* Filling  rule */
Enum edge_vis;   /* Edge visibility */
#endif
{
   float r, g, b, width;
   Index int_style, hatch_ind, pat_ind ;
   Index type;
   double modua, modub, sina, sinb, cosa, cosb, refx, refy;
   int i, k;
   Logical foundidx;

   /* set attributes according the fill bundle and index curatt.fill_ind */

   if(!fill_att_have)
      ATTfill(NFILLB, PSfill_bundle, cur.max_colind, &PSfillatt);
   if ( cur.color_mode == INDEXED )
   {
      r   = PScolour_table[PSfillatt.colindex].red;
      g = PScolour_table[PSfillatt.colindex].green;
      b  = PScolour_table[PSfillatt.colindex].blue;
   }
   else
   {
      r   = PSCOLOUR( PSfillatt.col.red, red );
      g = PSCOLOUR( PSfillatt.col.green, green );
      b  = PSCOLOUR( PSfillatt.col.blue, blue );
   }

   int_style = PSfillatt.style;
   hatch_ind = PSfillatt.hatch;
   pat_ind = PSfillatt.pattern;

   /* fill interior */
   switch (int_style)
   {
      case HOLLOW:      /* stroke boundary with fill colour  */
                        /* if edge is invisible              */
         if (edge_vis == VIS) break;
         PSOUT " closepath gsave\n");
         PSOUT "%f %f %f setrgbcolor S\n", r, g, b);
         PSOUT "grestore\n");
         break;

      case SOLID:       /* fill interior  with fill colour  */
         if (rule==NZ)
            PSOUT " closepath gsave  clip \n");
         else
            PSOUT " closepath gsave  eoclip \n");
         PSOUT "%f %f %f setrgbcolor fill\n", r, g, b);
         PSOUT "grestore\n");
         break;

      case PATTERN:     /* fill interior with predefined pattern  */
         if (rule==NZ)
            PSOUT " closepath gsave clip \n");
         else
            PSOUT " closepath gsave eoclip \n");

         if (cur.vdc_type == REAL)
         {
            modua = SQRT( curatt.pat_size.a.x.real*curatt.pat_size.a.x.real +
                    curatt.pat_size.a.y.real*curatt.pat_size.a.y.real );
            modub = SQRT( curatt.pat_size.b.x.real*curatt.pat_size.b.x.real +
                          curatt.pat_size.b.y.real*curatt.pat_size.b.y.real );
            sina = curatt.pat_size.a.y.real / modua;
            cosa = curatt.pat_size.a.x.real / modua;
            sinb = curatt.pat_size.b.y.real / modub;
            cosb = curatt.pat_size.b.x.real / modub;
            refx = curatt.fill_ref.x.real * cosb +
                   curatt.fill_ref.y.real * sinb ;
            refy = curatt.fill_ref.x.real * cosa +
                   curatt.fill_ref.y.real * sina ;
         }
         else
         {
            modua = SQRT( curatt.pat_size.a.x.intr*curatt.pat_size.a.x.intr +
                          curatt.pat_size.a.y.intr*curatt.pat_size.a.y.intr );
            modub = SQRT( curatt.pat_size.b.x.intr*curatt.pat_size.b.x.intr +
                          curatt.pat_size.b.y.intr*curatt.pat_size.b.y.intr );
            sina = (double)curatt.pat_size.a.y.intr / modua;
            cosa = (double)curatt.pat_size.a.x.intr / modua;
            sinb = (double)curatt.pat_size.b.y.intr / modub;
            cosb = (double)curatt.pat_size.b.x.intr / modub;
            refx = curatt.fill_ref.x.intr * cosb +
                   curatt.fill_ref.y.intr * sinb ;
            refy = curatt.fill_ref.x.intr * cosa +
                   curatt.fill_ref.y.intr * sina ;
         }

         foundidx = FALSE;
         for (i=0;i<npspat;i++)
         {
            if ( PSpat_table[i].patidx == pat_ind )
            {
               pat_ind = i;
               foundidx = TRUE;
               break;
            }
         }
         if (!foundidx) pat_ind = 0;

         PSOUT "[ %f %f %f %f 0 0 ] concat\n", cosb,sinb, cosa,sina);
         PSOUT "%d %d %f %f %f %f fapi\n", PSpat_table[pat_ind].nx,
             PSpat_table[pat_ind].ny, refx, refy, modub, modua);

         k = ( col_mode==COLOUR ? 13 : 40 );
         k = ( k > PSpat_table[pat_ind].nx ? PSpat_table[pat_ind].nx : k );

         if ( PSpat_table[pat_ind].pathead[0] == INDPATTERN )
            for (i = 1; i < (PSpat_table[pat_ind].ny*PSpat_table[pat_ind].nx*3)
                 ;i+=3)
            {
               r =PScolour_table[(int)PSpat_table[pat_ind].pathead[i]].red;
               g =PScolour_table[(int)PSpat_table[pat_ind].pathead[i]].green;
               b =PScolour_table[(int)PSpat_table[pat_ind].pathead[i]].blue;
               if ( ((i-1)/3)%k == 0 ) PSOUT "\n");
               if (col_mode == COLOUR)
                  PSOUT "%02x%02x%02x",(int)
                         (r*255.0),(int)(g*255.0),(int)(b*255.0));
               else
                  PSOUT "%02x",(int)(255*(0.59*r+0.31*g+0.1*b)));
            }
         else
            for (i=0; i < (PSpat_table[pat_ind].ny*PSpat_table[pat_ind].nx*3);)
            {
               if ( i%k == 0 ) PSOUT "\n");
               r = PSCOLOUR( PSpat_table[pat_ind].pathead[i++], red );
               g = PSCOLOUR( PSpat_table[pat_ind].pathead[i++], green );
               b = PSCOLOUR( PSpat_table[pat_ind].pathead[i++], blue );
               if (col_mode == COLOUR)
                  PSOUT "%02x%02x%02x",(int)
                        (r*255.0),(int)(g*255.0),(int)(b*255.0));
               else
                  PSOUT "%02x",(int)(255.*(0.59*r+0.31*g+0.1*b)));
            }

         PSOUT "\nfapdo grestore\n");
         break;

      case HATCH:  /* fill interior depending on the style requested */

         /* Close the path and clip according to the current clipping rule */
         if (rule == NZ)
            PSOUT "closepath gsave clip\n");
         else
            PSOUT "closepath gsave eoclip\n");

         /* Send out line attributes */
         PSOUT "%f setlinewidth %f %f %f setrgbcolor [] 0 setdash\n",
                dd_linewidth, r, g, b);

         /* Don't want negative style indices */
         if ( cgmralgks && hatch_ind < 0 ) hatch_ind = -hatch_ind;

         /*
           Group styles for efficiency - reduce everything to horizontal
           and/or vertical hatching.
         */
         switch (hatch_ind)
         {
              case 3:
                  PSOUT "%%negative slope hatch: rotate + horizontal\n");
                  PSOUT "45 rotate\n");
              case 1:
                  PSOUT "%%horizontal hatch\n");

                  /* get bounding box coordinates in PostScript */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def newpath\n");

                  /* set up a loop for drawing horizontal lines */
                  PSOUT "ymin %f ymax {dup xmin exch m xmax exch l S} for\n",
                        5*dd_linewidth);
                  break;

              case 4:
                  PSOUT "%%positive slope hatch: rotate + vertical\n");
                  PSOUT "45 rotate\n");
              case 2:
                  PSOUT "%%vertical hatch\n");

                  /* get bounding box coordinates in PostScript */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def newpath\n");

                  /* set up a loop for drawing horizontal lines */
                  PSOUT "xmin %f xmax {dup ymin m ymax l S} for\n",
                         5*dd_linewidth);
                  break;

              case 6:
                  PSOUT "%%sloping cross hatch: rotate + h&v hatch\n");
                  PSOUT "45 rotate\n");
              case 5:
                  PSOUT "%%horizontal & vertical cross hatch\n");

                  /* do the vertical first */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def newpath\n");
                  PSOUT "xmin %f xmax {dup ymin m ymax l S} for\n",
                         5*dd_linewidth);

                  /* now do the horizontal */
                  PSOUT "ymin %f ymax {dup xmin exch m xmax exch l S} for\n",
                         5*dd_linewidth);
                  break;

              case 7:
                  PSOUT "%%RAL GKS like double spaced horizontal & sloping\n");
                  /* do the horizontal first */
                  PSOUT "gsave\n");
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def newpath\n");
                  PSOUT "ymin %f ymax {dup xmin exch m xmax exch l S} for\n",
                         10*dd_linewidth);
                  PSOUT "grestore\n");

                  /* prepare for sloping */
                  PSOUT "gsave\n");
                  PSOUT "30 rotate\n");

                  /* do the double spaced negative  */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def\n");
                  PSOUT "ymin 1.1547 %f mul ymax ", 10*dd_linewidth);
                  PSOUT "{dup xmin exch m xmax exch l S} for\n");
                  PSOUT "grestore\n");

                  PSOUT "-30 rotate\n");

                  /* do the double spaced negative  */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def\n");
                  PSOUT "ymin 1.1547 %f mul ymax ", 10*dd_linewidth);
                  PSOUT "{dup xmin exch m xmax exch l S} for\n");
                  break;

              case 8:
                  PSOUT "%% RAL GKS like double spaced & crossing\n");

                  /* do the first vertical */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def newpath\n");
                  PSOUT "xmin %f xmax {dup ymin m ymax l S} for\n",
                         10*dd_linewidth);

                  /* do the second vertical */
                  PSOUT "xmin %f add %f xmax {dup ymin m ymax l S} for\n",
                         2*dd_linewidth, 10*dd_linewidth);

                  /* now do the first horizontal */
                  PSOUT "ymin %f ymax {dup xmin exch m xmax exch l S} for\n",
                         10*dd_linewidth);

                  /* finally do the second horizontal */
                  PSOUT "ymin %f add %f ymax ",
                        2*dd_linewidth, 10*dd_linewidth);
                  PSOUT "{dup xmin exch m xmax exch l S} for\n");
                  break;

              case 9:
                  PSOUT "%% RAL GKS double spaced crossing & sloping\n");

                  /* prepare for sloping */
                  PSOUT "45 rotate\n");

                  /* do the first negative */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def\n");
                  PSOUT "xmin %f xmax {dup ymin m ymax l S} for\n",
                         10*dd_linewidth);

                  /* do the second negative */
                  PSOUT "xmin %f add %f xmax {dup ymin m ymax l S} for\n",
                         2*dd_linewidth, 10*dd_linewidth);

                  /* do the first positive */
                  PSOUT "ymin %f ymax {dup xmin exch m xmax exch l S} for\n",
                         10*dd_linewidth);

                  /* do the second positive */
                  PSOUT "ymin %f add %f ymax ",
                         2*dd_linewidth, 10*dd_linewidth);
                  PSOUT "{dup xmin exch m xmax exch l S} for\n");
                  break;

              default :    /* horizontal hatch */
                  PSOUT "%%horizontal hatch\n");

                  /* get bounding box coordinates in PostScript */
                  PSOUT "pathbbox /ymax exch def /xmax exch def ");
                  PSOUT "/ymin exch def /xmin exch def newpath\n");

                  /* set up a loop for drawing horizontal lines */
                  PSOUT "ymin %f ymax {dup xmin exch m xmax exch l S} for\n",
                         5*dd_linewidth);
                  break;

         } /* switch (hatch_index)  */

         PSOUT "grestore\n");
         break;

      default :     /* Empty : do nothing but closing the path*/
         PSOUT "closepath\n");
         break;
   }/* switch (int_style)  */

   fill_att_have = FALSE;       /* Lose fill attributes */

   /***** Do the edges of the filled area *****/
   if (edge_vis == VIS) PSedge();

   return;
}

/*********************************************************************
* PSedge() :  Function to make the current path into an edge (set)
*
* Input Arguments:
*     None.
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSedge ( void )
#else
static void PSedge ( )
#endif
{
   float r, g, b, width;
   Index type;

   /* set attributes according the edge bundle and index curatt.edge_ind */
   ATTedge(NEDGEB, PSedge_bundle, cur.max_colind, &PSedgeatt);

   if ( cur.color_mode == INDEXED )
   {
      r   = PScolour_table[PSedgeatt.colindex].red;
      g = PScolour_table[PSedgeatt.colindex].green;
      b  = PScolour_table[PSedgeatt.colindex].blue;
   }
   else
   {
      r   = PSCOLOUR( PSedgeatt.col.red, red );
      g = PSCOLOUR( PSedgeatt.col.green, green );
      b  = PSCOLOUR( PSedgeatt.col.blue, blue );
   }

   if (cur.edgewidth_mode == SCALED)
      width = dd_linewidth*PSedgeatt.width;
   else
      width = (float) PSedgeatt.width;
   type = PSedgeatt.type;

   /* Output command to convert current path to edge */
   PSOUT "%f %f %f %ld %f clnstat S\n", r, g, b,(long)type, width);
   line_att_changed = TRUE;

   return;
}

/*********************************************************************
* PSins_pattab() :
* For the initial installation  of the pattern table
* argument i is the index of the table
* arguments d1, .. , d8 are integers representing 8 8 bit patterns
* As a result a 8x8 bit pattern is installed for index i
* The data to specify the ellipse is passed through 2 arrays pi, and pr.
* If the current vdc type is real data is passed through pr else data
* is passed through pi.
* Pattern defined with direct colour regardless of current mode to
* keep the pattern consistent throughout the picture.
* Input Arguments:
*     i  : Index of the pattern to be installed
*     d1 .. d8 : integers representing the bit map of the pattern
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSins_pattab( int i, int d1, int d2, int d3, int d4,
                                 int d5, int d6, int d7, int d8 )
#else
static void PSins_pattab(i, d1, d2, d3, d4, d5, d6, d7, d8)
int i, d1, d2, d3, d4, d5, d6, d7, d8;
#endif
{
   int k, j, pat[8];
   float *pii, tmp;
   pat[0]=d1;pat[1]=d2;pat[2]=d3;pat[3]=d4;
   pat[4]=d5;pat[5]=d6;pat[6]=d7;pat[7]=d8;
   PSpat_table[i].patidx = (Index)i;
   PSpat_table[i].nx = 8;
   PSpat_table[i].ny = 8;
   PSpat_table[i].prec = 10;
   PSpat_table[i].pathead = (float *) MALLOC ( 8*8*3, sizeof(float) );
   pii = PSpat_table[i].pathead;
   for (k=7; k>=0; k--)
      for (j=7;j>=0;j--)
      {
         *(pii++) = tmp = 255.0*(float)((pat[k] >> j) & 1);
         *(pii++) = tmp;
         *(pii++) = tmp;
      }
}


/*********************************************************************
* PSins_coltab() :
* For the initial installation  of the colour table
* There are 64 entries index form 0 to 63 to be installed in the
* colour table when initialized.
* PScolour_table is allocated with memory storage and the entries which
* are RGB triples are put in.
* Input Arguments:
*     None.
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSins_coltab( void )
#else
static void PSins_coltab()
#endif
{
   register int i;
   size_t maxind = cur.max_colind+1;

   if ( maxind < 256 ) maxind = 256;
   PScolour_table = (Colourentry*) calloc( maxind, sizeof(Colourentry) );

   PScolour_table[0] = PSdftcoltab[0];  /* White Background */
   PScolour_table[1] = PSdftcoltab[1];  /* Black Foreground */

   if (col_mode == COLOUR)
      for (i=2;i<256;i++)
      {
         PScolour_table[i].red = (float)GKScol[i].red/255.0;
         PScolour_table[i].green = (float)GKScol[i].green/255.0;
         PScolour_table[i].blue = (float)GKScol[i].blue/255.0;
      }
   else
      for (i=2;i<COLTAB_N;i++)
         PScolour_table[i] = PSdftcoltab[i];
}


/*********************************************************************
* PSarcref() : For calculating the centre, start angle and end angle
*              for an arc  given 3 points on the curve.
* These 3 point are (xa,ya) (xb,yb) and (xc,yc) which are the start
* intermediate and end point respectively.
* Results are put in the structure ref which contains the
* coordinates of the centre of the arc, start angle and end angle
* in radian
* Input Arguments:
*    ref : Pointer to the structure containing the required information
*          --- x coord of centre, y coord of centre, radius, start angle,
*          end angle.
*     xa : x coord of the start point of the arc
*     ya : y coord of the start point of the arc
*     xb : x coord of the intermediate point of the arc
*     yb : y coord of the intermediate point of the arc
*     xc : x coord of the end point of the arc
*     yc : y coord of the end point of the arc
* Output Arguments:
*     ref : Pointer to the structure containing required info.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static Enum PSarcref ( PS_arcref *ref, double xa, double ya,
                       double xb, double yb, double xc, double yc)
#else
static Enum PSarcref (ref, xa, ya, xb, yb, xc, yc)
PS_arcref *ref;
double xa, ya, xb, yb, xc, yc;
#endif
{
  double xab, yab, absq, xac, yac, acsq, xbc, ybc, bcsq;
  double xao, yao, xco, yco;
  double abc, area, dtemp;
  double delta = 10e-5;
  double iclock = 1.0;
  double vdc_range;

  xab = xb - xa;  yab = yb - ya; absq = SQR(xab) + SQR(yab);
  xac = xc - xa;  yac = yc - ya; acsq = SQR(xac) + SQR(yac);

  /*
   If start and end points coincide, draw full circle assuming
   b is directly opposite a and c.
  */
  if (acsq < delta*absq)
  {
     ref->cent.x = (xa+xb) / 2.0;
     ref->cent.y = (ya+yb) / 2.0;
     ref->r = SQRT( (xa - ref->cent.x)*(xa - ref->cent.x)
                    + (ya - ref->cent.y)*(ya - ref->cent.y) );
     ref->startang = 0.0;
     ref->endang = 2.0 * PI;
     return(ARC_OK);
  }

  xbc = xc - xb; ybc = yc - yb; bcsq = SQR(xbc) + SQR(ybc);
  ref->start.x = xa; ref->start.y = ya;
  ref->end.x = xc;   ref->end.y = yc;

  /*
   If intermediate point coincides with either end point, then
   draw a line between the end points.
  */
  if ( (absq < delta*acsq) || (bcsq < delta*acsq) )
  {
     /*
      draw straight line
     */
     return(ARC_LINE);
  }

  abc = xab*ybc - xbc*yab;
  area = (double) FABS( abc );

  /*
   If all three points are collinear, draw a straight line between the
   start and end points if the intermediate point lies between them,
   if it does not lie between them, then draw two semi-infinite lines
   from the start point toward the end point, and vice versa.
  */
  if ( area < delta * ( absq + bcsq ))
  {
     /*
      intermediate point lies between start and end points
     */
     if ( (xab*xbc == (double) FABS(xab*xbc)) &&
          (yab*ybc == (double) FABS(yab*ybc)) )
     {
        /* draw straight line */
        return(ARC_LINE);
     }

     /*
      intermediate point lies outside of the start/end points
     */
     else
     {
        /* draw semi-infinite lines */
        return(ARC_SLINE);
     }

  }

  vdc_range = (double) SQRT( ( vdc_xrange + vdc_yrange ) );

  /*
   If the sum of the sides is small enough when compared against the
   vdc_extent, then assume points to be coincidental and draw a dot.
  */
  if ( (absq + bcsq + acsq) < ( delta * vdc_range ) )
  {
     /* draw a dot */
     return(ARC_DOT);
  }

  /*
   normal arc, so derive data accordingly
  */
  else
  {
     iclock = (abc/area) ;
     ref->cent.x = xb - (absq*ybc + bcsq*yab)/2.0/abc;
     ref->cent.y = yb + (absq*xbc + bcsq*xab)/2.0/abc;
     xao = xa - ref->cent.x;
     yao = ya - ref->cent.y;
     xco = xc - ref->cent.x;
     yco = yc - ref->cent.y;
     ref->r = SQRT(xao*xao + yao*yao);

     ref->startang = asin(yao/ref->r);
     if ((ref->startang>0)&&(xao<0))
        ref->startang = PI - ref->startang;
     else if ((ref->startang<0)&&(xao<0))
        ref->startang = PI - ref->startang;
     else if ((ref->startang<0)&&(xao>0))
        ref->startang = 2.0*PI + ref->startang;

     ref->endang = asin(yco/ref->r);
     if ((ref->endang>0)&&(xco<0))
        ref->endang = PI - ref->endang;
     else if ((ref->endang<0)&&(xco<0))
        ref->endang = PI - ref->endang;
     else if ((ref->endang<0)&&(xco>0))
        ref->endang = 2.0*PI + ref->endang;
  }

  if (iclock<0.0)
  {
     dtemp = ref->endang;
     ref->endang = ref->startang;
     ref->startang = dtemp;
  }
  return(ARC_OK);
}



/*********************************************************************
* PSarcctr() : PostScript Output for CGM arc centre element given
*              the coordinates of the centre , start angle and
*              end angle of the arc.
* Input Arguments:
*     pi : integer data
*     pr : floating point data
*     closed : 0/1 , closed, not closed
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSarcctr( Long *pi, Float *pr, Enum closed )
#else
static void PSarcctr(pi,pr,closed)
Long   *pi;
Float  *pr;
Enum closed;  /* 0 : not closed, 1: closed */
#endif
{
   double xao, yao, xco, yco;
   PS_arcref arcref;

   PSOUT "gsave newpath  %%create path of cir arc ctr\n");
   if (cur.vdc_type == REAL)
   {
      arcref.cent.x = (double)pr[0];
      arcref.cent.y = (double)pr[1];
      xao = (double)pr[2];
      yao = (double)pr[3];
      xco = (double)pr[4];
      yco = (double)pr[5];
      arcref.r = (double)pr[6];
   }
   else
   {
      arcref.cent.x = (double)pi[0];
      arcref.cent.y = (double)pi[1];
      xao = (double)pi[2];
      yao = (double)pi[3];
      xco = (double)pi[4];
      yco = (double)pi[5];
      arcref.r = (double)pi[6];
   }
   arcref.startang =  atan2(yao, xao);
   if (arcref.startang < 0.)
       arcref.startang += 2.0*PI;
   arcref.endang = atan2(yco, xco);
   if (arcref.endang < 0.)
      arcref.endang += 2.0*PI;

   if (closed == NOTCLOSED)
   {
      PSattrib ( LINE );
      PSOUT " %f %f %f ",
             arcref.cent.x, arcref.cent.y, arcref.r);
      PSOUT " %f %f arc stroke\n",
             radtodegree*arcref.startang, radtodegree*arcref.endang);
      PSOUT "grestore\n");
      return;
   }
   if (*(pi+7)) /* pi[7]==1 for chord close type */
   {
      PSOUT " %f %f %f ",
             arcref.cent.x, arcref.cent.y, arcref.r);
      PSOUT " %f %f arc closepath\n",
             radtodegree*arcref.startang, radtodegree*arcref.endang);
   } else     /* pi[7]==0 for pie close type */
   {
     PSOUT " %f %f moveto ", arcref.cent.x, arcref.cent.y);
     PSOUT " %f %f lineto \n",
               arcref.cent.x+arcref.r*cos(arcref.startang),
               arcref.cent.y+arcref.r*sin(arcref.startang));
     PSOUT " %f %f %f ",
               arcref.cent.x, arcref.cent.y, arcref.r);
     PSOUT " %f %f arc closepath\n",
                radtodegree*arcref.startang, radtodegree*arcref.endang);
   }
   PSfill (NZ, curatt.edge_vis);
   PSOUT "grestore\n");
}



/*********************************************************************
* PSellipse() : PostScript Output for CGM ellipse element given
*              the coordinates of the centre , start angle and
*              end angle of the arc.
* The data to specify the ellipse is passed through 2 arrays pi, and pr.
* If the current vdc type is real data is passed through pr else data
* is passed through pi.
*
* In each case the p?[0] = x coord of the centre
*                  p?[1] = y coord of the centre
*                  p?[2] = x coord of first CDP end point
*                  p?[3] = y coord of first CDP end point
*                  p?[4] = x coord of second CDP end point
*                  p?[5] = y coord of second CDP end point
*
* Input Arguments:
*     pi : integer data
*     pr : floating point data
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSellipse( Long *pi, Float *pr)
#else
static void PSellipse(pi,pr)
Long   *pi;
Float  *pr;
#endif
{
  double modua, modub,sina, cosa, sinb,  cosb, refx, refy, det;
  double save1x, save1y, save2x, save2y;

  PSOUT "gsave newpath  %%create new path for ellipse\n");
  /*** Calculate the transformation for the anisotropic ellipse ****/
  /*** such that a unit circle is transformed to the specified  ****/
  /*** ellipse.                                                 ****/
  if (cur.vdc_type == REAL)
  {
     refx = (double)pr[0]; refy = (double)pr[1];
     save1x = (double)pr[2];
     save1y = (double)pr[3];
     save2x = (double)pr[4];
     save2y = (double)pr[5];
     modua = SQRT( (pr[2]-pr[0])*(pr[2]-pr[0]) +
                   (pr[3]-pr[1])*(pr[3]-pr[1]) );
     /*
       If CDP1 and the Centre Point are coincident, no unique ellipse can
       be constructed, so we return.
     */
     if (FABS(modua) <= cur.realmin)
     {
        PSOUT "grestore\n");
        return;
     }
     modub = SQRT( (pr[4]-pr[0])*(pr[4]-pr[0]) +
                   (pr[5]-pr[1])*(pr[5]-pr[1]) );
     /*
       If CDP2 and the Centre Point are coincident, no unique ellipse can
       be constructed, so we return.
     */
     if (FABS(modub) <= cur.realmin)
     {
        PSOUT "grestore\n");
        return;
     }
     sina = (pr[3]-pr[1]) / modua;
     cosa = (pr[2]-pr[0]) / modua;
     sinb = (pr[5]-pr[1]) / modub;
     cosb = (pr[4]-pr[0]) / modub;
    }
  else
  {
     refx = (double)pi[0]; refy = (double)pi[1];
     save1x = (double)pi[2];
     save1y = (double)pi[3];
     save2x = (double)pi[4];
     save2y = (double)pi[5];
     modua = SQRT( (pi[2]-pi[0])*(pi[2]-pi[0]) +
                   (pi[3]-pi[1])*(pi[3]-pi[1]) );
     /*
       If CDP1 and the Centre Point are coincident, no unique ellipse can
       be constructed, so we return.
     */
     if (FABS(modua) <= cur.realmin)
     {
        PSOUT "grestore\n");
        return;
     }
     modub = SQRT( (pi[4]-pi[0])*(pi[4]-pi[0]) +
                   (pi[5]-pi[1])*(pi[5]-pi[1]) );
     /*
       If CDP2 and the Centre Point are coincident, no unique ellipse can
       be constructed, so we return.
     */
     if (FABS(modub) <= cur.realmin)
     {
        PSOUT "grestore\n");
        return;
     }
     sina = (pi[3]-pi[1]) / modua;
     cosa = (pi[2]-pi[0]) / modua;
     sinb = (pi[5]-pi[1]) / modub;
     cosb = (pi[4]-pi[0]) / modub;
  }


  det = modua*cosa*modub*sinb - modua*sina*modub*cosb;
  if (FABS(det) <= cur.realmin)
  {
     if (FABS(modua-modub)<= cur.realmin)
     {
       /* CDP1 and CDP2 are on a circle */
       if (cur.vdc_type == REAL)
          PSOUT " %f %f %f 0 360 arc ", pr[0], pr[1], modua );
       else
          PSOUT " %ld %ld %ld 0 360 arc ", pi[0], pi[1], (long)modua);
        PSfill (NZ, curatt.edge_vis);
     }
     else
        /* CDP1 and CDP2 are on a line */
        PSOUT "%f %f moveto %f %f lineto stroke\n",
              save1x, save1y, save2x, save2y);
     PSOUT "grestore\n");
     return;
  }

  /*** Output to PostScript to concatenate the transformation ***/
  PSOUT "/mat1 {[ %f %f %f %f %f %f ] } def\n",
        modua*cosa, modua*sina, modub*cosb, modub*sinb, refx, refy);
  PSOUT "mat1 concat\n");
  /*** Output to PostScript to do a unit circle after         ***/
  /*** the transformation                                     ***/
  PSOUT "0 0 1 0 360 arc\n");
  PSOUT "mat1 mat1 invertmatrix concat\n");
  /*** Do the elliptical fill are according to current settings ***/
  PSfill (NZ, curatt.edge_vis);
  PSOUT "grestore\n");
}


/*********************************************************************
* PSelarc() :  PostScript Output for CGM elliptical arc element given
*              given the coordinates of the centre , start angle end
*              angle, a start vector and an end vector of the elliptical
*              arc.
* The data to specify the ellipse is passed through 2 arrays pi, and pr.
* If the current vdc type is real data is passed through pr else data
* is passed through pi.
*
* In each case the p?[0] = x coord of the centre
*                  p?[1] = y coord of the centre
*                  p?[2] = x coord of first CDP end point
*                  p?[3] = y coord of first CDP end point
*                  p?[4] = x coord of second CDP end point
*                  p?[5] = y coord of second CDP end point
*                  p?[6] = x coord of start vector
*                  p?[7] = y coord of start vector
*                  p?[8] = x coord of end vector
*                  p?[9] = y coord of end vector
*
* If a closed ellipital arc is required, (i.e. closed==1), pi[10]
* contains the close type where
* close type == pie when p?[10]== 0   and
* close type == chord when p?[10]==1
* Input Arguments:
*     pi : integer data
*     pr : floating point data
*     closed : 0/1 , closed, not closed
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void PSelarc( Long *pi, Float *pr, Enum closed)
#else
static void PSelarc(pi,pr,closed)
Long *pi;
Float *pr;
Enum closed;  /* 0 : not closed, 1: closed */
#endif
{
  double cdp1x, cdp1y, cdp2x, cdp2y, refx, refy;
  double stx, sty, enx, eny; /* start x,y and end x,y */
  double modua, modub, sina, cosa, sinb, cosb;
  double det; /* determinant for inverse transform */
  double temp1, save1x, save1y, save2x, save2y;
  long calli[8];
  float callr[7];

  PSOUT "gsave newpath  %%start new path for elliptical arc\n");
  if (cur.vdc_type == REAL)
  {
     /* pick-up the arguments */
     refx = (double)pr[0];
     refy = (double)pr[1];
     cdp1x = save1x = (double)pr[2];
     cdp1y = save1y = (double)pr[3];
     cdp2x = save2x = (double)pr[4];
     cdp2y = save2y = (double)pr[5];
     stx = (double)pr[6];
     sty = (double)pr[7];
     enx = (double)pr[8];
     eny = (double)pr[9];
     callr[0] = pr[0];
     callr[1] = pr[1];
     callr[2] = pr[6];
     callr[3] = pr[7];
     callr[4] = pr[8];
     callr[5] = pr[9];
     callr[6] = SQRT( (pr[2]-pr[0])*(pr[2]-pr[0]) +
                      (pr[3]-pr[1])*(pr[3]-pr[1]) );
  }
  else
  {
     /* pick-up the arguments */
     refx = (double)pi[0];
     refy = (double)pi[1];
     save1x = cdp1x = (double)pi[2];
     save1y = cdp1y = (double)pi[3];
     save2x = cdp2x = (double)pi[4];
     save2y = cdp2y = (double)pi[5];
     stx = (double)pi[6];
     sty = (double)pi[7];
     enx = (double)pi[8];
     eny = (double)pi[9];
     calli[0] = pi[0];
     calli[1] = pi[1];
     calli[2] = pi[6];
     calli[3] = pi[7];
     calli[4] = pi[8];
     calli[5] = pi[9];
     calli[6] = (long) SQRT ( (pi[2]-pi[0])*(pi[2]-pi[0]) +
                              (pi[3]-pi[1])*(pi[3]-pi[1]) );
  }
  calli[7]=pi[10];

  /* Get the CDP's to be in the 1st quarter */
  if (cdp1x<refx)
  {
     cdp1x += 2.0*(refx-cdp1x);
     cdp1y += 2.0*(refy-cdp1y);
  }
  if (cdp2x<refx)
  {
     cdp2x += 2.0*(refx-cdp2x);
     cdp2y += 2.0*(refy-cdp2y);
  }

  /* Work out the transformation matrix parameters */
  modua = SQRT( (cdp1x-refx)*(cdp1x-refx) +
                (cdp1y-refy)*(cdp1y-refy) );
  /*
     If CDP1 and the Centre Point are coincident, no unique ellipse can
     be constructed, so we return.
  */
  if (FABS(modua) <= cur.realmin)
  {
     PSOUT "grestore\n");
     return;
  }
  modub = SQRT( (cdp2x-refx)*(cdp2x-refx) +
                (cdp2y-refy)*(cdp2y-refy) );
  /*
     If CDP2 and the Centre Point are coincident, no unique ellipse can
     be constructed, so we return.
  */
  if (FABS(modub) <= cur.realmin)
  {
     PSOUT "grestore\n");
     return;
  }

  sina = (cdp1y-refy) / modua;
  cosa = (cdp1x-refx) / modua;
  sinb = (cdp2y-refy) / modub;
  cosb = (cdp2x-refx) / modub;

  /*
     If the ellipse is slanted to the left, i.e. it is positioned in
     the second and fourth quarters, swap the values to effect the 90
     degrees rotation.
  */
  if (sina>sinb)
  {
     temp1 = sina; sina=sinb; sinb=temp1;
     temp1 = modua; modua=modub; modub=temp1;
     temp1 = cosa; cosa=cosb; cosb=temp1;
  }


  det = modua*cosa*modub*sinb - modua*sina*modub*cosb;
  if (FABS(det) <= cur.realmin)
  {
     if (FABS(modua-modub)<= cur.realmin)
        /* CDP1 and CDP2 are on a circle */
        PSarcctr(calli, callr, closed);
     else
        /* CDP1 and CDP2 are on a line */
        PSOUT "%f %f moveto %f %f lineto stroke\n",
              save1x, save1y, save2x, save2y);
     PSOUT "grestore\n");
     return;
  }


  /* Send the transformation and concatenate */
  PSOUT "/mat1 {[ %f %f %f %f %f %f ] } def\n",
         modua*cosa, modua*sina, modub*cosb, modub*sinb, refx, refy);
  PSOUT " mat1 concat\n");
  if (closed == NOTCLOSED)
  {
      PSattrib ( LINE );
      PSOUT "0 0 1\n");
      PSOUT "%f %f mat1 idtransform exch atan\n", stx, sty);
      PSOUT "%f %f mat1 idtransform exch atan\n", enx, eny);
      PSOUT "2 copy sub abs %f lt {360 add} if arc\n", cur.realmin);
      PSOUT " mat1 mat1 invertmatrix concat stroke\n");
      PSOUT "grestore\n");
      return;
  }
  if (*(pi+10)) /* pi[10]==1 for chord close type */
  {
      PSOUT "0 0 1\n");
      PSOUT "%f %f mat1 idtransform exch atan\n", stx, sty);
      PSOUT "%f %f mat1 idtransform exch atan\n", enx, eny);
      PSOUT "2 copy sub abs %f lt {360 add} if arc closepath\n", cur.realmin);
  } else     /* pi[10]==0 for pie close type */
  {
     PSOUT " 0 0 moveto ");
     PSOUT "0 0 1\n");
     PSOUT "%f %f mat1 idtransform exch atan\n", stx, sty);
     PSOUT "%f %f mat1 idtransform exch atan\n", enx, eny);
     PSOUT "2 copy sub abs %f lt {360 add} if arc closepath\n", cur.realmin);
  }
  PSOUT " mat1 mat1 invertmatrix concat\n");
  PSfill (NZ, curatt.edge_vis);
  PSOUT "grestore\n");
  return;
}



/********************************************************************
* PScellarray () : Do the PostScript output of cell array element
*                  given the 3 corner points and the cell array
*                  element data.
* The 3 corner points are stored in pr if current VDC type is real and
* in pi otherwise.
* Number of cell elements in x direction is stored in pi[6] and
* Number of cell elements in y direction is stored in pi[7]
* Subsequent colour values are stored in pi from pi[8]
*
* Input Arguments:
*     pi        : integer data
*     pr        : floating point data
*     num       :  more data to come is negative
*     cont_list : 1 if this call is a continuation of data stream.
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static void    PScellarray ( Long *pi, Float *pr, Long num, Logical cont_list)
#else
static void    PScellarray (pi,pr,num,cont_list)
Long *pi, num;
Float *pr;
Logical cont_list;
#endif
{
   int nx, ny , idx, j, k;
   float xa,ya, xb, yb, xd, yd ,r , g, b;
   double modua,modub,sina,cosa,sinb,cosb;

/*** If this call is not caused by continuation of data stream ***/
/*** get the 3 corner points                                   ***/
   if ( !cont_list )
   {
      if (cur.vdc_type == REAL)
      {
         xa = pr[0]; ya = pr[1];
         xd = pr[2]; yd = pr[3];
         xb = pr[4]; yb = pr[5];
      }
      else
      {
        xa = (float)pi[0]; ya = (float)pi[1];
        xd = (float)pi[2]; yd = (float)pi[3];
        xb = (float)pi[4]; yb = (float)pi[5];
      }
      pi +=6;
      xd = xa + xd - xb;
      yd = ya + yd - yb;
 /*** By now vector ab is the base vector   ***/
 /*** and vector ac is the up vector        ***/
      nx = (int)*pi++;
      ny = (int)*pi++;
      pi++;    /* ignore local precision */
   }
   cont_list = (num < ZERO );
 /**** Calculate transformation and concatenate  ****/
   PSOUT " gsave  \n");
   modua = SQRT( (xb-xa)*(xb-xa)+(yb-ya)*(yb-ya) );
   modub = SQRT( (xd-xa)*(xd-xa)+(yd-ya)*(yd-ya) );
   sina =  (yb-ya)/ modua;
   cosa =  (xb-xa)/ modua;
   sinb =  (yd-ya)/ modub;
   cosb =  (xd-xa)/ modub;
   PSOUT "/picstr %d string def\n",nx);
   PSOUT "[ %f %f %f %f %f %f ] concat\n",
          modua*cosa,modua*sina, modub*cosb,modub*sinb,xa,ya);
   PSOUT "%d %d 8\n",nx,ny);
   PSOUT "[%d 0 0 %d 0 0]\n",nx,ny);
   PSOUT "{currentfile picstr readhexstring pop} ");
   if (col_mode == COLOUR)
     PSOUT "false 3 colorimage\n");
   else
     PSOUT "image\n");

 /***  set decimal places and number of elements per line ***/
 /*** for output formatting                               ***/
   k = (col_mode == COLOUR ? 13 : 40);
   if ( k > nx ) k = nx;
 /*** a for loop to output data stream  ***/
   for (j = ZERO; j < abs((int)num) ; j++ )
   {
     if ( j )
     {
        if ( !(j%k) ) PSOUT "\n");
     }
     if ( cur.color_mode == INDEXED )
     {
        idx = *pi++;
        if (idx>cur.max_colind) idx = 1;
        r = PScolour_table[idx].red;
        g = PScolour_table[idx].green;
        b = PScolour_table[idx].blue;
     }
     else
     {
       r = PSCOLOUR(*pi++, red);
       g = PSCOLOUR(*pi++, green);
       b = PSCOLOUR(*pi++, blue);
       if (r<0.0) r=0.0; else if (r>1.0) r= 1.0;
       if (b<0.0) b=0.0; else if (b>1.0) b= 1.0;
       if (g<0.0) g=0.0; else if (g>1.0) g= 1.0;
     }
     if (col_mode == COLOUR)
       PSOUT "%02x%02x%02x",(int)(r*255),(int)(g*255),(int)(b*255));
     else
       PSOUT "%02x",(int)(255*(0.59*r+0.31*g+0.1*b)));
  }
  PSOUT "\ngrestore\n");
  return;
}

/********************************************************************
*  PSpset() : Do the PostScript output of polygon set element
*             given the data of vertices of the polygons in the
*             set and the edge visibility of each edge.
*
* Input Arguments:
*     n      :  no. of points
*     pi     : integer data
*     pr     : floating point data
*     cont   : 1 if this call is a continuation of data stream.
* Output Arguments:
*     None.
* Return value:
*     None.
* Maintenance Log:
*  25 Jul 91 AMR  Added code to free off malloc'd data structures.

*********************************************************************/


#ifdef PROTO
static void PSpset ( Long n, Long *pi, Float *pr, Logical cont)
#else
static void PSpset (n, pi, pr, cont)

Long n, *pi;
Float *pr;
Logical cont;
#endif
{
   static Polygonset polset;  /* The polygonset */
   static Long start;         /* Starting place */
   static char *func = "PSpset";  /* Function name */

   /* Allocate space for polygon set stucture */
   if(!cont)
   {  /* First set of points, allocate space */
      start = 0;
      polset.n = abs(n);
      polset.pts = (Point *)MALLOC( polset.n, sizeof( Point));
      if(polset.pts == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      polset.eofl = (Enum *)MALLOC( polset.n, sizeof( Enum));
      if(polset.eofl == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
   }
   else
   {  /* Additional set of points, reallocate */
      start = polset.n;
      polset.n += abs(n);
      polset.pts = (Point *)realloc( (void *)polset.pts,
                              (size_t)(polset.n*sizeof(Point)));
      if(polset.pts == NULL)
         exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));
      polset.eofl = (Enum *)realloc( (void *)polset.pts,
                              (size_t)(polset.n*sizeof(Enum)));
      if(polset.eofl == NULL)
         exit ((int)CGMerror( func, ERR_NOMEMORY, FATAL, NULLSTR));
   }

   /* Put data into polygon set stucture (get as Points) */
   PTSget (polset.n, pi, pr, polset.pts+start, polset.eofl+start);

   if(n > 2)
   {  /* No more points */

       if(PSfillatt.style != EMPTY)
       { /* Draw polygon set */
         Int npoly;         /* Number of polygons in set */
         Enum edge_vis;     /* Saved current edge visibility */
         Long first, last;  /* First and last points in polygon */
         Int i;             /* loop index */

         npoly = POLnumber(polset.n, polset.eofl);
         if(npoly > 0)
         {
           /* Make path for polygon set */
            PSOUT "gsave newpath");
            first = 0;
            for(i=0; i<npoly; i++)
            {  /* For each polygon */
               POLend( polset.n, polset.eofl, first, NULL, &last);
               PSLINEfunc ( last-first+1, polset.pts+first);
               PSOUT "\nclosepath ");
               first = last+1;
            }

            /* Fill it without edges */
            PSfill(EO, INVIS);
         }
      }

      /* Draw edges, if visible */
      if(curatt.edge_vis == VIS)
      {
         PSOUT "gsave newpath");
         POLedge( &polset, NULL, PSLINEfunc);
         PSOUT "\n");
         PSedge();
         PSOUT "grestore \n");
      }
   }
   if(n >= 0)
   {  /* No more points, free the space allocated */
       FREE(polset.eofl);
       FREE(polset.pts);
   }

   return;
}

/********************************************************************
* PSpattable() : Do the CGM pattern table element which allow user
*                to set up new patterns and to change existing patterns.
*
* Input Arguments:
*     n      :  no. of points
*     pi     : integer data
*     cont   : 1 if this call is a continuation of data stream.
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/


#ifdef PROTO
static void PSpattable ( Long n, Long *pi, Logical cont )
#else
static void PSpattable ( n, pi, cont )
Long n, *pi;
Logical cont;
#endif

{
  int i;
  static long nx, ny, curpatidx, j, patdatidx;
  Logical foundidx;
  Prec loc_prec;

     if ( ! cont )
     {
        curpatidx = *pi++;
        nx = *pi++;
        ny = *pi++;
        loc_prec = *pi++;
        foundidx = FALSE;
        for (i=0;i<npspat;i++)
        {
           if ( PSpat_table[i].patidx == (Index)curpatidx)
           {
              FREE(PSpat_table[i].pathead);
              foundidx = TRUE;
              break;
           }
        }

        if (!foundidx)
        {
           i = npspat++;
           PSpat_table = (PS_pattabtype *)realloc((char *)PSpat_table,
                         (size_t)(npspat * sizeof(PS_pattabtype)));
           PSpat_table[i].patidx = (Index)curpatidx;
        }

        PSpat_table[i].nx = nx;
        PSpat_table[i].ny = ny;
        PSpat_table[i].prec = loc_prec;
        PSpat_table[i].pathead = (float*)MALLOC(nx*ny*3,sizeof(float));
        if (cur.color_mode==INDEXED)
          PSpat_table[i].pathead[0]=INDPATTERN;
        patdatidx = 0;
     }
     for (j = ZERO; j < abs((int)n) ; j++ )
     {
        if ( PSpat_table[curpatidx].pathead[0] == INDPATTERN )
        {
           patdatidx++;
           PSpat_table[curpatidx].pathead[patdatidx++] = (float)*(pi++);
           patdatidx++;
        }
        else
        {
           PSpat_table[curpatidx].pathead[patdatidx++] = *(pi++);
           PSpat_table[curpatidx].pathead[patdatidx++] = *(pi++);
           PSpat_table[curpatidx].pathead[patdatidx++] = *(pi++);
        }
     }
}

/********************************************************************
* PScoltab() : Do the CGM colour table element which allow user
*
* Input Arguments:
*     pi     : integer data
*     num    :  no. of points
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/

#ifdef PROTO
static int PScoltab( Long *pi, Long num)
#else
static int PScoltab(pi,num)
Long *pi , num;
#endif
{
  long i, start;
  start = *(pi++);
  for (i=ZERO;i<num;i++)
    if ((start+i)<=cur.max_colind)
    {
       PScolour_table[start + i].red   = PSCOLOUR(*(pi++),red);
       PScolour_table[start + i].green = PSCOLOUR(*(pi++),green);
       PScolour_table[start + i].blue  = PSCOLOUR(*(pi++),blue);
    }
    else
      return 1; /* signal colour index out of range error */
  return 0;
}



/********************************************************************
* PStxtdraw() : Do the text drawing for PostScript output.
*               Go through the link list of text substring items and
*               direct to respective drawing method procedures to
*               render the text strings.
*
* Input Arguments:
*     list : Pointer to the head of the link list of text items
*     txtp : Coord for origin of text element
* Output Arguments:
*     None.
* Return value:
*     None.
*********************************************************************/


#ifdef PROTO
static void PStxtdraw( Textitem *list, Point txtp)
#else
static void PStxtdraw(list, txtp)
Textitem *list;
Point txtp;
#endif
{
    Textitem *txtlp = list, *txtcp = NULL;
    float red, green, blue;

     if (rt_mode) txtlp=TXTrestrict(txtlp, rt_wid, rt_hgt, &PStextatt);

     if (txtlp!=NULL)
     {
     TXTalign(txtlp, txtp, TXTgwidth(txtlp), TXTgheight(txtlp));
     while (txtlp)
     {
        if ( cur.color_mode == INDEXED )
        {
           red   = PScolour_table[txtlp->colourindex].red;
           green = PScolour_table[txtlp->colourindex].green;
           blue  = PScolour_table[txtlp->colourindex].blue;
        }
        else
        {
           red   = PSCOLOUR( txtlp->text_col.red, red );
           green = PSCOLOUR( txtlp->text_col.green, green );
           blue  = PSCOLOUR( txtlp->text_col.blue, blue );
        }
        PSOUT "%f %f %f setrgbcolor\n", red, green, blue);

        switch (txtlp->text_method)
        {
#ifdef HERSHEY
           case HER:
                PSOUT "gsave    %% Start of Hershey string\n");
                HERtext(txtlp, txtp, PSHERtext);
                PSOUT "grestore %% End of Hershey string\n");
                break;
#endif
#ifdef BEZIER
           case BEZ:
                PSOUT "gsave    %% Start of Bezier string\n");
                BEZtext(txtlp, &PStextatt, txtp, PSBEZtext);
                PSOUT "grestore %% End of Bezier string\n");
                break;
#endif
#ifdef TEX
           case TEX:
                /* No routine yet */
                break;
#endif
           case HDW:
           case PS:
           default:
                PStext(txtlp, txtp);
                break;
        }
        txtlp = txtlp->next;
     }
     TXTfree(list);
     }
     return;
}


/***********************************************************************
* PStext() : PostScript text output for PostScript text drawing method
*
* Input Arguments:
*     list : Pointer to the head of the link list of text items
*     txtp : Coord for origin of text element
* Output Arguments:
*     None.
* Return value:
*     None.
************************************************************************/


#ifdef PROTO
static void PStext( Textitem *itemp, Point txtp)
#else
static void PStext(itemp, txtp)
Textitem *itemp;
Point txtp;
#endif
{
   float spacew;
   char *cp;
   int i, no, k;
   Tmatrix matrix;
/*
   1. get all the attributes needed and send them down
         - calculate the corrected width of a space character
   2. convert the string into hex
   3. send the common attributes (font name, height, expansion)
   3. have a switch on precision
          - send the rest of the attributes (matrix)
          - send the string and origin(s)
*/

   spacew =  itemp->char_height;
   spacew *= itemp->rwd[0];

   PSOUT "/%s %f %f txstat\n", ftnames[itemp->text_font], spacew,
                                 itemp->char_exp);
   PSOUT "<");
   no = 40;
   for(cp=itemp->str, k = no ; *cp != '\0'; cp++)
   {
       if ( !--k )
       {
           PSOUT "\n" );
           k = no;
       }
       PSOUT "%02X", *cp);
   }
   PSOUT ">\n");
   switch(itemp->text_prec)
   {
       case STRING:   /*STRING: */
          PSOUT " %f %f txstr\n", itemp->origin[0].x, itemp->origin[0].y);
          break;
       case CHAR:     /* CHAR:   */
       case STROKE:   /* STROKE: */
          no = 78 / (dec_all + 4);
          PSOUT "[");
          for(cp=itemp->str, k=no, i=0;*cp !='\0'; cp++, i++)
          {
             if ( !--k )
             {
                 PSOUT "\n" );
                 k = no;
             }
             PSOUT "%.*f ", dec_right, itemp->origin[i].x);
          }
          PSOUT "]\n");
          PSOUT "[");
          for(cp=itemp->str, k=no, i=0;*cp != '\0'; cp++, i++)
          {
             if ( !--k )
             {
                PSOUT "\n" );
                k = no;
             }
             PSOUT "%.*f ", dec_right, itemp->origin[i].y);
          }
          PSOUT "]\n");
          TXTxform(txtp, matrix);
          PSOUT " %f %f %f %f %f %f txchar\n",
                   matrix[0][0], matrix[0][1],
                   matrix[1][0], matrix[1][1],
                   matrix[2][0], matrix[2][1] );
          break;
   }
   return;
}

/***********************************************************************
* PSstktext() : PostScript stroke text output for Bezier or Hershey text
*               drawing method
*
* Input Arguments:
*     n    : number of points
*    pt : array of points
*   Mat    : a 3 by 2 matrix that repersent the transformation for the
*            points.
* methodfg : 0 for Bezier, 1 for Hershey
*
* Output Arguments:
*     None.
* Return value:
*     None.
*
* Modification Log
*
*    1 Jul 91 CDS  PSstktext: Changed to use newchar, and improve performance.
************************************************************************/

#ifdef PROTO
static void PSstktext(int n, Point *pt, Tmatrix mat,
                      int methodfg, int newchar)
#else
static void PSstktext(n,pt,mat,methodfg,newchar)
int n, newchar;
int methodfg;
Point *pt;
Tmatrix mat;
#endif
{
   register long k;
   register Point *ptend;
   float xmin, xmax, ymin, ymax;
   register long no, i, j;

   if (newchar)
   {
      PSOUT "grestore gsave newpath\n");

      PSOUT "[");
      for (i=0;i<3;i++)
        for (j=0;j<2;j++)
         PSOUT "%f ", mat[i][j]);
      PSOUT "] concat \n");
   }

   /* Initialise xmax, xmin, ymax, ymin */
   xmax = xmin = pt[0].x;
   ymax = ymin = pt[0].y;

   /* Move to the first point in the list */
   PSOUT "%.*f %.*f m ",
         dec_right, pt->x, dec_right, pt->y);

   ptend = pt+n;

   /* Output 'n' points starting at pr+1 */
   no = 40 / (dec_all + 4);
   k = no-1;
   while(++pt < ptend)
   {
      if ( !--k )
      {
          PSOUT "\n" );
          k = no;
      }

      /* Extend path to the new point */
      PSOUT " %.*f %.*f l",
            dec_right, pt->x, dec_right, pt->y);

      if ((pt->x)>xmax)
         xmax = pt->x;
      else
         if ((pt->x)<xmin)
            xmin = pt->x;

      if ((pt->y)>ymax)
         ymax = pt->y;
      else
         if ((pt->y)<ymin)
            ymin = pt->y;

   }

   /* Depending on system either ... */
   if (!methodfg)
      /* ... close the path and fill the region */
      PSOUT "\nclosepath eofill\n");
   else
      /* ... or draw the lines */
      PSOUT "\nstroke\n");
}


/****************************************************** PStrapfill ****/

#ifdef PROTO
static void PStrapfill( Long np, Long *pi, Float *pr,
                        Enum set, Logical cont )
#else
static void PStrapfill( np, pi, pr, set, cont)
 /*
    Fill a polygon or polygon set by trapezoid decomposition
 */

Long np;      /* Number of Points (got from *(pi-1)) */
Long *pi;     /* Integer Data */
Float *pr;    /* Real Data */
Enum set;     /* Polygon set flag */
Logical cont; /* True, if this set of points is a continuation */
#endif

{
   static Polygonset polset;  /* The polygonset */
   Int tzdec_all;             /* Length of numbers in trapezoids */
   float test;                /* Used to calculate precision */
   Rect clip;                 /* Clipping rectangle or VDC extent */

   /* Get data into polygon set structure */
   POLget(np, pi, pr, set==SET, cont, &polset);

   if(np > 2)
   {  /* No more points */

      /* Calculate precision of trapezoids */
       for( tzdec_right=0, test=tzytol;   test<1.0;
            tzdec_right++, test *= 10.0);
       if(cur.vdc_type == REAL)
          tzdec_all = tzdec_right + dec_all - dec_right;
       else
          tzdec_all = cur.vdcint_bits*LOG2 + 1;
      /* and number of trapezoids per line */
       PTSclipget(NULL, &clip);
       if((clip.left < 0.0) || (clip.bot < 0.0))
       {  /* Allow for negative coordinates */
          tzno = 40/(3*tzdec_all + 11);
       }
       else
       {  /* Can assume all coordinates are positive (TRPfill clips) */
          tzno = 40/(3*tzdec_all + 8);
       }
       if(tzno == 0) tzno = 1;

       /* Fill with trapezoids */
       TRPfill( &polset, NTZ, tzytol, PSTRPdraw);

       /* Draw edges, if visible */
       if(curatt.edge_vis == VIS)
       {
          PSOUT "gsave newpath");
          POLedge( &polset, NULL, PSLINEfunc);
          PSOUT "\n");
          PSedge();
          PSOUT "grestore\n");
       }
  }
   if(np >= 0)
   {  /* No more points, free the space allocated */
       FREE(polset.eofl);
       FREE(polset.pts);
   }

   return;
}

/****************************************************** PSLINEfunc *****/

#ifdef PROTO
void PSLINEfunc( Long np, Point *pt )
#else
void PSLINEfunc( np, pt )
 /*
    Device line function for utilities such as POLedge
 */

Long np;           /* Number of vertices */
Point *pt;         /* The vertices */
#endif

{
   register long i, k, no;
   register Prec prec;

   /* Set the number of points on each line */
   prec = dec_right;
   no = 40/( dec_all + 4);

   k = no;
   PSOUT "\n%.*f %.*f m ",(int)prec, pt->x, (int)prec, pt->y );
   pt++;
   for(i=1, k=no; i<np; i++, pt++)
   {
      if( !--k )
      {
         PSOUT "\n");
         k = no;
      }
      PSOUT "%.*f %.*f l ", (int)prec, pt->x, (int)prec, pt->y );
   }

   return;
}
/****************************************************** PSdodecomp ****/

#ifdef PROTO
static Logical PSdodecomp ( Long np, Long *pi, Float *pr, Enum set )
#else
static Logical PSdodecomp ( np, pi, pr, set)
 /*
    Determine whether the polygon should be filled by decomposition
 */

Long np;      /* Number of Points (got from *(pi-1)) */
Long *pi;     /* Integer Data */
Float *pr;    /* Real Data */
Enum set;     /* Polygon set flag */
#endif

{
   Point *vtx;  /* The vertices */
   Int nsx;     /* Number of self intersections */

   if(PSfillatt.style == EMPTY) return FALSE;   /* Not if empty */
   if(PSfillatt.style == HOLLOW) return FALSE;  /* Not if hollow */
   if(np < 0) return TRUE;       /* If not final entry, return true */
   if(np < MAXARB) return FALSE; /* Polygon(set) has few vertices */
   if(set) return TRUE;          /* else, if Polygon set */
   if(np > MAXSIM) return TRUE;  /* else, if polygon has many vertices */

   /* Put data into points */
   vtx = PTSlist( np, pi, pr, NULL);

   /* Count the self intersections of the polygon */
   nsx = POLselfx( np, vtx, 1);

   /* Free the allocations */
   FREE(vtx);

   return (nsx > 0);
}
/****************************************************** PSTRPdraw *****/

#ifdef PROTO
void PSTRPdraw( Int ntz, Trapezoid *tz )
#else
void PSTRPdraw( ntz, tz )
 /*
    Draw a series of trapezoids
 */

Int ntz;          /* Number trapezoid */
Trapezoid *tz;    /* The trapezoids */
#endif

{
   register Int i;  /* Loop index */
   register Int k;  /* Number of trapezoids still to be done in line */
   register Int no; /* Number of trapezoids per line */
   register Prec prec;  /* precision */


   PSOUT "gsave newpath %%create path in trapezoids\n");

   no = tzno;
   prec = tzdec_right;
   for(i=0, k=tzno+1; i<ntz; i++, tz++)
   {  /* For each trapezoid */
      if( !--k )
      {
         PSOUT "\n");
         k=no;
      }
      PSOUT "%.*f %.*f %.*f %.*f %.*f %.*f tz ",
             (int)prec, tz->top_left,
             (int)prec, tz->base_left,
             (int)prec, tz->top_right,
             (int)prec, tz->base_right,
             (int)prec, tz->top,
             (int)prec, tz->base);

   }
   PSOUT "\n");

   PSfill(NZ, INVIS);   /* Fill with simple interior rule & no edges */
   PSOUT "grestore\n");

   return;
}
/***********************************************************************
* PSBEZtext() : PostScript text output for Bezier text drawing method
*
* Input Arguments:
*     n : number of points
*    pt : array of points
*   Mat : a 3 by 2 matrix that repersent the transformation for the
*         points.
* Output Arguments:
*     None.
* Return value:
*     None.
************************************************************************/

#ifdef PROTO
void PSBEZtext( int n, Point *pt, Tmatrix mat, int newchar)
#else
void PSBEZtext(n,pt,mat,newchar)
int n;
int newchar;
Point *pt;
Tmatrix mat;
#endif
{
    PSstktext(n,pt,mat,0,newchar);
}

/***********************************************************************
* PSHERtext() : PostScript text output for Hershey text drawing method
*
* Input Arguments:
*     n : number of points
*    pt : array of points
*   Mat : a 3 by 2 matrix that repersent the transformation for the
*         points.
* Output Arguments:
*     None.
* Return value:
*     None.
************************************************************************/

#ifdef PROTO
void PSHERtext(int n, Point *pt, Tmatrix mat, int newchar)
#else
void PSHERtext(n,pt,mat,newchar)
int n;
int newchar;
Point *pt;
Tmatrix mat;
#endif
{
    PSstktext(n,pt,mat,1,newchar);
}


/************************************************************************
 *    PSalias(): Function which reads the alias.psd file and expects
 *               to find the input fontname's aliases on the same line
 *               as the fontname itself, separated by spaces or tabs.
 *
 *               IN: Pointer to the long fontname.
 *               OUT: None.
 *               RETURN VALUE: Pointer to the aliased fontname if found,
 *                             to the original name if not.
 *
 *               Author: PLP
 *
 * Maintenance Log:
 *  25 Jul 91 AMR  Changed FILESEP in shortname malloc to strlen(FILESEP)
 *                 May have potentially caused problems in the future.
 *  25 Jul 91 AMR  Simplified the return statement by removing the :? and ,
 *                 operators.
 *
 * ALGORITHM
 * ---------
 *
 * Read the alias.psd file. Look for the line which begins with the
 * original fontname; the alias should be there too, separated by
 * space(s) and/or tab(s).
 *
 * LOCALS
 * ------
 * fp
 * pathname
 * str1
 * str2
 */

#ifdef PROTO
static char *PSalias( char *longname )
#else
static char *PSalias(longname)
char *longname;
#endif
{
   FILE *fpin;
   char str1[256], str2[128], *pathname;
   char *shortname, *func="PSalias";

   /* Allocate memory for the aliased fontname, gets freed in calling routine*/
   shortname = (char *)MALLOC (1,(NAMELGTH+strlen(FILESEP)+EXTLGTH+1) );
   if( shortname == NULL )
      /* Exit on the spot if malloc failed */
      exit ((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

   /* Initialise the new name for check later on */
   *shortname = '\0';

   /* Open file */
   fpin = CGMfopen((pathname=CGMpath("alias", "psd")), F_READ, CLEAR_TEXT);
   if ( fpin == NULL )
      /* Exit on the spot if open failed */
      exit((int)CGMerror(func, ERR_OPENFILE, FATAL, pathname));

   /* free pathname - no longer needed */
   FREE(pathname);

   /* Read the file and try to find a line beginning with the original name */
   while (fgets(str1,255,fpin))
   {
      /* Skip irregular lines */
      if (!sscanf(str1,"%s %s", str2, shortname))
         continue;

      /* Break if match found, for shortname already has the alias */
      if (!strcmp(longname, str2))
         break;

     /* Initialise the shortname for check later on */
      *shortname = '\0';
   }

   (void)fclose(fpin);

   /*
     If no match found, i.e. shortname is a null string, issue warning
     and return longname.
   */

   if (*shortname=='\0')
   {
     (void)CGMerror(func, ERR_NOALIAS, WARNING, longname);
     return(longname);
   }
   else
     return(shortname);
}


/*************************************************************************
 *    PSreadafm(): Function which reads a PostScript font metrics
 *                 file, extracts the necessary information and stores
 *                 into the text structure.
 *
 *                 IN: Fontname, pointer to a Textitem structure.
 *                 OUT: None.
 *                 RETURN VALUE: None.
 *
 *                 Author: WKL
 *
 * ALGORITHM
 * ---------
 *
 * Read the appropriate afm file. Get the widths (straightforward, supplied
 * in the file), CapHeight, Ascender and Bounding Box information. Normalize
 * all data to the height (1000), then correct to reflect the difference
 * between PoscScript's and CGM's definition of character height.
 *
 * LOCALS
 * ------
 * acknown
 * b
 * chknown
 * fpin
 * func
 * i
 * idx
 * j
 * l
 * lnbuf
 * parsep
 * pathname
 * r
 * rth
 * t
 * tmp
 * w
 *
 */

#ifdef PROTO
static void PSreadafm( char *fontname, Textitem *thisitem)
#else
static void PSreadafm(fontname, thisitem)
char *fontname;
Textitem  *thisitem ;
#endif
{
   FILE *fpin;
   char lnbuf[256], *tmp, *parsep, *pathname, *func="PSreadafm";
   int idx, w, l, b, r, t, chknown, acknown, i;
   float   rth = 0.;

   /* Open file, but don't forget to alias the PS fontname! */
   tmp = PSalias(fontname);
   pathname = CGMpath(tmp, "afm");
   fpin = CGMfopen(pathname, F_READ, CLEAR_TEXT);
   if (fpin == NULL)
       exit((int)CGMerror(func, ERR_OPENFILE, FATAL, pathname));

   /* free pathname and alias - no longer needed */
   FREE(pathname); FREE(tmp);

   chknown = 0;
   acknown = 0;
   lnbuf[0] = '\0';

   /*
      Read in the header up to the "StartCharMetrics" line and
      get CapHeight and Ascender data.
   */
   while ( fgets(lnbuf,100,fpin)&&(strncmp(lnbuf,"StartCharMetrics",16)))
   {
      /* When string begins with CapHeight...*/
      if (!strncmp(lnbuf,"CapHeight",9))
      {
         /* skip till point to a decimal number...*/
         parsep = lnbuf;
         PSskp2dgt(&parsep);
         /* ... convert from string to integer idx...*/
         (void)sscanf(parsep,"%d",&idx);
         /* ... and set flag that CapHeight is known. */
         chknown++;
         /*
           In PostScript, font metrics information is specified in
           releation to the requested (PostScript) character height, i.e.
           base to top distance. In .afm files this height is taken to be
           1000 units. Therefore, to normalize font data we divide by
           1000.
           However, CGM defines character height to be the distance
           between base-line and cap-line (i.e. vertical spacing is not
           included), resulting in larger characters drawn for the same
           height value. This will be catered for later.
           For now then, rth is the PostScript normalized base to cap-line
           distance.
         */
         rth = (float)idx/1000.;
      }

      /* When string begins with Ascender...*/
      if (!strncmp(lnbuf,"Ascender",8))
      {
         /* skip till point to a decimal number...*/
         parsep = lnbuf;
         PSskp2dgt(&parsep);
         /* ... convert from string to integer idx...*/
         (void)sscanf(parsep,"%d",&idx);
         /* ... and set flag that Ascender is known. */
         acknown++;
         /*
          Normalize the Ascender height and substract normalized CapHeight.
          This is the Cap to Top distance.
         */
         thisitem->rtop = (float)idx/1000. - rth;
         /* Default for "funny" fonts */
         if (thisitem->rtop < 0.0)
            thisitem->rtop = 0.0;
      }
   }

   /*
     We are now at the beginning of metrics information for each character.
     Get the width and bounding box information for characters between
     32 and 127.
    */
   idx = 0;
   thisitem->rmaxwd = 0.;
   while (fgets(lnbuf,100,fpin)&&(idx<126))
   {
      /*
        Format of the .afm file line is assumed to be:
        C ascii_code ; WX width ; N character_name ; B llx lly ulx uly ;
      */

         /* Skip till point to first digit of Ascii value */
         parsep = lnbuf;
         PSskp2dgt(&parsep);
         /* Get the Ascii value */
         (void)sscanf(parsep,"%d",&idx);
         /* Get out if first character turns out to be grater than 128 */
         if (idx>=128) break;

         /* Skip remaining digits of Ascii code */
         PSskpdgt(&parsep);
         /* Skip till point to the first digit of width */
         PSskp2dgt(&parsep);
         /* Get the value for width */
         (void)sscanf(parsep,"%d",&w);

         /* Skip remaining digits of width */
         PSskpdgt(&parsep);
         /* Skip till point to the first digit of bounding box */
         PSskp2dgt(&parsep);
         /* Get the value for lower left corner's X */
          (void)sscanf(parsep,"%d",&l);

         /* Skip again... */
         PSskpdgt(&parsep); PSskp2dgt(&parsep);
         /* Get the value for lower left corner's Y */
         (void)sscanf(parsep,"%d",&b);

         /* Skip again... */
         PSskpdgt(&parsep); PSskp2dgt(&parsep);
         /* Get the value for upper right corner's X */
         (void)sscanf(parsep,"%d",&r);

         /* Skip again... */
         PSskpdgt(&parsep); PSskp2dgt(&parsep);
         /* Get the value for upper right corner's Y */
         (void)sscanf(parsep,"%d",&t);

         /* Store normalized width into the structure */
         thisitem->rwd[idx-32] = (float)w/1000. ;

         /* Looking for maximum width in the character set */
         if ((float)w>thisitem->rmaxwd) thisitem->rmaxwd = (float)w;

         /* Base to bottom distance is measured for character 'g' */
         if (idx==103)
            thisitem->rbot = ((float)b * (-1.)) / 1000.;

         /*
           When a font does not have CapHeight information in its .afm,
           we define it as height of the character H (Ascii 72).
          */
         if((!chknown)&&(idx==72))
            rth = ((float)t) / 1000.;

         /*
           For Symbol, which does not have Ascender information, we
           define it is ascender of the letter xi (Ascii 120). Code not
           quite correct and needs to be changed.
         */
         if((!acknown)&&(idx==120))
            thisitem->rtop = (float)t/1000. - rth;
   }

   /* Maximum width information needs to be normalized */
   thisitem->rmaxwd /= 1000. ;

   /*
      There is a difference in how PostScript and CGM define
      character height. In PostScript, it is the distance between
      the base line and the top line (i.e. vertical spacing
      is included). In CGM, it is the distance between the
      base line and the cap line (i.e. vertical spacing not
      included).
      This difference means that in order to achieve the CGM
      height we must increase PostScript height by 1/rth. The
      font data needs to be changed accordingly.
   */
   thisitem->rmaxwd /= rth;
   thisitem->rtop   /= rth;
   thisitem->rbot   /= rth;
   for (i=0;i<95;i++)
      thisitem->rwd[i] /= rth;

   /* We're finished - close the file */
   (void)fclose(fpin);
}

/*************************************************************************
 *    PSskp2dgt(): Function which increments the input character pointer
 *                 until pointing to a digit.
 *
 *                 IN: Pointer to a pointer of a string being parsed.
 *                 OUT: None.
 *                 RETURN VALUE: None.
 *
 *                 Author: WKL/PLP
 *
 * ALGORITHM
 * ---------
 *
 * Skip over all characters that are not a digit, +, -, =, and end of string.
 *
 * LOCALS
 * ------
 *
 */

#ifdef PROTO
static void PSskp2dgt( char **cp )
#else
static void PSskp2dgt(cp)
char **cp;
#endif
{
   while(!isdigit(**cp) && **cp!='+' && **cp!='-' && **cp!='.' &&  **cp!='\0')
      (*cp)++;
   return;
}

/*************************************************************************
 *    PSskpdgt(): Function which increments the input character pointer
 *                until pointing to a non-digit. Used after last skp2digit
 *                to jump over the number just processed.
 *
 *
 *                IN: Pointer to a pointer to a string being parsed.
 *                OUT: None.
 *                RETURN VALUE: None.
 *
 *                Author: WKL/PLP
 *
 * ALGORITHM
 * ---------
 *
 * Skip until either a non-digit, end of string or a suitable arithmetic
 * symbol is reached.
 *
 * LOCALS
 * ------
 *
 */

#ifdef PROTO
static void PSskpdgt( char **cp )
#else
static void PSskpdgt(cp)
char **cp;
#endif
{
   while(isdigit(**cp) || **cp=='+' || **cp=='-' || **cp=='.' ||  **cp=='\0')
      (*cp)++;
   return;
}


/***********************************************************************
* PSmfname() : return the new filename needed for multiple metafiles
*
* Input Arguments:
*     None.
* Output Arguments:
*     None.
* Return value:
*     PS output filename
* Algorithm:
*     Replaces the extension part of the filename with successive numbers
*       using EXTLGTH to calculate how many it can create.
*     When the limit has been reached, it asks the user for another filename
*       then starts modifying the extensions again.
* Maintenance Log:
*  25 Jul 91 AMR  Changed declaration of numstr from [EXTLGTH] to [EXTLGTH+1]
*                 Didn't appear to cause problems but could of.
*  25 Jul 91 AMR  As above but for newfname, using NAMELGTH rather than EXTLGTH.
*  22 Aug 91 AMR  Added the output of the original output filename when writing
*                 the multiple metafile message.
*
* Bugs:
*     If the extension in cgmofile was a number, it will be overwritten without
*       warning when generating extra files.
*     fname not freed, but only created once during the execution of RAL-CGM.
************************************************************************/

#ifdef PROTO
static char * PSmfname( void )
#else
static char * PSmfname()
#endif
{
  static unsigned int maxpsfiles = 0, nextfile = 0;
  static char *fname;
  char numstr[EXTLGTH+1], *func="PSmfname";
  unsigned int nextfnam, nextextn;

  /*
   if first time called, copy cgmofile into fname and calculate
   the maximum number that can be held in the extension.
  */
  if ( maxpsfiles == 0 )
  {
    register unsigned int j = 9;
    register unsigned short int i;

    for(i=0; i<EXTLGTH; i++)
    {
      maxpsfiles += j;
      j *= 10;
    }

    PUTMSG "Input file contains multiple metafiles;\n");
    PUTMSG "The following files have been created for PS output...\n");
    PUTMSG "\t%s\n",cgmofile);
    fname = (char *)MALLOC (1, sizeof(char) * ( strlen(cgmofile) +
          EXTLGTH + strlen(FILESEP) ) );

    if (!fname)
      exit((int)CGMerror(func, ERR_NOMEMORY, FATAL, NULLSTR));

    (void)strcpy(fname,cgmofile);
  }

  nextfile++;
  nextfnam = nextfile/(maxpsfiles+1);
  nextextn = nextfile%(maxpsfiles+1);

  /*
   replace filename with replacement provided by the user.
   start extension with the original one specified in cgmofile
  */
  if (nextextn == 0)
  {
    char newfname[NAMELGTH+1], *fnptr;
    int fnlen;

    PUTMSG"Cannot create any more files by modifying the extension -\n");
    PUTMSG"\tPlease enter a new base part to continue with :\n");

    /* go back to the original file to get original extension */
    (void)strcpy(fname, cgmofile);
    fnlen = CGMgetfname(fname, &fnptr);

    (void)strncpy(newfname, fnptr, (fnlen > NAMELGTH ? NAMELGTH : fnlen) );
    newfname[ ( fnlen > NAMELGTH ? NAMELGTH : fnlen ) ] = '\0';

    (void)sprintf(numstr,"%-1u",nextfnam);
    if (strlen(numstr) > NAMELGTH)
      exit((int)CGMerror(func, ERR_2MANYFILES, FATAL, NULLSTR));

    if ( strlen(numstr) > (NAMELGTH - strlen(newfname)) )
      newfname[ NAMELGTH - strlen(numstr) ] = '\0';

    (void)strcat(newfname,numstr);

    CGMmodfname(fname, newfname);
  }
  /*
   replace file extension with number of metafile.
  */
  else
  {
    (void)sprintf(numstr,"%-01u",nextextn);
    CGMmodfextn(fname,numstr);
  }

  return fname;
}


/***********************************************************************
* PSspecarc() : draw the special cases for a mathematically degenerate
*               circular arc.
*
* Input Arguments:
*     type : which special case is it? line, 2 semi-inf. lines or dot?
*     ref  : structure containing points needed to generate the output.
* Output Arguments:
*     None.
* Return value:
*     None.
*
* Algorithm :
*     This routine draws a dot, a line, or two semi-infininte lines, depending
*     on the value passed in type, ARC_DOT, ARC_LINE, ARC_SLINE.
*     The valuses defining the points used in the drawing are passed
*
* Notes :
*     Calling routines should note that this routine does not perform a
*     save or restore, and hence will alter the postscript graphics state
*     when it returns.
*
************************************************************************/

#ifdef PROTO
static void PSspecarc( Enum type , PS_arcref *ref )
#else
static void PSspecarc( type , ref )
Enum type;
PS_arcref *ref;
#endif
{
   double m, c, mx, my;
   double xmin, ymin, xmax, ymax;
   double vdc_xmin, vdc_xmax, vdc_ymin, vdc_ymax;
   switch (type)
   {
      case ARC_DOT:
         PSOUT "%f %f dot\n", ref->start.x, ref->start.y);
         break;

      case ARC_LINE:
         PSOUT "%f %f m %f %f l S\n",ref->start.x, ref->start.y, ref->end.x,
               ref->end.y);
         break;

      case ARC_SLINE:
         if (cur.vdc_type == 1)
         {
            vdc_xmin = cur.vdc_extent.a.x.real;
            vdc_xmax = cur.vdc_extent.b.x.real;
            vdc_ymin = cur.vdc_extent.a.y.real;
            vdc_ymax = cur.vdc_extent.b.y.real;
         }
         else
         {
            vdc_xmin = cur.vdc_extent.a.x.intr;
            vdc_xmax = cur.vdc_extent.b.x.intr;
            vdc_ymin = cur.vdc_extent.a.y.intr;
            vdc_ymax = cur.vdc_extent.b.y.intr;
         }

         mx = ref->start.x - ref->end.x;
         /*
          If the line is vertical, save time by trapping for it
         */
         if ( (double) FABS( mx ) == 0.0 )
         {
            xmax = ref->start.x; xmin = ref->start.x;
            ymax = vdc_ymax; ymin = vdc_ymin;
         }
         else
         {
            my = ref->start.y - ref->end.y;
            /*
             If the line is horizontal, save time by trapping for it
            */
            if ( (double) FABS( my ) == 0.0 )
            {
               xmax = vdc_xmax; xmin = vdc_xmin;
               ymax = ref->start.y; ymin = ref->start.y;
            }
            else
            {
               m = my/mx;
               c = ref->start.y - ( m * ref->start.x );
               /*
                If the line is more horizontal than vertical then find limits
                by equating for xmin = vdc_xmin and xmax = vdc_xmax
               */
               if ( (double) FABS( m ) < 1.0 )
               {
                  xmin = ( vdc_ymin - c ) / m;
                  xmax = ( vdc_ymax - c ) / m;
                  ymin = vdc_ymin; ymax = vdc_ymax;
               }
               /*
                If the line is more vertical than horizontal then find limits
                by equating for ymin = vdc_ymin and ymax = vdc_ymax
               */
               else
               {
                  ymin = ( m * vdc_xmin ) + c;
                  ymax = ( m * vdc_xmax ) + c;
                  xmin = vdc_xmin; xmax = vdc_xmax;
               }
            }
         }
         PSOUT "%f %f m %f %f l S\n",xmin, ymin, xmax, ymax);
         break;

      default:
         PUTMSG "CGMOps: Warning - Invalid value in PSspecarc\n");
   }
}

/***********************************************************************
* PSinitpsfile() : Optionally create, and write the prologue to the
*                  PostScript output file.
*
* Input Arguments:
*     str:   pointer to the text describing the metafile or picture.
*               PSprprolog uses it.
* Output Arguments:
*     None.
* Return value:
*     None.
*
* Algorithm :
*     Opens a new output file if needed. Done because one file should be
*     produced per metafile normally, but one file per picture when EPSF
*     is being produced.
*     Then the PostScript prologue should be output to the top of the
*     PostScript file.
*
************************************************************************/

#ifdef PROTO
static void PSinitpsfile(char *str)
#else
static void PSinitpsfile(str)
char *str;
#endif
{
  char *pathname, *func="PSinitpsfile";

  /*
   Create new output file?
     Depends on whether EPSF or not, and how many pages/mf's have already been
     output.
     Oh, and also whether we're outputing to stdout of course!!
  */
  if ( ( ( (no_of_page > 0) && (ps_mode==EPSF) ) ||
         ( (no_of_mf   > 0) && (ps_mode==NORMALPS) ) ) &&
       (*cgmofile != '\0') )
  {
    pathname=PSmfname();
    (void)fclose(cgmop);
    cgmop = CGMfopen (pathname, F_WRITE, CLEAR_TEXT);

    if (cgmop == NULL)
      exit(CGMerror(func, ERR_OPENFILE, FATAL, pathname));

    /* Write the name of the output file created to stderr. */
    PUTMSG "\t%s\n",pathname);
    if (ps_mode==NORMALPS)
      no_of_page=0;
  }

  /*
   Let's dump the PostScript prologue at the top of our output file.
  */
  PSprprolog(str);
}

/***********************************************************************
* PSendpsfile() : Write the ending comments to the PostSCript output file.
*
* Input Arguments:
*     None.
* Output Arguments:
*     None.
* Return value:
*     None.
*
* Algorithm :
*     Write the trailer comments, then depending on whether EPSF is being
*     output or not, write the bounding box size, or the number of pages in
*     the file.
*
************************************************************************/

#ifdef PROTO
static void PSendpsfile(void)
#else
static void PSendpsfile()
#endif
{
  PSOUT "%s\n","%%Trailer");
  PSOUT "end\n");
  PSOUT "%s\n","%%DocumentFonts:");

  if ( ps_mode == EPSF )
  {
    int x, y;

    if ( touching_sides == LANDSCAPE)
    {
       x = psbb;
       y = psbb * ( vdc_yrange / vdc_xrange );
    }
    else
    {
       x = psbb * ( vdc_xrange / vdc_yrange );
       y = psbb;
    }
    if( dev_ori == orient)
    {  /* Axes not rotated */
       PSOUT "%s 0 0 %d %d\n","%%BoundingBox:", x, y);
    }
    else
    { /* Axes rotated, but BoundingBox NOT rotated */
       PSOUT "%s 0 0 %d %d\n","%%BoundingBox:", y, x);
    }
  }
  else
    PSOUT "%s %d\n","%%Pages:",no_of_page);
}
