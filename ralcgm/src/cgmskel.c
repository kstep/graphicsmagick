/*   RAL-CGM Interpreter module:  @(#) cgmskel.c  version 3.2
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
 * Description:  Skeleton output driver
 *
 * Author:  A N Other

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
 *  dd mon yy INIT Comment
 *
 * Comments:
 *    PLEASE REPLACE ALL OCCURENCES OF 'XXX' WITH A SUITABLE MNEUMONIC
 */

/*  Include relevant parameters and structures */

#include "cgmout.h"

/*  Include device specific variables and structures */

#include "cgmxxx.h"


#ifdef PROTO       /* Standard C use function prototyping */
void CGMOxxx( Code, Long*, Float*, char* ),
     XXXinit( void ),
     XXXclose( void ),
     XXXnewpic( char *name ),
     XXXmenu( void ),
     XXXattrib( Code type ),
     XXXline( Int n, Long *pi, Float *pr, Enum set ),
     XXXmarker( Int, Long*, Float* ),
     XXXtext( char* ),
     XXXcells( Int, Long* ),
     XXXgdp( Code type, Long *pi, Float *pr, Enum close ),
     XXXfill( Int, Lpoint* ),
     XXXedge( Int n, Lpoint *pt, Enum *edgeflag );

                               /*  CGM Utilities    */

extern void GDPcentre( Point *cen, Float *rad, Float *ang,
                       Point a, Point b, Point c ),
            GDParc( Point cen, Float rad,
                    Point a, Point b, Float *ang, Point *c, Point *d ),
            GDPcircle( Point cen, Float rad, Float ang, Point a,
                       Float rat, Long *np, LPOINT(pt), Enum cl ),
            GDPellipse( Point cen, Point cdp1, Point cdp2, Point a, Point b,
                        Float rat, Long *np, LPOINT(pt), Enum cl );

extern void CGMfill( Long n, Long *pi, Float *pr, LPOINT(pl), Enum set );
#else
void CGMOxxx(),     /* Main driver routine */
     XXXinit(),      /* Initialise Graphics */
     XXXclose(),     /* Close down Graphics  */
     XXXnewpic(),    /* Start a new frame  */
     XXXmenu(),      /* Select Options */
     XXXattrib(),    /* Set Iris Attributes */
     XXXline(),      /* Draw a polyline from Data Area */
     XXXmarker(),    /* draw polymarkers */
     XXXtext(),      /* output text */
     XXXcells(),     /* Draw cell arrays */
     XXXgdp(),       /* draw GDPs */
     XXXfill(),      /* polygon fill */
     XXXedge();/* Edge drawing */

                               /*  CGM Utilities    */

extern void GDPcentre(),     /* find circle centre */
            GDParc(),        /* Generate points on arc */
            GDPcircle(),     /* Generate points on circle */
            GDPellipse();    /* Generate points for ellipse and arc */
extern void FILfill();       /* General Fill routine */
#endif

/*   Direct colour calculation */

#define RGBVAL(x,y)     ( (short) 255 * (x - cur.min_rgb.y)\
                          / (cur.max_rgb.y - cur.min_rgb.y) )

/*   Debug messages  */

#define DMESS         (void) fprintf(stderr,

static char *func = "CGMXXX", emess[48];

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

/**************************************************** CGMOxxx **********/
#ifdef PROTO
void CGMOxxx ( Code c, Long *pi, Float *pr, char *str )
#else
void CGMOxxx ( c, pi, pr, str )

/*  Output Driver for device XXX  */

Code c;
Long *pi;
Float *pr;
char *str;
#endif
{
   static Logical first = TRUE;
   register Long n, i, j, num;
   Code major;
   Enum close;
   Lpoint localpts[ARRAY_MAX];

#ifdef DEBUG
   DMESS "\n  CGMxxx code: 0x%x\n", c);
