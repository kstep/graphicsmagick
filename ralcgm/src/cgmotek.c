/*   RAL-CGM Interpreter module:  @(#) cgmotek.c  version 3.7
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
 * Description:  Output driver for Tektronix 4200s and EMUTEK
 *
 * Author:  K E V Palmen

 * Arguments:     stream   FILE structure for output file (not used)
 *
 *                c        CGM element code (character form)
 *
 *                *pi      Pointer to Integer Array
 *
 *                *pr      Pointer to Real Array
 *
 *                *str     Pointer to string
 *
 * Maintenance Log:
 *  13 Jan 92 KEVP Started.
 *  30 Jan 92 KEVP Made able to do Polyline.
 *   6 Feb 92 KEVP Made able to do colour, with default colour table.
 *   7 Feb 92 KEVP Made able to do Polygon in HOLLOW, SOLID & PATTERN.
 *  19 Feb 92 KEVP Made able to do Cell Array.
 *  04 Mar 92 KEVP Made able to Clip Polyline.
 *  06 Mar 92 KEVP Made able to do Markers.
 *  09 Mar 92 KEVP Made able to Clip Markers.
 *  17 Mar 92 KEVP Made able to do Software Text (Hershey & Bezier).
 *  20 Mar 92 KEVP Made able to set colour table.
 *   1 Apr 92 KEVP Simulate HATCH fill with HOLLOW.
 *   2 Apr 92 KEVP Made able to do GDPs.
 *   8 Apr 92 KEVP Made able to do Polygon Edges.
 *  28 Apr 92 KEVP Made able to do PolygonSet.
 *  17 Aug 92 KEVP Made able to handle buffering of primitives.
 *   3 Sep 92 KEVP Renamed CLIPget, LINEclip and MARKer to PTS....
 *  23 Sep 92 KEVP Made TEKpoint move points out of range into range
 *                 instead of ignoring them.
 *  28 Sep 92 KEVP Set default clipping rectangle at VDCEXT entry-pt.
 *  29 Sep 92 KEVP In TEKtrans, get VDC extent by utility PTSvdcextget.
 *   7 Oct 92 KEVP Allowed CELLfill to update fist row for next chunk.
 *   9 Oct 92 KEVP Made TEKcdraw eliminate polygons easily found to be
 *                 outside the clipping rectangle.
 *   9 Oct 92 KEVP Clip to VDC extent, if clipping indicator is OFF.
 *  21 Oct 92 KEVP In TEKgdp declared number of rasters per VDC.
 *  21 Oct 92 KEVP For BEZIER text set fill-style solid BEFORE
 *                 setting the colour (for tek-panel colour setting ).
 *  22 Oct 92 KEVP Implemented Background Colour
 *  22 Oct 92 KEVP Used cur.max_colindex instead of COLTAB_N when calling
 *                 ATTribute utilities.
 *  30 Oct 92 RTP  Correct function prototype for TEKscolour().
 *                 Replace ANSI by PROTO.
 *                 Add FILE to CGMOtek call.
 *   4 Nov 92 RTP  Correct Colour matching utilities.
 *   5 Nov 92 KEVP Correct on-point calculation by TEKarc.
 *                 Correct evaluation of ARC..CLOSE closure flag.
 *                 Put quit option into TEKmenu.
 *                 Make background colour go black in TEKclose.
 *                 Free the allocations made for points 'pt'.
 *   6 Nov 92 KEVP Replace hardware PIE with ARC followed by centre
 *                 and first point to close. This was done because the
 *                 hardware PIE can not accurately place the centre
 *                 points of pies of small turning angle.
 *   6 Nov 92 KEVP Include prototype files for Bezier and Hershey Util's.
 *  10 Nov 92 RTP  Make internal functions static
 *  10 Nov 92 KEVP Put in option for long colour table.
 *  16 Nov 92 KEVP Improved points handling with new PTSlist utility.
 *  10 Dec 92 KEVP Put in option for normal size colourtable.
 *   6 Jan 93 KEVP Enabled to handle reversed VDC extent.
 *                 Set graininess in shared attributes for Bezier text.
 *  11 Jan 93 KEVP For CMS prevent put each coordinate on newline to
 *                 prevent space character occurring at end of line.
 *                 (Short term fix)
 *  15 Jan 93 RTP  Add assignment before CLIPCODE to prevent Iris compile
 *                 error
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *
 *  ml:end  (Editor search target for maintenance Log update)
 *
 *   LIMITATIONS requiring considerable effort to correct.
 *
 *   1. For IBM/CMS, works only on Waterloo C (linemode needed) <?time>.
 *   2. No correctly clippable Polygon or PolygonSet <1 month +>.
 *   3. No HATCHed Fill or scalable CGM defined PATTERNed fill <2-4 Wks>.
 *   4. No hardware text <1-2 weeks>.
 *
 */

/*  Include relevant parameters, structures and utilities */

#include "cgmout.h"    /* Structures and parameters */
#include "cgmtek.h"    /* Local variables and structures */
#include "cgmgraph.h"  /* Graphics Utilities */
#include "cgmpts.h"    /* Points Utilities */
#include "cgmcell.h"   /* Cell array utilities */
#include "cgmgdp.h"    /* GDP Utilities */
#include "cgmcolr.h"   /* CGM Colour Table utilies */


#ifdef PROTO       /* Standard C use function prototyping */
void CGMOtek( FILE *, Code, Long*, Float*, char* ),
     TEKoptions ( char * );

static void TEKinit( void ),
     TEKclose( void ),
     TEKnewpic( char *name ),
     TEKmenu( void ),
     TEKattrib( Code type ),
     TEKline( Long n, Point *pt),
     TEKmarker( Long n, Point *pt),
     TEKtext( Textitem *txt ),
     TEKcells( Long, Long* , Logical),
     TEKgdp( Code type, Long *pi, Float *pr, Enum close ),
     TEKarc( Point *cen, Double rad, Point *a, Point *b, Double *ang,
             Point *s, Point *o, Point *e),
     TEKfill( Long n, Point *pt, Enum status ),
     TEKfset( Long n, Point *pt, Enum *fl ),
     TEKedge( Long n, Point *pt, Enum *fl ),
     TEKcdraw( Int, Point*, Tmatrix, int ),
     TEKtrans( void ),
     TEKscolour ( Index, RGBcolour, Code ),
     TEKcmapupd( void ),
     TEKrsetcoltab(),
#ifdef CHARCHECK
     TEKtcharch( void ),
#endif
     TEKint( Int),
     TEKpoint( Int ix, Int iy ),
     TEKreal( Float);

static Int  TEKcindex ( RGBcolour col, Int *nsetcol, Int maxcol,
                 RGBcolour *coltab, int nred, int ngreen, int nblue );

static Logical TEKpanedge( Long n, Enum *fl );

#else
void CGMOtek(),     /* Main driver routine */
     TEKoptions();  /* Option handling routine (called by CGMMAIN) */

static void TEKinit(),      /* Initialise Graphics */
     TEKclose(),     /* Close down Graphics  */
     TEKnewpic(),    /* Start a new frame  */
     TEKmenu(),      /* Select Options */
     TEKattrib(),    /* Set Tek Attributes */
     TEKline(),      /* Draw a Polyline from a Clipped Points List */
     TEKmarker(),    /* Draw Polymarkers */
     TEKtext(),      /* Draw Text */
     TEKcells(),     /* Draw Cell Arrays */
     TEKgdp(),       /* Draw GDPs */
     TEKarc(),       /* Draw arc */
     TEKfill(),      /* Polygon Fill */
     TEKfset(),      /* Polygon Set Fill */
     TEKedge(),      /* Edge drawing */
     TEKcdraw(),     /* Draw part of text character or cell array */
     TEKtrans(),     /* Set the VDC-TSC coordinate transformation */
     TEKscolour(),   /* Set the TEK colour */
     TEKcmapupd(),   /* Update surface colour map */
     TEKrsetcoltab(),  /* Reset device colour table to default */
#ifdef CHARCHECK
     TEKtcharch(),   /* Chack TCHAR translates correctly */
#endif
     TEKint(),       /* Integer to TEK */
     TEKpoint(),     /* Point to TEK */
     TEKreal();      /* Real number to TEK */

static Int  TEKcindex();    /* Emulate direct colour or extended colour table */

static Logical TEKpanedge();   /* Polygon Set edges drawable in Panels */

#endif

/*   Normalisation of direct colour */

#define TEKRGB(r,k)   ( (Posint) (NCOLVAL-0.001) * ((r) - cur.min_rgb.k)\
                        / (cur.max_rgb.k - cur.min_rgb.k) )

/*   Debug messages  */

#define DMESS         (void) fprintf(stderr,

static char *func = "CGMOTEK", emess[48];
static FILE *cgmot;

/*  Text methods available */

static Enum Text_method[] = {
#ifdef BEZIER
         BEZ,
#endif
#ifdef HERSHEY
         HER,
#endif
         HDW,
         (Enum) 0 };

static Logical att_flag = TRUE;

/**************************************************** CGMOtek **********/
#ifdef PROTO
void CGMOtek ( FILE *stream, Code c, Long *pi, Float *pr, char *str )
#else
void CGMOtek ( stream, c, pi, pr, str )

/*  Output Driver for device TEK  */

FILE *stream;
Code c;
Long *pi;
Float *pr;
char *str;
#endif
{
   static Logical first = TRUE;
   register Long n, i, j, numcont, number;
   Code major;
   Enum close;             /* Closure code for GDPs etc */
   Point *pt = NULL;       /* Points List */
   Enum  *fl = NULL;       /* Flag list for Fill Area Set */
   Rect clip;              /* Clipping Rectangle */
   Textitem *tekst_item;   /* Text item */

#ifdef DEBUG
/*   stderr = fopen("debug","w"); */
   DMESS "\n  CGMtek code: 0x%x\n", c);
