/*   RAL-CGM Interpreter module:  @(#) cgmoigl.c  version 3.4
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
 * Description: Output driver for Iris Graphics Library (Iris 3130)
 *
 *
 * Arguments:     stream   FILE structure for output file (not used)
 *
 *                c        CGM element code (character form)
 *
 * Maintenance Log:
 *   2 Nov 88 RTP  Original version stabilised
 *  17 Nov 88 RTP  Use typedefs
 *  16 Dec 88 RTP  Add bit patterns 16x16 only
 *  16 May 89 RTP  Defer initialisation till firstprimitive
 *   3 Aug 89 RTP  Correct error in DISJTLINE
 *  29 Sep 89 RTP  Use pointers instead of comarea
 *   6 Sep 90 RTP  Change internal routines to static
 *   9 Sep 90 RTP  Add Attribute checking routines
 *  12 Sep 90 RTP  Change GDP & fill routine names
 *   2 Oct 90 RTP  Add line drawing routine IGLtextline
 *                 Add attribute checking routines
 *  15 Oct 90 RTP  Add Text routines
 *   3 Dec 90 RTP  Change GDP calls from float to Double
 *  12 Dec 90 RTP  Change arguments to 'keepaspect' to long variables
 *  14 Dec 90 RTP  Replace 'sqrt' and 'fabs' by macros SQRT and FABS
 *  17 Dec 90 RTP  Correct checking of Text attributes
 *  28 Feb 91 RTP  Correct Cell array position
 *                 Add qdevice & qread for menu processing
 *   8 Mar 91 RTP  Add TXTfree() function to free memory
 *   3 May 91 CDS  Added new argument to IGLtxline and IGLtxfill, made
 *                 changing output matrix dependant on newchar. Added
 *                 pop and push around calls to Hershey and Bezier in
 *  14 May 91 RTP  Add ANSI declarations
 *  17 May 91 RTP  Change calls to calloc to use MALLOC
 *  20 May 91 RTP  Change free() calls to Macro FREE
 *  19 Jun 91 RTP  Add IGLoptions routine
 *                 Add header files for utilities
 *   2 Aug 91 RTP  Add SCCS id
 *   7 Aug 91 RTP  Tidy up after lint
 *  23 Aug 91 RTP  Add popup menu
 *                 Move bitplane check so that max colours is right when
 *                   end of metafile reached
 *   2 Sep 91 RTP  Open textport in fullscreen mode for random frame access
 *  21 Oct 91 RTP  Set current color after Cell Arrays
 *  29 Jul 92 RTP  Add VIRTUAL_RGB flag for Indigos so that direct colour is
 *                 now supported.
 *   5 Aug 92 RTP  Add Restricted text utility
 *  24 Aug 92 RTP  Add global iglvirtualrgb for indigos
 *                 Correct GKSCIRCLE centre point
 *   2 Sep 92 RTP  Remove CGMFILL and CGMEDGE macros. Replace with function
 *                 pointers
 *  20 Aug 93 RTP  Add New GL line functions (bgnline etc)
 *   9 Feb 94 RTP  Change 1<<n to 1L<<n where necessary
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *
 * Limitations:
 *
 *   Fill Area not yet correct (currently uses Convex fill only)
 *   Only 16*16 Patterns supported
 *   Patterns only supported as pixel maps
 *   Auxilary colour not supported
 *   Raster Text only in one size
 *   Redraw needed if Window moved
 *   Direct colour not supported in Window mode
 *   Colour mapping needs attention
 */

#include "cgmigl.h"
#include "cgmout.h"

#include "cgmtext.h"

#include "cgmgdp.h"
#include "cgmpoly.h"
#include "cgmtrap.h"
#include "cgmfill.h"
#include "cgmgraph.h"
#ifdef BEZIER
#include "cgmbez.h"
#endif
#ifdef HERSHEY
#include "cgmher.h"
#endif

#ifdef PROTO        /*  Function Prototyping */
   void CGMOigl( Code, Long*, Float*, char* );
   void IGLoptions ( char * );
   static void IGLinit( void ),
               IGLclose( long ),
               IGLnewpic( char * ),
               IGLmenu( Logical ),
               IGLattrib( Code ),
               IGLline( Long, Long*, Float*, Enum ),
               IGLmarker( Long, Long*, Float* ),
               IGLtext( Textitem * ),
               IGLcells( Long, Long* ),
               IGLgdp( Code, Long *, Float*, Enum );

  void IGLfill( Long, Point* );
  void IGLedge( Long, Point*, Enum* );
  void IGLtxline( Int, Point*, Tmatrix, int );
  void IGLtxfill( Int, Point*, Tmatrix, int );

#else
   void CGMOigl();     /* Main driver routine */
   void IGLoptions();  /* Parse Iris specific options */
   static void IGLinit(),      /* Initialise Graphics */
               IGLclose(),     /* Close down Graphics  */
               IGLnewpic(),    /* Start a new frame  */
               IGLmenu(),      /* Select Options */
               IGLattrib(),    /* Set Iris Attributes */
               IGLline(),      /* Draw a polyline from Data Area */
               IGLmarker(),    /* draw polymarkers */
               IGLtext(),      /* output text */
               IGLcells(),     /* Draw cell arrays */
               IGLgdp();       /* draw GDPs */

   void IGLfill(),             /* polygon fill */
        IGLedge();             /* Edge drawing */
   void IGLtxline();           /* Text line drawing routine */
   void IGLtxfill();           /* Text Fill area routine */

#endif /* ANSI */

/*   Direct colour calculation */

#define RGBVAL(x,y)  ( (short) ( ( 255 * (x - cur.min_rgb.y) )\
                      / (cur.max_rgb.y - cur.min_rgb.y) ) )

/*   Error messages  */

#define DMESS         (void) fprintf(stderr,

static char *func="CGMoigl", mess[48];
static int rt_mode;
static double rt_wid, rt_hgt;

/**************************************************** CGMOigl **********/

#ifdef PROTO
void CGMOigl ( Code c, Long *pi, Float *pr, char *str )
#else
void CGMOigl ( c, pi, pr, str )

/*  Outputs Metafile Element 'c' to the Iris Graphic Library */

Code c;
Long *pi;
Float *pr;
char *str;
#endif
{
   static Logical first = TRUE, inpic = FALSE, newpic= FALSE,
                  firstpic = TRUE;
   static char pic_name[32];
   Textitem *text_item;
   register Long n, i, j, num;
   Code major;

#ifdef DEBUG1
   DMESS "\n  CGMigl code: 0x%x\n", c);
#endif

   if ( c == (Code) EOF )
   {
      if ( iglmovie ) IGLmenu( WAIT );
      return;
   }
#ifdef IRIS  /* Only if running code on Iris */
   if ( iglopen ) IGLmenu ( NOWAIT );
   if ( igldebug )
   {
      DMESS "\n  Code: 0x%x\n", c);
      if ( inpic )IGLmenu (WAIT);
   }
#endif

   major = c>>8;
   num = *pi++;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         if ( newpic )
         {

/*  If first drawing element perform new frame processing */

            IGLnewpic(pic_name);
            newpic = FALSE;
            inpic = TRUE;
            first = TRUE;
         }

         switch (c)
         {
            case NONOP:
               return;

            case LINE:         /*  Polyline   */
               if ( first ) IGLattrib(LINE);
               IGLline ( num, pi, pr, NOSET);
               first = ( num >= ZERO );
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first ) IGLattrib(LINE);
               for (i = 1; i <= abs(num); i += 2)
               {
                  IGLline ((Long)2, pi, pr, NOSET);
                  pi += 4; pr += 4;
               }
               first = ( num >= ZERO );
               break;

            case MARKER:         /*  Polymarker  */
               if ( first ) IGLattrib(MARKER);
               IGLmarker ( num, pi, pr);
               first = ( num >= ZERO );
               break;

            case TEXT:         /*  Text   */
               ATTtext(NTEXTB, text_bundle, ncolours, &igltext);
               if (cur.vdc_type == REAL)
               {
                  igltxt.pos.x = *pr++;
                  igltxt.pos.y = *pr++;
               }
               else
               {
                  igltxt.pos.x = (Float) *pi++;
                  igltxt.pos.y = (Float) *pi++;
               }
               rt_mode = 0;
               text_item = TXTaccinf(num, str, &igltext, text_method);
               if ( (Enum) num == FINAL ) IGLtext( text_item );
               break;

            case RESTRTEXT:         /*  Restricted Text */
               ATTtext(NTEXTB, text_bundle, ncolours, &igltext);
               if (cur.vdc_type == REAL)
               {
                  rt_wid = igltxt.width  = *pr++;
                  rt_hgt = igltxt.height = *pr++;
                  igltxt.pos.x = *pr++;
                  igltxt.pos.y = *pr++;
               }
               else
               {
                  rt_wid = igltxt.width  = *pi++;
                  rt_hgt = igltxt.height = *pi++;
                  igltxt.pos.x = *pi++;
                  igltxt.pos.x = *pi++;
               }
               rt_mode = 1;
               text_item = TXTaccinf(num, str, &igltext, text_method);
               if ( (Enum) num == FINAL ) IGLtext( text_item );
               break;

            case APNDTEXT:         /*  Append Text   */
               ATTtext(NTEXTB, text_bundle, ncolours, &igltext);
               text_item = TXTaccinf(num, str, &igltext, text_method);
               if ( (Enum) num == FINAL ) IGLtext( text_item );
               break;

            case POLYGON:         /*  Polygon   */
               IGLattrib(POLYGON);
               FILarea ( num, pi, pr, NOSET, !first, IGLfill, IGLedge );
               break;

            case POLYGONSET:         /*  Polygon Set  */
               IGLattrib(POLYGON);
               FILarea ( num, pi, pr, SET, !first, IGLfill, IGLedge );
               break;

            case CELLARRAY:         /*  Cell Array  */
               if ( first )
               {
                  if (cur.vdc_type == REAL)
                  {
                     cell.p.x = *pr++;
                     cell.p.y = *pr++;
                     cell.q.x  = *pr++;
                     cell.q.y  = *pr++;
                     cell.r.x  = *pr++;
                     cell.r.y  = *pr++;
                     pi += 6;
                  }
                  else
                  {
                     cell.p.x = *pi++;
                     cell.p.y = *pi++;
                     cell.q.x  = *pi++;
                     cell.q.y  = *pi++;
                     cell.r.x  = *pi++;
                     cell.r.y  = *pi++;
                  }
                  cell.nx = *pi++;
                  cell.ny = *pi++;
                  pi++;              /* Ignore Local Precision */
                  cell.d.x  = (cell.r.x - cell.p.x)/cell.nx;
                  cell.dd.x = (cell.q.x - cell.r.x)/cell.nx;
                  cell.d.y  = (cell.r.y - cell.q.y)/cell.ny;
                  cell.dd.y = (cell.r.y - cell.p.y)/cell.ny;
                  cell.px = FABS( (cell.r.x - cell.p.x) *
                   (sc_right - sc_left) / (wc_right - wc_left) ) + 0.5;
                  cell.py = FABS( (cell.r.y - cell.q.y) *
                         (sc_top - sc_bot) / (wc_top - wc_bot) ) + 0.5;
                  IGLattrib (CELLARRAY);
               }
               IGLcells ( num, pi );
               first = ( num >= ZERO );
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               j = *pi++;
               if ( cgmralgks )
               {
                  Enum close;

                  close = ( j == GKSPIE   ? PIE :
                            j == GKSCHORD ? CHORD
                                          : NOCLOSE );
                  IGLgdp ( (Code) j, pi, pr, close);
               }
               else
               {
                  (void) sprintf(mess,"(GDP type: %d)", j);
                  (void) CGMerror(func, ERR_NOGDP, ERROR, mess);
               }
               break;

            case RECT:         /*  Rectangle   */
               IGLgdp (RECT, pi, pr, NOCLOSE);
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVPRIM,ERROR,mess);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */

         switch (c)
         {

            case BEGMF:         /*  Begin Metafile   */
               IGLinit();
               new_linewidth  = FALSE;
               new_markersize = FALSE;
               new_edgewidth  = FALSE;
               new_charheight = FALSE;
               break;

            case ENDMF:         /*  End Metafile   */
/*
               IGLclose(iglpic);
*/
               break;

            case BEGPIC:        /*  Begin Picture Descriptor   */
               strcpy (pic_name, str);
               if (firstpic)
               {
                  for ( i = 0; i < ncolours; i++ )
                  {
                      defcolor[i] = colors[i];
                  }
                  firstpic = FALSE;
               }
               if ( cgmverbose && ! iglfullscrn )
                  DMESS "New frame: %s\n", pic_name);
               else
