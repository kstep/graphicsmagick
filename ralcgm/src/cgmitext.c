/*   RAL-CGM Interpreter module:  @(#) cgmitext.c  version 3.5
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
 * Description: Input clear text CGM from file
 *
 *
 * Maintenance Log:
 *   3 Oct 88 RTP  Original version stabilised
 *   7 Nov 88 RTP  Handle Text states correctly
 *  15 Nov 88 RTP  Handle Encoding Defaults
 *  17 Nov 88 RTP  Use typedefs
 *   9 Jan 89 RTP  Move output to seperate function
 *  24 Jan 89 RTP  Handle Control elements correctly
 *  22 Feb 89 RTP  First 2 bytes passed as arguement
 *   8 Mar 89 RTP  Correct MFelement list processing
 *                 Remove interactive code
 *  11 Jul 89 RTP  Random access to frames
 *  17 Sep 89 RTP  Include ANSI prototyping
 *   9 Oct 89 RTP  Change comarea to use arrays pint & preal
 *  21 Mar 90 RTP  Remove ENDMFDEFAULTS from mfelemlist
 *  25 Mar 90 RTP  Change ERRORMESS macro to set micerror
 *                 Check for insufficient points in LINE etc
 *  25 Jul 90 RTP  Incorporate all include files in cgmin.h
 *                 Add variable buffer option
 *  13 Aug 90 RTP  Add variable buffer routines
 *  16 Aug 90 RTP  Add error handler routine
 *   6 Sep 90 RTP  Change internal routines to static
 *   4 Oct 90 RTP  Use typedefs Int, Long and Float
 *   2 Nov 90 RTP  Add line no to error messages
 *                 Prevent EOF if NONOPs in Pic body
 *  11 Feb 91 RTP  Add ( void ) to ANSI prototypes
 *                 cast atof() to (double)
 *   6 Mar 91 RTP  Add ABSTRACT as alternative for ABS
 *  15 Mar 91 RTP  Change cgmstate to Enum type
 *  18 Mar 91 RTP  Correct buffer calculation for direct colour Cell Array
 *  22 Apr 91 RTP  Add CGMdefaults routine at BEGPICBODY
 *  14 May 91 RTP  Add ANSI declarations
 *   2 Aug 91 RTP  Add SCCS id
 *   7 Aug 91 RTP  Add check for valid CGM version
 *   8 Oct 91 RTP  Change max and n to unsigned long so that 32 bit precision
 *                 is handled correctly
 *  12 Nov 92 RTP  Add cliprect_flag
 *  10 Feb 94 RTP  Move definition of DMESS to cgmmach.h
 *  20 Jul 94 KEVP Corrected Conversion of (VDC)REALPREC
 *  12 Aug 94 KEVP Enable interpreter to finish with a return rather than an exit
 *  17 Oct 94 KEVP Enable interpreter to finish with a return within Metafile Defaults
 *                           Replacement.
 *  19 Oct 94 KEVP Interpret auxilliary colour according to current colour mode.
 *   7 Mar 95 RTP  Remove use of CGM element count
 *   1 Jun 95 KEVP For (VDC)REALPREC, define cur.(real/vdc)min, so that
 *                 the macros REQUAL and VEQUAL are always realistic.

 */

#include "cgmin.h"

extern FILE *cgmi;

#ifdef PROTO

/*  Output Drivers  */

extern Code CGMframe ( Code );

/*  Routines in this Module  */

   void CGMItext( void );
   static void MITexternal( Code ),
               MITgettoken( char* ),
               MITstring( char* ),
               MITvdc( int, Long*, Float* ),
               MITcol( struct colour *c, Enum type );
   static Code MITmfdesc( Code c ),
               MITpdesc( Code c, Logical single ),
               MITpbody( Code c, Logical single ),
               MITtext( Code ),
               MITcode( void ),
               MITdecode( char* );
   static Long MITcells( Long *pi, Long nx, Enum mode ),
               MITpoints( Long *pi, Float *pr, Enum set );
   static Enum MITenum( char* );
   static Long MITint( void );
   static double MITreal( void );
#else
/*  Output Drivers  */

   extern Code CGMframe( );

   /*  Routines in this Module  */

   void CGMItext();           /*  Main interpreter */
   static void MITexternal(), /*  External elements */
               MITgettoken(), /*  Get next token */
               MITstring(),   /*  Get a string */
               MITvdc(),      /*  Get n VDC values */
               MITcol();      /*  Get a colour  */
   static Code MITmfdesc(),   /*  Metafile Descriptor elements */
               MITpdesc(),    /*  Picture Descriptor elements */
               MITpbody(),    /*  Picture Body elements */
               MITtext(),     /*  Text elements */
               MITcode(),     /*  Get a CGM element code */
               MITdecode();   /*  Decode MF element to character value */
   static Long MITcells(),    /*  Get a cell array */
               MITpoints();   /*  Get a Points list */
   static Enum MITenum();     /*  Decode an enumerated type */
   static Long MITint();      /*  Get an Integer */
   static double MITreal();   /*  Get a real */
#endif

/*  Local constants   */

        /*  Seperators for text parsing  */

#define SOFTSEP    (Code) 1
#define HARDSEP    (Code) 2
#define TERM       (Code) 3
#define QUOTE      (Code) 4

/*  Internal Encoding variables  */

static Logical mitnewline = TRUE, mitnulstr, mitincr, miterror;
static Code mitsep = TERM;
static Posint mitlinenr = 1;
static char *func = "CGMitext", mess[80];

/*  Tables for element decoding */

  extern const struct commands cgmelement[];

/*  Macros */

#define NEXTCODE(c)  ( c = ( c ? c : MITcode() ) )

/**************************************************** CGMItext *********/

#ifdef PROTO
void CGMItext ( void )
#else
void CGMItext ( )
#endif

{
   Code c = ZERO;

   mitnewline = cgmterm;

/* Read first token and check if it is BEGMF */

   MITgettoken( str );

   if ( strcmp( str, "BEGMF" ) )
   {
      exit ( CGMerror(func, ERR_NOTCGM, FATAL, NULLSTR) );
   }
   c = BEGMF;

   while ( TRUE )
   {
            /*     Metafile Descriptor Elements   */

      c = MITmfdesc( c );
      if ( cgmEOF ) break;

      while ( NEXTCODE(c) != ENDMF )
      {
            /*    Picture Descriptor Elements  */

         c = MITpdesc(c, MULTIPLE);
         if(cgmfinished) break;

            /*    Picture Body   */

         c = MITpbody (c, MULTIPLE);
         if(cgmfinished) break;
      }
      if(cgmfinished) break;
   }
   return;
}