#endif

   if ( c == (Code) EOF ) exit(0);
   cgmot = stream;
   major = c>>8;
   numcont = *pi++;  /* Get number of data with continuation indicator */
   number = abs(numcont);  /* Get number of data by itself */

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         switch (c)
         {
            case LINE:         /*  Polyline   */
               if ( first ) TEKattrib(LINE);
               pt = PTSlist (number, pi, pr, NULL);
               PTSclipget (NULL, &clip);
               tl_final = (numcont >= ZERO);
               PTSlineclip(number, pt, clip, TEKline);
               first = ( numcont >= ZERO );
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first ) TEKattrib(LINE);
               pt = PTSlist (number, pi, pr, NULL);
               PTSclipget (NULL, &clip);
               tl_final = TRUE;
               for (i = 1; i <= number; i += 2)
               {
                  PTSlineclip (2, pt, clip, TEKline);
                  pt += 2;
               }
               first = ( numcont >= ZERO );
               break;

            case MARKER:         /*  Polymarker  */
               if ( first ) TEKattrib(MARKER);
               pt = PTSlist (number, pi, pr, NULL);
               TEKmarker ( number, pt);
               first = ( numcont >= ZERO );
               break;

            case TEXT:         /*  Text   */
               /* Get attributes and put into text item */
               TEKattrib(TEXT);
               tekst_item = TXTaccinf(numcont,str, &tektxt, preftxmeth);
               /* Get text point */
               txtpos = *PTSlist (1L, pi, pr, NULL);
               /* Set restricted text flag off */
               restrtxt = FALSE;
               /* If final, draw the text */
               if ( (Enum)numcont == FINAL ) TEKtext( tekst_item );
               break;

            case RESTRTEXT:         /*  Restricted Text */
               /* Get attributes and put into text item */
               TEKattrib(TEXT);
               tekst_item = TXTaccinf(numcont,str, &tektxt, preftxmeth);
               /* Get text dimensions (as relative opposite corner) */
               txtcorner = *PTSlist (1L, pi, pr, NULL);
               /* Get text point */
               txtpos = *PTSlist (1L, pi+2, pr+2, NULL);
               /* Set restricted text flag on */
               restrtxt = TRUE;
               /* If final, draw the text */
               if( (Enum)numcont == FINAL ) TEKtext( tekst_item );
               break;

            case APNDTEXT:         /*  Append Text   */
               /* Get attributes and put into text item */
               TEKattrib(TEXT);
               tekst_item = TXTaccinf(numcont,str, &tektxt, preftxmeth);
               /* If final, draw the text */
               if( (Enum)numcont == FINAL ) TEKtext( tekst_item );
               break;

            case POLYGON:         /*  Polygon   */
               if(first)
               {    /* Only 1st Chunk is drawn */
                  TEKattrib(POLYGON);
                  pt = PTSlist(number, pi, pr, NULL);
                  TEKfill ( number, pt, FINALPOLY );
                  if(edgeafter)
                  {
                    TEKattrib(EDGEVIS);
                    TEKedge(number, pt, NULL );
                  }
               }
               first = (numcont >= ZERO);
               break;

            case POLYGONSET:         /*  Polygon Set  */
               if(first)
               {   /* Only first chunk is drawn */
                  TEKattrib(POLYGON);
                  pt = PTSlist (number, pi, pr, &fl);
                  TEKfset ( number, pt, fl );
                  if(edgesetaft)
                  {
                    TEKattrib(EDGEVIS);
                    TEKedge(number, pt, fl );
                  }
               }
               first = (numcont >= ZERO);
               break;

            case CELLARRAY:         /*  Cell Array  */
               if ( first )
               {
                  TEKattrib (CELLARRAY);

/*  set up local Cell Array parameters here */

                  CELLparget( pi,pr, &cell.p,&cell.q,&cell.r,
                              &cell.nx,&cell.ny, &cell.lp);
                  pi += 9; /* Move to Colour data */
#ifdef DEBUG
    DMESS "\n Cell Array Parameters P=(%f,%f),Q=(%f,%f),R=(%f,%f)\n",
          cell.p.x,cell.p.y,cell.q.x,cell.q.y,cell.r.x,cell.r.y);
    DMESS " Cell Array Parameters %d by %d \n",cell.nx,cell.ny);
#endif
               }
               TEKcells ( number, pi, first );
               first = ( numcont >= ZERO );
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               j = *pi++;
               if ( cgmralgks )
               {
                  close = ( j == GKSPIE ? PIE
                                        : (j == GKSCHORD ? CHORD
                                                         : NOCLOSE ));
                  TEKgdp ( (Code) j, pi,pr, close);
               }
               else
               {
                  CGMerror(func, ERR_NOGDP, ERROR, NULLSTR);
               }
               break;

            case RECT:         /*  Rectangle   */
               TEKgdp (RECT, pi,pr, NOCLOSE);
               break;

            default:
               (void) sprintf( emess, "(code: 0x%x)", c);
               (void) CGMerror (func, ERR_INVPRIM, ERROR, emess);
               break;
         }
         break;

      case 0x30:  /*  Delimiter Elements  */

         switch (c)
         {

            case BEGMF:         /*  Begin Metafile   */
               TEKinit();
               /* Make sure shared attributes are being picked up */
               tekline.shared = &tekshare;
               tekmark.shared = &tekshare;
               tektxt.shared = &tekshare;
               tekfill.shared = &tekshare;
               tekedge.shared = &tekshare;

               break;

            case ENDMF:         /*  End Metafile   */
#ifdef CHARCHECK    /* Check characters */
               TEKtcharch();
#endif
               TEKclose();
               break;

            case BEGPIC:        /*  Begin Picture */
               TEKnewpic(str);
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               TEKtrans();
               TEKattrib(BEGPIC);
               break;

            case ENDPIC:         /*  End  Picture    */
               COLRfree();
               TEKmenu();
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVDELIM, ERROR, emess );
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
            case SCALEMODE:        /*  Scaling Mode   */
            case LINEWIDTHMODE:    /*  Line width Specification  */
            case MARKERSIZEMODE:   /*  Marker size Specification  */
            case EDGEWIDTHMODE:    /*  Edge width Specification  */
               break;

            case VDCEXT:           /*  VDC Extent    */
              /* Set default clipping rectangle */
               PTSdefclip();
               break;

            case BACKCOLR:         /*  Background Colour  */
               TEKcoluse[0]       = OUT_OF_DATE;
               TEKcoltab[0].red   = TEKRGB(cur.back.red  ,red);
               TEKcoltab[0].green = TEKRGB(cur.back.green,green);
               TEKcoltab[0].blue  = TEKRGB(cur.back.blue ,blue);
#ifdef DEBUG_CMAP
               DMESS "BACKCOLR: ( %d,%d,%d ) %d ",
                      TEKCOLTAB(0));
#endif
               cmap_change = TRUE;
               TEKcmapupd();   /* Update surface colour map */
               break;

            default:
               (void) sprintf (emess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, emess);
         }
         break;

      case 0x33:  /* Control Elements  */

         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision  */
            case VDCREALPREC:          /* VDC Real Precision  */
            case AUXCOLR:              /* Auxiliary Colour  */
            case TRANSPARENCY:         /* Transparency  */
            case CLIPRECT:             /* Clip Rectangle  */
            case CLIP:                 /* Clip Indicator  */
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, emess);
         }
         break;

      case 0x34:  /* Circles and Ellipses  */

         switch(c)
         {
            case CIRCLE:       /* Circle      */
               TEKgdp (CIRCLE, pi,pr, NOCLOSE);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               TEKgdp (ARC3PT, pi,pr, NOCLOSE);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               TEKgdp (ARC3PTCLOSE, pi,pr, (Enum) *(pi+6) );
               break;

            case ARCCTR:       /* Circle Arc centre */
               TEKgdp (ARCCTR, pi,pr, NOCLOSE);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               TEKgdp (ARCCTRCLOSE, pi,pr, (Enum) *(pi+7) );
               break;

            case ELLIPSE:       /* Ellipse    */
               TEKgdp (ELLIPSE, pi,pr, NOCLOSE);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               TEKgdp (ELLIPARC, pi,pr, NOCLOSE);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               TEKgdp (ELLIPARCCLOSE, pi,pr, (Enum) *(pi+10)+1 );
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVOUT, ERROR, emess );
         }
         break;

      case 0x35:  /* Attributes  */
         att_flag = TRUE;
         switch(c)
         {
            case CHARHEIGHT:       /*  Character Height   */
            case LINEWIDTH:       /*  Line Width */
            case MARKERSIZE:       /*  Marker Size */
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
            case ALTCHARSETINDEX:   /*  Alter Character Set Index */
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVATT, ERROR, emess );
               break;
         }
         break;

      case 0x36:  /* More Attributes  */
         att_flag = TRUE;
         switch(c)
         {
            case EDGEWIDTH:       /*  Edge Width */
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
               break;

            case PATSIZE:       /*  Pattern Size  */
               break;

            case COLRTABLE:       /*  Colour Table  */

#ifdef DEBUG_CMAP
            DMESS "\n COLRTAB: number %d, first index %d \n",
            number,*pi);
#endif
               COLRput (number,pi);
               i = *pi++;
               for(j=0;j<number && i<n_coltab;j++,i++)
               {
                 if(TEKcoluse[i] == UP_TO_DATE)
                    TEKcoluse[i]    = OUT_OF_DATE;
                 TEKcoltab[i].red   = TEKRGB(*pi++,red);
                 TEKcoltab[i].green = TEKRGB(*pi++,green);
                 TEKcoltab[i].blue  = TEKRGB(*pi++,blue);
#ifdef DEBUG_CMAP
                 DMESS "COLRTABLE: %d ( %d,%d,%d ) %d ",
                                    i,TEKCOLTAB(i));
#endif
               }
               cmap_change = TRUE;
               break;

            case ASF:       /*  Aspect source flags  */
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVATT, ERROR, emess );
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
               (void) sprintf ( emess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVESC, ERROR, emess);
               break;
         }
         break;

      default:
         (void) sprintf ( emess, "(Code: 0x%x)", c);
         (void) CGMerror ( func, ERR_INVELEM, ERROR, emess);
         break;
     }

     return;
  }

/************************************************** TEKoptions *********/

#ifdef PROTO
void TEKoptions( char *opts )
#else
/* Deals with options on the command line */
void TEKoptions(opts)
char *opts;     /* Option String */
#endif