#endif

   if ( c == (Code) EOF ) exit(0);
   major = c>>8;
   num = *pi++;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         switch (c)
         {
            case LINE:         /*  Polyline   */
               if ( first ) XXXattrib(LINE);
               XXXline ( num, pi, pr, NOSET);
               first = ( num >= ZERO );
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first ) XXXattrib(LINE);
               for (i = 1; i <= abs(num); i += 2)
               {
                  XXXline (2L, pi, pr, NOSET);
                  pi += 4; pr += 4;
               }
               first = ( num >= ZERO );
               break;

            case MARKER:         /*  Polymarker  */
               if ( first ) XXXattrib(MARKER);
               XXXmarker ( num, pi, pr);
               first = ( num >= ZERO );
               break;

            case TEXT:         /*  Text   */
/*  Save Text Position from (*pr,*pr+1) or (*pi,*pi+1) */
               ATTtext(NTEXTB, text_bundle, NCOLOURS, &xxxtext);
               xxxtext_item = TXTaccinf(num, str, &xxxtext, text_method);
               if ( (Enum)num == FINAL ) XXXtext( xxxtext_item );
               break;

            case RESTRTEXT:         /*  Restricted Text */
/*  Save Text box size from *pr, *pr+1 or *pi, *pi+1
    and Text Position from (*pr+2,*pr+3) or (*pi+2,*pi+3) */
               ATTtext(NTEXTB, text_bundle, NCOLOURS, &xxxtext);
               xxxtext_item = TXTaccinf(num, str, &xxxtext, text_method);
               if ( (Enum)num == FINAL ) XXXtext( xxxtext_item );
               break;

            case APNDTEXT:         /*  Append Text   */
               ATTtext(NTEXTB, text_bundle, NCOLOURS, &xxxtext);
               xxxtext_item = TXTaccinf(num, str, &xxxtext, text_method);
               if ( (Enum)num == FINAL ) XXXtext( xxxtext_item );
               break;

            case POLYGON:         /*  Polygon   */
               XXXattrib(POLYGON);
               CGMfill ( num, pi, pr, localpts, NOSET);
               break;

            case POLYGONSET:         /*  Polygon Set  */
               XXXattrib(POLYGON);
               CGMfill ( num, pi, pr, localpts, SET);
               break;

            case CELLARRAY:         /*  Cell Array  */
               if ( first )
               {
                  XXXattrib (CELLARRAY);

/*  set up local Cell Array parameters here */

               }
               XXXcells ( num, pi );
               first = ( num >= ZERO );
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               j = *pi++;
               if ( cgmralgks )
               {
                  Enum close;
                  close = ( j == GKSPIE ? PIE
                                        : (j == GKSCHORD ? CHORD
                                                         : NOCLOSE ));
                  XXXgdp ( (Code) j, pi, pr, close);
               }
               else
               {
                  CGMerror(func, ERR_NOGDP, ERROR, NULLSTR);
               }
               break;

            case RECT:         /*  Rectangle   */
               XXXgdp (RECT, pi, pr, NOCLOSE);
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
               XXXinit();
               break;

            case ENDMF:         /*  End Metafile   */
               XXXclose();
               break;

            case BEGPIC:        /*  Begin Picture Descriptor   */
               XXXnewpic(str);
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               XXXattrib(BEGPIC);
               break;

            case ENDPIC:         /*  End  Picture    */
               XXXmenu();
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
               break;

            case BACKCOLR:         /*  Background Colour  */
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
               XXXgdp (CIRCLE, pi, pr, NOCLOSE);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               XXXgdp (ARC3PT, pi, pr, NOCLOSE);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               XXXgdp (ARC3PTCLOSE, pi, pr, (Enum) *(pi+6)+1 );
               break;

            case ARCCTR:       /* Circle Arc centre */
               XXXgdp (ARCCTR, pi, pr, NOCLOSE);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               XXXgdp (ARCCTRCLOSE, pi, pr, (Enum) *(pi+7)+1 );
               break;

            case ELLIPSE:       /* Ellipse    */
               XXXgdp (ELLIPSE, pi, pr, NOCLOSE);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               XXXgdp (ELLIPARC, pi, pr, NOCLOSE);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close*/
               XXXgdp (ELLIPARCCLOSE, pi, pr, (Enum) *(pi+10)+1 );
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

            case ALTCHARSETINDEX:   /*  Altern Character Set Index */
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

            case PATTABLE:       /*  Pattern Table  */

            case PATSIZE:       /*  Pattern Size  */

            case COLRTABLE:       /*  Colour Table  */

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