/**************************************************** MITmfdesc ********/

#ifdef PROTO
static Code MITmfdesc ( Code code )
#else
static Code MITmfdesc ( code )

Code code;
#endif

   /*   Metafile Descriptor elements  */

{
   Code c;

   while ( NEXTCODE(code) != BEGPIC )
   {
      register Long i, j, *pi=pint+1;
      unsigned Long max, n;
      Float f;

      c = ZERO;
      miterror = FALSE;

#ifdef DEBUG
      DMESS "\nMF desc: %x\n", code);
#endif

      switch (code)
      {
         case BEGMF:
            if (cgmstate != MF_CLOSED )
            {
               miterror = TRUE;
               (void) sprintf(mess,"(Line %u)",mitlinenr);
               (void) CGMerror(func, ERR_MFOPEN, ERROR, mess);
               break;
            }
            MITstring (str);

/*  Set defaults for common structure */

            CGMdefaults ( BEGMF, CLEAR_TEXT );

/* Set current defaults which differ from character defaults */

            cur.col_bits = 8;
            cur.colind_bits = 7;
            cur.max_rgb = curitext.max_rgb;

/*  change CGM state  */

            cgmstate = MF_DESC;
            break;

         case EOF:
            cgmEOF = TRUE;
            break;

         case ENDMF:
            cgmstate = MF_CLOSED;    /* change CGM state */
            break;

         case MFDESC:         /*  Metafile Description  */
            MITstring(str);
            break;

         case MFVERSION:         /*  Metafile version   */
            cgmverno = *pint = MITint();
            if ( cgmverno > CGMVERSION )
            {
               miterror = TRUE;
               exit( CGMerror(func, ERR_VERSION, FATAL, NULLSTR) );
            }
            break;

         case VDCTYPE:         /*  VDC Type   */
            cur.vdc_type = MITenum("INTEGER/REAL");
            break;

         case INTEGERPREC:         /*  Integer Precision  */
            curitext.min_int = MITint();
            max = curitext.max_int = MITint();
            for ( i = 1; max ; i++ ) max >>= 1;
            cur.int_bits = i;
            break;

         case REALPREC:         /*  Real Precision   */
            cur.min_real = curitext.min_real = MITreal();
            cur.max_real = curitext.max_real = MITreal();
            curitext.real_digits = MITint();

/*  Now set common defaults, where necessary  */

            for ( i = ZERO, f = 1.0 ; f < cur.max_real; i++ ) f *= 2.0;
            cur.real_bits = i;
            cur.real_places = (Prec)( cur.real_bits - curitext.real_digits/LOG2 - 1);
            for ( i = ZERO, f = cur.max_real ; i < curitext.real_digits; i++) f *= 0.1;
            cur.realmin = f;
            break;

         case INDEXPREC:         /*  Index Precision   */
            curitext.min_index = MITint();
            max = curitext.max_index = MITint();
            for ( i = 1; max ; i++ ) max >>= 1;
            cur.index_bits = i;
            break;

         case COLRPREC:         /*  Colour Precision  */
            max = curitext.col_prec = MITint();
            for ( i = ZERO; max; i++ ) max >>= 1;
            cur.col_bits = i;
            cur.colprec_flag = TRUE;
            break;

         case COLRINDEXPREC:         /*  Colour Index Precision  */
            max = curitext.colind_prec = MITint();
            for ( i = 1; max; i++ ) max >>= 1;
            cur.colind_bits = i;
            break;

         case MAXCOLRINDEX:         /*  Maximum Colour Index  */
            cur.max_colind = MITint();
            break;

         case CHARCODING:         /*  Character coding Announcer  */
            *pint = MITenum ("BASIC7BIT/BASIC8BIT/EXTD7BIT/EXTD8BIT");
            break;

         case COLRVALUEEXT:         /*  Colour value extent  */
            MITcol( &curitext.min_rgb, DIRECT );
            cur.min_rgb = curitext.min_rgb;
            MITcol( &curitext.max_rgb, DIRECT );
            cur.max_rgb = curitext.max_rgb;
            cur.colval_flag = TRUE;
            break;

         case MFELEMLIST:         /*  Metafile element List  */

/*  Change state  */

            cgmstate = MF_ELEMLIST;

/*  Ignore until first quote */

            while ( mitsep != QUOTE && mitsep != TERM ) MITgettoken(str);

/*  Get codes until next QUOTE */

            n = ZERO;
            while ( mitsep != TERM )
            {
               Code k;

               MITgettoken(str);
               if ( strlen(str) == ZERO ) continue;
               k = MITdecode(str);
               if ( k == ENDMFDEFAULTS ) continue;
               *pi++ = ( k >= 0x20 ? k : k - 0x10 );
               n++;
#ifdef DEBUG
    DMESS " MFELEMLIST %s Code: %x (%d)\n", str, *(pi-1), mitsep );
#endif
/*
               if ( mitsep == QUOTE ) break;
*/
            }

            *pint = n;
            cgmstate = MF_DESC;
            break;

         case BEGMFDEFAULTS:  /*  Begin Metafile defaults Replacement  */
            cgmstate = MF_DEFAULTS;
            c = ENDMFDEFAULTS;
            break;

         case ENDMFDEFAULTS:
            while ( NEXTCODE(c) != ENDMFDEFAULTS )
            {
               c = ( (c & 0xff00) == 0x3200 ? MITpdesc(c, SINGLE)
                                            : MITpbody(c, SINGLE) );
               if(cgmfinished) break;
            }
            if(cgmfinished) break;
            c = ZERO;
            cgmstate = MF_DESC;
            break;

         case FONTLIST:         /*  Font List   */
            j = n = ZERO;
            while ( mitsep != TERM )
            {
               MITstring (&str[j]);
               if ( strlen(&str[j]) )
               {
                 j += strlen(&str[j]) + 1;
                 *pi++ = j;
               }
            }
            *pint = j;
#ifdef FONTSYSTEM
            FNTflist(pi,pint, str, &cgmfonts);
#endif
            break;

         case CHARSETLIST:         /*  Character set list  */
            j = ZERO;
            while ( mitsep != TERM )
            {
               *pi++ = MITenum(
               "STD94/STD96/STD94MULTIBYTE/STD96MULTIBYTE/COMPLETECODE");
               MITstring(&str[j]);
               if ( strlen(&str[j]) )
               {
                  j += strlen(&str[j]) + 1;
                  *pi++ = j;
               }
            }
            *pint = j;
#ifdef FONTSYSTEM
            FNTclist(pi,pint, str, &cgmcsets);
#endif
            break;

         default:
            MITexternal ( code );
            break;

      }