{
   int i, len = strlen(opts);
   char *func="TEKoptions",*temp=" ";

   for ( i=0; i < len; i++ )
   {
      switch ( toupper( opts[i] ) )
      {
         case 'C':  /*  8 bitplanes  */
           n_coltab = 256;
           n_bitp = 8;           /* Surface has 8 bit-planes */
           n_collev = NCOLVAL;   /* Colour Levels in TEK RGB units */
           break;
         case 'N':  /*  normal bitplanes  */
           n_coltab = DEF_N_COLTAB;
           n_bitp = DEF_BITPLANES;  /* Surface has default bit-planes */
           n_collev = DEF_COLLEV;   /* Colour Levels in TEK RGB units */
           break;
         default:   /*  Unknown Option */
           *temp = opts[i];
           (void)CGMerror(func,ERR_INVOPT,ERROR,temp);
           break;
      }
   }
   return;
}

/***************************************************** TEKattrib *******/

#ifdef PROTO
static void TEKattrib ( Code type )
#else
static void TEKattrib (type)

/*  Converts CGM Attribute 'type' to local Attribute  and
    sets these attributes in the hardware */

Code type;
#endif

{
    static Logical last_prim = NONOP;  /* Last Primitive */
    int line_style;              /* Tek line style to be used */
    int tpat;                    /* Tek Pattern index */


#ifdef DEBUG
    DMESS  "\n TEKattrib: type 0x%x\n", type);
#endif

    /* If drawing, update surface colour map */
    if(type != BEGPIC) TEKcmapupd();

    switch (type)
    {
       case LINE:

          /* if no attribute has changed and
             last primitive was a line, return */
#ifdef DEBUG
         DMESS "\n TEKattrib: att_flag %d, last_prim 0x%x \n",
               att_flag, last_prim);
#endif
          if ( ! att_flag && last_prim == LINE ) return;

          /* Get local attributes */

          ATTline(NLINEB, line_bundle, cur.max_colind, &tekline);

#ifdef DEBUG
          DMESS "TEKattrib: Line attributes got \n");
#endif
          /* Set Hardware attributes */

          line_style = *(tlstyle+tekline.type-1);
          T_LINESTYLE(line_style);  /* TEK Line Style */

#ifdef DEBUG
          DMESS "\n TEKattrib: Linetype %d, TEK linestyle %d \n",
                tekline.type, line_style);
#endif

          TEKscolour(tekline.colindex, tekline.col, LINE );

          if(line_style > 0)
          {
             if(tekshare.transparency == OFF)
             {
                TEKscolour(tekshare.auxindex,tekshare.aux, AUXCOLR);
             }
             else
             {
                T_BACKINDEX(0);
             }
          }

          last_prim = LINE;
          break;


      case MARKER:

         /* if no attribute has changed and
            last primitive was a marker, return */
          if ( ! att_flag  && last_prim == MARKER ) return;

          ATTmarker(NMARKB, mark_bundle, cur.max_colind, &tekmark);

          /* Set Hardware attributes here  */

                        /* Enforce mimimum marker size, if scaled */
          if((tekmark.sizemode == SCALED) && (tekmark.size < MNMKSZ))
          tekmark.size = MNMKSZ;

          T_LINESTYLE(0);       /* TEK Line style SOLID */

          TEKscolour(tekmark.colindex, tekmark.col, MARKER );

          last_prim = MARKER;
          break;

      case TEXT:

          /* Get text attributes for Text Item */

          ATTtext(NTEXTB, text_bundle, cur.max_colind, &tektxt);

          /*  Hardware attributes are not set here.
          This because we don't yet know the text method
          and attributes are subject to variation in cases
          append text.
          They are set in TEKtext instead. */

          break;

      case POLYGON:   /*  Fill attributes  */

          /* if no attribute has changed and
            last primitive was a Fill Area, return */

          if ( ! att_flag && last_prim == POLYGON ) return;

          /* Get local attributes  */

          ATTfill(NFILLB, fill_bundle, cur.max_colind, &tekfill);

#ifdef DEBUG
          DMESS "\n TEKattrib: tekfill.style %d \n",tekfill.style);
#endif
          /* Set Hardware attributes  */

          switch(tekfill.style)
          {
             case SOLID:
                edgeincl = ( curatt.edge_vis == ON );
                TEKscolour( tekfill.colindex, tekfill.col, POLYGON);
                break;
             case HOLLOW: case HATCH:
                edgeincl = FALSE;
                T_LINESTYLE(0);
                TEKscolour( tekfill.colindex, tekfill.col, POLYGON);
                break;
             case PATTERN:  /* Set Pattern Index */
                edgeincl = ( curatt.edge_vis == ON );
                tpat = (tekfill.pattern & 0xFF) + 1;
                T_FILLPATTERN (tpat);
                break;
             case EMPTY:
                edgeincl = ( curatt.edge_vis == ON );
                break;
             default:
                break;
          }

          /* If edge is to be drawn by TEKFILL, take edge attributes */
          if(edgeincl)
          {
              /* Set Local Edge Attributes */
              ATTedge (NEDGEB, edge_bundle, cur.max_colind, &tekedge );

              /* and Hardware Attributes */
             line_style = *(tlstyle+tekedge.type-1);
             T_LINESTYLE(line_style);
             TEKscolour(tekedge.colindex, tekedge.col, EDGEVIS );
             if(line_style > 0)
                if(tekshare.transparency == OFF)
                   TEKscolour(tekshare.auxindex,tekshare.aux, AUXCOLR);
                else
                {  T_BACKINDEX(0); }

             edgeafter = FALSE;   /* Edge not to be drawn after TEKfill */
          }
          else
          {  /* Edge not included in TEKfill */
             edgeafter = (curatt.edge_vis == ON);
                            /* Edge to be drawn after TEKfill, if vis */
          }

          last_prim = POLYGON;
          break;

      case EDGEVIS:    /*  Edge attributes */

          /* if no attribute has changed and
            last primitive was a edge, return */

          if ( ! att_flag && last_prim == EDGEVIS ) return;

          /* Set local attributes */
          ATTedge(NEDGEB, edge_bundle, cur.max_colind, &tekedge );

          /* Set Hardware attributes here  */
          line_style = *(tlstyle+tekedge.type-1);
          T_LINESTYLE(line_style);
          TEKscolour(tekedge.colindex, tekedge.col, EDGEVIS );
          if(line_style > 0)
             if(tekshare.transparency == OFF)
                TEKscolour(tekshare.auxindex,tekshare.aux, AUXCOLR);
             else
             {  T_BACKINDEX(0); }

          last_prim = EDGEVIS;
          break;

      case CELLARRAY:  /*  Cell array - may affect texture & colour*/

          /* If last primitive was Cell Array, return. */
          if( last_prim == CELLARRAY ) return;

          /* Set Local attributes for cells */
          cdraw_mode = POLYGON;      /* TEKcdraw to fill */
          tekfill.style = SOLID;     /* Cells of solid fill */
          edgeincl = FALSE;          /* with no edges */

          last_prim = CELLARRAY;
          break;

      case BEGPIC:    /* Reset Picture attributes to default */
         last_prim = ZERO;
         att_flag = TRUE;
         return;

      default:
         (void) sprintf ( emess, "(Type: 0x%x)", type);
         (void) CGMerror ( func, ERR_INVATT, ERROR, emess );
         break;
   }

   att_flag = FALSE;

   return;
}

/***************************************************** TEKinit *********/

#ifdef PROTO
static void TEKinit( void )
#else
static void TEKinit()
#endif

/*  Initialise Variables and colour map */

{
   Int index;     /* Colour Index */

   T_DAENABLEON;  /* Enable Dialog Area */
   T_ALPHAMODE;   /* Enter ALPHA Mode */
   T_TEKCODE;     /* Select TEK Mode */
   T_IGNOREDEL;   /* Ignore Delete character */
   T_CBACKGROUND; /* Set Background Colour to Black */
   T_BACKINDEX(0);    /* Set Background Indices to 0 */
   T_BORDER;      /* Set View Border Invisible */
   T_CMODE;       /* Set Colour Mode to RGB Overpaint */
   T_EOLSTRING;   /* Set EOL String */
   T_EOMCHAR;     /* Set EOM Character */
   T_ERRORLEVEL;  /* Set Error Level to 2 */
   T_LINEINDEX(1);    /* Set Line Colour Index to 1 (White) */
   T_LINESTYLE(0);    /* Set Line Style to Solid */
   T_SDEFINITIONS(n_bitp);    /* Set One Surface with bit-planes */
   T_VIEWPORT (0,0,(int)D_WIDTH,(int)D_HEIGHT);
                      /* Set Viewport to Whole Display */
   T_WINDOW (0,0,(int)D_WIDTH,(int)D_HEIGHT);
                      /* Set Window to match Viewport */

#ifdef DEBUG_CMAP
       DMESS "\n");
#endif

   /* Initialise Colour Table */
   TEKrsetcoltab();
   return;
}

/***************************************************** TEKnewpic *******/

#ifdef PROTO
static void TEKnewpic( char *pic_name )
#else
static void TEKnewpic( pic_name )
char *pic_name;

/*   Begin new Picture */

#endif
{
   Index index;    /* Colour Index */

   T_RENEW;           /* Clear display */
   TEKrsetcoltab();   /* Reset Colour Table */
   return;
}

/***************************************************** TEKmenu *********/

#ifdef PROTO
static void TEKmenu( void )
#else
static void TEKmenu()
#endif

/*  End of frame processing to select options */

/*
    Currently, this promts for a character, then moves to next frame,
    unless the character is a 'Q', 'q' or it's the last frame,
    in which case it quits.
*/
{
   char c;

   NOW;      /* Update screen. Don't wait till output buffer is full */
   printf ("Type <RETURN> to go to Next Frame\n");
   printf ("or 'q' to quit now.\n");

   c = getchar();
   switch(c)
   {
      case 'Q':
      case 'q':  /* Quit Now */
         TEKclose();
         exit(0);
      default:
         break;
   }
   return;
}

/***************************************************** TEKclose ********/

#ifdef PROTO
static void TEKclose( void )
#else
static void TEKclose()
#endif

/*  Close down the Graphics  */