/* Only reset colours that have been used */
                  for ( i = mincol; i < maxcol; i++ )
                  {
                      if ( ! colors[i].flag )
                      {
                         colors[i] = defcolor[i];
                      }
                  }
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               if (cur.vdc_type == REAL)
               {
                  wc_left   = cur.vdc_extent.a.x.real;
                  wc_bot    = cur.vdc_extent.a.y.real;
                  wc_right  = cur.vdc_extent.b.x.real;
                  wc_top    = cur.vdc_extent.b.y.real;
               }
               else
               {
                  wc_left   = cur.vdc_extent.a.x.intr;
                  wc_bot    = cur.vdc_extent.a.y.intr;
                  wc_right  = cur.vdc_extent.b.x.intr;
                  wc_top    = cur.vdc_extent.b.y.intr;
               }
               newpic = TRUE;   /*  defer processing till first draw */
               IGLattrib(BEGPIC);
               break;

            case ENDPIC:         /*  End  Picture    */
               if ( iglopen )
               {
                  if ( igldouble ) swapbuffers();
                  IGLmenu( ! iglmovie );
               }
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVDELIM,ERROR,mess);
         }
         break;


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {
            case MFVERSION:         /*  Metafile version   */

            case MFDESC:         /*  Metafile Description  */

            case VDCTYPE:         /*  VDC Type   */

            case INTEGERPREC:         /*  Integer Precision  */

            case REALPREC:         /*  Real Precision   */

            case INDEXPREC:         /*  Index Precision   */

            case COLRPREC:         /*  Colour Precision  */

            case COLRINDEXPREC:         /*  Colour Index Precision  */

            case MAXCOLRINDEX:         /*  Maximum Colour Index  */

            case COLRVALUEEXT:         /*  Colour value extent  */

            case MFELEMLIST:         /*  Metafile element List  */

            case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement*/

            case ENDMFDEFAULTS: /* End Metafile defaults Replacement */

            case FONTLIST:         /*  Font List   */

            case CHARSETLIST:         /*  Character set list  */

            case CHARCODING:         /*  Character coding Announcer  */
               break;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {
            case COLRMODE:         /*  Colour Selection Mode */

            case SCALEMODE:         /*  Scaling Mode   */

            case LINEWIDTHMODE:    /*  Line width Specification  */

            case MARKERSIZEMODE:   /*  Marker size Specification  */

            case EDGEWIDTHMODE:     /*  Edge width Specification  */
               break;

            case VDCEXT:         /*  VDC Extent    */
               if (cur.vdc_type == REAL)
               {
                  wc_left   = cur.vdc_extent.a.x.real;
                  wc_bot    = cur.vdc_extent.a.y.real;
                  wc_right  = cur.vdc_extent.b.x.real;
                  wc_top    = cur.vdc_extent.b.y.real;
                  cur.clip_rect.a.x.real = wc_left;
                  cur.clip_rect.a.y.real = wc_bot;
                  cur.clip_rect.b.x.real = wc_right;
                  cur.clip_rect.b.y.real = wc_top;
               }
               else
               {
                  wc_left   = cur.vdc_extent.a.x.intr;
                  wc_bot    = cur.vdc_extent.a.y.intr;
                  wc_right  = cur.vdc_extent.b.x.intr;
                  wc_top    = cur.vdc_extent.b.y.intr;
                  cur.clip_rect.a.x.intr = wc_left;
                  cur.clip_rect.a.y.intr = wc_bot;
                  cur.clip_rect.b.x.intr = wc_right;
                  cur.clip_rect.b.y.intr = wc_top;
               }
               break;

            case BACKCOLR:         /*  Background Colour  */
               colors[0].r = RGBVAL( cur.back.red, red );
               colors[0].g = RGBVAL( cur.back.green, green );
               colors[0].b = RGBVAL( cur.back.blue, blue );
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVPDESC,ERROR,mess);
         }
         break;

      case 0x33:  /* Control Elements  */

         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision  */

            case VDCREALPREC:       /* VDC Real Precision  */

            case AUXCOLR:       /* Auxiliary Colour  */

            case TRANSPARENCY:       /* Transparency  */
               break;

            case CLIPRECT:       /* Clip Rectangle  */
               if ( !inpic || newpic ) break;
               if (cur.vdc_type == REAL)
               {
                  cl_left  = sc_left +
                     FABS( (cur.clip_rect.a.x.real - wc_left ) * iglatt.xgrain);
                  cl_bot   = sc_bot +
                     FABS( (cur.clip_rect.a.y.real - wc_bot) * iglatt.ygrain);
                  cl_right = sc_left +
                     FABS( (cur.clip_rect.b.x.real - wc_left ) * iglatt.xgrain);
                  cl_top   = sc_bot +
                     FABS( (cur.clip_rect.b.y.real - wc_bot) * iglatt.ygrain);
               }
               else
               {
                  cl_left  = sc_left +
                     FABS( (cur.clip_rect.a.x.intr - wc_left ) * iglatt.xgrain);
                  cl_bot   = sc_bot +
                     FABS( (cur.clip_rect.a.y.intr - wc_bot) * iglatt.ygrain);
                  cl_right = sc_left +
                     FABS( (cur.clip_rect.b.x.intr - wc_left ) * iglatt.xgrain);
                  cl_top   = sc_bot +
                     FABS( (cur.clip_rect.b.y.intr - wc_bot) * iglatt.ygrain);
               }

/* make sure clip is not outside screen */

               if (cl_left < sc_left) cl_left = sc_left;
               if (cl_bot < sc_bot) cl_bot = sc_bot;
               if (cl_right > sc_right) cl_right = sc_right;
               if (cl_top > sc_top) cl_top = sc_top;

            case CLIP:       /* Clip Indicator  */
               if ( !inpic || newpic ) break;
               if (cur.clip_ind == ON)
                  scrmask (cl_left, cl_right, cl_bot, cl_top);
               else
                  scrmask (sc_left, sc_right, sc_bot, sc_top);
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVCONT,ERROR,mess);
         }
         break;

      case 0x34:  /* Circles and Ellipses  */

         if ( newpic )
         {
            IGLnewpic(pic_name);
            newpic = FALSE;
            inpic = TRUE;
         }

         switch(c)
         {
            case CIRCLE:       /* Circle      */
               IGLgdp (CIRCLE, pi, pr, NOCLOSE);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               IGLgdp (ARC3PT, pi, pr, NOCLOSE);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               IGLgdp (ARC3PTCLOSE, pi, pr, (Enum) *(pi+6) );
               break;

            case ARCCTR:       /* Circle Arc centre */
               IGLgdp (ARCCTR, pi, pr, NOCLOSE);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               IGLgdp (ARCCTRCLOSE, pi, pr, (Enum) *(pi+7) );
               break;

            case ELLIPSE:       /* Ellipse    */
               IGLgdp (ELLIPSE, pi, pr, NOCLOSE);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               IGLgdp (ELLIPARC, pi, pr, NOCLOSE);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               IGLgdp (ELLIPARCCLOSE, pi, pr, (Enum) *(pi+10) );
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVOUT,ERROR,mess);
         }
         break;

      case 0x35:  /* Attributes  */
         att_flag = TRUE;
         switch(c)
         {
            case CHARHEIGHT:       /*  Character Height   */
               new_charheight = TRUE;
               break;

            case LINEWIDTH:       /*  Line Width */
               new_linewidth = ( cur.linewidth_mode == ABSOLUTE );
               break;

            case MARKERSIZE:       /*  Marker Size */
               new_markersize = ( cur.markersize_mode == ABSOLUTE );
               break;

            case LINEINDEX:       /*  Line Bundle index  */

            case LINETYPE:       /*  Line Type   */

            case LINECOLR:       /*  Line Colour  */

            case MARKERINDEX:       /*  Marker Bundle index  */

            case MARKERTYPE:       /*  Marker Type   */

            case MARKERCOLR:       /*  Marker Colour  */

            case TEXTINDEX:       /*  Text Bundle index  */

            case TEXTFONTINDEX:       /*  Text Font Index  */

            case TEXTPREC:       /*  Text Precision   */

            case CHAREXPAN:       /*  Character Expansion Factor  */

            case CHARSPACE:       /*  Character Spacing  */

            case TEXTCOLR:       /*  Text Colour   */

            case CHARORI:       /*  Character Orientation */

            case TEXTPATH:       /*  Text Path */

            case TEXTALIGN:       /*  Text Alignment */

            case CHARSETINDEX:      /*  Character Set Index */

            case ALTCHARSETINDEX:   /*  Altern Character Set Index */
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVATT,ERROR,mess);
               break;
         }
         break;

      case 0x36:  /* More Attributes  */
         att_flag = TRUE;
         switch(c)
         {
            case EDGEWIDTH:       /*  Edge Width */
               new_edgewidth = ( cur.edgewidth_mode == ABSOLUTE );
               break;

            case FILLINDEX:       /*  Fill Bundle index  */

            case INTSTYLE:       /*  Interior Style  */

            case FILLCOLR:       /*  Fill Colour */

            case HATCHINDEX:       /*  Hatch Index  */

            case PATINDEX:       /*  Pattern Index  */

            case EDGEINDEX:       /*  Edge Bundle index  */

            case EDGETYPE:       /*  Edge Type  */

            case EDGECOLR:       /*  Edge Colour  */

            case EDGEVIS:       /*  Edge Visibility  */

            case FILLREFPT:       /*  Fill Reference Point  */
               break;

            case PATTABLE:       /*  Pattern Table  */
               j = *(pi++) - 1;
               if ( j > PATTERNS )
               {
                  (void) sprintf(mess, "(Index: %d)", j);
                  (void) CGMerror(func,ERR_PATINDEX,ERROR,mess);
                  break;
               }
               iglpat[j].col  = *pi++;
               iglpat[j].row  = *pi++;
               n = iglpat[j].row * iglpat[j].col;
               if ( maxpat + n > PATT_TAB )
               {
                  (void) sprintf(mess,
                       "(needs: %d  available: %d) Pattern ignored",
                                 n, PATT_TAB - maxpat);
                  (void) CGMerror(func,ERR_PATTABLE,WARNING,mess);
                  break;
               }
               else
               {
                  short *pt = &patts[maxpat];

                  iglpat[j].prec = (*pi++) - 1;
                  iglpat[j].ptab = pt;
                  maxpat += n;

#ifdef DEBUG1
   DMESS "  User Pattern: %d - %d x %d prec: %d\n",
                                   j+1, iglpat[j].row, iglpat[j].col,
                                        iglpat[j].prec );
#endif

/*  Set Pattern table here  */

                  for ( i = ZERO; i < n; i++)  *pt++ = *pi++;
               }
               break;

            case PATSIZE:       /*  Pattern Size  */