/* if no error then output code  */

      if ( code = ( miterror ? 0 : CGMframe( code ) ) ) break;
      code = c;
   }
   return (code);
}

/**************************************************** MITpdesc *********/

#ifdef PROTO
static Code MITpdesc ( Code code, Logical single)
#else
static Code MITpdesc ( code, single )

Code code;
Logical single;
#endif

/*    Picture Descriptor Elements   */

{

   while ( NEXTCODE(code) != BEGPICBODY )
   {
      miterror = FALSE;

#ifdef DEBUG
      DMESS "\nPic desc: %x\n", code);
#endif

      switch (code)
      {
         case BEGPIC:            /*  Begin Picture  */
            if ( cgmstate == PIC_OPEN )
            {
               miterror = TRUE;
               (void) sprintf(mess,"(Line %u)",mitlinenr);
               (void) CGMerror(func, ERR_PICOPEN, ERROR, mess);
               break;
            }
            MITstring (str);

            if ( cgmstate == MF_DESC )
               CGMdefaults ( MFDESC, CLEAR_TEXT );
            else
               CGMdefaults ( ENDPIC, CLEAR_TEXT );

            cgmstate = PIC_DESC;
            break;

         case EOF:
            break;

         case ENDPIC:         /*  End  Picture    */
            cgmstate = PIC_CLOSED;
            return (code);

         case SCALEMODE:         /*  Scaling Mode   */
            cur.scale_mode = MITenum("ABSTRACT/METRIC");
            cur.scale_factor = MITreal();
            break;

         case COLRMODE:         /*  Colour Selection Mode */
            cur.color_mode = MITenum("INDEXED/DIRECT");
            break;

         case LINEWIDTHMODE:         /*  Line width Specification  */
            cur.linewidth_mode = MITenum("ABS:ABSTRACT/SCALED");
            break;

         case MARKERSIZEMODE:         /*  Marker size Specification  */
            cur.markersize_mode = MITenum("ABS:ABSTRACT/SCALED");
            break;

         case EDGEWIDTHMODE:         /*  Edge width Specification  */
            cur.edgewidth_mode = MITenum("ABS:ABSTRACT/SCALED");
            break;

         case VDCEXT:         /*  VDC Extent    */
            if (cur.vdc_type == REAL)
            {
               cur.vdc_extent.a.x.real = MITreal();
               cur.vdc_extent.a.y.real = MITreal();
               cur.vdc_extent.b.x.real = MITreal();
               cur.vdc_extent.b.y.real = MITreal();
            }
            else
            {
               cur.vdc_extent.a.x.intr = MITint();
               cur.vdc_extent.a.y.intr = MITint();
               cur.vdc_extent.b.x.intr = MITint();
               cur.vdc_extent.b.y.intr = MITint();
            }
            break;

         case BACKCOLR:         /*  Background Colour  */
            MITcol(&cur.back, DIRECT);
            break;

         default:
            MITexternal ( code );
            break;
      }

/* if no error then output code  */

      code = (miterror ? 0 : CGMframe( code ) ) ;
      if ( single || cgmfinished ) break;
   }
   return (code);
}

/**************************************************** MITpbody *********/

#ifdef PROTO
static Code MITpbody ( Code code, Logical single)
#else
static Code MITpbody ( code, single )

Code code;
Logical single;
#endif