{
   T_CMAP(0,black);  /* Set background colour black */
   PTSfree();        /* Free the points store */
   T_DAVISIBILITY;   /* Make dialog area visible */
   T_ANSICODE;       /* Select ANSI code */
   NOW;    /* Update screen. Don't wait till output buffer is full */
   return;
}


/***************************************************** TEKline *********/

#ifdef PROTO
static void TEKline ( Long n, Point *pt )
#else
static void TEKline ( n, pt )

/*   draws n point polyline starting at pt. */
/*   clipping is assumed to have been done, if necessary */

Long n;      /* Number of points in polyline */
Point *pt;   /* The Points */
#endif
{

   int i;     /* Do loop index (0 to n-1)*/
   int ix;    /* X coord in Terminal Space Coords */
   int iy;    /* Y coord in Terminal Space Coords */

   if(tl_first)         /* If first chunk */
   {
      T_VECTORMODE;     /* Enter Vector Mode */
   }

   for (i = 0; i < n; i++,pt++)
   {
       ix = TSCX(pt->x);
       iy = TSCY(pt->y);
       TEKpoint (ix,iy);
   }

   if(tl_final)         /* If final chunk */
   {
      T_ALPHAMODE;      /* return to Alphamode */
   }

   tl_first = tl_final;  /* Next chunk is first, if and only if */
                         /* this chunk is final */

   return;
}

/***************************************************** TEKmarker *******/

#ifdef PROTO
static void TEKmarker ( Long nmk, Point *pt)
#else
static void TEKmarker ( nmk, pt)

/*   draws nmk markers starting at pt  */

Long nmk;
Point *pt;
#endif
{
   tl_final = TRUE;
   PTSmarker( nmk, pt, NMMKSZ, &tekmark, ON, TEKline);
   return;
}

/***************************************************** TEKtext *********/
#ifdef PROTO
static void TEKtext ( Textitem *txt )
#else
static void TEKtext ( txt )

/*  draws TEXT string 'txt->str' according to current settings */

Textitem *txt;
#endif
{
   Textitem * curtxt = txt;     /* Current Text Item */
   Logical unsetline = TRUE;    /* True if TEK linestyle is not set */

   if(restrtxt) curtxt = txt = TXTrestrict(curtxt,
                               txtcorner.x,txtcorner.y,&tektxt);
   if(curtxt != NULL)
   {
      TXTalign ( txt, txtpos, TXTgwidth(txt), TXTgheight(txt) );
      while ( curtxt != NULL )
      {
         switch( curtxt->text_method )
         {
#ifdef HERSHEY
            case HER:
               if(unsetline)   /* Make sure TEK linestyle is Solid */
               {
                  T_LINESTYLE(0);       /* Set TEK linestyle solid */
                  unsetline = FALSE;
               }
               TEKscolour( tektxt.colindex, tektxt.col, LINE );
               cdraw_mode = LINE;
               HERtext( curtxt, txtpos, TEKcdraw );
               break;
#endif
#ifdef BEZIER
            case BEZ:
               tekfill.style = SOLID;
               TEKscolour( tektxt.colindex, tektxt.col, POLYGON);
               cdraw_mode = POLYGON;
               edgeincl = FALSE;
               BEZtext( curtxt, &tektxt, txtpos, TEKcdraw );
               break;
#endif
#ifdef TEX
            case TEX:
              /* No routine yet */
               break;
#endif
            case HDW:
            default:
               /* Local text method */
               /* Currently, Hershey */
               if(unsetline)   /* Make sure TEK linestyle is Solid */
               {
                  T_LINESTYLE(0);       /* Set TEK linestyle solid */
                  unsetline = FALSE;
               }
               TEKscolour( tektxt.colindex, tektxt.col, LINE );
               cdraw_mode = LINE;
#ifdef HERSHEY
               HERtext( curtxt, txtpos, TEKcdraw );
#else
               (void) CGMerror ( func, ERR_TXTSYSNFND, WARNING, NULLSTR );
#endif
               break;
         }
         curtxt = curtxt->next;
      }
   }
   TXTfree(txt);   /* Free the space allocated to the text items */
   return;
}
/***************************************************** TEKcells ********/
#ifdef PROTO
static void TEKcells ( Long nc, Long *pi , Logical first)
#else
static void TEKcells ( nc, pi, first)

/*  Draws a Cell array of num values 'pi' */

Long nc;          /* Number of Cells to be Coloured */
Long *pi;         /* Cell Colour Data */
Logical first;    /* True, if first chunk */
#endif

{
   static Int iy;   /* Row to start Cell Array chunk */

   if(first)
   {           /* If first chunk, */
      CELLtran (cell.p, cell.q, cell.r, cell.nx, cell.ny, cell.tm);
      iy = 0;  /* evaluate transformation and start at 1st row. */
   }
   CELLfill(nc, pi, cell.tm, cell.nx, cell.ny, &iy,
           TEKscolour, TEKcdraw);
   return;
}
/***************************************************** TEKgdp **********/

#ifdef PROTO
static void TEKgdp ( Code type, Long *pi, Float *pr, Enum close )
#else
static void TEKgdp ( type, pi,pr, close )

/*  draws GDP 'type' with points starting at pi/pr
    or a similar CGM primitives */

Code type;
Long *pi; Float *pr;
Enum close;
#endif
{
   Long np;               /* Number of points in software primitive */
   Enum *fl = NULL;                   /* Unused POLYGONSET Edgeflags */
   Logical fill,                      /* Filled primitive */
           edge,                      /* Edges to be drawn */
           arc,                       /* Arc (not enclosed) */
           centreknown = FALSE;       /* Centre is known */
   Point *lpt = tekpt;                /* Pointer to local pts array */
   Point *pt;                         /* Array for input points */
   Point *start, *on, *end;           /* GKS GDP point indentities */
   Point *startvec, *endvec = NULL;   /* CGM vector identities */
   Point *centre = NULL;              /* Centre point indentity */
   Double rad, ang;                   /* Radius and Angle subtended */
   Double nrast;                      /* Num'r of rasters per VDC unit */
   Point *rectangle = NULL;           /* Rectangle */
   Rect clip;                         /* Clipping Rectangle */
   Logical software = FALSE, /* Software GDP (ie, LINE or POLYGON) */
           panel = FALSE;             /* TEK panel to be used */
   Int ix,iy, jx,jy, kx,ky, lx,ly;
                            /* Points in terminal space coordinates */

/* Temporary Define for Local Points */
#define LPT (Lpoint *)lpt


/*  work out if this is an arc  */

   arc = ( type == GKSARC || type == ARC3PT || type == ARCCTR ||
                                               type == ELLIPARC );

/*  decide whether to draw fill area, edge or both and set attributes  */

   fill = ( (!arc) && (curatt.int_style != EMPTY) );
   edge = (   arc  || (curatt.edge_vis == ON)     );

   if ( !arc ) TEKattrib ( POLYGON );
   else        TEKattrib ( LINE );

/* only bother, if area is visible */

   if ( ! (fill || edge) ) return;

/* set number of rasters per VDC unit */

   nrast = tekshare.xgrain;

#ifdef DEBUG
   DMESS "\n TEKgdp: Rasters per VDC unit %f \n",nrast);
#endif
/* get clipping rectangle */

   PTSclipget (NULL, &clip);

/* get the points */

   pt = PTSlist (6, pi, pr, NULL);

/*  If GKS GDP, convert it to a CGM Primitive */

   if  ( type == GKSCIRCLE || type == GKSARC ||
         type == GKSPIE    || type == GKSCHORD  )
       GDPcgm( (Index)type, pt, &type, &close );

#ifdef DEBUG
       DMESS "\n Type %x, close %x ",type,close);
       DMESS "p1 (%f,%f), p2(%f,%f)\n",pt->x,pt->y,(pt+1)->x,(pt+1)->y);
#endif

/* Name the Points */

   start = pt; on = pt+1; end = pt+2;        /* ARC3PT Points */
   centre = pt;                              /* Centre point */

/*  Now work out significant points  */

   switch ( type )
   {
      case ARCCTR: case ARCCTRCLOSE:
         startvec = pt+1; endvec = pt+2;  /* Name the vectors */
         *(pt+4) = *centre;     /* Move centre out of the way */
         centre = pt+4;
         rad = (double)(pt+3)->x;     /* get radius */
         TEKarc (centre, rad, startvec,endvec, &ang, start,on,end);
         centreknown = TRUE;
         endvec = NULL;  /* We can now proceed as a 3 point arc */

      case ARC3PT: case ARC3PTCLOSE:
         if ( ! centreknown )
         {
                centre = pt+4;
                GDPcentre ( centre, &rad, &ang, start,on,end);

#ifdef DEBUG1
                DMESS  "\n  Centre: (%f,%f)  rad: %f ang: %f\n",
                          centre->x, centre->y, rad, ang );
#endif
         }

         if ( REQUAL( ang, 0.0 ) )
         {
                     /*  Points are co-linear  */
            np = 3;
            if(close == PIE)
            {  /* Pie: Add centre */
              *(pt+3) = *centre;
              np++;
            }
            lpt = pt;  /* Use Input as local points */
            software = TRUE;
            break;
         }

         /* Determine whether clipping excludes the possiblity of
            using hardware curves */
         software = software ||
                    CLIPPING(start->x,clip.left,clip.right) ||
                    CLIPPING(start->y,clip.bot,clip.top)    ||
                    CLIPPING(on->x,   clip.left,clip.right) ||
                    CLIPPING(on->y,   clip.bot,clip.top)    ||
                    CLIPPING(end->x,  clip.left,clip.right) ||
                    CLIPPING(end->y,  clip.bot,clip.top);

         if ( software )
         {
                   /*  Work out points  */
            GDPcircle( centre,rad,ang, start, nrast, &np, LPT, close );
         }
         break;

      case CIRCLE:
         rad = (double)(pt+1)->x;
         end->x = centre->x + (float)rad;
         end->y = centre->y;
         on->x  = centre->x - (float)rad;
         on->y  = centre->y;
#ifdef DEBUG
 DMESS "\n CIRCLE: end(%f,%f), on(%f,%f)\n",end->x,end->y,on->x,on->y);
