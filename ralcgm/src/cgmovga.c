/*   RAL-CGM Interpreter module:  @(#) cgmovga.c  version 3.2
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
 * W M LAM @ Rutherford Appleton Laboratory (wml@uk.ac.rl.ib)
 *         @ The Polytechnic of Wales (wmlam@uk.ac.pow.genvax)
 *
 * Description:  Output driver for IBM PS/2 in VGA or EGA mode
 *
 * Author:  WAI MAN LAM
 *
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
 *
 *   1 Jul 91 WML  Original version stabilised
 *  21 Aug 91 RTP  Add SCCS id
 *   8 Oct 91 RTP  Add VGAoptions for High and Low Res
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *                 and REQUAL etc to cgmout.h
 *
 * Limitations:
 *   Fill Patterns not supported
 *   No facility to search for Frames
 *   Interactive control at end of Frame to select options needed
 *   Clip rect not supported
 *   POLYGONSET not supported
 */

/*  Include relevant parameters and structures */

#include "cgmout.h"
#include "cgmtext.h"

/*  Include device specific variables and structures */

#include "cgmvga.h"
#include "cgmgdp.h"
#ifdef BEZIER
#include "cgmbez.h"
#endif
#ifdef HERSHEY
#include "cgmher.h"
#endif

#ifdef PROTO       /* Standard C use function prototyping */
void CGMOvga( Code, Long*, Float*, char* ),
     VGAoptions( char* ),
     VGAinit( void ),
     VGAclose( void ),
     VGAnewpic( char * ),
     VGAmenu( void ),
     VGAattrib( Code ),
     VGAline( Long , Long *, Float *, Code, Enum ),
     VGAmarker( Long, Long *, Float * ),
     VGAtext( Textitem * ),
     VGAcells( Long, Long * ),
     VGAgdp(   Code , Long *, Float *, Enum ),
     VGAfill(  Long , Long *, Float *, Enum ),
     VGAwindow( void ),
     pause( void ),
     VGAtxline( Int, Point *, Tmatrix, int ),
     VGAtxfill ( Int ,Point *,Tmatrix,int  ),
     setcolor( Index, Posint, Posint, Posint ),
     VGAsetdef( void ),
     setclip( void );

#else

void CGMOvga(),      /* Main driver routine */
     VGAoptions(),   /* VGA Options */
     VGAinit(),      /* Initialise Graphics system */
     VGAclose(),     /* Close down Graphics system */
     VGAnewpic(),    /* Start a new frame  */
     VGAmenu(),      /* Select Options */
     VGAattrib(),    /* Set Attributes */
     VGAline(),      /* Line draw routine */
     VGAmarker(),    /* draw polymarkers */
     VGAtext(),      /* output text */
     VGAcells(),     /* Draw cell arrays */
     VGAgdp(),             /* draw GDPs */
     VGAfill(),      /* polygon fill */
     VGAwindow(),    /* Set VDC extend */
     setcolor(),     /* Set or match color */
     pause(),             /* Pause until hit any key */
     VGAtxline(),    /* Display hershey text */
     VGAtxfill(),    /* Display bezier text */
     VGAsetdef(),    /* Set default locally */
     setclip();      /* store clipping information */

#endif

/**   Local Macros - Convert World coordinate to screen coordinate **/
#define XPC(wx)   ( (double) ( A * (double)(wx) + B ) )
#define YPC(wy)   ( (double) ( C * (double)(wy) + D ) )

/**   Direct colour calculation **/
#define RGBVAL(x,y)  ( (long)( 63.0 * (float)(x - cur.min_rgb.y)\
                      /(float)(cur.max_rgb.y - cur.min_rgb.y) ))

/*   Debug messages  */

/**** Debug Flags ******/
/* #define DEBUG       */
/* #define DEBUG_FILL  */
/* #define DEBUG_CLIP  */
/* #define DEBUG_COLOR */
/* #define DEBUG_BACK  */
/* #define DEBUG_CELL  */
/* #define DEBUG_LINE  */
/* #define DEBUG_TXTF  */
/***********************/

static char *func = "CGMOvga", emess[48];

/***      Local Variable      ***/

static struct {  /* Cell array Parameters */
   Point p, q, r, d, dd;
   unsigned short nx, ny, px, py;
} cell;