{
   Code c, cf;

   while ( TRUE )
   {
      Logical cont_list = FALSE;
      register Long i, n, j, *pi = pint+1;
      register Float f, *pr = preal;
      Long max, nx;

      NEXTCODE ( code );
      c = ZERO;
      miterror = FALSE;

#ifdef DEBUG
      DMESS "\nPic Element: %x\n", code);
#endif

      switch (code)
      {
         case BEGPICBODY:         /*  Begin Picture Body  */
            if ( cgmstate == PIC_OPEN )
            {
               miterror = TRUE;
               (void) sprintf(mess,"(Line %u)",mitlinenr);
               (void) CGMerror(func, ERR_PICOPEN, ERROR, mess);
               break;
            }
            cgmstate = PIC_OPEN;
            CGMdefaults ( VDCEXT, CLEAR_TEXT );
            break;

         case EOF:
            break;

         case ENDMF:         /*  End  Picture    */
            cgmstate = MF_CLOSED;
            single = TRUE;     /*  forces exit  */
            (void) sprintf(mess,"(Line %u)",mitlinenr);
            (void) CGMerror(func, ERR_NOENDPIC, ERROR, mess);
            break;

         case ENDPIC:         /*  End  Picture    */
            cgmstate = PIC_CLOSED;
            single = TRUE;     /*  forces exit  */
            break;

         case LINE:         /*  Polyline   */
         case DISJTLINE:         /*  Disjoint Polyline  */
         case MARKER:         /*  Polymarker  */
            *pint = MITpoints( pi, preal, NOSET );
            if ( *pint < ZERO ) c = code;
            break;

         case POLYGON:         /*  Polygon   */
         case POLYGONSET:         /*  Polygon Set  */
            *pint = MITpoints( pi, preal, (Logical)(code == POLYGONSET) );
            if ( *pint < ZERO ) c = code;
            break;

         case APNDTEXT:         /*  In TEXT state */
            code = MITcode();
            c = MITtext (code);
            break;

         case RESTRTEXT:         /*  Restricted Text */
            MITvdc ( (Int)2, pi, preal);
            pi += 2; pr +=2;

         case TEXT:         /*  Text   */
            MITvdc ( (Int)2, pi, pr);
            *pint = MITenum("NOTFINAL/FINAL");
            MITstring (str);
            if ( ! *pint ) /* Not final */
            {
               c = APNDTEXT;
               cgmstate = TEXT_OPEN;
            }
            break;

         case CELLARRAY:         /*  Cell Array  */
            if ( ! cont_list )
            {
               MITvdc ( (Int)6, pi, preal);
               pi += 6;
               nx = *pi++ = MITint();   /* nx */
               *pi++ = MITint();        /* ny */
               n = MITint();       /* Local colour Precision */
               for ( j = ZERO; n ; j++, n >>= 1);
               *pi++ = j;
            }
            *pint = MITcells( pi, nx, cur.color_mode );
            if ( cont_list = ( *pint < ZERO ) ) c = code;
            break;

         case GDP:         /*  Generalised Drawing Primitive  */
            *pi++ = MITint();     /*  GDP type */
            *pint = MITpoints ( pi, preal, NOSET );
            MITstring (str);
            break;

         case RECT:         /*  Rectangle   */
            MITvdc ( (Int)4, pi, preal );
            break;

         case LINEINDEX:       /*  Line Bundle index  */
            curatt.line_ind = MITint();
            break;

         case LINETYPE:       /*  Line Type   */
            curatt.line_type = MITint();
            break;

         case LINEWIDTH:       /*  Line Width */
            if (cur.linewidth_mode == SCALED || cur.vdc_type == REAL )
                   curatt.line_width.real = MITreal();
            else   curatt.line_width.intr = MITint();
            curatt.linewidth_flag = TRUE;
            break;

         case LINECOLR:       /*  Line Colour  */
            MITcol (&curatt.line, cur.color_mode );
            break;

         case MARKERINDEX:       /*  Marker Bundle index  */
            curatt.mark_ind = MITint();
            break;

         case MARKERTYPE:       /*  Marker Type   */
            curatt.mark_type = MITint();
            break;

         case MARKERSIZE:       /*  Marker Size */
            if (cur.markersize_mode == SCALED || cur.vdc_type == REAL )
                   curatt.mark_size.real = MITreal();
            else   curatt.mark_size.intr = MITint();
            curatt.markersize_flag = TRUE;
            break;

         case MARKERCOLR:       /*  Marker Colour  */
            MITcol (&curatt.marker, cur.color_mode );
            break;

         case TEXTPATH:       /*  Text Path */
            curatt.text_path = MITenum("RIGHT/LEFT/UP/DOWN");
            break;

         case FILLINDEX:       /*  Fill Bundle index  */
            curatt.fill_ind = MITint();
            break;

         case INTSTYLE:       /*  Interior Style  */
            curatt.int_style = MITenum("HOLLOW/SOLID/PAT/HATCH/EMPTY");
            break;

         case HATCHINDEX:       /*  Hatch Index  */
            curatt.hatch_ind = MITint();
            break;

         case PATINDEX:       /*  Pattern Index  */
            curatt.pat_ind = MITint();
            break;

         case FILLCOLR:       /*  Fill Colour */
            MITcol (&curatt.fill, cur.color_mode );
            break;

         case EDGEINDEX:       /*  Edge Bundle index  */
            curatt.edge_ind = MITint();
            break;

         case EDGETYPE:       /*  Edge Type    */
            curatt.edge_type = MITint();
            break;

         case EDGEWIDTH:       /*  Edge Width  */
            if (cur.edgewidth_mode == SCALED || cur.vdc_type == REAL )
                   curatt.edge_width.real = MITreal();
            else   curatt.edge_width.intr = MITint();
            curatt.edgewidth_flag = TRUE;
            break;

         case EDGECOLR:       /*  Edge Colour */
            MITcol (&curatt.edge, cur.color_mode );
            break;

         case EDGEVIS:       /*  Edge Visibility  */
            curatt.edge_vis = MITenum("OFF/ON");
            break;

         case CIRCLE:       /* Circle      */
            MITvdc ( (Int)3, pi, preal );
            break;

         case ARC3PT:       /* Circular Arc  3 point */
            MITvdc ( (Int)6, pi, preal );
            break;

         case ARC3PTCLOSE:       /* Circular Arc  3 point close */
            MITvdc ( (Int)6, pi, preal );
            *(pi+6) = MITenum("PIE/CHORD");
            break;

         case ARCCTR:       /* Circle Arc centre */
            MITvdc ( (Int)7, pi, preal );
            break;

         case ARCCTRCLOSE:       /* Circle Arc centre close */
            MITvdc ( (Int)7, pi, preal );
            *(pi+7) = MITenum("PIE/CHORD");
            break;

         case ELLIPSE:       /* Ellipse    */
            MITvdc ( (Int)6, pi, preal );
            break;

         case ELLIPARC:       /* Elliptical Arc */
            MITvdc ( (Int)10, pi, preal );
            break;

         case ELLIPARCCLOSE:       /* Elliptical Arc close*/
            MITvdc ( (Int)10, pi, preal );
            *(pi+10) = MITenum("PIE/CHORD");
            break;

         case CHARORI:       /*  Character Orientation */
            if ( cur.vdc_type == REAL )
            {
               curatt.char_up.x.real = MITreal();
               curatt.char_up.y.real = MITreal();
               curatt.char_base.x.real = MITreal();
               curatt.char_base.y.real = MITreal();
            }
            else
            {
               curatt.char_up.x.intr = MITint();
               curatt.char_up.y.intr = MITint();
               curatt.char_base.x.intr = MITint();
               curatt.char_base.y.intr = MITint();
            }
            break;

         case TEXTALIGN:       /*  Text Alignment */
            curatt.text_halign = MITenum(
                "NORMHORIZ/LEFT/CTR/RIGHT/CONTHORIZ");
            curatt.text_valign = MITenum(
                "NORMVERT/TOP/CAP/HALF/BASE/BOTTOM/CONTVERT");
            curatt.text_hcont = MITreal();
            curatt.text_vcont = MITreal();
            break;

         case PATSIZE:       /*  Pattern Size  */
            if ( cur.vdc_type == REAL )
            {
               curatt.pat_size.a.x.real = MITreal();
               curatt.pat_size.a.y.real = MITreal();
               curatt.pat_size.b.x.real = MITreal();
               curatt.pat_size.b.y.real = MITreal();
            }
            else
            {
               curatt.pat_size.a.x.intr = MITint();
               curatt.pat_size.a.y.intr = MITint();
               curatt.pat_size.b.x.intr = MITint();
               curatt.pat_size.b.y.intr = MITint();
            }
            break;

         case FILLREFPT:       /*  Fill Reference Point  */
            if ( cur.vdc_type == REAL )
            {
               curatt.fill_ref.x.real = MITreal();
               curatt.fill_ref.y.real = MITreal();
            }
            else
            {
               curatt.fill_ref.x.intr = MITint();
               curatt.fill_ref.y.intr = MITint();
            }
            break;

         case PATTABLE:       /*  Pattern Table  */
            if ( !cont_list )
            {
               *pi++ = MITint();   /* Pattern index */
               nx = *pi++ = MITint();   /* nx */
               *pi++ = MITint();        /* ny */

/* Local colour Precision convert to character form */

               n = MITint();      /*  Local Colour Precision */
               for ( j = ZERO; n ; j++, n >>= 1);
               *pi++ = j;
            }
            *pint = MITcells( pi, nx, cur.color_mode );
            if ( cont_list = ( *pint < ZERO ) ) c = code;
            break;

         case COLRTABLE:       /*  Colour Table  */
            if ( !cont_list )
            {
               *pi++ = MITint();  /* colour index */
            }
            *pint = MITcells( pi, (Long)1, DIRECT );
            if ( cont_list = ( *pint < ZERO ) ) c = code;
            break;

         case ASF:       /*  Aspect source flags  */
            n = ZERO;
            while ( mitsep != TERM )
            {
               Enum type;

               type = MITenum( "LINETYPE/LINEWIDTH/LINECOLR\
/MARKERTYPE/MARKERSIZE/MARKERCOLR\
/TEXTFONTINDEX/TEXTPREC/CHAREXPAN:CHAREXP/CHARSPACE/TEXTCOLR\
/INTSTYLE/FILLCOLR/HATCHINDEX/PATINDEX/EDGETYPE/EDGEWIDTH/EDGECOLR\
/ALLEDGE/ALLFILL/ALLTEXT/ALLMARKER/ALLLINE/ALL");
               if ( mitnulstr && mitsep == TERM ) break;
               if ( type > ASFS ) type += 506 - ASFS;
               *pi++ = type;
               *pi++ = MITenum("INDIV/BUNDLED");
               n++;
            }
            *pint = n;

/* Now set current values for ASFs  */

            CGMsetasf( pint );
            break;

/*  Control elements */

         case VDCINTEGERPREC:       /*  VDC Integer Precision  */
            curitext.min_vdc.intr = MITint();
            max = curitext.min_vdc.intr = MITint();
            for ( i = 1; max ; max >>=1, i++ )
            ;
            cur.vdcint_bits = i;
            break;

         case VDCREALPREC:       /*  VDC Real Precision  */
            cur.min_vdc = curitext.min_vdc.real = MITreal();
            cur.max_vdc = curitext.max_vdc.real = MITreal();
            curitext.vdc_digits = MITint();

/*  Now set common defaults, where necessary  */

            for ( i = ZERO, f = 1.0 ; f < cur.max_vdc; i++ ) f *= 2.0;
            cur.vdc_bits = i;
            cur.vdc_places = (Prec)(cur.vdc_bits - curitext.vdc_digits/LOG2 - 1);
            for ( i = ZERO, f = cur.max_vdc ; i < curitext.vdc_digits; i++) f *= 0.1;
            cur.vdcmin = f;
         break;

         case CLIP:       /*  Clip Indicator  */
            cur.clip_ind = MITenum("OFF/ON");
            break;

         case CLIPRECT:       /* Clip Rectangle  */
            if ( cur.vdc_type == REAL )
            {
               cur.clip_rect.a.x.real = MITreal();
               cur.clip_rect.a.y.real = MITreal();
               cur.clip_rect.b.x.real = MITreal();
               cur.clip_rect.b.y.real = MITreal();
            }
            else
            {
               cur.clip_rect.a.x.intr = MITint();
               cur.clip_rect.a.y.intr = MITint();
               cur.clip_rect.b.x.intr = MITint();
               cur.clip_rect.b.y.intr = MITint();
            }
            curatt.cliprect_flag = TRUE;
            break;

         default:
            c = MITtext ( code );
            break;

      }

/* if no error then output code  */

      cf = (miterror ? 0 : CGMframe( code ) );
      code = ( cf > 0 ? cf : c);
      if ( single || cgmfinished ) break;
   }
   return (code);
}