#endif
         if( CLIPPING(end->x,clip.left,clip.right) ||
             CLIPPING(end->y,clip.bot,clip.top)    ||
             CLIPPING(on->x, clip.left,clip.right) ||
             CLIPPING(on->y, clip.bot,clip.top) )
         {  /* Try different points */
            end->x = centre->x;
            end->y = centre->y + (float)rad;
            on->x  = centre->x;
            on->y  = centre->y - (float)rad;
#ifdef DEBUG
 DMESS "\n CIRCLE: end(%f,%f), on(%f,%f)\n",end->x,end->y,on->x,on->y);
#endif
            if( CLIPPING(end->x,clip.left,clip.right) ||
                CLIPPING(end->y,clip.bot,clip.top)    ||
                CLIPPING(on->x, clip.left,clip.right) ||
                CLIPPING(on->y, clip.bot,clip.top) )
            {
              GDPcircle ( centre,rad,2*PI, end,
                          nrast, &np, LPT, close );
              software = TRUE;
            }
            else
            {
               start = end;  /* Make start = end for TEK CURVE */
            }
         }
         else
         {
            start = end;     /* Make start = end for TEK CURVE */
         }
         break;


      case RECT:
         rectangle = lpt;
         rectangle->x = pt->x;
         rectangle->y = pt->y;
         rectangle++;
         rectangle->x = (pt+1)->x;
         rectangle->y = pt->y;
         rectangle++;
         rectangle->x = (pt+1)->x;
         rectangle->y = (pt+1)->y;
         rectangle++;
         rectangle->x = pt->x;
         rectangle->y = (pt+1)->y;
         rectangle++;
         if(fill)  np = 4;
         if(edge){ np = 5; *rectangle = *lpt;}
         software = TRUE;
         break;

      case ELLIPSE:
         (pt+3)->x = 1.0;  (pt+3)->y = 0.0;
         GDPellipse(centre, pt+1, pt+2, pt+3, pt+3,
                    nrast, &np, LPT, close);
         software = TRUE;
         break;

      case ELLIPARC: case ELLIPARCCLOSE:
         if ( VEQUAL( (pt+1)->x*(pt+2)->y - (pt+2)->x*(pt+1)->y, 0.0 ) )
         {
                     /*  CDP Points are co-linear  */
            np = 2;
            lpt = pt;  /* (pt,pt+1,pt+2) are local pts */
            software = TRUE;
            break;
         }
         GDPellipse(centre, pt+1, pt+2, pt+3, pt+4,
                    nrast, &np, LPT, close);
         software = TRUE;
         break;

      case LINE:  /* GKS GDP collinear */
         np = 3;
         lpt = pt;  /* Use input as local points */
         software = TRUE;
         break;

      default:
         return;
   }

/* now draw primitives  */

   if(software)
   {   /* primitive drawn by LINE or POLYGON */
#ifdef DEBUG
       DMESS "\n");
       DMESS "SWcurve %d (%f,%f),(%f,%f),...\n",np,(lpt)->x,(lpt)->y,
              (lpt+1)->x,(lpt+1)->y);
#endif
       if(np > 250) np = 250;  /* Stop bug in np */

       if(!arc && (np > 2))
       {
          TEKfill(np,lpt,FINALPOLY);
          if(edgeafter)
          {
             TEKattrib(EDGEVIS);
             TEKedge(np,lpt,NULL);
          }
       }
       else
       {
          tl_final = TRUE;
          PTSlineclip(np, lpt, clip, TEKline);
       }
   }
   else
   {   /* primitive drawn by TEK CURVE */
#ifdef DEBUG
        DMESS "\n HWcurve (%f,%f),(%f,%f),(%f,%f)\n",start->x,start->y,
               on->x,on->y,end->x,end->y);
#endif
       panel = fill && ( (curatt.int_style == SOLID) ||
                         (curatt.int_style == PATTERN) );

       ix = TSCX(start->x);  /* Convert to TSC */
       iy = TSCX(start->y);  /* Convert to TSC */

       if(panel) { T_BEGINPANEL(ix,iy,(Int)edgeincl); }
       else { T_MOVE(ix,iy); }

       jx = TSCX(on->x);   /* Convert to TSC */
       jy = TSCX(on->y);   /* Convert to TSC */
       kx = TSCX(end->x);  /* Convert to TSC */
       ky = TSCX(end->y);  /* Convert to TSC */

       switch(close)
       {
          case NOCLOSE:
             T_ARC(jx,jy,kx,ky);    /* Draw Arc.*/
             break;
          case CHORD:
             T_CHORD(jx,jy,kx,ky);  /* Draw Chord. */
             break;
          case PIE:
             T_ARC(jx,jy,kx,ky);    /* Draw Arc, */
             lx = TSCX(centre->x);  /* Convert centre to TSC, */
             ly = TSCY(centre->y);
             T_DRAW (lx,ly);        /* Draw to Centre, */
             T_DRAW (ix,iy);        /* and Close */
             break;
          default:
             break;
       }
       if(panel){ T_ENDPANEL; }

       if(edgeafter)
       {   /* Edge needs drawing after */

          TEKattrib(EDGEVIS);
          T_MOVE (ix,iy);
          switch (close)
          {
             case NOCLOSE:
                T_ARC(jx,jy,kx,ky);      /* Draw Arc. */
                break;
             case CHORD:
                T_CHORD(jx,jy,kx,ky);     /* Draw Chord. */
                break;
             case PIE:
                T_ARC(jx,jy,kx,ky);       /* Draw Arc, */
                T_DRAW (lx,ly);           /* Draw to Centre, */
                T_DRAW (ix,iy);           /* and Close */
                break;
             default:
                break;
          }
       }
   }
   return;
}
#undef LPT
/****************************************************** TEKarc *********/

#ifdef PROTO
static void TEKarc ( Point *centre, Double radius, Point *a, Point *b,
              Double *ang, Point *s, Point *o,  Point *e)
#else
static void TEKarc ( centre, radius, a, b, ang, s, o, e)

/*
    Given the centre, radius and
    start vector (a->x, a->y) and end vector (b->x, b->y)
    returns start and end points of a circular arc
    (s->x,s->y) & (e->x,e->y), also a point (o->x,o->y) on the arc
    and angle (ang) subtended
*/

Point *centre;
Double radius;
Point *a, *b;              /* Start and End Vectors */
Double *ang;               /* Angle subtended in Radians */
Point *s, *o, *e;          /* Start end and on points */
#endif

/* This routine is based on the GDParc utility written by RTP */

{
   register float ang1, ang2, ang3;  /* Angles of vectors */

#ifdef DEBUG
   DMESS "\n TEKarc: centre: (%f,%f) startvec (%f,%f) endvec (%f,%f)\n",
                         centre->x, centre->y,  a->x, a->y, b->x, b->y);
#endif

   ang1 = atan2( a->y, a->x );   /* Angle of start vector */
   ang2 = atan2( b->y, b->x );   /* Angle of end vector */
   *ang = ang2 - ang1;           /* Angle subtended */
   ang3 = 0.5*(ang1+ang2);       /* Angle of on vector */
   if ( *ang < 0 )
   {  /* Negative turning angle from crossing atan2 range limit */
      *ang += 2*PI;   /* Make turnung angle positive */
      ang3 += PI;     /* Correct the on angle */
   }

#ifdef DEBUG
   DMESS "\n TEKarc: startang %f onang %f endang %f, radians",
                 ang1, ang3, ang2);
   DMESS "\n TEKarc: turnang %f radians %f degrees \n",
            *ang, *ang * 180.0/PI);
#endif

/*  Work out start, on & end  points of arc */

   s->x = radius * cos( ang1 ) + centre->x;
   s->y = radius * sin( ang1 ) + centre->y;
   e->x = radius * cos( ang2 ) + centre->x;
   e->y = radius * sin( ang2 ) + centre->y;
   o->x = radius * cos( ang3 ) + centre->x;
   o->y = radius * sin( ang3 ) + centre->y;

#ifdef DEBUG
   DMESS  "\n  Arc between: (%f,%f) and (%f, %f)\n",
                           s->x, s->y, e->x, e->y);
   DMESS  "  with (%f,%f) on it.\n",o->x,o->y);
#endif

   return;
}
/***************************************************** TEKfill *********/

#ifdef PROTO
static void TEKfill ( Long num, Point *pt, Enum status )
#else
static void TEKfill ( num, pt, status )

/*  Fills a Polygon */

Long num;    /* Number of Vertices */
Point *pt;   /* Coords of Vertices */
Enum status; /* Status of polygon: final or followed by another */
#endif

{
   Int edgeshow;  /* Edge to be Shown in Panel */
   Int ix, iy;  /* First point in Terminal Space Coords */

   ix = TSCX(pt->x);  /* Convert First Point to TSC */
   iy = TSCY(pt->y);

   tl_final = TRUE;  /* Set final chunk flag to true */

   switch (tekfill.style)
   {
      case HOLLOW:
         TEKline ( num, pt);                 /* Draw as Polyline */
         T_DRAW(ix,iy);                      /* Close Polygon */
         break;

      case SOLID:
      case PATTERN:                      /* SOLID or PATTERNED FILL */
         edgeshow = (Int)edgeincl;
         T_BEGINPANEL(ix,iy,edgeshow);   /* Begin TEK Panel */
         TEKline ( num-1, ++pt);         /* Send remaining vertices */
         if(status == FINALPOLY)
         {                               /* Final polygon in set, */
            T_ENDPANEL;                  /* then End TEK Panel */
         }
         break;

      case HATCH:                            /* HATCHED FILL */
         TEKline( num, pt);    /* Not yet supported: Draw as HOLLOW */
         T_DRAW(ix,iy);                      /* Close Polygon */
         break;

      case EMPTY:                            /* EMPTY FILL */
         if(edgeincl)                        /* Edge only */
         {    /* Edge is included */
            TEKline ( num, pt);              /* Draw as Polyline */
            T_DRAW(ix,iy);                   /* Close Polygon */
         }
         break;
   }
   return;
}
/***************************************************** TEKfset *********/

#ifdef PROTO
static void TEKfset ( Long num, Point *pt, Enum *fl )
#else
static void TEKfset ( num, pt, fl )

