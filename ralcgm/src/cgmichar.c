/*   RAL-CGM Interpreter module:  @(#) cgmichar.c  version 3.4
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
 * Arguments:   None
 *
 * Maintenance Log:
 *   3 Oct 88 RTP  Original version stabilised
 *   7 Nov 88 RTP  Handle text states correctly
 *  15 Nov 88 RTP  Handle Encoding defaults
 *  17 Nov 88 RTP  Use typedefs to avoid 'int'
 *  18 Nov 88 RTP  Allow Native coding input
 *   9 Jan 89 RTP  Move output to seperate function
 *  24 Jan 89 RTP  Change handling of Control elements
 *  26 Jan 89 RTP  Handle GKS1.11 VDC precisions
 *  21 Feb 89 RTP  Handle Character substitution
 *  22 Feb 89 RTP  Assume first opcode has been read
 *   8 May 89 RTP  Input file rewound so opcode not read
 *  11 Jul 89 RTP  Random access to frames
 *  15 Aug 89 RTP  Tidy up ASFs
 *  17 Aug 89 RTP  Include ANSI prototyping
 *  18 Aug 89 RTP  Change MICnextbyte to CGMnextbyte
 *  22 Sep 89 RTP  Add lookup table for MICreal
 *  26 Sep 89 RTP  Change Character substitution method
 *   9 Oct 89 RTP  Change Comarea to use arrays pi & pr
 *  21 Mar 90 RTP  Remove ENDMFDEFAULTS from mfelemlist
 *  25 Mar 90 RTP  Change CGMerror macro to set micerror
 *                 Check for insufficient points in LINE etc
 *  31 Jul 90 RTP  Add Variable Buffer handling
 *                 Runlength Cell Arrays over a buffer
 *                 boundary are now handled correctly
 *                 Delete special handling of GKS 1.11 CGMs
 *   7 Aug 90 RTP  Add flags for VDC dependent defaults
 *                 Check if they have changed at BEGPICBODY
 *  16 Aug 90 RTP  Add error handler routine
 *   6 Sep 90 RTP  Change internal routines to static
 *   9 Sep 90 RTP  Change parameters in FNT*list to include
 *                 number of strings
 *  19 Sep 90 RTP  Remove unused variables and clear lint problems
 *   4 Oct 90 RTP  Change typedefs to Int, Long and Float
 *  30 Oct 90 RTP  Add check for RAL-GKS 1.11 CGMs
 *  15 Mar 91 RTP  Change cgmstate to Enum type
 *  14 May 91 RTP  Add ANSI declarations
 *  24 May 91 RTP  Move CGMnextbyte from cgmrand.c and call MICnextbyte
 *   2 Aug 91 RTP  Add SCCS id
 *  23 Sep 91 RTP  Add default colour check
 *  17 Jul 92 RTP  Correct Character substitution code to handle Uniras CGMs
 *   3 Aug 92 RTP  Correct buffer check for Direct colour Runlength Cell Arrays
 *   7 Aug 92 RTP  Correct character substitution for ESC
 *  12 Nov 92 RTP  Add cliprect_flag
 *   9 Feb 94 RTP  Change 1<<n to 1L<<n where necessary
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *  20 Jul 94 KEVP Use pow function for converting very high (vdc)real precisions.
 *  12 Aug 94 KEVP Enable interpreter to finish with a return rather than an exit
 *  17 Oct 94 KEVP Enable interpreter to finish with return within Metafile Defaults
 *                 Replacement.
 *  19 Oct 94 KEVP Interpret Auxilliary Colour according to Current Colour Mode.
 *  30 Nov 94 KEVP Corrected test to determine use of pow function (see 20 Jul 94)
 *   1 Mar 95 KEVP Allow for FONTLIST overrunning the next BEGPIC element
 *                 when at end of metafile descriptor.
 *
 */

/*  Standard include file */

#include "cgmin.h"

/*   Output drivers  */

#ifdef PROTO
   extern void CGMoutput( Code ),
               CGMOtext( FILE *, Code );
   extern Code CGMframe( Code );
#else
   extern void CGMoutput(),      /* Output Routine */
               CGMOtext();    /* Clear Text Output Routine */
   extern Code CGMframe();    /* Output Routine */
#endif

/*  Files used  */

extern FILE *cgmi;

/*  Routines in this Module  */

#ifdef PROTO
   void CGMIchar( void );
   static void MICexternal( Code ),
               MICstring( char* ),
               MICvdc( Int, Long*, Float* );
   static Code MICmfdesc( Code c ),
               MICpdesc( Code c, Logical single ),
               MICpbody( Code c, Logical single ),
               MICtext( Code ),
               MICnextbyte( void ),
               MICopcode( Logical ignore );
   static Long MICint( Code byte, Logical allowed, Logical *present ),
               MICcol( Code byte, struct colour *c,
                       Enum type, Prec prec ),
               MICcells( Code *c, Long *pi, Long nx, Long ny,
                       Enum mode, Prec prec ),
               MICpoints( Code *c, Long *pi, Float *pr, Logical set ),
               MICbit( Code *byte, Code *bit, Prec prec );
   static double MICreal( Code byte, Prec defexp, Prec *points, Prec allowed );
#else
   void CGMIchar();     /* Main input routine */
   static void MICexternal(),  /* External elements */
               MICstring(),    /* Input a text string */
               MICvdc();       /* Input n VDCs */
   static Code MICmfdesc(),    /* Metafile Descriptor elements */
               MICpdesc(),     /* Picture Descriptor elements */
               MICpbody(),     /* Picture body elements */
               MICtext(),      /* Input text elemants */
               MICnextbyte(),  /* Get next byte from input */
               MICopcode();    /* Get a CGM element code */
   static Long MICint();       /* Get an Integer */
   static Long MICcol(),       /* Get a colour */
               MICcells(),     /* Get a colour list */
               MICpoints(),    /* Get a points list */
               MICbit();       /* Get the next bit for bitstreams */
   static double MICreal();    /* Get a real value */
#endif

/*  Macros to get next byte/ next opcode */

#define NEXTBYTE(c)    ( c = MICnextbyte() )
#define NEXTCODE(c)    ( c ? c : (c = MICopcode(TRUE)) )

#define NEXTINT(x)     MICint(x, ZERO, (Logical *) NULL)
#define NEXTREAL(y,z)  MICreal(y, curichar.z.defexp, (Prec *) NULL, \
                               curichar.z.expald )

/*  Internal Encoding variables */

static Logical micerror = FALSE;
Logical miccharsub = FALSE , micsubchar[CHARSUBNUM];

static char *func = "CGMichar", mess[40];
static Logical RALGKS111 = FALSE;

/***************************************************** CGMIchar ********/

#ifdef PROTO
void CGMIchar ( void )
#else
void CGMIchar ()
#endif

{
   Code c = ZERO;

   while ( TRUE )
   {
            /*     Metafile Descriptor Elements   */

      (void) NEXTCODE(c);
      c = MICmfdesc ( c );
      if ( cgmEOF ) break;

      while ( NEXTCODE(c) != ENDMF)   /* Until End of Metafile  */
      {
            /*    Picture Descriptor Elements  */

         c = MICpdesc(c, MULTIPLE);
         if(cgmfinished) break;

            /*    Picture Body   */

         c = MICpbody (c, MULTIPLE);
         if(cgmfinished) break;
      }
      if(cgmfinished) break;
   }
   return;
}