/*  Not yet implemented */

               break;

            case COLRTABLE:       /*  Colour Table  */
               {
                  i = *pi++;   /* colour Table Index */
                  if ( i < mincol ) mincol = i;
                  for (j = ZERO; j < num ; i++, j++)
                  {
                     if ( i > ncolours )
                     {
                        (void) sprintf(mess, "(Index: %d)", i);
                        (void) CGMerror(func,ERR_MAXCOLR,ERROR,mess);
                        break;
                     }
                     colors[i].r = RGBVAL( *pi++, red );
                     colors[i].g = RGBVAL( *pi++, green );
                     colors[i].b = RGBVAL( *pi++, blue );
                     colors[i].flag = TRUE;
                  }
                  if ( i > maxcol ) maxcol = i;
               }
               break;

            case ASF:       /*  Aspect source flags  */
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVATT,ERROR,mess);
               break;

         }
         break;

      case 0x37:  /* Escape And External Elements  */
         switch (c)
         {
            case ESCAPE:         /*  Escape  */
               break;

            case MESSAGE:         /*  Message  */
               break;

            case APPLDATA:         /*  Application Data  */
               break;

            default:
               (void) sprintf(mess, "(code: 0x%x)", c);
               (void) CGMerror(func,ERR_INVESC,ERROR,mess);
               break;
         }
         break;

      default:
         (void) sprintf(mess, "(code: 0x%x)", c);
         (void) CGMerror(func,ERR_INVELEM,ERROR,mess);
         break;
     }

     return;
  }

/***************************************************** IGLoptions ******/
#ifdef PROTO
void IGLoptions( char *opts )
#else
void IGLoptions( opts )
char *opts;
#endif

{
   int i, len = strlen(opts);

   if ( opts == NULL | !len ) return;

   for ( i=0; i < len; i++ )
   {
      switch ( toupper( opts[i] ) )
      {
         case 'M':  /*  Movie mode */
           igldouble = TRUE;
         case 'C':  /*  Continuous play mode */
           iglmovie = TRUE;
           break;
         case 'S':  /*  Single frame mode */
           iglmovie = FALSE;
           igldouble = FALSE;
           break;
         case 'D':  /*  Double buffer mode */
           igldouble = TRUE;
           break;
         case 'B':  /*  Single buffer mode */
           igldouble = FALSE;
           break;
         case 'F':  /*  Textport off */
           iglfullscrn = TRUE;
           break;
         case 'V':  /*  Virtual 24bit on */
           iglvirtualrgb = TRUE;
           break;
         case 'P':  /*  Virtual 24bit off */
           iglvirtualrgb = FALSE;
           break;
         case 'T':  /*  Textport on */
           iglfullscrn = FALSE;
           break;
         case 'L':  /* Debug mode on */
           igldebug = TRUE;
           break;
      }
   }
#ifdef DEBUG
   DMESS "Iris options:");
   if ( iglmovie ) DMESS " movie");
   if ( igldouble ) DMESS " double buffer");
   else             DMESS " single buffer");
   if (  ! iglfullscrn ) DMESS " textport");
   if ( igldebug ) DMESS " debug");
   DMESS "/n");
#endif
   return;
}

/***************************************************** IGLattrib *******/

#ifdef PROTO
static void IGLattrib ( Code type)
#else
static void IGLattrib ( type )

/*  Converts CGM Attribute 'type' to Iris Attribute  */

Code type;
#endif

{
    static Logical first_text = TRUE, first_fill = TRUE,
                   first_line = TRUE, first_mark = TRUE,
                   last_prim = ZERO;

    register Long j;

#ifdef DEBUG1
   DMESS  "IGLattrib: type 0x%x\n", type);
#endif

    switch (type)
    {
       case LINE:

/* if no attribute has changed and last primitive was a line return */

          if ( ! att_flag && last_prim == LINE ) return;

          if ( first_line )
          {
/*  define CGM linestyles  */

              deflinestyle (1, 0xFFFF);
              deflinestyle (2, 0xF0F0);
              deflinestyle (3, 0x8888);
              deflinestyle (4, 0xE4E4);
              deflinestyle (5, 0xF888);
              setlinestyle (ZERO);

              first_line = FALSE;
          }

          ATTline(NLINEB, line_bundle, ncolours, &iglline );

          if (cur.linewidth_mode == SCALED )
             igl.linewidth = (int) iglline.width;
          else
             igl.linewidth = (int)(iglline.width*iglatt.xgrain);
          if ( igl.linewidth < 1 ) igl.linewidth = 1;
          if (cur.color_mode == INDEXED)
             igl.colr = iglline.colindex;
          else
          {
             igl.red = RGBVAL(iglline.col.red, red);
             igl.green = RGBVAL(iglline.col.green, green);
             igl.blue = RGBVAL(iglline.col.blue, blue);
          }

/* now check if there is a change and set accordingly */

          if( iglline.type != current.linestyle )
          {
             setlinestyle (iglline.type);
             current.linestyle = iglline.type;
          }

          if ( igl.linewidth != current.linewidth )
          {
             linewidth ( igl.linewidth );
          /* lsrepeat ( igl.linewidth );  */
             current.linewidth = igl.linewidth;
          }

#ifdef DEBUG1
   DMESS "  Line type: %d  width: %d  colour: %d\n",
                   iglline.type, igl.linewidth, igl.colr);
#endif

          last_prim = LINE;
          break;


      case MARKER:

/* if no attribute has changed and last primitive was a marker return */

          if ( ! att_flag  && last_prim == MARKER ) return;

          if ( first_mark )
          {
              float point[2];

/*  Generate markers   */

/* point */
              makeobj (markers[0]=genobj() );
                 pnt2( 0.0, 0.0 );
              closeobj();

/* plus */
              makeobj (markers[1]=genobj() );
                 BEGINLINE;
                   MOVE(-0.5,  0.0);
                   DRAW( 0.5,  0.0);
       ENDLINE;
       BEGINLINE;
                   MOVE( 0.0, -0.5);
                   DRAW( 0.0,  0.5);
       ENDLINE;
              closeobj();

/* asterix  */
              makeobj (markers[2]=genobj() );
                 BEGINLINE;
                   MOVE(-0.5,  0.0);
                   DRAW( 0.5,  0.0);
                 ENDLINE;
                 BEGINLINE;
                   MOVE( 0.0, -0.5);
                   DRAW( 0.0,  0.5);
                 ENDLINE;
                 BEGINLINE;
                   MOVE(-0.5, -0.5);
                   DRAW( 0.5,  0.5);
                 ENDLINE;
                 BEGINLINE;
                   MOVE(-0.5,  0.5);
                   DRAW( 0.5, -0.5);
                 ENDLINE;
              closeobj();

/* circle  */
              makeobj (markers[3]=genobj() );
                 circ (0.0, 0.0, 0.5);
              closeobj();

/* cross */
              makeobj (markers[4]=genobj() );
                 BEGINLINE;
                   MOVE(-0.5, -0.5);
                   DRAW( 0.5,  0.5);
                 ENDLINE;
                 BEGINLINE;
                   MOVE(-0.5,  0.5);
                   DRAW( 0.5, -0.5);
                 ENDLINE;
              closeobj();

              first_mark = FALSE;
          }

          ATTmarker(NMARKB, mark_bundle, ncolours, &iglmark);
          if (cur.markersize_mode == SCALED )
             igl.markersize = 4.0 * iglmark.size / iglatt.xgrain;
          else
             igl.markersize = iglmark.size;
          if (cur.color_mode == INDEXED)
             igl.colr = iglmark.colindex;
          else
          {
             igl.red = RGBVAL(iglmark.col.red, red);
             igl.green = RGBVAL(iglmark.col.green, green);
             igl.blue = RGBVAL(iglmark.col.blue, blue);
          }