/**************************************************** MITtext **********/

#ifdef PROTO
static Code MITtext ( Code code )
#else
static Code MITtext ( code )

/*  Check for Text elements restricted if in State TEXT */

Code code;
#endif
{
   switch (code)
   {

      case APNDTEXT:
          if ( cgmstate == TEXT_OPEN )
          {
              *pint = MITenum("NOTFINAL/FINAL");
              MITstring (str);
              cgmstate = ( *pint ? PIC_OPEN : TEXT_OPEN );
              return ( *pint ? ZERO : APNDTEXT );
          }
          miterror = TRUE;
          (void) sprintf(mess,"(Line %u)",mitlinenr);
          (void) CGMerror(func, ERR_APNDTXT, ERROR, mess);
          break;

      case TEXTINDEX:       /*  Text Bundle index  */
         curatt.text_ind = MITint();
         break;

      case TEXTFONTINDEX:       /*  Text Font Index  */
         curatt.text_font = MITint();
         break;

      case TEXTPREC:       /*  Text Precision   */
         curatt.text_prec = MITenum("STRING/CHAR/STROKE");
         break;

      case CHAREXPAN:       /*  Character Expansion Factor  */
         curatt.char_exp = MITreal();
         break;

      case CHARSPACE:       /*  Character Spacing  */
         curatt.char_space = MITreal();
         break;

      case TEXTCOLR:       /*  Text Colour   */
         MITcol (&curatt.text, cur.color_mode );
         break;

      case CHARHEIGHT:       /*  Character Height   */
         if ( cur.vdc_type == REAL )
             curatt.char_height.real = MITreal();
         else
             curatt.char_height.intr = MITint();
         curatt.charheight_flag = TRUE;
         break;

      case CHARSETINDEX:       /*  Character Set Index */
         curatt.char_set = MITint();
         break;

      case ALTCHARSETINDEX:    /*  Alternative Character Set Index */
         curatt.altchar_set = MITint();
         break;

      case AUXCOLR:       /*  Auxiliary Colour  */
         MITcol ( &cur.aux, cur.color_mode );
         break;

      case TRANSPARENCY:       /*  Transparency  */
         cur.transparency = MITenum("OFF/ON");
         break;


      case ESCAPE:         /*  Escape  */
         *pint = MITint();
         MITstring(str);
         break;

      default:   /*  Haven't found code yet. Try External elements */
         MITexternal ( code );
         break;
  }
  return ( cgmstate == TEXT_OPEN ? APNDTEXT : NOP );

}