/****************************************************** MICmfdesc ******/

#ifdef PROTO
static Code MICmfdesc ( Code code )
#else
static Code MICmfdesc ( code )

Code code;
#endif

   /*   Metafile Descriptor elements  */

{
   Code c;

   while ( NEXTCODE(code) != BEGPIC)
   {
      register Long j, n, *pi = pint+1;

      c = ZERO;
      micerror = FALSE;

#ifdef DEBUG
      DMESS " MF Desc: %x\n", code);
#endif

      switch (code)
      {
         case BEGMF:        /*  Begin Metafile   */
            if ( cgmstate != MF_CLOSED )
            {
               micerror = TRUE;
               (void) CGMerror(func, ERR_MFOPEN, ERROR, NULLSTR);
               break;
            }

            for ( j = ZERO; j < CHARSUBNUM; j++) micsubchar[j] = FALSE;
            miccharsub = FALSE;

            if ( ( c = MICopcode(FALSE) ) != st_term )
            {
#ifdef DEBUG
   DMESS " Character Substitution:\n" );
#endif
/*
               NEXTBYTE(c);
               c = ( c < PARABIT ? c + PARABIT : c - PARABIT);
*/
               while ( TRUE )
               {
                  if ( c == 0x1b || (micsubchar[0x1b] && c == 0x5b) )
                  {
                     NEXTBYTE(c);
                     if ( c == 0x5c ) break;
                     else continue;
                  }
#ifdef DEBUG
   DMESS " %02x", c );
#endif
                  if ( c == 0x7e )
                  {
                     NEXTBYTE(c);
                     continue;
                  }

                  c = ( c < PARABIT ? c + PARABIT : c - PARABIT);
#ifdef DEBUG
   DMESS "=%02x\n", c );
#endif
                  if ( c <= 0x20 || c == 0x7f )
                  {
                     if ( c == 0x7f ) c = 0x21;
                     micsubchar[c] = TRUE;
                     miccharsub = TRUE;
                  }
                  NEXTBYTE(c);
               }
            }

/*  Set standard defaults for all structures */

            CGMdefaults( BEGMF, CHARACTER );

/*  change CGM state  */

            c = ZERO;
            cgmstate = MF_DESC;
            MICstring(str);
            break;

         case MFDESC:       /*  Metafile Description  */
            MICstring(str);

/*  Check if old metafile */

            RALGKS111 =  ( strncmp(str, "RAL GKS 1.11", 12) == 0 ) ||
                         ( strncmp(str, " RAL GKS 7.4", 12) == 0 );
            break;


         case EOF:           /*  End of File */
            break;

         case ENDMF:         /*  End Metafile   */
            cgmstate = MF_CLOSED;

/* Reset Character substituion */
            miccharsub = FALSE;

            break;

         case MFVERSION:         /*  Metafile version   */
            cgmverno = *pint = NEXTINT(ZERO);
            if ( cgmverno > CGMVERSION )
            {
               micerror = TRUE;
               exit( CGMerror(func, ERR_VERSION, FATAL, NULLSTR) );
            }
            break;

         case VDCTYPE:      /*  VDC Type   */
            cur.vdc_type = NEXTINT(ZERO);
            break;

         case INTEGERPREC:  /*  Integer Precision  */
            cur.int_bits = curichar.int_prec = NEXTINT(ZERO);
            break;

         case INDEXPREC:    /*  Index Precision   */
            cur.index_bits = curichar.index_prec = NEXTINT(ZERO);
            break;

         case COLRPREC:     /*  Colour Precision  */
            cur.col_bits = curichar.col_prec = NEXTINT(ZERO);
            cur.colprec_flag = TRUE;
            break;

         case COLRINDEXPREC: /*  Colour Index Precision  */
            cur.colind_bits = curichar.colind_prec = NEXTINT(ZERO);
            break;

         case MAXCOLRINDEX:  /*  Maximum Colour Index  */
            cur.max_colind = NEXTINT(ZERO);
            break;

         case CHARCODING:    /*  Character coding Announcer  */
            *pint = NEXTINT(ZERO);
            break;

         case REALPREC:      /*  Real Precision   */
            cur.real_bits = curichar.real.prec = NEXTINT(ZERO);
            cur.real_places = curichar.real.min = NEXTINT(ZERO);
            if(abs(curichar.real.min) < sizeof(long)<<3)
            {
               cur.realmin = ( curichar.real.min < ZERO ?
               1.0 / ( 1L<<(-curichar.real.min) ) :
               (Float) ( 1L<<curichar.real.min ) );
       }
            else cur.realmin = pow((double)2,(double)curichar.real.min);
            if(curichar.real.prec < sizeof(long))
             cur.max_real =(float) ( 1L<<curichar.real.prec ) - 1;
            else  cur.max_real = pow((double)2,(double)curichar.real.prec) - 1;
            cur.min_real = - cur.max_real;
            cur.real_defexp = curichar.real.defexp = NEXTINT(ZERO);
            cur.real_expald = curichar.real.expald = NEXTINT(ZERO);
            break;

         case COLRVALUEEXT:  /*  Colour value extent  */
            (void) MICcol(ZERO, &cur.min_rgb, DIRECT, curichar.col_prec);
            (void) MICcol(ZERO, &cur.max_rgb, DIRECT, curichar.col_prec);
            curichar.min_rgb = cur.min_rgb;
            curichar.max_rgb = cur.max_rgb;
            cur.colval_flag = TRUE;
            break;

         case MFELEMLIST:     /*  Metafile element List  */
            cgmstate = MF_ELEMLIST;
            n = ZERO;
            while ( ( c = MICopcode(FALSE) ) != st_term )
            {
               if ( c == st_start ) continue;
               if ( c == ENDMFDEFAULTS ) continue;
               *pi++ = (c & PARABIT ? NEXTINT(c) : c );
               n++;
            }
            *pint = n;

            c = ZERO;
            cgmstate = MF_DESC;
            break;

         case BEGMFDEFAULTS:  /*  Begin Metafile defaults Replacement  */
            c = ENDMFDEFAULTS;
            cgmstate = MF_DEFAULTS;
            break;

         case ENDMFDEFAULTS:  /*  End Metafile defaults Replacement  */
            while ( NEXTCODE(c) != ENDMFDEFAULTS)
            {
               c = ((c & 0xff00) == 0x3200 ? MICpdesc(c, SINGLE) :
                                             MICpbody(c, SINGLE) );
               if(cgmfinished) break;
            }
            if(cgmfinished) break;
/*  If RAL-GKS 1.11  Output VDC real precision */
            if ( RALGKS111 )
            {
               cur.vdc_bits = curichar.vdc.prec = 10;
               cur.vdc_places = curichar.vdc.min = -13;
               cur.vdcmin = ( curichar.vdc.min >= ZERO ?
                                 (float)( 1L << curichar.vdc.min ) :
                                 1.0 / ( 1L << -curichar.vdc.min ) );
               cur.max_vdc = ( 1L<<curichar.vdc.prec ) - 1;
               cur.min_vdc = - cur.max_vdc;
               curichar.vdc.defexp = -10;
               curichar.vdc.expald = 0;
               CGMoutput( VDCREALPREC );
               (void) CGMerror(func, ERR_RALGKS111, WARNING, NULLSTR);
            }