/* now check if there is a change and set accordingly */

          if( iglmark.type != current.marktype )
          {
             marker = markers[iglmark.type - 1];
             current.marktype = iglmark.type;
          }

          if ( iglmark.type == 1 )  markscale = 1.0;
          else
          {
             current.linewidth = 1;
             linewidth ( current.linewidth );
             current.linestyle = ZERO;
             setlinestyle (ZERO);
             markscale = igl.markersize;
          }
#ifdef DEBUG1
   DMESS "  Marker type: %d  size: %f  colour: %d\n",
                   igl.marktype, igl.markersize, igl.colr);
#endif

          last_prim = MARKER;
          break;

      case TEXT:

          if ( iglline.type != 0 )
          {
             iglline.type = 0;
             setlinestyle (iglline.type);
          }
          if ( iglline.width != 1 )
          {
             iglline.width = 1;
             linewidth ( igl.linewidth );
          }

          if (cur.color_mode == INDEXED)
             igl.colr = igltext.colindex;
          else
          {
             igl.red = RGBVAL(igltext.col.red, red);
             igl.green = RGBVAL(igltext.col.green, green);
             igl.blue = RGBVAL(igltext.col.blue, blue);
          }

#ifdef DEBUG
   DMESS "  Text Font: %d  prec: %d  colour: %d\n",
                   igl.font, igl.prec, igl.colr);
#endif

          if ( first_text || current.font != igl.font )
          {
   /*   Get new font details */
   /*  Set alignment parameters as fractions of height and width */
             igltxt.left = 0.0;
             igltxt.centre = 0.5;
             igltxt.right = 1.0;
             igltxt.base = igltxt.bottom = 0.0;
             igltxt.half = 0.5;
             igltxt.top = igltxt.cap = 1.0;
          }

   /*  Set text sizes  */

          if ( cur.vdc_type == REAL )
             igltxt.height = curatt.char_height.real;
          else
             igltxt.height = curatt.char_height.intr;

          igltxt.width = igl.expansion * igltxt.height;

          first_text = FALSE;
          last_prim = TEXT;
          break;

      case POLYGON:  /*  Fill attributes  */

/* if no attribute has changed and last primitive was a Fill Area return */

          if ( ! att_flag && last_prim == POLYGON ) return;

          ATTfill(NFILLB, fill_bundle, ncolours, &iglfill);

          if ( first_fill && (iglfill.style == PATTERN
                           || iglfill.style == HATCH) )
          {
/*  first time through generate Hatch Styles  */

             for (j = ZERO; j < HATCHES ; j++)
                defpattern( (short)(j+1), (short) 16, hatch[j]);
             setpattern (ZERO);

             first_fill = FALSE;
          }

          switch ( iglfill.style )
          {
             register Long i, j, k, m;
             register short *pat;

             case HOLLOW:
                setlinestyle ( ZERO );
                linewidth ( 1 );

             case SOLID:
                igl.texture = ZERO;
                break;

             case PATTERN:
                if (cur.color_mode == DIRECT)
                {
                   (void) CGMerror(func,ERR_DIRPATT,ERROR,mess);
                   break;
                }
                patind = iglfill.pattern - 1;

/*  Check if pattern is 16 by 16 */

                if ( iglpat[patind].row != 16 &&
                     iglpat[patind].col != 16 )
                {
                   (void) sprintf(mess, "(Pattern: %d size %d x %d)",
                                patind, iglpat[patind].row,
                                        iglpat[patind].col );
                   (void) CGMerror(func,ERR_BIGPATT,ERROR,mess);
                   break;
                }
/*  Set current pattern masks */

                patprec = iglpat[patind].prec;
                pat = (short *) iglpat[patind].ptab;

                for ( i = 0; i < iglpat[patind].row; i++ )
                {
                   for ( k = 0; k < patprec; k++) patmask[k][i] = 0;

#ifdef DEBUG1
   DMESS "Pattern row %d:", i);
#endif
                   for ( j = 0, m = 15; j < iglpat[patind].col;
                                                    j++, m--, pat++ )
                   {
#ifdef DEBUG1
   DMESS " %d", *pat);
#endif
                      for ( k = 0; k < patprec; k++)
                         patmask[k][i] |= ( *pat>>k & 1 ) <<m;
                   }
#ifdef DEBUG1
   for ( k = 0; k < patprec; k++)
      DMESS " 0x%x", patmask[k][i] );
   DMESS "\n");
#endif
                }
                igl.texture = HATCHES + 1;
                igl.colr = ncolours - 1;
                break;

             case HATCH:
                igl.texture = (iglfill.hatch < ZERO
                                   ? GKShatch[abs(curatt.hatch_ind)-1]
                                   : iglfill.hatch );
                break;

             case EMPTY:
                return;

          }

          if ( curatt.int_style != PATTERN )
          {
             if (cur.color_mode == INDEXED)
                igl.colr = iglfill.colindex;
             else
             {
                igl.red = RGBVAL(iglfill.col.red, red);
                igl.green = RGBVAL(iglfill.col.green, green);
                igl.blue = RGBVAL(iglfill.col.blue, blue);
             }
          }

/* now check if there is a change and set accordingly */

          if( igl.texture != current.texture )
          {
             current.texture = igl.texture;
             setpattern ( igl.texture );
          }

#ifdef DEBUG1
   DMESS "  Fill style: %d  pattern index: %d texture: %d colour: %d\n",
                   iglfill.style, patind+1, igl.texture, igl.colr);
#endif

          last_prim = POLYGON;
          break;

      case EDGEVIS:    /*  Edge attributes */

/* if no attribute has changed and last primitive was a edge return */

          if ( ! att_flag && last_prim == EDGEVIS ) return;

          if ( first_line )
          {
/*  define CGM linestyles  */

              deflinestyle (1, 0xFFFF);
              deflinestyle (2, 0xF0F0);
              deflinestyle (3, 0x8888);
              deflinestyle (4, 0xE4E4);
              deflinestyle (5, 0xF888);
              setlinestyle (ZERO);

              first_line = FALSE;
          }

          ATTedge(NEDGEB, edge_bundle, ncolours, &igledge);

          if (cur.edgewidth_mode == SCALED )
             igl.linewidth = (Int) igledge.width;
          else
             igl.linewidth = (Int)(igledge.width*iglatt.xgrain);
          if ( igl.linewidth < 1 ) igl.linewidth = 1;
          if (cur.color_mode == INDEXED)
             igl.colr = igledge.colindex;
          else
          {
             igl.red = RGBVAL(igledge.col.red, red);
             igl.green = RGBVAL(igledge.col.green, green);
             igl.blue = RGBVAL(igledge.col.blue, blue);
          }

/* now check if there is a change and set accordingly */

          if( igl.linestyle != current.linestyle )
          {
             setlinestyle (igl.linestyle);
             current.linestyle = igl.linestyle;
          }

          if ( igl.linewidth != current.linewidth )
          {
             linewidth ( igl.linewidth );
             current.linewidth = igl.linewidth;
          }

#ifdef DEBUG1
   DMESS "  Edge type: %d  width: %d  colour: %d\n",
           igl.linestyle, igl.linewidth, igl.colr);
#endif

          last_prim = EDGEVIS;
         break;

      case CELLARRAY:  /*  Cell array - affects texture & colour*/
#ifdef DEBUG1
   DMESS "  Cell array: (%f,%f) (%f,%f)\n",
           cell.p.x, cell.p.y, cell.q.x, cell.q.y);
   DMESS "  Cell size: (%f,%f) (%f,%f)",
           cell.d.x, cell.d.y, cell.dd.x, cell.dd.y);
   DMESS " : %d x %d\n", cell.nx, cell.ny);
#endif
         setpattern(ZERO);
         current.texture = ZERO;
#ifdef IRIS
         if ( cur.color_mode == INDEXED )
         for ( j=1; j<ncolours; j++ )
         {
/* map any colours that have not been used yet */
            if ( colors[j].flag )
            {
#ifdef DEBUG1
   DMESS "code: %x Colour: %d (%d)\n", type, igl.colr, ncolours);
#endif
               mapcolor((Colorindex) j, colors[j].r, colors[j].g, colors[j].b);
               colors[j].flag = FALSE;
            }
         }
#endif
         last_prim = CELLARRAY;
         current.colr = 0;
         return;

      case BEGPIC:
         igl = igldefault;
         current.colr = cur.back.index;
         last_prim = ZERO;
         att_flag = TRUE;
         return;

      default:
         (void) sprintf ( mess, "(code: 0x%x)", type);
         (void) CGMerror(func,ERR_INVATT,ERROR,mess);
         break;
   }

   if (cur.color_mode == DIRECT)
      RGBcolor ( igl.red, igl.green, igl.blue);
   else
      if (igl.colr != current.colr)
      {
         if ( colors[igl.colr].flag )
         {
#ifdef DEBUG1
   DMESS "code: %x Colour: %d (%d)\n", type, igl.colr, ncolours);
#endif
            mapcolor(igl.colr, colors[igl.colr].r,
                     colors[igl.colr].g, colors[igl.colr].b);
            colors[igl.colr].flag = FALSE;
         }
         color(igl.colr);
         current.colr = igl.colr;
      }

   att_flag = FALSE;

   return;
}

/***************************************************** IGLinit *********/

#ifdef PROTO
static void IGLinit( void )
#else
static void IGLinit()
#endif

/*  Initialise Variables and colour map */

{
    register Long i;

#ifdef DEBUG1
   DMESS  "IGLinit:\n" );
#endif

/*  Make default colour map  */

   for ( i = ZERO; i < 256; i++ )
   {
      defcolor[i].r = GKScol[i].red;
      defcolor[i].g = GKScol[i].green;
      defcolor[i].b = GKScol[i].blue;
   }

        /*  256 - 511  Grey scales */

   for ( i = 256; i < 512; i++ )
   {
      defcolor[i].r = i;
      defcolor[i].g = i;
      defcolor[i].b = i;
   }

/*  512 - 1023  Rainbows  */

   for ( i = 512; i < 1024; i++ )
   {
      short r, g, b;

      r = (i>>0) & 7;
      g = (i>>3) & 7;
      b = (i>>6) & 7;
      r = ( 255*r )/7;
      g = ( 255*g )/7;
      b = ( 255*b )/7;
      defcolor[i].r = r;
      defcolor[i].g = g;
      defcolor[i].b = b;
   }

   ncolours = COLOUR_TAB;
   mincol = COLOUR_TAB;
   maxcol = 0;

   for ( i = 0; i < ncolours; i++ )
   {
/*  if flag is TRUE then colour has not been mapped */
      defcolor[i].flag = TRUE;
      colors[i] = defcolor[i];
   }