/***************************************************** XXXattrib *******/

#ifdef PROTO
void XXXattrib ( Code type )
#else
void XXXattrib (type)

/*  Converts CGM Attribute 'type' to local Attribute  */

Code type;
#endif

{
    static Logical last_prim = NONOP;

#ifdef DEBUG
    DMESS  "XXXattrib: type 0x%x\n", type);
#endif

    switch (type)
    {
       case LINE:

/* if no attribute has changed and last primitive was a line return */

          if ( ! att_flag && last_prim == LINE ) return;

/* Set local attributes here  */

          last_prim = LINE;
          break;


      case MARKER:

/* if no attribute has changed and last primitive was a marker return */

          if ( ! att_flag  && last_prim == MARKER ) return;

/* Set local attributes here  */

          last_prim = MARKER;
          break;

      case TEXT:

/* if no attribute has changed and last primitive was text return */

          if ( ! att_flag && last_prim == TEXT ) return;

/* Set local attributes here  */

          last_prim = TEXT;
          break;

      case POLYGON:  /*  Fill attributes  */

/* if no attribute has changed and last primitive was a Fill Area return */

          if ( ! att_flag && last_prim == POLYGON ) return;

/* Set local attributes here  */

          last_prim = POLYGON;
          break;

      case EDGEVIS:    /*  Edge attributes */

/* if no attribute has changed and last primitive was a edge return */

          if ( ! att_flag && last_prim == EDGEVIS ) return;

/* Set local attributes here  */

          last_prim = EDGEVIS;
          break;

      case CELLARRAY:  /*  Cell array - may affect texture & colour*/
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

/***************************************************** XXXinit *********/

#ifdef PROTO
void XXXinit( void )
#else
void XXXinit()
#endif

/*  Initialise Variables and colour map */

{
   return;
}

/***************************************************** XXXnewpic *******/

#ifdef PROTO
void XXXnewpic( char *pic_name )
#else
void XXXnewpic( pic_name )
#endif

/*   New Frame processing started at first drawing element  */

char *pic_name;
{
   return;
}

/***************************************************** XXXmenu *********/

#ifdef PROTO
void XXXmenu( void )
#else
void XXXmenu()
#endif

/*  End of frame processing to select options */

{
   return;
}

/***************************************************** XXXclose ********/

#ifdef PROTO
void XXXclose( void )
#else
void XXXclose()
#endif

/*  Close down the Graphics  */

{
   return;
}


/***************************************************** XXXline *********/

#ifdef PROTO
void XXXline ( Int n, Long *pi, Float *pr, Enum set )
#else
void XXXline ( n, pi, pr, set )

/*   draws n point polyline starting at pi/pr  */

Int n;
Long *pi;
Float *pr;
Enum set;
#endif
{
   return;
}

/***************************************************** XXXmarker *******/

#ifdef PROTO
void XXXmarker ( Int n, Long *pi, Float *pr)
#else
void XXXmarker ( n, pi, pr)

/*   draws n markers starting at pi/pr  */

Int n;
Long *pi;
Float *pr;
#endif
{
   return;
}

/***************************************************** XXXtext *********/

#ifdef PROTO
void XXXtext ( Textitem *txt )
#else
void XXXtext ( txt )

/*  draws TEXT string 's' according to current settings */

Textitem *txt;
#endif
{
   Textitem curtxt = txt;
   Point txtpos; /* Current text position */

   TXTalign ( txt, txtpos, TXTgwidth(txt), TXTgheight(txt) );
   while ( curtxt != NULL )
   {
      switch( curtxt->text_method )
      {
#ifdef HERSHEY
         case HER:
            HERtext( curtxt, txtpos, XXXtxtline );
            break;
#endif
#ifdef BEZIER
         case BEZ:
            BEZtext( curtxt, &xxxtxt, txtpos, XXXtxtfill );
            break;
#endif
#ifdef TEX
         case TEX:
            break;
#endif
         case HDW:
         default:
                  /* Local text method */
            break;
      }
      curtxt = curtxt->next;
   }
   TXTfree(txt);
   return;
}

/***************************************************** XXXcells ********/

#ifdef PROTO
void XXXcells ( Int num, Long *pi )
#else
void XXXcells ( num, pi )

/*  Draws a Cell array of n values 'pi' */

Int num;
Long *pi;
#endif
{
   return;
}

/***************************************************** XXXgdp **********/

#ifdef PROTO
void XXXgdp ( Code type, Long *pi, Float *pr, Enum close )
#else
void XXXgdp ( type, pi, pr, close )

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
   Float rad, ang, ang1, ang2, wvrat;
   Point centre, cdp1, cdp2;
   Lpoint *pt = localpts;

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

   if ( fill ) XXXattrib ( POLYGON );
   else        XXXattrib ( LINE );

/* make sure area is visible */

   if ( ! (fill || edge) ) return;

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

/*  Now work out significant points  */

   switch ( type )
   {
      case ARCCTR:
      case ARCCTRCLOSE:
         centre = p1;
         rad = p4.x;
         CGMarc (centre, rad, p2, p3, &ang, &p1, &p3);
         centreknown = TRUE;

      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
         if ( ! centreknown )
                CGMcentre ( &centre, &rad, &ang, p1, p2, p3);

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
               CGMcircle ( centre, rad, ang, p1, wvrat, &np, pt, close );
               type = SOFTWARE;
            }
            break;
         }
         if ( close == CHORD || fill == PATTERN )
         {
                   /*  Work out points  */
            CGMcircle ( centre, rad, ang, p1, wvrat, &np, pt, close );
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
         centre = p1;
         rad = SQRT( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
         if ( fill == PATTERN )
         {
            CGMcircle ( centre, rad, 2*PI, p2, wvrat, &np, pt, close );
            type = SOFTWARE;
         }
         break;

      case CIRCLE:
         centre = p1;
         rad = p2.x;
         p1.x += rad;
         if ( fill == PATTERN )
         {
            CGMcircle ( centre, rad, 2*PI, p1, wvrat, &np, pt, close );
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
            pt = localpts;
            type = SOFTWARE;
         }
         break;

      case ELLIPSE:
         centre = p1;
         cdp1 = p2;
         cdp2 = p3;
         p4.x = 1.0;  p4.y = 0.0;
         CGMellipse(centre, cdp1, cdp2, p4, p4, wvrat, &np, pt, close );
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
         CGMellipse(centre, cdp1, cdp2, p4, p5, wvrat, &np, pt, close);
         type = SOFTWARE;
         break;

      default:
         break;
   }

/* now draw primitives  */

   switch ( type )
   {
      case SOFTWARE:
         break;

      case ARCCTR:
      case ARCCTRCLOSE:
      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
         break;

      case GKSCIRCLE:
         break;

      case CIRCLE:
         break;

      case LINE:    /*   Points are co-linear  */
         break;

      case RECT:
         break;

      case ELLIPSE:

      case ELLIPARC:

      case ELLIPARCCLOSE:

      default:
         break;
   }
   return;
}

/***************************************************** XXXfill *********/

#ifdef PROTO
void XXXfill ( Int np, Lpoint *pt )
#else
void XXXfill ( np, pt )

/*  Fills convex polygon of np points in *pt  */

Int np;
Lpoint *pt;
#endif

{
   return;
}

/***************************************************** XXXedge *********/

#ifdef PROTO
void XXXedge ( Int np, Long *pt, Enum *edgeflag )
#else
void XXXedge ( np, pt, edgeflag )

/*  Draws edge for convex polygon of np points in pt[]  */

Int np;
Lpoint *pt;
Enum *edgeflag;
#endif

{
   return;
}