static int entry=1; /* no. of color entries */
/* Fill mask */
static unsigned char solidfill[] = { 0xFF, 0xFF, 0xFF, 0xFF,
                                     0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char emptyfill[] = { 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00};

Logical newpic=FALSE;

/**************************************************** CGMOvga **********/

void CGMOvga ( Code c, Long *pi, Float *pr, Char *str )

/*  Output Driver for device VGA & EGA screen */
{
   static Logical first = TRUE;
   register Long n, i, j, num;
   Code major;
   Enum close;
   Lpoint localpts[ARRAY_MAX];
   Textitem *txt_item;


   if ( c == (Code) EOF ) exit(0);
   major = c>>8;
   num = *pi++;

   switch (major)
   {
      case 0x00:               /* Graphics Primitives  */

         switch (c)
         {
            case NONOP:
               break;

            case LINE:         /*  Polyline   */
               if ( first ) VGAattrib( LINE );
               VGAline ( num, pi, pr, LINE, cur.vdc_type);
               first = ( num >= ZERO );
               break;

            case DISJTLINE:         /*  Disjoint Polyline  */
               if ( first ) VGAattrib( LINE );
               for (i = 1; i <= labs( num ); i += 2)
               {
                  VGAline ( 2L, pi, pr, LINE, cur.vdc_type );
                  pi += 4; pr += 4;
               }
               first = ( num >= ZERO );
               break;

            case MARKER:         /*  Polymarker  */
               if ( first ) VGAattrib( MARKER );
               VGAmarker ( num, pi, pr );
               first = ( num >= ZERO );
               break;

            case TEXT:               /*  Text   */
               VGAattrib( TEXT );
               if ( cur.vdc_type == REAL)
               {
                  txtpos.x = (float)pr[0];
                  txtpos.y = (float)pr[1];
               }
               else
               {
                  txtpos.x=(float)pi[0];
                  txtpos.y=(float)pi[1];
               }
               txt_item = TXTaccinf( num, str, &vgatext, text_method );
               if ( (Enum)num == FINAL ) VGAtext( txt_item );
               break;


            case RESTRTEXT:            /*  Restricted Text */
               VGAattrib( TEXT );
               if ( cur.vdc_type == REAL )
               {
                  txtpos.x=(float)pr[2];
                  txtpos.y=(float)pr[3];
               }
               else
               {
                  txtpos.x=(float)pi[2];
                  txtpos.y=(float)pi[3];
               }

               txt_item = TXTaccinf( num, str, &vgatext, text_method );
               if ( (Enum)num == FINAL ) VGAtext( txt_item );
               break;

            case APNDTEXT:           /*  Append Text   */
               VGAattrib( TEXT );
               txt_item = TXTaccinf( num, str, &vgatext, text_method );
               if ( (Enum)num == FINAL ) VGAtext( txt_item );
               break;

            case POLYGON:         /*  Polygon   */
               VGAattrib( POLYGON );
               VGAfill( num, pi, pr, cur.vdc_type );
               if ( curatt.edge_vis == ON )
               {
                  VGAattrib( EDGEVIS );
                  VGAline( num, pi, pr, EDGEVIS, cur.vdc_type);
               }
               break;

            case POLYGONSET:         /*  Polygon Set  */
               VGAattrib( POLYGON );
               printf("\nPOLYGONSET not yet support\n");
               break;

            case CELLARRAY:         /*  Cell Array  */
               if ( first )
               {
                  if (cur.vdc_type == REAL)
                  {
                     cell.p.x = (float)XPC( *pr++ );
                     cell.p.y = (float)YPC( *pr++ );
                     cell.q.x = (float)XPC( *pr++ );
                     cell.q.y = (float)YPC( *pr++ );
                     cell.r.x = (float)XPC( *pr++ );
                     cell.r.y = (float)YPC( *pr++ );
                     pi += 6;
                  }
                  else
                  {
                     cell.p.x = (float)XPC( *pi++ );
                     cell.p.y = (float)YPC( *pi++ );
                     cell.q.x = (float)XPC( *pi++ );
                     cell.q.y = (float)YPC( *pi++ );
                     cell.r.x = (float)XPC( *pi++ );
                     cell.r.y = (float)YPC( *pi++ );

                  }
                  cell.nx = *pi++;
                  cell.ny = *pi++;
                  pi++;              /* Ignore Local Precision */
                  cell.d.x = fabs( cell.q.x - cell.r.x );
                  cell.d.y = cell.q.y - cell.r.y;
                  cell.dd.x = fabs( (cell.r.x-cell.p.x) /(float)cell.nx );
                  cell.dd.y = (cell.q.y-cell.r.y) /(float)cell.ny;
#ifdef DEBUG_CELL
                  printf(" CellArray parm.\n");
                  printf("cell.p.x:%f cell.p.y:%f\n",cell.p.x,cell.p.y);
                  printf("cell.r.x:%f cell.r.y:%f\n",cell.r.x,cell.r.y);
                  printf("cell.d.x:%f cell.dd.x:%f\n",cell.d.x,cell.dd.x);
                  pause();
#endif

                  VGAattrib ( CELLARRAY );
               }

/*  set up local Cell Array parameters here */

               VGAcells ( num, pi );
               first = ( num >= ZERO );
               break;

            case GDP:         /*  Generalised Drawing Primitive  */
               j = *pi++;
               close = ( cgmralgks && j == GKSPIE ? PIE :
                       ( cgmralgks && j == GKSCHORD ? CHORD
                                                    : ANY ));
               VGAgdp ( (Code) j, pi, pr, close);
               break;

            case RECT:         /*  Rectangle   */
               VGAgdp (RECT, pi, pr, ANY);
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
               VGAinit();
               break;

            case ENDMF:         /*  End Metafile   */
               VGAclose();
               break;

            case BEGPIC:        /*  Begin Picture Descriptor   */
               VGAnewpic( str );
               break;

            case BEGPICBODY:         /*  Begin Picture Body  */
               VGAattrib( BEGPIC );
               VGAsetdef();
               break;

            case ENDPIC:         /*  End  Picture    */
               newpic=FALSE;
               free( vgacolrtb ); /* free color table */
               VGAmenu();
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVDELIM, ERROR, emess );
         }
         break;


      case 0x31:  /* Metafile Descriptor Elements  */
         switch (c)
         {
            case MFVERSION:        /* Metafile version */

            case MFDESC:        /* Metafile Description */

            case VDCTYPE:        /* VDC Type */

            case INTEGERPREC:        /* Integer Precision */

            case REALPREC:        /* Real Precision */

            case INDEXPREC:        /* Index Precision */

            case COLRPREC:        /* Colour Precision */

            case COLRINDEXPREC: /* Colour Index Precision */

            case MAXCOLRINDEX:        /* Maximum Colour Index */

            case COLRVALUEEXT:        /* Colour value extent */

            case MFELEMLIST:        /* Metafile element List */

            case BEGMFDEFAULTS: /* Begin Metafile defaults Replacement */

            case ENDMFDEFAULTS: /* End Metafile defaults Replacement */

            case FONTLIST:        /* Font List */

            case CHARSETLIST:        /* Character set list */

            case CHARCODING:        /* Character coding Announcer */
               break;
         }
         break;

      case 0x32:  /* Picture Descriptor Elements  */
         switch (c)
         {
            case COLRMODE:             /*  Colour Selection Mode */

            case SCALEMODE:                     /*  Scaling Mode */

            case LINEWIDTHMODE:     /*  Line width Specification */

            case MARKERSIZEMODE:   /*  Marker size Specification */

            case EDGEWIDTHMODE:     /*  Edge width Specification */

            case VDCEXT:                          /*  VDC Extent */

            case BACKCOLR:                 /*  Background Colour */

               break;

            default:
               (void) sprintf (emess, "(code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, emess);
         }
         break;

      case 0x33:  /* Control Elements  */

         switch(c)
         {
            case VDCINTEGERPREC:       /* VDC Integer Precision */

            case VDCREALPREC:             /* VDC Real Precision */

            case AUXCOLR:                   /* Auxiliary Colour */

            case TRANSPARENCY:                  /* Transparency */
                 break;

            case CLIPRECT:                    /* Clip Rectangle */

            case CLIP:                        /* Clip Indicator */
                  setclip();
                  break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVCONT, ERROR, emess);
         }
         break;

      case 0x34:  /* Circles and Ellipses  */

         switch(c)
         {
            case CIRCLE:       /* Circle */
               VGAgdp (CIRCLE, pi, pr, ANY);
               break;

            case ARC3PT:       /* Circular Arc  3 point */
               VGAgdp (ARC3PT, pi, pr, ANY);
               break;

            case ARC3PTCLOSE:       /* Circular Arc  3 point close */
               VGAgdp (ARC3PTCLOSE, pi, pr, (Enum) *(pi+6) );
               break;

            case ARCCTR:       /* Circle Arc centre */
               VGAgdp (ARCCTR, pi, pr, ANY);
               break;

            case ARCCTRCLOSE:       /* Circle Arc centre close */
               VGAgdp (ARCCTRCLOSE, pi, pr, (Enum) *(pi+7) );
               break;

            case ELLIPSE:        /* Ellipse */
               VGAgdp (ELLIPSE, pi, pr, ANY);
               break;

            case ELLIPARC:       /* Elliptical Arc */
               VGAgdp (ELLIPARC, pi, pr, ANY);
               break;

            case ELLIPARCCLOSE:       /* Elliptical Arc close */
               VGAgdp (ELLIPARCCLOSE, pi, pr, (Enum) *(pi+10) );
               break;

            default:
               (void) sprintf ( emess, "(Code: 0x%x)", c);
               (void) CGMerror ( func, ERR_INVOUT, ERROR, emess );
         }
         break;

      case 0x35:  /* Attributes */
         att_flag = TRUE;
         switch(c)
         {
            case CHARHEIGHT:           /* Character Height */

            case LINEWIDTH:                  /* Line Width */

            case MARKERSIZE:                /* Marker Size */

            case LINEINDEX:           /* Line Bundle index */

            case LINETYPE:                    /* Line Type */

            case LINECOLR:                  /* Line Colour */

            case MARKERINDEX:       /* Marker Bundle index */

            case MARKERTYPE:                /* Marker Type */

            case MARKERCOLR:              /* Marker Colour */

            case TEXTINDEX:           /* Text Bundle index */

            case TEXTFONTINDEX:         /* Text Font Index */

            case TEXTPREC:               /* Text Precision */

            case CHAREXPAN:  /* Character Expansion Factor */

            case CHARSPACE:           /* Character Spacing */

            case TEXTCOLR:                  /* Text Colour */

            case CHARORI:         /* Character Orientation */

            case TEXTPATH:                    /* Text Path */

            case TEXTALIGN:              /* Text Alignment */

            case CHARSETINDEX:      /* Character Set Index */

            case ALTCHARSETINDEX:  /* Altern Character Set Index */
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
            case EDGEWIDTH:               /*  Edge Width */

            case FILLINDEX:       /*  Fill Bundle index  */

            case INTSTYLE:           /*  Interior Style  */

            case FILLCOLR:               /*  Fill Colour */

            case HATCHINDEX:            /*  Hatch Index  */

            case PATINDEX:            /*  Pattern Index  */

            case EDGEINDEX:       /*  Edge Bundle index  */

            case EDGETYPE:                /*  Edge Type  */

            case EDGECOLR:              /*  Edge Colour  */

            case EDGEVIS:           /*  Edge Visibility  */

            case FILLREFPT:    /*  Fill Reference Point  */

            case PATTABLE:            /*  Pattern Table  */

            case PATSIZE:              /*  Pattern Size  */
                  break;

            case  COLRTABLE:           /*  Colour Table  */
            {
               unsigned long color;long i=0L,j;

               i=*pi++; /* color index */
               if ( (num+i) >= (Long)coltbsize )
               {
                  /* reallocated local color table */
                  coltbsize += ( int )num;
                  vgacolrtb = ( rgbcolor *)realloc( (void *)vgacolrtb,
                               sizeof( rgbcolor ) * coltbsize );
               }

               /* Fill the internal color table & Set color palette */
               for (j =0; j<num; j++,i++)
               {
                  unsigned long red,green,blue;
                  vgacolrtb[i].r =(unsigned short)*pi++;
                  vgacolrtb[i].g =(unsigned short)*pi++;
                  vgacolrtb[i].b =(unsigned short)*pi++;
                  red   = (unsigned long)RGBVAL( vgacolrtb[i].r, red);
                  green = (unsigned long)RGBVAL( vgacolrtb[i].g, green);
                  blue  = (unsigned long)RGBVAL( vgacolrtb[i].b, blue);
                  color = red | ( green<<8 ) | ( blue<<16 );
                  if ( i<(Long)nc ) _remappalette((short)i,color);
               }

            }
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

/***************************************************** VGAoptions ******/
#ifdef PROTO
void VGAoptions( char *opts )
#else
void VGAoptions( opts )
char *opts;
#endif

{
   int i, len = strlen(opts);

   if ( opts == NULL | !len ) return;

   for ( i=0; i < len; i++ )
   {
      switch ( toupper( opts[i] ) )
      {
         case 'H':
            vgares = HIGH;
            break;

         case 'L':
            vgares = LOW;
            break;

         default:
            break;
      }
   }

   return;
}

/***************************************************** VGAattrib *******/

void VGAattrib ( Code type)

/*  Converts CGM Attribute 'type' to local Attribute  */
{
    static Logical last_prim = NONOP;
#ifdef DEBUG
    DMESS  "VGAattrib: type 0x%x\n", type);
#endif

    switch (type)
    {
       case LINE:
         if ( ! att_flag && last_prim == LINE ) return;

         ATTline(NLINEB,line_bundle,ncol,&vgaline);

         _setfillmask( solidfill );
         setcolor( vgaline.colindex,  vgaline.col.red,
                   vgaline.col.green, vgaline.col.blue);

         /* Set local attributes here  */
         last_prim = LINE;
         break;

      case MARKER:
      /* if no attribute has changed and last primitive was a marker return */

        if ( ! att_flag  && last_prim == MARKER ) return;

        ATTmarker( NMARKB, mark_bundle, ncol, &vgamark );
        setcolor( vgamark.colindex,  vgamark.col.red,
                  vgamark.col.green, vgamark.col.blue );

        /* Set local attributes here */
        last_prim = MARKER;
        break;

      case TEXT:
        /* if no attribute has changed and last primitive was a marker return */

        if ( ! att_flag  && last_prim == TEXT ) return;
        ATTtext( NTEXTB, text_bundle, ncol, &vgatext );
        setcolor( vgatext.colindex,  vgatext.col.red,
                  vgatext.col.green, vgatext.col.blue);


          last_prim = TEXT;
          break;

      case POLYGON:  /*  Fill attributes  */

        ATTfill( NFILLB, fill_bundle, ncol, &vgafill );
        switch ( vgafill.style )
        {
           case HOLLOW:
#ifdef DEBUG_FILL
   printf(" FILLarea HOLLOW FILL ");
   pause();
#endif
                _setfillmask( emptyfill );
                break;

           case PATTERN:
#ifdef DEBUG_FILL
   printf(" FILLarea PATTERN FILL ");
   pause();
#endif
             /* PATTERN not support */
             printf(" Pattern not support yet ");

           case SOLID:
#ifdef DEBUG_FILL
   printf(" FILLarea SOLID FILL ");
   pause();
#endif
                _setfillmask( solidfill );
                setcolor( vgafill.colindex,  vgafill.col.red,
                          vgafill.col.green, vgafill.col.blue);
                break;

           case HATCH:
#ifdef DEBUG_FILL
       printf(" FILLarea HATCH FILL ");
       pause();
#endif
                _setfillmask( hatch[vgafill.hatch%(HATCHES-1)] );
                setcolor( vgafill.colindex,  vgafill.col.red,
                          vgafill.col.green, vgafill.col.blue);
                break;

        }
        last_prim = POLYGON;
        break;

      case EDGEVIS:    /*  Edge attributes */
      /* if no attribute has changed and last primitive was a edge return */
          if ( ! att_flag && last_prim == EDGEVIS ) return;

          ATTedge(NEDGEB,edge_bundle,ncol,&vgaedge);

          /* Set local attributes here        */

          _setfillmask( solidfill );
          setcolor( vgaedge.colindex,  vgaedge.col.red,
                    vgaedge.col.green, vgaedge.col.blue);
          last_prim = EDGEVIS;
          break;

      case CELLARRAY:  /*  Cell array - may affect texture & colour*/
          _setfillmask( solidfill );
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

/***************************************************** VGAinit *********/

void VGAinit( void )

/*  Initialise Variables and colour map */

{
   if ( vgares == LOW )
      _setvideomode( _MAXCOLORMODE ); /* maxium colour */
   else _setvideomode(_MAXRESMODE );         /* maxium resolution  */

   _getvideoconfig( &config);/* Return the current graphics environment */
                             /* configuration and store them in config        */
   nc=config.numcolors;             /* Maxium no. of colors available */

   /* Set default window & viewport */
   cl.a.x = (float)( config.numxpixels - config.numypixels) / 2.0;
   cl.a.y = 0.0;
   cl.b.x = cl.a.x + (float)config.numypixels;
   cl.b.y = (float)config.numypixels - 1.0;


   vgaline.shared = &vgaatt;
   vgamark.shared = &vgaatt;
   vgatext.shared = &vgaatt;
   vgafill.shared = &vgaatt;
   vgaedge.shared = &vgaatt;

}


/*** Set local default attribute ***/
void VGAsetdef( void )
{

  /********* COLOR ************/
   Ar = (Float)(VGAREDMAX - VGAREDMIN)/
        (Float)(cur.max_rgb.red - cur.min_rgb.red);
   Ab = (Float)(VGABLUEMAX - VGABLUEMIN)/
        (Float)(cur.max_rgb.blue - cur.min_rgb.blue);
   Ag = (Float)(VGAGREENMAX - VGAGREENMIN )/
        (Float)(cur.max_rgb.green - cur.min_rgb.green);

   Br = (Float)VGAREDMAX   - Ar * (Float)cur.max_rgb.red;
   Bb = (Float)VGABLUEMAX  - Ab * (Float)cur.max_rgb.blue;
   Bg = (Float)VGAGREENMAX - Ag * (Float)cur.max_rgb.green;


  if ( cur.color_mode==INDEXED )
  {  int i;
     coltbsize = COLTBSIZE;
     vgacolrtb = (rgbcolor *)malloc( coltbsize * sizeof( rgbcolor ) );
     /* copy default color table to local color table */
     memmove( vgacolrtb, defcolrtb, sizeof( defcolrtb ) );

     /* Set hardware palette */
     for (i=0; i<nc; i++)
     {
        unsigned long red, green, blue, color;
        red   = (unsigned long)RGBVAL( defcolrtb[i].r, red);
        green = (unsigned long)RGBVAL( defcolrtb[i].g, green);
        blue  = (unsigned long)RGBVAL( defcolrtb[i].b, blue);
        color = red | ( green<<8 ) | ( blue<<16 );
        _remappalette(i,color);
     }

  }
  else
  {
     entry = 1; /* color entry */
     coltbsize = nc;
     vgacolrtb = (rgbcolor *)malloc( nc * sizeof( rgbcolor ) );
  }

  setclip();
  VGAwindow();

  vgacolrtb[0].r =(unsigned short)cur.back.red;
  vgacolrtb[0].g =(unsigned short)cur.back.green;
  vgacolrtb[0].b =(unsigned short)cur.back.blue;
  _setbkcolor( (long)( (long)RGBVAL(cur.back.red,red)|
                     ( (long)RGBVAL(cur.back.green,green) <<8) |
                     ( (long)RGBVAL(cur.back.blue,blue) <<16)));

  _clearscreen(_GVIEWPORT);

}


void pause( void )
{
   while ( !kbhit() ); getch();
}

void setclip()
{

  if ( cur.clip_ind == ON )
  {
#ifdef DEBUG_CLIP
       printf(" Clipping is ON\n");
       pause();
#endif
    if (cur.vdc_type==REAL)
    {
        vgaclip.a.x = (Float)cur.clip_rect.a.x.real;
        vgaclip.a.y = (Float)cur.clip_rect.a.y.real;
        vgaclip.b.x = (Float)cur.clip_rect.b.x.real;
        vgaclip.b.y = (Float)cur.clip_rect.b.y.real;
    }
    else
    {
        vgaclip.a.x = (Float)cur.clip_rect.a.x.intr;
        vgaclip.a.y = (Float)cur.clip_rect.a.y.intr;
        vgaclip.b.x = (Float)cur.clip_rect.b.x.intr;
        vgaclip.b.y = (Float)cur.clip_rect.b.y.intr;
    }
  }
  else  /* cur.clip_ind == OFF */
  {
#ifdef DEBUG_CLIP
       printf(" Clipping is OFF\n");
       pause();
#endif
     if ( cur.vdc_type == REAL )
     {
         vgaclip.a.x = (Float)cur.vdc_extent.a.x.real;
         vgaclip.a.y = (Float)cur.vdc_extent.a.y.real;
         vgaclip.b.x = (Float)cur.vdc_extent.b.x.real;
         vgaclip.b.y = (Float)cur.vdc_extent.b.y.real;
     }
     else
    {
         vgaclip.a.x = (Float) cur.vdc_extent.a.x.intr;
         vgaclip.a.y = (Float) cur.vdc_extent.a.y.intr;
         vgaclip.b.x = (Float) cur.vdc_extent.b.x.intr;
         vgaclip.b.y = (Float) cur.vdc_extent.b.y.intr;
    }
  }
#ifdef DEBUG_CLIP
       printf(" Clip_rect a.x:%f a.y:%f b.x:%f b.y:%f\n",vgaclip.a.x,
                vgaclip.a.y, vgaclip.b.x, vgaclip.b.y );
       pause();
#endif
}

void VGAwindow( void )
{
    Px1 = cl.a.x;  Px2 = cl.b.x;
    Py1 = cl.b.y;  Py2 = cl.a.y;

    if ( cur.vdc_type == REAL )
    {
       Wx1 = cur.vdc_extent.a.x.real;
       Wx2 = cur.vdc_extent.b.x.real;
       Wy1 = cur.vdc_extent.a.y.real;
       Wy2 = cur.vdc_extent.b.y.real;
    }
    else
    {
       Wx1 = (Float) cur.vdc_extent.a.x.intr;
       Wx2 = (Float) cur.vdc_extent.b.x.intr;
       Wy1 = (Float) cur.vdc_extent.a.y.intr;
       Wy2 = (Float) cur.vdc_extent.b.y.intr;
    }

    /* coefficence for Screen mapping */
    A = (Float)( Px1 - Px2 )/( Wx1 - Wx2 );
    B = ( Wx1 * (Float)Px2 - Wx2 * (Float)Px1 ) / ( Wx1 - Wx2 );
    C = (Float)( Py1 - Py2 )/( Wy1 - Wy2 );
    D = ( Wy1 * (Float)Py2 - Wy2 * (Float)Py1 ) / ( Wy1 - Wy2 );

    vgaatt.xgrain = A;
    vgaatt.ygrain = C;
    if ( A > C ) wvrat = A;
    else wvrat = C;

}



/***************************************************** VGAnewpic *******/

void VGAnewpic(char *pic_name)

/*   New Frame processing started at first drawing element  */
{
   newpic=TRUE;

   return;
}

/***************************************************** VGAmenu *********/

void VGAmenu( void )

/*  End of frame processing to select options */

{
   pause();
   _clearscreen(_GVIEWPORT);
   return;
}

/***************************************************** VGAclose ********/

void VGAclose( void )

/*  Close down the Graphics  */
{
   pause();
   _setvideomode(_DEFAULTMODE); /* Set to defaultmode */

   return;
}

/***************************************************** VGAline *********/

void VGAline ( Long n, Long *pi, Float *pr , Code type, Enum vdctype)
/*   draws n point polyline starting at pi/pr  */
{
   /* Line style need implement */

   double w;  /* line width */
   Double dx, dy, ddx, ddy, lw, Beta=0.0, len=0.0;
   Double seglen=0.0, dotspace=2.0;
   Point a, b, c, d, p1, p2;
   short finish = 0;
   unsigned short m, style;

   register Long i;

   if ( n<2 ) return; /* not a line or edge */

    switch ( type )
    {
      case LINE:
            if ( vgaline.widthmode == SCALED )
                 w = vgaline.width / wvrat; /* window scaled */
            else w = vgaline.width;

            style = m = linestyle[ vgaline.type ];

           break;

      case EDGEVIS:
            if ( vgaedge.widthmode == SCALED )
                 w = vgaedge.width / wvrat; /* window scaled */
            else w = vgaedge.width;

            style = m = edgestyle[ vgaedge.type ];

           break;
    }

    seglen = dotspace / wvrat; /* Convert to window coordinate */

  for ( i = 0; i <= n-2; i++ )
  {
    if ( vdctype == INTEGER )
    {
       p1.x = (Double) pi[ i*2 ]; /* Start point */
       p1.y = (Double) pi[ i*2+1 ];
       p2.x = (Double) pi[ i*2+2 ]; /* End   point */
       p2.y = (Double) pi[ i*2+3 ];
    }
    else /* REAL vdc */
    {
       p1.x= (Double)pr[ i*2   ]; /* Start point */
       p1.y= (Double)pr[ i*2+1 ];
       p2.x= (Double)pr[ i*2+2 ]; /* End   point */
       p2.y= (Double)pr[ i*2+3 ];
    }

    /* find the length of line */
    lw = sqrt(( p1.x - p2.x ) * ( p1.x-p2.x ) +
               ( p1.y - p2.y ) * ( p1.y-p2.y ));

    if ( ! REQUAL(lw, 0.0) )
           Beta= atan2( p2.y-p1.y, p2.x-p1.x ); /* evlation angle */
    else Beta = 0.0;

    a.x = ( w/2.0 ) * cos( Beta + PI / 2.0 ) + p1.x;
    a.y = ( w/2.0 ) * sin( Beta + PI / 2.0)  +p1.y;
    d.x = ( w/2.0 ) * cos( Beta + 3 * PI / 2.0 ) +p1.x;
    d.y = ( w/2.0 ) * sin( Beta + 3 * PI / 2.0 ) +p1.y;

    dx = seglen * cos( Beta ); /* seg width */
    dy = seglen * sin( Beta );
    ddx = d.x-a.x;  /* linewidth */
    ddy = d.y-a.y;

   finish=0;
   len=0;
   while ( !finish )
   {
     len += seglen;
     if ( fabs( len ) > lw )
     {
        finish = 1;
        b.x = p2.x; /* end points of the line */
        b.y = p2.y;
     }
     else
     {
        b.x = a.x + dx;
        b.y = a.y + dy;
     }

     if ( m & 0x0001 == 0x0001 ) /* mask with line style */
     {
          Float p[8];

          p[ 0 ] = a.x;       p[ 1 ] = a.y;
          p[ 2 ] = b.x;       p[ 3 ] = b.y;
          p[ 4 ] = b.x + ddx; p[ 5 ] = b.y + ddy;
          p[ 6 ] = a.x + ddx; p[ 7 ] = a.y + ddy;

#ifdef DEBUG_LINE
       printf(" The four points of the line segment are:\n");
       for (i=0; i<4; i++)
           printf(" ( %f, %f )\n",p[ 2*i ], p[ 2*i+1 ]);
       pause();
#endif

          VGAfill ( 4L , pi, p , REAL );
      }
     m = _rotr( m, 1 ); /* rotate m 1 bit to right */
     a = b;
   }

  }
  return;
}

/***************************************************** VGAmarker *******/

void VGAmarker ( Long n, Long *pi, Float *pr)

/*   draws n markers starting at pi/pr  */
{
  Long i;
  Double x,y,msize;

  /* find the length in world coordinate system */
  if ( vgamark.sizemode == SCALED )
       msize = (Double)vgamark.size * 3.0/(A*2.0);
  else
     msize=(Double)(vgamark.size/2.0);

  for ( i=0; i<n; i++ )
  {
     if ( cur.vdc_type == REAL )
     {
        x = pr[2*i];
        y = pr[2*i+1];
     }
     else
     {
        x = (Double)pi[2*i];
        y = (Double)pi[2*i+1];
     }

     switch (vgamark.type)
     {
        case 2:        /* "*" */
        case 3:        /* "+" */
           _moveto_w( XPC( x-msize ) ,YPC( y ) );
           _lineto_w( XPC( x+msize ) ,YPC( y ) );
           _moveto_w( XPC( x ) , YPC( y+msize ) );
           _lineto_w( XPC( x ) , YPC( y-msize ) );
           if ( vgamark.type == 2 ) break;

        case 5: /* "X" */
           _moveto_w( XPC( x-msize ), YPC( y+msize ) );
           _lineto_w( XPC( x+msize ), YPC( y-msize ) );
           _moveto_w( XPC( x-msize ), YPC( y-msize ) );
           _lineto_w( XPC( x+msize ), YPC( y+msize ) );
           break;

        case 1: /* "." */
           _setpixel_w(XPC(x),YPC(y));
           break;

        case 4: /* "O" */
           _ellipse_w( _GBORDER, XPC(x-msize), YPC(y+msize),
                                 XPC(x+msize), YPC(y-msize));
           break;

     }
 }
return;
}

/***************************************************** VGAtext *********/

void VGAtext (Textitem *txt )
/*  draws TEXT according to current settings */
{
   Textitem *curtxt = txt;
   TXTalign ( txt, txtpos, TXTgwidth(txt), TXTgheight(txt) );
   while ( curtxt != NULL )
   {
      switch ( curtxt->text_method )
      {
        case HER:
#ifdef HERSHEY
        HERtext( curtxt, txtpos, VGAtxline );
        break;
#endif

        case BEZ:
#ifdef BEZIER
        BEZtext( curtxt, &vgatext, txtpos, VGAtxfill);
        break;
#endif
         case TEX: /* not support yet ! */
         case HDW: /* not support yet ! */
         default:  ;;
      }
      curtxt = curtxt->next;
   }

   TXTfree(txt);

   return;
}

/***************************************************** VGAtxline *******/

void VGAtxline ( Int np, Point *pt, Tmatrix matrix, int newchar)
{
  Int i=0;
  int firstp; /* first point ? */

  firstp = TRUE;
  for (i=0;i<np;i++)
  {
     double x,y;
     x= (double)( pt[i].x * (float)matrix[0][0] +
                  pt[i].y * (float)matrix[1][0] +
                            (float)matrix[2][0] );
     y= (double)( pt[i].x * (float)matrix[0][1] +
                  pt[i].y * (float)matrix[1][1] +
                            (float)matrix[2][1] );
     if (firstp)
     {
       _moveto_w( XPC(x), YPC(y) );
       firstp = FALSE;
     }
     else
      /* connect points together */
      _lineto_w( XPC( x ), YPC( y ));
  }

}

/***************************************************** VGAtxfill *******/

void VGAtxfill ( Int np, Point *pt, Tmatrix matrix, int newchar)
{
  Int i;
  Float *p;
  Long *dummy;
  int firstp;

  p = ( Float * )malloc( ( 2* np * sizeof(Float) + 2) );
  if ( p==NULL ) CGMerror (func, ERR_NOMEMORY, FATAL, NULLSTR);

  for (i=0;i<np;i++)
  {
     p[ 2*i ] = (Float)( pt[i].x *(Float)matrix[0][0] +
                         pt[i].y *(Float)matrix[1][0] +
                                  (Float)matrix[2][0] );

     p[2*i+1] = (Float)( pt[i].x *(Float)matrix[0][1] +
                         pt[i].y *(Float)matrix[1][1] +
                                  (Float)matrix[2][1] );
  }

 #ifdef DEBUG_TXTF
       printf(" FILL TEXT with %d points\n",np);
       for (i=0; i<np; i++)
           printf(" ( %f, %f )\n", p[2*i], p[2*i+1]);
       pause();
 #endif

  VGAfill ( (Long)np , dummy, p , REAL );
  free(p);

}




/***************************************************** VGAcells ********/
void VGAcells ( Long num, Long *pi )

/*  Draws a Cell array of n values 'pi' */
{
  double *pt;
  float addx, addy, bddx, bddy;
  register int i,j;
  int c=0;

  addx = (cell.r.x-cell.p.x) / (float)cell.nx;
  addy = (cell.r.y-cell.p.y) / (float)cell.nx;
  bddx = (cell.q.x-cell.r.x) / (float)cell.ny;
  bddy = (cell.q.y-cell.r.y) / (float)cell.ny;

    pt = (double *)malloc( 8 * sizeof(double) );
    if (pt == NULL) CGMerror ("VGAcells", ERR_NOMEMORY, FATAL, NULLSTR);

  for ( j=1; j <= (int)cell.ny; j++)
  {
    for (i=1;i<=(int)cell.nx;i++)
    {
       pt[0] = cell.p.x + (j-1) * bddx + (i-1) * addx;
       pt[1] = cell.p.y + (j-1) * bddy + (i-1) * addy;
       pt[2] = pt[0] + addx;
       pt[3] = pt[1] + addy;
       pt[4] = pt[2] + bddx;
       pt[5] = pt[3] + bddy;
       pt[6] = pt[0] + bddx;
       pt[7] = pt[1] + bddy;
       if ( cur.color_mode == DIRECT )
       {
          setcolor( 0, (short)pi[0], (short)pi[1], (short)pi[2] );
          pi += 3;
       }
       else
          setcolor((Index)*pi++,0,0,0);

       _polygon_w(_GFILLINTERIOR,pt,4);
    }
  }
  free(pt);
  return;
}
/***************************************************** VGAgdp **********/

void VGAgdp ( Code type, Long *pi, Float *pr, Enum close )

/*  draws GDP 'type' with points starting at pi/pr
    for convenience CGM primitives are handled as GDPs  */
{
   Float *p;
   Long j, np, i;
   Logical fill, edge, GKSgdp, isarc, centreknown = FALSE;
   Point p1, p2, p3, p4, p5, centre, cdp1, cdp2;
   Double rad=0.0, ang, ang1, ang2;
   Enum vdctype;
   Lpoint *pt = localpts;

   centre.x = 0.0;
   centre.y = 0.0;
   ang = 0.0;

/*  work out if a RAL-GKS GDP  */

   GKSgdp = ( type == GKSCIRCLE || type == GKSARC ||
              type == GKSPIE    || type == GKSCHORD );

/*  work out if this is an arc  */

   isarc = ( type == GKSARC || type == ARC3PT ||
             type == ARCCTR || type == ELLIPARC );
/*  decide whether to draw fill area, edge or both and set attributes  */

   fill = ( ! isarc && curatt.int_style != EMPTY
                    && curatt.int_style != HOLLOW );
   edge = ( curatt.edge_vis == ON || curatt.int_style == HOLLOW );

  /* make sure area is visible */

   if ( ! (fill || edge || isarc) ) return;

  /*  At present ignore transformation of GKS GDPs */

   if (GKSgdp)
   {
       pi += 6; pr += 6;
   }

/*  get 5 points and convert to real values */

   if (cur.vdc_type == REAL)
   {
      p1.x = *pr++;  p1.y = *pr++;
      p2.x = *pr++;  p2.y = *pr++;
      p3.x = *pr++;  p3.y = *pr++;
      p4.x = *pr++;  p4.y = *pr++;
      p5.x = *pr++;  p5.y = *pr++;
   }
   else
   {
      p1.x =  *pi++; p1.y = *pi++;
      p2.x =  *pi++; p2.y = *pi++;
      p3.x =  *pi++; p3.y = *pi++;
      p4.x =  *pi++; p4.y = *pi++;
      p5.x =  *pi++; p5.y = *pi++;
   }

/*  Now work out significant points  */

   switch ( type )
   {
      case ARCCTR:
      case ARCCTRCLOSE:
            centre = p1;
            rad = p4.x;
            GDParc ( &centre, (double)rad, &p2, &p3, &ang, &p1, &p3 );
            centreknown = TRUE;

      case GKSARC:
      case GKSCHORD:
      case GKSPIE:
      case ARC3PT:
      case ARC3PTCLOSE:
            if ( ! centreknown )
                  GDPcentre ( &centre, &rad, &ang, &p1, &p2, &p3);

            if ( REQUAL( ang, 0.0 ) )
            {
               /* Points are co-linear        */
               p2 = p3;
               type = LINE;

               break;
            }

            /* work out points */
            GDPcircle ( &centre, (Double)rad, (Double)ang,
                        &p1, (Double)wvrat, &np, pt, close );

           break;

      case GKSCIRCLE:
           centre = p1;
           rad = sqrt(( p1.x - p2.x ) * ( p1.x - p2.x ) +
                      ( p1.y - p2.y ) * ( p1.y - p2.y ) );
           GDPcircle ( &centre, (Double)rad, (Double)(2.0 * PI),
                       &p2, (Double)wvrat, &np, pt, close );
           break;

      case CIRCLE:
            centre = p1;
            rad = p2.x;
            p1.x += rad;
            GDPcircle ( &centre, (Double)rad, (Double)( 2 * PI ),
                        &p1, (Double)wvrat, &np, pt, close );
           break;


      case RECT:
           break;

      case ELLIPSE:
           centre = p1;
           cdp1 = p2; cdp2 = p3;
           p4.x = 1.0; p4.y = 0.0;
           GDPellipse( &centre, &cdp1, &cdp2, &p4, &p4, (Double)wvrat,
                        &np, pt, close );
           break;

      case ELLIPARC:
      case ELLIPARCCLOSE:
           centre = p1;
           cdp1 = p2;        cdp2 = p3;
           if ( VEQUAL(p2.x * p3.y - p3.x * p2.y, 0.0) )
           {
              /* Points are co-linear        */
              p1 = p3;
              type = LINE;
              break;
           }
           GDPellipse( &centre, &cdp1, &cdp2, &p4, &p5, (Double)wvrat,
                        &np, pt, close);
           break;

      default:
           break;
   }

/* now draw primitives  */

   switch ( type )
   {
      case SOFTWARE:
      break;

      case CIRCLE:
      case ARC3PT:
      case ELLIPSE:
      case ARCCTR:
      case ELLIPARC:
           p = (Float *)malloc( 2*(size_t)(np-2)*sizeof(Float) );
           if ( p == NULL )
                CGMerror ("VGAgdp", ERR_NOMEMORY, FATAL, NULLSTR);

           /* 1st and last pts are the centre, so ignore them */
           for (i=1; i<np-1; i++)
           {
              p[ 2*i-2 ] = pt[i].x;
              p[ 2*i-1 ] = pt[i].y;
           }
           np -= 2;
           break;

      case ARC3PTCLOSE:
      case ARCCTRCLOSE:
      case ELLIPARCCLOSE:
           p = (Float *)malloc( 2*(size_t)np*sizeof(Float) );
           if ( p == NULL )
                CGMerror ("VGAgdp", ERR_NOMEMORY, FATAL, NULLSTR);

           for (i=0; i<np; i++)
           {
              p[ 2*i ]   = pt[i].x;
              p[ 2*i+1 ] = pt[i].y;
           }
           break;

      case LINE:    /*         Points are co-linear  */
           p = (Float *)malloc( 4*sizeof(Float) );
           if ( p == NULL)
                CGMerror ("VGAgdp", ERR_NOMEMORY, FATAL, NULLSTR);

           p[0] = p1.x; p[1] = p1.y;
           p[2] = p2.x; p[3] = p2.y;
           np = 2;

           break;

      case RECT:
           p = (Float *)malloc( 10*sizeof(Float) );
           if ( p == NULL )
                CGMerror ("VGAgdp", ERR_NOMEMORY, FATAL, NULLSTR);

            p[0] = p1.x; p[1] = p1.y;
            p[2] = p2.x; p[3] = p1.y;
            p[4] = p2.x; p[5] = p2.y;
            p[6] = p1.x; p[7] = p2.y;
            p[8] = p1.x; p[9] = p1.y;
            np = 5;

            break;

      case GKSARC:
      case GKSCIRCLE:
      case GKSPIE:
      case GKSCHORD:
      default:
         break;
   }

   if ( isarc )
   {
      VGAattrib( LINE );
      VGAline( np, pi, p, LINE, REAL );
   }
   else
   {
      if ( fill )
      {
         VGAattrib( POLYGON );
         VGAfill( np, pi, p, REAL);
      }
      if ( edge )
      {
         VGAattrib( EDGEVIS );
         VGAline( np, pi, p, EDGEVIS, REAL );
      }
   }
   free(p);

   return;
}

/***************************************************** VGAfill *********/

void VGAfill ( Long np, Long *pi, Float *pr, Enum vdctype )

/* fill polygon of np points in *pi or *pr */
/* Ignore fill pattern at the moment */

{
  Long i;
  struct _wxycoord *p;

#ifdef DEBUG_FILL
    printf("Fill area No. of points %d\n",np);
    pause();
    if ( vdctype == REAL )
       for ( i=0; i<np; i++)
          printf(" Fill reference point x: %f  y: %f\n",pr[2*i],pr[2*i+1]);
    else
       for ( i=0; i<np; i++)
          printf(" Fill reference point x: %ld  y: %ld\n",pi[2*i],pi[2*i+1]);
    pause();
#endif

  if ( np < 2L ) return; /* Not a polygon */


  p =( struct _wxycoord *)malloc( (short)np*sizeof(struct _wxycoord) );
  if (p==NULL) CGMerror ("VGAfill", ERR_NOMEMORY, FATAL, NULLSTR);

  for (i=0; i<np; i++)
  {
      /* Convert points to screen coordinate */
      if ( vdctype == REAL )
      {
          p[i].wx = XPC( pr[i*2] );
          p[i].wy = YPC( pr[i*2+1]);
      }
      else
      {
          p[i].wx = XPC( pi[i*2]);
          p[i].wy = YPC( pi[i*2+1]);
      }
   }
   _polygon_wxy(_GFILLINTERIOR,p,(short)np);
   free(p);
   return;
}


/***************************************************** setcolour *******/

void setcolor( Index index,Posint red, Posint green, Posint blue)
{
  int i,j;
  long color;
  unsigned long small=0;


  if ( cur.color_mode == DIRECT )
  {
     /* look up the color table and select the appropiate color */
#ifdef DEBUG_COLOR
       printf(" RBG Intensity %d %d %d\n",(int)red,(int)green,(int)blue);
       pause();
#endif

     for (i=0; i<entry, i<nc ;i++)
     {
        if ( red   ==(Posint) vgacolrtb[i].r  &&
             green ==(Posint) vgacolrtb[i].g  &&
             blue  ==(Posint) vgacolrtb[i].b  )
        {
#ifdef DEBUG_COLOR
       printf(" %d %d %d is in the table\n",(int)red,(int)green,(int)blue);
       pause();
#endif
           _setcolor(i);
           return;
        }
      }
      /* if the color not in the table and 'hardware palette '
         is not full */
      if ( entry<nc )
      {
#ifdef DEBUG_COLOR
       printf(" %d %d %d will stored in table\n",(int)red,(int)green,
                (int)blue);
       pause();
#endif
        color =   RGBVAL(red, red)|
                ( RGBVAL(green, green)<<8 )|
                ( RGBVAL(blue, blue)<<16);
        _remappalette(entry,color);
        _setcolor(entry);
        entry++;
        return;
      }
  }

  if ( cur.color_mode == INDEXED )
  {
#ifdef DEBUG_COLOR
       printf(" Color index: %d\n",(int)index);
       pause();
#endif
     if (index<(Index)nc)
     {
        _setcolor((short)index);
        return;
     }
     else
     {
        red   =(Posint) vgacolrtb[index].r;
        green =(Posint) vgacolrtb[index].g;
        blue  =(Posint) vgacolrtb[index].b;
     }
  }

 /*******   Match to nearest Colour        ******/

#ifdef DEBUG_COLOR
       printf(" Match ( %d, %d, %d )\n",(int)red,(int)green,(int)blue);
       pause();
#endif

     small =(unsigned long)( (long)(red - (Posint)vgacolrtb[0].r) *
                             (long)(red - (Posint)vgacolrtb[0].r) +
                             (long)(green - (Posint)vgacolrtb[0].g) *
                             (long)(green - (Posint)vgacolrtb[0].g) +
                             (long)(blue  - (Posint)vgacolrtb[0].b) *
                             (long)(blue  - (Posint)vgacolrtb[0].b) );


     for (i=0;i<nc;i++)
     {

        unsigned long temp;

        temp =(unsigned long)( (long)(red   - (Posint)vgacolrtb[i].r) *
                               (long)(red   - (Posint)vgacolrtb[i].r) +
                               (long)(green - (Posint)vgacolrtb[i].g) *
                               (long)(green - (Posint)vgacolrtb[i].g) +
                               (long)(blue  - (Posint)vgacolrtb[i].b) *
                               (long)(blue  - (Posint)vgacolrtb[i].b) );
        if (small>temp) { small=temp; j=i;}
     }
#ifdef DEBUG_COLOR
       printf(" The nearest color is ( %d, %d, %d )\n", (int)vgacolrtb[j].r,
                (int)vgacolrtb[j].g, (int)vgacolrtb[j].b);
       pause();
#endif

     _setcolor(j);
     return;
}