/* Initialise Attribute structures */

   iglline.shared = &iglatt;
   iglmark.shared = &iglatt;
   igltext.shared = &iglatt;
   iglfill.shared = &iglatt;
   igledge.shared = &iglatt;

/* Set Hardware font characteristics in font structure */



/* Set flags for Scale factors */

   new_linewidth  = FALSE;
   new_markersize = FALSE;
   new_edgewidth  = FALSE;
   new_charheight = FALSE;

   return;
}

/***************************************************** IGLnewpic *******/

#ifdef PROTO
static void IGLnewpic( char *pic_name)
#else
static void IGLnewpic(pic_name)

/*   New Frame processing started at first drawing element  */

char *pic_name;
#endif
{
   static Logical first = TRUE;
   register Long i;
   Float scale;
   static Scoord left = 0, bot = 0,     /* Max screen dimensions */
          right = XMAXSCREEN, top = YMAXSCREEN;

#ifdef DEBUG1
   DMESS "IGLnewpic: pic no %d '%s'\n", iglpic, pic_name);
#endif

   if ( first )
   {
      if ( ismex() )  /*  Are we in the Window Manager */
      {
         Long j, k;

         j = XMAXSCREEN * FABS( wc_right - wc_left );
         k = XMAXSCREEN * FABS ( wc_top - wc_bot );
         keepaspect ( j, k );
         foreground();
         iglpic = winopen("CGM");
         getsize (&j, &k);
         left = ZERO;
         bot  = ZERO;
         right = j;
         top   = k;
         winattach();
	 clear();
      }
      else
      {
         ginit();
         color ( ZERO );
         clear ();
         cursoff();
         if ( cur.color_mode == INDEXED &&
              !iglfullscrn && (sc_right + 200) < XMAXSCREEN )
         {
/*  set text color to white */

            textcolor (8);
            pagecolor (4);   /* Textport color = blue  */
            textport ( (short) sc_right, XMAXSCREEN, ZERO, YMAXSCREEN );
         }
         else tpoff();
      }
      iglopen = TRUE;

      if ( igldouble )
      {
         doublebuffer();
      }

/* Queue left and middle mouse buttons */

      qdevice(LEFTMOUSE);
      qdevice(MIDDLEMOUSE);

      first = FALSE;
   }  /* first */

/*  Find out how many colours supported  */

   i = 1L<<getplanes();
#ifdef DEBUG
   DMESS "Number of colours = %d\n", i );
#endif
#ifndef VIRTUAL_RGB
   if ( cur.color_mode == DIRECT && !iglvirtualrgb && getplanes() < 24 )
   {
      exit ( CGMerror(func,ERR_DIRCOL,FATAL, NULLSTR) );
   }
#endif
   ncolours = ( i < COLOUR_TAB ? i : COLOUR_TAB);
   if ( cur.color_mode == DIRECT )
      RGBmode();
#ifdef IRIS4D
   else
      cmode();
   concave(TRUE);
#endif

   gconfig();

   ortho2 ( wc_left, wc_right, wc_bot, wc_top);
   asp_rat = FABS( (wc_top - wc_bot) / (wc_right - wc_left) );

/*  Make sure picture fits on screen  */

   sc_bot = bot;
   sc_left = left;

   if ( asp_rat < (top-bot+1)/(right-left+1) )
   {
/* Picture is wider than screen, change width */
      sc_top = top;
      sc_right = left + (Float)(top - bot + 0.5) / asp_rat;
      if (sc_right > right)
      {
         sc_right = right;
         sc_top = bot + (Float)(right - left + 0.5) * asp_rat;
      }
   }
   else
   {
/* Picture is taller than screen, change height */
      sc_right = right;
      sc_top = bot + (Float)(right - left + 0.5) * asp_rat;
      if (sc_top > top)
      {
         sc_top = top;
         sc_right = left + (Float)(top - bot + 0.5) / asp_rat;
      }
   }

/*  Turn off Textport if not enough screen left */

   if ( ! ismex() && (sc_right + 200 ) > right ) tpoff();

   iglatt.xgrain = (sc_right - sc_left + 1) / FABS(wc_right - wc_left);
   iglatt.ygrain = (sc_top - sc_bot + 1) / FABS(wc_top - wc_bot);
   iglwvrat = ( iglatt.xgrain > iglatt.ygrain ? iglatt.xgrain
                                              : iglatt.ygrain );

   viewport (sc_left, sc_right, sc_bot, sc_top);

/*  Set defaults dependent on VDC */

   scale = ( FABS(wc_right - wc_left) > FABS(wc_top - wc_bot)
                      ? FABS(wc_right - wc_left)
                      : FABS(wc_top - wc_bot) );

   if ( cur.vdc_type == REAL )
   {
      if ( !new_linewidth && cur.linewidth_mode == ABSOLUTE )
                     curatt.line_width.real = scale / 1000;
      if ( !new_markersize && cur.markersize_mode == ABSOLUTE )
                     curatt.mark_size.real = scale / 100;
      if ( !new_edgewidth && cur.edgewidth_mode == ABSOLUTE )
                     curatt.edge_width.real = scale / 1000;
      if ( !new_charheight )
                     curatt.char_height.real = scale / 100;
   }
   else
   {
      if ( !new_linewidth && cur.linewidth_mode == ABSOLUTE )
                     curatt.line_width.intr = scale / 1000;
      if ( !new_markersize && cur.markersize_mode == ABSOLUTE )
                     curatt.mark_size.intr = scale / 100;
      if ( !new_edgewidth && cur.edgewidth_mode == ABSOLUTE )
                     curatt.edge_width.intr = scale / 1000;
      if ( !new_charheight )
                     curatt.char_height.intr = scale / 100;
   }

/*  Set Background colour and Clear screen  */

   if ( cur.color_mode == INDEXED )
   {
        mapcolor ( (Colorindex) cur.back.index,
                   colors[cur.back.index].r,
                   colors[cur.back.index].g,
                   colors[cur.back.index].b);
        color( (Colorindex) cur.back.index );
   }
   else RGBcolor( (short) cur.back.red,
                  (short) cur.back.green,
                  (short) cur.back.blue);

   setpattern(ZERO);
   if ( ismex() ) wintitle(pic_name);
   clear();

#ifdef DEBUG1
   DMESS " Window: %f %f %f %f\n",
                             wc_left, wc_right, wc_bot, wc_top);
   DMESS " Viewport: %d %d %d %d\n",
                             sc_left, sc_right, sc_bot, sc_top);
   DMESS " Aspect: %f  wvrat: %f (x %f) (y %f)\n",
                             asp_rat, iglatt.xgrain, iglatt.ygrain);
#endif

/*  Set Clip rectangle */

   if (cur.vdc_type == REAL)
   {
      cl_left  = sc_left +
         FABS( (cur.clip_rect.a.x.real - wc_left ) * iglatt.xgrain);
      cl_bot   = sc_bot +
         FABS( (cur.clip_rect.a.y.real - wc_bot) * iglatt.ygrain);
      cl_right = sc_left +
         FABS( (cur.clip_rect.b.x.real - wc_left ) * iglatt.xgrain);
      cl_top   = sc_bot +
         FABS( (cur.clip_rect.b.y.real - wc_bot) * iglatt.ygrain);
   }
   else
   {
      cl_left  = sc_left +
         FABS( (cur.clip_rect.a.x.intr - wc_left ) * iglatt.xgrain);
      cl_bot   = sc_bot +
         FABS( (cur.clip_rect.a.y.intr - wc_bot) * iglatt.ygrain);
      cl_right = sc_left +
         FABS( (cur.clip_rect.b.x.intr - wc_left ) * iglatt.xgrain);
      cl_top   = sc_bot +
         FABS( (cur.clip_rect.b.y.intr - wc_bot) * iglatt.ygrain);
   }

/* make sure clip is not outside screen */

   if (cl_left < sc_left) cl_left = sc_left;
   if (cl_bot < sc_bot) cl_bot = sc_bot;
   if (cl_right > sc_right) cl_right = sc_right;
   if (cl_top > sc_top) cl_top = sc_top;

   if (cur.clip_ind == ON)
      scrmask (cl_left, cl_right, cl_bot, cl_top);
   else
      scrmask (sc_left, sc_right, sc_bot, sc_top);

   return;
}

/***************************************************** IGLmenu *********/

#ifdef PROTO
static void IGLmenu( Logical wait )
#else
static void IGLmenu( wait )
Logical wait;
#endif

/*  If wait is FALSE then
       check for pause or abort
    Else if wait is TRUE then
       check run end of frame menu
*/