/***********************************************/
            c = ZERO;
            cgmstate = MF_DESC;
            break;

         case FONTLIST:      /*  Font List   */
            j = ZERO;
            while ( ( c = MICopcode(FALSE) ) == st_start)
            {
               MICstring(&str[j]);
               j += strlen(&str[j]) + 1;
               *pi++ = j;
            }
            *pint = j;
#ifdef FONTSYSTEM
            FNTflist(pi,pint,str,&cgmfonts);
#endif
            /* Has a BEGPIC been overrun? */
            if(cgmrandom && (c == BEGPIC)) cgmoverrun = 2;

            break;

         case CHARSETLIST:   /*  Character set list  */
            j = ZERO;
            while ( (c = MICopcode(FALSE)) & PARABIT )
            {
               char chr[2];

               *pi++ = NEXTINT(c);
               MICstring( chr );
#ifdef EBCDIC
               chr[0] = cgmascii[chr[0]];
#endif
/*  double && changed to single & in last expression - CDO  */
               sprintf( &str[j], "%d/%d", chr[0]>>4, chr[0]&0x0f );
               j += strlen(&str[j]) + 1;
               *pi++ = j;
            }
            *pint = j;
#ifdef FONTSYSTEM
            FNTclist(pi,pint,str,&cgmcsets);
#endif
            break;

         default:
            MICexternal ( code );
            break;

      }

/* Output element if no error */

      code = ( micerror ? 0 : CGMframe( code ) );
      if ( code ) break;
      code = c;
   }

   if ( code == BEGMF ) miccharsub = FALSE;

   return (code);
}

/****************************************************** MICpdesc *******/

#ifdef PROTO
static Code MICpdesc ( Code code, Logical single )
#else
static Code MICpdesc ( code, single )

Code code;
Logical single;
#endif

{
#ifdef DEBUG
   Code c;
#endif

   while ( NEXTCODE(code) != BEGPICBODY)
   {
#ifdef DEBUG
      c = ZERO;
#endif
      micerror = FALSE;

#ifdef DEBUG
      DMESS " Pic Desc: %x (%x)", code, c);
      if ( single ) DMESS " (single)");
      DMESS "/n");
#endif

      switch (code)
      {
         case BEGPIC:         /*  Begin Picture Descriptor   */
            if ( cgmstate == PIC_DESC )
            {
               micerror = TRUE;
               (void) CGMerror(func, ERR_PICOPEN, ERROR, NULLSTR);
               break;
            }
            MICstring(str);

            if ( cgmstate == MF_DESC )
               CGMdefaults ( MFDESC, CHARACTER );
            else
               CGMdefaults ( ENDPIC, CHARACTER );

            cgmstate = PIC_DESC;
            break;

         case EOF:           /*  End of File */
            break;

         case ENDPIC:
            cgmstate = PIC_CLOSED;
            return(code);

         case SCALEMODE:      /*  Scaling Mode   */
            cur.scale_mode = NEXTINT(ZERO);
            cur.scale_factor = NEXTREAL(ZERO,real);
            break;

         case COLRMODE:       /*  Colour Selection Mode */
            cur.color_mode = NEXTINT(ZERO);
            break;

         case LINEWIDTHMODE:  /*  Line width Specification  */
            cur.linewidth_mode = NEXTINT(ZERO);
            break;

         case MARKERSIZEMODE: /*  Marker size Specification  */
            cur.markersize_mode = NEXTINT(ZERO);
            break;

         case EDGEWIDTHMODE:  /*  Edge width Specification  */
            cur.edgewidth_mode = NEXTINT(ZERO);
            break;

         case VDCEXT:         /*  VDC Extent    */
            if (cur.vdc_type == REAL)
            {
               cur.vdc_extent.a.x.real = NEXTREAL(ZERO,vdc);
               cur.vdc_extent.a.y.real = NEXTREAL(ZERO,vdc);
               cur.vdc_extent.b.x.real = NEXTREAL(ZERO,vdc);
               cur.vdc_extent.b.y.real = NEXTREAL(ZERO,vdc);
            }
            else
            {
               cur.vdc_extent.a.x.intr = NEXTINT(ZERO);
               cur.vdc_extent.a.y.intr = NEXTINT(ZERO);
               cur.vdc_extent.b.x.intr = NEXTINT(ZERO);
               cur.vdc_extent.b.y.intr = NEXTINT(ZERO);
            }
            break;

         case BACKCOLR:       /*  Background Colour  */
            (void) MICcol (ZERO, &cur.back, DIRECT, curichar.col_prec);
            break;

         default:
            MICexternal ( code );
            break;

      }

/* Output element if no error */

      code = ( micerror ? 0 : CGMframe( code ) ) ;
      if ( single || cgmfinished ) break;
   }
   return (code);
}

/****************************************************** MICpbody *******/

#ifdef PROTO
static Code MICpbody ( Code code, Logical single)
#else
static Code MICpbody ( code, single )

Code code;
Logical single;
#endif

{
   Code c, cf;
   static Long nx, ny;
   static Prec loc_prec;
   Logical first = TRUE;

/*  returns if ENDPIC found in  Picture Descriptor */

   if ( cgmstate == MF_CLOSED ) return (code);

   while ( NEXTCODE(code) )
   {
      register Long n, *pi=pint+1;
      Float *pr=preal;

      c = ZERO;
      micerror = FALSE;

#ifdef DEBUG
      DMESS " Pic Body: %x", code);
      if ( !first ) DMESS " (continuation)");
      if ( single ) DMESS " (single)");
      DMESS "\n");