/**************************************************** MITexternal ******/

#ifdef PROTO
static void MITexternal ( Code code )
#else
static void MITexternal ( code )

Code code;
#endif

{
   switch (code)   /*  All other External Elements */
   {
      case MESSAGE:         /*  Message  */
         *pint = MITenum("NOACTION/ACTION");
         MITstring(str);
         break;

      case APPLDATA:         /*  Application Data  */
         *pint = MITint();
         MITstring(str);
         break;

      default:
         miterror = TRUE;
         switch ( cgmstate )
         {
            case MF_CLOSED:
               (void) sprintf( mess, "while Metafile Closed");
               break;
            case MF_DESC:
               (void) sprintf( mess, "in Metafile Description");
               break;
            case MF_ELEMLIST:
               (void) sprintf( mess, "in MF Element List");
               break;
            case MF_DEFAULTS:
               (void) sprintf( mess, "in MF Defaults Replacement");
               break;
            case PIC_DESC:
               (void) sprintf( mess, "in Picture Description");
               break;
            case PIC_OPEN:
               (void) sprintf( mess, "in Picture Body");
               break;
            case PIC_CLOSED:
               (void) sprintf( mess, "while Picture Closed");
               break;
            case TEXT_OPEN:
               (void) sprintf( mess, "in Partial Text");
               break;
            default:
               (void) sprintf( mess, "in Unknown State");
               break;
         }
         (void) sprintf(mess,"%s (Line %u)",mess, mitlinenr);
         (void) CGMerror(func, ERR_INVELEM, ERROR, mess);
   }
   return;
}

/**************************************************** MITcode **********/

#ifdef PROTO
static Code MITcode ( void )
#else
static Code MITcode ()
#endif

/*  get next token and return Element code  */

{
   char s1[STRING_MAX];
   Code c;

   while ( TRUE )
   {
/*     clear last element to terminator  */

      while (!mitnewline && mitsep != TERM ) MITgettoken(s1);

/*     get next non-blank token   */

      do {
         MITgettoken(s1);
      } while ( ! strlen(s1) );

      c = MITdecode(s1);
      if ( c ) break;
   }
   return ( c );
}

/**************************************************** MITdecode ********/

#ifdef PROTO
static Code MITdecode( char *s )
#else
static Code MITdecode(s)

/*  Decode a string as an Element code */

char *s;
#endif

{
   register Int i, k, h, len, hit;
   static Int htable[HASHSIZE], maxhits = 0;
#ifdef HASHTEST
   static Int misses = 0, miss = 0;
#endif
   static Logical first= TRUE;
   char c[17];
   Code code;

   if ( first )
   {

/*  Clear Hash table */
      for ( i = 0; i < HASHSIZE; i++) htable[i] = 0;

/*  Create Hash table */
      for ( i = 0; ; i++)
      {
         strcpy(c, cgmelement[i].string);
         len = strlen(c);

/*  Hashing Algorithm */

         for ( h = k = 0; k < len; k++) h += (k+1)*c[k];
         h %= HASHSIZE;

/*  If miss then look at next entry */
         hit = 0;
         while ( htable[h] )
         {
	    hit++;
#ifdef HASHTEST
	    misses++;
#endif
            if ( h++ >= HASHSIZE ) h = 0;
         }
#ifdef HASHTEST
	 if ( hit ) miss++;
#endif
         if ( hit > maxhits ) maxhits = hit;
         htable[h] = i+1;
         if ( strcmp( cgmelement[i].string,  "EOF" ) == 0 ) break;
      }
      first = FALSE;
/* for test effectiveness of hashing algorithm */
#ifdef HASHTEST
      DMESS "Hash table: Elements: %d Size: %d\n", i,  HASHSIZE);
      DMESS "            Misses: %d First time: %d Maxhits: %d\n", misses, miss, maxhits);
#endif
   }

   len = strlen(s);

/* Hash string */
   for ( h = k = 0; k < len; k++) h += (k+1)*s[k];
   h %= HASHSIZE;

/*  Check in hash table until sure of a hit or miss */
   for ( i = 0; i <= maxhits ; i++, h++ )
   {
      if ( h >= HASHSIZE ) h = 0;
      hit = htable[h];
      if ( !hit--) break;
      code = cgmelement[hit].code;
      if ( strlen(cgmelement[hit].string) != len ) continue;

      if( strncmp(s, cgmelement[hit].string, len) == ZERO )
      {
         if ( code < 0x20 && cgmstate != MF_ELEMLIST &&
              code != (Code) EOF ) break;
         mitincr =  ( strncmp (s, "INCR", (int)4) == ZERO );
         return (code);
      }
   }

   miterror = TRUE;
   (void) sprintf (mess, "'%s' (Line %u)", s, mitlinenr );
   (void) CGMerror(func, ERR_ELEMENT, ERROR, mess);

   return ( 0 );
}

/**************************************************** MITgettoken ******/

#ifdef PROTO
static void MITgettoken( char *s )
#else
static void MITgettoken(s)

/*  read characters until Separator is found and return in string 's' */

char *s;
#endif