{
   short val, left = 0, middle = 0;
   Logical abort = FALSE, random = FALSE;

#ifdef IRIS   /* Only run on Iris */

/* Read queued events and store number of button presses */

   while ( qtest() )
   {
      switch( qread(&val) )
      {
         case LEFTMOUSE:
            left++;
            break;
         case MIDDLEMOUSE:
            middle++;
            break;
         default:
            break;
      }
   }
/* if no event and not endpic Return */
   if ( !left && !middle && !wait ) return;

   if ( wait ) ringbell();

/* Make sure no button is pressed */
   while ( getbutton(LEFTMOUSE) || getbutton(MIDDLEMOUSE) || getbutton(RIGHTMOUSE) )
   ;

   if ( ismex() )
   {
      long menu, submenu, option, num;
      char menustr[80];
      char *MainMenu=
           "Options %t|Next|Redraw|Last|Find|Quit %x11";
      char *Numbers=
           "0 %x0|1 %x1|2 %x2|3 %x3|4 %x4|5 %x5|6 %x6|7 %x7|8 %x8|9 %x9 %l";

/* Wait for left or right mouse button */
      while ( ! getbutton(LEFTMOUSE) && ! getbutton(RIGHTMOUSE) )
      ;
      menu = defpup( MainMenu );
      do
      {
         option = dopup( menu );
         switch ( option )
         {
            case 1:    /* next */
               break;
            case 2:    /* redraw */
               cgmnext--;
               break;
            case 3:    /* last */
               cgmnext -= 2;
               break;
            case 4:    /* random */
               num = 0;
               do
               {
                  submenu = newpup( );
                  sprintf( menustr, "Find Frame %d %%x11 %%l", num );
                  addtopup( submenu, menustr );
                  addtopup( submenu, Numbers );
                  addtopup( submenu, "undo %x12" );
                  option = dopup( submenu );
                  if ( option >= 0 && option <= 9 )
                     num = 10*num + option ;
                  if ( option == 12 ) num /= 10;
                  freepup( submenu );
               } while ( option != 11 );
               cgmnext = num;
               break;
            case 11:
               abort = TRUE;
               break;
            default:
               break;
         }
      } while ( option == -1 );
      freepup( menu );
   }
   else
   {

/*  if left & middle buttons then abort  */
      if ( left && middle )
      {
         abort = TRUE;
      }
/*  if left and not endpic then wait for left button */
      else if ( left && ! wait )
      {
         while ( ! getbutton(LEFTMOUSE) )  /* Wait for next press */
         ;
         while ( getbutton(LEFTMOUSE) )    /* Wait for release */
            if ( getbutton(MIDDLEMOUSE) ) abort = TRUE; /* Abort if middle */
      }
/*  if WAIT then wait for button press */
      else if ( wait )
         {
         if ( cgmverbose && ! iglfullscrn )
             printf( "Press LEFT mouse button for next frame\n" );
/*  Wait for a button press */
         while ( ! getbutton(LEFTMOUSE) && ! getbutton(MIDDLEMOUSE) )
         ;
/*  If middle button prompt for next frame */
         random = getbutton(MIDDLEMOUSE);
      }
      while ( getbutton(LEFTMOUSE) )
         if ( getbutton(LEFTMOUSE)
           && getbutton(MIDDLEMOUSE) ) abort = TRUE;
   }

   if ( random )
   {
      color(0);
      viewport( 0, XMAXSCREEN, 0, YMAXSCREEN);
      clear();
      if ( iglfullscrn ) tpon();
      gconfig();
      color(1);
      printf( "Next Frame (current %d):\n", --cgmnext );
      scanf( "%d", &cgmnext );
  /*  abort on negative frame number */
      if ( cgmnext <= 0 ) abort = TRUE;
      if ( iglfullscrn ) tpoff();
   }

   if ( abort )
   {
      IGLclose( iglpic );
      exit(0);
   }

   qreset();                     /*  Clear event queue */
#endif

   return;
}

/***************************************************** IGLclose ********/

#ifdef PROTO
static void IGLclose( long iglpic )
#else
static void IGLclose( iglpic )
long iglpic;
#endif

/*  Close down the Graphics  */

{

#ifdef DEBUG1
   DMESS  "IGLclose:\n");
#endif

/* Return if Graphics not open */

    if ( ! iglopen ) return;

/* clear screen & remap first 8 colours to Iris default colors */

    if (cur.color_mode == DIRECT)
       RGBcolor(0, 0, 0);
    else
    {
       mapcolor( BLACK,   0,   0,   0);
       color(0);
    }
    clear();
    mapcolor( RED,     255,   0,   0);
    mapcolor( GREEN,     0, 255,   0);
    mapcolor( YELLOW,  255, 255,   0);
    mapcolor( BLUE,      0,   0, 255);
    mapcolor( MAGENTA, 255,   0, 255);
    mapcolor( CYAN,      0, 255, 255);
    mapcolor( WHITE,   255, 255, 255);

   if ( ismex() )
   {
       winclose(iglpic);
   }
   else
   {
       greset();
       color(ZERO);
       clear();
       tpon();
       curson();
       textinit();
       gexit();
   }
   iglopen = FALSE;

   return;
}


/***************************************************** IGLline *********/

#ifdef PROTO
static void IGLline (  Long n, Long *pi, Float *pr, Enum set)
#else
static void IGLline ( n, pi, pr, set)

/*   draws n point polyline starting at pi/pr  */

Long n, *pi;
Float *pr;
Enum set;
#endif
{
   static Logical first = TRUE;
   register Long j, np;
   Logical vis;
   float point[2];

#ifdef DEBUG1
   DMESS  "IGLline: %d points (set: %d)\n", n, set);
#endif

   np = abs(n);
   if ( np < 2 ) return;

   if (first)
   {
      resetls (FALSE);
      vis = FALSE;     /*  Move to first point */
   }

   for ( j = ZERO; j < np; j++ )
   {
      if ( cur.vdc_type == INTEGER )
      {
         if ( vis )
    {
       DRAW( *pi, *(pi+1) );
         }
         else
         {
       BEGINLINE;
       MOVE( *pi, *(pi+1) );
    }
         pi += 2;
      }
      else
      {
         if ( vis )
    {
       DRAW( *pr, *(pr+1) );
         }
         else
    {
       BEGINLINE;
       MOVE( *pr, *(pr+1) );
    }
         pr += 2;
      }
      vis = ( set ? ( *pi++ & 0x01 ) : TRUE);
   }
   ENDLINE;

   first = ( n >= ZERO );
   if (first) resetls (TRUE);

   return;
}

/***************************************************** IGLmarker *******/

#ifdef PROTO
static void IGLmarker ( Long n, Long *pi, Float *pr)
#else
static void IGLmarker ( n, pi, pr)

/*   draws n markers starting at pi/pr  */

Long n, *pi;
Float *pr;
#endif
{
   register Long j;

#ifdef DEBUG1
   DMESS  "IGLmarker: %d points\n", n);
#endif

   if ( REQUAL(markscale,0.0) ) return;

   pushmatrix ();
   scale (markscale, markscale, ZERO);

   for ( j = ZERO; j < abs(n); j++)
   {
      pushmatrix();
      if ( cur.vdc_type == INTEGER )
      {
         translate ( *pi/markscale, *(pi+1)/markscale , 0.0);
         pi += 2;
      }
      else
      {
         translate ( *pr/markscale, *(pr+1)/markscale , 0.0);
         pr += 2;
      }
      callobj (marker);
      popmatrix();
   }
   popmatrix ();
   return;
}

/***************************************************** IGLtext *********/

#ifdef PROTO
static void IGLtext ( Textitem *txt )
#else
static void IGLtext ( txt )

/*  draws TEXT Textitem list 'txt' according to current settings */

Textitem *txt;
#endif
{
   Long len;
   Textitem *curtxt = txt, *txtcp;
   char *s;

#ifdef DEBUG1
   DMESS  "IGLtext: at (%f,%f)\n", igltxt.pos.x, igltxt.pos.y );
#endif

   if ( rt_mode) curtxt = TXTrestrict(curtxt, rt_wid, rt_hgt, &igltext);

   if ( curtxt != NULL )
   {
      TXTalign ( txt, igltxt.pos, TXTgwidth(txt), TXTgheight(txt) );

      while ( curtxt != NULL )
      {
         igltext.colindex = curtxt->colourindex;
         igltext.col.red = curtxt->text_col.red;
         igltext.col.green = curtxt->text_col.green;
         igltext.col.blue = curtxt->text_col.blue;
         IGLattrib(TEXT);
         switch ( curtxt->text_method )
         {
            case HER:
#ifdef HERSHEY
               pushmatrix();
               HERtext( curtxt, igltxt.pos, IGLtxline );
               popmatrix();
               break;
#endif
            case BEZ:
#ifdef BEZIER
               pushmatrix();
               BEZtext( curtxt, &igltext, igltxt.pos, IGLtxfill );
               popmatrix();
               break;
#endif
         case TEX:
         case HDW:
         default:

               s = curtxt->str;
               len = strlen(s);
#ifdef DEBUG
   DMESS  "Hardware Text  len: %d  height: %f  width: %f\n",
                          len, igltxt.height, igltxt.width);
#endif

               switch (curatt.text_halign)
               {
                  case ZERO:  /*  Normal Horizontal  */
                     break;
                  case 1:  /*  Left  */
                     igltxt.pos.x -= igltxt.left * len * igltxt.width;
                     break;
                  case 2:  /*  Centre*/
                     igltxt.pos.x -= igltxt.centre * len * igltxt.width;
                     break;
                  case 3:  /*  Right */
                     igltxt.pos.x -= igltxt.right * len * igltxt.width;
                     break;
                  case 4:  /*  Continuous Horiz  */
                     igltxt.pos.x -= curatt.text_hcont * igltxt.width;
                     break;
               }

               switch (curatt.text_valign)
               {
                  case ZERO:  /*  Normal Vertical  */
                     break;
                  case 1:  /*  Top   */
                     igltxt.pos.y -= igltxt.top * igltxt.height;
                     break;
                  case 2:  /*  Cap   */
                     igltxt.pos.y -= igltxt.cap * igltxt.height;
                     break;
                  case 3:  /*  Half */
                     igltxt.pos.y -= igltxt.half * igltxt.height;
                     break;
                  case 4:  /*  Base */
                     igltxt.pos.y -= igltxt.base * igltxt.height;
                     break;
                  case 5:  /*  Bottom */
                     igltxt.pos.y -= igltxt.bottom * igltxt.height;
                     break;
                  case 6:  /*  Continuous Vertical  */
                     igltxt.pos.y -=  curatt.text_hcont * igltxt.height;
                     break;
               }
#ifdef DEBUG1
   DMESS  "  Text at: (%f,%f)\n", igltxt.pos.x, igltxt.pos.y);
#endif

               switch (curatt.text_prec)
               {
                  case ZERO:   /*  string  */
                  case 1:   /*  character  */
                  case 2:   /*  stroke  */
                     cmov2 ( igltxt.pos.x, igltxt.pos.y );
                     charstr (s);
                     igltxt.pos.x += len * igltxt.width + igl.space;
                     break;
               }

#ifdef DEBUG1
   DMESS  "  New text pos: (%f,%f)\n", igltxt.pos.x, igltxt.pos.y );
#endif
               break;
         }
         curtxt = curtxt->next;
      }
      TXTfree(txt);
   }

   return;
}

/***************************************************** IGLcells ********/

#ifdef PROTO
static void IGLcells ( Long num, Long *pi )
#else
static void IGLcells ( num, pi )

/*  Draws a Cell array of n values 'pi' */

Long num, *pi;
#endif
{
   static Long scanline, line, cells;
   static Logical first = TRUE, angled;
   static Colorindex *ix;
   static RGBvalue *red, *green, *blue;
   static Float x0, y0;
   register Long i, k, m;
   Long *pmax;
   register Float x1, y1;

#ifdef DEBUG_CELL
   DMESS  "IGLcells: %d x %d cells\n",cell.nx,cell.ny);