#endif

      switch (code)
      {
         case BEGPICBODY:      /*  Begin Picture Body  */

            if ( cgmstate == PIC_OPEN )
            {
               micerror = TRUE;
               (void) CGMerror(func, ERR_PICOPEN, ERROR, NULLSTR);
               break;
            }
            cgmstate = PIC_OPEN;
/*  Set VDC dependent defaults */
            CGMdefaults ( VDCEXT, CHARACTER );
            break;

         case EOF:           /*  End of File */
            single = TRUE;
            break;

         case ENDPIC:          /*  End  Picture    */
            cgmstate = PIC_CLOSED;
            single = TRUE;     /*  forces exit  */
            break;

         case LINE:         /*  Polyline   */
         case DISJTLINE:         /*  Disjoint Polyline  */
         case MARKER:         /*  Polymarker  */
            c = code;
            *pint = MICpoints( &c, pi, preal, NOSET );
            break;

         case POLYGON:         /*  Polygon   */
         case POLYGONSET:         /*  Polygon Set  */
            c = code;
            *pint = MICpoints (&c, pi, preal,
                              (Logical)(code == POLYGONSET) );
            break;

         case APNDTEXT:       /*  Append Text   */
            if ( cgmstate != TEXT_OPEN )
            {
               micerror = TRUE;
               (void) CGMerror(func, ERR_APNDTXT, ERROR, NULLSTR);
               break;
            }
            code = MICopcode(FALSE);
            c = MICtext (code);
            break;

         case RESTRTEXT:      /*  Restricted Text */
            MICvdc ( (Int)2, pi, preal );
            pi += 2; pr += 2;

         case TEXT:           /*  Text   */
            MICvdc ( (Int)2, pi, pr );
            *pint = NEXTINT(ZERO);
            MICstring (str);
            if ( *pint ) cgmstate = PIC_OPEN;
            else
            {
               c = APNDTEXT;
               cgmstate = TEXT_OPEN;
            }
            break;

         case CELLARRAY:      /*  Cell Array  */
            if ( first )
            {
               MICvdc ( (Int)6, pi, preal );
               pi += 6;
               *pi++ = nx = NEXTINT(ZERO);
               *pi++ = ny = NEXTINT(ZERO);
               *pi++ = loc_prec = NEXTINT(ZERO);
            }
            c = code;
            *pint = MICcells(&c, pi, nx, ny, cur.color_mode, loc_prec);
            first = ( *pint >= ZERO );
            break;

         case GDP:           /*  Generalised Drawing Primitive  */
            *pi++ = NEXTINT(ZERO);
            *pint = MICpoints (&c, pi, preal, NOSET);
            MICstring (str);
            c = ZERO;
            break;

         case RECT:          /*  Rectangle   */
            MICvdc ( (Int)4, pi, preal);
            break;

         case LINECOLR:      /*  Line Colour  */
            (void) MICcol (ZERO, &curatt.line, ANY, (Prec) ZERO);
            break;

         case MARKERCOLR:    /*  Marker Colour  */
            (void) MICcol (ZERO, &curatt.marker, ANY, (Prec) ZERO);
            break;

         case FILLCOLR:      /*  Fill Colour */
            (void) MICcol (ZERO, &curatt.fill, ANY, (Prec) ZERO);
            break;

         case EDGECOLR:     /*  Edge Colour */
            (void) MICcol (ZERO, &curatt.edge, ANY, (Prec) ZERO);
            break;

         case LINEINDEX:     /*  Line Bundle index  */
            curatt.line_ind = NEXTINT(ZERO);
            break;

         case LINETYPE:      /*  Line Type   */
            curatt.line_type = NEXTINT(ZERO);
            break;

         case MARKERINDEX:   /*  Marker Bundle index  */
            curatt.mark_ind = NEXTINT(ZERO);
            break;

         case MARKERTYPE:    /*  Marker Type   */
            curatt.mark_type = NEXTINT(ZERO);
            break;

         case TEXTPATH:      /*  Text Path */
            curatt.text_path = NEXTINT(ZERO);
            break;

         case FILLINDEX:       /*  Fill Bundle index  */
            curatt.fill_ind = NEXTINT(ZERO);
            break;

         case INTSTYLE:        /*  Interior Style  */
            curatt.int_style = NEXTINT(ZERO);
            break;

         case HATCHINDEX:      /*  Hatch Index  */
            curatt.hatch_ind = NEXTINT(ZERO);
            break;

         case PATINDEX:        /*  Pattern Index  */
            curatt.pat_ind = NEXTINT(ZERO);
            break;

         case EDGEINDEX:       /*  Edge Bundle index  */
            curatt.edge_ind = NEXTINT(ZERO);
            break;

         case EDGETYPE:        /*  Edge Type  */
            curatt.edge_type = NEXTINT(ZERO);
            break;

         case EDGEVIS:         /*  Edge Visibility  */
            curatt.edge_vis = NEXTINT(ZERO);
            break;

         case CIRCLE:         /* Circle      */
            MICvdc ( (Int)3, pi, preal);
            break;

         case ARC3PT:         /* Circular Arc  3 point */
            MICvdc ( (Int)6, pi, preal);
            break;

         case ARC3PTCLOSE:    /* Circular Arc  3 point close */
            MICvdc ( (Int)6, pi, preal);
            *(pi+6) = NEXTINT(ZERO);
            break;

         case ARCCTR:         /* Circle Arc centre */
            MICvdc ( (Int)7, pi, preal);
            break;

         case ARCCTRCLOSE:     /* Circle Arc centre close */
            MICvdc ( (Int)7, pi, preal);
            *(pi+7) = NEXTINT(ZERO);
            break;

         case ELLIPSE:         /* Ellipse    */
            MICvdc ( (Int)6, pi, preal);
            break;

         case ELLIPARC:        /* Elliptical Arc */
            MICvdc ( (Int)10, pi, preal);
            break;

         case ELLIPARCCLOSE:   /* Elliptical Arc close*/
            MICvdc ( (Int)10, pi, preal);
            *(pi+10) = NEXTINT(ZERO);
            break;

         case LINEWIDTH:       /*  Line Width */
            if (cur.linewidth_mode == SCALED)
                    curatt.line_width.real = NEXTREAL(ZERO, real);
            else
            {
               if (cur.vdc_type == REAL)
                    curatt.line_width.real = NEXTREAL(ZERO,vdc);
               else curatt.line_width.intr = NEXTINT(ZERO);
            }
            curatt.linewidth_flag = TRUE;
            break;

         case MARKERSIZE:      /*  Marker Size */
            if (cur.markersize_mode == SCALED)
                    curatt.mark_size.real = NEXTREAL(ZERO,real);
            else
            {
               if (cur.vdc_type == REAL)
                    curatt.mark_size.real = NEXTREAL(ZERO,vdc);
               else curatt.mark_size.intr = NEXTINT(ZERO);
            }
            curatt.markersize_flag = TRUE;
            break;

         case EDGEWIDTH:        /*  Edge Width  */
            if (cur.edgewidth_mode == SCALED)
                    curatt.edge_width.real = NEXTREAL(ZERO,real);
            else
            {
               if (cur.vdc_type == REAL)
                    curatt.edge_width.real = NEXTREAL(ZERO,vdc);
               else curatt.edge_width.intr = NEXTINT(ZERO);
            }
            curatt.edgewidth_flag = TRUE;
            break;

         case CHARORI:         /*  Character Orientation */
            if (cur.vdc_type == REAL)
            {
               curatt.char_up.x.real = NEXTREAL(ZERO,vdc);
               curatt.char_up.y.real = NEXTREAL(ZERO,vdc);
               curatt.char_base.x.real = NEXTREAL(ZERO,vdc);
               curatt.char_base.y.real = NEXTREAL(ZERO,vdc);
            }
            else
            {
               curatt.char_up.x.intr = NEXTINT(ZERO);
               curatt.char_up.y.intr = NEXTINT(ZERO);
               curatt.char_base.x.intr = NEXTINT(ZERO);
               curatt.char_base.y.intr = NEXTINT(ZERO);
            }
            break;

         case TEXTALIGN:       /*  Text Alignment */
            curatt.text_halign = NEXTINT(ZERO);
            curatt.text_valign = NEXTINT(ZERO);
            curatt.text_hcont  = NEXTREAL(ZERO,real);
            curatt.text_vcont  = NEXTREAL(ZERO,real);
            break;

         case PATSIZE:         /*  Pattern Size  */
            if (cur.vdc_type == REAL)
            {
               curatt.pat_size.a.x.real = NEXTREAL(ZERO,vdc);
               curatt.pat_size.a.y.real = NEXTREAL(ZERO,vdc);
               curatt.pat_size.b.x.real = NEXTREAL(ZERO,vdc);
               curatt.pat_size.b.y.real = NEXTREAL(ZERO,vdc);
            }
            else
            {
               curatt.pat_size.a.x.intr = NEXTINT(ZERO);
               curatt.pat_size.a.y.intr = NEXTINT(ZERO);
               curatt.pat_size.b.x.intr = NEXTINT(ZERO);
               curatt.pat_size.b.y.intr = NEXTINT(ZERO);
            }
            break;

         case FILLREFPT:       /*  Fill Reference Point  */
            if (cur.vdc_type == REAL)
            {
               curatt.fill_ref.x.real = NEXTREAL(ZERO,vdc);
               curatt.fill_ref.y.real = NEXTREAL(ZERO,vdc);
            }
            else
            {
               curatt.fill_ref.x.intr = NEXTINT(ZERO);
               curatt.fill_ref.y.intr = NEXTINT(ZERO);
            }
            break;

         case PATTABLE:        /*  Pattern Table  */
            if ( first )
            {
               *pi++ = NEXTINT(ZERO);  /* Pattern Index */
               *pi++ = nx = NEXTINT(ZERO);
               *pi++ = ny = NEXTINT(ZERO);
               *pi++ = loc_prec = NEXTINT(ZERO);
            }
            c = code;
            *pint = MICcells(&c, pi, nx*ny, 1L, cur.color_mode, loc_prec);
            first = ( *pint >= ZERO );
            break;

         case COLRTABLE:      /*  Colour Table  */
            if ( first )
            {
               *pi++ = NEXTINT(ZERO);
            }
            c = code;
            *pint = MICcells(&c, pi, 1L, 1L, DIRECT, curichar.col_prec);
            first = ( *pint >= ZERO );
            break;

         case ASF:           /*  Aspect source flags  */
            n = ZERO;
            while ( (c = MICopcode(FALSE) ) & PARABIT )
            {
               *pi++ = NEXTINT(c);
               *pi++ = NEXTINT(ZERO);
               n++;
            }
            *pint = n;
            CGMsetasf( pint );   /* Set current values for ASFs */
            break;

/*  Control elements */

         case VDCINTEGERPREC:    /*  VDC Integer Precision  */
            curichar.vdcint_prec = NEXTINT(ZERO);
            break;

         case VDCREALPREC:       /*  VDC Real Precision  */
            cur.vdc_bits = curichar.vdc.prec = NEXTINT(ZERO);
            cur.vdc_places = curichar.vdc.min = NEXTINT(ZERO);
            if(abs(curichar.vdc.min) <= sizeof(long)<<3)
            {
                cur.vdcmin = ( curichar.vdc.min < ZERO ?
                                 1.0 / ( 1L << -curichar.vdc.min ) :
                                 (Float)( 1L << curichar.vdc.min ) );
            }
            else   cur.vdcmin = pow( (double)2, (double) curichar.vdc.min);
            if(curichar.vdc.prec <= sizeof(long))
               cur.max_vdc = ( (Long) 1L<<curichar.vdc.prec ) - 1;
            else cur.max_vdc = pow((double)2, (double) curichar.vdc.prec) - 1;
            cur.min_vdc = - cur.max_vdc;
            cur.vdc_defexp = curichar.vdc.defexp = NEXTINT(ZERO);
            cur.vdc_expald = curichar.vdc.expald = NEXTINT(ZERO);
            RALGKS111 = FALSE;
            break;

         case CLIP:              /*  Clip Indicator  */
            cur.clip_ind = NEXTINT(ZERO);
            break;

         case CLIPRECT:         /* Clip Rectangle  */
            if (cur.vdc_type == REAL)
            {
               cur.clip_rect.a.x.real = NEXTREAL(ZERO,vdc);
               cur.clip_rect.a.y.real = NEXTREAL(ZERO,vdc);
               cur.clip_rect.b.x.real = NEXTREAL(ZERO,vdc);
               cur.clip_rect.b.y.real = NEXTREAL(ZERO,vdc);
            }
            else
            {
               cur.clip_rect.a.x.intr = NEXTINT(ZERO);
               cur.clip_rect.a.y.intr = NEXTINT(ZERO);
               cur.clip_rect.b.x.intr = NEXTINT(ZERO);
               cur.clip_rect.b.y.intr = NEXTINT(ZERO);
            }
            break;

         default:  /*  Code not yet found try text */
            c = MICtext (code);
            break;

      }