/*  Fills a Polygon Set */

Long num;      /* Number of Vertices */
Point *pt;     /* Coords of Vertices */
Enum  *fl;     /* Polygon Set Edge-Out Flags */
#endif

{
   Enum *curfl;              /* Current edge-out flag */
   Logical eisave;           /* Edge inclusion save */
   Logical somevis=FALSE;    /* Some edge visible */
   Long nv;                  /* Number of vertices in polygon */
   Long i;                   /* Loop variable, not used inside loop */

   /* Determine how edges are to be drawn for this POLYGONSET */

   eisave = edgeincl;   /* Save edge inclusion for future POLYGON(SET) */

   if(curatt.edge_vis == ON)
   {
   /* Check for case, where no edge-out flag is VIS or CLOSEVIS */
      somevis = FALSE;
      for(i=0,curfl=fl; i<num; i++,curfl++)
      {
         if( (*curfl == VIS) || (*curfl == CLOSEVIS) )
         {
            somevis = TRUE;   /* Visible edge found */
            break;
         }
      }

      if(somevis)
      {
         if( edgeincl && TEKpanedge( num, fl ))
         {  /* Edges can be drawn by TEKfill */
            edgesetaft = edgeafter;
         }
         else
         {  /* Edges must be drawn afterwards, if previously included */
            edgesetaft = edgeincl || edgeafter;
            edgeincl = FALSE;
         }
      }
      else
      {
         edgeincl = FALSE;    /* No edges for THIS polygonset */
         edgesetaft = FALSE;
      }
   }                           /* End of if(curatt(edge.. */

   if(edgeincl)
   {   /* Edges to be included if polygon's edge-out flags visible */
      for(i=0,nv=1; i<num-1; i++,pt++,fl++)
      {
         if((*fl == CLOSEVIS) || (*fl == CLOSEINVIS))
         {  /* End of polygon */
            edgeincl = (*fl == CLOSEVIS);
            TEKfill( nv, pt-nv+1, FOLLOWED );
            nv = 1;
         }
         else nv++;  /* Add one to number of vertices in polygon */
      }
      edgeincl = (*fl == CLOSEVIS) || (*fl == VIS);
      TEKfill ( nv, pt-nv+1, FINALPOLY );
   }
   else
   {  /* No edges to be drawn here */
      for(i=0,nv=1; i<num-1; i++,pt++,fl++)
      {
         if((*fl == CLOSEVIS) || (*fl == CLOSEINVIS))
         {  /* End of polygon */
            TEKfill( nv, pt-nv+1, FOLLOWED );
            nv = 1;
         }
         else nv++;  /* Add one to number of vertices in polygon */
      }
      TEKfill ( nv, pt-nv+1, FINALPOLY );
   }

   edgeincl = eisave;  /* Restore original edge inclusion flag */

   return;
}
/***************************************************** TEKpanedge *******/

#ifdef PROTO
static Logical TEKpanedge ( Long num, Enum *fl )
#else
static Logical TEKpanedge ( num, fl )

/*  Returns True if the edges for a polygon set can be drawn
    in TEK panels. */

Long num;      /* Number of Vertices */
Enum  *fl;     /* Polygon Set Flags */
#endif

/* ALGORITHM

If final edge is visible,
   Assumes true, till
   a polygon is found that has edges both visible and invisible.
Else
   Assumes false.
*/
{
   Int i;                    /* Loop index not used inside loop */
   Enum *final;              /* Final Flag */
   Enum *next;               /* Next Flag */
   Logical endpoly = FALSE;  /* True, if at end of polygon */

   if(fl == NULL) return TRUE;  /* True, if not PolygonSet */

   final = fl+num-1;   /* False, if final edge is invisible */
   if((*final == CLOSEINVIS) || (*final == INVIS)) return FALSE;

   for(i=0,next=fl+1; i<num-1; i++,fl++,next++)
   {
       if(*fl != *next)
       {  /* Neighbouring flags differ */
           if(*fl == VIS || *fl == INVIS)
           {   /* Inside Polygon */
               if((*fl)+EDGECLOSE != *next)
               {  /* Differing edge visiblity at closure */
                  return FALSE;   /* Found unsuitable polygon */
               }
           }                      /* End of if(inside polygon) */
       }                          /* End of if( *fl != *next) */
   }                              /* End of for loop */

   return TRUE;  /* No polygon found with differing edge visibility */
}
/***************************************************** TEKedge *********/

#ifdef PROTO
static void TEKedge ( Long np, Point *pt, Enum *fl )
#else
static void TEKedge ( np, pt, fl )

/*  Draws edge for polygon of np points in pt[]  */

Long np;         /* Number of vertices */
Point *pt;       /* Vertices */
Enum *fl;        /* Edge-out flags */
#endif

{
   Int ix,iy, jx,jy;  /* Points in TSC */
   Int i;             /* Loop index, not used within loop */
   Long nvis;         /* Number of vertices in a visible edge section */

   ix = TSCX(pt->x);           /* Convert First Point to TSC */
   iy = TSCY(pt->y);

   tl_final = TRUE;  /* Set first chunk flag to true */

   if( fl == NULL)
   {   /* No edge flags (not polygon set) */
         TEKline ( np, pt);                  /* Draw as Polyline */
         T_DRAW(ix,iy);                      /* Close Polygon */
   }
   else
   {   /* Polygon set */
      for(i=0,nvis=1; i<np-1; i++,pt++,fl++)
      {
         switch(*fl)
         {
            case VIS:
               nvis++;        /* Add vertex to visible section */
               break;
            case INVIS:
               if(nvis > 1)
               {
                 TEKline( nvis, pt-nvis+1);  /* Draw visible section */
                 nvis = 1;
               }
               break;
            case CLOSEVIS:
               if(nvis > 1)
               {
                 TEKline( nvis, pt-nvis+1);  /* Draw visible section */
                 nvis = 1;
               }
               else
               {
                 jx = TSCX(pt->x);  /* Convert final pt to TSC */
                 jy = TSCY(pt->y);
                 T_MOVE(jx,jy);     /* Move to final point */
               }
               T_DRAW(ix,iy);       /* Close polygon */
               ix = TSCX((pt+1)->x);   /* Convert vertex to TSC */
               iy = TSCY((pt+1)->y);
               break;
            case CLOSEINVIS:
               if(nvis > 1)
               {
                 TEKline( nvis, pt-nvis+1);  /* Draw visible section */
                 nvis = 1;
               }
               ix = TSCX((pt+1)->x);       /* Convert vertex to TSC */
               iy = TSCY((pt+1)->y);
               break;
         }
      }

      switch (*fl)
      {
         case VIS: case CLOSEVIS:
            if(nvis > 1) TEKline( nvis, pt-nvis+1 );
            else
            {
               jx = TSCX(pt->x);   /* Convert last vertex to TSC */
               jy = TSCX(pt->y);
               T_MOVE(jx,jy);      /* Move to last vertex */
            }
            T_DRAW(ix,iy);
            break;
         case INVIS: case CLOSEINVIS:
            if(nvis > 1) TEKline( nvis, pt-nvis+1 );
            break;
       }
   }
   return;
}
/***************************************************** TEKcdraw *******/

#ifdef PROTO
static void TEKcdraw ( Int n, Point *pt, Tmatrix tm, int new)
#else
static void TEKcdraw ( n, pt, tm, new)

/*   draws n point polyline or polygon starting at pt */
/*   as part of a text character or cell array. */

Int n;       /* Number of points in polyline or polygon */
Point *pt;   /* The Points */
Tmatrix tm;  /* The transformation matrix */
int new;     /* New character/cell array flag */
#endif
/*
cdraw_mode determines whether a polyline or filled polygon is drawn.
*/
{
   Int i;               /* Loop variable, not used inside the loop */
   static Rect clip;    /* Clipping Rectangle */
   Long num;            /* Number of points as a long integer */
   Logical clipneed;    /* True, if it may need clipping */
   Point *curpt;        /* Current Point */
   Point tpt;           /* Temporary place for point */
   Enum cl1;            /* Clipcode for first point */
   Logical eliminated;  /* True if, polygon has been eliminated */

#ifdef DEBUG_TM
  DMESS "\n TEKcdraw:  Cell Array Transformation \n");
  DMESS "%f %f %f \n", tm[0][0],tm[1][0],tm[2][0]);
  DMESS "%f %f %f \n", tm[0][1],tm[1][1],tm[2][1]);
#endif
   /* Set number of points as long */
   num = (Long)n;
   /* Get clipping rectangle, if new character */
   if(new) PTSclipget( NULL, &clip);

   /* Assume no clipping needed, till
    a point outside the clipping rectangle is found */
   clipneed = FALSE;

   /* Transform Points and possibly eliminate from clipping */
   for(i=0,curpt=pt; i<n; i++,curpt++)
   {
     tpt.x = (float)( (double)curpt->x * tm[0][0] +
                      (double)curpt->y * tm[1][0] +
                                         tm[2][0] );
     tpt.y = (float)( (double)curpt->x * tm[0][1] +
                      (double)curpt->y * tm[1][1] +
                                         tm[2][1] );
     *curpt = tpt;
     if(!clipneed)
        clipneed = (CLIPPING(tpt.x,clip.left,clip.right) ||
                    CLIPPING(tpt.y,clip.bot,clip.top) );
   } /* End of loop over points */


   if(clipneed)
   {  /* Clipping, first eliminate, if easily found to be outside */

       /* Check X-wise outsidedness */
      tpt = *pt;
      cl1 = CLIPCODE(tpt.x,clip.left,clip.right);
      eliminated = TRUE; /* Assume X-wise outside, till disproven */
      for (i=1,curpt=pt+1; i<num; i++, curpt++)
      {
         if(cl1 != 0)
         {
            tpt = *curpt;
            if(cl1 != CLIPCODE(tpt.x,clip.left,clip.right))
            {
               eliminated = FALSE;   /* Not X-wise outside */
               break;
            }
         }
         else
         {
            eliminated = FALSE;      /* Not X-wise outside */
            break;
         }
      }
      if(eliminated) return;  /* X-wise outside, therefore outside */

      /* Check Y-wise outsidedness */
      tpt = *pt;
      cl1 = CLIPCODE(tpt.y,clip.bot,clip.top);
      eliminated = TRUE;  /* Assume Y-wise outside, till disproven */
      for (i=1,curpt=pt+1; i<num; i++, curpt++)
      {
         if(cl1 != 0)
         {
            tpt = *curpt;
            if(cl1 != CLIPCODE(tpt.y,clip.bot,clip.top))
            {
               eliminated = FALSE;    /* Not Y-wise outside */
               break;
            }
         }
         else
         {
            eliminated = FALSE;      /* Not Y-wise outside */
            break;
         }
      }
      if(eliminated) return; /* Y-wise outside, therefore outside */

      /* The polygon may still be outside even though it is
         neither X-wise outside nor Y-wise outside,
         but further checking is more complicated and so is left to
         the clipping utility.
      */
   }

   /* Draw according the drawing mode and clipping indicator */
   switch(cdraw_mode)
   {
      case LINE:   /* Polyline */
         tl_final = TRUE;
         if( clipneed )
         {    /* Clipping to be done */
            PTSlineclip(num, pt, clip, TEKline);
         }
         else
         {    /* No clipping */
            TEKline (num, pt);
         }
         break;
     case POLYGON:  /* Filled Polygon */
        if( clipneed )
        {
           /* Clipping not implemented yet */
           TEKfill (num, pt, FINALPOLY);
        }
        else
        {    /* No clipping */
           TEKfill (num, pt, FINALPOLY);
        }
         break;
   }
   return;
}
/***************************************************** TEKtrans ********/