#endif

   if ( !iglvirtualrgb && cur.color_mode == DIRECT )
   {
      (void) CGMerror(func,ERR_DIRCELL,ERROR,mess);
      return;
   }

   if ( first )
   {
      x0 = cell.p.x - cell.dd.x;
      y0 = cell.p.y + cell.d.y;
      k = scanline = ZERO;
      line = ( 2 * cell.py + 1 );
      angled = !( VEQUAL( cell.dd.x, 0.0 )
                  && VEQUAL( cell.dd.y, 0.0 ) );
      cells = cell.nx * cell.ny;
      if ( cur.color_mode == DIRECT )
      {
         red = (RGBvalue *) MALLOC ( cells, sizeof(RGBvalue) );
         green = (RGBvalue *) MALLOC ( cells, sizeof(RGBvalue) );
         blue = (RGBvalue *) MALLOC ( cells, sizeof(RGBvalue) );
         if ( red == NULL || green == NULL || blue == NULL )
         {
            (void) CGMerror ("IGLcells", ERR_NOMEMORY, ERROR, NULLSTR);
            return;
         }
         pmax = pi + 3*cells;
      }
      else
      {
         ix = (Colorindex *) MALLOC ( cells, sizeof(Colorindex) );
         if ( ix == NULL )
         {
            (void) CGMerror ("IGLcells", ERR_NOMEMORY, ERROR, NULLSTR);
            return;
         }
         pmax = pi + cells;
      }
   }

#ifdef DEBUG_CELL
   DMESS  " cells: %d x %d", cell.nx, cell.ny);
   DMESS  " pixels: %d x %d", cell.px, cell.py );
   if ( angled ) DMESS "(rotated)");
#endif

   for ( ; scanline < cell.ny; scanline++ )
   {
              /* get next scanline  */

#ifdef DEBUG_CELL
   DMESS  " Line: %d (%d %d)\n", line, k, scanline );
#endif

      for ( ; k < cell.nx ; k++)
      {
         if ( pi > pmax ) return;
         if ( cur.color_mode == DIRECT )
         {
            red[k] = *pi++;
            green[k] = *pi++;
            blue[k] = *pi++;
         }
         else
            ix[k] = *pi++;
      }
      k = ZERO;
      x0 += cell.dd.x;
      y0 -= cell.d.y;

/*  Check if scanline is printable  */

      if ( cell.py < cell.ny && scanline == line/(2 * cell.ny) && scanline )
      {
#ifdef DEBUG_CELL
   DMESS  " scanline skipped %d (%d)\n", scanline, line/(2*cell.ny) );
#endif
          continue;
      }

      line += 2 * cell.py;

      if ( angled || cell.px > cell.nx || cell.py > cell.ny)
      {
/* pixels greater than no of cells */

         for ( m = ZERO, x1 = x0, y1 = y0; m < cell.nx; m++ )
         {
            if ( cur.color_mode == DIRECT )
               RGBcolor( (short) red[m], (short) green[m], (short) blue[m] );
            else
               color (ix[m]);
            pmv2 (x1, y1);
            rpdr2 (cell.d.x, cell.dd.y);
            rpdr2 (cell.dd.x, -cell.d.y);
            rpdr2 (-cell.d.x, -cell.dd.y);
            pclos();
            x1 += cell.d.x;
            y1 += cell.dd.y;
         }
      }
      else
      {
         if ( cell.px < cell.nx )
         {
                /* pixels less than number of cells */
            for ( i = 1; i < cell.px; i++)
               if ( cur.color_mode == DIRECT )
               {
                  red[i] = red[ (i * cell.nx) / cell.px ];
                  green[i] = green[ (i * cell.nx) / cell.px ];
                  blue[i] = blue[ (i * cell.nx) / cell.px ];
               }
               else
                  ix[i] = ix[ (i * cell.nx) / cell.px ];
         }

/*  pixels less than or equal to number of cells  */

#ifdef DEBUG_CELL
   DMESS  " Scanline: %d (%f %f)\n", cell.px, x0, y0 );
#endif

         cmov2 (x0, y0);
         if ( cur.color_mode == DIRECT )
            writeRGB( (short) cell.px, red, green, blue);
         else
            writepixels( (short) cell.px, ix);
      }

   }
   first = ( num >= ZERO );
   if ( first ) FREE( ix );

   return;
}

/***************************************************** IGLgdp **********/

#ifdef PROTO
static void IGLgdp ( Code type, Long *pi, Float *pr, Enum close )
#else
static void IGLgdp ( type, pi, pr, close )

/*  draws GDP 'type' with points starting at pi/pr
    for convenience CGM primitives are handled as GDPs  */

Code type;
Long *pi;
Float *pr;
Enum close;
#endif
{
   Long j, np;
   Logical fill, edge, GKSgdp, isarc, centreknown = FALSE;
   Point p1, p2, p3, p4, p5;
   Double rad, ang, ang1, ang2;
   Point centre, cdp1, cdp2;
   Lpoint *pt = iglpts;
   Float point[2];

/*  work out if a RAL-GKS GDP  */

   GKSgdp = ( type == GKSCIRCLE || type == GKSARC ||
              type == GKSPIE    || type == GKSCHORD );

/*  work out if this is an arc  */

   isarc = ( type == GKSARC || type == ARC3PT || type == ARCCTR ||
                                               type == ELLIPARC );

/*  decide whether to draw fill area, edge or both and set attributes  */

   fill = ( ! isarc && curatt.int_style != EMPTY
                  && curatt.int_style != HOLLOW) * curatt.int_style;
   edge = ( isarc || curatt.edge_vis == ON || curatt.int_style == HOLLOW );

   if ( fill ) IGLattrib ( POLYGON );
   else        IGLattrib ( LINE );

/* make sure area is visible */

   if ( ! (fill || edge) ) return;

#ifdef DEBUG1
   DMESS  "IGLgdp: 0x%x  int style: %d", type, fill);
   if ( GKSgdp ) DMESS " (RAL-GKS) ");
   if ( edge ) DMESS " (edge on) ");
   if ( close == PIE ) DMESS " (pie) ");
   if ( close == CHORD ) DMESS " (chord) ");
   if ( isarc ) DMESS " (arc) ");
   DMESS "\n");
#endif

/*  At present ignore transformation of GKS GDPs */

   if (GKSgdp)
   {
       pi += 6; pr += 6;
   }

/*  get 5 points and convert to real values */

   if (cur.vdc_type == REAL)
   {
      p1.x = *pr++;   p1.y = *pr++;
      p2.x = *pr++;   p2.y = *pr++;
      p3.x = *pr++;   p3.y = *pr++;
      p4.x = *pr++;   p4.y = *pr++;
      p5.x = *pr++;   p5.y = *pr++;
   }
   else
   {
      p1.x = *pi++;   p1.y = *pi++;
      p2.x = *pi++;   p2.y = *pi++;
      p3.x = *pi++;   p3.y = *pi++;
      p4.x = *pi++;   p4.y = *pi++;
      p5.x = *pi++;   p5.y = *pi++;
   }

#ifdef DEBUG1

/*  Print type of GDP and parameters   */

   switch ( type )
   {
      case GKSARC:
      case ARC3PT:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PTCLOSE:
         DMESS "  Arc 3pt: (%f,%f) (%f,%f) (%f,%f)",
                             p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
         break;

      case ARCCTR:
      case ARCCTRCLOSE:
         DMESS "  Arc ctr: (%f,%f) %f (%f,%f) (%f,%f)",
                             p1.x, p1.y, p4.x, p2.x, p2.y, p3.x, p3.y);
         break;

      case GKSCIRCLE:
         DMESS "  Circle: (%f,%f) (%f,%f)",
                                    p1.x, p1.y, p2.x, p2.y);
         break;

      case CIRCLE:
         DMESS "  Circle: (%f,%f)  rad: %f",
                                    p1.x, p1.y, p2.x);
         break;

      case RECT:
         DMESS "  Rectangle: (%f,%f) (%f,%f)",
                                   p1.x, p1.y, p2.x, p2.y);
         break;

      case ELLIPSE:
         DMESS "  Ellipse: (%f,%f) (%f,%f) (%f,%f)",
                                   p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
         break;

      case ELLIPARCCLOSE:
      case ELLIPARC:
         DMESS "  Elliparc: (%f,%f) (%f,%f) (%f,%f) (%f,%f) (%f,%f)",
                     p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y,
                     p5.x, p5.y );
         break;

      default:
         (void) sprintf ( mess, "(type: 0x%x)", type);
         (void) CGMerror(func,ERR_NOGDP,ERROR,mess);
         break;
   }
   switch ( close )
   {
      case PIE:
         DMESS " (pie)\n");
         break;
      case CHORD:
         DMESS " (chord)\n");
         break;
      case NOCLOSE:
      default:
         DMESS "\n");
         break;
   }
#endif

/*  Now work out significant points  */

   switch ( type )
   {
      case ARCCTR:
      case ARCCTRCLOSE:
         centre = p1;
         rad = p4.x;
         GDParc (&centre, rad, &p2, &p3, &ang, &p1, &p3);
         centreknown = TRUE;

      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
         if ( ! centreknown )
                GDPcentre ( &centre, &rad, &ang, &p1, &p2, &p3);

#ifdef DEBUG1
   DMESS  "  Centre: (%f,%f)  rad: %f ang: %f\n",
             centre.x, centre.y, rad, ang );
#endif

         if ( REQUAL( ang, 0.0 ) )
         {
                     /*  Points are co-linear  */
            p2 = p3;
            type = LINE;
            break;
         }
         if ( REQUAL(ang, 2.0 * PI) )
         {
                    /*  Points form circle  */
            if (GKSgdp) type = GKSCIRCLE;
            else        type = CIRCLE;
            if ( fill == PATTERN )
            {
               GDPcircle ( &centre, (Double)rad, (Double)ang, &p1,
                           (Double) iglwvrat, &np, pt, close );
               type = SOFTWARE;
            }
            break;
         }
         if ( close == CHORD || fill == PATTERN )
         {
                   /*  Work out points  */
            GDPcircle ( &centre, (Double)rad, (Double)ang, &p1,
                        (double) iglwvrat, &np, pt, close );
            type = SOFTWARE;
         }
         else
         {
            ang1 = atan2( p1.y - centre.y, p1.x - centre.x);
            ang2 = atan2( p3.y - centre.y, p3.x - centre.x);
#ifdef DEBUG1
   DMESS  "  angles (radians): %f (%f to %f)\n", ang, ang1, ang2 );