/* Output element if no error */

      cf = ( micerror ? 0 : CGMframe( code ) );
      code = (cf > 0 ? cf : c );
      if ( single || cgmfinished ) break;
   }
   return (code);
}

/****************************************************** MICtext ********/

#ifdef PROTO
static Code MICtext ( Code code )
#else
static Code MICtext ( code )

Code code;
#endif

{
   switch (code)
   {
      case APNDTEXT:
         if ( cgmstate == TEXT_OPEN )
         {
            *pint = NEXTINT(ZERO);
            MICstring(str);
            cgmstate = ( *pint ? PIC_OPEN : TEXT_OPEN );
            return ( *pint ? ZERO : APNDTEXT );
         }
         micerror = TRUE;
         (void) CGMerror(func, ERR_NOTAPNDTXT, ERROR, NULLSTR);
         break;

      case TEXTINDEX:     /*  Text Bundle index  */
         curatt.text_ind = NEXTINT(ZERO);
         break;

      case TEXTFONTINDEX: /*  Text Font Index  */
         curatt.text_font = NEXTINT(ZERO);
         break;

      case TEXTPREC:      /*  Text Precision   */
         curatt.text_prec = NEXTINT(ZERO);
         break;

      case CHAREXPAN:       /*  Character Expansion Factor  */
         curatt.char_exp = NEXTREAL(ZERO,real);
         break;

      case CHARSPACE:       /*  Character Spacing  */
         curatt.char_space = NEXTREAL(ZERO,real);
         break;

      case TEXTCOLR:      /*  Text Colour   */
         (void) MICcol (ZERO, &curatt.text, ANY, (Prec) ZERO);
         break;

      case CHARHEIGHT:      /*  Character Height   */
         if (cur.vdc_type == REAL)
              curatt.char_height.real = NEXTREAL(ZERO,vdc);
         else curatt.char_height.intr = NEXTINT(ZERO);
         curatt.charheight_flag = TRUE;
         break;

      case CHARSETINDEX:  /*  Character Set Index */
         curatt.char_set = NEXTINT(ZERO);
         break;

      case ALTCHARSETINDEX: /*  Alternative Character Set Index */
         curatt.altchar_set = NEXTINT(ZERO);
         break;

      case AUXCOLR:           /*  Auxiliary Colour  */
         (void) MICcol (ZERO, &cur.aux, ANY, curichar.col_prec);
         break;

      case TRANSPARENCY:      /*  Transparency  */
         cur.transparency = NEXTINT(ZERO);
         break;

      case ESCAPE:           /*  Escape  */
         *pint = NEXTINT(ZERO);
         MICstring(str);
         break;

      default:  /*  Still not found try External Elements */
         MICexternal ( code );
         break;
   }
   return ( cgmstate == TEXT_OPEN ? APNDTEXT : NOP );
}