{
   static Logical intext = FALSE, quote = FALSE, comma = FALSE;
   Logical after = FALSE;
   static Code textchar;
   register Code c;
   register Long j;
   char *s1 = s;

/*  Get characters until separator found */

   mitsep = ZERO;
   mitnewline = FALSE;

   while (! mitsep )
   {
      if ( (c=fgetc(cgmi)) == EOF)
      {
         if ( cgmstate != MF_CLOSED )
         {
            miterror = TRUE;
            (void) sprintf(mess,"(Line %u)", mitlinenr);
            (void) CGMerror(func, ERR_EOF, FATAL, mess);
         }
         mitsep = TERM;
         strcpy(s, "EOF");
         return;
      }
#ifdef DEBUG2
   DMESS " Next byte: %x '%c'\n", c, c);
#endif

      switch (c)
      {
         case '\n':
         case '\v':
         case '\f':
         case '\r':
            mitlinenr++;
            mitnewline = cgmterm;

         case '\t':
         case '(':
         case ')':    /*  Treat bracket as soft Separator  */
         case ' ':     /*    White space     */
            if (after) mitsep = SOFTSEP;
            break;

         case ',':     /* Treat comma as white space
                          unless it follows a comma */
            if ( comma )
            {
               mitsep = HARDSEP;
               after = TRUE;
            }
            else if (after) mitsep = SOFTSEP;
            comma = TRUE;
            break;

         case '/':
         case ';':     /*    Terminator      */
            mitsep = TERM;
            comma = FALSE;
            break;

         case '\0':    /*  Null character - ignore */
         case '_':     /*  underscore - ignore */
#ifdef EBCDIC
         case '':     /* Local currency character - ignore */
#endif
         case '$':     /* Dollar character - ignore */
            break;

         case '%':     /*   Comments - ignore till next % */
            while ( (c=fgetc(cgmi)) != '%');
            break;

         case '\'':
         case '"':     /*   Text String      */
            comma = FALSE;

  /*  if first quote then signal end of last token  */

            if ( ! intext )
            {
               textchar = c;
               ungetc (c, cgmi);
               intext = TRUE;
               quote = FALSE;
               mitsep = QUOTE;
               break;
            }

            if ( cgmstate == MF_ELEMLIST )
            {
               if ( ! quote )
               {
/*  Start of MF element list */
                  mitsep = SOFTSEP;
                  quote = TRUE;
                  textchar = c;
               }
               else
               {
/*  End of MF element list  */
                  mitsep = QUOTE;
                  intext = FALSE;
               }
               break;
            }
/* Ignore if double quotes */

            if ( quote ) *s1++ = c;

            j = ZERO;
            while ( ( c=fgetc(cgmi) ) != textchar && c != EOF )
            {
               if ( ++j < STRING_MAX ) *s1++ = c;
               else if (j == STRING_MAX)
               {
                  (void) sprintf(mess,"(Line %u)", mitlinenr);
                  (void) CGMerror(func, ERR_BIGSTR, WARNING, mess);
               }
            }

            quote = TRUE;
            after = TRUE;
            continue;

         default:  /*  all other characters */
            *s1++ = toupper(c);
            after = TRUE;
            comma = FALSE;
            break;
      }

/*  if code is not start string character then signal end of string */
      if ( c != textchar && cgmstate != MF_ELEMLIST )
      {
         intext = FALSE;
         quote = FALSE;
      }
   }
   *s1 = '\0';
   mitnulstr = (strlen(s) == ZERO);

#ifdef DEBUG
   DMESS " Token: %x '%s' sep: %d", c, s, mitsep);
   switch ( mitsep )
   {
      case TERM:
          DMESS " TERM\n");
          break;
      case QUOTE:
          DMESS " QUOTE\n");
          break;
      case SOFTSEP:
          DMESS " SOFTSEP\n");
          break;
      case HARDSEP:
          DMESS " HARDSEP\n");
          break;
      default:
          DMESS " UNKNOWN\n");
          break;
   }
#endif

   return;
}

/**************************************************** MITint ***********/

#ifdef PROTO
static Long MITint( void )
#else
static Long MITint()
#endif

/*  Reads token and converts to integer  */

{
   register Long i, n, base = ZERO;
   register Long k = ZERO;
   char s[STRING_MAX], c;

   MITgettoken(s);
   for ( i = ZERO; c = s[i]; i++)
   {
      if ( base )
      {
         if ( c >= '0' && c <= '9' ) n = (Long)c - (Long)'0';
            else if ( c >= 'A' && c <= 'F' )
                n = (Long)c - (Long)'A' + 10;
            else n = base + 1;  /* forces error */

         if ( n > base )
         {
            miterror = TRUE;
            (void) sprintf(mess, "(%c in '%s') (Line %u)", c, s, mitlinenr );
            (void) CGMerror(func, ERR_INTCHR, ERROR, mess);
            return (ZERO);
         }
         k = n + k*base;
      }
      if ( c == '#' )
      {
         s[i] = '\0';
         base = atol(s);
         s[i] = '#';
      }
   }
   if ( ! base ) k = atol(s);

   return ( k );
}

/**************************************************** MITreal **********/

#ifdef PROTO
static Double MITreal( void )
#else
static Double MITreal()
#endif

/*  Convert next token to Real number  */

{
   char s[STRING_MAX];

   MITgettoken(s);

   return (Double) atof(s);
}

/**************************************************** MITenum **********/

#ifdef PROTO
static Enum MITenum( char *s )
#else
static Enum MITenum(s)

/*  Return position of next enumerated type in string 's'  */

char *s;
#endif

{
   char s1[STRING_MAX], s2[STRING_MAX];
   register Long i, j;
   Int toklen;
   register Enum k;

   while (TRUE)
   {
      i = k = ZERO;
      MITgettoken(s1);
      toklen = strlen(s1);
      if ( mitnulstr && (mitsep == TERM) ) break;

      while ( s[i] )
      {
/*  get next enumerated type from string s  */

         for( j = ZERO;
              s[i] && (s[i] != '/') && (s[i] != ':');
              s2[j++] = s[i++] )
         ;
         s2[j] = '\0';

         if ( toklen == j && strncmp( s1, s2, toklen ) == ZERO )
         {
            return ( k );
         }
         if ( s[i] != ':' ) k++;
         i++;
      }
      miterror = TRUE;
      (void) sprintf(mess, "('%s' not found in '%s') (Line %u)",
                              s1, s, mitlinenr );
      (void) CGMerror(func, ERR_INVENUM, ERROR, mess);
      if ( ! cgmterm )  return (ZERO);
   }
   return (ZERO);
}