#endif
            if ( ang < 0.0 )
            {
                         /*  Clockwise  - Swap points */
               ang  = ang1;
               ang1 = ang2;
               ang2 = ang;
            }
#ifdef DEBUG1
   DMESS  "  angles (degrees): %f (%f to %f)\n",
                       ang*180.0/PI , ang1*180.0/PI, ang2*180.0/PI );
#endif
         }
         break;

      case GKSCIRCLE:
         centre = p2;
         rad = SQRT( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
         if ( fill == PATTERN )
         {
            GDPcircle ( &centre, (Double) rad, (Double) 2*PI, &p2,
                        (Double) iglwvrat, &np, pt, close );
            type = SOFTWARE;
         }
         break;

      case CIRCLE:
         centre = p1;
         rad = p2.x;
         p1.x += rad;
         if ( fill == PATTERN )
         {
            GDPcircle ( &centre, (Double)rad, (Double)2*PI, &p1,
                        (Double) iglwvrat, &np, pt, close );
            type = SOFTWARE;
         }
         break;


      case RECT:
         if ( fill == PATTERN )
         {
            np = 4;
            BEGINLIST(pt)
            STOREPOINT ( pt, p1);
            STOREVALUE ( pt, p1.x, p2.y );
            STOREPOINT ( pt, p2 );
            STOREVALUE ( pt, p2.x, p1.y );
            ENDLIST(pt)
            pt = iglpts;
            type = SOFTWARE;
         }
         break;

      case ELLIPSE:
         centre = p1;
         cdp1 = p2;
         cdp2 = p3;
         p4.x = 1.0;  p4.y = 0.0;
         GDPellipse( &centre, &cdp1, &cdp2, &p4, &p4, (Double) iglwvrat,
                     &np, pt, close );
         type = SOFTWARE;
         break;

      case ELLIPARC:

      case ELLIPARCCLOSE:
         centre = p1;
         cdp1 = p2;
         cdp2 = p3;
         if ( VEQUAL( p2.x*p3.y - p3.x*p2.y, 0.0 ) )
         {
                     /*  Points are co-linear  */
            p1 = p3;
            type = LINE;
            break;
         }
         GDPellipse( &centre, &cdp1, &cdp2, &p4, &p5, iglwvrat,
                     &np, pt, close);
         type = SOFTWARE;
         break;

      default:
         break;
   }

/* now draw primitives  */

   switch ( type )
   {
      case SOFTWARE:
         if (fill)
         {
            if ( close == PIE )
            {
/*  split in two to fill so areas are convex  */
               j = np/2;
               IGLfill( j, pt);
               j--;
               IGLfill( np-j, pt + j);
            }
            else
               IGLfill ( np, pt);
         }
         if (edge)
         {
            if ( ! GKSgdp ) IGLattrib(EDGEVIS);
            BEGINLINE;
            MOVE( pt->x, pt->y );
            pt++;
            for ( j = 1; j < np ; pt++, j++ )
       {
          DRAW( pt->x, pt->y );
       }
         }
         ENDLINE;
         break;

      case ARCCTR:
      case ARCCTRCLOSE:
      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
         ang1 *= 1800/PI;  ang2 *= 1800/PI;
         if (fill) arcf ( centre.x, centre.y, rad, (short)ang1, (short)ang2 );
         if (edge)
         {
            if ( ! GKSgdp )  IGLattrib ( EDGEVIS );
            arc  ( centre.x, centre.y, rad, (short)ang1, (short)ang2 );
            if ( ! isarc )
            {              /*  Add in centre */
          BEGINLINE;
               MOVE ( p1.x, p1.y );
               DRAW ( centre.x, centre.y );
               DRAW ( p3.x, p3.y );
          ENDLINE;
            }
         }
         break;

      case GKSCIRCLE:
         if ( fill ) circf ( centre.x, centre.y, rad );
         if ( edge ) circ  ( centre.x, centre.y, rad );
         break;

      case CIRCLE:
         if ( fill ) circf ( centre.x, centre.y, rad );
         if ( edge )
         {
            IGLattrib ( EDGEVIS );
            circ  ( centre.x, centre.y, rad );
         }
         break;

      case LINE:    /*   Points are co-linear  */
         BEGINLINE;
         MOVE ( p1.x, p1.y );
         DRAW ( p2.x, p2.y );
    ENDLINE;
         break;

      case RECT:
         if ( fill ) rectf ( p1.x, p1.y, p2.x, p2.y );
         if ( edge )
         {
            IGLattrib (EDGEVIS);
            rect ( p1.x, p1.y, p2.x, p2.y );
         }
         break;

      case ELLIPSE:

      case ELLIPARC:

      case ELLIPARCCLOSE:

      default:
         break;
   }
   return;
}

/***************************************************** IGLfill *********/

#ifdef PROTO
void IGLfill ( Long np, Point *pt )
#else
void IGLfill ( np, pt )

/*  Fills convex polygon of np points in *pt  */

Long np;
Point *pt;
#endif

{
   register Int i;
   Colorindex wm;
   Coord point[2];

#ifdef DEBUG1
   DMESS  "IGLfill: %d points 0x%x int style: %d\n",
                    np, pt, curatt.int_style);
#endif

   switch (curatt.int_style)
   {
      case SOLID:
      case HATCH:
         bgnpolygon();
         for ( i=0; i<np; i++, pt++ )
         {
            point[0] = pt->x; point[1] = pt->y;
	    v2f( point );
         }
         endpolygon();
         break;

      case PATTERN:
         wm = getwritemask ();
         for ( i = 0; i < patprec; i++ )
         {
            writemask ( (Colorindex) 1L<<i );
            defpattern ( igl.texture, (short) 16, patmask[i] );
            setpattern ( igl.texture );
#ifdef DEBUG1
   DMESS  "Mask: 0x%x Pattern: %d prec: %d (0x%x)\n",
                          1L<<i, igl.texture, patprec, wm );
#endif
            bgnpolygon();
            for ( i=0; i<np; i++, pt++ )
            {
               point[0] = pt->x; point[1] = pt->y;
	       v2f( point );
            }
            endpolygon();
        }
         writemask ( wm );
         break;
   }

   return;
}

/***************************************************** IGLedge *********/

#ifdef PROTO
void IGLedge ( Long np, Point *pt, Enum *edgeflag )
#else
void IGLedge ( np, pt, edgeflag )

/*  Fills convex polygon of np points in pt[]  */

Long np;
Point *pt;
Enum *edgeflag;
#endif

{
   Long j;
   Logical vis, close, set = (edgeflag != NULL);
   Point *p0;

#ifdef DEBUG
   DMESS "IGLedge: %d points 0x%x 0x%x\n", np, pt, edgeflag);
#endif

   if ( curatt.int_style != HOLLOW ) IGLattrib( EDGEVIS );

   for ( j = 0, close = TRUE, vis = FALSE; j < np; j++, pt++)
   {
      float point[2];

      if ( vis )
      {
         DRAW ( pt->x, pt->y );
      }
      else
      {
         BEGINLINE;
         MOVE ( pt->x, pt->y );
      }
      if ( close ) {
         p0 = pt;
         close = FALSE;
      }

      if ( set )
      {
         vis = (*edgeflag & VIS);
         if ( *edgeflag & EDGECLOSE )  /* close */
         {
        if ( vis )
             {
                DRAW ( p0->x, p0->y );
           ENDLINE;
             }
             vis = FALSE;
             close = TRUE;
         }
         edgeflag++;
      }
      else vis = TRUE;
   }
   ENDLINE;
   return;
}

/***************************************************** IGLtxline *******/

#ifdef PROTO
void IGLtxline ( Int np, Point *pt, Tmatrix matrix, int newchar )
#else
void IGLtxline ( np, pt, matrix, newchar )

/*  Draw a Polyline - used by text routines    */

Int np;
Point *pt;
Tmatrix matrix;
int newchar;
#endif

{
   int i;
   Float point[2];

/*  Transpose 2D to 3D matrix */
   if ( newchar )
   {
      tmat[0][0] = (Float) matrix[0][0];
      tmat[1][1] = (Float) matrix[1][1];
      tmat[1][0] = (Float) matrix[1][0];
      tmat[0][1] = (Float) matrix[0][1];
      tmat[3][0] = (Float) matrix[2][0];
      tmat[3][1] = (Float) matrix[2][1];

      popmatrix();
      pushmatrix();
      multmatrix( tmat );
   }
   BEGINLINE;
   MOVE(pt->x, pt->y);
   pt++;
   for ( i=1; i<np; i++, pt++ )
   {
      DRAW(pt->x,pt->y);
   }
   ENDLINE;

   return;
}

/***************************************************** IGLtxfill *******/

#ifdef PROTO
void IGLtxfill ( Int np, Point *pt, Tmatrix matrix, int newchar )
#else
void IGLtxfill ( np, pt, matrix, newchar )

/*  Fill a Polygon - used by text routines    */
/*  Currently only uses Line drawing routine  */

Int np;
Point *pt;
Tmatrix matrix;
int newchar;
#endif

{
   int i;
   Point *pt0 = pt;
   float point[2];

/*  Transpose 2D to 3D matrix */
   if ( newchar )
   {
      tmat[0][0] = (Float) matrix[0][0];
      tmat[0][1] = (Float) matrix[0][1];
      tmat[1][1] = (Float) matrix[1][1];
      tmat[1][0] = (Float) matrix[1][0];
      tmat[3][0] = (Float) matrix[2][0];
      tmat[3][1] = (Float) matrix[2][1];

      popmatrix();
      pushmatrix();
      multmatrix( tmat );
   }

/* pmv2(pt->x, pt->y);  */
   BEGINPOLYGON;
   MOVE(pt->x, pt->y);
   pt++;
   for ( i=1; i<np; i++, pt++)
   {
/*    pdr2(pt->x,pt->y);  */
      DRAW(pt->x,pt->y);
   }
   ENDPOLYGON;

   pt = pt0;
   BEGINLINE;
   MOVE(pt->x, pt->y);
   pt++;
   for ( i=1; i<np; i++, pt++)
   {
      DRAW(pt->x,pt->y);
   }
   ENDLINE;

   return;
}