/****************************************************** MICexternal ****/

#ifdef PROTO
static void MICexternal ( Code code )
#else
static void MICexternal ( code )

Code code;
#endif

{

#ifdef DEBUG
      DMESS " External: %x\n", code);
#endif

   switch ( code )
   {
      case MESSAGE:           /*  Message  */

      case APPLDATA:          /*  Application Data */
         *pint = NEXTINT(ZERO);
         MICstring(str);
         break;

      default:                /*  Finally give up */
         micerror = TRUE;
         if ( code & PARABIT ) break;

         if ( cgmstate != MF_CLOSED )
         {
            switch ( cgmstate )
            {
               case MF_DESC:
                  (void) sprintf(mess, "in Metafile Description");
                  break;
               case MF_ELEMLIST:
                  (void) sprintf(mess, "in MF Element list");
                  break;
               case MF_DEFAULTS:
                  (void) sprintf(mess, "in MF Defaults Replacement");
                  break;
               case PIC_DESC:
                  (void) sprintf(mess, "in Picture Description");
                  break;
               case PIC_OPEN:
                  (void) sprintf(mess, "in Picture Body");
                  break;
               case PIC_CLOSED:
                  (void) sprintf(mess, "while Picture Closed");
                  break;
               case TEXT_OPEN:
                  (void) sprintf(mess, "in Partial Text");
                  break;
               default:
                  (void) sprintf(mess, "in Unknown State");
                  break;
            }
            micerror = TRUE;
            (void) sprintf( mess, "%s (Code: 0x%x)", mess, code);
            (void) CGMerror(func, ERR_INVELEM, ERROR, mess);
            break;
      }
   }

   return;
}

/****************************************************** MICnextbyte ****/

#ifdef PROTO
static Code MICnextbyte ( void )
#else
static Code MICnextbyte ()
#endif

/*  get next Character from input perform character substitution
    (and translate - CMS only) */
{
   register Code c;
   Logical invalid = TRUE, substitute = FALSE;

   while ( invalid )
   {

      if ( (c = fgetc( cgmi )) == EOF ) return(EOF);

#ifdef EBCDIC
      if (cgminnative)  c =  cgmascii[c];
#endif

      if ( ! miccharsub )
      {
/*  Ignore Carriage Return and Null characters  */
         if ( c == 0x0a || c == 0x00 ) continue;
         break;
      }

      invalid = FALSE;

/*  Character substitution announcer */

      if ( c == 0x7e ) invalid = substitute = TRUE;

/*  Ignore substituted characters  */

      else if ( c <= 0x20 && micsubchar[c] )    invalid = TRUE;
      else if ( c == 0x7f && micsubchar[0x21] ) invalid = TRUE;

#ifdef DEBUG
      if ( invalid && ! substitute )
             DMESS "Character ignored: %02x\n", c);
#endif

      if ( invalid ) continue;

/*  Translate Character substitution */

      if ( substitute )
      {
         if ( c >= 0x40 && c <= 0x60 && micsubchar[c-PARABIT] )
            c -= PARABIT;
         else if ( c == 0x3f && micsubchar[0x21] ) c = 0x7f;
         else if ( c == 0x3e ) c = 0x7e;

#ifdef DEBUG
         DMESS "Character substituted: %02x\n", c);
#endif
         break;
      }

   }
   return ( c );
}

/****************************************************** MICopcode ******/

#ifdef PROTO
static Code MICopcode( Logical ignore )
#else
static Code MICopcode( ignore )

/*  Get next MF Element code and return its character value
    ignore is TRUE if intermediate parameters should be ignored */

Logical ignore;
#endif

{
   register Code c;

   do {
/* Check for end of file */

     if ( NEXTBYTE(c) == EOF ) break;
     c &= 0x7f;   /* Ignore top bit */

/* if next byte starts with 0x3n or escape, shift and add next byte  */

      if ( (c & 0xf0) == 0x30 || c == 0x1b )
      {
         c <<= 8;
         c |= MICnextbyte();
      }

#ifdef DEBUG1
   DMESS " Opcode: %04x\n", c);
#endif
   } while ( ignore && ( c & PARABIT) );

   return ( c );
}

/****************************************************** MICstring ******/

#ifdef PROTO
static void MICstring ( char *s1 )
#else
static void MICstring (s1)

/*  Get a text string. If CMS translate EBCDIC to ASCII  */

char *s1;
#endif
{
   register Code c;
   char *s2;

#ifdef DEBUG
   DMESS "MICstring: ");
#endif

   s2 = s1;

   while ( NEXTBYTE(c) != EOF )
   {
      if (c == 0x98) continue;
      if (c == 0x9c) break;
      if (c == 0x1b)
      {
         if ( NEXTBYTE (c) == 0x5c ) break;
         if ( c == 0x58 ) continue;
      }
#ifdef EBCDIC
      *s2++ = cgmebcdic[c];
#else
      *s2++ = c;
#endif
   }
   *s2++ = '\0';

#ifdef DEBUG
   DMESS "'%s'\n", s1);
#endif
   return;
}

/****************************************************** MICpoints ******/

#ifdef PROTO
static Long MICpoints ( Code *code, Long *pi, Float *pr, Logical set)
#else
static Long MICpoints (code, pi, pr, set)

/*  Get a list of points starting at pi[0]/pr[0] and returns
    Number of points and next code. 'set' indicates if POLYGON SET */

Code *code;
Long *pi;
Float *pr;
Logical set;
#endif

{
   register Long n = ZERO, j;
   static Long ix, iy, *pmax;
   static Prec exp_x, exp_y;
   static Logical first = TRUE;
   static Float *pmaxreal;
   static double xx, yy;
   Code byte = ZERO;

   if ( first )
   {
       pmax = pimax - 4;
       pmaxreal = prmax - 4;
       exp_x = exp_y = curichar.vdc.defexp;
       ix = iy = ZERO;
       xx = yy = 0.0;
   }

   while ( ( byte = MICopcode(FALSE) ) < (Code) 0xff )
   {
      if ( byte == EOF )
      {
         exit ( CGMerror(func, ERR_EOF, FATAL, NULLSTR) );
      }
      if ( !(byte & PARABIT) ) break;
      if ( byte == 0x50 )
      {
         Code bit = 6;
         Prec bits = ZERO;

         /*  incremental coding - Skip at present  */
         CGMerror (func, ERR_HUFFMAN, ERROR, "Ignore points" );

         byte = MICopcode(FALSE);
         while (TRUE)
         {
            while ( MICbit(&byte, &bit, (Prec) 1) && bits <= 5) bits++;
            if ( !bits ) bits = 1;
            j = MICbit (&byte, &bit, bits);
            if ( j < ZERO || (bits == 5 && j == 0x1f) ) break;
         }
      }

      if (cur.vdc_type == REAL)
      {
         *pr++ = xx += MICreal(byte, exp_x, &exp_x, curichar.vdc.expald);
         *pr++ = yy += MICreal(ZERO, exp_y, &exp_y, curichar.vdc.expald);
      }
      else
      {
         *pi++ = ix += NEXTINT(byte);
         *pi++ = iy += NEXTINT(ZERO);
      }
      if (set) *pi++ = NEXTINT(ZERO);
      n++;

 /*  Communications area full must end on even no of points */
      if ( pi > pmax && !(n & 1) )
      {
#ifdef FIXED_BUFFER
         first = FALSE;
         return (-n);
#else
         pi = CGMialloc( pi, 0L );
         pmax = pimax - 4;
#endif
      }

      if ( pr > pmaxreal && !(n & 1) )
      {
#ifdef FIXED_BUFFER
         first = FALSE;
         return (-n);
#else
         pr = CGMralloc( pr, 0L );
         pmaxreal = prmax - 4;
#endif
      }
   }
   first = TRUE;
   *code = byte;
   return (n);
}