/**************************************************** MITcol ***********/

#ifdef PROTO
static void MITcol( struct colour *col, Enum type )
#else
static void MITcol( col, type )

/*  Read colour value and return in structure 'col' */

struct colour *col;
Enum type;
#endif

{
   if ( type == DIRECT )
   {
      col->red   = MITint();
      col->green = MITint();
      col->blue  = MITint();
   }
   else
   {
      col->index = MITint();
   }

   return;
}

/**************************************************** MITvdc ***********/

#ifdef PROTO
static void MITvdc( Int n, Long *pi, Float *pr )
#else
static void MITvdc( n, pi, pr )

/*  get 'n' VDC values starting at pint[k]/preal[k]  */

Int n;
Long *pi;
Float *pr;
#endif

{
   register Long i;

   if ( cur.vdc_type == REAL )
   {
      for ( i = ZERO; i < n ; i++ )
      {
         *pr++ = MITreal();
         if ( mitnulstr && (mitsep == TERM || mitsep == QUOTE) ) break;
      }
   }
   else
   {
      for ( i = ZERO; i < n ; i++ )
      {
         *pi++ = MITint();
         if ( mitnulstr && (mitsep == TERM || mitsep == QUOTE) ) break;
      }
   }

#ifdef DEBUG
   DMESS " %ld VDCs\n", n);
#endif

   return;
}

/**************************************************** MITstring ********/

#ifdef PROTO
static void MITstring( char *s )
#else
static void MITstring(s)

/*  Get a text string which must be delimited by QUOTES  */

char *s;
#endif
{

/* ignore until first QUOTE or TERMinator  */

   while ( mitsep != QUOTE && mitsep != TERM ) MITgettoken(s);

   if (mitsep != TERM ) MITgettoken(s);

#ifdef DEBUG
   DMESS " %s\n", s);
#endif

   return;
}

/**************************************************** MITpoints ********/

#ifdef PROTO
static Long MITpoints( Long *pi, Float *pr, Enum set )
#else
static Long MITpoints( pi, pr, set )

/*  Get a Points list starting at *pint/preal[0]
    terminated by TERM or QUOTE  */

Long *pi;
Float *pr;
Enum set;
#endif
{
   register Long i, *pmax;
   register Float *pmaxreal;
   register Long n = ZERO;
   register Float x;
   static Logical first = TRUE;
   static Long ix, iy;
   static Float xx, yy;

   if ( first )
   {
      pmax = pimax - 4;
      pmaxreal = prmax - 4;
      ix = iy = ZERO;
      xx = yy = 0.0;
   }

   while ( mitsep != TERM && mitsep != QUOTE )
   {
      if ( cur.vdc_type == REAL )
      {
         if ( pr > pmaxreal && !(n&1) )
         {
#ifdef FIXED_BUFFER
            first = FALSE;
            return (-n);
#else
            pr = CGMralloc ( pr, 0L );
            pmaxreal = prmax - 4;
#endif
         }
         x = MITreal();
         if ( mitnulstr && (mitsep == TERM || mitsep == QUOTE ) ) break;
         if ( mitincr ) xx = x += xx;
         *pr++ = x;
         x = MITreal();
         if ( mitincr ) yy = x += yy;
         *pr++ = x;
      }
      else
      {
         if ( pi > pmax && !(n&1) )
         {
#ifdef FIXED_BUFFER
            first = FALSE;
            return (-n);
#else
            pi = CGMialloc ( pi, 0L );
            pmax = pimax - 4;
#endif
         }
         i = MITint();
         if ( mitnulstr && (mitsep == TERM || mitsep == QUOTE ) ) break;
         if ( mitincr ) ix = i += ix;
         *pi++ = i;
         i = MITint();
         if ( mitincr ) iy = i += iy;
         *pi++ = i;
      }
      if ( set ) *pi++ = MITenum("INVIS/VIS/CLOSEINVIS/CLOSEVIS");
      n++;
   }
   first = TRUE;

#ifdef DEBUG
   DMESS " %ld Points\n", n);
#endif

   return (n);
}

/**************************************************** MITcells *********/

#ifdef PROTO
static Long MITcells( Long *pi, Long nx, Enum mode )
#else
static Long MITcells( pi, nx, mode )

/*  get a list of Colours Terminated by <TERM>
    returns number of cells  */

Long *pi, nx;
Enum mode;
#endif

{
   Long *pmax, ncol;
   static Logical first = TRUE;
   register Long n = ZERO;


   if ( first )
   {
/*  Work out length of single scan line */
      ncol = ( mode == DIRECT ? 3*nx : nx );
/*  Work out end of buffer */
      pmax = pi + ((pimax - pi) / ncol ) * ncol ;
#ifdef FIXED_BUFFER
/*  Check if scan line fits in a buffer */
      if ( pi + ncol > pmax )
      {
         miterror = TRUE;
         (void) sprintf(mess,"(Line %u)", mitlinenr);
         (void)CGMerror(func, ERR_BIGCELL, ERROR, mess );
         return (0);
      }
#endif
   }

   while ( mitsep != TERM )
   {
      if ( pi > pmax )
      {
#ifdef DEBUG
      DMESS "Buffer full  pi:%x pmax:%x  ", pi, pmax);
#endif
#ifdef FIXED_BUFFER
         first = FALSE;
         return (-n);
#else
         pi = CGMialloc( pi, 0L );
         pmax = pimax - ( cur.color_mode == DIRECT ? 3 : 1 );
#endif
#ifdef DEBUG
      DMESS "After pi:%x pmax:%x\n", pi, pmax);
#endif
      }

      if ( mode == DIRECT )
      {
         *pi++  = MITint();
         if ( mitnulstr && mitsep == TERM ) break;
         *pi++  = MITint();
         if ( mitnulstr && mitsep == TERM ) break;
         *pi++  = MITint();
      }
      else  *pi++  = MITint();
      n++;
      if ( mitnulstr && (mitsep == TERM) ) break;
   }

#ifdef DEBUG
   DMESS " %ld Cells\n", n);
#endif
   first = TRUE;

   return (n);
}