#ifdef PROTO
static void TEKtrans ( void)
#else
static void TEKtrans ()

/* Sets variables for transformation from VDC to Terminal Space Coords */
/* These variables are used in the macros TSCX and TSCY */

#endif

{
   Rect vdcext;     /* VDC extent */
   float vdx,vdy;   /* Display Space to VDC extent Ratios */

   PTSvdcextget( &vdcext );
   xvdca = vdcext.left;
   yvdca = vdcext.bot;
   vdx = FABS(D_WIDTH/(vdcext.right - xvdca));
   vdy = FABS(D_HEIGHT/(vdcext.top  - yvdca));

#ifdef DEBUG
       DMESS "\n vdx %f, vdy %f \n",vdx,vdy);
#endif

   /* Set VDC unit to Minimum of the X and Y ratios (vdx & vdy).     */
   /* This makes the VDC extent, become the largest rectangle of the */
   /* its aspect ratio in the display area (at lower left corner).   */
   if(vdx > vdy)
   {
       vdc_unit = vdy;  /* VDC extent fills height of display */
   }
   else
   {
       vdc_unit = vdx;  /* VDC extent fills width of display */
   }

   /* VDC unit vectors need to be set. These have the same absolute
      value as vdc_unit, but will be negative, if the VDC extent
      is reversed in the respective X or Y.
   */
   if(vdcext.left < vdcext.right) vdcux =  vdc_unit;
   else                           vdcux = -vdc_unit;
   if(vdcext.bot < vdcext.top)    vdcuy =  vdc_unit;
   else                           vdcuy = -vdc_unit;

   /* Set the resolution in the shared attributes */
   tekshare.xgrain = tekshare.ygrain = TSCRAST*vdc_unit;

   return;
}

/********************************************************* TEKscolour **/

#ifdef PROTO
static void TEKscolour( Index index, RGBcolour dircol, Code type)
#else
static void TEKscolour( index, dircol, type )
 /*
    Set a colour index or colour table entry according
    to the current colour mode.
 */
Index     index;    /* Colour Index */
RGBcolour dircol;   /* Direct colour (in CGM units) */
Code     type;      /* Type of primitive */
#endif

{
   Int nsetcol;          /* Local copy of number of set colours */
   Int tindex;           /* Colour Index for TEK colour table */
   RGBcolour *cgmcol;    /* RGB Value from CGM colour table */
   RGBcolour col;        /* RGB Value normalised 0 to 100 inclusive */

#ifdef DEBUG_CMAP
   DMESS "\n TEKscolour: index %d rgb (%d,%d,%d) type %4x\n",index,
              dircol.red,dircol.green,dircol.blue, type );
#endif

   if(cur.color_mode == DIRECT)
   {  /* Direct colour: Select or generate appropriate colour index */
      nsetcol = colset_n;
      col.red   = TEKRGB(dircol.red  ,red);
      col.green = TEKRGB(dircol.green,green);
      col.blue  = TEKRGB(dircol.blue ,blue);
      tindex = TEKcindex( col, &colset_n, n_coltab, TEKcoltab,
                         n_collev,n_collev,n_collev);
      if(colset_n == nsetcol+1)
      {  /* New colour added to colour table: put into hardware. */
         T_CMAP(tindex,col);
         TEKcoluse[tindex] = UP_TO_DATE;
      }
   }
   else
   {  /* Indexed colour */
     if(index > n_coltab)
     {  /* Colour Index too high to use: find index of nearest colour. */
        cgmcol = COLRtab(index);

#ifdef DEBUG_CMAP
       DMESS "TEKscolour: COLRtab(index) = %d\n",cgmcol);
#endif


        if(cgmcol != NULL)
        { /* Colour of required index found in CGM colour table */
           col.red   = TEKRGB(cgmcol->red,  red);
           col.green = TEKRGB(cgmcol->green,green);
           col.blue  = TEKRGB(cgmcol->blue,blue);
#ifdef DEBUG_CMAP
           DMESS "\n TEKscolour: index %d rgb (%d,%d,%d)\n",
                     index,col.red,col.green,col.blue);
#endif
           nsetcol = n_coltab;
           tindex = TEKcindex( col, &nsetcol, n_coltab, TEKcoltab,
                               n_collev,n_collev,n_collev);
        }
        else
        { /* Colour index out of device range and unset, use index 1 */
           tindex = 1;
        }
     }
     else
     {  /* Index in range: use it. */
        tindex = index;

#ifdef DEBUG_CMAP
        DMESS "\n TEKscolour: index %d in range \n",index);
#endif
     }
   }

   /* Update colour map, if a new index is being used */
   if(TEKcoluse[tindex] == NOT_USED)
   {
      col = TEKcoltab[tindex];
      T_CMAP(tindex,col);
      TEKcoluse[tindex] = UP_TO_DATE;
   }

   /* for either colormode */
   switch (type)
   {
     case LINE: case MARKER: case EDGEVIS:
        T_LINEINDEX(tindex);      /* Set TEK line colour index */
        break;

     case POLYGON:
        switch (tekfill.style)
        {
           case HOLLOW: case HATCH:
              T_LINESTYLE(0);      /* Set TEK line attributes for */
              T_LINEINDEX(tindex);   /* emulation of hollow fill  */
              break;

           case SOLID:
              T_FILLPATTERN(-tindex);  /* Set TEK pattern to a solid */
              break;                   /* colour (-colour index)     */
        }
        break;

     case CELLARRAY:
        T_FILLPATTERN(-tindex);  /* Set TEK pattern for Cell Array */
        break;

     case AUXCOLR:
        T_BACKINDEX(tindex);     /* Set TEK Background indices */
        break;
   }
   return;
 }

/********************************************************* TEKcindex ***/

#ifdef PROTO
static Int TEKcindex( RGBcolour col,
               Int *nsetcol, Int maxcol, RGBcolour *coltab,
               int nred, int ngreen, int nblue)
#else
static Int TEKcindex( col, nsetcol,maxcol,coltab, nred,ngreen,nblue )
 /*
    Emulate Direct colour or an out of range colour index
    for a colour table, returning a suitable colour index.
    (Should be superceded by a Utility)
 */
RGBcolour col;       /* Direct colour (normalised 0 to 100 inclusive) */
Int   *nsetcol;      /* Number of colours in colour table already set */
Int   maxcol;        /* Maximum number colours in colour table */
RGBcolour *coltab;   /* The colour table (normalised 0 to 100 incl) */
int   nred;          /* Number of red levels to be resolved */
int   ngreen;        /* Number of green levels */
int   nblue;         /* Number of blue levels */
#endif

{
  RGBcolour dirlev;         /* RGB levels resolved from direct colour */
  RGBcolour *curplace;      /* Current place in colour table */
  RGBcolour curcol;         /* Current colour */
  int   index;              /* Current colour index (counting from 0) */
  long  mindist;            /* Distance from nearest colour */
  long  curdist;            /* Distance from current colour */
  int   nearest = -1;       /* Index of nearest colour */

#ifdef DEBUG_CMAP
  DMESS "\nTEKcindex: col (%d,%d,%d) \n",
             col.red,col.green,col.blue);
#endif

  /* Find Matching Colour and return its index, if found */
  dirlev.red   = LEVCOL(col.red,  nred);
  dirlev.green = LEVCOL(col.green, ngreen);
  dirlev.blue  = LEVCOL(col.blue, nblue);
  curplace = coltab;

#ifdef DEBUG_CMAP
  DMESS "TEKcindex: dirlev <%d,%d,%d> \n",
           dirlev.red,dirlev.green,dirlev.blue);
#endif

  for (index=0;index<*nsetcol;index++)
  {
     curcol = *curplace;
     if((dirlev.red   == LEVCOL(curcol.red,nred)) &&
        (dirlev.green == LEVCOL(curcol.green,ngreen)) &&
        (dirlev.blue  == LEVCOL(curcol.blue,nblue)))
     {
#ifdef DEBUG_CMAP
        DMESS "TEKcindex: colour index %d matched.\n",index);
#endif

        return index;     /* match found: return index. */
     }
     else
     {
        curplace++;  /* Increment to next colour table entry */
     }
  }

  /* Here, no matching colour is found */
  if(*nsetcol < maxcol)
  {    /* Vacancy exists in colour table, fill it and return its index */
     *curplace = col;
#ifdef DEBUG_CMAP
     DMESS  "TEKcindex: index %d set to ( %d,%d,%d ) returned \n",
            index, col.red, col.green, col.blue);