/****************************************************** MICvdc *********/

#ifdef PROTO
static void MICvdc ( Int n, Long *pi, Float *pr)
#else
static void MICvdc (n, pi, pr)

/*  Get 'n' VDC values starting at pint[i]/preal[i] */

Int n;
Long *pi;
Float *pr;
#endif

{
   register Long j;

   for (j = ZERO ; j < n ; j++)
   {
      if (cur.vdc_type == REAL) *pr++ = NEXTREAL(ZERO,vdc);
      else                      *pi++ = NEXTINT(ZERO);
   }
   return;
}

/****************************************************** MICint *********/

#ifdef PROTO
static Long MICint ( Code byte, Logical allowed, Logical *present )
#else
static Long MICint ( byte, allowed, present )

/*  Get an integer. Byte contains next character if already found.
    allowed says if Exponent allowed and present if Exponent Present  */

Code byte;
Logical allowed, *present;
#endif

{
   register Code mask, bits;
   register Long num, neg;

   bits = 4 - allowed;
   mask = (1L<<bits) - 1;
   num = ZERO;

/* if argument zero get next byte  */
   if (!byte) NEXTBYTE(byte);
   if ( byte == EOF )
   {
      exit ( CGMerror(func, ERR_EOF, FATAL, NULLSTR) );
   }

/* check if negative  */
   neg = ( byte & 0x10 );
   if (allowed) *present = (byte & 0x08);

/*  Continue while continuation bit set  */

   while ( byte )
   {
      num <<= bits;
      num |= (byte & mask);
      bits = 5;
      mask = (1L<<bits) - 1;
      if (byte & 0x20) NEXTBYTE(byte);
      else byte = ZERO;
   }
   return (neg ? -num : num);
}

/****************************************************** MICreal ********/

#ifdef PROTO
static double MICreal ( Code byte, Prec defexp, Prec *ptlist, Prec allowed)
#else
static double MICreal ( byte, defexp, ptlist, allowed )

/* Get a real value.
   'byte' contains character if already read,
   'defexp' give Default Exponent,
   'ptlist' is set if in a Points list where Exponent
          is derived from last point,
   'allowed' indicates if Exponent is allowed  */

Code byte;
Prec defexp;
Prec *ptlist;
Prec allowed;
#endif

{
   static Logical first = TRUE;
   static double ptab[2*NPOWER+1];

   Long expnt;
   Logical present = ZERO;
   register double x, *power2=ptab+NPOWER;

   if ( first)
   {

/*  Set up Power table   */
      *(power2) = 1.0;
      for ( expnt=1, x=1.0; expnt <= NPOWER; expnt++ )
      {
         x *= 2.0;
         *(power2+expnt) = x;
         *(power2-expnt) = 1.0/x;
      }
      first = FALSE;
   }

   if (ptlist != NULL) defexp = *ptlist;

   x = MICint (byte, allowed == ALLOWED, &present);
   if (present) expnt = NEXTINT(ZERO);
   else expnt = defexp;

   if (ptlist != NULL) *ptlist = expnt;

/*  adjust values if outside power table  */

   while ( expnt > NPOWER )
   {
      x *= *(power2+NPOWER);
      expnt -= NPOWER;
   }
   while ( expnt < -NPOWER )
   {
      x *= *(power2-NPOWER);
      expnt += NPOWER;
   }

   return ( x * *(power2+expnt) );
}

/****************************************************** MICcells *******/

#ifdef PROTO
static Long MICcells ( Code *code, register Long *pi,
                       Long nx, Long ny, Enum mode, Prec prec)
#else
static Long MICcells ( code, pi, nx, ny, mode, prec )

/*  Get a Colour List starting at 'pi', with nx*ny cells
    using color_mode 'mode' and precision 'prec'.
    returns number of cells  */

Code *code;
register Long *pi;
Long nx, ny;
Enum mode;
Prec prec;
#endif

{
   static Logical first = TRUE;
   static Int type, repeat, col;
   static struct colour c;
   register Long n = ZERO, *pmax = pimax - 1;
   static Code bit, byte;

   if ( first )
   {
      type = NEXTBYTE(byte) & 0x03;
      if ( mode == DIRECT )  type |= 4;
      repeat = ZERO;
      bit = 5, byte = ZERO;
      if ( byte == EOF )
      {
         exit ( CGMerror(func, ERR_EOF, FATAL, NULLSTR) );
      }
      col = ZERO;
   }
#ifdef DEBUG
   DMESS "Precisions local: %d index: %d colour: %d\n",
               prec, curichar.colind_prec, curichar.col_prec);
#endif
   if ( !prec ) prec = ( mode == INDEXED ? curichar.colind_prec
                                         : curichar.col_prec);
   if ( mode == DIRECT ) pmax -= 2;

/* Adjust end of buffer pointer to end on row boundary */
#ifdef FIXED_BUFFER
   pmax = pi + (( pimax-pi ) / nx) * nx;
#endif

#ifdef DEBUG
   DMESS " cells: %dx%d=%d  type: %d  prec: %d ", nx, ny, nx*ny, type, prec);
   DMESS "buffer size: %d", pmax - pi );
   if ( repeat ) DMESS " Colour %d repeat %d", col, repeat);
   if ( ! first ) DMESS " (continuation)\n");
   else DMESS "\n");
#endif

#ifdef FIXED_BUFFER
   if ( pi + nx > pmax )
   {
      micerror = TRUE;
      (void) CGMerror(func, ERR_BIGCELL, ERROR, NULLSTR);
      return (0);
   }
#else
   pi = CGMialloc( pi, (long)nx*ny*(mode == DIRECT ? 3 : 1) );
   pmax = pimax - (mode == DIRECT ? 3 : 1);