#endif
     (*nsetcol)++;        /* One more colour set */
     return index;      /* return colour index */
  }
  else
  {    /* Colour table is full - find nearest colour */
     curplace = coltab;
     mindist = 3*NCOLVAL*NCOLVAL;
#ifdef DEBUG_CMAP
  DMESS "TEKcindex: col <%d,%d,%d> \n", col.red,col.green,col.blue);
#endif
     for(index=0;index<maxcol;index++)
     {
        curdist = (col.red - curplace->red) *
                  (col.red - curplace->red);
#ifdef DEBUG_CMAP
    DMESS " min: %d ", mindist );
    DMESS "differences red: (%d - %d)", col.red, curplace->red, curdist );
#endif
        if(curdist < mindist)
        {
           curdist += (col.green - curplace->green) *
                      (col.green - curplace->green);
#ifdef DEBUG_CMAP
    DMESS " green: (%d - %d) %d", col.green, curplace->green, curdist );
#endif
           if(curdist < mindist)
           {
              curdist += (col.blue - curplace->blue) *
                         (col.blue - curplace->blue);
#ifdef DEBUG_CMAP
    DMESS " blue: (%d - %d) %d", col.blue, curplace->blue, curdist );
#endif
              if(curdist < mindist)
              {  /* nearer colour found */
                 mindist = curdist;
                 nearest = index;
              }
           }
        }
        curplace++;   /* Increment to next colour table entry */
#ifdef DEBUG_CMAP
    DMESS "\n");
#endif
     }

#ifdef DEBUG_CMAP
    DMESS "TEKcindex: nearest index %d returned \n",nearest);
#endif

     return nearest;  /* Return index of nearest colour */
  }
/*
  return -1;
*/

}
/*************************************************** TEKcmapupd ********/
#ifdef PROTO
static void TEKcmapupd( void )
#else
static void TEKcmapupd()
#endif
/* Updates the TEK Surface colour map */
{
   Int tindex;     /* TEK Colour Index */
   RGBcolour col;  /* RGB values to be set */

   if(cmap_change)
   {
      for(tindex=0; tindex<n_coltab; tindex++)
      {
           if(TEKcoluse[tindex] == OUT_OF_DATE )
           {  /* Colourmap needs changing */
               col = TEKcoltab[tindex];
               T_CMAP(tindex,col);
#ifdef DEBUG_CMAP
               DMESS "\n TEKcmapupd: CMAP %d ( %d,%d,%d ) %d ",
               tindex,TEKCOLTAB(tindex));
#endif
               TEKcoluse[tindex] = UP_TO_DATE;
           }
      }
      cmap_change = FALSE;
   }
   return;
}
/*************************************************** TEKrsetcoltab *****/
#ifdef PROTO
static void TEKrsetcoltab()
#else
static void TEKrsetcoltab()
#endif

/* Set Colour table to default */

{
   Int index;  /* Colour Index */

   colset_n = 8;

   if((TEKcoltab[0].red   != TEKdftcoltab[0].red)||
      (TEKcoltab[0].green != TEKdftcoltab[0].green)||
      (TEKcoltab[0].blue  != TEKdftcoltab[0].blue))
   {
      TEKcoltab[0] = TEKdftcoltab[0];
      TEKcoluse[0] = OUT_OF_DATE;
   }

   for(index=1;index<DEF_N_COLTAB;index++)
   {
       /* Set TEK Colourtable */
       TEKcoluse[index] = NOT_USED;
       TEKcoltab[index] = TEKdftcoltab[index];
#ifdef DEBUG_CMAP
       DMESS "rsetcoltab %d ( %d,%d,%d ) %d\n", index, TEKCOLTAB(index));
#endif
   }

   if(DEF_N_COLTAB < n_coltab)
   {
      for(index=DEF_N_COLTAB; index<n_coltab; index++)
      {
         TEKcoluse[index] = NOT_USED;
         TEKcoltab[index] = TEKdftcoltab[1];

      }
   }

   cmap_change = TRUE;

   return;
}
/***************************************************** TEKint **********/

#ifdef PROTO
static void TEKint ( Int i)
#else
static void TEKint ( i)

/* Converts an Integer to TEK Character Code and Sends it*/

Int i;
#endif

/* NB: TEKCHOUT is used rather than TEKOUT, to allow '%' to be output */
{
  int  ib, loadd;   /* Absolute value and low byte's offset */
  char lo, hi1, hi2;   /* The low, 1st high and 2nd high bytes */

  /* get absolute value and offset for low byte (depends on sign) */
  if(i >= 0)
  {
     ib = i;
     loadd = 0x30;
  }
  else
  {
     ib = -i;
     loadd = 0x20;
  }

  lo = (ib&0xF) + loadd;  /* get low byte */
  if(ib > 0xF)      /* If 1st high byte necessary */
  {
     hi1 = ((ib>>4)&0x3F) + 0x40; /* get 1st high byte */
     if((ib>>4) > 0x3F)  /* If 2nd high byte necessary */
     {
        hi2 = ((ib>>10)&0x3F) + 0x40; /* get 2nd high byte */
        if((ib>>10) > 0x3F)
        {
          /* Integer out of range -32767 to 32767 */
#ifdef DEBUG
          DMESS "\n TEK Integer %d out of range \n",i);
#endif
        }
        TEKCHOUT(hi2);  /* send 2nd high byte */
     }
     TEKCHOUT(hi1);  /* send 1st high byte */
  }
  TEKCHOUT(lo);  /* send low byte */

  return;
}

/***************************************************** TEKpoint ********/

#ifdef PROTO
static void TEKpoint ( Int ix, Int iy)
#else
static void TEKpoint ( ix, iy)

/* Converts coordinates of a Point to TEK Character Code and Sends it*/

Int  ix;
Int  iy;  /* Point in Terminal Space Coordinates */
#endif

/* NB: TEKCHOUT is used rather than TEKOUT, to allow '%' to be output */

#define LTS 4095   /* Limit of Terminal Space (range 0 to 4095) */

{
  char hiy, eb, loy, hix, lox;   /* Bytes for Coords */

#ifdef XYDEBUG
  DMESS " TSC (%d,%d)",ix,iy);
#endif

  /* If point is out of range, put it into range
     NB: This is not true clipping */
  if(ix < 0)     ix = 0;
  if(ix > LTS)   ix = LTS;
  if(iy < 0)     iy = 0;
  if(iy > LTS)   iy = LTS;

  /* Convert Point to TEK code */
  {
     hix  = (ix >> 7) + 0x20;               /* Get High X */
     hiy  = (iy >> 7) + 0x20;               /* Get High Y */
     lox  = ((ix >> 2) & 0x1F) + 0x40;      /* Get Low X  */
     loy  = ((iy >> 2) & 0x1F) + 0x60;      /* Get Low Y */
     eb   = ((iy&3) << 2) + (ix&3) + 0x60;  /* Get Extra Byte */

#ifdef CMS
    /* Send newline to ensure space character never occurs at end of line
       where it could be ignored as a trailing space */
     NOW;
#endif

     TEKCHOUT(hiy);  /* Send High Y byte */
     TEKCHOUT(eb);   /* Send  Extra Byte */
     TEKCHOUT(loy);  /* Send  Low Y Byte */
     TEKCHOUT(hix);  /* Send High X Byte */
     TEKCHOUT(lox);  /* Send  Low X Byte */

#ifdef XYDEBUG
     DMESS "  %c%c%c%c%c ",hiy,eb,loy,hix,lox);
#endif
  }
  return;
}

/***************************************************** TEKreal *********/

#ifdef PROTO
static void TEKreal ( Float r)
#else
static void TEKreal ( r)

/* Converts a Real Number to TEK Character Code and Sends it*/

Float r;
#endif

#define H14 0.00006014   /* 2 to the power of -14 */
#define LIM 16384.75     /* Maximum value of doublable premantissa */
#define FRACT(r) FABS(r-(float)(int)(r+0.5))  /* Fractional part */

{
  int mantissa, exponent, sign;
  float premant;  /* Premantissa: will be adjusted to become mantissa */

  /* Initialise Exponent, Premantissa and Sign */
  exponent = 0;
  if(r >= 0.0)
  {  /* Positive Real Number */
    premant = r;
    sign    = 1;
  }
  else
  {  /* Negative Real Number: Make Premantissa Positive, Note Sign */
    premant = -r;
    sign    = -1;
  }
  if(premant >= 2*LIM)
  {      /* The premantissa is too big to be an integer */
     for(exponent=0;exponent<16;exponent++)
     {
         premant = 0.5*premant;
         if(premant < 2*LIM)
         {  /* The premantissa is no longer too big to be an integer */
             continue;
         }
     }
  }
  else
  {
     if((premant < LIM) && (FRACT(premant) >= H14))
     {     /* The premantissa can doubled to improve accuracy */
        if(premant >= H14)
        {      /* The premantissa is not neglible */
           for(exponent=0;exponent>-16;exponent--)
           {
              premant = 2.0*premant;
              if((premant >= LIM) || (FRACT(premant) < H14))
              { /* The premantissa can not be doubled or is integer */
                  continue;
              }
           }
        }
     }
  }
  /* Absolute value of Mantissa is now nearest integer to premantissa */
  mantissa = (int)(premant + 0.5);
  mantissa = sign*mantissa;   /* sign put back in */

  TEKint(mantissa);   /* Send Mantissa */
  TEKint(exponent);   /* Send Exponent */

  return;
}
/**************************************************** TEKtcharch ********/
#ifdef CHARCHECK
#ifdef PROTO
static void TEKtcharch( void )
#else
static void TEKtcharch()
#endif
/* Used to check that characters are translated correctly by TCHAR */
/* CHARCHECK needs to be #defined and snoopy mode used */
{
   Int i;
   char ch;
   TEKOUT("\n");
   for(i=0x20;i<0x40;i++)
   {
     TEKCHOUT(i);
   }
   TEKOUT("\n");
   for(i=0x40;i<0x60;i++)
   {
     TEKCHOUT(i);
   }
   TEKOUT("\n");
   for(i=0x60;i<0x80;i++)
   {
     TEKCHOUT(i);
   }
   TEKOUT("\n");
   return;
 }
#endif /* CHARCHECK */
/*********************************/
/*       UTILITIES               */