#endif

   switch (type)
   {
       case ZERO:    /*  Normal coding - Index */
             while ( (byte = MICopcode(FALSE)) & PARABIT)
             {
                *pi++ = NEXTINT(byte);
                n++;
                if ( pi >= pmax )  /*  Communications area full */
                {
#ifdef FIXED_BUFFER
                    first = FALSE;
                    return (-n);
#else
                    pi = CGMialloc( pi, 0L );
                    pmax = pimax - 1;
#endif
                }
             }
             break;

       case 1:    /*  Bitstream coding - Index  */
             byte = MICopcode(FALSE);
             while ( byte & PARABIT )
             {
                col = MICbit(&byte, &bit, prec);
                if (col == -1) break;
                *pi++ = col;
                n++;
                if ( pi >= pmax )  /*  Communications area full */
                {
#ifdef FIXED_BUFFER
                    first = FALSE;
                    return (-n);
#else
                    pi = CGMialloc( pi, 0L );
                    pmax = pimax - 1;
#endif
                }
             }
             break;

       case 2:    /*  Runlength coding - Index  */
             while ( repeat || ((byte = MICopcode(FALSE)) & PARABIT) )
             {
                if ( !repeat )
                {
                   col = NEXTINT(byte),
                   repeat = NEXTINT(ZERO);
                }
                for ( ; repeat; )
                {
                   *pi++ = col;
                   n++; repeat--;
                   if ( pi >= pmax )  /*  Communications area full */
                   {
#ifdef FIXED_BUFFER
                       first = FALSE;
                       return (-n);
#else
                       pi = CGMialloc( pi, 0L );
                       pmax = pimax;
#endif
                    }
                }
             }
             break;

       case 3:    /*  Runlength bitstream coding - Index  */
             while ( repeat || ( NEXTBYTE(byte) & PARABIT) )
             {
                if ( ! repeat )
                {
                   bit = 5;
                   col = MICbit(&byte, &bit, prec);
                   if ( col == -1 ) break;
                   repeat = NEXTINT(ZERO);
                }
#ifdef DEBUG1
    DMESS " Colour: %d repeat: %d\n", col, repeat);
#endif
                for ( ; repeat; )
                {
                   *pi++ = col;
                   n++; repeat--;
                   if ( pi >= pmax )  /*  Communications area full */
                   {

#ifdef FIXED_BUFFER
                       first = FALSE;
                       return (-n);
#else
                       pi = CGMialloc(  pi, 0L );
                       pmax = pimax - 1;
#endif
                    }
                }
             }
             break;

       case 4:    /*  Normal coding - Direct  */
             while ( NEXTBYTE(byte) & PARABIT)
             {
                col = MICcol(byte, &c, DIRECT, prec);
                *pi++ = c.red;
                *pi++ = c.green;
                *pi++ = c.blue;
                n++;
                if ( pi >= pmax )  /*  Communications area full */
                {
#ifdef FIXED_BUFFER
                    first = FALSE;
                    return (-n);
#else
                    pi = CGMialloc( pi, 0L );
                    pmax = pimax - 3;
#endif
                }
             }
             break;

       case 5:    /*  Bitstream coding - Direct  */
             if ( first ) NEXTBYTE(byte);
             while ( byte & PARABIT )
             {
                col = MICbit(&byte, &bit, prec);
                if ( col == -1 ) break;
                *pi++ = col;
                col = MICbit(&byte, &bit, prec);
                if ( col == -1 ) break;
                *pi++ = col;
                col = MICbit(&byte, &bit, prec);
                if ( col == -1 ) break;
                *pi++ = col;
                n++;
                if ( pi >= pmax )  /*  Communications area full */
                {
#ifdef FIXED_BUFFER
                    first = FALSE;
                    return (-n);
#else
                    pi = CGMialloc( pi, 0L );
                    pmax = pimax - 3;
#endif
                }
             }
             break;

       case 6:    /*  Runlength coding - Direct  */
       case 7:
             while ( repeat || (NEXTBYTE(byte) & PARABIT) )
             {
                if ( ! repeat )
                {
                   col = MICcol(byte, &c, DIRECT, prec);
                   if ( col == -1 ) break;
                   repeat = NEXTINT(ZERO);
                }
                for ( ; repeat; )
                {
                   *pi++ = c.red;
                   *pi++ = c.green;
                   *pi++ = c.blue;
                   n++; repeat--;
                   if ( pi >= pmax )  /*  Communications area full */
                   {
#ifdef FIXED_BUFFER
                       first = FALSE;
                       return (-n);
#else
                       pi = CGMialloc( pi, 0L );
                       pmax = pimax - 3;
#endif
                   }
                }
             }
             break;

       default:
             micerror = TRUE;
             (void) sprintf(mess,"(type: %d)",type);
             (void) CGMerror(func, ERR_INVCOLR, ERROR, mess);
   }
   first = TRUE;
   if ( ( byte & 0xfff0 ) == 0x30 )
   {
      byte <<= 8;
      byte |= MICnextbyte();
   }
   *code = byte;

#ifdef DEBUG
   DMESS " cells: %d\n", n);
#endif
   return (n);
}

/****************************************************** MICcol *********/

#ifdef PROTO
static Long MICcol ( Code byte, struct colour *c, Enum type, Prec prec)
#else
static Long MICcol ( byte, c, type, prec )

/*  Get a colour value.
    byte is the last byte read, type is colour_mode
    and prec is local precision.
    Return next Byte for next colour in list and Colour 'c' */

Code byte;
struct colour *c;
Enum type;
Prec prec;
#endif

{
   register Long j, k = ZERO, r = ZERO, g = ZERO, b = ZERO;

   if (!prec) prec = (cur.color_mode == INDEXED ?
                           curichar.colind_prec : curichar.col_prec);

   if (type == DIRECT || cur.color_mode == DIRECT)
   {
      for (j = prec; j > ZERO ; j -= 2, byte = ZERO)
      {
 /*   check if next byte has already been read  */
         if (!byte) NEXTBYTE(byte);
 /*   strip off 2 bits at each pass  */
         r <<= 2;
         r |= (byte & 0x20) >> 4 | (byte & 0x04) >> 2;
         g <<= 2;
         g |= (byte & 0x10) >> 3 | (byte & 0x02) >> 1;
         b <<= 2;
         b |= (byte & 0x08) >> 2 | (byte & 0x01) ;
      }

 /*   color prec is odd then divide by 2 */
      if ( prec & 0x01 )
      {
         r >>= 1;
         g >>= 1;
         b >>= 1;
/*  save last 3 bits  */
         k = byte & 0x07;
      }
/* if colour precision NOT set convert to 8 bit colour */
      if ( ! cur.colprec_flag )
      {
         r <<= 2; g <<= 2; b <<= 2;
      }
      c->red = r; c->green = g; c->blue = b;
   }
   else
   {
      c->index = NEXTINT(byte);
   }
   return (k);
}

/****************************************************** MICbit *********/

#ifdef PROTO
static Long MICbit ( Code *byte, Code *bit, Prec nbit)
#else
static Long MICbit ( byte, bit, nbit )

/*  Get next bit ( for bitstream ) */

Code *byte, *bit;
Prec nbit;
#endif

{
    register Long n, col= ZERO;

    for (n = ZERO; n < nbit ; n++)
    {
       if( *bit < ZERO )
       {
          NEXTBYTE(*byte);
          if ( ! (*byte & PARABIT) ) return (-1);
          *bit = 5;
       }
       col <<= 1;
       col |= ( *byte >> (*bit)-- ) & 0x01;
    }
    return (col);
}
